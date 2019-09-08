#include "promotiondialog.h"
#include "ui_promotiondialog.h"

PromotionDialog::PromotionDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PromotionDialog)
{
    ui->setupUi(this);
}

PromotionDialog::~PromotionDialog()
{
    delete ui;
}

int PromotionDialog::getPromotion(){


}
