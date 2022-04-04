#ifndef TICTACTOEMAINWINDOW_H
#define TICTACTOEMAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "tictactoeserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TicTacToeMainWindow; }
QT_END_NAMESPACE

const int DEFAULT_NO_ROWS = 4;
const int DEFAULT_NO_COLS=  4;

class Cell
{
public:
    QRect           m_CellRect;
    QString         m_strText;
    Cell()
    {
        //Just initialize some character to display
        m_strText="i";
    }
};

class HelloWorldServer;

class TicTacToeMainWindow : public QMainWindow
{
    Q_OBJECT
    QTcpSocket *m_pClientSocket;
    HelloWorldServer* m_pBoxServer;
public:
    int m_width;
    int m_Height;
    int m_NoOfRows;
    int m_NoOfCols;
    int m_XOffset; //Offset from which drawing start
    int m_YOffset;
    Cell            m_CellArray[DEFAULT_NO_ROWS][DEFAULT_NO_COLS];
    QString         m_Message;

    TicTacToeMainWindow(QWidget *parent = nullptr);
    ~TicTacToeMainWindow();
    void DisplayStatusMessage(QString Message);
    void DisplayRemotePCMessage(QString Message);
    void SendMovesToRemotePC(QString Message);
    void displayError(QAbstractSocket::SocketError socketError);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent* ev);
    void Initialize();
    QString CreateMessage();

private slots:

    void on_pushButtonStart_clicked();

    void on_pushButtonConnect_clicked();

private:
    Ui::TicTacToeMainWindow *ui;
};
#endif // TICTACTOEMAINWINDOW_H
