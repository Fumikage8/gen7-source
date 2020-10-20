#include <stdarg.h>

// gfl2
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_DrawUtilText.h>
#include <util/include/gfl2_UtilTextRenderPath.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)



//-----------------------------------------------------------------------------
/**
 * @brief	DrawUtilText::Impl構造体
 */
//-----------------------------------------------------------------------------
struct DrawUtilText::Impl
{
	enum ShaderType
	{
		VERTEX_COLOR_SHADER = 0,			///!
		TEXTURE_MAP_SHADER,						///!
		RGBTEXTURE_MAP_SHADER,				///!
		TEXTURE_LIGHTING_SHADER,			///!
		FRONT_DOFTEXTURE_MAP_SHADER,	///!
		BACK_DOFTEXTURE_MAP_SHADER,		///!
		DEBUG_SHADER,									///!
		//FILL_SHADER,                ///! フィルスクリーンシェーダー
		SHADER_MAX,

		ENUM_FORCE_DWORD(FragmentOperationEnum)
	};

  Impl( void )
		: m_StackSize(0)
		, m_pHeapAllocator(NULL)
		, m_pDeviceAllocator(NULL)
		, m_TextProj()
		, m_TextMv()
		, m_pDefaultFont(NULL)
		, m_RectDrawer()
		, m_pWorkBuffer(NULL)
		, m_pGraphicsBuffer(NULL)
		, m_pDrawStringBuf(NULL)
    , m_TextColor(1.0f, 1.0f, 1.0f, 1.0f)
    , m_TextScale(1.0f, 1.0f)
		, m_color( 255, 0, 0, 255 )
	{
		;
	}

  s32														m_StackSize;
  heap::NwAllocator**						m_pHeapAllocator;
  heap::NwAllocator**						m_pDeviceAllocator;

	nw::math::MTX44								m_TextProj;
  nw::math::MTX34								m_TextMv;

  nw::font::Font*								m_pDefaultFont;
  nw::font::RectDrawer					m_RectDrawer;
	void*													m_pWorkBuffer;
	void*													m_pGraphicsBuffer;
  nw::font::DispStringBuffer*		m_pDrawStringBuf;
  gfl2::math::Vector4						m_TextColor;
  gfl2::math::Vector2						m_TextScale;
	nw::ut::Color4u8							m_color;
};

//-----------------------------------------------------------------------------
/// @brief	初期化
//-----------------------------------------------------------------------------
void DrawUtilText::Initialize( heap::NwAllocator* pMemAllocator, heap::NwAllocator* pDevAllocator, util::UtilTextRenderPath* pRenderPath )
{

 // s_Gp = GFL_NEW(pMemAllocator->GetHeapBase()) DrawUtilText( pMemAllocator, pDevAllocator );
	m_pRenderPath = pRenderPath;

	m_ppUtf16 = GFL_NEW_ARRAY(pMemAllocator->GetHeapBase()) wchar_t*[mLineMax];

	for( u32 i = 0; i < mLineMax; ++i )
	{
		m_ppUtf16[i] = GFL_NEW_ARRAY( pMemAllocator->GetHeapBase() ) wchar_t[mLineCharMax];
	}
}

//-----------------------------------------------------------------------------
/// @brief	初期化
//-----------------------------------------------------------------------------
void DrawUtilText::Terminate( void )
{
	if( m_ppUtf16 != NULL )
	{
		for( u32 i = 0; i < mLineMax; ++i )
		{
			GFL_SAFE_DELETE_ARRAY( m_ppUtf16[i] );
		}
		GFL_SAFE_DELETE_ARRAY( m_ppUtf16 );
	}

	//GFL_SAFE_DELETE( s_Gp );
}



//-----------------------------------------------------------------------------
/// @brief	コンストラクタ
//-----------------------------------------------------------------------------
DrawUtilText::DrawUtilText( heap::NwAllocator* pMemAllocator, heap::NwAllocator* pDevAllocator, s32 stackMax, u32 charMax, u32 lineMax, u32 lineCharaMax ) :
  // @fix cov_ctr[11954]:未初期化メンバを初期化
  m_ppUtf16( NULL ),
  m_pImpl( NULL ),
  m_pRenderPath( NULL ),
  mpDrawShaderBuff( NULL ),
  mShaderSize( 0 ),
  mLineMax(lineMax),
  mLineCharMax(lineCharaMax)
{
	heap::HeapBase* pHeap = pMemAllocator->GetHeapBase();

	m_pImpl = GFL_NEW(pHeap) Impl();
	
	m_pImpl->m_StackSize = stackMax;
	m_pImpl->m_pHeapAllocator = GFL_NEW(pHeap) heap::NwAllocator*[m_pImpl->m_StackSize];
	m_pImpl->m_pDeviceAllocator = GFL_NEW(pHeap) heap::NwAllocator*[m_pImpl->m_StackSize];

	for( int i=0; i<m_pImpl->m_StackSize; ++i )
	{
		m_pImpl->m_pHeapAllocator[i] = NULL;
		m_pImpl->m_pDeviceAllocator[i] = NULL;
	}

	m_pImpl->m_pHeapAllocator[0] = pMemAllocator;
	m_pImpl->m_pDeviceAllocator[0] = pDevAllocator;

	InitFontRenderer( charMax );
}
//-----------------------------------------------------------------------------
/// @brief	デストラクタ
//-----------------------------------------------------------------------------
DrawUtilText::~DrawUtilText( void )
{
  GFL_ASSERT( m_ppUtf16 == NULL );  //Terminateを呼んでください

  if( m_pImpl->m_StackSize >= 2 )
	{
		GFL_ASSERT_STOP_MSG( m_pImpl->m_pHeapAllocator[1] == NULL, "アロケーターがプッシュされたままです。\n" );
		GFL_ASSERT_STOP_MSG( m_pImpl->m_pDeviceAllocator[1] == NULL, "アロケーターがプッシュされたままです。\n" );
	}

	m_pImpl->m_RectDrawer.Finalize();

	if( m_pImpl->m_pDrawStringBuf )
	{
		m_pImpl->m_pHeapAllocator[0]->Free( m_pImpl->m_pDrawStringBuf );
	}

	if( m_pImpl->m_pGraphicsBuffer )
	{
		m_pImpl->m_pDeviceAllocator[0]->Free( m_pImpl->m_pGraphicsBuffer );
	}

	if( m_pImpl->m_pWorkBuffer )
  {
    m_pImpl->m_pDeviceAllocator[0]->Free( m_pImpl->m_pWorkBuffer );
  }

  GFL_SAFE_DELETE_ARRAY(m_pImpl->m_pHeapAllocator);
  GFL_SAFE_DELETE_ARRAY(m_pImpl->m_pDeviceAllocator);

	GFL_DELETE( m_pImpl );
	m_pImpl = NULL;
}



//-----------------------------------------------------------------------------
/// @brief	レンダーパスの設定
//-----------------------------------------------------------------------------
void DrawUtilText::SetRenderPath( util::UtilTextRenderPath* pRenderPath )
{
	m_pRenderPath = pRenderPath;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtilText::SetFont( nw::font::Font* pFont )
{
	m_pImpl->m_pDefaultFont = pFont;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtilText::SetTextScale( f32 scale_x, f32 scale_y )
{
	m_pImpl->m_TextScale = math::Vector2( scale_x, scale_y );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtilText::SetTextColor( const gfl2::math::Vector4 &color )
{
	m_pImpl->m_TextColor = color;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtilText::DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, ... )
{
	//wchar_t tmp[LINE_CHAR_MAX];
  
  gfl2::heap::HeapBase* heap = m_pImpl->m_pDeviceAllocator[0]->GetHeapBase();
  wchar_t* tmp = GFL_NEW_LOW_ARRAY(heap) wchar_t[mLineCharMax];

	va_list VA;
	va_start( VA, pUtf16 );
	vswprintf( tmp, mLineCharMax, pUtf16, VA );
	va_end( VA );

	DrawText( posX, posY, tmp, m_pImpl->m_TextColor );

  GFL_DELETE_ARRAY( tmp );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtilText::DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl2::math::Vector4 &color, nw::font::Font *pFont )
{
	if( m_pRenderPath != NULL )
	{
		if ( pFont == NULL )
			pFont = m_pImpl->m_pDefaultFont;
  
		if ( pFont == NULL )
			return;

		u32 add_count = m_pRenderPath->GetAddTextCount();
		if( add_count >= mLineMax )
			return;

		UtilTextInfo textInfo;
		u32 length = std::wcslen( pUtf16 ) + 1;
		if( length > mLineCharMax ) length = mLineCharMax;
		memcpy( m_ppUtf16[add_count], pUtf16, (sizeof(wchar_t) * (length-1)) );
		textInfo.pUtf16			= m_ppUtf16[add_count];
		textInfo.pFont			= pFont;
		textInfo.posX				= posX;
		textInfo.posY				= posY;
		textInfo.scale			= m_pImpl->m_TextScale;
		textInfo.startColor	= color;

		m_pRenderPath->AddDrawableText( textInfo,this );
	}
	else {
		GFL_ASSERT_STOP_MSG( false, "RenderPathが未設定" );
	}
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtilText::DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl2::math::Vector4 &startColor, const gfl2::math::Vector4 &endColor, nw::font::Font *pFont )
{
	if( m_pRenderPath != NULL )
	{
		if ( pFont == NULL )
			pFont = m_pImpl->m_pDefaultFont;
  
		if ( pFont == NULL )
			return;

		u32 add_count = m_pRenderPath->GetAddTextCount();
		if( add_count >= mLineMax )
			return;

		UtilTextInfo textInfo;
		u32 length = std::wcslen( pUtf16 ) + 1;
		if( length > mLineCharMax ) length = mLineCharMax;
		memcpy( m_ppUtf16[add_count], pUtf16, (sizeof(wchar_t) * (length-1)) );
		textInfo.pUtf16			= m_ppUtf16[add_count];
		textInfo.pFont			= pFont;
		textInfo.posX				= posX;
		textInfo.posY				= posY;
		textInfo.scale			= m_pImpl->m_TextScale;
		textInfo.startColor = startColor;
		textInfo.endColor		= endColor;
		textInfo.gradation	= true;

		m_pRenderPath->AddDrawableText( textInfo,this );
	}
	else {
		GFL_ASSERT_STOP_MSG( false, "RenderPathが未設定" );
	}
}

//-----------------------------------------------------------------------------
/// @brief	描画テキストの登録をクリア
//-----------------------------------------------------------------------------
void DrawUtilText::ClearDrawText( void )
{
	for( u32 i = 0; i < mLineMax; ++i )
	{
		memset( m_ppUtf16[i], 0x00, sizeof(wchar_t) * mLineCharMax );
	}

	if( m_pRenderPath != NULL ) m_pRenderPath->ClearDrawableText();
}



//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtilText::InitFontRenderer( u32 charMax )
{
#if 0
  static const u8   nwfont_RectDrawerShaderData[] = {
    #include "nwfont_RectDrawerShader.cdat"
  };

	const u32 shaderSize = sizeof(nwfont_RectDrawerShaderData);
	const u32 workBufferSize = nw::font::RectDrawer::GetWorkBufferSize( nwfont_RectDrawerShaderData, shaderSize );
	m_pImpl->m_pWorkBuffer = GFL_NEW( m_pImpl->m_pDeviceAllocator[0]->GetHeapBase()) u8[workBufferSize];
	m_pImpl->m_RectDrawer.Initialize( m_pImpl->m_pWorkBuffer, nwfont_RectDrawerShaderData, shaderSize );
#endif

	heap::NwAllocator* pMemAllocator = m_pImpl->m_pHeapAllocator[0];
	heap::NwAllocator* pDevAllocator = m_pImpl->m_pDeviceAllocator[0];
#if defined(GF_PLATFORM_WIN32)
	// RectDrawerの初期化
	const u32 vtxBufferSize = nw::font::RectDrawer::GetWorkBufferSize( charMax );
	pMemAllocator->Free( m_pImpl->m_pWorkBuffer );
	m_pImpl->m_pWorkBuffer = pDevAllocator->Alloc( vtxBufferSize );
	m_pImpl->m_RectDrawer.Initialize( m_pImpl->m_pWorkBuffer, charMax );

	// 文字列格納バッファ確保
	const u32 drawBufferSize = nw::font::CharWriter::GetDispStringBufferSize( charMax );
	const u32 gfxBufferSize = nw::font::CharWriter::GetGraphicsBufferSize( charMax );

	void* pDispStringBuffer = pMemAllocator->Alloc( drawBufferSize );
	m_pImpl->m_pGraphicsBuffer = pDevAllocator->Alloc( gfxBufferSize );

	m_pImpl->m_pDrawStringBuf = nw::font::CharWriter::InitDispStringBuffer( pDispStringBuffer, m_pImpl->m_pGraphicsBuffer, charMax );
#elif defined(GF_PLATFORM_CTR)

  if(mpDrawShaderBuff){

    const u32 workBufferSize = nw::font::RectDrawer::GetWorkBufferSize( mpDrawShaderBuff, mShaderSize );
    m_pImpl->m_pWorkBuffer = GFL_NEW( pDevAllocator->GetHeapBase()) u8[workBufferSize];
    m_pImpl->m_RectDrawer.Initialize( m_pImpl->m_pWorkBuffer, mpDrawShaderBuff, mShaderSize );

  }
  else{
#if GFL_DEBUG   // @fix GFCHECK[8441]
    // フォント描画用のシェーダを読み込み、nw::font::RectDrawerに設定します。
    nn::fs::FileReader file( L"rom:/data/app/test/fukushima_test/layout_sample_data/shaders/nwfont2_RectDrawerShader.shbin" );
    u32 size = file.GetSize();
    GFL_ASSERT_STOP( size > 0 );

    // バッファ作成 このメモリはlytが管理する。
    pDevAllocator->SetIsAllocLow(true);
    void* buff = pDevAllocator->Alloc( size, 128 );
    GFL_ASSERT_STOP( buff );
    pDevAllocator->SetIsAllocLow(false);

    s32 result = file.Read( buff, size );
    GFL_ASSERT_STOP( result >= 0 );

    const u32 workBufferSize = nw::font::RectDrawer::GetWorkBufferSize( buff, size );
    m_pImpl->m_pWorkBuffer = GFL_NEW( pDevAllocator->GetHeapBase()) u8[workBufferSize];
    m_pImpl->m_RectDrawer.Initialize( m_pImpl->m_pWorkBuffer, buff, size );

    pDevAllocator->Free( buff );
#endif  // GFL_DEBUG
  }
	// 文字列格納バッファ確保
	u32 drawBufferSize = nw::font::CharWriter::GetDispStringBufferSize( charMax );
	void* drawBuffer = pMemAllocator->Alloc( drawBufferSize, nw::font::CharWriter::DISP_STRING_BUFFER_ALIGNMENT );

	u32 graphicsBufferSize = nw::font::CharWriter::GetGraphicsBufferSize( charMax );
	m_pImpl->m_pGraphicsBuffer = pDevAllocator->Alloc( graphicsBufferSize, nw::font::CharWriter::GRAPHICS_BUFFER_ALIGNMENT );

	m_pImpl->m_pDrawStringBuf = nw::font::CharWriter::InitDispStringBuffer( drawBuffer, m_pImpl->m_pGraphicsBuffer, graphicsBufferSize, charMax );
#endif
}



//-----------------------------------------------------------------------------
/// @brief	描画前
//-----------------------------------------------------------------------------
void DrawUtilText::BeginDraw( u32 width, u32 height )
{

  // 左上原点とし、Y軸とZ軸の向きが逆になるように設定します。
  nw::math::MTX44         proj;
  f32 znear   =  0.0f;
  f32 zfar    = -1.0f;
  f32 t       =  0;
  f32 b       =  static_cast<f32>(width);
  f32 l       =  0;
  f32 r       =  static_cast<f32>(height);
#if defined(GF_PLATFORM_WIN32)
	nw::math::MTX44Ortho(&m_pImpl->m_TextProj, t, b, r, l, znear, zfar);
#elif defined( GF_PLATFORM_CTR )
	//nw::math::MTX44OrthoPivot(&s_Gp->m_pImpl->m_TextProj, l, r, b, t, znear, zfar, nw::math::PIVOT_UPSIDE_TO_TOP);
	nw::math::MTX44OrthoPivot(&m_pImpl->m_TextProj, t, b, r, l, znear, zfar, nw::math::PIVOT_UPSIDE_TO_TOP);
#endif

  // モデルビュー行列を単位行列に設定
  nw::math::MTX34 mv;
  nw::math::MTX34Identity(&m_pImpl->m_TextMv);

// CTRのみ：上画面の設定を引き継いでしまうため以下の処理を実行
#if defined(GF_PLATFORM_CTR)
	u32 view_x, view_y;
	view_x = view_y = 0;
	u32 frameBufferHeight = height;

	// FragmentOperation::Add3DCommandと同様の処理をしているはず・・・
	nn::gr::CommandBufferJumpHelper* jh = gfx::GFGL::GetJumpHelper();

	nn::gr::Viewport viewport;
	viewport.Set( view_x, frameBufferHeight - view_y - height, height, width );
	jh->Put( viewport.MakeCommand( jh->GetCurrentPtr() ) );

	nn::gr::Scissor scissor;
	scissor.Set( view_x, frameBufferHeight - view_y - height, height, width );
	jh->Put( scissor.MakeCommand( jh->GetCurrentPtr() ) );
#endif
}

//-----------------------------------------------------------------------------
/// @brief	描画後
//-----------------------------------------------------------------------------
void DrawUtilText::EndDraw( void )
{
}

//-----------------------------------------------------------------------------
/// @brief	描画
//-----------------------------------------------------------------------------
void DrawUtilText::Draw( UtilTextInfo& info )
{

	nw::font::DispStringBuffer* pDrawStringBuf = m_pImpl->m_pDrawStringBuf;

	nw::font::WideTextWriter writer;
  writer.SetDispStringBuffer( pDrawStringBuf );
  writer.SetFont( info.pFont );
  writer.SetCursor(static_cast<f32>(info.posX), static_cast<f32>(info.posY));
	writer.SetScale( info.scale.x, info.scale.y );

	if( !info.gradation )
	{
		writer.SetTextColor(nw::ut::Color8(static_cast<u8>(info.startColor.x * 255.0f), static_cast<u8>(info.startColor.y * 255.0f), static_cast<u8>(info.startColor.z * 255.0f), static_cast<u8>(info.startColor.w * 255.0f)));
	}
	else {
		nw::ut::Color8 col1(static_cast<u8>(info.startColor.x * 255.0f), static_cast<u8>(info.startColor.y * 255.0f), static_cast<u8>(info.startColor.z * 255.0f), static_cast<u8>(info.startColor.w * 255.0f));
		nw::ut::Color8 col2(static_cast<u8>(info.endColor.x * 255.0f), static_cast<u8>(info.endColor.y * 255.0f),   static_cast<u8>(info.endColor.z * 255.0f),   static_cast<u8>(info.endColor.w * 255.0f));
		writer.SetTextColor( col1, col2 );
	}

  writer.StartPrint();
  writer.Print( info.pUtf16 );
  writer.EndPrint();

#if defined(GF_PLATFORM_WIN32)
	m_pImpl->m_RectDrawer.BuildVertexElements( pDrawStringBuf, NULL );
#elif defined(GF_PLATFORM_CTR)
	// 文字列描画を行うコマンドを作成
	m_pImpl->m_RectDrawer.BuildTextCommand( &writer );
#endif

	nw::font::DrawContent drawContent;
	drawContent.projectionMatrix    = &m_pImpl->m_TextProj;
	drawContent.viewMatrix          = &m_pImpl->m_TextMv;
	drawContent.localMatrix					= &nw::math::MTX34::Identity();
	drawContent.dispStringBuffer		= pDrawStringBuf;

	// 描画クラスによる描画処理
#if defined(GF_PLATFORM_WIN32)
	m_pImpl->m_RectDrawer.Draw( drawContent );
#elif defined(GF_PLATFORM_CTR)
	nn::gr::CommandBufferJumpHelper* jh = gfx::GFGL::GetJumpHelper();

	m_pImpl->m_RectDrawer.DrawBegin( *jh );
	m_pImpl->m_RectDrawer.Draw( *jh, drawContent );
	m_pImpl->m_RectDrawer.DrawEnd( *jh );
#endif
}


/**
 * @brief	RectDrawShaderをわたす
 */
void DrawUtilText::SetRectDrawerShader(void* pBuff,int size)
{
  mpDrawShaderBuff=pBuff;
  mShaderSize=size;
}



GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)
