#include "network.h"
#include "clientdialog.h"
#include "ui_clientdialog.h"
#include "serverdialog.h"
#include "ui_serverdialog.h"
#include <QMessageBox>
#include <QtNetwork>
#include <QRegExp>

bool Client::isIP(QString str){
    QRegExp re("^([1-9]|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5]).(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5]).(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5]).(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])$");
    return re.exactMatch(str);
}

Client::Client(){
    dialog = new ClientDialog;
    socket = new QTcpSocket(this);
    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_0);

    connect(dialog->ui->pushButton,&QPushButton::clicked,this,&Client::connectStart);//OK
    connect(dialog->ui->pushButton_2,&QPushButton::clicked,this,&Client::connectCancel);//Cancel

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Client::displayError);

}

Client::~Client(){
    if(socket){
       socket->close();
       delete socket;
       socket=nullptr;
    }
}

void Client::showDialog(){

    dialog->show();
}

void Client::connectStart(){
    if(!isIP(dialog->ui->lineEdit->text())){
        QMessageBox::critical(nullptr,"NetGame Client","Please check that the IP is correct.");
        return ;
    }

    dialog->ui->pushButton->setEnabled(false);
    socket->abort();
    socket->connectToHost(dialog->ui->lineEdit->text(),20193);
    connect(socket,&QTcpSocket::readyRead,this,&Client::connectSuccess);
}

void Client::connectCancel(){
    socket->abort();
    dialog->ui->pushButton->setEnabled(true);
}

void Client::connectSuccess(){
    //in.startTransaction();
    QString str;
    //bool clientBlack;
    in>>str;//>>clientBlack
    qDebug()<<str;//<<" "<<clientBlack;
    if(str=="server OK"){
        dialog->close();
        delete dialog;


        disconnect(socket,&QIODevice::readyRead,this,&Client::connectSuccess);
        connect(socket,&QIODevice::readyRead,this,&Client::receiveMessage);
        emit readyNewNetGame();//clientBlack
    }
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(nullptr, tr("NetGame Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(nullptr, tr("NetGame Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the NetGame server is running, "
                                    "and check that the IP "
                                    "is correct."));
        break;
    default:
        QMessageBox::information(nullptr, tr("NetGame Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }
    dialog->ui->pushButton->setEnabled(true);
}

void Client::receiveMessage(){

    QString command;
    QList<int> args;
    in >> command;
    while(!in.atEnd()){
        int i;
        in>>i;
        args.push_back(i);
    }

    emit getMessage(command,args);
}

void Client::sendMessage(QString str){
    QByteArray array;
    array.clear();
    QDataStream out(&array,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out<<str;
    socket->write(array);
}

void Client::sendMessage(QString str,QList<int> args){
    QByteArray array;
    array.clear();
    QDataStream out(&array,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out<<str;
    for(int i:args){
        out<<i;
    }
    socket->write(array);
}

Server::Server(){
    dialog = new ServerDialog;
    initServer();
    connect(dialog->ui->pushButton,&QPushButton::clicked,this,&Server::startServer);
    connect(dialog->ui->pushButton_2,&QPushButton::clicked,this,&Server::cancelServer);
}

Server::~Server(){
    if(server){
        server->close();
        delete server;
    }
    if(socket){
        socket->close();
        delete socket;
    }
}

void Server::showDialog(){
    dialog->show();
}

void Server::initServer(){
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    dialog->ui->lineEdit->setText(ipAddress);
}

void Server::startServer(){
    if(server){
        delete server;
        server = nullptr;
    }
    server = new QTcpServer;
    if (!server->listen(QHostAddress::Any,20193)) {
        QMessageBox::critical(nullptr, tr("Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server->errorString()));
        return;
    }
    qDebug()<<server->serverPort();
    dialog->ui->pushButton->setEnabled(false);
    connect(server,&QTcpServer::newConnection,this,&Server::acceptConnection);
}

void Server::cancelServer(){
    if(server)
        server->close();
    dialog->ui->pushButton->setEnabled(true);
}

void Server::acceptConnection(){
    socket = server->nextPendingConnection();
    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_0);
    if(socket)
        connect(socket,&QTcpSocket::readyRead,this,&Server::receiveMessage);
    //bool clientBlack = QRandomGenerator::global()->bounded(0,1);
    //QList<int> l;
    //l.push_back(clientBlack);
    sendMessage("server OK");//,l
    dialog->close();
    delete dialog;
    emit readyNewNetGame();//clientBlack
}

void Server::receiveMessage(){
    QString command;
    QList<int> args;
    in >> command;
    while(!in.atEnd()){
        int i;
        in>>i;
        args.push_back(i);
    }

    /*if (!in.commitTransaction())
        return;*/
    emit getMessage(command,args);
}

void Server::sendMessage(QString str){
    QByteArray array;
    array.clear();
    QDataStream out(&array,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out<<str;
    socket->write(array);
}

void Server::sendMessage(QString str,QList<int> args){
    QByteArray array;
    array.clear();
    QDataStream out(&array,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out<<str;
    for(int i:args){
        out<<i;
    }
    socket->write(array);
}
