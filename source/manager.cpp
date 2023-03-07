#include "manager.h"

extern bool flag;
extern Manager manager;
extern Canteen canteen;
extern Log logger;
extern int serial_number;
extern int year,month,day,hour,minute,sec,msec;

Manager::Manager(){

}

//开户    入口参数：学号字符串，姓名字符串
void Manager::CreateAccount(QString &id,QString &name){
    Student *person = new Student(id,name,0);
    manager.Q.push_back(person);
    idget[id]=person;
}

//销户    入口参数：执行销户的Student对象指针
void Manager::DeleteAccount(Student *k){
    k->banned=1;idget.erase(k->ID);
}

//发卡    入口参数：执行发卡的Student对象指针
void Manager::CreateCard(Student* k){
    QString str="3"+QString::number(serial_number++);
    int sum=0;
    for(auto u:str){
        sum+=u.toLatin1()-'0';
    }
    str+=9-sum%10+'0';
    Card *newcard= new Card(str,0,0);
    k->card.push_back(newcard);
    cardget[str]=k;
}

//补卡    入口参数：执行补卡的Student对象指针
void Manager::IssueCard(Student* k){
//    按规则获取卡号
    QString str="3"+QString::number(serial_number++);
    int sum=0;
    for(auto u:str){
        sum+=u.toLatin1()-'0';
    }
    str+=9-sum%10+'0';
    k->card.back()->banned=1;//当前卡挂失
    Card *newcard= new Card(str,k->card.back()->money,0);
    k->card.push_back(newcard);
    cardget[str]=k;
}

//挂失    入口参数：执行挂失的Student对象指针
void Manager::LossCard(Student* k){
    k->card.back()->banned=1;
}

//解挂    入口参数：执行解挂的Student对象指针
void Manager::RestoreCard(Student* k){
    k->card.back()->banned=0;
}

//充值    入口参数：执行充值的Student对象指针，充值金额
bool Manager::InvestMoney(Student* k,double m){
    if(k->card.back()->money+m>=1000)return 0;//超额
    k->card.back()->money+=m;return 1;
}

//消费    入口参数：flag=0未输入密码 =1已输入密码，卡号字符串，消费金额，消费窗口    出口参数：不同提示信息代表的数
int Manager::Consume(bool flag,QString& cardid,double payment,int window){
    if(!cardget.count(cardid))return 0;
    Student *k=cardget[cardid];
    if(k->banned)return 0;
    if(k->card.back()->number!=cardid||k->card.back()->banned)return 1;
    if(!flag&&((!IfSame(k)&&payment>=20)||(IfSame(k)&&k->sum+payment>=20))) return -1;//需要输入密码
    if(k->card.back()->money<payment)return 2;//余额不足
    k->card.back()->money-=payment;
    canteen.AddCanteenData(window,k,payment);//食堂窗口记录数据
//    更新当前时间段和已消费金额
    if(IfSame(k)){
        k->sum+=payment;
        k->sum=k->sum>=20?0:k->sum;
    }else{
        k->sum=payment>20?0:payment;
        k->ly=year;k->lm=month;k->ld=day;
        k->ltime=GetTime();
    }
    if(GetTime()==-1)k->sum=0;
    return 3;
}

//清空数据
void Manager::Clear(){
    cardget.clear();
    idget.clear();
    Q.clear();
    find_result.clear();
}

//清空汇总数据
void Manager::OprClear(){
    opr.clear();
}

//检查当前时间是否与该生上笔消费处于同一时间段入口  入口参数：执行检查的学生对象指针    出口参数：1同一时间段 0不同时间段
bool Manager::IfSame(Student* k){
    return k->ly==year&&k->lm==month&&k->ld==day&&k->ltime==GetTime();
}

//获取当前时间段   出口参数：1早餐 2中餐 3晚餐 -1其他
int Manager::GetTime(){
    if(hour==7||hour==8)return 1;
    if(hour==11||hour==12)return 2;
    if(hour==17||hour==18)return 3;
    return -1;
}

//储存操作信息    入口参数：年，月，日，时，分，秒，毫秒，窗口，卡号字符串，学号字符串，金额
void Manager::Store(QString &opt,int y,int mo,int d,int h,int mi,int s,int ms,int window,QString &cardid,QString &id,double money){
    Operation o={y,mo,d,h,mi,s,ms,window,opt,cardid,id,money};
    opr.push_back(o);
}

//对批量操作按时间顺序
void Manager::OprSort(){
    std::sort(opr.begin(),opr.end());
}

//执行批量操作    入口参数：需要被执行的操作
void Manager::RunOperation(Operation k){
    if(!CheckTime(k.year,k.month,k.day,k.hour,k.minute,k.sec,k.msec))return;
    SetTime(k.year,k.month,k.day,k.hour,k.minute,k.sec,k.msec);
    if(k.opt=="消费"){
        if(k.window<1||k.window>99||k.cardid.length()!=7||k.money<=0)return;
        int ret=Consume(1,k.cardid,k.money,k.window);
        if(ret==0){
            QString s="消费";logger.NotFoumd(1,s,k.cardid);
            return;
        }
        logger.Consume(ret,cardget[k.cardid],k.money,k.window);
    }else if(k.opt=="充值"){
        if(!idget.count(k.id)){
            QString s="充值";logger.NotFoumd(0,s,k.id);
            return;
        }
        Student* u=idget[k.id];
        if(u->card.empty()||u->card.back()->banned)logger.InvestMoney(0,u,k.money);
        else if(InvestMoney(u,k.money))logger.InvestMoney(1,u,k.money);
        else logger.InvestMoney(2,u,k.money);
    }else if(k.opt=="挂失"){
        if(!idget.count(k.id)){
            QString s="挂失";logger.NotFoumd(0,s,k.id);
            return;
        }
        Student* u=idget[k.id];
        if(u->card.empty()||u->card.back()->banned){logger.LossCard(1,u);return;}
        LossCard(u);logger.LossCard(1,u);
    }else if(k.opt=="解挂"){
        if(!idget.count(k.id)){
            QString s="解挂";logger.NotFoumd(0,s,k.id);
           return;
        }
        Student* u=idget[k.id];
        if(u->card.empty()||!u->card.back()->banned){logger.RestoreCard(1,u);return;}
        RestoreCard(u);logger.RestoreCard(1,u);
    }else if(k.opt=="销户"){
        if(!idget.count(k.id)){
            QString s="销户";logger.NotFoumd(0,s,k.id);
            return;
        }
        Student* u=idget[k.id];
        DeleteAccount(u);logger.DeleteAccount(u->name,u->ID);
    }else if(k.opt=="补卡"){
        if(!idget.count(k.id)){
            QString s="补卡";logger.NotFoumd(0,s,k.id);
            return;
        }
        Student* u=idget[k.id];
        if(!u->card.back()->banned){logger.IssueCard(0,u);return;}
        IssueCard(u);logger.IssueCard(1,u);
    }
}

/********************************************************
 * 动态规划实现字符串匹配
 * dp[i][j]=1/0表示匹配串的前i个字符能/不能与原串的前j个字符匹配
 * 入口参数：原串，匹配串
 * 出口参数：1表示匹配成功，0表示匹配失败
*********************************************************/
int Manager::MatchStr(QString &originStr,QString &matchStr){
    int orilen=(int)originStr.length();
    int matlen=(int)matchStr.length();
    std::vector<std::vector<int> >dp(matlen+1,std::vector<int>(orilen+1,0));
    dp[0][0]=1;
    //因为字符串从0开始填入,故动态规划中考虑匹配串字符时往前挪一位
    for(int i=1;i<=matlen;i++){
        dp[i][0]=dp[i - 1][0]&&(matchStr[i-1]=='*');
        for(int j=1;j<=orilen;j++) {
            if(matchStr[i-1]=='*')//为空或者原串第j个字符
                dp[i][j]=(dp[i-1][j]||dp[i][j-1]);
            else//字符相同或者为?
                dp[i][j]=(dp[i-1][j-1]&&(matchStr[i-1]=='?'||originStr[j-1]==matchStr[i-1]));
        }
    }
    return dp[matlen][orilen];
}

//查询    入口参数：学号和姓名字符串   出口参数：查询到的结果数量
int Manager::Find(QString &id,QString &name){
    int num=0;
    find_result.clear();
    for(auto k:Q){
        if(k->banned)continue;
        if(MatchStr(k->ID,id)&&MatchStr(k->name,name)){
           find_result.push_back(k);
           num++;
        }
    }
    return num;
}

//输入的姓名，学号格式是否合法    入口参数：学号和姓名字符串   出口参数：合法化的不同情况对应的值
int Manager::Check(QString &id,QString &name){
    if(manager.idget[id])
        return 2;
    if(id.isEmpty()||name.isEmpty()||id.length()!=10)
        return 1;
    for(auto k:id)if(k>'9'||k<'0')
        return 1;
    for(auto k:name)if((k<='9'&&k>='0')||k=='?'||k=='*')
        return 1;
    return 0;
}

//比较时间大小    入口参数：年，月，日，时，分，秒，毫秒 出口参数：1表示该时间晚于当前时间 0表示该事件早于当前时间
bool Manager::CheckTime(int y,int mo,int d,int h,int mi,int s,int ms){
    if(year<y)return 1;if(year>y)return 0;
    if(month<mo)return 1;if(month>mo)return 0;
    if(day<d)return 1;if(day>d)return 0;
    if(hour<h)return 1;if(hour>h)return 0;
    if(minute<mi)return 1;if(minute>mi)return 0;
    if(sec<s)return 1;if(sec>s)return 0;
    if(msec<ms)return 1;if(msec>ms)return 0;
    return 1;
}

//设置时间  入口参数：年，月，日，时，分，秒，毫秒
void Manager::SetTime(int y,int mo,int d,int h,int mi,int s,int ms){
    year=y;month=mo;day=d;hour=h;minute=mi;sec=s;msec=ms;
}

/****************************************************************
 * 应用Apriori算法进行数据挖掘
 * 候选消除算法
 * 在原有数据中扫描一遍 清除不满足条件的元素 形成一级频繁集
 * 找出k-1级频繁集的所有元素形成的k元组 清除不满足条件的k元组 形成k级频繁集
 * 直至找不出某级频繁集 则上一级频繁集为挖掘结果
*****************************************************************/
//深搜找n元组    入口参数：当前找n元组的第k个元素，n元组，从第pos个同学开始枚举，挖掘对象共有cnt次消费
void Manager::dfs(int k,int n,int pos,int cnt){
    if(k==n+1){//形成n元组
        int sum=0;
        for(int i=0;i<cnt;i++){
            int flag=1;
            for(int j=0;j<n;j++){
                if(!stu[i][q3[j]])flag=0;
            }
            if(flag)sum++;
        }
        if(sum>=20){//满足条件
            for(QString u:q3){
                if(!vis[u]) q1.push_back(u),vis[u]=1;
            }
        }
        return;
    }
    for(int i=pos;i<(int)q2.size();i++){
        q3.push_back(q2[i]);
        dfs(k+1,n,i+1,cnt);
        q3.pop_back();
    }
}
void Manager::FindFriends(Student* k){
    stu.clear();q1.clear();q3.clear();
    int t=5,cnt=-1;
    //在汇总数据中查找与k同学在相近时间相近窗口消费的同学 放入q1当中
    for(int i=0;i<(int)canteen.AllMessage.size();i++){
        if(!canteen.AllMessage[i]->Check2(i-1==-1?canteen.AllMessage[(int)canteen.AllMessage.size()-1]->key:canteen.AllMessage[i-1]->key))
            wrong.push_back(i-1==-1?canteen.AllMessage[(int)canteen.AllMessage.size()-1]:canteen.AllMessage[i-1]);
        int h=canteen.AllMessage[i]->t2.toInt()/1000000,mi=canteen.AllMessage[i]->t2.toInt()/10000%100,window=canteen.AllMessage[i]->window;
        if(k->ID==canteen.AllMessage[i]->ID){
            if(h==7||h==8||h==11||h==12||h==17||h==18);
            else continue;//不考虑不在就餐时间的消费记录
            cnt++;
            stu.resize(cnt+1);
            for(auto j=i-1;j>=0;j--){
                QString t1=canteen.AllMessage[i]->t1;int jwin=canteen.AllMessage[j]->window;
                if(canteen.AllMessage[j]->t1==t1&&abs(window-jwin)<=2){
                    int jh=canteen.AllMessage[j]->t2.toInt()/1000000,jmi=canteen.AllMessage[j]->t2.toInt()/10000%100;
                    if((jh==h&&jmi<=mi+t&&jmi>=mi-t)||(jh==h-1&&mi+60-jmi<=t)){
                        if(!vis[canteen.AllMessage[j]->ID])q1.push_back(canteen.AllMessage[j]->ID);
                        vis[canteen.AllMessage[j]->ID]=1;
                        stu[cnt][canteen.AllMessage[j]->ID]=1;
                    }
                    else break;
                }
                else break;
            }
            for(auto j=i+1;j<(int)canteen.AllMessage.size();j++){
                QString t1=canteen.AllMessage[i]->t1;int jwin=canteen.AllMessage[i]->window;
                if(canteen.AllMessage[j]->t1==t1&&abs(window-jwin)<=2){
                    int jh=canteen.AllMessage[j]->t2.toInt()/1000000,jmi=canteen.AllMessage[j]->t2.toInt()/10000%100;
                    if((jh==h&&jmi<=mi+t&&jmi>=mi-t)||(jh==h+1&&jmi+60-mi<=t)){
                        if(!vis[canteen.AllMessage[j]->ID])q1.push_back(canteen.AllMessage[j]->ID);
                        vis[canteen.AllMessage[j]->ID]=1;
                        stu[cnt][canteen.AllMessage[j]->ID]=1;
                    }
                    else break;
                }
                else break;
            }
        }
    }
    //生成k同学的频繁项集 在这里生成三级频繁选项集
    for(int i=1;i<=3;i++){
        qDebug()<<i<<q1.size();
        q2.clear();
        for(auto k:q1)q2.push_back(k);
        q1.clear();vis.clear();
        dfs(1,i,0,cnt);
        if(q1.empty())break;
    }
    find_result.clear();
    find_result.push_back(k);
    for(auto u:q2)find_result.push_back(idget[u]);
}
