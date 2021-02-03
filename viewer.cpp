#include "viewer.h"
#include "ui_viewer.h"

Viewer::Viewer(QWidget *parent)
    : QMainWindow(parent)
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

    connect(ui->listWidget,&QAbstractItemView::activated,this,&Viewer::jump);
    connect(mPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(onPositionChanged(qint64)));
    connect(ui->actionSelect_Folder,SIGNAL(triggered()),this,SLOT(chkFolder()));
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
    printf("n");
}

void Viewer::on_pFrameButton_clicked()
{
    printf("p");
}

void Viewer::jump(const QModelIndex &index)
{
    if(index.isValid()){
        mList->setCurrentIndex(index.row());
        mPlayer->play();
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
}

void Viewer::chkFolder()
{
    mPlayer->stop();
    mList->clear();
    ui->listWidget->clear();
    dir=new QDir(QFileDialog::getExistingDirectory(this,"Select Folder",QDir::currentPath(),QFileDialog::ShowDirsOnly));
    vids=dir->entryList(QStringList()<<"*.mp4",QDir::Files);
    for(const QString& v:vids){
        content.push_back(QUrl::fromLocalFile(dir->path()+"/"+v));
        QFileInfo fi(v);
        ui->listWidget->addItem(fi.fileName());
    }
    mList->addMedia(content);
    ui->listWidget->setCurrentRow(mList->currentIndex()!=-1 ? mList->currentIndex() : 0);
}
