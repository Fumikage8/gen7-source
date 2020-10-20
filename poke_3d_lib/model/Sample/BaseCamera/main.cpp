#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
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
#include <math/include/gfl2_PathController.h>
#include <math/include/gfl2_Easing.h>

//#include "../../include/gfl2_BaseModelResource.h"
#include "../../include/gfl2_BaseModel.h"
#include "../../include/gfl2_BaseCamera.h"

#include <Animation/include/gfl2_AnimationPack.h>

#include <system/include/InputDevice/gfl2_Pad.h>

#include <vector>

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

#if GFL_DEBUG
  virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
  {
		return malloc(size);
  }
#else
  virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment)
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

	renderer::DefaultRenderingPipeLine*       m_pRenderingPipeLine;

  scenegraph::instance::DrawEnvNode*        m_pDrawEnvNode;

  scenegraph::resource::ResourceNode*       m_pModelResourceRootNode;                     ///< モデルリソース
  scenegraph::resource::ResourceNode*       m_pDrawEnvResourceRootNode;                   ///< DrawEnvリソース
	scenegraph::resource::ResourceNode*       m_pAnimationResourceRootNode;                 ///< アニメーションリソース

  poke_3d::model::BaseModel                 m_BaseModel;
  poke_3d::model::BaseCamera                m_BaseCamera;

  s32                                       m_Rno;
  s32                                       m_Counter;

  math::PathController                      m_PathController;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
    m_pRenderingPipeLine(NULL),
    m_pDrawEnvNode(NULL),
    m_pModelResourceRootNode(NULL),
    m_pDrawEnvResourceRootNode(NULL),
    m_pAnimationResourceRootNode(NULL),
    m_Rno(0),
    m_Counter(0)
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
    std::vector<f32> valueList[math::Easing::INOUT_BOUNCE + 1];
    u32 divCount = 20;
    for (u32 type = 0; type <= math::Easing::INOUT_BOUNCE; ++type)
    {
      for (u32 i = 0; i <= divCount; ++i)
      {
        f32 value = math::Easing::GetValue(math::Easing::EaseFunc(type), (f32)i / divCount);
        valueList[type].push_back(value);
      }
    }

		math::Matrix				lerpMat;
		math::Vector				col0(0, 0, 0, 0);

		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( &m_Allocator, DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::None );

		{
      scenegraph::SceneGraphManager::InitDescription desc(100000);
      
			scenegraph::SceneGraphManager::Initialize(&m_Allocator, desc);
			scenegraph::resource::ResourceManager::Initialize(&m_Allocator);
			m_pRenderingPipeLine = new renderer::DefaultRenderingPipeLine(&m_Allocator);
		}

    u32 bufSize = 0x100000;
    void* pBuf = malloc(bufSize);
    heap::Manager::Initialize(pBuf, bufSize, 1);

    gfl2::system::InputDevice::Pad::Initialize();

    // DrawEnv読み込み、作成
    {
      scenegraph::resource::GfEnvData envResourceData;
      envResourceData.ReadFile("R:/home/niji/git_program/gflib2/Animation/Sample/data/EnvTestMap/DrawEnvTest.gfenv");
      m_pDrawEnvResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&m_Allocator, &envResourceData);
      m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNode(&m_Allocator, m_pDrawEnvResourceRootNode);

      // ライトは邪魔
      for (u32 i = 0; i < m_pDrawEnvNode->LightSetMax; ++i)
      {
        gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSetNode = m_pDrawEnvNode->GetLightSet(i);
        if (pLightSetNode == NULL)
        {
          continue;
        }
        pLightSetNode->RemoveAllLight();
      }

      m_BaseCamera.Create(gfl2::gfx::GLMemory::GetAllocator(), heap::Manager::GetDefaultNewUseHeap(), m_pDrawEnvNode, 0);
    }

    // モデルリソース読み込み
    {
      scenegraph::resource::GfMdlData modelResourceData;
      modelResourceData.ReadFile("R:/home/niji/git_program/gflib2/Animation/Sample/data/EnvTestMap/DrawEnvTest.gfmdl");
      m_pModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&m_Allocator, &modelResourceData);
    }

    // アニメーションリソース読み込み
    {
      scenegraph::resource::GfMotData animationResourceData;
      animationResourceData.ReadFile("R:/home/niji/git_program/gflib2/Animation/Sample/data/EnvTestMap/DrawEnvTest.gfmot");
      m_pAnimationResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&m_Allocator, &animationResourceData);
    }

    // モデル作成
    {
      poke_3d::model::BaseModel* pBaseModel = &m_BaseModel;

      // 作成
      pBaseModel->Create(gfx::GLMemory::GetAllocator(), heap::Manager::GetDefaultNewUseHeap(), m_pModelResourceRootNode);

      // 位置設定
      pBaseModel->SetPosition(0, 0, 0);

      // 回転設定
      pBaseModel->SetRotation(0, 0, 0);

      // 拡縮設定
      pBaseModel->SetScale(1, 1, 1);
    }


		return true;
	}

	virtual bool OnUpdate( void )
	{
    gfl2::system::InputDevice::Pad::Update();

    static math::Easing::EaseFunc easeFunc = math::Easing::LINEAR;

    if (m_Rno)
    {
      // パス制御
      static s32 maxFrame = 200;

      ++m_Counter;
      if (m_Counter >= maxFrame)
      {
        m_Rno = 0;
      }
      math::Vector3 pos = m_PathController.GetValue((f32)m_Counter / maxFrame);
      m_BaseCamera.SetPosition(pos);
    }
    else
    {
      // Aボタンでローカルアニメーション
      if (gfl2::system::InputDevice::Pad::IsPush(0, gfl2::system::InputDevice::Button::A))
      {
        if (m_BaseCamera.GetAnimationResourceNode() != NULL)
        {
          m_BaseCamera.ChangeAnimationLocalSmoothByResourceNode(NULL, 20, easeFunc);
        }
        else
        {
          m_BaseCamera.ChangeAnimationLocalSmoothByResourceNode(m_pAnimationResourceRootNode, 20, easeFunc);
          m_BaseCamera.SetAnimationLoop(true);
        }
      }
      // Bボタンでグローバルアニメーション
      else if (gfl2::system::InputDevice::Pad::IsPush(0, gfl2::system::InputDevice::Button::B))
      {
        m_BaseCamera.ChangeAnimationGlobalSmoothByResourceNode(m_pAnimationResourceRootNode, 20, easeFunc);
        m_BaseCamera.SetAnimationLoop(false);
      }
      // Yボタンでパス制御
      else if (gfl2::system::InputDevice::Pad::IsPush(0, gfl2::system::InputDevice::Button::Y))
      {
        math::Vector3 valueList[] =
        {
          math::Vector3(0.0f, 0.0f, 0.0f),
          math::Vector3(300.0f, 60.0f, 0.0f),
          math::Vector3(0.0f, 100.0f, 300.0f),
          math::Vector3(-300.0f, 60.0f, 0.0f),
        };

        u32 valueCount = sizeof(valueList) / sizeof(valueList[0]);

        // 現在位置からの相対アニメーションにしたい
        gfl2::math::Vector3 pos = m_BaseCamera.GetPosition();
        for (u32 i = 0; i < valueCount; ++i)
        {
          valueList[i] += pos;
        }

        m_PathController.Initialize(heap::Manager::GetDefaultNewUseHeap(), valueList, valueCount, true);

        m_Rno = 1;
        m_Counter = 0;
      }

      // 左スティックでカメラ位置移動
      gfl2::system::InputDevice::StickPos posLeft = gfl2::system::InputDevice::Pad::GetStick(0, gfl2::system::InputDevice::Stick::Left);
      gfl2::math::Vector3 pos = m_BaseCamera.GetPosition();
      pos.x += posLeft.x * 3;
      pos.z += posLeft.y * 3;
      m_BaseCamera.SetPosition(pos);

      // 右スティックでカメラ回転
      gfl2::system::InputDevice::StickPos posRight = gfl2::system::InputDevice::Pad::GetStick(0, gfl2::system::InputDevice::Stick::Right);
      gfl2::math::Quaternion rot = m_BaseCamera.GetRotationQuat();
      gfl2::math::Quaternion rotY(gfl2::math::Vector3(0, 1, 0), -posRight.x * 0.005f);
      m_BaseCamera.SetRotationQuat(rot * rotY);

      // カメラアニメーション更新
      m_BaseCamera.UpdateAnimation();
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

    m_pRenderingPipeLine->SetProjectionMatrix(m_BaseCamera.GetProjectionMatrix());
    m_pRenderingPipeLine->SetViewMatrix(m_BaseCamera.GetViewMatrix());

		m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
		m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );

		m_pRenderingPipeLine->Execute();

    GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
    m_BaseModel.Destroy();

    GFL_SAFE_DELETE(m_pModelResourceRootNode);
    GFL_SAFE_DELETE(m_pAnimationResourceRootNode);

    GFL_SAFE_DELETE(m_pDrawEnvNode);
    GFL_SAFE_DELETE(m_pRenderingPipeLine);

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
