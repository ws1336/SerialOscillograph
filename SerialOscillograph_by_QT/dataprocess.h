#ifndef DATAPROCESS_H
#define DATAPROCESS_H
#include <QDebug>
#include <QByteArray>
class DataProcess
{
public:
    DataProcess();
    void add(QByteArray &input);
    void clear();
    bool process(char* output);
    int getFullsize() const;
    void setFullsize(int value);
private:
    int isStart();
    int isEnd();
    bool isFull();
private:
    bool startFlag;
    int loc;
    int fullsize;
    QByteArray data;
};

#endif // DATAPROCESS_H
