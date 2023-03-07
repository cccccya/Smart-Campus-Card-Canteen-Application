#include <QApplication>
#include <QDebug>
#include <vector>
#include "mainwindow.h"
#include "manager.h"
#include "canteen.h"

bool flag;
Manager manager;
Canteen canteen;
int serial_number=12346;
int year=0,month=0,day=0,hour=0,minute=0,sec=0,msec=0;
QFile filelog("log\\LOG.txt");
QTextStream out(&filelog);
Log logger;
std::vector<Message*>wrong;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
