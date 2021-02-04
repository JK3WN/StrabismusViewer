#include "viewer.h"
#include "ui_viewer.h"

Viewer::Viewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Viewer)
{
    ui->setupUi(this);
    def=new QFile("defdir.txt");
    mPlayer=new QMediaPlayer(this,QMediaPlayer::VideoSurface);
    mList=new QMediaPlaylist();
    label=new QLabel;
    counter=new QString("Video Count : ");

    mList->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    mPlayer->setPlaylist(mList);
    mPlayer->setVideoOutput(ui->vidPlayer);
    ui->playButton->setEnabled(0);
    ui->frameBox->setReadOnly(1);
    range=new QIntValidator(0,0);
    ui->frameBox->setValidator(range);

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->nFrameButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->pFrameButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->navigationBar->setRange(0,0);
    ui->statusbar->addWidget(label);
    label->setText(counter->append(QString::number(mList->mediaCount())));

    connect(ui->listWidget,&QAbstractItemView::activated,this,&Viewer::jump);
    connect(mPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(onPositionChanged(qint64)));
    connect(ui->actionSelect_Folder,SIGNAL(triggered()),this,SLOT(chkFolder()));
    //connect(mPlayer,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(durChanged(QMediaPlayer::MediaStatus)));
    connect(mPlayer,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(durChanged(QMediaPlayer::MediaStatus)));
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
        ui->frameBox->setValidator(range);
        ui->playButton->setEnabled(1);
        ui->frameBox->setReadOnly(0);
    }
}

void Viewer::chkFolder()
{
    mPlayer->stop();
    mList->clear();
    content.clear();
    ui->listWidget->clear();
    dir=new QDir(QFileDialog::getExistingDirectory(this,"Select Folder"));
    vids=dir->entryList(QStringList()<<"*.mp4",QDir::Files);
    for(const QString& v:vids){
        content.push_back(QUrl::fromLocalFile(dir->path()+"/"+v));
        QFileInfo fi(v);
        ui->listWidget->addItem(fi.fileName());
    }
    mList->addMedia(content);
    ui->listWidget->setCurrentRow(mList->currentIndex()!=-1 ? mList->currentIndex() : 0);
    counter=new QString("Video Count : ");
    label->setText(counter->append(QString::number(mList->mediaCount())));
    ui->playButton->setEnabled(0);
    ui->frameBox->setReadOnly(1);
}

void Viewer::on_toolButton_clicked()
{
    printf("search");
}

void Viewer::on_frameBox_returnPressed()
{
    mPlayer->setPosition(ui->frameBox->text().toInt());
}

void Viewer::durChanged(QMediaPlayer::MediaStatus status)
{
    if(status==QMediaPlayer::BufferedMedia){
        qDebug()<<"Dur "<<mPlayer->duration();
        ui->allFrame->setNum(static_cast<int>(mPlayer->duration()));
        ui->navigationBar->setRange(0,mPlayer->duration());
        range=new QIntValidator(0,mPlayer->duration());
        ui->frameBox->setValidator(range);
    }
}
