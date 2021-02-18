#include "viewer.h"
#include "ui_viewer.h"
#include "videoframer.h"

Viewer::Viewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Viewer)
{   
    ui->setupUi(this);
    timer=new QTimer;
    timer->start(100);
    mPlayer=new QMediaPlayer(this,QMediaPlayer::VideoSurface);
    mList=new QMediaPlaylist();
    label=new QLabel;
    counter=new QString("Video Count : ");
    setFocus();
    cur=QTime(0,0,0);
    all=QTime(0,0,0);
    mPlayer->setVideoOutput(framer);

    mList->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    mPlayer->setPlaylist(mList);
    mPlayer->setNotifyInterval(33);
    disableControl();
    ui->filterBox->setPlaceholderText("Search...");

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->nFrameButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pFrameButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->actionSelect_Folder->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    ui->statusbar->addWidget(label);
    ui->statusbar->setSizeGripEnabled(0);
    label->setText(counter->append(QString::number(mList->mediaCount())));
    ui->curTime->setText(cur.toString("mm:ss"));
    ui->allTime->setText(all.toString("mm:ss"));

    connect(ui->listWidget,&QAbstractItemView::activated,this,&Viewer::jump);
    connect(ui->actionSelect_Folder,SIGNAL(triggered()),this,SLOT(chkFolder()));
    connect(mPlayer,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(durChanged(QMediaPlayer::MediaStatus)));
    connect(mPlayer,&QMediaPlayer::positionChanged,this,&Viewer::posChanged);
    connect(ui->filterBox,SIGNAL(returnPressed()),this,SLOT(on_toolButton_clicked()));
    connect(timer,SIGNAL(timeout()),this,SLOT(frontback()));
    connect(ui->photoReset,SIGNAL(clicked()),this,SLOT(resetPhoto()));
    connect(framer,SIGNAL(frameAvailable(QImage)),this,SLOT(processFrame(QImage)));

    defimg.load(":/image/default_vid.png");
    defimg=defimg.scaled(ui->vidLabel->width(),ui->vidLabel->height());
    ui->vidLabel->setPixmap(defimg);
    resetPhoto();
}

Viewer::~Viewer()
{
    delete ui;
}


void Viewer::on_playButton_clicked()
{
    switch (mPlayer->state()) {
        case QMediaPlayer::PlayingState:
            mPlayer->pause();
            ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        default:
            mPlayer->play();
            ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
    }
}

void Viewer::jump(const QModelIndex &index)
{
    if(index.isValid()){
        mList->setCurrentIndex(index.row());
        mPlayer->play();
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        ui->frameBox->setValidator(range);
        enableControl();
    }
    setFocus();
}

void Viewer::chkFolder()
{
    mPlayer->stop();
    mList->clear();
    content.clear();
    ui->listWidget->clear();
    dir=new QDir(QFileDialog::getExistingDirectory(this,"Select Folder"));
    vids=dir->entryList(QStringList()<<"*.mp4"<<"*.mov"<<"*.wmv",QDir::Files);
    for(const QString& v:vids){
        content.push_back(QUrl::fromLocalFile(dir->path()+"/"+v));
        QFileInfo fi(v);
        ui->listWidget->addItem(fi.fileName());
    }
    mList->addMedia(content);
    ui->listWidget->setCurrentRow(mList->currentIndex()!=-1 ? mList->currentIndex() : 0);
    counter=new QString("Video Count : ");
    label->setText(counter->append(QString::number(mList->mediaCount())));
    disableControl();
}

void Viewer::on_toolButton_clicked()
{
    if(mList->isEmpty()) return;
    QString filter=ui->filterBox->text();
    mPlayer->stop();
    mList->clear();
    content.clear();
    ui->listWidget->clear();
    vids=dir->entryList(QStringList()<<"*.mp4"<<"*.wmv"<<"*.mov",QDir::Files);
    for(const QString& v:vids.filter(filter)){
        content.push_back(QUrl::fromLocalFile(dir->path()+"/"+v));
        QFileInfo fi(v);
        ui->listWidget->addItem(fi.fileName());
    }
    mList->addMedia(content);
    ui->listWidget->setCurrentRow(mList->currentIndex()!=-1 ? mList->currentIndex() : 0);
    counter=new QString("Video Count : ");
    label->setText(counter->append(QString::number(mList->mediaCount())));
    disableControl();
}

void Viewer::on_frameBox_returnPressed()
{
    mPlayer->setPosition(ui->frameBox->text().toInt()*33);
}

void Viewer::durChanged(QMediaPlayer::MediaStatus status)
{
    if(status==QMediaPlayer::BufferedMedia){
        ui->allFrame->setNum(static_cast<int>(mPlayer->duration())/33);
        ui->navigationBar->setRange(0,mPlayer->duration()/33);
        range=new QIntValidator(0,mPlayer->duration()/33);
        ui->frameBox->setValidator(range);
        all=QTime(mPlayer->duration()/3600000,(mPlayer->duration()/60000)%60,(mPlayer->duration()/1000)%60);
        ui->allTime->setText(all.toString("mm:ss"));
    }
}

void Viewer::on_navigationBar_sliderMoved(int position)
{
    mPlayer->setPosition(position*33);
    setFocus();
}

void Viewer::posChanged(qint64 pos)
{
    if(!ui->navigationBar->isSliderDown()) ui->navigationBar->setValue(pos/33);
    ui->frameBox->setText(QString::number(pos/33,10));
    cur=QTime(pos/3600000,(pos/60000)%60,(pos/1000)%60);
    ui->curTime->setText(cur.toString("mm:ss"));
    setFocus();
}

void Viewer::on_nFrameButton_pressed()
{
    go=1;
}

void Viewer::on_pFrameButton_pressed()
{
    go=-1;
}

void Viewer::frontback()
{
    mPlayer->setPosition(mPlayer->position()+go*33);
}

void Viewer::on_nFrameButton_released()
{
    go=0;
}

void Viewer::on_pFrameButton_released()
{
    go=0;
}

void Viewer::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key()==Qt::Key_Right&&ui->nFrameButton->isEnabled()) go=1;
    else if(ev->key()==Qt::Key_Left&&ui->pFrameButton->isEnabled()) go=-1;
    else if(ev->key()==Qt::Key_Space&&ui->playButton->isEnabled()) on_playButton_clicked();
}

void Viewer::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key()==Qt::Key_Right||ev->key()==Qt::Key_Left) go=0;
}

void Viewer::disableControl()
{
    ui->playButton->setEnabled(0);
    ui->nFrameButton->setEnabled(0);
    ui->pFrameButton->setEnabled(0);
    ui->frameBox->setReadOnly(1);
    ui->navigationBar->setEnabled(0);
    range=new QIntValidator(0,0);
    ui->frameBox->setValidator(range);
    ui->navigationBar->setRange(0,0);
    ui->allFrame->clear();
    ui->normLeft->setEnabled(0);
    ui->normRight->setEnabled(0);
    ui->straRight->setEnabled(0);
    ui->straLeft->setEnabled(0);
    ui->topLeft->setEnabled(0);
    ui->topMid->setEnabled(0);
    ui->topRight->setEnabled(0);
    ui->midLeft->setEnabled(0);
    //ui->midMid->setEnabled(0);
    ui->midRight->setEnabled(0);
    ui->botLeft->setEnabled(0);
    ui->botMid->setEnabled(0);
    ui->botRight->setEnabled(0);
    ui->photoReset->setEnabled(0);
    ui->saveButton->setEnabled(0);
}

void Viewer::enableControl()
{
    ui->playButton->setEnabled(1);
    ui->nFrameButton->setEnabled(1);
    ui->pFrameButton->setEnabled(1);
    ui->frameBox->setReadOnly(0);
    ui->navigationBar->setEnabled(1);
    ui->normLeft->setEnabled(1);
    ui->normRight->setEnabled(1);
    ui->straRight->setEnabled(1);
    ui->straLeft->setEnabled(1);
    ui->topLeft->setEnabled(1);
    ui->topMid->setEnabled(1);
    ui->topRight->setEnabled(1);
    ui->midLeft->setEnabled(1);
    //ui->midMid->setEnabled(1);
    ui->midRight->setEnabled(1);
    ui->botLeft->setEnabled(1);
    ui->botMid->setEnabled(1);
    ui->botRight->setEnabled(1);
    ui->photoReset->setEnabled(1);
    ui->saveButton->setEnabled(1);
}

void Viewer::resetPhoto()
{
    defimg.load(":/image/default_vid.png");
    defimg=defimg.scaled(ui->nine1->width(),ui->nine1->height());
    ui->nine1->setPixmap(defimg);
    //qDebug()<<ui->nine1->width()<<", "<<ui->nine1->height();
    ui->nine2->setPixmap(defimg);
    //qDebug()<<ui->nine2->width()<<", "<<ui->nine2->height();
    ui->nine3->setPixmap(defimg);
    ui->nine4->setPixmap(defimg);
    ui->nine6->setPixmap(defimg);
    ui->nine7->setPixmap(defimg);
    ui->nine8->setPixmap(defimg);
    ui->nine9->setPixmap(defimg);
    for(int i=4;i<12;i++) saveImg[i]=defimg.copy(0,0,defimg.width(),defimg.height());
    defimg=defimg.scaled(ui->pNormRight->width(),ui->pNormRight->height());
    ui->pNormLeft->setPixmap(defimg);
    ui->pNormRight->setPixmap(defimg);
    ui->pAbnoLeft->setPixmap(defimg);
    ui->pAbnoRight->setPixmap(defimg);
    ui->nine5L->setPixmap(defimg);
    ui->nine5R->setPixmap(defimg);
    //qDebug()<<ui->nine5R->width()<<", "<<ui->nine5R->height();
    for(int i=0;i<4;i++) saveImg[i]=defimg.copy(0,0,defimg.width(),defimg.height());
}

void Viewer::processFrame(QImage img)
{
    this->img=img;
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.scaled(ui->vidLabel->width(),ui->vidLabel->height(),Qt::KeepAspectRatio);
    ui->vidLabel->setPixmap(eyeimg);
}

void Viewer::on_topLeft_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    saveImg[4]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine1->width(),ui->nine1->height(),Qt::KeepAspectRatio);
    ui->nine1->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_topMid_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    saveImg[5]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine2->width(),ui->nine2->height(),Qt::KeepAspectRatio);
    ui->nine2->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_topRight_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    saveImg[6]=eyeimg;
    eyeimg=eyeimg.scaled(ui->nine3->width(),ui->nine3->height(),Qt::KeepAspectRatio);
    ui->nine3->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_midLeft_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    saveImg[7]=eyeimg;
    eyeimg=eyeimg.scaled(ui->nine4->width(),ui->nine4->height(),Qt::KeepAspectRatio);
    ui->nine4->setPixmap(eyeimg);
    setFocus();
}
/*
void Viewer::on_midMid_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.scaled(ui->nine5->width(),ui->nine5->height(),Qt::KeepAspectRatio);
    //ui->nine5->setPixmap(eyeimg);
    setFocus();
}
*/
void Viewer::on_midRight_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    saveImg[8]=eyeimg;
    eyeimg=eyeimg.scaled(ui->nine6->width(),ui->nine6->height(),Qt::KeepAspectRatio);
    ui->nine6->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_botLeft_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    saveImg[9]=eyeimg;
    eyeimg=eyeimg.scaled(ui->nine7->width(),ui->nine7->height(),Qt::KeepAspectRatio);
    ui->nine7->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_botMid_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    saveImg[10]=eyeimg;
    eyeimg=eyeimg.scaled(ui->nine8->width(),ui->nine8->height(),Qt::KeepAspectRatio);
    ui->nine8->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_botRight_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    saveImg[11]=eyeimg;
    eyeimg=eyeimg.scaled(ui->nine9->width(),ui->nine9->height(),Qt::KeepAspectRatio);
    ui->nine9->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_normRight_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.copy(0,0,eyeimg.width()/2,eyeimg.height());
    saveImg[0]=eyeimg;
    eyeimg=eyeimg.scaled(ui->pNormRight->width(),ui->pNormRight->height(),Qt::KeepAspectRatio);
    ui->pNormRight->setPixmap(eyeimg);
    ui->nine5R->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_normLeft_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.copy(eyeimg.width()/2+1,0,eyeimg.width()/2,eyeimg.height());
    saveImg[1]=eyeimg;
    eyeimg=eyeimg.scaled(ui->pNormLeft->width(),ui->pNormLeft->height(),Qt::KeepAspectRatio);
    ui->pNormLeft->setPixmap(eyeimg);
    ui->nine5L->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_straRight_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.copy(0,0,eyeimg.width()/2,eyeimg.height());
    saveImg[2]=eyeimg;
    eyeimg=eyeimg.scaled(ui->pAbnoRight->width(),ui->pAbnoRight->height(),Qt::KeepAspectRatio);
    ui->pAbnoRight->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_straLeft_clicked()
{
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.copy(eyeimg.width()/2+1,0,eyeimg.width()/2,eyeimg.height());
    saveImg[3]=eyeimg;
    eyeimg=eyeimg.scaled(ui->pAbnoLeft->width(),ui->pAbnoLeft->height(),Qt::KeepAspectRatio);
    ui->pAbnoLeft->setPixmap(eyeimg);
    setFocus();
}

void Viewer::on_saveButton_clicked()
{
    dir->mkdir("Result");
    QDir::setCurrent(dir->absolutePath()+"/Result");
    for(int i=0;i<12;i++){
        fileImg.setFileName(fileName[i]);
        fileImg.open(QIODevice::WriteOnly);
        saveImg[i].save(&fileImg,"PNG");
        fileImg.close();
    }
}

void Viewer::resized()
{

}

void Viewer::resizeEvent(QResizeEvent *event)
{
    resized();
    QWidget::resizeEvent(event);
}
