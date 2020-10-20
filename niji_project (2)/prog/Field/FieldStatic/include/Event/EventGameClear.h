//=============================================================================
/**
 *	@file	  EventGameClear.h
 *	@brief  イベント：ゲームクリアの流れ
 *	@author Miyuki Iwasawa	
 *	@date	  2016.01.21
 */
//=============================================================================
#if !defined(  __EVENTGAMECLEAR_H__ )
#define __EVENTGAMECLEAR_H__
#pragma once

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

namespace Field{

//-----------------------------------------------------------------------------
/*
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *	@class  EventGameClear
*/
//-----------------------------------------------------------------------------
class EventGameClear : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventGameClear ); // コピーを禁止

  public:

  enum GameClearSaveType{ 
    GAME_CLEAR_SAVE_TYPE_DENDOUIRI_1ST,   //初回殿堂入りタイミングでの更新
    GAME_CLEAR_SAVE_TYPE_ENDING,          //エンディング時の更新(nijiでは初回クリアの1回しか通らない)
    GAME_CLEAR_SAVE_TYPE_DENDOUIRI_BOUEI, //2回目以降の殿堂入りでの更新
  };

  //--------------------------------------------------------------
  /**
   * @brief ゲームクリアイベントに渡す引数
   */
  //--------------------------------------------------------------
  struct EventParam 
  {
    bool isAlreadyClear;  ///<クリア済みかどうか
  };

  //--------------------------------------------------------------
  /**
   * @brief ゲームクリアイベントを生成する
   * @param[in]   p_gman
   * @return  ゲームクリアイベント
   */
  //--------------------------------------------------------------
  static EventGameClear * Create( GameSys::GameManager * p_gman, bool isAlreadyClear );

  //--------------------------------------------------------------
  /**
   * @brief ゲームクリア時のセーブデータ更新を実行する
   * @param[in]   p_gman
   */
  //--------------------------------------------------------------
  static void SetGameClearStatus( GameSys::GameManager * p_gman, gfl2::heap::HeapBase * pTempHeap, GameClearSaveType type );

  //--------------------------------------------------------------
  /**
   * @brief ゲームクリアイベントにパラメータを設定する
   * @param[in]   evParam
   *
   * @note 値を戻す必要がないので、evParamはテンポラリでよい。
   */
  //--------------------------------------------------------------
  void SetParam( const EventParam & evParam );

  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap       workをとるためのメモリクラス
   */
  //--------------------------------------------------------------
  EventGameClear( gfl2::heap::HeapBase* heap);
  virtual ~EventGameClear();

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager* gmgr );

  private:
  
  /// 各操作シーケンスの定義
  enum Sequence
  {
    FIELD_CLOSE,
    SAVE_START,
    SAVE_WAIT,
    SCENE_PART_ENDLOOP_START,
    SCENE_PART_ENDLOOP_WAIT,
    END,
    SOFT_RESET,
  };

  const Sequence * mSeqTable;

  u32 mSeq;

  EventParam mEventParam;

  const Sequence * DecideSequenceTable( GameSys::GameManager* gmar );
  void SetNextSeq( void );

};

} // namespace Field 

#endif // __EVENTGAMECLEAR_H__

