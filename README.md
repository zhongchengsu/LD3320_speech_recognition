##语音模块开发——控制洗衣机

####硬件：

* LD3320：是一款“语音识别”专用芯片，由ICRoute公司设计生产
* stc11L08xe：是一个51单片机，flash 8k, 内部ram 256bytes 扩展ram 1024bytes，eeprom 32k
* HC-SR501: 人体感应模块

####功能：

* LD3320辅助识别语音和播放mp3音乐
* 单片机负责控制逻辑和语音存储, 当语音模块将识别结果发给单片，单片机按照相应的结果控制洗衣机。
* 人体感应模块触发单片机进入工作状态，无人靠近是不对任何其他声音进行识别，减少误识别率。
* 语音说识别开发板：开发板在淘宝上购买，购买地址：https://detail.tmall.com/item.htm?spm=a220m.1000858.0.0.l4N5QU&id=41365254224&skuId=4611686059792642128&areaId=320200&is_b=1&cat_id=2&q=ld+3320
* 开发板资料地址：http://yunpan.taobao.com/s/3Y0buw4Zvw
* 人体感应模块购买地址：https://item.taobao.com/item.htm?spm=a1z09.2.0.0.yIfjNm&id=45610204788&_u=986c9hs6301


##准备工作——keil软件安装

* Kei_MDK安装，这里的程序使用的是keil mdk 4.72.9版本，下载地址：http://www.121down.com/soft/softview-13682.html
* Keil c51插件安装 软件下载地址：http://www.liangchan.net/liangchan/5688.html
* 下载程序烧写工具STC-ISP，下载地址：www.stcmcu.com
* cool edit语音处理软件，用于编辑和压缩Mp3文件
* ultra edit 用于将Mp3文件二进制转换成字符，适合代码编译

##编译代码

* 控制代码编译，进入 keil4 App双击YS-V0.7.uvproj文件，点击Project->Rebuilt all target file。工程编译完成的可以烧录的文件是obj中的V07A.hex文件。
* 语音数据编译，语音数据最终会烧录到eeprom中。打开eeprom中的eeprom.uvproj,并且进行编译。生成的eeprom.hex就是mp3数据烧录文件。

## 烧录方法

* 将开放板和usb转串口连接好
* 打开STC-ISP工具。
  * 单片机型号中选择STC11L08XE
  * 串口号选择刚刚连接的usb转串口的接口；
  * 点击打开程序文件，打开V07A.hex文件；
  * 点击打开eeprom文件，打开eeprom.hex文件；
  * 点击下载/编程
* 将开板的电源插拔一下
* 等待烧录成功

##EEPROM 和Mp3文件
STC11L08XE的存储能力有限，所以所有的Mp3文件都要放在EEPROM中。文件是挨个挨个收尾项链的存放的。所以在EERPOM中的前128字节记录了这些文件在EEPROM中的首地址和数据长度。数据格式如下：


    /* 
     *  eeprom中前128字节的数据存储方式 
     *  byte0  byte1  byte2  byte3  byte4  byte5  byte6  byte7
     *  addrH0 addrL0 lengthH0 lengthL0 addrH1 addrL1 lengthH1 lengthL1
     *  addrH0:第一个Mp3文件的起始地址的高8位
     *  addrL0:第一个Mp3文件的起始地址的低8位
     *  lengthH0：第一个Mp3文件长度的高8位
     *  lengthL0：第一个Mp3文件长度的低8位
     */
    typedef struct sound{
      uint32_t addr;
      uint32_t dataSize;
    } soundPara_t;


由于eeprom只有32k，所以建议将Mp3文件先经过cool edit编辑，删除没用的语音数据，并且使用8Kbps,8000Hz,Mono参数进行压缩。

## 使用方法

* 人体靠近设备，设备发现人体后进入识别等待状态。所以手机语音是无法被识别的
* 给设备发送语音指令，指令分为一级指令和二级指令
  * 一级指令共4个：
     * 小天鹅： 进入二级指令模式，等待接受洗衣控制指令
     * 你是谁： 播放语音“我是小天鹅”
     * 你几岁： 播放语音“这是秘密哦”
     * 唱首歌吧：播放“小苹果”音乐片段
  * 二级指令共12个：
     * 关机： 开启洗衣机
     * 开机： 关闭洗衣机
     * 启动： 启动洗衣机
     * 暂停： 暂停洗衣机
     * 选择棉麻： 选择棉麻程序
     * 选择混合： 选择混合程序
     * 选择羊毛： 选择羊毛程序
     * 选择羽绒服： 选择羽绒服程序
     * 选择快洗： 选择快洗程序
     * 选择节能： 选择节能程序
     * 选择单脱水： 选择单脱水程序
     * 选择筒自洁： 选择筒自洁程序

##版本

* v1.0.0：2015.11.29 by John SU
* v1.1.0: 2015.12.09 by John Su, Add human-body detection