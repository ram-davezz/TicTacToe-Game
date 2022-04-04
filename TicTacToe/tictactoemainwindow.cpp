#include "tictactoemainwindow.h"
#include "ui_tictactoemainwindow.h"

#include <QtGui>
#include <QPalette>
#include<QMessageBox>

const int DEFAULT_X_OFFSET= 100;
const int DEFAULT_Y_OFFSET= 100;
const int DEFAULT_WIDTH  =  100;
const int DEFAULT_HEIGHT =  100;

TicTacToeMainWindow::TicTacToeMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TicTacToeMainWindow)
{
    ui->setupUi(this);
    Initialize();
    m_pClientSocket=new QTcpSocket(this);

}

TicTacToeMainWindow::~TicTacToeMainWindow()
{
    delete ui;
}


void TicTacToeMainWindow::paintEvent(QPaintEvent *pEvent)
{
    QWidget::paintEvent(pEvent);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::red);

    painter.setPen(Qt::blue);

    for (int iRowIndex = 0; iRowIndex<m_NoOfRows; iRowIndex++)
    {
        for (int iColIndex = 0; iColIndex<m_NoOfCols; iColIndex++)
        {
            painter.drawRect(m_CellArray[iRowIndex][iColIndex].m_CellRect);

            if(m_CellArray[iRowIndex][iColIndex].m_strText!="i")
            {
                painter.drawText(m_CellArray[iRowIndex][iColIndex].m_CellRect.left()-m_Height/2,
                                m_CellArray[iRowIndex][iColIndex].m_CellRect.top()-m_width/2,
                                m_CellArray[iRowIndex][iColIndex].m_strText);
            }

        }
    }

}

void TicTacToeMainWindow ::Initialize()
{
     m_NoOfRows=DEFAULT_NO_ROWS;
     m_NoOfCols=DEFAULT_NO_COLS;
     m_width=DEFAULT_WIDTH;
     m_Height=DEFAULT_HEIGHT;
     m_XOffset=DEFAULT_X_OFFSET;
     m_YOffset=DEFAULT_Y_OFFSET;
     //Initialize the Rectange

    for (int iRowIndex = 0; iRowIndex<m_NoOfRows; iRowIndex++)
    {
        for (int iColIndex = 0; iColIndex<m_NoOfCols; iColIndex++)
        {
             m_CellArray[iRowIndex][iColIndex].m_CellRect.setSize(QSize(m_width, m_Height));
             m_CellArray[iRowIndex][iColIndex].m_CellRect.setTopLeft(QPoint(m_XOffset + (m_width*(iColIndex+1)), m_YOffset + (m_Height*(iRowIndex+1))));

        }
    }
}

QString TicTacToeMainWindow ::CreateMessage()
{
    m_Message="";
    for(int i=0;i<DEFAULT_NO_ROWS;i++)
    {
        for(int j=0;j<DEFAULT_NO_COLS;j++)
        {
            m_Message+=m_CellArray[i][j].m_strText;
        }
    }
    return m_Message;
}

void TicTacToeMainWindow::on_pushButtonStart_clicked()
{
    if(ui->pushButtonStart->text()=="Start")
    {
         m_pBoxServer= new HelloWorldServer(this);
        bool success = m_pBoxServer->listen(QHostAddress::Any, quint16(ui->plainTextEditPort->toPlainText().toInt()));
        if(!success)
        {
            DisplayStatusMessage("Server failed...");
        }
        else
        {
            DisplayStatusMessage("Server Started...");
            this->setWindowTitle(this->windowTitle()+"|"+ui->plainTextEditPort->toPlainText());
            ui->pushButtonStart->setText("Stop");
        }

    }
    else
    {
        m_pBoxServer->close();
        DisplayStatusMessage("Server Stopped...");
        ui->pushButtonStart->setText("Start");
    }

}

void TicTacToeMainWindow::on_pushButtonConnect_clicked()
{
    try
    {
        if(ui->pushButtonConnect->text()=="Connect")
        {
            m_pClientSocket->connectToHost(ui->plainTextEditRemoteIPAddr->toPlainText(),quint16(ui->plainTextEditRemotePort->toPlainText().toInt()) );
                connect(m_pClientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
            ui->pushButtonConnect->setText("Disconnect");
            DisplayStatusMessage("Connected to Host =>" + ui->plainTextEditRemoteIPAddr->toPlainText() + "Port =>" + ui->plainTextEditRemotePort->toPlainText());
        }
        else
        {
            m_pClientSocket->disconnectFromHost();
            m_pClientSocket->close();
            ui->pushButtonConnect->setText("Connect");
            DisplayStatusMessage("Disconnected from Host =>" + ui->plainTextEditRemoteIPAddr->toPlainText());
        }
    }
    catch(QException* ex)
    {
        DisplayStatusMessage(QString("Error in Connection=>") + QString(ex->what()));
    }
}

void TicTacToeMainWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);


    if (event->button() == Qt::LeftButton)
    {
        for(int i=0;i<DEFAULT_NO_ROWS;i++)
        {
            for(int j=0;j<DEFAULT_NO_COLS;j++)
            {
                QRect widgetRect = m_CellArray[i][j].m_CellRect;
                widgetRect.moveTopLeft(this->parentWidget()->mapToGlobal(widgetRect.topLeft()));
                QPoint CurretPoint=event->pos();
                if(widgetRect.contains(CurretPoint))
                {
                    m_CellArray[i][j].m_strText="X";
                    QString Message=CreateMessage();
                    DisplayStatusMessage(Message);
                    this->repaint();
                    //Now send moves to the Remote Connect PC
                    SendMovesToRemotePC(Message);
                    return;

                }
            }
        }
    }

}

void TicTacToeMainWindow::DisplayStatusMessage(QString Message)
{
    ui->labelResult->setText(Message);
}
void TicTacToeMainWindow::DisplayRemotePCMessage(QString Message)
{
    //Check the message length first ..
    //This is a simple protocol message ie Just storing only cell moves
    //User might need to expand the protocol struct
    //to accomodate other settings paramaters eg Message="GridSize"+"#"+"X or Y"+"#"+Message;
    //based on the requirements
    if(Message.length()>=DEFAULT_NO_ROWS*DEFAULT_NO_COLS)
    {
        DisplayStatusMessage(Message);
    }
    else
    {
        DisplayStatusMessage("Message Length Small");
        return;
    }

    int index=0;
    for(int i=0;i<DEFAULT_NO_ROWS;i++)
    {
        for(int j=0;j<DEFAULT_NO_COLS;j++)
        {
            m_CellArray[i][j].m_strText=Message[index];
            index=index+1;
        }
    }
   this->repaint();
}

void TicTacToeMainWindow::SendMovesToRemotePC(QString Message)
{
    //This is a simple protocol message ie Just storing only cell moves
    //User might need to expand the protocol struct
    //to accomodate other settings paramaters eg Message="GridSize"+"#"+"X or Y"+"#"+Message;
    //based on the requirements
    m_pClientSocket->write(QString(Message + "\n").toUtf8());
}
void TicTacToeMainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
            case QAbstractSocket::RemoteHostClosedError:
                break;
            case QAbstractSocket::HostNotFoundError:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The host was not found. Please check the "
                                            "host name and port settings."));
                break;
            case QAbstractSocket::ConnectionRefusedError:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The connection was refused by the peer. "
                                            "Make sure the fortune server is running, "
                                            "and check that the host name and port "
                                            "settings are correct."));
                break;
            default:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The following error occurred: %1.")
                                         .arg(m_pClientSocket->errorString()));
            }

}
