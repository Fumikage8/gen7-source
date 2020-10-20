#include <heap/include/gfl2_Heap.h>
#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_MathAABB.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>


#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_AABBTree.h>

#include <Animation/include/gfl2_DrawEnvAnimationController.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::system;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

// アニメーションは30FPSで作られているが実行環境は60FPS
static const f32 SC_STEP_FRAME = 30.0f / 60.0f;

class GLAllocator : public IGLAllocator
{
public:
	GLAllocator():IGLAllocator(){}
	virtual ~GLAllocator(){}

#if GFL_DEBUG
    virtual void*	_SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
		{
			return malloc(size);
		}
#else
    virtual void*	SystemMemoryMalloc(u32 size, u32 alignment)
		{
			return malloc(size);
		}
#endif

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
	math::Matrix																							m_Projection;   // 透視変換行列
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode;
	scenegraph::resource::ResourceNode												*m_pEnvResourceRootNode;
	scenegraph::resource::ResourceNode												*m_pAnimationResourceRootNodeList[2];
  animation::DrawEnvAnimationController                     m_DrawEnvAnimationController;

	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode;
	DefaultRenderingPipeLine																	*m_RenderingPipeLine;
	scenegraph::instance::DrawEnvNode													*m_pDrawEnvNode;
	scenegraph::instance::LightSetNode												*m_pLightSetNode;
	scenegraph::instance::LightNode														*m_pDirectionalLightNode;
	scenegraph::instance::LightNode														*m_pAmbientLightNode;

	s32																												m_FrameCnt;
	u64																												m_PerformanceTime;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_PerformanceTime( 0 ),
		m_pDrawEnvNode( NULL )
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );
    GFGL::GLMemory::SetInstanceAllocator(&m_Allocator);

		m_Handle = GFGL::CreateDisplay( &m_Allocator, DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::None );

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::InitDescription			desc(1024 * 2);
			desc.m_AABBTreeSize.SetMinMax( math::Vector3(-500.0f, -500.0f, -500.0f), math::Vector3(500.0f, 500.0f, 500.0f) );
			scenegraph::SceneGraphManager::Initialize(&m_Allocator);

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize(&m_Allocator);
		
			//描画パイプラインを作成
			m_RenderingPipeLine = new DefaultRenderingPipeLine(&m_Allocator);
		}

		{
			scenegraph::resource::GfMdlData							resourceData;
			resourceData.ReadFile( "../data/EnvTestMap/DrawEnvTest.gfmdl" );

			m_pModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &m_Allocator, &resourceData );
			m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( &m_Allocator, m_pModelResourceRootNode );
		}

		{
			scenegraph::resource::GfEnvData							resourceData;
			resourceData.ReadFile( "../data/EnvTestMap/DrawEnvTest.gfenv" );

			m_pEnvResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &m_Allocator, &resourceData );
			m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNode( &m_Allocator, m_pEnvResourceRootNode );
		}

    static const char* sc_animationFilePathList[] = 
    {
      "../data/EnvTestMap/DrawEnvTest.gfmot",
      "../data/EnvTestMap/DrawEnvTest2.gfmot",
    };

    m_DrawEnvAnimationController.Initialize(gfl2::heap::Manager::GetDefaultNewUseHeap(), m_pDrawEnvNode);

    for (u32 i = 0; i < gfl2::animation::DrawEnvAnimationController::SLOT_TYPE_COUNT; ++i)
		{
			scenegraph::resource::GfMotData							resourceData;
			resourceData.ReadFile(sc_animationFilePathList[i]);

			m_pAnimationResourceRootNodeList[i] = scenegraph::resource::ResourceManager::CreateResourceNode( &m_Allocator, &resourceData );

      // ライトアニメーション
      animation::LightAnimationSlot* pLightSlot = m_DrawEnvAnimationController.GetLightSlot(0, static_cast<gfl2::animation::DrawEnvAnimationController::SlotType>(i));
      pLightSlot->SetAnimationResource(m_pAnimationResourceRootNodeList[i]);

      pLightSlot->SetStepFrame(SC_STEP_FRAME);
      pLightSlot->SetLoop(true);

      // カメラアニメーション
      animation::CameraAnimationSlot* pCameraSlot = m_DrawEnvAnimationController.GetCameraSlot(0, static_cast<gfl2::animation::DrawEnvAnimationController::SlotType>(i));
      pCameraSlot->SetAnimationResource(m_pAnimationResourceRootNodeList[i]);

      pCameraSlot->SetStepFrame(SC_STEP_FRAME);
      pCameraSlot->SetLoop(true);

      // フォグアニメーション
      animation::FogAnimationSlot* pFogSlot = m_DrawEnvAnimationController.GetFogSlot(0, static_cast<gfl2::animation::DrawEnvAnimationController::SlotType>(i));
      pFogSlot->SetAnimationResource(m_pAnimationResourceRootNodeList[i]);

      pFogSlot->SetStepFrame(SC_STEP_FRAME);
      pFogSlot->SetLoop(true);

      // グレア(ブルーム)アニメーション
      animation::GlareAnimationSlot* pGlareSlot = m_DrawEnvAnimationController.GetGlareSlot();
      pGlareSlot->SetAnimationResource(m_pAnimationResourceRootNodeList[i]);

      pGlareSlot->SetStepFrame(SC_STEP_FRAME);
      pGlareSlot->SetLoop(true);
    }

    // 補間リクエスト
    m_DrawEnvAnimationController.LightInterpRequest(10);
    m_DrawEnvAnimationController.CameraInterpRequest(50);
    m_DrawEnvAnimationController.FogInterpRequest(10);
    m_DrawEnvAnimationController.GlareInterpRequest(10);

		m_RenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );

		return true;
	}

	virtual bool OnUpdate( void )
	{
    // アニメーションコントローラー更新
    m_DrawEnvAnimationController.Update();

    //kawa20150514
    /*
    {
      if(m_pDrawEnvNode)
      {
        static const u32 COUNT_MAX = 30;
        static u32 count = 0;
        static f32 camRotXArray[COUNT_MAX] = { 0 };
        static f32 camRotYArray[COUNT_MAX] = { 0 };
        static f32 camRotZArray[COUNT_MAX] = { 0 };
        static f32 camRotWArray[COUNT_MAX] = { 0 };
        static f32 fogNearLength[COUNT_MAX] = { 0 };
        static f32 glareStrength[COUNT_MAX] = { 0 };
        
        // camera
        {
          scenegraph::instance::CameraNode* pCameraNode = m_pDrawEnvNode->GetCamera(0);
          const math::Quaternion& rot = pCameraNode->GetLocalRotation();
          f32 valX = rot.GetX();
          f32 valY = rot.GetY();
          f32 valZ = rot.GetZ();
          f32 valW = rot.GetW();
          camRotXArray[count] = valX;
          camRotYArray[count] = valY;
          camRotZArray[count] = valZ;
          camRotWArray[count] = valW;
        }

        // fog
        {
          f32 val = m_pDrawEnvNode->GetFogNearLength();
          fogNearLength[count] = val;
        }

        // glare
        {
          f32 val = m_pDrawEnvNode->GetGlareStrength();
          glareStrength[count] = val;
        }

        count = (count+1) % COUNT_MAX;
      }
    }
    */

    // ワールド行列更新
    scenegraph::SceneGraphManager::TraverseModel();

		if ( m_FrameCnt % 60 == 0 )
		{
			char		buff[32];
			memset(buff, 0, sizeof(buff));
			sprintf_s( buff, sizeof(buff), "%d\n", static_cast<s32>(m_PerformanceTime / 60) );
			::OutputDebugStringA( buff );
			m_PerformanceTime = 0;
		}

		++m_FrameCnt;

		return true;
	}

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

		m_RenderingPipeLine->SetProjectionMatrix( m_pDrawEnvNode->GetCamera( 0 )->GetProjectionMatrix() );
		m_RenderingPipeLine->SetViewMatrix( m_pDrawEnvNode->GetCamera( 0 )->GetViewMatrix() );

		m_RenderingPipeLine->SetRenderTarget( pBackBuffer );
		m_RenderingPipeLine->SetDepthStencil( pDepthBuffer );

		m_RenderingPipeLine->Execute();

		{
			class TransformNodeVisitor
			{
			public:
				TransformNodeVisitor(){}
				virtual ~TransformNodeVisitor() {}

				void Visit( scenegraph::instance::TransformNode* pNode )
				{
					pNode->SetTransformDirty( false );
				}
			};

			scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitor>				visitor;
			scenegraph::SceneGraphManager::Traverse<scenegraph::instance::TransformNode, TransformNodeVisitor>( &visitor );
		}

    GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
		if (m_pDrawEnvNode != NULL) delete m_pDrawEnvNode;
		if (m_pEnvResourceRootNode != NULL) delete m_pEnvResourceRootNode;
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