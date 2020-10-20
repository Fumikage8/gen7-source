#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <gfx/include/gfl2_DrawUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>

#include <gfx/include/gfl2_Surface.h>
#include <gfx/include/gfl2_TimerQuery.h>

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <heap/include/gfl2_HeapMacros.h>

#include <fs/include/gfl2_FsSystem.h>

#include <nw/font2.h>

#include <stdlib.h>
#include <nn/fs.h>
#include <nn/hio.h>

using namespace gfl2;
using namespace gfl2::gfx;

static const u32 HEAPID_DEVICE         = 1;

extern "C" void nninitStartUp(void)
{
	static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x60000;
	static const u32 HEAP_MEMORY      = 0x0cc0000 + 0x30000 + FOR_DISABLE_OPTIMIZE_MEMORY;
  static const u32 DEVICE_MEMORY    = 0x02b78000 - 0x30000 + 0x40000;
	static const u32 DEBUG_HEAP          = 0x0100000;

	size_t device_memory_size = DEVICE_MEMORY;
  size_t heap_memory_size = HEAP_MEMORY + DEBUG_HEAP;

  //size_tはunsignd intなので0以下チェックできない。なのでs32にした
  const s32 assingment   = nn::os::GetAppMemorySize();
  const s32 currentUsing = nn::os::GetUsingMemorySize();
  const s32 programsize  = nn::os::GetCodeRegionSize();

  const s32 available    = assingment - currentUsing;
  const s32 freeSize     = available - static_cast<s32>(device_memory_size + heap_memory_size);

  {
    GFL_ASSERT_STOP( heap_memory_size % 4096 == 0 );
    GFL_ASSERT_STOP( device_memory_size % 4096 == 0 );
    nn::Result result;
    result = nn::os::SetHeapSize( heap_memory_size );
    NN_UTIL_PANIC_IF_FAILED( result );
    result = nn::os::SetDeviceMemorySize( device_memory_size );
    NN_UTIL_PANIC_IF_FAILED( result );
  }



	static const u32 RESIDENT         = 0x5ED000; //0x05c0000 + 0x4000 + 0x19000 + 0x10000;
  static const u32 RESIDENT_DEVICE  = 0x0EB8000 - 0x20000 - 0x10000 + 0x40000;
  static const u32 DLL_LOAD         = 0x01b8000+FOR_DISABLE_OPTIMIZE_MEMORY;

  size_t  systemHeapTotalSize = nn::os::GetHeapSize();
  size_t  systemHeapSize = (systemHeapTotalSize - RESIDENT);
  void*   systemHeapAdrs = reinterpret_cast<void*>(nn::os::GetHeapAddress());

  size_t  devieHeapTotalSize = nn::os::GetDeviceMemorySize();
  size_t  devieHeapSize = (devieHeapTotalSize - RESIDENT_DEVICE);
  void*   deviceHeapAdrs = reinterpret_cast<void*>(nn::os::GetDeviceMemoryAddress());

  gfl2::heap::Manager::Initialize( systemHeapAdrs, (int)systemHeapSize, 2, 16 );
  gfl2::heap::HeapBase* systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );

#if 1 //PM_DEBUG
  gfl2::heap::GflHeapBase::SetDebugFreeMemClear(gfl2::heap::GflHeapBase::DEBUG_FREE_MEMC_FF);
#endif  
  // for debug
  if ( gfl2::heap::Manager::IsExistHeapId( gfl2::heap::HEAPID_SYSTEM ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] SystemHeap = NULL!!\n");
  }
 
  gfl2::heap::HeapBase* deviceHeap = GFL_CREATE_HEAP_BUFFER( deviceHeapAdrs, HEAPID_DEVICE , devieHeapSize, gfl2::heap::HEAP_TYPE_EXP );
  if ( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEVICE ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] DeviceHeap = NULL!!\n");
  }
}

class GLAllocator : public IGLAllocator
{
public:
	GLAllocator():IGLAllocator()
  {
		gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
		m_pDevAllocator = GFL_NEW(pDevHeap) gfl2::heap::NwAllocator( pDevHeap );

    // VRAMA全体
    this->vramAHeap.Initialize(nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA), nn::gx::GetVramSize(nn::gx::MEM_VRAMA));

    // VRAMB全体
    this->vramBHeap.Initialize(nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB), nn::gx::GetVramSize(nn::gx::MEM_VRAMB));

    this->allocateBack = false;
  }
	virtual ~GLAllocator()
  {
    
  }

#if GFL_DEBUG
	virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
	{
		return m_pDevAllocator->Alloc(size, alignment);
	}
#else
	virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment)
	{
		return m_pDevAllocator->Alloc(size, alignment);
	}
#endif

	virtual void SystemMemoryFree(void* const address)
	{
		m_pDevAllocator->Free(address);
	}

	virtual void* LocalMemoryMalloc( Pool allocation, u32 size, u32 alignment )
	{
		if ( allocation == Pool::LocalA )
			NN_LOG("Pool::LocalA\n");

		return this->LocalMemoryMalloc(size, alignment);
	}

	virtual void* LocalMemoryMalloc(u32 size, u32 alignment)
	{
    if (this->allocateBack)
    {
      alignment = -alignment;
    }

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

  void SetAllocateBack()
  {
    this->allocateBack = true;
  }
  void FreeBack()
  {
    this->vramAHeap.Free(NN_FND_FRAMEHEAP_FREE_TAIL);
    this->vramBHeap.Free(NN_FND_FRAMEHEAP_FREE_TAIL);
  }

	gfl2::heap::NwAllocator*									m_pDevAllocator;

private:
  bool allocateBack;
  nn::fnd::FrameHeap vramAHeap;
  nn::fnd::FrameHeap vramBHeap;
};

/**
 * @brief ポケモンモデル用のヒープ取得
 * @return gfl2::heap::HeapBase* ヒープ領域
 */
static gfl2::heap::HeapBase* GetPokemonModelHeap()
{
  if (!gfl2::heap::Manager::IsExistHeapId(gfl2::heap::HEAPID_SYSTEM))
  {
    u32 bufSize = 0x6400000;
    void* pBuf = malloc(bufSize);
    gfl2::heap::Manager::Initialize(pBuf, bufSize, 1);
  }
  return gfl2::heap::Manager::GetDefaultNewUseHeap();
}

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

  void*                     m_pHostIoMemory;

	s32																												m_VSyncCount;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0)
	{
	}

	virtual ~SimpleDraw(){}

	struct LocalFunc{
    static void *ReadData( GLAllocator *pAllocator, const char* pFilePath, s32 *outSize = NULL )
    {//自前データロード
      nn::fs::FileReader file( pFilePath );
      u32 fileSize = file.GetSize();
			void* pShaderBinary = pAllocator->SystemMemoryMallocAlign(fileSize, 128);
      file.Read(pShaderBinary, fileSize);

			if ( outSize )
				*outSize = fileSize;

			return pShaderBinary;
    }


		static void *ReadDataHIO( GLAllocator *pAllocator, const char* pFilePath, s32 *outSize = NULL )
    {//自前データロード
			nn::hio::HostFile hf;
			void							*pBuffer = NULL;

			nn::Result hfResult = hf.Open( pFilePath, nn::hio::HostFile::ACCESS_MODE_READ, nn::hio::HostFile::OPEN_DISP_OPEN_ALWAYS);

			if (hfResult.IsSuccess())
			{
				s64		head;
				s64		tail;

				hf.Seek( &tail, 0, nn::hio::HostFile::SEEK_TYPE_END );
				hf.Seek( &head, 0, nn::hio::HostFile::SEEK_TYPE_SET );

				u32 fileSize = tail - head;
				pBuffer = pAllocator->SystemMemoryMallocAlign(fileSize, 128);

				hf.Read( pBuffer, fileSize);

				hf.Close();

				if ( outSize )
					*outSize = fileSize;
			}

			return pBuffer;
    }
  };

	virtual bool OnInit( void )
	{
		gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
		gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
		gfl2::heap::HeapBase* pMemHeapForTemp = pMemHeap;

    // ファイルシステム初期化
    static const u32 ROMFS_BUFFER_SIZE = 1024 * 64;
    static char buffer[ROMFS_BUFFER_SIZE];
    nn::fs::Initialize();

		{
			gfl2::fs::System::Initialize(pDevHeap,pMemHeap);

			const size_t    maxFile       = 4096;
			const size_t    maxDirectory  = 128;
			const bool      useCache      = true;
			gfl2::fs::System::MountCtrRomDevice(pMemHeap, "rom:", "/", maxFile, maxDirectory, useCache);  // rsfファイルで設定しているところを起点に指定する

			gfl2::fs::System::CreateAsyncFileManagerForCtrRomDevice( pMemHeap, 0, 0 );
		}

		char				*pBuffer = new char[256];

	#if GFL_DEBUG
    // HIO初期化
    m_pHostIoMemory = m_Allocator.SystemMemoryMallocAlign(nn::hio::WORKMEMORY_SIZE, 128);

    nn::Result initRes = nn::hio::Initialize(m_pHostIoMemory);
    if (initRes.IsFailure())
    {
      NN_LOG("Couldn't init host file!\n");
    }
	#endif

		GFGL::Initialize( &m_Allocator );
		{
			gfl2::gfx::GFGL::InitDesc desc;
			desc.commandBufferSize = 0x120000;
			desc.requestCount = 256;
			desc.colorBufferAllocation = Pool::LocalA;
			desc.depthBufferAllocation = Pool::LocalB;
			GFGL::InitializeCTR( &m_Allocator, desc );
		}

		m_HandleList[CtrDisplayNo::LEFT] = GFGL::CreateDisplay( &m_Allocator, DisplayMode::CTR_LEFT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::RIGHT] = GFGL::CreateDisplay( &m_Allocator, DisplayMode::CTR_RIGHT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::DOWN] = GFGL::CreateDisplay( &m_Allocator, DisplayMode::CTR_DOWN, Format::A8R8G8B8, Format::D24S8, MultiSample::None);

    // DrawUtilの初期化
    DrawUtil::Initialize(&m_Allocator, 400, 240, 1024, 8);
    DrawUtil::InitializeExtension(&m_Allocator, 2048, 16);

		{
			VertexElement element[] = {
				{ 0, StreamUsage::Position,	0, VertexType::Float, 4 },
				{ 0, StreamUsage::TexCoord,	0, VertexType::Float, 4 },
				{ 0, StreamUsage::Color,		0, VertexType::Float, 4 },
				VertexElement::VERTEX_ELEMENT_END()
			};

      //CTR
      nn::fs::FileReader file(L"rom:/shader.shbin");
      u32 fileSize = file.GetSize();
      void* pShaderBinary = m_Allocator.SystemMemoryMallocAlign(fileSize, 128);
      file.Read(pShaderBinary, fileSize);
      m_pVertexShader = GFGL::CreateVertexShaderFromMemory( &m_Allocator, pShaderBinary, fileSize, element);
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
      m_pPixelShader = GFGL::CreatePixelShaderFromMemory( &m_Allocator, pixelShaderCommand, pixelShaderSize);
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

			m_pVertexBuffer = GFGL::CreateVertexBuffer( &m_Allocator, sizeof(VertexFormat), vertexNum, Usage::WriteOnly, Pool::Default, vertexDataArray );
		}

		{
			u8 idxVertexID[] = { 0, 1, 2, 3 };
			m_pIndexBuffer = GFGL::CreateIndexBuffer( &m_Allocator, sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index8, idxVertexID );
		}

		{
			ConstantBufferElement element[] = {
				{ ConstantType::Matrix, 1 },
				ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};

			m_pConstantBuffer[0] = GFGL::CreateConstantBuffer( &m_Allocator, element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[1] = GFGL::CreateConstantBuffer( &m_Allocator, element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[2] = GFGL::CreateConstantBuffer( &m_Allocator, element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[3] = GFGL::CreateConstantBuffer( &m_Allocator, element, Usage::WriteOnly, Pool::Default );

      for (u32 i = 0; i < 3; ++i)
      {
        m_pProjBuffer[i] = GFGL::CreateConstantBuffer( &m_Allocator, element, Usage::WriteOnly, Pool::Default );
        m_pViewBuffer[i] = GFGL::CreateConstantBuffer( &m_Allocator, element, Usage::WriteOnly, Pool::Default );
      }
		}

    Surface* pBackBuffer = GFGL::GetBackBuffer(m_HandleList[CtrDisplayNo::LEFT]);
    const Surface::SurfaceDesc& backBufferDesc = pBackBuffer->GetSurfaceDesc();
		m_pRenderTexture = GFGL::CreateTexture( &m_Allocator, backBufferDesc.width, backBufferDesc.height, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );

		m_pDepthStencilTexture = GFGL::CreateTextureFromSurface(  &m_Allocator, GFGL::GetDepthStencilBuffer( m_HandleList[CtrDisplayNo::LEFT] ));

		m_pBlendStateObject = GFGL::CreateBlendStateObject( &m_Allocator );

		m_pBlendStateObject->SetAlphaTestEnable( false );
		m_pBlendStateObject->SetBlendEnable( true );
		m_pBlendStateObject->SetBlendOp( BlendOp::Add );
		m_pBlendStateObject->SetBlendFunc( BlendFunc::SrcAlpha, BlendFunc::InverseSrcAlpha );
		m_pBlendStateObject->UpdateState();

		m_RasterizerStateObject = GFGL::CreateRasterizerStateObject( &m_Allocator );
		m_RasterizerStateObject->SetCullMode( CullMode::CCW );
		m_RasterizerStateObject->UpdateState();

		m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject( &m_Allocator );
		m_DepthStencilStateObject->SetDepthTestEnable(false);
		m_DepthStencilStateObject->SetDepthWriteEnable(true);
		m_DepthStencilStateObject->SetStencilOp(PolygonFace::CW, StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);
		m_DepthStencilStateObject->SetStencilFunc(PolygonFace::CW, CompareFunc::Always, 0x00, 0xFF);
    m_DepthStencilStateObject->SetStencilTestEnable(true);
    m_DepthStencilStateObject->SetStencilWriteMask(0xFF);
		m_DepthStencilStateObject->UpdateState();

		m_DepthStencilStateObject2 = GFGL::CreateDepthStencilStateObject( &m_Allocator );
		m_DepthStencilStateObject2->SetDepthTestEnable(false);
		m_DepthStencilStateObject2->SetDepthWriteEnable(false);
		m_DepthStencilStateObject2->SetStencilTestEnable(false);
		m_DepthStencilStateObject2->SetStencilWriteMask(0);
		m_DepthStencilStateObject2->UpdateState();

		m_SamplerStateObject = GFGL::CreateSamplerStateObject( &m_Allocator );
		m_SamplerStateObject->UpdateState();

    m_RasterizerStateObject->SetCullMode(CullMode::None);
    m_RasterizerStateObject->UpdateState();

		ImageData				image;
    nn::fs::FileReader texFile(L"rom:/rgba8.ctpk");
    
    u32 fileSize = texFile.GetSize();

    void* fileBuf = m_Allocator.SystemMemoryMallocAlign(fileSize, 128);
    texFile.Read(fileBuf, fileSize);

    image.ReadFromMemory(static_cast<u8*>(fileBuf));

		m_pTexture = GFGL::CreateTextureFromImage( &m_Allocator, image, Pool::Default );

    m_pDisplayList = GFGL::CreateDisplayList( &m_Allocator );

    // 立体視設定
    GFGL::SetStereoModeEnable(true);

    // ここから後方確保に変更
    m_Allocator.SetAllocateBack();

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

      // DrawUtilにProjectionの行列を設定
      DrawUtil::SetProjectionMatrix(m_cbNeverChanges.Projection);

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

      gfl2::math::Matrix34 view = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
			m_cbChangesEveryFrame.View = view;
      DrawUtil::SetViewMatrix(view);

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

      math::Matrix34 world = matY * matX;
      DrawUtil::SetModelMatrix(world);
			m_cbChangesEveryObject.World = world;
			m_pConstantBuffer[2]->Update(&m_cbChangesEveryObject, sizeof(cbChangesEveryObject));
      m_pConstantBuffer[3]->Update(&m_cbChangesEveryObject, sizeof(cbChangesEveryObject));
		}

    // 立体視用の行列作成
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

		++m_FrameCnt;

		return true;
	}

	virtual bool OnDraw( void )
	{
    // GPUプロファイラ用のファイルを出力
    static bool s_export = true;
    if (s_export)
    {
      s_export = false;
      GFGL::ExportOrcsFileRequest("asdf.orcs"); 
    }

    GFGL::SetActiveDisplay( m_HandleList[CtrDisplayNo::LEFT] );

		{
      Surface*			pBackBuffer = GFGL::GetBackBuffer( m_HandleList[CtrDisplayNo::LEFT] );
      Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[CtrDisplayNo::LEFT] );

      //オフスクリーンへのレンダリング
			GFGL::SetRenderTarget( m_pRenderTexture->GetSurface() );
			GFGL::SetDepthStencil( pDepthBuffer );//深度バッファは使いまわす

			GFGL::BeginScene();

			GFGL::ClearRenderTarget( m_pRenderTexture->GetSurface(), Color::GetBisque());
			GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );

			GFGL::EndScene();
		}

    // ディスプレイリストにコマンドを記録
    GFGL_BEGIN_DISPLAY_LIST(m_pDisplayList);
    {
      DrawUtil::BeginRender();

      GFGL::SetBlendState( m_pBlendStateObject );
      GFGL::SetDepthStencilState(m_DepthStencilStateObject2);

      GFGL::SetVertexShader( m_pVertexShader );
      GFGL::SetPixelShader( m_pPixelShader );

      GFGL::SetVertexShaderConstant( 2, m_pConstantBuffer[2] );

      GFGL::SetTexture( 0, m_pTexture );
      GFGL::SetSamplerState( 0, m_SamplerStateObject );

      GFGL::SetIndices( m_pIndexBuffer );
      GFGL::SetStreamSource( 0, m_pVertexBuffer );

      math::Matrix44 projection = math::Matrix44::GetPerspective(PI / 4.0f, 400.0f / 300.f, 0.1f, 1000.0f, true);
      math::Matrix34 view = math::Matrix34::GetLookAt(math::Vector3(0.0f, 100.0f, 100.0f), math::Vector3::GetZero(), math::Vector3::GetYUnit());
      math::Matrix44 world = math::Matrix44::GetTranslation(math::Vector3(-25.0f, 0.0f, 0.0f));
      DrawUtil::SetProjectionMatrix(projection);
      DrawUtil::SetViewMatrix(view);
      DrawUtil::SetModelMatrix(world);

      float x = 0.0f;
      float y = 0.0f;
      float w = 10.0f;
      float h = 10.0f;
      math::Vector4 uv[] = 
      {
        math::Vector4(0.0f, 0.0f, 0.0f),    // [0]
        math::Vector4(0.0f, 1.0f, 0.0f),    // [1]
        math::Vector4(1.0f, 0.0f, 0.0f),    // [2]
        math::Vector4(1.0f, 1.0f, 0.0f)     // [3]
      };

      math::Vector4 color = math::Vector4(1.0f, 0.0f, 0.0f, 1.0f);

      DrawUtil::VertexFormat vertexFormat[] =
      {
        // [0]
        DrawUtil::VertexFormat(	math::Vector4( x, y, 0.0f),
                      uv[0],
                      math::Vector4(color.x, color.y, color.z)),
        // [1]
        DrawUtil::VertexFormat(	math::Vector4( x, y + h, 0.0f),
                      uv[1],
                      math::Vector4(color.x, color.y, color.z)),
        // [2]
        DrawUtil::VertexFormat(	math::Vector4( x + w, y, 0.0f),
                      uv[2],
                      math::Vector4(color.x, color.y, color.z))
      };
      DrawUtil::SetMaterialAlpha(1.0f);
      DrawUtil::DrawTriangles(vertexFormat, 3, false);

      // 円の描画
      DrawUtil::SetModelMatrix(math::Matrix44::GetIdentity());
      DrawUtil::DrawCircle(math::Vector4::GetWUnit(), 10.0f, math::Vector4(0.0f, 1.0f, 0.0f));

      // 円柱の描画
      DrawUtil::SetModelMatrix(math::Matrix44::GetTranslation(gfl2::math::Vector3(15.0f, 0.0f, 0.0f)));
      DrawUtil::DrawCylinder(math::Vector4::GetWUnit(), 10.0f, 20.0f, math::Vector4(0.0f, 1.0f, 0.0f));

      // 矩形の描画
      DrawUtil::DrawRect(185.0f, 20.0f, 30.0f, 30.0f, math::Vector4(1.0f, 1.0f, 0.0f));
      
      // AABBの描画
      f32 rotate = (f32)(PI * (m_FrameCnt % 30)) / 15.0f;
      math::Matrix44 matY = math::Matrix44::GetRotationY(rotate);
      DrawUtil::SetModelMatrix(matY);
      DrawUtil::DrawAABB(math::Vector4(20.0f, 20.0f, 20.0f), math::Vector4(40.0f, 40.0f, 40.0f), math::Vector4(1.0f, 0.0f, 1.0f));

      // アルファの設定をしてから矩形を描画
      DrawUtil::SetMaterialAlpha(0.5f);
      DrawUtil::DrawRect(30.0f, 30.0f, 60.0f, 120.0f, math::Vector4(0.0f, 1.0f, 0.0f));

      DrawUtil::EndRender();
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
      GFGL::CallDisplayList(m_pDisplayList);

			GFGL::EndScene();

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();
		}

    GFGL::SwapDisplay();

    // バッファを切り替え
    DrawUtil::SwapBuffers();

		//この区間で描画が終わればCPUの影響を受けない。
		m_VSyncCount = GetVSyncCount();
		VSyncWaitBySleep( m_VSyncCount, 1 );
		m_VSyncCount = GetVSyncCount();

		return true;
	}

	// 垂直同期回数を取得
  s32 GetVSyncCount( void )
  {
    return nngxCheckVSync(NN_GX_DISPLAY_BOTH);
  }

	void VSyncWaitBySleep( s32 nowCount, s64 milliseconds )
  {
    while( nowCount == GetVSyncCount() )
    {
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( milliseconds ) );
    }
  }

	virtual bool OnTerm( void )
	{
		GFGL::Finish();

    DrawUtil::TerminateExtension();
    DrawUtil::Terminate();

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

	#if GFL_DEBUG
			// HIO終了処理
			nn::hio::Finalize();
	#endif
    m_Allocator.SystemMemoryFree(m_pHostIoMemory);
		return true;
	}

private:
	DisplayDeviceHandle					m_HandleList[CtrDisplayNo::NumberOf];
	GLAllocator									m_Allocator;
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
