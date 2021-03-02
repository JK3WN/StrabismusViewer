#include "viewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication a(argc, argv);
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS","windowsmediafoundation");
    /*
    QFont f=QApplication::font();
    f.setStyleStrategy(QFont::PreferAntialias);
    QApplication::setFont(f);
    */
    QCoreApplication::setApplicationName("Strabismus Viewer");
    Viewer w;
    w.show();
    return a.exec();
}
