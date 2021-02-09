#include "viewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS","windowsmediafoundation");
    QApplication a(argc, argv);
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS","windowsmediafoundation");
    QCoreApplication::setApplicationName("Strabismus Viewer");
    Viewer w;
    //w.setFixedSize(1090,626);
    w.setWindowState(Qt::WindowMaximized);
    w.show();
    return a.exec();
}
