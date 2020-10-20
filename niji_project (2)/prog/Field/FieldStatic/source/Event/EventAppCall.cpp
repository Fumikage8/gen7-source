//=================================================================================================
/**
 * @file    EventAppCall.cpp
 * @brief   アプリケーションのイベントを呼び出すイベント
 * @author  obata_toshihiro   k.ohno
 * @date    2011.06.29   2015.6.9
 */
//=================================================================================================

#if 1
#include "Field/FieldStatic/include/Event/EventAppCall.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <Field/FieldStatic/source/Event/EventFieldmapControl.h>

#include "Sound/include/Sound.h"

#include "Field\FieldStatic\include\Launcher\FieldApplicationLauncher.h"

#include "App/AppEvent/include/Bag/BagEvent.h"

#include "FieldScript/include/EventScriptCall.h"
#include "niji_conv_header/field/script/inc/field_event.inc"

#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h" 


GFL_NAMESPACE_BEGIN( Field )

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
EventAppCall::EventAppCall( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap ),
m_call_target( CALL_TARGET_POKELIST ),
m_fadeMode( E_FADE_EXEC ),
m_param(NULL),
m_param_alloc_flag(false),
m_context(NULL),
m_end_callback_func(NULL),
m_before_field_open_callback_func(NULL),
//m_fieldCloseType(FIELD_CLOSE_RESIDENT_APP_KEEP),
m_field_control(false),
m_is_bgm_push(false),
m_seq( 0 ),
m_pAppParam(NULL),
m_isFoceOut(false)
{
  m_heap = heap;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 呼び出すアプリケーションを設定する
 *
 * @param call_target
 * @param p_param
 * @param enc_callback_func
 * @param before_field_open_callback_func
 *
 * @note 終了、フィールドオープン前コールバックを設定できる
 */
/* -------------------------------------------------------------------------*/
void EventAppCall::SetCallTargetApplication(
  CallTarget call_target,
  void* p_param,
  CALLBACK_FUNC enc_callback_func,
  CALLBACK_FUNC before_field_open_callback_func )
{
  // 既にセットされている場合はアサート
  if( m_param )
  {
    GFL_ASSERT(0); //,"既にパラメータがセットされています\n");
  }
  else
  {
    m_param = p_param;
  }

  m_call_target = call_target;
  m_field_control = true;

  // 終了コールバック設定
  m_end_callback_func = enc_callback_func;
  // フィールドオープン前コールバック
  m_before_field_open_callback_func = before_field_open_callback_func;
  // コールバック用ワークを作成
  m_callback_work.p_param = p_param;
  m_callback_work.p_app_call = this;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 呼び出すアプリケーションを設定する(パラメータアロック番)
 *
 * @param call_target       呼び出すアプリケーション
 * @param size              パラメータサイズ
 * @param enc_callback_func 終了コールバック
 * @param before_field_open_callback_func フィールドオープン前コールバック
 *
 * @return パラメータポインタ
 */
/* -------------------------------------------------------------------------*/
void* EventAppCall::SetCallTargetApplicationAndAllocParam(
  CallTarget call_target,
  u32 size,
  CALLBACK_FUNC enc_callback_func,
  CALLBACK_FUNC before_field_open_callback_func )
{
  // パラメータをアロックする
  void* p_param = AllocParam(size);

  // 終了コールバック設定
  m_end_callback_func = enc_callback_func;
  // フィールドオープン前コールバック
  m_before_field_open_callback_func = before_field_open_callback_func;
  m_call_target = call_target;
  m_field_control = true;

  // コールバック用ワークを作成
  m_callback_work.p_param = p_param;
  m_callback_work.p_app_call = this;

  return p_param;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 呼び出すアプリケーションを設定する(パラメータアロック、フィールドをコントロールしない)
 *
 * @param call_target       呼び出すアプリケーション
 * @param size              パラメータサイズ
 * @param enc_callback_func 終了コールバック
 * @param before_field_open_callback_func フィールドオープン前コールバック
 *
 * @return パラメータポインタ
 */
/* -------------------------------------------------------------------------*/
void* EventAppCall::SetCallTargetApplicationAndAllocParamNotFieldControl(
  CallTarget call_target,
  u32 size,
  CALLBACK_FUNC enc_callback_func,
  CALLBACK_FUNC before_field_open_callback_func )
{
  void* p_param = SetCallTargetApplicationAndAllocParam( call_target, size ,enc_callback_func , before_field_open_callback_func );
  m_field_control = false;
  return p_param;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 戻り値の設定
 *
 * @param ret_wk1 戻り値1（イベントワーク）
 * @param ret_wk2 戻り値2（イベントワーク）
 * @param ret_wk3 戻り値3（イベントワーク）
 * @param ret_wk4 戻り値4（イベントワーク）
 */
/* ------------------------------------------------------------------------- */
void EventAppCall::SetRetWork( u16 ret_wk1, u16 ret_wk2, u16 ret_wk3, u16 ret_wk4 )
{
  m_callback_work.ret_wk1 = ret_wk1;
  m_callback_work.ret_wk2 = ret_wk2;
  m_callback_work.ret_wk3 = ret_wk3;
  m_callback_work.ret_wk4 = ret_wk4;
}

//--------------------------------------------------------------
// 呼び出すアプリケーションを設定する
//--------------------------------------------------------------
void EventAppCall::SetCallTargetApplicationNotFieldControl(
  CallTarget call_target,
  void* p_param,
  CALLBACK_FUNC enc_callback_func,
  CALLBACK_FUNC before_field_open_callback_func )
{
  SetCallTargetApplication( call_target , p_param ,enc_callback_func ,before_field_open_callback_func );
  m_field_control = false;
}

//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
EventAppCall::~EventAppCall()
{
  GFL_PRINT("EventAppCall 終了\n");
}





//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool EventAppCall::BootChk( GameSys::GameManager* game_manager )
{
  GFL_UNUSED( game_manager );
  return true;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void EventAppCall::InitFunc( GameSys::GameManager* game_manager )
{
  GFL_UNUSED( game_manager );
}



//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventAppCall::MainFunc( GameSys::GameManager* p_gman )
{
  enum
  {
    SEQ_RESORT = 0,
    SEQ_SET_EVENT,
    SEQ_WAIT_VOLUME,
    SEQ_CALL_APP_EVENT,
    SEQ_APP_CALLBACK_B,
    SEQ_RCV_FIELD,
    SEQ_APP_CALLBACK_A,
    SEQ_WAIT_RESORT,
    SEQ_END
  };


#ifdef DEBUG_ONLY_FOR_uchida_yuto
  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_FIELD_CLOSE,
    TICK_FIELD_OPEN,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(EventAppCall_MainFunc,TICK_MAX);
#endif

  switch( m_seq )
  {
  case SEQ_RESORT:
    if( m_AppNo == Field::SubScreen::OUT_REQ_ID_POKE_PLANET )
    {
      GameSys::GameData * gd = p_gman->GetGameData();
      Field::ZoneID zone_id = gd->GetStartLocation()->GetZoneID();
      if( zone_id != Field::ZONEID_ERROR )
      {
        void * zone_data = gd->GetFieldZoneDataLoader()->GetZoneData( zone_id );
        Field::ZoneDataAccessor accessor( zone_data );
        if( accessor.IsPokeRideFlyEnable() == false )
        {
          Field::EventScriptCall::CallScript( p_gman, SCRID_FLD_EV_RESORT_JUMP_EFFECT, NULL );
          m_isFoceOut = true;
          m_seq = SEQ_WAIT_RESORT;
          break;
        }
      }
    }
    m_seq = SEQ_SET_EVENT;
    /* FALL THROUGH */
  case SEQ_SET_EVENT:
    {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      GFL_DEBUG_TICK_SET_FLAG(EventAppCall_MainFunc,true);
      GFL_DEBUG_TICK_RESET(EventAppCall_MainFunc);

      GFL_DEBUG_TICK_START(EventAppCall_MainFunc, TICK_FIELD_CLOSE, "EventFieldClose TOTAL");
#endif

      // BGMのプッシュ
      if( m_is_bgm_push == true )
      {
        Sound::PushBGMReq(); 
      }

      // フィールドマップから生成するデータを設定
      Field::Application::SetupFieldmapData( p_gman, &m_fieldmapData );

      // 復帰時に設定するため、現在のロケーションを保持しておく
      // 復帰時に設定するため、現在のロケーションを保持しておく
      m_playerLocation.worldId  = p_gman->GetFieldmap()->GetWorldID();
      m_playerLocation.zoneId   = p_gman->GetFieldmap()->GetZoneID();
      m_playerLocation.position = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;
      m_playerLocation.quaternion = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().rotation;


      //	m_playerLocation = p_gman->GetFieldmap()->GetLocatioin();
      //  m_playerLocation.position = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;

      // クローズイベント
      EventFieldmapClose* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
        p_gman->GetGameEventManager( ) );
      EventFieldmapClose::Desc desc;
      desc.closeType = FM_CLOSE_MODE_APP; // メモリを破棄

      if(m_AppNo==Field::SubScreen::OUT_REQ_ID_POKE_PLANET){
        //１、EventFieldmapCloseコール時に
        //　　EventFieldmapClose::DescのisSkyboxKeepをtrueにして
        //　　EventFieldmapClose::SetDescに渡してあげます。
        desc.isSkyboxKeep = true;
        
      }

      // メモリに余裕のあるのアプリだけは、一部リソースを保持したままジャンプする。
      switch( m_AppNo )
      {
      case Field::SubScreen::OUT_REQ_ID_POKELIST:
      case Field::SubScreen::OUT_REQ_ID_ZUKAN:
      case Field::SubScreen::OUT_REQ_ID_BAG:
      //case Field::SubScreen::OUT_REQ_ID_ALBUM:
      case Field::SubScreen::OUT_REQ_ID_TR_CARD:
      case Field::SubScreen::OUT_REQ_ID_POKE_PLANET:
      case Field::SubScreen::OUT_REQ_ID_CONFIG:
        desc.closeType = FM_CLOSE_MODE_APP_KEEPRES; // リソースを保持したままアプリジャンプする
        break;
      }

      p_event->SetDesc( desc );
      if( m_isFoceOut == false )
      {
        p_event->SetFadeCallBackDefault();  //デフォルトフェード
      }
      else
      {
        p_event->SetFadeCallBack( NULL, NULL, NULL ); // フェードしない
      }
      m_seq = SEQ_WAIT_VOLUME;
    }
    break;

  case SEQ_WAIT_VOLUME:
    {
      int chg_vol = CheckVolumeControl();
      if( chg_vol == 1 )
      {
        // @note QR読み込みもここを通るが、図鑑と同じ音量なので専用に定義はしていない
        Sound::ChangeBGMPlayerVolume( Sound::BGM_VOLUME_ZUKAN, Sound::BGM_FADE_APP );
      }
      else if( chg_vol == 2 )
      {
        Sound::ChangeBGMPlayerVolume( Sound::BGM_VOLUME_APP, Sound::BGM_FADE_APP );
      }
    }
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    GFL_DEBUG_TICK_END(EventAppCall_MainFunc, TICK_FIELD_CLOSE);
#endif
    m_seq = SEQ_CALL_APP_EVENT;
    break;

  case SEQ_CALL_APP_EVENT:
    {
      GameSys::GameEvent* p_event = NULL;
      p_event = Field::Application::CallAppEvent( m_AppNo, p_gman, &m_fieldmapData );
      // 各アプリケーションごとのリスナー設定(必要に応じて)
      SetupEvent( p_event );
    }
    m_seq = SEQ_APP_CALLBACK_B;
    break;

  case SEQ_APP_CALLBACK_B:
    {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      GFL_DEBUG_TICK_START(EventAppCall_MainFunc, TICK_FIELD_OPEN, "EventFieldOpen TOTAL");
#endif
      if( m_before_field_open_callback_func )
      {
        m_before_field_open_callback_func( &m_callback_work, p_gman );
      }
    }
    m_seq = SEQ_RCV_FIELD;
    break;

  case SEQ_RCV_FIELD:
    if( m_field_control )
    {
      if( CheckVolumeControl() != 0 ){
        Sound::ChangeBGMPlayerVolume( Sound::BGM_VOLUME_NORMAL, Sound::BGM_FADE_APP );
      }
      // BGMのポップ
      if( m_is_bgm_push == true )
      {
        Sound::PopBGMReq( Sound::BGM_FADE_FAST );
      }
      {
        EventFieldmapOpen* p_op_event;
        p_op_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
                p_gman->GetGameEventManager( ) );
        p_op_event->SetDesc( FM_OPEN_MODE_APP_RECOVER, m_playerLocation);

        p_op_event->SetFadeCallBackDefault();
      }
    }
    m_seq = SEQ_APP_CALLBACK_A;
    break;

  case SEQ_APP_CALLBACK_A:
    this->RetAppProc( p_gman );
    if( m_end_callback_func )
    {
      m_end_callback_func( &m_callback_work, p_gman );
    }
    m_seq = SEQ_END;
    break;

  case SEQ_WAIT_RESORT:
    m_seq = SEQ_SET_EVENT;
    break;

  case SEQ_END:
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    GFL_DEBUG_TICK_END(EventAppCall_MainFunc, TICK_FIELD_OPEN);

    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(EventAppCall_MainFunc);
    gfl2::debug::ResetPrintFlag();
#endif
    GFL_PRINT("イベント終わり\n");
    return GameSys::GMEVENT_RES_FINISH;


  default:
    GFL_ASSERT(0);
    break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  BGMのボリュームを変更するかをチェック
 */
//-----------------------------------------------------------------------------
int EventAppCall::CheckVolumeControl(void)
{
  // 図鑑とQR読み込み
  if( m_call_target == CALL_TARGET_ZUKAN ||
      m_call_target == CALL_TARGET_QR_READ )
  {
    return 1;
  }
  if( m_call_target == CALL_TARGET_CONFIG ||
      m_call_target == CALL_TARGET_TRAINERPASS )
  {
    return 2;
  }
  return 0;
}

/* -------------------------------------------------------------------------*/
/**
 *  @brief イベントのセットアップを行なう
 *
 *  @param  pEvent  イベント
 */
/* -------------------------------------------------------------------------*/
void EventAppCall::SetupEvent( GameSys::GameEvent* pEvent )
{
  switch( m_AppNo )
  {
  case Field::SubScreen::OUT_REQ_ID_BAG:
    {
      // APP_PARAMを生成
      m_pAppParam = GflHeapAllocMemory( m_heap, sizeof(app::bag::APP_PARAM) );

      // ゲームイベントに渡す
      App::Event::BagEvent* pBagEvent = reinterpret_cast<App::Event::BagEvent*>( pEvent );
      pBagEvent->SetupAppParam( reinterpret_cast<app::bag::APP_PARAM*>(m_pAppParam) );
      pBagEvent->SetupFieldParam( &m_fieldmapData.item_use_work, m_fieldmapData.weather );
    }
    break;
  }
}



//----------------------------------------------------------------------------
/**
 *  @brief  アプリケーションからの戻り値をチェック
 */
//-----------------------------------------------------------------------------
void EventAppCall::RetAppProc( GameSys::GameManager* p_gman )
{
  switch( m_AppNo )
  {
  case Field::SubScreen::OUT_REQ_ID_BAG:
    {
      // アイテム呼び出し
      app::bag::APP_PARAM* p_param = static_cast<app::bag::APP_PARAM*>(m_pAppParam);
      if ((p_param->call_mode == app::bag::CALL_FIELD_MENU) && (p_param->select_item != ITEM_DUMMY_DATA))
      {
        FieldItemUse::CallItem( p_gman ,p_param->select_item );
      }
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  プロセスのパラメータを生成
 *
 *  @param  size  サイズ
 */
//-----------------------------------------------------------------------------
void* EventAppCall::AllocParam( u32 size )
{
  if( !m_param_alloc_flag )
  {
    void * p_work = GflHeapAllocMemory( HEAPID_EVENT_DEVICE, size );
    m_param = p_work;
    m_param_alloc_flag = true;
  }else{
    GFL_ASSERT_STOP_MSG(0,"EventAppCall::AllocParamが多重で呼ばれました。\n");
  }
  return m_param;
}


//----------------------------------------------------------------------------
/**
 *  @brief  プロセスのパラメータを破棄
 */
//-----------------------------------------------------------------------------
void EventAppCall::FreeParam( void )
{
  if( m_param_alloc_flag ){
    GflHeapFreeMemory( m_param );
    m_param = NULL;
    m_param_alloc_flag = false;
  }
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void EventAppCall::EndFunc( GameSys::GameManager* p_gman )
{
  GFL_UNUSED( p_gman );

  this->FreeParam();
  GflHeapSafeFreeMemory( m_pAppParam );
}

//--------------------------------------------------------------
/**
 * @brief フィールドからアプリケーションに切り替える際 / アプリケーションからフィールドに戻る際、
 *        フェードアウトを行うかどうかを設定する。
 */
//--------------------------------------------------------------
void EventAppCall::SetFadeMode(FadeMode mode)
{
  m_fadeMode = mode;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief フィールドをコントロールするか切り替える
 *
 * @param is_control フィールドコントロールフラグ
 */
/* ------------------------------------------------------------------------- */
void EventAppCall::SetFieldControl( bool is_control )
{
  m_field_control = is_control;
}


/* -------------------------------------------------------------------------*/
/**
 * @brief アプリ切り替え時にBGMのプッシュ/ポップを行うか設定する
 *
 * @param is_push trueのときBGMをプッシュ/ポップする
 */
/* -------------------------------------------------------------------------*/
void EventAppCall::SetBgmPushPopFlag(bool is_push )
{
  m_is_bgm_push = is_push;
}




GFL_NAMESPACE_END( Field )
#endif
