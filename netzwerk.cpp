#include "netzwerk.h"
#include <QDebug>
#include <thread>
#include <cassert>
#include <QTcpServer>
#include <QString>

Netzwerk::Netzwerk()
{
    _ServerName=
            //"tueisec-pra17.sec.ei.tum.de";
            QHostAddress::LocalHost;
            //"129.187.229.95";

    _PortNumber=qint16(1234);
    isServer=0;
    qDebug()<<"creating network object";
    //connectToServer();
    //beginGame(2);
}




void Netzwerk::startServer()
{
    server= new QTcpServer(this);
    connect(server,SIGNAL(newConnection()),this,SLOT(newConnection()));
    if(!server->listen(QHostAddress::Any,1234))
        qDebug()<<"Server could not start";
    else
        qDebug()<<"Server started on port 1234";
}

void Netzwerk::stopServer()
{
    if(server!=NULL)
        server->deleteLater();
    qDebug()<<"Server stopped";
}



void Netzwerk::newConnection()//for server
{
    //get the socket that is connected
    socket = server->nextPendingConnection();
    emit clientConnectionReceived(QString("Client connection received. You are now server."));
    //socket->write("Hello client!\n");
    qDebug()<<"new connection detected from"<< socket->peerAddress()<<socket->peerPort();
    socket->flush();
//    connect(socket, SIGNAL(connected()), this, SLOT(enableConnection()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
    connect(socket, SIGNAL(destroyed()), this, SLOT(disableConnection()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readIncomingMessage()));

    isServer=1;
    emit weAreServer();
}




void Netzwerk::connectToServer(QHostAddress host,qint16 port)//for client
{
    _ServerName = host;
    _PortNumber = port;
    //create a tcpsocket and connect it to the configured host and port
    //enable connection when the socket is connected
    socket = new QTcpSocket(this);
    socket->connectToHost(_ServerName,_PortNumber);
    qDebug()<<"Connecting to "<<_ServerName<<" on port "<<_PortNumber;

//    connect(socket, SIGNAL(connected()), this, SLOT(enableConnection()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
    connect(socket, SIGNAL(destroyed()), this, SLOT(disableConnection()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readIncomingMessage()));
    isServer=0;
    emit weAreClient();
}












void Netzwerk::setServerName(QString serverName)
{
    _ServerName = QHostAddress(serverName);
}

void Netzwerk::setPort(qint16 portNumber)
{
    if(portNumber<1024) qDebug()<<"Invalid port";
    else _PortNumber=portNumber;
}









void Netzwerk::disconnectFromServer()
{
    qDebug()<<"Disconnecting from server";
    //dynamic_cast<QTcpSocket*>(_io_Stream.device())->disconnectFromHost();
    socket->disconnectFromHost();
}










void Netzwerk::enableConnection()
{
    //_isConnected=true;
//    //when the signal comes from a valid source in connectToServer/newconnection
//    //assert(QObject::sender()!=nullptr);
//    qDebug()<<"connected";
//    //the qtcpsocket s is now the device for the datastream
//    _io_Stream.setDevice(dynamic_cast<QTcpSocket*>(QObject::sender()));
//    //the new message is then processed
//    connect(QObject::sender(),SIGNAL(readyRead()),this,SLOT(readIncomingMessage()));
}







void Netzwerk::disableConnection()
{
    //disconnect the socket from the stream
    _io_Stream.setDevice(nullptr);
}








void Netzwerk::socketError()
{
    qDebug()<<"Connection Error!";
    if(_io_Stream.device()!=NULL)
        _io_Stream.device()->deleteLater();
}







void Netzwerk::disconnected()
{
    //disconnect the assigned socket from the stream to delete later
    qDebug()<<"Disconnected";
    if(_io_Stream.device()!=NULL)
        _io_Stream.device()->deleteLater();
}








void Netzwerk::readIncomingMessage()
{
    QByteArray ba = socket->peek(2);
    if(socket->bytesAvailable()<ba[1]+2){
        qDebug()<<"New Message Incoming"<<socket->bytesAvailable();
        return;//ensure incoming message has right length
    }
    QString displaymes;
    switch(ba[0]){
    case 0x01: displaymes = "Start Game"; break;
    case 0x02: displaymes = "Confirm Start Game"; break;
    case 0x10: displaymes = "Set Stone"; break;
    case 0x11: displaymes = "Shift Stone"; break;
    case 0x12: displaymes = "Jump Stone"; break;
    case 0x13: displaymes = "Confirm Move"; break;
    case 0x20: displaymes = "Delete Stone"; break;
    case 0x21: displaymes = "Confirm Deletion"; break;
    default: displaymes = "Undefined";
    }
    ba = socket->readAll();
    socket->flush();
    qDebug()<<"Received message "<<displaymes<<ba<<"with size"<<ba.size();
    quint8 mes[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    for(int i = 0; i < ba.size(); i++) mes[i] = ba[i];
    qDebug()<<mes[0]<<mes[1]<<mes[2]<<mes[3]<<mes[4]<<mes[5]<<mes[6];
    emit newMessageReceived(mes[0],mes[1],mes[2],mes[3],mes[4],mes[5],mes[6]);
}













void Netzwerk::beginGame(quint8 beginner)//from server to client
{
    if(socket!=NULL)
    {
        QDataStream _io_Stream(&barray, QIODevice::ReadWrite);
        quint8 cmd = 0x01;
        quint8 lange = 0x02;
        quint8 gruppennummer = 0x02;
        _io_Stream<<cmd<<lange<<beginner<<gruppennummer;
        socket->write(barray);
        qDebug()<<"Sending message: begin";
        barray.clear();
    }
    else{
        qDebug()<<"No Socket Available";
    }
}

void Netzwerk::confirmBeginGame()//from client to server
{
    if(socket!=NULL)
    {
        QDataStream _io_Stream(&barray, QIODevice::ReadWrite);
        quint8 cmd = 0x02;
        quint8 lange = 0x01;
        quint8 gruppennummer = 0x02;
        _io_Stream<<cmd<<lange<<gruppennummer;
        socket->write(barray);
        qDebug()<<"Sending message: confirm begin";
        barray.clear();
    }
}

void Netzwerk::endGame()
{
    qDebug()<<"Game ended";
    socket->deleteLater();
    server->deleteLater();
    isServer=0;
    _ServerName=QHostAddress("127.0.0.1");
    _PortNumber=1234;
    barray.clear();
    if(_io_Stream.device()!=NULL)
        _io_Stream.device()->deleteLater();
    _isConnected=false;
}

void Netzwerk::setStone(quint8 himmelNeu, quint8 quadradNeu, quint8 steinZahl, quint8 geschlossen)
{
    if(socket!=NULL)
    {
        QDataStream _io_Stream(&barray, QIODevice::ReadWrite);
        quint8 cmd = 0x10;
        quint8 lange = 0x04;

        _io_Stream<<cmd<<lange<<himmelNeu<<quadradNeu<<steinZahl<<geschlossen;
        socket->write(barray);
        qDebug()<<"Sending message: set stone";
        barray.clear();
    }
}

void Netzwerk::shiftStone(quint8 himmelAlt, quint8 quadradAlt, quint8 himmelNeu, quint8 geschlossen)
{
    if(socket!=NULL){
        QDataStream _io_Stream(&barray, QIODevice::ReadWrite);
        quint8 cmd = 0x11;
        quint8 lange = 0x04;

        _io_Stream<<cmd<<lange<<himmelAlt<<quadradAlt<<himmelNeu<<geschlossen;
        socket->write(barray);
        qDebug()<<"Sending message: shift stone";
        barray.clear();
    }
}

void Netzwerk::jumpStone(quint8 himmelAlt, quint8 quadradAlt, quint8 himmelNeu, quint8 quadradNeu, quint8 geschlossen)
{
    if(socket!=NULL){
        QDataStream _io_Stream(&barray, QIODevice::ReadWrite);
        quint8 cmd = 0x12;
        quint8 lange = 0x05;

        _io_Stream<<cmd<<lange<<himmelAlt<<quadradAlt<<himmelNeu<<quadradNeu<<geschlossen;
        socket->write(barray);
        qDebug()<<"Sending message: jump stone";
        barray.clear();
    }
}

void Netzwerk::confirmMove(quint8 statusCode)
{
    if(socket!=NULL){
        QDataStream _io_Stream(&barray, QIODevice::ReadWrite);
        quint8 cmd = 0x13;
        quint8 lange = 0x01;

        _io_Stream<<cmd<<lange<<statusCode;
        socket->write(barray);
        qDebug()<<"Sending message: confirm move";
        barray.clear();
    }
}

void Netzwerk::deleteStone(quint8 himmelDel, quint8 quadradDel)
{
    if(socket!=NULL){
        QDataStream _io_Stream(&barray, QIODevice::ReadWrite);
        quint8 cmd = 0x20;
        quint8 lange = 0x02;

        _io_Stream<<cmd<<lange<<himmelDel<<quadradDel;
        socket->write(barray);
        qDebug()<<"Sending message: delete stone";
        barray.clear();
    }
}

void Netzwerk::confirmDelete(quint8 statusCode, quint8 steinZahl)
{
    if(socket!=NULL){
        QDataStream _io_Stream(&barray, QIODevice::ReadWrite);
        quint8 cmd = 0x21;
        quint8 lange = 0x02;

        _io_Stream<<cmd<<lange<<statusCode<<steinZahl;
        socket->write(barray);
        qDebug()<<"Sending message: confirm deletion";
        barray.clear();
    }
}
