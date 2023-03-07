#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <QFile>
#include <QDebug>
#include <QString>
#include <Qtextcodec>
#include <string>
#include "student.h"

class Log{
public:
    Log();
    ~Log();
    void CreateAccount(bool flag,QString &name,QString &id);
    void DeleteAccount(QString &name,QString &id);
    void CreateCard(bool flag,Student *k);
    void IssueCard(bool flag,Student *k);
    void LossCard(bool flag,Student *k);
    void RestoreCard(bool flag,Student *k);
    void InvestMoney(int flag,Student *k,double money);
    void Consume(int flag,Student *k,double money,int window);
    void NotFoumd(bool flag,QString &opr,QString &id);
};

#endif // LOG_H
