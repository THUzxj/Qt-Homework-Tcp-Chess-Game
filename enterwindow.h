#ifndef ENTERWINDOW_H
#define ENTERWINDOW_H

#include <QWidget>

class Server;
class Client;

namespace Ui {
class EnterWindow;
}

class EnterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EnterWindow(QWidget *parent = nullptr);
    ~EnterWindow();
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

private:
    Ui::EnterWindow *ui;

signals:
    void getClient(Client* c);
    void getServer(Server* s);
};

#endif // ENTERWINDOW_H
