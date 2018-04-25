#ifndef SECWIDGET_H
#define SECWIDGET_H

#include <QWidget>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QtDebug>
#include "gshwindow.h"
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringList>
#include <string.h>
#include <QTimer>

namespace Ui {
class SecWidget;
}

class SecWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SecWidget(QWidget *parent = 0);
    ~SecWidget();

    void setFrom(QString str);
    void setTo(QString str);
    QString getFrom();
    QString getTo();
    int getSence();
    int getStartNum();

    void noteSPath(QString p);

    void initText(QString title,QString numName);

signals:
    void start();
private slots:
    void clickFrom();
    void clickTo();

    void clickStart();

private:
    Ui::SecWidget *ui;

    QTimer*timer;

    QString lastSeletePath;

    QString fromPath;
    QString toPath;
    int number;
    long i;

};

#endif // SECWIDGET_H
