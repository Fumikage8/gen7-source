// =============================================================================

// GLWidget Class
// =============================================================================
// -----------------------------------------------------------------------------
// Construct
// -----------------------------------------------------------------------------
gfl::qt::GLWidget::GLWidget( QWidget* parent )
: QGLWidget( parent ), _pCamera( 0 ), _ShiftMode( 0 ), _TargetFPS( GFL_FPS ), m_PickButton( Qt::LeftButton )
{
	//	_Time.start( );
	_Time.Start( );

	/* デフォルト値を設定　変えたければ後から変更可能 */
	setMinimumWidth( DEFAULT_GLWIDGET_SIZE_X );
	setMinimumHeight( DEFAULT_GLWIDGET_SIZE_Y );
	//	_TimerId =
	startTimer( 0 ); // すぐ戻ってくるタイマー

	// マウスピック初期化
	for( s32 i = 0; i < PICK_MAX; ++i ){
		_PickId[i] = PICK_NO_USE;
	}

	//	glutJoystickFunc( PadCallback, -1 );

	setFocusPolicy( Qt::StrongFocus );
	show( );
}

// -----------------------------------------------------------------------------
// FPS
// -----------------------------------------------------------------------------
void gfl::qt::GLWidget::SetTargetFPS( const s32 fps )
{
	//	killTimer( _TimerId );
	_TargetFPS = fps;
	// タイマーの単位はミリセカンド
	//	_TimerId = startTimer( 1000 / fps );
}

// -----------------------------------------------------------------------------
// Destruct
// -----------------------------------------------------------------------------
gfl::qt::GLWidget::~GLWidget( ) { }

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
void gfl::qt::GLWidget::initializeGL( )
{
#ifdef USE_GLEW
	//　GLEWの初期化
	s32 ret = glewInit( );
	GFL_ASSERT_MSG( ret == GLEW_OK, "Error!! GLEW initialize" );
#endif
	//	qglClearColor(trolltechPurple.dark());
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//	object = makeAxis( );
	glShadeModel( GL_FLAT );
	//	glShadeModel( GL_SMOOTH );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
}

// ---------------------------------------------------------------------------
// Size
// ---------------------------------------------------------------------------
QSize gfl::qt::GLWidget::minimumSizeHint( ) const
{
	return QSize( 64, 48 );
}

// ---------------------------------------------------------------------------
// size
// ---------------------------------------------------------------------------
QSize gfl::qt::GLWidget::sizeHint( ) const
{
	return QSize( 640, 480 );
}

// ---------------------------------------------------------------------------
// Resize
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::resizeGL( int width, int height )
{
	glViewport( 0, 0, width, height );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	//	glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
	gfl::core::Camera* cam = GetCamera( );
	f32 ps, nc, fc;
	if( cam ){
		ps = cam->_Perspective;
		nc = cam->_NearClip;
		fc = cam->_FarClip;
	} else {
		QMessageBox::critical( this, "Error", "カメラが設定されていません。SetGLWidgetAndCamera()を呼んでください。" );
		ps = GFL_DEFAULT_PERSPECTIVE;
		nc = GFL_DEFAULT_NEAR_CLIP;
		fc = GFL_DEFAULT_FAR_CLIP;
	}
	gluPerspective( ps, ( f32 )width / ( f32 )height, nc, fc );
	glMatrixMode( GL_MODELVIEW );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::qt::GLWidget::Ortho2D(s32 left, s32 right, s32 bottom, s32 top)
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( left,right,bottom,top );
	glMatrixMode( GL_MODELVIEW );
}


#if 0 //
// -----------------------------------------------------------------------------
// Camera
// -----------------------------------------------------------------------------
gfl::core::Camera* gfl::qt::GLWidget::GetCamera( void )
{
	return(( GLWindow* )parent( ) )->GetCamera( );
}
#endif

// -----------------------------------------------------------------------------
// SetLight
// -----------------------------------------------------------------------------
void gfl::qt::GLWidget::SetLight( void )
{
	//	GLfloat pos[4] = { 0.0, 0.0, 0.0, 1.0 };
	static const GLfloat amb_default[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	static const GLfloat diff_default[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat spec_default[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glLightfv( GL_LIGHT0, GL_POSITION, gfl::core::Vector4( (gfl::core::Vector3&)GetCamera( )->_Trans, 1.0f ).Get( ) );
	glLightfv( GL_LIGHT0, GL_AMBIENT, amb_default );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diff_default );
	glLightfv( GL_LIGHT0, GL_SPECULAR, spec_default );
}

// ---------------------------------------------------------------------------
// Render
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::Render( const GLenum mode )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	SetLight( );
	GlLoadMatrix( GetCamera( )->_Matrix ); // カメラのマトリックスをロード

	// グラウンドのみ表示
	Render::Ground( );

	// ジョイント名前IDを初期化
	s32 name_id = 0;

	// セレクトモードは名前をつける
	if( mode == GL_SELECT ){
		glLoadName( name_id );
	}

	// 骨を表示
	Render::Bone( METER( 1.0f ), METER( 0.3f ), METER( 0.15f ) );

	// 骨がピックされたら軸を表示
	if( PICK_NO_USE != CheckPickId( name_id++ ) ){
		Render::Axis( METER( 1.0f ) );
	}
}

// ---------------------------------------------------------------------------
// Paint
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::paintGL( )
{
	Render( GL_RENDER );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::timerEvent( QTimerEvent* )
{
	updateGL( );

	// FPS -> 割り込み 変換 75HZ のモニターに対応出来ない…
	//	wglSwapIntervalEXT( 60 / _FPS );
	s32 dv = gfl::core::Time::GFL_MICRO_SECOND / _TargetFPS - _Time.Elapsed( );
	if(0<dv){
		s32 wait = gfl::core::Time::GFL_MICRO_SECOND / dv;
//		if( 0 < wait ){
		gfl::core::Time::SleepMilliSecond( gfl::core::Time::MicroSecondToMilliSecond( dv ) );
//		}
	}

	dv = _Time.Elapsed( );
	if(0<dv){
		_CurrentFPS = gfl::core::Math::RoundFloat( ( f32 )gfl::core::Time::GFL_MICRO_SECOND / ( f32 )dv );
	}
	_Time.Start( );
}

// =============================================================================
// Mouse section
// =============================================================================
// ---------------------------------------------------------------------------
// calc mouse move vector
// ---------------------------------------------------------------------------
gfl::core::Vector gfl::qt::GLWidget::CalcMouseVector( const s32 x, const s32 y )
{
	gfl::core::Matrix mat;

	mat.SetUnit( );
	gfl::core::Camera* cam = GetCamera( );

	mat.SetRotateY( cam->_Rotation.GetY( ) );
	mat.RotateX( cam->_Rotation.GetX( ) );

	mat.Trans( static_cast < f32 >( x ) * CENTI( -2.0f ), static_cast < f32 >( y ) * CENTI( 2.0f ), 0.0f );
	return mat.GetTrans( );
}
// ---------------------------------------------------------------------------
// mouse press
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::mousePressEvent( QMouseEvent *event )
{
	_LastPos = event->pos( );
	gfl::core::Camera* cam = GetCamera( );
	if( ( event->buttons( ) & m_PickButton ) && !cam->_Mode ){
		s32 id = Select( event->x( ), event->y( ) );
		if( 0 <= id ){
			s32 i = CheckPickId( id );
			if( 0 <= i ){ // もう登録されている
				_PickId[i] = PICK_NO_USE;
			} else if( _ShiftMode ){
				AddPickId( id );
			} else {
				SetPickId( id );
			}
		}
	}
}

// ---------------------------------------------------------------------------
// mouse release
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::mouseReleaseEvent( QMouseEvent *event )
{
	if( !( event->buttons( ) & Qt::LeftButton ) ){
		ClearPickId( );
	}
}

// ---------------------------------------------------------------------------
// mouse moved
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::mouseMoveEvent( QMouseEvent *event )
{
	gfl::core::Camera* cam = GetCamera( );
	if( gfl::core::Camera::MODE_ORBIT != cam->_Mode ){
		return;
	}

	const s32 x = event->x( ) - _LastPos.x( );
	const s32 y = event->y( ) - _LastPos.y( );
	_LastPos = event->pos( );

	if( event->buttons( ) & Qt::MidButton ){ // 注視点移動
		gfl::core::Vector v = CalcMouseVector( x, y );
		cam->_Trans += v;
		cam->_Interest += v;
	} else {
		if( event->buttons( ) & Qt::LeftButton ){ // カメラ移動
			static const f32 MOUSE_MOVE_SPEED = -0.03f;
			cam->AddRotationY( static_cast < f32 >( x ) * MOUSE_MOVE_SPEED );
			cam->AddRotationX( static_cast < f32 >( y ) * MOUSE_MOVE_SPEED );
		} else if( event->buttons( ) & Qt::RightButton ){ // カメラと注視点の距離
			static const f32 MOUSE_MOVE_SPEED = METER( -0.03f );
			cam->AddDistance( static_cast < f32 >( y ) * MOUSE_MOVE_SPEED );
			cam->AddDistance( static_cast < f32 >( x ) * MOUSE_MOVE_SPEED );
		}

		cam->SetTransByInterest( );
		cam->SetRotationByInterest( );
	}

	cam->SetMatrixByPosition( );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
s32 gfl::qt::GLWidget::CheckPickId( const s32 id )
{
	for( s32 i = 0; i < PICK_MAX; ++i ){
		if( id == _PickId[i] ){
			return i;
		}
	}
	return PICK_NO_USE;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::SetPickId( const s32 id )
{
	_PickId[0] = id;

	for( s32 i = 1; i < PICK_MAX; ++i ){
		_PickId[i] = PICK_NO_USE;
	}
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void gfl::qt::GLWidget::AddPickId( const s32 id )
{
	// まだ登録されていない
	if( PICK_NO_USE == CheckPickId( id ) ){
		for( s32 i = 0; i < PICK_MAX; ++i ){
			if( PICK_NO_USE == _PickId[i] ){
				_PickId[i] = id;
				return;
			}
		}
	}
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
s32 gfl::qt::GLWidget::Select( const s32 x, const s32 y )
{
	GLint hits;
#define MAXSELECT 1024
	GLuint selectBuf[MAXSELECT];

	// セレクトバッファを初期化
	glSelectBuffer( MAXSELECT, selectBuf );
	// セレクトモード
	glRenderMode( GL_SELECT );
	// セレクト名の初期化
	glInitNames( );
	glPushName( -1 );

	glPushMatrix( );

	// 現在のビューポート
	s32 vp[4];
	glGetIntegerv( GL_VIEWPORT, vp );

	glMatrixMode( GL_PROJECTION );

	glLoadIdentity( );
	// これがミソ！
	gluPickMatrix( x - 1, vp[3] - y - 1, 5, 5, vp );
	gfl::core::Camera* cam = GetCamera( );
	gluPerspective( cam->_Perspective, ( f32 )vp[2] / ( f32 )vp[3], cam->_NearClip, cam->_FarClip );

	glMatrixMode( GL_MODELVIEW );

	Render( GL_SELECT );

	glPopMatrix( );

	hits = glRenderMode( GL_RENDER );

	// なぜかプロジェクションマトリックスが壊されるので元に戻す
	resizeGL( vp[2], vp[3] );
	if( hits <= 0 ){
		return -1;
	}
	return selectBuf[( hits - 1 )*4 + 3];
}

#ifdef USE_GLEW
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool gfl::qt::GLWidget::LoadShaderSource( GLuint shader, const QString& fname )
{
	QByteArray src( gfl::qt::File::Load( fname ) );
	GLsizei length = src.size( );
	if( length ){
		const GLchar *source = src.data( );
		glShaderSource( shader, 1, &source, &length );
		return true;
	}
	return false;
}
void gfl::qt::GLWidget::PrintShaderInfoLog( GLuint shader )
{
	GLsizei bufSize;

	glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &bufSize );

	if( bufSize > 1 ){
		GLchar *infoLog;

		infoLog = ( GLchar * )malloc( bufSize );

		GFL_ASSERT_MSG( infoLog, "Could not allocate InfoLog buffer.\n" );

		{
			GLsizei length;

			glGetShaderInfoLog( shader, bufSize, &length, infoLog );
			gfl::core::Debug::PrintConsole( "InfoLog:\n%s\n\n", infoLog );
			free( infoLog );
		}
	}
}
void gfl::qt::GLWidget::PrintProgramInfoLog( GLuint program )
{
	GLsizei bufSize;

	glGetProgramiv( program, GL_INFO_LOG_LENGTH, &bufSize );

	if( bufSize > 1 ){
		GLchar *infoLog;

		infoLog = ( GLchar * )malloc( bufSize );

		GFL_ASSERT_MSG( infoLog, "Could not allocate InfoLog buffer.\n" );

		{
			GLsizei length;

			glGetProgramInfoLog( program, bufSize, &length, infoLog );
			gfl::core::Debug::PrintConsole( "InfoLog:\n%s\n\n", infoLog );
			free( infoLog );
		}
	}
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
u32 gfl::qt::GLWidget::LoadShader( const QString& vert_fname, const QString& frag_fname )
{
	GLuint gl2Program;

	/* シェーダオブジェクトの作成 */
	GLuint vertShader = glCreateShader( GL_VERTEX_SHADER );
	GLuint fragShader = glCreateShader( GL_FRAGMENT_SHADER );

	if( !gfl::qt::GLWidget::LoadShaderSource( vertShader, vert_fname ) ){
		GFL_ASSERT_MSG( 0, "Fail to load [vertex shader] %s\n", vert_fname.toAscii( ).data( ) );
	}
	if( !gfl::qt::GLWidget::LoadShaderSource( fragShader, frag_fname ) ){
		GFL_ASSERT_MSG( 0, "Fail to load [fragment shader] %s\n", frag_fname.toAscii( ).data( ) );
	}
	/* シェーダプログラムのコンパイル／リンク結果を得る変数 */
	GLint compiled;

	/* バーテックスシェーダのソースプログラムのコンパイル */
	glCompileShader( vertShader );
	glGetShaderiv( vertShader, GL_COMPILE_STATUS, &compiled );
	gfl::qt::GLWidget::PrintShaderInfoLog( vertShader );
	GFL_ASSERT_MSG( compiled != GL_FALSE, "Compile error in vertex shader.\n" );

	/* フラグメントシェーダのソースプログラムのコンパイル */
	glCompileShader( fragShader );
	glGetShaderiv( fragShader, GL_COMPILE_STATUS, &compiled );
	gfl::qt::GLWidget::PrintShaderInfoLog( fragShader );
	GFL_ASSERT_MSG( compiled != GL_FALSE, "Compile error in fragment shader.\n" );

	/* プログラムオブジェクトの作成 */
	gl2Program = glCreateProgram( );

	/* シェーダオブジェクトのシェーダプログラムへの登録 */
	glAttachShader( gl2Program, vertShader );
	glAttachShader( gl2Program, fragShader );

	/* シェーダオブジェクトの削除 */
	glDeleteShader( vertShader );
	glDeleteShader( fragShader );

	/* シェーダプログラムのリンク */
	glLinkProgram( gl2Program );

	GLint linked;
	glGetProgramiv( gl2Program, GL_LINK_STATUS, &linked );
	gfl::qt::GLWidget::PrintProgramInfoLog( gl2Program );
	GFL_ASSERT_MSG( linked != GL_FALSE, "Link error.\n" );

	/* シェーダプログラムの適用 */
	glUseProgram( gl2Program );

	return gl2Program;
}
#endif
//==============================================================================
//==============================================================================
gfl::qt::GLWidget::Project::Project( )
{
	l_x = gfl::core::Math::F32_MAX;
	h_x = gfl::core::Math::F32_MIN;
	l_y = gfl::core::Math::F32_MAX;
	h_y = gfl::core::Math::F32_MIN;
	l_z = gfl::core::Math::F32_MAX;
	h_z = gfl::core::Math::F32_MIN;
}

//==============================================================================
//==============================================================================
void gfl::qt::GLWidget::Project::SetProject( const gfl::core::Vector& pos, GLint viewport[], GLdouble modelview[], GLdouble projection[] )
{
	double x, y, z;

	gluProject( pos.GetX( ), pos.GetY( ), pos.GetZ( ), modelview, projection, viewport, &x, &y, &z );

	if( x < l_x ) l_x = x;
	if( h_x < x ) h_x = x;
	if( y < l_y ) l_y = y;
	if( h_y < y ) h_y = y;
	if( z < l_z ) l_z = z;
	if( h_z < z ) h_z = z;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::qt::GLWidget::GlMultMatrix(const gfl::core::Matrix& mat)
{
#if GFL_DEFAULT_AOS
	if( sizeof(gfl::core::Matrix) == sizeof(gfl::core::Matrix44)){
		glMultMatrixf( mat.Get( ) );
	}else{
		glMultMatrixf( gfl::core::Matrix44(mat).Get( ) );
	}
#else // 
	gfl::core::MatrixAos44 mat44;
	mat.ToAOS( &mat44 );
	glMultMatrixf( mat44.Get( ) );
#endif
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::qt::GLWidget::GlLoadMatrix(const gfl::core::Matrix44& mat)
{
#if GFL_DEFAULT_AOS
	if( sizeof(gfl::core::Matrix) == sizeof(gfl::core::Matrix44)){
		glLoadMatrixf( mat.Get( ) );
	}else{
		glLoadMatrixf( gfl::core::Matrix44(mat).Get( ) );
	}
#else // 
	glLoadMatrixf( mat.GetTranspose().Get( ) );
#endif
}
