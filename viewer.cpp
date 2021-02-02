#include "viewer.h"
#include "ui_viewer.h"

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Viewer)
{
    ui->setupUi(this);
    mPlayer=new QMediaPlayer(this,QMediaPlayer::VideoSurface);
    mList=new QMediaPlaylist();
    mPlayer->setPlaylist(mList);
    mPlayer->setVideoOutput(ui->vidPlayer);

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->nFrameButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->pFrameButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->navigationBar->setRange(0,0);

    connect(mPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(onPositionChanged(qint64)));

    dir=new QDir("C:/Users/user/Desktop/");
    QStringList vids=dir->entryList(QStringList()<<"*.mp4",QDir::Files);
    QList<QMediaContent> content;
    for(const QString& v:vids){
        content.push_back(QUrl::fromLocalFile(dir->path()+"/"+v));
        QFileInfo fi(v);
        ui->listWidget->addItem(fi.fileName());
    }
    mList->addMedia(content);
    ui->listWidget->setCurrentRow(mList->currentIndex()!=-1 ? mList->currentIndex() : 0);
    /*
    dir->setNameFilters(QStringList()<<"*.mp4");
    foreach(QFileInfo var,dir->entryInfoList()){
        ui->listWidget->addItem(var.fileName());
    }
    */
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

void Viewer::on_nFrameButton_clicked()
{
    printf("b");
}

void Viewer::on_pFrameButton_clicked()
{
    printf("c");
}

void Viewer::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    printf("qwer");
}
