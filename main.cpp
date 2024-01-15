#include <QCoreApplication>
#include <socketmanager.h>
#include <QThread>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SocketManager socket;

    while(1)
    {
        QCoreApplication::processEvents();
   //     QThread::msleep(1);
    }
    return a.exec();
}
