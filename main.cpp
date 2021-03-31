#include "viewer.h"
#include <QSplashScreen>
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication a(argc, argv);
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS","windowsmediafoundation");
    QCoreApplication::setApplicationName("Nine Cardinal Video Capturer");
    QCoreApplication::setOrganizationName("HVRC");
    QPixmap pixmap(":/image/SplashImage.png");
    QSplashScreen splash(pixmap.scaled(960,540,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    splash.show();
    QTimer::singleShot(1000,&splash,&QWidget::close);
    Viewer w;
    QTimer::singleShot(1100,&w,SLOT(show()));
    w.show();
    return a.exec();
}
