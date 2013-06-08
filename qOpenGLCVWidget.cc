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
	m_bufferMutex.lock();
	if(mOrigImage.channels()==3)
	    glDrawPixels(mOrigImage.size().width, mOrigImage.size().height, GL_BGR, GL_UNSIGNED_BYTE,mOrigImage.ptr());
	else
	    glDrawPixels(mOrigImage.size().width, mOrigImage.size().height, GL_LUMINANCE, GL_UNSIGNED_BYTE,mOrigImage.ptr());		
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mOrigImage.size().width, mOrigImage.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, mOrigImage.ptr());
	m_bufferMutex.unlock();
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
	  m_bufferMutex.lock();
	  m_currBufferImageTime = timestamp;
	  image.copyTo(m_buffer);
	  image.copyTo(mOrigImage);
	  m_bufferMutex.unlock();
	  redraw();
	  // redraw if necessary;
	
}


void CQtOpenCVViewerGl::addAlpha(const cv::Mat_<uint8_t> &alphaChannel, qint64 timestamp)
{

  cv::Mat_<uint8_t> alphaCopy = alphaChannel.clone();
    m_bufferMutex.lock();

    cv::Size size= mOrigImage.size();
    if (size.area()!=0)
    {
      cv::resize(alphaCopy,alphaCopy,mOrigImage.size());
      cv::addWeighted(mOrigImage,0.5,alphaCopy,0.5,0,mOrigImage);
    }
    m_bufferMutex.unlock();
    glDraw();    
}

void CQtOpenCVViewerGl::updateBuffer(const cv::Mat_< uint16_t >& image, qint64 timestamp)
{
    cv::Mat_<uint8_t> img;
    convert16to8bit(image, img);
    updateBuffer(img, timestamp);
}


bool CQtOpenCVViewerGl::showImage(cv::Mat_< uint8_t > image, qint64 timestamp)
{
  showImage((cv::Mat)image,timestamp);

}

void CQtOpenCVViewerGl::updateBuffer(const cv::Mat& image, qint64 timestamp)
{
  
  if(image.channels()==1)
  {
    if(image.depth() == 2)
    {
      cv::Mat_<uint16_t> mat;
      image.convertTo(mat,CV_16UC1);
      updateBuffer(mat,timestamp);
    }
    else
    {
      cv::Mat_<uint8_t> mat;
      image.convertTo(mat,CV_8UC1);
      updateBuffer(mat,timestamp);
    }
  }
  else
  {
    if(image.depth() == 2)
    {
      cv::Mat_<uint16_t> mat;
      image.convertTo(mat,CV_16UC3);
      updateBuffer(mat,timestamp);
    }
    else
    {
      cv::Mat_<uint8_t> mat;
      image.convertTo(mat,CV_8UC3);
      updateBuffer(mat,timestamp);
    }
  }
}

void CQtOpenCVViewerGl::redraw()
{
  mixImages();
  glDraw();

}

void CQtOpenCVViewerGl::updateOverlayBuffer(const cv::Mat_< uint8_t >& image, qint64 timestamp)
{
    m_overlayMutex.lock();
    image.copyTo(m_overlayImage);
    m_currOverlayImageTime = timestamp;
    m_overlayMutex.unlock();
    redraw();
}

void CQtOpenCVViewerGl::mixImages()
{
  
  if (m_currBufferImageTime == m_currOverlayImageTime)
  {
    m_bufferMutex.lock();
    m_overlayMutex.lock();
    cv::Mat_<uint8_t> alphaCopy = m_overlayImage.clone();
    cv::Size size= mOrigImage.size();
    if (size.area()!=0)
    {

      cv::resize(alphaCopy,alphaCopy,size,0,0,cv::INTER_NEAREST); //NOTE delete the last three parameters to use linear interpolation.
      cv::addWeighted(m_buffer,0.5,alphaCopy,0.5,0,mOrigImage);
      
    }
    m_overlayMutex.unlock();
    m_bufferMutex.unlock();
  }
}

void CQtOpenCVViewerGl::convert16to8bit(cv::InputArray in, cv::OutputArray out)
{
  if(in.channels()==3)
  {
    std::cerr << "3 channel images not yet supported in 16bit" << std::endl;
    assert(in.channels()==3);
  }
  uint16_t * in_ = (uint16_t*)in.getMat().ptr();
  cv::Mat tmp_out(in.getMat().size().height, in.getMat().size().width, cv::DataType<uint8_t>::type);
  uint8_t * out_ = tmp_out.ptr();
  uint16_t * end = in_ + (in.getMat().size().height * in.getMat().size().width);
  while(in_!=end)
  {
    *out_++ = (*in_++)>>8;
  }
  tmp_out.copyTo(out);
}
