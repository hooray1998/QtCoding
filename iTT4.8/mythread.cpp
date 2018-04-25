#include "mythread.h"

MyThread::MyThread()
{


}

void MyThread::run()
{

    if(function==1)
    {
        gsh();
    }
    else if(function==2)
    {
        deleteOldPicture();
    }
    else if(function ==3)
    {
        moveUntreatedPicture();
    }
}
void MyThread::showProgress()
{

           //emit Progress(i/10000000);
           //qDebug()<<"emit"<<i/10000000;


}

void MyThread::init(Function f, QString fr, QString t, long snum, long endnum, int s)
{
    function = f;
    from = fr;
    to = t;
    startNumber  = snum;
    endNumber = endnum;
    sence = s;


}
void MyThread::setOglist(QFileInfoList og)
{
    oglist = og;
}
QString MyThread::newName(int s,int v)
{
    char name[9];

         sprintf(name,"%02d%07d",s,v);

    return QString::fromLocal8Bit(name);
}
void MyThread::gsh()
{

        QString curname = newName(startNumber,endNumber);

        QDir dir;

        dir.setPath(from);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setSorting(QDir::Name );

        QFileInfoList list = dir.entryInfoList();

        QString startName = list.at(0).fileName().split(".").at(0);

        startTime = clock();
        int size = list.size();
        for(;i<size;i++)
        {
             if(clock()-startTime>=100)
           {
                 emit Progress(i*100/(size));
                 startTime = clock();
           }


            QFileInfo finfo = list.at(i);
           if(!finfo.fileName().contains(startName))
            {
               startName = finfo.fileName().split(".").at(0);
               curname = newName(startNumber,++endNumber);
           }
           if(finfo.fileName().contains(startName))
           {

                QFile file(list.at(i).filePath());
                QString n = finfo.fileName();

               for(int i=0;i<9;i++)
               {
                   n[i] = curname[i];
               }

               if(!file.copy(to+"/"+n))
                  qDebug("copy失败");

           }

        }

    //emit Progress(99);

}
void MyThread::deleteOldPicture()
{
    startTime = clock();
    for(int j=0;j<startNumber;j++)
    {
        if(clock()-startTime>=100)
        {
            emit Progress((j)*40/(startNumber));
            startTime = clock();
        }

        QFileInfo finfo = oglist.at(j);
        QFile file(oglist.at(j).filePath());

            if(!file.remove())
               qDebug("remove失败");
    }
    QDir dir;

    dir.setPath(to);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    int size = list.size();

    startTime = clock();

    for(int i=0;i<size;i++)
    {
          if(clock()-startTime>=100)
        {
            emit Progress(40+((i)*60/(size)));
            startTime = clock();
        }

        QFileInfo finfo = list.at(i);

            QFile file(list.at(i).filePath());
           if(!file.remove())
               qDebug("remove失败");

    }
    emit Progress(99);

}
void MyThread::moveUntreatedPicture()
{
    startTime = clock();
    for(int i=startNumber;i<endNumber;i++)
   {

        if(clock()-startTime>=100)
        {
            emit Progress((i-startNumber)*100/(endNumber-startNumber));
            startTime = clock();
        }
            QFileInfo finfo = oglist.at(i);
        QString fn  = finfo.fileName().split(".").at(0);
            QFile file(oglist.at(i).filePath());
            if(!file.copy(to+"/"+fn+".jpg"))
                   qDebug("fail in copy");
           if(!file.remove())
               qDebug("remove失败");

   }
    emit Progress(99);

}
