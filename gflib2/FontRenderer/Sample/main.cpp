#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <FontString/include/gfl2_FontString.h>
#include <FontRenderer/include/gfl2_FontRenderer.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>

#include <gfx/Sample/Framework/IApplication.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::fontstring;
using namespace gfl2::fontrenderer;
using namespace gfl2::system;

class GLAllocator : public IGLAllocator
{
public:
	GLAllocator():IGLAllocator(){}
	virtual ~GLAllocator(){}

	virtual void* SystemMemoryMalloc(u32 size, u32 alignment){
		return malloc(size);
	}

	virtual void SystemMemoryFree(void* const address)
	{
		free( address );
	}

	virtual void* LocalMemoryMalloc(u32 size, u32 alignment){
		return NULL;
	}

	virtual void LocalMemoryFree(void* const address)
	{
		
	}
};

class SimpleDraw : public Sample::Framework::IApplication
{
private:

public:
	// 定数バッファのデータ定義①　変更しないデータ
	struct cbNeverChanges {
		math::Matrix InvViewport;   // 逆ビューポート行列
	};

	s16 m_screenWidth;
	s16 m_screenHeight;

	u32	m_FrameCnt;

	FILE* m_fpFileTimer;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0)
	{
	}

	virtual ~SimpleDraw(){}


	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );
		
		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );
		// @attention: 解像度をDisplayModeに応じて設定できるようにする
    m_screenWidth = static_cast<s16>(GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) );
	  m_screenHeight = static_cast<s16>(GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ) );

		//******************************************************************************
		// @attention: ファイルIOが来たら書き換える
		fopen_s(&m_fpFileTimer, "../resource/render_performance.txt", "w");

		FILE*	hFile;
		s32		file_size;
		c8*		read_buff;
		s32		align = 16;

		fopen_s( &hFile, "../resource/output_binary.bin", "rb" );

		if ( hFile == NULL ){
			assert(0);
			return NULL;
		}

		fseek( hFile, 0, SEEK_END );
		file_size = ftell( hFile );
		fseek( hFile, 0, SEEK_SET );

		file_size += align;//必ずNULLが入るようにしたい。
		u32 buf_size = file_size + ((-file_size) & (align-1));

		read_buff = reinterpret_cast<c8*>( malloc(buf_size) );
		memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );
		//******************************************************************************

		// 文字コードを作成
		FontString::Create(read_buff, file_size);
		FontRenderer::Create(m_screenWidth, m_screenHeight);
		return true;
	}

	virtual bool OnUpdate( void )
	{
		FontRenderer::Update();
		++m_FrameCnt;
		return true;
	}

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

		GFGL::SetRenderTarget( pBackBuffer );
		GFGL::SetDepthStencil( pDepthBuffer );

		GFGL::BeginScene();

		GFGL::ClearRenderTarget( pBackBuffer, Color::GetWhite() );
		GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );

		GFGL::SetViewport(0, 0, m_screenWidth, m_screenHeight);

		// 描画前に計測開始
		u64	startMill = Timer::PerformanceCounter::GetMicroSecond();
		// フォントを描画する際には, 必ずBegin()とEnd()の間に描画処理を書いてください
		FontRenderer::Begin();

		u16 fontNum = 0;
		// UTF-16の文字が入っていたらコンバートを掛けてから描画します
		c16 message[] = 
			L"<center>良質な<fontcol=0xFF0000FF>『遊び』</fontcol>を</center><br>"
			L"<center>継続的に作り出すことによって</center><br>"
			L"<center>世界中の人に</center><br>"
			L"<center>楽しさ、面白さ、発見を。</center><br>"
			L"<center>そして</center><br>"
			L"<right>勇気と希望、やさしさ、夢、冒険心</right><br>"
			L"<left>を提供していきます。</left><br>";
		FontString::ConvertUTF16(message, message);
		fontNum += FontRenderer::Draw(message, 100.0f, 100.0f, 30.0f, 40.0f);
		// ASCIIの範囲内であればコンバートせずそのまま描画可能です
		FontRenderer::Draw(
			L"&lt;"
			L"<fontcol=0xFF0000FF>Red</fontcol>"
			L"<fontcol=0x00FF00FF>Green</fontcol>"
			L"<fontcol=0x0000FFFF>Blue</fontcol>"
			L"<fontcol=0xFFFF00FF>Yellow</fontcol>"
			L"<fontcol=0x00FFFFFF>Cyan</fontcol>"
			L"<fontcol=0xFF00FFFF>Magenta</fontcol>"
			L"&gt;",
			0.0f, 400.0f, 30.0f, 40.0f);

    c16 messageRed[] = L"強制的に赤で描画";
    FontString::ConvertUTF16(messageRed, messageRed);
    FontRenderer::SetDefaultFontColor(math::Vector(1.0f, 0.0f, 0.0f, 1.0f));
    FontRenderer::Draw(messageRed, 50.0f, 560.0f, 32.0f, 32.0f);

		GFGL::Finish();
		FontRenderer::End();
		u64 endMill = Timer::PerformanceCounter::GetMicroSecond();
		u64 timeToRenderFont = endMill - startMill;
		fprintf_s(m_fpFileTimer, "frame\t%d\t%d\n", m_FrameCnt, (u32)timeToRenderFont);

		GFGL::EndScene();

		GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
		GFGL::Finish();

		FontRenderer::Destroy();
		FontString::Destroy();

		fclose(m_fpFileTimer);

		GFGL::SetActiveDisplay(NULL);
		GFGL::DestroyDisplay(m_Handle);
		GFGL::Terminate();
		return true;
	}

private:
	DisplayDeviceHandle					m_Handle;
	GLAllocator							m_Allocator;
};

SimpleDraw	*g_pApp = NULL;

Sample::Framework::IApplication* GetApplication()
{
	if( g_pApp == NULL ) {
		g_pApp = new SimpleDraw();
	}
	return g_pApp;
}

void TerminateApplication()
{
	delete g_pApp;
}