#ifndef CANTEEN_H
#define CANTEEN_H

#include <QString>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <queue>
#include <QDebug>
#include "student.h"
#include "message.h"
#include "window.h"

extern int year,month,day,hour,minute,sec,msec;
extern std::vector<Message*>wrong;

class Canteen{
public:
    Canteen();
    inline QString GetString(int n);
    void AddCanteenData(int window,Student* k,double payment);
    bool Compare(Message* a,Message* b);
    void QuickSort(int l, int r);
    void Merge();
//    存储各窗口数据
    std::vector<std::vector<Message*> >CanteenData;
//    窗口数组
    std::vector<Window>win;
//    存储汇总数组
    std::vector<Message*>AllMessage;
private:
//    数字格式化字符串数组
    QString getstring[10]={"00","01","02","03","04","05","06","07","08","09"};
};

#endif // CANTEEN_H
