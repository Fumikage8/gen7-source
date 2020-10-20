/**
* @brief  デバッグ用カメラ位置調整イベント
* @file   FieldEventKairikyRock.cpp
* @author ikeuchi_yuya
* @data   2015.05.23
*/

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"

// field
#include "Field/FieldRo/include/Event/FieldEventKairikyRock.h"
#include "Field/FieldRo/include/Gimmick/FieldGimmickKairikyRock.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Event );

/**
* @brief コンストラクタ
*
* @param  pSystemHeap GameEventに渡すヒープ
*
* @return 無し
*/ 
EventKairikyRock::EventKairikyRock( gfl2::heap::HeapBase* pSystemHeap ) 
: GameSys::GameEvent( pSystemHeap )
, m_pGimmickKairikyRock( NULL )
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/ 
EventKairikyRock::~EventKairikyRock()
{
}

//--------------------------------------------------------------
/**
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @retval  true  起動してよい
* @retval  false 起動しない
*/
//--------------------------------------------------------------
bool EventKairikyRock::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventKairikyRock::InitFunc(GameSys::GameManager* gmgr)
{
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventKairikyRock::MainFunc(GameSys::GameManager* gmgr)
{
  if( m_pGimmickKairikyRock)
  {
    if( m_pGimmickKairikyRock->ExecuteKairiky() )
    {
      return GameSys::GMEVENT_RES_FINISH;
    }
    return GameSys::GMEVENT_RES_CONTINUE;
  }
  return GameSys::GMEVENT_RES_FINISH;
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventKairikyRock::EndFunc(GameSys::GameManager* gmgr)
{
  m_pGimmickKairikyRock = NULL;
}

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );