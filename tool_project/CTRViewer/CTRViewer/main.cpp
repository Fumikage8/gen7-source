//=============================================================================
/**
 * @file    main.cpp
 * @brief   アプリケーションメイン
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.5.17   ishiguro  「4/9更新のエッジマップ描画時ステート変更」に対応する為、マテリアル初期値コピー実行。
 *
 */
//=============================================================================
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

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include "MyRenderingPipeLine.h"
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <Animation/include/gfl2_AnimationController.h>
#include <system/include/InputDevice/gfl2_Pad.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CameraNode.h>

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <heap/include/gfl2_HeapMacros.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <model/include/gfl2_PokemonModel.h>

#include <fs/include/gfl2_FsSystem.h>
//#include <fs/source/internal/gfl2_FsUtil.h>

#include <nw/font2.h>

#include <stdlib.h>
#include <nn/fs.h>
#include <nn/hio.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::system;
using namespace gfl2::renderingengine;
//using namespace gfl2::renderingengine::scenegraph;
using namespace gfl2::animation;

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
	struct AnimationSet{
		gfl2::renderingengine::scenegraph::resource::ResourceNode			*m_pMainMotion;
		gfl2::renderingengine::scenegraph::resource::ResourceNode			*m_pBlendMotion;
		f32																														m_MotionStartFrames;
    f32																														m_MotionEndFrames;
    u32																														m_MotionInterpFrames;
	};

  s32 m_OverrideMotionIndices[gfl2::animation::OverridePartsType::NumberOf];

  util::FixedSizeContainer<AnimationSet, poke_3d::model::PokemonModel::AVAILABLE_MOTION_NUM>							m_AnimationSetContainer;
	s32																															m_PlayIndex;

	u32												m_FrameCnt;
  gfl2::gfx::DisplayList    *m_pDisplayLists[2];

  void*                     m_pHostIoMemory;


	MyRenderingPipeLine																				*m_pRenderingPipeLine;
	gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_pBinModelRootNode;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_pModelInstanceNode;
	
	gfl2::gfx::Surface																				*m_pFrameBuffer;
	gfl2::gfx::Texture																				*m_pDepthStencilTexture;

	gfl2::renderingengine::scenegraph::instance::DrawEnvNode	*m_pDrawEnvNode;
	gfl2::renderingengine::scenegraph::instance::CameraNode		*m_pCameraNode;

	static const u32																					s_LightMax = 4;
	gfl2::renderingengine::scenegraph::instance::LightSetNode	*m_pLightSetNode[s_LightMax];
	gfl2::renderingengine::scenegraph::instance::LightNode		*m_pDirectionalLightNode[s_LightMax];
	gfl2::renderingengine::scenegraph::instance::LightNode		*m_pAmbientLightNode[s_LightMax];
	InputDevice::StickPos																			m_Rot;

	math::Matrix																							m_Projection;
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	f32																												m_cpuTime;
  f32																												m_gpuTime;
	f32																												m_cpuAvTime;
  f32																												m_gpuAvTime;
	b32																												m_Visible;
	
	poke_3d::model::PokemonModel															m_PokemonModel;
  gfl2::renderingengine::scenegraph::resource::ResourceNode	*m_pMotionResourceRootNode[poke_3d::model::PokemonModel::MOTION_MAX];
	c8																												*m_pGfmdlFileData;
  c8																												*m_pGfmotFileData[poke_3d::model::PokemonModel::MOTION_MAX];
  c8																												*m_pGfvshFileData[poke_3d::model::PokemonModel::MATERIAL_MAX];
  c8																												*m_pGfgshFileData[poke_3d::model::PokemonModel::MATERIAL_MAX];
  c8																												*m_pGffshFileData[poke_3d::model::PokemonModel::MATERIAL_MAX];
  c8																												*m_pTextureFileData[poke_3d::model::PokemonModel::COLORTEXTURE_MAX];
	nn::hio::DirectoryEntryA																	m_TimeStamp;
	gfl2::math::Matrix34																			m_PCViewerCameraMatrix;
	math::Matrix44																						m_PCViewerProjMatrix;
	gfl2::util::DrawUtilText																	*m_pDrawUtilText;
  b32                                                       m_IsPlayingAnimation;
  s32                                                       m_PlayingFrame;
  s32                                                       m_LoopPlayingFrame[poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM];
  s32                                                       m_MotionEndFrames;

  // 停止中の場合のモーションのインデックス
  s32                                                       m_StopMotionIndex;
  f32                                                       m_StopMotionFrame;
  s32                                                       m_StopPrevMotionIndex;
  f32                                                       m_StopPrevMotionFrame;
  f32                                                       m_StopInterpWeight;
	s32																												m_VSyncCount;

  // 背景表示のON/OFF切り替えフラグ
  b32                                                       m_IsBackgroundVisible;

  // ポケモンの模様表示用のメンバ
  u8                                                        m_MarkFrame1;
  u8                                                        m_MarkFrame2;
  u8                                                        m_MarkFrame3;
  u8                                                        m_MarkFrame4;

  // 移動関連のメンバ
  math::Vector3                                             m_WalkPosition;
  b32                                                       m_WalkEnabled;
  f32                                                       m_WalkSpeedOriginal;

  math::Vector3                                             m_BasePosition;

	gfl2::heap::NwAllocator*									m_pMemAllocator;
	gfl2::util::UtilTextRenderingPipeLine*		m_pUtilTextRenderingPipeLine;
	gfl2::str::ResFont*												m_pResFont;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_PokemonModel(),
    m_IsPlayingAnimation(true),
    m_PlayingFrame(0),
    m_IsReadModel(false)
	{
		m_pBinModelRootNode = NULL;
		m_pModelInstanceNode = NULL;

		m_pGfmdlFileData = NULL;
		for( u32 i = 0; i < poke_3d::model::PokemonModel::MOTION_MAX; ++i )
		{
			m_pGfmotFileData[i] = NULL;
			m_pMotionResourceRootNode[i] = NULL;
    }

		for( u32 i = 0; i < poke_3d::model::PokemonModel::MATERIAL_MAX; ++i )
    {
			m_pGfvshFileData[i] = NULL;
			m_pGfgshFileData[i] = NULL;
			m_pGffshFileData[i] = NULL;
    }

    for( u32 i = 0; i < poke_3d::model::PokemonModel::COLORTEXTURE_MAX; ++i )
    {
			m_pTextureFileData[i] = NULL;
		}

    for ( u32 i = 0; i < poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM; ++i )
    {
      m_LoopPlayingFrame[i] = 0;
    }

		memset( &m_TimeStamp, 0, sizeof(m_TimeStamp) );
		m_PlayIndex = -1;

    for ( u32 overrideMotionNo = 0; overrideMotionNo < gfl2::animation::OverridePartsType::NumberOf; ++overrideMotionNo )
    {
      m_OverrideMotionIndices[overrideMotionNo] = -1;
    }

		m_PCViewerCameraMatrix = gfl2::math::Matrix34::GetIdentity();

    m_WalkPosition = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
    m_WalkEnabled = false;
    m_WalkSpeedOriginal = 0.0f;
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

#if 0
	void FileSystemInitialize(gfl2::heap::HeapBase* heap)
	{
		gfl2::fs::Result            res;
		gfl2::fs::AsyncFileManager* romAsyncFileManager = NULL;

		// (1) ファイルシステムの初期化処理
		res = gfl2::fs::System::Initialize();

		m_Singlton.m_romAsyncFileManager = NULL;
		m_Singlton.m_romArcFileTable = NULL;
	
		// (2) デバイスのマウント
		const size_t    maxFile       = 4096;
		const size_t    maxDirectory  = 128;
		const bool      useCache      = true;
		res = gfl2::fs::System::MountCtrRomDevice(heap, "rom:", "/", maxFile, maxDirectory, useCache);  // rsfファイルで設定しているところを起点に指定する
		// romfilesが
		// R:\home\niji\git_program\gflib2\SystemTest\roapp_template\project\romfiles
		// のとき、
		// "rom:/data"は
		// R:\home\niji\git_program\gflib2\SystemTest\roapp_template\project\romfiles\data
		// を指している。

		gfl2::fs::Util::SetDefaultDevice(gfl2::fs::System::GetCtrRomDevice());  // @todo Util直す

		// (3) ファイルマネージャ生成
		res = gfl2::fs::System::CreateAsyncFileManagerForCtrRomDevice(heap);
		romAsyncFileManager = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

		// arcファイルリストを設定する
		/*{
			#include <arc_out/arc_file_utf8.h>
			romAsyncFileManager->SetArcFileNameTable( heap, ArchiveFileTable, sizeof(ArchiveFileTable)/sizeof(ArchiveFileTable[0]), 0, NULL );
		}*/

		//ファイルマネージャを設定する
		m_Singlton.m_romAsyncFileManager = romAsyncFileManager;

	}
#endif

	virtual bool OnInit( void )
	{
		gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
		gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
		gfl2::heap::HeapBase* pMemHeapForTemp = pMemHeap;

    u32         mateSize = sizeof(gfl2::renderingengine::scenegraph::resource::MaterialResourceNode);

    // 仮
    // ファイルシステム初期化
    static const u32 ROMFS_BUFFER_SIZE = 1024 * 64;
    static char buffer[ROMFS_BUFFER_SIZE];
    nn::fs::Initialize();
    //nn::fs::MountRom(16, 16, buffer, ROMFS_BUFFER_SIZE);


		{
			gfl2::fs::System::Initialize(pDevHeap,pMemHeap);

			const size_t    maxFile       = 4096;
			const size_t    maxDirectory  = 128;
			const bool      useCache      = true;
			gfl2::fs::System::MountCtrRomDevice(pMemHeap, "rom:", "/", maxFile, maxDirectory, useCache);  // rsfファイルで設定しているところを起点に指定する

			//gfl2::fs::Util::SetDefaultDevice( gfl2::fs::System::GetCtrRomDevice() );  // @todo Util直す

			gfl2::fs::System::CreateAsyncFileManagerForCtrRomDevice( pMemHeap, 0, 0 );

			// アロケーターの生成
			m_pMemAllocator = GFL_NEW(pMemHeap) gfl2::heap::NwAllocator( pMemHeap );
			//m_pDevAllocator = GFL_NEW(pDevHeap) gfl2::heap::NwAllocator( pDevHeap );
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

		// hid の初期化
		nn::hid::Initialize();
		system::InputDevice::Pad::Initialize();

		m_HandleList[CtrDisplayNo::LEFT] = GFGL::CreateDisplay(&m_Allocator, DisplayMode::CTR_LEFT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::RIGHT] = GFGL::CreateDisplay(&m_Allocator, DisplayMode::CTR_RIGHT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::DOWN] = GFGL::CreateDisplay(&m_Allocator, DisplayMode::CTR_DOWN, Format::A8R8G8B8, Format::D24S8, MultiSample::None);

		const gfx::Surface			*pFrameBuffer = GFGL::GetBackBuffer( m_HandleList[CtrDisplayNo::LEFT] );
		m_pDepthStencilTexture = GFGL::CreateTextureFromSurface(&m_Allocator, GFGL::GetDepthStencilBuffer( m_HandleList[CtrDisplayNo::LEFT] ));

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			gfl2::renderingengine::scenegraph::SceneGraphManager::Initialize(&m_Allocator);

			//リソースノードを生成するマネージャ初期化
			gfl2::renderingengine::scenegraph::resource::ResourceManager::Initialize(&m_Allocator);

      gfl2::gfx::DrawUtil::Initialize(&m_Allocator, 400, 240, 128, 16);
		}

		{
			m_pUtilTextRenderingPipeLine = GFL_NEW( pMemHeap ) gfl2::util::UtilTextRenderingPipeLine( &m_Allocator, 512, 256 );

			m_pResFont = gfl2::str::CreateResFont( "cbf_std_kujira.bffnt", pMemHeap, pDevHeap, pMemHeapForTemp );			

			// DrawUtilTextの生成
			m_pDrawUtilText = new gfl2::util::DrawUtilText( m_pMemAllocator, m_Allocator.m_pDevAllocator );
			m_pDrawUtilText->Initialize( m_pMemAllocator, m_Allocator.m_pDevAllocator, m_pUtilTextRenderingPipeLine->GetUtilTextRenderPath() );
			m_pDrawUtilText->SetFont( m_pResFont );
		}

		{
			//描画パイプラインを作成
			m_pRenderingPipeLine = new MyRenderingPipeLine( &m_Allocator, 512, 256, pFrameBuffer );
			m_pRenderingPipeLine->SetDepthTexture( m_pDepthStencilTexture );
		}
    
    // ************************************************************
    m_pDisplayLists[0] = GFGL::CreateDisplayList(&m_Allocator);
    m_pDisplayLists[1] = GFGL::CreateDisplayList(&m_Allocator);

		{
			u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[CtrDisplayNo::LEFT]);
			u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[CtrDisplayNo::LEFT]);

			m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(&m_Allocator);
			m_pCameraNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateCameraNode(&m_Allocator);

			{
				m_pCameraNode->SetNear( 1.1f );
				m_pCameraNode->SetFar( 1000.0f );
				m_pCameraNode->SetFovy( math::ConvDegToRad(37.0f) );
				m_pCameraNode->SetAspectRatio( (f32)displayWidth / (f32)displayHeight );
			}

			m_PCViewerProjMatrix = math::Matrix44::GetPerspective( math::ConvDegToRad(37.0f), (f32)displayWidth / (f32)displayHeight, 1.1f, 1000.0f);

			f32				aabbRange = 100000.0f;
			m_pDrawEnvNode->SetAABB( math::AABB( math::Vector4( -aabbRange, -aabbRange, -aabbRange ), math::Vector4( aabbRange, aabbRange, aabbRange ) ) );
			m_pDrawEnvNode->AddCamera( m_pCameraNode );
		}

		for( u32 i = 0; i < s_LightMax; ++i )
		{
			m_pLightSetNode[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(&m_Allocator);
			m_pDirectionalLightNode[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(&m_Allocator);
			m_pAmbientLightNode[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(&m_Allocator);

			{
				m_pDirectionalLightNode[i]->SetNodeName( "DirectionalLight" );
				util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode[i]->GetNodeName()) );
				m_pDirectionalLightNode[i]->SetNodeNameHash( nameHash.value );
				m_pDirectionalLightNode[i]->SetLightType( LightType::Directional );
				math::Vector3 position = math::Vector3(10000000.0f, 0.0f, 0.0f);
				m_pDirectionalLightNode[i]->SetPosition( position );
				math::Vector3 direction = -position.Normalize();
				m_pDirectionalLightNode[i]->SetDirection( direction );
				m_pDirectionalLightNode[i]->SetColor( math::Vector3( 1.0f, 1.0f, 1.0f ) );
			}

			{
				m_pAmbientLightNode[i]->SetNodeName( "AmbientLight" );
				util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode[i]->GetNodeName()) );
				m_pAmbientLightNode[i]->SetNodeNameHash( nameHash.value );
				m_pAmbientLightNode[i]->SetLightType( LightType::Ambient );
				m_pAmbientLightNode[i]->SetColor( math::Vector3( 0.7f, 0.7f, 0.7f ) );
			}

			
			m_pLightSetNode[i]->SetSetNo( i );
			m_pLightSetNode[i]->AddLight( m_pDirectionalLightNode[i] );//LightSetNodeの子となる
			m_pLightSetNode[i]->AddLight( m_pAmbientLightNode[i] );//LightSetNodeの子となる
			m_pDrawEnvNode->AddLightSet( m_pLightSetNode[i] );//DrawEnvNodeの子となる
			m_pRenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );
		}

    GFGL::SetStereoModeEnable(false);

		m_cpuAvTime = m_gpuAvTime = m_cpuTime = m_gpuTime = 0;
		m_Visible = true;

    // ここから後方確保に変更
    m_Allocator.SetAllocateBack();

		return true;
	}

	virtual bool OnUpdate( void )
	{
		system::InputDevice::Pad::Update();

		{//モデル操作
			if ( InputDevice::Pad::IsPush( 0, InputDevice::Button::R1 ) )
			{
				m_Visible = !m_Visible;
			}

      if ( InputDevice::Pad::IsPush( 0, InputDevice::Button::L1 ) )
      {
#if defined(DOF_ENABLE)
        m_pRenderingPipeLine->ChangeDofType();
#endif
      }
			if ( InputDevice::Pad::IsPush( 0, InputDevice::Button::Start ) )
			{
				b32				fDataUpdate = false;
				{
					nn::hio::HostDirectoryA			hd;

          char infoDatDir[256];
          nn::hio::GetEnvironmentVariable(infoDatDir, sizeof(infoDatDir), "NIJI_TOOL_DIR");
          ::std::strcat(infoDatDir, "NijiModelViewer\\tmp\\info_dat\\");

          NN_LOG("open : %s\n", infoDatDir);

					nn::Result openResult = hd.Open(infoDatDir);
          if (openResult.IsFailure())
          {
            NN_LOG("faild to open : %s\n", infoDatDir);
            return;
          }

					while( 1 )
					{
						nn::hio::DirectoryEntryA		entry;

						nn::Result		result = hd.GetNextEntry( &entry );

						if ( ::std::strcmp( entry.filename, "info.dat" ) == 0 )
						{
							if ( m_TimeStamp.lastWriteTime.year < entry.lastWriteTime.year || 
									 m_TimeStamp.lastWriteTime.month < entry.lastWriteTime.month ||
									 m_TimeStamp.lastWriteTime.day < entry.lastWriteTime.day ||
									 m_TimeStamp.lastWriteTime.hour < entry.lastWriteTime.hour || 
									 m_TimeStamp.lastWriteTime.minute < entry.lastWriteTime.minute ||
									 m_TimeStamp.lastWriteTime.second < entry.lastWriteTime.second )
							{
								fDataUpdate = true;
							}

							m_TimeStamp = entry;
							break;
						}

						if ( result.IsSuccess() == false )
							break;
					}
				}

				if ( fDataUpdate )
				{
					u32				infoSize = sizeof( poke_3d::model::PokemonModel::PokemonModelInfo );

					poke_3d::model::PokemonModel::PokemonModelInfo		InfoData;

					nn::hio::HostFile hf;

          char infoDatPath[256];
          nn::hio::GetEnvironmentVariable(infoDatPath, sizeof(infoDatPath), "NIJI_TOOL_DIR");
          ::std::strcat(infoDatPath, "NijiModelViewer\\tmp\\info_dat\\info.dat");

					nn::Result hfResult = hf.Open( infoDatPath, nn::hio::HostFile::ACCESS_MODE_READ, nn::hio::HostFile::OPEN_DISP_OPEN_ALWAYS);

					if (hfResult.IsFailure())
          {
            NN_LOG("faild to open : %s\n", infoDatPath);
          }
          else
					{
						u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[CtrDisplayNo::LEFT]);
						u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[CtrDisplayNo::LEFT]);

						hf.Read( &InfoData, infoSize);
						hf.Close();

						ReadModel( &InfoData );

						m_PCViewerCameraMatrix = math::Matrix34( InfoData.m_CameraMatrix[0], InfoData.m_CameraMatrix[1], InfoData.m_CameraMatrix[2] );
						m_PCViewerProjMatrix = math::Matrix44::GetPerspective( math::ConvDegToRad(InfoData.m_FieldOfView), (f32)displayWidth / (f32)displayHeight, InfoData.m_NearClip, InfoData.m_FarClip);

            // 歩き位置初期化
            m_WalkPosition.Set(0, 0, 0);
            m_BasePosition.Set(InfoData.m_Translate.x, InfoData.m_Translate.y, InfoData.m_Translate.z);
						m_PokemonModel.SetPosition( m_BasePosition );
						m_PokemonModel.SetRotation( InfoData.m_Rotation.x, InfoData.m_Rotation.y, InfoData.m_Rotation.z );
						m_PokemonModel.SetScale( InfoData.m_Scale.x, InfoData.m_Scale.y, InfoData.m_Scale.z );

						gfl2::renderingengine::scenegraph::instance::CameraNode*			pCamera = m_pDrawEnvNode->GetCamera( 0 );
						pCamera->SetLocalTranslate( math::Vector3(0, 0, 0) );
						pCamera->SetLocalRotation( math::Quaternion(0,0,0,1) );
						m_Rot.x = m_Rot.y = 0.0f;

						for( u32 i = 0; i < s_LightMax; ++i )
							m_pDirectionalLightNode[i]->SetDirection( *((math::Vector3*)&InfoData.m_LightVector) );

            m_IsPlayingAnimation = InfoData.m_IsPlayingAnimation;
            m_PlayingFrame = InfoData.m_PlayingFrame;
            m_StopMotionIndex = InfoData.m_StopMotionIndex;
            m_StopMotionFrame = InfoData.m_StopMotionFrame;
            m_StopPrevMotionIndex = InfoData.m_StopPrevMotionIndex;
            m_StopPrevMotionFrame = InfoData.m_StopPrevMotionFrame;
            m_StopInterpWeight = InfoData.m_StopInterpWeight;
            m_IsBackgroundVisible = InfoData.m_IsBackgroundVisible;

            m_MarkFrame1 = InfoData.m_MarkFrame1;
            m_MarkFrame2 = InfoData.m_MarkFrame2;
            m_MarkFrame3 = InfoData.m_MarkFrame3;
            m_MarkFrame4 = InfoData.m_MarkFrame4;

            m_PokemonModel.SetMarkingFrames(m_MarkFrame1, m_MarkFrame2, m_MarkFrame3, m_MarkFrame4);

						// GPUプロファイラ用のファイルを出力
						//GFGL::ExportOrcsFileRequest("asdf.orcs"); 

            // ループアニメ初期設定
            for (s32 i = 0; i < 4; ++i)
            {
              b32 loopEnable = m_PokemonModel.GetLoopAnimationFlag(i);
              m_PokemonModel.SetUpLoopMotion(i, loopEnable);
            }

            for ( u32 i = 0; i < poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM; ++i )
            {
              m_LoopPlayingFrame[i] = InfoData.m_LoopPlayingFrame[i];
            }
					}
				}
			}

      if (m_IsReadModel)
      {
        if (!m_IsPlayingAnimation)
        {// アニメーションが停止していたらフレームを設定してそのフレームで固定する
          if (m_StopMotionIndex >= 0)
          {
            m_PokemonModel.ChangeAnimationByResourceNode(m_pMotionResourceRootNode[m_StopMotionIndex]);
            m_PokemonModel.SetAnimationFrame(m_StopMotionFrame);
            m_WalkPosition.x = 0;
            m_WalkPosition.y = 0;
            m_WalkPosition.z = 0;

            for (s32 loopMotionIndex = gfl2::animation::OverridePartsType::Loop01;
                 loopMotionIndex <= gfl2::animation::OverridePartsType::Loop04;
                 ++loopMotionIndex)
            {
              s32 overrideMotionIndex = m_OverrideMotionIndices[loopMotionIndex];
              if (overrideMotionIndex < 0)
              {
                continue;
              }

              for (s32 i = 0; i < poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM; ++i)
              {
                m_PokemonModel.ChangeAnimationByResourceNode(
                  m_pMotionResourceRootNode[overrideMotionIndex],
                  loopMotionIndex + 1,  // 全体のモーションが1つあるので+1
                  poke_3d::model::PokemonModel::s_LoopAnimationBit[i]);
                m_PokemonModel.SetAnimationFrame(
                  m_LoopPlayingFrame[i],
                  loopMotionIndex + 1,
                  poke_3d::model::PokemonModel::s_LoopAnimationBit[i]);
              }
            }
          }
        }
        else
        {
			    if (--m_MotionEndFrames <= 0)
			    {
				    PlayNextMotion();
			    }
        }
      }
		}

		{//カメラ操作
			gfl2::renderingengine::scenegraph::instance::CameraNode*			pCamera = m_pDrawEnvNode->GetCamera( 0 );
			const InputDevice::StickPos&					pos = InputDevice::Pad::GetStick( 0, InputDevice::Stick::Left );
			InputDevice::StickPos									rot;

			if ( InputDevice::Pad::IsPress( 0, InputDevice::Button::X ) )
			{
				rot.y += 1.0f;
			}
			else if ( InputDevice::Pad::IsPress( 0, InputDevice::Button::B ) )
			{
				rot.y -= 1.0f;
			}

			if ( InputDevice::Pad::IsPress( 0, InputDevice::Button::A ) )
			{
				rot.x += 1.0f;
			}
			else if ( InputDevice::Pad::IsPress( 0, InputDevice::Button::Y ) )
			{
				rot.x -= 1.0f;
			}

			m_Rot.x += rot.x;
			m_Rot.y += rot.y;

			math::Matrix34													rotYMat = math::Matrix34::GetRotationY( -m_Rot.x / 20.0f );
			math::Matrix34													rotXMat = math::Matrix34::GetRotationX( -m_Rot.y / 20.0f );
			math::Vector3													  camPos = pCamera->GetLocalTranslate();

			math::Matrix34 mat = rotYMat * rotXMat;

      math::Vector3 axisX = mat.GetElemAxisX();
      math::Vector3 axisZ = mat.GetElemAxisZ();
      camPos += axisX * (pos.x * 2.0f);
      camPos += axisZ * (-pos.y * 2.0f);

      pCamera->SetLocalTranslate(camPos);
      math::Quaternion rotQuat;
      rotQuat.MatrixToQuaternion(mat);
      pCamera->SetLocalRotation(rotQuat);

			m_pRenderingPipeLine->SetViewMatrix( pCamera->GetViewMatrix() );	
		}

    u32 displayWidth = GFGL::GetDisplayWidth(m_HandleList[CtrDisplayNo::LEFT]);
    u32 displayHeight = GFGL::GetDisplayHeight(m_HandleList[CtrDisplayNo::LEFT]);

    if ( m_pModelInstanceNode )
    {
			if ( m_Visible )
			{
        b32 oldLoopFlags[poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM] =
        {
          m_PokemonModel.GetLoopAnimationFlag(0),
          m_PokemonModel.GetLoopAnimationFlag(1),
          m_PokemonModel.GetLoopAnimationFlag(2)
        };
        // キーアニメーションの更新
        m_PokemonModel.UpdateKeyAnimation();

        for (s32 i = 0; i < poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM; ++i)
        {
          b32 loopEnable = m_PokemonModel.GetLoopAnimationFlag(i);
          if (oldLoopFlags[i] != loopEnable)
          {
            m_PokemonModel.SetUpLoopMotion(0, loopEnable, poke_3d::model::PokemonModel::s_LoopAnimationBit[i]);
          }

          m_PokemonModel.SetLoopMotionFrame(0, poke_3d::model::PokemonModel::s_LoopAnimationBit[i]);
        }

        // アニメーションの更新
  			f32 interpWeight = m_PokemonModel.UpdateAnimation();

        // 歩き速度取得
        f32 stepFrame = m_PokemonModel.GetAnimationStepFrame();
        f32 frame0 = m_PokemonModel.GetAnimationFrame();
        f32 frame1 = m_PokemonModel.GetAnimationFrame();
        // 補間終了判定
        if (interpWeight >= 1.0f)
        {
          interpWeight = -1;
        }
        // 補間中ならframe0は補間元
        if (interpWeight > 0.0f)
        {
          frame0 = m_PokemonModel.GetAnimationController()->GetJointSlot()->GetFrame();
        }
        gfl2::math::Vector3 walkSpeed = m_PokemonModel.GetWalkSpeedManual(stepFrame, frame0, frame1, interpWeight);
        m_WalkPosition.y = walkSpeed.y * m_PokemonModel.GetScale().y;
        m_WalkPosition.z += walkSpeed.z * m_PokemonModel.GetScale().z;
			}
			m_PokemonModel.SetVisible(m_Visible);

      // 位置設定
      m_PokemonModel.SetPosition(m_BasePosition + m_WalkPosition);
    }

		{//トラバース処理にてワールド座標を更新する場合。
			//m_pModelInstanceNode->SetBillboardRotation( m_PCViewerCameraMatrix );

      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
      
      gfl2::math::Matrix44 viewMtx;
      math::Matrix34			camera;
      camera = m_PCViewerCameraMatrix *  m_pCameraNode->GetViewMatrix();

      memcpy(&viewMtx,&camera,sizeof(camera));
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseApplyBillboard(&viewMtx);
     
		}
		++m_FrameCnt;

		// GPU時間表示
    {
      f32 cpuTime = GFGL::GetTotalCPUTimeMicroSecond();
      f32 gpuTime = GFGL::GetTotalGPUTimeMicroSecond();

			m_cpuTime += cpuTime;
			m_gpuTime += gpuTime;

			if ( m_FrameCnt % 30 == 0 )
			{
				m_cpuTime /= 30;
				m_gpuTime /= 30;

				m_cpuAvTime = m_cpuTime;
				m_gpuAvTime = m_gpuTime;

				m_cpuTime = m_gpuTime = 0;
			}

			{
				m_pDrawUtilText->ClearDrawText();

				m_pDrawUtilText->SetTextScale( 0.5f, 0.5f );

        static char date_time[256];
        static wchar_t date_time_w[256];
        sprintf(date_time, "%s %s\n", __DATE__, __TIME__);
        mbstowcs(date_time_w, date_time, strlen(date_time));

        m_pDrawUtilText->DrawText( 0, 10, date_time_w);
				m_pDrawUtilText->DrawText( 0, 30, L"CPU %d micro seq", (u32)m_cpuAvTime );
				m_pDrawUtilText->DrawText( 0, 40, L"GPU %d micro seq", (u32)m_gpuAvTime );

				if ( 0 )
				{
					math::AABB				aabb = m_pModelInstanceNode->GetWorldAABB();
					math::Matrix34			camera;

					camera = m_PCViewerCameraMatrix *  m_pCameraNode->GetViewMatrix();

					math::Matrix44		viewProjMat44 = m_PCViewerProjMatrix * camera;
					math::Matrix44		cullingMatrix[4];

					math::AABB::Util::CalcFrustumPlanes( cullingMatrix, viewProjMat44 );

					if ( math::AABB::Util::IsIntersectFrustum( cullingMatrix, aabb ) == false )
						m_pDrawUtilText->DrawText( 0, 0, L"Out" );
					else
						m_pDrawUtilText->DrawText( 0, 0, L"In" );

					m_pDrawUtilText->DrawText( 0, 60, L"%f, %f, %f", aabb.GetMin().x, aabb.GetMin().x, aabb.GetMin().x );
					m_pDrawUtilText->DrawText( 0, 70, L"%f, %f, %f", aabb.GetMax().x, aabb.GetMax().x, aabb.GetMax().x );
				}
			}
    }

		{
			u32			disp = CtrDisplayNo::LEFT;

			math::Matrix44 projMat[CtrDisplayNo::NumberOf];
			math::Matrix34 viewMat[CtrDisplayNo::NumberOf];

			static f32 depthLevel = 4.0f;
			static f32 factor = 1.0f;

			math::Matrix34			camera;

			camera = m_PCViewerCameraMatrix *  m_pCameraNode->GetViewMatrix();

			GFGL::GetStereoMatrix(
				m_PCViewerProjMatrix, camera, depthLevel, factor, 
				&projMat[CtrDisplayNo::LEFT], &viewMat[CtrDisplayNo::LEFT], &projMat[CtrDisplayNo::RIGHT], &viewMat[CtrDisplayNo::RIGHT]);

			m_pRenderingPipeLine->SetProjectionMatrix( projMat[disp] );
			m_pRenderingPipeLine->SetViewMatrix( viewMat[disp] );

      {
        // 背景モデル描画の際にファークリップを変更する必要があるので計算して渡す
        gfl2::math::Matrix44 backgroundProjection = gfl2::math::Matrix44::GetPerspective(
          gfl2::math::ConvDegToRad(30.0f),
          400.0f / 240.0f,
          1.1f,
          100000.0f);

        u32 disp = CtrDisplayNo::LEFT;

        math::Matrix44 backgroundProjMat[CtrDisplayNo::NumberOf];
        math::Matrix34 backgroundViewMat[CtrDisplayNo::NumberOf];

        GFGL::GetStereoMatrix(
          backgroundProjection, camera, depthLevel, factor, 
          &backgroundProjMat[CtrDisplayNo::LEFT], &backgroundViewMat[CtrDisplayNo::LEFT], &backgroundProjMat[CtrDisplayNo::RIGHT], &backgroundViewMat[CtrDisplayNo::RIGHT]);
        m_pRenderingPipeLine->SetCameraDataToBackgroundPath(
          true,
          backgroundProjMat[disp],
          backgroundViewMat[disp]);
      }

      m_pRenderingPipeLine->SetEnabledBackgroundVisible(m_IsBackgroundVisible);
			m_pRenderingPipeLine->Update();
		}

		return true;
	}

	virtual bool OnDraw( void )
	{
    GFGL::SetActiveDisplay( m_HandleList[CtrDisplayNo::LEFT] );

		{
			u32			disp = CtrDisplayNo::LEFT;
      GFGL::SetActiveDisplay( m_HandleList[disp] );

      Surface		*pBackBuffer = GFGL::GetBackBuffer( m_HandleList[disp] );
      Surface		*pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[disp] );
			u32				displayWidth = GFGL::GetDisplayWidth(m_HandleList[disp]);
			u32				displayHeight = GFGL::GetDisplayHeight(m_HandleList[disp]);

			m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
			m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
			m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

			m_pRenderingPipeLine->Execute();

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();	
		}

		{
			u32			disp = CtrDisplayNo::DOWN;
      GFGL::SetActiveDisplay( m_HandleList[disp] );

      Surface		*pBackBuffer = GFGL::GetBackBuffer( m_HandleList[disp] );
      Surface		*pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[disp] );
			u32				displayWidth = GFGL::GetDisplayWidth(m_HandleList[disp]);
			u32				displayHeight = GFGL::GetDisplayHeight(m_HandleList[disp]);

			GFGL::SetRenderTarget( pBackBuffer );
			GFGL::SetDepthStencil( pDepthBuffer );

			GFGL::BeginScene();
			GFGL::ClearRenderTargetDepthStencil( pBackBuffer, Color( 0.5f, 0.5f, 0.5f), pDepthBuffer, 1.0f, 255 );
			GFGL::SetViewport(0, 0, displayWidth, displayHeight );
			GFGL::SetScissor(0, 0, displayWidth, displayHeight );

			GFGL::EndScene();

			m_pDrawUtilText->SetRenderPath( m_pUtilTextRenderingPipeLine->GetUtilTextRenderPath() );
			{
				m_pUtilTextRenderingPipeLine->SetRenderTarget( pBackBuffer );
				m_pUtilTextRenderingPipeLine->SetDepthStencil( pDepthBuffer );
				m_pUtilTextRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
				m_pUtilTextRenderingPipeLine->Execute();
			}

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();	
		}

		gfl2::gfx::GFGL::Finish();
		GFGL::SwapDisplay();
    gfl2::gfx::DrawUtil::SwapBuffers();

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

		GFGL::SetActiveDisplay(NULL);

    gfl2::gfx::DrawUtil::Terminate();

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

	void ReadModel( poke_3d::model::PokemonModel::PokemonModelInfo *pInfoData )
	{
		gfl2::gfx::GFGL::Finish();
		GFGL::SwapDisplay();

    // 後方確保した分のVRAMを開放する
    m_Allocator.FreeBack();

		GFL_SAFE_DELETE( m_pBinModelRootNode );
		if ( m_pModelInstanceNode )
			m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );

		GFL_SAFE_DELETE( m_pGfmdlFileData );
    for( u32 i = 0; i < poke_3d::model::PokemonModel::MOTION_MAX; ++i )
		{
			GFL_SAFE_DELETE( m_pGfmotFileData[i] );
			GFL_SAFE_DELETE( m_pMotionResourceRootNode[i] );
    }

    for( u32 i = 0; i < poke_3d::model::PokemonModel::MATERIAL_MAX; ++i )
    {
			GFL_SAFE_DELETE( m_pGfvshFileData[i] );
			GFL_SAFE_DELETE( m_pGfgshFileData[i] );
			GFL_SAFE_DELETE( m_pGffshFileData[i] );
    }

    for( u32 i = 0; i < poke_3d::model::PokemonModel::COLORTEXTURE_MAX; ++i )
    {
			GFL_SAFE_DELETE( m_pTextureFileData[i] );
		}
#if 0
		{
			//リソースローダー経由でノードは生成されます。
			gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData							resourceData;

			s32		size;

			//c8		*pBinData = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, "R:/home/niji/exec/data_ctr/data/test/ishiguro_test/Kusa/f1.ctr.gfbmdl" );//分離草
			c8		*pBinData = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, "R:/home/niji/workspace/04_field/graphic/ibe_mana/exports/f1.ctr.gfbmdl" );//分離草
			//c8		*pBinData = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, "C:/home/niji/tool/DCCTools/Maya/Sample/Basic/Exports/pSphere1.ctr.gfbmdl" );
			
			resourceData.SetModelData( pBinData );

			m_pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	
		}

		{
			static const c8* pShaderFileList[] = {
				//分離草
				//"R:/home/niji/exec/data_ctr/data/test/ishiguro_test/Kusa/Shaders/Kusa.ctr.gfbvsh",
				"R:/home/niji/workspace/04_field/graphic/ibe_mana/exports/Shaders/Kusa.ctr.gfbvsh",
				"R:/home/niji/exec/data_ctr/data/test/ishiguro_test/Kusa/Shaders/KusaTest.ctr.gfbfsh",

				//分離前草
				/*"R:/home/niji/workspace/04_field/graphic/ibe_mana/exports/Shaders/Kusa.ctr.gfbvsh",
				"R:/home/niji/workspace/04_field/graphic/ibe_mana/exports/Shaders/encgrassA_top_1_land01_encgrass_03_Manual.ctr.gfbfsh"*/

				/*"C:/home/niji/tool/DCCTools/Maya/Sample/Basic/Exports/Shaders/Default.ctr.gfbvsh",
				"C:/home/niji/tool/DCCTools/Maya/Sample/Basic/Exports/Shaders/Comb1_Manual.ctr.gfbfsh",
				"C:/home/niji/tool/DCCTools/Maya/Sample/Basic/Exports/Shaders/Comb2_Manual.ctr.gfbfsh",*/
			};

			u32				shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);

			for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
			{
				gfl2::renderingengine::scenegraph::resource::GfBinaryShdData							resourceData;

				s32		size;
				c8		*pBinData = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, pShaderFileList[shaderNo] );

				resourceData.SetShaderData( pBinData );

				gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
				m_pBinModelRootNode->AddChild( pBinShdNode );
			}
		}

		{
			static const c8*			pTextureFileList[] = {
				//分離草
				"R:/home/niji/exec/data_ctr/data/test/ishiguro_test/Kusa/Textures/land01_encgrass_01.ctr.btex",
				"R:/home/niji/exec/data_ctr/data/test/ishiguro_test/Kusa/Textures/land01_encgrass_02.ctr.btex"

				//分離前草,
				//"R:/home/niji/workspace/04_field/graphic/ibe_mana/exports/Textures/land01_encgrass_01.ctr.btex",
				//"R:/home/niji/workspace/04_field/graphic/ibe_mana/exports/Textures/land01_encgrass_02.ctr.btex",
			};

			/*static const c8*			pTextureFileList[] = {
				"C:/home/niji/tool/NijiModelViewer/tmp/3940024801/DummyTex.ctr.btex",
				"C:/home/niji/tool/NijiModelViewer/tmp/3940024801/pm0100_00_Body1.ctr.btex",
				"C:/home/niji/tool/NijiModelViewer/tmp/3940024801/pm0100_00_Eye1.ctr.btex",
			};*/

			u32				texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);

			for( u32 texNo = 0; texNo < texNum; ++texNo )
			{
				gfl2::renderingengine::scenegraph::resource::GfBinaryTexData							texResourceData;
				gfl2::renderingengine::scenegraph::resource::ResourceNode								*pBinTexRootNode;

				s32		texsize;
				s32		size;
				c8		*pTexBinData = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, pTextureFileList[texNo] );
			
				texResourceData.SetTextureData( pTexBinData );

				//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
				pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
				m_pBinModelRootNode->AddChild( pBinTexRootNode );
			}
		}
#else

		if ( 1 )
		{
			gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData							resourceData;
			m_pGfmdlFileData = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, pInfoData->m_GfmdlFilePath );

			resourceData.SetModelData( m_pGfmdlFileData );

			m_pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &m_Allocator, &resourceData );
		}

		if ( 1 )
		{
			for( u32 shaderNo = 0; shaderNo < pInfoData->m_GfvshFileNum; ++shaderNo )
			{
				gfl2::renderingengine::scenegraph::resource::GfBinaryShdData							resourceData;

				m_pGfvshFileData[shaderNo] = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, pInfoData->m_GfvshFilePaths[shaderNo] );

				resourceData.SetShaderData( m_pGfvshFileData[shaderNo] );

				gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &m_Allocator, &resourceData );
				m_pBinModelRootNode->AddChild( pBinShdNode );
			}
		}

		if ( 1 )
		{
			for( u32 shaderNo = 0; shaderNo < pInfoData->m_GffshFileNum; ++shaderNo )
			{
				gfl2::renderingengine::scenegraph::resource::GfBinaryShdData							resourceData;

				m_pGfgshFileData[shaderNo] = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, pInfoData->m_GffshFilePaths[shaderNo] );

				resourceData.SetShaderData( m_pGfgshFileData[shaderNo] );

				gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &m_Allocator, &resourceData );
				m_pBinModelRootNode->AddChild( pBinShdNode );
			}
		}

		if ( 1 )
		{
			for( u32 texNo = 0; texNo < pInfoData->m_TextureFileNum; ++texNo )
			{
				gfl2::renderingengine::scenegraph::resource::GfBinaryTexData							texResourceData;
				gfl2::renderingengine::scenegraph::resource::ResourceNode								*pBinTexRootNode;

				m_pTextureFileData[texNo] = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, pInfoData->m_TextureFilePaths[texNo] );
			
				texResourceData.SetTextureData( m_pTextureFileData[texNo] );

				pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &m_Allocator, &texResourceData );
				m_pBinModelRootNode->AddChild( pBinTexRootNode );
			}
		}
#endif

		{
			for( u32 motNo = 0; motNo < pInfoData->m_GfmotFileNum; ++motNo )
			{
				s32				fileSize;

				gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
				m_pGfmotFileData[motNo] = (c8*)LocalFunc::ReadDataHIO( &m_Allocator, pInfoData->m_GfmotFilePaths[motNo], &fileSize );

				animationResourceData.SetMotionData( m_pGfmotFileData[motNo] );

				m_pMotionResourceRootNode[motNo] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &m_Allocator, &animationResourceData );
			}
		}

		{
			m_PokemonModel.Destroy();
      gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
			m_PokemonModel.Create(&m_Allocator, pMemHeap, m_pBinModelRootNode);
      m_PokemonModel.CreateColorShaderTextures(&m_Allocator);
      
      m_IsReadModel = true;

      for (s32 i = 0; i < pInfoData->m_RareColorConstantSettingInfoNum; ++i)
      {
        m_PokemonModel.AddRareConstantColor(
          pInfoData->m_RareColorConstantSettingInfos[i].m_OriginalColor.GetMaterialIndex(),
          pInfoData->m_RareColorConstantSettingInfos[i].m_ColorOffset.GetConstantColorIndex(),
          pInfoData->m_RareColorConstantSettingInfos[i].m_ColorOffset.GetColorOffset());
      }
			m_pModelInstanceNode = m_PokemonModel.GetModelInstanceNode();
      m_pModelInstanceNode->CopyMaterialDepthStencilStateToStateObject();

			{
				m_PokemonModel.SetEnabledColorShaderTexture( true );

				m_pRenderingPipeLine->ClearColorShaderPath();

				u32			colTexNum = m_PokemonModel.GetColorTextureNum();
				for( u32 texNo = 0; texNo < colTexNum; ++texNo )
				{
					const gfl2::renderingengine::scenegraph::resource::TextureResourceNode			*pTextureNode;
					const gfl2::gfx::Texture																										*pOriginalColorTexture;
					const gfl2::gfx::Texture																										*pColorShaderTexture;

					pTextureNode = m_PokemonModel.GetOriginalColorTextureResourceNode(texNo);
					const c8*				pName = pTextureNode->GetName();
					pOriginalColorTexture = pTextureNode->GetTexture();
					pColorShaderTexture = m_PokemonModel.GetColorShaderTextureResourceNode(texNo)->GetTexture();

					m_pRenderingPipeLine->AddColorShaderTexture( pOriginalColorTexture, pColorShaderTexture );
				}

        m_PokemonModel.SetInterpColor(pInfoData->m_ColorShaderColor);
        m_pRenderingPipeLine->SetColorShaderTextureColor(pInfoData->m_ColorShaderColor);

        // 環境光の設定
        m_PokemonModel.SetEnabledAmbientColor(pInfoData->m_IsAmbientColorEnabled);
        m_PokemonModel.SetAmbientColor(pInfoData->m_AmbientColor);
      }

			//m_PokemonModel.ChangeAnimation( m_pMotionResourceRootNode[ pInfoData->m_MotionIndices[0] ] );
			//m_PokemonModel.SetAnimationLoop( true );

      {
        // PokemonModelにモーションリソースノードを設定
        for( u32 motNo = 0; motNo < pInfoData->m_GfmotFileNum; ++motNo )
        {
          m_PokemonModel.SetMotionResourceNode(motNo, m_pMotionResourceRootNode[motNo]);
        }

        for ( u32 overrideMotionNo = 0; overrideMotionNo < gfl2::animation::OverridePartsType::NumberOf; ++overrideMotionNo )
        {
          m_OverrideMotionIndices[overrideMotionNo] = pInfoData->m_OverrideMotionIndices[overrideMotionNo];
          m_PokemonModel.SetOverridePartsMotionIndex(
            static_cast<gfl2::animation::OverridePartsType>(overrideMotionNo),
            overrideMotionNo + 1,  // 通常のモーションが0番のため, +1して対応
            m_OverrideMotionIndices[overrideMotionNo]);
        }
      }

			m_AnimationSetContainer.clear();
			m_PlayIndex = -1;

			for( u32 i = 0; i < poke_3d::model::PokemonModel::AVAILABLE_MOTION_NUM; ++i )
			{
				s32			motionIndex = pInfoData->m_MotionIndices[i];

				if ( motionIndex == -1 )
					continue;

				AnimationSet				set;

				set.m_pMainMotion = m_pMotionResourceRootNode[ motionIndex ];
				set.m_MotionStartFrames = pInfoData->m_MotionStartFrames[i];
				set.m_MotionEndFrames = pInfoData->m_MotionEndFrames[i];
				set.m_MotionInterpFrames = pInfoData->m_MotionInterpFrames[i];

				m_AnimationSetContainer.push_back( set );
			}

			if ( m_AnimationSetContainer.size() )
			{
				m_PlayIndex = 0;
				PlayNextMotion();
			}
			m_pModelInstanceNode->SetUpDrawTag();
      m_PokemonModel.SetKeyCallBackInfo();
    }

		m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
	}

	void PlayNextMotion()
	{
    m_PokemonModel.SetKeyCallBackInfo();

		if ( m_AnimationSetContainer.size() )
		{
      if (m_PlayIndex == 0)
      {
        // 歩き位置初期化
        m_WalkPosition.Set(0, 0, 0);
      }

      if (m_PlayIndex == 0)
      {
        m_PokemonModel.ChangeAnimationByResourceNode( m_AnimationSetContainer[m_PlayIndex].m_pMainMotion );
      }
      else
      {
        m_PokemonModel.ChangeAnimationSmoothByResourceNode( m_AnimationSetContainer[m_PlayIndex].m_pMainMotion, m_AnimationSetContainer[m_PlayIndex - 1].m_MotionInterpFrames );
      }
      m_PokemonModel.SetAnimationStartFrame( m_AnimationSetContainer[m_PlayIndex].m_MotionStartFrames );
      m_PokemonModel.SetAnimationFrame( m_AnimationSetContainer[m_PlayIndex].m_MotionStartFrames );
      // m_PokemonModel.SetAnimationEndFrame( m_AnimationSetContainer[m_PlayIndex].m_MotionEndFrames );
      m_MotionEndFrames = m_AnimationSetContainer[m_PlayIndex].m_MotionEndFrames - m_AnimationSetContainer[m_PlayIndex].m_MotionStartFrames + 1;
      m_PokemonModel.SetAnimationLoop( false );

			++m_PlayIndex;
			if ( m_PlayIndex >= m_AnimationSetContainer.size() )
      {
				m_PlayIndex = 0;
      }
		}
	}

private:
	DisplayDeviceHandle					m_HandleList[CtrDisplayNo::NumberOf];
	GLAllocator									m_Allocator;
  b32                         m_IsReadModel;
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
