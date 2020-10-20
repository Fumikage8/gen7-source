#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <Animation/include/gfl2_AnimationController.h>

#include <math/include/gfl2_QuaternionNative.h>
#include <math/include/gfl2_Quaternion.h>

#include <math/include/gfl2_EulerRotation.h>

#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <Animation/include/gfl2_AnimationPack.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::animation;

// どこか1箇所だけ1にして, あとは0にしてからビルドしてください.
#define MOTION_INTERP_TEST 1 
#define MOTION_PARTIAL_TEST 0 

// アニメーションは30FPSで作られているが実行環境は60FPS
static const f32 SC_STEP_FRAME = 30.0f / 60.0f;

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

  static const u32 MODEL_COUNT = 2;
  static const u32 ANIM_COUNT = 3;

	math::Matrix44															m_Projection;
	math::Matrix34															m_View;
	math::Matrix34															m_World;

	renderer::DefaultRenderingPipeLine				*m_pRenderingPipeLine;

	u32																			m_FrameCnt;
  u32                                       m_AnimationNo;

  poke_3d::model::CharaModelFactory         m_CharaModelFactory;
  poke_3d::model::CharaModel                m_CharaModelList[MODEL_COUNT];
  scenegraph::resource::ResourceNode*       m_pModelResourceRootNode;                     ///< モデルリソース
	scenegraph::resource::ResourceNode*       m_pAnimationResourceRootNodeList[ANIM_COUNT]; ///< アニメーションリソース

  s32                                       m_Counter;

  u32                                       m_UpdateNo;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
    m_AnimationNo(0),
    m_Counter(0),
    m_UpdateNo(0)
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		math::Matrix				lerpMat;
		math::Vector				col0(0, 0, 0, 0);

		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::None );

		{
      scenegraph::SceneGraphManager::InitDescription desc(100000);
      
			scenegraph::SceneGraphManager::Initialize(desc);
			scenegraph::resource::ResourceManager::Initialize();
			m_pRenderingPipeLine = new renderer::DefaultRenderingPipeLine();
		}

    u32 bufSize = 0x100000;
    void* pBuf = malloc(bufSize);
    heap::Manager::Initialize(pBuf, bufSize, 1);

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			m_pRenderingPipeLine->SetProjectionMatrix( m_Projection );
		}

		return true;
	}

	virtual bool OnUpdate( void )
	{
    u32 dummyId = 0;
    switch (m_UpdateNo)
    {
    case 0:
      {
        // 非同期読み込み開始
        m_CharaModelFactory.LoadAsync(dummyId);
      }
      ++m_UpdateNo;
      break;

    case 1:
      {
        // 読み込み終了待ち
        if (!m_CharaModelFactory.IsLoaded(dummyId))
        {
          break;
        }

        // モデル作成
        for (u32 i = 0; i < MODEL_COUNT; ++i)
        {
          poke_3d::model::CharaModel* pCharaModel = &m_CharaModelList[i];

          // 作成
          bool result = m_CharaModelFactory.Create(gfx::GLMemory::GetAllocator(), heap::Manager::GetDefaultNewUseHeap(), pCharaModel, dummyId);
          GFL_ASSERT(result);

          // 位置設定
          pCharaModel->SetPosition(0, 0, 0);

          // 回転設定
          pCharaModel->SetRotation(0, 0, 0);

          // 拡縮設定
          pCharaModel->SetScale(1, 1, 1);
        }

        // 親子関係
        m_CharaModelList[1].SetParent(&m_CharaModelList[0], 6);

        // アニメーションリソース読み込み
        {
          static const c8* s_AnimatinPathList[] =
          {
            "R:/home/niji/git_program/gflib2/skelton/resource/pm0025_00/pm0025_00_ba10_waitA01.gfmot",
            "R:/home/niji/git_program/gflib2/skelton/resource/pm0025_00/pm0025_00_ba51_walk01.gfmot",
            "R:/home/niji/git_program/gflib2/skelton/resource/pm0025_00/pm0025_00_ba52_run01.gfmot",
          };

          for (u32 i = 0; i < ANIM_COUNT; ++i)
          {
            scenegraph::resource::GfMotData animationResourceData;
            animationResourceData.ReadFile(s_AnimatinPathList[i]);
            m_pAnimationResourceRootNodeList[i] = scenegraph::resource::ResourceManager::CreateResourceNode(&animationResourceData);
          }
        }


        m_CharaModelList[0].ChangeAnimationByResourceNode(m_pAnimationResourceRootNodeList[m_AnimationNo]);
        m_CharaModelList[0].SetAnimationStepFrame(0.5f);
        m_CharaModelList[1].ChangeAnimationByResourceNode(m_pAnimationResourceRootNodeList[m_AnimationNo]);
        m_CharaModelList[1].SetAnimationStepFrame(0.5f);
      }
      ++m_UpdateNo;
      break;

    case 2:
      {
        {
          // ビュー変換行列
          math::Vector3 eyePosition( 80.0f, 60.0f, 150.0f );  // 視点(カメラの位置)
          math::Vector3 focusPosition( 0.0f, 40.0f,  0.0f );  // 注視点
          math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

          m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

          m_pRenderingPipeLine->SetViewMatrix( m_View );
        }

        // 親
        {
          math::Vector3 rot = m_CharaModelList[0].GetRotation();
          m_CharaModelList[0].SetRotation(rot);
        }

        // 子
        {
          math::Vector3 rot = m_CharaModelList[1].GetRotation();
          rot.z = -PI / 2;
          m_CharaModelList[1].SetRotation(rot);

          m_CharaModelList[1].SetScale(0.4f, 0.4f, 0.4f);
        }

        if (++m_Counter >= 120)
        {
          m_Counter = 0;
          ++m_AnimationNo;
          if (m_AnimationNo >= ANIM_COUNT)
          {
            m_AnimationNo = 0;
          }

          m_CharaModelList[0].ChangeAnimationSmoothByResourceNode(m_pAnimationResourceRootNodeList[m_AnimationNo], 20);
          m_CharaModelList[1].ChangeAnimationSmoothByResourceNode(m_pAnimationResourceRootNodeList[m_AnimationNo], 20);
        }

        m_CharaModelList[0].SetAnimationStepFrame(0.5f);
        m_CharaModelList[0].UpdateAnimation();
        m_CharaModelList[1].SetAnimationStepFrame(0.5f);
        m_CharaModelList[1].UpdateAnimation();
      }
      break;
    }


    // ワールド行列更新
    scenegraph::SceneGraphManager::TraverseModel();

		return true;
	}

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

		m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
		m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );

		m_pRenderingPipeLine->Execute();

    GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
    for (u32 i = 0; i < MODEL_COUNT; ++i)
    {
      m_CharaModelList[i].SetParent(NULL);
    }
    for (u32 i = 0; i < MODEL_COUNT; ++i)
    {
      m_CharaModelList[i].Destroy();
    }
    m_CharaModelFactory.UnloadAll();

    delete m_pModelResourceRootNode;
    for (u32 i = 0; i < ANIM_COUNT; ++i)
    {
      delete m_pAnimationResourceRootNodeList[i];
    }

		if (m_pRenderingPipeLine != NULL) delete m_pRenderingPipeLine;

		scenegraph::resource::ResourceManager::Terminate();
    scenegraph::SceneGraphManager::Terminate();

		GFGL::Finish();

		GFGL::SetActiveDisplay(NULL);
		GFGL::DestroyDisplay(m_Handle);
		GFGL::Terminate();
		return true;
	}

	//いずれＩＯクラスができたら置き換える。
	static c8* ReadFile( const char* filename, s32* p_size, s32 align = 16 )
	{
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
		::std::memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

	
		return( read_buff );
	}


private:
	DisplayDeviceHandle					m_Handle;
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
