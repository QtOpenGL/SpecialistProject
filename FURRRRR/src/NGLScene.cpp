#include <QMouseEvent>
#include <QApplication>

#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

#include <QGLWidget>

#include<libnoise/noise.h>

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;

NGLScene::NGLScene(QWindow *_parent) : OpenGLWindow(_parent)
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  setTitle("Simple Fuir shader");
  m_normalSize=0.1;
  m_modelName="cylinder";

}


NGLScene::~NGLScene()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";

  glDeleteTextures(1,&m_textureName);
  glDeleteTextures(1,&m_furTexture);

  Init->NGLQuit();
}

void NGLScene::resizeEvent(QResizeEvent *_event )
{
  if(isExposed())
  {
  int w=_event->size().width();
  int h=_event->size().height();
  // set the viewport for openGL
  glViewport(0,0,w,h);
  // now set the camera size values as the screen size has changed
  m_cam->setShape(45,(float)w/h,0.05,350);
  renderLater();
  }
}

void NGLScene::loadTexture()
{
/*
    static unsigned int seed = 23234;
    srand(seed);

    unsigned int width = 1028;
    unsigned int height = 1028;
    unsigned int index = 0;
    unsigned int density = 100000;

    //create pixel array of data set it all to zero
    unsigned char *data = new unsigned char[width*height*4];

    for( unsigned int y=0; y < height; ++y)
    {
        for( unsigned int x = 0; x < width; ++x)
        {
            //red
            data[index++]= 0;
            //green
            data[index++]= 0;
            //blue
            data[index++]= 0;
            //alpha the important one
            data[index++]= 255;
        }
    }

    //randomly add hairs
    for(unsigned int tmp = 0; tmp < density; tmp++)
    {
        int pixel = rand() % (height*width)+1;
        //red
        data[pixel*4]= 255;
        //green
        data[pixel*4+1]= 255;
        //blue
        data[pixel*4+2]= 255;
        //alpha the important one
        data[pixel*4+3]= 255;
    }
*/
    QImage image;

    //bool loaded=image.load("texture/tennisTexture.jpg", "JPG");
    bool loaded=image.load("texture/noise.png", "PNG");
    if(loaded == true)
    {
        QImage GLimage;
        GLimage = QGLWidget::convertToGLFormat(image);

        glGenTextures(1,&m_furTexture);
        glBindTexture(GL_TEXTURE_2D,m_furTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     GLimage.width(),
                     GLimage.height(),
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     GLimage.bits()
                     );
    }
}

void NGLScene::loadImageTexture()
{
  /*
  QImage image;

  bool loaded=image.load("texture/TennisBall.jpg", "JPG");
  if(loaded == true)
  {
      QImage GLimage;
      GLimage = QGLWidget::convertToGLFormat(image);

      glGenTextures(1,&m_textureName);
      glBindTexture(GL_TEXTURE_2D,m_textureName);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   GLimage.width(),
                   GLimage.height(),
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   GLimage.bits()
                   );
  }
  */
    noise::module::Perlin myModule;

    static unsigned int seed = 23234;
    unsigned int index = 0;

    myModule.SetOctaveCount(20);
    myModule.SetSeed(seed);

    unsigned int width = 500;
    unsigned int height = 500;

    double value = myModule.GetValue (14.50, 20.25, 75.75);
    std::cout << value << std::endl;

    //create pixel array of data set it all to zero
    unsigned char *data = new unsigned char[width*height*4];

    for(unsigned int y=0; y < height; ++y)
    {
        for( unsigned int x = 0; x < width; ++x)
        {
            double value = myModule.GetValue((double)x/width, 0.2, (double)y/height);

            if(value > 1.0)
            {
                value = 1.0;
            }
            else if(value < -1.0)
            {
                value = -1.0;
            }

            unsigned char tmp = ((value)+1)*126;

            //red
            data[index++]= tmp;
            //green
            data[index++]= tmp;
            //blue
            data[index++]= tmp;
            //alpha the important one
            data[index++]= 255;
        }
    }

    glGenTextures(1,&m_textureName);
    glBindTexture(GL_TEXTURE_2D,m_textureName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 data
                 );
}

void NGLScene::loadCourtTexture()
{
  QImage image;

  bool loaded=image.load("texture/tennis-court.jpg", "JPG");
  if(loaded == true)
  {
      QImage GLimage;
      GLimage = QGLWidget::convertToGLFormat(image);

      glGenTextures(1,&m_courtTexture);
      glBindTexture(GL_TEXTURE_2D,m_courtTexture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   GLimage.width(),
                   GLimage.height(),
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   GLimage.bits()
                   );
  }
}

void NGLScene::loadTextureShader()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    shader->createShaderProgram("TextureShader");

    shader->attachShader("TextureVertex",ngl::VERTEX);
    shader->attachShader("TextureFragment",ngl::FRAGMENT);
    shader->loadShaderSource("TextureVertex","shaders/textureVert.glsl");
    shader->loadShaderSource("TextureFragment","shaders/textureFrag.glsl");

    shader->compileShader("TextureVertex");
    shader->compileShader("TextureFragment");
    shader->attachShaderToProgram("TextureShader","TextureVertex");
    shader->attachShaderToProgram("TextureShader","TextureFragment");

    shader->linkProgramObject("TextureShader");

}

void NGLScene::loadPhongShader()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    shader->createShaderProgram("Phong");

    shader->attachShader("PhongVertex",ngl::VERTEX);
    shader->attachShader("PhongFragment",ngl::FRAGMENT);
    shader->loadShaderSource("PhongVertex","shaders/PhongVertex.glsl");
    shader->loadShaderSource("PhongFragment","shaders/PhongFragment.glsl");

    shader->compileShader("PhongVertex");
    shader->compileShader("PhongFragment");
    shader->attachShaderToProgram("Phong","PhongVertex");
    shader->attachShaderToProgram("Phong","PhongFragment");

    shader->linkProgramObject("Phong");

    // now pass the modelView and projection values to the shader
    shader->setShaderParam1i("Normalize",1);
    shader->setShaderParam3f("viewerPos",m_cam->getEye().m_x,m_cam->getEye().m_y,m_cam->getEye().m_z);

    // now set the material and light values
    ngl::Material m(ngl::POLISHEDSILVER);
    m.loadToShader("material");

    ngl::Mat4 iv;
    iv=m_cam->getProjectionMatrix();
    /// now setup a basic 3 point lighting system
    ngl::Light key(ngl::Vec3(2,1,3),ngl::Colour(1,1,1,1),ngl::POINTLIGHT);
    key.setTransform(iv);
    key.enable();
    key.loadToShader("light[0]");
    ngl::Light fill(ngl::Vec3(-2,1.5,3),ngl::Colour(1,1,1,1),ngl::POINTLIGHT);
    fill.setTransform(iv);
    fill.enable();
    fill.loadToShader("light[1]");
    ngl::Light back(ngl::Vec3(2,1,-2),ngl::Colour(1,1,1,1),ngl::POINTLIGHT);
    back.setTransform(iv);
    back.enable();
    back.loadToShader("light[2]");
}

void NGLScene::loadLights()
{

}

void NGLScene::loadFurShader()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    shader->createShaderProgram("normalShader");

    shader->attachShader("normalVertex",ngl::VERTEX);
    shader->attachShader("normalFragment",ngl::FRAGMENT);
    shader->loadShaderSource("normalVertex","shaders/furVert.glsl");
    shader->loadShaderSource("normalFragment","shaders/furFrag.glsl");

    shader->compileShader("normalVertex");
    shader->compileShader("normalFragment");
    shader->attachShaderToProgram("normalShader","normalVertex");
    shader->attachShaderToProgram("normalShader","normalFragment");

    shader->attachShader("normalGeo",ngl::GEOMETRY);
    shader->loadShaderSource("normalGeo","shaders/furGeo.glsl");
    shader->compileShader("normalGeo");
    shader->attachShaderToProgram("normalShader","normalGeo");

    shader->linkProgramObject("normalShader");
    shader->use("normalShader");

    shader->registerUniform("normalShader", "imageTexture");
    shader->registerUniform("normalShader", "furStrengthTexture");

}


void NGLScene::initialize()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::instance();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  ngl::Vec3 from(0,0,3);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);

  m_cam= new ngl::Camera(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam->setShape(45,(float)720.0/576.0,0.5,150);

  loadPhongShader();

  loadFurShader();

  loadTextureShader();

  loadCourtTexture();

  loadImageTexture();
  loadTexture();

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();


  m_furLocation = glGetUniformLocation(shader->getProgramID("normalShader"), "furStrengthTexture");
  m_imgLocation = glGetUniformLocation(shader->getProgramID("normalShader"), "imageTexture");


  glEnable(GL_DEPTH_TEST); // for removal of hidden surfaces

  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->createSphere("ball",1.0,50);
  prim->createTrianglePlane("court", 36, 78, 50, 50, 50);

  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());
}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_transformStack.getMatrix()*m_mouseGlobalTX;
  MV=M*m_cam->getViewMatrix() ;
  MVP= MV*m_cam->getProjectionMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
  shader->setShaderParamFromMat4("M",M);
}
void NGLScene::loadMatricesToNormalShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["normalShader"]->use();
  ngl::Mat4 MV;
  ngl::Mat4 MVP;

  MVP=m_transformStack.getMatrix()*m_mouseGlobalTX*m_cam->getVPMatrix();
  shader->setShaderParamFromMat4("MVP",MVP);

}

void NGLScene::render()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Rotation based on the mouse position for our global transform
   ngl::Transformation trans;
   ngl::Mat4 rotX;
   ngl::Mat4 rotY;
   // create the rotation matrices
   rotX.rotateX(m_spinXFace);
   rotY.rotateY(m_spinYFace);
   // multiply the rotations
   m_mouseGlobalTX=rotY*rotX;
   // add the translations
   m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
   m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
   m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

  glPointSize(4.0);
  glLineWidth(4.0);

  (*shader)["TextureShader"]->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,m_textureName);

  loadMatricesToShader();

  prim->draw("ball");
  //prim->draw("cube");

/*
  (*shader)["normalShader"]->use();
  glUniform1i(m_imgLocation, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,m_textureName);
  glUniform1i(m_imgLocation, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,m_furTexture);
  glUniform1i(m_furLocation, 1);

  loadMatricesToShader();

  prim->draw("ball");

  glUniform1i(m_imgLocation, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_courtTexture);
  glUniform1i(m_imgLocation, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,m_textureName);
  glUniform1i(m_furLocation, 1);

  loadMatricesToShader();

  prim->draw("court");
*/
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    renderLater();

  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    renderLater();

   }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
		m_modelPos.m_z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
		m_modelPos.m_z-=ZOOM;
	}
	renderLater();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  switch (_event->key())
  {
    case Qt::Key_Plus : m_normalSize+=0.01; break;
    case Qt::Key_Minus : m_normalSize-=0.01; break;
    case Qt::Key_Escape : QApplication::quit(); break;


  }
 renderLater();
}
