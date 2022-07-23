# 轻量级菜单框架(C语言)

#### 介绍
1.  采用链表方式实现多级深度菜单（通过配置选择采用动态分配或者数组实现）

2.  菜单控制框架完全独立，因此菜单操作和菜单内容显示可自由搭配选择，无需修改代码

3.  支持中英文菜单切换

4.  支持可视范围内部分菜单选项显示等扩展功能

5.  移植即可使用

6.  提供命令行界面demo 和 STM32 平台的 demo

#### 软件架构
软件架构说明

#### 使用说明
1.  使用前初始化函数 Menu_Init, 设置主菜单内容
2.  周期调用函数 Menu_Task, 用来处理菜单显示和执行相关回调函数
3.  使用其他函数对菜单界面控制

#### demo样式
博客：

[轻量级多级菜单控制框架程序（C语言）](https://blog.csdn.net/qq_24130227/article/details/121167276?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22121167276%22%2C%22source%22%3A%22qq_24130227%22%7D&ctrtid=VbyfV)

命令行效果图：

![](https://img-blog.csdnimg.cn/22d1476746f64b82ae8a614a47d9d7de.gif)

STM32 + OLED 效果图：

![](https://img-blog.csdnimg.cn/721e44b87f634c7e9aabe3191a3876a1.gif)

#### 关于作者
1.  CSDN 博客 [大橙子疯](https://blog.csdn.net/qq_24130227?spm=1010.2135.3001.5343)
2.  联系邮箱 const_zpc@163.com
3.  了解更多可关注微信公众号

![大橙子疯嵌入式](微信公众号.jpg)

