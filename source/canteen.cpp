#include "canteen.h"

Canteen::Canteen(){
    CanteenData.resize(100);win.resize(100);
//    从文件获取食堂各窗口记录位置
    QFile file("data\\wz003.txt");
    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString str = stream.readLine();
        if(str=="WZ"){
            while(!stream.atEnd()){
                str=stream.readLine(); // 不包括“/n”的一行文本
                QString p=str.section(',',1,1).trimmed();p.chop(1);
                int window=str.section(',',0,0).trimmed().toInt(),pos=p.toInt();
                CanteenData[window].resize(pos);
                win[window].l=0;win[window].r=p.toInt()-1;
            }
        }
        file.close();
    }else{
        for(int i=1;i<100;i++)win[i].l=0,win[i].r=-1;
    }
}

//数字返回其格式化字符串 1->"01"   入口参数：需要格式化输出的数字 出口参数：格式化输出字符串
inline QString Canteen::GetString(int n){
    return n>9?QString::number(n):getstring[n];
}

//消费数据存入食堂各窗口   入口参数：窗口，消费学生，消费金额
void Canteen::AddCanteenData(int window,Student* k,double payment){
    QString s1=QString::number(year)+GetString(month)+GetString(day),s2=GetString(hour)+GetString(minute)+GetString(sec)+GetString(msec/10);
//    统计当天消费次数和金额
    if(win[window].y==year&&win[window].m==month&&win[window].d==day)win[window].money+=payment,win[window].times++;
    else win[window].y=year,win[window].m=month,win[window].d=day,win[window].money=payment,win[window].times=1;
//    更新窗口数据首尾位置
    if(win[window].r==-1)win[window].r++;
    else{
        win[window].r=(win[window].r+1)%60000;
        if(win[window].l==win[window].r)win[window].l++;
    }
    if(win[window].l==win[window].r+1)CanteenData[window][win[window].r]=(new Message(s1,s2,k->ID,k->card.back()->number,payment,window));
    else CanteenData[window].push_back(new Message(s1,s2,k->ID,k->card.back()->number,payment,window));
}

//自定义比较函数 按时间大小排序
struct cmp{
    bool operator() (const Message *a,const Message *b){
        if(a->t1==b->t1)return a->t2>b->t2;
        return a->t1>b->t1;
    }
};

//数据汇总
void Canteen::Merge()
{
//    归并排序
    std::vector<std::vector<Message*> >windata;
    std::priority_queue<Message*,std::vector<Message*>,cmp>q;
    int pos[100];
    windata.resize(100);
//    各窗口从首到尾放入排序数组，保证窗口内部数据有序
    for(int i=1;i<100;i++){
        pos[i]=0;
        if(win[i].l==win[i].r+1&&win[i].r!=-1){
            for(int j=win[i].l;j<=60000;j++){
                if(CanteenData[i][j]!=nullptr){
                        windata[i].push_back(CanteenData[i][j]);
                        if(!CanteenData[i][j]->Check())wrong.push_back(CanteenData[i][j]);//Hash值检查该数据是否被修改
                }
            }
            for(int j=0;j<=win[i].r;j++){
                if(CanteenData[i][j]!=nullptr){
                        windata[i].push_back(CanteenData[i][j]);
                        if(!CanteenData[i][j]->Check())wrong.push_back(CanteenData[i][j]);//Hash值检查该数据是否被修改
                }
            }
        }else{
            for(int j=win[i].l;j<=win[i].r;j++){
                if(CanteenData[i][j]!=nullptr){
                        windata[i].push_back(CanteenData[i][j]);
                        if(!CanteenData[i][j]->Check())wrong.push_back(CanteenData[i][j]);//Hash值检查该数据是否被修改
                }
            }
        }
    }
//    将各窗口数据放入堆中
    for(int i=1;i<100;i++){
        if(!windata[i].empty())q.push(windata[i][0]),pos[i]++;
    }
//    多路归并，堆顶元素放入汇总数据，并从该窗口放入一个数据
    while(!q.empty()){
        Message *s=q.top();q.pop();AllMessage.push_back(s);
        int win=s->window;
        if(pos[win]<(int)windata[win].size()){
            q.push(windata[win][pos[win]++]);
        }
    }
////    快速排序
////    各窗口从首到尾放入排序数组
//    for(int i=1;i<100;i++){
//        if(win[i].l==win[i].r+1&&win[i].r!=-1){
//            for(int j=win[i].l;j<=60000;j++){
//                if(CanteenData[i][j]!=nullptr){
//                        AllMessage.push_back(CanteenData[i][j]);
//                        if(!CanteenData[i][j]->Check())wrong.push_back(CanteenData[i][j]);
//                }
//            }
//            for(int j=0;j<=win[i].r;j++){
//                if(CanteenData[i][j]!=nullptr){
//                        AllMessage.push_back(CanteenData[i][j]);
//                        if(!CanteenData[i][j]->Check())wrong.push_back(CanteenData[i][j]);
//                }
//            }
//        }else{
//            for(int j=win[i].l;j<=win[i].r;j++){
//                if(CanteenData[i][j]!=nullptr){
//                        AllMessage.push_back(CanteenData[i][j]);
//                        if(!CanteenData[i][j]->Check())wrong.push_back(CanteenData[i][j]);
//                }
//            }
//        }
//    }
//    QuickSort(0,((int)AllMessage.size()-1));
    for(int i=0;i<(int)AllMessage.size();i++){
        AllMessage[i]->key2=AllMessage[i]->GetKey2(i-1==-1?AllMessage[(int)AllMessage.size()-1]->key:AllMessage[i-1]->key);
    }
}

//比较Message*所指元素的时间大小   入口参数：需要比较的Message对象指针   出口参数：1表示a信息时间晚于b信息
bool Canteen::Compare(Message* a,Message* b){
    if(a->t1==b->t1)return a->t2>b->t2;
    return a->t1>b->t1;
}

//递归实现快排    传入参数：排序左右区间
void Canteen::QuickSort(int l, int r){
    if(l<r){
        int i=l,j=r;
        Message* x=AllMessage[i];
        while(i<j){
            //从右向左找第一个小于x的数
            while(i<j&&Compare(AllMessage[j],x))
                j--;
            if(i<j)
                AllMessage[i++]=AllMessage[j];
            //从左向右找第一个大于x的数
            while(i<j&&!Compare(AllMessage[i],x))
                i++;
            if(i<j)
                AllMessage[j--]=AllMessage[i];
        }
        AllMessage[i]=x;
        QuickSort(l, i-1);//递归调用
        QuickSort(i+1, r);//递归调用
    }
}
