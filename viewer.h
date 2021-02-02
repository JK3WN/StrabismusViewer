#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QDir>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class Viewer; }
QT_END_NAMESPACE

class Viewer : public QWidget
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = nullptr);
    QUrl url;
    ~Viewer();

private slots:
    void on_playButton_clicked();
    void on_nFrameButton_clicked();
    void on_pFrameButton_clicked();
    void jump(const QModelIndex &index);

private:
    Ui::Viewer *ui;
    QMediaPlayer *mPlayer;
    QMediaPlaylist *mList;
    QDir *dir;
    QString StrToString(char *str);
};
#endif // VIEWER_H
