#if !defined(NIJI_PROJECT_CONFIG_EVENT_H_INCLUDED)
#define NIJI_PROJECT_CONFIG_EVENT_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    ConfigEvent.h
 @author  Yuto Uchida
 @date    2015.06.11
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>
#include "GameSys/include/GameEvent.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(config)
  class ConfigProc;
GFL_NAMESPACE_END(config)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)

  //==============================================================================
  /**
   @class     ConfigEvent
   */
  //==============================================================================
  class ConfigEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN(ConfigEvent);

  public:
    //! @brief  このイベントを起動する関数
    static void StartEvent(GameSys::GameManager* gmgr);

  public:
    ConfigEvent(gfl2::heap::HeapBase* heap);
    virtual ~ConfigEvent();

    virtual bool BootChk(GameSys::GameManager* gmgr);
    virtual void InitFunc(GameSys::GameManager* gmgr);
    virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
    virtual void EndFunc(GameSys::GameManager* gmgr);

  private:
    app::config::ConfigProc* m_proc;
  };

GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_CONFIG_EVENT_H_INCLUDED

