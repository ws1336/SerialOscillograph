#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>

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

private:
    bool SerialSta;
    DataProcess processor;
    bool chartSta;
    QSerialPort *port;
    Ui::MainWindow *ui;

    QChart *m_chart;
    QLineSeries m_series[Channel_number];
    QVector<QPointF> points[Channel_number];
    QValueAxis *axisX;
    QValueAxis *axisY;
    int chartSize = 200;
    int timeId;
    int min=0;
    int max=200;
};

#endif // MAINWINDOW_H
