#include "viewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Strabismus Viewer");
    Viewer w;
    //w.setFixedSize(1280,720);
    w.show();
    return a.exec();
}
