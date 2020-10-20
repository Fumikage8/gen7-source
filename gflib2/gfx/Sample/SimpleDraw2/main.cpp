#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Matrix34.h>
#include <math/include/gfl2_Matrix44.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

//#include <gfx/Sample/Framework/IApplication.h>
#include "../../Sample/Framework2/include/IApplication.h"
#include <gfx/include/gfl2_Surface.h>
#include <gfx/include/gfl2_TimerQuery.h>


#include <stdlib.h>

#define TEST_MODE_STEREO_DRAW 0       // コマンド再利用と立体視描画
#define TEST_MODE_RENDER_TEXTURE 1    // テクスチャに描画するサンプル
#define TEST_MODE_SURFACE_TEXTURE 2   // バックバッファをテクスチャとして使うサンプル

#define TEST_MODE TEST_MODE_SURFACE_TEXTURE

#if defined(NN_PLATFORM_CTR)
#define TEST_CTR
#else
#define TEST_WIN
#endif


#if defined(TEST_CTR)
// @node 仮CTR用インクルード
#include <nn.h>
#include <nn/fs.h>

  // HIO用定義
  #if NN_SWITCH_ENABLE_HOST_IO
  #define TEST_CTR_HIO
  #include <nn/hio.h>
  #endif

#endif

#if defined(TEST_WIN)
#define TEST_LOG(str, ...) { char outputString[1024]; sprintf_s(outputString, sizeof(outputString), str, __VA_ARGS__); OutputDebugStringA(outputString); }
#else
#define TEST_LOG(str, ...) NN_LOG(str, __VA_ARGS__)
#define TEST_LOG2(str) NN_LOG(str)
#endif

using namespace gfl2;
using namespace gfl2::gfx;

#if defined(TEST_WIN)
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
#else
class GLAllocator : public IGLAllocator
{
public:
	GLAllocator():IGLAllocator()
  {
    // デバイスメモリ全体
    this->fcramHeap.Initialize(nn::os::GetDeviceMemoryAddress(), nn::os::GetDeviceMemorySize());

    // VRAMA全体
    this->vramAHeap.Initialize(nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA), nn::gx::GetVramSize(nn::gx::MEM_VRAMA));

    // VRAMB全体
    this->vramBHeap.Initialize(nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB), nn::gx::GetVramSize(nn::gx::MEM_VRAMB));
  }
	virtual ~GLAllocator()
  {
    this->fcramHeap.Finalize();
  }

	virtual void* SystemMemoryMalloc(u32 size, u32 alignment){
    return fcramHeap.Allocate(size, alignment);
	}

	virtual void SystemMemoryFree(void* const address)
	{
    fcramHeap.Free(address);
	}

	virtual void* LocalMemoryMalloc(u32 size, u32 alignment){
    // A, B容量が大きいほうを使う
    if (this->vramAHeap.GetAllocatableSize(alignment) > this->vramBHeap.GetAllocatableSize(alignment))
    {
      return this->vramAHeap.Allocate(size, alignment);
    }
    return this->vramBHeap.Allocate(size, alignment);
	}

	virtual void LocalMemoryFree(void* const address)
	{
    // 個別に開放できない。。
	}

private:
  nn::fnd::ExpHeap fcramHeap;
  nn::fnd::FrameHeap vramAHeap;
  nn::fnd::FrameHeap vramBHeap;
};
#endif

class SimpleDraw : public Sample::Framework::IApplication
{
public:
	// 定数バッファのデータ定義①　変更しないデータ
	struct cbNeverChanges {
		math::Matrix44 Projection;   // 透視変換行列
	};

	// 定数バッファのデータ定義②　変更頻度の低いデータ
	struct cbChangesEveryFrame {
		math::Matrix44  View;  // ビュー変換行列
	};

	// 定数バッファのデータ定義③　変更頻度の高いデータ
	struct cbChangesEveryObject {
		math::Matrix44 World;      // ワールド変換行列
	};

	VertexBuffer				*m_pVertexBuffer;
	IndexBuffer					*m_pIndexBuffer;
	Shader							*m_pVertexShader;   // 頂点シェーダ
	Shader							*m_pPixelShader;    // ピクセル・シェーダ
	ConstantBuffer			*m_pConstantBuffer[4];

  ConstantBuffer      *m_pProjBuffer[3];
  ConstantBuffer      *m_pViewBuffer[3];

	// 定数バッファのデータ
	cbNeverChanges										m_cbNeverChanges;       // 透視変換行列
	cbChangesEveryFrame								m_cbChangesEveryFrame;  // ビュー変換行列　光源座標
	cbChangesEveryObject							m_cbChangesEveryObject; // ワールド変換行列

	BlendStateObject					*m_pBlendStateObject;    // ブレンド・ステート・オブジェクト
	RasterizerStateObject			*m_RasterizerStateObject;// ラスタライザ・ステート・オブジェクト
	DepthStencilStateObject		*m_DepthStencilStateObject;	// 深度/ステンシル・ステート・オブジェクト
	DepthStencilStateObject		*m_DepthStencilStateObject2;	// 深度/ステンシル・ステート・オブジェクト
	SamplerStateObject				*m_SamplerStateObject;				// サンプラー・ステート・オブジェクト
	Texture										*m_pTexture;    // テクスチャ

	u32												m_FrameCnt;
	gfl2::gfx::Texture				*m_pRenderTexture;
	gfl2::gfx::Texture				*m_pDepthStencilTexture;
  gfl2::gfx::DisplayList    *m_pDisplayList;

  gfl2::gfx::TimerQuery     *m_pTimerQuery;

  void*                     m_pHostIoMemory;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0)
	{
		
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
#if defined (TEST_CTR)
    // 仮
    // ファイルシステム初期化
    static const u32 ROMFS_BUFFER_SIZE = 1024 * 64;
    static char buffer[ROMFS_BUFFER_SIZE];
    nn::fs::Initialize();
    nn::fs::MountRom(16, 16, buffer, ROMFS_BUFFER_SIZE);

#if defined(TEST_CTR_HIO)
    // HIO初期化
    m_pHostIoMemory = m_Allocator.SystemMemoryMalloc(nn::hio::WORKMEMORY_SIZE, 128);
    nn::Result initRes = nn::hio::Initialize(m_pHostIoMemory);
    if (initRes.IsFailure())
    {
      NN_LOG("Couldn't init host file!\n");
    }
#endif

#endif
		GFGL::Initialize( &m_Allocator );

		m_HandleList[CtrDisplayNo::LEFT] = GFGL::CreateDisplay(DisplayMode::CTR_LEFT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::RIGHT] = GFGL::CreateDisplay(DisplayMode::CTR_RIGHT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::DOWN] = GFGL::CreateDisplay(DisplayMode::CTR_DOWN, Format::A8R8G8B8, Format::D24S8, MultiSample::None);

		{
			VertexElement element[] = {
				{ 0, StreamUsage::Position,	0, VertexType::Float, 4 },
				{ 0, StreamUsage::TexCoord,	0, VertexType::Float, 4 },
				{ 0, StreamUsage::Color,		0, VertexType::Float, 4 },
				VertexElement::VERTEX_ELEMENT_END()
			};

#if defined(TEST_WIN)
		#if defined(_WIN64)
			//HLSL
			m_pVertexShader = GFGL::CreateVertexShaderFromFile( "../../Sample/data/SimpleDraw.vsh", "VS", element, NULL, FileFormat::ShaderCode );
			m_pPixelShader = GFGL::CreatePixelShaderFromFile( "../../Sample/data/SimpleDraw.psh", "PS", NULL, FileFormat::ShaderCode );
		#else
			//GLSL
			m_pVertexShader = GFGL::CreateVertexShaderFromFile( "../../Sample/data/SimpleDraw.vert", "VS", element, NULL, FileFormat::ShaderCode );
			m_pPixelShader = GFGL::CreatePixelShaderFromFile( "../../Sample/data/SimpleDraw.frag", "PS", NULL, FileFormat::ShaderCode );
		#endif
#else
      //CTR
      nn::fs::FileReader file(L"rom:/shader.shbin");
      u32 fileSize = file.GetSize();
      void* pShaderBinary = m_Allocator.SystemMemoryMalloc(fileSize, 128);
      file.Read(pShaderBinary, fileSize);
      m_pVertexShader = GFGL::CreateVertexShaderFromMemory(pShaderBinary, fileSize, element);
      u32 pixelShaderCommand[256];
      u32 pixelShaderSize = 0;
      {
        nn::gr::Combiner cm;
        cm.stage[0].SetupTexture0();
        cm.stage[1].SetupPrevious();
        cm.stage[2].SetupPrevious();
        cm.stage[3].SetupPrevious();
        cm.stage[4].SetupPrevious();
        cm.stage[5].SetupPrevious();
        u32* endAddr = cm.MakeCommand(pixelShaderCommand);

        // サブルーチン用のコマンド追加
        u32 kickCommandSize = sizeof(u32) * 2; // キックコマンドは8バイト
        endAddr = nn::gr::AddDummyDataForCommandBuffer(endAddr, (endAddr - pixelShaderCommand) * sizeof(u32) + kickCommandSize);
        endAddr = nn::gr::MakeChannelKickCommand(endAddr, nn::gr::CMD_BUF_CH1);

        pixelShaderSize = (endAddr - pixelShaderCommand) * sizeof(u32);
      }
      m_pPixelShader = GFGL::CreatePixelShaderFromMemory(pixelShaderCommand, pixelShaderSize);
#endif
		}

		{
			struct VertexFormat{
				math::Vector				pos;
				math::Vector				uv;
				math::Vector				color;
				VertexFormat( math::Vector pos_, math::Vector uv_, math::Vector color_ )
				{
					pos = pos_;
					uv = uv_;
					color = color_;
				}
			};

			float									vscale = 0.9f;
			static const u32			vertexNum = 4;

			VertexFormat vertexDataArray[vertexNum] = {
				VertexFormat( math::Vector( -vscale,  vscale, 0.0f ), math::Vector(  0.0f, 0.0f, 0.0f ), math::Vector(1.0f,0.0f,0.0f,1.0f) ),
				VertexFormat( math::Vector(  vscale,  vscale, 0.0f ), math::Vector(  1.0f, 0.0f, 0.0f ), math::Vector(0.0f,1.0f,0.0f,1.0f) ),
				VertexFormat( math::Vector( -vscale, -vscale, 0.0f ), math::Vector(  0.0f, 1.0f, 0.0f ), math::Vector(0.0f,0.0f,1.0f,1.0f) ),
				VertexFormat( math::Vector(  vscale, -vscale, 0.0f ), math::Vector(  1.0f, 1.0f, 0.0f ), math::Vector(1.0f,1.0f,1.0f,0.0f) ),
			};

			m_pVertexBuffer = GFGL::CreateVertexBuffer( sizeof(VertexFormat), vertexNum, Usage::WriteOnly, Pool::Default, vertexDataArray );
		}

		{
			u8 idxVertexID[] = { 0, 1, 2, 3 };
			
			m_pIndexBuffer = GFGL::CreateIndexBuffer( sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index8, idxVertexID );
		}

		{
			ConstantBufferElement element[] = {
				{ ConstantType::Matrix, 1 },
				ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};

			m_pConstantBuffer[0] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[1] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[2] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[3] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );

      for (u32 i = 0; i < 3; ++i)
      {
        m_pProjBuffer[i] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
        m_pViewBuffer[i] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
      }
		}

		
#if TEST_MODE == TEST_MODE_RENDER_TEXTURE
		m_pRenderTexture = GFGL::CreateTexture( 256, 256, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
#elif TEST_MODE == TEST_MODE_SURFACE_TEXTURE
		m_pDepthStencilTexture = GFGL::CreateTextureFromSurface(GFGL::GetDepthStencilBuffer( m_HandleList[CtrDisplayNo::LEFT] ));
#endif

		m_pBlendStateObject = GFGL::CreateBlendStateObject();

		m_pBlendStateObject->SetAlphaTestEnable( true );
		m_pBlendStateObject->SetBlendEnable( true );
		m_pBlendStateObject->SetBlendOp( BlendOp::Add );
		m_pBlendStateObject->SetBlendFunc( BlendFunc::SrcAlpha, BlendFunc::InverseSrcAlpha );
		m_pBlendStateObject->UpdateState();

		m_RasterizerStateObject = GFGL::CreateRasterizerStateObject();
		m_RasterizerStateObject->SetCullMode( CullMode::CCW );
		//m_RasterizerStateObject->SetFillMode( FillMode::Line );
		m_RasterizerStateObject->UpdateState();

		m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
		m_DepthStencilStateObject->SetDepthTestEnable(false);
		m_DepthStencilStateObject->SetDepthWriteEnable(true);
		m_DepthStencilStateObject->SetStencilOp(PolygonFace::CW, StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);
		m_DepthStencilStateObject->SetStencilFunc(PolygonFace::CW, CompareFunc::Always, 0x00, 0xFF);
    m_DepthStencilStateObject->SetStencilTestEnable(true);
    m_DepthStencilStateObject->SetStencilWriteMask(0xFF);
		m_DepthStencilStateObject->UpdateState();

		m_DepthStencilStateObject2 = GFGL::CreateDepthStencilStateObject();
		m_DepthStencilStateObject2->SetDepthTestEnable(false);
		m_DepthStencilStateObject2->SetDepthWriteEnable(false);
		m_DepthStencilStateObject2->SetStencilTestEnable(false);
		m_DepthStencilStateObject2->SetStencilWriteMask(0);
		m_DepthStencilStateObject2->UpdateState();

		m_SamplerStateObject = GFGL::CreateSamplerStateObject();
		m_SamplerStateObject->UpdateState();

#if 1 // 3DSテスト用
    m_pBlendStateObject->SetAlphaTestEnable(false);
    m_pBlendStateObject->SetBlendEnable(false);
    m_pBlendStateObject->UpdateState();

    m_RasterizerStateObject->SetCullMode(CullMode::None);
    m_RasterizerStateObject->UpdateState();
#endif

		// **********************************************************



		ImageData				image;
#if defined(TEST_WIN)
		//image.ReadFromFile( "C:/home/niji/git_gflib2/gfx/Sample/data/dxt1_mip.dds" );
		image.ReadFromFile( "C:/home/niji/git_gflib2/gfx/Sample/data/rgba8.tga", ImageData::SwapType::OpenGL );
#else
    nn::fs::FileReader texFile(L"rom:/rgba8.ctpk");
    
    u32 fileSize = texFile.GetSize();

    void* fileBuf = m_Allocator.SystemMemoryMalloc(fileSize, 128);
    texFile.Read(fileBuf, fileSize);

    image.ReadFromMemory(static_cast<u8*>(fileBuf));
#endif

		m_pTexture = GFGL::CreateTextureFromImage( image, Pool::Default );


    // ************************************************************
    m_pDisplayList = GFGL::CreateDisplayList();

    m_pTimerQuery = GFGL::CreateTimerQuery();

#if defined(TEST_CTR)
    // 立体視設定
    GFGL::SetStereoModeEnable(true);
#endif
		return true;
	}

	virtual bool OnUpdate( void )
	{
    u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[CtrDisplayNo::LEFT]);
    u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[CtrDisplayNo::LEFT]);

		{// 定数バッファ①を更新

			// 射影変換行列(パースペクティブ(透視法)射影)
			m_cbNeverChanges.Projection = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
          (f32)displayWidth / (f32)displayHeight,
					1.1f,							// 前方投影面までの距離
					123.0f);							// 後方投影面までの距離

      m_pProjBuffer[CtrDisplayNo::LEFT]->Update(&m_cbNeverChanges.Projection, sizeof(m_cbNeverChanges.Projection));
      m_pProjBuffer[CtrDisplayNo::RIGHT]->Update(&m_cbNeverChanges.Projection, sizeof(m_cbNeverChanges.Projection));

      // 下画面用はピボットさせる
      math::Matrix44 downProj = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
          (f32)displayWidth / (f32)displayHeight,
					1.1f,							// 前方投影面までの距離
					123.0f,							// 後方投影面までの距離
          true);              // ピボット

      m_pProjBuffer[CtrDisplayNo::DOWN]->Update(&downProj, sizeof(downProj));
		}

		{// 定数バッファ②を更新
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 2.0f, -4.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 0.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_cbChangesEveryFrame.View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
      for (u32 i = 0; i < CtrDisplayNo::NumberOf; ++i)
      {
        m_pViewBuffer[i]->Update(&m_cbChangesEveryFrame.View, sizeof(m_cbChangesEveryFrame.View));
      }
		}

		{// 定数バッファ③を更新
			// ワールド変換行列
			math::Matrix34		matY, matX;
			f32						rotate;

			rotate = (f32)(PI * (m_FrameCnt % 3000)) / 1500.0f;
			matY = math::Matrix34::GetRotationY(rotate);
			rotate = (f32)(PI * (m_FrameCnt % 1500)) / 750.0f;
			matX = math::Matrix34::GetRotationX(rotate);

			m_cbChangesEveryObject.World = matY * matX;
			m_pConstantBuffer[2]->Update(&m_cbChangesEveryObject, sizeof(cbChangesEveryObject));

      m_cbChangesEveryObject.World[0].SetW(-1);
      m_cbChangesEveryObject.World[1].SetW(1);
      m_pConstantBuffer[3]->Update(&m_cbChangesEveryObject, sizeof(cbChangesEveryObject));
		}


    // 立体視用の行列作成
#if defined(TEST_CTR)
    {
      static f32 depthLevel = 4.0f;
      static f32 factor = 1.0f;

      math::Matrix44 projL = math::Matrix44::GetIdentity();
      math::Matrix44 viewL = math::Matrix44::GetIdentity();
      math::Matrix44 projR = math::Matrix44::GetIdentity();
      math::Matrix44 viewR = math::Matrix44::GetIdentity();

      GFGL::GetStereoMatrix(
        m_cbNeverChanges.Projection, (*(math::Matrix34*)&m_cbChangesEveryFrame.View), depthLevel, factor, 
        &projL, (math::Matrix34*)&viewL, &projR, (math::Matrix34*)&viewR);

      m_pProjBuffer[CtrDisplayNo::LEFT]->Update(&projL, sizeof(projL));
      m_pProjBuffer[CtrDisplayNo::RIGHT]->Update(&projR, sizeof(projR));

      m_pViewBuffer[CtrDisplayNo::LEFT]->Update(&viewL, sizeof(viewL));
      m_pViewBuffer[CtrDisplayNo::RIGHT]->Update(&viewR, sizeof(viewR));
    }
#endif

		++m_FrameCnt;
    //m_FrameCnt = 10374; // 見た目よかったのでここで固定

    // GPU時間表示
    {
      f32 cpuTime = GFGL::GetTotalCPUTimeMicroSecond() / 1000.0f;
      f32 cpuPer60fps = (100.0f * cpuTime) / (1000.0f / 60.0f);
      f32 gpuTime = GFGL::GetTotalGPUTimeMicroSecond() / 1000.0f;
      f32 gpuPer60fps = (100.0f * gpuTime) / (1000.0f / 60.0f);

      TEST_LOG("*Total CPU : %.3fms, %.3f%% | GPU : %.3fms, %.3f%%\n", cpuTime, cpuPer60fps, gpuTime, gpuPer60fps);
    }

    {
      f32 cpuTime = (m_pTimerQuery->GetCPUElapsedTimeMicroSeconds() / 1000.0f);
      f32 cpuPer60fps = (100.0f * cpuTime) / (1000.0f / 60.0f);
      f32 gpuTime = (m_pTimerQuery->GetGPUElapsedTimeMicroSeconds() / 1000.0f);
      f32 gpuPer60fps = (100.0f * gpuTime) / (1000.0f / 60.0f);

      TEST_LOG("-Query CPU : %.3fms, %.3f%% | GPU : %.3fms, %.3f%%\n", cpuTime, cpuPer60fps, gpuTime, gpuPer60fps);
    }

		return true;
	}

#if TEST_MODE == TEST_MODE_STEREO_DRAW
	virtual bool OnDraw( void )
	{
    GFGL::SetActiveDisplay( m_HandleList[CtrDisplayNo::LEFT] );

    // 初回用に適当にフレームバッファ設定しておく
		{
      Surface*			pBackBuffer = GFGL::GetBackBuffer( m_HandleList[CtrDisplayNo::LEFT] );
      Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[CtrDisplayNo::LEFT] );

      //オフスクリーンへのレンダリング
			GFGL::SetRenderTarget( pBackBuffer );
			GFGL::SetDepthStencil( pDepthBuffer );

			GFGL::BeginScene();
			GFGL::EndScene();
		}

    // ディスプレイリストにコマンドを記録
    GFGL_BEGIN_DISPLAY_LIST(m_pDisplayList);
    {
			GFGL::SetBlendState( m_pBlendStateObject );
      GFGL::SetDepthStencilState(m_DepthStencilStateObject2);

			GFGL::SetVertexShader( m_pVertexShader );
			GFGL::SetPixelShader( m_pPixelShader );

			//GFGL::SetVertexShaderConstant( 0, m_pConstantBuffer[0] ); // 立体視用に射影行列は後で設定
			//GFGL::SetVertexShaderConstant( 1, m_pConstantBuffer[1] ); // 立体視用にビュー行列は後で設定
			GFGL::SetVertexShaderConstant( 2, m_pConstantBuffer[2] );

			GFGL::SetTexture( 0, m_pTexture );
			GFGL::SetSamplerState( 0, m_SamplerStateObject );

			GFGL::SetIndices( m_pIndexBuffer );
			GFGL::SetStreamSource( 0, m_pVertexBuffer );

			//　描画
			GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

      // もう一枚
      GFGL::SetVertexShaderConstant(2, m_pConstantBuffer[3]);
      GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);
    }
    GFGL_END_DISPLAY_LIST(m_pDisplayList);

    static const Color sc_ClearColorList[] =
    {
      Color(0.3f, 0.3f, 0.3f, 1.0f),
      Color(0.3f, 0.3f, 0.3f, 1.0f),
      Color(0.5f, 0.5f, 0.5f, 1.0f),
    };

    for (u32 i = 0; i < CtrDisplayNo::NumberOf; ++i)
		{
      GFGL::SetActiveDisplay( m_HandleList[i] );

      Surface* pBackBuffer = GFGL::GetBackBuffer( m_HandleList[i] );
      Surface* pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[i] );

      //フロントバッファへのレンダリング
			GFGL::SetRenderTarget( pBackBuffer );
			GFGL::SetDepthStencil( pDepthBuffer );

			GFGL::BeginScene();

      // ビューポートとシザーテストの設定
      u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[i]);
      u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[i]);

			GFGL::SetViewport(0, 0, displayWidth, displayHeight);
			GFGL::SetScissor(0, 0, displayWidth, displayHeight);

			GFGL::ClearRenderTarget( pBackBuffer, sc_ClearColorList[i]);
			GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );

      // 射影行列とビュー行列設定
			GFGL::SetVertexShaderConstant( 0, m_pProjBuffer[i] );
			GFGL::SetVertexShaderConstant( 1, m_pViewBuffer[i] );

      // ディスプレイリストを使いまわす
      if (i == 0)
      {
        // 処理負荷計測テスト
        GFGL::BeginTimerQuery(m_pTimerQuery);
        GFGL::CallDisplayList(m_pDisplayList);
        GFGL::EndTimerQuery(m_pTimerQuery);
      }
      else
      {
        GFGL::CallDisplayList(m_pDisplayList);
      }

			GFGL::EndScene();

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();

		}

		GFGL::SwapDisplay();

		return true;
	}
#elif TEST_MODE == TEST_MODE_RENDER_TEXTURE
	virtual bool OnDraw( void )
	{
    GFGL::SetActiveDisplay( m_HandleList[CtrDisplayNo::LEFT] );

    //オフスクリーンへのレンダリング
		{
      Surface*			pBackBuffer = GFGL::GetBackBuffer( m_HandleList[CtrDisplayNo::LEFT] );
      Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[CtrDisplayNo::LEFT] );

			GFGL::SetRenderTarget( m_pRenderTexture->GetSurface() );
			GFGL::SetDepthStencil( pDepthBuffer );//深度バッファは使いまわす

			GFGL::BeginScene();

			GFGL::ClearRenderTarget( Color::GetBisque());
			GFGL::ClearDepthStencil( 1.0f, 255 );

			GFGL::SetViewport(0, 0, 256, 256);
			GFGL::SetScissor(0, 0, 256, 256);

			GFGL::SetVertexShader( m_pVertexShader );
			GFGL::SetPixelShader( m_pPixelShader );

			GFGL::SetVertexShaderConstant( 0, m_pProjBuffer[0] );
			GFGL::SetVertexShaderConstant( 1, m_pViewBuffer[0] );
			GFGL::SetVertexShaderConstant( 2, m_pConstantBuffer[2] );

			GFGL::SetBlendState( m_pBlendStateObject );
			GFGL::SetRasterizerState( m_RasterizerStateObject );
			GFGL::SetDepthStencilState( m_DepthStencilStateObject );
			
			GFGL::SetTexture( 0, m_pTexture );
			GFGL::SetSamplerState( 0, m_SamplerStateObject );//SetTexture後に行う

			GFGL::SetIndices( m_pIndexBuffer );
			GFGL::SetStreamSource( 0, m_pVertexBuffer );
	
			//　描画
			GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

			GFGL::EndScene();
		}

    static const Color sc_ClearColorList[] =
    {
      Color(0.3f, 0.3f, 0.3f, 1.0f),
      Color(0.3f, 0.3f, 0.3f, 1.0f),
      Color(0.5f, 0.5f, 0.5f, 1.0f),
    };

		{
      u32 i = 0;
      GFGL::SetActiveDisplay( m_HandleList[i] );

      Surface* pBackBuffer = GFGL::GetBackBuffer( m_HandleList[i] );
      Surface* pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[i] );

			GFGL::SetRenderTarget( pBackBuffer );
			GFGL::SetDepthStencil( pDepthBuffer );

			GFGL::BeginScene();

      // ビューポートとシザーテストの設定
      u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[i]);
      u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[i]);

			GFGL::SetViewport(0, 0, displayWidth, displayHeight);
			GFGL::SetScissor(0, 0, displayWidth, displayHeight);

			GFGL::ClearRenderTarget( sc_ClearColorList[i]);
			GFGL::ClearDepthStencil( 1.0f, 255 );

			GFGL::SetBlendState( m_pBlendStateObject );
      GFGL::SetDepthStencilState(m_DepthStencilStateObject2);

			GFGL::SetVertexShader( m_pVertexShader );
			GFGL::SetPixelShader( m_pPixelShader );

			GFGL::SetVertexShaderConstant( 0, m_pProjBuffer[0] );
			GFGL::SetVertexShaderConstant( 1, m_pViewBuffer[0] );
			GFGL::SetVertexShaderConstant( 2, m_pConstantBuffer[2] );

			GFGL::SetTexture( 0, m_pRenderTexture );
			GFGL::SetSamplerState( 0, m_SamplerStateObject );

			GFGL::SetIndices( m_pIndexBuffer );
			GFGL::SetStreamSource( 0, m_pVertexBuffer );

			//　描画
			GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

      // もう一枚
      GFGL::SetVertexShaderConstant(2, m_pConstantBuffer[3]);
      GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);


			GFGL::EndScene();

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();

		}

		GFGL::SwapDisplay();

		return true;
  }
#elif TEST_MODE == TEST_MODE_SURFACE_TEXTURE
	virtual bool OnDraw( void )
	{
    GFGL::SetActiveDisplay( m_HandleList[CtrDisplayNo::LEFT] );

    static const Color sc_ClearColorList[] =
    {
      Color(0.3f, 0.3f, 0.3f, 1.0f),
      Color(0.3f, 0.3f, 0.3f, 1.0f),
      Color(0.5f, 0.5f, 0.5f, 1.0f),
    };

		{
      u32 i = 0;
      GFGL::SetActiveDisplay( m_HandleList[i] );

      Surface* pBackBuffer = GFGL::GetBackBuffer( m_HandleList[i] );
      Surface* pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[i] );

			GFGL::SetRenderTarget( pBackBuffer );
			GFGL::SetDepthStencil( pDepthBuffer );

			GFGL::BeginScene();

      // ビューポートとシザーテストの設定
      u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[i]);
      u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[i]);

			GFGL::SetViewport(0, 0, displayWidth, displayHeight);
			GFGL::SetScissor(0, 0, displayWidth, displayHeight);

			GFGL::ClearRenderTarget( sc_ClearColorList[i]);
			GFGL::ClearDepthStencil( 0.0f, 200 ); // ステンシルバッファをテクスチャとして使う。ここのステンシル値がカラーとして赤くレンダリングされるはず

			GFGL::SetBlendState( m_pBlendStateObject );
      GFGL::SetDepthStencilState(m_DepthStencilStateObject2);

			GFGL::SetVertexShader( m_pVertexShader );
			GFGL::SetPixelShader( m_pPixelShader );

			GFGL::SetVertexShaderConstant( 0, m_pProjBuffer[0] );
			GFGL::SetVertexShaderConstant( 1, m_pViewBuffer[0] );
			GFGL::SetVertexShaderConstant( 2, m_pConstantBuffer[2] );

			GFGL::SetTexture( 0, m_pDepthStencilTexture );
			GFGL::SetSamplerState( 0, m_SamplerStateObject );

			GFGL::SetIndices( m_pIndexBuffer );
			GFGL::SetStreamSource( 0, m_pVertexBuffer );

			//　描画
			GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

      // もう一枚
      GFGL::SetVertexShaderConstant(2, m_pConstantBuffer[3]);
      GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);


			GFGL::EndScene();

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();

		}

		GFGL::SwapDisplay();

		return true;
  }
#endif

	virtual bool OnTerm( void )
	{
		GFGL::Finish();

		delete m_pRenderTexture;
		delete m_pTexture;
		delete m_SamplerStateObject;
		delete m_DepthStencilStateObject;
		delete m_RasterizerStateObject;
		delete m_pBlendStateObject;

		delete m_pIndexBuffer;
		delete m_pVertexBuffer;
		delete m_pConstantBuffer[2];
		delete m_pConstantBuffer[1];
		delete m_pConstantBuffer[0];
		delete m_pVertexShader;
		delete m_pPixelShader;

		GFGL::SetActiveDisplay(NULL);

    for (u32 i = 0; i < CtrDisplayNo::NumberOf; ++i)
    {
      GFGL::DestroyDisplay(m_HandleList[i]);
    }
		GFGL::Terminate();

#if defined(TEST_CTR_HIO)
    // HIO終了処理
    nn::hio::Finalize();
    m_Allocator.SystemMemoryFree(m_pHostIoMemory);
#endif
		return true;
	}

private:
	DisplayDeviceHandle					m_HandleList[CtrDisplayNo::NumberOf];
	GLAllocator									m_Allocator;

	//いずれＩＯクラスができたら置き換える。
	static c8* ReadFile( const char* filename, s32* p_size, s32 align = 16 )
	{
#if 0
		FILE		*hFile;
		int			file_size;
		char		*read_buff;

		fopen_s( &hFile, filename, "rb" );

		if ( hFile == NULL ){
			assert(0);
			return NULL;
		}

		fseek( hFile, 0, SEEK_END );
		file_size = ftell( hFile );
		*p_size = file_size;
		fseek( hFile, 0, SEEK_SET );

		file_size += align;//必ずNULLが入るようにしたい。
		u32 buf_size = file_size + ((-file_size) & (align-1));

		read_buff = reinterpret_cast<c8*>( GLMemory::Malloc(buf_size) );
		memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

	
		return( read_buff );
#endif
    return NULL;
	}
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