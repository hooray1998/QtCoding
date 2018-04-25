#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_help_txt.h"
/////////////////////////////////////////////////////////////
/// getCurrentPicture 函数需要优化，要对当前图片有效性提前判断。
/////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    lastSPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    mythread = new MyThread;
    checkThreadIsEndTimer =new QTimer;
    curThreadOperation = 0;
    connect(checkThreadIsEndTimer,SIGNAL(timeout()),this,SLOT(checkThreadIsEnd()));
    connect(mythread,SIGNAL(Progress(int)),this,SLOT(showProcess(int)));

    progressWidget = new QWidget;
    progress = new QProgressBar;
    progress->setFixedHeight(100);
    tishilabel = new QLabel;
    progressLayout = new QVBoxLayout;
    progressLayout->addWidget(progress);
    progressLayout->addWidget(tishilabel);
    progressWidget->setLayout(progressLayout);
    progressWidget->resize(400,200);

    num  = new QSpinBox;
    label1 = new QLabel("请输入转移的数量");
    label2 = new QLabel("请输入转移到的文件路径");
    lineedit = new QLineEdit;
    button = new QPushButton("选择");
    start = new QPushButton("开始");

    layout = new QGridLayout;
    lay2 = new QHBoxLayout;
    cancel= new QPushButton("取消");
    lineedit->setMinimumWidth(200);
    lineedit->setReadOnly(true);
    num->setMaximumWidth(70);
       layout->addWidget(label1,0,0);
    layout->addWidget(num,0,1);
    layout->addWidget(label2,1,0);
    layout->addWidget(lineedit,1,1);
    layout->addWidget(button,1,2);

    lay2->addWidget(start);
    lay2->addWidget(cancel);

    layout->addLayout(lay2,2,1);
    widget = new QWidget;
   widget->setLayout(layout);
   widget->setWindowTitle(QString("转移未处理图片"));
   widget->setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
   connect(button,SIGNAL(clicked(bool)),this,SLOT(chooseMovePath()));
   connect(start,SIGNAL(clicked(bool)),this,SLOT(startMove()));
   connect(cancel,SIGNAL(clicked(bool)),widget,SLOT(close()));
 /////////////////////////////////////////
    timer = new QTimer;

    //setWindowFlags(Qt::WindowCloseButtonHint&Qt::WindowMinimizeButtonHint);

    FixLayout();


   // this->setMouseTracking(true);
///////////////////////////////////////////////////////////////////
//设置QSS
    this->initStyle();

// 快捷键设置
    ui->pushButtonNextPicture->setShortcut(Qt::Key_Down);
    ui->pushButtonLastPicture->setShortcut(Qt::Key_Up);
    ui->pushButtonSavePlate->setShortcut(tr("space"));


    isPress = false;




    connect(&swidget,SIGNAL(start()),this,SLOT(gshSlot()));
    init();
    //


///////////////////////////////////////


}

MainWindow::~MainWindow()
{
    delete ui;

}
void MainWindow::FixLayout()
{

    int dh =QApplication::desktop()->height();
    int dw =QApplication::desktop()->width();


  this->setWindowFlags(Qt::FramelessWindowHint );
                     //   Qt::WindowSystemMenuHint |
                      //   Qt::WindowMinMaxButtonsHint);


    MainWindow::setWindowTitle("iTT");
    MainWindow::setWindowState(Qt::WindowMaximized);

    ui->pushButtonMined->setStyleSheet("border:none;");
    ui->pushButtonClose->setStyleSheet("border:none;");  //去掉边框
    ui->labelTitleIco->setStyleSheet("border:none;");
    ui->labelTitle->setStyleSheet("border:none;");

    ui->widget->setFixedHeight(24);
    ui->labelPictureCount->setFixedSize(dw*1/4,dh*1/12);
    ui->label->setGeometry(27,34,dw*13/20,dh*8/9);
    ui->label->setFixedSize(dw*14/20,dh*8/9);
    ui->statusBar->setFixedSize(dw,dh*1/50);

    ui->horizontalLayout_10->setContentsMargins(dw/180,0,dw/180,0);
    ui->verticalLayout_3->setContentsMargins(0,0,0,0);

    ui->groupBox->setFixedSize(dw*1/4,dh*6/19);
    //ui->groupBoxForButtons->setMaximumHeight(dh*1/7);
    ui->timeLcdNumber->setFixedHeight(dh*1/12);
    ui->tabWidget->setFixedSize(dw*1/4,dh*3/8);
    ui->tabWidget->setTabText(1,"设置");
    ui->tabWidget->setTabText(0,"普通");


    ui->tabWidget->setCurrentIndex(0);
    ui->labelTitleIco->setPixmap(QPixmap(":/new/prefix1/cpn.ico").scaled(ui->labelTitleIco->width()*2/3,ui->labelTitleIco->height()*2/3));

    l_x = 27;
    l_y = 34;
    l_width = ui->label->width();
    l_height = ui->label->height();

}
void MainWindow::init()
{
    pointNum = 0;

    currentColor = 1;

    readInfoFile();
    startCount = fileinfo.lastNum;

    if(fileinfo.finishNum<fileinfo.lastNum)
        fileinfo.finishNum = fileinfo.lastNum;
    isStore = false;
    Stored = false;
    FindpnNumber();
    RunTime = 0;

    shutTime = new QTimer;
    QObject::connect(shutTime,SIGNAL(timeout()),this,SLOT(updateTime()));//一秒更新一次时间
    shutTime->start(1000);

    ui->timeLcdNumber->setDigitCount(8);
    lastFinishNumber = fileinfo.finishNum;

    for(int i=0;i<60;i++)
        lastMinuteCount[i] = lastFinishNumber;

    ShowCalculateNum();
}

bool isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
    {
      return true;
    }
    else
    {
       bool ok = dir.mkdir(fullPath);//创建多级目录
       return ok;
    }
}
FileInfo::FileInfo()
{
    finishNum = 0;
    lastNum = 0;
}


CurrentPlateNumber::CurrentPlateNumber()
{

}
CurrentPicture::CurrentPicture()
{
    picture = NULL;
}
void MainWindow::updateFile()
{

    QString filePath = QCoreApplication::applicationDirPath();
    if(filePath.isEmpty())
    {
        QMessageBox::warning(this,"Error Message:","当前目录路径失效.");
        return ;
    }
    QFile *file = new QFile;
    file->setFileName(filePath+"/fileinfo.txt");
    bool ok =file->open(QIODevice::WriteOnly);
    if(fileinfo.finishNum<fileinfo.lastNum)
        fileinfo.finishNum = fileinfo.lastNum;

    if(ok)
    {
        QTextStream out(file);
        out<<fileinfo.finishNum<<" "<<fileinfo.lastNum<<"  ";
        out<<fileinfo.currentPath<<"  ";
        out<<fileinfo.savePath<<"  ";
        out<<fileinfo.specialPath<<" ";
        file->close();
        delete file;
    }
    else
    {
        QMessageBox::warning(this,"Warning Message:","Fail Open fileinfo File.");
        return ;
    }
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    if(e->delta()<0)
    {
        //下一张图片
        on_pushButtonNextPicture_clicked();
    }
    else
    {
        on_pushButtonLastPicture_clicked();
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    Point temp(x,y);
    newPoint(temp);

    if(ui->label->geometry().contains(event->pos())&&isPress)
    {
        ui->label->isPress = true;

        ui->label->movePos.setValue(temp);

        ui->label->endpos[ui->label->pn_Number].setValue(temp);
        ui->label->update();

    }
    else
    {
        ui->label->isPress = false;

        ui->label->movePos.setValue(temp);

        ui->label->update();

    }
    return QWidget::mouseMoveEvent(event);

}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{

    Point po(event->x(),event->y());

    if(event->button()==Qt::LeftButton)


    if(ui->label->geometry().contains(event->pos()))
    {
    Point temp(event->x(),event->y());
    newPoint(temp);
    ui->label->endpos[ui->label->pn_Number].setValue(temp);
    ui->label->update();

    isPress = false;
    }
    return QWidget::mouseReleaseEvent(event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if(ui->label->pn_Number<0) return;
    if(ui->label->geometry().contains(event->pos()))
    {
    if(isStore)
    {
        QMessageBox msgBox;
        msgBox.setText("该图片已经保存");
        //显示文本
        msgBox.setInformativeText("是否继续修改?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        switch (ret)
        {
          case QMessageBox::Ok:
            //event->accept();
            isStore = false;
            return ;
            break;
          case QMessageBox::Cancel:
            //event->ignore();
            return ;
            break;
          default:
              // should never be reached
              break;
          }


      }
        else
        {
             ui->label->pn_Number--;
            ui->label->repaint();
            ui->lineEditSavePlateNumber->clear();

            ui->label->update();
        }
    }





}
void MainWindow::mousePressEvent(QMouseEvent *event)
{

       if(event->button()==Qt::LeftButton){

           if(ui->labelTitleIco->geometry().contains(event->pos()))
           {

               helpDoc *help = new helpDoc;

               help->show();

               return ;
           }
           Point po(event->x(),event->y());

           if(ui->label->geometry().contains(event->pos()))
           {
               if(isStore)
               {
                   QMessageBox msgBox;
                   msgBox.setText("该车牌曾被处理");
                   //显示文本
                   msgBox.setInformativeText("是否进行修改?");
                   msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                   msgBox.setDefaultButton(QMessageBox::Ok);
                   int ret = msgBox.exec();
                   switch (ret)
                   {
                     case QMessageBox::Ok:
                       //event->accept();
                       isStore = false;
                       fileinfo.finishNum--;
                       return ;
                       break;
                     case QMessageBox::Cancel:
                       //event->ignore();
                       return ;
                       break;
                     default:
                         // should never be reached
                         break;
                     }


                 }


               if(ui->label->pn_Number>=19)
                   return ;
               ui->label->pn_Number++;
                isPress = true;
                Point temp(event->x(),event->y());
                newPoint(temp);
                ui->label->startpos[ui->label->pn_Number].setValue(temp);
                ui->label->colorArray[ui->label->pn_Number] = currentColor;

               if(pointNum<3)
               {



                   if(pointNum==3)
                   {
                            cpn.cpnumber[0].center.x = (tempP[0].x+tempP[2].x)/2;
                            cpn.cpnumber[0].center.y = (tempP[0].y+tempP[2].y)/2;
                            cpn.cpnumber[0].width = sqrt(  (tempP[0].x-tempP[1].x)*(tempP[0].x-tempP[1].x)  + (tempP[0].y-tempP[1].y)*(tempP[0].y-tempP[1].y))/cpn.picture->width();
                            cpn.cpnumber[0].height=sqrt(  (tempP[2].x-tempP[1].x)*(tempP[2].x-tempP[1].x)  + (tempP[2].y-tempP[1].y)*(tempP[2].y-tempP[1].y))/cpn.picture->height();


                   }
               }
           }
       }
       else if(event->button()==Qt::BackButton)
           on_pushButtonSavePlate_clicked();
       else if(event->button()==Qt::ForwardButton)
       {
           if(ui->yellow->isChecked())
           {
               on_blue_clicked();
           }
           else
           {
               on_yellow_clicked();
           }
       }


       return QWidget::mousePressEvent(event);
}
bool MainWindow::Contain(int x,int y)
{
    if(x>=l_x&&y>=l_y&&x<=l_width+x&&y<=l_height+l_y)
        return true;
    return false;
}
bool MainWindow::Contain(Point &p)
{
    int x=p.x;
    int y=p.y;
    if(x>=l_x&&y>=l_y&&x<=l_width+x&&y<=l_height+l_y)
        return true;
    return false;
}
void MainWindow::on_pushButton_PicturePath_clicked()
{
    QString folderName = QFileDialog::getExistingDirectory(this,tr("选择图片所在位置"),lastSPath);
    if(!folderName.isNull())
    {
            lastSPath = folderName;
        if(folderName!=ui->lineEditPicture->text())
        {
            ui->lineEditPicture->setText(folderName);
            fileinfo.lastNum=0;
            fileinfo.currentPath = folderName;
            updateFile();
            readInfoFile();
        }
    }
}
void MainWindow::on_pushButton_SavePath_clicked()
{
    QString folderName = QFileDialog::getExistingDirectory(this,tr("选择保存位置"),lastSPath);
    if(!folderName.isNull())
    {
            lastSPath = folderName;
        if(folderName!=ui->lineEditSave->text())
        {
            ui->lineEditSave->setText(folderName);
            fileinfo.savePath = folderName;
            updateFile();
        }
    }
}
void MainWindow::on_specialPath_clicked()
{

    QString folderName = QFileDialog::getExistingDirectory(this,tr("选择特殊车牌保存位置"),lastSPath);
    if(!folderName.isNull())
    {
            lastSPath = folderName;
        if(folderName!=ui->lineEditSpecial->text())
        {
            ui->lineEditSpecial->setText(folderName);
            fileinfo.specialPath = folderName;
            updateFile();
        }
    }

}
bool MainWindow::WindowIsModify()///////////////////////////////////////////////////////////
{
    if(isStore)
        return false;
    return true;
}
bool MainWindow::ModifyWarning()
{

    {
        QMessageBox msgBox;
        msgBox.setText("车牌信息未保存.");
        //显示文本
        msgBox.setInformativeText("是否确认离开?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Ok:
            //event->accept();
            return false;
            break;
          case QMessageBox::Cancel:
            //event->ignore();
            return true;
            break;
          default:
              // should never be reached
             return true;
              break;
        }
    }
}


void MainWindow::updateWindow(int i)
{
    if(i==-2)//向前
    {
        if(fileinfo.lastNum<=0) return ;
        if(WindowIsModify())
            if(ModifyWarning())
                return ;
        fileinfo.lastNum--;
    }
    else if(i==-1)//向后
    {
        if(fileinfo.lastNum==(oglist.size()-1))
            return ;
        if(WindowIsModify())
            if(ModifyWarning())
                return ;
        fileinfo.lastNum++;
    }
    else
    {
        if(WindowIsModify())
            if(ModifyWarning())
                return ;
        fileinfo.lastNum = i;
    }

        ui->label->pn_Number=-1;
        pointNum = 0;
        updateFile();
        ui->label->update();
        ShowCalculateNum();

}

void MainWindow::on_pushButtonLastPicture_clicked()
{
    updateWindow(-2);
    getCurrentPicture();
    FindpnNumber();
}
void MainWindow::on_pushButtonNextPicture_clicked()
{
    if(!isStore)
    {
        QMessageBox::warning(this,"警告","请先保存当前图片");
        return ;
    }
    updateWindow(-1);
    getCurrentPicture();
    FindpnNumber();
    ui->statusBar->clearMessage();
}
void MainWindow::newPoint(Point &p)
{
    p.x = p.x-l_x;
    p.y = p.y-l_y;
}
QString  MainWindow::newName()
{
    char name[9];
    sprintf(name,"01%07d",fileinfo.lastNum+1);

    return QString::fromLocal8Bit(name);
}
QString  MainWindow::newName(int s,int v)
{
    if(s!=-1)
    {
        char name[10];
         sprintf(name,"%02d%07d",s,v);
        return QString::fromLocal8Bit(name);
    }
    else
    {
        char name[8];
        sprintf(name,"%7d",v);
        return QString::fromLocal8Bit(name);
    }
}
void MainWindow::on_pushButtonSavePlate_clicked()
{
    if(isStore)
        return;

    int snum = pnnum;

    QString PicName = ui->lineEditSave->text() +"/"+ newName();
    if(snum!=-1)
    {
        QFile *file = new QFile;

         file->setFileName(PicName+".jpg");
         file->remove();

        for(int i=1;i<=snum;i++)
        {
            char name[4];
            sprintf(name,"%02d",i);
            QString count(QString::fromLocal8Bit(name));

            file->setFileName(PicName+"_"+count+".jpg");
             file->remove();

            file->setFileName(PicName+"_"+count+".txt");
            file->remove();

        }

    }
    /*


    //先删除之前的车牌，进行更新
    QString pname = newName();
    QDir dir;
    dir.setPath(fileinfo.savePath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    clock_t t14=clock();

    QFileInfoList list2 = dir.entryInfoList();
   int i=0;

   */
/*
    QFile *file = new QFile;
    for(;i<list2.size();i++)
    {
        QFileInfo finfo = list2.at(i);

       if(finfo.fileName().contains(pname))
       {
           file->setFileName(finfo.filePath());
           file->remove();
       }


    }

    */
    ////移除车牌结束。

    int lw = ui->label->pixmap()->width();
    int lh = ui->label->pixmap()->height();


    cpn.picture->save(PicName+".jpg");

    int num = ui->label->pn_Number;
    for(int i=0;i<=num;i++)
    {
        char name[4];
        sprintf(name,"%02d",i+1);
        QString count(QString::fromLocal8Bit(name));

        ui->label->capturePixmap[i].save(PicName+"_"+count+".jpg");

        //QRect r = ui->label->capturePixmap[i].rect();


        int w = ui->label->endpos[i].x - ui->label->startpos[i].x;
        int h = ui->label->endpos[i].y - ui->label->startpos[i].y;
        int cx = ui->label->startpos[i].x+w/2;
        int cy = ui->label->startpos[i].y+h/2;

        QFile *file = new QFile;
        file->setFileName(PicName+"_"+count+".txt");
        bool ok =file->open(QIODevice::WriteOnly| QIODevice::Text );
        if(ok)
        {
            QTextStream out(file);
            out<<ui->label->colorArray[i]<<"\n"
              <<(double)cx/lw<<" "<<(double)cy/lh<<"\n"
             <<(double)w/lw<<" "<<(double)h/lh<<"\n";
            file->close();
            delete file;
        }
        else
        {
            QMessageBox::warning(this,"Warning Message:","Fail in save Car-Plate.");
            return ;
        }

  }

    ui->lineEditSavePlateNumber->setFont(QFont(NULL,70));
    ui->lineEditSavePlateNumber->setText(QString("%1").arg(num+1));
    //ui->statusBar->showMessage(QString("delete %1 ms,save %2 ms").arg(t2-t1).arg(t3-t2));

    isStore = true;
    Stored = true;
    fileinfo.finishNum++;

    ///////////////////////////////////////////////////////////
}
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_1)
    {
        on_yellow_clicked();
        return ;
    }
    if(e->key()==Qt::Key_2)
    {
        on_blue_clicked();
        return ;
    }
    if(e->key()==Qt::Key_3)
    {
        on_black_clicked();
        return ;
    }
    if(e->key()==Qt::Key_4)
    {
        on_white_clicked();
        return ;
    }
    if(e->key()==Qt::Key_5)
    {
        on_green_clicked();
        return ;
    }
    if(e->key()==Qt::Key_6)
    {
        on_yellowgreen_clicked();
        return ;
    }

    if(e->key()==Qt::Key_F1)
    {
        if(ui->yellow->isChecked())
        {
            on_blue_clicked();
        }
        else
        {
            on_yellow_clicked();
        }

    }
    if(e->key()==Qt::Key_Escape)
    {
        on_pushButtonSpecial_clicked();
    }
}
bool MainWindow::getCurrentPicture()
{
    if(oglist.size()==0)
    {
        QMessageBox::warning(this,"Error Message:","该路径下无图片文件.");
        on_pushButton_PicturePath_clicked();
        return false;
    }
     int   num = fileinfo.lastNum;

    if(num<oglist.size())
    {
        QFileInfo finfo = oglist.at(num);
        cpn.pictureName = finfo.fileName();

        if(cpn.picture!=NULL)
        {
            delete cpn.picture;
        }
        cpn.picture =new QPixmap(finfo.filePath());


        if(cpn.picture->isNull())
        {
            on_pushButtonSpecial_clicked();

            return false;
        }
        ui->lineEditSavePlateNumber->clear();
        ui->label->setPixmap(cpn.picture->scaled(ui->label->size()));


        ui->labelPictureCount->setFont(QFont(NULL,30));
        ui->labelPictureCount->setText(QString::number(fileinfo.lastNum+1)+" / "+QString::number(oglist.size()));
    }
    else
    {
        QMessageBox::information(this,"恭喜","当前目录任务完成，辛苦辛苦");
        QApplication::quit();
    }

    return true;
}
void MainWindow::readInfoFile()
{
    QString filePath = QCoreApplication::applicationDirPath();
    if(filePath.isEmpty())
    {
        QMessageBox::warning(this,"Error Message:","程序路径不存在.");
        return ;
    }
    bool quit =true;
    while(quit)
    {
        quit = false;

        QFile *file = new QFile;
        file->setFileName(filePath+"/fileinfo.txt");
        bool ok =file->open(QIODevice::ReadOnly);
        if(ok)
        {
            QTextStream in(file);
            in>>fileinfo.finishNum>>fileinfo.lastNum;
            in>>fileinfo.currentPath;
            in>>fileinfo.savePath;
            in>>fileinfo.specialPath;

            ui->lineEditPicture->setText(fileinfo.currentPath);
            ui->lineEditSave->setText(fileinfo.savePath);
            ui->lineEditSpecial->setText(fileinfo.specialPath);
             file->close();
             delete file;

            QDir dir;
            dir.setPath(fileinfo.currentPath);
            QDir dirSave;
            dirSave.setPath(fileinfo.savePath);
            QDir dirSpecial;
            dirSpecial.setPath(fileinfo.specialPath);
            if(!dir.exists())
            {
                QMessageBox::warning(this,"Error Message:","图片获取路径不存在.");
                on_pushButton_PicturePath_clicked();
                quit = true;
                continue;
            }
            if(!dirSave.exists())
            {
                QMessageBox::warning(this,"Error Message:","图片存储路径不存在.");
                on_pushButton_SavePath_clicked();

                quit = true;
                continue;
            }
            if(!dirSpecial.exists())
            {
                QMessageBox::warning(this,"Error Message:","特殊图片存储路径不存在.");
                on_specialPath_clicked();

                quit = true;
                continue;
            }

            dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
            dir.setSorting(QDir::Name | QDir::Reversed);

            oglist = dir.entryInfoList();
            if(!getCurrentPicture())
            {
                quit=true;
                continue;
            }
        }
        else
        {
            QMessageBox::warning(this,"Warning Message:","未发现配置文件");
            return ;
        }
    }
}
void MainWindow::on_yellow_clicked()
{

    ui->yellow->setChecked(true);
    ui->blue->setChecked(false);
    ui->black->setChecked(false);
    ui->white->setChecked(false);
    ui->green->setChecked(false);
    ui->yellowgreen->setChecked(false);
    currentColor = 1;
}
void MainWindow::on_blue_clicked()
{
    /////////////////////////////////////////////////////////////
    ui->yellow->setChecked(false);
    ui->blue->setChecked(true);
    ui->black->setChecked(false);
    ui->white->setChecked(false);
    ui->green->setChecked(false);
    ui->yellowgreen->setChecked(false);
    currentColor = 2;
}
void MainWindow::on_black_clicked()
{
    ui->yellow->setChecked(false);
    ui->blue->setChecked(false);
    ui->black->setChecked(true);
    ui->white->setChecked(false);
    ui->green->setChecked(false);
    ui->yellowgreen->setChecked(false);
    currentColor = 3;
}
void MainWindow::on_white_clicked()
{
    ui->yellow->setChecked(false);
    ui->blue->setChecked(false);
    ui->black->setChecked(false);
    ui->white->setChecked(true);
    ui->green->setChecked(false);
    ui->yellowgreen->setChecked(false);
    currentColor = 4;
}
void MainWindow::on_green_clicked()
{
    ui->yellow->setChecked(false);
    ui->blue->setChecked(false);
    ui->black->setChecked(false);
    ui->white->setChecked(false);
    ui->green->setChecked(true);
    ui->yellowgreen->setChecked(false);
    currentColor = 5;
}

void MainWindow::on_yellowgreen_clicked()
{
    ui->yellow->setChecked(false);
    ui->blue->setChecked(false);
    ui->black->setChecked(false);
    ui->white->setChecked(false);
    ui->green->setChecked(false);
    ui->yellowgreen->setChecked(true);
    currentColor = 6;
}
void MainWindow::on_pushButtonSpecial_clicked()
{
    if(Stored)
    {
        QMessageBox::warning(this,"警告：","该车牌曾被保存过\n不能再次进行特殊处理");
        return ;

    }
    ui->label->pn_Number = -1;
    ui->label->update();

    QString cpnName = fileinfo.currentPath+"/"+cpn.pictureName;
    QPixmap cp(cpnName);
    cp.save(ui->lineEditSpecial->text()+"/"+cpn.pictureName);
    QFile file(cpnName);
   if( file.remove())
   {
       QMessageBox::information(this,"提示",QString("%1已经移除到指定目录").arg(cpnName));
       readInfoFile();
       getCurrentPicture();
       FindpnNumber();

   }
   else
   {
       ui->statusBar->showMessage(QString("%1移除失败，请手动移除").arg(cpnName));
   }
}

void MainWindow:: SearchSaveNumber()
{
    //start when the path of current or save is changed.

    QString curPath = fileinfo.currentPath;
    QString savePath = fileinfo.savePath;

    QDir dir(savePath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    QFileInfoList savelist = dir.entryInfoList();
    if(savelist.size()==0) return ;

    QStringList startList = savelist.at(0).fileName().split("/");
    int slen = startList.size();
    QString sname = startList.at(slen-1);
    bool ok1=false;
    int sc = QString(sname.split(".").at(0)).toLong(&ok1)/10000000;


    qDebug()<<sc<<" "<<sname<<"\n"<<QString(savePath+"/"+newName(sc,1)+".jpg");
    int i=1;
    while(true)
    {
       if(QFile(savePath+"/"+newName(sc,i)+".jpg").exists())
           i++;
       else
           break;
    }
    fileinfo.finishNum = i-1;
    qDebug()<<"max="<<i-1;
    updateFile();
}
void MainWindow::FindpnNumber()
{

    Stored = false;
    isStore = false;

/////////////////////////////////////////////////////////////
/// \brief locatePn
///
    QString PicName = ui->lineEditSave->text() +"\\"+ newName();
    QFile *file = new QFile;
    file->setFileName(PicName+".jpg");
    if(file->exists())
    {
        Stored = true;
        isStore = true;

        int i=1;
        char name[4];
        while(1)
        {
            sprintf(name,"%02d",i);
            QString count(QString::fromLocal8Bit(name));
            file->setFileName(PicName+"_"+count+".txt");
            if(file->exists())
            {
                ui->label->pn_Number=i-1;
                 bool ok =file->open(QIODevice::ReadOnly|QIODevice::Text);

                 int color;
                 double xcx,xcy,xw,xh;

                if(ok)
                {
                    QTextStream in(file);
                    in>>color>>xcx>>xcy>>xw>>xh;
                    setCurPn(i-1,color,xcx,xcy,xw,xh);
                    file->close();

                }
                else
                {
                       delete file;
                    break;
                }
            }
            else
                break;
            i++;
        }
        ui->label->pn_Number = i-2;
        pnnum = i-1;
        ui->lineEditSavePlateNumber->setFont(QFont(NULL,70));
        ui->lineEditSavePlateNumber->setText(QString("%1").arg(pnnum));

        ui->label->update();


    }
    else
    {

        ui->lineEditSavePlateNumber->setFont(QFont(NULL,70));
        ui->lineEditSavePlateNumber->setText("-1");
        isStore = false;
        pnnum = -1;

    }




////////////////////////////

    /*

    for(;i<list.size();i++)
    {
        QFileInfo finfo = list.at(i);

       if(finfo.fileName().contains(pname))
       {
           if(!existPicture)
               existPicture = true;
           if(finfo.fileName().contains(".txt"))
           {
               locatePn[PnNumber++] = i;
           }

       }
    }
    if(existPicture&&PnNumber==0)
    {
        ui->lineEditSavePlateNumber->setFont(QFont(NULL,70));
        ui->lineEditSavePlateNumber->setText("0");

        isStore = true;
        Stored = true;
        ui->label->pn_Number = -1;
        pnnum = 0;
    }
    else if(!existPicture)
    {

    }
    else
    {
        Stored = true;
        isStore = true;
        ui->lineEditSavePlateNumber->setFont(QFont(NULL,70));
        ui->lineEditSavePlateNumber->setText(QString("%1").arg(PnNumber));
        pnnum = PnNumber;
        ui->label->pn_Number = PnNumber-1;
        int color;
        double xcx,xcy,xw,xh;
        for(int i=0;i<PnNumber;i++)
        {
            QFileInfo finfo = list.at(locatePn[i]);

            QFile *file = new QFile(finfo.filePath());
             bool ok =file->open(QIODevice::ReadOnly|QIODevice::Text);


            if(ok)
            {
                QTextStream in(file);
                in>>color>>xcx>>xcy>>xw>>xh;

                setCurPn(i,color,xcx,xcy,xw,xh);


                file->close();
                delete file;
            }
        }


        ui->label->update();
    }*/

}
bool MainWindow::judgeIsCurPn(QString p, QString curname)
{
    int len = curname.length();
    for(int i=0;i<len;i++)
    {
        if(curname[i]!=p[i])
            return false;
    }

        if(!p.contains(".txt"))
        {
            return false;
        }
    return true;
}


void MainWindow::setCurPn(int count,int color, double xcx, double xcy, double xw, double xh)
{

    int lw = ui->label->pixmap()->width();
    int lh = ui->label->pixmap()->height();

    int cx = xcx*lw,
         cy = xcy*lh,
         w = xw*lw,
         h = xh*lh;

    Point start,end;
    start.x = cx-w/2;
    start.y = cy-h/2;
    end.x = cx+w/2;
    end.y = cy+h/2;
   ui->label->startpos[count].setValue(start);
   ui->label->endpos[count].setValue(end);

    ui->label->colorArray[count] = color;

}

void MainWindow::on_pushButtonChangeName_clicked()
{
    swidget.setFrom(ui->lineEditSave->text());

    swidget.initText("格式化","起始编号:");
    swidget.noteSPath(lastSPath);
    swidget.show();
}

void MainWindow::on_pushButtonSkip_clicked()
{
    SearchSaveNumber();
    bool ok;
    const  QString text = QString("请输入页码: max=%1").arg(fileinfo.finishNum);
    int i = QInputDialog::getInt(this,tr("跳转"),text,startCount+1,1,fileinfo.finishNum,10,&ok);
    if(ok)
    {
        updateWindow(i-1);
        //更新界面  =0  set count label
        getCurrentPicture();
        FindpnNumber();
        ShowCalculateNum();
    }

}
void MainWindow::ShowCalculateNum()
{
    //ui->labelCurFinishNumber->setFont(QFont(NULL,20));
    ui->labelCurFinishNumber->setText(QString("本次完成：%1张").arg(fileinfo.finishNum-lastFinishNumber));
}

void MainWindow::updateTime()
{
    RunTime++;
        showSpeedAndRunTime();
        QTime currentTime = QTime::currentTime();
        ui->timeLcdNumber->display(currentTime.toString());
    shutTime->start(1000);
}
void MainWindow::showSpeedAndRunTime()
{

    int curSecond=RunTime%60;
    int runMinute = RunTime/60;
    QString runtime = QString("程序已运行%1分%2秒").arg(runMinute).arg(curSecond);

    int speed = fileinfo.finishNum-lastMinuteCount[curSecond];
    lastMinuteCount[curSecond]+=speed;

    QString speedStr = QString("处理速度：%1张每分钟").arg(speed);

    ui->labelSpeed->setText(runtime+"\n"+speedStr);


}
void MainWindow::initStyle()
{
    //加载样式表
    QFile file(":/qss/psblack.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}
void MainWindow::gshSlot()
{
    int newSence = swidget.getSence();
    int newCount = swidget.getStartNum();

    QString fromP = swidget.getFrom();
    QString saveP = swidget.getTo();
    if(QDir(fromP).exists()&&QDir(saveP).exists())
    {

    ui->pushButtonChangeName->setEnabled(false);
    ui->pushButtonDeleteOld->setEnabled(false);
    ui->pushButtonMoveUntreated->setEnabled(false);

        mythread->init(gsh,fromP,saveP,newSence,newCount);
        mythread->start();
        checkThreadIsEndTimer->start(300);
        curThreadOperation = 1;
        progressWidget->show();
        tishilabel->setFont(QFont(NULL,50));
        tishilabel->setText("正在格式化");
        progressWidget->setWindowTitle("格式化进度");
    }
    else
    {
        QMessageBox::warning(this,"警告","无效路径");
    }
}


void MainWindow::on_pushButtonDeleteOld_clicked()
{
    if(QMessageBox::warning(this,"警告","请谨慎操作",QMessageBox::Ok|QMessageBox::Cancel)!=QMessageBox::Ok) return;

    QMessageBox msgBox;
    msgBox.setText("警告");
    //显示文本
    msgBox.setInformativeText("该操作会删除已经处理过的图片原始图片和截取后的信息文件，请务必在格式化或备份信息文件后进行该操作。\n是否继续删除");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret)
    {
      case QMessageBox::Ok:
        break;
      case QMessageBox::Cancel:
        return ;
      default:
          // should never be reached
          break;
      }

    mythread->init(deleteOld,fileinfo.currentPath,fileinfo.savePath,
                   fileinfo.finishNum,0);
    mythread->setOglist(oglist);
    mythread->start();
    checkThreadIsEndTimer->start(300);
    curThreadOperation = 2;
    progressWidget->show();
    tishilabel->setFont(QFont(NULL,50));
    tishilabel->setText("正在清除");
    progressWidget->setWindowTitle("清楚内存进度");
    ui->pushButtonChangeName->setEnabled(false);
    ui->pushButtonDeleteOld->setEnabled(false);
    ui->pushButtonMoveUntreated->setEnabled(false);
}

void MainWindow::on_pushButtonMoveUntreated_clicked()
{
    num->setMinimum(1);
   num->setMaximum(oglist.size()-fileinfo.finishNum);
   widget->show();
   widget->move((QApplication::desktop()->width()-widget->width())/2,(QApplication::desktop()->height()-200));
}

void MainWindow::chooseMovePath()
{
    QString folderName = QFileDialog::getExistingDirectory(this,tr("选择文件夹所在位置"),lastSPath);
    if(!folderName.isNull())
    {
            lastSPath = folderName;
        if(folderName!=lineedit->text())
        {
            lineedit->setText(folderName);
            moveP= folderName;
        }
    }
}
void MainWindow::startMove()
{
   if(!QDir(moveP).exists()) return ;
   widget->close();

    size = oglist.size();
   int n = size-num->value();
   mythread->init(moveUntreated,fileinfo.currentPath,moveP,n,size);
    mythread->setOglist(oglist);
   mythread->start();
   checkThreadIsEndTimer->start(300);
   curThreadOperation = 3;
   progressWidget->show();
    tishilabel->setFont(QFont(NULL,50));
    tishilabel->setText("正在移动");
    progressWidget->setWindowTitle("移动未处理图片进度");
    ui->pushButtonChangeName->setEnabled(false);
    ui->pushButtonDeleteOld->setEnabled(false);
    ui->pushButtonMoveUntreated->setEnabled(false);
}
void MainWindow::showProcess(int p)
{
    progress->setValue(p);
//    ui->lineEditSavePlateNumber->setText(QString("%1").arg(p));
}

void MainWindow::checkThreadIsEnd()
{
    if(mythread->isFinished())
    {
        checkThreadIsEndTimer->stop();
        mythread->terminate();

        switch (curThreadOperation) {
        case 1:
        {
            getCurrentPicture();
            FindpnNumber();
            progress->setValue(100);
            tishilabel->setFont(QFont(NULL,50));
            ui->pushButtonChangeName->setEnabled(true);
            ui->pushButtonDeleteOld->setEnabled(true);
            ui->pushButtonMoveUntreated->setEnabled(true);
            tishilabel->setText("格式化完成");
        }
            break;
       case 2://delOld
        {
            fileinfo.finishNum=0;
            fileinfo.lastNum = 0;
            lastFinishNumber=0;
            updateFile();
            readInfoFile();
            progress->setValue(100);
            FindpnNumber();
            tishilabel->setFont(QFont(NULL,50));
            tishilabel->setText("移除完成");
            ui->pushButtonChangeName->setEnabled(true);
            ui->pushButtonDeleteOld->setEnabled(true);
            ui->pushButtonMoveUntreated->setEnabled(true);
        }
            break;
        case 3:
        {
            readInfoFile();
            FindpnNumber();
            progress->setValue(100);
            tishilabel->setFont(QFont(NULL,50));
            tishilabel->setText("转移完成");
            ui->pushButtonChangeName->setEnabled(true);
            ui->pushButtonDeleteOld->setEnabled(true);
            ui->pushButtonMoveUntreated->setEnabled(true);
        }
            break;
        default:
            break;
        }
    }
}

