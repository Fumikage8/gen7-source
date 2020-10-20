/**
* @brief  デバッグ用カメラ位置調整イベント
* @file   FieldEventKairikyRock.cpp
* @author ikeuchi_yuya
* @data   2015.05.23
*/

#if !defined( __FIELDEVENTKAIRIKYROCK_H_INCLUDED__ )
#define __FIELDEVENTKAIRIKYROCK_H_INCLUDED__
#pragma once

// niji
#include "GameSys/include/GameEvent.h"

//
// 前方宣言
//
GFL_NAMESPACE_BEGIN( Field );
class FieldGimmickKairikyRock;
GFL_NAMESPACE_END( Field );

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Event );

class EventKairikyRock : public GameSys::GameEvent
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( EventKairikyRock );

public:

  /**
  * @brief コンストラクタ
  *
  * @param  pSystemHeap GameEventに渡すヒープ
  *
  * @return 無し
  */ 
  EventKairikyRock( gfl2::heap::HeapBase* pSystemHeap );
  
  /**
  * @brief デストラクタ
  *
  * @return 無し
  */ 
  virtual ~EventKairikyRock();

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

  //--------------------------------------------------------------
  /**
  * @brief   カイリキギミックを設定
  */
  //--------------------------------------------------------------
  void SetKairikyGimmick(Field::FieldGimmickKairikyRock* pGimmickKairikyRock){ m_pGimmickKairikyRock = pGimmickKairikyRock; }

private:

  Field::FieldGimmickKairikyRock* m_pGimmickKairikyRock;

}; // class EventKairikyRock

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );

#endif // __FIELDEVENTKAIRIKYROCK_H_INCLUDED__