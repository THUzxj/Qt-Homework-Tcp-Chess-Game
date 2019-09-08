#include "pawndialog.h"
#include "ui_pawndialog.h"

PawnDialog::PawnDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PawnDialog)
{
    ui->setupUi(this);
/*    connect(ui->pushButton,&QPushButton::clicked,[&](){_value = 1;});
    connect(ui->pushButton_2,&QPushButton::clicked,[&](){_value = 2;});
    connect(ui->pushButton_3,&QPushButton::clicked,[&](){_value = 3;});
    connect(ui->pushButton_4,&QPushButton::clicked,[&](){_value = 4;});
    connect(ui->pushButton,&QPushButton::clicked,this,&PawnDialog::close);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&PawnDialog::close);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&PawnDialog::close);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&PawnDialog::close);*/

    //设置扩展属性
    ui->pushButton->setProperty("value",1);
    ui->pushButton_2->setProperty("value",2);
    ui->pushButton_3->setProperty("value",3);
    ui->pushButton_4->setProperty("value",4);

    connect(ui->pushButton,&QPushButton::clicked,this,&PawnDialog::accept);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&PawnDialog::accept);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&PawnDialog::accept);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&PawnDialog::accept);

}

PawnDialog::~PawnDialog()
{
    delete ui;
}

int PawnDialog::value(){
    return _value;
}

void PawnDialog::accept(){
    // 获取信号发送者
            QPushButton *button = qobject_cast<QPushButton *>(sender());

            // 设置返回值
            if (button)
            {
                _value = button->property("value").toInt();
            }

            // 向下传递
            QDialog::accept();
}
