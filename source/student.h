#ifndef STUDENT_H
#define STUDENT_H

#include <QString>
#include <iostream>
#include <list>
#include "card.h"
#include "message.h"

extern int year,month,day,hour,minute,sec,msec;

class Student{
public:
    Student();
    Student(QString &s1,QString &s2,bool b){
        ID+=s1;name+=s2;banned=b;
    }
    inline QString GetString(int n);
//    学号 姓名 卡密码
    QString ID,name,password="8888";
//    账户是否在系统中
    bool banned;
//    上次消费 年 月 日 时间段 金额
    int ly=0,lm=0,ld=0,ltime=0,sum=0;
//    链表存卡
    std::list<Card*> card;
private:
    //    数字格式化字符串数组
    QString getstring[10]={"00","01","02","03","04","05","06","07","08","09"};

};

#endif // STUDENT_H
