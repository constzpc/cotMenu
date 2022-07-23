/**
  **********************************************************************************************************************
  * @file    usart.c
  * @author  const-zpc
  * @date    2021-03-25
  * @brief   该文件提供功能模块层管理功能，以管理 USART 驱动的以下功能：
  *           + USART 底层驱动，引脚配置
  *           + USART 中断服务函数
  *           + USART 发送/接收
  *
  **********************************************************************************************************************
  *
  **********************************************************************************************************************
  */
  
/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "usart.h"
#include "sys.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* Private define ----------------------------------------------------------------------------------------------------*/

#define SUPPORT_USART_NUM       2
#define SUPPORT_FUN_MAX_NUM     10

/** 串口BUF大小 */
#define USART_BUF_SIZE          (1024ul)  // 发送/接收缓存

/* Private typedef ---------------------------------------------------------------------------------------------------*/

/**
  * @breif AT指令任务列表结构体定义
  */
typedef struct
{
    uint8_t  state;                                 /*!< 控制状态 */
    
    uint8_t  end;                                   /*!< 循环队列尾哨兵 */
    
    uint8_t  head;                                  /*!< 循环队列首哨兵 */
    
    uint16_t  num;                                   /*!< 循环队列中能存储的最多组数 */
}UBufferCtrl_t;

/**
  * @brief USART 句柄信息结构体定义
  */
typedef struct{
    uint8_t arrSendBuf[USART_BUF_SIZE];            	        /*!< 发送缓存 */

    uint8_t *pRecvBuf;                                      /*!< 接收缓存 */
    
    UBufferCtrl_t tRecvBufCtrl;                             /*!< 接收缓存队列控制 */

    uint8_t ucRecvByte1msTic;                      		    /*!< 接收一字节1ms计时 */
    
    uint8_t isRecvEnd;                              	    /*!< 接收完成标志 */
    
    uint16_t uiSendBufLenth;                        	    /*!< 发送缓存长度 */
    
    UsartFun pCallFun[SUPPORT_FUN_MAX_NUM];                 /*!< 注册的接收定时回调函数 */
} USART_DriveHandleType;

static uint8_t sg_arrUasrt1RecvBuf[USART_BUF_SIZE + 1];
static uint8_t sg_arrUasrt2RecvBuf[USART_BUF_SIZE + 1];

/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
//static 
USART_DriveHandleType sg_tUsartDriveHandle[SUPPORT_USART_NUM] = {0};

/* Private function prototypes ---------------------------------------------------------------------------------------*/
static void Usart1DrvInit(uint32_t baudrate);
static void Usart3DrvInit(uint32_t baudrate);
static void RecvDataHandler(USART_DriveHandleType *pHandle, uint8_t data);
static int InitBuffer(USART_DriveHandleType *pHandle, uint8_t *pBuf, uint16_t size);
static int AddByteToBuffer(UBufferCtrl_t* pCtrl, uint8_t *pBuf, uint8_t data);
static int ReadBytesToBuffer(UBufferCtrl_t* pCtrl, uint8_t *pBuf, uint8_t *pOut);
static void StartRecvByteTimer(USART_DriveHandleType *pHandle, uint8_t time);
static void StopRecvByteTimer(USART_DriveHandleType *pHandle);

/* Private function --------------------------------------------------------------------------------------------------*/
  
/**
  * @brief      USART 驱动初始化.
  * @param[in]  drv 串口驱动
  * @param[in]  baudrate 波特率
  * @retval     None
  */
static void Usart1DrvInit(uint32_t baudrate)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief      USART 驱动初始化.
  * @param[in]  drv 串口驱动
  * @param[in]  baudrate 波特率
  * @retval     None
  */
static void Usart3DrvInit(uint32_t baudrate)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //USART3_TX   GPIOB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //USART3_RX	  GPIOB.11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE);
    
    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

/**
  * @brief      串口驱动初始化.
  * @retval     None
  */
void FML_USART_Init(void)
{
    Usart1DrvInit(115200);
    Usart3DrvInit(115200);
    
    memset(&sg_tUsartDriveHandle, 0, sizeof(sg_tUsartDriveHandle));
    InitBuffer(&sg_tUsartDriveHandle[0], sg_arrUasrt1RecvBuf, sizeof(sg_arrUasrt1RecvBuf) - 1);
    InitBuffer(&sg_tUsartDriveHandle[1], sg_arrUasrt2RecvBuf, sizeof(sg_arrUasrt2RecvBuf) - 1);
}

/**
  * @brief      接收数据处理函数.
  * @param[in][out] pHandle 串口处理句柄
  * @param[in]  data 串口数据
  * @retval     None
  */
static void RecvDataHandler(USART_DriveHandleType *pHandle, uint8_t data)
{
    StartRecvByteTimer(pHandle, 50);

    if (AddByteToBuffer(&pHandle->tRecvBufCtrl, pHandle->pRecvBuf, data))
    {
        StopRecvByteTimer(0);
        pHandle->isRecvEnd = 1;
    }
}

/**
  * @brief  USART1 接收中断函数.
  * @retval None
  */
void USART1_IRQHandler(void)
{
    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1, USART_FLAG_ORE);
    }
    
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearFlag(USART1, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        
        RecvDataHandler(&sg_tUsartDriveHandle[0], USART_ReceiveData(USART1));
     } 
} 

/**
  * @brief  USART3 接收中断函数.
  * @retval None
  */
void USART3_IRQHandler(void)
{
    if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(USART3);
        USART_ClearFlag(USART3, USART_FLAG_ORE);
    }
    
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearFlag(USART3, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        
        RecvDataHandler(&sg_tUsartDriveHandle[1], USART_ReceiveData(USART3));
     } 
} 

/**
  * @brief      USART1 发送数据.
  * @param[in]  drv 串口驱动
  * @param[in]  data 数据
  * @param[in]  len  数据长度
  * @retval     0,发送成功; -1,发送失败
  */
int FML_USART_Transmit(uint8_t drv, const void *data, uint32_t len) 
{
    uint8_t *ptmpData = (uint8_t *)data;
    uint32_t i;
    uint32_t timeout = 0xFFFF;
    
    if (drv >= 2)
    {
        return -1;
    }
    
    if (drv == 0)
    {
        USART_ClearFlag(USART1, USART_FLAG_TC);
        
        for (i = 0; i < len && timeout; i++)
        {
            timeout = 0xFFFF;
            USART1->DR = ptmpData[i];
            
            while((USART1->SR & 0X40) == 0 && timeout--)
            {
            }
        }
    }
    else
    {
        USART_ClearFlag(USART3, USART_FLAG_TC);
        
        for (i = 0; i < len && timeout; i++)
        {
            timeout = 0xFFFF;
            USART3->DR = ptmpData[i];
            
            while((USART3->SR & 0X40) == 0 && timeout--)
            {
            }
        }
    }
    
    if (!timeout)
    {
        return -1;
    }
    
    return 0;
}

/**
  * @brief      添加字节到缓存中
  * @param[in][out] pHandle - USART_DriveHandleType 句柄
  * @param[in]  pBuf - 缓存
  * @param[in]  size - 缓存大小
  * @retval     返回的值含义如下
  *             @arg -1: 失败
  *             @arg 0: 成功
  */
static int InitBuffer(USART_DriveHandleType *pHandle, uint8_t *pBuf, uint16_t size)
{
    pHandle->tRecvBufCtrl.end = 0;
    pHandle->tRecvBufCtrl.head = 0;
    pHandle->tRecvBufCtrl.num = size;
    pHandle->tRecvBufCtrl.state = 0x01;
    pHandle->pRecvBuf = pBuf;
    
    memset(pBuf, 0, size);

    return 0;
}

/**
  * @brief      添加字节到缓存中
  * @param[in][out] pCtrl - UBufferCtrl_t 缓存控制
  * @param[in]  pBuf - 缓存
  * @param[in]  data - 字节数据
  * @retval     返回的值含义如下
  *             @arg -1: 写入失败
  *             @arg 0: 写入成功
  */
static int AddByteToBuffer(UBufferCtrl_t* pCtrl, uint8_t *pBuf, uint8_t data)
{
    if (0x00 == ((pCtrl->state) & 0x02))                      /* 允许向缓冲区写入数据 */
    {
        pBuf[(pCtrl->end)++] = data;

        if ((pCtrl->end) >= (pCtrl->num))
        {
            (pCtrl->end) = 0;
        }

        if (((pCtrl->state) & 0x01) != 0x00)                 /* 缓冲区中无数据待发送或处理 */
        {
            (pCtrl->state) &= ~0x01;                         /* 缓冲区中有数据待发送或处理 */
        }
        else if (((pCtrl->state) & 0x04) != 0x00)            /* 缓存区已满 */
        {
            (pCtrl->head) = (pCtrl->end);                     /* 随尾哨兵移动首哨兵 */
        }
        else if ((pCtrl->end) == (pCtrl->head))
        {
            if (pCtrl->state & 0x80)                         /* 是否锁定旧数据 */
            {
                (pCtrl->state) |= 0x02;                      /* 禁止向缓冲区写入数据 */
            }
            else
            {
                (pCtrl->state) |= 0x04;                      /* 缓冲区已满 */
            }
        }

        return 0;
    }

    return -1;
}

/**
  * @brief      从缓存中读取字节数据流
  * @param[in][out] pCtrl - UBufferCtrl_t 缓存控制
  * @param[in]  pBuf - 缓存
  * @param[out] pOut - 读取写入的缓存
  * @return     读取到的字节长度
  */
static int ReadBytesToBuffer(UBufferCtrl_t* pCtrl, uint8_t *pBuf, uint8_t *pOut)
{
    uint16_t readlenth = 0;
    
    if (((pCtrl->state) & 0x01) == 0x00)
    {
        if (pCtrl->end > pCtrl->head)
        {
            readlenth = pCtrl->end - pCtrl->head;
            memcpy(pOut, &pBuf[pCtrl->head - 1], readlenth);
        }
        else
        {
            readlenth = pCtrl->num - pCtrl->head + 1;
            memcpy(pOut, &pBuf[pCtrl->head - 1], readlenth);
            memcpy(&pOut[readlenth], &pBuf[0], pCtrl->end - 1);
            readlenth += pCtrl->end - 1;
        }
        
        pCtrl->head = pCtrl->end;
        
        if (((pCtrl->state) & 0x02) == 0x00)     /* 允许向缓冲区中存入数据 */
        {
            (pCtrl->state) |= 0x01;              /* 缓冲区内无数据等待处理或发送 */
        }
    }
    
    (pCtrl->state) &= ~0x02;                         /* 允许向缓冲区中存入数据 */
    (pCtrl->state) &= ~0x04;                         /* 缓冲区未满 */
    
    return readlenth;
}

/**
  * @brief      启动字节接收超时计数器.
  * @param[in][out] pHandle 串口处理句柄
  * @prarm[in]  time 超时时间
  * @retval     None
  */
static void StartRecvByteTimer(USART_DriveHandleType *pHandle, uint8_t time)
{
    pHandle->ucRecvByte1msTic = time;
}

/**
  * @brief      停止字节接收超时计数器.
  * @param[in][out] pHandle 串口处理句柄
  * @retval     None
  */
static void StopRecvByteTimer(USART_DriveHandleType *pHandle)
{
    pHandle->ucRecvByte1msTic = 0;
}

/**
  * @brief      USART 毫秒定时器计时函数.
  * @retval     None
  */
void FML_USART_MillisecondTimer(void)
{
    int i;
    
    for (i = 0; i < SUPPORT_USART_NUM; i++)
    {
        if (sg_tUsartDriveHandle[i].ucRecvByte1msTic)
        {
            sg_tUsartDriveHandle[i].ucRecvByte1msTic--;
            
            if (sg_tUsartDriveHandle[i].ucRecvByte1msTic == 0)
            {
                sg_tUsartDriveHandle[i].isRecvEnd = 1;
            }
        }
    }
}

/**
  * @brief      注册串口数据接收回调函数.
  * @param[in]  drv 串口驱动
  * @param[in]  pUsartFun 回调函数
  * @retval     0,成功; -1,失败
  */
int FML_USART_Register(uint8_t drv, const UsartFun pUsartFun)
{
    int i;
    
    if (drv < SUPPORT_USART_NUM)
    {
        for (i = 0; i < SUPPORT_FUN_MAX_NUM; i++)
        {
            if (sg_tUsartDriveHandle[drv].pCallFun[i] == 0)
            {
                sg_tUsartDriveHandle[drv].pCallFun[i] = pUsartFun;
                return 0;
            }
        }
    }
    
    return -1;
}

/**
  * @brief      注销串口数据接收回调函数.
  * @param[in]  drv 串口驱动
  * @param[in]  pUsartFun 回调函数
  * @retval     None
  */
void FML_USART_UnRegister(uint8_t drv, const UsartFun pUsartFun)
{
    int i;
        
    if (drv < SUPPORT_USART_NUM)
    {
        for (i = 0; i < SUPPORT_FUN_MAX_NUM; i++)
        {
            if (sg_tUsartDriveHandle[drv].pCallFun[i] == pUsartFun)
            {
                sg_tUsartDriveHandle[drv].pCallFun[i] = NULL;
                break;
            }
        }
    }
}

/**
  * @brief      串口解包接收数据任务.
  * @retval     None
  */
void FML_USART_RecvTask(void)
{
    int drv;
    uint16_t i;
    uint16_t lenth;
    uint8_t tmpBuf[USART_BUF_SIZE + 1] = {0};
    
    for (drv = 0; drv < SUPPORT_USART_NUM; drv++)
    {
        if (sg_tUsartDriveHandle[drv].isRecvEnd)
        {
            sg_tUsartDriveHandle[drv].isRecvEnd = 0;
            lenth = ReadBytesToBuffer(&sg_tUsartDriveHandle[drv].tRecvBufCtrl, sg_tUsartDriveHandle[drv].pRecvBuf, tmpBuf);

            if (lenth > 0)
            {
                tmpBuf[lenth] = 0;
                
                for (i = 0; i < SUPPORT_FUN_MAX_NUM; i++)
                {
                    if (sg_tUsartDriveHandle[drv].pCallFun[i] != NULL)
                    {
                        sg_tUsartDriveHandle[drv].pCallFun[i](tmpBuf, lenth);
                    }
                }
            }
        }
    }
}

/**
  * @brief      printf 用法类似
  * @param[in]  drv 串口驱动
  * @param[in]  fmt 字符串格式
  * @retval     None
  */
void USART_Printf(uint8_t drv, const char *fmt, ...)
{
    uint32_t len;
	va_list arg;
	va_start(arg, fmt);
	memset(sg_tUsartDriveHandle[drv].arrSendBuf, 0, sizeof(sg_tUsartDriveHandle[drv].arrSendBuf));
	vsnprintf((char *)sg_tUsartDriveHandle[drv].arrSendBuf, USART_BUF_SIZE, fmt, arg);
	va_end(arg);
	
	len = strlen((const char *)sg_tUsartDriveHandle[drv].arrSendBuf);
	FML_USART_Transmit(drv, (const uint8_t *)sg_tUsartDriveHandle[drv].arrSendBuf, len);
}

