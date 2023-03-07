#include "message.h"

Message::Message(){

}

Message::Message(QString &T1,QString &T2,QString &id,QString &cid,double payment,int win){
    t1=T1;t2=T2;ID=id;cardid=cid;money=payment;window=win;
    key=GetKey();key2=0;
}
//根据信息得到Hash值
int Message::GetKey(){
    int v=0;
    for(auto k:t1){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:t2){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:ID){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:cardid){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:QString::number(money)){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:QString::number(window)){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    return v;
}

//判断Hash值是否改变
bool Message::Check(){
    int v=0;
    for(auto k:t1){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:t2){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:ID){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:cardid){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:QString::number(money)){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    for(auto k:QString::number(window)){
        char c=k.toLatin1();
        v=(1ll*v*528+c)%P;
    }
    return v==key;
}

//根据前后信息得到Hash值2
int Message::GetKey2(int i){
    return 1ll*(i+913)*(key+528)%P;
}

//判断Hash值2是否改变
bool Message::Check2(int i){
    return key2==1ll*(i+913)*(key+528)%P;
}
