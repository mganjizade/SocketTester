#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H
#include <google/protobuf/any.pb.h>
#include <google/protobuf/message.h>
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <Connection.pb.h>
#include <QTimer>
#define GOOGLE_PROTOBUF_NO_LITE_RUNTIME
class SocketManager : public QObject
{
    Q_OBJECT
public:
    explicit SocketManager(QObject *parent = nullptr);

private:
    QTcpSocket *socket;
    int cntpsd = 0;
    int cntDoa = 0;
    PSDDataRdyDOUBLE psd;
    DOADataRdydouble doa;
    QByteArray data;
signals:
public slots:
    void onConnected() {
        qDebug() << "Connected to the server";

        QTimer *timer = new QTimer(this);

        connect(socket, &QTcpSocket::readyRead, this, &SocketManager::readData);
        connect(timer,&QTimer::timeout,this, [this](){qDebug() << "counterPSD = " <<  cntpsd; cntpsd = 0; qDebug() << "counterDOA = " <<  cntDoa; cntDoa = 0;});
        timer->start(1000);
        // Send a Person message with an array of scores
        sendPersonMessage();
    }

    void onDisconnected() {
        qDebug() << "Disconnected from the server";
    }

    void readData() {
        while (socket->bytesAvailable() > 0) {
            data= socket->readAll();}

        //        }

        // google::protobuf::Any anymsg;

        // Detect the type of message and handle accordingly
        //            if (DefaultData(data)) {
        //                qDebug() << "Received Message Default";}

        DoaData(data);
        PSDdata(data);
        //qDebug() << "size" << data.size();



        // DefaultData(data);

        // }

    }

    void sendPersonMessage() {


        // Create a Person message with an array of scores
        Threshold th;
        th.set_thr(-120);
        google::protobuf::Any anyMessage;
        QByteArray serializedMessage;
        serializedMessage.resize(th.ByteSize());
        th.SerializeToArray(serializedMessage.data(),th.ByteSize());

        Frequency fr;

        fr.set_freq(1000);
        google::protobuf::Any anyMessage1;
        anyMessage1.PackFrom(fr);
        QByteArray serializedMessage1;
        serializedMessage1.resize(fr.ByteSizeLong());
        fr.SerializeToArray(serializedMessage1.data(), serializedMessage1.size());
        //             QDataStream out(socket);
        //             out.setVersion(QDataStream::Qt_5_15);
        //             out << qint64(serializedMessage.size());
        //             out.writeRawData(serializedMessage.constData(), serializedMessage.size());
        //            socket->waitForBytesWritten();
        //   QByteArray serializedData(freq.ByteSize(), 0);
        //  freq.SerializeToArray(serializedData.data(), serializedData.size());

        // Send the serialized data to the server
        socket->write(serializedMessage);
        socket->waitForBytesWritten();
        socket->write(serializedMessage1);
        socket->waitForBytesWritten();
    }

    bool DefaultData(const QByteArray &data) {
        Defaults Def;
        if (Def.ParseFromArray(data.data(), data.size())) {
            //qDebug() << "Received Person:";

            //qDebug() << "max: " << Def.maximumfreqrange();
            return true;
        }
        return false;
    }

    void PSDdata(const QByteArray &data) {

        psd.ParseFromArray(data.data(), data.size());
        if (psd.type()== "psd") {
            cntpsd++;
            //return true;
        }
        //return false;

    }

    void DoaData(const QByteArray &data) {

        doa.ParseFromArray(data.data(), data.size()) ;
        if(doa.type() == "doa")
        {
            cntDoa++;
        }
        // return true;

        //return false;
    }
};

#endif // SOCKETMANAGER_H
