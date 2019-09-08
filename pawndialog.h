#ifndef PAWNDIALOG_H
#define PAWNDIALOG_H

#include <QDialog>

namespace Ui {
class PawnDialog;
}

class PawnDialog : public QDialog
{
    Q_OBJECT
private:
    int _value;

public:
    explicit PawnDialog(QWidget *parent = nullptr);
    ~PawnDialog();
    int value();
    void accept();
private:
    Ui::PawnDialog *ui;
};

#endif // PAWNDIALOG_H
