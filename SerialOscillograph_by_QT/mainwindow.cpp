#include "mainwindow.h"
#include "ui_mainwindow.h"
const int color[10]={
    0xff0000,0xff8000,0xffff00,0xc0ff00,
    0x00ff00,0x00ffff,0x0000ff,0x8000ff,
    0xff00ff,0x000000,
};
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SerialSta=false;
    port=new QSerialPort;
    connect(port,SIGNAL(readyRead()),this,SLOT(readread()));
    QList<QSerialPortInfo> strlist=QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::const_iterator iter;
    for(iter=strlist.constBegin();iter!=strlist.constEnd();++iter)
    {
        ui->cb_SerialPort->addItem((*iter).portName());
    }

    chartSta=false;
    m_chart = new QChart;
    ui->chartView->setChart(m_chart);
    ui->chartView->setRubberBand(QChartView::RectangleRubberBand);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    //ui->chartView
    axisX = new QValueAxis;
    axisX->setRange(0,chartSize);
    axisX->setLabelFormat("%g");
    axisX->setTickCount(10);
    //axisX->setTitleText("axisX");
    axisY = new QValueAxis;
    axisY->setRange(-1.0,1.0);
    axisY->setTickCount(10);
    //axisY->setTitleText("axisY");


    for(int i=0;i<Channel_number;++i){
        m_series[i].setColor(color[i]);
        m_series[i].setName(QString("Ch ")+QString::number(i));
        m_chart->addSeries(&m_series[i]);
        m_chart->setAxisX(axisX,&m_series[i]);
        m_chart->setAxisY(axisY,&m_series[i]);
        m_series[i].setUseOpenGL(true);//openGl 加速
        //qDebug()<<m_series[i].useOpenGL();
    }
    m_chart->legend()->setLabelColor(QColor(0,0,0));
    m_chart->legend()->setAlignment(Qt::AlignRight);
    m_chart->legend()->setMarkerShape(QLegend::MarkerShapeCircle);
    //m_chart->legend()->setShowToolTips(true);
    //m_chart->legend()->hide();
    //m_chart->setTitle("串口示波器--基于QT");



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readread()
{
    QByteArray arr= port->readAll();
    bool changed=false;
    /*QString str;
    for(int i=0;i<arr.length();i++)
    {
        str+=QString("%1").arg((uchar)arr.at(i),2,16,QLatin1Char('0')).toUpper()+" ";
    }
    ui->textBrowser->append(str);*/

    float rec_data[11];
    processor.add(arr);

    for(int i=0;i<Channel_number;i++)
    {
       points[i] = m_series[i].pointsVector();
    }
    while(processor.process((char *)rec_data)&&chartSta==true)
    {
        min++;
        max++;
        for(int i=0;i<((int *)rec_data)[0];i++)
        {
           points[i].append(QPointF(max-1,rec_data[i+1]));
        }
        changed = true;
    }
    if(changed)
    {
        for(int i=0;i<Channel_number;i++)
        {
           m_series[i].replace(points[i]);
        }
        axisX->setRange(min,max);
    }

}

void MainWindow::on_btn_OpenSerial_clicked()
{
    if(SerialSta)
    {
        port->close();
        ui->btn_OpenSerial->setText("打开串口");
        //ui->labelstatu->setStyleSheet("border-image: url(:/new/img/red.png);");
        SerialSta=false;
        //ui->labelstate->setText("串口已关闭！");
    }
    else
    {
#ifdef Q_OS_WIN
        port->setPortName(ui->comboBoxserial->currentText());
#else
        port->setPortName("/dev/"+ui->cb_SerialPort->currentText());
#endif
        port->setBaudRate(ui->cb_baudRate->currentText().toInt());
        port->setDataBits(QSerialPort::Data8);
        if(port->open(QIODevice::ReadWrite))
        {
            processor.clear();
            ui->btn_OpenSerial->setText("关闭串口");
            //ui->labelstatu->setStyleSheet("border-image: url(:/new/img/lv.png);");
            SerialSta=true;
            //ui->labelstate->setText("串口已打开！");
        }
        else
        {
            SerialSta=false;
            QMessageBox::information(this,"提示","串口打开失败",QMessageBox::Ok);
            //ui->te_receive->append("串口打开失败！");
            //ui->labelstatu->setStyleSheet("border-image: url(:/new/img/red.png);");
        }
    }
}

void MainWindow::on_btn_help_clicked()
{
    QString strtext="1.串口默认设置：1起始位，1停止位，8数据位，无校验位。\n"
                    "2.数据格式：“QTSO”+通道数(int32)+各个通道数据(float)。\n"
                    "3.Linux系统非root用户会无法打开串口，解决方案如下：\n"
                    "    打开串口前在终端中输入指令：sudo chmod 666 /dev/*\n"
                    "    *符号位置输入序显示的串口号，然后输入计算机用户密码。\n";
    QMessageBox::information(NULL, "帮助信息", strtext, QMessageBox::Ok);
}

void MainWindow::on_btn_StopDis_clicked()
{
    if(chartSta==false)
    {
        processor.clear();
        chartSta=true;
        ui->btn_StopDis->setText("暂停更新");
    }
    else
    {
        chartSta=false;
        ui->btn_StopDis->setText("开始更新");
    }
}
