#include "viewer.h"
#include "ui_viewer.h"

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Viewer)
{
    ui->setupUi(this);
    mPlayer=new QMediaPlayer(this,QMediaPlayer::VideoSurface);

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->nFrameButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->pFrameButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->navigationBar->setRange(0,0);

    mPlayer->setVideoOutput(ui->vidPlayer);

    QDir dir("C:/Users/user/Desktop");
    dir.setNameFilters(QStringList()<<"*.mp4");
    foreach(QFileInfo var,dir.entryInfoList()){
        ui->listWidget->addItem(var.fileName());
    }
}

Viewer::~Viewer()
{
    delete ui;
}
void Viewer::on_playButton_clicked()
{
    printf("a");
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

}
