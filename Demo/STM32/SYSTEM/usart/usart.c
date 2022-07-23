/**
  **********************************************************************************************************************
  * @file    usart.c
  * @author  const-zpc
  * @date    2021-03-25
  * @brief   ���ļ��ṩ����ģ�������ܣ��Թ��� USART ���������¹��ܣ�
  *           + USART �ײ���������������
  *           + USART �жϷ�����
  *           + USART ����/����
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

/** ����BUF��С */
#define USART_BUF_SIZE          (1024ul)  // ����/���ջ���

/* Private typedef ---------------------------------------------------------------------------------------------------*/

/**
  * @breif ATָ�������б�ṹ�嶨��
  */
typedef struct
{
    uint8_t  state;                                 /*!< ����״̬ */
    
    uint8_t  end;                                   /*!< ѭ������β�ڱ� */
    
    uint8_t  head;                                  /*!< ѭ���������ڱ� */
    
    uint16_t  num;                                   /*!< ѭ���������ܴ洢��������� */
}UBufferCtrl_t;

/**
  * @brief USART �����Ϣ�ṹ�嶨��
  */
typedef struct{
    uint8_t arrSendBuf[USART_BUF_SIZE];            	        /*!< ���ͻ��� */

    uint8_t *pRecvBuf;                                      /*!< ���ջ��� */
    
    UBufferCtrl_t tRecvBufCtrl;                             /*!< ���ջ�����п��� */

    uint8_t ucRecvByte1msTic;                      		    /*!< ����һ�ֽ�1ms��ʱ */
    
    uint8_t isRecvEnd;                              	    /*!< ������ɱ�־ */
    
    uint16_t uiSendBufLenth;                        	    /*!< ���ͻ��泤�� */
    
    UsartFun pCallFun[SUPPORT_FUN_MAX_NUM];                 /*!< ע��Ľ��ն�ʱ�ص����� */
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
  * @brief      USART ������ʼ��.
  * @param[in]  drv ��������
  * @param[in]  baudrate ������
  * @retval     None
  */
static void Usart1DrvInit(uint32_t baudrate)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1_RX	  GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //USART ��ʼ������
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
  * @brief      USART ������ʼ��.
  * @param[in]  drv ��������
  * @param[in]  baudrate ������
  * @retval     None
  */
static void Usart3DrvInit(uint32_t baudrate)
{
    //GPIO�˿�����
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

    //USART3_RX	  GPIOB.11��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE);
    
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //USART ��ʼ������
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
  * @brief      ����������ʼ��.
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
  * @brief      �������ݴ�����.
  * @param[in][out] pHandle ���ڴ�����
  * @param[in]  data ��������
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
  * @brief  USART1 �����жϺ���.
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
  * @brief  USART3 �����жϺ���.
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
  * @brief      USART1 ��������.
  * @param[in]  drv ��������
  * @param[in]  data ����
  * @param[in]  len  ���ݳ���
  * @retval     0,���ͳɹ�; -1,����ʧ��
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
  * @brief      ����ֽڵ�������
  * @param[in][out] pHandle - USART_DriveHandleType ���
  * @param[in]  pBuf - ����
  * @param[in]  size - �����С
  * @retval     ���ص�ֵ��������
  *             @arg -1: ʧ��
  *             @arg 0: �ɹ�
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
  * @brief      ����ֽڵ�������
  * @param[in][out] pCtrl - UBufferCtrl_t �������
  * @param[in]  pBuf - ����
  * @param[in]  data - �ֽ�����
  * @retval     ���ص�ֵ��������
  *             @arg -1: д��ʧ��
  *             @arg 0: д��ɹ�
  */
static int AddByteToBuffer(UBufferCtrl_t* pCtrl, uint8_t *pBuf, uint8_t data)
{
    if (0x00 == ((pCtrl->state) & 0x02))                      /* �����򻺳���д������ */
    {
        pBuf[(pCtrl->end)++] = data;

        if ((pCtrl->end) >= (pCtrl->num))
        {
            (pCtrl->end) = 0;
        }

        if (((pCtrl->state) & 0x01) != 0x00)                 /* �������������ݴ����ͻ��� */
        {
            (pCtrl->state) &= ~0x01;                         /* �������������ݴ����ͻ��� */
        }
        else if (((pCtrl->state) & 0x04) != 0x00)            /* ���������� */
        {
            (pCtrl->head) = (pCtrl->end);                     /* ��β�ڱ��ƶ����ڱ� */
        }
        else if ((pCtrl->end) == (pCtrl->head))
        {
            if (pCtrl->state & 0x80)                         /* �Ƿ����������� */
            {
                (pCtrl->state) |= 0x02;                      /* ��ֹ�򻺳���д������ */
            }
            else
            {
                (pCtrl->state) |= 0x04;                      /* ���������� */
            }
        }

        return 0;
    }

    return -1;
}

/**
  * @brief      �ӻ����ж�ȡ�ֽ�������
  * @param[in][out] pCtrl - UBufferCtrl_t �������
  * @param[in]  pBuf - ����
  * @param[out] pOut - ��ȡд��Ļ���
  * @return     ��ȡ�����ֽڳ���
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
        
        if (((pCtrl->state) & 0x02) == 0x00)     /* �����򻺳����д������� */
        {
            (pCtrl->state) |= 0x01;              /* �������������ݵȴ�������� */
        }
    }
    
    (pCtrl->state) &= ~0x02;                         /* �����򻺳����д������� */
    (pCtrl->state) &= ~0x04;                         /* ������δ�� */
    
    return readlenth;
}

/**
  * @brief      �����ֽڽ��ճ�ʱ������.
  * @param[in][out] pHandle ���ڴ�����
  * @prarm[in]  time ��ʱʱ��
  * @retval     None
  */
static void StartRecvByteTimer(USART_DriveHandleType *pHandle, uint8_t time)
{
    pHandle->ucRecvByte1msTic = time;
}

/**
  * @brief      ֹͣ�ֽڽ��ճ�ʱ������.
  * @param[in][out] pHandle ���ڴ�����
  * @retval     None
  */
static void StopRecvByteTimer(USART_DriveHandleType *pHandle)
{
    pHandle->ucRecvByte1msTic = 0;
}

/**
  * @brief      USART ���붨ʱ����ʱ����.
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
  * @brief      ע�ᴮ�����ݽ��ջص�����.
  * @param[in]  drv ��������
  * @param[in]  pUsartFun �ص�����
  * @retval     0,�ɹ�; -1,ʧ��
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
  * @brief      ע���������ݽ��ջص�����.
  * @param[in]  drv ��������
  * @param[in]  pUsartFun �ص�����
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
  * @brief      ���ڽ��������������.
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
  * @brief      printf �÷�����
  * @param[in]  drv ��������
  * @param[in]  fmt �ַ�����ʽ
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

