#include "socketmanager.h"

SocketManager::SocketManager(QObject *parent)
    : QObject{parent}
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &SocketManager::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &SocketManager::onDisconnected);
    socket->connectToHost("127.0.0.1", 8085); // Connect to the server


}
