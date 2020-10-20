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
//#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <Animation/include/gfl2_AnimationController.h>

#include <math/include/gfl2_QuaternionNative.h>
#include <math/include/gfl2_Quaternion.h>

#include <math/include/gfl2_EulerRotation.h>

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

	virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
	{
		return malloc(size);
	}

	/*virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment){
		return malloc(size);
	}*/

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
	static const u32 MODEL_MAX = 2;
	math::Matrix44																							m_Projection;
	math::Matrix34																							m_View;
	math::Matrix34																							m_World;

	renderer::DefaultRenderingPipeLine												*m_RenderingPipeLine;
	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode;
	scenegraph::resource::ResourceNode												*m_pMotionResourceRootNodeList[MODEL_MAX];
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode;
	AnimationController																				m_AnimationControler;

	u32																					              m_FrameCnt;
  u32                                                       m_AnimationNo;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
    m_AnimationNo(0)
	{
	
	}

	virtual ~SimpleDraw(){}

  static void AssertLoop(const char* buff, b32 isStop)
  {
    assert(false);
  }

	virtual bool OnInit( void )
	{
    gfl2::debug::GFL_ASSERT_SetLoopFunc(&AssertLoop);

		math::Matrix				lerpMat;
		math::Vector				col0(0, 0, 0, 0);

		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::None );

		{
			scenegraph::SceneGraphManager::Initialize();
			scenegraph::resource::ResourceManager::Initialize();
			m_RenderingPipeLine = new renderer::DefaultRenderingPipeLine();
		}

		{
			static const c8*				s_pModelFilePath = {
        "../data/pm0025_00/pm0025_00.gfmdl",
			};

			static const c8*				s_pMotionFilePathList[MODEL_MAX] = {
        "../data/pm0025_00/pm0025_00_ba20_buturi01.gfmot",
        "../data/pm0025_00/pm0025_00_ba20_buturi02.gfmot",
			};

			scenegraph::resource::GfMdlData modelResourceData;
			scenegraph::resource::GfMotData motionResourceDataList[MODEL_MAX];

			modelResourceData.ReadFile( s_pModelFilePath );
			m_pModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &modelResourceData );

      for (u32 i = 0; i < MODEL_MAX; ++i)
      {
        motionResourceDataList[i].ReadFile(s_pMotionFilePathList[i]);
        m_pMotionResourceRootNodeList[i] = scenegraph::resource::ResourceManager::CreateResourceNode( &motionResourceDataList[i] );
      }
		}

		{
			m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode );

#if MOTION_PARTIAL_TEST 
      AnimationController::InitDescription desc;
      desc.jointSlotCount = 2;
			m_AnimationControler.Initialize( m_pModelInstanceNode, desc );
      
      // Slot0:下半身は走りモーション
      JointAnimationSlot* pSlot0 = m_AnimationControler.GetJointSlot(0);
      pSlot0->SetAnimationResource(m_pMotionResourceRootNodeList[0]);
      pSlot0->SetLoop(true);

      // Slot1:上半身は指差し
      JointAnimationSlot* pSlot1 = m_AnimationControler.GetJointSlot(1);
      pSlot1->SetAnimationResource(m_pMotionResourceRootNodeList[1]);
      pSlot1->SetLoop(true);

      // Slot1:下半身のジョイントを無効化
      static const c8* s_LowerBodyJointList[] =
      {
        "Waist",
        "Hips",
        "EndHips",
        "LThigh",
        "LLeg",
        "LFoot",
        "LToe",
        "RThigh",
        "RLeg",
        "RFoot",
        "RToe",
      };
      for (u32 i = 0; i < sizeof(s_LowerBodyJointList) / sizeof(s_LowerBodyJointList[0]); ++i)
      {
        pSlot1->SetPartialAnimation(s_LowerBodyJointList[i], false);
      }

#elif MOTION_INTERP_TEST

			m_AnimationControler.Initialize( gfl2::heap::Manager::GetDefaultNewUseHeap(),  m_pModelInstanceNode );

      AnimationSlotBase* pSlotList[] =
      {
        m_AnimationControler.GetJointSlot(),
        m_AnimationControler.GetMaterialSlot(),
      };

      for (u32 i = 0; i < sizeof(pSlotList) / sizeof(pSlotList[0]); ++i)
      {
        AnimationSlotBase* pSlot = pSlotList[i];
        pSlot->SetAnimationResource(m_pMotionResourceRootNodeList[m_AnimationNo]);
        pSlot->SetLoop(true);

        pSlot->SetStepFrame(SC_STEP_FRAME * 0.5f);
      }
      //pSlot->SetStepFrame(1.0f);
#endif
		}

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
		}

		return true;
	}

	virtual bool OnUpdate( void )
	{
    //Sleep(16);
		{
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 30.0f, 150.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 20.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

#if MOTION_PARTIAL_TEST 
    {
      // 部分再生テスト

      m_AnimationControler.Update();
			m_pModelInstanceNode->SetLocalMatrix( math::Matrix::GetRotationY( math::ConvDegToRad(90.0f) ) );
    }
#elif MOTION_INTERP_TEST
		{
      // モーション補完テスト

#if 0
      // 適当なフレーム数再生したらモーション補完してみる
      if (++m_FrameCnt > 80)
      {
        m_FrameCnt = 0;
        m_AnimationNo ^= 1;

        JointAnimationSlot* pSlot = m_AnimationControler.GetJointInterpSlot();
        pSlot->SetAnimationResource(m_pMotionResourceRootNodeList[m_AnimationNo]);
        pSlot->SetLoop(true);
        pSlot->SetStepFrame(SC_STEP_FRAME);

        f32 interpFrame = (m_AnimationNo == 0) ? 30.0f : 10.0f;
        m_AnimationControler.InterpRequest(interpFrame);
      }
#endif

      m_AnimationControler.Update();
      math::Quaternion rot;
      rot.RadianToQuaternion(0, math::ConvDegToRad(30), 0);
      m_pModelInstanceNode->SetLocalRotation(rot);
		}
#endif

    // ワールド行列更新
    scenegraph::SceneGraphManager::TraverseModel();

		return true;
	}

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

		m_RenderingPipeLine->SetRenderTarget( pBackBuffer );
		m_RenderingPipeLine->SetDepthStencil( pDepthBuffer );

		m_RenderingPipeLine->Execute();

    GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{

    for (u32 i = 0; i < MODEL_MAX; ++i)
    {
      if (m_pMotionResourceRootNodeList[i] != NULL) delete m_pMotionResourceRootNodeList[i];
    }
    if (m_pModelResourceRootNode != NULL) delete m_pModelResourceRootNode;
    if (m_pModelInstanceNode != NULL) delete m_pModelInstanceNode;

		if (m_RenderingPipeLine != NULL) delete m_RenderingPipeLine;

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
		memset( read_buff, 0, buf_size );
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