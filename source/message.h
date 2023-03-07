#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>

class Message
{
public:
    Message();
    Message(QString &T1,QString &T2,QString &id,QString &cid,double payment,int win);
    bool Check();
    bool Check2(int line);
    int GetKey();
    int GetKey2(int line);
//    年月日 时分秒毫秒 学号 卡号 金额
    QString t1,t2,ID,cardid;
//    消费金额
    double money;
//    消费窗口 该条信息的Hash值 该条消息与消息所在位置的Hash值
    int window,key,key2;
    bool operator <(const Message& x)
    {
        if(t1==x.t1)return t2<x.t2;
        return t1<x.t1;
    }
private:
//    Hash值模数
    const int P=998244353;
};

#endif // MESSAGE_H
