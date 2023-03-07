#include "log.h"

extern int year,month,day,hour,minute,sec,msec;
extern QFile filelog;
extern QTextStream out;
extern FILE* fp;

Log::Log(){
    filelog.open(QIODevice::WriteOnly);
    out.setCodec("UTF-8");
}

Log::~Log(){
    filelog.close();
}

//开户日志  传入参数：是否成功，姓名，学号
void Log::CreateAccount(bool flag,QString &name,QString &id){
    if(flag){
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [开户][成功] 姓名:"+name+" 学号："+id;
        out<<s<<endl;
    }else{
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [开户][失败:该学号已有账户] 姓名:"+name+" 学号："+id;
        out<<s<<endl;
    }
}

//销户日志  传入参数：姓名，学号
void Log::DeleteAccount(QString &name,QString &id){
    out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
    QString s=" [销户][成功] 姓名:"+name+" 学号："+id;
    out<<s<<endl;
}

//发卡日志  传入参数：是否成功，发卡的学生对象指针
void Log::CreateCard(bool flag,Student *k){
    if(flag){
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [发卡][成功] 姓名:"+k->name+" 学号："+k->ID;
        out<<s<<endl;
    }else{
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [发卡][失败:该生已发过卡] 姓名:"+k->name+" 学号："+k->ID;
        out<<s<<endl;
    }
}

//补卡日志  传入参数：是否成功，补卡的学生对象指针
void Log::IssueCard(bool flag,Student *k){
    if(flag){
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [补卡][成功] 姓名:"+k->name+" 学号："+k->ID;
        out<<s<<endl;
    }else{
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [补卡][失败] 姓名:"+k->name+" 学号："+k->ID;
        out<<s<<endl;
    }
}

//挂失日志  传入参数：是否成功，挂失的学生对象指针
void Log::LossCard(bool flag,Student *k){
    if(flag){
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [挂失][成功] 姓名:"+k->name+" 学号："+k->ID;
        out<<s<<endl;
    }else{
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [挂失][失败:无可挂失卡] 姓名:"+k->name+" 学号："+k->ID;
        out<<s<<endl;
    }
}

//补卡日志  传入参数：是否成功，补卡的学生对象指针
void Log::RestoreCard(bool flag,Student *k)
{
    if(flag){
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [解挂][成功] 姓名:"+k->name+" 学号："+k->ID;
        out<<s<<endl;
    }else{
        out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
        QString s=" [解挂][失败:无可解挂卡] 姓名:"+k->name+" 学号："+k->ID;
        out<<s<<endl;
    }
}

//充值日志  传入参数：不同情况对应的标志值，充值的学生对象指针，充值金额
void Log::InvestMoney(int flag,Student *k,double money){
    QString s1,s2,s3,s4,s;
    switch (flag){
        case 1:
            out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
            s1=" [充值][成功] 姓名:"+k->name+" 学号："+k->ID+" 充值金额:";s2=" 充值后余额:";
            out<<s1<<money<<s2<<k->card.back()->money<<endl;
            break;
        case 0:
            out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
            s3=" [充值][失败:该账户无可用卡] 姓名:"+k->name+" 学号："+k->ID+" 充值金额:";
            out<<s3<<money<<endl;
            break;
        case 2:
            out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
            s4=" [充值][失败:超出最大限额] 姓名:"+k->name+" 学号："+k->ID+" 充值金额:";
            out<<s4<<money<<endl;
            break;
        default:
            break;
    }
}

//消费日志 入口参数：flag表示是否输入过密码，消费的学生对象指针，消费金额，消费窗口
void Log::Consume(int flag,Student *k,double money,int window){
    QString s1,s2,s3,s4,s5,s;
    switch(flag){
        case 3:
            out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
            s1=" [消费][成功] 姓名:"+k->name+" 学号："+k->ID+" 窗口:"+QString::number(window)+" 消费金额:";s2=" 消费后余额:";
            out<<s1<<money<<s2<<k->card.back()->money<<endl;
            break;
        case 2:
            out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
            s3=" [消费][失败:余额不足] 姓名:"+k->name+" 学号："+k->ID+" 消费金额:";
            out<<s3<<money<<endl;
            break;
        case 1:
            out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
            s4=" [消费][失败:该卡不可用] 姓名:"+k->name+" 学号："+k->ID+" 消费金额:";
            out<<s4<<money<<endl;
            break;
        default:
            break;
    }
}

//账户或卡号不存在  入口参数：0表示账户不存在 1表示卡不存在，操作名字符串，学号
void Log::NotFoumd(bool flag,QString &opr,QString &id){
    out<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<sec<<":"<<msec;
    QString s;
    if(flag==0)s=" ["+opr+"][失败:账户不存在] "+"学号："+id;
    else s=" ["+opr+"][失败:该卡不存在] "+"卡号："+id;
    out<<s<<endl;
}
