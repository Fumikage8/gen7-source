#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

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

#include "MyRenderingPipeLine.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <Animation/include/gfl2_AnimationController.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;
using namespace gfl2::animation;


#include <stdlib.h>

#if defined(NN_PLATFORM_CTR)
#define TEST_CTR
#else
#define TEST_WIN
#endif

#if defined(GF_PLATFORM_CTR)
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

#if defined(GF_PLATFORM_CTR)
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
#else
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
		math::Matrix34  View;  // ビュー変換行列
	};

	// 定数バッファのデータ
	cbNeverChanges										m_cbNeverChanges;       // 透視変換行列
	cbChangesEveryFrame								m_cbChangesEveryFrame;  // ビュー変換行列　光源座標

	u32												m_FrameCnt;
  gfl2::gfx::DisplayList    *m_pDisplayLists[2];

  void*                     m_pHostIoMemory;


	MyRenderingPipeLine																				*m_pRenderingPipeLine;
	scenegraph::resource::ResourceNode												*m_pBinModelRootNode;
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode;
	scenegraph::resource::ResourceNode												*m_pMotionResourceRootNode;
	gfl2::gfx::Texture																				*m_pDepthStencilTexture;

	math::Matrix																							m_Projection;
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	f32																												m_cpuTime;
  f32																												m_gpuTime;
	b32																												m_Visible;

#if 0
#if defined(GF_PLATFORM_WIN32)
	AnimationControler																				m_AnimationControler;
#endif
#endif

  animation::AnimationController                            m_AnimationController;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0)
	{
		
	}

	virtual ~SimpleDraw(){}

	struct LocalFunc{
    static void *ReadData( GLAllocator *pAllocator, const char* pFilePath )
    {//自前データロード
#if defined(GF_PLATFORM_CTR)
      nn::fs::FileReader file( pFilePath );
      u32 fileSize = file.GetSize();
      void* pShaderBinary = pAllocator->SystemMemoryMalloc(fileSize, 128);
      file.Read(pShaderBinary, fileSize);
			return pShaderBinary;
#else
			FILE		*hFile;
			int			file_size;
			char		*read_buff;

			fopen_s( &hFile, pFilePath, "rb" );

			if ( hFile == NULL ){
				assert(0);
				return NULL;
			}

			fseek( hFile, 0, SEEK_END );
			file_size = ftell( hFile );
			fseek( hFile, 0, SEEK_SET );

			u32		align = 16;
			file_size += align;//必ずNULLが入るようにしたい。
			u32 buf_size = file_size + ((-file_size) & (align-1));

			read_buff = reinterpret_cast<c8*>( GLMemory::Malloc(buf_size) );
			memset( read_buff, 0, buf_size );
			fread( read_buff, file_size, 1, hFile );
			fclose( hFile );

			return read_buff;
#endif
    }
  };

	virtual bool OnInit( void )
	{
#if defined (GF_PLATFORM_CTR)
    // 仮
    // ファイルシステム初期化
    static const u32 ROMFS_BUFFER_SIZE = 1024 * 64;
    static char buffer[ROMFS_BUFFER_SIZE];
    nn::fs::Initialize();
    nn::fs::MountRom(16, 16, buffer, ROMFS_BUFFER_SIZE);

		#if GFL_DEBUG
    // HIO初期化
    /*m_pHostIoMemory = m_Allocator.SystemMemoryMalloc(nn::hio::WORKMEMORY_SIZE, 128);
		
    nn::Result initRes = nn::hio::Initialize(m_pHostIoMemory);
    if (initRes.IsFailure())
    {
      NN_LOG("Couldn't init host file!\n");
    }*/
		#endif

#endif
    GFGL::Initialize( &m_Allocator );

		m_HandleList[CtrDisplayNo::LEFT] = GFGL::CreateDisplay(DisplayMode::CTR_LEFT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::RIGHT] = GFGL::CreateDisplay(DisplayMode::CTR_RIGHT, Format::A8R8G8B8, Format::D24S8, MultiSample::None);
		m_HandleList[CtrDisplayNo::DOWN] = GFGL::CreateDisplay(DisplayMode::CTR_DOWN, Format::A8R8G8B8, Format::D24S8, MultiSample::None);

		m_pDepthStencilTexture = GFGL::CreateTextureFromSurface(GFGL::GetDepthStencilBuffer( m_HandleList[CtrDisplayNo::LEFT] ));

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::Initialize();

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();

			//描画パイプラインを作成
			m_pRenderingPipeLine = new MyRenderingPipeLine( 512, 256 );
			m_pRenderingPipeLine->SetDepthTexture( m_pDepthStencilTexture );
		}
		
    // ************************************************************
    m_pDisplayLists[0] = GFGL::CreateDisplayList();
    m_pDisplayLists[1] = GFGL::CreateDisplayList();

#if 1
		{
			scenegraph::resource::GfBinaryMdlData							resourceData;

			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pBinData = (c8*)LocalFunc::ReadData( &m_Allocator, "rom:/exports/pm0025_00.ctr.gfbmdl" );
#else
			c8		*pBinData = (c8*)LocalFunc::ReadData( &m_Allocator, "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/pm0025_00.wingl.gfbmdl" );
#endif
			
			resourceData.SetModelData( pBinData );

			m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
		}

		{
			static const char* pShaderFileList[] = {
#if defined(GF_PLATFORM_CTR)
				"rom:/exports/Shader/PDC_Body_Shadow.ctr.gfbvsh",
				"rom:/exports/Shader/PDC_pm_keikouo_Neon.ctr.gfbvsh",
				"rom:/exports/Shader/PDC_Body_Shadow.ctr.gfbfsh",
				"rom:/exports/Shader/PDC_pm_keikouo_Neon.ctr.gfbfsh",

				//"rom:/EdgeMapVColAShader/Shader/Comb2_Manual.ctr.gfbvsh",

				//"rom:/EdgeMap/exports/Shader/Comb3_Manual.ctr.gfvsh",

				//"rom:/IDEdgeMap/exports/Shader/Comb2_Manual.ctr.gfvsh",
				//"rom:/IDEdgeMap/exports/Shader/Comb3_Manual.ctr.gfvsh",
#else
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shaders/PokeNormal.wingl.gfbvsh",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shaders/PDC_Body_Shadow.wingl.gfbfsh",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shaders/PDC_pm_keikouo_Neon.wingl.gfbfsh",
#endif
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
			}
		}

		{
			static const c8*			pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
				"rom:/exports/Texture/pm0025_00_Eye1.ctr.tex",
				"rom:/exports/Texture/pm0025_00_EyeNor.ctr.tex",
				"rom:/exports/Texture/pm0025_00_EyeHlm.ctr.tex",
				"rom:/exports/Texture/pm0025_00_Mouth1.ctr.tex",
				"rom:/exports/Texture/pm0025_00_MouthNor.ctr.tex",
				"rom:/exports/Texture/pm0025_00_MouthHlm.ctr.tex",
				"rom:/exports/Texture/pm0025_00_BodyALta.ctr.tex",
				"rom:/exports/Texture/pm0025_00_BodyA1.ctr.tex",
				"rom:/exports/Texture/DummyTex.ctr.tex",
				"rom:/exports/Texture/pm0025_00_BodyANor.ctr.tex",
				"rom:/exports/Texture/Default_Lta_10D.ctr.tex",
				"rom:/exports/Texture/pm0025_00_Hta.ctr.tex",
				"rom:/exports/Texture/LinerTable.ctr.tex",
				"rom:/exports/Texture/pm0025_00_BodyAHlm.ctr.tex",
				"rom:/exports/Texture/pm0025_00_BodyB1.ctr.tex",
				"rom:/exports/Texture/pm0025_00_BodyBInc.ctr.tex",
				"rom:/exports/Texture/pm0025_00_BodyBNor.ctr.tex",
				"rom:/exports/Texture/pm0025_00_BodyBHlm.ctr.tex"
#else
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_Eye1.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_EyeNor.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_EyeHlm.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_Mouth1.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_MouthNor.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_MouthHlm.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyALta.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyA1.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/DummyTex.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyANor.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/Default_Lta_10D.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_Hta.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/LinerTable.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyAHlm.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyB1.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyBInc.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyBNor.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyBHlm.wingl.btex"
#endif
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

				pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
				m_pBinModelRootNode->AddChild( pBinTexRootNode );
			}
		}
#else
    {
#if defined(GF_PLATFORM_WIN32)
      const char* packFilePath = "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/pm0025_00.wingl.gfbmdlp";
#elif defined(GF_PLATFORM_CTR)
      //const char* packFilePath = "rom:/encgrass.ctr.gfbmdlp";
      const char* packFilePath = "rom:/pm0025_00.ctr.gfbmdlp";
#endif
      scenegraph::resource::GfBinaryMdlDataPack mdlPackData;
      c8* pMdlBinDataPack = (c8*)LocalFunc::ReadData( &m_Allocator, packFilePath);
      
      mdlPackData.SetModelData(pMdlBinDataPack);
      m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &mdlPackData );
    }
#endif

		{
			m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode );
		}

		if ( 0 )
		{// アニメーション初期化
			static const c8*				s_pMotionFilePath = {
				//"../data/pm0025_00/wait.gfmot",
#if defined(GF_PLATFORM_CTR)
				//"rom:/pm0025_00_ba20_buturi01.bgfmot",
				"rom:/pm0025_00_ba10_waitA01.bgfmot",
#else
				//"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00_ba20_buturi01.bgfmot",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00_ba10_waitA01.bgfmot",
#endif
			};

      scenegraph::resource::GfBinaryMotData animationResourceData;
      c8* pMotData = (c8*)LocalFunc::ReadData( &m_Allocator, s_pMotionFilePath ); // @note しまったこれだと開放できない
      animationResourceData.SetMotionData(pMotData);

			m_pMotionResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );

      AnimationController::InitDescription desc;
      desc.jointSlotCount = 1;
      desc.materialSlotCount = 1;
      desc.visibilitySlotCount = 1;
			m_AnimationController.Initialize( m_pModelInstanceNode, desc );

      // ジョイントアニメーション
      JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
      pSlot->SetAnimationResource(m_pMotionResourceRootNode);
      pSlot->SetStepFrame(0.5f);
      pSlot->SetLoop(true);

      // マテリアルアニメーション
      MaterialAnimationSlot* pMaterialSlot = m_AnimationController.GetMaterialSlot();
      pMaterialSlot->SetAnimationResource(m_pMotionResourceRootNode);
      pMaterialSlot->SetStepFrame(0.5f);
      pMaterialSlot->SetLoop(true);

      // ビジビリティアニメーション
      VisibilityAnimationSlot* pVisibilitySlot = m_AnimationController.GetVisibilitySlot();
      pVisibilitySlot->SetAnimationResource(m_pMotionResourceRootNode);
      pVisibilitySlot->SetStepFrame(0.5f);
      pVisibilitySlot->SetLoop(true);

		}

		m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );

#if defined(GF_PLATFORM_CTR)
    // 立体視設定
    GFGL::SetStereoModeEnable(false);
#endif

		m_cpuTime = m_gpuTime = 0;
		m_Visible = true;

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
					1000.0f);							// 後方投影面までの距離

			m_cbNeverChanges.Projection = math::Matrix44::GetOrthographic( 
				-100,
				100,
				-60,
				60,
				1.1f, 1000.0f );
			
			m_cbNeverChanges.Projection = m_cbNeverChanges.Projection;
		}

		{// 定数バッファ②を更新
			// ビュー変換行列
			math::Vector3 eyePosition( 100.0f, 20.0f, 100.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 20.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_cbChangesEveryFrame.View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
		}

#if 0
#if defined(GF_PLATFORM_WIN32)
		if ( m_AnimationControler.GetFrameSize() )
		{
			u32			animFrame = (m_FrameCnt/2) % m_AnimationControler.GetFrameSize();
			m_AnimationControler.ApplyAnimation( animFrame );
		}
#endif
#endif

#if defined(GF_PLATFORM_CTR)
	#if GFL_DEBUG
    // GPUプロファイラ用のファイルを出力
		if ( m_FrameCnt == 60 )
		{
			//GFGL::ExportOrcsFileRequest("asdf.orcs"); 
		}
	#endif
#endif


    if (m_Visible)
    {
      //m_AnimationController.Update();
    }

    // ピカチュウを回転させる
    math::Quaternion		rot;
    rot.RadianToQuaternion( 0, math::ConvDegToRad( (m_FrameCnt%360) ), 0 );
    m_pModelInstanceNode->SetLocalRotation(rot);

		{//トラバース処理にてワールド座標を更新する場合。
      scenegraph::SceneGraphManager::TraverseModelFast();
		}
		++m_FrameCnt;

		m_pRenderingPipeLine->Update();

		// GPU時間表示
    {
      f32 cpuTime = GFGL::GetTotalCPUTimeMicroSecond() / 1000.0f;
      f32 gpuTime = GFGL::GetTotalGPUTimeMicroSecond() / 1000.0f;

			m_cpuTime += cpuTime;
			m_gpuTime += gpuTime;

			if ( m_FrameCnt % 60 == 0 )
			{
				m_cpuTime /= 60;
				m_gpuTime /= 60;
				//nn::dbg::detail::Printf("*Total CPU : %.3fms | GPU : %.3fms\n", m_cpuTime, m_gpuTime);
			}
    }

		m_pModelInstanceNode->SetVisible( m_Visible );

		return true;
	}

	virtual bool OnDraw( void )
	{
    GFGL::SetActiveDisplay( m_HandleList[CtrDisplayNo::LEFT] );


		math::Matrix44 projMat[CtrDisplayNo::NumberOf];
    math::Matrix34 viewMat[CtrDisplayNo::NumberOf];

#if defined(GF_PLATFORM_CTR)
		{
			static f32 depthLevel = 4.0f;
			static f32 factor = 1.0f;

			GFGL::GetStereoMatrix(
				m_cbNeverChanges.Projection, m_cbChangesEveryFrame.View, depthLevel, factor, 
				&projMat[CtrDisplayNo::LEFT], &viewMat[CtrDisplayNo::LEFT], &projMat[CtrDisplayNo::RIGHT], &viewMat[CtrDisplayNo::RIGHT]);
		}

#else

		for (u32 i = 0; i < 2; ++i)
		{
			projMat[i] = m_cbNeverChanges.Projection;
			viewMat[i] = m_cbChangesEveryFrame.View;
		}

#endif

    for (u32 i = 0; i < 1; ++i)
		{
      GFGL::SetActiveDisplay( m_HandleList[i] );

      Surface		*pBackBuffer = GFGL::GetBackBuffer( m_HandleList[i] );
      Surface		*pDepthBuffer = GFGL::GetDepthStencilBuffer( m_HandleList[i] );
			u32				displayWidth = GFGL::GetDisplayWidth(m_HandleList[i]);
			u32				displayHeight = GFGL::GetDisplayHeight(m_HandleList[i]);

			m_pRenderingPipeLine->SetProjectionMatrix( projMat[i] );
			m_pRenderingPipeLine->SetViewMatrix( viewMat[i] );	

			m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
			m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
			m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

			m_pRenderingPipeLine->Execute();

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();	
		}

		gfl2::gfx::GFGL::Finish();


		GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
		GFGL::Finish();

		GFGL::SetActiveDisplay(NULL);

    for (u32 i = 0; i < CtrDisplayNo::NumberOf; ++i)
    {
      GFGL::DestroyDisplay(m_HandleList[i]);
    }
		GFGL::Terminate();

#if defined(GF_PLATFORM_CTR)

		#if GFL_DEBUG
			// HIO終了処理
			//nn::hio::Finalize();
		#endif
    m_Allocator.SystemMemoryFree(m_pHostIoMemory);
#endif
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
