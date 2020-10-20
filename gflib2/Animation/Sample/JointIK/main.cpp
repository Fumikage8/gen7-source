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
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_McnfData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

#include <math/include/gfl2_QuaternionNative.h>
#include <math/include/gfl2_Quaternion.h>

#include <system/include/InputDevice/gfl2_Mouse.h>
#include <system/include/InputDevice/gfl2_Pad.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::animation;

using namespace gfl2::system;

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
	scenegraph::resource::ResourceNode*       m_pMotionConfigResourceRootNode;      ///< モーション設定リソース
	scenegraph::instance::ModelInstanceNode*  m_pModelInstanceNode;                 ///< モデルインスタンス
	scenegraph::instance::MotionConfigInstanceNode*  m_pMotionConfigInstanceNode;   ///< モーションコンフィグインスタンス
	AnimationController                       m_AnimationController;                ///< アニメーションコントローラー
  JointController                           m_JointController;                    ///< ジョイントコントローラー

	u32                                       m_FrameCnt;                           ///< フレームカウント
  u32                                       m_AnimationNo;                        ///< アニメーション番号

  f32                                       m_InterestWeight;                     ///< 注視ウェイト
  math::Vector                              m_InterestPosition;                   ///< 注視位置

  f32                                       m_ikPosY;

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

    InputDevice::Mouse::Initialize();
    InputDevice::Pad::Initialize();

		{
			scenegraph::SceneGraphManager::Initialize();
			scenegraph::resource::ResourceManager::Initialize();
			m_RenderingPipeLine = new renderer::DefaultRenderingPipeLine();
		}

		{
			static const c8* s_pModelFilePath = "C:/home/niji/git_gflib2/Animation/Sample/data/tr0001old/tr0001old.gfmdl";
			static const c8* s_pMotionConfigFilePath = "C:/home/niji/git_gflib2/Animation/Sample/data/tr0001old/tr0001old.mcnf";
			//static const c8* s_pAnimationFilePath = "C:/home/niji/git_gflib2/Animation/Sample/data/tr0001old/tr0001_00_fi01_wait01.gfmot";
			static const c8* s_pAnimationFilePath = "C:/home/niji/git_gflib2/Animation/Sample/data/tr0001old/tr0001_00_fi20_walk01.gfmot";

      // モデル読み込み
			scenegraph::resource::GfMdlData modelResourceData;
			modelResourceData.ReadFile(s_pModelFilePath);
			m_pModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&modelResourceData);

      // モーション設定
      scenegraph::resource::McnfData motionConfigResourceData;
      motionConfigResourceData.ReadFile(s_pMotionConfigFilePath);
      m_pMotionConfigResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&motionConfigResourceData);

      // アニメーション
			scenegraph::resource::GfMotData animationResourceData;
			animationResourceData.ReadFile(s_pAnimationFilePath);
			m_pAnimationResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&animationResourceData);
		}

		{
			m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode );

      AnimationController::InitDescription desc;
      desc.jointSlotCount = 1;
			m_AnimationController.Initialize( m_pModelInstanceNode, desc );

      // ジョイントアニメーション
      JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
      pSlot->SetAnimationResource(m_pAnimationResourceRootNode);
      pSlot->SetLoop(true);
      pSlot->SetStepFrame(0.1f);
		}

    {
      // ジョイントコントローラー
      m_pMotionConfigInstanceNode = scenegraph::instance::InstanceCreator::CreateMotionConfigInstanceNode(m_pMotionConfigResourceRootNode);
      m_JointController.Initialize(m_pModelInstanceNode, m_pMotionConfigInstanceNode);

      // IKのコールバックを登録
      m_JointController.GetIKController()->SetCallback(IKCallback, this);
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

    m_InterestWeight = 0.0f;
    m_ikPosY = 30.5f;

		return true;
	}

	virtual bool OnUpdate( void )
	{
    InputDevice::Mouse::Update();
    InputDevice::Pad::Update();

		{
			// ビュー変換行列
			math::Vector3 eyePosition( 200.0f, 200.0f, 300.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 100.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

    // ターゲット移動
    math::Vector targetPos(0.0f, 100.0f, 100.0f);

    InputDevice::MousePos mousePos = InputDevice::Mouse::GetLeftDownOriginPosRatio();
    mousePos.x = mousePos.x * 2.0f - 1.0f;
    mousePos.y = mousePos.y * 2.0f - 1.0f;

    targetPos.x = mousePos.x * 360;
    targetPos.y = mousePos.y * 240;

    // アニメーション更新
    m_AnimationController.Update();

    // ワールド行列更新
    scenegraph::SceneGraphManager::TraverseModel();

    // IK目標位置設定
    ++m_FrameCnt;

    InputDevice::StickPos leftStick = InputDevice::Pad::GetStick(0, InputDevice::Stick::Left);
    m_ikPosY -= leftStick.y;

    // ワールド行列更新後にジョイントコントローラーを更新
    m_JointController.Update();

		return true;
	}

  static bool IKCallback(math::Vector3* out_pPosition, math::Vector3* out_pNormal, const gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode, void* pUserData)
  {
    const c8* pNodeName = pJointNode->GetNodeName();
    if (strcmp(pNodeName, "LFoot") == 0)
    {
      return false;
    }

    SimpleDraw* pInstance = reinterpret_cast<SimpleDraw*>(pUserData);

    *out_pPosition = pJointNode->GetWorldMatrix().GetElemPosition();

    out_pPosition->y = pInstance->m_ikPosY;

    return true;
  }

  bool IsToutch(const math::Vector3& touchPos, const math::Vector3& bonePos)
  {
    math::Vector3 tmp = m_View.TransformCoord(bonePos);
    tmp = m_Projection.TransformCoord(tmp);

    tmp.x *= 360;
    tmp.y *= 240;

    f32 downForce = 0.0f;
    if (InputDevice::Mouse::IsPress(InputDevice::MouseButton::Left))
    {
      f32 subx = tmp.x - touchPos.x;
      f32 suby = tmp.y - touchPos.y;
      if ((subx * subx) + (suby * suby) < 500)
      {
        return true;
      }
    }
    return false;
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
    m_JointController.Finalize();
    m_AnimationController.Finalize();

    if (m_pMotionConfigResourceRootNode != NULL) delete m_pMotionConfigResourceRootNode;
    if (m_pAnimationResourceRootNode != NULL) delete m_pAnimationResourceRootNode;
    if (m_pModelResourceRootNode != NULL) delete m_pModelResourceRootNode;
    if (m_pModelInstanceNode != NULL) delete m_pModelInstanceNode;

    if (m_RenderingPipeLine != NULL) delete m_RenderingPipeLine;

		scenegraph::resource::ResourceManager::Terminate();
    scenegraph::SceneGraphManager::Terminate();

    InputDevice::Pad::Terminate();
    InputDevice::Mouse::Terminate();

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