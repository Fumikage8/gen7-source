//==============================================================================
/**
 * @file        GameOverEvent.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 15:23:06
 */
//==============================================================================

#if !defined(__GAMEOVEREVENT_H__) // 重複定義防止
#define __GAMEOVEREVENT_H__ // 重複定義防止マクロ
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>
#include "App/GameOver/include/GameOverProcParam.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)


//==============================================================================
/**
 @class     GameOverAppEvent
 @brief     図鑑の呼び出しEvent
 */
//==============================================================================
class GameOverAppEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(GameOverAppEvent);

public:
  //! @brief  このイベントを起動する関数
  static void StartEvent(GameSys::GameManager* gmgr);

public:
  GameOverAppEvent(gfl2::heap::HeapBase* heap);
  virtual ~GameOverAppEvent();

  virtual bool BootChk(GameSys::GameManager* gmgr);
  virtual void InitFunc(GameSys::GameManager* gmgr);
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:
  app::GameOver::GameOverProcParam  m_param;
  app::GameOver::GameOverProc       *m_proc;
};


GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)


#endif // __GAMEOVEREVENT_H__ 重複定義防止
