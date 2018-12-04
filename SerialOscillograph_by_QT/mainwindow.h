#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QFile>
#include <QFileDialog>

#include "QtCharts/QChart"
#include "QLineSeries"
#include "QValueAxis"
#include "QTimer"
#include "QTime"
#include "QList"
#include "qmath.h"
#include "QPointF"
#include "QDebug"
#include "QChartView"
#include "dataprocess.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

#define Channel_number 10



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void readread();

    void on_btn_OpenSerial_clicked();

    void on_btn_help_clicked();

    void on_btn_StopDis_clicked();

    void on_action_save_triggered();

    void on_action_load_triggered();
private:
    QString writeDataToCSV();
    void readDataFromCSV(QTextStream &sDataStream);
private:
    bool SerialSta;
    DataProcess processor;
    bool chartSta;
    QSerialPort *port;
    Ui::MainWindow *ui;
    QLabel *lb_StatusBar_SerialStatus;
    QLabel *lb_StatusBar_DataRecNum;

    QChart *m_chart;
    QLineSeries m_series[Channel_number];
    QVector<QPointF> points[Channel_number];
    QValueAxis *axisX;
    QValueAxis *axisY;

    unsigned long int dataRecNum=0;
    int chartSize = 200;
    int timeId;
    int Xmin=0;
    int Xmax=0;
    float Ymin=3.4028235e38f;
    float Ymax=1.4e-45f;
};

#endif // MAINWINDOW_H
