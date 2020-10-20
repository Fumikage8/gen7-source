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
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <Animation/include/gfl2_AnimationController.h>

#include <math/include/gfl2_QuaternionNative.h>
#include <math/include/gfl2_Quaternion.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::animation;

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
	math::Matrix44                              m_Projection;                         ///< 射影行列
	math::Matrix34                              m_View;                               ///< ビュー行列
	math::Matrix34                              m_World;                              ///< ワールド行列

	renderer::DefaultRenderingPipeLine*       m_RenderingPipeLine;                  ///< レンダリングパイプライン
	scenegraph::resource::ResourceNode*       m_pModelResourceRootNode;             ///< モデルリソース
	scenegraph::resource::ResourceNode*       m_pAnimationResourceRootNode;         ///< アニメーションリソース
	scenegraph::instance::ModelInstanceNode*  m_pModelInstanceNode;                 ///< モデルインスタンス
	AnimationController                       m_AnimationController;                ///< アニメーションコントローラー

	u32                                       m_FrameCnt;                           ///< フレームカウント
  u32                                       m_AnimationNo;                        ///< アニメーション番号

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
    m_AnimationNo(0)
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		math::Matrix				lerpMat;
		math::Vector				col0(0, 0, 0, 0);

		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

		{
			scenegraph::SceneGraphManager::Initialize();
			scenegraph::resource::ResourceManager::Initialize();
			m_RenderingPipeLine = new renderer::DefaultRenderingPipeLine();
		}

		{
			static const c8* s_pModelFilePath = "../data/pm0501_00_mijumaru/pm0501_00.gfmdl";
			//static const c8* s_pAnimationFilePath = "../data/pm0501_00_mijumaru/pm0501_00_ba20_buturi01.gfmot";
			static const c8* s_pAnimationFilePath = "R:/home/niji/git_program/tool_project/AnimationConverter/test_file/new_format/pm0501_00_ba20_buturi01.bgfmot";


      // モデル読み込み
			scenegraph::resource::GfMdlData modelResourceData;
			modelResourceData.ReadFile(s_pModelFilePath);
			m_pModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&modelResourceData);

      // アニメーション
			scenegraph::resource::GfBinaryMotData animationResourceData;
      s32 fileSize = 0;
      c8* pBinaryMot = this->ReadFile(s_pAnimationFilePath, &fileSize);
      animationResourceData.SetMotionData(pBinaryMot);
			m_pAnimationResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&animationResourceData);
		}

		{
			m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode );

      AnimationController::InitDescription desc;
      desc.jointSlotCount = 1;
      desc.materialSlotCount = 1;
      desc.keySlotCount = 1;
			m_AnimationController.Initialize( m_pModelInstanceNode, desc );

      AnimationSlotBase* pSlotList[] =
      {
        m_AnimationController.GetJointSlot(),
        m_AnimationController.GetMaterialSlot(),
        m_AnimationController.GetKeySlot(),
      };

      for (u32 i = 0; i < sizeof(pSlotList) / sizeof(pSlotList[0]); ++i)
      {
        AnimationSlotBase* pSlot = pSlotList[i];
        pSlot->SetAnimationResource(m_pAnimationResourceRootNode);
        pSlot->SetLoop(true);

        pSlot->SetStepFrame(SC_STEP_FRAME * 0.5f);
      }

      m_AnimationController.GetKeySlot()->SetCallback(this, KeyCallback);
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
		{
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 100.0f, 300.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 100.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

    m_AnimationController.Update();

    m_pModelInstanceNode->SetLocalTranslate(math::Vector3(0, 50, 0));
    math::Quaternion rot;
    rot.RadianToQuaternion(0, math::ConvDegToRad(30), 0);
    m_pModelInstanceNode->SetLocalRotation(rot);

    // ワールド行列更
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

		return true;
	}

	virtual bool OnTerm( void )
	{
    if (m_pAnimationResourceRootNode != NULL) delete m_pAnimationResourceRootNode;
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

  static void KeyCallback(const c8* pKeyName, f32 value, void* pUserData)
  {
    c8 buf[256];
    sprintf_s(buf, "%s, %f\n", pKeyName, value);
    OutputDebugStringA(buf);
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