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
    ui->actionReset_All_Screenshots->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    ui->actionSave_Screenshots->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
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
    connect(ui->actionReset_All_Screenshots,SIGNAL(triggered()),this,SLOT(resetPhoto()));
    connect(ui->actionSave_Screenshots,SIGNAL(triggered()),this,SLOT(save()));
    connect(framer,SIGNAL(frameAvailable(QImage)),this,SLOT(processFrame(QImage)));
    connect(saver,&Saver::saving,this,&Viewer::saveBar);

    connect(ui->pNormRight,SIGNAL(clicked()),this,SLOT(normRight_clicked()));
    connect(ui->pNormLeft,SIGNAL(clicked()),this,SLOT(normLeft_clicked()));
    connect(ui->pAbnoRight,SIGNAL(clicked()),this,SLOT(straRight_clicked()));
    connect(ui->pAbnoLeft,SIGNAL(clicked()),this,SLOT(straLeft_clicked()));
    connect(ui->nine1,SIGNAL(clicked()),this,SLOT(topLeft_clicked()));
    connect(ui->nine2,SIGNAL(clicked()),this,SLOT(topMid_clicked()));
    connect(ui->nine3,SIGNAL(clicked()),this,SLOT(topRight_clicked()));
    connect(ui->nine4,SIGNAL(clicked()),this,SLOT(midLeft_clicked()));
    connect(ui->nine6,SIGNAL(clicked()),this,SLOT(midRight_clicked()));
    connect(ui->nine7,SIGNAL(clicked()),this,SLOT(botLeft_clicked()));
    connect(ui->nine8,SIGNAL(clicked()),this,SLOT(botMid_clicked()));
    connect(ui->nine9,SIGNAL(clicked()),this,SLOT(botRight_clicked()));

    ui->saveLabel->setVisible(0);
    ui->saveProgress->setVisible(0);
    ui->nineLayout->setAlignment(Qt::AlignCenter);
    defimg.load(":/image/default_full.png");
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
        now=index.data(Qt::DisplayRole).toString();
        now.remove(now.size()-4,4);
        resetPhoto();
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
}

void Viewer::enableControl()
{
    ui->playButton->setEnabled(1);
    ui->nFrameButton->setEnabled(1);
    ui->pFrameButton->setEnabled(1);
    ui->frameBox->setReadOnly(0);
    ui->navigationBar->setEnabled(1);
}

void Viewer::resetPhoto()
{
    memset(capt,false,sizeof(capt));
    defimg.load(":/image/default_full.png");
    defimg=defimg.scaled(ui->nine1->width(),ui->nine1->height(),Qt::KeepAspectRatio);
    ui->nine1->setPixmap(defimg);
    ui->nine2->setPixmap(defimg);
    ui->nine3->setPixmap(defimg);
    ui->nine4->setPixmap(defimg);
    ui->nine6->setPixmap(defimg);
    ui->nine7->setPixmap(defimg);
    ui->nine8->setPixmap(defimg);
    ui->nine9->setPixmap(defimg);
    for(int i=4;i<12;i++) saveImg[i]=defimg.copy(0,0,defimg.width(),defimg.height());
    defimg.load(":/image/default_short.png");
    defimg=defimg.scaled(ui->pNormRight->width(),ui->pNormRight->height(),Qt::KeepAspectRatio);
    ui->pNormLeft->setPixmap(defimg);
    ui->pNormRight->setPixmap(defimg);
    ui->pAbnoLeft->setPixmap(defimg);
    ui->pAbnoRight->setPixmap(defimg);
    ui->nine5L->setPixmap(defimg);
    ui->nine5R->setPixmap(defimg);
    for(int i=0;i<4;i++) saveImg[i]=defimg.copy(0,0,defimg.width(),defimg.height());
    setFocus();
}

void Viewer::processFrame(QImage img)
{
    this->img=img;
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.scaled(ui->vidLabel->width(),ui->vidLabel->height(),Qt::KeepAspectRatio);
    ui->vidLabel->setPixmap(eyeimg);
}

void Viewer::normRight_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.copy(0,0,eyeimg.width()/2,eyeimg.height());
    saveImg[0]=eyeimg;
    eyeimg=eyeimg.scaled(ui->pNormRight->width(),ui->pNormRight->height(),Qt::KeepAspectRatio);
    ui->pNormRight->setPixmap(eyeimg);
    ui->nine5R->setPixmap(eyeimg);
    capt[0]=true;
    setFocus();
}

void Viewer::normLeft_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.copy(eyeimg.width()/2,0,eyeimg.width()/2,eyeimg.height());
    saveImg[1]=eyeimg;
    eyeimg=eyeimg.scaled(ui->pNormLeft->width(),ui->pNormLeft->height(),Qt::KeepAspectRatio);
    ui->pNormLeft->setPixmap(eyeimg);
    ui->nine5L->setPixmap(eyeimg);
    capt[1]=true;
    setFocus();
}

void Viewer::straRight_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.copy(0,0,eyeimg.width()/2,eyeimg.height());
    saveImg[2]=eyeimg;
    eyeimg=eyeimg.scaled(ui->pAbnoRight->width(),ui->pAbnoRight->height(),Qt::KeepAspectRatio);
    ui->pAbnoRight->setPixmap(eyeimg);
    capt[2]=true;
    setFocus();
}

void Viewer::straLeft_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    eyeimg=eyeimg.copy(eyeimg.width()/2,0,eyeimg.width()/2,eyeimg.height());
    saveImg[3]=eyeimg;
    eyeimg=eyeimg.scaled(ui->pAbnoLeft->width(),ui->pAbnoLeft->height(),Qt::KeepAspectRatio);
    ui->pAbnoLeft->setPixmap(eyeimg);
    capt[3]=true;
    setFocus();
}

void Viewer::topLeft_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    saveImg[4]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine1->width(),ui->nine1->height(),Qt::KeepAspectRatio);
    ui->nine1->setPixmap(eyeimg);
    capt[4]=true;
    setFocus();
}

void Viewer::topMid_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    saveImg[5]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine2->width(),ui->nine2->height(),Qt::KeepAspectRatio);
    ui->nine2->setPixmap(eyeimg);
    capt[5]=true;
    setFocus();
}

void Viewer::topRight_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    saveImg[6]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine3->width(),ui->nine3->height(),Qt::KeepAspectRatio);
    ui->nine3->setPixmap(eyeimg);
    capt[6]=true;
    setFocus();
}

void Viewer::midLeft_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    saveImg[7]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine4->width(),ui->nine4->height(),Qt::KeepAspectRatio);
    ui->nine4->setPixmap(eyeimg);
    capt[7]=true;
    setFocus();
}

void Viewer::midRight_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    saveImg[8]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine6->width(),ui->nine6->height(),Qt::KeepAspectRatio);
    ui->nine6->setPixmap(eyeimg);
    capt[8]=true;
    setFocus();
}

void Viewer::botLeft_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    saveImg[9]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine7->width(),ui->nine7->height(),Qt::KeepAspectRatio);
    ui->nine7->setPixmap(eyeimg);
    capt[9]=true;
    setFocus();
}

void Viewer::botMid_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    saveImg[10]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine8->width(),ui->nine8->height(),Qt::KeepAspectRatio);
    ui->nine8->setPixmap(eyeimg);
    capt[10]=true;
    setFocus();
}

void Viewer::botRight_clicked()
{
    if(img.isNull()) return;
    eyeimg=QPixmap::fromImage(img);
    saveImg[11]=eyeimg.copy(0,0,eyeimg.width(),eyeimg.height());
    eyeimg=eyeimg.scaled(ui->nine9->width(),ui->nine9->height(),Qt::KeepAspectRatio);
    ui->nine9->setPixmap(eyeimg);
    capt[11]=true;
    setFocus();
}

void Viewer::save()
{
    ui->saveLabel->setVisible(1);
    ui->saveProgress->setVisible(1);
    ui->saveProgress->setValue(0);
    max=1;
    for(int i=0;i<12;i++){
        if(capt[i]) max++;
    }
    if(max==1) return;
    ui->saveLabel->setText("Saving "+QString::number(1)+"/"+QString::number(max)+" Images");
    ui->saveProgress->setMaximum(max);
    dir->mkdir(now);
    QDir::setCurrent(dir->absolutePath()+"/"+now);
    saver->setParam(max,capt,saveImg);
    saver->start();
    setFocus();
}

void Viewer::resized()
{
    if(img.isNull()) eyeimg.load(":/image/default_full.png");
    else eyeimg=QPixmap::fromImage(img);
    ui->vidLabel->setFixedSize(ui->navigationBar->width(),ui->navigationBar->width()*9/32);
    eyeimg=eyeimg.scaled(ui->vidLabel->width(),ui->vidLabel->height(),Qt::KeepAspectRatio);
    ui->vidLabel->setPixmap(eyeimg);
    sh=ui->vidLabel->height()/4<ui->vidLabel->width()*9/144?ui->vidLabel->height()/4:ui->vidLabel->width()*9/144;
    ui->pNormRight->setFixedSize(sh*16/9,sh);
    ui->pNormLeft->setFixedSize(sh*16/9,sh);
    ui->pAbnoRight->setFixedSize(sh*16/9,sh);
    ui->pAbnoLeft->setFixedSize(sh*16/9,sh);
    ui->nine5R->setFixedSize(sh*16/9,sh);
    ui->nine5L->setFixedSize(sh*16/9,sh);
    ui->nine1->setFixedSize(sh*32/9,sh);
    ui->nine2->setFixedSize(sh*32/9,sh);
    ui->nine3->setFixedSize(sh*32/9,sh);
    ui->nine4->setFixedSize(sh*32/9,sh);
    ui->nine6->setFixedSize(sh*32/9,sh);
    ui->nine7->setFixedSize(sh*32/9,sh);
    ui->nine8->setFixedSize(sh*32/9,sh);
    ui->nine9->setFixedSize(sh*32/9,sh);
    eyeimg=saveImg[0];
    eyeimg=eyeimg.scaled(ui->pNormRight->width(),ui->pNormRight->height(),Qt::KeepAspectRatio);
    ui->pNormRight->setPixmap(eyeimg);
    eyeimg=eyeimg.scaled(ui->nine5R->width(),ui->nine5R->height(),Qt::KeepAspectRatio);
    ui->nine5R->setPixmap(eyeimg);
    eyeimg=saveImg[1];
    eyeimg=eyeimg.scaled(ui->pNormLeft->width(),ui->pNormLeft->height(),Qt::KeepAspectRatio);
    ui->pNormLeft->setPixmap(eyeimg);
    eyeimg=eyeimg.scaled(ui->nine5L->width(),ui->nine5L->height(),Qt::KeepAspectRatio);
    ui->nine5L->setPixmap(eyeimg);
    eyeimg=saveImg[2];
    eyeimg=eyeimg.scaled(ui->pAbnoRight->width(),ui->pAbnoRight->height(),Qt::KeepAspectRatio);
    ui->pAbnoRight->setPixmap(eyeimg);
    eyeimg=saveImg[3];
    eyeimg=eyeimg.scaled(ui->pAbnoLeft->width(),ui->pAbnoLeft->height(),Qt::KeepAspectRatio);
    ui->pAbnoLeft->setPixmap(eyeimg);
    eyeimg=saveImg[4];
    eyeimg=eyeimg.scaled(ui->nine1->width(),ui->nine1->height(),Qt::KeepAspectRatio);
    ui->nine1->setPixmap(eyeimg);
    eyeimg=saveImg[5];
    eyeimg=eyeimg.scaled(ui->nine2->width(),ui->nine2->height(),Qt::KeepAspectRatio);
    ui->nine2->setPixmap(eyeimg);
    eyeimg=saveImg[6];
    eyeimg=eyeimg.scaled(ui->nine3->width(),ui->nine3->height(),Qt::KeepAspectRatio);
    ui->nine3->setPixmap(eyeimg);
    eyeimg=saveImg[7];
    eyeimg=eyeimg.scaled(ui->nine4->width(),ui->nine4->height(),Qt::KeepAspectRatio);
    ui->nine4->setPixmap(eyeimg);
    eyeimg=saveImg[8];
    eyeimg=eyeimg.scaled(ui->nine6->width(),ui->nine6->height(),Qt::KeepAspectRatio);
    ui->nine6->setPixmap(eyeimg);
    eyeimg=saveImg[9];
    eyeimg=eyeimg.scaled(ui->nine7->width(),ui->nine7->height(),Qt::KeepAspectRatio);
    ui->nine7->setPixmap(eyeimg);
    eyeimg=saveImg[10];
    eyeimg=eyeimg.scaled(ui->nine8->width(),ui->nine8->height(),Qt::KeepAspectRatio);
    ui->nine8->setPixmap(eyeimg);
    eyeimg=saveImg[11];
    eyeimg=eyeimg.scaled(ui->nine9->width(),ui->nine9->height(),Qt::KeepAspectRatio);
    ui->nine9->setPixmap(eyeimg);
    ui->normal->setFixedWidth(ui->pNormRight->width());
    ui->abnormal->setFixedWidth(ui->pNormLeft->width());
    setFocus();
}

void Viewer::resizeEvent(QResizeEvent *event)
{
    resized();
    QWidget::resizeEvent(event);
}

void Viewer::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::WindowStateChange) resized();
}

void Viewer::saveBar(int i1)
{
    ui->saveProgress->setValue(i1);
    if(i1<max+1){
        ui->saveLabel->setText("Saving "+QString::number(i1+1)+"/"+QString::number(max)+" Images");
    }
    else{
        ui->saveLabel->setText("Save Complete");
        ui->saveProgress->setVisible(0);
    }
}
