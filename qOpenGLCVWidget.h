#ifndef QOPENGLCVWIDGET_H
#define QOPENGLCVWIDGET_H

#include <QGLWidget>
#include <QThread>
#include <QMutex>

#include <opencv2/opencv.hpp>

class CQtOpenCVViewerGl : public QGLWidget
{
    Q_OBJECT
public:
    explicit CQtOpenCVViewerGl(QWidget *parent = 0);

public slots:
    bool    showImage( cv::Mat image, qint64 timestamp =0 ); /// Used to set the image to be viewed
    bool    showImage( cv::Mat_<uint8_t> image, qint64 timestamp =0 ); /// Used to set the image to be viewed
    void    addAlpha(const cv::Mat_<uint8_t> &alphaChannel, qint64 timestamp =0);
    void    updateBuffer(const cv::Mat_<uint8_t> &image, qint64 timestamp =0 );
    void    updateBuffer(const cv::Mat_<uint16_t> &image, qint64 timestamp =0 );
    void    updateBuffer(const cv::Mat &image, qint64 timestamp =0 );
    void    updateOverlayBuffer(const cv::Mat_<uint8_t> &image, qint64 timestamp =0);
protected:
    void 	initializeGL(); /// OpenGL initialization
    void 	paintGL(); /// OpenGL Rendering
    void 	resizeGL(int width, int height);        /// Widget Resize Event

private:
    cv::Mat     mOrigImage, m_overlayImage,m_buffer;             /// original OpenCV image to be shown
    QColor 	mBgColor;		/// Background color
    QMutex m_bufferMutex,m_overlayMutex;
    GLuint texName;
    qint64 lastImageTime;
    qint64 m_currBufferImageTime; // timestamp that indicate the ID of the current image in buffer
    qint64 m_currOverlayImageTime; // timestamp that indicate the ID of the current image in buffer
    void redraw();
    void mixImages();
    void convert16to8bit(cv::InputArray in, cv::OutputArray out);
};

#endif // QOPENGLCVWIDGET_H
