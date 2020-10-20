#if !defined(NIJI_PROJECT_APP_APPEVENT_ZUKAN_ZUKANEVENT_H_INCLUDED)
#define NIJI_PROJECT_APP_APPEVENT_ZUKAN_ZUKANEVENT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanEvent.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑の呼び出しEvent
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanProc;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)


//==============================================================================
/**
 @class     ZukanEvent
 @brief     図鑑の呼び出しEvent
 */
//==============================================================================
class ZukanEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanEvent);

public:
  //! @brief  このイベントを起動する関数
  static void StartEvent(GameSys::GameManager* gmgr);

public:
  ZukanEvent(gfl2::heap::HeapBase* heap);
  virtual ~ZukanEvent();

  virtual bool BootChk(GameSys::GameManager* gmgr);
  virtual void InitFunc(GameSys::GameManager* gmgr);
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:
  app::zukan::ZukanProcParam m_procParam;
  app::zukan::ZukanProc*     m_proc;
};


GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_APPEVENT_ZUKAN_ZUKANEVENT_H_INCLUDED

