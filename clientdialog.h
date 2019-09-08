#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QWidget>

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent = nullptr);
    ~ClientDialog();

private:
    Ui::ClientDialog *ui;

    friend class Client;
};

#endif // CLIENTDIALOG_H
