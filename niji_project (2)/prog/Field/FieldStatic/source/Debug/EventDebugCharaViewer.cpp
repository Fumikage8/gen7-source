//======================================================================
/**
 * @file EventDebugCharaViewer.cpp
 * @date 2016/03/16
 * @author saita_kazuki
 * @brief デバッグ：CharaViewer
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "Field/FieldStatic/include/Debug/EventDebugCharaViewer.h"

// event
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

// test
#include "Test/CharaViewer/include/CharaViewerProc.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @brief   イベント起動
 * @param   gmgr ゲームマネージャクラス
 * @param   desc 設定
 * @return  EventDebugItemMakeクラス
 */
EventDebugCharaViewer* EventDebugCharaViewer::StartEvent( GameSys::GameManager *gmgr, const Description& desc )
{
  GameSys::GameEventManager* pEventManager = gmgr->GetGameEventManager();
  EventDebugCharaViewer* pEvent = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventDebugCharaViewer >( pEventManager );
  pEvent->SetDescription( desc );
  return pEvent;
}

/**
 *  @class  EventDebugCharaViewer
 *  @brief  コンストラクタ
 *  @param  pHeap   workをとるためのメモリクラス
 *  @param  desc 初期化情報
 */
EventDebugCharaViewer::EventDebugCharaViewer( gfl2::heap::HeapBase *pHeap )
  : GameSys::GameEvent( pHeap )
  , m_desc()
{
}

/**
 *  @brief  イベントを動かしてよいかどうかの判定関数
 *  @param  pGameManager  ゲームマネージャ
 *  @retval true  起動して良い
 *  @retval false 起動してはいけない
 */
bool EventDebugCharaViewer::BootChk( GameSys::GameManager *pGameManager )
{
  return true;
}

/**
 *  @brief  初期化関数
 *  @param  pGameManager  ゲームマネージャ
 */
void EventDebugCharaViewer::InitFunc( GameSys::GameManager *pGameManager )
{
}

/**
 *  @brief  メイン関数
 *  @param  pGameManager    ゲームマネージャ
 *  @return GMEVENT_RESUTLT イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT EventDebugCharaViewer::MainFunc( GameSys::GameManager *pGameManager )
{
  enum
  {
    SEQ_CLOSE_FIELD,
    SEQ_CALL_PROC,
    SEQ_WAIT_PROC,
    SEQ_OPEN_FIELD,
    SEQ_END,
  };

  switch( this->GetSeqNo() )
  {
  case SEQ_CLOSE_FIELD:
    if( m_desc.isFieldControl )
    {
      // クローズイベント
      EventFieldmapClose * p_event = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventFieldmapClose >( pGameManager->GetGameEventManager() );
      EventFieldmapClose::Desc desc;
      desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
      p_event->SetDesc( desc );
      p_event->SetFadeCallBackDefault();  //フェードはデフォルト
    }
    this->AddSeqNo();
    break;

  case SEQ_CALL_PROC:
    m_pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, test::chara_viewer::CharaViewerProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "CharaViewer.cro" );	 
    m_pProc->SetFieldDebugModeEnabled(true);
    this->AddSeqNo();
    break;

  case SEQ_WAIT_PROC:
    if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
    {
      this->AddSeqNo();
    }
    break;

  case SEQ_OPEN_FIELD:
    if( m_desc.isFieldControl )
    {
      EventFieldmapOpen * p_event = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventFieldmapOpen >( pGameManager->GetGameEventManager() );
      p_event->SetDesc( FM_OPEN_MODE_APP_RECOVER, *pGameManager->GetGameData()->GetStartLocation() );
      p_event->SetFadeCallBackDefault();
    }
    this->AddSeqNo();
    break;

  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;

}

/**
 *  @brief  終了関数
 *  @param  pGameManager    ゲームマネージャ
 */
void EventDebugCharaViewer::EndFunc( GameSys::GameManager *pGameManager )
{
}

/**
 * @brief   設定
 * @param   desc 設定
 */
void EventDebugCharaViewer::SetDescription( const Description& desc )
{
  m_desc = desc;
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG

