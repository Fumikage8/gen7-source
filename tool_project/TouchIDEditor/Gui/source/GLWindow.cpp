//==============================================================================

#include "../include/MainWindow.h"

/**
* @file	GLWindow.cpp
* @brief	
* @author	kimura_shin
* @data	2012/03/19, 17:28
*/
// =============================================================================

GLWindow::GLWindow( )
{
  // Create main layout
  QVBoxLayout *mainLayout = DbgNew QVBoxLayout;

  // Create OpenGl widget
  GLWidget* wid = DbgNew GLWidget( this );

  // ここがミソ これがないと動きません
  gfl::core::Camera* cam =  DbgNew gfl::core::Camera;
  cam->_NearClip = 1.0f;
  cam->_FarClip = 1000.0f;

  SetGLWidgetAndCamera( wid, cam );
  wid->SetTargetFPS( DEFAULT_OUT_FPS );

  mainLayout->addWidget( wid );

  // ドラッグアンドドロップを受付可能にする
  setAcceptDrops(true);

  // メインレイアウトをセットして表示
  setLayout( mainLayout );

  resize(320,240);
  show( );
}

// ---------------------------------------------------------------------------
// Destruct
// ---------------------------------------------------------------------------
GLWindow::~GLWindow( void )
{
}

// ----------------------------------------------------------------
// ドラッグ＆ドロップ
// ----------------------------------------------------------------
void GLWindow::dragEnterEvent( QDragEnterEvent* event )
{
  // ファイルのドラッグを受け付ける
  if( event->mimeData( )->hasFormat( "text/uri-list" ) ){
    event->acceptProposedAction( );
  }
}

void GLWindow::dropEvent( QDropEvent* event )
{
  // ドロップされたファイルのパスを取得
  QUrl url = event->mimeData( )->urls( ).first( );
  QString request = url.toLocalFile( );

  // プロジェクトファイルだった
  if( request.endsWith( ".gfmdl" ) ){
    MainWindow::GetInstance()->LoadFile(request);
  } else {
    QMessageBox::critical(this,"Error",".gfmdl じゃないと嫌どす");
  }
}
