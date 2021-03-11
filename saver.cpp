#include "saver.h"

Saver::Saver()
{
}

Saver::~Saver()
{
}

void Saver::setParam(int max,bool capt[12],QPixmap saveImg[12])
{
    this->max=max;
    for(int i=0;i<12;i++){
        this->capt[i]=capt[i];
        this->saveImg[i]=saveImg[i];
    }
}

void Saver::run()
{
    for(int i=0;i<12;i++){
        emit saving(i+1);
        if(!capt[i]) continue;
        fileImg.setFileName(fileName[i]);
        fileImg.open(QIODevice::WriteOnly);
        saveImg[i].save(&fileImg,"PNG");
        fileImg.close();
    }
    emit saving(max);
    QPixmap resImg(3880,1960);
    painter.begin(&resImg);
    painter.fillRect(0,0,3880,1960,QColor(255,255,255));
    painter.setFont(QFont("Arial",60));
    painter.drawText(1670,10,420,80,Qt::AlignCenter,"Normal");
    painter.drawText(2320,10,420,80,Qt::AlignCenter,"Strabismus");
    painter.drawText(1350,230,200,100,Qt::AlignCenter,"Right");
    painter.drawText(1350,600,200,100,Qt::AlignCenter,"Left");
    QSize size=saveImg[0].size().scaled(640,360,Qt::KeepAspectRatio);
    if(capt[0]){
        painter.drawPixmap(1880-size.width()/2,280-size.height()/2,size.width(),size.height(),saveImg[0],0,0,saveImg[0].width(),saveImg[0].height());
        painter.drawPixmap(1940-size.width(),1400-size.height()/2,size.width(),size.height(),saveImg[0],0,0,saveImg[0].width(),saveImg[0].height());
    }
    if(capt[1]){
        painter.drawPixmap(1880-size.width()/2,650-size.height()/2,size.width(),size.height(),saveImg[1],0,0,saveImg[1].width(),saveImg[1].height());
        painter.drawPixmap(1940,1400-size.height()/2,size.width(),size.height(),saveImg[1],0,0,saveImg[1].width(),saveImg[1].height());
    }
    if(capt[2]) painter.drawPixmap(2530-size.width()/2,280-size.height()/2,size.width(),size.height(),saveImg[2],0,0,saveImg[2].width(),saveImg[2].height());
    if(capt[3]) painter.drawPixmap(2530-size.width()/2,650-size.height()/2,size.width(),size.height(),saveImg[3],0,0,saveImg[3].width(),saveImg[3].height());
    size=saveImg[4].size().scaled(1280,360,Qt::KeepAspectRatio);
    if(capt[4]) painter.drawPixmap(650-size.width()/2,1030-size.height()/2,size.width(),size.height(),saveImg[4],0,0,saveImg[4].width(),saveImg[4].height());
    if(capt[5]) painter.drawPixmap(1940-size.width()/2,1030-size.height()/2,size.width(),size.height(),saveImg[5],0,0,saveImg[5].width(),saveImg[5].height());
    if(capt[6]) painter.drawPixmap(3230-size.width()/2,1030-size.height()/2,size.width(),size.height(),saveImg[6],0,0,saveImg[6].width(),saveImg[6].height());
    if(capt[7]) painter.drawPixmap(650-size.width()/2,1400-size.height()/2,size.width(),size.height(),saveImg[7],0,0,saveImg[7].width(),saveImg[7].height());
    if(capt[8]) painter.drawPixmap(3230-size.width()/2,1400-size.height()/2,size.width(),size.height(),saveImg[8],0,0,saveImg[8].width(),saveImg[8].height());
    if(capt[9]) painter.drawPixmap(650-size.width()/2,1770-size.height()/2,size.width(),size.height(),saveImg[9],0,0,saveImg[9].width(),saveImg[9].height());
    if(capt[10]) painter.drawPixmap(1940-size.width()/2,1770-size.height()/2,size.width(),size.height(),saveImg[10],0,0,saveImg[10].width(),saveImg[10].height());
    if(capt[11]) painter.drawPixmap(3230-size.width()/2,1770-size.height()/2,size.width(),size.height(),saveImg[11],0,0,saveImg[11].width(),saveImg[11].height());
    painter.end();
    fileImg.setFileName("Result.png");
    fileImg.open(QIODevice::WriteOnly);
    resImg.save(&fileImg,"PNG");
    fileImg.close();
    emit saving(max+1);
}
