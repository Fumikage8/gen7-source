﻿/**
* @brief  フィールドテスト環境構築用プロセス
* @file   FieldTestProc.cpp
* @author ikuta_junya
* @data   2015.02.07
*/

// gfl2
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_heap_manager.h>
#include <System/include/HeapDefine.h>

// niji
#include "Test/FieldTest/include/FieldTestMyRenderingPipeLine.h" // @todo field用に置き換える 150215 ikuta_junya
#include "System/include/GflUse.h"              // for DisplayDeviceHandle @todo GflUse.h参照箇所を増やしたくないので、別ファイル化したい 150215 ikuta_junya 
#include "Debug/DebugWin/include/DebugWinSystem.h"

// FieldTest
#include "test/FieldTest/include/FieldTestProc.h"
#include "test/FieldTest/include/FieldSystem.h"
#include "test/FieldTest/include/Camera/FieldCamera.h"
#include "test/FieldTest/include/Camera/FieldCameraUtility.h"
#include "test/FieldTest/include/Terrain/FieldTerrain.h"
#include "test/FieldTest/include/field_transform.h"
#include "test/FieldTest/include/chara_rsc_manager.h"

// FieldTest ( Debug )
#include "test/FieldTest/include/Debug/FieldDebugMenu.h"
#include "test/FieldTest/include/Debug/FieldDebugTypes.h"
#include "test/FieldTest/include/Debug/FieldDebugFrustumDraw.h"
#include "test/FieldTest/include/Debug/FieldDebugBoxDraw.h"
#include "test/FieldTest/include/Debug/FieldDebugInstanceMemoryCollection.h"
#include "test/FieldTest/include/Debug/FieldDebugModelInstanceInfo.h"

#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "../include/systemCollision.h"
#include "../include/movemodel_manager.h"
#include "../include/chara_define.h"
#include "../include/FieldActionCmdManager.h"
#include "GameSys/include/GameManager.h"

#include "test/FieldTest/include/TestEncountGrass.h"
#include "test/FieldTest/include/TestShadow.h"

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// 天球テスト
#include "test/FieldTest/include/Skybox/Skybox.h"
#include "test/FieldTest/include/Skybox/SkyboxModel.h"
#include "test/FieldTest/include/Skybox/Sky.h"
#include "test/FieldTest/include/Skybox/Cloud.h"

//フィールドスクリプト
#include "FieldScript/include/FieldScriptSystem.h"

// Effect
#include "test/FieldTest/include/FieldEffectManager.h"
#include "test/FieldTest/include/FieldEffectFootManager.h"

#include "Sound/include/Sound.h"

// 天候
#include "test/FieldTest/include/Weather/Weather.h"
#include "test/FieldTest/include/Weather/WeatherEffectManager.h"

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// 新コリジョンシステム
#include "../include/Collision/BaseActor.h"
#include "../include/Collision/StaticActor.h"
#include "../include/Collision/DynamicActor.h"
#include "../include/Collision/CollisionScene2D.h"
#include "../include/Collision/CollisionScene3D.h"

// RTTIデバッグシステム
#include <system/include/InputDevice/gfl2_Pad.h>
#include "../include/RttiDebug/RttiEditor.h"
#include "../include/RttiDebug/RttiManager.h"

#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN( Test );
GFL_NAMESPACE_BEGIN( FieldTest );

#define TEST_GRAPHIC                          // これをコメントアウトすると、池内さんが作成した仮地形になります 150223 ikuta_junya
#define TEST_SKYBOX                           // これをコメントアウトすると、天球モデルが作成されません 150226 saita_kazuki
#define CHECK_MEMORY_LEAK_IN_FIELD_PROC 1     // スタートボタンを押してフィールドプロックを抜ける際にリークチェックが行われる
#define TEST_ENCGRASS                         // これをコメントアウトすると、エンカウント草が作成されません 150313 tomoya
#define TEST_SHADOW                           // これをコメントアウトすると、影が作成されません 150313 tomoya
#define TEST_EFFECT                           // これをコメントアウトすると、エフェクトシステムが作成されません。 150316 saita_kazuki
#define TEST_WEATHER                          // これをコメントアウトすると、天候が作成されません。TEST_EFFECTもONにする必要があります 150316 saita_kazuki
#define TEST_FOOT_SPRAY                       // これをコメントアウトすると、足元エフェクトが作成されません。TEST_EFFECTもONにする必要があります 150316 saita_kazuki

#if PM_DEBUG
void TickDump( void )
{
#if FIELD_DEBUG_TICK_ENABLE
  GFL_RELEASE_PRINT("-- FieldTick Dump --\n");
  for( u32 i = 0; i < Field::debug::FieldDebugTypes::TickDefine::MAX ; i++ )
  {
    GFL_RELEASE_PRINT("%s Ave[%6d]\n",
      Field::debug::FieldDebugTypes::GetTickLabel( static_cast<Field::debug::FieldDebugTypes::TickDefine::Index>( i )),
      FIELD_DEBUG_TICK_US_AVE_GET(i) 
      );

  }
#endif // #if FIELD_DEBUG_TICK_ENABLE

#if GAME_MANAGER_DEBUG_TICK_ENABLE
  GFL_RELEASE_PRINT("-- GameManagerTick Dump --\n");
  for( u32 i = 0; i < GameSys::GameManager::TickDefine::MAX ; i++ )
  {
    GFL_RELEASE_PRINT("%s Ave[%6d]\n",
      GameSys::GameManager::GetTickLabel( static_cast<GameSys::GameManager::TickDefine::Index>( i )),
      GAME_MANAGER_DEBUG_TICK_US_AVE_GET(i) 
      );
  }
#endif // #if GAME_MANAGER_DEBUG_TICK_ENABLE

#if USE_DEBUG_WIN
  Debug::DebugWin::DebugWinWork *pWork = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork );
  if( pWork )
  {
    const int update = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE );
    const int upnext = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT );
    const int draw   = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW );
    const int gpu    = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU );

    GFL_RELEASE_PRINT("-- Tick Dump --\n");
    GFL_RELEASE_PRINT("Update Ave[%6d]\n", update );
    GFL_RELEASE_PRINT("UpNext Ave[%6d]\n", upnext );
    GFL_RELEASE_PRINT("Draw Ave[%6d]\n", draw );
    GFL_RELEASE_PRINT("GPU Ave[%6d]\n", gpu );
  }
#endif // #if USE_DEBUG_WIN
}
#endif //#if PM_DEBUG

class TransformNodeVisitorFastTest
{
public:
  TransformNodeVisitorFastTest() : m_UpdateCount( 0 ), m_UpdateSkipCount( 0 ) {}
  virtual ~TransformNodeVisitorFastTest() {}

  void Visit( gfl2::renderingengine::scenegraph::instance::TransformNode* pNode )
  {
    gfl2::renderingengine::scenegraph::instance::TransformNode* pParent = (gfl2::renderingengine::scenegraph::instance::TransformNode*)pNode->GetParent();
    const gfl2::math::SRT& localSRT = pNode->GetLocalSRT();

    gfl2::math::Matrix34 local;

    // 親が居ない場合
    if (pParent == NULL)
    {
      // @todo
      if( pParent->IsWorldTransformDirty() )
      {
        localSRT.ToMatrix(local);
        pNode->SetWorldMatrix(local);

        m_UpdateCount++;
      }
      else
      {
        m_UpdateSkipCount++;
      }
      return;
    }

    // 行列更新

    const gfl2::math::Vector3& parentScale = pParent->GetLocalScale();
    if (pNode->IsNonInheritanceScale())
    {
      // 親のスケールを伝播させない
      if ((parentScale.x == 1.0f) && (parentScale.y == 1.0f) && (parentScale.z == 1.0f))
      {
        // 親のスケールが全て１なので通常ので大丈夫
        localSRT.ToMatrix(local);
      }
      else
      {
        localSRT.ToMatrixNonInheritanceScale(local, parentScale);
      }
    }
    else
    {
      // 親のスケールを伝播させる
      localSRT.ToMatrix(local);
    }

    m_UpdateCount++;
    pNode->SetWorldMatrix(pParent->GetWorldMatrix() * local);
  }

  u32 m_UpdateCount;
  u32 m_UpdateSkipCount;
};

class TransformNodeVisitorFastTest2
{
public:
  TransformNodeVisitorFastTest2() : m_UpdateCount( 0 ), m_UpdateSkipCount( 0 ) {}
  virtual ~TransformNodeVisitorFastTest2() {}

  void Visit( gfl2::renderingengine::scenegraph::instance::TransformNode* pNode ) 
  {
    gfl2::renderingengine::scenegraph::instance::TransformNode* pParent = (gfl2::renderingengine::scenegraph::instance::TransformNode*)pNode->GetParent();
    const gfl2::math::SRT& localSRT = pNode->GetLocalSRT();

    gfl2::math::Matrix34 local;

    // 親が居ない場合
    if (pParent == NULL)
    {
      if( pNode->IsLocalTransformDirty() )
      {
        localSRT.ToMatrix(local);
        pNode->SetWorldMatrix(local);
        pNode->SetLocalTransformDirty( false );
        m_UpdateCount++;
      }
      else
      {
        pNode->SetLocalTransformDirty( false );
        pNode->SetWorldTransformDirty( false );
        m_UpdateSkipCount++;
      }
      return;
    }

    // 行列更新
    if( pParent->IsWorldTransformDirty() || pNode->IsLocalTransformDirty() )
    {
      const gfl2::math::Vector3& parentScale = pParent->GetLocalScale();
      if (pNode->IsNonInheritanceScale())
      {
        // 親のスケールを伝播させない
        if ((parentScale.x == 1.0f) && (parentScale.y == 1.0f) && (parentScale.z == 1.0f))
        {
          // 親のスケールが全て１なので通常ので大丈夫
          localSRT.ToMatrix(local);
        }
        else
        {
          localSRT.ToMatrixNonInheritanceScale(local, parentScale);
        }
      }
      else
      {
        // 親のスケールを伝播させる
        localSRT.ToMatrix(local);
      }
    
      pNode->SetLocalTransformDirty( false );
      pNode->SetWorldMatrix(pParent->GetWorldMatrix() * local);
      m_UpdateCount++;
    }
    else
    {
      pNode->SetLocalTransformDirty( false );
      pNode->SetWorldTransformDirty( false );
      m_UpdateSkipCount++;
    }
  }

  u32 m_UpdateCount;
  u32 m_UpdateSkipCount;

};


void UpdateModelFast( void )
{
  // @todo 危険なアクセス
  gfl2::renderingengine::scenegraph::DagNode* pRootNode = gfl2::renderingengine::scenegraph::SceneGraphManager::GetRootNode();
  gfl2::renderingengine::scenegraph::instance::TransformNode* pTransformRootNode = ( gfl2::renderingengine::scenegraph::instance::TransformNode* ) pRootNode->GetChild( 2 );
  gfl2::renderingengine::scenegraph::DagNode* pChild = pTransformRootNode->GetChild();

  pTransformRootNode->SetLocalTransformDirty( false );
  pTransformRootNode->SetWorldTransformDirty( false );

  if( pChild == NULL )
  {
    return;
  }

  u32 taverseMode = 1;
#if PM_DEBUG
  taverseMode = Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::MODEL_TRAVERSE_MODE ];
  Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeTraverseCount = 0;
  Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeWorldMatrixUpdateCount = 0;
  Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeWorldMatrixUpdateSkipCount = 0;
#endif

  // 0:行列全更新 1:ダーティチェックを行って更新
  switch( taverseMode )
  {
  case 0:
    {
      gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::instance::TransformNode, TransformNodeVisitorFastTest> visitor;
      gfl2::renderingengine::scenegraph::Traverser::TraverseSiblingFast<gfl2::renderingengine::scenegraph::instance::TransformNode, TransformNodeVisitorFastTest>( pChild, &visitor, gfl2::renderingengine::scenegraph::Traverser::DEFAULT_DEPTH-1 );

#if PM_DEBUG
      Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeTraverseCount = visitor.m_UpdateCount + visitor.m_UpdateSkipCount;
      Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeWorldMatrixUpdateCount = visitor.m_UpdateCount;
      Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeWorldMatrixUpdateSkipCount = visitor.m_UpdateSkipCount;
#endif // #if PM_DEBUG
      break;
    }
  case 1:
    {
      gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::instance::TransformNode, TransformNodeVisitorFastTest2> visitor;
      gfl2::renderingengine::scenegraph::Traverser::TraverseSiblingFast<gfl2::renderingengine::scenegraph::instance::TransformNode, TransformNodeVisitorFastTest2>( pChild, &visitor, gfl2::renderingengine::scenegraph::Traverser::DEFAULT_DEPTH-1 );

#if PM_DEBUG
      Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeTraverseCount = visitor.m_UpdateCount + visitor.m_UpdateSkipCount;
      Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeWorldMatrixUpdateCount = visitor.m_UpdateCount;
      Field::debug::FieldDebugTypes::s_DrawInfo.transformNodeWorldMatrixUpdateSkipCount = visitor.m_UpdateSkipCount;
#endif // #if PM_DEBUG
      break;
    }
  }
}

void UpdateModelFastAfter( void )
{

}

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
FieldTestProc::FieldTestProc( void ) : 
  m_pDefaultCameraData( NULL ), m_pFieldCamera( NULL ), m_pFieldSystem( NULL ),
  m_pTestEncountGrass(NULL),
  m_pTestShadow(NULL)
#if PM_DEBUG
  , m_pFieldDebugMenu( NULL )
  , m_pDebugFrustumDraw(NULL)
  , m_pDebugBoxDraw(NULL)
#endif
{

}

/**
* @brief デストラクタ
*
* @return 無し
*/
FieldTestProc::~FieldTestProc( void )
{

}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
gfl2::proc::Result FieldTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
  FIELD_DEBUG_TICK_SET_FLAG( true );

  GFL_PRINT("FieldTestProc::InitFunc\n");

#if defined(GF_PLATFORM_CTR)
  // 立体視設定
  gfl2::gfx::GFGL::SetStereoModeEnable( false );
#endif

  //gfl2::gfx::GLMemory::SetAllocator( RscManager::getInstance()->GetAllocator() );

  // 常駐システム用heap取得
  gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
  gfl2::heap::HeapBase *deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );

#if CHECK_MEMORY_LEAK_IN_FIELD_PROC
  // フィールドプロック内でのヒープメモリリークを検知しやすいように専用の物を作成
  m_pFieldHeap = GFL_CREATE_HEAP( systemHeap, HEAPID_FIELD , System::HeapSize::HEAP_FIELD_SIZE , gfl2::heap::HEAP_TYPE_EXP, false );
  m_pfieldHeapAllocator = GFL_NEW(m_pFieldHeap) System::nijiAllocator( m_pFieldHeap, System::nijiAllocator::APPLICATION );
  m_pFieldDeviceHeap = GFL_CREATE_HEAP( deviceHeap, HEAPID_FIELD_DEVICE, System::HeapSize::HEAP_FIELD_DEVICE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  m_pfieldDeviceHeapAllocator = GFL_NEW(m_pFieldHeap) System::nijiAllocator( m_pFieldDeviceHeap, System::nijiAllocator::APPLICATION );

  // フィールドプロック内でのVRAMリークを検知しやすいように専用の物を作成
  m_pFieldGfglHeapOld = gfl2::gfx::GFGL::GetAllocator();
  gfl2::gfx::GFGL::SetAllocator( m_pfieldDeviceHeapAllocator );
#else
  m_pFieldHeap = systemHeap;
  m_pfieldHeapAllocator = GFL_NEW(m_pFieldHeap) System::nijiAllocator( m_pFieldHeap, System::nijiAllocator::APPLICATION, System::nijiAllocator::VRAM_MALLOC_AB );
  m_pFieldDeviceHeap = deviceHeap;
  m_pfieldDeviceHeapAllocator = GFL_NEW(m_pFieldHeap) System::nijiAllocator( m_pFieldDeviceHeap, System::nijiAllocator::APPLICATION, System::nijiAllocator::VRAM_MALLOC_AB );
  m_pFieldGfglHeapOld = gfl2::gfx::GFGL::GetAllocator();
#endif

  // ヒープ使用量収集処理の初期化
  FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_Initialize();

  // 通常newが使用するヒープを指定 @note 通常new使用箇所が多く、HEAPID_SYSYTEMが空になってしまうので暫定対応
  gfl2::heap::Manager::SetDefaultNewUseHeap( m_pFieldDeviceHeap );

  // リソースマネージャーの初期化
  RscManager::getInstance()->GetAllocator()->init( m_pFieldDeviceHeap );

  // 描画パイプラインを作成

  int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( "MyRenderingPipeLine" );
  FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
  m_pRenderingPipeLine = GFL_NEW( m_pFieldHeap ) Test::FieldTest::MyRenderingPipeLine( 512, 256 );
  m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT] ));
  m_pRenderingPipeLine->SetDepthTexture( m_pDepthStencilTexture );
  FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

  // アクションコマンドワークの初期化
  field::mmodel::ActionCmdManager::Initialize( m_pFieldHeap );

  m_moveModelManager = GFL_NEW(m_pFieldHeap) fld::mmodel::MoveModelManager(m_pFieldHeap);
  m_moveModelManager->Initialize();

  // キャラクターマネージャーにヒープ、ファイルリードマネージャーを登録
  RscManager::getInstance()->Setup( m_pFieldDeviceHeap, pGameManager->GetAsyncFileManager() );

#ifdef TEST_ENCGRASS
  {
    //int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( "EncGrass" );
    //FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection( _index );
    // テストのエンカウント草クラス
    m_pTestEncountGrass = GFL_NEW( m_pFieldHeap ) TestEncountGrass();
    m_pTestEncountGrass->Initialize( m_pFieldDeviceHeap, pGameManager->GetAsyncFileManager(), m_pRenderingPipeLine );
    //FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection( _index );
  }
#endif // TEST_ENCGRASS

#ifdef TEST_SHADOW
  m_pTestShadow = GFL_NEW( m_pFieldHeap ) TestShadow();
  m_pTestShadow->Initialize( m_pFieldDeviceHeap, pGameManager->GetAsyncFileManager(), m_pRenderingPipeLine );
#endif // TEST_SHADOW

#if USE_SKYBOX_BUFFER_CLEAR
  // gamemanagerのバッファクリアを無くす
  GameSys::GameManager::BUFFER_CLEAR_SETTING bufferClearSetting[2];
  pGameManager->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT , &bufferClearSetting[0] );
  pGameManager->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::RIGHT, &bufferClearSetting[1] );
  bufferClearSetting[0].clearTarget &= ~(GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL);
  bufferClearSetting[1].clearTarget &= ~(GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL);
  pGameManager->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT , bufferClearSetting[0] );
  pGameManager->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::RIGHT, bufferClearSetting[1] );
#endif

  // ライティング用に環境ノードを追加
  {
    m_pDrawEnvNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode();
    // ライト0
    {
      gfl2::renderingengine::scenegraph::instance::LightSetNode		*m_pLightSetNode;
      gfl2::renderingengine::scenegraph::instance::LightNode			*m_pDirectionalLightNode;
      gfl2::renderingengine::scenegraph::instance::LightNode			*m_pAmbientLightNode;

      m_pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode();
      m_pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode();
      m_pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode();

      {
        m_pDirectionalLightNode->SetNodeName( "DirectionalLight01" );
        gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
        m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
        m_pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
        gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 10000000.0f);
        m_pDirectionalLightNode->SetPosition( position );
        gfl2::math::Vector3 direction = -position.Normalize();
        m_pDirectionalLightNode->SetDirection( direction );
        m_pDirectionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) );
      }

      {
        m_pAmbientLightNode->SetNodeName( "AmbientLight01" );
        gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
        m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
        m_pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
        m_pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
      }

      m_pLightSetNode->SetSetNo( 0 );
      m_pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
      m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
      m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる
    }

    // ライト2
    {
      gfl2::renderingengine::scenegraph::instance::LightSetNode		*m_pLightSetNode;
      gfl2::renderingengine::scenegraph::instance::LightNode			*m_pDirectionalLightNode;
      gfl2::renderingengine::scenegraph::instance::LightNode			*m_pAmbientLightNode;

      m_pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode();
      m_pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode();
      m_pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode();

      {
        m_pDirectionalLightNode->SetNodeName( "DirectionalLight02" );
        gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
        m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
        m_pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
        gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 10000000.0f);
        m_pDirectionalLightNode->SetPosition( position );
        gfl2::math::Vector3 direction = -position.Normalize();
        m_pDirectionalLightNode->SetDirection( direction );
        m_pDirectionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) );
      }

      {
        m_pAmbientLightNode->SetNodeName( "AmbientLight02" );
        gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
        m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
        m_pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
        m_pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
      }

      m_pLightSetNode->SetSetNo( 2 );
      m_pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
      m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
      m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる
    }


    gfl2::math::AABB				envSpace( gfl2::math::Vector(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector(500000.0f, 500000.0f, 500000.0f) );
    m_pDrawEnvNode->SetAABB( envSpace );
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pDrawEnvNode );

    m_pRenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );
  }

  // 地形初期化
#ifdef TEST_GRAPHIC
  m_pFieldTerrain = GFL_NEW( m_pFieldHeap ) Field::Terrain::FieldTerrain();
  m_pFieldTerrain->Initialize( m_pFieldHeap, m_pFieldDeviceHeap, pGameManager->GetAsyncFileManager(), m_pRenderingPipeLine );
#endif

  // プレイヤーの配置
  fld::mmodel::MoveModelHeader pika;
  pika.mmdl_id		= fld::mmodel::MMDL_PLAYER;
  pika.move_code	= fld::mmodel::MOVE_CODE_PLAYER;
  pika.rsc_id		= chara::RSCID_TEST_KISEKAE;
  fld::mmodel::MoveModel* pModelPlayer = m_moveModelManager->CreateMoveModel( &pika );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelPlayer->getModelInstanceNode() );

#if 0
  // プレイヤーにアタッチする小物
  fld::mmodel::MoveModelHeader parts;
  parts.mmdl_id		= fld::mmodel::MMDL_ATTACH_0;
  parts.move_code	= fld::mmodel::MOVE_CODE_ATTACH_PLAYER;
  parts.rsc_id		= chara::RSCID_ATTACH_OBJ_TSURIZAO;
  fld::mmodel::MoveModel* pModelAttach = m_moveModelManager->CreateMoveModel( &parts );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelAttach->getModelInstanceNode() );
#endif
  if( pModelPlayer )
  {
    FieldTransform* pTransform = pModelPlayer->GetTransform();
    pTransform->SetTranslation( gfl2::math::Vector( 0.0f, 350.0f, 0.0f ) );
  }

#if 0
  // NPCの配置
  fld::mmodel::MoveModelHeader npc;
  npc.mmdl_id		= fld::mmodel::MMDL_NPC_0;
  npc.move_code	= fld::mmodel::MOVE_CODE_NPC;
  npc.rsc_id		= chara::RSCID_HERO;
  fld::mmodel::MoveModel* pModelNpc = m_moveModelManager->CreateMoveModel( &npc );
  pModelNpc->GetTransform()->SetTranslation( gfl2::math::Vector( -2109.0f, -50.0f , 517.0f ) );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelNpc->getModelInstanceNode() );
  pModelNpc->setInterest(true);
#endif 

  // 検証用NPC
  {
    struct NPC_SET
    {
      fld::mmodel::MMDL_ID mmdlId;
      chara::RSC_ID rscId;
      gfl2::math::Vector pos;
    };

    NPC_SET workTalble[] = 
    {
      fld::mmodel::MMDL_TEST_KENTAUROS, chara::RSCID_TEST_KENTAUROS, gfl2::math::Vector( 365.0f, 350.0f, 1450.0f ),
      fld::mmodel::MMDL_TEST_NPC_1500, chara::RSCID_TEST_NPC_1500, gfl2::math::Vector( -50.0f, 350.0f, 0.0f ),
      fld::mmodel::MMDL_TEST_NPC_2000, chara::RSCID_TEST_NPC_2000, gfl2::math::Vector( 0.0f, 350.0f, 0.0f ),
      fld::mmodel::MMDL_TEST_NPC_1500_02, chara::RSCID_TEST_NPC_1500, gfl2::math::Vector( 50.0f, 350.0f, 0.0f ),
      fld::mmodel::MMDL_TEST_NPC_2000_02, chara::RSCID_TEST_NPC_2000, gfl2::math::Vector( 100.0f, 350.0f, 0.0f ),
      fld::mmodel::MMDL_TEST_NPC_1500_03, chara::RSCID_TEST_NPC_1500, gfl2::math::Vector( 460.0f, 350.0f, 1165.0f ),
      fld::mmodel::MMDL_TEST_NPC_2000_03, chara::RSCID_TEST_NPC_2000, gfl2::math::Vector( 510.0f, 350.0f, 1165.0f ),
      fld::mmodel::MMDL_TEST_NPC_1500_04, chara::RSCID_TEST_NPC_1500, gfl2::math::Vector( 560.0f, 350.0f, 1165.0f ),
      fld::mmodel::MMDL_TEST_NPC_2000_04, chara::RSCID_TEST_NPC_2000, gfl2::math::Vector( 610.0f, 350.0f, 1165.0f ),
    };
#if 0
    u32 workTable[][2] = 
    {
#if 0
      {
        fld::mmodel::MMDL_EDGE_TEST_00,  chara::RSCID_EDGE_TEST_00
      },
      {
        fld::mmodel::MMDL_EDGE_TEST_01,  chara::RSCID_EDGE_TEST_01
        },
        {
          fld::mmodel::MMDL_MAT_TEST_00,  chara::RSCID_MAT_TEST_00
        },
        {
          fld::mmodel::MMDL_MAT_TEST_01,  chara::RSCID_MAT_TEST_01
          },
#endif
#if 0
          {
            fld::mmodel::MMDL_TEST_KISEKAE, chara::RSCID_TEST_RIM   // 着せ替え検証用
          },
#endif
            {
              fld::mmodel::MMDL_TEST_KENTAUROS, chara::RSCID_TEST_KENTAUROS // 着せ替え検証用
            },
            {
              fld::mmodel::MMDL_TEST_NPC_1500, chara::RSCID_TEST_NPC_1500   // 着せ替え検証用
              },
              {
                fld::mmodel::MMDL_TEST_NPC_2000, chara::RSCID_TEST_NPC_2000   // 着せ替え検証用
              },
            {
              fld::mmodel::MMDL_TEST_NPC_1500_02, chara::RSCID_TEST_NPC_1500   //
              },
              {
                fld::mmodel::MMDL_TEST_NPC_2000_02, chara::RSCID_TEST_NPC_2000   //
              },
            {
              fld::mmodel::MMDL_TEST_NPC_1500_03, chara::RSCID_TEST_NPC_1500   //
              },
              {
                fld::mmodel::MMDL_TEST_NPC_2000_03, chara::RSCID_TEST_NPC_2000   //
              },
            {
              fld::mmodel::MMDL_TEST_NPC_1500_04, chara::RSCID_TEST_NPC_1500   //
              },
              {
                fld::mmodel::MMDL_TEST_NPC_2000_04, chara::RSCID_TEST_NPC_2000   //
              },
    };
#endif

    for( u32 i = 0; i < GFL_NELEMS( workTalble ); i++ )
    {
      fld::mmodel::MoveModelHeader moveModelHeader;
      moveModelHeader.mmdl_id		= static_cast<fld::mmodel::MMDL_ID>( workTalble[i].mmdlId );
      moveModelHeader.move_code	= fld::mmodel::MOVE_CODE_NPC;
      moveModelHeader.rsc_id		= static_cast<chara::RSC_ID>( workTalble[i].rscId );
      fld::mmodel::MoveModel* pMoveModelWork = m_moveModelManager->CreateMoveModel( &moveModelHeader );
      if( pMoveModelWork )
      {
        m_pRenderingPipeLine->AddEdgeRenderingTarget( pMoveModelWork->getModelInstanceNode() );

        // 位置指定
        FieldTransform* pTransform = pMoveModelWork->GetTransform();
        u32 distanceCount = i;
        pTransform->SetTranslation( workTalble[i].pos );
      }
    }

#if 0
    f32 startX = -100.0f;
    f32 distance = 50.0f;

    for (int i = 0; i < 4; i++)
    {
      fld::mmodel::MoveModelHeader moveModelHeader;
      moveModelHeader.mmdl_id		= static_cast< fld::mmodel::MMDL_ID>( fld::mmodel::MMDL_MAT_TEST_00_001 + i );
      moveModelHeader.move_code	= fld::mmodel::MOVE_CODE_NPC;
      moveModelHeader.rsc_id		= chara::RSCID_MAT_TEST_00;
      fld::mmodel::MoveModel* pMoveModelWork = m_moveModelManager->CreateMoveModel( &moveModelHeader );
      if( pMoveModelWork )
      {
        FieldTransform* pTransform = pMoveModelWork->GetTransform();
        u32 distanceCount = i;

        if( distanceCount >= 2 )
        {
          distanceCount += 1;
        }

        pTransform->SetTranslation( gfl2::math::Vector( startX + distance * distanceCount, 0.0f, 0.0f ) );
        // pTransform->Update();

        m_pRenderingPipeLine->AddEdgeRenderingTarget( pMoveModelWork->getModelInstanceNode() );
      }

      moveModelHeader.mmdl_id		= static_cast< fld::mmodel::MMDL_ID>( fld::mmodel::MMDL_MAT_TEST_01_001 + i );
      moveModelHeader.move_code	= fld::mmodel::MOVE_CODE_NPC;
      moveModelHeader.rsc_id		= chara::RSCID_MAT_TEST_01;
      pMoveModelWork = m_moveModelManager->CreateMoveModel( &moveModelHeader );
      if( pMoveModelWork )
      {
        FieldTransform* pTransform = pMoveModelWork->GetTransform();
        u32 distanceCount = i;

        if( distanceCount >= 2 )
        {
          distanceCount += 1;
        }

        pTransform->SetTranslation( gfl2::math::Vector( startX + distance * distanceCount, 0.0f, 0.0f ) );
        // pTransform->Update();

        m_pRenderingPipeLine->AddEdgeRenderingTarget( pMoveModelWork->getModelInstanceNode() );
      }

    }
#endif
  }


  // ダミーマップの配置
#ifndef TEST_GRAPHIC

#if USE_RTTI_DEBUG
  // RTTIデバッガの初期化
  {
    // アロケーターの生成
    gfl2::heap::NwAllocator* m_pMemAllocator = GFL_NEW(m_pFieldHeap)       gfl2::heap::NwAllocator( m_pFieldHeap );
    gfl2::heap::NwAllocator* m_pDevAllocator = GFL_NEW(m_pFieldDeviceHeap) gfl2::heap::NwAllocator( m_pFieldDeviceHeap );

    // フォントの生成
    gfl2::str::ResFont* m_pResFont = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", m_pFieldHeap, m_pFieldDeviceHeap, m_pFieldHeap );

    // DrawUtilTextの生成
    gfl2::util::DrawUtilText* mpDrawUtilText = GFL_NEW(m_pFieldHeap) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator);
    mpDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
    mpDrawUtilText->SetFont( m_pResFont );

    // RttiDebugerの初期化
    gfl2::rttidebug::RttiEditor::getInstance()->initialize      ( m_pFieldDeviceHeap , 1024*512  , mpDrawUtilText );
    gfl2::rttidebug::RttiParamManager::getInstance()->initialize( m_pFieldDeviceHeap , 1024*1024 );
    m_isDrawRttiEditor = false;
  }
#endif

  // 新コリジョンシステム検証用の壁
  fld::mmodel::MoveModelHeader cityKabe;
  cityKabe.mmdl_id	  = fld::mmodel::MMDL_CITY_KABE;
  cityKabe.move_code	= fld::mmodel::MOVE_CODE_NONE;
  cityKabe.rsc_id		  = chara::RSCID_MAP_01_KABE;	
  fld::mmodel::MoveModel* pModelCityKabe = m_moveModelManager->CreateMoveModel( &cityKabe );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelCityKabe->getModelInstanceNode() );

  // 球コリジョンシステム検証用の床(コリジョンモデルのみ
  fld::mmodel::MoveModelHeader cityYuka;
  cityYuka.mmdl_id	  = fld::mmodel::MMDL_CITY_YUKA;
  cityYuka.move_code	= fld::mmodel::MOVE_CODE_NONE;
  cityYuka.rsc_id		  = chara::RSCID_MAP_01_YUKA;	
  fld::mmodel::MoveModel* pModelCityYuka = m_moveModelManager->CreateMoveModel( &cityYuka );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelCityYuka->getModelInstanceNode() );

  // 3Dコリジョンシーン
  {
    m_pCollsionScene3D = GFL_NEW(m_pFieldHeap) CollisionScene3D;
    m_pCollsionScene3D->setup( m_pfieldHeapAllocator );

    // プレイヤー用アクターの追加
    DynamicActor* pTestDynamicActor = m_pCollsionScene3D->createDynamicActor();
    pTestDynamicActor->createSphere3D( gfl2::math::Vector3(0.0f,0.0f,0.0f),
                                       75.0f );
    m_pCollsionScene3D->registDynamicActor(pTestDynamicActor);

    // 壁用アクターの追加
    StaticActor* pTestStaticActor = m_pCollsionScene3D->createStaticActor();
    pTestStaticActor->createMesh3D( gfl2::math::Vector3(0.0f,0.0f,0.0f),
                                    pModelCityKabe->GetStaticCollision() );
    m_pCollsionScene3D->registStaticActor( pTestStaticActor );
  }

  // 2Dコリジョンシーン
  {
    m_pCollsionScene2D = GFL_NEW(m_pFieldHeap) CollisionScene2D;
    m_pCollsionScene2D->setup( m_pfieldHeapAllocator );

    // プレイヤー用アクターの追加
    DynamicActor* pTestDynamicActor = m_pCollsionScene2D->createDynamicActor();
    pTestDynamicActor->createSphere2D( gfl2::math::Vector3(0.0f,0.0f,0.0f),
                                       75.0f );
    m_pCollsionScene2D->registDynamicActor(pTestDynamicActor);

    // 球：配置モデル用アクターの追加
    StaticActor* pTestStaticActor = m_pCollsionScene2D->createStaticActor();
    pTestStaticActor->createSphere2D( gfl2::math::Vector3(-200.0f,0.0f,200.0f),
                                      75.0f );
    m_pCollsionScene2D->registStaticActor( pTestStaticActor );

    // 箱：配置モデル用アクターの追加
    StaticActor* pTestStaticBoxActor = m_pCollsionScene2D->createStaticActor();
    pTestStaticBoxActor->createBox2D( 
                                   gfl2::math::Vector3(200.0f,0.0f,200.0f),
                                   gfl2::math::Vector3(1.0f,0.0f,0.0f),
                                   gfl2::math::Vector3(0.0f,0.0f,1.0f),
                                   100.0f,
                                   100.0f );
    m_pCollsionScene2D->registStaticActor( pTestStaticBoxActor );

  }

#else

#if 0
  // @note このブロックが4ブロック分のあたり判定を持っています
  fld::mmodel::MoveModelHeader map_00_01;
  map_00_01.mmdl_id	   = fld::mmodel::MMDL_MAP_00_01;
  map_00_01.move_code	 = fld::mmodel::MOVE_CODE_NONE;
  map_00_01.rsc_id		 = chara::RSCID_MAP00_00_01;
  fld::mmodel::MoveModel* pModelCity_00_01 = m_moveModelManager->CreateMoveModel( &map_00_01 );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelCity_00_01->getModelInstanceNode() );

  fld::mmodel::MoveModelHeader map_00_00;
  map_00_00.mmdl_id	   = fld::mmodel::MMDL_MAP_00_00;
  map_00_00.move_code	 = fld::mmodel::MOVE_CODE_NONE;
  map_00_00.rsc_id		 = chara::RSCID_MAP00_00_00;
  fld::mmodel::MoveModel* pModelCity_00_00 = m_moveModelManager->CreateMoveModel( &map_00_00 );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelCity_00_00->getModelInstanceNode() );

  fld::mmodel::MoveModelHeader map_01_00;
  map_01_00.mmdl_id	   = fld::mmodel::MMDL_MAP_01_00;
  map_01_00.move_code	 = fld::mmodel::MOVE_CODE_NONE;
  map_01_00.rsc_id		 = chara::RSCID_MAP00_01_00;	
  fld::mmodel::MoveModel* pModelCity_01_00 = m_moveModelManager->CreateMoveModel( &map_01_00 );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelCity_01_00->getModelInstanceNode() );

  fld::mmodel::MoveModelHeader map_01_01;
  map_01_01.mmdl_id	   = fld::mmodel::MMDL_MAP_01_01;
  map_01_01.move_code	 = fld::mmodel::MOVE_CODE_NONE;
  map_01_01.rsc_id		 = chara::RSCID_MAP00_01_01;	
  fld::mmodel::MoveModel* pModelCity_01_01 = m_moveModelManager->CreateMoveModel( &map_01_01 );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelCity_01_01->getModelInstanceNode() );
#endif
#endif // #ifndef TEST_GRAPHIC

  // カメラ作成
  m_pFieldCamera = GFL_NEW( m_pFieldHeap ) Field::Camera::FieldCamera();
  m_pFieldCamera->Initialize( m_moveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER ) );

  float aspect = (float)400 / (float)240;
  Field::Camera::FieldCamera::ProjectionSetting projectionSetting = 
  {
    1.0f,					  // near
    100000.0f,			// far
    aspect,					// aspect
    30.0f
  };
  m_pFieldCamera->SetProjectionSetting( projectionSetting );

  m_pDefaultCameraData = Field::Camera::CreateDefaultCameraData( m_pFieldHeap );
  m_pFieldCamera->Change( m_pDefaultCameraData, true );

  m_pFieldSystem = GFL_NEW( m_pFieldHeap ) Field::FieldSystem();
  m_pFieldSystem->m_pFieldCamera = m_pFieldCamera;
  m_pFieldSystem->SetMoveModelManager ( m_moveModelManager );

  pGameManager->SetFieldSystem( m_pFieldSystem ); 

  {
    Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = pGameManager->GetFieldScriptSystem();
    pFieldScriptSystem->Create( m_pFieldHeap, pGameManager->GetAsyncFileManager());
  }

  // 天球生成
#ifdef TEST_SKYBOX
  this->CreateSkybox( m_pFieldHeap, m_pFieldDeviceHeap, m_pfieldDeviceHeapAllocator);
#endif TEST_SKYBOX



#ifdef TEST_EFFECT

  GFL_PRINT("Effect Init Start\n");
  // エフェクト初期化
  m_pEffectManager = GFL_NEW( m_pFieldHeap) Field::Effect::Manager();
  m_pEffectManager->Initialize( m_pFieldDeviceHeap, m_pfieldDeviceHeapAllocator);

  // エフェクトレンダーパスの設定情報を更新
  gfl2::Effect::EffectRenderPath::Config config;
  m_pEffectManager->GetEffectRenderPathConfig( &config);
  m_pRenderingPipeLine->GetEffectRenderPath()->SetConfig( config);

  GFL_PRINT("Effect Init End\n");

#endif // TEST_EFFECT


#ifdef TEST_WEATHER
  m_pWeather = GFL_NEW( m_pFieldHeap) Field::Weather::Weather( m_pFieldHeap, m_pfieldDeviceHeapAllocator, m_pEffectManager);
  m_pWeather->Create();
  m_pWeather->Initialzie();

  m_pWeather->GetWeatherEffectManager()->SetRenderingPipeLine( m_pRenderingPipeLine);

  // 天候を雨にする
  m_pWeather->Request( Field::Weather::TYPE_RAIN, 0.0f);
#endif // TEST_WEATHER

#ifdef TEST_FOOT_SPRAY
  m_pEffectFootManager = GFL_NEW( m_pFieldHeap) Field::Effect::FootManager( m_pFieldHeap, m_pfieldDeviceHeapAllocator, m_pEffectManager, m_pRenderingPipeLine);
  m_pEffectFootManager->RegisterMoveModel( m_moveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER));
  m_pEffectFootManager->Initialize();
#endif // TEST_FOOT_SPRAY

#if PM_DEBUG
  m_pFieldDebugMenu = GFL_NEW( m_pFieldHeap ) Field::debug::FieldDebugMenu( m_pFieldHeap, pGameManager );


  m_pDebugFrustumDraw = GFL_NEW( m_pFieldHeap ) DebugFrustumDraw();
  m_pDebugFrustumDraw->Initialize( m_pFieldDeviceHeap, RscManager::getInstance()->GetHighModelResourceNodeTable()[DebugFrustumDraw::RES_INDEX], m_pRenderingPipeLine );

  m_pDebugBoxDraw = GFL_NEW( m_pFieldHeap ) DebugBoxDraw();
  m_pDebugBoxDraw->Initialize( m_pFieldDeviceHeap, RscManager::getInstance()->GetHighModelResourceNodeTable()[DebugBoxDraw::RES_INDEX], m_pRenderingPipeLine );
#endif // if PM_DEBUG

  //#if defined(GF_PLATFORM_CTR)
  Sound::StartBGMReq(STRM_BGM_T_01);
  //#endif

  GFL_RELEASE_PRINT( "FieldTestProc::InitFunc END\n" );
  GFL_RELEASE_PRINT( "[HEAPID_FIELD] TotalFree=0x%x(%fKB) TotalSize=0x%x(%fKB)\n", m_pFieldHeap->GetTotalFreeSize(), m_pFieldHeap->GetTotalFreeSize() / 1024.0f, m_pFieldHeap->GetTotalSize(), m_pFieldHeap->GetTotalSize() / 1024.0f );
  GFL_RELEASE_PRINT( "[HEAPID_FIELD_DEVICE] TotalFree=0x%x(%fKB) TotalSize=0x%x(%fKB)\n", m_pFieldDeviceHeap->GetTotalFreeSize(), m_pFieldDeviceHeap->GetTotalFreeSize() / 1024.0f, m_pFieldDeviceHeap->GetTotalSize(), m_pFieldDeviceHeap->GetTotalSize() / 1024.0f );

  gfl2::heap::HeapBase *pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
  GFL_RELEASE_PRINT( "[HEAPID_SYSTEM] TotalFree=0x%x(%fKB) TotalSize=0x%x(%fKB)\n", pSystemHeap->GetTotalFreeSize(), pSystemHeap->GetTotalFreeSize() / 1024.0f, pSystemHeap->GetTotalSize(), pSystemHeap->GetTotalSize() / 1024.0f );
  GFL_RELEASE_PRINT( "[HEAPID_DEVICE] TotalFree=0x%x(%fKB) TotalSize=0x%x(%fKB)\n", pDeviceHeap->GetTotalFreeSize(), pDeviceHeap->GetTotalFreeSize() / 1024.0f, pDeviceHeap->GetTotalSize(), pDeviceHeap->GetTotalSize() / 1024.0f );

  FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_DumpAll();

#if PM_DEBUG

  // トラバースして、モデルの情報を表示
  DebugTraverseModelInstanceInfoDraw();

#endif // PM_DEBUG


#ifndef TEST_GRAPHIC
  {
    // ムーブコードマネージャーの初期化
    m_FieldMoveCodeManager.Initialize(m_pFieldDeviceHeap);

    // アクションコマンドマネージャーの初期化
    m_FieldActionComandManager.Initialize(m_pFieldDeviceHeap);

    // 非同期読み込み開始
    u32 characterId = 0;
    u32 arcId       = ARCID_CHARA_MODEL;
    m_FieldCharaModelManager.Initialize(this->GetGameManager()->GetAsyncFileManager(), m_pFieldDeviceHeap, arcId);
    m_FieldCharaModelManager.LoadAsync(characterId);

    // 非同期読み込み待ち
    while( true ) 
    {
      if (m_FieldCharaModelManager.IsLoaded(characterId))
      {
        break;
      }
    }

    // 動作モデル作成
    Field::MoveModel::FieldMoveModelHeader header;
    header.characterId = characterId;
    header.moveCodeId  = Field::MoveModel::FIELD_MOVE_CODE_PLAYER;
    m_FieldCharaModelManager.Create( Field::MoveModel::FIELD_MOVE_MODEL_WORK_0 , &header );

    // SRT設定
    Field::MoveModel::FieldMoveModel* pCharaModel = m_FieldCharaModelManager.getFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_WORK_0 );
    pCharaModel->SetPosition(-300, 0, 300);
    pCharaModel->SetRotation(0, 0, 0);
    pCharaModel->SetScale(1, 1, 1);

    // 動作モデルのコリジョンを作成
    pCharaModel->getDynamicActor()->createSphere3D( gfl2::math::Vector3(0.0f,0.0f,0.0f), 75.0f );
    m_pCollsionScene3D->registDynamicActor(pCharaModel->getDynamicActor());

#if USE_RTTI_DEBUG
    // RTTIエディタにメンバ情報を表示するリクエストを出す
    m_FieldCharaModelManager.CreateRttiEditorPage( Field::MoveModel::FIELD_MOVE_MODEL_WORK_0 );
#endif

    m_pRenderingPipeLine->AddEdgeRenderingTarget( pCharaModel->GetModelInstanceNode_() );
  }
#endif

  return gfl2::proc::RES_FINISH;
}

/**
* @brief	  PROCのメイン更新を行う関数
* @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result FieldTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
#if PM_DEBUG
  
  //
  // デバッグ操作による介入
  //
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

#ifndef TEST_GRAPHIC

  m_FieldCharaModelManager.PrevUpdate();

#if USE_RTTI_DEBUG
  
  if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::R3 ) )
  {
    m_isDrawRttiEditor = m_isDrawRttiEditor ? false : true;
  }

  // 次のリクエストをエディタで表示する
  if     ( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::Right ) ){ gfl2::rttidebug::RttiEditor::getInstance()->nextTag(); }
  // 前のリクエストをエディタで表示する
  else if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::Left  ) ){ gfl2::rttidebug::RttiEditor::getInstance()->prevTag(); }

  // 次のパラメータを選択する
  if     ( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::Down ) ){ gfl2::rttidebug::RttiEditor::getInstance()->nextParam(); }
  // 前のパラメータを選択する
  else if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::Up   ) ){ gfl2::rttidebug::RttiEditor::getInstance()->prevParam(); }
  // 選択中のパラメータの値を増やす
  else if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::R2   ) ){ gfl2::rttidebug::RttiEditor::getInstance()->addValue();  }
  // 選択中のパラメータの値を減らす
  else if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::L2   ) ){ gfl2::rttidebug::RttiEditor::getInstance()->pullValue(); }

  gfl2::rttidebug::RttiEditor::getInstance()->setEnableDraw( m_isDrawRttiEditor );
  gfl2::rttidebug::RttiEditor::getInstance()->update();
  gfl2::rttidebug::RttiEditor::getInstance()->draw();

#endif
#endif

  //
  // GPU/CPU負荷をDUMP
  // キーが押されてから、WORK_TICK_PRINT_AVE_COUNTフレーム分の平均値を出す。
  // 全体Tickの方は仕組みが違うので、20フレーム固定
  // Tickの中に書くと数値がおかしくなるのでここ
  //
  static u32 s_AveVount = 0; 
  if( s_AveVount > 0 )
  {
    s_AveVount--;
    if( s_AveVount == 0 )
    {
      // FIELD_DEBUG_TICK_SET_FLAG( false );
      // GAME_MANAGER_DEBUG_TICK_SET_FLAG( false );
      TickDump();
    };
  }

  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
    {
      s_AveVount = Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::WORK_TICK_PRINT_AVE_COUNT ];
      FIELD_DEBUG_TICK_RESET();

#if GAME_MANAGER_DEBUG_TICK_ENABLE
      GameSys::GameManager::s_TickResetRequest = true;
#endif
      // GAME_MANAGER_DEBUG_TICK_RESET();
      FIELD_DEBUG_TICK_SET_FLAG( true );
      GAME_MANAGER_DEBUG_TICK_SET_FLAG( true );
    }
  }

#endif

  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::UPDATE );
  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::DEBUG_UPDATE );

#if PM_DEBUG
  
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    // カメラを元に戻す
    if( pButton->IsTrigger( gfl2::ui::BUTTON_X) )
    {
      m_pFieldCamera->Change( m_pDefaultCameraData, true );
    }
  }


  // 処理負荷計測用
  fld::mmodel::MoveModel* pModelPlayer = m_moveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
  if( pModelPlayer )
  {
    pModelPlayer->setVisible  ( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::ENABLE_PLAYER_VISIBLE  ] );
    pModelPlayer->setAnimation( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::ENABLE_PLAYER_ANIMAION ] );

    if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::ENABLE_PLAYER_VISIBLE ] )
    {
      pModelPlayer->addSceneGraph();
    }
    else
    {
      pModelPlayer->removeSceneGraph();
    }

    pModelPlayer->setIk( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::ENABLE_PLAYER_IK ] == 1 );

    if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::ENABLE_POKE_RIDE ] )
    {
      pModelPlayer->GetTransform()->SetEffectTranslation( gfl2::math::Vector( 0.0f, 80.0f, 0.0f ));
    }
    else
    {
      pModelPlayer->GetTransform()->SetEffectTranslation( gfl2::math::Vector( 0.0f, 0.0f, 0.0f ));
    }
  }

  // エッジ表示設定の更新
  m_pRenderingPipeLine->SetEdgeMode( static_cast<Test::FieldTest::MyRenderingPipeLine::EdgeMode>( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_EDGE_MODE ] ));

  //
  // 動作モデルのOn/Off処理
  //


  u32 workTable[][2] = 
  {
    {
      fld::mmodel::MMDL_MAP_00_00,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAP_00_00
    },
    {
      fld::mmodel::MMDL_MAP_00_01,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAP_00_01
      },
      {
        fld::mmodel::MMDL_MAP_01_00,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAP_01_00
      },
      {
        fld::mmodel::MMDL_MAP_01_01,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAP_01_01
        },
#if 0
        {
          fld::mmodel::MMDL_EDGE_TEST_00,  Field::debug::FieldDebugTypes::DRAW_SWITCH_EDGE_TEST_00,
        },
        {
          fld::mmodel::MMDL_EDGE_TEST_01,  Field::debug::FieldDebugTypes::DRAW_SWITCH_EDGE_TEST_01,
          },
          {
            fld::mmodel::MMDL_MAT_TEST_00,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_00,
          },
          {
            fld::mmodel::MMDL_MAT_TEST_01,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_01,
            },
            {
              fld::mmodel::MMDL_MAT_TEST_00_001,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_00_PLUS,
            },
            {
              fld::mmodel::MMDL_MAT_TEST_00_002,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_00_PLUS,
              },
              {
                fld::mmodel::MMDL_MAT_TEST_00_003,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_00_PLUS,
              },
              {
                fld::mmodel::MMDL_MAT_TEST_00_004,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_00_PLUS,
                },
                {
                  fld::mmodel::MMDL_MAT_TEST_01_001,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_01_PLUS,
                },
                {
                  fld::mmodel::MMDL_MAT_TEST_01_002,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_01_PLUS,
                  },
                  {
                    fld::mmodel::MMDL_MAT_TEST_01_003,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_01_PLUS,
                  },
                  {
                    fld::mmodel::MMDL_MAT_TEST_01_004,  Field::debug::FieldDebugTypes::DRAW_SWITCH_MAT_TEST_01_PLUS,
                    },
#endif
#if 0
                    {
                      fld::mmodel::MMDL_TEST_KISEKAE, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_KISEKAE,
                    },
#endif
                    {
                      fld::mmodel::MMDL_TEST_RIM, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_RIM,
                      },
                      {
                        fld::mmodel::MMDL_TEST_KENTAUROS, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_KENTAUROS,
                      },
                      {
                        fld::mmodel::MMDL_TEST_NPC_1500, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_NPC_1500,
                        },
                        {
                          fld::mmodel::MMDL_TEST_NPC_2000, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_NPC_2000,
                        },
                      {
                        fld::mmodel::MMDL_TEST_NPC_1500_02, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_NPC_1500_02,
                        },
                        {
                          fld::mmodel::MMDL_TEST_NPC_2000_02, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_NPC_2000_02,
                        },
                      {
                        fld::mmodel::MMDL_TEST_NPC_1500_03, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_NPC_1500_03,
                        },
                        {
                          fld::mmodel::MMDL_TEST_NPC_2000_03, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_NPC_2000_03,
                        },
                      {
                        fld::mmodel::MMDL_TEST_NPC_1500_04, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_NPC_1500_04,
                        },
                        {
                          fld::mmodel::MMDL_TEST_NPC_2000_04, Field::debug::FieldDebugTypes::DRAW_SWITCH_TEST_NPC_2000_04,
                        },
  };

  for( u32 i = 0; i < GFL_NELEMS( workTable ); i++ )
  {
    fld::mmodel::MoveModel* pModelModel = m_moveModelManager->GetMoveModel( static_cast<fld::mmodel::MMDL_ID>( workTable[i][0] ) );
    if( pModelModel )
    {
      fld::mmodel::MoveModel::LOD lod = pModelModel->getLod();
      pModelModel->m_IsInterpLoop = false;
      pModelModel->setInterest( false );

      switch( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ workTable[i][1] ] )
      {
      case 0: // 表示OFF
        {
          pModelModel->setVisible( false );
          pModelModel->setAnimation( false );
          break;
        }
      case 1: // ハイモデル(アニメーション更新あり)
        {
          pModelModel->setVisible( true );
          pModelModel->setAnimation( true );
          pModelModel->setAnimationUpdateFrame( 1 );
          lod = fld::mmodel::MoveModel::LOD_HIGH;
          break;
        }
      case 2: // ハイモデル(アニメーションブレンド)
        {
          pModelModel->setVisible( true );
          pModelModel->setAnimation( true );
          pModelModel->setAnimationUpdateFrame( 1 );
          pModelModel->m_IsInterpLoop = true;
          lod = fld::mmodel::MoveModel::LOD_HIGH;
          break;
        }
       case 3: // ハイモデル(アニメーション更新あり、注目)
        {
          pModelModel->setVisible( true );
          pModelModel->setAnimation( true );
          pModelModel->setAnimationUpdateFrame( 1 );
          pModelModel->setInterest( true );
          lod = fld::mmodel::MoveModel::LOD_HIGH;
          break;
        }

      case 4: // ハイモデル(アニメーション更新なし)
        {
          pModelModel->setVisible( true );
          pModelModel->setAnimation( false );
          lod = fld::mmodel::MoveModel::LOD_HIGH;
          pModelModel->setAnimationUpdateFrame( 1 );
          break;
        }
      case 5: // LOWモデル(アニメーション更新あり)
        {
          pModelModel->setVisible( true );
          pModelModel->setAnimation( true );
          lod = fld::mmodel::MoveModel::LOD_LOW;
          pModelModel->setAnimationUpdateFrame( 1 );
          break;
        }
      case 6: // LOWモデル(アニメーション2フレームに1回更新)
        {
          pModelModel->setVisible( true );
          pModelModel->setAnimation( true );
          lod = fld::mmodel::MoveModel::LOD_LOW;
          pModelModel->setAnimationUpdateFrame( 2 );
          break;
        }
      case 7: // LOWモデル(アニメーション更新なし)
        {
          pModelModel->setVisible( true );
          pModelModel->setAnimation( false );
          pModelModel->setAnimationUpdateFrame( 1 );
          lod = fld::mmodel::MoveModel::LOD_LOW;
          break;
        }
      }// 

      // LOD切り替え
      if( pModelModel->CheckChangeLodTest( lod ) )
      {
        m_pRenderingPipeLine->RemoveEdgeRenderingTarget( pModelModel->getModelInstanceNode() );
        pModelModel->UpdateLodTest( lod );
        m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelModel->getModelInstanceNode() );
      }

    } // if( pModelModel )
  } // for


  if( m_pSkybox )
  {
    m_pSkybox->SetEnable( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_SKYBOX ] == 1 );
  }

#endif
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::DEBUG_UPDATE );



  // GFL_PRINT("FieldTestProc::UpdateFunc\n");
  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::MMDL_UPDATE );

  // キャラクターの動きを更新
  m_moveModelManager->UpdateAllModels();

  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::MMDL_UPDATE );
  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::TRAVERSE_MDL_FAST );

  // トラバース処理にてワールド座標を更新
  UpdateModelFast();
  // gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();

  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::TRAVERSE_MDL_FAST );
 
  // UpdateModelFastAfter();

  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::COLLISION );

  // コリジョン更新
#if PM_DEBUG
  if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::ENABLE_PLAYER_COLLISION ] )
#endif PM_DEBUG
  {
    systemCollision::getInstance()->registDynamicActor( m_moveModelManager->GetMoveModel(fld::mmodel::MMDL_PLAYER), &fld::mmodel::MoveModel::rayCallbackFunc );

#ifndef TEST_GRAPHIC
#else
    systemCollision::getInstance()->registTerrain ( m_pFieldTerrain->GetCollisionModel(), NULL);
#endif  // #ifndef TEST_GRAPHIC

    systemCollision::getInstance()->update();
    systemCollision::getInstance()->resetActor();
  }

#ifndef TEST_GRAPHIC
  
  // コリジョン前更新
  m_FieldCharaModelManager.Update();
  // 新コリジョンシステムの更新
  m_pCollsionScene3D->update();
  m_pCollsionScene2D->update();
  // コリジョン後更新
  m_FieldCharaModelManager.PostUpdate();

  // トラバース処理にてワールド座標を更新
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
  
#endif

  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::COLLISION );
  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::CAMERA_UPDATE );

  // カメラ更新
  m_pFieldCamera->Update();

  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::CAMERA_UPDATE );

  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::GRASS_UPDATE );
#ifdef TEST_ENCGRASS
  // テストのエンカウント草更新
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = m_moveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();
      gfl2::math::Vector trans = pTransform->GetTranslation();
      m_pTestEncountGrass->Update( gfl2::math::Vector3(trans.GetX(), trans.GetY(), trans.GetZ()) );
    }
  }
#endif // TEST_ENCGRASS
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::GRASS_UPDATE );

  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::SHADOW_UPDATE );
#ifdef TEST_SHADOW
  m_pTestShadow->Update( m_moveModelManager );
#endif
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::SHADOW_UPDATE );


  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::EFFECT_UPDATE );
#ifdef TEST_WEATHER
  m_pWeather->Update();

  gfl2::math::Vector _pos;
  m_pFieldCamera->GetCameraPosition( &_pos);
  gfl2::math::Vector3 pos( _pos.GetX(), _pos.GetY(), _pos.GetZ());
  m_pWeather->GetWeatherEffectManager()->UpdateAfterCameraUpdate( pos);

  // @todo 仮対応
  gfl2::math::Vector _playerPos = m_moveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER)->GetTransform()->GetTranslation();
  gfl2::math::Vector3 playerPos( _playerPos.x, _playerPos.y, _playerPos.z);
  m_pWeather->GetWeatherEffectManager()->UpdatePlayerPos( playerPos);
#endif // TEST_WEATHER

#ifdef TEST_FOOT_SPRAY

  m_pEffectFootManager->Update();

#endif // TEST_FOOT_SPRAY

#ifdef TEST_EFFECT

  // エフェクトシステム更新
  m_pEffectManager->Update();

#endif // TEST_EFFECT
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::EFFECT_UPDATE );
  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::TRRAIN_UPDATE );

  {
    gfl2::math::Vector _playerPos = m_moveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER) ->GetTransform()->GetTranslation();
    gfl2::math::Vector3 playerPos( _playerPos.x, _playerPos.y, _playerPos.z);

    gfl2::math::Matrix44 cullingMatrix;
    m_pFieldCamera->GetCullingMatrix( &cullingMatrix );
#ifdef TEST_GRAPHIC
    m_pFieldTerrain->Update( playerPos, cullingMatrix );
#endif
  }
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::TRRAIN_UPDATE );
#if 0
  // レンダリングパイプライン更新
  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE );
  m_pRenderingPipeLine->Update();
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE );
#endif
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::UPDATE );
#if PM_DEBUG
  gfl2::math::Matrix34 viewMat;
  Field::Camera::FieldCamera::ProjectionSetting projSetting;
  m_pFieldCamera->GetGameCamera()->GetViewMatrix( &viewMat );
  m_pFieldCamera->GetProjectionSetting( &projSetting );
  m_pDebugFrustumDraw->Update( viewMat, projSetting.farZ );


  // 主人公のいるブロック周辺のAABBを表示
  {
    gfl2::math::AABB aabb( gfl2::math::Vector(0,0,0), gfl2::math::Vector(100,100,400) );
    m_pDebugBoxDraw->Update(aabb);
  }

#endif // PM_DEBUG 

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
//------------------------------------------------------------------
void FieldTestProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::PRE_DRAW );


  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  // バッファクリア設定更新
#if USE_SKYBOX_BUFFER_CLEAR
  // スカイボックスでバッファクリアしているので、スカイボックスが無効な場合はバッファクリアする必要がある。
  if( m_pSkybox &&m_pSkybox->IsEnable() )
  {
    // gamemanagerのバッファクリアを無くす
    GameSys::GameManager::BUFFER_CLEAR_SETTING bufferClearSetting[2];
    pGameManager->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT , &bufferClearSetting[0] );
    pGameManager->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::RIGHT, &bufferClearSetting[1] );
    bufferClearSetting[0].clearTarget &= ~(GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL);
    bufferClearSetting[1].clearTarget &= ~(GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL);
    pGameManager->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT , bufferClearSetting[0] );
    pGameManager->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::RIGHT, bufferClearSetting[1] );
  }
  else
  {
    // gamemanagerのバッファクリアを行う
    GameSys::GameManager::BUFFER_CLEAR_SETTING bufferClearSetting[2];
    pGameManager->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT , &bufferClearSetting[0] );
    pGameManager->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::RIGHT, &bufferClearSetting[1] );
    bufferClearSetting[0].clearTarget |= (GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL);
    bufferClearSetting[1].clearTarget |= (GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL);
    pGameManager->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT , bufferClearSetting[0] );
    pGameManager->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::RIGHT, bufferClearSetting[1] );
  }
#endif


  // 射影変換行列(パースペクティブ(透視法)射影)
  gfl2::math::Matrix44 projection;
  gfl2::math::Matrix34 view;
  b32 pivotFlag = true;

  // 下画面に3Dを表示する
  {
    if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_DOWN_DISPLAY ] == 0 )
    {
      Field::Camera::FieldCamera::ProjectionSetting setting;
      m_pFieldCamera->GetProjectionSetting( &setting );
      setting.aspect = (float)400 / (float)240;
      m_pFieldCamera->SetProjectionSetting( setting );
    }
    else
    {
      Field::Camera::FieldCamera::ProjectionSetting setting;
      m_pFieldCamera->GetProjectionSetting( &setting );
      setting.aspect = (float)320 / (float)240;
      m_pFieldCamera->SetProjectionSetting( setting );
    }
  }

  m_pFieldCamera->GetProjectionMatrix( &projection, pivotFlag );
  m_pFieldCamera->GetViewMatrix( &view );

#if 0
#if defined(GF_PLATFORM_CTR)
  f32 depthLevel = 4.0f;
  f32 factor	   = 1.0f;
  gfl2::gfx::GFGL::GetStereoMatrix(	projection,
    view,
    depthLevel,
    factor, 
    &m_ProjMatTable[gfl2::gfx::CtrDisplayNo::LEFT],
    &m_ViewMatTable[gfl2::gfx::CtrDisplayNo::LEFT],
    &m_ProjMatTable[gfl2::gfx::CtrDisplayNo::RIGHT],
    &m_ViewMatTable[gfl2::gfx::CtrDisplayNo::RIGHT]);
#endif
#if defined(GF_PLATFORM_WIN32)


#endif
#endif
  for (u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
  {
    m_ProjMatTable[i] = projection;
    m_ViewMatTable[i] = view;
  }

  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::PRE_DRAW );

#if PM_DEBUG
  // フレームレート切り替え
  // @todo Updateに仕込むと、トリガー入力情報が落ちず、無限切り替えになってしまう
  {
    GameSys::FrameMode mode( GameSys::FRAMEMODE_30 );
    s32 work = Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::FRAME_LATE_MODE  ];
    switch( work )
    {
    case 0: mode = GameSys::FRAMEMODE_30; break;
    case 1: mode = GameSys::FRAMEMODE_20; break;
    case 2: mode = GameSys::FRAMEMODE_15; break;
    }
    if( pGameManager->GetFrameMode() != mode )
    {
      pGameManager->RequestFrameMode( mode );  
    }
  }
#endif // if PM_DEBUG

}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void FieldTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{


  // @todo 立体視の場合はPreDrawFuncでコマンドをセーブし、DrawFuncで実行する形にする 150218 ikuta_junya
  switch( displayNo )
  {
    // 左画面描画
  case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_DOWN_DISPLAY ] == 1 )
      {
        break;
      }

      FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::DRAW_LEFT );
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
      m_pRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );	

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext drawContext;
      drawContext.m_pRenderTarget = pBackBuffer;
      drawContext.m_pDepthStencil = pDepthBuffer;
      drawContext.m_DisplayHeight = displayHeight;
      drawContext.m_DisplayWidth = displayWidth;
      drawContext.m_View = m_ViewMatTable[ displayNo ];
      drawContext.m_Projection = m_ProjMatTable[ displayNo ];


      // レンダリングパイプライン更新
      FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE );
#ifndef TEST_GRAPHIC
      m_pRenderingPipeLine->Update();
#else
      m_pRenderingPipeLine->UpdateTest( drawContext );
#endif
      FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE );


      FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_EXECUTE );
#ifndef TEST_GRAPHIC
      m_pRenderingPipeLine->Execute();
#else
      m_pRenderingPipeLine->ExecuteTest( drawContext );
#endif
      FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_EXECUTE );
      FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::DRAW_LEFT );
      break;
    } // case gfl2::gfx::CtrDisplayNo::LEFT

    // 右画面描画(立体視時のみ)
  case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
#if 0
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
      m_pRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );	

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
#endif
      break;
    } // case gfl2::gfx::CtrDisplayNo::RIGHT

    // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:
    {
      if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_DOWN_DISPLAY ] == 0 )
      {
        break;
      }

      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
      m_pRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );	

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext drawContext;
      drawContext.m_pRenderTarget = pBackBuffer;
      drawContext.m_pDepthStencil = pDepthBuffer;
      drawContext.m_DisplayHeight = displayHeight;
      drawContext.m_DisplayWidth = displayWidth;
      drawContext.m_View = m_ViewMatTable[ displayNo ];
      drawContext.m_Projection = m_ProjMatTable[ displayNo ];

      m_pRenderingPipeLine->Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::DOWN

  } // switch( displayNo )
}

/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result FieldTestProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  // コントローラーによる入力を取得(UIの入力システムに乗っかっています 要修正)
  GameSys::GameManager*     game_manager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager*  device_manager  = game_manager->GetUiDeviceManager();
  gfl2::ui::Button*         pButton         = device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsHold( gfl2::ui::BUTTON_START )  )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result FieldTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_PRINT("FieldTestProc::EndFunc\n");
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );

#if PM_DEBUG
  GFL_SAFE_DELETE( m_pDebugFrustumDraw );
  GFL_SAFE_DELETE( m_pDebugBoxDraw );
  GFL_SAFE_DELETE( m_pFieldDebugMenu );
#endif // if PM_DEBUG

#ifdef TEST_FOOT_SPRAY
  m_pEffectFootManager->Terminate();
  GFL_SAFE_DELETE( m_pEffectFootManager);
#endif // TEST_FOOT_SPRAY

#ifdef TEST_WEATHER
  m_pWeather->Destroy();
  GFL_SAFE_DELETE( m_pWeather);
#endif // TEST_WEATHER

#ifdef TEST_EFFECT
  m_pEffectManager->Terminate();
  GFL_SAFE_DELETE( m_pEffectManager);
#endif // TEST_EFFECT

#ifdef TEST_SHADOW
  GFL_SAFE_DELETE(m_pTestShadow);
#endif

#ifdef TEST_ENCGRASS
  GFL_SAFE_DELETE(m_pTestEncountGrass);
#endif

#ifdef TEST_SKYBOX
  m_pSkybox->Destroy();
  GFL_SAFE_DELETE( m_pSkybox);
#endif // TEST_SKYBOX

#ifdef TEST_GRAPHIC
  GFL_SAFE_DELETE(m_pFieldTerrain);
#else
  GFL_SAFE_DELETE(m_pCollsionScene2D);
  GFL_SAFE_DELETE(m_pCollsionScene3D);
#endif

  GFL_SAFE_DELETE(m_pDrawEnvNode);

  GFL_SAFE_DELETE( m_pDepthStencilTexture );
  GFL_SAFE_DELETE(m_pRenderingPipeLine);
  GFL_SAFE_DELETE(m_moveModelManager);

  {
    Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = pGameManager->GetFieldScriptSystem();
    pFieldScriptSystem->Delete();
  }

  pGameManager->SetFieldSystem( NULL ); 
  GFL_SAFE_DELETE( m_pFieldSystem );

  // アクションコマンドワークの削除
  field::mmodel::ActionCmdManager::Finalize();

  // キャラクターマネージャー後処理
  RscManager::getInstance()->Terminate();

  // カメラデータ削除
  GFL_SAFE_DELETE( m_pFieldCamera );
  GFL_SAFE_DELETE( m_pDefaultCameraData->pData );
  GFL_SAFE_DELETE( m_pDefaultCameraData );

  // 通常newが使用するヒープを指定、外す @note 通常new使用箇所が多く、HEAPID_SYSYTEMが空になってしまうので暫定対応
  gfl2::heap::Manager::SetDefaultNewUseHeap( NULL );

#if CHECK_MEMORY_LEAK_IN_FIELD_PROC
  GFL_DELETE( m_pfieldHeapAllocator );
  GFL_DELETE( m_pfieldDeviceHeapAllocator );
  GFL_DELETE_HEAP( m_pFieldDeviceHeap );
  GFL_DELETE_HEAP( m_pFieldHeap );
  gfl2::gfx::GFGL::SetAllocator( m_pFieldGfglHeapOld );
#endif

  return gfl2::proc::RES_FINISH;
}


/**
* @brief   天球の生成
*
* @param   pHeap 生成に使用するヒープ (Skyboxにも渡される)
* @param   pDeviceHeap メモリ使用量計測に使用する
* @param   pGLAllocator 天球がリソースを作成する際に使用する
*/
void FieldTestProc::CreateSkybox( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::gfx::IGLAllocator* pGLAllocator)
{
  GameSys::GameManager*       pGameManager	    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::fs::AsyncFileManager* pAsyncFileManager = pGameManager->GetAsyncFileManager();

  // @note Createをしなければ他の処理も動かない
  m_pSkybox = GFL_NEW( pHeap) Skybox::Skybox();

#if PM_DEBUG
  s32 oldSystemHeapFreeSize = pHeap->GetTotalFreeSize();
  s32 oldDeviceHeapFreeSize = pDeviceHeap->GetTotalFreeSize();
#endif // PM_DEBUG

  m_pSkybox->StartCreate( pHeap, pGLAllocator, pAsyncFileManager);
  m_pSkybox->UpdateCreate();
  m_pSkybox->Initialize();

  for( u32 i = 0 ; i < Skybox::Sky::MODEL_TYPE_MAX; ++i)
  {
    m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pSkybox->GetSky()->GetModel( i)->GetModelInstanceNode());
    m_pSkybox->GetSky()->GetModel( i)->GetModelInstanceNode()->SetSortPriority(100);
  }
  for( u32 i = 0 ; i < Skybox::Cloud::MODEL_TYPE_MAX; ++i)
  {
    m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pSkybox->GetCloud()->GetModel( i)->GetModelInstanceNode());
    m_pSkybox->GetCloud()->GetModel( i)->GetModelInstanceNode()->SetSortPriority(101);
  }

#if PM_DEBUG
  s32 currentSystemHeapFreeSize = pHeap->GetTotalFreeSize();
  s32 currentDeviceHeapFreeSize = pDeviceHeap->GetTotalFreeSize();

  s32 systemHeapAllocSize = (oldSystemHeapFreeSize - currentSystemHeapFreeSize);
  s32 deviceHeapAllocSize = (oldDeviceHeapFreeSize - currentDeviceHeapFreeSize);

  GFL_PRINT( "----------------------------------------------------\n");
  GFL_PRINT( "Skybox Heap Allocate Size \n");
  GFL_PRINT( "  System Heap : 0x%x\n", systemHeapAllocSize);
  GFL_PRINT( "  Device Heap : 0x%x\n", deviceHeapAllocSize);
  GFL_PRINT( "----------------------------------------------------\n");
#endif // PM_DEBUG

}


# if 0

// 【高さor交点の衝突判定】
//  2015.02.10 saita_kazuki
// 使い方の説明のためにCommonApplicationでテストとして書いたものをサンプルとして下に書きます。
// これを参考に使用する際は、判定するオブジェクト、位置の取得、位置の更新などを使用箇所に合ったものに変更してください。
// また、CollisionModelでVectorを使用しているので、ここでもVectorを使用しています。(コリジョンモデル内の置き換えに合わせて置き換えたい)

// ------------------------------------
// コリジョンモデル生成
bool CommonApplication::InitializeCollisionModel( gfl2::heap::HeapBase *pSystemHeap)
{
  // 高さコリジョンメッシュデータ読み込み
  {
    s32 size;
#if defined( GF_PLATFORM_CTR)
    c8 *pBinData = (c8*)LocalFunc::ReadData( &System::GflUse::m_Allocator, "rom:/CollisionTest/takasa_test.gfbcol" , &size);
#else
    c8 *pBinData = (c8*)LocalFunc::ReadData( &System::GflUse::m_Allocator, "R:/home/niji/git_program/niji_project/romfiles/CollisionTest/takasa_test.gfbcol" , &size);
#endif // GF_PLATFORM_CTR

    gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
    resourceData.SetModelData( pBinData, size);

    gfl2::renderingengine::scenegraph::resource::ResourceNode *pCollisionBinModelRootNode_Takasa;
    pCollisionBinModelRootNode_Takasa = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

    m_pCollisionModel_Takasa = GFL_NEW( pSystemHeap) gfl2::collision::CollisionModel();
    m_pCollisionModel_Takasa->Initialize( pCollisionBinModelRootNode_Takasa );
  }

  // 交点コリジョンメッシュデータ読み込み
  {
    s32 size;
#if defined( GF_PLATFORM_CTR)
    c8 *pBinData = (c8*)LocalFunc::ReadData( &System::GflUse::m_Allocator, "rom:/CollisionTest/intersection_test.gfbcol" , &size);
#else
    c8 *pBinData = (c8*)LocalFunc::ReadData( &System::GflUse::m_Allocator, "R:/home/niji/git_program/niji_project/romfiles/CollisionTest/intersection_test.gfbcol" , &size);
#endif // GF_PLATFORM_CTR

    gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
    resourceData.SetModelData( pBinData, size);

    gfl2::renderingengine::scenegraph::resource::ResourceNode *pCollisionBinModelRootNode_Intersection;
    pCollisionBinModelRootNode_Intersection = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

    m_pCollisionModel_Intersection = GFL_NEW( pSystemHeap) gfl2::collision::CollisionModel();
    m_pCollisionModel_Intersection->Initialize( pCollisionBinModelRootNode_Intersection );
  }

  return true;
}

// ------------------------------------
// 高さコリジョンとの判定
void CommonApplication::UpdateCollision_Height()
{
  // 当たり判定に使用する位置を取得
  const gfl2::math::Matrix& worldMatrix = m_pModelInstanceNode->GetWorldMatrix();
  gfl2::math::Vector pos = worldMatrix.GetElemPosition();

  // 高さ判定コールバッククラスを作成
  Test::FieldTest::SimpleCollisionCallbackable_Height callBack;

  // コリジョンモデルのGetHeightで高さを取得。コールバッククラスを渡します。
  // この中でメッシュの全ポリゴンのXZ平面に位置が含まれているかを判定し、当たっていれば最大10件まで衝突情報を溜めます。
  m_pCollisionModel_Takasa->GetHeight(
    pos,
    NULL,
    &callBack
    );

  // 一度でも当たったか判定
  if( callBack.IsHit())
  {
    // 指定高さに一番近い&&指定位置よりも上にある高さを返します
    f32 newHeight = callBack.GetNearestHeight( pos.y);

    // 取得した高さに更新します
    gfl2::math::Vector newLocalPos = m_pModelInstanceNode->GetLocalTranslate();
    newLocalPos.y = newHeight;
    m_pModelInstanceNode->SetLocalTranslate( newLocalPos);
  }
}

// ------------------------------------
// 交点コリジョンとの判定
void CommonApplication::UpdateCollision_Intersection()
{
  // ※使い方は高さ版とほぼ同じです。違うところのみコメント書きます

  const gfl2::math::Matrix& worldMatrix = m_pModelInstanceNode->GetWorldMatrix();
  gfl2::math::Vector pos = worldMatrix.GetElemPosition();

  // 交点判定コールバッククラスを作成
  Test::FieldTest::SimpleCollisionCallbackable_Intersection callBack;

  // コリジョンモデルのGetIntersectionで高さを取得。コールバッククラスを渡します。
  // 下で使用しているのはIntersectionExです。こちらは表裏も判定し、表からレイが当たらないといけません。。
  // こちらは位置の代わりに線分の始点と終点を渡します。
  m_pCollisionModel_Intersection->GetIntersectionEx(
    m_rayStart, m_rayEnd,
    NULL,
    &callBack
    );

  if( callBack.IsHit())
  {
    // 指定位置にもっとも近い交点を取得します。
    gfl2::math::Vector intersection = callBack.GetNearestIntersection( pos);

    m_pModelInstanceNode->SetLocalTranslate( intersection);
  }
}
#endif


GFL_NAMESPACE_END( FieldTest );
GFL_NAMESPACE_END( Test );
