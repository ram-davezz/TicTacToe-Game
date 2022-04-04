#ifndef HELLOWORLDSERVER_H
#define HELLOWORLDSERVER_H
#include <QTcpServer>

#include "tictactoemainwindow.h"

class TicTacToeMainWindow;

class HelloWorldServer : public QTcpServer
{
public:

    Q_OBJECT

        public:
            HelloWorldServer(TicTacToeMainWindow* pHelloServer,QObject *parent=0);
        TicTacToeMainWindow* m_pMainWindow;
        void addMessage(QString Msg);
        HelloWorldServer* m_pBoxServer;

        private slots:
            void readyRead();
            void disconnected();

        protected:
            void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

        private:
            QSet<QTcpSocket*> clients;


};
#endif // HELLOWORLDSERVER_H
