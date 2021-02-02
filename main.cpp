#include "viewer.h"
#include <QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Viewer w;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));
    QCoreApplication::setApplicationName("Strabismus Viewer");
    w.setFixedSize(550,330);
    w.show();
    return a.exec();
}
