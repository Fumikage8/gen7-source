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
#include <Thread/include/gfl2_ThreadStatic.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_heap_manager.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <gfx/include/gfl2_types.h>
#include <arc_def_index/arc_def.h>
#include <system\include\Date\\gfl2_Date.h>
#include <model/include/gfl2_DressUpModel.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>

#include <debug/include/gfl2_DebugTick.h>

// niji
#include "System/include/GflUse.h"              // for DisplayDeviceHandle
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/TimeZone.h"
#include "System/include/RenderPath/EdgeMapRenderPath.h"

// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h" 
#include "Field/FieldRo/include/FieldRenderingPipeLineManager.h" 
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "Field/FieldRo/include/FieldEnvManager.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldRo/include/EnvSound/EnvSoundManager.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"
#include "Field/FieldRo/include/DropItem/FieldDropItemCreator.h"
#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"
#include "Field/FieldStatic/include/Sodateya/FieldSodateya.h"
#include "Field/FieldRo/include/Skybox/FieldSkyboxDataManager.h"

// Terrain
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainBgPartsManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainTypes.h"

// Camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/FieldCameraUiDeviceTranslator.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetTypeMoveModel.h"
#if PM_DEBUG
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaCollisionManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"
#include "Field/FieldDebug/include/Menu/FieldDebugMenuAttrDispEditor.h"
#endif  //#if PM_DEBUG
// Camera ( poke_3d_lib )
#include <model/include/gfl2_BaseCamera.h>

#include "Field/FieldRo/include/Dowsing/FieldDowsingTool.h"


// キャラクターシステム
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelShadowManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelAreaCharaAccessor.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

// フィールドレジデント
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

// フィールドサウンド
#include "Field/FieldStatic/include/Sound/FieldSound.h"

//EvTime
#include "Field/FieldStatic/include/Time/EvTime.h"


// コリジョンシステム
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/CollisionSceneDefault.h"
#include "Field/FieldRo/include/Collision/CollisionSceneNone.h"

//下画面
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"
#include "Field/FieldStatic/include/Subscreen/FieldSubscreen.h"
#include "Field/FieldStatic/include/Event/EventXMenu.h"

// Debug
#include "../include/Debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "../include/Debug/FieldDebugDraw.h"

// 配置モデル
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"

//フィールドスクリプト
#include "FieldScript/include/FieldScriptSystem.h"

// 天球
#include "System/include/Skybox/Skybox.h"

// ゾーン
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include <niji_conv_header/field/zone/zone_id.h> // SubScreenの起動方法

// エフェクト
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

// 往来
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"

// エンカウント
#include "Field/FieldRo/include/Encount/FieldEncount.h"

// conv_header
#include <niji_conv_header/field/field_resident/field_resident_pack.h>
#include <niji_conv_header/field/chara/chara_model_id.h>
#include "niji_conv_header/field/FieldGimmickTypes.h"

//アクションボタン
#include "Field/FieldRo/include/ActionButton/ActionButtonManager.h"

// TrialModel
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"

//天候
#include "Field/FieldRo/include/Weather/WeatherManager.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"

// その他
#include "arc_index/NijiRenderPath.gaix"
#include "AppLib/include/Tool/TalkWindow.h"
#include "Savedata/include/SaveData.h"
#include <niji_conv_header/sound/SoundDefine.h>

// ショップ
#include "App/AppEvent/include/Shop/ShopBuyEvent.h"

// プレイヤー陰影
#include "Field/FieldRo/include/Shade/FieldPlayerShade.h"

// バトルフェスツール
#include "Field/FieldStatic/include/BattleFes/BattleFesTool.h"

//エンディング用スタッフリスト
#include "App/StaffList/include/FieldStaffList.h"

#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTool.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugHeapLog.h"
#include "Field/FieldRo/include/Debug/FieldDebugInfo.h"
#include "Sound/include/PMSoundSystem.h"
#define DEBUG_DRAW_TICK_TOMOYA (0)

#else

#define DEBUG_DRAW_TICK_TOMOYA (0)

#endif 

GFL_NAMESPACE_BEGIN( Field );

#define ENABLE_TERRAIN 1
// 配置情報で置けるようになるまでいったん封じます
#define ENABLE_STATIC_MODEL 1
// エフェクトシステム切り替え
#define ENABLE_EFFECT 1


//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

const u32 Fieldmap::APP_RENDERING_MANAGER_HEAP_SIZE = 0x800;
const u32 Fieldmap::APP_RENDERING_MANAGER_HEAP_DEVICE_SIZE = 0x1000;
static const u32 TERRAIN_LISTENER_MAX( 128 );



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
Fieldmap::Fieldmap( const FieldmapDesc& desc ) : 
m_Seq(0),
m_Desc(desc),
m_ReadyFlag(false),
m_InitializeStateList(),
m_TerminateStateList(),
m_pFieldHeap(NULL),
m_pFieldDeviceHeap(NULL),
m_pFieldDeviceHeapAllocator(NULL),
m_pGameManager(NULL),
m_pFieldResident(NULL),
m_pWorld(NULL),
m_pArea(NULL),
m_ZoneId(desc.playerLocation.zoneId),
m_reqZoneId(0),
m_seqZoneChange(0),
m_pPlacementDataManager(NULL),
m_pEventCheck(NULL),
m_pFieldRenderingPipeLineManager( NULL),
m_pRenderingPipeLine( NULL),
m_pAppRenderingHeap( NULL),
m_pAppRenderingManager( NULL),
m_playerPosition(0.0f,0.0f,0.0f),
m_playerQuaternion(0.0f,0.0f,0.0f,1.0f),
m_pFieldMoveModelShadowManger(NULL)
// 地形システム
,m_pTerrainManager( NULL )
,m_pCameraTarget( NULL )
,m_pCameraManager( NULL )
,m_pCameraUiDeviceTranslator( NULL )
,m_pDowsingTool( NULL )
,m_pFieldWindowManager( NULL )

// エフェクトシステム
,m_pEffectHeap( NULL )
,m_pEffectSystem( NULL )
,m_pEffectManager( NULL )
,m_pEncount( NULL )
,m_pTrialModelFactory( NULL )
,m_pDropItemCreator( NULL )
,m_isSkyboxKeep( false)
#if PM_DEBUG
, m_pDebugInfoWin(NULL)
, m_pDebugDllModule(NULL)
, m_pFieldDebugMenu( NULL )
#endif
,m_pShopBuyEvent(NULL)
,m_pFieldPlayerShade( NULL )
,m_pEffectCommonModule( NULL )
,m_pEffectUniqueModule( NULL )
,m_pEffectJoinFestaModule( NULL )
,m_pFieldStaffList(NULL)
,m_pStaffrollModule(NULL)
{
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    m_pGimmick[ i ] = NULL;
  }
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
bool Fieldmap::Initialize( GameSys::GameManager* pGameManager )
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

#if PM_DEBUG
      // Debug用Roモジュール読み込み
      InitializeDebugDll();
#endif

      // 次へ
      m_Seq++;
    }
    //break; through

  case SEQ_ENDWAIT:
    {
      gfl2::util::List<State>::Iterator iter = m_InitializeStateList.Begin();
      u32 end_count = 0;
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

#if FIELD_CTR_PROFILING_INIT
        FIELD_CTR_PROFILING_END
#endif

#if defined(GF_PLATFORM_CTR)
        // CPUアクセスの優先度を高める
        nn::gx::CTR::SetMemAccessPrioMode( nn::gx::CTR::MEM_ACCESS_PRIO_MODE_2 );
#endif


#if PM_DEBUG
        Sound::GetPMSoundSystem()->DumpHeapInfo();
#endif //PM_DEBUG
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
  if( m_CloseFlag ){ return true; }
  //******************************************

#if PM_DEBUG
  // DrawSkipモードを切り替えるデバッグ処理
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::GM_SKIP_DRAWMODE_SET ) < GameSys::GameManager::SKIP_DRAWMODE_MAX )
  {
    if( m_pGameManager->GetIsSkipDraw() == false )
    {
      m_pGameManager->SetEnableSkipDrawMode( static_cast<GameSys::GameManager::SkipDrawMode>(FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::GM_SKIP_DRAWMODE_SET )) );
    }
  }
#endif

#if FIELD_CTR_PROFILING_UPDATE
  {
    static u32 PROFILING_UPDATE_STATE = 0;

    GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    if     ( PROFILING_UPDATE_STATE == 0 && pButton->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      PROFILING_UPDATE_STATE++;
    }
    else if( PROFILING_UPDATE_STATE == 1 )
    {
      FIELD_CTR_PROFILING_START;
      PROFILING_UPDATE_STATE++;
    }
    else if( PROFILING_UPDATE_STATE == 2 )
    {
      FIELD_CTR_PROFILING_END;
      PROFILING_UPDATE_STATE = 0;
    }
  }
#endif

  // 入力値更新
  m_pCameraUiDeviceTranslator->Update();

  // Field3DObjectNodeのExecuteTerminate
  this->Field3DObjectNodeExecuteTerminate();
  
  // ギミックの更新
  UpdateFirstGimmick();
  // 通常更新処理
  this->UpdateNormal();
  // ギミックの更新
  UpdateLastGimmick();
  // StartLocationの更新
  
  if( GetPlayerCharacter()->GetCharaDrawInstance() )
  {
    Field::Location location;
    this->GetPlayerLocation( &location );
    m_pGameManager->GetGameData()->SetStartLocation( &location );
  }

#if PM_DEBUG
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    //gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    if( pButton->IsTrigger(gfl2::ui::BUTTON_START) && pButton->IsHold(gfl2::ui::BUTTON_L) )
    {
      gfl2::heap::HeapBase * pAppHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      gfl2::heap::HeapBase * pAppHeapDev = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      gfl2::heap::HeapBase * pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      gfl2::heap::HeapBase * pDllHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );

      GFL_PRINT( "AppHeap[0x%x][0x%x]\n",             pAppHeap->GetTotalFreeSize(), pAppHeap->GetTotalAllocatableSize() );
      GFL_PRINT( "AppDeviceHeap[0x%x][0x%x]\n",       pAppHeapDev->GetTotalFreeSize(), pAppHeapDev->GetTotalAllocatableSize() );
      GFL_PRINT( "FieldHeap[0x%x][0x%x]\n",       m_pFieldHeap->GetTotalFreeSize(), m_pFieldHeap->GetTotalAllocatableSize() );
      GFL_PRINT( "FieldDeviceHeap[0x%x][0x%x]\n", m_pFieldDeviceHeap->GetTotalFreeSize(), m_pFieldDeviceHeap->GetTotalAllocatableSize() );
      GFL_PRINT( "DLLHeap[0x%x][0x%x]\n",       pDllHeap->GetTotalFreeSize(), pDllHeap->GetTotalAllocatableSize() );
      GFL_PRINT( "DebugHeap[0x%x][0x%x]\n",       pDebugHeap->GetTotalFreeSize(), pDebugHeap->GetTotalAllocatableSize() );


      Debug::DebugHeapLog::GetInstance()->ShowLog();

      MyRenderingPipeLine* myRenderingPipeLine = m_pFieldRenderingPipeLineManager->GetMyRenderingPipeLine();
      GFL_PRINT( "MainRenderPath              ObjectCnt %d\n", myRenderingPipeLine->GetMainRenderPath()->GetDrawManager()->ObjectCnt() );
      GFL_PRINT( "NijiEdgeMapSceneRenderPath  ObjectCnt %d\n", myRenderingPipeLine->GetEdgeMapSceneRenderPath()->GetDrawManager()->ObjectCnt() );

      GFL_PRINT( "======================================\n" );
      System::nijiAllocator::DebugVramMemoryInfoDump();
      System::nijiAllocator::DebugSetResidentVramCheck();
      GFL_PRINT( "======================================\n" );
    }

    // 現在の表示カメラの情報をDump
    if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::INFO_LEVEL ) > 0 )
    {
      m_pCameraManager->GetMainGamePlayCamera()->Dump();
    }

    m_pDebugInfoWin->Update();
  }
#endif // PM_DEBUG

  return false;
}

/**
* @brief  PROCのPre描画処理
*/
void Fieldmap::PreDraw(void)
{
  // Tick計測ワーク定義
  enum
  {
    TICK_SETCLEARRENDERTARGET,
    TICK_FIELD_PREDRAW,
    TICK_APP_PREDRAW,
    TICK_BUY_EVENT,
    TICK_PREDRAW_ALL,
    TICK_MAX,
  };

  b32 isEnableTickProf = false;
#if FIELD_TICK_PROFILING_UPDATE_NORMAL
  if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD)->IsTrigger(gfl2::ui::BUTTON_DOWN) )
  {
    isEnableTickProf = true;
  }
#endif

  GFL_DEBUG_TICK_SETUP   (Fieldmap,TICK_MAX);
  GFL_DEBUG_TICK_SET_FLAG(Fieldmap,isEnableTickProf);
  GFL_DEBUG_TICK_RESET   (Fieldmap);

  GFL_DEBUG_TICK_START(Fieldmap, TICK_PREDRAW_ALL, "PreDraw");

  // Skybox状態とRenderingPipeLineManagerの状況を見てクリアするかどうかを変更する。
  GFL_DEBUG_TICK_START(Fieldmap, TICK_SETCLEARRENDERTARGET, "SetClearRenderTarget");
  SetClearRenderTarget();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_SETCLEARRENDERTARGET);

  GFL_DEBUG_TICK_START(Fieldmap, TICK_FIELD_PREDRAW, "m_pFieldRenderingPipeLineManager->PreDraw");
  m_pFieldRenderingPipeLineManager->PreDraw();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_FIELD_PREDRAW);

  //AppRenderingManager
  GFL_DEBUG_TICK_START(Fieldmap, TICK_APP_PREDRAW, "m_pAppRenderingManager->PreDraw");
  m_pAppRenderingManager->PreDraw();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_APP_PREDRAW);

  GFL_DEBUG_TICK_START(Fieldmap, TICK_BUY_EVENT, "m_pShopBuyEvent->PreDrawFunc");
  if( m_pShopBuyEvent )
  {
    m_pShopBuyEvent->PreDrawFunc();
  }
  GFL_DEBUG_TICK_END(Fieldmap, TICK_BUY_EVENT);

#if PM_DEBUG
  if( m_ReadyFlag )
  {
    // デバッグ表示物登録処理
    DebugRegistRender();
  } // 
#endif  //#if PM_DEBUG

  GFL_DEBUG_TICK_END(Fieldmap, TICK_PREDRAW_ALL);

#if FIELD_TICK_PROFILING_UPDATE_NORMAL
  gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
  GFL_DEBUG_TICK_DUMP_ALL(Fieldmap);
  gfl2::debug::ResetPrintFlag();
#endif
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
  // Tick計測ワーク定義
  enum
  {
    TICK_FIELD_DRAW,
    TICK_WINDOW_DRAW,
    TICK_APP_DRAW,
    TICK_DRAW_RTTI,
    TICK_BUY_EVENT,
    TICK_DRAW_CAMERA_COMPOSIT,
    TICK_DRAW_ALL,
    TICK_MAX,
  };

  b32 isEnableTickPrint = false;
#if FIELD_TICK_PROFILING_UPDATE_NORMAL
  if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD)->IsTrigger(gfl2::ui::BUTTON_DOWN)
   || GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD)->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    isEnableTickPrint = true;
  }
#endif

  GFL_DEBUG_TICK_SETUP   (Fieldmap,TICK_MAX);
  GFL_DEBUG_TICK_SET_FLAG(Fieldmap,true);
  GFL_DEBUG_TICK_RESET   (Fieldmap);

  GFL_DEBUG_TICK_START(Fieldmap, TICK_DRAW_ALL, "Draw");

  // フィールドパイプライン
  GFL_DEBUG_TICK_START(Fieldmap, TICK_FIELD_DRAW, "m_pFieldRenderingPipeLineManager->Draw");
  m_pFieldRenderingPipeLineManager->Draw( displayNo, m_pCameraManager->GetMainViewCamera());
  GFL_DEBUG_TICK_END(Fieldmap, TICK_FIELD_DRAW);

  // DMA命令
  FieldGimmick* pFieldGimmick = GetFieldGimmick(GIMMICK_IDX_ZONE);
  if (pFieldGimmick != NULL)
  {
    pFieldGimmick->TransferTexture(displayNo);
  }

  //会話ウィンドウ系のDraw ウィンドウが無いときは何もしない
  GFL_DEBUG_TICK_START(Fieldmap, TICK_WINDOW_DRAW, "m_pFieldWindowManager->Draw");
  if( m_pFieldWindowManager ) m_pFieldWindowManager->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( displayNo ) );
  GFL_DEBUG_TICK_END(Fieldmap, TICK_WINDOW_DRAW);

  //レイアウトパイプラインの描画
  GFL_DEBUG_TICK_START(Fieldmap, TICK_APP_DRAW, "m_pAppRenderingManager->Draw");
  m_pAppRenderingManager->Draw(displayNo);
  GFL_DEBUG_TICK_END(Fieldmap, TICK_APP_DRAW);

#if PM_DEBUG
  GFL_DEBUG_TICK_START(Fieldmap, TICK_DRAW_RTTI, "DrawDebugRtti");
  DrawDebugRtti( displayNo);
  GFL_DEBUG_TICK_END(Fieldmap, TICK_DRAW_RTTI);
#endif // PM_DEBUG

//  m_pSubScreen->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( displayNo ));

  GFL_DEBUG_TICK_START(Fieldmap, TICK_BUY_EVENT, "m_pShopBuyEvent->DrawFunc");
  if( m_pShopBuyEvent )
  {
    m_pShopBuyEvent->DrawFunc( displayNo );
  }
  GFL_DEBUG_TICK_END(Fieldmap, TICK_BUY_EVENT);

#if PM_DEBUG
  GFL_DEBUG_TICK_START(Fieldmap, TICK_DRAW_CAMERA_COMPOSIT, "DrawCameraCompositionAssist->DrawFunc");
  Debug::DebugDraw::DrawCameraCompositionAssist( displayNo);
  GFL_DEBUG_TICK_END(Fieldmap, TICK_DRAW_CAMERA_COMPOSIT);
#endif

  GFL_DEBUG_TICK_END(Fieldmap, TICK_DRAW_ALL);

#if PM_DEBUG
  if( m_pFieldDebugMenu && displayNo==gfl2::gfx::CtrDisplayNo::LEFT )
  {
    m_pFieldDebugMenu->SetPerformanceForCpuDraw( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_DRAW_ALL) );
    m_pFieldDebugMenu->SetDrawTagNumForAllScene( m_pFieldRenderingPipeLineManager->GetMyRenderingPipeLine()->GetMainRenderPath()->GetDrawManager()->GetVisibleTagNum() );

    if( gfl2::debug::DebugWin_IsOpen() ) // 地形ブロックをループする分の負荷があるので念のため
    {
      m_pTerrainManager->UpdateDrawTagInfo( m_pFieldDebugMenu );
    }
  }
#endif

#if FIELD_TICK_PROFILING_UPDATE_NORMAL
  if( isEnableTickPrint )
  {
    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(Fieldmap);
    gfl2::debug::ResetPrintFlag();
  }
#endif
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
bool Fieldmap::Terminate(void)
{
  enum
  {
    SEQ_GPU_IDL_WAIT,
    SEQ_ENDWAIT,
    SEQ_DELETE,
  };

  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_TOTAL,
    TICK_END_WAIT,
    TICK_DELETE,
    TICK_ONETIME,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(Terminate,TICK_MAX);

  if( m_Seq != SEQ_GPU_IDL_WAIT )
  {
    GFL_DEBUG_TICK_START(Terminate, TICK_ONETIME, "Field::Terminate ONEFRAME");
  }

  switch(m_Seq){
  case SEQ_GPU_IDL_WAIT:  /// TerminateStateList内でも待つようになっているので、ここでは待たない。

    GFL_DEBUG_TICK_SET_FLAG(Terminate,DEBUG_DRAW_TICK_TOMOYA);
    GFL_DEBUG_TICK_RESET(Terminate);

    GFL_DEBUG_TICK_START(Terminate, TICK_TOTAL, "Field::Terminate TOTAL");

    GFL_DEBUG_TICK_START(Terminate, TICK_ONETIME, "Field::Terminate ONEFRAME");

#if FIELD_CTR_PROFILING_TERM
      FIELD_CTR_PROFILING_START
#endif

    m_Seq++;
    //break; through

  case SEQ_ENDWAIT:
    {
      GFL_DEBUG_TICK_START(Terminate, TICK_END_WAIT, "Field::Terminate END_WAIT");
      // Field3DObjectNodeのExecuteTerminate
      this->Field3DObjectNodeExecuteTerminate();

      gfl2::util::List<State>::Iterator iter = m_TerminateStateList.Begin();
      u32 end_count = 0;
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
      if( m_TerminateStateList.Size() != end_count )
      {
        break;
      }
      else
      {
        m_Seq++;
      }
      GFL_DEBUG_TICK_END(Terminate, TICK_END_WAIT);
    }
    break;

  case SEQ_DELETE:
    {
      GFL_DEBUG_TICK_START(Terminate, TICK_DELETE, "Field::Terminate DELETE");
      // ベースシステム破棄
      TerminateState();

      TerminateHeap();

      // レンダーバッファのクリア設定のリセット
      ResetClearRenderTarget();

#if PM_DEBUG
      // Debug用Roモジュール破棄
      TerminateDebugDll();
#endif

#if defined(GF_PLATFORM_CTR)
      // CPUアクセスの優先度を基に戻す
      nn::gx::CTR::SetMemAccessPrioMode( nn::gx::CTR::MEM_ACCESS_PRIO_MODE_1 );
#endif

      GFL_DEBUG_TICK_END(Terminate, TICK_DELETE);
    }

    GFL_DEBUG_TICK_END(Terminate, TICK_ONETIME);
    //GFL_RELEASE_PRINT( "Terminate OneFrame %d Seq %d\n", GFL_DEBUG_TICK_US_GET(Terminate, TICK_ONETIME), m_Seq );

    m_Seq = 0;

#if PM_DEBUG
    // アプリケーションのVRAM領域がクリアかチェック
    System::nijiAllocator::DebugCheckApplicationVramClear();
#endif

#if FIELD_CTR_PROFILING_TERM
    FIELD_CTR_PROFILING_END
#endif
    GFL_DEBUG_TICK_END(Terminate, TICK_TOTAL);

#ifdef DEBUG_ONLY_FOR_takahashi_tomoya
#if DEBUG_DRAW_TICK_TOMOYA
    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(Terminate);
    gfl2::debug::ResetPrintFlag();
#endif
#endif

    return true;
  }

  GFL_DEBUG_TICK_END(Terminate, TICK_ONETIME);
  //GFL_RELEASE_PRINT( "Terminate OneFrame %d Seq %d\n", GFL_DEBUG_TICK_US_GET(Terminate, TICK_ONETIME), m_Seq );
  
  return false;
}

/**
* @brief   イベントチェックの実行
*/
void Fieldmap::EventCheck()
{
  b32 isEnableTickProf = false;
#if FIELD_TICK_PROFILING_UPDATE_NORMAL
  if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD)->IsTrigger(gfl2::ui::BUTTON_DOWN)
   || GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD)->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    isEnableTickProf = true;
  }
#endif

  GFL_DEBUG_TICK_SETUP   (Fieldmap,1);
  GFL_DEBUG_TICK_SET_FLAG(Fieldmap,isEnableTickProf);
  GFL_DEBUG_TICK_RESET   (Fieldmap);

  // イベントチェックメイン処理
  GFL_DEBUG_TICK_START(Fieldmap, 0, "m_pEventCheck->EventCheck");
  if(m_pEventCheck) { m_pEventCheck->EventCheck(); }
  GFL_DEBUG_TICK_END(Fieldmap, 0);

#if FIELD_TICK_PROFILING_UPDATE_NORMAL
  gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
  GFL_DEBUG_TICK_DUMP_ALL(Fieldmap);
  gfl2::debug::ResetPrintFlag();
#endif
}

/**
* @brief   天球リソースを保持するリクエストを発行
*/
void Fieldmap::RequestKeepSkyboxResource( void )
{
  m_isSkyboxKeep = true;
}

/**
 *  @brief MoveModelManagerの取得
 */
MoveModel::FieldMoveModelManager* Fieldmap::GetMoveModelManager( void )
{
  return GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
}

/**
 *  @brief  ギミックID取得
 *  @param  idx enum Field::GimmickIndex
 */
u32 Fieldmap::GetFieldGimmickID( GimmickIndex idx )
{
  u32 result = 0;

  switch( idx )
  {
  case GIMMICK_IDX_ZONE:
    {
      // ZoneはLoaderからIDをもらう
      void* pZoneData = m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_ZoneId);
      ZoneDataAccessor zoneDataAccessor( pZoneData);
      result = zoneDataAccessor.GetGimmickID();
    }
    break;
  }

  return result;
}

/**
 *  @brief  往来Managerの取得
 */
Field::TrafficManager *Fieldmap::GetTrafficManager( void )
{
  return m_pGameManager->GetGameData()->GetFieldTrafficManager();
}

/**
 *  @brief  主人公のロケーションの取得
 */
void Fieldmap::GetPlayerLocation( Field::Location* p_location )
{
  p_location->worldId = this->GetWorldID();
  p_location->zoneId = this->GetZoneID();
  p_location->position = this->GetPlayerCharacter()->GetLocalSRT().translate;
  p_location->quaternion = this->GetPlayerCharacter()->GetLocalSRT().rotation;
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


  // フィールドイベント用ヒープを生成
  m_pFieldExtHeap = m_pGameManager->GetGameData()->GetFieldArea()->GetFieldExtHeap();

  GFL_PRINT( "FieldHeap 0x%x\n", m_pFieldHeap );
  GFL_PRINT( "FieldDeviceHeap 0x%x\n", m_pFieldDeviceHeap );
}

/**
 *  @brief ヒープ破棄
 */
void Fieldmap::TerminateHeap( void )
{
  if(m_pFieldExtHeap){
    m_pFieldExtHeap = NULL;
  }

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
  m_InitializeStateList.PushBack( State(this, &Fieldmap::State_ScriptInitialize) );
  m_InitializeStateList.PushBack( State(this, &Fieldmap::State_Ready) );

  // 破棄処理リスト
  m_TerminateStateList.PushBack( State(this, &Fieldmap::State_RenderSkip) );
  m_TerminateStateList.PushBack( State(this, &Fieldmap::State_ScriptTerminate) );
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
  gfl2::fs::AsyncFileManager* pAsyncFileManager = m_pGameManager->GetAsyncFileManager();

  // フィールド描画パイプライン生成・初期化
  {
    FieldRenderingPipeLineManager::InitializeDescription desc;
    desc.pHeap = m_pFieldDeviceHeap;
    desc.pTemporaryHeapForFileRead = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD);
    desc.pAllocator = m_pFieldDeviceHeapAllocator;
    desc.pAsyncFileManager = pAsyncFileManager;

    m_pFieldRenderingPipeLineManager = GFL_NEW( m_pFieldDeviceHeap) FieldRenderingPipeLineManager;
    m_pFieldRenderingPipeLineManager->Initialize( desc);

  }

  // レイアウト描画用の設定。
  {
    m_pAppRenderingHeap = GFL_NEW( m_pFieldHeap ) app::util::Heap();
    m_pAppRenderingHeap->LocalHeapCreate( m_pFieldHeap, m_pFieldDeviceHeap,
      Fieldmap::APP_RENDERING_MANAGER_HEAP_SIZE, Fieldmap::APP_RENDERING_MANAGER_HEAP_DEVICE_SIZE );

    m_pAppRenderingManager = GFL_NEW( m_pFieldHeap ) app::util::AppRenderingManager();

    app::util::AppRenderingManager::Description appRenderingManagerDesc;
    
    appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

    appRenderingManagerDesc.isUpperDisplayStereo = true;  // ポケファインダーのために立体視Onになるはず
    appRenderingManagerDesc.callGlobalTraverseModel = false;  //トラバースはフィールド全体の枠組みのなかでやるので、ここはfalse
    
    m_pAppRenderingManager->StartAsyncCreate(
          appRenderingManagerDesc,
          m_pAppRenderingHeap,
          NULL,
          pAsyncFileManager
      );
  }

}

void Fieldmap::TerminateRenderingPipeLine(void)
{
  GFL_SAFE_DELETE(m_pAppRenderingManager);
  m_pAppRenderingHeap->Delete();
  GFL_SAFE_DELETE(m_pAppRenderingHeap);

  m_pFieldRenderingPipeLineManager->Terminate();
  GFL_SAFE_DELETE(m_pFieldRenderingPipeLineManager);
}

//! レンダリングパイプライン　生成生成待ち
bool Fieldmap::WaitInitializeRenderingPipeLine(void)
{
  bool is_result_app = m_pAppRenderingManager->IsCreated();
  bool is_result_3d = m_pFieldRenderingPipeLineManager->InitializeWait();

  if(is_result_app && is_result_3d)
  {
    m_pRenderingPipeLine = m_pFieldRenderingPipeLineManager->GetMyRenderingPipeLine();
#if PM_DEBUG
    m_pRenderingPipeLine->GetDebugTextBeforeRenderPath()->SetupFieldmap(this);
#endif

    return true;
  }
  return false;
}

//! レンダリングパイプライン　破棄が可能になるのをチェック
bool Fieldmap::CanDestroyRenderingPipeLine(void)
{
  if(m_pAppRenderingManager->CanDestroy()){
    return true;
  }
  return false;
}


// 古いレンダリングターゲットの設定を保存
void Fieldmap::TempOldClearRenderTargetSetting()
{
  m_pGameManager->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT, &m_OldMainDispClearSetting );
}

// クリアレンダーターゲット設定を変更
void Fieldmap::SetClearRenderTarget()
{
  GameSys::GameManager::BUFFER_CLEAR_SETTING bufferClearSetting = m_OldMainDispClearSetting;

  switch( m_pFieldRenderingPipeLineManager->GetNowExecuteType() )
  {
  case FieldRenderingPipeLineManager::EXECUTE_TYPE_NORMAL:
    {
      if( m_pSkybox->IsEnable() )
      {
        // デプスのクリアのみ行なう
        bufferClearSetting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
      }
      else
      {
        // カラーもデプスもクリアする
        bufferClearSetting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
      }
    }
    break;
  case FieldRenderingPipeLineManager::EXECUTE_TYPE_CAPTURE:
    {
      // カラーもデプスもクリアする
      bufferClearSetting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
    }
    break;
  default:
    GFL_ASSERT( 0);
    break;
  }

  m_pGameManager->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT, bufferClearSetting );
  m_pGameManager->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::RIGHT, bufferClearSetting );
}

// クリアレンダーターゲット設定を元に戻す
void Fieldmap::ResetClearRenderTarget()
{
  m_pGameManager->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT, m_OldMainDispClearSetting );
  m_pGameManager->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::RIGHT, m_OldMainDispClearSetting );
}


/**
 *  @brief 更新処理
 */
void Fieldmap::UpdateNormal( void )
{
  if( this->m_pGameManager->GetGameEventManager()->IsExists() == false ){
    EvTime::Update( this->m_pGameManager->GetGameData(), true );
  }

#if PM_DEBUG
  // イベント終了座標チェック
  if (!GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameEventManager()->IsExists())
  {
    Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();
    Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = this->GetMoveModelManager();

    if (pFieldScriptSystem && pFieldScriptSystem->GetDebugScriptEventAfterCheckFlag() )
    {
      // 最終座標を出力
      if (FieldScript::FieldScriptSystem::DebugEndPositionOutPutEnableFlag)
      {
        u16 zoneId = m_pGameManager->GetGameData()->GetNowZoneID();
        u16 areaId = m_pGameManager->GetGameData()->GetFieldArea()->GetAreaID();
        pFieldCharaModelManager->OutPutsEndPositionForEventScript(zoneId, areaId);
      }

      // 最終座標をチェック
      // if (FieldScript::FieldScriptSystem::DebugEndPositionCheckEnableFlag) // システム化 170525ROM対応
      {
        pFieldCharaModelManager->CheckSafetyPositionForEventScript();
      }

      // 最終座標チェックをクリア
      pFieldCharaModelManager->ClearCheckSafetyPositionForEventScript();

      // システム化
      //if (ScriptAssert::ScriptAssertEnableFlag)
      {
        // 残留チェック
        pFieldCharaModelManager->CheckActionEndForEventScript();

        // レジスト状態チェック
        m_pPlacementDataManager->CheckMoveModelActorResistState();
      }

      // チェックしました
      pFieldScriptSystem->SetDebugScriptEventAfterCheckFlag( false );
    }
  }
#endif


  b32 isEnableTickPrint = false;
#if FIELD_TICK_PROFILING_UPDATE_NORMAL
  if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD)->IsTrigger(gfl2::ui::BUTTON_DOWN)
   || GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD)->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    isEnableTickPrint = true;
  }
#endif

  // Tick計測ワーク定義
  enum
  {
    TICK_ACTIONBUTTON_MANAGER_UPDATE,
    TICK_MOVEMODEL_PREVUPDATE,
    TICK_UPDATE_COLLISION,
    TICK_UPDATE_ZONE,
    TICK_UPDATE_ENVMANAGER,
    TICK_UPDATE_ENVSOUNDMANAGER,
    TICK_FIELDWINDOW_MANAGER,
    TICK_CAMERA_UPDATE,
    TICK_UPDATE_TERRAIN,
    TICK_UPDATE_STATICMODEL,
    TICK_WEATHER_MANAGER_UPDATE,
    TICK_WEATHER_CONTROL_UPDATE,
    TICK_TRAFFIC_MANAGER_UPDATE,
    TICK_TRIALMODEL_UPDATE,
    TICK_GIMMICKENCOUNT_MANAGER_UPDATE,
    TICK_PLAYER_SHADE_UPDATE,
    TICK_MOVEMODEL_CULLING,
    TICK_STATICMODEL_CULLING,
    TICK_TRAVERSEMODEL_FAST,
    TICK_MOVEMODEL_POSTUPDATE,
    TICK_TRIALMODEL_POSTUPDATE,
    TICK_UPDATE_EFFECT,
    TICK_UPDATE_SCRIPT_EFFECT,
    TICK_UPDATE_CAMERAEYECOLLISION,
    TICK_GIMMICK_POSTUPDATE,
    TICK_APPRENDER_MANAGER_UPDATE,
    TICK_TRAVERSEMODEL_FAST_CHILD,
    TICK_RENDERINGPIPELINE_MANAGER_UPDATE,
    TICK_UPDATE_ALL,
    TICK_MAX,
  };

  GFL_DEBUG_TICK_SETUP   (Fieldmap,TICK_MAX);
  GFL_DEBUG_TICK_SET_FLAG(Fieldmap,true);
  GFL_DEBUG_TICK_RESET   (Fieldmap);

  // 動作モデルマネージャーを取得
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = this->GetMoveModelManager();
  Field::MoveModel::FieldMoveModelPlayer * pPlayerCharacter = this->GetPlayerCharacter();

  GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_ALL, "UpdateNormal");

#if PM_DEBUG
  if( m_nDebugVisibleMoveModel_Old != FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_MOVEMODEL ) )
  {
    m_pPlacementDataManager->DebugFunc_ChangeVisibleMoveModel();
    m_nDebugVisibleMoveModel_Old = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_MOVEMODEL );
  }
  if( m_nDebugVisiblePlayer_Old != FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_PLAYER ) )
  {
    MoveModel::FieldMoveModelPlayer *pPlayer = GetPlayerCharacter();
    if( pPlayer )
    {
      pPlayer->SetVisible( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_PLAYER ) != 0 );
      if( pPlayer->GetOnMoveModel() )
      {
        pPlayer->GetOnMoveModel()->SetVisible( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_PLAYER ) != 0 );
      }
    }
    m_nDebugVisiblePlayer_Old = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_PLAYER );
  }
#endif

  //アクションボタンチェック処理
  GFL_DEBUG_TICK_START(Fieldmap, TICK_ACTIONBUTTON_MANAGER_UPDATE, "m_pActionButtonManager->Update");
  m_pActionButtonManager->Update(m_pEventCheck->GetTalkTarget(), pFieldCharaModelManager, m_pEffectManager, m_pEventCheck->IsActionButtonInterestNotClear());
  GFL_DEBUG_TICK_END(Fieldmap, TICK_ACTIONBUTTON_MANAGER_UPDATE);

  // キャラクター 前更新(コリジョン以外の座標/姿勢の変更を行う)
  GFL_DEBUG_TICK_START(Fieldmap, TICK_MOVEMODEL_PREVUPDATE, "pFieldCharaModelManager->PrevUpdate");
  pFieldCharaModelManager->PrevUpdate();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_MOVEMODEL_PREVUPDATE);

  {
    // コリジョン更新処理
    GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_COLLISION, "UpdateCollision");
    UpdateCollision();
    GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_COLLISION);
  }

  // プレイヤー座標確定
  //-----------------------------------------------------------------------------------------------------------------

  // Ui用の座標を更新
  if(pPlayerCharacter->GetCharaDrawInstance())
  {
    m_playerPosition   = pPlayerCharacter->GetCharaDrawInstance()->GetPosition();
    m_playerQuaternion = pPlayerCharacter->GetCharaDrawInstance()->GetRotationQuat();
  }

  // Zoneの更新処理
  {
    GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_ZONE, "UpdateZone");
    this->UpdateZone(pPlayerCharacter);
    GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_ZONE);
  }

  // EnvManager更新
  GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_ENVMANAGER, "UpdateEnvManager");
  UpdateEnvManager();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_ENVMANAGER);

  // 環境音更新
  GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_ENVSOUNDMANAGER, "updateEnvSoundManager");
  this->updateEnvSoundManager();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_ENVSOUNDMANAGER);

  GFL_DEBUG_TICK_START(Fieldmap, TICK_FIELDWINDOW_MANAGER, "m_pFieldWindowManager->Update");
  if( m_pFieldWindowManager ) m_pFieldWindowManager->Update();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_FIELDWINDOW_MANAGER);

  // カメラ更新
  GFL_DEBUG_TICK_START(Fieldmap, TICK_CAMERA_UPDATE, "m_pCameraManager->Update");
  m_pCameraManager->Update();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_CAMERA_UPDATE);

  // 地形制御点を更新
#if ENABLE_TERRAIN
  GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_TERRAIN, "UpdateTerrain");
  UpdateTerrain();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_TERRAIN);
#endif

#if ENABLE_STATIC_MODEL
  GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_STATICMODEL, "UpdateStaticModel");
  UpdateStaticModel();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_STATICMODEL);
#endif

  //天候マネージャ更新
  GFL_DEBUG_TICK_START(Fieldmap, TICK_WEATHER_MANAGER_UPDATE, "m_pWeatherManager->Update");
  m_pWeatherManager->Update();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_WEATHER_MANAGER_UPDATE);

  GFL_DEBUG_TICK_START(Fieldmap, TICK_WEATHER_CONTROL_UPDATE, "m_pWeatherControl->Update");
  m_pWeatherControl->Update(m_ZoneId);
  GFL_DEBUG_TICK_END(Fieldmap, TICK_WEATHER_CONTROL_UPDATE);

  // 往来更新
  {
    GFL_DEBUG_TICK_START(Fieldmap, TICK_TRAFFIC_MANAGER_UPDATE, "GetTrafficManager->Update");
    if( GetTrafficManager() ) GetTrafficManager()->Update();
    GFL_DEBUG_TICK_END(Fieldmap, TICK_TRAFFIC_MANAGER_UPDATE);
  }

  // trialModel
  GFL_DEBUG_TICK_START(Fieldmap, TICK_TRIALMODEL_UPDATE, "m_pTrialModelFactory->Update");
  if( m_pTrialModelFactory )
  {
    m_pTrialModelFactory->Update();
  }
  GFL_DEBUG_TICK_END(Fieldmap, TICK_TRIALMODEL_UPDATE);

  // ギミックエンカウントマネージャー更新
  {
    GFL_DEBUG_TICK_START(Fieldmap, TICK_GIMMICKENCOUNT_MANAGER_UPDATE, "m_pGimmickEncountManager->Update");
    GimmickEncount::Work::UpdateDescription desc;
    desc.playerPosition = m_playerPosition;

    if( m_pGimmickEncountManager ) m_pGimmickEncountManager->Update( desc);
    GFL_DEBUG_TICK_END(Fieldmap, TICK_GIMMICKENCOUNT_MANAGER_UPDATE);
  }

  // プレイヤー陰影更新
  GFL_DEBUG_TICK_START(Fieldmap, TICK_PLAYER_SHADE_UPDATE, "m_pFieldPlayerShade->Update");
  if (m_pFieldPlayerShade)
  {
    m_pFieldPlayerShade->Update(GetPlayerCharacter());
  }
  GFL_DEBUG_TICK_END(Fieldmap, TICK_PLAYER_SHADE_UPDATE);

  // 動作モデルカリング(カリング対象となるものは全てSceneのルートノードから直結されているのでトラバースより先にして問題ない)
  GFL_DEBUG_TICK_START(Fieldmap, TICK_MOVEMODEL_CULLING, "pFieldCharaModelManager->UpdateCulling");
  gfl2::math::Matrix44 viewProj;
  gfl2::math::Matrix34 view;
  gfl2::math::Vector3  cameraPos(0.0f,0.0f,0.0f);
  gfl2::math::Vector3  targetPos(0.0f,0.0f,0.0f);
  m_pCameraManager->GetMainGamePlayCamera()->GetProjectionMatrix( &viewProj );
  m_pCameraManager->GetMainGamePlayCamera()->GetViewMatrix( &view );
  viewProj = viewProj * view;
  m_pCameraManager->GetMainGamePlayCamera()->GetCameraAndTargetPosition(&targetPos,&cameraPos);
  pFieldCharaModelManager->UpdateCulling(viewProj, cameraPos, ( m_pCameraManager->GetUnitNoForMainGamePlayCamera() == Field::Camera::UnitNo::FIELD ), m_ZoneId);
  GFL_DEBUG_TICK_END(Fieldmap, TICK_MOVEMODEL_CULLING);

  // 配置モデルカリング(カリング対象となるものは全てSceneのルートノードから直結されているのでトラバースより先にして問題ない)
  GFL_DEBUG_TICK_START(Fieldmap, TICK_STATICMODEL_CULLING, "m_pStaticModelManager->UpdateCulling");
  m_pStaticModelManager->UpdateCulling( viewProj );
  GFL_DEBUG_TICK_END(Fieldmap, TICK_STATICMODEL_CULLING);

  // 動作モデルが持つサブオブジェクトの更新
  // note:動作モデルの座標が確定し、且つトラバースの前にする必要があります
  pFieldCharaModelManager->UpdateSubobject();

  // トラバース直前のギミック更新
  UpdateTraverseBeforeGimmick();

  // トラバース処理にてワールド座標を更新
  GFL_DEBUG_TICK_START(Fieldmap, TICK_TRAVERSEMODEL_FAST, "TraverseModelFast");
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_TRAVERSEMODEL_FAST);

  // キャラクター 後更新(座標の決定後に必要な処理を行う)
  GFL_DEBUG_TICK_START(Fieldmap, TICK_MOVEMODEL_POSTUPDATE, "pFieldCharaModelManager->PostUpdate");
  pFieldCharaModelManager->PostUpdate();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_MOVEMODEL_POSTUPDATE);

  // trialModel 座標確定後の処理
  GFL_DEBUG_TICK_START(Fieldmap, TICK_TRIALMODEL_POSTUPDATE, "m_pTrialModelFactory->UpdateAfterTraverse");
  if( m_pTrialModelFactory )
  {
    m_pTrialModelFactory->UpdateAfterTraverse();
  }
  GFL_DEBUG_TICK_END(Fieldmap, TICK_TRIALMODEL_POSTUPDATE);

#if ENABLE_EFFECT
  GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_EFFECT, "UpdateEffectSystem");
  UpdateEffectSystem();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_EFFECT);
#endif
  {
    // スクリプトエフェクト更新
    GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_SCRIPT_EFFECT, "pFieldScriptSystem->GetScriptEffectManager()->Update");
    Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();
    if( pFieldScriptSystem )  pFieldScriptSystem->GetScriptEffectManager()->Update();
    GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_SCRIPT_EFFECT);
  }

  // カメラ周辺メッシュのOFF
  GFL_DEBUG_TICK_START(Fieldmap, TICK_UPDATE_CAMERAEYECOLLISION, "m_pCameraManager->UpdateCameraEyeCollision");
  m_pTerrainManager->UpdateCameraCollisonPrev();
  m_pCameraManager->UpdateCameraEyeCollision();
  m_pTerrainManager->UpdateCameraCollisonAfter();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_CAMERAEYECOLLISION);

  // ギミックの更新
  GFL_DEBUG_TICK_START(Fieldmap, TICK_GIMMICK_POSTUPDATE, "UpdateCalculateAfterGimmick");
  UpdateCalculateAfterGimmick();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_GIMMICK_POSTUPDATE);

  //下画面更新
  //m_pSubScreen->Update();

  //AppRenderingManagerの更新処理
  GFL_DEBUG_TICK_START(Fieldmap, TICK_APPRENDER_MANAGER_UPDATE, "m_pAppRenderingManager->Update");
  m_pAppRenderingManager->Update(); // 非同期生成処理を進めるために描画していないときもUpdateを毎フレーム呼ぶ。
  GFL_DEBUG_TICK_END(Fieldmap, TICK_APPRENDER_MANAGER_UPDATE);

  // 後更新トランスフォームのトラバース処理
  GFL_DEBUG_TICK_START(Fieldmap, TICK_TRAVERSEMODEL_FAST_CHILD, "TraverseModelFastFromChild");
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFastFromChild( GetFieldRootNode()->GetAfterTransRootNode() );
  GFL_DEBUG_TICK_END(Fieldmap, TICK_TRAVERSEMODEL_FAST_CHILD);

  GFL_DEBUG_TICK_START(Fieldmap, TICK_RENDERINGPIPELINE_MANAGER_UPDATE, "m_pFieldRenderingPipeLineManager->Update");
  m_pFieldRenderingPipeLineManager->Update();
  GFL_DEBUG_TICK_END(Fieldmap, TICK_RENDERINGPIPELINE_MANAGER_UPDATE);

  // ふれあい鳴き声設定の更新
  Field::PokemonFureai::VoiceUpdate();

  GFL_DEBUG_TICK_END(Fieldmap, TICK_UPDATE_ALL);

#if PM_DEBUG
  if( m_pFieldDebugMenu )
  {
    m_pFieldDebugMenu->SetPerformanceForZone               ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_UPDATE_ZONE         ) );
    m_pFieldDebugMenu->SetPerformanceForTraverse           ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_TRAVERSEMODEL_FAST  ) );
    m_pFieldDebugMenu->SetPerformanceForMoveModelPrevUpdate( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_MOVEMODEL_PREVUPDATE) );
    m_pFieldDebugMenu->SetPerformanceForMoveModelPostUpdate( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_MOVEMODEL_POSTUPDATE) );
  }
#endif

#if FIELD_TICK_PROFILING_UPDATE_NORMAL
  if( isEnableTickPrint )
  {
    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(Fieldmap);
    gfl2::debug::ResetPrintFlag();
  }
#endif

#if PM_DEBUG
  // RTTI Debug Update
  UpdateDebugRtti();
#endif

}


/**
 * @brief ゾーン更新
 */
void Fieldmap::UpdateZone( Field::MoveModel::FieldMoveModel* pPlayerCharacter )
{
  enum
  {
    SEQ_NONE,
    SEQ_DELETE,
    SEQ_CREATE,

    SEQ_MAX,
  };

  gfl2::math::Vector3  controlPos = m_playerPosition;

  switch( m_seqZoneChange ){
  case SEQ_NONE:
    {
      // ゾーンが変わったかチェック　リクエスト発行まで
      const World::ZoneInfo * pZone = m_pWorld->IsHitZoneInfo( controlPos );
      if( pZone && (pZone->m_ZoneID != m_ZoneId) && (pZone->m_AreaID == this->m_pArea->GetAreaID()) )
      {

#if FIELD_CTR_PROFILING_ZONECHANGE
        FIELD_CTR_PROFILING_START
#endif

        GFL_PRINT( "ZoneChange Start nowZone %d nextZone %d\n", m_ZoneId, pZone->m_ZoneID );
        // ゾーン更新リクエスト
        m_seqZoneChange = SEQ_DELETE;
        m_reqZoneId     = pZone->m_ZoneID;

        // LastZoneの保存
        m_pGameManager->GetGameData()->GetSituation()->SetLastZoneID( m_ZoneId );

        // break through
      }
      else
      {
        break;
      }
    }
    // break through

  case SEQ_DELETE:
    {
      // 往来
      updateZone_TrafficDelete();

      updateZone_PlacementDataDelete();
      m_seqZoneChange = SEQ_CREATE;
    }
    break;

  case SEQ_CREATE:
    {
#if PM_DEBUG
      // FieldResidentにデバッグ用とのグローバルなZONEIDを設定
      m_pFieldResident->DebugSetZone( m_reqZoneId );
#endif

      // スクリプト
      updateZone_ScriptTop(m_reqZoneId);

      // ギミックエンカウント抽選
      if( m_pGimmickEncountManager ) m_pGimmickEncountManager->LotteryZoneGimmickFlag( m_reqZoneId);

      // きのみ配置ステータス更新
      if( m_pGameManager->GetGameData()->GetFieldBerryManager() ) m_pGameManager->GetGameData()->GetFieldBerryManager()->UpdateBerrysStatusToExist();

      // 釣りスポット復活
      if( m_pGameManager->GetGameData()->GetFishingSpotData() ) m_pGameManager->GetGameData()->GetFishingSpotData()->ResetFishingSpot();

      //　配置情報の反映
      updateZone_PlacementDataCreate(m_reqZoneId);

      // 往来
      updateZone_TrafficCreate();
    
      // スクリプト
      updateZone_ScriptBottom(m_reqZoneId);

      //天候の変更リクエスト
      {
        ZoneDataLoader* pZoneDataLoader = m_pGameManager->GetGameData()->GetFieldZoneDataLoader();
        ZoneDataAccessor zoneDataAccessor( pZoneDataLoader->GetZoneData( m_reqZoneId) );
        m_pWeatherControl->ChangeWeatherRequest();
      }

      // @fix GFNMCat[1187] キャラの最大表示数をリソースから可変にする
      ZoneDataAccessor zoneDataAccessor( m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData(m_ZoneId) );
      if( zoneDataAccessor.IsMoveModelDrawNumControl() ) { GetMoveModelManager()->SetDispMaxCnt(Field::MoveModel::FieldMoveModelManager::DEFAULT_DISP_MAX_CNT); }
      else                                               { GetMoveModelManager()->SetDispMaxCnt(Field::MoveModel::FIELD_MOVE_MODEL_MAX); }

      // ゾーン更新完了
      GFL_PRINT( "ZoneChange Complate nowZone %d nextZone %d\n", m_ZoneId, m_reqZoneId );
      m_ZoneId = m_reqZoneId;
      
      // 地名表示
      {
        m_pEffectManager->RequestStartPlaceName( false );
      }

      m_seqZoneChange = SEQ_NONE;

#if FIELD_CTR_PROFILING_ZONECHANGE
      FIELD_CTR_PROFILING_END
#endif

    }
    break;
  }

}

/**
 *  @brief  スクリプト更新
 */
void Fieldmap::updateZone_ScriptTop( ZoneID zoneId )
{
  Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();
  gfl2::fs::BinLinkerAccessor* pBinLinker = m_pArea->GetAccessor( Area::AREA_DATA_IDX_ZONE_SCRIPT_RESOURCE );
  int index = m_pWorld->GetZoneIndexInArea(zoneId);

  //ゾーンスクリプトの破棄と生成
  pFieldScriptSystem->FinalizeZoneScript();
  pFieldScriptSystem->ClearZoneLocalWork();

  pFieldScriptSystem->InitializeZoneScript( pBinLinker->GetData(index), pBinLinker->GetDataSize(index) , zoneId );

  //ゾーン　フラグワーク初期化スクリプトコール
  pFieldScriptSystem->CallZoneChange(
    m_pArea->GetAccessor( Area::AREA_DATA_IDX_INIT_SCRIPT_RESOURCE ), index, 
    zoneId, SCR_ZONEINIT_WALK_FLAGS, m_pFieldHeap );

  // 隠しアイテムの復活処理
  if( !m_pGameManager->GetGameData()->IsNowJoinFestaFieldData() )
  {
    m_pGameManager->GetGameData()->GetEventWork()->ClearReviveItemFlags();
  }

  //サウンドアップデート
  m_pGameManager->GetGameData()->GetFieldSound()->SetCurrentZoneID(  zoneId );
  m_pGameManager->GetGameData()->GetFieldSound()->ChangeFieldBGMByZoneChange( true );
  
  /// 空を飛ぶ、到着フラグをセット.
  WarpDataTools   *pWarpData = m_pGameManager->GetGameData()->GetWarpDataTools();
  pWarpData->UpdateArrive( m_pGameManager->GetGameData(), zoneId );
}

/**
 *  @brief  スクリプト更新
 */
void Fieldmap::updateZone_ScriptBottom( ZoneID zoneId )
{
  Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();

  //ゾーン オブジェクト初期化スクリプトコール
  pFieldScriptSystem->CallZoneChange(
    m_pArea->GetAccessor( Area::AREA_DATA_IDX_INIT_SCRIPT_RESOURCE ),
    m_pWorld->GetZoneIndexInArea(zoneId), 
    zoneId, SCR_ZONEINIT_WALK_OBJECT, m_pFieldHeap );
}

/**
 *  @brief 　配置データ破棄
 */
void Fieldmap::updateZone_PlacementDataDelete()
{
  m_pPlacementDataManager->DeleteZoneActionEvent();
  m_pPlacementDataManager->DeleteZoneBgEvent();
  m_pPlacementDataManager->DeleteZoneEncount();
  m_pPlacementDataManager->DeleteZoneMapJump();
  m_pPlacementDataManager->RequestDeleteDropItem();
  m_pPlacementDataManager->DeleteZoneItem();
  m_pPlacementDataManager->UnRegistZoneModel();
  m_pPlacementDataManager->DeleteZonePos();
  m_pPlacementDataManager->UnRegistZoneTrainer();
  m_pPlacementDataManager->RequestDeleteZoneStaticModel();
  m_pPlacementDataManager->UnRegistZoneGimmickEncount();
  m_pPlacementDataManager->DeleteZonePokeFinder();
  m_pPlacementDataManager->UnRegisterZoneBerryPoint();
  m_pPlacementDataManager->UnregisterZoneFishingSpot();
  m_pPlacementDataManager->UnRegistZonePushRock();
}

/**
 *  @brief 　配置データ作成
 */
void Fieldmap::updateZone_PlacementDataCreate( ZoneID zoneId )
{
  // 遅延解放
  m_pPlacementDataManager->DeleteZoneModel();
  m_pPlacementDataManager->DeleteZoneTrainer();
  m_pPlacementDataManager->DeleteDropItem();
  m_pPlacementDataManager->DeleteZoneStaticModel();
  m_pPlacementDataManager->DeleteZoneGimmickEncount();
  m_pPlacementDataManager->DeleteZoneBerryPointWork();
  m_pPlacementDataManager->DeleteZoneFishingSpotWork();
  m_pPlacementDataManager->DeleteZonePushRockWork();
  if( m_pGameManager->GetGameData()->GetFieldItemManager() ) m_pGameManager->GetGameData()->GetFieldItemManager()->TerminateDropItemData();

  // 生成
  m_pPlacementDataManager->CreateZoneActionEvent( zoneId );
  m_pPlacementDataManager->CreateZoneBgEvent( zoneId );
  m_pPlacementDataManager->CreateZoneEncount( zoneId );
  m_pPlacementDataManager->CreateZoneMapJump( zoneId );
  m_pPlacementDataManager->CreateZoneModelWork( zoneId );
  m_pPlacementDataManager->CreateZoneModelResource( zoneId );
  m_pPlacementDataManager->CreateZonePos( zoneId );
  m_pPlacementDataManager->CreateZoneTrainerWork( zoneId );
  m_pPlacementDataManager->CreateZoneTrainerResource( zoneId );
  m_pPlacementDataManager->CreateZoneStaticModel( zoneId );
  m_pPlacementDataManager->CreateZoneGimmickEncount( zoneId );
  m_pPlacementDataManager->CreateZoneGimmickEncountResource( zoneId );
  m_pPlacementDataManager->CreateZoneItem( zoneId );
  m_pPlacementDataManager->CreateDropItem();
  m_pPlacementDataManager->CreateZonePokeFinder( zoneId );
  m_pPlacementDataManager->CreateZoneBerryPointWork(zoneId);
  m_pPlacementDataManager->CreateZoneBerryPointResource(zoneId);
  m_pPlacementDataManager->CreateZoneFishingSpotWork(zoneId);
  m_pPlacementDataManager->CreateZoneFishingSpotResource(zoneId);
  m_pPlacementDataManager->CreateZonePushRockWork(zoneId);
  m_pPlacementDataManager->CreateZonePushRockResource(zoneId);
}

/**
 *  ゾーンチェンジ　往来の破棄
 */
void Fieldmap::updateZone_TrafficDelete()
{
  Field::TrafficManager *pTrafficManager = GetTrafficManager();
  if(pTrafficManager)
  {
    pTrafficManager->UnRegistActor();
  }
}

/**
 *  ゾーンチェンジ　往来の生成
 */
void Fieldmap::updateZone_TrafficCreate()
{
  // 遅延解放
  Field::TrafficManager *pTrafficManager = GetTrafficManager();
  if(pTrafficManager)
  {
    // アクターとパスを破棄する
    pTrafficManager->DeleteZoneActor();
    pTrafficManager->DeleteZonePath();

    pTrafficManager->CreateZonePath( GetDeviceHeap(), m_reqZoneId, m_pPlacementDataManager );
    pTrafficManager->Request_ReloadActor();
  }
} 

/**
 *  @brief  環境音マネージャーの更新
 */
void Fieldmap::updateEnvSoundManager()
{
  MoveModel::FieldMoveModelPlayer* pPlayerCharacter = reinterpret_cast<MoveModel::FieldMoveModelPlayer*>(GetPlayerCharacter());
  gfl2::math::Vector3  controlPos = m_playerPosition;
  gfl2::math::Quaternion controlRotate = m_playerQuaternion;
  gfl2::math::Quaternion controlCameraRotate = m_pCameraManager->GetMainViewCamera()->GetBaseCamera()->GetRotationQuat();

  bool isPokeRide = (pPlayerCharacter->GetOnMoveModel() != NULL);

  m_pFieldResident->GetEnvSoundManager()->Update( controlPos, controlRotate, controlCameraRotate, isPokeRide, static_cast<FieldEnvSoundAccessor::EnvSoundTimeZone>(GameSys::TimeZone::GetNowTimeZone()));
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

// @class インスタンス破棄処理のvisitor
class ExecuteCountVisitor
{
public:

  ExecuteCountVisitor(){ m_Count = 0; }
  virtual ~ExecuteCountVisitor(){}

  void Visit( gfl2::renderingengine::scenegraph::DagNode* pNode )
  {
    m_Count ++;
  }

  int m_Count;
private:

};


/**
 *  @brief 3DModelInstanceのインスタンス破棄処理
 */
void Fieldmap::Field3DObjectNodeExecuteTerminate()
{
  gfl2::renderingengine::scenegraph::Traverser::Visitor<IField3DObjectNode, ExecuteTerminateVisitor> visitor;
  gfl2::renderingengine::scenegraph::Traverser::TraverseFast<IField3DObjectNode, ExecuteTerminateVisitor>( GetFieldRootNode()->GetInstanceRootNode(), &visitor );


#if PM_DEBUG
#if 0
  // ノード数カウント
  {
    gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::DagNode, ExecuteCountVisitor> l_visitor;
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseFast<gfl2::renderingengine::scenegraph::DagNode, ExecuteCountVisitor>( &l_visitor );

    TOMOYA_PRINT( " node count %d\n", l_visitor.m_Count );
  }
#endif
#endif // PM_DEBUG
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
    INITIALIZE_00,
    INITIALIZE_00_00,
    INITIALIZE_00_WAIT,
    INITIALIZE_01_WAIT,
    INITIALIZE_02_WAIT,
    INITIALIZE_03_WAIT,
    INITIALIZE_END,
  };

  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_START = INITIALIZE_END,
    TICK_MOVEMODEL_TICK,
    TICK_SUBSCREEN_TICK,
    TICK_ONE_TICK,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(FieldInitialize,TICK_MAX);

  if((*p_seq) != INITIALIZE_AREA)
  {
    GFL_DEBUG_TICK_START(FieldInitialize, TICK_ONE_TICK, "Field::Initialize ONE");
  }



  switch( *p_seq )
  {
  case INITIALIZE_AREA:
    {
      GFL_DEBUG_TICK_SET_FLAG(FieldInitialize,DEBUG_DRAW_TICK_TOMOYA);
      GFL_DEBUG_TICK_RESET(FieldInitialize);

      GFL_DEBUG_TICK_START(FieldInitialize, TICK_ONE_TICK, "Field::Initialize ONE");

      GFL_DEBUG_TICK_START(FieldInitialize, INITIALIZE_AREA, "Field::Initialize INITIALIZE_AREA");

#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "Fieldmap Initialize Start" );
#endif 

      // ワールド取得
      m_pWorld = m_pGameManager->GetGameData()->GetFieldWorld();
      // エリア取得
      m_pArea = m_pGameManager->GetGameData()->GetFieldArea();


      // レンダリングパイプラインの初期化
      InitializeRenderingPipeLine();

      (*p_seq)++;

      GFL_DEBUG_TICK_END(FieldInitialize, INITIALIZE_AREA);
      GFL_DEBUG_TICK_START(FieldInitialize, INITIALIZE_00, "Field::Initialize INITIALIZE_00");

      //break; through
    } // case INITIALIZE_AREA

  case INITIALIZE_00:
    {
      b32 enterFlag( true );
      
      u64 start_us = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
      u64 now_us = 0;
      s32 count = 0;

      const u64 NEXT_FRAME_US = 20000;
      const s32 NEXT_FRAME_COUNT = 30;

      do 
      {
        enterFlag = ( true );

        if( !WaitInitializeRenderingPipeLine() ){
          enterFlag = false;
        }


        now_us = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
        now_us -= start_us;

        count ++;

        if( (enterFlag == false) && (now_us < NEXT_FRAME_US) && (count <NEXT_FRAME_COUNT) ) // 継続ならスリープ
        {
          gfl2::thread::ThreadStatic::CurrentSleep(4);  /// Sleep
        }

      }while( (enterFlag == false) && (now_us < NEXT_FRAME_US) && (count <NEXT_FRAME_COUNT) );  // 合計時間がNEXT_FRAME_US usを超えたら次に行く。

      // アトリビュートパラメーター初期化
      {
        void* pAttrParameterTableData = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__ATTRIBUTE_PARAMTER_TABLE_BIN);
        Attribute::Initialize( pAttrParameterTableData);
      }

#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "AttributeInitialized" );
#endif 

      // アクションコマンドの作成
      {
        m_pFieldActionComandManager = GFL_NEW(m_pFieldDeviceHeap) Field::MoveModel::FieldActionCommandManager();
        m_pFieldActionComandManager->Initialize(m_pFieldDeviceHeap);
      }

      // コリジョンシステムの作成
      {
        // 地面用コリジョンシーンの作成
        m_pCollsionSceneForTerrain = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TERRAIN );
        // 配置モデル用コリジョンシーンの作成
        m_pCollsionSceneForStaticModel = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_STATIC_MODEL );
        // 往来モデル用コリジョンシーンの作成
        m_pCollsionSceneForTrafficModel = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAFFIC_MODEL );
        // 波乗り用コリジョンシーンの作成
        m_pCollsionSceneForOshikaeshiNaminori = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_NAMINORI );
        // 地面馬用コリジョンシーンの作成
        m_pCollsionSceneForOshikaeshiRoba2 = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_ROBA2 );
        // ライド進入禁止用コリジョンシーンの作成
        m_pCollsionSceneForOshikaeshiRide = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_RIDE );
      }

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
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_INTRPOS ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAINER_EYE ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_NAMINORI ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT_ACTION ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_FINDER ),
          m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_FISHING_POS )
        };

        // ジョインフェスタの場合は、専用イベントチェックに切り替える
        if (IsJoinFestaGimmick() == true || IsBattleFesGimmick() == true)
        {
          desc.mode = FieldEventCheck::CHECKMODE_JOIN_FESTA;
        }

        m_pEventCheck = GFL_NEW(m_pFieldHeap) FieldEventCheck();
        m_pEventCheck->Initialize( desc ); 
      }

#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "m_pEventCheck Initialize End" );
#endif 

      // ダウジング
      {
        m_pDowsingTool = GFL_NEW(m_pFieldHeap) Dowsing::Tool();
        Dowsing::Tool::InitializeDescription initDesc;
        initDesc.m_pFieldEventCheck = m_pEventCheck;

        m_pDowsingTool->Initialize( initDesc );
      }

      (*p_seq)++;
    }
    break;

  case INITIALIZE_00_00:
    {
      // レンダリングパイプラインの初期化待ち
      if( WaitInitializeRenderingPipeLine() == false ){

        TOMOYA_PRINT( "WaitInitializeRenderingPipeLine not end\n" );
        break;
      }
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "RenderingPipeLineInitialized" );
#endif 

#if PM_DEBUG
      InitializeDeubgRtti();
#endif // PM_DEBUG
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "InitializeDeubgRtti End" );
#endif 

      // 地形システムを生成
      {
#if ENABLE_TERRAIN
        InitializeTerrain();
#endif
#if PM_DEBUG
        Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "Terrain Initialize End" );
#endif 
      }

      /// UIViewManager よりも後に作られなくてはならない.
      m_pFieldWindowManager = GFL_NEW(m_pFieldHeap) Field::FieldWindow::FieldWindowManager( m_pFieldHeap, m_pAppRenderingHeap, m_pAppRenderingManager,m_pGameManager->GetAsyncFileManager() );
      GFL_ASSERT( m_pFieldWindowManager );
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "FieldWindowManager Initialize End" );
#endif 

      // 動作モデル初期化
      InitializeMoveModel();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "InitializeMoveModel Initialize End" );
#endif 

#if ENABLE_STATIC_MODEL
      // 配置モデルの初期化
      InitializeStaticModel();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "InitializeStaticModel Initialize End" );
#endif 
#endif
      //暗黒天球対象ゾーンチェックし、フラグセット
      SetSkyboxDarkFlag(); //@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >> Mcatコメント内有るaの案件対処

      // 天球生成開始
      StartCreateSkybox();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "StartCreateSkybox Initialize End" );
#endif 

      // カメラ生成
      InitializeCamera();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "InitializeCamera Initialize End" );
#endif 

      // エンカウント生成
      {
        m_pEncount = GFL_NEW( m_pFieldHeap) Encount::Encount( this, m_pArea->GetAccessor( Area::AREA_DATA_IDX_ENCOUNT_DATA) );
        // @fix GFNMCat[1222][1223] シンボルエンカウントデータを読み込む
        m_pEncount->Initialize( m_pFieldHeap);
      }
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "Encount Initialize End" );
#endif 

      //アクションボタンマネージャ生成
      {
        m_pActionButtonManager = GFL_NEW( m_pFieldHeap) ActionButton::ActionButtonManager;
        m_pActionButtonManager->Initialize(m_pFieldHeap, GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager() );
      }
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "m_pActionButtonManager Initialize End" );
#endif 

      {//フィールドサブスクリーン
        m_pSubScreen = m_pFieldResident->GetFieldSubScreenProcManager();

        // ジョインフェスタの場合、ジョインフェスタ下画面を起動する
        if (IsJoinFestaGimmick() == true || IsBattleFesGimmick() == true)
        {
          m_pSubScreen->CallWindow( Field::SubScreen::WIN_JOIN_FESTA );
        } else {
          m_pSubScreen->CallWindow( Field::SubScreen::WIN_MENU_MAIN );
        }
      }
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "m_pSubScreen Initialize End" );
#endif
      
      // キャラクターモデルのリソース読み込み開始
      StartLoadMoveModel();

      (*p_seq)++;

      GFL_DEBUG_TICK_END(FieldInitialize, INITIALIZE_00);
      GFL_DEBUG_TICK_START(FieldInitialize, INITIALIZE_00_WAIT, "Field::Initialize INITIALIZE_00_WAIT");
      break;
    }

  case INITIALIZE_00_WAIT:
    {
      b32 enterFlag( true );

      u64 start_us = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
      u64 now_us = 0;
      s32 count = 0;

      const u64 NEXT_FRAME_US = 20000;
      const s32 NEXT_FRAME_COUNT = 30;

      do 
      {
        enterFlag = ( true );
      
        // 動作モデル　リソース読み込み待ち
        if( !WaitLoadMoveModel() )
        {
          TOMOYA_PRINT( "WaitLoadMoveModel not end\n" );
          enterFlag = false;
        }
        // エンカウント初期化待ち
        // @fix GFNMCat[1222][1223] シンボルエンカウントデータ読み込み待ち
        if( m_pEncount->WaitInitialize() == false)
        {
          TOMOYA_PRINT( "Encount::WaitInitialize not end\n");
          SAITA_PRINT( "Encount::WaitInitialize not end\n");
          enterFlag = false;
        }

        // 下画面初期化待ち
        if( !m_pSubScreen->IsChangeComplate() )
        {
          GFL_DEBUG_TICK_START(FieldInitialize, TICK_SUBSCREEN_TICK, "Field::Initialize TICK_SUBSCREEN_TICK");
          m_pSubScreen->Update();
          GFL_DEBUG_TICK_END(FieldInitialize, TICK_SUBSCREEN_TICK);
          TOMOYA_PRINT( "m_pSubScreen->IsChangeComplate() not end tick %d\n", GFL_DEBUG_TICK_US_GET( FieldInitialize, TICK_SUBSCREEN_TICK ) );
        }

        // Skyboxの生成を実行
        UpdateCreateSkybox();

        now_us = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
        now_us -= start_us;

        count ++;

        if( (enterFlag == false) && (now_us < NEXT_FRAME_US) && (count <NEXT_FRAME_COUNT) ) // 継続ならスリープ
        {
          gfl2::thread::ThreadStatic::CurrentSleep(4);  /// Sleep
        }

      }while( (enterFlag == false) && (now_us < NEXT_FRAME_US) && (count <NEXT_FRAME_COUNT) );  // 合計時間がNEXT_FRAME_US usを超えたら次に行く。

      if( !enterFlag )
      {
        break;
      }

#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "WaitLoadMoveModel Initialize End" );
#endif 

      Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();

      // 動作モデル　リソースセットアップ
      SetupMoveModel();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "SetupMoveModel Initialize End" );
#endif 

      // エフェクトシステム初期化
#if ENABLE_EFFECT
      InitializeEffectSystem();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "InitializeEffectSystem Initialize End" );
#endif //PM_DEBUG
#endif

      // 追加エフェクトのRoモジュール読込
      // @fix ラプラスライド中画面復帰時にエフェクトモジュールが読み込まれる前にラプラスライド用エフェクトが再生されてエラー。対処内容→モジュール読込箇所をギミック初期化直前からエフェクトシステム初期化直後へ変更
      InitializeEffectDll();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog(m_pFieldDeviceHeap, "InitializeEffectDll Initialize End");
#endif 

      // プレイヤーの作成
      CreatePlayer();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "CreatePlayer Initialize End" );
#endif 

    //天候マネージャ生成
    {
      m_pWeatherManager = GFL_NEW( m_pFieldHeap) weather::WeatherManager;
      weather::WeatherManager::InitParam param;
      param.pHeap = m_pFieldHeap;
      param.pEffectManager = m_pEffectManager;
      param.pCameraManager = m_pCameraManager;
      param.pCollisionSceneContainer = m_pTerrainManager->GetCollsionScene();
      m_pWeatherManager->Initialize(param);
    }
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "m_pWeatherManager Initialize End" );
#endif

#if USE_RTTI_DEBUG
      // RTTIエディタにメンバ情報を表示するリクエストを出す
      pFieldCharaModelManager->CreateRttiEditorPage( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "CreateRttiEditorPage Initialize End" );
#endif 
#endif

      // ライド状態への復帰
      StartRecoverPlayerRide();

      // サブオブジェクトの復帰
      StartRecoverSubobject();

      (*p_seq)++;
      GFL_DEBUG_TICK_END(FieldInitialize, INITIALIZE_00_WAIT);
      GFL_DEBUG_TICK_START(FieldInitialize, INITIALIZE_01_WAIT, "Field::Initialize INITIALIZE_01_WAIT");
    }
    //break; through

  case INITIALIZE_01_WAIT:
    {
      // ライドとサブオブジェクトのセットアップ完了待ち。
      {
        bool result1 = WaitRecoverPlayerRide();
        bool result2 = WaitRecoverSubobject();
        bool result3 = WaitTerrainSystemInitialize();
        bool result4 = InitializeWaitEffectSystem();

        // 下画面初期化待ち
        if( !m_pSubScreen->IsChangeComplate() )
        {
          GFL_DEBUG_TICK_START(FieldInitialize, TICK_SUBSCREEN_TICK, "Field::Initialize TICK_SUBSCREEN_TICK");
          m_pSubScreen->Update();
          GFL_DEBUG_TICK_END(FieldInitialize, TICK_SUBSCREEN_TICK);
          TOMOYA_PRINT( "m_pSubScreen->IsChangeComplate() not end tick %d\n", GFL_DEBUG_TICK_US_GET( FieldInitialize, TICK_SUBSCREEN_TICK ) );
        }

        // Skyboxの生成を実行
        UpdateCreateSkybox();

        if( !result1 || !result2 || !result3 || !result4 )
        {
          TOMOYA_PRINT( "01Wait %d %d %d %d not end\n", result1, result2, result3, result4 );
          break;
        }
      }
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "RecoverPlayerRide Initialize End" );
#endif 

      Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();

      // プレイヤーをカメラシステムにターゲットとして登録
      {
        Field::MoveModel::FieldMoveModelPlayer * pPlayerCharacter = this->GetPlayerCharacter();
        m_pCameraTarget = GFL_NEW( m_pFieldHeap ) Camera::Target::TargetTypeMoveModelPlayer( m_pFieldHeap, pPlayerCharacter );

        m_pCameraManager->ChangeCameraTarget( m_pCameraTarget );
        m_pCameraManager->SetUpForNormalMap();
      }

      // ギミックエンカウントマネージャー取得
      {
        m_pGimmickEncountManager = m_pFieldResident->GetGimmickEncountManager();
      }

      // 配置
      CreatePlacementData();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "CreatePlacementData Initialize End" );
#endif 

      // 往来
      CreateTraffic();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "CreateTraffic Initialize End" );
#endif 

      // TrialModel
      {
        m_pTrialModelFactory = GFL_NEW( m_pFieldHeap ) TrialModel::TrialModelFactory();
        Field::TrialModel::TrialModelFactory::SetupParam prm;
        prm.pFieldmap       = this;
        prm.pPipeLine       = m_pRenderingPipeLine;
        prm.pEffectManager  = m_pEffectManager;
        prm.pColScene       = m_pTerrainManager->GetCollsionScene();
        prm.pCameraManager  = m_pCameraManager;
        prm.pTrialShadow    = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__SHADOW_TRIAL_BIN );
        prm.pFinderShadow   = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__SHADOW_FINDER_BIN );
        m_pTrialModelFactory->Setup( prm );
      }
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "m_pTrialModelFactory Initialize End" );
#endif 

      // ドロップアイテム生成モジュール初期化
      InitializeDropItemCreator();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "InitializeDropItemCreator Initialize End" );
#endif 

      // ギミック初期化
      InitializeGimmick();

      // Highモデル制御点を設定
#if ENABLE_TERRAIN
      SetupBlockControlPositionTerrain();
#endif

      // 育て屋初期化
      CreateSodateya();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "Sodateya Initialize End" );
#endif 

      // プレイヤー陰影初期化
      InitializePlayerShade();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog(m_pFieldDeviceHeap, "InitializePlayerShade Initialize End");
#endif 

      (*p_seq)++;
      GFL_DEBUG_TICK_END(FieldInitialize, INITIALIZE_01_WAIT);
      GFL_DEBUG_TICK_START(FieldInitialize, INITIALIZE_02_WAIT, "Field::Initialize INITIALIZE_02_WAIT");
      break;
    } // case INITIALIZE_01_WAIT

  case INITIALIZE_02_WAIT:
    {
      b32 enterFlag( true );
      u64 start_us = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
      u64 now_us = 0;
      s32 count = 0;

      const u64 NEXT_FRAME_US = 20000;
      const s32 NEXT_FRAME_COUNT = 30;

      do 
      {
        enterFlag = ( true );

  #if ENABLE_TERRAIN
        // Highモデル制御点を設定
        {
          if( UpdateSetUpTerrain() )
          {
            TOMOYA_PRINT( "UpdateSetUpTerrain not end\n" );
            enterFlag = false;
          }
        }
  #endif
        // スカイボックス待ち
        if( !UpdateCreateSkybox())
        {
          TOMOYA_PRINT( "UpdateCreateSkybox not end\n" );
          enterFlag = false;
        }

        // 下画面初期化待ち
        if( !m_pSubScreen->IsChangeComplate() )
        {
          GFL_DEBUG_TICK_START(FieldInitialize, TICK_SUBSCREEN_TICK, "Field::Initialize TICK_SUBSCREEN_TICK");
          m_pSubScreen->Update();
          GFL_DEBUG_TICK_END(FieldInitialize, TICK_SUBSCREEN_TICK);
          TOMOYA_PRINT( "m_pSubScreen->IsChangeComplate() not end tick %d\n", GFL_DEBUG_TICK_US_GET( FieldInitialize, TICK_SUBSCREEN_TICK ) );
          enterFlag = false;
        }

        // ギミック初期化
        if( !UpdateCreateGimmick() )
        {
          TOMOYA_PRINT( "UpdateCreateGimmick not end\n" );
          enterFlag = false;
        }

        now_us = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
        now_us -= start_us;

        count ++;

        if( (enterFlag == false) && (now_us < NEXT_FRAME_US) && (count <NEXT_FRAME_COUNT) ) // 継続ならスリープ
        {
          gfl2::thread::ThreadStatic::CurrentSleep(2);  /// Sleep
        }

      }while( (enterFlag == false) && (now_us < NEXT_FRAME_US) && (count <NEXT_FRAME_COUNT) );  // 合計時間がNEXT_FRAME_US usを超えたら次に行く。


      if( enterFlag )
      {
        (*p_seq)++;
#if PM_DEBUG
        Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "INITIALIZE_01_WAIT Initialize End" );
#endif 

        // break;
      }
      else
      {
        break;
      }

      GFL_DEBUG_TICK_END(FieldInitialize, INITIALIZE_02_WAIT);
      GFL_DEBUG_TICK_START(FieldInitialize, INITIALIZE_03_WAIT, "Field::Initialize INITIALIZE_03_WAIT");
    } //  case INITIALIZE_WAIT:


  case INITIALIZE_03_WAIT:
    {
      CreateSkyboxAndEnvManager();
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "CreateSkyboxAndEnvManager Initialize End" );
#endif
      //天候の初リクエスト消化
      m_pWeatherControl->ChangeWeatherFast(m_ZoneId);
      // 下画面初期はサスペンドしておく
      m_pSubScreen->Suspend();

      GFL_DEBUG_TICK_END(FieldInitialize, INITIALIZE_03_WAIT);
      GFL_DEBUG_TICK_START(FieldInitialize, INITIALIZE_END, "Field::Initialize INITIALIZE_END");
      (*p_seq)++;
    }
    //break; through

  case INITIALIZE_END:
    {
      // @fix GFNMCat[1187] キャラの最大表示数をリソースから可変にする
      ZoneDataAccessor zoneDataAccessor( m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData(m_ZoneId) );
      if( zoneDataAccessor.IsMoveModelDrawNumControl() ) { GetMoveModelManager()->SetDispMaxCnt(Field::MoveModel::FieldMoveModelManager::DEFAULT_DISP_MAX_CNT); }
      else                                               { GetMoveModelManager()->SetDispMaxCnt(Field::MoveModel::FIELD_MOVE_MODEL_MAX); }

      // 往来のカリング優先順位を設定
      GetMoveModelManager()->SetTrafficCullingPriority( zoneDataAccessor.IsDispLowPriorityForTraffic() ? Field::MoveModel::FieldMoveModelManager::TRAFFIC_CULLING_PRIORITY_HIGH : Field::MoveModel::FieldMoveModelManager::TRAFFIC_CULLING_PRIORITY_MIDDLE );

      // エンカウント草用のブロックとグリッド座標を更新
      // @fix GFNMCat[2031] フィールド再生成時、フィールドモデルが草むらにいた場合、モデル再配置に影響から一瞬草むらが揺れてしまう
      for( u32 i=0 ; i<Field::MoveModel::FIELD_MOVE_MODEL_MAX ; i++ )
      {
        if( GetMoveModelManager()->GetFieldMoveModel(i)
         && GetMoveModelManager()->GetFieldMoveModel(i)->IsSetup() )
        {
          GetMoveModelManager()->GetFieldMoveModel(i)->EncountGrassCreaterUpdateBlockAndGridPos();
        }
      }

      m_pWeatherManager->Update();
      if ( m_pWeatherControl->IsChanging() )
      {
        break;
      }

      GFL_DEBUG_TICK_END(FieldInitialize, INITIALIZE_END);

      GFL_DEBUG_TICK_END(FieldInitialize, TICK_ONE_TICK);
      //GFL_RELEASE_PRINT( "!!Initialize E Seq %d time %d us\n", *p_seq, GFL_DEBUG_TICK_US_GET(FieldInitialize, TICK_ONE_TICK) );

#if PM_DEBUG
      {
        gfl2::heap::HeapBase * pDebugHeapDevice = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
        Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
        m_pFieldDebugMenu = GFL_NEW( pDebugHeapDevice ) Field::Debug::FieldDebugMenu( pDebugHeapDevice, pFieldCharaModelManager , m_pFieldEnvManager, m_pEffectManager, NULL, m_pWeatherControl, GetTrafficManager() ); 
        // m_pFieldDebugMenu->SetupTrafficManager( GetTrafficManager() );

        m_pDebugInfoWin = GFL_NEW( pDebugHeapDevice ) Field::DebugInfoWin();
        m_pDebugInfoWin->Initialize( this );
      }
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "DebugMenu->SetupTrafficManager Initialize End" );
#endif

#if defined(DEBUG_ONLY_FOR_takahashi_tomoya)
#if DEBUG_DRAW_TICK_TOMOYA
    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(FieldInitialize);
    gfl2::debug::ResetPrintFlag();
#endif
#endif
    
      return true;
    }

  } // switch( *p_seq )

  GFL_DEBUG_TICK_END(FieldInitialize, TICK_ONE_TICK);
  //GFL_RELEASE_PRINT( "!!Initialize E Seq %d time %d us\n", *p_seq, GFL_DEBUG_TICK_US_GET(FieldInitialize, TICK_ONE_TICK) );
  return false;
}

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
  Field::FieldSound* pFieldSound = m_pGameManager->GetGameData()->GetFieldSound();
  switch( *p_seq )
  {
  case INITIALIZE:
    {
      //リバーブ
      ZoneDataAccessor zoneDataAccessor( m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_ZoneId ) );
      bool reverb = zoneDataAccessor.IsReverbEnable();
      if( reverb ){
        pFieldSound->SetReverb(m_pFieldHeap);
      }
      //足音データ
      pFieldSound->LoadFootSoundData( m_pFieldHeap );
      //スクリプトシステム
      pFieldScriptSystem->Create( m_pFieldHeap, m_pGameManager->GetAsyncFileManager()); 
      (*p_seq) ++;
    }
    return false;
  case INITIALIZE_WAIT:
    {
      pFieldScriptSystem->UpdateCreate();

      if( !pFieldScriptSystem->IsCreated() ||
          !pFieldSound->IsLoadedFootSoundData()){
        return false;
      }

      if( m_Desc.openMode == FM_OPEN_MODE_INIT ){
        pFieldScriptSystem->CallFieldInit( m_pArea->GetAccessor( Area::AREA_DATA_IDX_INIT_SCRIPT_RESOURCE ),
          m_pWorld->GetZoneIndexInArea(static_cast<ZoneID>(m_Desc.playerLocation.zoneId)), m_pFieldHeap );
      }else{
        pFieldScriptSystem->CallFieldRecover( m_pArea->GetAccessor( Area::AREA_DATA_IDX_INIT_SCRIPT_RESOURCE ),
          m_pWorld->GetZoneIndexInArea(static_cast<ZoneID>(m_Desc.playerLocation.zoneId)), m_pFieldHeap );
      }
      //セーブ復帰フラグを折る
      pFieldScriptSystem->SetContinueRecoverFlag(false);

#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "pFieldScriptSystem Initialize End" );
#endif 
    }
    break;
  }
  return true;
}

/**
 *  @brief  画面表示が可能な状態になるのを待つ
 *  @param  p_seq
 */
bool Fieldmap::State_Ready( int* p_seq )
{
  // プレイヤー座標セット
  Field::MoveModel::FieldMoveModelPlayer* pPlayerCharacter = this->GetPlayerCharacter();
  GFL_ASSERT_STOP( pPlayerCharacter );
  m_playerPosition   = pPlayerCharacter->GetCharaDrawInstance()->GetPosition();
  m_playerQuaternion = pPlayerCharacter->GetCharaDrawInstance()->GetRotationQuat();

  // 入力変換機能、開始 
  m_pCameraUiDeviceTranslator->Ready();

  // 追従座標更新、開始
  m_pCameraTarget->Ready();

  // カメラ、開始
  m_pCameraManager->Ready();

  // ギミック準備完了
  ReadyGimmick();

  // 配置モデル準備
  ReadyStaticModel();

  // ギミック準備完了（StaticModelの準備後用）
  ReadyGimmickAfter();

  // GFNMCat[4179] フィールド表示前にEnv関連の準備をする
  ReadyEnvManager();
  
  // 古いクリアレンダリングターゲットのセッティングを保存
  TempOldClearRenderTargetSetting();

  m_ReadyFlag = true;

#if PM_DEBUG
  GFL_PRINT("[FLD START]SceneGraphManager NodeContainer =\"%d/%d\"\n", 
    gfl2::renderingengine::scenegraph::SceneGraphManager::GetNodeCount(),
    gfl2::renderingengine::scenegraph::SceneGraphManager::GetNodeMax()
    );

  // 常駐ヒープ監視開始
  m_pGameManager->GetGameEventManager()->SetDebugChkHeapData(true);

#endif

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)

  Debug::DebugHeapLog::GetInstance()->AddLog( m_pFieldDeviceHeap, "Ready End" );

#if defined(DEBUG_ONLY_FOR_takahashi_tomoya)
  Debug::DebugHeapLog::GetInstance()->ShowLog();
#endif // defined(DEBUG_ONLY_FOR_takahashi_tomoya)

  m_pArea->DebugCheckResourceSize();
#endif  // defined(GF_PLATFORM_CTR)
#endif  // PM_DEBUG

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
  Debug::DebugTypes::s_DebugFieldEndTick = nn::os::Tick::GetSystemCurrent();
  m_pDebugInfoWin->StartSetupTimeDisp();
#endif
#endif

  m_pGameManager->SetVSyncSkipFlag( false ); // VSYNC待ち復帰
  return true;
}

/**
 *  @brief  レンダリングしないフレームを1フレーム作る
 *  @param  p_seq
 */
bool Fieldmap::State_RenderSkip( int* p_seq )
{
  //GFL_RELEASE_PRINT( "State_RenderSkip seq %d\n", *p_seq );
  switch(*p_seq)
  {
  case 0:
    // 
    TerminateReqPlacementData();

    // 念のためモーションブラーを停止
    m_pRenderingPipeLine->GetMotionBlurPath()->RenderOff();

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
  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_DROPITEM,
    TICK_GIMMICK,
    TICK_TRIAL_MODEL,
    TICK_ACTION_BUTTON,
    TICK_MMECREATER,
    TICK_SCRIPT,
    TICK_EFFECT_SYSTEM,
    TICK_TRAFFIC,
    TICK_CAMERA_TARGET,
    TICK_ENCOUNT,
    TICK_CAMERAMANAGER,
    TICK_PLACEMENTDATA,
    TICK_SKYBOX_ENVMAN,
    TICK_MOVEMODEL,
    TICK_STATICMODEL,
    TICK_TERRAIN,
    TICK_EVENTCHECK,
    TICK_WINDOWMAN,
    TICK_PIPELINE,
    TICK_DEBUG_RTTI,
    TICK_WEATHER,
    TICK_SODATEYA,
    TICK_PLAYER_SHADE,
    TICK_EFFECT_DLL,
    TICK_STAFFROLL,

    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(FieldTerminate,TICK_MAX);

  enum
  {
    SEQ_TERMINATE_START,
    SEQ_TERMINATE_WAIT,
    SEQ_TERMINATE00,
    SEQ_TERMINATE01,
    SEQ_TERMINATE02,
    SEQ_TERMINATE03,
    SEQ_TERMINATE04,
    SEQ_TERMINATE04_WAIT,
  };

  //GFL_RELEASE_PRINT( "State_ManagerTerminate seq %d\n", *p_seq );

  switch( *p_seq )
  {
  case SEQ_TERMINATE_START:
    {
      TOMOYA_PRINT( "SEQ_TERMINATE_START\n" );
      m_pTerrainManager->TerminateReq();

      {
        Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();
        if( pFieldScriptSystem )  pFieldScriptSystem->FinalizeFieldmapTerminateTop();
      }
      ++(*p_seq);
      // break;
    }

  case SEQ_TERMINATE_WAIT:
    {
      TOMOYA_PRINT( "SEQ_TERMINATE_WAIT\n" );
      b32 enterFlag( true );

      //サブウインドウ破棄
    //  if(false == m_pSubScreen->Finalize()){
    //    TOMOYA_PRINT( "SubScreen Finalize false\n" );
    //    enterFlag = false;
    //  }
      // ギミック破棄
      if( !FinishGimmick() )
      {
        enterFlag = false;
      }
      // 地形の読み込みが一旦終わるまで待つ
      if( m_pTerrainManager->UpdateWaitTerminate() )
      {
        TOMOYA_PRINT( "Terrain Finalize false\n" );
        enterFlag = false;
      }

      if( enterFlag )
      {
        ++(*p_seq);
      }
      else
      {
        return false;
      }
    } // case SEQ_TERMINATE_WAIT:
    // break through

  case SEQ_TERMINATE00:
    {
      GFL_DEBUG_TICK_SET_FLAG(FieldTerminate,true);
      GFL_DEBUG_TICK_RESET(FieldTerminate);
      //-----------------------------------------------

      //レンダリングパイプラインが破棄可能になるのを待つ
      if( CanDestroyRenderingPipeLine() == false ){
        TOMOYA_PRINT( "AppRendering Finalize false\n" );
        break;
      }

      TOMOYA_PRINT( "TERMINATE00\n" );

#if PM_DEBUG
      GFL_SAFE_DELETE( m_pDebugInfoWin );
      GFL_SAFE_DELETE( m_pFieldDebugMenu );
#endif

      //天候マネージャの破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_WEATHER, "TICK_WEATHER");
      m_pWeatherManager->Terminate();
      GFL_SAFE_DELETE( m_pWeatherManager );
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_WEATHER);

      // 育て屋モジュール破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_SODATEYA, "Sodateya");
      DeleteSodateya();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_SODATEYA);

      // プレイヤーの陰影モジュール破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_PLAYER_SHADE, "TICK_PLAYER_SHADE");
      TerminatePlayerShade(); 
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_PLAYER_SHADE);

      // ドロップアイテム生成モジュール破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_DROPITEM, "DrawItem");
      TerminateDropItemCreator();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_DROPITEM);
  
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_STAFFROLL, "Staffroll");
      //エンディング用スタッフロールモジュールの破棄（無ければスルーする）
      TerminateFieldStaffroll();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_STAFFROLL);

      // ギミック破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_GIMMICK, "TICK_GIMMICK");
      TerminateGimmick();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_DROPITEM);

      // TrialModel
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_TRIAL_MODEL, "TICK_TRIAL_MODEL");
      GFL_SAFE_DELETE( m_pTrialModelFactory );
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_DROPITEM);

      //アクションボタンマネージャの破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_ACTION_BUTTON, "TICK_ACTION_BUTTON");
      m_pActionButtonManager->Terminate(m_pEffectManager);
      GFL_SAFE_DELETE( m_pActionButtonManager );
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_ACTION_BUTTON);

      // スクリプトエフェクトシステム破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_SCRIPT, "TICK_SCRIPT");
      {
        Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();
        if( pFieldScriptSystem )  pFieldScriptSystem->GetScriptEffectManager()->Terminate();
        if( pFieldScriptSystem )  pFieldScriptSystem->FinalizeFieldmapTerminateBottom();
      }
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_SCRIPT);

#if ENABLE_EFFECT
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_EFFECT_SYSTEM, "TICK_EFFECT_SYSTEM");
      // エフェクトシステム破棄
      TerminateEffectSystem();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_EFFECT_SYSTEM);
#endif

      // 追加エフェクトのRoモジュール破棄(エフェクトシステムの破棄完了後に破棄する）
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_EFFECT_DLL, "TICK_EFFECT_DLL");
      TerminateEffectDll();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_EFFECT_DLL);

      // 往来
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_TRAFFIC, "TICK_TRAFFIC");
      TerminateTraffic();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_TRAFFIC);

      // カメラターゲット破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_CAMERA_TARGET, "TICK_CAMERA_TARGET");
      GFL_SAFE_DELETE( m_pCameraTarget );
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_CAMERA_TARGET);

      // エンカウント破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_ENCOUNT, "TICK_ENCOUNT");
      // @fix GFNMCat[1222][1223] 保持しているシンボルエンカウントデータを破棄
      m_pEncount->Terminate();
      GFL_SAFE_DELETE( m_pEncount );
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_ENCOUNT);

      // カメラシステム破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_CAMERAMANAGER, "TICK_CAMERAMANAGER");
      GFL_SAFE_DELETE( m_pCameraUiDeviceTranslator );
      GFL_SAFE_DELETE( m_pCameraManager );
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_CAMERAMANAGER);

      // ギミックエンカウントマネージャー
      m_pGimmickEncountManager = NULL;

      // 配置情報破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_PLACEMENTDATA, "TICK_PLACEMENTDATA");
      TerminatePlacementData();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_PLACEMENTDATA);

      ++(*p_seq);
      //break; through

  case SEQ_TERMINATE01:

      TOMOYA_PRINT( "TERMINATE01\n" );

      // 動作モデルのはき
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_MOVEMODEL, "TICK_MOVEMODEL");
      TerminateMoveModel();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_MOVEMODEL);

      // アクションコマンドのはき
      m_pFieldActionComandManager->Terminate();
      GFL_SAFE_DELETE(m_pFieldActionComandManager);

      ++(*p_seq);
      // break; through

  case SEQ_TERMINATE02:

      TOMOYA_PRINT( "TERMINATE02\n" );
      // SkyboxとEnvManagerの破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_SKYBOX_ENVMAN, "TICK_SKYBOX_ENVMAN");
      TerminateSkyboxAndEnvManager();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_SKYBOX_ENVMAN);


      // 配置モデルの破棄
#if ENABLE_STATIC_MODEL
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_STATICMODEL, "TICK_STATICMODEL");
      TerminateStaticModel();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_STATICMODEL);
#endif

      ++(*p_seq);
      // break; through

  case SEQ_TERMINATE03:

      TOMOYA_PRINT( "TERMINATE03\n" );

      // 地形システムの破棄
#if ENABLE_TERRAIN
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_TERRAIN, "TICK_TERRAIN");
      TerminateTerrain();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_TERRAIN);
#endif  

      // ダウジング破棄
      GFL_SAFE_DELETE( m_pDowsingTool );

      // イベントチェック処理破棄    
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_EVENTCHECK, "TICK_EVENTCHECK");
      GFL_SAFE_DELETE( m_pEventCheck );
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_EVENTCHECK);

      GFL_DEBUG_TICK_START(FieldTerminate, TICK_WINDOWMAN, "TICK_WINDOWMAN");
      GFL_SAFE_DELETE( m_pFieldWindowManager );
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_WINDOWMAN);


      // レンダリングパイプラインの破棄
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_PIPELINE, "TICK_PIPELINE");
      TerminateRenderingPipeLine();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_PIPELINE);

#if PM_DEBUG
      GFL_DEBUG_TICK_START(FieldTerminate, TICK_DEBUG_RTTI, "TICK_DEBUG_RTTI");
      FinalineDebugRtti();
      GFL_DEBUG_TICK_END(FieldTerminate, TICK_DEBUG_RTTI);
#endif

#if defined( GF_PLATFORM_CTR )
      /*
      gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
      GFL_DEBUG_TICK_DUMP_ALL(FieldTerminate);
      gfl2::debug::ResetPrintFlag();
      */
#endif
      ++(*p_seq);
    }
    // break through

  case SEQ_TERMINATE04:
    {
      // ジョインフェスタオープン時とクロース時は次のフィールドで下画面が変わるため下画面破棄しておきます。
      if( m_CloseMode == FM_CLOSE_MODE_JFOPEN || m_CloseMode == FM_CLOSE_MODE_JFCLOSE )
      {
        // 下画面破棄リクエスト
        m_pSubScreen->DeleteWindowRequest();
      }
      ++(*p_seq);
    }
    // break through
  case SEQ_TERMINATE04_WAIT:
    {
      // ジョインフェスタオープン時とクロース時は次のフィールドで下画面が変わるため下画面破棄しておきます。
      if( m_CloseMode == FM_CLOSE_MODE_JFOPEN || m_CloseMode == FM_CLOSE_MODE_JFCLOSE )
      {
        // 下画面破棄待ち
        if( !m_pSubScreen->IsDeleteWindowComplete() )
        {
          return false;
        }
      }
      m_pSubScreen=NULL;
    }

    return true;
  }

  return false;
}


/**
 *  @brief  フィールドスクリプト関連の破棄
 *  @param  p_seq
 */
bool Fieldmap::State_ScriptTerminate( int* p_seq )
{
  //GFL_RELEASE_PRINT( "State_ScriptTerminate seq %d\n", *p_seq );

  {
    Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();
    pFieldScriptSystem->Delete();  
  }
  {
    Field::FieldSound* pFieldSound = m_pGameManager->GetGameData()->GetFieldSound();
    pFieldSound->ReleaseFootSoundData();
    pFieldSound->ResetReverb();
  }
  return true;
}


/**
 *  @brief ギミック　初期化
 */
void Fieldmap::InitializeGimmick()
{
  gfl2::heap::HeapBase       *pHeap    = m_pFieldDeviceHeap;
  gfl2::fs::AsyncFileManager *pFileMgr = m_pGameManager->GetAsyncFileManager();

  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    u32 gimmickID = GetFieldGimmickID( static_cast<GimmickIndex>( i ) );
    m_pGimmick[ i ] = FieldGimmickBuilder::CreateGimmick( pHeap, pFileMgr, this, gimmickID, i, GetZoneID(), m_pPlacementDataManager );
    if( m_pGimmick[ i ] ){ m_pGimmick[ i ]->Create(); }
  }

  // ポケセン・ジョインフェスタ以外ではカイリキ岩ギミックを作成
  u32 kairikiGimmickIndex = GIMMICK_IDX_MAX-1;
  if( !m_pGimmick[ kairikiGimmickIndex ] )
  {
    m_pGimmick[ kairikiGimmickIndex ] = FieldGimmickBuilder::CreateGimmick( pHeap, pFileMgr, this, GIMMICK_TYPE_KAIRIKY_ROCK, kairikiGimmickIndex , GetZoneID(), m_pPlacementDataManager );
    if( m_pGimmick[ kairikiGimmickIndex ] ){ m_pGimmick[ kairikiGimmickIndex ]->Create(); }
  }
}

/**
 *  @brief ギミック　生成更新処理
 */
b32 Fieldmap::UpdateCreateGimmick()
{
  bool bFinish = true;
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( !m_pGimmick[ i ] ) { continue; }
    m_pGimmick[ i ]->UpdateCreate();
    if( !m_pGimmick[ i ]->IsCreated() )
    {
      bFinish = false;
    }
  }

  return bFinish;
}

/**
 *  @brief ギミック　レディ処理
 */
void Fieldmap::ReadyGimmick()
{
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( !m_pGimmick[ i ] ) { continue; }
    m_pGimmick[ i ]->Ready();
  }
}

/**
 *  @brief ギミック　レディ処理
 */
void Fieldmap::ReadyGimmickAfter()
{
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( !m_pGimmick[ i ] ) { continue; }
    m_pGimmick[ i ]->ReadyAfter();
  }
}

/**
 *  @breif ギミック　破棄
 */
b32 Fieldmap::FinishGimmick()
{
  b32 bFinish = true;
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( !m_pGimmick[ i ] ){ continue; }
    if( !m_pGimmick[ i ]->Finalize() )
    {
      bFinish = false;
    }
  }

  return bFinish;
}

/**
 *  @brief ギミックの破棄
 */
void Fieldmap::TerminateGimmick()
{
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( !m_pGimmick[ i ] ){ continue; }
    gfl2::ro::Module *pModule = NULL;
    pModule = m_pGimmick[ i ]->GetModule();
    m_pGimmick[ i ]->Delete();
    GFL_SAFE_DELETE( m_pGimmick[ i ] );

    // モジュールのアンロード
    if( pModule )
    {
      gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
      roMgr->DisposeModule( pModule );
    }
  }
}

/**
 *  @brief ギミック更新　　タイミング：更新前 
 */
void Fieldmap::UpdateFirstGimmick()
{
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( m_pGimmick[ i ] ) { m_pGimmick[ i ]->UpdateFirst(); }
  }
}

/**
 *  @brief ギミック更新　　タイミング：更新後
 */
void Fieldmap::UpdateLastGimmick()
{
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( m_pGimmick[ i ] ) { m_pGimmick[ i ]->UpdateLast(); }
  }
}

/**
 *  @brief ギミック更新　　タイミング：トラバース直前
 */
void Fieldmap::UpdateTraverseBeforeGimmick()
{
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( m_pGimmick[ i ] ) { m_pGimmick[ i ]->TraverseBefore(); }
  }
}

/**
 *  @brief ギミック更新　　タイミング：Calculate後
 */
void Fieldmap::UpdateCalculateAfterGimmick()
{
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    if( m_pGimmick[ i ] ) { m_pGimmick[ i ]->CalculateAfter(); }
  }
}

/**
*  @brief ギミックがバトルフェスギミックかどうか
*/
bool Fieldmap::IsBattleFesGimmick()
{
  // ジョインフェスタシーンかどうか調べる
  b32 isExecuteBattleFes = false;
  for (u32 i = 0; i < GIMMICK_IDX_MAX; ++i)
  {
    u32 gimmickID = GetFieldGimmickID(static_cast<GimmickIndex>(i));
    if (gimmickID == Field::GIMMICK_TYPE_BATTLE_FES)
    {
      isExecuteBattleFes = true;
      break;
    }
  }
  return isExecuteBattleFes;
}

/**
 *  @brief ギミックがジョインフェスタギミックかどうか
 */
bool Fieldmap::IsJoinFestaGimmick()
{
  // ジョインフェスタシーンかどうか調べる
  b32 isExecuteJoinFesta = false;
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    u32 gimmickID = GetFieldGimmickID( static_cast<GimmickIndex>( i ) );
    if( gimmickID == Field::GIMMICK_TYPE_JOIN_FESTA
      || gimmickID == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM )
    {
      isExecuteJoinFesta = true;
      break;
    }
  }
  return isExecuteJoinFesta;
}


/**
 *  @brief ギミックがバトル施設ギミックかどうか
 */
bool Fieldmap::IsBattleInstGimmick()
{
  // ジョインフェスタシーンかどうか調べる
  b32 isExecuteBattleInst = false;
  for( u32 i = 0; i < GIMMICK_IDX_MAX; ++ i )
  {
    u32 gimmickID = GetFieldGimmickID( static_cast<GimmickIndex>( i ) );
    if( gimmickID == Field::GIMMICK_TYPE_BATTLE_INST )
    {
      isExecuteBattleInst = true;
      break;
    }
  }
  return isExecuteBattleInst;
}

/**
 *  @brief NPC動作モデルヒープ確保に変更が必要なゾーンか
 */
bool Fieldmap::IsNpcHeapLargeZone()
{
  b32 isHeapLargeZone = false;
  if (m_ZoneId == Z_F0701PH0802)
  {
    isHeapLargeZone = true;
  }
  return isHeapLargeZone;
}


/**
 *  @brief 育て屋　初期化
 */
void Fieldmap::CreateSodateya()
{
  m_pSodateya = GFL_NEW( m_pFieldHeap ) FieldSodateya(this);
}

/**
*  @brief 育て屋　破棄
*/
void Fieldmap::DeleteSodateya()
{
  GFL_SAFE_DELETE( m_pSodateya );
}

Field::SubScreen::FieldSubScreen* Fieldmap::GetSubScreen( void )
{
  return m_pGameManager->GetSubScreen();
}

/**
 *  @brief エフェクトシステム　初期化
 */
void Fieldmap::InitializeEffectSystem()
{
  // @fix GFNMCat[1374] サメハダーでダッシュ中に陸地に降りようとしたら操作不能になった。
  // パーティクル,エミッター数をエフェクトの使用状況から再設定
  static const u32 EMITTER_SET_NUM = 48;
  static const u32 EMITTER_NUM =  EMITTER_SET_NUM * 8; // 240
  // ---------
  static const u32 PARTICLE_NUM = 768; // 
  static const u32 STRIPE_NUM  = 1;

  {
        m_pEffectHeap = GFL_NEW( m_pFieldHeap ) gfl2::Effect::GFHeap( m_pFieldDeviceHeap );
        gfl2::Effect::Config effect_config;
        effect_config.SetEffectHeap( m_pEffectHeap );
#if defined(GF_PLATFORM_CTR)
        effect_config.SetParticleMemType( nw::eft::BUFFER_LOCATION_FCRAM );
#endif
        effect_config.SetEmitterNum( EMITTER_NUM );
        effect_config.SetEmitterSetNum( EMITTER_SET_NUM );
        effect_config.SetParticleNum( PARTICLE_NUM );
        effect_config.SetStripeNum(STRIPE_NUM);
        effect_config.SetResourceNum( Field::Effect::EFFECT_RESOURCE_MAX );

        m_pEffectSystem = GFL_NEW( m_pFieldHeap ) gfl2::Effect::System( effect_config );
        m_pEffectGroupIDList = GFL_NEW( m_pFieldHeap ) gfl2::util::List<u32>( m_pFieldHeap, 1 );
        m_pEffectGroupIDList->PushFront( 0 );

        {
          gfl2::Effect::EffectRenderPath::Config config;
          gfl2::Effect::EffectRenderPath *pEffRenderPath = NULL;
          config.m_pSystem        = m_pEffectSystem;
          config.m_cpGroupIDList  = m_pEffectGroupIDList;

          m_pFieldRenderingPipeLineManager->SetEffectRenderPathConfig( config);
          m_pFieldRenderingPipeLineManager->SetupEffectRenderPathConfig( FieldRenderingPipeLineManager::EXECUTE_TYPE_NORMAL);
        }

        m_pEffectManager = GFL_NEW( m_pFieldHeap ) Effect::EffectManager();
        gfl2::fs::BinLinkerAccessor *pAccessor = m_pArea->GetAccessor( Area::AREA_DATA_IDX_EFFECT_RESOURCE );
        m_pEffectManager->InitializeCommon( m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__EFFECT_COMMON_PACK ), m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__EFFECT_LAYOUT_PACK ) );
        Field::Effect::EffectManager::SetupParam param;
        param.pHeapBase = m_pFieldDeviceHeap;
        param.pPipeLine = m_pRenderingPipeLine;
        param.pResource = pAccessor;
        param.pRootNode = GetFieldRootNode();
        param.pCameraManager = m_pCameraManager;
        param.pWindowManager = m_pFieldWindowManager;
        param.pEffectHeap = m_pEffectHeap;
        param.pEffectSystem = m_pEffectSystem;
        param.pWordSetLoader = m_pFieldResident->GetWordSetLoader();
        param.pFileManager   = m_pGameManager->GetAsyncFileManager();
        param.cpEncountGrassColorTable = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__ENCOUNTCOLORTABLE_BIN );
        param.worldID        = m_pWorld->GetWorldID();
        m_pEffectManager->Initialize( param );


  }

  // 動作モデル用の影エフェクトを作成
  m_pFieldMoveModelShadowManger = GFL_NEW( m_pFieldHeap ) MoveModel::FieldMoveModelShadowManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager() , m_pEffectManager );
}

bool Fieldmap::InitializeWaitEffectSystem()
{
  return m_pEffectManager->InitializeWait();
}

/**
 *  @brief エフェクトシステム更新
 */
void Fieldmap::UpdateEffectSystem()
{
  // エフェクト更新
  m_pEffectSystem->BeginFrame();
  m_pEffectSystem->Calc( 0 );
  m_pEffectManager->Update();

  // 動作モデル用の影エフェクトを更新
  m_pFieldMoveModelShadowManger->UpdateShadow();
}

/**
 * @brief エフェクトシステム破棄
 */
void Fieldmap::TerminateEffectSystem()
{
  // 動作モデル用の影エフェクトを削除
  GFL_SAFE_DELETE( m_pFieldMoveModelShadowManger );

  m_pEffectManager->Terminate();
  GFL_SAFE_DELETE( m_pEffectManager );
  GFL_SAFE_DELETE( m_pEffectGroupIDList );
  GFL_SAFE_DELETE( m_pEffectSystem );
  GFL_SAFE_DELETE( m_pEffectHeap );
}


/**
 *  @brief 往来の生成
 */
void Fieldmap::CreateTraffic()
{
  Field::TrafficManager *pTrafficManager = GetTrafficManager();
  if(pTrafficManager)
  {
    Traffic::SetupCollision setup;
    setup.pCollisionSceneForTerrain         = m_pCollsionSceneForTerrain;
    setup.pCollisionSceneForStaticModel     = m_pCollsionSceneForStaticModel;
    setup.pCollisionSceneForTrafficModel    = m_pCollsionSceneForTrafficModel;
    setup.pCollisionSceneForEventPos        = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_POS );
    setup.pCollisionSceneForEventTalk       = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK );
    setup.pCollisionSceneForEventPush       = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH );
    setup.pCollisionSceneForGround          = m_pTerrainManager->GetCollsionScene();
    pTrafficManager->SetupCollisionScene( setup );
    pTrafficManager->CreateZonePath( GetDeviceHeap(), m_ZoneId, m_pPlacementDataManager );
    pTrafficManager->CreateZoneActorResource();
    pTrafficManager->Request_ReloadActor();
  
  }
}


/**
 *  @brief 往来の破棄
 */
void Fieldmap::TerminateTraffic()
{
  if(GetTrafficManager())
  {
    GetTrafficManager()->DeleteZoneActorResource();
    GetTrafficManager()->DeleteZonePath();
  }
}

/**
 *  @brief  配置情報の生成
 */
void Fieldmap::CreatePlacementData()
{
  PlacementDataManager::DescFieldmap desc;
  desc.pTerrainManager = m_pTerrainManager;
  desc.pRenderingPipeLine = m_pRenderingPipeLine;
  m_pPlacementDataManager = m_pFieldResident->GetPlacementDataManager();
  m_pPlacementDataManager->InitializeFieldmap(desc);
  m_pPlacementDataManager->CreateAreaMoveModelResource();
  m_pPlacementDataManager->CreateAreaTrainerResource();
  m_pPlacementDataManager->CreateAreaStaticModel();
  m_pPlacementDataManager->CreateZoneModelResource( m_ZoneId );
  m_pPlacementDataManager->CreateZonePos( m_ZoneId );
  m_pPlacementDataManager->CreateZoneActionEvent( m_ZoneId );
  m_pPlacementDataManager->CreateZoneMapJump( m_ZoneId );
  m_pPlacementDataManager->CreateZoneStaticModel( m_ZoneId );
  m_pPlacementDataManager->CreateZoneBgEvent( m_ZoneId );
  m_pPlacementDataManager->CreateZoneEncount( m_ZoneId );
  m_pPlacementDataManager->CreateZoneTrainerResource( m_ZoneId );
  m_pPlacementDataManager->CreateZoneItem( m_ZoneId );
  m_pPlacementDataManager->CreateDropItem();
  m_pPlacementDataManager->CreateAreaEnvSound(m_pFieldDeviceHeap);
  m_pPlacementDataManager->CreateZoneGimmickEncountResource(m_ZoneId);
  m_pPlacementDataManager->CreateZonePokeFinder( m_ZoneId );
  m_pPlacementDataManager->CreateZoneBerryPointResource(m_ZoneId);
  m_pPlacementDataManager->CreateZoneFishingSpotResource(m_ZoneId);
  m_pPlacementDataManager->CreateZonePushRockResource(m_ZoneId);
}

/**
 *  @brief  配置情報の破棄リクエスト発行
 */
void Fieldmap::TerminateReqPlacementData()
{
#if ENABLE_STATIC_MODEL
  // インスタンス破棄のリクエスト発行
  m_pPlacementDataManager->RequestDeleteAreaStaticModel();
  m_pPlacementDataManager->RequestDeleteZoneStaticModel();
  m_pPlacementDataManager->RequestDeleteDropItem();
  m_pPlacementDataManager->RequestDeleteZoneGimmickEncount();
  m_pPlacementDataManager->RequestDeleteZoneBerryPoint();
  m_pPlacementDataManager->RequestDeleteZoneFishingSpot();
#endif
}

/**
*  @brief 配置情報の破棄
*/
void Fieldmap::TerminatePlacementData()
{
  m_pPlacementDataManager->DeleteZonePushRockResource();
  m_pPlacementDataManager->DeleteZoneFishingSpotResource();
  m_pPlacementDataManager->DeleteZoneBerryPointResource();
  m_pPlacementDataManager->DeleteZoneGimmickEncountResource();
  m_pPlacementDataManager->DeleteAreaEnvSound();
  m_pPlacementDataManager->DeleteDropItem();
  m_pPlacementDataManager->DeleteZoneItem();
  m_pPlacementDataManager->DeleteZoneBgEvent();
  m_pPlacementDataManager->DeleteZoneStaticModel();
  m_pPlacementDataManager->DeleteZoneEncount();
  m_pPlacementDataManager->DeleteZoneActionEvent();
  m_pPlacementDataManager->DeleteZoneMapJump();
  m_pPlacementDataManager->DeleteZonePos();
  m_pPlacementDataManager->DeleteZoneModelResource();
  m_pPlacementDataManager->DeleteZoneTrainerResource();
  m_pPlacementDataManager->DeleteAreaStaticModel();
  m_pPlacementDataManager->DeleteAreaMoveModelResource();
  m_pPlacementDataManager->DeleteAreaTrainerResource();

  m_pPlacementDataManager->TerminateFieldmap();
  //if( m_pGameManager->GetGameData()->GetFieldItemManager()) m_pGameManager->GetGameData()->GetFieldItemManager()->TerminateDropItemData();
}


/**
 *  @brief キャラクターモデルの初期化
 */
void Fieldmap::InitializeMoveModel()
{
  // ジョインフェスタシーンかどうか調べる
  b32 isExecuteJoinFesta = IsJoinFestaGimmick();
  b32 isExecuteBattleFes = IsBattleFesGimmick();
  b32 isExecuteBattleInst = IsBattleInstGimmick();
  b32 isNpcHeapLargeZone = IsNpcHeapLargeZone();

  // 着せ替えパーツリソースの非同期ロード
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  // バトルフィールドにいる（専用サングラス着用）
  if (isExecuteBattleFes)
  {
    m_DressUpParam = m_pGameManager->GetGameData()->GetPlayerStatusConst()->GetDressup();
    BattleFesTool::GetSunglassDressUpParam(&m_DressUpParam);
  }
  // 通常
  else if( !pPlayer->IsRideMoveModelPrevClearResource() )
  {
    m_DressUpParam = m_pGameManager->GetGameData()->GetPlayerStatusConst()->GetDressup();

    //@fix 大森さん要望 ゲーム開始時は防止とバッグを外す。
    if( m_pGameManager->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_DRESSUP_BEFORE )  )
    {
      m_DressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG] = -1;
      m_DressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
    }

#if PM_DEBUG
    if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_BAG ) == 0 )
    {
      // デバッグ用：主人公のカバンを表示しない
      m_DressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG] = -1;
    }
#endif
  }
  // ライド中
  else
  {
    m_DressUpParam = m_pGameManager->GetGameData()->GetPlayerStatusConst()->GetDressup();
    // @fix NMCat[181] 水上ライド中にバッグ画面等を開閉すると、フィールド上のライド衣装が陸上用のものになる
    EventPokemonRideOn::GetPokemonRideDressUpParam( m_pGameManager,  &m_DressUpParam, true ); // ライドスーツに置き換え
  }

  // 動作モデルマネージャーの初期化
  pFieldCharaModelManager->InitializeManagerResource(m_pRenderingPipeLine,
                                                     m_pFieldDeviceHeapAllocator,
                                                     m_pFieldDeviceHeap,
                                                     m_pGameManager->GetAsyncFileManager(),
                                                     m_DressUpParam,
                                                     isExecuteJoinFesta,
                                                     isExecuteBattleInst,
                                                     isExecuteBattleFes,
                                                     isNpcHeapLargeZone );
}

/**
 *  @brief キャラクターモデルリソースの非同期読み込み開始
 */
void Fieldmap::StartLoadMoveModel()
{
  // プレイヤーのキャラクターID取得
  u32 loadCharacterID = GetPlayerCharacterID();
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
  pFieldCharaModelManager->LoadStaticAsync(loadCharacterID);
  pFieldCharaModelManager->LoadDressUpPartsAsync(m_DressUpParam);
}

/**
 *  @brief 読み込み完了待ち
 */
b32 Fieldmap::WaitLoadMoveModel()
{
  // 動作モデル関連のマネージャーを取得
  Field::MoveModel::FieldMoveModelManager*      pFieldCharaModelManager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();

  // プレイヤーのキャラクターID取得
  u32 loadCharacterID = GetPlayerCharacterID();

  bool result_normal = pFieldCharaModelManager->IsLoaded(loadCharacterID);
  bool result_dressup = pFieldCharaModelManager->IsDressUpPartsLoaded(m_DressUpParam);


  if( result_dressup && result_normal )
  {
    return true;
  }
  return false;
}

/**
 *  @brief キャラクターモデル　セットアップ
 */
void Fieldmap::SetupMoveModel()
{
  // 動作モデル関連のマネージャーを取得
  Field::MoveModel::FieldMoveModelManager*      pFieldCharaModelManager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();

  // プレイヤーのキャラクターID取得
  u32 loadCharacterID = GetPlayerCharacterID();

  // キャラクターリソースノードの初期化
  pFieldCharaModelManager->SetupModel(loadCharacterID);

  // エリアで読み込んだキャラクターリソースを設定
  Field::MoveModel::FieldMoveModelAreaCharaAccessor areaCharaAccessor;
  areaCharaAccessor.Initialize( m_pArea->GetAccessor( Area::AREA_DATA_IDX_CHARACTOR_RESOURCE ) );
  for( s32 i=0; i<areaCharaAccessor.GetDataNum(); ++i )
  {
    pFieldCharaModelManager->Set(areaCharaAccessor.GetCharacterID(i), areaCharaAccessor.GetCharacterResource(i));
    pFieldCharaModelManager->SetupModel( areaCharaAccessor.GetCharacterID(i) );
  }

  // 着せ替えパーツリソースノードの初期化
  pFieldCharaModelManager->SetupDressUpParts(&m_DressUpParam);

}

/**
 *  @brief コリジョンの更新
 */
void Fieldmap::UpdateCollision()
{
  // 動作モデル関連のマネージャーを取得
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModel*        pPlayer                 = static_cast<Field::MoveModel::FieldMoveModel*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

#if PM_DEBUG
  // 視線コリジョンのデバッグ
  if( GetFieldDebugMenu()
   && GetFieldDebugMenu()->IsUseTranierEyeParam()
   && pPlayer
   && pPlayer->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TRAINER_EYE) 
   && pPlayer->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TRAINER_EYE)->GetShapeType() == BaseActor::SHAPE_CYLINDER )
  {
    static_cast<CylinderShape*>( pPlayer->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TRAINER_EYE)->GetShape() )->m_height = GetFieldDebugMenu()->GetTrainerEyeCylinderHeight();
    static_cast<CylinderShape*>( pPlayer->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TRAINER_EYE)->GetShape() )->m_pos.y  = GetFieldDebugMenu()->GetTrainerEyeCylinderOffsetY();
  }
#endif

  // @note 【デバッグRダッシュ処理】 Lボタンが押されている間は自機の壁コリジョンを行わない 150526 ikuta_junya
#if PM_DEBUG
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  //gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  b32 debugDash = false;
  b32 debugTerrainActorActive = true;

  // デバッグダッシュ判定
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) || FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_WALL_COLLISION ) == 0 )
  {
    debugDash = true;
  }

  // 自機の壁コリジョンを一時的に無効化
  if( debugDash 
   && pPlayer
   && pPlayer->IsSetup()
   && pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_TERRAIN ) )
  {
    debugTerrainActorActive = pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_TERRAIN )->IsActive();
    pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_TERRAIN )->SetActive( false );
  }
#endif

  // Tick計測ワーク定義
  enum
  {
    TICK_SOLVE_NAMINORI,
    TICK_SOLVE_RIDE,
    TICK_SOLVE_ROBA2,
    TICK_SOLVE_TERRAIN,
    TICK_SOLVE_MOVEMODEL,
    TICK_SOLVE_STATICMODEL,
    TICK_CHECK_NAMINORI,
    TICK_CHECK_RIDE,
    TICK_CHECK_ROBA2,
    TICK_CHECK_TERRAIN,
    TICK_CHECK_MOVEMODEL,
    TICK_CHECK_STATICMODEL,
    TICK_ADJUST_GROUND,
    TICK_ALL,
    TICK_MAX,
  };

  GFL_DEBUG_TICK_SETUP   (Fieldmap,TICK_MAX);
  GFL_DEBUG_TICK_SET_FLAG(Fieldmap,true);
  GFL_DEBUG_TICK_RESET   (Fieldmap);

  GFL_DEBUG_TICK_START(Fieldmap, TICK_ALL, "Fieldmap::UpdateCollision");

  {
    GFL_DEBUG_TICK_START(Fieldmap, TICK_SOLVE_MOVEMODEL, "m_pCollsionSceneForTrafficModel:Update0");

    // 往来モデル用コリジョンの更新
    {
      // 動作モデルの座標/姿勢をコリジョンアクターの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_TRAFFIC_MODEL        );
      pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_TRAFFIC_MODEL );
      // コリジョンの更新
      m_pCollsionSceneForTrafficModel->Update();
      // ダイナミックアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyActorToMoveModel( Field::MoveModel::COLLISION_TYPE_TRAFFIC_MODEL );
    }

    GFL_DEBUG_TICK_END(Fieldmap, TICK_SOLVE_MOVEMODEL);

    GFL_DEBUG_TICK_START(Fieldmap, TICK_SOLVE_STATICMODEL, "m_pCollsionSceneForStaticModel:Update0");

    // 配置モデル用コリジョンの更新
    {
      // 動作モデルの座標/姿勢をコリジョンアクターの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_STATIC_MODEL        );
      pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_STATIC_MODEL );
      // コリジョンの更新
      m_pCollsionSceneForStaticModel->Update();
      // ダイナミックアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyActorToMoveModel( Field::MoveModel::COLLISION_TYPE_STATIC_MODEL );
    }

    GFL_DEBUG_TICK_END(Fieldmap, TICK_SOLVE_STATICMODEL);

    u32 collisionUpdateCnt = 2;

    GFL_DEBUG_TICK_START(Fieldmap, TICK_SOLVE_NAMINORI, "m_pCollsionSceneForOshikaeshiNaminori:Update0");

    // プレイヤーの周辺より2.5mはなれている波乗り用コリジョンを弾く(プレイヤー専用のシーンなので周辺距離は短くてよい)
    const f32 COLLECTION_R_NAMINORI = 250.0f;
    m_pCollsionSceneForOshikaeshiNaminori->MeshCooking( pPlayer->GetLocalSRT().translate,COLLECTION_R_NAMINORI );

    // 波乗り用コリジョンの更新
    for( u32 i=0 ; i<collisionUpdateCnt ; i++ )
    {
      // 動作モデルの座標/姿勢をコリジョンアクターの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_NAMINORI );
      // コリジョンの更新
      m_pCollsionSceneForOshikaeshiNaminori->Update();
      // コリジョンアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyActorToMoveModel( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_NAMINORI );
    }

    GFL_DEBUG_TICK_END(Fieldmap, TICK_SOLVE_NAMINORI);

    GFL_DEBUG_TICK_START(Fieldmap, TICK_SOLVE_ROBA2, "m_pCollsionSceneForOshikaeshiRoba2:Update0");

    // プレイヤーの周辺より2.5mはなれている地面馬用コリジョンを弾く(プレイヤー専用のシーンなので周辺距離は短くてよい)
    const f32 COLLECTION_R_ROBA2 = 250.0f;
    m_pCollsionSceneForOshikaeshiRoba2->MeshCooking( pPlayer->GetLocalSRT().translate,COLLECTION_R_ROBA2 );

    // 地面馬用コリジョンの更新
    for( u32 i=0 ; i<collisionUpdateCnt ; i++ )
    {
      // 動作モデルの座標/姿勢をコリジョンアクターの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 );
      // コリジョンの更新
      m_pCollsionSceneForOshikaeshiRoba2->Update();
      // コリジョンアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyActorToMoveModel( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 );
    }

    GFL_DEBUG_TICK_END(Fieldmap, TICK_SOLVE_ROBA2);

    GFL_DEBUG_TICK_START(Fieldmap, TICK_SOLVE_RIDE, "m_pCollsionSceneForOshikaeshiRide:Update0");

    // プレイヤーの周辺より2.5mはなれているライド進入禁止用コリジョンを弾く(プレイヤー専用のシーンなので周辺距離は短くてよい)
    const f32 COLLECTION_R_RIDE = 250.0f;
    m_pCollsionSceneForOshikaeshiRide->MeshCooking( pPlayer->GetLocalSRT().translate,COLLECTION_R_RIDE );

    // ライド進入禁止用コリジョンの更新
    for( u32 i=0 ; i<collisionUpdateCnt ; i++ )
    {
      // 動作モデルの座標/姿勢をコリジョンアクターの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_RIDE );
      // コリジョンの更新
      m_pCollsionSceneForOshikaeshiRide->Update();
      // コリジョンアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyActorToMoveModel( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_RIDE );
    }

    GFL_DEBUG_TICK_END(Fieldmap, TICK_SOLVE_RIDE);

    GFL_DEBUG_TICK_START(Fieldmap, TICK_SOLVE_TERRAIN, "m_pCollsionSceneForTerrain:Update0");

    // プレイヤーの周辺より1ブロック分はなれている壁用コリジョンを弾く
    const f32 COLLECTION_R = (400.0f * 1.41421f) / 2.0f;
    m_pCollsionSceneForTerrain->MeshCooking( pPlayer->GetLocalSRT().translate,COLLECTION_R );

    // 壁用コリジョンの更新
    // memo: 壁の隅での押し返しを安定させるために2回必要
    for( u32 i=0 ; i<collisionUpdateCnt ; i++ )
    {
      // 動作モデルの座標/姿勢をコリジョンアクターの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_TERRAIN );
      // コリジョンの更新
      m_pCollsionSceneForTerrain->Update();
      // コリジョンアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
      pFieldCharaModelManager->ApplyActorToMoveModel(Field::MoveModel::COLLISION_TYPE_TERRAIN );
    }

    GFL_DEBUG_TICK_END(Fieldmap, TICK_SOLVE_TERRAIN);
  }

  GFL_DEBUG_TICK_START(Fieldmap, TICK_ADJUST_GROUND, "pFieldCharaModelManager:AdjustGround0");

  {
    Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = this->GetMoveModelManager();
    f32 QUATER_BLOCK = 4200.0f/4.0f;

    for( u32 i=0 ; i<Field::MoveModel::FIELD_MOVE_MODEL_MAX ; i++ )
    {
      if( pFieldCharaModelManager->GetFieldMoveModel(i)
       && pFieldCharaModelManager->GetFieldMoveModel(i)->IsSetup() )
      {
        gfl2::math::Vector3 basePos( pFieldCharaModelManager->GetFieldMoveModel(i)->GetLocalSRT().translate );

        // 真下か隣接する地形ブロックがあるか設定
        // note: 隣接する地形ブロックがコリジョンをカバーしているときがある
        if(  m_pTerrainManager->IsHighBlock( basePos )
          || m_pTerrainManager->IsHighBlock( basePos + gfl2::math::Vector3( QUATER_BLOCK,0.0f,         0.0f) )
          || m_pTerrainManager->IsHighBlock( basePos + gfl2::math::Vector3(-QUATER_BLOCK,0.0f,         0.0f) )
          || m_pTerrainManager->IsHighBlock( basePos + gfl2::math::Vector3(         0.0f,0.0f, QUATER_BLOCK) )
          || m_pTerrainManager->IsHighBlock( basePos + gfl2::math::Vector3(         0.0f,0.0f,-QUATER_BLOCK) ) )
        {
          pFieldCharaModelManager->GetFieldMoveModel(i)->SetCreateGround(true);
        }
        else
        {
          pFieldCharaModelManager->GetFieldMoveModel(i)->SetCreateGround(false);
        }

        // 真下か隣接する地形ブロックが読み込まれたタイミングで、レイキャストが失敗していた場合でも強制的に再トライ
        // note: 隣接する地形ブロックがコリジョンをカバーしているときがある
        if(  m_pTerrainManager->IsHighBlockTrg( basePos )
          || m_pTerrainManager->IsHighBlockTrg( basePos + gfl2::math::Vector3( QUATER_BLOCK,0.0f,         0.0f) )
          || m_pTerrainManager->IsHighBlockTrg( basePos + gfl2::math::Vector3(-QUATER_BLOCK,0.0f,         0.0f) )
          || m_pTerrainManager->IsHighBlockTrg( basePos + gfl2::math::Vector3(         0.0f,0.0f, QUATER_BLOCK) )
          || m_pTerrainManager->IsHighBlockTrg( basePos + gfl2::math::Vector3(         0.0f,0.0f,-QUATER_BLOCK) ) )
        {
          pFieldCharaModelManager->GetFieldMoveModel(i)->ResetFailedRaycastInfo();
        }
      }
    }
  }

  {
    // キャラクター を地面に接地
    pFieldCharaModelManager->AdjustGround();
  }

  GFL_DEBUG_TICK_END  (Fieldmap, TICK_ADJUST_GROUND);

  // 配置モデル・往来モデル・地面用コリジョンが解決しているかチェック
  {
    // 動作モデルの座標/姿勢をコリジョンアクターの座標/姿勢に適用する
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_STATIC_MODEL         );
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_TERRAIN              );
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_NAMINORI  );
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_TRAFFIC_MODEL        );
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_STATIC_MODEL  );
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_STATIC_TYPE_TRAFFIC_MODEL );
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2     );
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_RIDE      );
    pFieldCharaModelManager->ApplyMoveModelToActor( Field::MoveModel::COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION );

    // コリジョンの更新
    GFL_DEBUG_TICK_START(Fieldmap, TICK_CHECK_STATICMODEL, "m_pCollsionSceneForStaticModel:Update1");
    m_pCollsionSceneForStaticModel->Update();
    GFL_DEBUG_TICK_END  (Fieldmap, TICK_CHECK_STATICMODEL);

    // 壁用コリジョンの更新
    GFL_DEBUG_TICK_START(Fieldmap, TICK_CHECK_TERRAIN, "m_pCollsionSceneForTerrain:Update1");
    m_pCollsionSceneForTerrain->Update();
    GFL_DEBUG_TICK_END  (Fieldmap, TICK_CHECK_TERRAIN);

    GFL_DEBUG_TICK_START(Fieldmap, TICK_CHECK_NAMINORI, "m_pCollsionSceneForOshikaeshiNaminori:Update1");
    m_pCollsionSceneForOshikaeshiNaminori->Update();
    GFL_DEBUG_TICK_END  (Fieldmap, TICK_CHECK_NAMINORI);

    GFL_DEBUG_TICK_START(Fieldmap, TICK_CHECK_ROBA2, "m_pCollsionSceneForOshikaeshiRoba2:Update1");
    m_pCollsionSceneForOshikaeshiRoba2->Update();
    GFL_DEBUG_TICK_END  (Fieldmap, TICK_CHECK_ROBA2);

    GFL_DEBUG_TICK_START(Fieldmap, TICK_CHECK_RIDE, "m_pCollsionSceneForOshikaeshiRide:Update1");
    m_pCollsionSceneForOshikaeshiRide->Update();
    GFL_DEBUG_TICK_END  (Fieldmap, TICK_CHECK_RIDE);

    GFL_DEBUG_TICK_START(Fieldmap, TICK_CHECK_MOVEMODEL, "m_pCollsionSceneForTrafficModel:Update1");
    m_pCollsionSceneForTrafficModel->Update();
    GFL_DEBUG_TICK_END  (Fieldmap, TICK_CHECK_MOVEMODEL);

    // まだ衝突しているなら基の座標に戻す
    pFieldCharaModelManager->CheckCollisionSolved();
  }

  GFL_DEBUG_TICK_END(Fieldmap, TICK_ALL);

#if PM_DEBUG
  // 自機の壁コリジョンを一時的に無効化、戻す
  if( debugDash
   && pPlayer
   && pPlayer->IsSetup()
   && pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_TERRAIN ) )
  {
    pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_TERRAIN )->SetActive( debugTerrainActorActive );
  }
#endif

#if PM_DEBUG
  if( m_pFieldDebugMenu )
  {
    m_pFieldDebugMenu->SetPerformanceForAllScene        ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_ALL              ) );
    m_pFieldDebugMenu->SetPerformanceForMoveModelScene  ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_SOLVE_MOVEMODEL  ) + GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_CHECK_MOVEMODEL  ) );
    m_pFieldDebugMenu->SetPerformanceForStaticModelScene( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_SOLVE_STATICMODEL) + GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_CHECK_STATICMODEL) );
    m_pFieldDebugMenu->SetPerformanceForNaminoriScene   ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_SOLVE_NAMINORI   ) + GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_CHECK_NAMINORI   ) );
    m_pFieldDebugMenu->SetPerformanceForRideScene       ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_SOLVE_RIDE       ) + GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_CHECK_RIDE       ) );
    m_pFieldDebugMenu->SetPerformanceForRoba2Scene      ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_SOLVE_ROBA2      ) + GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_CHECK_ROBA2      ) );
    m_pFieldDebugMenu->SetPerformanceForTerrainScene    ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_SOLVE_TERRAIN    ) + GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_CHECK_TERRAIN    ) );
    m_pFieldDebugMenu->SetPerformanceForAdjustGround    ( GFL_DEBUG_TICK_US_GET(Fieldmap, TICK_ADJUST_GROUND    ) );
  }
#endif
}

/**
 *  キャラクターモデル　はき
 */
void Fieldmap::TerminateMoveModel()
{
#if USE_RTTI_DEBUG 
  // RTTIエディタに非表示リクエストを出す 
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->DeleteRttiEditorPage( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ); 
#endif

  // キャラクターシステムの破棄
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->TerminateMoveModelResource();
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->TerminateSubobject();
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->TerminateManagerResource();
}

/**
 *  @brief 主人公のキャラクターIDを取得
 */
u32 Fieldmap::GetPlayerCharacterID() const
{
  // キャラクターリソースの非同期ロード
  u32 loadCharacterID = CHARA_MODEL_ID_PC0002_00;

  // 性別変更
  u8 sex = m_pGameManager->GetGameData()->GetPlayerStatusConst()->GetSex();
  if( PM_MALE == sex )
  {
    loadCharacterID = CHARA_MODEL_ID_PC0001_00;
  }
  return loadCharacterID;
}



/**
 *  @brief  主人公を作成
 */
void Fieldmap::CreatePlayer(b32 setSRT)
{
  // 動作モデルマネージャーの取得
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
  if( !pFieldCharaModelManager )
  {
    return;
  }

  // プレイヤーの作成
  Field::MoveModel::FieldMoveModelHeaderWork     headerWork;
  Field::MoveModel::FieldMoveModelHeaderResource headerResource;
  headerWork.moveCodeId                                   = Field::MoveModel::FIELD_MOVE_CODE_PLAYER;
  headerWork.eventId                                      = static_cast<u32>(FIELD_EVENTID_PLAYER);
  headerResource.pDressUpParam                            = &m_DressUpParam;
  headerResource.pCollisionSceneForTerrain                = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TERRAIN             );
  headerResource.pCollisionSceneForStaticModel            = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_STATIC_MODEL        );
  headerResource.pCollisionSceneForTrafficModel           = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAFFIC_MODEL       );
  headerResource.pCollisionSceneForEventPos               = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_POS           );
  headerResource.pCollisionSceneForEventIntrPos           = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_INTRPOS       );
  headerResource.pCollisionSceneForEventTalk              = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK          );
  headerResource.pCollisionSceneForEventPush              = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH          );
  headerResource.pCollisionSceneForEncount                = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT             );
  headerResource.pCollisionSceneForTrainerEye             = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAINER_EYE         );
  headerResource.pCollisionSceneForOshikaeshiNaminori     = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_NAMINORI );
  headerResource.pCollisionSceneForEventNaminori          = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_NAMINORI      );
  headerResource.pCollisionSceneForGimmickEncount         = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT         );
  headerResource.pCollisionSceneForGimmickEncountAction   = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT_ACTION  );
  headerResource.pCollisionSceneForEventFinder            = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_FINDER  );
  headerResource.pCollisionSceneForStaticModelBBAnimation = m_pStaticModelManager->GetCollisionSceneForBBAnimation();
  headerResource.pCollisionSceneForOshikaeshiRoba2        = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_ROBA2 );
  headerResource.pCollisionSceneForOshikaeshiRide         = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_RIDE  );
  headerResource.pCollisionSceneForFishingPos             = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_FISHING_POS  );
  headerResource.characterId                              = GetPlayerCharacterID();

  // ワークとリソースの初期化(既に初期化されているのならスルーされます)
  pFieldCharaModelManager->InitializeMoveModelWork    ( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER , &headerWork     );
  pFieldCharaModelManager->InitializeMoveModelResource( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER , &headerResource );

  // プレイヤーのインスタンスを動作モデルマネージャーから取得
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );

  // ワークとリソース共に初めて初期化されたのなら原点に表示
  if( (m_Desc.openMode == FM_OPEN_MODE_INIT) && setSRT )
  {
    pCharaModel->GetCharaDrawInstance()->SetPosition(m_Desc.playerLocation.position); 
    pCharaModel->GetCharaDrawInstance()->SetRotationQuat(m_Desc.playerLocation.quaternion);
    pCharaModel->GetCharaDrawInstance()->SetScale   (1.0f, 1.0f, 1.0f);
  }

  // コリジョンの作成
  Field::MoveModel::ApplicationDataForPlayer* pPlayerData = (Field::MoveModel::ApplicationDataForPlayer*)pFieldCharaModelManager->GetApplicationData( headerResource.characterId );
  const gfl2::math::Vector3 collisionOffset( pPlayerData->offsetX,pPlayerData->offsetY,pPlayerData->offsetZ );
  const f32 PLAYER_HALF_WIDTH         = pPlayerData->collisionR;
  const f32 PLAYER_HALF_WIDTH_FILTER  = pPlayerData->collisioFilterR;
  const f32 PLAYER_HEIGHT             = pPlayerData->collisionHeight;
  const f32 PLAYER_EYE_WIDTH          = 21.0f; // コリジョン抜けをしないぎりぎりのサイズ
  const f32 PLAYER_EYE_HEIGHT         = 1000.0f;
  const f32 PLAYER_EYE_HEIGHT_OFFSET  = -400.0f;
  
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TERRAIN                  )->CreateSphere  ( collisionOffset, PLAYER_HALF_WIDTH );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_STATIC_MODEL             )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TRAFFIC_MODEL            )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_POS                )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_INTRPOS            )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_TALK               )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_PUSH               )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_ENCOUNT                  )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TRAINER_EYE              )->CreateCylinder( collisionOffset + gfl2::math::Vector3(0.0f,PLAYER_EYE_HEIGHT_OFFSET,0.0f) , PLAYER_EYE_WIDTH  , PLAYER_EYE_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION)->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_NAMINORI      )->CreateSphere  ( collisionOffset, PLAYER_HALF_WIDTH );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_NAMINORI           )->CreateSphere  ( gfl2::math::Vector3(0.0f,0.0f,0.0f) , PLAYER_HALF_WIDTH_FILTER ); // 狙ってオフセットを削っています
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT          )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION   )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_FINDER             )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2         )->CreateSphere  ( collisionOffset, PLAYER_HALF_WIDTH );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_RIDE          )->CreateSphere  ( collisionOffset, PLAYER_HALF_WIDTH );
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_FISHING_POS              )->CreateCylinder( collisionOffset, PLAYER_HALF_WIDTH , PLAYER_HEIGHT );

  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TERRAIN             )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TERRAIN            ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_STATIC_MODEL        )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_STATIC_MODEL       ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAFFIC_MODEL       )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TRAFFIC_MODEL      ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_POS           )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_POS          ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_INTRPOS       )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_INTRPOS      ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK          )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_TALK         ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH          )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_PUSH         ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT             )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_ENCOUNT            ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAINER_EYE         )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_TRAINER_EYE        ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_NAMINORI )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_NAMINORI));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_NAMINORI      )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_NAMINORI     ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT         )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT         ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT_ACTION  )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION  ));
  m_pStaticModelManager->GetCollisionSceneForBBAnimation()->RegistDynamicActor( pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_FINDER        )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_FINDER       ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_ROBA2    )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2   ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_RIDE     )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_RIDE    ));
  m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_FISHING_POS         )->RegistDynamicActor(pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_FISHING_POS    ));

  pCharaModel->SetCollisionSceneContainerForGround( m_pTerrainManager ? m_pTerrainManager->GetCollsionScene()                                       : NULL );
  pCharaModel->SetCollisionSceneContainerForWall  ( m_pTerrainManager ? m_pTerrainManager->GetCollisionSceneWall(Field::Terrain::WallIndex::NORMAL) : NULL );

  // 押し返しをしないフィルターにはクッキングデータを利用しないようにする
  // note: プレイヤー専用のシーンであり、且つ1フレームに複数回実行する必要がないため
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->DisableCookingData();

  // ライド進入禁止用アクターを無効化
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_RIDE)->SetActive(false);

  // @fix GFNMCat[961] カーラエわんの草むら野生エンカウントが発生しない
  // プレイヤーが持つエンカウント用DynamicActorが、最初に当たったコリジョンの押し返しを受けていたため、
  // 次以降のコリジョンが最初に当たったコリジョンの内側に存在していた場合に当たらなくなっていたため、押し返しを無効にする
  // (エンカウント範囲は当たっているアクター全てを取得し、地面アトリビュートとエンカウント範囲に設定されているアトリビュートを見て、そのフレームで採用するエンカウント範囲を求めている)
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_ENCOUNT)->DisableAdjustVector();

  // @fix GFNMCat[961] カーラエわんの草むら野生エンカウントが発生しない の類似案件
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT)->DisableAdjustVector();
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION)->DisableAdjustVector();
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION)->DisableAdjustVector();
  pCharaModel->GetDynamicActor(Field::MoveModel::COLLISION_TYPE_EVENT_TALK)->DisableAdjustVector();
}

/**
 * @brief ライド状態に復帰
 */
void Fieldmap::StartRecoverPlayerRide()
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  if( pPlayer->IsRideMoveModelPrevClearResource() )
  {
    Field::MoveModel::FieldMoveModelHeaderWork     headerWork     = pPlayer->GetRideMoveModelHeaderWorkPrevClearResource();
    Field::MoveModel::FieldMoveModelHeaderResource headerResource = pPlayer->GetRideMoveModelHeaderResourcePrevClearResource();

    // ライドポケモンの非同期ロード開始
    pFieldCharaModelManager->LoadDynamicAsync( Field::MoveModel::FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_RIDE, headerResource.characterId );
  }
}

bool Fieldmap::WaitRecoverPlayerRide()
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  // ライドモデル復帰前
  // // @fix GFNMCat[715] ロードが1フレームで終わらなかった場合に対応する
  if( pPlayer->IsRideMoveModelPrevClearResource() && !pPlayer->GetOnMoveModel() )
  {
    Field::MoveModel::FieldMoveModelHeaderWork     headerWork     = pPlayer->GetRideMoveModelHeaderWorkPrevClearResource();
    Field::MoveModel::FieldMoveModelHeaderResource headerResource = pPlayer->GetRideMoveModelHeaderResourcePrevClearResource();
    headerResource.pos    = gfl2::math::Vector3(0,0,0);
    headerResource.qua    = gfl2::math::Quaternion(0,0,0,1);
    headerResource.pos    = gfl2::math::Vector3(0,0,0);
    headerResource.qua    = gfl2::math::Quaternion(0,0,0,1);

    if( pFieldCharaModelManager->IsLoaded( headerResource.characterId ) )
    {
      pFieldCharaModelManager->SetupModel( headerResource.characterId );
      pFieldCharaModelManager->TerminateMoveModelWorkResource ( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
      pFieldCharaModelManager->InitializeMoveModelWork    ( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON , &headerWork );
      pFieldCharaModelManager->InitializeMoveModelResource( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON , &headerResource );
      Field::MoveModel::FieldMoveModel*      pRide  = static_cast<Field::MoveModel::FieldMoveModel*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON ));
      pPlayer->RideOnMoveModel(pRide);
      return true;
    }

    return false;
  }
  // ライドモデル復帰後
  else
  {
    return true;
  }
}

/**
  * @brief サブオブジェクトの復帰
  */
void Fieldmap::StartRecoverSubobject()
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  for( s32 i=0 ; i<Field::MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; i++ )
  {
    if( pPlayer->IsSubobjectPrevClearResource(i) )
    {
      u32 charaId = pPlayer->GetSubobjectHeaderPrevClearResource(i).characterId;

      // サブオブジェクトの非同期ロード開始
      pFieldCharaModelManager->LoadDynamicAsync( Field::MoveModel::FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_SUBOBJECT, charaId );
    }
  }
}

bool Fieldmap::WaitRecoverSubobject()
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  bool result = true;

  for( s32 i=0 ; i<Field::MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; i++ )
  {
    if( pPlayer->IsSubobjectPrevClearResource(i) )
    {
      u32 charaId = pPlayer->GetSubobjectHeaderPrevClearResource(i).characterId;

      if( !pFieldCharaModelManager->IsLoaded( charaId ) )
      {
        result = false;
        continue;
      }

      if( pPlayer->GetSubobject(i) == NULL )
      {
        pFieldCharaModelManager->SetupModel( charaId );

        // まだ利用されていないサブオブジェクトワークを検索
        u32 freeIndex = pFieldCharaModelManager->GetFieldSubobjectIndexFromFreeSpace();

        // サブオブジェクトを作成
        const Field::MoveModel::ApplicationDataForCommon* pAppData = pFieldCharaModelManager->GetApplicationData( charaId );
        if( pAppData->type == Field::MoveModel::TYPE_SUBOBJECT && freeIndex != Field::MoveModel::FIELD_SUB_OBJECT_MAX  )
        {
          // コンバートリストからSubobjectアプリケーションデータを取得
          Field::MoveModel::ApplicationDataForSubobject* pSubobjectData = (Field::MoveModel::ApplicationDataForSubobject*)pAppData;

          // ジョイント名を取得
          c8 jointName[Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE];
          for( u32 j=0; j<Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE; j++ ){ Field::MoveModel::utf16_to_ascii(pSubobjectData->jointName[j],&jointName[j]); }

          // サブオブジェクトの設定
          Field::MoveModel::FieldSubobjectHeader header;
          header.characterId = charaId;
          pFieldCharaModelManager->InitializeSubobject( (Field::MoveModel::FIELD_SUB_OBJECT_ID)freeIndex , &header );

          // 持たせる
          pPlayer->SetSubobject(i, pFieldCharaModelManager->GetFieldSubobject(freeIndex),jointName);
        }
      }
    }
  }

  return result;
}

/**
 *  @brief カメラの初期化処理
 */
void Fieldmap::InitializeCamera()
{
  {
    m_pCameraManager = GFL_NEW( m_pFieldHeap ) Camera::CameraManager;
    Camera::CameraManager::InitializeDescription desc;
    desc.m_pSystemHeap = m_pFieldHeap;
    desc.m_pDeviceHeap = m_pFieldDeviceHeap;
    desc.m_pDeviceAllocator = m_pFieldDeviceHeapAllocator;
    desc.m_pTarget = NULL;
    desc.m_pCameraAreaDataAccessor = m_pArea->GetAccessor( Area::AREA_DATA_IDX_CAMERA_AREA_DATA );
    desc.m_pCameraZoneDefaultData = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__CAMERA_DATA_ZONE_DEFAULT_BIN );
    desc.m_pCameraSupportZoneDefaultData = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__CAMERA_SUPPORT_DATA_ZONE_DEFAULT_BIN );
    desc.m_pFieldMap = this;
    desc.m_pZoneData = m_pGameManager->GetGameData()->GetFieldZoneDataLoader();
    desc.m_pPlacementDataManager = m_pFieldResident->GetPlacementDataManager();
    desc.m_pNijiDeviceAllocator = m_pFieldDeviceHeapAllocator;
    desc.m_pSystemAnimationData = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__CAMERA_SYS_ANIME_BIN );
    desc.m_pCameraEyeCollsionScene = m_pFieldResident->GetCollisionScene( Field::FieldResident::COLLISION_SCENE_CAMERA_EYE );
    m_pCameraManager->Initialize( desc );
  }

  {
    m_pCameraUiDeviceTranslator = GFL_NEW( m_pFieldHeap ) Camera::UiDeviceTranslator;
    Camera::UiDeviceTranslator::InitializeDescription desc;
    desc.m_pCameraManager = m_pCameraManager;
    desc.m_pSystemHeap = m_pFieldHeap;
    desc.m_pFieldmap = this;
    m_pCameraUiDeviceTranslator->Initialize( desc );
  }
}

/**
*  @brief 暗黒天球対象フラグのセット
*　@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >>　Mcatコメント内有るaの案件対処
*/
void Fieldmap::SetSkyboxDarkFlag()
{
  m_pGameManager->GetGameData()->SetSkyboxDarkFlag(false);

  Field::EventWork * evwk = m_pGameManager->GetGameData()->GetEventWork();
  if (!evwk->CheckEventFlag(SYS_FLAG_TENKOU_DARK))
  {
    return;
  }

  if (Field::ZoneDataAccessor::IsDarkWeatherZone(m_ZoneId))
  {
    m_pGameManager->GetGameData()->SetSkyboxDarkFlag(true);
  }
  else
  {
    m_pGameManager->GetGameData()->SetSkyboxDarkFlag(false);
  }
}

/**
 *  @brief Skybox生成 開始
 */
void Fieldmap::StartCreateSkybox()
{
  gfl2::heap::HeapBase* pTemporaryHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD);

  m_pSkybox = m_pGameManager->GetSkybox();
  if( m_pSkybox->IsCreateResource() == false)
  {
    System::Skybox::Skybox::Description desc;
    desc.rotateRadY = 0.0f;
    m_pSkybox->StartCreateResource( desc, pTemporaryHeap);
  }

  {
    gfl2::fs::AsyncFileManager* pAsyncFileManager = m_pGameManager->GetAsyncFileManager();

    m_pSkyboxDataManager = GFL_NEW( m_pFieldHeap) Skybox::DataManager();
    Skybox::DataManager::Description desc;
    desc.pHeap              = m_pFieldHeap;
    desc.pTempHeap          = pTemporaryHeap;
    desc.worldID            = m_Desc.playerLocation.worldId;
    desc.pAsyncFileManager  = pAsyncFileManager;
    m_pSkyboxDataManager->Initialize( desc);

  }
}

/**
 *  @brief Skyboxリソース　生成処理　更新
 */
b32 Fieldmap::UpdateCreateSkybox()
{
  return ( m_pSkybox->UpdateCreateResource() && m_pSkyboxDataManager->WaitInitialize() );
}

/**
 *  @brief SkyboxとEnvManagerの生成
 */
void Fieldmap::CreateSkyboxAndEnvManager()
{
  void* pZoneData = m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_ZoneId);
  ZoneDataAccessor zoneDataAccessor( pZoneData);

  // 天球パーツ生成、初期化
  m_pSkybox->CreateParts( NULL, true, System::nijiAllocator::VRAM_MALLOC_A);
  m_pSkybox->Initialize( System::Skybox::Skybox::SKY_TYPE_SUNNY);

  // 天球専用パスに登録する
  for( u32 i=0 ; i<System::Skybox::Skybox::PARTS_TYPE_MAX ; i++ )
  {
    m_pRenderingPipeLine->AddSkyBoxRenderingTarget( m_pSkybox->GetPartsModel((System::Skybox::Skybox::PartsType)i)->GetModelInstanceNode() );
  }

  // ゾーンの天球フラグがfalseなら天球OFF
  if( zoneDataAccessor.IsSkyboxEnable() == false)
  {
    m_pSkybox->SetEnable( false);
  }

  // ジョインフェスタのRR団イベント例外処理
  if( IsJoinFestaGimmick() == true )
  {
    // RR団イベント中の場合、ギミックモデルで空モデル表示するので天球はOFF
    if( m_pGameManager->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_BFES_ROCKET_NOTTORI ) )
    {
      m_pSkybox->SetEnable( false );
      ICHI_PRINT(">ICHI RR団のっとりイベント中につき天球OFF\n");
    }
  }

  // そのゾーンの天球フラグをデバッグフラグにも設定
#if PM_DEBUG
  s32 visibleSkyboxInitialValue = ( zoneDataAccessor.IsSkyboxEnable() ? 1 : 0 );
  FIELD_DEBUG_SWITCH_INITIAL_SET( Debug::DebugTypes::Switch::VISIBLE_SKYBOX, visibleSkyboxInitialValue );
#endif // PM_DEBUG

  // EnvManager初期化
  {
    u8 envPackIndex = zoneDataAccessor.GetEnvironmentIndex();
    void* pEnvPack = m_pArea->GetAccessor( Area::AREA_DATA_IDX_ENVIRONMENT_RESOURCE)->GetData( envPackIndex);

    m_pFieldEnvManager = GFL_NEW( m_pFieldHeap) Field::EnvManager();
    m_pFieldEnvManager->Initialize( m_pFieldDeviceHeap, m_pFieldDeviceHeapAllocator, m_pRenderingPipeLine , pEnvPack, m_pSkybox, m_pSkyboxDataManager);
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode = m_pFieldEnvManager->GetDrawEnvNode();
    m_pFieldRenderingPipeLineManager->AddDrawEnv( FieldRenderingPipeLineManager::EXECUTE_TYPE_NORMAL, pNode);
  }

  //天候管理
  {
    m_pWeatherControl = GFL_NEW( m_pFieldHeap) weather::WeatherControl;
    weather::WeatherControl::InitParam param;
    param.pHeap = m_pFieldHeap;
    param.pAsyncFileReadManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
    param.pGLAllocator = m_pFieldDeviceHeapAllocator;
    param.pSkybox = m_pSkybox;
    param.pWeatherManager = m_pWeatherManager;
    param.pEnvManager = m_pFieldEnvManager;
    param.pZoneDataLoader = m_pGameManager->GetGameData()->GetFieldZoneDataLoader();
    param.pGameData = m_pGameManager->GetGameData();
    param.EnvIndex = zoneDataAccessor.GetWeatherEnvIndex();
    param.ZoneId = m_ZoneId;
    m_pWeatherControl->Initialize(param);
    // 往来に引き渡す
    Field::TrafficManager *pTrafficManager = GetTrafficManager();
    if( pTrafficManager )
    {
      pTrafficManager->SetWeatherControl( m_pWeatherControl );
    }
  }
}

/**
 *  @brief 画面表示前のEnvManagerの準備
 */
void Fieldmap::ReadyEnvManager()
{
  s32 hour,minute,second;
  this->GetAlolaRealTime( &hour, &minute, &second );

  gfl2::math::Vector3  controlPos = m_playerPosition;
  gfl2::math::Matrix44 projMat;
  m_pCameraManager->GetMainViewCamera()->GetProjectionMatrix(&projMat);

  m_pFieldEnvManager->Ready( controlPos, hour, minute, second, projMat, m_pRenderingPipeLine );
}

/**
 *  @brief EnvManagerの更新
 */
void Fieldmap::UpdateEnvManager()
{
  s32 hour,minute,second;
  this->GetAlolaRealTime( &hour, &minute, &second );

  gfl2::math::Vector3  controlPos = m_playerPosition;
  gfl2::math::Matrix44 projMat;
  m_pCameraManager->GetMainViewCamera()->GetProjectionMatrix(&projMat);
  m_pFieldEnvManager->Update( controlPos, hour, minute, second, projMat, m_pRenderingPipeLine );
}

/**
 *  @brief SkyboxとEnvManagerの破棄
 */
void Fieldmap::TerminateSkyboxAndEnvManager()
{
  // EnvManager破棄
  m_pFieldEnvManager->Terminate();
  GFL_SAFE_DELETE( m_pFieldEnvManager);

  // 天球専用パスから外す
  for( u32 i=0 ; i<System::Skybox::Skybox::PARTS_TYPE_MAX ; i++ )
  {
    m_pRenderingPipeLine->RemoveSkyBoxRenderingTarget( m_pSkybox->GetPartsModel((System::Skybox::Skybox::PartsType)i)->GetModelInstanceNode() );
  }

  // 天球データ管理クラス
  m_pSkyboxDataManager->Terminate();
  GFL_SAFE_DELETE( m_pSkyboxDataManager);

  // 天球の破棄
  m_pSkybox->Terminate();
  if( m_isSkyboxKeep)
  {
    // リソースは残す
    m_pSkybox->DestroyParts();
    m_isSkyboxKeep = false;
  }
  else
  {
    // リソースも破棄
    m_pSkybox->DestroyAll();
  }

  m_pWeatherControl->Terminate();
  GFL_SAFE_DELETE( m_pWeatherControl);
}


/**
 *  @brief StaticModel　初期化
 */
void Fieldmap::InitializeStaticModel()
{
  m_pStaticModelManager = m_pFieldResident->GetStaticModelManager();

  StaticModel::StaticModelManager::InitializeDataAccessor initDataAccessor;
  initDataAccessor.pHeap                  = m_pFieldDeviceHeap;
  initDataAccessor.pResourcePackBinLinker = m_pArea->GetAccessor( Area::AREA_DATA_IDX_STATICMODEL_RESOURCE);
  initDataAccessor.pFieldRootNode         = this->GetFieldRootNode();
  initDataAccessor.pSETableData           = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__STATIC_MODEL_SE_TABLE_BIN);;

  m_pStaticModelManager->InitializeFirst( initDataAccessor);

#if PM_DEBUG
  m_pStaticModelManager->SetZoneID( m_ZoneId);
#endif // PM_DEBUG

}

/**
 *  @brief StaticModel　レディ
 */
void Fieldmap::ReadyStaticModel()
{
  Field::MoveModel::FieldMoveModel * pPlayerCharacter = this->GetPlayerCharacter();
  m_pStaticModelManager->Ready( pPlayerCharacter->GetDynamicActor( MoveModel::COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION));
}

/**
 *  @brief StaticModel　更新
 */
void Fieldmap::UpdateStaticModel()
{
  Field::MoveModel::FieldMoveModel * pPlayerCharacter = this->GetPlayerCharacter();
  m_pStaticModelManager->UpdateBBAnimationCollision( pPlayerCharacter->GetDynamicActor( MoveModel::COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION));
  m_pStaticModelManager->Update();
}

/**
 *  @brief StaticModel　破棄
 */
void Fieldmap::TerminateStaticModel()
{
  m_pStaticModelManager->Finalize();
}

/**
 *  @brief Terrain初期化
 */
void Fieldmap::InitializeTerrain()
{
  STATIC_ASSERT( TERRAIN_LISTENER_MAX >= StaticModel::StaticModelManager::STATIC_MODEL_MAX );   // リスナーを使用するのは配置モデルのみ
  
  Terrain::TerrainManager::InitializeDescription initDesc;
  initDesc.m_pFileManager = m_pGameManager->GetAsyncFileManager(),
  initDesc.pTerrainLayoutData = NULL;
  initDesc.m_pDeviceHeap = m_pFieldDeviceHeap;
  initDesc.m_pSystemHeap = m_pFieldHeap;
  initDesc.pAreaResourceDataAccessor = m_pArea->GetAccessor( Area::AREA_DATA_IDX_TERRAIN_RESOURCE );
  initDesc.m_pDeviceAllocator = m_pFieldDeviceHeapAllocator;
  initDesc.m_pEnvManager = m_pFieldEnvManager;
  initDesc.m_pRenderingPipeLine = m_pRenderingPipeLine;
  initDesc.m_StaticLodListenerMax = TERRAIN_LISTENER_MAX;
  initDesc.m_BlockControlMode = static_cast< Terrain::TerrainManager::BlockControlMode::Value >( m_pWorld->GetWorldInfo().m_BlockControl );
  initDesc.m_MemoryMode = Area::GetTerrainMemoryMode( m_pArea->GetAreaID() ); // @note 急遽追加のためAreaIDでハードコーディングしている、アッパーならワールド管理表などで指定できるようにする

  // 壁コリジョンを登録するシーンを設定
  initDesc.m_pWallCollsionSceneTable[ Terrain::WallIndex::NORMAL] = m_pCollsionSceneForTerrain;
  initDesc.m_pWallCollsionSceneTable[ Terrain::WallIndex::OSHIKAESHI_NAMINORI] = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_NAMINORI );
  initDesc.m_pWallCollsionSceneTable[ Terrain::WallIndex::EVENT_NAMINORI] = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_NAMINORI );
  initDesc.m_pWallCollsionSceneTable[ Terrain::WallIndex::OSHIKAESHI_ROBA2] = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_ROBA2 );
  initDesc.m_pWallCollsionSceneTable[ Terrain::WallIndex::OSHIKAESHI_RIDE] = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_RIDE );

  // カメラコリジョン対象を登録するシーン
  initDesc.m_pCameraEyeCollsionScene = m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_CAMERA_EYE );

  // 配置情報生成
  {
    Terrain::TerrainLayoutData* pLayoutData = GFL_NEW( m_pFieldHeap ) Terrain::TerrainLayoutData();
    pLayoutData->Create( m_pArea->GetAccessor( Area::AREA_DATA_IDX_TERRAIN_LAYOUT_RESOURCE)->GetData( 0 ), m_pArea->GetAccessor( Area::AREA_DATA_IDX_TERRAIN_LAYOUT_RESOURCE)->GetDataSize( 0 ), m_pFieldHeap );
    initDesc.pTerrainLayoutData = pLayoutData; // 所有権も渡している
  }

  m_pTerrainManager = GFL_NEW( m_pFieldHeap ) Terrain::TerrainManager();
  m_pTerrainManager->Initialize( initDesc );
}

/**
 *  @brief Terrain　システム初期化待ち
 */
b32 Fieldmap::WaitTerrainSystemInitialize( void )
{
  // true 終了、false 継続
  return m_pTerrainManager->InitializeWait();
}

/**
 *  @brief Terrain　Highモデル制御点の設定
 */
void Fieldmap::SetupBlockControlPositionTerrain()
{
  // Updateによる制御更新を有効化(フラグが立つだけ)
  m_pTerrainManager->ControlStart();   
  
  // 基準点を指定(この後のUpdateで実際の変更リクエストが掛かる)
  m_pTerrainManager->SetBlockControlPosition( m_Desc.playerLocation.position );

  // スクリプトによる、制御点の変更を復帰
  Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_pGameManager->GetFieldScriptSystem();
  if( pFieldScriptSystem->IsTerrainBlockControlSet() )
  {
    // 外部基準点を指定(こっちは即時に変更リクエストが掛かる)
    gfl2::math::Vector3 pos;
    pFieldScriptSystem->GetTerrainBlockControlPosition( &pos );
    m_pTerrainManager->SetExternalBlockControlPointByPosition( pos );
  }

  // スクリプトによるBGパーツの読み込み設定を復帰
  Field::FieldScript::FieldScriptSystem::BG_PARTS_INFO* pBgPartsInfo = pFieldScriptSystem->GetBgPartsInfo();
  Field::Terrain::TerrainBgPartsManager* pBgPartsManager = m_pTerrainManager->GetBgPartsManager();
  for( u32 i = 0; i < Field::FieldScript::FieldScriptSystem::BG_PARTS_MAX; i++ )
  {
    Field::FieldScript::FieldScriptSystem::BG_PARTS_PARAM* pParam = &pBgPartsInfo->paramTable[i];
    if( pParam->use )
    {
      gfl2::heap::HeapBase *useDeviceHeap = NULL;
      System::nijiAllocator*useDeviceHeapAllocator = NULL;
      
      if( pParam->useEventHeap )
      {
        // イベントヒープを利用
        useDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
        useDeviceHeapAllocator = System::GflUse::GetEventDeviceAllocator();
      }
      else
      {
        // フィールドの拡張ヒープを利用
        useDeviceHeap = m_pArea->GetFieldExtHeap();
        useDeviceHeapAllocator = m_pArea->GetFieldExtHeapAllocator();
        GFL_ASSERT_STOP( useDeviceHeap );
      }

      pBgPartsManager->LoadBgPartsAsync( pParam->partsNo, useDeviceHeap, useDeviceHeapAllocator );
      pBgPartsManager->SetVisible( pParam->partsNo, pParam->visible );
    }
  }
}


/**
 *  @brief  TerrainのSetup待ち
 */
b32 Fieldmap::UpdateSetUpTerrain()
{
  return m_pTerrainManager->UpdateStartSetup();
}

/**
 *  @brief Terrain更新
 */
void Fieldmap::UpdateTerrain()
{
  // 今の時間取得
  GameSys::AlolaTime date;
  date.Initialize();
  s32 hour    = date.GetHour();
  s32 minute  = date.GetMinute();
  s32 second  = date.GetSecond();

  MoveModel::FieldMoveModel* pMoveModel = this->GetPlayerCharacter();

  if( pMoveModel 
   && pMoveModel->GetCharaDrawInstance() 
   && pMoveModel->GetCharaDrawInstance()->GetModelInstanceNode() )
  {
    gfl2::math::Vector3 pos = pMoveModel->GetCharaDrawInstance()->GetPosition();
    m_pTerrainManager->SetBlockControlPosition( pos );
  }
  else
  {
    // ライド乗り降り中にここを通る可能性がある。座標変更はない
  }

  gfl2::math::Matrix44 viewProj;
  gfl2::math::Matrix34 view;
  m_pCameraManager->GetMainGamePlayCamera()->GetProjectionMatrix( &viewProj );
  m_pCameraManager->GetMainGamePlayCamera()->GetViewMatrix( &view );
  viewProj = viewProj * view;
  m_pTerrainManager->Update( hour, minute, second, &viewProj );
}

/**
 *  @brief Terrain破棄
 */
void Fieldmap::TerminateTerrain()
{
  GFL_SAFE_DELETE( m_pTerrainManager );
}

/**
 *  @brief ドロップアイテム生成モジュール初期化
 */
void Fieldmap::InitializeDropItemCreator()
{
  DropItem::Creator::InitializeDescription desc;
  desc.pDropItemData          = m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__DROP_ITEM_BIN);
  desc.pPlacementDataManager  = m_pPlacementDataManager;

  m_pDropItemCreator = GFL_NEW( m_pFieldDeviceHeap) DropItem::Creator();
  m_pDropItemCreator->Initialize( desc);
}

/**
 *  @brief ドロップアイテム生成モジュール破棄
 */
void Fieldmap::TerminateDropItemCreator()
{
  GFL_SAFE_DELETE( m_pDropItemCreator );
}


/**
*  @brief プレイヤー陰影モジュール初期化
*/
void Fieldmap::InitializePlayerShade()
{
  gfl2::fs::BinLinkerAccessor* pAccessor = m_pArea->GetAccessor(Area::AREA_DATA_IDX_TERRAIN_RESOURCE);
  if (pAccessor->GetDataSize(Field::Terrain::AreaResourceDataIndex::SHADE_AREA) > 0)
  {
    // 影領域データが存在する時のみモジュール生成
    Shade::FieldPlayerShade::InitDesc desc;
    desc.pHeap          = m_pFieldHeap;
    desc.pShadeAreaData = pAccessor->GetData(Field::Terrain::AreaResourceDataIndex::SHADE_AREA);
    
    m_pFieldPlayerShade = GFL_NEW(m_pFieldHeap) Shade::FieldPlayerShade();
    m_pFieldPlayerShade->Initialize(desc);
  }
}
/**
*  @brief プレイヤー陰影モジュール破棄
*/
void Fieldmap::TerminatePlayerShade()
{
  if (m_pFieldPlayerShade)
  {
    m_pFieldPlayerShade->Terminate();
    GFL_SAFE_DELETE(m_pFieldPlayerShade);
  }
}


/**
*  @brief 追加のエフェクトモジュール初期化
*/
void Fieldmap::InitializeEffectDll()
{
  // 不要なゾーンの場合は読込を行わない
  switch (m_ZoneId)
  {
  case Z_F0401BT0101:
  case Z_F0401BT0102:
  case Z_F0401BT0103:
  case Z_F0401BT0104:
  // @fix GFMMCat[322] SUB_310 カフェ占領のスカル団イベントのバトル遷移時(ポケセン内)にヒープメモリ不足でアサートが発生する。対処、ポケセンでバトルは発生しない前提のプログラムだったのでバトルが発生するポケセンでは追加のエフェクトモジュール読み込みを行わないように対処。（本来読込を行う共通モジュール内にはライド用エフェクトと天候エフェクトが含まれている。）
  // バトルが発生するポケセンは共通モジュールのエフェクト読み込みを行わない
  case Z_F0301PC0401: // サブイベント310番(カフェ占領のスカル団)でバトルが発生するポケセン
  case Z_F0401PC0101: // サブイベント50番(きんのたま)でバトルが発生するポケセン
    return;
  }

  // ゾーンごとに読込モジュールを分ける
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager);
  switch (m_ZoneId)
  {
  case Z_F0701PH0301: // デンジュモク
  case Z_F0701PH0601: // カミツルギ
  case Z_F0701PH0501: // フェローチェ
  case Z_F0301I1304:  // 電気試練
    // 指定したゾーンに該当する場合は専用のエフェクトモジュールを読み込む
    m_pEffectUniqueModule = roMgr->LoadModule(m_pGameManager->GetAsyncFileManager(), "FieldEffectUnique.cro", NULL);
    roMgr->StartModule(m_pEffectUniqueModule);
    break;
  case Z_F0601JF0101:
  case Z_F0601JF0102:
  case Z_F0601JF0103:
    // ジョインフェスタの場合はJF用のモジュールを読み込む
    m_pEffectJoinFestaModule = roMgr->LoadModule(m_pGameManager->GetAsyncFileManager(), "FieldEffectJoinFesta.cro", NULL);
    roMgr->StartModule(m_pEffectJoinFestaModule);
    break;

  default:
    // 指定したゾーン以外のゾーンの場合は共通エフェクトモジュールを読み込む
    m_pEffectCommonModule = roMgr->LoadModule(m_pGameManager->GetAsyncFileManager(), "FieldEffectCommon.cro", NULL);
    roMgr->StartModule(m_pEffectCommonModule);
    break;
  }
}
/**
*  @brief 追加のエフェクトモジュール破棄
*/
void Fieldmap::TerminateEffectDll()
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager);
  if (m_pEffectUniqueModule)
  {
    roMgr->DisposeModule(m_pEffectUniqueModule);
    m_pEffectUniqueModule = NULL;
  }
  if (m_pEffectCommonModule)
  {
    roMgr->DisposeModule(m_pEffectCommonModule);
    m_pEffectCommonModule = NULL;
  }
  if (m_pEffectJoinFestaModule)
  {
    roMgr->DisposeModule(m_pEffectJoinFestaModule);
    m_pEffectJoinFestaModule = NULL;
  }

}

/**
*  @brief エンディング用のモジュール初期化
*/
bool Fieldmap::InitializeFieldStaffroll()
{
  if (
    (m_ZoneId != Z_F0301PL0601EV01) && //チャンピオン部屋
    (m_ZoneId != Z_F0102EV02_T0101) && //リリィタウン(前作と一緒）
    (m_ZoneId != Z_F0301D1101EV01) && //いかがわしき屋敷エントランス１F
    (m_ZoneId != Z_F0301I1201EV01) && //ジガルデ装置の部屋	
    (m_ZoneId != Z_F0401D0602EV01) && //エンドケーブ
    (m_ZoneId != Z_F0501I0201EV01) && //エーテル財団・保護区
    (m_ZoneId != Z_F0102I0501EV01) && //ハウオリ乗船場
    (m_ZoneId != Z_F0102EV02_C0103) && //ポートエリア
    (m_ZoneId != Z_F0102D0102EV01) && //マハロ山道吊り橋
    (m_ZoneId != Z_F0102D0203EV01) &&    //いくさの遺跡
    (m_ZoneId != Z_F0102EV05_S0101) //一番道路
    )
  {
    return true; //エンディングゾーンではないので処理無し
  }

  if (!m_pSubScreen->IsCanCreateFieldStaffList())
  {
    return false;
  }

  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager);
  m_pStaffrollModule = roMgr->LoadModule(m_pGameManager->GetAsyncFileManager(), "StaffList.cro", NULL);
  roMgr->StartModule(m_pStaffrollModule);

  m_pFieldStaffList = GFL_NEW(m_pFieldHeap) App::StaffList::FieldStaffList(this);
  return true;
}

/**
*  @brief エンディング用のモジュール　準備待ち
*/
bool Fieldmap::InitializeFieldStaffrollReady()
{
  return InitializeFieldStaffroll();
}

/**
*  @brief エンディング用のモジュール　準備待ち
*/
bool Fieldmap::InitializeFieldStaffrollReadyWait()
{
  if (m_pFieldStaffList != NULL)
  {
    if (!m_pFieldStaffList->IsCreated())
    {
      return false;
    }
  }
  return true;
}

/**
*  @brief エンディング用のモジュール　破棄準備
*/
void Fieldmap::TerminateFieldStaffrollReady()
{
  if (m_pFieldStaffList == NULL) return;
  //エンディング用スタフロールモジュール終了リクエスト
  m_pFieldStaffList->SetRequest(App::StaffList::EX_REQUEST_NO_END);
}

/**
*  @brief エンディング用のモジュール　破棄準備待ち
*/
bool Fieldmap::TerminateFieldStaffrollReadyWait()
{
  if (m_pFieldStaffList == NULL) return true;

  return m_pFieldStaffList->IsModuleFree();
}

/**
*  @brief エンディング用のモジュール　破棄
*/
void Fieldmap::TerminateFieldStaffroll()
{
  if (m_pFieldStaffList)
  {
    GFL_SAFE_DELETE(m_pFieldStaffList);
    m_pFieldStaffList = NULL;
  }

  if (m_pStaffrollModule)
  {
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager);
    roMgr->DisposeModule(m_pStaffrollModule);
    m_pStaffrollModule = NULL;
  }
}

/**
 *  @brie アローラ時間の取得
 *  @param pOutHour 時間の格納先
 *  @param pOutMinute 分の格納先
 *  @param pOutSecond 秒の格納先
 *  @note アローラ時間のリアルタイムを格納
 */
void Fieldmap::GetAlolaRealTime( s32* pOutHour, s32* pOutMinute, s32* pOutSecond ) const
{
  GameSys::AlolaTime date;
  date.Initialize();

  *pOutHour    = date.GetHour();
  *pOutMinute  = date.GetMinute();
  *pOutSecond  = date.GetSecond();
}

#if PM_DEBUG
// RTTIの初期化・破棄
void Fieldmap::InitializeDeubgRtti()
{
  // DrawUtilの初期化
  {
    gfl2::heap::HeapBase * pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::heap::HeapBase * pDebugHeapDev = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    // RTTIデバッグの描画に必要なフォントの生成
    m_pDebugResFont = System::GflUse::GetSystemFont()->GetResFont();

    // RTTIデバッグの描画に必要なDrawUtilText要アロケータを作成
    m_pDebugMemAllocator = GFL_NEW(pDebugHeapDev)       gfl2::heap::NwAllocator( pDebugHeapDev );
    
    // RTTIデバッグの描画に必要なDrawUtilTextの生成
    m_pDebugDrawUtilText = GFL_NEW(pDebugHeapDev) gfl2::util::DrawUtilText(m_pDebugMemAllocator, m_pDebugMemAllocator);
    m_pDebugDrawUtilText->Initialize( m_pDebugMemAllocator, m_pDebugMemAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
    m_pDebugDrawUtilText->SetFont( m_pDebugResFont );
  
    // デバッグ変数に登録
    FIELD_DEBUG_VALUE_SET( Debug::DebugTypes::s_pDrawUtilTextUpper, m_pDebugDrawUtilText );

#if USE_RTTI_DEBUG
    gfl2::rttidebug::RttiEditor::getInstance()->SetDrawUtilText(m_pDebugDrawUtilText);
#endif
  }
}

void Fieldmap::UpdateDebugRtti()
{
#if USE_RTTI_DEBUG

  static b32 oldR3Press    = false;
  static b32 oldRightPress = false;
  static b32 oldLeftPress  = false;
  static b32 oldDownPress  = false;
  static b32 oldUpPress    = false;
  static b32 oldR2Press    = false;
  static b32 oldL2Press    = false;
  static b32 isDraw        = false;

  // RTTIデバッガの描画オンオフ
  if( !oldR3Press && gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::R3 ) )
  {
    isDraw = isDraw ? false : true;
  }

  // 次のリクエストをエディタで表示する
  if     ( !oldRightPress && gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Right ) ){ gfl2::rttidebug::RttiEditor::getInstance()->NextTag(); }
  // 前のリクエストをエディタで表示する
  else if( !oldLeftPress  && gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Left  ) ){ gfl2::rttidebug::RttiEditor::getInstance()->PrevTag(); }

  // 次のパラメータを選択する
  if     ( !oldDownPress  && gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Down  ) ){ gfl2::rttidebug::RttiEditor::getInstance()->NextParam(); }
  // 前のパラメータを選択する
  else if( !oldUpPress    && gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Up    ) ){ gfl2::rttidebug::RttiEditor::getInstance()->PrevParam(); }
  // 選択中のパラメータの値を増やす
  else if( !oldR2Press    && gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::R2    ) ){ gfl2::rttidebug::RttiEditor::getInstance()->AddValue();  }
  // 選択中のパラメータの値を減らす
  else if( !oldL2Press    && gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::L2    ) ){ gfl2::rttidebug::RttiEditor::getInstance()->PullValue(); }

  // RTTIデバッガの描画
  gfl2::rttidebug::RttiEditor::getInstance()->SetEnableDraw( 0 != isDraw );
  gfl2::rttidebug::RttiEditor::getInstance()->Update();

  oldR3Press    = gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::R3    );
  oldRightPress = gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Right );
  oldLeftPress  = gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Left  );
  oldDownPress  = gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Down  );
  oldUpPress    = gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Up    );
  oldR2Press    = gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::R2    );
  oldL2Press    = gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::L2    );

#endif
}

void Fieldmap::DrawDebugRtti( gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  // 左画面描画
  case gfl2::gfx::CtrDisplayNo::LEFT:
#if USE_RTTI_DEBUG
    gfl2::rttidebug::RttiEditor::getInstance()->Draw();
#elif PM_DEBUG
    // テキストクリア、RTTIが有効になっている場合はDraw内部で行われている
    m_pDebugDrawUtilText->ClearDrawText();
#endif
    break;
  }
}

void Fieldmap::FinalineDebugRtti()
{
  // DrawUtilText破棄
  FIELD_DEBUG_VALUE_SET( Debug::DebugTypes::s_pDrawUtilTextUpper, NULL );
  m_pDebugDrawUtilText->Terminate();
  
  GFL_SAFE_DELETE( m_pDebugDrawUtilText );
  m_pDebugResFont = NULL;
  GFL_SAFE_DELETE( m_pDebugMemAllocator );

}
#endif


#if PM_DEBUG
/**
 *  @brief デバッグDLLの読み込み
 */
void Fieldmap::InitializeDebugDll()
{
  gfl2::heap::HeapBase * pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DLL_LOAD );

  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  m_pDebugDllModule = roMgr->LoadModule( m_pGameManager->GetAsyncFileManager(), "FieldDebug.cro", pHeap);
  roMgr->StartModule( m_pDebugDllModule );

}

/**
 *  @brief デバッグDLLの破棄
 */
void Fieldmap::TerminateDebugDll()
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  roMgr->DisposeModule( m_pDebugDllModule );
  m_pDebugDllModule = NULL;
}

/**
 *  @brief デバッグ表示物登録処理
 */
void Fieldmap::DebugRegistRender()
{
  // true = アクティブなコリジョンのみ表示
  b32 colDrawActiveOnlyFlag = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::COL_DARW_ONLY_ACTIVE );

  // 地形(高さ)表示
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_TERRAIN_COLLISION ) )
  {
    Field::Debug::Menu::DebugMenuAttrDispEditor::UpdateColor();
    gfl2::math::Vector4 errorColor = Field::Debug::DebugTypes::s_AttrColorTable[ ATTRIBUTE_MAX ];
    gfl2::math::Vector3 offset( 0.0f, Field::Debug::DebugTypes::s_AttrDrawOffset, 0.0f );
    m_pTerrainManager->GetCollsionScene()->DebugRegistRenderStaticActor( errorColor, colDrawActiveOnlyFlag, offset, Field::Debug::DebugTypes::s_AttrColorTable, ATTRIBUTE_MAX );
  }

  // 壁
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_WALL_COLLISION ) )
  {
    m_pCollsionSceneForTerrain->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 1.0f, 0.0f), colDrawActiveOnlyFlag);
  }

  // 壁(なみのり)
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_WALL_COLLISION_NAMINORI ) )
  {
    m_pCollsionSceneForOshikaeshiNaminori->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 1.0f, 0.0f), colDrawActiveOnlyFlag);
  }

  // 壁(地面馬)
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_WALL_COLLISION_ROBA2 ) )
  {
    m_pCollsionSceneForOshikaeshiRoba2->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 1.0f, 0.0f), colDrawActiveOnlyFlag);
  }

  //　壁(ライド)
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_WALL_COLLISION_RIDE ) )
  {
    m_pCollsionSceneForOshikaeshiRide->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 1.0f, 0.0f), colDrawActiveOnlyFlag);
  }

  // 配置物コリション
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_PLACEMENT_COLLISION ) )
  {
    m_pCollsionSceneForStaticModel->DebugRegistRenderStaticActor(gfl2::math::Vector4(1.0f, 0.0f, 0.0f), colDrawActiveOnlyFlag);
  }

  // POSイベント
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_EVENT_POS_COLLISION ) )
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_POS )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_FISHING_POS )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
  }

  // intrPOSイベント
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_EVENT_INTRPOS_COLLISION ) )
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_INTRPOS )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
  }

  // 話しかけイベント
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_EVENT_TALK_COLLISION ) )
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
  }

  // 押し込みイベント
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_EVENT_PUSH_COLLISION ) )
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
  }

  // ポケファインダーイベント
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_EVENT_POKEFINDER ) )
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_FINDER )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
  }

  // エンカウント
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_ENCOUNT_COLLISION ) )
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
  }

  // 視線エンカウント
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_TRAINER_EYE_COLLISION ) ) 
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAINER_EYE )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAINER_EYE )->DebugRegistRenderDynamicActor(gfl2::math::Vector4(0.0f, 1.0f, 0.0f), colDrawActiveOnlyFlag);
  }

  // TRフォーカス
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_FOCUS_EYE_COLLISON ) )
  {
    if( m_pCameraManager->GetFocus() )
    {
      Field::Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Field::Debug::DebugCollisionRenderManager );
      if( pManager )
      {
        if( !( colDrawActiveOnlyFlag && !m_pCameraManager->GetFocus()->GetFocusEyeActor()->IsActive()) )
        { 
          pManager->RegistRenderCollisionActor( m_pCameraManager->GetFocus()->GetFocusEyeActor(), gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        }
      }
    }
  }
  
  // TRフォーカス（継続用）
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_FOCUS_EYE_KEEP_COLLISON ) )
  {
    if( m_pCameraManager->GetFocus() )
    {
      Field::Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Field::Debug::DebugCollisionRenderManager );
      if( pManager )
      {
        if( !( colDrawActiveOnlyFlag && !m_pCameraManager->GetFocus()->GetFocusEyeActorForKeep()->IsActive()) )
        { 
          pManager->RegistRenderCollisionActor( m_pCameraManager->GetFocus()->GetFocusEyeActorForKeep(), gfl2::math::Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        }
      }
    }
  }

  // カメラエリア
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_AREA_COLLISION ) ) 
  {
    m_pCameraManager->GetAreaCollisionManager()->DebugRegistRenderCollisionArea( 
      gfl2::math::Vector4(0.5f, 0.5f, 0.0f),
      m_pCameraManager->GetHitAttr()
      );
  }

  // カメラ接触非表示メッシュ
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_EYE_COLLISION ) ) 
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_CAMERA_EYE )->DebugRegistRenderStaticActor(gfl2::math::Vector4(0.0f, 0.0f, 1.0f), colDrawActiveOnlyFlag);
  }

  // 環境音
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_ENV_SOUND_COLLISION ) )
  {
    m_pFieldResident->GetEnvSoundManager()->DebugRegistRenderCollision( gfl2::math::Vector4(0.0f, 0.5f, 0.5f) );
  }

  // ギミックエンカウントアクション用コリジョン表示
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_GIMMICK_ENCOUNT_ACTION_COLLISION ) )
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT_ACTION )->DebugRegistRenderStaticActor(gfl2::math::Vector4(1.0f, 0.6f, 0.07f), colDrawActiveOnlyFlag);
  }

  // ギミックエンカウントエンカウント用コリジョン表示
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_GIMMICK_ENCOUNT_ENCOUNT_COLLISION ) )
  {
    m_pFieldResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT )->DebugRegistRenderStaticActor(gfl2::math::Vector4(1.0f, 0.8f, 0.8f), colDrawActiveOnlyFlag);
  }

  // カメラ関連
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET ) )
  {
    m_pCameraManager->DebugRenderResistCameraTarget( colDrawActiveOnlyFlag );
  }
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION ) )
  {
    m_pCameraManager->DebugRenderResistCameraTargetPosition( colDrawActiveOnlyFlag );
  }

  
  // 自機の現在の高さを設定(カメラエリア向け)
  Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Debug::DebugCollisionRenderManager );
  if( pManager )
  {
    pManager->SetPlayerPosY( this->GetPlayerPosition().y );
  }
}


#endif


#if PM_DEBUG
// FileRead内容の監視
void Fieldmap::UpdateCheckFileRead_InitializeTime()
{
#if GFL_FS_DEBUG_LOG
  if( m_pGameManager == NULL ){ return ; }

  gfl2::fs::AsyncFileManager* pAsyncFileManager = m_pGameManager->GetAsyncFileManager();

#if defined(DEBUG_ONLY_FOR_takahashi_tomoya)
  u32 num = pAsyncFileManager->GetFsDebugLogArcFileInfoNum();
  const gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfo * cpInfo = NULL;
  static wchar_t  strbuf[128];

  pAsyncFileManager->CalcFsDebugLogArcFileInfo();
  bool is_print =false;

  for( u32 i=0; i<num; ++i )
  {
    cpInfo = pAsyncFileManager->GetFsDebugLogArcFileInfo( i );

    // 同期読み込みのレコードかチェック
    if( cpInfo->kind == gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoKind::SYNC )
    {
      // そのレコードを表示
      pAsyncFileManager->PrintFsDebugLogArcFileInfoBufLight( strbuf, 127, i );
      GFL_PRINT( "FsSync %ls\n", strbuf );
      is_print = true;
    }
  }

  if( is_print )
  {
    GFL_PRINT( "FsSync OneFrame End\n" );
  }
#endif// defined(DEBUG_ONLY_FOR_takahashi_tomoya)

  pAsyncFileManager->ClearFsDebugLogArcFileInfo();

#endif
}

// FileRead内容の監視
void Fieldmap::UpdateCheckFileRead_NormalTime()
{
#if GFL_FS_DEBUG_LOG
  if( m_pGameManager == NULL ){ return ; }

  gfl2::fs::AsyncFileManager* pAsyncFileManager = m_pGameManager->GetAsyncFileManager();

  u32 num = pAsyncFileManager->GetFsDebugLogArcFileInfoNum();

  const gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfo * cpInfo = NULL;
  bool is_error = false;
  s32 syncReadUs = 0;
  s32 errorUs = m_pGameManager->GetGameEventManager()->IsExists() ? 96000 : 0; // イベント起動中かそうでないかでリミットを変更(96000はkujira/sangoと同等)

  pAsyncFileManager->CalcFsDebugLogArcFileInfo();

  for( u32 i=0; i<num; ++i )
  {
    cpInfo = pAsyncFileManager->GetFsDebugLogArcFileInfo( i );

    // 同期読み込みのレコードかチェック
    if( cpInfo->kind == gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoKind::SYNC )
    {
      syncReadUs += cpInfo->diffTime[ gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoTerm::FILE_FINISH_START ];

      if( syncReadUs > errorUs )
      {
        is_error = true;
      }
    }
  }

  m_pDebugInfoWin->UpdateFileReadError( is_error, syncReadUs );


#if defined(DEBUG_ONLY_FOR_takahashi_tomoya)
  static wchar_t  strbuf[128];
  bool is_print = false;
  for( u32 i=0; i<num; ++i )
  {
    cpInfo = pAsyncFileManager->GetFsDebugLogArcFileInfo( i );

    // 同期読み込みのレコードかチェック
    if( cpInfo->kind == gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoKind::SYNC )
    {
      // そのレコードを表示
      pAsyncFileManager->PrintFsDebugLogArcFileInfoBufLight( strbuf, 127, i );
      is_print = true;
      GFL_PRINT( "FsSync %ls\n", strbuf );
    }
  }
  if( is_print )
  {
    GFL_PRINT( "FsSync OneFrame End\n" );
  }
#endif// defined(DEBUG_ONLY_FOR_takahashi_tomoya)

  pAsyncFileManager->ClearFsDebugLogArcFileInfo();

#endif
}
#endif

GFL_NAMESPACE_END( Field );
