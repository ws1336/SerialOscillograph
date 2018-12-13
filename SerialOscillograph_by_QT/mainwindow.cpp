#include "mainwindow.h"
#include "ui_mainwindow.h"
/*曲线颜色*/
//调整次序
const int color[10]={
    0xff0000,0xffff00,0x00ff00,0x0000ff,0xff00ff,
    0xff8000,0xc0ff00,0x00ffff,0x8000ff,0x000001,
};
/*const int color[10]={
    0xff0000,0xff8000,0xffff00,0xc0ff00,0x00ff00,
    0x00ffff,0x0000ff,0x8000ff,0xff00ff,0x000001,
};*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SerialSta=false;
    port=new QSerialPort;
    connect(port,SIGNAL(readyRead()),this,SLOT(readData()));
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

    //设置坐标系
    axisX = new QValueAxis;
    axisX->setRange(Xmin,Xmax);
    axisX->setLabelFormat("%g");
    axisX->setTickCount(11);
    //axisX->setTitleText("X轴");
    axisY = new QValueAxis;
    axisY->setRange(Ymin,Ymax);
    axisY->setTickCount(11);
    //axisY->setTitleText("Y轴");

    for(int i=0;i<Channel_number;++i)
    {
        m_series[i].setPen(QPen(QBrush(color[i]), 2));//设置线条颜色和粗细
        m_series[i].setName(QString("CH")+QString::number(i));
        m_chart->addSeries(&m_series[i]);
        m_chart->setAxisX(axisX,&m_series[i]);
        m_chart->setAxisY(axisY,&m_series[i]);
        m_series[i].setUseOpenGL(true);//openGl 加速
    }

    m_chart->legend()->setLabelColor(QColor(0,0,0));
    m_chart->legend()->setAlignment(Qt::AlignRight);
    m_chart->legend()->setContentsMargins(0,0,0,0);
    m_chart->legend()->setMarkerShape(QLegend::MarkerShapeCircle);
    //m_chart->legend()->hide();
    //m_chart->setTitle("串口示波器--基于QT");

    ui->statusBar->setContentsMargins(8,0,0,0);

    lb_StatusBar_SerialStatus = new QLabel;
    lb_StatusBar_SerialStatus->setText("串口状态：已关闭");
    ui->statusBar->insertWidget(0,lb_StatusBar_SerialStatus,1);

    lb_StatusBar_DataRecNum = new QLabel;
    lb_StatusBar_DataRecNum->setText(QString("已接收字节：%1").arg(dataRecNum));//修改状态栏
    ui->statusBar->insertWidget(1,lb_StatusBar_DataRecNum,1);

    ui->leXSize->setValidator(new QIntValidator(0, 99999, this));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readData()
{
    QByteArray arr= port->readAll();
    dataRecNum+=arr.size();
    lb_StatusBar_DataRecNum->setText(QString("已接收字节：%1").arg(dataRecNum));//修改状态栏

    bool changed=false;//是否变动了points
    float rec_data[11];
    processor.add(arr);//通讯协议processor添加数据

    //通讯成功&正在更新，持续添加点直至所有数据被添加到曲线
    while(processor.process((char *)rec_data)&&chartSta==true)
    {
        Xmax++;
        if(Xmax>XSize)//调节X轴（窗口）显示范围
            Xmin=Xmax-XSize;

        for(int i=0;i<((int *)rec_data)[0]/4;i++)//接收到的数据为float型，每个数据4字节故/4
        {
           m_series[i].append(QPointF(Xmax,rec_data[i+1]));
        }
        changed = true;
    }
    if(changed)//变动了points，重新计算曲线最大最小值
    {
        Ymin=3.4028235e38f;
        Ymax=1.4e-45f;
        for(int i=0;i<Channel_number;i++)
        {
           //数据量超过窗口长度
           int pointssize=m_series[i].pointsVector().size();
           if(pointssize>XSize)
           {
               for(int j=pointssize-XSize;j<pointssize;j++)
               {
                   if(m_series[i].pointsVector().at(j).y()>Ymax)
                       Ymax=m_series[i].pointsVector().at(j).y();
                   if(m_series[i].pointsVector().at(j).y()<Ymin)
                       Ymin=m_series[i].pointsVector().at(j).y();
               }
           }
           else//数据量不足窗口长度
           {
               for(int j=0;j<pointssize;j++)
               {
                   if(m_series[i].pointsVector().at(j).y()>Ymax)
                       Ymax=m_series[i].pointsVector().at(j).y();
                   if(m_series[i].pointsVector().at(j).y()<Ymin)
                       Ymin=m_series[i].pointsVector().at(j).y();
               }
           }
        }
        //重新设置坐标轴（窗口）显示范围
        if(ui->checkBoxXAuto->isChecked())
            axisX->setRange(Xmin,Xmax);
        if(ui->checkBoxYAuto->isChecked())
            axisY->setRange(Ymin,Ymax);
    }
}

void MainWindow::on_btn_OpenSerial_clicked()
{
    if(SerialSta)
    {
        port->close();
        lb_StatusBar_SerialStatus->setText("串口状态：已关闭");
        ui->btn_OpenSerial->setText("打开串口");
        //ui->labelstatu->setStyleSheet("border-image: url(:/new/img/red.png);");
        SerialSta=false;
    }
    else
    {
#ifdef Q_OS_WIN
        port->setPortName(ui->cb_SerialPort->currentText());
#else
        port->setPortName("/dev/"+ui->cb_SerialPort->currentText());
#endif
        port->setBaudRate(ui->cb_baudRate->currentText().toInt());
        port->setDataBits(QSerialPort::Data8);
        if(port->open(QIODevice::ReadWrite))
        {
            processor.clear();
            lb_StatusBar_SerialStatus->setText("串口状态：已开启");
            ui->btn_OpenSerial->setText("关闭串口");
            //ui->labelstatu->setStyleSheet("border-image: url(:/new/img/lv.png);");
            SerialSta=true;
        }
        else
        {
            SerialSta=false;
            QMessageBox::information(this,"提示","串口打开失败",QMessageBox::Ok);
            //ui->labelstatu->setStyleSheet("border-image: url(:/new/img/red.png);");
        }
    }
}

void MainWindow::on_btn_help_clicked()
{
    QString strtext="1.串口通讯格式：1起始位，1停止位，8数据位，无校验位。\n"
                    "2.数据组织形式：\"QTSO\"+通道数(int32)+各通道数据(float)\n"
                    "   参考本项目代码以及提供的STM32单片机配套代码。\n"
                    "3.缩放方式：按下左键选中区域放大，右键单击区域缩小。\n"
                    "4.数据保存格式：***.csv\n"
                    "5.Linux系统非root用户会无法打开串口，解决方案如下：\n"
                    "   打开串口前在终端输入：sudo chmod 666 /dev/*\n"
                    "   *号为串口号，回车后输入计算机用户密码，再回车。\n";
    QMessageBox::information(this, "帮助信息", strtext, QMessageBox::Ok);
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

void MainWindow::on_action_save_triggered()
{
    QString filename;
    filename = QFileDialog::getSaveFileName(this,tr("保存文件"),"",tr("text(*.csv)"));
    if(filename.isEmpty())
        return;
    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning(this,tr("Error"),tr("Read file error:%1!").arg(file.errorString()));
        return ;
    }

    QTextStream write(&file);
    write<<writeDataToCSV();
    file.close();

}

void MainWindow::on_action_load_triggered()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(this,tr("载入文件"),"",tr("text(*.csv)"));
    if(filename.isEmpty())
        return;
    QFile file(filename);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning(this,tr("Error"),tr("Read file error:%1!").arg(file.errorString()));
        return ;
    }

    QTextStream read(&file);
    readDataFromCSV(read);
    file.close();
}

QString MainWindow::writeDataToCSV()
{
    for(int i=0;i<Channel_number;i++)//得到当前曲线points
    {
       points[i] = m_series[i].pointsVector();
    }
    QString sDataStr;
    for(int i=0;i<Channel_number;i++)//得到当前曲线points
    {
       sDataStr.append(QString("CH%1:").arg(i));
       QVector<QPointF>::iterator iter;
       for(iter=points[i].begin();iter!=points[i].end();iter++)
       {
         sDataStr.append(QString(",%1").arg(iter->y()));
       }
       sDataStr.append("\n");
    }

    return sDataStr;
}
//此函数代码来自SACsvStream
int advplain(const QString &s, QString &fld, int i)
{
    int j;
    j = s.indexOf(',', i); // 查找,
    if (j < 0) // 没找到
        j = s.length();
    fld = s.mid (i,j-i);//提取内容
    return j;
}
//此函数中部分代码来自SACsvStream
void MainWindow::readDataFromCSV(QTextStream &sDataStream)
{

    QString sline[10];
    QString tempValue;
    int mmax=0;
    float ymin=3.4028235e38f;
    float ymax=1.4e-45f;
    for(int i=0;i<Channel_number;i++)//得到当前曲线points
    {
        int xmax=0;
        points[i].clear();
        sline[i]=sDataStream.readLine();
        if(sline[i].size()<6)
            continue;
        sline[i]=sline[i].mid(5);
        int k=0, j=0;
        do {
            j = advplain(sline[i], tempValue, k);
            points[i].append(QPointF(xmax++,tempValue.toDouble()));
            if(tempValue.toFloat()>ymax)
                ymax=tempValue.toFloat();
            if(tempValue.toFloat()<ymin)
                ymin=tempValue.toFloat();
            tempValue.clear();
            k = j + 1;
        } while (j < sline[i].length());
        if(xmax>mmax)
            mmax=xmax;
    }
    for(int i=0;i<Channel_number;i++)
    {
       m_series[i].replace(points[i]);
    }
    Xmax=mmax;
    if(mmax>200)
        Xmin=mmax-200;
    else
        Xmin=0;
    axisX->setRange(0,mmax);
    axisY->setRange(ymin,ymax);
}

void MainWindow::on_btn_ClearSeries_clicked()
{
    for(int i=0;i<Channel_number;i++)
    {
       points[i].clear();
       m_series[i].replace(points[i]);
    }
    Xmin=0;
    Xmax=0;
    Ymin=3.4028235e38f;
    Ymax=1.4e-45f;
    axisX->setRange(0,1);
    axisY->setRange(0,1);
}

void MainWindow::on_btn_SetXSize_clicked()
{
    int xsize=0;
    XSize=ui->leXSize->text().toInt();

}
