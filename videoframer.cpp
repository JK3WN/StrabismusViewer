#include "videoframer.h"
#include <QtWidgets>
#include <qabstractvideosurface.h>
#include <qvideosurfaceformat.h>

VideoFramer::VideoFramer(QWidget *widget,QObject *parent)
    : QAbstractVideoSurface(parent)
    , widget(widget)
    , imageFormat(QImage::Format_Invalid)
{
}

QList<QVideoFrame::PixelFormat> VideoFramer::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()
            <<QVideoFrame::Format_RGB32;
}

bool VideoFramer::isFormatSupported(const QVideoSurfaceFormat &format) const
{
    const QImage::Format imageFormat=QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size=format.frameSize();
    return imageFormat!=QImage::Format_Invalid&&!size.isEmpty()&&format.handleType()==QAbstractVideoBuffer::NoHandle;
}

bool VideoFramer::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format imageFormat=QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size=format.frameSize();
    if(imageFormat!=QImage::Format_Invalid&&!size.isEmpty()){
        this->imageFormat=imageFormat;
        imageSize=size;
        sourceRect=format.viewport();
        QAbstractVideoSurface::start(format);
        widget->updateGeometry();
        updateVideoRect();
        return true;
    }
    else return false;
}

void VideoFramer::stop()
{
    currentFrame = QVideoFrame();
    targetRect = QRect();

    QAbstractVideoSurface::stop();

    widget->update();
}

bool VideoFramer::present(const QVideoFrame &frame)
{
    if (frame.isValid())
    {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        const QImage image(cloneFrame.bits(),
                           cloneFrame.width(),
                           cloneFrame.height(),
                           QVideoFrame::imageFormatFromPixelFormat(cloneFrame .pixelFormat()));
        emit frameAvailable(image); // this is very important
        cloneFrame.unmap();
    }

    if (surfaceFormat().pixelFormat() != frame.pixelFormat()
            || surfaceFormat().frameSize() != frame.size()) {
        setError(IncorrectFormatError);
        stop();

        return false;
    } else {
        currentFrame = frame;

        widget->repaint(targetRect);

        return true;
    }
}

void VideoFramer::updateVideoRect()
{
    QSize size = surfaceFormat().sizeHint();
    size.scale(widget->size().boundedTo(size), Qt::KeepAspectRatio);

    targetRect = QRect(QPoint(0, 0), size);
    targetRect.moveCenter(widget->rect().center());
}

void VideoFramer::paint(QPainter *painter)
{
    if (currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {
        const QTransform oldTransform = painter->transform();

        if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
           painter->scale(1, -1);
           painter->translate(0, -widget->height());
        }

        QImage image(
                currentFrame.bits(),
                currentFrame.width(),
                currentFrame.height(),
                currentFrame.bytesPerLine(),
                imageFormat);

        painter->drawImage(targetRect, image, sourceRect);

        painter->setTransform(oldTransform);

        currentFrame.unmap();
    }
}
