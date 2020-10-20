#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <gfx/include/gfl2_DrawUtil.h>

#include <gfx/Sample/Framework/IApplication.h>

using namespace gfl2;
using namespace gfl2::gfx;

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
public:
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

		DrawUtil::Initialize(&m_Allocator, &m_Handle);
		ImageData				imageData;
		imageData.ReadFromFile("C:/home/niji/git_gflib2/gfx/Sample/data/rgba8.tga");
		DrawUtil::SetTexture(imageData);

		return true;
	}

	virtual bool OnUpdate( void )
	{
		// アルファ値を毎フレーム更新
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

		// 簡易描画のシザリング設定(描画前に行うこと)
		//DrawUtil::SetScissor(100, 100, 300, 300);	// シザリング領域設定
		DrawUtil::ClearScissor();									// シザリング解除

		// カリングの設定(描画前に行うこと)
		//DrawUtil::SetCullMode(CullMode::CCW);		// 反時計回り(こちらがデフォルト)
		//DrawUtil::SetCullMode(CullMode::CW);			// 時計周り
		DrawUtil::SetCullMode(CullMode::None);		// 両面描画

		// ワイヤーフレームON/OFFの設定(描画前に行うこと)
		//DrawUtil::SetFillMode(FillMode::Line);			// ワイヤーフレーム描画
		DrawUtil::SetFillMode(FillMode::Fill);			// 通常描画

		GFGL::BeginScene();

		GFGL::ClearRenderTarget( pBackBuffer, Color::GetWhite() );
		GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );

		u32 displayWidth = (GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) );
		u32 displayHeight = (GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ) );
		GFGL::SetViewport(0, 0, displayWidth, displayHeight);

		// 簡易描画開始(簡易描画を始める前に必ず呼ぶ)
		DrawUtil::BeginRender();

		DrawUtil::SetMaterialAlpha(1.0f);
		DrawUtil::SetProjectionMatrix(
			math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					123.0f)							// 後方投影面までの距離
		);

		math::Vector3 eyePosition( 5.0f, 5.0f, -5.0f );  // 視点(カメラの位置)
		math::Vector3 focusPosition( 0.0f, 0.0f,  0.0f );  // 注視点
		math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
		DrawUtil::SetViewMatrix(
			math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection)
		);

		DrawUtil::SetModelMatrix(math::Matrix::GetIdentity());
		// 黒の矩形を描画
		DrawUtil::FillRect(0.0f, 300.0f, 100.0f, 100.0f, math::Vector(0.0f, 0.0f, 0.0f));
		// テクスチャ付きの矩形を描画
		DrawUtil::DrawRect(200.0f, 360.0f, 100.0f, 50.0f);
		// ワールド行列を反映させて赤のAABBを描画
		DrawUtil::SetModelMatrix(math::Matrix::GetTranslation(math::Vector(-1.25f, 0.0f, -2.5f)));
		DrawUtil::DrawAABB(
			math::Vector(0.0f, 0.0f, 0.0f),
			math::Vector(1.0f, 2.0f, 3.0f),
			math::Vector(1.0f, 0.0f, 0.0f));
		// AABBの開始点, 終了点を指定して描画
		DrawUtil::DrawAABB(
			math::AABB(
				math::Vector(-0.5f, -0.5f, -0.5f),
				math::Vector(0.5f, 0.5f, 0.5f)
				));
		// 頂点フォーマットを用意して, 青い三角形を描画
		math::Vector color = math::Vector(0.0f, 0.0f, 1.0f);
		DrawUtil::VertexFormat vertexFormat[] =
		{
			DrawUtil::VertexFormat(	math::Vector( 0.0f, -1.0f, 0.0f),
										math::Vector(color.x, color.y, color.z),
										math::Vector( 0.0f, 0.0f, 0.0f)),
			DrawUtil::VertexFormat(	math::Vector( 1.0f, 0.0f, 0.0f),
										math::Vector(color.x, color.y, color.z),
										math::Vector( 0.0f, 0.0f, 0.0f)),
			DrawUtil::VertexFormat(	math::Vector( 1.0f, -1.0f, 0.0f),
										math::Vector(color.x, color.y, color.z),
										math::Vector(0.0f, 0.0f, 0.0f)),
		};
		DrawUtil::DrawVertices(PrimitiveType::Triangles, vertexFormat, sizeof(vertexFormat) / sizeof(vertexFormat[0]), false);

		// アルファ値を設定して, 画面全体を緑で描画(フェードアウト)
		DrawUtil::SetMaterialAlpha(static_cast<f32>(m_FrameCnt % 300) / 300.0f);
		DrawUtil::FillScreen(math::Vector(0.0f, 1.0f, 0.0f));
		
		// 簡易描画終了(簡易描画を終えたら必ず呼ぶ)
		DrawUtil::EndRender();
		
		GFGL::EndScene();

		GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
		GFGL::Finish();

		DrawUtil::Terminate();

		GFGL::SetActiveDisplay(NULL);
		GFGL::DestroyDisplay(m_Handle);
		GFGL::Terminate();
		return true;
	}

private:
	DisplayDeviceHandle					m_Handle;
	GLAllocator									m_Allocator;
	int													m_FrameCnt;
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