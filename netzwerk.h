#ifndef NETZWERK_H
#define NETZWERK_H
#include <QString>
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <future>
#include <QTcpServer>

class Netzwerk:public QObject
{
        Q_OBJECT
    public:
        Netzwerk();
        QString PlayMes;
        bool isServer;
    private:
        QHostAddress _ServerName;
        qint16 _PortNumber;

        QByteArray barray;
        QDataStream _io_Stream;
        QTcpSocket *socket;
        QTcpServer *server;
        bool _isConnected;
        //QTcpSocket *socketToClient;
        QTcpSocket *chatSocket;




    public slots:
        void setServerName(QString);
        void setPort(qint16);

        void disconnectFromServer();
        void enableConnection();
        void disableConnection();
        void disconnected();
        void socketError();
        void readIncomingMessage();
        void startServer();
        void stopServer();


        //for server
        void newConnection();
        //for client
        void connectToServer(QHostAddress server=QHostAddress::LocalHost,qint16 port=1234);



        void beginGame(quint8 beginner);
        void confirmBeginGame();
        void setStone(quint8 himmelNeu, quint8 quadradNeu, quint8 steinZahl, quint8 geschlossen);
        void shiftStone(quint8 himmelAlt, quint8 quadradAlt, quint8 himmelNeu, quint8 geschlossen);
        void jumpStone(quint8 himmelAlt, quint8 quadradAlt, quint8 himmelNeu, quint8 quadradNeu, quint8 geschlossen);
        void confirmMove(quint8 statusCode);
        void deleteStone(quint8 himmelDel, quint8 quadradDel);
        void confirmDelete(quint8 statusCode, quint8 steinZahl);

        void endGame();

    signals:
        void clientConnectionReceived(QString);
        void newMessageReceived(quint8 cmd, quint8 lange,
                                quint8 msg1, quint8 msg2=0,quint8 ms3 = 0,quint8 ms4 = 0,quint8 ms5 = 0);
        void weAreServer();
        void weAreClient();

};

#endif // NETZWERK_H
