#include "../include/GLWidget.h"
#include "../include/MainWindow.h"

#include "../include/GLUtil.h"
#include <qt/gfl_Tga.h>

#include "../include/FillTextureShader.h"
#include <qt/Shader/Type.h>

// -----------------------------------------------------------------------------
// Construct
// -----------------------------------------------------------------------------
GLWidget::GLWidget( QWidget* parent )
  : gfl::qt::GLWidget( parent ), m_ShowAllStep(0), m_pMotion( 0 )
{
  makeCurrent();

  for( s32 i = 0; i < MODEL_MAX; ++i ){
    SetModel( 0, i );
  }
  setMinimumWidth( 320 );
  setMinimumHeight( 240 );

  m_fOutPut = false;
  m_fIsPainted = false;
  m_fPainedRTClear = false;
  m_fFlush = true;//初期化フラッシュ
  m_fMouseReaveFlush = false;
  m_RenderRTNo = 0;

}

// -----------------------------------------------------------------------------
// Destruct
// -----------------------------------------------------------------------------
GLWidget::~GLWidget( )
{
  Clear();

  {
    if ( m_FrameBuffer )
    {
      glDeleteFramebuffersEXT( 1, &m_FrameBuffer );
      m_FrameBuffer = 0;
    }
    if ( m_RenderBuffer )
    {
      glDeleteRenderbuffersEXT( 1, &m_RenderBuffer );
      m_RenderBuffer = 0;
    }

    glDeleteTextures( RTType::NumberOf, m_RenderTexture );	
  }

  {
    if ( m_FrameBuffer64 )
    {
      glDeleteFramebuffersEXT( 1, &m_FrameBuffer64 );
      m_FrameBuffer64 = 0;
    }
    if ( m_RenderBuffer64 )
    {
      glDeleteRenderbuffersEXT( 1, &m_RenderBuffer64 );
      m_RenderBuffer64 = 0;
    }

    glDeleteTextures( RT64Type::NumberOf, m_RenderTexture64 );	
  }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void GLWidget::Clear()
{
  for( s32 i = 0; i < MODEL_MAX; ++i ){
    delete GetModel(i);
  }
  delete m_pMotion;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void GLWidget::initializeGL()
{
#ifdef USE_GLEW
  //　GLEWの初期化
  s32 ret = glewInit( );
  GFL_ASSERT_MSG( ret == GLEW_OK, "Error!! GLEW initialize" );
#endif

  makeCurrent();

  GL_ERR();

  {
    m_RTSize.Set(1024, 1024);
    //　テクスチャ4枚生成
    glGenTextures( RT64Type::NumberOf, m_RenderTexture64 );
    GL_ERR();

    //　4枚分の設定をしておく
    glEnable( GL_TEXTURE_2D );

    for( int i=0; i < RT64Type::NumberOf; i++ )
    {
      glBindTexture( GL_TEXTURE_2D, m_RenderTexture64[i] );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, (s32)m_RTSize.GFL_VECTOR_X, (s32)m_RTSize.GFL_VECTOR_Y, 0, GL_RGBA, GL_HALF_FLOAT_ARB, 0 );
      glBindTexture( GL_TEXTURE_2D, 0 );
      GL_ERR();
    }

    glDisable( GL_TEXTURE_2D );
    GL_ERR();

    //　レンダーバッファの設定
    glGenRenderbuffersEXT( 1, &m_RenderBuffer64 );
    GL_ERR();
    glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_RenderBuffer64 );
    GL_ERR();
    glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y );
    GL_ERR();

    //　フレームバッファの設定
    glGenFramebuffersEXT( 1, &m_FrameBuffer64 );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FrameBuffer64 );
    for( int i=0; i < RT64Type::NumberOf; i++ )
    {
      glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, m_RenderTexture64[i], 0 );
    }
    glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_RenderBuffer64 );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
    GL_ERR();
  }

  {
    //　テクスチャ4枚生成
    glGenTextures( RTType::NumberOf, m_RenderTexture );
    GL_ERR();

    //　4枚分の設定をしておく
    glEnable( GL_TEXTURE_2D );

    for( int i=0; i < RTType::NumberOf; i++ )
    {
      glBindTexture( GL_TEXTURE_2D, m_RenderTexture[i] );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
      glBindTexture( GL_TEXTURE_2D, 0 );
      GL_ERR();
    }

    glDisable( GL_TEXTURE_2D );
    GL_ERR();

    //　レンダーバッファの設定
    glGenRenderbuffersEXT( 1, &m_RenderBuffer );
    GL_ERR();
    glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_RenderBuffer );
    GL_ERR();
    glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y );
    GL_ERR();

    //　フレームバッファの設定
    glGenFramebuffersEXT( 1, &m_FrameBuffer );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FrameBuffer );
    for( int i=0; i < RTType::NumberOf; i++ )
    {
      glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, m_RenderTexture[i], 0 );
    }
    glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_RenderBuffer );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
    GL_ERR();
  }
}

// ---------------------------------------------------------------------------
// Resize
// ---------------------------------------------------------------------------
void GLWidget::resizeGL( int width, int height )
{
  makeCurrent();

  m_Width = width;
  m_Height = height;
  glViewport( 0, 0, width, height );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );

  gfl::core::Camera* cam = GetCamera( );
  f32 nc, fc;
  if( cam ){
    nc = cam->_NearClip;
    fc = cam->_FarClip;
  } else {
    QMessageBox::critical( this, "Error", "カメラが設定されていません。SetGLWidgetAndCamera()を呼んでください。" );
    nc = GFL_DEFAULT_NEAR_CLIP;
    fc = GFL_DEFAULT_FAR_CLIP;
  }
  const f32 w = cam->_ControlDistance;
  const f32 h = (w*height)/width;
  glOrtho( -w, w, -h, h, nc, fc);

  glMatrixMode( GL_MODELVIEW );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void GLWidget::mousePressEvent(QMouseEvent* event)
{
  using namespace Gui;
  makeCurrent();

  gfl::qt::GLWidget::mousePressEvent(event);

  gfl::core::Camera* cam = GetCamera( );

  if( gfl::core::Camera::MODE_ORBIT != cam->_Mode )
  {
    MainWindow *pMainWindow = dynamic_cast<MainWindow*>( MainWindow::GetInstance() );
    QColor qlineColor = pMainWindow->GetIdWidget()->GetIdColor();
    s32 deviceType = pMainWindow->GetToolWidget()->GetDevice();
    s32 tolerance = pMainWindow->GetToolWidget()->GetTolerance();

    if ( deviceType == ToolWidget::TOOL_BUCKET )
    {
      PaintUtil::FPoint pos;

      pos.x = static_cast<float>(event->pos().x()) / static_cast<float>(this->size().width());
      pos.y = static_cast<float>(event->pos().y()) / static_cast<float>(this->size().height());

      pos.x *= this->m_RTSize.GFL_VECTOR_X;
      pos.y *= this->m_RTSize.GFL_VECTOR_Y;

      PaintUtil::DrawBucketInfo info;

      info.mousePos = QPoint(pos.x, pos.y);
      info.color = qlineColor;
      info.tolerance = tolerance;
      info.RTSize.Set( this->m_RTSize );
      info.frameBuffer = this->m_FrameBuffer;
      info.renderTexture = this->m_RenderTexture[RTType::Painted];
      info.src_attachments = GL_COLOR_ATTACHMENT0_EXT + RTType::MainCopy;
      info.dest_attachments = GL_COLOR_ATTACHMENT0_EXT + RTType::Painted;

      PaintUtil::DrawBucket( info );
      m_MousePoints.push_back( PaintUtil::FPoint( -2, -2 ) ); //ダミートリガー
      refreshWindows();
      m_fIsPainted = true;
    }
    else
    {
      this->recordMouseMove(event);
    }

  }

  resizeGL( m_Width, m_Height );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
  makeCurrent();

  gfl::qt::GLWidget::mouseReleaseEvent(event);

  m_fFlush = true;
  m_fMouseReaveFlush = true;

  resizeGL( m_Width, m_Height );
  repaint();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
  makeCurrent();

  gfl::qt::GLWidget::mouseMoveEvent(event);

  gfl::core::Camera* cam = GetCamera( );

  if( gfl::core::Camera::MODE_ORBIT == cam->_Mode )
  {
    //ペイント解除とみなす
    m_fFlush = true;
    m_fMouseReaveFlush = true;
    return;
  }
  else
  {
    this->recordMouseMove(event);
  }

  resizeGL( m_Width, m_Height );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void GLWidget::recordMouseMove(QMouseEvent* event)
{
  using namespace Gui;

  gfl::core::Camera* cam = GetCamera( );
  if( gfl::core::Camera::MODE_ORBIT == cam->_Mode )
    return;

  MainWindow *pMainWindow = dynamic_cast<MainWindow*>( MainWindow::GetInstance() );
  QColor qlineColor = pMainWindow->GetIdWidget()->GetIdColor();
  s32 deviceType = pMainWindow->GetToolWidget()->GetDevice();

  makeCurrent();

  if( event->buttons( ) & Qt::LeftButton )
  { 
    // 書き込み
    QPoint mousePos = event->pos();
    int test = this->size().width();
    test = this->size().height();

    PaintUtil::FPoint pos;

    pos.x = static_cast<float>(mousePos.x()) / static_cast<float>(this->size().width());
    pos.y = static_cast<float>(mousePos.y()) / static_cast<float>(this->size().height());

    pos.x *= 2.0f;
    pos.x -= 1.0f;

    pos.y *= 2.0f;
    pos.y -= 1.0f;
    pos.y *= -1.0f;

    m_MousePoints.push_back( pos );
    refreshWindows();
    m_fIsPainted = true;
  }

  if ( m_MousePoints.size() > 5 )
  {
    //これ以上溜め込まない。一度フラッシュ。
    m_fFlush = true;
  }
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GLWidget::refreshWindows()
{
  repaint();

  {
    //更新を促す
    std::vector<Gui::UvPaintWindow*> pUvPaintWindowList = MainWindow::GetInstance( )->GetUvPaintWindowList();

    for( unsigned int i = 0; i < pUvPaintWindowList.size(); ++i )
    {
      pUvPaintWindowList[i]->GetUvPaintView()->update();
      pUvPaintWindowList[i]->GetUvPaintView()->repaint();
    }
  }
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GLWidget::keyPressEvent( QKeyEvent * event )
{
  gfl::qt::GLWidget::keyPressEvent( event );

  makeCurrent();

  switch( event->key() ){
  case Qt::Key_F1:
    m_RenderRTNo = 0;
    break;
  case Qt::Key_F2:
    m_RenderRTNo = 1;
    break;
  case Qt::Key_F3:
    m_RenderRTNo = 2;
    break;
  case Qt::Key_F4:
    m_RenderRTNo = 3;
    break;
  case Qt::Key_F5:
    m_RenderRTNo = 4;
    break;
  case Qt::Key_F6:
    m_RenderRTNo = 5;
    break;
  }
  repaint();
}

// -----------------------------------------------------------------------------
// Render
// -----------------------------------------------------------------------------
void GLWidget::setProjectionMatrix( gfl::core::Vector2 size )
{
  f32 width = size.GFL_VECTOR_X;
  f32 height = size.GFL_VECTOR_Y;

  width = m_Width;
  height = m_Height;

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );

  gfl::core::Camera* cam = GetCamera( );
  f32 nc, fc;
  if( cam ){
    nc = cam->_NearClip;
    fc = cam->_FarClip;
  } else {
    QMessageBox::critical( this, "Error", "カメラが設定されていません。SetGLWidgetAndCamera()を呼んでください。" );
    nc = GFL_DEFAULT_NEAR_CLIP;
    fc = GFL_DEFAULT_FAR_CLIP;
  }
  const f32 w = cam->_ControlDistance;
  const f32 h = (w*height)/width;
  glOrtho( -w, w, -h, h, nc, fc);
}

// -----------------------------------------------------------------------------
// Render
// -----------------------------------------------------------------------------
void GLWidget::Render( const GLenum mode )
{
  using namespace Gui;
  if(mode == GL_SELECT){
    return;
  }
  if(m_ShowAllStep){
    if(m_ShowAllStep++ == 3){
      MainWindow* mw = MainWindow::GetInstance();
      mw->GetDisplayWidget()->ShowAll();
      s32 x = (1280-mw->width())>>1;
      s32 y = (1024-64-mw->height())>>1;
      if(0<=x&&0<=y){
        mw->move(x,y);
      }
      m_ShowAllStep = 0;
    }
  }

#ifdef WAIT_LOAD 
  if( m_RenderState == RENDER_WAIT ){
    return;
  }
  m_RenderState = RENDER_NOW;
#endif

  makeCurrent();
  glUseProgram( 0 );

  glEnable( GL_DEPTH_TEST );

#ifdef MAYA_INPORT
  if ( 0 )
#endif
  {
    glClearColor( 0.5f,0.5f,0.5f,0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_ALPHA_TEST );

    mainRender( mode );
#ifdef WAIT_LOAD
    m_RenderState = RENDER_NONE;
#endif
    return;
  }

  {
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    //　ビューポート設定
    glViewport( 0, 0, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y );
    setProjectionMatrix( m_RTSize );
    glMatrixMode( GL_MODELVIEW );

    //　フレームバッファをバインド
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FrameBuffer64 );

    {
      //モデル描画ＲＴ
      static const GLuint attachments[2] = 
      {
        GL_COLOR_ATTACHMENT0_EXT + RT64Type::Main,	// 本体カラー
        GL_COLOR_ATTACHMENT0_EXT + RT64Type::Depth	// 深度レンダリング
      }; 

      glDrawBuffers( 2, attachments );
      glClearColor( 0.5f,0.5f,0.5f,1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      mainRender( mode );
    }

    if (m_MousePoints.size())
    {
      //UvPaintViewテクスチャにペイント描画
      uvRender();
    }

    //　フレームバッファをバインド
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FrameBuffer );

    {
      //32ビットテクスチャに変換
      glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::MainCopy );
      glClearColor( 0.0f, 1.0f, 0.0f, 0.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      PaintUtil::DrawTexture( m_RenderTexture64[ RT64Type::Main ], GL_NEAREST );
    }

    {
      //現在書き込み中描画
      glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::Paint );
      glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      MainWindow *pMainWindow = dynamic_cast<MainWindow*>( MainWindow::GetInstance() );
      s32 deviceType = pMainWindow->GetToolWidget()->GetDevice();

      switch(deviceType){
      case ToolWidget::TOOL_POINT:
      case ToolWidget::TOOL_BRUSH:
        {
          s32 lineWidth = pMainWindow->GetToolWidget()->GetLine();
          QColor qlineColor = pMainWindow->GetIdWidget()->GetIdColor();

          PaintUtil::DrawPointInfo info;
          info.pointList = m_MousePoints;
          info.lineWidth = lineWidth;
          info.qlineColor = qlineColor;
          info.screanSize.GFL_VECTOR_X = this->size().width();
          info.screanSize.GFL_VECTOR_Y = this->size().height();

          PaintUtil::DrawPoints( info );
        }
        break;
      default:
        break;
      }
    }

    {
      glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::Painted );

      if ( m_fPainedRTClear == false )
      {
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        m_fPainedRTClear = true;
      }

      if ( m_fFlush )
      {
        //書き込み蓄積

        //蓄積前
        {
          MainWindow *pMainWindow = dynamic_cast<MainWindow*>( MainWindow::GetInstance() );
          s32 deviceType = pMainWindow->GetToolWidget()->GetDevice();

          switch(deviceType){
          case ToolWidget::TOOL_POINT:
          case ToolWidget::TOOL_BRUSH:
            {
              s32 lineWidth = pMainWindow->GetToolWidget()->GetLine();
              QColor qlineColor = pMainWindow->GetIdWidget()->GetIdColor();

              PaintUtil::DrawPointInfo info;
              info.pointList = m_MousePoints;
              info.lineWidth = lineWidth;
              info.qlineColor = qlineColor;
              info.screanSize.GFL_VECTOR_X = this->size().width();
              info.screanSize.GFL_VECTOR_Y = this->size().height();

              PaintUtil::DrawPoints( info );
            }
            break;
          default:
            break;
          }
        }

        //蓄積後
        if ( m_fMouseReaveFlush == false && m_MousePoints.size() )
        {
          //備蓄中
          PaintUtil::FPoint		lastPos = m_MousePoints[ m_MousePoints.size() - 1 ];
          m_MousePoints.clear(); //蓄積したのでログ削除
          m_MousePoints.push_back(lastPos);
        }
        else
        {
          //マウスが放された。
          m_MousePoints.clear(); //蓄積したのでログ削除

          //残さず削除
          glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
          glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        }

        m_fFlush = false;
        m_fMouseReaveFlush = false;
      }
    }

    if ( m_RenderRTNo == RTType::Debug )
    {
      //合成
      glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::Debug );
      glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      PaintUtil::DrawTextures( m_RenderTexture[RTType::MainCopy], m_RenderTexture[RTType::Painted], m_RenderTexture[RTType::Paint] );
      glUseProgram( 0 );
    }

    glUseProgram( 0 );

    //　フレームバッファのアンバインド
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

    glPopAttrib();
  }

  glClearColor( 0, 1, 0, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  switch( m_RenderRTNo ){
  case 0:
    PaintUtil::DrawTexture( m_RenderTexture64[ RT64Type::Main ] );
    break;
  case 1:
    PaintUtil::DrawTexture( m_RenderTexture64[ RT64Type::Depth ] );
    break;
  case 2:
    PaintUtil::DrawTexture( m_RenderTexture[ RTType::Painted ] );
    break;
  case 3:
    PaintUtil::DrawTexture( m_RenderTexture[ RTType::Paint ] );
    break;
  case 4:
    PaintUtil::DrawTexture( m_RenderTexture[ RTType::MainCopy ] );
    break;
  case 5:
    PaintUtil::DrawTexture( m_RenderTexture[ RTType::Debug ] );
    break;
  }


#ifdef WAIT_LOAD
  m_RenderState = RENDER_NONE;
#endif
}

// -----------------------------------------------------------------------------
// Render
// -----------------------------------------------------------------------------
void GLWidget::mainRender( const GLenum mode )
{	
  DisplayWidget* dw = MainWindow::GetInstance( )->GetDisplayWidget();
  if(dw->CheckLight()){
    SetLight( );
    glEnable( GL_LIGHTING );
  }else{
    glDisable( GL_LIGHTING );
  }

  glMatrixMode( GL_MODELVIEW );
  GlLoadMatrix( GetCamera( )->_Matrix ); // カメラのマトリックスをロード

  glEnable(GL_BLEND); //ブレンドの有効化
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f( 0.75f, 0.75f, 0.75f );

  {
    gfl::cmdl::Model* model = GetModel(MODEL_ID);

    if(model)
    {
      model->SetLightEnable( dw->CheckLight() );
      model->SetCamera( GetCamera() );

      gfl::core::Matrix* mat = MainWindow::GetInstance()->GetMotionWidget()->GetMatrix();
      if(mat)
      {
        model->RenderSkin( mat, gfl::qt::Render::SHD_NORMAL );
      }else{
        model->RenderPrimitive();
      }
    }
  }

  glUseProgram( 0 );
  gfl::qt::Render::Axis( METER( 1.0f ) );
  glDisable(GL_BLEND);
}

// -----------------------------------------------------------------------------
// Render
// -----------------------------------------------------------------------------
void GLWidget::uvRender()
{
#ifdef MAYA_INPORT

  DisplayWidget* dw = MainWindow::GetInstance( )->GetDisplayWidget();
  if(dw->CheckLight()){
    SetLight( );
    glEnable( GL_LIGHTING );
  }else{
    glDisable( GL_LIGHTING );
  }

  glMatrixMode( GL_MODELVIEW );
  GlLoadMatrix( GetCamera( )->_Matrix ); // カメラのマトリックスをロード

  glEnable(GL_BLEND); //ブレンドの有効化
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f( 0.75f, 0.75f, 0.75f );

  {
    gfl::cmdl::Model* model = GetModel(MODEL_ID);

    if(model){

      model->SetCamera( GetCamera() );

      gfl::core::Matrix* mat = MainWindow::GetInstance()->GetMotionWidget()->GetMatrix();
      if(mat){
        drawModelUv( model, mat );
      }
    }
  }

  glUseProgram( 0 );

  //設定を元に戻す
  glViewport( 0, 0, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y );
  setProjectionMatrix( m_RTSize );
  glMatrixMode( GL_MODELVIEW );
  glDisable(GL_BLEND);

#endif
}

// -----------------------------------------------------------------------------
// Render
// -----------------------------------------------------------------------------
void GLWidget::drawModelUv( gfl::cmdl::Model *pModel, gfl::core::Matrix* mat )
{
#ifdef MAYA_INPORT
  using namespace gfl::cmdl;
  using namespace gfl::maya;

  glUseProgram( 0 );

  Mesh* mesh = pModel->GetMesh( );
  const s32 shape_count = pModel->GetShapeCount( );

  glEnable( GL_TEXTURE_2D );

  std::vector<Gui::UvPaintWindow*> pUvPaintWindowList = MainWindow::GetInstance( )->GetUvPaintWindowList();

  //まずクリア
  for( unsigned int viewNo = 0; viewNo < pUvPaintWindowList.size(); ++viewNo )
  {
    Gui::UvPaintView *pUvPaintView = pUvPaintWindowList[ viewNo ]->GetUvPaintView();
    pUvPaintView->SetRenderTarget( Gui::UvPaintView::RTType::MainViewPaint );
    glClearColor( 0.0f, 0.5f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }

  for( s32 layerNo = 0; layerNo < DrawLayer::NumberOf; ++layerNo )
  {	
    for( s32 i = 0; i < pModel->GetMeshCount(); ++i )
    {
      gfl::cmdl::Material *pMaterial = mesh[i].GetMaterial();
      Shape *shape = mesh[i].GetShape();

      if(!pMaterial){ // skip ParticleShape
        continue;
      }

      {
        static const char* translucencyKindNames[DrawLayer::NumberOf] = {
          "Layer0", "Layer1", "Layer2", "Layer3"
        };

        if( pMaterial->m_TranslucencyKind != translucencyKindNames[ layerNo ] )
          continue;
      }

      shape->Skining( mat, pModel->GetBone(), pMaterial );

      if(shape->GetIndexCount())
      {
        Material::TextureMapper *pTextureMapper = pMaterial->GetTextureMapper();
        Gui::UvPaintView *pUvPaintView = NULL;

        for( int textureNo = 0; textureNo < pMaterial->GetTextureMapperCoordCount(); ++textureNo )
        {
          QString mapperImageName = pTextureMapper[ textureNo ].GetFileName();

          for( unsigned int viewNo = 0; viewNo < pUvPaintWindowList.size(); ++viewNo )
          {
            QString viewImageName = pUvPaintWindowList[ viewNo ]->GetImageFileName();

            if ( mapperImageName == viewImageName )
            {
              pUvPaintView = pUvPaintWindowList[ viewNo ]->GetUvPaintView();
              break;
            }
          } 

          if ( pUvPaintView == NULL )
            continue;

          pMaterial->DraswUvSpaceNo( textureNo );

          {
            pUvPaintView->SetRenderTarget( Gui::UvPaintView::RTType::MainViewPaint );
            //　ビューポート設定
            glViewport( 0, 0, pUvPaintView->GetTextureSize().GFL_VECTOR_X, pUvPaintView->GetTextureSize().GFL_VECTOR_Y );
            setProjectionMatrix( pUvPaintView->GetTextureSize() );
            glMatrixMode( GL_MODELVIEW );
          }

          pMaterial->SetCamera( GetCamera() );
          pMaterial->glBind( Material::ShaderType::ModelRender, *pModel->GetLookupTable() );

          {
            glActiveTexture( GL_TEXTURE13 );
            glEnable(GL_TEXTURE_2D);
            glBindTexture( GL_TEXTURE_2D, m_RenderTexture[ RTType::Painted ] );
            //glBindTexture( GL_TEXTURE_2D, m_RenderTexture[ RTType::MainCopy ] );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
          }

          {
            glActiveTexture( GL_TEXTURE14 );
            glEnable(GL_TEXTURE_2D);
            glBindTexture( GL_TEXTURE_2D, m_RenderTexture[ RTType::Paint ] );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
          }

          {
            glActiveTexture( GL_TEXTURE15 );
            glEnable(GL_TEXTURE_2D);
            glBindTexture( GL_TEXTURE_2D, m_RenderTexture64[ RT64Type::Depth ] );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
          }

          if ( shape->GetWorkVertices( ) )
          {
            glEnableVertexAttribArray( GLInterface::StreamUsage::Position );
            glVertexAttribPointer( GLInterface::StreamUsage::Position, 3, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 3, shape->GetWorkVertices( ) );
          }else{
            glDisableVertexAttribArray( GLInterface::StreamUsage::Position );
          }

          if ( shape->GetWorkTextures(0) )
          {
            glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord0 );
            glVertexAttribPointer( GLInterface::StreamUsage::TexCoord0, 2, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 2, shape->GetWorkTextures(0) );
          }else{
            glDisableVertexAttribArray( GLInterface::StreamUsage::TexCoord0 );
          }

          if ( shape->GetWorkTextures(1) )
          {
            glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord1 );
            glVertexAttribPointer( GLInterface::StreamUsage::TexCoord1, 2, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 2, shape->GetWorkTextures(1) );
          }else{
            glDisableVertexAttribArray( GLInterface::StreamUsage::TexCoord1 );
          }

          if ( shape->GetWorkTextures(2) )
          {
            glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord2 );
            glVertexAttribPointer( GLInterface::StreamUsage::TexCoord2, 2, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 2, shape->GetWorkTextures(2) );
          }else{
            glDisableVertexAttribArray( GLInterface::StreamUsage::TexCoord2 );
          }

          if ( shape->GetWorkNormals() )
          {
            glEnableVertexAttribArray( GLInterface::StreamUsage::Normal );
            glVertexAttribPointer( GLInterface::StreamUsage::Normal, 3, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 3, shape->GetWorkNormals() );
          }else{
            glDisableVertexAttribArray( GLInterface::StreamUsage::Normal );
          }

          if ( shape->GetWorkColor() )
          {
            glEnableVertexAttribArray( GLInterface::StreamUsage::Color );
            glVertexAttribPointer( GLInterface::StreamUsage::Color, 4, GLInterface::VertexType::UByte, GL_FALSE, sizeof(unsigned char) * 4, shape->GetWorkColor() );
          }else{
            glDisableVertexAttribArray( GLInterface::StreamUsage::Color );
          }

          glDrawArrays(GL_TRIANGLES, 0, shape->GetIndexCount());

          pMaterial->DraswUvSpaceNo( -1 );
        }

      }
    }
  }

  glUseProgram(0);//シェーダーアンバインド
  glDisable( GL_TEXTURE_2D );
#endif
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void GLWidget::RenderBoundingBox()
{
  if(GetModel(0)){
    glClearColor( 0.5f,0.5f,0.5f,0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    GlLoadMatrix(GetCamera()->_Matrix);
    GetModel(0)->RenderBoundingBox();
  }
}
