#ifndef NETWORK_H
#define NETWORK_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>

class ClientDialog;
class ServerDialog;

class Client:public QObject
{
    Q_OBJECT
private:
    QTcpSocket* socket = nullptr;
    ClientDialog* dialog;
    QDataStream in;
    static bool isIP(QString str);
public:
    Client();
    ~Client();
    void showDialog();
public slots:
    void connectStart();
    void connectCancel();
    void connectSuccess();
    void displayError(QAbstractSocket::SocketError socketError);


    void receiveMessage();
    void sendMessage(QString str);
    void sendMessage(QString str,QList<int> args);

signals:
    void readyNewNetGame();
    void getMessage(QString, QList<int>);

};

class Server:public QObject
{
    Q_OBJECT
private:
    QTcpServer* server = nullptr;
    QTcpSocket* socket;
    ServerDialog* dialog;
    QDataStream in;
public:
    Server();
    ~Server();
    void showDialog();

    void initServer();
    void startServer();
    void cancelServer();

    void acceptConnection();
    void receiveMessage();
    void sendMessage(QString str);
    void sendMessage(QString str,QList<int> args);

public slots:

signals:
    void getMessage(QString, QList<int>);
    void readyNewNetGame();
};

#endif // NETWORK_H
