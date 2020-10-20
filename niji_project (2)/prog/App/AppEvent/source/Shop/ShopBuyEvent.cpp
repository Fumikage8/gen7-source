//=============================================================================
/**
 * @file    ShopBuyEvent.cpp
 * @brief   ショップ購入画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================

// module
#include "App/AppEvent/include/Shop/ShopBuyEvent.h"
#include "App/Shop/include/ShopProc.h"
#include "App/Shop/include/ShopFrame.h"
// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "Field/FieldRo/include/Fieldmap.h"

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

static const u32 MANAGER_HEAP_SIZE = 0x1000;          //!< マネージャヒープサイズ
static const u32 LOCAL_HEAP_SIZE        = 0x00F000;   //!< ローカルヒープサイズ
static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x180000;   //!< ローカルデバイスヒープサイズ


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
ShopBuyEvent::ShopBuyEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
  , m_parentHeap( NULL )
  , m_pManagerHeap( NULL )
  , m_roShopMobule( NULL )
  , m_pRenderingManager( NULL )
  , m_pFrameManager( NULL )
  , m_pFrame( NULL )
  , m_isDrawEnable( false )
  , m_frameSeq( 0 )
{
  gfl2::std::MemClear( &m_appParam, sizeof(App::Shop::APP_PARAM) );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ShopBuyEvent::~ShopBuyEvent()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2015.10.02
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  ShopBuyEventクラス
 */
//-----------------------------------------------------------------------------
ShopBuyEvent * ShopBuyEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, ShopBuyEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupParam
 * @brief   パラメータ設定
 * @date    2015.10.02
 *
 * @param   type              ショップタイプ
 * @param   id                ショップID
 * @param   howmany_msg       メッセージ：いくつ買う？
 * @param   really_msg        メッセージ：xxコでxx円です
 * @param   really_msg2       メッセージ：xxコでxx円です（複数形）
 * @param   hereitis_msg      メッセージ：まいど
 * @param   youcanttake_msg   メッセージ：持ちきれない
 * @param   nomoney_msg       メッセージ：お金が足りない
 * @param   selected_msg      メッセージ：今日は終了しました（フェスショップ用）
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::SetupParam(
      App::Shop::ShopType type,
      u32 id,
      u32 howmany_msg,
      u32 really_msg,
      u32 really_msg2,
      u32 hereitis_msg,
      u32 youcanttake_msg,
      u32 nomoney_msg,
      u32 selected_msg )
{
  m_appParam.type            = type;
  m_appParam.id              = id;
  m_appParam.howmany_msg     = howmany_msg;
  m_appParam.really_msg      = really_msg;
  m_appParam.really_msg2     = really_msg2;
  m_appParam.hereitis_msg    = hereitis_msg;
  m_appParam.youcanttake_msg = youcanttake_msg;
  m_appParam.nomoney_msg     = nomoney_msg;
  m_appParam.selected_msg    = selected_msg;

  m_appParam.pShopFeatureDelegate = NULL;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetupParamForFC
 * @brief   パラメータ設定(FesCircle用)
 * @date    2015.10.02
 *
 * @param   type              ショップタイプ
 * @param   id                ショップID
 * @param   howmany_msg       メッセージ：いくつ買う？
 * @param   really_msg        メッセージ：xxコでxx円です
 * @param   really_msg2       メッセージ：xxコでxx円です（複数形）
 * @param   hereitis_msg      メッセージ：まいど
 * @param   youcanttake_msg   メッセージ：持ちきれない
 * @param   nomoney_msg       メッセージ：お金が足りない
 * @param   selected_msg      メッセージ：今日は終了しました（フェスショップ用）
 * @param   pShopFeatureDeleagte ShopItem配列提供オブジェクト
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::SetupParamForFC(
      App::Shop::ShopType type,
      u32 id,
      u32 howmany_msg,
      u32 really_msg,
      u32 really_msg2,
      u32 hereitis_msg,
      u32 youcanttake_msg,
      u32 nomoney_msg,
      u32 selected_msg,
      App::Shop::IShopFeatureDelegate* pShopFeatureDeleagte
)
{
  SetupParam(type, id, howmany_msg, really_msg, really_msg2, hereitis_msg, youcanttake_msg, nomoney_msg, selected_msg);
  m_appParam.pShopFeatureDelegate  = pShopFeatureDeleagte;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupHeap
 * @brief   Procが使用するヒープをセット
 * @date    2015.10.10
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::SetupHeap( gfl2::heap::HeapBase * heap )
{
  m_parentHeap = heap;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2015.10.02
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool ShopBuyEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2015.10.02
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.10.02
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT ShopBuyEvent::MainFunc( GameSys::GameManager * gmgr )
{
#if 1
  switch( GetSeqNo() )
  {
  case 0:
    if( InitFrame() == false )
    {
      AddSeqNo();
    }
    break;

  case 1:
    if( MainFrame() == false )
    {
      AddSeqNo();
    }
    break;

  case 2:
    if( EndFrame() == false )
    {
      return GameSys::GMEVENT_RES_FINISH;
    }
    break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;

#else

  enum
  {
    EVSEQ_PROC_CREATE = 0,
    EVSEQ_PROC_WAIT,
    EVSEQ_PROC_END,
  };

  switch( GetSeqNo() )
  {
  case EVSEQ_PROC_CREATE:
    {
      App::Shop::ShopProc * proc = GameSys::SetNextShopProc();
      proc->Setup( &m_appParam, m_parentHeap );
    }
    SetSeqNo( EVSEQ_PROC_WAIT );
    break;

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      SetSeqNo( EVSEQ_PROC_END );
    }
    break;

  case EVSEQ_PROC_END:
#if PM_DEBUG
    if( IsLauncherCall() != false )
    {
      debug::launcher::LauncherProc::CallParam param;
      debug::launcher::LauncherProc::InitCallParam( &param );
      debug::launcher::LauncherProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( gmgr->GetProcManager(), &param );
      proc->CreateStartListData< debug::startmenu::StartLauncher >();
    }
#endif  // PM_DEBUG
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;

#endif

}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.10.02
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::EndFunc( GameSys::GameManager * gmgr )
{
}


//-----------------------------------------------------------------------------
/**
 * @func    InitFrame
 * @brief   フレーム初期化
 * @date    2015.10.22
 *
 * @retval  true  = 処理中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopBuyEvent::InitFrame(void)
{
  switch( m_frameSeq )
  {
  case 0:
    CreateHeap();
    CreateRenderManager();
    m_frameSeq++;
    /* FALL THROUGH */
  case 1:
    m_pRenderingManager->Update();
    if( m_pRenderingManager->IsCreated() == false )
    {
      break;
    }
    CreateFrame();
    SetupFieldMap( this );
    m_isDrawEnable = true;
    m_frameSeq = 0;
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    MainFrame
 * @brief   フレームメイン処理
 * @date    2015.10.22
 *
 * @retval  true  = 処理中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopBuyEvent::MainFrame(void)
{
  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    m_isDrawEnable = false;
    return false;
  }
  m_pRenderingManager->Update();
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFrame
 * @brief   フレーム終了処理
 * @date    2015.10.22
 *
 * @retval  true  = 処理中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopBuyEvent::EndFrame(void)
{
  switch( m_frameSeq )
  {
  case 0:
    if( DeleteFrame() == false )
    {
      break;
    }
    SetupFieldMap( NULL );
    /*  結果の通知  */
    if(m_appParam.pShopFeatureDelegate)
    {
      m_appParam.pShopFeatureDelegate->ShopFeatureDelegate_HandlingResult(m_appParam);
    }
    //スクリプトへ結果をわたす
    if (m_appParam.type == Shop::TYPE_BP_WAZA)
    {
      Field::FieldScript::Work* p_work = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldScriptSystem()->GetScriptObject()->GetWork();
      p_work->SetWorkValue(SCWK_RET0, m_appParam.select_action); //s16 BP技覚え用：選択した行動/技番号
      p_work->SetWorkValue(SCWK_RET1, m_appParam.select_cost);   //s16 BP技覚え用：選択した行動で消費するコイン/BP
      p_work->SetWorkValue(SCWK_RET2, m_appParam.select_pos);    //u32 BP技覚え用：選択したリスト位置　※キャンセル時は不定値
      //何も選ばなかった（キャンセル）時はSCWK_ANSWERに０をセット、選んだときは１をセット
      if ((m_appParam.select_action == Shop::BP_WAZA_SELECT_ACTION_CANCEL))
      {
        p_work->SetWorkValue(SCWK_ANSWER, 0);
      }
      else
      {
        p_work->SetWorkValue(SCWK_ANSWER, 1);
      }
    }
    m_frameSeq++;
    /* FALL THROUGH */
  case 1:
    if( DeleteRenderManager() == false )
    {
      break;
    }
    DeleteHeap();
    m_frameSeq = 0;
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    PreDrawFunc
 * @brief   描画下準備
 * @date    2015.10.22
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::PreDrawFunc(void)
{
  if( m_isDrawEnable != false )
  {
    m_pRenderingManager->PreDraw();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   描画処理
 * @date    2015.10.22
 *
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::DrawFunc( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    m_pFrameManager->Draw( displayNo );
    m_pRenderingManager->Draw( displayNo );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateHeap
 * @brief   ヒープ生成
 * @date    2015.10.22
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::CreateHeap(void)
{
  GFL_ASSERT_STOP( m_parentHeap );

  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( m_parentHeap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  // ローカルヒープ生成
  m_heap.LocalHeapCreate( m_parentHeap, m_parentHeap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteHeap
 * @brief   ヒープ削除
 * @date    2015.10.22
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::DeleteHeap(void)
{
  // ローカルヒープ削除
  m_heap.Delete();
  // マネージャヒープ削除
  GFL_DELETE_HEAP( m_pManagerHeap );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateRenderManager
 * @brief   描画マネージャ生成
 * @date    2015.10.22
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::CreateRenderManager(void)
{
  m_pRenderingManager = GFL_NEW(m_pManagerHeap) app::util::AppRenderingManager();

  app::util::AppRenderingManager::Description desc;
  for( int i=0; i<app::util::AppRenderingManager::RenderPlaceType::NUM; i++ )
  {
    desc.renderDesc[i].renderOrder     = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[i].pokeModelNumMax = 0;
    desc.renderDesc[i].modelNumMax     = 0;
  }

  m_pRenderingManager->StartAsyncCreate(
    desc,
    &m_heap,
    NULL,
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRenderManager
 * @brief   描画マネージャ削除
 * @date    2015.10.22
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopBuyEvent::DeleteRenderManager(void)
{
  if( m_pRenderingManager->CanDestroy() == false )
  {
    return false;
  }
  GFL_DELETE m_pRenderingManager;
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateFrame
 * @brief   フレーム生成
 * @date    2015.10.22
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::CreateFrame(void)
{
  {
    GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    m_roShopMobule = rm->LoadModule( gm->GetAsyncFileManager(), "Shop.cro" );
    rm->StartModule( m_roShopMobule, true );
  }
  m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
  m_pFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, App::Shop::ShopFrame >( m_pFrameManager );
  m_pFrame->Setup( &m_appParam, &m_heap, m_pRenderingManager );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFrame
 * @brief   フレーム削除
 * @date    2015.10.22
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopBuyEvent::DeleteFrame(void)
{
  if( m_pFrameManager == NULL )
  {
    return true;
  }
  if( m_pFrameManager->End() != false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pFrameManager );
  GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager)->DisposeModule( m_roShopMobule );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupFieldMap
 * @brief   イベントをフィールドマップにセット
 * @date    2015.10.22
 *
 * @param   p   イベントのポインタ
 */
//-----------------------------------------------------------------------------
void ShopBuyEvent::SetupFieldMap( ShopBuyEvent * p )
{
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->SetShopBuyEvent( p );
}


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
