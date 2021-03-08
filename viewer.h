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
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "videoframer.h"
#include "saver.h"
#include "clickablelabel.h"

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
    void processFrame(QImage img);
    void normRight_clicked();
    void normLeft_clicked();
    void straRight_clicked();
    void straLeft_clicked();
    void topLeft_clicked();
    void topMid_clicked();
    void topRight_clicked();
    void midLeft_clicked();
    void midRight_clicked();
    void botLeft_clicked();
    void botMid_clicked();
    void botRight_clicked();
    void save();
    void resized();
    void saveBar(int i1);

protected:
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *event);

private:
    Ui::Viewer *ui;
    QMediaPlayer *mPlayer;
    QMediaPlaylist *mList;
    QDir *dir;
    QStringList vids;
    QList<QMediaContent> content;
    QTextStream *defloc;
    QLabel *label;
    QString *counter,now;
    QIntValidator *range;
    QTimer *timer;
    int go=0,sh=90, max=1;
    QTime cur,all;
    QPixmap defimg,eyeimg,saveImg[12],vidimg;
    QImage img;
    VideoFramer *framer=new VideoFramer(this);
    Saver *saver=new Saver();
    QFile fileImg;
    bool capt[12]={false};
    QPainter painter;
    QStringList fileName={"NormalRight.png","NormalLeft.png","AbnormalRight.png","AbnormalLeft.png","TopLeft.png","TopMiddle.png","TopRight.png","MiddleLeft.png","MiddleRight.png","BottomLeft.png","BottomMiddle.png","BottomRight.png"};
<<<<<<< Updated upstream
    QProcess *process=new QProcess(this);
=======
>>>>>>> Stashed changes
};
#endif // VIEWER_H
