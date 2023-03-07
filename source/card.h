#ifndef CARD_H
#define CARD_H

#include <string>
#include <vector>
#include <QString>
#include <map>

class Card{
public:
    Card();
    Card(QString &n,double m,bool b);
//    卡号
    QString number;
//    钱数
    double money;
//    是否挂失
    bool banned;
};

#endif // CARD_H
