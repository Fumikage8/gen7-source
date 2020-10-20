//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventGameOver.h
 *  @brief  ゲームオーバーイベント
 *  @author tomoya takahashi
 *  @date   2012.04.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __EVENTGAMEOVER_H__
#define __EVENTGAMEOVER_H__

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameProc.h"


namespace Field
{

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

  // 通常戦闘負け　イベント
  class EventNormalLose : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN(EventNormalLose); // コピーコンストラクタ＋代入禁止
    
  public:
    // コンストラクタデストラクタ
    EventNormalLose( gfl2::heap::HeapBase* heap ) : 
      GameSys::GameEvent( heap ), m_Seq(0) {}

    virtual ~EventNormalLose(){}
      

    //--------------------------------------------------------------
    /**
     * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
     * @param   p_gman       ゲームマネージャークラス
     * @retval  true  起動してよい
     * @retval  false 起動しない
     */
    //--------------------------------------------------------------
    virtual bool BootChk(GameSys::GameManager* p_gman);

    //--------------------------------------------------------------
    /**
     * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
     * @param   p_gman       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void InitFunc(GameSys::GameManager* p_gman);

    //--------------------------------------------------------------
    /**
     * @brief   呼び出される関数 純粋仮想関数
     * @param   p_gman       ゲームマネージャークラス
     * @return  GMEVENT_RESULT  イベント制御関数の戻り値
     */
    //--------------------------------------------------------------
    virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

    //--------------------------------------------------------------
    /**
     * @brief   呼び出される関数 純粋仮想関数
     * @param   p_gman       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void EndFunc(GameSys::GameManager* p_gman);


  private:


    enum
    {
      SEQ_MESSAGE_PROC_CALL,
      SEQ_GAMEOVER_MAPCHANGE,
      SEQ_END,
    };
    
  private:

    s32 m_Seq;
  };


} // namespace Field
  
#endif  // __EVENTGAMEOVER_H__

