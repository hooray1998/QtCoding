#include "secwidget.h"
#include "ui_secwidget.h"

SecWidget::SecWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SecWidget)
{
    ui->setupUi(this);

    timer = new QTimer;


    ui->spinBox->setMaximum(9999999);
    ui->spinBox->setMinimum(1);
    connect(ui->pushButton_1,SIGNAL(clicked(bool)),this,SLOT(clickFrom()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(clickTo()));
    connect(ui->pushButtonStart,SIGNAL(clicked(bool)),this,SLOT(clickStart()));


}

SecWidget::~SecWidget()
{
    delete ui;
}

void SecWidget::noteSPath(QString p)
{
    lastSeletePath = p;
}
void SecWidget::setFrom(QString str)
{
    ui->lineEdit->setText(str);
}

void SecWidget::clickFrom()
{
    QString folderName = QFileDialog::getExistingDirectory(this,tr("选择起始路径"),lastSeletePath);
    if(!folderName.isNull())
    {
            lastSeletePath = folderName;
        if(folderName!=ui->lineEdit->text())
        {
            ui->lineEdit->setText(folderName);
        }
    }

}
void SecWidget::clickTo()
{
    QString folderName = QFileDialog::getExistingDirectory(this,tr("选择保存位置"),lastSeletePath);
    if(!folderName.isNull())
    {
            lastSeletePath = folderName;
        if(folderName!=ui->lineEdit_2->text())
        {
            ui->lineEdit_2->setText(folderName);
        }
    }
}


void SecWidget::clickStart()
{
    hide();
    i=0;
    emit start();
}
void SecWidget::initText(QString title, QString numName)
{
    setWindowTitle(title);

    ui->labelNumber->setText(numName);


}
QString SecWidget::getTo()
{
    return ui->lineEdit_2->text();
}
QString SecWidget::getFrom()
{
    return ui->lineEdit->text();
}
int SecWidget::getSence()
{
    return ui->scese->value();
}
int SecWidget::getStartNum()
{
    return ui->spinBox->value();
}
