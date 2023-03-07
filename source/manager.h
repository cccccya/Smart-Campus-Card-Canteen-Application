#ifndef MANAGER_H
#define MANAGER_H

#include <QDebug>
#include <string>
#include <vector>
#include <QString>
#include <map>
#include <Qtextcodec>
#include <iostream>
#include <QFile>
#include <QProgressDialog>
#include "student.h"
#include "canteen.h"
#include "log.h"
#include "card.h"

//各项操作信息结构体
struct Operation{
    //该操作的年 月 日 时 分 秒 毫秒 窗口
    int year,month,day,hour,minute,sec,msec,window;
    //操作 卡号 学号
    QString opt,cardid,id;
    //(充值/消费)金额
    double money;
//    重载运算符 按时间比较大小
    bool operator <(const Operation& x){
        if(year<x.year)return 1;if(year>x.year)return 0;
        if(month<x.month)return 1;if(month>x.month)return 0;
        if(day<x.day)return 1;if(day>x.day)return 0;
        if(hour<x.hour)return 1;if(hour>x.hour)return 0;
        if(minute<x.minute)return 1;if(minute>x.minute)return 0;
        if(sec<x.sec)return 1;if(sec>x.sec)return 0;
        if(msec<x.msec)return 1;if(msec>x.msec)return 0;
        return 0;
    }
};

class Manager{
public:
    Manager();
    Student* GetStudent(QString& cardid){return cardget[cardid];}
    Student* FindSelectStudent(QString &id){return idget[id];}

    void CreateAccount(QString &id,QString &name);
    void DeleteAccount(Student* k);
    void CreateCard(Student* k);
    void IssueCard(Student* k);
    void LossCard(Student* k);
    void RestoreCard(Student* k);
    bool InvestMoney(Student* k,double m);
    int Consume(bool flag,QString& cardid,double payment,int window);
    void Clear();

    int Find(QString &id,QString &name);
    int MatchStr(QString &originStr,QString &matchStr);
    int Check(QString &id,QString &name);

    bool CheckTime(int y,int mo,int d,int h,int mi,int s,int ms);
    void SetTime(int y,int mo,int d,int h,int mi,int s,int ms);

    void Store(QString &opt,int y,int mo,int d,int h,int mi,int s,int ms,int window,QString &cardid,QString &id,double money);
    void OprSort();
    void RunOperation(Operation k);
    void OprClear();

    bool IfSame(Student* k);
    int GetTime();

    void FindFriends(Student *k);
    void dfs(int k,int n,int pos,int cnt);
//    表格1数据项
    std::vector<Student*>Q;
//    表格2数据项
    std::vector<Student*>find_result;
//    储存批量操作
    std::vector<Operation>opr;
private:
    //卡号映射学生
    std::map<QString,Student*>cardget;
    //学号映射学生
    std::map<QString,Student*>idget;
//    数据挖掘中储存学生
    std::vector<QString>q1,q2,q3;
    std::vector<std::map<QString,bool> >stu;
    std::map<QString,bool>vis;
};

#endif // MANAGER_H
