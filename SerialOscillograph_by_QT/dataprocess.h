#ifndef DATAPROCESS_H
#define DATAPROCESS_H
#include <QDebug>
#include <QByteArray>
class DataProcess
{
public:
    DataProcess();
    void add(QByteArray &input);
    bool process(char* output);
private:
    int isStart();
    int isEnd();
    bool isFull();
private:
    bool startFlag=false;
    int loc;
    int fullsize=4096;
    QByteArray data;
};

#endif // DATAPROCESS_H
