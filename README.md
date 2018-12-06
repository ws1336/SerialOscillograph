# SerialOscillograph
这是一个使用Qt5开发的串口示波器上位机

串口通讯使用了QSerialPort，波形绘制使用了QChart。

项目包含了QT上位机源码，和与之配合的STM32单片机头文件。

通讯数据二进制形式（区别于字符型），数据类型为32位float，通讯效率较高，详见代码。

数据保存格式为csv格式，代码使用了SACsvStream中部分代码。

合作或意见反馈请联系ws1336@qq.com

持续改进中......
