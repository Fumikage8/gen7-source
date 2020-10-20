#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathUtil.h>
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
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_McnfData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_AABBTree.h>
#include <system/include/InputDevice/gfl2_Pad.h>
#include "MyRenderingPipeLine.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfCollisionMdlData.h>
#include <Collision/include/gfl2_CollisionModel.h>

#include <gfx/include/gfl2_DrawUtil.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::system;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;
using namespace gfl2::animation;

class GLAllocator : public IGLAllocator
{
public:
  GLAllocator():IGLAllocator(){}
  virtual ~GLAllocator(){}

  virtual void* SystemMemoryMalloc(u32 size){
    return malloc(size);
  }

  virtual void SystemMemoryFree(void* const address)
  {
    free( address );
  }

  virtual void* LocalMemoryMalloc(u32 size){
    return NULL;
  }

  virtual void LocalMemoryFree(void* const address)
  {

  }
};

class SimpleDraw : public Sample::Framework::IApplication, public collision::ICollisionModelCallbackable
{
public:
  math::Matrix																							m_Projection;   // 透視変換行列
  math::Matrix																							m_View;  // ビュー変換行列
  math::Matrix																							m_World;      // ワールド変換行列

  scenegraph::resource::ResourceNode*                       m_pModelResourceRootNodeField;
  scenegraph::resource::ResourceNode*                       m_pModelResourceRootNodeChara;
  scenegraph::resource::ResourceNode*                       m_pAnimationResourceRootNodeChara[2];
  scenegraph::resource::ResourceNode*                       m_pMotionConfigResourceRootNodeChara;
  scenegraph::resource::ResourceNode*                       m_pEnvResourceRootNode;
	AnimationController                       m_AnimationController;                ///< アニメーションコントローラー
  JointController                           m_JointController;                    ///< ジョイントコントローラー

  scenegraph::resource::ResourceNode*                       m_pCollisionResource;

  scenegraph::instance::ModelInstanceNode*                  m_pModelInstanceNodeField;
  scenegraph::instance::ModelInstanceNode*                  m_pModelInstanceNodeChara;
  MyRenderingPipeLine*                                      m_RenderingPipeLine;
  math::Vector																							m_ScrenaSize;
  scenegraph::instance::DrawEnvNode*                        m_pDrawEnvNode;
  scenegraph::instance::LightSetNode*                       m_pLightSetNode;
  scenegraph::instance::LightNode*                          m_pDirectionalLightNode;
  scenegraph::instance::LightNode*                          m_pAmbientLightNode;

  s32																												m_FrameCnt;
  u64																												m_PerformanceTime;

  InputDevice::StickPos																			m_Rot;
  math::Vector																							m_ModelRot;

  math::Vector                                              m_CharaPos;
  math::Vector                                              m_CameraPos;

  collision::CollisionModel                                 m_CollisionModel;
  math::Vector                                              m_TerrainNormal;
  const scenegraph::resource::CollisionTriangle*            m_pOldTerrain;
  bool                                                      m_foundTerrain;

  math::Vector                                              m_FootPosL;
  math::Vector                                              m_FootPosR;

  s32                                                       m_CurrentAnimationId;

  bool                                                      m_IKEnabled;

  SimpleDraw() : 
    Sample::Framework::IApplication(),
    m_FrameCnt(0),
    m_PerformanceTime( 0 ),
    m_pDrawEnvNode( NULL ),
    m_pOldTerrain(NULL)
  {
    m_pModelResourceRootNodeField = NULL;
    m_pModelResourceRootNodeChara = NULL;

    m_pModelInstanceNodeField = NULL;
    m_pModelInstanceNodeChara = NULL;

    m_Rot.x = m_Rot.y = 0.0f;
    m_CurrentAnimationId = 0;
    m_IKEnabled = true;

    m_ScrenaSize.Set( 1280, 720, 0 );
  }

  virtual ~SimpleDraw(){}

  virtual bool OnInit( void )
  {
    InputDevice::Pad::Initialize();

    GFGL::Initialize( &m_Allocator );

    m_Handle = GFGL::CreateDisplay( m_ScrenaSize.x, m_ScrenaSize.y, false, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

    DrawUtil::Initialize(&m_Allocator, &m_Handle);

    {//レンダリングエンジンの初期化
      //全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
      scenegraph::SceneGraphManager::InitDescription			desc(1024 * 2);
      desc.m_AABBTreeSize.SetMinMax( math::Vector(-500.0f, -500.0f, -500.0f), math::Vector(500.0f, 500.0f, 500.0f) );
      scenegraph::SceneGraphManager::Initialize();

      //リソースノードを生成するマネージャ初期化
      scenegraph::resource::ResourceManager::Initialize();

      //描画パイプラインを作成
      m_RenderingPipeLine = new MyRenderingPipeLine( m_ScrenaSize.x, m_ScrenaSize.y );
    }

    // コリジョンデータ
    {
      scenegraph::resource::GfCollisionMdlData resourceData;
      resourceData.ReadFile("../data/niji_test_map/takasa_test.gfmdl");

      m_pCollisionResource = scenegraph::resource::ResourceManager::CreateResourceNode(&resourceData);

      m_CollisionModel.Initialize(m_pCollisionResource);
    }

    // フィールド
    {
      static const char* s_pFilePath = "../data/niji_test_map/niji_test_map01.gfmdl";

      scenegraph::resource::GfMdlData resourceData;
      resourceData.ReadFile(s_pFilePath);

      m_pModelResourceRootNodeField = scenegraph::resource::ResourceManager::CreateResourceNode(&resourceData);
      m_pModelInstanceNodeField = scenegraph::instance::InstanceCreator::CreateModelInstanceNode(m_pModelResourceRootNodeField);
    }

    // キャラ
    {
      static const char* s_pFilePath = "../data/tr0001_00_hero/new/tr0001_00.gfmdl";

      scenegraph::resource::GfMdlData resourceData;
      resourceData.ReadFile(s_pFilePath);

      m_pModelResourceRootNodeChara = scenegraph::resource::ResourceManager::CreateResourceNode(&resourceData);
      m_pModelInstanceNodeChara = scenegraph::instance::InstanceCreator::CreateModelInstanceNode(m_pModelResourceRootNodeChara);

      m_CharaPos.Set(140, 0, -372);
      m_TerrainNormal.Set(0, 1, 0);

      m_CharaPos.Set(-1611.93591, 251.435074, -0.430891275);
      m_Rot.x = -40.23;
      m_Rot.y = -2.0;

    }

    // キャラアニメ
    {
      static const char* s_pFilePath[] = 
      {
        "../data/tr0001_00_hero/new/tr0001_00_fi01_wait01.gfmot",
        "../data/tr0001_00_hero/new/tr0001_00_fi20_walk01.gfmot",
      };

      for (u32 i = 0; i < 2; ++i)
      {
        scenegraph::resource::GfMotData resourceData;
        resourceData.ReadFile(s_pFilePath[i]);

        m_pAnimationResourceRootNodeChara[i] = scenegraph::resource::ResourceManager::CreateResourceNode(&resourceData);
      }

      AnimationController::InitDescription desc;
      desc.jointSlotCount = 1;
      desc.useJointInterp = true;
      m_AnimationController.Initialize(m_pModelInstanceNodeChara, desc);

      // ジョイントアニメーション
      JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
      pSlot->SetAnimationResource(m_pAnimationResourceRootNodeChara[0]);
      pSlot->SetLoop(true);
      pSlot->SetStepFrame(0.1f);
    }

    {
      static const char* s_pFilePath = "../data/tr0001_00_hero/new/tr0001_00.mcnf";

      // モーション設定
      scenegraph::resource::McnfData resourceData;
      resourceData.ReadFile(s_pFilePath);

      m_pMotionConfigResourceRootNodeChara = scenegraph::resource::ResourceManager::CreateResourceNode(&resourceData);

      // ジョイントコントローラー
      m_JointController.Initialize(m_pModelInstanceNodeChara, m_pMotionConfigResourceRootNodeChara);

      // IKのコールバックを登録
      m_JointController.GetIKController()->SetCallback(IKCallback, this);
    }

    // シーン環境
    {
      scenegraph::resource::GfEnvData							resourceData;
      resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/tr0001_00_hero/new/tr0001_00.gfenv" );

      m_pEnvResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
      m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNodeInstanceNode( m_pEnvResourceRootNode );
    }

    m_RenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );

    {
      // 射影変換行列(パースペクティブ(透視法)射影)
      m_Projection = math::Matrix::GetPerspective(
        math::Util::ConvDegToRad(37.0f),		// 視野角30°
        m_ScrenaSize.x / m_ScrenaSize.y,	// アスペクト比
        0.1f,							// 前方投影面までの距離
        10000.0f);							// 後方投影面までの距離

      m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
    }

    {
      // ビュー変換行列
      math::Vector eyePosition( 0.0f, 50.0f, 80.0f, 1.0f );  // 視点(カメラの位置)
      math::Vector focusPosition( 0.0f, 50.0f,  0.0f, 1.0f );  // 注視点
      math::Vector upDirection( 0.0f, 1.0f,  0.0f, 1.0f );  // カメラの上方向

      m_View = math::Matrix::GetLookAt(eyePosition, focusPosition, upDirection);
    }

    return true;
  }

  virtual bool OnUpdate( void )
  {
    InputDevice::Pad::Update();

    // モーション切り替え
    {
      if (InputDevice::Pad::IsPush(0, InputDevice::Button::A))
      {
        m_CurrentAnimationId ^= 1;

        JointAnimationSlot* pSlot = m_AnimationController.GetJointInterpSlot();
        pSlot->SetAnimationResource(m_pAnimationResourceRootNodeChara[m_CurrentAnimationId]);
        pSlot->SetLoop(true);
        pSlot->SetStepFrame(0.1f);

        m_AnimationController.InterpRequest(20);
      }
    }

    // キャラの移動
    {
      const InputDevice::StickPos& pos = InputDevice::Pad::GetStick( 0, InputDevice::Stick::Left );

      static const f32 MoveSpeed = 3.0f;

      math::Vector moveVec(pos.x, 0, pos.y);
      math::Matrix rotYMat = math::Matrix::GetRotationY( -m_Rot.x / 20.0f );

      m_CharaPos += rotYMat.TransformCoord(moveVec) * MoveSpeed;

      // 地形の高さ取得
      m_foundTerrain = false;
      m_CollisionModel.GetHeight(m_CharaPos, m_pOldTerrain, this);

      // 見つからなかったら前回の地形の際近接点にしてみる
      if (!m_foundTerrain)
      {
        if (m_pOldTerrain != NULL)
        {
          m_CharaPos = m_pOldTerrain->GetClosestPoint(m_CharaPos);
        }
      }

      static const f32 RotSpeed = 0.05f;
      static f32 rot = 1.64f;
      rot += InputDevice::Pad::IsPress(0, InputDevice::Button::L1) * RotSpeed;
      rot -= InputDevice::Pad::IsPress(0, InputDevice::Button::R1) * RotSpeed;

      math::SRT* pSRT = m_pModelInstanceNodeChara->GetLocalSRT();
      pSRT->translate = m_CharaPos;
      pSRT->rotation.RadianYToQuaternion(rot);
    }



    // カメラの移動
    {
      math::Vector ofs(0.0f, 400.0f, 600.0f);

      const InputDevice::StickPos& rot = InputDevice::Pad::GetStick(0, InputDevice::Stick::Right);

      static const f32 RotSpeed = 0.5f;

      m_Rot.x += rot.x * RotSpeed;
      m_Rot.y += rot.y * RotSpeed;

      math::Matrix rotYMat = math::Matrix::GetRotationY( -m_Rot.x / 20.0f );
      math::Matrix rotXMat = math::Matrix::GetRotationX( -m_Rot.y / 20.0f );
      math::Matrix rotMat = rotYMat * rotXMat;

      m_CameraPos = m_CharaPos + rotMat.TransformCoord(ofs);

      math::Matrix viewMatrix = math::Matrix::GetLookAt(m_CameraPos, m_CharaPos + math::Vector(0, 100, 0), math::Vector(0, 1, 0));
      m_RenderingPipeLine->SetViewMatrix(viewMatrix);
    }

    // アニメーション
    m_AnimationController.Update();

    // ワールド行列更新
    scenegraph::SceneGraphManager::TraverseModel();

    // ジョイント
    if (InputDevice::Pad::IsPush(0, InputDevice::Button::B))
    {
      m_IKEnabled ^= 1;
    }
    if (m_IKEnabled)
    {
      m_JointController.Update();
    }

    ++m_FrameCnt;

    return true;
  }

  virtual bool OnDraw( void )
  {
    m_RenderingPipeLine->SetPos(m_CharaPos, m_FootPosL, m_FootPosR);

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
    if (m_pDrawEnvNode != NULL) delete m_pDrawEnvNode;
    if (m_pEnvResourceRootNode != NULL) delete m_pEnvResourceRootNode;

    if (m_pMotionConfigResourceRootNodeChara != NULL) delete m_pMotionConfigResourceRootNodeChara;

    if (m_pModelInstanceNodeChara != NULL) delete m_pModelInstanceNodeChara;
    if (m_pModelResourceRootNodeChara != NULL) delete m_pModelResourceRootNodeChara;

    if (m_pModelInstanceNodeField != NULL) delete m_pModelInstanceNodeField;
    if (m_pModelResourceRootNodeField != NULL) delete m_pModelResourceRootNodeField;

    if (m_RenderingPipeLine != NULL) delete m_RenderingPipeLine;

    scenegraph::resource::ResourceManager::Terminate();
    scenegraph::SceneGraphManager::Terminate();

    DrawUtil::Terminate();

    GFGL::Finish();

    GFGL::SetActiveDisplay(NULL);
    GFGL::DestroyDisplay(m_Handle);
    GFGL::Terminate();

    InputDevice::Pad::Terminate();

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

  bool CollisionModelHeightCallback(const renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height)
  {
    static const f32 HeightLimit = 32.0f;
    static const f32 HeightLimitSq = HeightLimit * HeightLimit;

    // 高さの差が一定値以下なら採用する
    f32 sub = m_CharaPos.y - height;
    if (sub * sub < HeightLimitSq)
    {
      // 見つかった
      m_foundTerrain = true;

      // 地形の高さに
      m_CharaPos.y = height;

      // 地形法線保存
      m_TerrainNormal = pTriangle->m_Normal;
      m_pOldTerrain = pTriangle;

      // デバッグ描画用
      m_RenderingPipeLine->SetCurrentTerrain(pTriangle);

      // 衝突検出終了
      return false;
    }

    // 衝突検出続行
    return true;
  }

  static bool IKCallback(math::Vector* out_pPosition, math::Vector* out_pNormal, const gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode, void* pUserData)
  {

    SimpleDraw* pInstance = reinterpret_cast<SimpleDraw*>(pUserData);

    *out_pPosition = pJointNode->GetWorldMatrix().GetElemPosition();

    //out_pPosition->y = pInstance->m_ikPosY;

    class CallbackObject : public collision::ICollisionModelCallbackable
    {
    public:
        bool CollisionModelHeightCallback(const renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height)
        {
          static const f32 HeightLimit = 100.0f;
          static const f32 HeightLimitSq = HeightLimit * HeightLimit;

          *m_pNormal = pTriangle->m_Normal;

          height += m_Offset;

          // 地面にめり込んでいるか？
          if (m_pPosition->y > height)
          {
            // 衝突検出続行
            return true;
          }

          // 高さの差が一定値以下なら採用する
          f32 sub = m_pPosition->y - height;
          if (sub * sub > HeightLimitSq)
          {
            // 衝突検出続行
            return true;
          }

          // 地形の高さに
          m_pPosition->y = height;
          m_Found = true;

          // 衝突検出終了
          return false;
        }

    public:
      math::Vector* m_pPosition;
      math::Vector* m_pNormal;
      bool m_Found;
      f32 m_Offset;
    };
    CallbackObject callbackObject;
    callbackObject.m_pPosition = out_pPosition;
    callbackObject.m_pNormal = out_pNormal;
    callbackObject.m_Found = false;

    const c8* pNodeName = pJointNode->GetNodeName();
    if ((strcmp(pNodeName, "LToe") == 0) || (strcmp(pNodeName, "RToe") == 0))
    {
      callbackObject.m_Offset = 5.6f;
    }
    else
    {
      callbackObject.m_Offset = 12.0f;
    }

    pInstance->m_CollisionModel.GetHeight(*out_pPosition, pInstance->m_pOldTerrain, &callbackObject);

    if (strcmp(pNodeName, "LToe") == 0)
    {
      pInstance->m_FootPosL = *out_pPosition;
    }
    else
    {
      pInstance->m_FootPosR = *out_pPosition;
    }

    return callbackObject.m_Found;
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
