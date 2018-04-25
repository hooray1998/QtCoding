#include "helpdoc.h"
#include "ui_helpdoc.h"

helpDoc::helpDoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::helpDoc)
{
    ui->setupUi(this);
    helpDoc::setWindowTitle("help");
}

helpDoc::~helpDoc()
{
    delete ui;
}
