#include "qOpenGLCVWidget.h"
#include "qOpenGLCVWidget.moc"
#include <QDateTime>

CQtOpenCVViewerGl::CQtOpenCVViewerGl(QWidget *parent) :
    QGLWidget(parent)
{
     mBgColor = QColor::fromRgb(0, 0, 150);
     lastImageTime = QDateTime::currentMSecsSinceEpoch();
}

void CQtOpenCVViewerGl::initializeGL()
{
    makeCurrent();
    //Adjust the viewport
    glViewport(0,0,this->width(), this->height());

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //Adjust the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-this->width()/2, this->width()/2, this->height()/2, -this->height()/2, -1, 1);	
    qglClearColor(mBgColor.darker());
//     glShadeModel(GL_FLAT);
//     glGenTextures(1, &texName);
//     glBindTexture(GL_TEXTURE_2D, texName);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void CQtOpenCVViewerGl::resizeGL(int width, int height)
{
    makeCurrent();
    //Adjust the viewport
    glViewport(0,0,this->width(), this->height());
    
    //Adjust the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-this->width()/2, this->width()/2, this->height()/2, -this->height()/2, -1, 1);	
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glTranslatef(0.0,0.0,-1.0);
    qglClearColor(mBgColor.darker());
}

void CQtOpenCVViewerGl::paintGL()
{
	float wzoom, hzoom, zoom;
	wzoom = (float)(this->width()) / (float)(mOrigImage.size().width);
	hzoom = (float)(this->height()) / (float)(mOrigImage.size().height);
	if(wzoom > hzoom)
	  zoom = hzoom;
	else
	  zoom = wzoom;
	//Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//Set the raster position
	glRasterPos2i(-this->width()/2,-this->height()/2);
	
	//Invert the image (the data coming from OpenCV is inverted)
	glPixelZoom(zoom,-zoom);
	

	//Draw image from OpenCV capture
	bufferMutex.lock();
	if(mOrigImage.channels()==3)
	  glDrawPixels(mOrigImage.size().width, mOrigImage.size().height, GL_BGR, GL_UNSIGNED_BYTE,mOrigImage.ptr());		
	else
	  glDrawPixels(mOrigImage.size().width, mOrigImage.size().height, GL_LUMINANCE, GL_UNSIGNED_BYTE,mOrigImage.ptr());		
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mOrigImage.size().width, mOrigImage.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, mOrigImage.ptr());
	bufferMutex.unlock();
// 	glEnable(GL_TEXTURE_2D);
// 	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
// 	glBindTexture(GL_TEXTURE_2D, texName);
// 	glBegin(GL_QUADS);
// 	  glTexCoord2f(0.0, 0.0); glVertex3f(0.0,0.0,0.0);
// 	  glTexCoord2f(0.0, 1.0); glVertex3f(0.0,1.0,0.0);
// 	  glTexCoord2f(1.0, 1.0); glVertex3f(1.0,1.0,0.0);
// 	  glTexCoord2f(1.0, 0.0); glVertex3f(1.0,0.0,0.0);
// 	glEnd();
// 	glFlush();
// 	glDisable(GL_TEXTURE_2D);
}

bool CQtOpenCVViewerGl::showImage( cv::Mat image, qint64 timestampus )
{
    qint64 temp = QDateTime::currentMSecsSinceEpoch();
    if(temp > lastImageTime + 50)
	lastImageTime = temp;
      
    updateBuffer(image,timestampus);
    glDraw();
    return true;
}

void CQtOpenCVViewerGl::updateBuffer(const cv::Mat_< uint8_t >& image, qint64 timestamp)
{
	bufferMutex.lock();
	image.copyTo(mOrigImage);
	bufferMutex.unlock();
}


void CQtOpenCVViewerGl::addAlpha(const cv::Mat_<uint8_t> &alphaChannel)
{

  cv::Mat_<uint8_t> alphaCopy = alphaChannel.clone();
    //cv::Mat_<uint8_t> matArray[] = {alphaChannel,alphaChannel,alphaChannel};
    bufferMutex.lock();

    //cv::Mat threeChannelImg;
    //cv::merge(matArray,3,threeChannelImg);
    cv::Size size= mOrigImage.size();
    if (size.area()!=0)
    {
      cv::resize(alphaCopy,alphaCopy,mOrigImage.size());
      //cv::mixChannels(&alphaChannel,1,&threeChannelImg,1,fromto,3);
      
      //cv::Mat secondOp(threeChannelImg.size(),threeChannelImg.depth());
      //assert(mOrigImage.size()==threeChannelImg.size());
      //assert(mOrigImage.depth()==threeChannelImg.depth());
      //int channel1=mOrigImage.channels();
      //int channel2 = threeChannelImg.channels();
      //assert(mOrigImage.channels()==threeChannelImg.channels());
      //mOrigImage = firstOp+secondOp;
      //mOrigImage = firstOp
      cv::addWeighted(mOrigImage,0.5,alphaCopy,0.5,0,mOrigImage);
    }
    bufferMutex.unlock();
    glDraw();    
}

bool CQtOpenCVViewerGl::showImage(cv::Mat_< uint8_t > image, qint64 timestamp)
{
  showImage((cv::Mat)image,timestamp);

}

void CQtOpenCVViewerGl::updateBuffer(const cv::Mat& image, qint64 timestamp)
{
  cv::Mat_<uint8_t> mat;
  image.convertTo(mat,CV_8UC1);
  updateBuffer(mat,timestamp);
}

