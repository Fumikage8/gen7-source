#if PM_DEBUG
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugEventBattleInstTest.h
 *	@brief  バトル施設テスト
 *	@author	onoue masayuki
 *	@date		2016.03.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if !defined( __FIELDDEBUGEVENTBATTLEINSTTEST_H__ )
#define __FIELDDEBUGEVENTBATTLEINSTTEST_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameEvent.h"

GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
/**
 *  @class 自動全マップジャンプ
 */
class DebugEventBattleInstTest : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugEventBattleInstTest); //コピーコンストラクタ＋代入禁止

public:

  enum Mode
  {
    MODE_DLL_SIZE = 0,     // DLL サイズチェック
    MODE_MAX
  };

    /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:

    InitializeDescription( void ) :
       m_Mode( MODE_DLL_SIZE )
    {
    }

    ~InitializeDescription( void )
    {
    }

    Mode                          m_Mode;         // ジャンプ型式
  };



public:
  DebugEventBattleInstTest( gfl2::heap::HeapBase* heap );
  virtual ~DebugEventBattleInstTest();

  void SetDesc( const InitializeDescription& rDesc );


  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:
  //--------------------------------------------------------------
  /**
  * @brief   DLLサイズをチェック
  */
  //--------------------------------------------------------------
  void TestDllSize();

private:
  InitializeDescription m_InitDesc;
  gfl2::heap::HeapBase* m_pHeap;

};


GFL_NAMESPACE_END(Field);

#endif	// __FIELDDEBUGEVENTBATTLEINSTTEST_H__


#endif // PM_DEBUG