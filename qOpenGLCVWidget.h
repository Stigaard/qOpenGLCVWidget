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
protected:
    void 	initializeGL(); /// OpenGL initialization
    void 	paintGL(); /// OpenGL Rendering
    void 	resizeGL(int width, int height);        /// Widget Resize Event

private:
    cv::Mat     mOrigImage;             /// original OpenCV image to be shown
    QColor 	mBgColor;		/// Background color
    QMutex bufferMutex;
    GLuint texName;
    qint64 lastImageTime;
  
};

#endif // QOPENGLCVWIDGET_H
