#include "dataprocess.h"

DataProcess::DataProcess()
{

}

bool DataProcess::process(char *output)
{

    if(startFlag || isStart()!=0xffffffff)//如果收到包头标志，开始处理
    {
        //预处理
        if(startFlag==false)
        {
            startFlag=true;
            data=data.mid(isStart());
            loc=0;
        }
        //如果接收完成，正式处理
        if(isEnd())
        {
            for(int i=4;i<isEnd();i++)
                output[i-4]=data.at(i);
            data=data.mid(isEnd());
            startFlag=false;
            return true;
        }
        else
        {
            return false;
        }
    }
    //判断是否接受超量
    if(isFull())
    {
        startFlag=false;
        loc=0;
        data=data.mid(fullsize-4);
    }
    return false;
}

int DataProcess::isStart()
{
    if(data.size()<=4)
        return 0xffffffff;
    for(int i=0;i<data.size()-4;i++)
    {
        if(data.at(i)=='Q')
            if(data.at(i+1)=='T')
                if(data.at(i+2)=='S')
                    if(data.at(i+3)=='O')
                    {
                        return i;
                    }
    }
    return 0xffffffff;
}

int DataProcess::isEnd()
{
    if(data.size()<12)
        return 0;
    int num;
    ((char*)(&num))[0]=data.at(4);
    ((char*)(&num))[1]=data.at(5);
    ((char*)(&num))[2]=data.at(6);
    ((char*)(&num))[3]=data.at(7);
    if(data.size()>=8+num*4)
    {
        return num*4+8;
    }
    else
        return 0;
}

bool DataProcess::isFull()
{
    if(data.size()>=fullsize)
        return true;
    return false;
}

void DataProcess::add(QByteArray &input)
{
    data.append(input);

}

void DataProcess::clear()
{
    data.clear();
    startFlag=false;
    loc=0;
}
