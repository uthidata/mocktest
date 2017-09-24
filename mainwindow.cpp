#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    n = new Netzwerk;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ButtonHostServer_clicked()
{
    if(n->isServer==0){
        n->startServer();
        ui->ButtonHostServer->setText("StopHosting");
        n->isServer=1;
    } else{
        n->stopServer();
        ui->ButtonHostServer->setText("Host Server");
        n->isServer=0;
    }

}

void MainWindow::on_ButtonConnect_clicked()
{
    if(!isConnected){
        n->connectToServer(QHostAddress(ui->LineEditHostname->text()),
                           qint16(ui->LineEditPortnumber->text().toLocal8Bit().toShort()));
        ui->ButtonConnect->setText("Disconnect");
        isConnected=1;
    } else{
        n->disconnectFromServer();
        ui->ButtonConnect->setText("Connect");
        isConnected=0;
    }
}

void MainWindow::on_ButtonBeginGame_clicked()
{
    n->beginGame(0);
}
