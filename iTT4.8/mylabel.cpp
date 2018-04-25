#include "mylabel.h"
#include <QPainter>
myLabel::myLabel(QWidget *parent) :
    QLabel(parent)
{
    pn_Number = -1;

    isPress = false;
    setMouseTracking(true);

}
void myLabel::paintEvent(QPaintEvent *)
{
        painter.begin(this);          //进行重绘;
        //QColor shadowColor = QColor(0, 0, 0, 200);                      //阴影颜色设置;
        painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::FlatCap));    //设置画笔;
                          //将背景图片画到窗体上;
        painter.drawPixmap(0, 0, pixmap()->scaled(size()));


        //画辅助线
      //  if(!isPress)
         {
            // painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::FlatCap));

             //Point p(movePos.x+100,movePos.y+100);
             //painter.drawLine(movePos.x,movePos.y,p.x,p.y);
//qDebug
            // qDebug()<<QString("%1,%2tttttt").arg(movePos.x).arg(movePos.y);
         }

            for(int i=0;i<=pn_Number;i++)
            {
                painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::FlatCap));    //设置画笔;
                painter.setBrush(Qt::transparent);


                int sx = (startpos[i].x<endpos[i].x)?startpos[i].x:endpos[i].x;
                int sy = (startpos[i].y<endpos[i].y)?startpos[i].y:endpos[i].y;
                int ex = (startpos[i].x>endpos[i].x)?startpos[i].x:endpos[i].x;
                int ey = (startpos[i].y>endpos[i].y)?startpos[i].y:endpos[i].y;
                QRect  selectedRect = QRect(sx,sy,ex-sx,ey-sy);
                judgeRect(selectedRect);
                capturePixmap[i] = pixmap()->copy(selectedRect);
                painter.drawPixmap(selectedRect.topLeft(), capturePixmap[i]);
                painter.drawRect(selectedRect);

                QRect selectedRect0 = QRect(sx,sy-20,30,20);
                QRect selectedRect00 = QRect(sx+30,sy-17,120,14);
                judgeRect(selectedRect0);
                judgeRect(selectedRect00);
                switch (colorArray[i]) {
                case 1:
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::yellow);
                    break;
                case 2:
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::blue);
                    break;
                case 3:
                    painter.setPen(Qt::white);
                    painter.setBrush(Qt::black);
                    break;
                case 4:
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::white);
                    break;
                  case 5:
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::green);
                    break;
                case 6:
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::yellow);
                    break;
                default:
                    painter.setPen(Qt::red);
                    painter.setBrush(Qt::red);
                    break;
                }
                painter.setFont(QFont("Arial",15));
                painter.drawRect(selectedRect0);
                painter.drawText(selectedRect0,Qt::AlignCenter,QString::number(i+1));

                if(colorArray[i]==6)
                painter.setBrush(Qt::green);
                painter.setFont(QFont("Arial",8));
                painter.drawRect(selectedRect00);
                painter.drawText(selectedRect00,QString("(%1,%2)w=%3,h=%4").arg(sx).arg(sy).arg(ex-sx).arg(ey-sy));
            }

       // else
        //painter.drawPixmap(0, 0, pixmap()->scaled(size()));

        painter.end();  //重绘结束;
}



Point::Point(int x,int y):x(x),y(y){}
Point::Point(Point &p):x(p.x),y(p.y){}
void Point::setValue(Point p)
{
    x = p.x;
    y = p.y;
}
void myLabel::judgeRect(QRect &r)
{
    if(r.x()<=0)
        r.setX(1);
    if(r.y()<=0)
        r.setY(1);
    if(r.width()<=0)
        r.setWidth(1);
    if(r.height()<=0)
        r.setHeight(1);


}
