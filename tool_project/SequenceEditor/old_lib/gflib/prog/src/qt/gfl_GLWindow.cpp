// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
gfl::qt::GLWindow::GLWindow(QWidget* parent) : QWidget(parent), _pGLWidget( 0 )
{
	setAttribute( Qt::WA_DeleteOnClose );
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
gfl::qt::GLWindow::~GLWindow( void )
{
	delete GetCamera( );
	_pGLWidget->SetCamera( 0 );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void gfl::qt::GLWindow::SetGLWidgetAndCamera( GLWidget* wid, gfl::core::Camera* cam )
{
	_pGLWidget = wid;
	if( !cam ){
		cam = DbgNew gfl::core::Camera;
	}
	wid->SetCamera( cam );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
gfl::core::Camera* gfl::qt::GLWindow::GetCamera( void )
{
	return _pGLWidget->GetCamera( );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void gfl::qt::GLWindow::keyPressEvent( QKeyEvent * event )
{
	if( event->key( ) == Qt::Key_Alt ){
		GetCamera( )->_Mode = gfl::core::Camera::MODE_ORBIT;
	} else if( event->key( ) == Qt::Key_Shift ){
		_pGLWidget->_ShiftMode = true;
	}
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void gfl::qt::GLWindow::keyReleaseEvent( QKeyEvent * event )
{
	if( event->key( ) == Qt::Key_Alt ){
		GetCamera( )->_Mode = gfl::core::Camera::MODE_NORMAL;
	} else if( event->key( ) == Qt::Key_Shift ){
		_pGLWidget->_ShiftMode = false;
	}
}
