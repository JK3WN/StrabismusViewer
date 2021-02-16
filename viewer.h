#ifndef VIEWER_H
#define VIEWER_H

#include <QMainWindow>
#include <QtMultimediaWidgets>
#include <QWidget>
#include <QDir>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QTimer>
#include <QKeyEvent>
#include <QTime>
#include "videoframer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Viewer; }
QT_END_NAMESPACE

class Viewer : public QMainWindow
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = nullptr);
    ~Viewer();

private slots:
    void on_playButton_clicked();
    void jump(const QModelIndex &index);
    void chkFolder();
    void on_toolButton_clicked();
    void on_frameBox_returnPressed();
    void durChanged(QMediaPlayer::MediaStatus status);
    void on_navigationBar_sliderMoved(int position);
    void posChanged(qint64 pos);
    void on_nFrameButton_pressed();
    void on_pFrameButton_pressed();
    void frontback();
    void on_nFrameButton_released();
    void on_pFrameButton_released();
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);
    void disableControl();
    void enableControl();
    void resetPhoto();
    void on_topLeft_clicked();
    void processFrame(QImage img);
    void on_topMid_clicked();
    void on_topRight_clicked();
    void on_midLeft_clicked();
    //void on_midMid_clicked();
    void on_midRight_clicked();
    void on_botLeft_clicked();
    void on_botMid_clicked();
    void on_botRight_clicked();
    void on_normRight_clicked();
    void on_normLeft_clicked();
    void on_straRight_clicked();
    void on_straLeft_clicked();
    void on_saveButton_clicked();

private:
    Ui::Viewer *ui;
    QMediaPlayer *mPlayer;
    QMediaPlaylist *mList;
    QDir *dir;
    QStringList vids;
    QList<QMediaContent> content;
    QTextStream *defloc;
    QLabel *label;
    QString *counter;
    QIntValidator *range;
    QTimer *timer;
    int go=0;
    QTime cur,all;
    QPixmap defimg,eyeimg,saveImg[12];
    QImage img;
    VideoFramer *framer=new VideoFramer(this);
    QFile fileImg;
    QStringList fileName={"NormalRight.png","NormalLeft.png","AbnormalRight.png","AbnormalLeft.png","TopLeft.png","TopMiddle.png","TopRight.png","MiddleLeft.png","MiddleRight.png","BottomLeft.png","BottomMiddle.png","BottomRight.png"};
};
#endif // VIEWER_H
