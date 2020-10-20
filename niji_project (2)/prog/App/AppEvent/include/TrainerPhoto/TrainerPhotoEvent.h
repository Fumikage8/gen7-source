#if !defined(TRAINER_PHOTO_EVENT_H_INCLUDED)
#define TRAINER_PHOTO_EVENT_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    TrainerPhotoEvent.h
 @author  Yuto Uchida
 @date    2015.09.01
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "App/TrainerPhoto/include/TrainerPhotoProc.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)

  //==============================================================================
  /**
   @class     TrainerPhotoEvent
   */
  //==============================================================================
  class TrainerPhotoEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN(TrainerPhotoEvent);

  public:
    //! @brief  このイベントを起動する関数
    static void StartEvent(GameSys::GameManager* gmgr);

  public:
    TrainerPhotoEvent(gfl2::heap::HeapBase* heap);
    virtual ~TrainerPhotoEvent();

    virtual bool BootChk(GameSys::GameManager* gmgr);
    virtual void InitFunc(GameSys::GameManager* gmgr);
    virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
    virtual void EndFunc(GameSys::GameManager* gmgr);

  private:
    app::photo::TrainerPhotoProc* m_proc;
    app::photo::APP_PARAM         m_appParam;
  };

GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)


#endif  // TRAINER_PHOTO_EVENT_H_INCLUDED

