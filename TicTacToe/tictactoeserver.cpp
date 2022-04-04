#include "tictactoeserver.h"
#include <QTcpSocket>

HelloWorldServer::HelloWorldServer(TicTacToeMainWindow* pHelloServer,QObject *parent) : QTcpServer(parent)
{
    m_pMainWindow=pHelloServer;
}

void HelloWorldServer::incomingConnection(qintptr socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clients.insert(client);

    m_pMainWindow->DisplayStatusMessage("New client from: "+client->peerAddress().toString());

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void HelloWorldServer::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    QString line;
    while(client->canReadLine())
    {
        line = QString::fromUtf8(client->readLine()).trimmed();
        //qDebug() << "Read line:" << line;
        m_pMainWindow->DisplayRemotePCMessage(line);

    }

}

void HelloWorldServer::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();
    clients.remove(client);
}
