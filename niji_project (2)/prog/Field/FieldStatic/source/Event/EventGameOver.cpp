//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventGameOver.cpp
 *  @brief  ゲームオーバーイベント
 *  @author tomoya takahashi
 *  @date   2012.04.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "Field/FieldStatic/include/Event/EventGameOver.h"
#include "Field/FieldStatic/include/Event/EventProcessCall.h"
#include "Field/FieldStatic/include/FieldWarpData.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "FieldScript/include/EventScriptCall.h" // for CallScript
#include "FieldScript/include/FieldScriptSystem.h"

#include "App/GameOver/include/GameOverProc.h"
#include "App/AppEvent/include/GameOver/GameOverEvent.h"

#include "niji_conv_header/field/script/inc/pokecen.inc"
#include "niji_conv_header/field/warpdata/FieldWarpDataType.h"


namespace Field{
  

//-----------------------------------------------------------------------------
/**
 *    イベントの実装
 */
//-----------------------------------------------------------------------------


  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  bool EventNormalLose::BootChk(GameSys::GameManager* /*p_gman*/)
  {
    return true;
  } 

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  void EventNormalLose::InitFunc(GameSys::GameManager* /*p_gman*/)
  {
  } 

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  GameSys::GMEVENT_RESULT EventNormalLose::MainFunc(GameSys::GameManager* p_gman)
  {
    switch( m_Seq ){
    // メッセージプロセスコール
    case SEQ_MESSAGE_PROC_CALL:
      {    
        /// ゲームオーバー画面イベントの生成.
        app::event::GameOverAppEvent::StartEvent( p_gman );
        ++m_Seq;
      }
      break;
    // マップチェンジコール
    case SEQ_GAMEOVER_MAPCHANGE:
      {
        WarpDataTools         *pWarpData = p_gman->GetGameData()->GetWarpDataTools();
        GameSys::GameData     *p_gdata = p_gman->GetGameData();

        int warp_id = p_gdata->GetWarpID()-1;
        WarpDataTools::RecoverType type = pWarpData->GetRecoverType(warp_id);

        // リザーブセット
        switch( type )
        {
        case WarpDataTools::RECOVER_TYPE_POKECEN:
          p_gman->GetFieldScriptSystem()->SetReserveScript( SCRID_POKECEN_RESERVE_GAMEOVER );
          break;
        case WarpDataTools::RECOVER_TYPE_RESCUE:
          p_gman->GetFieldScriptSystem()->SetReserveScript( SCRID_MYHOME_RESERVE_GAMEOVER );
          break;
        case WarpDataTools::RECOVER_TYPE_MYHOME:
        default:
          p_gman->GetFieldScriptSystem()->SetReserveScript( SCRID_MYHOME_RESERVE_GAMEOVER );
          break;
        }

        //NMCat[2548] ポニの大峡谷の試練で全滅すると、リザードンフライトが使えなくなる
        //- 全滅処理で一律リザードンフライトを使えるように戻す事になった
        p_gdata->GetEventWork()->ResetEventFlag( SYS_FLAG_CAN_NOT_FLY );

        //選んだ座標にワープ
        EventMapJump::CallMapJumpPosWithoutField( p_gman,
          warp_id, EventMapJump::WARP_TYPE_RECOVER,EventMapJumpWithoutField::EV_MAPJUMP_NO_EFFECT );
        ++m_Seq;
      }
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
  void EventNormalLose::EndFunc(GameSys::GameManager* p_gman)
  {
  }




} // namespace Field
