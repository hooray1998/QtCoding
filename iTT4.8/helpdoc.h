#ifndef HELPDOC_H
#define HELPDOC_H

#include <QWidget>

namespace Ui {
class helpDoc;
}

class helpDoc : public QWidget
{
    Q_OBJECT

public:
    explicit helpDoc(QWidget *parent = 0);
    ~helpDoc();

private:
    Ui::helpDoc *ui;
};

#endif // HELPDOC_H
