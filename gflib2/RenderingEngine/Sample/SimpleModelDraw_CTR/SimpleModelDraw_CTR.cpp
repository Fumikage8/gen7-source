#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>
#include <gfx/include/gfl2_Surface.h>
#include <gfx/include/gfl2_TimerQuery.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include "gfl_G3dGfModel.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;


#include <stdlib.h>

#if defined(NN_PLATFORM_CTR)
#define TEST_CTR
#else
#define TEST_WIN
#endif

#if defined(TEST_CTR)
// 仮：CTR用インクルード
#include <nn/fs.h>
#include <nn/hio.h>
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
		math::Matrix Projection;   // 透視変換行列
	};

	// 定数バッファのデータ定義②　変更頻度の低いデータ
	struct cbChangesEveryFrame {
		math::Matrix  View;  // ビュー変換行列
	};

	// 定数バッファのデータ定義③　変更頻度の高いデータ
	struct cbChangesEveryObject {
		math::Matrix World;      // ワールド変換行列
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
	SamplerStateObject				*m_SamplerStateObject;				// サンプラー・ステート・オブジェクト
	Texture										*m_pTexture;    // テクスチャ

	u32												m_FrameCnt;
	gfl2::gfx::Texture				*m_pRenderTexture;
  gfl2::gfx::DisplayList    *m_pDisplayLists[2];
  u32                       m_CurrentDisplayList;

  gfl2::gfx::TimerQuery     *m_pTimerQuery;

  void*                     m_pHostIoMemory;


	renderer::DefaultRenderingPipeLine												*m_pRenderingPipeLine;
	renderer::DrawManager																			*m_pDrawManager;
	scenegraph::resource::ResourceNode												*m_pBinModelRootNode;
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode;

	math::Matrix																							m_Projection;
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	gfl::grp::g3d::GfModel																		*m_pGfModel;


	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
    m_CurrentDisplayList(0)
	{
		
	}

	virtual ~SimpleDraw(){}

	struct LocalFunc{
    static void *ReadData( GLAllocator *pAllocator, const char* pFilePath )
    {//自前データロード

      nn::fs::FileReader file( pFilePath );
      u32 fileSize = file.GetSize();
      void* pShaderBinary = pAllocator->SystemMemoryMalloc(fileSize, 128);
      file.Read(pShaderBinary, fileSize);

      return pShaderBinary;
    }
  };

	virtual bool OnInit( void )
	{
#if defined (TEST_CTR)
    // 仮
    // ファイルシステム初期化
    static const u32 ROMFS_BUFFER_SIZE = 1024 * 64;
    static char buffer[ROMFS_BUFFER_SIZE];
    nn::fs::Initialize();
    nn::fs::MountRom(16, 16, buffer, ROMFS_BUFFER_SIZE);

    // HIO初期化
    m_pHostIoMemory = m_Allocator.SystemMemoryMalloc(nn::hio::WORKMEMORY_SIZE, 128);
    nn::Result initRes = nn::hio::Initialize(m_pHostIoMemory);
    if (initRes.IsFailure())
    {
      NN_LOG("Couldn't init host file!\n");
    }

#endif
		GFGL::Initialize( &m_Allocator );

		m_HandleList[CtrDisplayNo::LEFT] = GFGL::CreateDisplay(DisplayMode::CTR_LEFT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::RIGHT] = GFGL::CreateDisplay(DisplayMode::CTR_RIGHT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::DOWN] = GFGL::CreateDisplay(DisplayMode::CTR_DOWN, Format::A8R8G8B8, Format::D24S8, MultiSample::None);

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::Initialize();

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画マネージャの初期化。複数持ってレイヤー化してもよい。
			m_pDrawManager = renderer::DrawManager::Create();

			//描画パイプラインを作成
			m_pRenderingPipeLine = new renderer::DefaultRenderingPipeLine();
		}

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

		
#if 0
		m_pRenderTexture = GFGL::CreateTexture( 1280, 720, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
#else
		m_pRenderTexture = GFGL::CreateTexture( 256, 512, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
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
		m_DepthStencilStateObject->SetDepthTestEnable(true);
		m_DepthStencilStateObject->UpdateState();

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
		image.ReadFromFile( "C:/home/niji/git_gflib2/gfx/Sample/data/rgba8.tga" );
#else
    nn::fs::FileReader texFile(L"rom:/rgba8.ctpk");
    u32 fileSize = texFile.GetSize();

    void* fileBuf = m_Allocator.SystemMemoryMalloc(fileSize, 128);
    texFile.Read(fileBuf, fileSize);

    image.ReadFromMemory(static_cast<u8*>(fileBuf));
#endif

		m_pTexture = GFGL::CreateTextureFromImage( image, Pool::Default );


    // ************************************************************
    m_pDisplayLists[0] = GFGL::CreateDisplayList();
    m_pDisplayLists[1] = GFGL::CreateDisplayList();

    m_pTimerQuery = GFGL::CreateTimerQuery();

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix::GetPerspective(
					math::Util::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					10000.0f);							// 後方投影面までの距離

			m_pDrawManager->SetProjectionMatrix( m_Projection );
		}

		if ( 0 )
		{
			void        *pTextureData[3];
			void        *pVertexShaderData;
			void				*pGfModelData;
    
			pTextureData[0]		= LocalFunc::ReadData( &m_Allocator, "rom:/pm0025_00_BodyA1.3ds.tga");
			pTextureData[1]		= LocalFunc::ReadData( &m_Allocator, "rom:/DummyTex.3ds.tga");
			pTextureData[2]		= LocalFunc::ReadData( &m_Allocator, "rom:/pm0025_00_BodyANor.3ds.tga");

			pVertexShaderData = LocalFunc::ReadData( &m_Allocator, "rom:/PDC_Body_Shadow.3ds.gfvsh");
			pGfModelData			= LocalFunc::ReadData( &m_Allocator, "rom:/pm0025_00.3ds.gfbmdl");
    
			m_pGfModel = new gfl::grp::g3d::GfModel();
			m_pGfModel->SetTexture( 0, pTextureData[0] );
			m_pGfModel->SetTexture( 1, pTextureData[1] );
			m_pGfModel->SetTexture( 2, pTextureData[2] );
			m_pGfModel->SetVertexShader( pVertexShaderData );
			m_pGfModel->SetGfModel( pGfModelData );
		}

		{
			//リソースローダー経由でノードは生成されます。
			scenegraph::resource::GfBinaryMdlData							resourceData;

			s32		size;
			c8		*pBinData = (c8*)LocalFunc::ReadData( &m_Allocator, "rom:/exports/pm0025_00.3ds.gfbmdl" );
			
			resourceData.SetModelData( pBinData );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

			//delete pBinData;
		}

		{
			static const char* pShaderFileList[] = {
				"rom:/exports/Shader/PDC_Body_Shadow.3ds.gfvsh",
				"rom:/exports/Shader/PDC_pm_keikouo_Neon.3ds.gfvsh",
			};

			u32				shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);

			for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
			{
				scenegraph::resource::GfBinaryShdData							resourceData;

				s32		size;
				c8		*pBinData = (c8*)LocalFunc::ReadData( &m_Allocator, pShaderFileList[shaderNo] );

				resourceData.SetShaderData( pBinData );

				scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
				m_pBinModelRootNode->AddChild( pBinShdNode );

				//delete pBinData;
			}
		}

		{
			static const c8*			pTextureFileList[] = {
				"rom:/exports/Texture/pm0025_00_Eye1.3ds.tex",
				"rom:/exports/Texture/pm0025_00_EyeNor.3ds.tex",
				"rom:/exports/Texture/pm0025_00_EyeHlm.3ds.tex",
				"rom:/exports/Texture/pm0025_00_Mouth1.3ds.tex",
				"rom:/exports/Texture/pm0025_00_MouthNor.3ds.tex",
				"rom:/exports/Texture/pm0025_00_MouthHlm.3ds.tex",
				"rom:/exports/Texture/pm0025_00_BodyALta.3ds.tex",
				"rom:/exports/Texture/pm0025_00_BodyA1.3ds.tex",
				"rom:/exports/Texture/DummyTex.3ds.tex",
				"rom:/exports/Texture/pm0025_00_BodyANor.3ds.tex",
				"rom:/exports/Texture/Default_Lta_10D.3ds.tex",
				"rom:/exports/Texture/pm0025_00_Hta.3ds.tex",
				"rom:/exports/Texture/LinerTable.3ds.tex",
				"rom:/exports/Texture/pm0025_00_BodyAHlm.3ds.tex",
				"rom:/exports/Texture/pm0025_00_BodyB1.3ds.tex",
				"rom:/exports/Texture/pm0025_00_BodyBInc.3ds.tex",
				"rom:/exports/Texture/pm0025_00_BodyBNor.3ds.tex",
				"rom:/exports/Texture/pm0025_00_BodyBHlm.3ds.tex"
			};

			u32				texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);

			for( u32 texNo = 0; texNo < texNum; ++texNo )
			{
				scenegraph::resource::GfBinaryTexData							texResourceData;
				scenegraph::resource::ResourceNode								*pBinTexRootNode;

				s32		texsize;
				s32		size;
				c8		*pTexBinData = (c8*)LocalFunc::ReadData( &m_Allocator, pTextureFileList[texNo] );
			
				texResourceData.SetTextureData( pTexBinData );

				//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
				pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
				m_pBinModelRootNode->AddChild( pBinTexRootNode );

				//delete pTexBinData;
			}
		}

		{
			m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode );
		}

		m_pDrawManager->AddObject( m_pModelInstanceNode );

		GFGL::SetStereoModeEnable( false );

#if defined(TEST_CTR)
    // 立体視設定
    GFGL::SetStereoModeEnable(true);
#endif
		return true;
	}

	virtual bool OnUpdate( void )
	{
		{
			// ビュー変換行列
			math::Vector eyePosition( 100.0f, 100.0f, 100.0f, 1.0f );  // 視点(カメラの位置)
			math::Vector focusPosition( 0.0f, 0.0f,  0.0f, 1.0f );  // 注視点
			math::Vector upDirection( 0.0f, 1.0f,  0.0f, 1.0f );  // カメラの上方向

			m_View = math::Matrix::GetLookAt(eyePosition, focusPosition, upDirection);

			m_pDrawManager->SetViewMatrix( m_View );
		}

    u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[CtrDisplayNo::LEFT]);
    u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[CtrDisplayNo::LEFT]);

		{// 定数バッファ①を更新

			// 射影変換行列(パースペクティブ(透視法)射影)
			m_cbNeverChanges.Projection = math::Matrix::GetPerspective(
					math::Util::ConvDegToRad(37.0f),		// 視野角30°
          (f32)displayWidth / (f32)displayHeight,
					1.1f,							// 前方投影面までの距離
					123.0f);							// 後方投影面までの距離
			m_cbNeverChanges.Projection = m_cbNeverChanges.Projection.Transpose();

      m_pProjBuffer[CtrDisplayNo::LEFT]->Update(&m_cbNeverChanges.Projection, sizeof(m_cbNeverChanges.Projection));
      m_pProjBuffer[CtrDisplayNo::RIGHT]->Update(&m_cbNeverChanges.Projection, sizeof(m_cbNeverChanges.Projection));

      // 下画面用はピボットさせる
      math::Matrix downProj = math::Matrix::GetPerspective(
					math::Util::ConvDegToRad(37.0f),		// 視野角30°
          (f32)displayWidth / (f32)displayHeight,
					1.1f,							// 前方投影面までの距離
					123.0f,							// 後方投影面までの距離
          true);              // ピボット
      downProj = downProj.Transpose();

      m_pProjBuffer[CtrDisplayNo::DOWN]->Update(&downProj, sizeof(downProj));
		}

		{// 定数バッファ②を更新
			// ビュー変換行列
			math::Vector eyePosition( 0.0f, 20.0f, 100.0f, 1.0f );  // 視点(カメラの位置)
			math::Vector focusPosition( 0.0f, 20.0f,  0.0f, 1.0f );  // 注視点
			math::Vector upDirection( 0.0f, 1.0f,  0.0f, 1.0f );  // カメラの上方向

			m_cbChangesEveryFrame.View = math::Matrix::GetLookAt(eyePosition, focusPosition, upDirection);
			m_cbChangesEveryFrame.View = m_cbChangesEveryFrame.View.Transpose();
      for (u32 i = 0; i < CtrDisplayNo::NumberOf; ++i)
      {
        m_pViewBuffer[i]->Update(&m_cbChangesEveryFrame.View, sizeof(m_cbChangesEveryFrame.View));
      }
		}

		{// 定数バッファ③を更新
			// ワールド変換行列
			math::Matrix		matY, matX;
			f32						rotate;

			rotate = (f32)(PI * (m_FrameCnt % 3000)) / 1500.0f;
			matY = math::Matrix::GetRotationY(rotate);
			rotate = (f32)(PI * (m_FrameCnt % 1500)) / 750.0f;
			matX = math::Matrix::GetRotationX(rotate);

			m_cbChangesEveryObject.World = matY * matX;
			m_cbChangesEveryObject.World = m_cbChangesEveryObject.World.Transpose();
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

      math::Matrix projL;
      math::Matrix viewL;
      math::Matrix projR;
      math::Matrix viewR;

      GFGL::GetStereoMatrix(
        m_cbNeverChanges.Projection, m_cbChangesEveryFrame.View, depthLevel, factor, 
        &projL, &viewL, &projR, &viewR);

      m_pProjBuffer[CtrDisplayNo::LEFT]->Update(&projL, sizeof(projL));
      m_pProjBuffer[CtrDisplayNo::RIGHT]->Update(&projR, sizeof(projR));

      m_pViewBuffer[CtrDisplayNo::LEFT]->Update(&viewL, sizeof(viewL));
      m_pViewBuffer[CtrDisplayNo::RIGHT]->Update(&viewR, sizeof(viewR));
    }
#endif

		++m_FrameCnt;
    //m_FrameCnt = 10374; // 見た目よかったのでここで固定

    // GPU時間表示
		if ( 0 )
    {
      f32 cpuTime = GFGL::GetTotalCPUTimeMicroSecond() / 1000.0f;
      f32 cpuPer60fps = (100.0f * cpuTime) / (1000.0f / 60.0f);
      f32 gpuTime = GFGL::GetTotalGPUTimeMicroSecond() / 1000.0f;
      f32 gpuPer60fps = (100.0f * gpuTime) / (1000.0f / 60.0f);

      TEST_LOG("*Total CPU : %.3fms, %.3f%% | GPU : %.3fms, %.3f%%\n", cpuTime, cpuPer60fps, gpuTime, gpuPer60fps);
    }

		if ( 0 )
    {
      f32 cpuTime = (m_pTimerQuery->GetCPUElapsedTimeMicroSeconds() / 1000.0f);
      f32 cpuPer60fps = (100.0f * cpuTime) / (1000.0f / 60.0f);
      f32 gpuTime = (m_pTimerQuery->GetGPUElapsedTimeMicroSeconds() / 1000.0f);
      f32 gpuPer60fps = (100.0f * gpuTime) / (1000.0f / 60.0f);

      TEST_LOG("-Query CPU : %.3fms, %.3f%% | GPU : %.3fms, %.3f%%\n", cpuTime, cpuPer60fps, gpuTime, gpuPer60fps);
    }

		return true;
	}

	virtual bool OnDraw( void )
	{
#if defined(TEST_CTR)
    // GPUプロファイラ用のファイルを出力
		static int					s_Cnt = 60;

		if ( s_Cnt == 0 )
		{
			GFGL::ExportOrcsFileRequest("asdf.orcs"); 
		}
		--s_Cnt;
#endif

    GFGL::SetActiveDisplay( m_HandleList[CtrDisplayNo::LEFT] );


		math::Matrix projMat[CtrDisplayNo::NumberOf];
    math::Matrix viewMat[CtrDisplayNo::NumberOf];

		{
			static f32 depthLevel = 4.0f;
			static f32 factor = 1.0f;

			GFGL::GetStereoMatrix(
				m_cbNeverChanges.Projection, m_cbChangesEveryFrame.View, depthLevel, factor, 
				&projMat[CtrDisplayNo::LEFT], &viewMat[CtrDisplayNo::LEFT], &projMat[CtrDisplayNo::RIGHT], &viewMat[CtrDisplayNo::RIGHT]);
		}

    // ディスプレイリストにコマンドを記録
    GFGL_BEGIN_DISPLAY_LIST(m_pDisplayLists[m_CurrentDisplayList]);
    {
			m_pDrawManager->Draw();
			//m_pGfModel->Draw();
    }
    GFGL_END_DISPLAY_LIST(m_pDisplayLists[m_CurrentDisplayList]);

    static const Color sc_ClearColorList[] =
    {
      Color(0.3f, 0.3f, 0.3f, 1.0f),
      Color(0.3f, 0.3f, 0.3f, 1.0f),
      Color(0.5f, 0.5f, 0.5f, 1.0f),
    };

    u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[CtrDisplayNo::LEFT]);
    u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[CtrDisplayNo::LEFT]);

    for (u32 i = 0; i < 2; ++i)
		{
      GFGL::SetActiveDisplay( m_HandleList[i] );

      Surface* pBackBuffer = GFGL::GetBackBuffer( m_HandleList[i] );
      Surface* pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[i] );

      //フロントバッファへのレンダリング
			/*GFGL::SetRenderTarget( pBackBuffer );
			GFGL::SetDepthStencil( pDepthBuffer );

			GFGL::BeginScene();

			GFGL::SetViewport(0, 0, displayWidth, displayHeight);

			GFGL::ClearRenderTarget( pBackBuffer, sc_ClearColorList[i]);
			GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );*/


			m_pRenderingPipeLine->SetProjectionMatrix( projMat[i] );
			m_pRenderingPipeLine->SetViewMatrix( viewMat[i] );	

			m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
			m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );

			m_pRenderingPipeLine->Execute();

/*#if defined(GF_PLATFORM_CTR_TEST)
			{//proj
				nn::math::MTX44						mtx44 = nn::math::MTX44::Identity();
				u32												tempBuffer[1024];
				u32												*pSeek = tempBuffer;

				std::memcpy( &mtx44, &projMat[i], sizeof(nn::math::MTX44) );

				pSeek = nn::gr::CTR::MakeUniformCommandVS( pSeek, 86, mtx44 );

				u32												size = ((u32)pSeek - (u32)tempBuffer) / sizeof(u32);

				GFGL::AddCommandCopy( tempBuffer, size );
			}

			{//view
				nn::math::MTX34						mtx34 = nn::math::MTX34::Identity();
				u32												tempBuffer[1024];
				u32												*pSeek = tempBuffer;

				std::memcpy( &mtx34, &viewMat[i], sizeof(nn::math::MTX34) );

				pSeek = nn::gr::CTR::MakeUniformCommandVS( pSeek, 90, mtx34 );

				u32												size = ((u32)pSeek - (u32)tempBuffer) / sizeof(u32);

				GFGL::AddCommandCopy( tempBuffer, size );
			}
#endif*/

      //GFGL::CallDisplayList(m_pDisplayLists[m_CurrentDisplayList]);
			//m_pDrawManager->Draw();

			//GFGL::EndScene();

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();	
		}

		gfl2::gfx::GFGL::Finish();
		GFGL::SwapDisplay();
    m_CurrentDisplayList ^= 1;

		return true;
	}

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

#if defined(TEST_CTR)
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
