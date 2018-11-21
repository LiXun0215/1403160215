#include "mainwidget.h"
#include <QApplication>
#include <QDateTime>

int main(int argc, char *argv[])
{

    qRegisterMetaType< QList<QDateTime> >("QList<QDateTime>");

    qDebug()<<QDateTime::currentDateTime();//输出当前时间

    QApplication a(argc, argv);
    mainWidget w;
    w.show();

    return a.exec();
}
