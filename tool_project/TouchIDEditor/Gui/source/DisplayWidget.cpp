//==============================================================================

#include "../include/DisplayWidget.h"
#include "../include/MainWindow.h"

/**
* @file DisplayWidget.cpp
* @brief
* @author kimura_shin
* @data 2012/03/23, 16:12
*/
// =============================================================================

#define KEY_LIGHT "Light"
#define KEY_ORG_ALPHA "OriginalAlpha"
#define KEY_ID_ALPHA "IdAlpha"

//------------------------------------------------------------------------------
// Construct
//------------------------------------------------------------------------------
DisplayWidget::DisplayWidget( ) : QGroupBox( "表示" )
{
  QVBoxLayout* v_lay = DbgNew QVBoxLayout;
  QHBoxLayout* h_lay = DbgNew QHBoxLayout;

  QSettings* st = MainWindow::GetInstance( )->GetSettings( );
  QCheckBox* cb = DbgNew QCheckBox( "照明" );
  m_pLight = cb;

  QPushButton* pb = DbgNew QPushButton("全");
  pb->setShortcut(QKeySequence("a"));
  connect( pb, SIGNAL(clicked()),this,SLOT(ShowAll()) );
  pb->setToolTip("全身カメラをセットします。\nショートカットは a です。");
  h_lay->addWidget(pb);
  pb = DbgNew QPushButton("前");
  h_lay->addWidget(pb);
  pb->setShortcut( QKeySequence("f") );
  pb->setToolTip("正面カメラをセットします。\nショートカットは f です。");
  connect( pb, SIGNAL(clicked()), this, SLOT( SetCameraFront() ) );
  pb = DbgNew QPushButton("後");
  h_lay->addWidget(pb);
  pb->setToolTip("背面カメラをセットします。\nショートカットは b です。");
  pb->setShortcut( QKeySequence("b") );
  connect( pb, SIGNAL(clicked()), this, SLOT( SetCameraBack() ) );
  pb = DbgNew QPushButton("左");
  h_lay->addWidget(pb);
  pb->setToolTip("左側面カメラをセットします。\nショートカットは l です。");
  pb->setShortcut( QKeySequence("l") );
  connect( pb, SIGNAL(clicked()), this, SLOT( SetCameraLeft() ) );
  pb = DbgNew QPushButton("右");
  h_lay->addWidget(pb);
  pb->setShortcut( QKeySequence("r") );
  pb->setToolTip("右側面カメラをセットします。\nショートカットは r です。");
  connect( pb, SIGNAL(clicked()), this, SLOT( SetCameraRight() ) );

  v_lay->addLayout( h_lay );

  h_lay = DbgNew QHBoxLayout;
  h_lay->addWidget( DbgNew QLabel( "元α:" ) );
  QSlider* sl = DbgNew QSlider( Qt::Horizontal );
  m_pAlpha[GLWidget::MODEL_ORG] = sl;
  sl->setMaximum( 0xff );
  sl->setValue( st->value( KEY_ORG_ALPHA, 0 ).toInt( ) );
  connect( sl, SIGNAL( valueChanged( int ) ), this, SLOT( OriginalAlphaChanged( int ) ) );
  h_lay->addWidget( sl );
  v_lay->addLayout( h_lay );

  h_lay = DbgNew QHBoxLayout;
  h_lay->addWidget( DbgNew QLabel( "IDα:" ) );
  sl = DbgNew QSlider( Qt::Horizontal );
  m_pAlpha[GLWidget::MODEL_ID] = sl;
  sl->setMaximum( 0xff );
  sl->setValue( st->value( KEY_ID_ALPHA, 0xff ).toInt( ) );
  connect( sl, SIGNAL( valueChanged( int ) ), this, SLOT( IdAlphaChanged( int ) ) );
  h_lay->addWidget( sl );
  v_lay->addLayout( h_lay );

  setMaximumSize( 192, 128 );
  setLayout( v_lay );
}

//------------------------------------------------------------------------------
// Light
//------------------------------------------------------------------------------
void DisplayWidget::LightChanged( int val )
{
  MainWindow::GetInstance( )->GetSettings( )->setValue( KEY_LIGHT, val );
}

//------------------------------------------------------------------------------
// Original Alpha
//------------------------------------------------------------------------------
void DisplayWidget::OriginalAlphaChanged( int val )
{
  MainWindow::GetInstance( )->GetSettings( )->setValue( KEY_ORG_ALPHA, val );
}

//------------------------------------------------------------------------------
// Id Alpha
//------------------------------------------------------------------------------
void DisplayWidget::IdAlphaChanged( int val )
{
  MainWindow::GetInstance( )->GetSettings( )->setValue( KEY_ID_ALPHA, val );

  {//更新を促す
    std::vector<Gui::UvPaintWindow*> pUvPaintWindowList = MainWindow::GetInstance( )->GetUvPaintWindowList();

    for( unsigned int i = 0; i < pUvPaintWindowList.size(); ++i )
    {
      pUvPaintWindowList[i]->GetUvPaintView()->repaint();
    }
  }

  MainWindow::GetInstance( )->GetGLWindow()->GetGLWidget()->repaint();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void DisplayWidget::SetCamera(const f32 yang)
{
  gfl::core::Camera* cam = MainWindow::GetInstance( )->GetGLWindow()->GetCamera();
  cam->_ControlRotationX = 0.0f;
  cam->_ControlRotationY = yang;
  cam->UpdateMatrixByInterest();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void DisplayWidget::SetCameraFront()
{
  SetCamera(0.0f);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void DisplayWidget::SetCameraBack()
{
  SetCamera(gfl::core::Math::ONE_PI);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void DisplayWidget::SetCameraLeft()
{
  SetCamera(gfl::core::Math::HALF_PI);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void DisplayWidget::SetCameraRight()
{
  SetCamera(gfl::core::Math::ONE_HALF_PI);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void DisplayWidget::ShowAll()
{
  GLWidget* wid = MainWindow::GetInstance()->GetGLWindow()->GetGLWidget();
  gfl::cmdl::Model* model = wid->GetModel(0);
  if(!model){
    return;
  }

  // GL の描画用行列の取得
  GLint viewport[4];
  glGetIntegerv( GL_VIEWPORT, viewport );
  GLdouble modelview[16];
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  GLdouble projection[16];
  glGetDoublev( GL_PROJECTION_MATRIX, projection );

  gfl::qt::GLWidget::Project proj;

  const s32 shape_count = model->GetShapeCount( );
  gfl::cmdl::Shape* shape = model->GetShape( );

  for( s32 i = 0; i < shape_count; ++i, ++shape ){
    gfl::core::Vector v;
    f32 x = shape->m_BoundingBox.m_Size.GetX()*0.5f;
    f32 y = shape->m_BoundingBox.m_Size.GetY()*0.5f;
    f32 z = shape->m_BoundingBox.m_Size.GetZ()*0.5f;
    shape->m_BoundingBox.m_Matrix.Trans(&v,x,y,z);
    proj.SetProject( v, viewport, modelview, projection );
    shape->m_BoundingBox.m_Matrix.Trans(&v,-x,y,z);
    proj.SetProject( v, viewport, modelview, projection );
    shape->m_BoundingBox.m_Matrix.Trans(&v,x,y,-z);
    proj.SetProject( v, viewport, modelview, projection );
    shape->m_BoundingBox.m_Matrix.Trans(&v,-x,y,-z);
    proj.SetProject( v, viewport, modelview, projection );
    shape->m_BoundingBox.m_Matrix.Trans(&v,x,-y,z);
    proj.SetProject( v, viewport, modelview, projection );
    shape->m_BoundingBox.m_Matrix.Trans(&v,-x,-y,z);
    proj.SetProject( v, viewport, modelview, projection );
    shape->m_BoundingBox.m_Matrix.Trans(&v,-x,-y,-z);
    proj.SetProject( v, viewport, modelview, projection );
    shape->m_BoundingBox.m_Matrix.Trans(&v,x,-y,-z);
    proj.SetProject( v, viewport, modelview, projection );
  }

  double x, y, z;
  gluUnProject( ( proj.l_x + proj.h_x )*0.5f, ( proj.l_y + proj.h_y )*0.5f,
    ( proj.l_z + proj.h_z )*0.5f, modelview, projection, viewport, &x, &y, &z );

  const f32 x_scl = ( ( proj.h_x - proj.l_x ) / (f32)wid->width() );
  const f32 y_scl = ( ( proj.h_y - proj.l_y ) / (f32)wid->height() );

  const double adj = ( ( x_scl < y_scl ) ? y_scl : x_scl );
  if(0.0f < adj){
    gfl::core::Camera* cam = wid->GetCamera();
    cam->_ControlDistance *= adj;
    cam->SetInterest( gfl::core::Vector( x, y, z ) );
    cam->UpdateMatrixByInterest();
  }
}
