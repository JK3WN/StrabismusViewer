#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QDir>
#include <QMessageBox>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimediaWidgets/QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Viewer; }
QT_END_NAMESPACE

class Viewer : public QWidget
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = nullptr);
    ~Viewer();

private:
    Ui::Viewer *ui;
};
#endif // VIEWER_H
