//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   Fieldmap.cpp
 *  @brief  フィールドマップ
 *  @author GAME FREAK.inc
 *  @date   2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// gfl2
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_heap_manager.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <arc_def_index/arc_def.h>//@todo_saito 削除する

// niji
#include "System/include/GflvUse.h"              // for DisplayDeviceHandle
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "GameSystem/include/GameManager.h"
#include "GameSystem/include/GameData.h"
#include "ViewerProc/include/FieldRo/FieldGLAllocatorFromHeap.h"

// Field
#include "ViewerProc/include/FieldRo/Fieldmap.h"
#include "ViewerProc/include/FieldRo/MyRenderingPipeLine.h" 
///#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "ViewerProc/include/FieldRo/FieldEnvManager.h"
///#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"
///#include "Field/FieldStatic/include/FieldAttribute.h"

// Terrain
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainManager.h"
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainLayoutData.h"

// Camera
#include "ViewerProc/include/FieldRo/Camera/FieldCameraManager.h"
#include "ViewerProc/include/FieldRo/Camera/FieldCameraUnit.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetTypeMoveModel.h"

// Camera ( poke_3d_lib )
#include <model/include/gfl2_BaseCamera.h>


// キャラクターシステム
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel_define.h"
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModelManager.h"
///#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeManager.h"
///#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"

// フィールドレジデント
#include "ViewerProc/include/FieldStatic/FieldResident/FieldResident.h"

// コリジョンシステム
#include "ViewerProc/include/FieldStatic/Collision/BaseActor.h"
#include "ViewerProc/include/FieldStatic/Collision/StaticActor.h"
#include "ViewerProc/include/FieldStatic/Collision/DynamicActor.h"
#include "ViewerProc/include/FieldStatic/Collision/BaseCollisionScene.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneDefault.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneNone.h"

//下画面
///#include "Field/FieldRo/include/Subscreen/FieldSubWindowProcManager.h"

// Debug
///#include "../include/Debug/FieldDebugMenu.h"
///#include "../include/Debug/FieldDebugTypes.h"

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>

// 配置モデル
///#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
///#include "Field/FieldRo/include/StaticModel/StaticModel.h"

//フィールドスクリプト
///#include "FieldScript/include/FieldScriptSystem.h"

// 天球
///#include "System/include/Skybox/Skybox.h"@todo_saito

// ゾーン
///#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

// エフェクト
///#include "Field/FieldRo/include/Effect/FieldEffectManager.h"

// エンカウント
///#include "Field/FieldRo/include/Encount/FieldEncount.h"

// conv_header
#include <niji_conv_header/field/field_resident/field_resident_pack.h>

//アクションボタン
///#include "Field/FieldRo/include/ActionButton/ActionButtonManager.h"

#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );
f32 g_CameraYaw( 0.0f );
GFL_NAMESPACE_END( Debug );


#define ENABLE_TERRAIN 1
// 配置情報で置けるようになるまでいったん封じます
#define ENABLE_STATIC_MODEL 1
// エフェクトシステム切り替え
#define ENABLE_EFFECT 0

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//@todo レイアウトレンダリング用のローカルヒープサイズ設定 AppRenderingManagerが要求　サイズは今は適当 150421 by iwasawa
const u32 Fieldmap::APP_RENDERING_MANAGER_HEAP_SIZE = 0x13000;//0x3000
const u32 Fieldmap::APP_RENDERING_MANAGER_HEAP_DEVICE_SIZE = 0xB0000;

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

/**------------------------------------------------------------------
 *      公開関数
 ------------------------------------------------------------------*/

/**------------------------------------------------------------------
 *      GameProc仮想関数
 ------------------------------------------------------------------*/
/**
* @brief コンストラクタ
*
* @return 無し
*/ 
Fieldmap::Fieldmap( /*const FieldmapDesc& desc*/ ) : 
m_Seq(0),
///m_Desc(desc),
m_ReadyFlag(false),
m_InitializeStateList(),
m_TerminateStateList(),
m_pFieldHeap(NULL),
m_pFieldDeviceHeap(NULL),
m_pFieldDeviceHeapAllocator(NULL),
m_pFieldGfglHeapOld(NULL),
m_pGameManager(NULL),
m_pFieldResident(NULL),
///m_pWorld(NULL),
///m_pArea(NULL),
///m_ZoneId(desc.playerLocation.zoneId),
///m_pPlacementDataManager(NULL),
///m_pEventCheck(NULL),
m_pDepthStencilTexture( NULL),
m_pOutLineResBuf( NULL ),
m_pEdgeMapResBuf( NULL ),
m_pRenderingPipeLine( NULL),
///m_pAppRenderingHeap( NULL),
///m_pAppRenderingManager( NULL),

// 地形システム
m_pTerrainManager( NULL ),
m_pCameraTarget( NULL ),
m_pCameraManager( NULL )
///m_pFieldWindowManager( NULL ),

// エフェクトシステム
///m_pEffectManager( NULL ),
///m_pEncount( NULL ),
#if PM_DEBUG
///m_pFieldDebugMenu( NULL )
#endif

// @todo 仮コード
///m_IsPokeFinder( false ),
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
Fieldmap::~Fieldmap( void )
{
}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
bool Fieldmap::Initialize( GameSystem::GameManager* pGameManager )
{
  enum
  {
    SEQ_START,
    SEQ_ENDWAIT,
  };
  switch(m_Seq){

  case SEQ_START:
    // ベースシステムの初期化
    {
      m_pGameManager = pGameManager;
      m_pFieldResident = pGameManager->GetFieldResident();

      // ヒープ作成
      InitializeHeap();

      // 初期化処理のリストを作成
      // 破棄処理のリストを作成
      InitializeState();

      // 次へ
      m_Seq++;
    }
    break;

  case SEQ_ENDWAIT:
    {
      gfl2::util::List<State>::Iterator iter = m_InitializeStateList.Begin();
      int end_count = 0;
      for( ; iter != m_InitializeStateList.End(); ++iter, ++end_count ){

        if( (*iter).IsFinish() ){
          continue;
        }

        if( !((*iter).Execute()) )
        {
          break;
        }
      }

      if( m_InitializeStateList.Size() == end_count ){
        m_Seq=0;
        return true; // 終了
      }
    }
    break;
  }

  return false;
}

/**
* @brief    PROCのメイン更新を行う関数
* @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
bool Fieldmap::Update( void )
{
  //******************************************
  // 終了チェック
	{
		GameSystem::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
		if (pGameManager->IsEndRequest())
    {
			m_CloseFlag = true;
		}
  }
  if( m_CloseFlag ){ return true; }
  //******************************************

  // Field3DObjectNodeのExecuteTerminate
  this->Field3DObjectNodeExecuteTerminate();

  // 通常更新処理
  this->UpdateNormal();

#if PM_DEBUG
  {
    GameSystem::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    if( pButton->IsTrigger(gfl2::ui::BUTTON_START) )
    {
      GFL_PRINT( "FieldHeap[0x%x][0x%x]\n",       m_pFieldHeap->GetTotalFreeSize(), m_pFieldHeap->GetTotalAllocatableSize() );
      GFL_PRINT( "FieldDeviceHeap[0x%x][0x%x]\n", m_pFieldDeviceHeap->GetTotalFreeSize(), m_pFieldDeviceHeap->GetTotalAllocatableSize() );
    }

  }
#endif // PM_DEBUG

  return false;
}

/**
* @brief  PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void Fieldmap::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  gfl2::math::Matrix44 projMat[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewMat[ gfl2::gfx::CtrDisplayNo::NumberOf ];

  gfl2::math::Matrix34 view; 
  gfl2::math::Matrix44 projection;
  m_pCameraManager->GetMainViewCamera()->GetViewMatrix( &view );
  m_pCameraManager->GetMainViewCamera()->GetProjectionMatrix( &projection );

#if defined(GF_PLATFORM_CTR)

  static f32 depthLevel = 4.0f;
  static f32 factor     = 1.0f;

  gfl2::gfx::GFGL::GetStereoMatrix( projection,
                                    view,
                                    depthLevel,
                                    factor, 
                                    &projMat[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &viewMat[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &projMat[gfl2::gfx::CtrDisplayNo::RIGHT],
                                    &viewMat[gfl2::gfx::CtrDisplayNo::RIGHT]);
#else

  projMat[gfl2::gfx::CtrDisplayNo::RIGHT] = projection;
  viewMat[gfl2::gfx::CtrDisplayNo::RIGHT] = view;
  projMat[gfl2::gfx::CtrDisplayNo::LEFT]  = projection;
  viewMat[gfl2::gfx::CtrDisplayNo::LEFT]  = view;

#endif

  //レイアウトパイプラインのアップデート @todo
  ///m_pAppRenderingManager->Update();

  ///if( m_pFieldWindowManager ) m_pFieldWindowManager->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( displayNo ) );

  switch( displayNo )
  {
    // 左画面描画
    case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      gfl2::gfx::GFGL::SetActiveDisplay( System::GflvUse::m_HandleList[ displayNo ] );

      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer        ( System::GflvUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflvUse::m_HandleList[ displayNo ] );
      u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth      ( System::GflvUse::m_HandleList[ displayNo ] );
      u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight     ( System::GflvUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetViewMatrix      ( viewMat[gfl2::gfx::CtrDisplayNo::LEFT] );
      m_pRenderingPipeLine->SetProjectionMatrix( projMat[gfl2::gfx::CtrDisplayNo::LEFT] );
      m_pRenderingPipeLine->SetRenderTarget    ( pBackBuffer  );
      m_pRenderingPipeLine->SetDepthStencil    ( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize     ( displayWidth, displayHeight );

      m_pRenderingPipeLine->Update();
      m_pRenderingPipeLine->Execute();

      //レイアウトパイプラインの描画 @todo
      ///m_pAppRenderingManager->Draw(displayNo);

    #if USE_RTTI_DEBUG
      gfl2::rttidebug::RttiEditor::getInstance()->Draw();
    #endif

      ///m_pSubWindow->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( displayNo ));
      break;
    }

    // 右画面描画(立体視時のみ)
    case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      ///m_pSubWindow->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( displayNo ));
      break;
    }

    // 下画面描画
    case gfl2::gfx::CtrDisplayNo::DOWN:
    {
      ///m_pSubWindow->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( displayNo ));
      break;
    }

  }

}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
bool Fieldmap::Terminate(void)
{
  enum
  {
    SEQ_ENDWAIT,
    SEQ_DELETE,
  };

  switch(m_Seq){

  case SEQ_ENDWAIT:
    {
      // Field3DObjectNodeのExecuteTerminate
      this->Field3DObjectNodeExecuteTerminate();

      gfl2::util::List<State>::Iterator iter = m_TerminateStateList.Begin();
      int end_count = 0;
      for( ; iter != m_TerminateStateList.End(); ++iter, ++end_count ){

        if( (*iter).IsFinish() ){
          continue;
        }

        if( !((*iter).Execute()) )
        {
          break;
        }
      }

      // 完了のチェック
      if( m_TerminateStateList.Size() == end_count ){
        m_Seq++;
      }
    }
    break;

  case SEQ_DELETE:
    {
			m_ResourceAllocator.SystemMemoryFree(m_pBinData);
      // ベースシステム破棄
      TerminateState();

      TerminateHeap();
    }
    m_Seq = 0;

#if PM_DEBUG
    // アプリケーションのVRAM領域がクリアかチェック
    System::nijiAllocator::DebugCheckApplicationVramClear();
#endif
    return true;
  }

  return false;
}
#if 0
/**
* @brief   イベントチェックの実行
*/
void Fieldmap::EventCheck()
{
  // イベントチェックメイン処理
  if(m_pEventCheck) { m_pEventCheck->EventCheck(); }
}
#endif //@saito_del 
/**
 *  @brief MoveModelManagerの取得
 */
MoveModel::FieldMoveModelManager* Fieldmap::GetMoveModelManager( void )
{
  return GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetGameData()->GetFieldCharaModelManager();
}

/**------------------------------------------------------------------
 *      private関数
 ------------------------------------------------------------------*/
/**
 * @brief ヒープ生成
 */
void Fieldmap::InitializeHeap( void )
{
  // フィールドプロック内でのヒープメモリリークを検知しやすいように専用の物を作成
  m_pFieldHeap = m_pFieldResident->GetFieldHeap();
  m_pFieldDeviceHeap = m_pFieldResident->GetFieldDeviceHeap();
  m_pFieldDeviceHeapAllocator = m_pFieldResident->GetFieldDeviceHeapAllocator();

  // フィールドプロック内でのVRAMリークを検知しやすいように専用の物を作成
  m_pFieldGfglHeapOld = gfl2::gfx::GFGL::GetAllocator();
  gfl2::gfx::GFGL::SetAllocator( m_pFieldDeviceHeapAllocator );

  GFL_PRINT( "FieldHeap 0x%x\n", m_pFieldHeap );
  GFL_PRINT( "FieldDeviceHeap 0x%x\n", m_pFieldDeviceHeap );
}

/**
 *  @brief ヒープ破棄
 */
void Fieldmap::TerminateHeap( void )
{
  gfl2::gfx::GFGL::SetAllocator( m_pFieldGfglHeapOld );
  m_pFieldDeviceHeapAllocator = NULL;
  m_pFieldHeap = NULL;
  m_pFieldDeviceHeap = NULL;
}


/**
 *  @brief ステートの初期化
 */
void Fieldmap::InitializeState( void )
{
  m_InitializeStateList.CreateBuffer( m_pFieldHeap, INITIALIZE_STATE_NUM );
  m_TerminateStateList.CreateBuffer( m_pFieldHeap, TERMINATE_STATE_NUM );

  // 初期化処理リスト
  m_InitializeStateList.PushBack( State(this, &Fieldmap::State_ManagerInitialize) );
  ///m_InitializeStateList.PushBack( State(this, &Fieldmap::State_ScriptInitialize) );
  m_InitializeStateList.PushBack( State(this, &Fieldmap::State_Ready) );

  // 破棄処理リスト
  m_TerminateStateList.PushBack( State(this, &Fieldmap::State_RenderSkip) );
  ///m_TerminateStateList.PushBack( State(this, &Fieldmap::State_ScriptTerminate) );
  m_TerminateStateList.PushBack( State(this, &Fieldmap::State_ManagerTerminate) );
}

/**
 *  @brief ステートの破棄
 */
void Fieldmap::TerminateState( void )
{
  m_InitializeStateList.Clear();
  m_TerminateStateList.Clear();
}

//! レンダリングパイプライン　生成・破棄
void Fieldmap::InitializeRenderingPipeLine(void)
{
	m_pEdgeMapResBuf = ReadData("R:/home/niji/tool/FieldTools/MapViewer/Renderer/EdgeMapResource.bin", &m_ResourceAllocator);
	m_pOutLineResBuf = ReadData("R:/home/niji/tool/FieldTools/MapViewer/Renderer/OutLineResource.bin", &m_ResourceAllocator);

	GFL_ASSERT((m_pEdgeMapResBuf!=NULL)&&(m_pOutLineResBuf!=NULL));
  ///m_pRenderingPipeLine = GFL_NEW( m_pFieldHeap ) MyRenderingPipeLine(512,256);
	m_pRenderingPipeLine = GFL_NEW( m_pFieldHeap ) MyRenderingPipeLine(
    m_pFieldDeviceHeap,
    m_pFieldDeviceHeapAllocator,
    m_pEdgeMapResBuf,
    m_pOutLineResBuf,
    512, 
    256
    );


  m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflvUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT] ));
  m_pRenderingPipeLine->SetDepthTexture( m_pDepthStencilTexture );
#if 0
  // ブルーム描画パスの初期化
  {
    poke_3d::renderer::BloomRenderPath::Description desc;
    {
      desc.sourceType                               = poke_3d::renderer::BloomRenderPath::SOURCE_TYPE_FRAME_BUFFER;
      desc.bloomType                                = poke_3d::renderer::BloomRenderPath::BLOOM_TYPE_GAUSSIAN_FILTER_3X3;
      desc.highLuminanceRenderTargetReciprocalScale = 2;
      desc.highLuminanceCopyNumMax                  = 1;
      desc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::LEFT ] = gfl2::gfx::GFGL::GetBackBuffer( System::GflvUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT ] );
      desc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::RIGHT] = gfl2::gfx::GFGL::GetBackBuffer( System::GflvUse::m_HandleList[gfl2::gfx::CtrDisplayNo::RIGHT] );
      desc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::DOWN ] = gfl2::gfx::GFGL::GetBackBuffer( System::GflvUse::m_HandleList[gfl2::gfx::CtrDisplayNo::DOWN ] );
    }

    poke_3d::renderer::BloomRenderPath* bloomRenderPath = m_pRenderingPipeLine->GetBloomRenderPath();
    bloomRenderPath->SyncCreate(m_pFieldDeviceHeap,
                                m_pFieldDeviceHeap->GetLowerHandle(),
                                m_pFieldDeviceHeapAllocator,
                                m_pFieldDeviceHeapAllocator,
                                GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetAsyncFileManager(),
                                desc);

    bloomRenderPath->SetStrength(1.0f);
    bloomRenderPath->SetIntensity(0.66f);
    bloomRenderPath->SetWeight( gfl2::math::Vector(0.2989f, 0.5866f, 0.1144f) );
  }
#endif //@saito_del todo ブルームはExecがないと無理
#if 0
  //@todo レイアウト描画用の設定。この設計のままで行くかは要精査
  m_pAppRenderingHeap = GFL_NEW( m_pFieldHeap ) app::util::Heap();
  m_pAppRenderingHeap->LocalHeapCreate( m_pFieldHeap, m_pFieldDeviceHeap,
    Fieldmap::APP_RENDERING_MANAGER_HEAP_SIZE, Fieldmap::APP_RENDERING_MANAGER_HEAP_DEVICE_SIZE );

  m_pAppRenderingManager = GFL_NEW( m_pFieldHeap ) app::util::AppRenderingManager();
  m_pAppRenderingManager->Init(m_pAppRenderingHeap);
#endif //@saito_del
}

void Fieldmap::TerminateRenderingPipeLine(void)
{
  ///m_pAppRenderingManager->End();
  ///GFL_SAFE_DELETE(m_pAppRenderingManager);
  ///m_pAppRenderingHeap->Delete();
  ///GFL_SAFE_DELETE(m_pAppRenderingHeap);

  GFL_SAFE_DELETE(m_pDepthStencilTexture);
  GFL_SAFE_DELETE(m_pRenderingPipeLine);

	///GflHeapSafeFreeMemory( m_pOutLineResBuf );
  ///GflHeapSafeFreeMemory( m_pEdgeMapResBuf );
	m_ResourceAllocator.SystemMemoryFree(m_pOutLineResBuf);
	m_ResourceAllocator.SystemMemoryFree(m_pEdgeMapResBuf);

}

/**
 *  @brief 更新処理
 */
void Fieldmap::UpdateNormal( void )
{
  m_pRenderingPipeLine->Update();

  // 動作モデルマネージャーを取得
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetGameData()->GetFieldCharaModelManager();//@todo_saito

	//アクションボタンチェック処理
	///m_pActionButtonManager->Update(m_pEventCheck->GetTalkTarget(), pFieldCharaModelManager, m_pEffectManager);

  // キャラクター 前更新(コリジョン以外の座標/姿勢の変更を行う)
  pFieldCharaModelManager->PrevUpdate();


  // @note 【デバッグRダッシュ処理】 Rボタンが押されている間は自機の壁コリジョンを行わない 150526 ikuta_junya
#if PM_DEBUG
  GameSystem::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsHold( gfl2::ui::BUTTON_R ) == false )
#endif
  {
    // 地面用コリジョンの更新
    // memo: 壁の隅での押し返しを安定させるために三回程度必要
    //       壁と地面のメッシュがきちんと別れれば2回でいけるはず
    u32 collisionUpdateCnt = 3;

    for( u32 i=0 ; i<collisionUpdateCnt ; i++ )
    {
      // キャラクター 更新(コリジョンシーンへの座標/姿勢の登録を行う)
      pFieldCharaModelManager->Update();
      // コリジョンの更新
      m_pCollsionSceneForTerrain->Update();
      // キャラクター 後更新(コリジョンシーンからの結果を適用し、最終的な座標/姿勢の決定/イベントの実行を行う)
      pFieldCharaModelManager->PostUpdate();
    }

    // 配置モデル用コリジョンの更新
    // memo: 壁の隅のようなケースがないので一回で十分なはず
    {
      // キャラクター 更新(コリジョンシーンへの座標/姿勢の登録を行う)
      pFieldCharaModelManager->Update();
      // コリジョンの更新
      m_pCollsionSceneForStaticModel->Update();
      // キャラクター 後更新(コリジョンシーンからの結果を適用し、最終的な座標/姿勢の決定/イベントの実行を行う)
      pFieldCharaModelManager->PostUpdate();
    }
  }

  // EnvManager更新
  {
    s32 hour = 12;
    s32 minute = 0;
    s32 second = 0;

    // @todo EnvManagerは仮対応

    gfl2::math::Vector3  controlPos = pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER)->GetCharaDrawInstance()->GetPosition();
    gfl2::math::Matrix44 projMat;
    m_pCameraManager->GetMainViewCamera()->GetProjectionMatrix(&projMat);

    m_pFieldEnvManager->Update( controlPos, hour, minute, second, projMat, m_pRenderingPipeLine );
  }

  // 地形制御点を更新
#if ENABLE_TERRAIN
  {
    MoveModel::FieldMoveModel* pMoveModel = pFieldCharaModelManager->GetFieldMoveModel( MoveModel::FIELD_MOVE_MODEL_PLAYER );

    if( pMoveModel 
     && pMoveModel->GetCharaDrawInstance() 
     && pMoveModel->GetCharaDrawInstance()->GetModelInstanceNode() )
    {
      gfl2::math::Vector3 pos = pMoveModel->GetCharaDrawInstance()->GetPosition();
      ///m_pTerrainManager->SetBlockControlPosition( pos );
    }
    else
    {
    }

    m_pTerrainManager->Update();
  }
#endif 

#if ENABLE_STATIC_MODEL
  ///m_pStaticModelManager->Update();
#endif

  ///if( m_pFieldWindowManager ) m_pFieldWindowManager->Update();

  // カメラ更新
  m_pCameraManager->Update();

///#ifdef TEMP_CODE_IKUTA_150423
   Field::Debug::g_CameraYaw = m_pCameraManager->GetMainViewCamera()->GetRotationY();
///#endif // #if TEMP_CODE_IKUTA_150423 //@saito_del

      // トラバース処理にてワールド座標を更新
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();

#if ENABLE_EFFECT
  {
    // エフェクト更新
    m_pEffectManager->Update();
  }
#endif

  //下画面更新
  ///m_pSubWindow->Update();

#if USE_RTTI_DEBUG
  
  // RTTIデバッガの描画オンオフ
  if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::R3 ) )
  {
    m_isDrawRttiEditor = m_isDrawRttiEditor ? false : true;
  }

  // 次のリクエストをエディタで表示する
  if     ( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::Right ) ){ gfl2::rttidebug::RttiEditor::getInstance()->NextTag(); }
  // 前のリクエストをエディタで表示する
  else if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::Left  ) ){ gfl2::rttidebug::RttiEditor::getInstance()->PrevTag(); }

  // 次のパラメータを選択する
  if     ( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::Down ) ){ gfl2::rttidebug::RttiEditor::getInstance()->NextParam(); }
  // 前のパラメータを選択する
  else if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::Up   ) ){ gfl2::rttidebug::RttiEditor::getInstance()->PrevParam(); }
  // 選択中のパラメータの値を増やす
  else if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::R2   ) ){ gfl2::rttidebug::RttiEditor::getInstance()->AddValue();  }
  // 選択中のパラメータの値を減らす
  else if( gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::L2   ) ){ gfl2::rttidebug::RttiEditor::getInstance()->PullValue(); }

  // RTTIデバッガの描画
  gfl2::rttidebug::RttiEditor::getInstance()->SetEnableDraw( m_isDrawRttiEditor );
  gfl2::rttidebug::RttiEditor::getInstance()->Update();

#endif

}

// @class インスタンス破棄処理のvisitor
class ExecuteTerminateVisitor
{
public:

  ExecuteTerminateVisitor(){}
  virtual ~ExecuteTerminateVisitor(){}

  void Visit( gfl2::renderingengine::scenegraph::DagNode* pNode )
  {
    //IField3DObjectNode * obj = static_cast<IField3DObjectNode*>(pNode);
    //obj->ExecuteTerminateRequest();

    IField3DObjectNode * obj = pNode->SafeCast<IField3DObjectNode>();
    //StaticModel::StaticModel * obj = pNode->SafeCast<StaticModel::StaticModel>();
    if( obj)
    {
      obj->ExecuteTerminateRequest();
    }
  }
private:
};
/**
 *  @brief 3DModelInstanceのインスタンス破棄処理
 */
void Fieldmap::Field3DObjectNodeExecuteTerminate()
{
  gfl2::renderingengine::scenegraph::Traverser::Visitor<IField3DObjectNode, ExecuteTerminateVisitor> visitor;
  gfl2::renderingengine::scenegraph::Traverser::TraverseFast<IField3DObjectNode, ExecuteTerminateVisitor>( GetFieldRootNode()->GetInstanceRootNode(), &visitor );
}

/**------------------------------------------------------------------
 *      State関数
 ------------------------------------------------------------------*/
/**
 *  @brief  マネージャー関連の初期化
 *  @param  p_seq
 */
bool Fieldmap::State_ManagerInitialize( int* p_seq )
{
  enum
  {
    INITIALIZE_AREA,
    INITIALIZE,
    INITIALIZE_WAIT,
  };

  switch( *p_seq )
  {
  case INITIALIZE_AREA:
    {
      // ワールド取得
      ///m_pWorld = m_pGameManager->GetGameData()->GetFieldWorld();
      // エリア取得
      ///m_pArea = m_pGameManager->GetGameData()->GetFieldArea();
      (*p_seq)++;
      /* FALL THROUGH */

    } // case INITIALIZE_AREA

  case INITIALIZE:
    {
			m_ResourceAllocator.init(m_pFieldDeviceHeap);
      // レンダリングパイプラインの初期化
      InitializeRenderingPipeLine();

      // アトリビュートパラメーター初期化
      {
        void* pAttrParameterTableData = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__ATTRIBUTE_PARAMTER_TABLE_BIN);
        ///Attribute::Initialize( pAttrParameterTableData);
      }

#if USE_RTTI_DEBUG
      // RTTIデバッガの初期化
      {
        // RTTIデバッグの描画に必要なフォントの生成
        m_pResFont = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", m_pFieldHeap, m_pFieldDeviceHeap, m_pFieldHeap );

        // RTTIデバッグの描画に必要なDrawUtilText要アロケータを作成
        m_pMemAllocator = GFL_NEW(m_pFieldHeap)       gfl2::heap::NwAllocator( m_pFieldHeap );
        m_pDevAllocator = GFL_NEW(m_pFieldDeviceHeap) gfl2::heap::NwAllocator( m_pFieldDeviceHeap );

        // RTTIデバッグの描画に必要なDrawUtilTextの生成
        m_pDrawUtilText = GFL_NEW(m_pFieldHeap) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator);
        m_pDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
        m_pDrawUtilText->SetFont( m_pResFont );

        // Rttiデバッグの初期化
        gfl2::rttidebug::RttiEditor::getInstance()->Initialize      ( m_pFieldDeviceHeap , 1024*256 , m_pDrawUtilText );
        gfl2::rttidebug::RttiParamManager::getInstance()->Initialize( m_pFieldDeviceHeap , 1024*256 );
        m_isDrawRttiEditor = false;
      }
#endif

      // コリジョンシステムの作成
      {
        // 地面用コリジョンシーンの作成
        m_pCollsionSceneForTerrain = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TERRAIN );

        // 配置モデル用コリジョンシーンの作成
        m_pCollsionSceneForStaticModel = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_STATIC_MODEL );
      }
#if 0
      // イベントチェック初期化
      {
        FieldEventCheck::Desc desc = 
        {
          FieldEventCheck::CHECKMODE_NORMAL, 
          m_pGameManager, 
          this,
          m_pFieldHeap,
          m_pFieldDeviceHeapAllocator,
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_POS ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT )
        };
        m_pEventCheck = GFL_NEW(m_pFieldHeap) FieldEventCheck();
        m_pEventCheck->Initialize( desc ); 
      }
#endif //@saito_del
      // 地形システムを生成
      {
#if ENABLE_TERRAIN
        Terrain::TerrainManager::InitializeDescription initDesc;
        initDesc.m_pFileManager = GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetAsyncFileManager(),
        initDesc.pTerrainLayoutData = NULL; // @todo 
        initDesc.m_pDeviceHeap = m_pFieldDeviceHeap;
        initDesc.m_pSystemHeap = m_pFieldHeap;
        initDesc.pAreaResourceDataAccessor = NULL;//m_pArea->GetAccessor( Area::AREA_DATA_IDX_TERRAIN_RESOURCE );@satio_change
        initDesc.m_pDeviceAllocator = m_pFieldDeviceHeapAllocator;
        initDesc.m_pWallCollsionScene = m_pCollsionSceneForTerrain;
        initDesc.m_pEnvManager = m_pFieldEnvManager;
        initDesc.m_pRenderingPipeLine = m_pRenderingPipeLine;
        ///initDesc.m_StaticCullingListenerMax = 128; // @todo 大体の値、最適化する
        ///initDesc.m_StaticLodListenerMax = 128; // @todo 大体の値、最適化する
        
				/**
        // @todo 仮コード
        {
          // 配置情報生成
          {
            Terrain::TerrainLayoutData* pLayoutData = GFL_NEW( m_pFieldHeap ) Terrain::TerrainLayoutData();
            pLayoutData->Create( m_pArea->GetAccessor( Area::AREA_DATA_IDX_TERRAIN_LAYOUT_RESOURCE)->GetData( 0 ), m_pFieldHeap );
            //pLayoutData->Test();
            initDesc.pTerrainLayoutData = pLayoutData;
          }
        }
				*///@saito_del
        m_pTerrainManager = GFL_NEW( m_pFieldHeap ) Terrain::TerrainManager();
        m_pTerrainManager->Initialize( initDesc );
#endif
      }

      /// UIViewManager よりも後に作られなくてはならない.
      ///m_pFieldWindowManager = GFL_NEW(m_pFieldHeap) Field::FieldWindow::FieldWindowManager( m_pAppRenderingHeap, m_pAppRenderingManager, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
      ///GFL_ASSERT( m_pFieldWindowManager );

      // 動作モデル関連のマネージャーを取得
      Field::MoveModel::FieldMoveModelManager*      pFieldCharaModelManager    = GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetGameData()->GetFieldCharaModelManager();
      ///Field::MoveModel::FieldMoveCodeManager*       pFieldMoveCodeManager      = GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetGameData()->GetFieldMoveCodeManager();
      ///Field::MoveModel::FieldActionCommandManager*  pFieldActionCommandManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldActionComandManager();

      // 動作モデルマネージャーの初期化
      pFieldCharaModelManager->InitializeResource(m_pRenderingPipeLine,
                                                  m_pFieldDeviceHeapAllocator,
                                                  m_pFieldDeviceHeap,
                                                  m_pGameManager->GetAsyncFileManager() );
#if 0
#if ENABLE_STATIC_MODEL
      // 配置モデルの初期化
      {
        PlacementDataStaticModelAccessor accessor;
        void* pPlacementData = m_pFieldResident->GetPlacementDataManager()->GetZonePlacementData( m_ZoneId, PlacementDataManager::PACK_IDX_STATICMODEL);
        u32 modelMax = 0;
        if( pPlacementData)
        {
          accessor.Initialize( pPlacementData);
          modelMax = accessor.GetDataMax();
        }

        m_pStaticModelManager = m_pFieldResident->GetStaticModelManager();

        StaticModel::StaticModelManager::InitializeDataAccessor initDataAccessor;
        initDataAccessor.pHeap                  = m_pFieldDeviceHeap;
        initDataAccessor.pResourcePackBinLinker = m_pArea->GetAccessor( Area::AREA_DATA_IDX_STATICMODEL_RESOURCE);
        initDataAccessor.pRenderingPipeLine     = m_pRenderingPipeLine;
        initDataAccessor.pFieldRootNode         = this->GetFieldRootNode();
        initDataAccessor.modelMax               = modelMax;

        m_pStaticModelManager->InitializeFirst( initDataAccessor);

        // 地形LOD通知リスナーの登録
        Field::StaticModel::StaticModel::Handle_RegistLODChangeListener handle( m_pTerrainManager);
        u32 staticModelNum = m_pStaticModelManager->GetStaticModelNum();
        for( u32 i = 0; i < staticModelNum; ++i)
        {
          m_pStaticModelManager->GetStaticModel( i)->Set( &handle);
        }
      }
#endif

      // 天球生成開始
      {
        m_pSkybox = m_pGameManager->GetSkybox();
        System::Skybox::Skybox::Description desc;
        desc.rotateRadY = 0.0f;
        m_pSkybox->StartCreateResource( desc);
      }
#endif //@saito_del
#if PM_DEBUG
      ///m_pFieldDebugMenu = GFL_NEW( m_pFieldDeviceHeap ) Field::Debug::FieldDebugMenu( m_pFieldHeap, pFieldCharaModelManager );
#endif

      // カメラ生成
      {
        m_pCameraManager = GFL_NEW( m_pFieldHeap ) Camera::CameraManager;

        Camera::CameraManager::InitializeDescription desc;
        desc.m_pSystemHeap = m_pFieldHeap;
        desc.m_pDeviceHeap = m_pFieldDeviceHeap;
        desc.m_pDeviceAllocator = m_pFieldDeviceHeapAllocator;
        desc.m_pTarget = NULL;
        desc.m_pCameraAreaDataAccessor = NULL;//m_pArea->GetAccessor( Area::AREA_DATA_IDX_CAMERA_AREA_DATA );@saito_change
 
        m_pCameraManager->Initialize( desc );
      }
#if 0
      // エンカウント生成
      {
        m_pEncount = GFL_NEW( m_pFieldHeap) Encount::Encount( this, m_pArea->GetAccessor( Area::AREA_DATA_IDX_ENCOUNT_DATA) );
      }
			//アクションボタンマネージャ生成
			{
				m_pActionButtonManager = GFL_NEW( m_pFieldHeap) ActionButton::ActionButtonManager;
				m_pActionButtonManager->Initialize(m_pFieldHeap, GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager() );
			}

      {//@todoメモリもらうまではアプリにいれておく
        gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
        gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
				m_pSubWindow = GFL_NEW(devHeap) ::Field::SubScreen::FieldSubWindowProcManager(systemHeap,devHeap);
        m_pSubWindow->Initialize();
      }
#endif //@saito_del
      //===========================================================================================
      // todo: システムの確認用コード : ここから
      //===========================================================================================
			
			// プレイヤーの作成
      {
#if 0
        // キャラクターリソースの非同期ロード
        // @todo エリアキャラクターリソース読み込み　仮対応
        static u32 loadCharacterID[] = 
        {
          CHARA_MODEL_ID_TR0001_00,
          CHARA_MODEL_ID_TR0002_00,
          CHARA_MODEL_ID_PC0002_00,
        };
        for( u32 i=0; i<GFL_NELEMS(loadCharacterID); ++i ){
          pFieldCharaModelManager->LoadAsync(loadCharacterID[i]);

          // 非同期読み込み待ち
          while( !pFieldCharaModelManager->IsLoaded(loadCharacterID[i]) ) 
          {
            s64 milliseconds = 100;
          #if defined(GF_PLATFORM_WIN32)
            Sleep( milliseconds );
          #elif defined(GF_PLATFORM_CTR)
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( milliseconds ) );
          #endif
          }

          // キャラクターリソースノードの初期化
          pFieldCharaModelManager->SetupModel(loadCharacterID[i]);
        }
#endif //@saito_del

#if 0
        // 着せ替えパーツリソースの非同期ロード
        pFieldCharaModelManager->LoadDressUpPartsAsync(m_DressUpParam);

        while( !pFieldCharaModelManager->IsDressUpPartsLoaded(m_DressUpParam) )
        {
          s64 milliseconds = 100;
          #if defined(GF_PLATFORM_WIN32)
            Sleep( milliseconds );
          #elif defined(GF_PLATFORM_CTR)
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( milliseconds ) );
          #endif
        }

        // 着せ替えパーツリソースノードの初期化
        pFieldCharaModelManager->SetupDressUpParts(&m_DressUpParam);
#endif

        // プレイヤーの作成
        Field::MoveModel::FieldMoveModelHeader header;
        header.eventId                         = 0xffffffff;///FIELD_EVENTID_PLAYER;@saito_change
        header.characterId                     = CHARA_MODEL_ID_PC0002_00;
        header.pDressUpParam                   = NULL;
        header.moveCodeId                      = Field::MoveModel::FIELD_MOVE_CODE_PLAYER;
        header.pCollisionSceneForTerrain       = m_pCollsionSceneForTerrain;
        header.pCollisionSceneForStaticModel   = m_pCollsionSceneForStaticModel;
        header.pCollisionSceneForEventPos      = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_POS );
        header.pCollisionSceneForEventTalk     = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK );
        header.pCollisionSceneForEventPush     = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH );
        header.pCollisionSceneForEncount       = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT );
        pFieldCharaModelManager->CreateWork    ( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER , &header );
        pFieldCharaModelManager->CreateResource( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER , &header );
        Field::MoveModel::FieldMoveModel* pCharaModel = pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );

        // プレイヤーが完全に初期化された場合
        ///if( m_Desc.openMode == FM_OPEN_MODE_INIT )@saito_del
        {
          pCharaModel->GetCharaDrawInstance()->SetPosition(/*m_Desc.playerLocation.position*/0,0,0);//@saito_change 
          pCharaModel->GetCharaDrawInstance()->SetRotation(0.0f, 0.0f, 0.0f);
          pCharaModel->GetCharaDrawInstance()->SetScale   (1.0f, 1.0f, 1.0f);
        }

				const f32 PLAYER_HALF_WIDTH =  37.5f;
        const f32 PLAYER_HEIGHT     = 150.0f;


        // 動作モデルのコリジョン(地面との衝突用)を作成
        pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TERRAIN)->CreateSphere( gfl2::math::Vector3(0.0f,100.0f,0.0f), PLAYER_HALF_WIDTH  );
        m_pCollsionSceneForTerrain->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TERRAIN));

        // 動作モデルのコリジョン(配置モデルとの衝突用)を作成
        pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_STATIC_MODEL)->CreateCylinder( gfl2::math::Vector3(0.0f,100.0f,0.0f), PLAYER_HALF_WIDTH, PLAYER_HEIGHT );
        m_pCollsionSceneForStaticModel->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_STATIC_MODEL));

        // 動作モデルのコリジョン(イベント判定用)を作成
        pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_POS)->CreateCylinder( gfl2::math::Vector3(0.0f,100.0f,0.0f), PLAYER_HALF_WIDTH, PLAYER_HEIGHT );
        m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_POS )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_POS));

        pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_TALK)->CreateCylinder( gfl2::math::Vector3(0.0f,100.0f,0.0f), PLAYER_HALF_WIDTH, PLAYER_HEIGHT );
        m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_TALK));

        pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_PUSH)->CreateCylinder( gfl2::math::Vector3(0.0f,100.0f,0.0f), PLAYER_HALF_WIDTH, PLAYER_HEIGHT );
        m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_PUSH));

        pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_ENCOUNT)->CreateCylinder( gfl2::math::Vector3(0.0f,100.0f,0.0f), PLAYER_HALF_WIDTH, PLAYER_HEIGHT );
        m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_ENCOUNT));

        // 地形コリジョンを設定
        pCharaModel->SetCollisionSceneContainer( m_pTerrainManager->GetCollsionScene() );


#if USE_RTTI_DEBUG
        // RTTIエディタにメンバ情報を表示するリクエストを出す
        pFieldCharaModelManager->CreateRttiEditorPage( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
#endif
      }

      // プレイヤーをカメラシステムにターゲットとして登録 @todo 仮仕様
      {
        Field::MoveModel::FieldMoveModel* pCharaModel = pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
        m_pCameraTarget = GFL_NEW( m_pFieldHeap ) Camera::Target::TargetTypeMoveModel( pCharaModel );

        m_pCameraManager->ChangeCameraTarget( m_pCameraTarget );
				m_pCameraManager->SetUpTest();
				///m_pCameraManager->SetUpForNormalMap();
      }
#if 0
      // 配置テスト
      {
        m_pPlacementDataManager = m_pFieldResident->GetPlacementDataManager();
        m_pPlacementDataManager->CreateZoneModelResource( m_ZoneId );
        m_pPlacementDataManager->CreateZonePos( m_ZoneId );
        m_pPlacementDataManager->CreateZoneActionEvent( m_ZoneId );
        m_pPlacementDataManager->CreateZoneMapJump( m_ZoneId );
        m_pPlacementDataManager->CreateZoneStaticModel( m_ZoneId );
        m_pPlacementDataManager->CreateZoneBgEvent( m_ZoneId );
        m_pPlacementDataManager->CreateZoneEncount( m_ZoneId );
      }
#endif //@saito_del
      // エフェクトシステム初期化
#if ENABLE_EFFECT
      {
        m_pEffectManager = GFL_NEW( m_pFieldHeap ) Effect::EffectManager();
        gfl2::fs::BinLinkerAccessor *pAccessor = m_pArea->GetAccessor( Area::AREA_DATA_IDX_EFFECT_RESOURCE );
        m_pEffectManager->Initialize( m_pFieldHeap, m_pRenderingPipeLine, pAccessor );
      }
#endif

      //===========================================================================================
      // todo: システムの確認用コード : ここまで
      //===========================================================================================

      (*p_seq)++;
      break;
    } // case INITIALIZE

  case INITIALIZE_WAIT:
		/**
    m_pSkybox->UpdateCreateResource();


    if( !m_pSkybox->IsCreateResource())
    {
      break;
    }

    // 天球パーツ生成、初期化
    m_pSkybox->CreateParts();
    m_pSkybox->Initialize( System::Skybox::Skybox::SKY_TYPE_SUNNY);
		*///@saito_del

    // EnvManager初期化
    {
#if 0
      void* pZoneData = m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_ZoneId);
      ZoneDataAccessor zoneDataAccessor( pZoneData);
      u8 envPackIndex = zoneDataAccessor.GetEnvironmentIndex();
      void* pEnvPack = m_pArea->GetAccessor( Area::AREA_DATA_IDX_ENVIRONMENT_RESOURCE)->GetData( envPackIndex);
#endif//@saito_del
			
			void* pEnvPack = ReadData( "R:/home/niji/tool/FieldTools/MapViewer/Env/DrawEnv1.pack", &m_ResourceAllocator );
      m_pFieldEnvManager = GFL_NEW( m_pFieldHeap) Field::EnvManager();
      m_pFieldEnvManager->Initialize( m_pFieldDeviceHeap, m_pFieldDeviceHeapAllocator, m_pRenderingPipeLine , pEnvPack/*, m_pSkybox*/);//@todo_saito
      gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode = m_pFieldEnvManager->GetDrawEnvNode();
      m_pRenderingPipeLine->AddDrawEnv( pNode);
			m_pBinData = pEnvPack;
    }

		/**
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
			
		//@saito_add
	  }
		*/

    {
      return true;
      // break;
    }
  }

  return false;
}
#if 0
/**
 *  @brief  フィールドスクリプト関連の初期化
 *  @param  p_seq
 */
bool Fieldmap::State_ScriptInitialize( int* p_seq )
{
  enum
  {
    INITIALIZE,
    INITIALIZE_WAIT,
  };
  Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();

  switch( *p_seq )
  {
  case INITIALIZE:
    {
      pFieldScriptSystem->Create( m_pFieldHeap, m_pGameManager->GetAsyncFileManager());  
      (*p_seq) ++;
    }
    return false;
  case INITIALIZE_WAIT:
    {
      pFieldScriptSystem->UpdateCreate();
      if( !pFieldScriptSystem->IsCreated() ) return false;

      pFieldScriptSystem->CallFieldInit( m_pArea->GetAccessor( Area::AREA_DATA_IDX_INIT_SCRIPT_RESOURCE ),
        m_pWorld->GetZoneIndexInArea(static_cast<ZoneID>(m_Desc.playerLocation.zoneId)), m_pFieldHeap );
//      pFieldScriptSystem->CallFieldRecover( m_pArea->GetAccessor( Area::AREA_DATA_IDX_INIT_SCRIPT_RESOURCE ),
//        m_pWorld->GetZoneIndexInArea(static_cast<ZoneID>(m_Desc.playerLocation.zoneId)), m_pFieldHeap );
    }
    break;
  }
  return true;
}
#endif //@saito_del
/**
 *  @brief  画面表示が可能な状態になるのを待つ
 *  @param  p_seq
 */
bool Fieldmap::State_Ready( int* p_seq )
{
  m_ReadyFlag = true;
  return true;
}

/**
 *  @brief  レンダリングしないフレームを1フレーム作る
 *  @param  p_seq
 */
bool Fieldmap::State_RenderSkip( int* p_seq )
{
  switch(*p_seq)
  {
  case 0:
    {
#if ENABLE_STATIC_MODEL
      // インスタンス破棄のリクエスト発行
      ///m_pStaticModelManager->RequestDelayTerminate();
#endif
    }
    (*p_seq) ++;
    break;

  case 1:
    return true;
  }

  return false;
}

/**
 *  @brief  マネージャーを破棄する。
 *  @param  p_seq
 */
bool Fieldmap::State_ManagerTerminate( int* p_seq )
{
#if 0
  //サブウインドウ破棄
  if(false == m_pSubWindow->Finalize()){
    return false;
  }
	GFL_SAFE_DELETE( m_pSubWindow );
#endif //@saito_del
  //===========================================================================================
  // todo: システムの確認用コード : ここから
  //===========================================================================================

  // EnvManagerでできるようになったのでいったんコメントアウト
#if 0
  GFL_SAFE_DELETE( m_pDrawEnvNode );
#endif

  //===========================================================================================
  // todo: システムの確認用コード : ここまで
  //===========================================================================================
#if ENABLE_EFFECT
  // エフェクトシステム破棄
  {
    m_pEffectManager->Terminate();
    GFL_SAFE_DELETE( m_pEffectManager );
  }
#endif
  
	//アクションボタンマネージャの破棄
	///GFL_SAFE_DELETE( m_pActionButtonManager );

  // カメラターゲット破棄
  GFL_SAFE_DELETE( m_pCameraTarget );

  // エンカウント破棄
  ///GFL_SAFE_DELETE( m_pEncount );

  // カメラシステム破棄
  GFL_SAFE_DELETE( m_pCameraManager );
	/**
  // 配置情報破棄
  m_pPlacementDataManager->DeleteZoneBgEvent( m_ZoneId );
  m_pPlacementDataManager->DeleteZoneStaticModel( m_ZoneId );
  m_pPlacementDataManager->DeleteZoneActionEvent();
  m_pPlacementDataManager->DeleteZoneMapJump( m_ZoneId );
  m_pPlacementDataManager->DeleteZonePos( m_ZoneId );
  //m_pPlacementDataManager->DeleteZoneModel( m_ZoneId ); // 退避処理は、GetFieldCharaModelManager()->TerminateResource();で行なわれる。
	*///@saito_del
  // EnvManager破棄
  m_pFieldEnvManager->Terminate();
  GFL_SAFE_DELETE( m_pFieldEnvManager);

  // @todo 破棄方法を選択する(リソースを残すか、全破棄か)
  ///m_pSkybox->DestroyAll();

  // 配置モデルの破棄
#if ENABLE_STATIC_MODEL
  // 地形LOD通知リスナーの登録削除
#if 0
	Field::StaticModel::StaticModel::Handle_UnRegistLODChangeListener handle( m_pTerrainManager);
  u32 staticModelNum = m_pStaticModelManager->GetStaticModelNum();
  for( u32 i = 0; i < staticModelNum; ++i)
  {
    m_pStaticModelManager->GetStaticModel( i)->Set( &handle);
  }
  m_pStaticModelManager->Finalize();
#endif //@saito_del
#endif

#if USE_RTTI_DEBUG 
  // RTTIエディタに非表示リクエストを出す 
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->DeleteRttiEditorPage( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ); 
#endif
	{
		Field::MoveModel::FieldMoveModelManager*      pFieldCharaModelManager    = GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetGameData()->GetFieldCharaModelManager();
		pFieldCharaModelManager->Delete(Field::MoveModel::FIELD_MOVE_MODEL_PLAYER);
		// キャラクターシステムの破棄
    ///GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetGameData()->GetFieldCharaModelManager()->TerminateResource();
		pFieldCharaModelManager->TerminateResource();
		}
  // 地形システムの破棄
#if ENABLE_TERRAIN
  GFL_SAFE_DELETE( m_pTerrainManager );
#endif  

  // イベントチェック処理破棄    
  ///GFL_DELETE m_pEventCheck;

  ///GFL_SAFE_DELETE( m_pFieldWindowManager );

  // レンダリングパイプラインの破棄
  TerminateRenderingPipeLine();

  // エリアリソース破棄
  ///m_pArea->Terminate();

  // RTTIデバッグデバッグの破棄
#if USE_RTTI_DEBUG
  m_pDrawUtilText->Terminate();
  GFL_SAFE_DELETE( m_pDrawUtilText );
  gfl2::str::DeleteResFont( m_pResFont);
  GFL_SAFE_DELETE( m_pMemAllocator );
  GFL_SAFE_DELETE( m_pDevAllocator );
  gfl2::rttidebug::RttiEditor::getInstance()->Terminate();
  gfl2::rttidebug::RttiParamManager::getInstance()->Terminate();
#endif

#if PM_DEBUG
  ///GFL_SAFE_DELETE( m_pFieldDebugMenu );
#endif

  return true;
}

#if 0
/**
 *  @brief  フィールドスクリプト関連の破棄
 *  @param  p_seq
 */
bool Fieldmap::State_ScriptTerminate( int* p_seq )
{
  {
    Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();
    pFieldScriptSystem->Delete();  
  }
  return true;
}
#endif //@saito_del

GFL_NAMESPACE_END( Field );
