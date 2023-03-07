#include "mainwindow.h"
#include "ui_mainwindow.h"

extern bool flag;
extern int year,month,day,hour,minute,sec,msec;
extern Manager manager;
extern Canteen canteen;
extern Log logger;
extern std::vector<Message*>wrong;
int num[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

QProgressDialog *p0d;
QTimer *t;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("校园卡管理系统");
    //设置输入框最大输入长度
    ui->money->setMaxLength(6);
    ui->year->setMaxLength(4);
    ui->month->setMaxLength(2);
    ui->day->setMaxLength(2);
    ui->hour->setMaxLength(2);
    ui->min->setMaxLength(2);
    ui->sec->setMaxLength(2);
    ui->msec->setMaxLength(2);
//    时间lineEdit不可写
    ui->time1->setEnabled(false);
    ui->time2->setEnabled(false);
//    设置表格格式
    ui->tableWidget_1->setColumnCount(4);
    ui->tableWidget_1->setHorizontalHeaderLabels(QStringList()<<"姓名"<<"学号"<<"卡号"<<"余额");
    ui->tableWidget_1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);\
    ui->tableWidget_1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_2->setColumnCount(4);
    ui->tableWidget_2->setHorizontalHeaderLabels(QStringList()<<"姓名"<<"学号"<<"卡号"<<"余额");
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应宽度
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);//只读
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);//单击选择整行
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);//只能单选
//    点击批量开户 读取开户数据
    connect(ui->plkh,&QPushButton::clicked,[=](){
//        选择开户文件
        QString path=QFileDialog::getOpenFileName(this,"打开文件","data");
        QFile file(path);
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString str = stream.readLine();
        if(str=="KH"){
//            设置开户时间
            if(manager.CheckTime(2021,9,3,0,0,0,0))manager.SetTime(2021,9,3,0,0,0,0);
            else {QMessageBox::warning(this,"警告","不能倒流时间");file.close();return;}
            while(!stream.atEnd()){
                str=stream.readLine(); // 不包括“/n”的一行文本
                QString id=str.section(',',0,0).trimmed(),name=str.section(',',1,1).trimmed();
                name.chop(1);
                int ret=manager.Check(id,name);
                if(ret==0)//账户不存在
                {
                    manager.CreateAccount(id,name);
                    logger.CreateAccount(1,name,id);
                }
                else if(ret==2)//账户存在
                {
                    logger.CreateAccount(0,name,id);
                }
            }
            file.close();
            Reflush1();ReflushTime();
            QMessageBox::information(this,"提示","批量开户成功");
        }else{
            QMessageBox::warning(this,"警告","文件选择错误");
        }
    });
//    点击批量发卡 对账户无卡同学发卡
    connect(ui->plfk,&QPushButton::clicked,[=](){
        QMessageBox msgBox;
        msgBox.setText("将给所有同学发卡");
        msgBox.setInformativeText("您确定继续吗?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                if(manager.CheckTime(2021,9,3,0,0,0,0))manager.SetTime(2021,9,3,0,0,0,0);
                else {QMessageBox::warning(this,"警告","不能倒流时间");return;}
                flag=0;//判断是否进行过发卡操作
                for(auto k:manager.Q){
                    if(!k->card.empty()){
                        logger.CreateCard(0,k);
                        continue;
                    }
                    flag=1;
                    manager.CreateCard(k);logger.CreateCard(1,k);
                }
                Reflush1();ReflushTime();
                if(flag)QMessageBox::information(this,"提示","批量发卡成功");
                else if(manager.Q.empty())QMessageBox::information(this,"提示","无账户");
                else QMessageBox::information(this,"提示","所有账户均拥有卡");
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    批量卡操作
    connect(ui->plkcz,&QPushButton::clicked,[=](){
        QFile file1("data\\xf014.txt");
        QFile file2("data\\cz002.txt");
        if(!file1.open(QIODevice::ReadOnly)||!file2.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,"警告","未找到相关文件");return;
        }
        QTextStream stream1(&file1);
        stream1.setCodec("UTF-8");
        QString str;
        str=stream1.readLine();
        int window=1,cnt=0;
//        读取消费文件
        if(str=="XF"){
            while(!stream1.atEnd()){
                str=stream1.readLine(); // 不包括“/n”的一行文本
                if(str[0]=='W'){//更换窗口
                    str.remove(0,1);
                    window=str.toInt();
                    continue;
                }
                QString cardid=str.section(',',0,0).trimmed(),t1=str.section(',',1,1).trimmed(),t2=str.section(',',2,2).trimmed(),money=str.section(',',3,3).trimmed();
                money.chop(1);
                int y=t1.toInt()/10000,mo=(t1[4].toLatin1()-'0')*10+t1[5].toLatin1()-'0',d=(t1[6].toLatin1()-'0')*10+t1[7].toLatin1()-'0',h=(t2[0].toLatin1()-'0')*10+t2[1].toLatin1()-'0',mi=(t2[2].toLatin1()-'0')*10+t2[3].toLatin1()-'0',s=(t2[4].toLatin1()-'0')*10+t2[5].toLatin1()-'0',ms=((t2[6].toLatin1()-'0')*10+t2[7].toLatin1()-'0')*10;
                QString opt="消费",id="";
                manager.Store(opt,y,mo,d,h,mi,s,ms,window,cardid,id,money.toDouble());
                cnt++;
            }
            file1.close();
            qDebug()<<"ok";
        }else{
            QMessageBox::warning(this,"警告","打开错误文件");
        }
        QTextStream stream2(&file2);
        stream2.setCodec("UTF-8");
        str=stream2.readLine();
//        读取其他卡操作文件
        if(str=="CZ"){
            while(!stream2.atEnd()){
                str=stream2.readLine(); // 不包括“/n”的一行文本
                QString t=str.section(',',0,0).trimmed(),opt=str.section(',',1,1).trimmed(),id=str.section(',',2,2).trimmed(),money,cardid="";
                int y=(t[0].toLatin1()-'0')*1000+(t[1].toLatin1()-'0')*100+(t[2].toLatin1()-'0')*10+(t[3].toLatin1()-'0'),mo=(t[4].toLatin1()-'0')*10+t[5].toLatin1()-'0',d=(t[6].toLatin1()-'0')*10+t[7].toLatin1()-'0',h=(t[8].toLatin1()-'0')*10+t[9].toLatin1()-'0',mi=(t[10].toLatin1()-'0')*10+t[11].toLatin1()-'0',s=(t[12].toLatin1()-'0')*10+t[13].toLatin1()-'0',ms=((t[14].toLatin1()-'0')*10+t[15].toLatin1()-'0')*10;
                if(opt=="充值")money=str.section(',',3,3).trimmed(),money.chop(1);
                else id.chop(1);
                manager.Store(opt,y,mo,d,h,mi,s,ms,window,cardid,id,money.toDouble());
                cnt++;
            }
            file2.close();
            qDebug()<<"ok";
        }else{
            QMessageBox::warning(this,"警告","打开错误文件");
        }
        manager.OprSort();
        qDebug()<<"finishsort";
        QProgressDialog dialog(tr("批量卡操作进度"), tr("取消"), 0, cnt, this);
        dialog.setWindowTitle(tr("进度对话框"));     // 设置窗口标题
        dialog.setWindowModality(Qt::WindowModal);  // 将对话框设置为模态
        dialog.show();
        int sum=0;
        //循环进行所有操作
        for(auto k:manager.opr){
            manager.RunOperation(k);
            sum++;
            dialog.setValue(sum);
            QCoreApplication::processEvents();
            if(dialog.wasCanceled()){
                manager.opr.clear();
                return;
            }
        }
        qDebug()<<"finish";
        manager.OprClear();
        Reflush1();ReflushTime();
        dialog.setValue(cnt);
    });
//    清除数据
    connect(ui->clear,&QPushButton::clicked,[=](){
        QMessageBox msgBox;
        msgBox.setText("所有数据将被清除");
        msgBox.setInformativeText("您还要继续吗?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                manager.Clear();year=month=day=hour=minute=sec=msec=0;
                Reflush1();ReflushTime();
                QMessageBox::information(this,"提示","清空账户成功");
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    输入相关信息并查询
    connect(ui->ask,&QPushButton::clicked,[=](){
        QString id=ui->xuehao->text(),name=ui->xingming->text();
        if(id.isEmpty()&&name.isEmpty()){
            QMessageBox::warning(this,"警告","请输入学号或姓名");return;
        }
        if(!id.isEmpty()){
            for(auto k:id){
                if((k>'9'||k<'0')&&k!='?'&&k!='*'){
                    QMessageBox::warning(this,"警告","请输入正确的学号");return;
                }
            }
        }
        else id+="*";
        if(name.isEmpty())name+="*";
        manager.Find(id,name);
        QMessageBox::information(this,"提示","查找到"+QString::number(manager.find_result.size())+"条结果");
        Reflush2();
    });
//    查询一段时间的消费记录
    connect(ui->ask_xf,&QPushButton::clicked,[=](){
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        QString t1=ui->starttime->text(),t2=ui->endtime->text();
        if(t1.isEmpty()||t2.isEmpty()||t1>t2){
            QMessageBox::warning(this,"警告","请输入正确的查询日期区间");return;
        }
//        二分查找起止时间之内消费数据在汇总记录中的位置
        int l1=0,r1=canteen.AllMessage.size()-1,l2=l1,r2=r1;
        while(l1<r1){
            int mid=(l1+r1)/2;
            if(canteen.AllMessage[mid]->t1>=t1)r1=mid;
            else l1=mid+1;
        }
        while(l2<r2){
            int mid=(l2+r2+1)/2;
            if(canteen.AllMessage[mid]->t1<=t2)l2=mid;
            else r2=mid-1;
        }
        if(r1==-1||r2==-1){QMessageBox::information(this,"提示","该生共消费0元");return;}//起止时间内无记录
        double money_sum=0;
        for(int i=l1;i<=l2;i++)
            if(canteen.AllMessage[i]->ID==k->ID)
                money_sum+=canteen.AllMessage[i]->money;
        QMessageBox::information(this,"提示","该生共消费"+QString::number(money_sum)+"元");
    });
//    发卡
    connect(ui->fk,&QPushButton::clicked,[=](){
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        if(!k->card.empty()){
            QMessageBox::warning(this,"警告",k->name+"已办理过校园卡");
            logger.CreateCard(0,k);return;
        }
        QMessageBox msgBox;
        msgBox.setText(k->name+" "+k->ID+"将进行发卡操作");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                manager.CreateCard(k);
                QMessageBox::information(this,"提示","发卡成功");
                logger.CreateCard(1,k);
                Reflush2();
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    补卡
    connect(ui->bk,&QPushButton::clicked,[=](){
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        if(k->card.empty()){
            QMessageBox::warning(this,"警告",k->name+"还未开卡\n请先进行开卡操作");
            logger.IssueCard(0,k);return;
        }
        QMessageBox msgBox;
        msgBox.setText(k->name+" "+k->ID+"将进行补卡操作");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                manager.IssueCard(k);
                QMessageBox::information(this,"提示","补卡成功");
                logger.IssueCard(1,k);
                Reflush2();
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    挂失
    connect(ui->gs,&QPushButton::clicked,[=](){
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        if(k->card.empty()){
            QMessageBox::warning(this,"警告",k->name+"还未开卡\n请先进行开卡操作");
            logger.LossCard(0,k);return;
        }
        if(k->card.back()->banned){
            QMessageBox::warning(this,"警告",k->name+"无可用卡");
            logger.LossCard(0,k);return;
        }
        QMessageBox msgBox;
        msgBox.setText(k->name+" "+k->ID+"将进行挂失操作");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                manager.LossCard(k);
                QMessageBox::information(this,"提示","挂失成功");
                logger.LossCard(1,k);
                Reflush2();
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    解挂
    connect(ui->jg,&QPushButton::clicked,[=](){
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        if(k->card.empty()){
            QMessageBox::warning(this,"警告",k->name+"还未开卡\n请先进行开卡操作");
            logger.RestoreCard(0,k);return;
        }
        if(!k->card.back()->banned){
            QMessageBox::warning(this,"警告",k->name+"有可用卡");
            logger.RestoreCard(0,k);return;
        }
        QMessageBox msgBox;
        msgBox.setText(k->name+" "+k->ID+"将进行解挂操作");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                manager.RestoreCard(k);
                QMessageBox::information(this,"提示","解挂成功");
                logger.RestoreCard(1,k);
                Reflush2();
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    充值
    connect(ui->cz,&QPushButton::clicked,[=](){
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        QString m=ui->money->text();
        double money=m.toDouble();
        if(money==0){
            QMessageBox::warning(this,"警告","请输入正确格式的金额");return;
        }
        if(k->card.empty()){
            QMessageBox::warning(this,"警告",k->name+"还未开卡\n请先进行开卡操作");
            logger.InvestMoney(0,k,money);return;
        }
        if(k->card.back()->banned){
            QMessageBox::warning(this,"警告",k->name+"无可用卡");
            logger.InvestMoney(0,k,money);return;
        }
        QMessageBox msgBox;
        msgBox.setText(k->name+" "+k->ID+"将进行充值操作");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
//                通过返回值判断充值是否超额
                if(manager.InvestMoney(k,money)){
                    QMessageBox::information(this,"提示","充值成功");
                    logger.InvestMoney(1,k,money);
                }else{
                    QMessageBox::warning(this,"警告","超额\n充值失败");
                    logger.InvestMoney(2,k,money);
                }
                Reflush2();
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    开户
    connect(ui->kh,&QPushButton::clicked,[=](){
        QString id=ui->xuehao->text(),name=ui->xingming->text();
        int ret=manager.Check(id,name);
        switch(ret){
            case 0:
                break;
            case 1:
                QMessageBox::warning(this,"警告","请输入正确的学号和姓名");
                return;
            case 2:
                QMessageBox::warning(this,"警告","该学号已有账户");
                logger.CreateAccount(0,name,id);
                return;
            default:
                break;
        }
        QMessageBox msgBox;
        msgBox.setText("将给"+name+" "+id+"开户");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                manager.CreateAccount(id,name);
                QMessageBox::information(this,"提示","开户"
                                                   "成功");
                logger.CreateAccount(1,name,id);
                Reflush2();
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    销户
    connect(ui->xh,&QPushButton::clicked,[=](){
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        QMessageBox msgBox;
        msgBox.setText(k->name+" "+k->ID+"将进行销户操作");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                manager.DeleteAccount(k);
                QMessageBox::information(this,"提示","销户成功");
                logger.DeleteAccount(k->name,k->ID);
                Reflush2();
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    });
//    设置时间
    connect(ui->settime,&QPushButton::clicked,[=](){
        int y=ui->year->text().toInt(),mo=ui->month->text().toInt(),d=ui->day->text().toInt(),h=ui->hour->text().toInt(),mi=ui->min->text().toInt(),s=ui->sec->text().toInt(),ms=ui->msec->text().toInt()*10;
        if(mo>12||mo<1||d>num[mo]||d<0||h>23||h<0||mi>60||mi<0||s>60||s<0||ms>990||ms<0){
            QMessageBox::warning(this,"警告","时间格式不正确");return;
        }
        if(!manager.CheckTime(y,mo,d,h,mi,s,ms)){
            QMessageBox::warning(this,"警告","不能倒流时间");return;
        }
        manager.SetTime(y,mo,d,h,mi,s,ms);
        QMessageBox::information(this,"提示","设置时间成功");
        ReflushTime();
    });
//    消费
    connect(ui->pay,&QPushButton::clicked,[=](){
        int window=ui->window->text().toInt();
        QString cardid=ui->cardid->text();
        double payment=ui->payment->text().toDouble();
        if(window<1||window>99||cardid.length()!=7||payment<=0){
            QMessageBox::warning(this,"警告","数据格式不正确");return;
        }
//        根据函数返回值判断消费是否成功
        int ret=manager.Consume(0,cardid,payment,window);
        if(ret==-1){
            bool ok;
            QString text = QInputDialog::getText(this, tr("消费超额"),tr("请输入密码"), QLineEdit::Password,0, &ok);
            if (ok){
                if(text==manager.GetStudent(cardid)->password){
                    ret=manager.Consume(1,cardid,payment,window);
                }else{
                    QMessageBox::warning(this,"警告","密码错误");return;
                }
            }
        }
        if(!ret){QString s="消费";logger.NotFoumd(1,s,cardid);}
        else logger.Consume(ret,manager.GetStudent(cardid),payment,window);
        switch(ret){
            case 0:
                QMessageBox::warning(this,"警告","系统中无此卡");return;
            case 1:
                QMessageBox::warning(this,"警告","此卡已挂失");return;
            case 2:
                QMessageBox::warning(this,"警告","余额不足");return;
            case 3:
                QMessageBox::information(this,"提示","消费成功\n当天消费金额:"+QString::number(canteen.win[window].money)+"\n当天消费次数:"+QString::number(canteen.win[window].times));
                break;
            default:
                break;
        }
    });
//    修改卡密码
    connect(ui->changepw,&QPushButton::clicked,[=](){
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        bool ok;
        QString text = QInputDialog::getText(this, tr("修改密码"),tr("请输入原密码"), QLineEdit::Password,0, &ok);
        if (ok){
            if(text==k->password){
                bool yes;
                QString text2 = QInputDialog::getText(this, tr("修改密码"),tr("请输入新密码"), QLineEdit::Password,0, &yes);
                if(yes){
                    if(text2.length()==4)k->password=text2;
                    else QMessageBox::warning(this,"警告","密码格式错误");
                }

            }else{
                QMessageBox::warning(this,"警告","密码错误");return;
            }
        }
    });
//    汇总窗口数据
    connect(ui->hz,&QPushButton::clicked,[=](){
        canteen.AllMessage.clear();wrong.clear();
//        canteen.CanteenData[9][0]->window=78;
//        canteen.CanteenData[11][10]->ID.append("---");
        canteen.Merge();
        if(wrong.empty()) QMessageBox::information(this,"提示","汇总完毕");
        else{
            QFile wrongdata("wrongdata.txt");
            QTextStream OUT(&wrongdata);
            wrongdata.open(QIODevice::WriteOnly);
            OUT.setCodec("UTF-8");
            for(auto k:wrong){
                OUT<<k->t1<<k->t2<<" "<<k->ID<<" "<<k->cardid<<" "<<QString::number(k->money)<<" "<<QString::number(k->window)<<endl;
            }
            wrongdata.close();
            QMessageBox::warning(this,"警告","部分数据可能被修改\n已将其写入wrongdata.txt中");
        }
    });
//    数据挖掘
    connect(ui->wj,&QPushButton::clicked,[=](){
        wrong.clear();
        QModelIndexList index=ui->tableWidget_2->selectionModel()->selectedIndexes();
        if(index.empty()){
            QMessageBox::warning(this,"警告","请选择一个学生");return;
        }
        QString id=ui->tableWidget_2->item(index[0].row(),1)->text();
        Student* k=manager.FindSelectStudent(id);
        manager.FindFriends(k);
        Reflush2();
        if(wrong.empty()) QMessageBox::information(this,"提示","挖掘完毕");
        else{
            QFile wrongdata("wrongdata.txt");
            QTextStream OUT(&wrongdata);
            wrongdata.open(QIODevice::WriteOnly);
            OUT.setCodec("UTF-8");
            for(auto k:wrong){
                OUT<<k->t1<<k->t2<<" "<<k->ID<<" "<<k->cardid<<" "<<QString::number(k->money)<<" "<<QString::number(k->window)<<endl;
            }
            wrongdata.close();
            QMessageBox::warning(this,"警告","部分数据可能被修改\n已将其写入wrongdata.txt中");
        }
    });
}
MainWindow::~MainWindow(){
    delete ui;
}

//    刷新总览界面表格
void MainWindow::Reflush1(){
    int i=0;
    for(auto k:manager.Q)if(!k->banned)i++;
    ui->tableWidget_1->setRowCount(i);
    i=0;
    for(auto k:manager.Q){
        if(k->banned)continue;
        ui->tableWidget_1->setItem(i,0,new QTableWidgetItem(k->name));
        ui->tableWidget_1->setItem(i,1,new QTableWidgetItem(k->ID));
        if(k->card.empty()){
            ui->tableWidget_1->setItem(i,2,new QTableWidgetItem("无"));
            ui->tableWidget_1->setItem(i,3,new QTableWidgetItem("0.00"));
        }else{
            ui->tableWidget_1->setItem(i,2,new QTableWidgetItem(k->card.back()->number));
            ui->tableWidget_1->setItem(i,3,new QTableWidgetItem(QString::number(k->card.back()->money,10,2)));
        }
        i++;
    }
}
//    刷新查询界面表格
void MainWindow::Reflush2(){
    int i=0;
    for(auto k:manager.find_result)if(!k->banned)i++;
    ui->tableWidget_2->setRowCount(i);
    i=0;
    for(auto k:manager.find_result){
        if(k->banned)continue;
        ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(k->name));
        ui->tableWidget_2->setItem(i,1,new QTableWidgetItem(k->ID));
        if(k->card.empty()||k->card.back()->banned){
            ui->tableWidget_2->setItem(i,2,new QTableWidgetItem("无"));
            ui->tableWidget_2->setItem(i,3,new QTableWidgetItem("0.00"));
        }else{
            ui->tableWidget_2->setItem(i,2,new QTableWidgetItem(k->card.back()->number));
            ui->tableWidget_2->setItem(i,3,new QTableWidgetItem(QString::number(k->card.back()->money,10,2)));
        }
        i++;
    }
}
//刷新当前时间
void MainWindow::ReflushTime(){
    ui->time1->setText(QString::number(year)+"年"+QString::number(month)+"月"+QString::number(day)+"日"+QString::number(hour)+"点"+QString::number(minute)+"分"+QString::number(sec)+"秒"+QString::number(msec)+"毫秒");
    ui->time2->setText(QString::number(year)+"年"+QString::number(month)+"月"+QString::number(day)+"日"+QString::number(hour)+"点"+QString::number(minute)+"分"+QString::number(sec)+"秒"+QString::number(msec)+"毫秒");
}

void MainWindow::on_pushButton_clicked(){
    Reflush1();ReflushTime();
}

void MainWindow::on_pushButton_2_clicked(){
    Reflush2();ReflushTime();
}
