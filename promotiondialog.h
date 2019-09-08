#ifndef PROMOTIONDIALOG_H
#define PROMOTIONDIALOG_H

#include <QWidget>

namespace Ui {
class PromotionDialog;
}

class PromotionDialog : public QWidget
{
    Q_OBJECT

private:
    int _value;
public:
    explicit PromotionDialog(QWidget *parent = nullptr);
    ~PromotionDialog();
    int getPromotion();

private:
    Ui::PromotionDialog *ui;
};

#endif // PROMOTIONDIALOG_H
