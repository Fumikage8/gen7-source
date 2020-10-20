//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventRizaadonRide.cpp
 *  @brief  リザードンライドイベント
 *  @author miyuki iwasawa 
 *  @date   2015.09.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// pml
#include <pml/include/pmlib.h>

#include "Savedata/include/MyStatus.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

#include "Field/FieldStatic/include/Event/EventRizaadonRide.h"
#include "Field/FieldStatic/include/Event/EventProcessCall.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"


#include "FieldScript/include/EventScriptCall.h" // for CallScript
#include "FieldScript/include/FieldScriptSystem.h"

#include "App/AppEvent/include/TownMap/TownMapEvent.h"
#include "App/AppEvent/include/TownMap/TownMapEventListener.h"

#include "GameSys/include/DllProc.h"
#include "App/FieldDemo/include/FieldDemoProc.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>
#include <arc_index/demo_seq_script.gaix>

namespace Field{
  
//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *  @brief  リザードンライドシーケンス　コール
 *
 *  @param  p_gman        ゲームマネージャ
 *  @param  add_poke_id   AddPokeID
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
EventRizaadonRide* EventRizaadonRide::CallRizaadonRide( GameSys::GameManager* p_gman)
{
  EventRizaadonRide* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventRizaadonRide>( p_gman->GetGameEventManager() );

  // イベントにパラメータを通達

  return p_event;
}

//-----------------------------------------------------------------------------
/**
 *    イベントの実装
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ 
 */
//-----------------------------------------------------------------------------
EventRizaadonRide::EventRizaadonRide( gfl2::heap::HeapBase* heap ):
  GameSys::GameEvent( heap )
  ,m_Seq(0)
  ,m_WarpID(App::TownMap::FLY_WARPID_CANCEL)
  ,m_pHeap(NULL)
{
}

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventRizaadonRide::BootChk(GameSys::GameManager* /*p_gman*/)
{
  return true;
} 

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventRizaadonRide::InitFunc(GameSys::GameManager* p_gman)
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  m_pGameManager=p_gman;

  m_pFieldCharaModelManager   = p_gman->GetGameData()->GetFieldCharaModelManager();
  m_pPlayer                   = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( m_pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
} 

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventRizaadonRide::MainFunc(GameSys::GameManager* p_gman)
{
  enum{
    SEQ_FIELD_CLOSE,
    SEQ_APP_CALL,
    SEQ_APP_WAIT,
    SEQ_DEMO_CALL,
    SEQ_DEMO_WAIT,
    SEQ_FIELD_OPEN,
    SEQ_END,
  };
  switch( m_Seq ){
  case SEQ_FIELD_CLOSE:
    {
      // クローズイベント
      EventFieldmapClose* p_event;
      EventFieldmapClose::Desc desc;
      
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
                  p_gman->GetGameEventManager( ) );
      desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
      p_event->SetDesc( desc );
      p_event->SetFadeCallBackDefault();  //フェードはデフォルト
    }
    m_Seq++;
    break;
  case SEQ_APP_CALL:
    CallTownmap(p_gman);
    m_Seq++;
    break;
  case SEQ_APP_WAIT:
    {
      if( m_WarpID == App::TownMap::FLY_WARPID_CANCEL ){
        m_Seq = SEQ_FIELD_OPEN; //キャンセルされたので、そのまま帰る
      }else{
        m_Seq = SEQ_DEMO_CALL; //デモコール
      }
    }
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_DEMO_CALL:
    {
      s32 demo_seq_no = GARC_demo_seq_script_sd9080_fly_BSEQ;
      Field::Demo::FieldDemoProc* proc = NULL;
      GameSys::GameData * gd = p_gman->GetGameData();
      u8 sex = gd->GetPlayerStatusConst()->GetSex();

      m_DressUpParam = gd->GetPlayerStatusConst()->GetDressup( );

      // 着せ替え用のドレスアップデータ適用
      System::DressUp::OverwrideDressUpParamRide( sex, true, &m_DressUpParam );

      //デモコール
      if (gd->CheckFlyDemoShowed())
      {
        demo_seq_no = GARC_demo_seq_script_sd9080_fly_short_BSEQ;//ショート版
      }
      else
      {
        gd->SetFlyDemoShowed();
      }
      proc = GameSys::CallFieldDemoProc( demo_seq_no );
      proc->SetDressUpParam(&m_DressUpParam, (sex==PM_MALE));
    }
    
    m_Seq++;
    break;
  case SEQ_DEMO_WAIT:
    {
      // プロセスの終了待ち
      if( p_gman->IsProcessExists() )
      {
        break;
      }
      // レコード(ライド回数) #GFNMCat[1129]
      Savedata::IncRecord( Savedata::Record::RECID_RIDE_POKE );

      //選んだ座標にワープ
      EventMapJump::CallMapJumpPosWithoutField( p_gman,
        m_WarpID, EventMapJump::WARP_TYPE_FLY,EventMapJumpWithoutField::EV_MAPJUMP_BLACK_FADE );
    }
    m_Seq = SEQ_END;
    break;

  case SEQ_FIELD_OPEN:
    {
      EventFieldmapOpen* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
        p_gman->GetGameEventManager( ) );
      p_event->SetDesc( FM_OPEN_MODE_APP_RECOVER, *p_gman->GetGameData()->GetStartLocation() );
      p_event->SetFadeCallBackDefault();
    }
    m_Seq = SEQ_END;
    break;
  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventRizaadonRide::EndFunc(GameSys::GameManager* p_gman)
{
}

//-----------------------------------------------------------------------------
/**
 * @func    OnTermination
 * @brief   TownMapEvent終了の通知
 * @param   warp_id  選ばれたワープID キャンセルの場合 App::TownMap::FLY_WARPID_CANCEL
 */
//-----------------------------------------------------------------------------
void EventRizaadonRide::OnTermination(s32 warp_id)
{
  m_WarpID = warp_id;
}

//--------------------------------------------------------------
/**
 * @brief 図鑑登録アプリコール
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventRizaadonRide::CallTownmap(GameSys::GameManager* p_gman)
{
  App::Event::TownMapEvent* p_event = App::Event::TownMapEvent::StartEvent( p_gman );
  p_event->SetParam( App::TownMap::MODE_FLY );
  p_event->SetTownMapEventListener( this );
}

} // namespace Field
