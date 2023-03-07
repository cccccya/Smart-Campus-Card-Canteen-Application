#ifndef WINDOW_H
#define WINDOW_H

extern int year,month,day,hour,minute,sec,msec;

class Window
{
public:
    Window();
//    窗口最后一次消费 年 月 日 次数 累计金额
    int y=0,m=0,d=0,times=0;
    double money=0;
//    窗口消费记录的首位位置
    int l,r;
};

#endif // WINDOW_H
