#ifndef VIEWER_H
#define VIEWER_H

#include <QMainWindow>
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
#include <QVideoProbe>
#include <QTimer>
#include <QKeyEvent>
#include <QTime>

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

private:
    Ui::Viewer *ui;
    QMediaPlayer *mPlayer;
    QMediaPlaylist *mList;
    QDir *dir;
    QStringList vids;
    QList<QMediaContent> content;
    QFile *def;
    QTextStream *defloc;
    QLabel *label;
    QString *counter;
    QIntValidator *range;
    QTimer *timer;
    int go=0;
    QTime cur,all;
    QPixmap defimg;
};
#endif // VIEWER_H
