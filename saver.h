#ifndef SAVER_H
#define SAVER_H

#include <QThread>
#include <QPainter>
#include <QFile>

class Saver : public QThread
{
    Q_OBJECT

public:
    Saver();
    ~Saver();
    void setParam(int max,bool capt[12],QPixmap saveImg[12]);

protected:
    void run() override;

signals:
    void saving(int i1);

private:
    QStringList fileName={"NormalRight.png","NormalLeft.png","AbnormalRight.png","AbnormalLeft.png","TopLeft.png","TopMiddle.png","TopRight.png","MiddleLeft.png","MiddleRight.png","BottomLeft.png","BottomMiddle.png","BottomRight.png"};
    QPainter painter;
    QFile fileImg;
    bool capt[12];
    QPixmap saveImg[12];
    int max;
};

#endif // SAVER_H
