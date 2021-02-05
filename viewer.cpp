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
    mPlayer->setNotifyInterval(33);
    ui->playButton->setEnabled(0);
    ui->frameBox->setReadOnly(1);
    range=new QIntValidator(0,0);
    ui->frameBox->setValidator(range);
    ui->filterBox->setPlaceholderText("Search...");

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->nFrameButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->pFrameButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->navigationBar->setRange(0,0);
    ui->statusbar->addWidget(label);
    label->setText(counter->append(QString::number(mList->mediaCount())));

    connect(ui->listWidget,&QAbstractItemView::activated,this,&Viewer::jump);
    //connect(mPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(onPositionChanged(qint64)));
    connect(ui->actionSelect_Folder,SIGNAL(triggered()),this,SLOT(chkFolder()));
    connect(mPlayer,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(durChanged(QMediaPlayer::MediaStatus)));
    connect(mPlayer,&QMediaPlayer::positionChanged,this,&Viewer::posChanged);
    connect(ui->filterBox,SIGNAL(returnPressed()),this,SLOT(on_toolButton_clicked()));
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
    QString filter=ui->filterBox->text();
    mPlayer->stop();
    mList->clear();
    content.clear();
    ui->listWidget->clear();
    vids=dir->entryList(QStringList()<<"*.mp4",QDir::Files);
    for(const QString& v:vids.filter(filter)){
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
    }
}

void Viewer::on_navigationBar_sliderMoved(int position)
{
    mPlayer->setPosition(position*33);
}

void Viewer::posChanged(qint64 pos)
{
    if(!ui->navigationBar->isSliderDown()) ui->navigationBar->setValue(pos/33);
    ui->frameBox->setText(QString::number(pos/33,10));
}

void Viewer::on_nFrameButton_pressed()
{
    mPlayer->setPosition(mPlayer->position()+33);
}

void Viewer::on_pFrameButton_pressed()
{
    mPlayer->setPosition(mPlayer->position()-33);
}
