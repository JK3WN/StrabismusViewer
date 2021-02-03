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
    void on_nFrameButton_clicked();
    void on_pFrameButton_clicked();
    void jump(const QModelIndex &index);
    void chkFolder();

private:
    Ui::Viewer *ui;
    QMediaPlayer *mPlayer;
    QMediaPlaylist *mList;
    QDir *dir;
    QStringList vids;
    QList<QMediaContent> content;
    QFile *def;
    QTextStream *defloc;
};
#endif // VIEWER_H
