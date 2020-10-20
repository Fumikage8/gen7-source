//==============================================================================
/**
 * @file        FieldDemoEvent.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/06/15(月) 18:50:52
 */
//==============================================================================

// module
#include "App/AppEvent/include/FieldDemo/FieldDemoEvent.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "App/FieldDemo/include/FieldDemoProc.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
FieldDemoEvent::FieldDemoEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
FieldDemoEvent::~FieldDemoEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2015.06.11
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  FieldDemoEventクラス
 */
//-----------------------------------------------------------------------------
FieldDemoEvent * FieldDemoEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, FieldDemoEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupParam
 * @brief   外部設定パラメータをセット
 * @date    2015.06.11
 *
 * @param   param   外部設定パラメータ
 */
//-----------------------------------------------------------------------------
void FieldDemoEvent::SetupParam( const s32 iId, const bool isFade, const s32 demoType, const Field::Demo::DressUpSuitType suitType )
{
  m_desk.id = iId;
  m_desk.isFade = isFade;
  m_desk.demoType = demoType;
  m_desk.suitType = suitType;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupParam
 * @brief   外部設定パラメータをセット
 * @date    2015.06.11
 *
 * @param   param   外部設定パラメータ
 */
//-----------------------------------------------------------------------------
void FieldDemoEvent::SetupParam( const FieldDemoEventDesk& desk )
{
  m_desk = desk;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool FieldDemoEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void FieldDemoEvent::InitFunc( GameSys::GameManager * gmgr )
{
  //復帰座標の設定
  m_playerLocation = *gmgr->GetGameData()->GetStartLocation();

  //もしフィールドが無かったら、フィールドクローズをスキップする
  if( gmgr->GetFieldmap() == NULL ){
    SetSeqNo( EVSEQ_PROC_CREATE );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT FieldDemoEvent::MainFunc( GameSys::GameManager * gmgr )
{
  switch( GetSeqNo() )
  {
  case EVSEQ_PROC_FIELCLOSE:
    {
      Field::EventFieldmapClose* p_event;

      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventFieldmapClose > ( gmgr->GetGameEventManager( ) );
      Field::EventFieldmapClose::Desc desc;
      desc.closeType =  Field::FM_CLOSE_MODE_APP; // メモリを全破棄
      p_event->SetDesc( desc);
      if( m_desk.isFade ) p_event->SetFadeCallBackDefault();  //フェードはデフォルト
      else                p_event->SetFadeCallBack( NULL, NULL, NULL );
    }
    SetSeqNo( EVSEQ_PROC_CREATE );
    break;
  case EVSEQ_PROC_CREATE:
    {
      Field::Demo::FieldDemoProc * proc = GameSys::CallFieldDemoProc( m_desk.id, m_desk.demoType );
      if ( m_desk.suitType != Field::Demo::SUIT_TYPE_NONE )
      {
        GameSys::GameData * gd = gmgr->GetGameData();
        u8 sex = gd->GetPlayerStatusConst()->GetSex();
        m_dress_up_param = gd->GetPlayerStatusConst()->GetDressup();
        if (m_desk.suitType == Field::Demo::SUIT_TYPE_LAMD)
        {
          System::DressUp::OverwrideDressUpParamRide( sex, true, &m_dress_up_param );
        }
        else if (m_desk.suitType == Field::Demo::SUIT_TYPE_SEA)
        {
          System::DressUp::OverwrideDressUpParamRide( sex, false, &m_dress_up_param );
        }
        else if (m_desk.suitType == Field::Demo::SUIT_TYPE_SPACE)
        {
          System::DressUp::OverwrideDressUpParamSpace( sex, &m_dress_up_param );
        }
        else
        {
          GFL_ASSERT(0);
        }
        proc->SetDressUpParam( &m_dress_up_param, (sex==PM_MALE) );
      }
//      Field::Demo::FieldDemoProc * proc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, Field::Demo::FieldDemoProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
//  r:/home/niji/git_program/niji_project/prog/App/FieldDemo/include/FieldDemoProc.h(24): class FieldDemoProc : public GameSys::GameProc
    }
    SetSeqNo( EVSEQ_PROC_WAIT );
    break;

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if( gmgr->IsProcessExists() )
    {
      break;
    }
    if( m_desk.isFieldOpen ){
      SetSeqNo( EVSEQ_PROC_FIELDOPEN );
    }else{
      SetSeqNo( EVSEQ_PROC_END );
    }
    break;
  case EVSEQ_PROC_FIELDOPEN:
    {
      Field::EventFieldmapOpen* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventFieldmapOpen > ( gmgr->GetGameEventManager( ) );
      p_event->SetDesc( Field::FM_OPEN_MODE_APP_RECOVER, m_playerLocation );

      if( m_desk.isFade ) p_event->SetFadeCallBackDefault();     //フェードはデフォルト
      else                p_event->SetFadeCallBack( NULL, NULL, NULL );
    }
    SetSeqNo( EVSEQ_PROC_END );
    break;
  case EVSEQ_PROC_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void FieldDemoEvent::EndFunc( GameSys::GameManager * gmgr )
{
}


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
