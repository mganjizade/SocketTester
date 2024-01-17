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

#define HEADERSIZE 2
#define FOOTERSIZE 2
#define HEADER1 0xAA
#define HEADER2 0xFF
#define FOOTER1 0x55
#define FOOTER2 0x55
using namespace std;
class SocketManager : public QObject
{
    Q_OBJECT
public:
    explicit SocketManager(QObject *parent = nullptr);

private:
    QTcpSocket *socket;
    int cntpsd = 0;
    int cntDoa = 0;
    int cntall = 0;
    PSDDataRdyDOUBLE psd;
    DOADataRdydouble doa;
    QByteArray data;
    bool ResumeFlag =false;

signals:
public slots:
    void onConnected() {
        qDebug() << "Connected to the server";

        QTimer *timer = new QTimer(this);

        connect(socket, &QTcpSocket::readyRead, this, &SocketManager::readData);
        connect(timer,&QTimer::timeout,this, [this](){qDebug() << "counterPSD = " <<  cntpsd; cntpsd = 0;
            qDebug() << "counterDOA = " <<  cntDoa; cntDoa = 0; qDebug() << "cnt all = " <<  cntall; cntall = 0;});
        timer->start(1000);
        // Send a Person message with an array of scores
        sendPersonMessage();
    }

    void onDisconnected() {
        qDebug() << "Disconnected from the server";
    }

    void readData() {


        while (socket->bytesAvailable() > 0) {
            if(ResumeFlag)
                data.append(socket->readAll());
            else
                data= socket->readAll();
        }

        const char pattern[] = {static_cast<char>(HEADER1),static_cast<char>(HEADER2)};


        QByteArray::const_iterator it = data.constBegin();
        QByteArray::const_iterator end = data.constEnd();

        while (it < end) {
            // Search for the pattern starting from the current iterator position
            QByteArray::const_iterator result = std::search(it, end, pattern, pattern + sizeof(pattern) - 1);

            // Check if the pattern was found
            if (result != end) {
                // Calculate the index using iterators

                int IndexOFFind = std::distance(data.constBegin(), result) ;
                if(IndexOFFind + HEADERSIZE + FOOTERSIZE > data.size())
                {
                    ResumeFlag =true;
                    data = data.mid(IndexOFFind,data.size());
                    return;
                }
                int index = IndexOFFind + HEADERSIZE;
              //  qDebug() << "Pattern found at index:" << index << "size " << data.size();
                ushort len = (data.at(index++) & 0xff);
                ushort hlen = (data.at(index) << 8);
                len |= hlen;
                if(len > 3220)
                {
                    it = result + sizeof(pattern) - 1;
                    break;
                }
              //  qDebug() << "len" << len;
                len += ++index;

                if(len + index - HEADERSIZE > data.size())
                {
                    ResumeFlag =true;
                    data = data.mid(IndexOFFind,data.size());
                    return;
                }

                if(data.at(len) == FOOTER1 && data.at(len +1) == FOOTER2)
                {
                    ResumeFlag = false;
                    DoaData(data.mid(index,len - FOOTERSIZE - HEADERSIZE ));
                    PSDdata(data.mid(index,len - 4 ));
                    it = result + len + 2;
                }
                else
                    it = result + sizeof(pattern) - 1;




                // Move the iterator to the next position to continue searching

            } else {
                // Pattern not found, exit the loop
                break;
            }
        }


        cntall++;
        // google::protobuf::Any anymsg;

        // Detect the type of message and handle accordingly
        //            if (DefaultData(data)) {
        //                qDebug() << "Received Message Default";}


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
