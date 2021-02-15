#ifndef VIDEOFRAMER_H
#define VIDEOFRAMER_H
#include <QtWidgets>
#include <QAbstractVideoSurface>

class VideoFramer : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    VideoFramer(QWidget *widget,QObject *parent=0);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    bool isFormatSupported(const QVideoSurfaceFormat &format) const;
    bool start(const QVideoSurfaceFormat &format);
    void stop();
    bool present(const QVideoFrame &frame);
    QRect videoRect() const {return targetRect;}
    void updateVideoRect();
    void paint(QPainter *painter);

private:
    QWidget *widget;
    QImage::Format imageFormat;
    QRect targetRect,sourceRect;
    QSize imageSize;
    QVideoFrame currentFrame;

signals:
    void frameAvailable(QImage frame);
};

#endif // VIDEOFRAMER_H
