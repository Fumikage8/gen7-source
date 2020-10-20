#if !defined(NIJI_PROJECT_CAPTURE_TUTORIAL_EVENT_H_INCLUDED)
#define NIJI_PROJECT_CAPTURE_TUTORIAL_EVENT_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    CaptureTutorialEvent.h
 @author  Yuto Uchida
 @date    2015.06.24
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


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)
  class CaptureTutorialProc;
GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)

  //==============================================================================
  /**
   @class     CaptureTutorialEvent
   */
  //==============================================================================
  class CaptureTutorialEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN(CaptureTutorialEvent);

  public:
    //! @brief  このイベントを起動する関数
    // @param	isCapture  true:捕獲チュートリアル false:Z技チュートリアル
    static void StartEvent(GameSys::GameManager* gmgr, b32 isCapture = true);

  public:
    CaptureTutorialEvent(gfl2::heap::HeapBase* heap);
    virtual ~CaptureTutorialEvent();

    virtual bool BootChk(GameSys::GameManager* gmgr);
    virtual void InitFunc(GameSys::GameManager* gmgr);
    virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
    virtual void EndFunc(GameSys::GameManager* gmgr);

    // @brief 捕獲チュートリアルとZ技チュートリアルの切り分け
	// @param	isCapture  true:捕獲チュートリアル false:Z技チュートリアル
    void SetCapture( b32 isCapture );

  private:
    app::capture::CaptureTutorialProc*  m_pProc;
    b32 mIsCapture;
  };

GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_CAPTURE_TUTORIAL_EVENT_H_INCLUDED

