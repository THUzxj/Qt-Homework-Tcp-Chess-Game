#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QWidget>

namespace Ui {
class ServerDialog;
}

class ServerDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ServerDialog(QWidget *parent = nullptr);
    ~ServerDialog();

private:
    Ui::ServerDialog *ui;

    friend class Server;
};

#endif // SERVERDIALOG_H
