#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QThread>
#include <QtDebug>
#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <time.h>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <string.h>


enum Function
    {
        gsh=1 ,
        deleteOld,
        moveUntreated
    };

class MyThread : public QThread
{
    Q_OBJECT
public:
    MyThread();
    void init(Function f, QString fr, QString t, long snum, long endnum, int s=1);
    void setOglist(QFileInfoList og);
    QString newName(int ,int);
    void gsh();
    void deleteOldPicture();
    void moveUntreatedPicture();
signals:
    Progress(int);
private slots:
   void showProgress();
protected:
    void run();
private:

    int progressValue;

    QString from;
    QString to;
    Function function;
    long startNumber;
    long endNumber;
    int sence;
    QFileInfoList oglist;

    long i;
    clock_t startTime;
};

#endif // MYTHREAD_H
