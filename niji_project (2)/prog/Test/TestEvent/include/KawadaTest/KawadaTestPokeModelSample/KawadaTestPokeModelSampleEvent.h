#if !defined(NIJI_PROJECT_TEST_TESTEVENT_KAWADATESTPOKEMODELSAMPLEEVENT_H_INCLUDED)
#define NIJI_PROJECT_TEST_TESTEVENT_KAWADATESTPOKEMODELSAMPLEEVENT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestPokeModelSampleEvent.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.15
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


// 前方宣言
class KawadaTestProc;


//==============================================================================
/**
 @class     KawadaTestPokeModelSampleEvent
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestPokeModelSampleEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestPokeModelSampleEvent);

public:
  //! @brief  このイベントを起動する関数
  static void StartEvent(GameSys::GameManager* gmgr);

public:
  KawadaTestPokeModelSampleEvent(gfl2::heap::HeapBase* heap);
  virtual ~KawadaTestPokeModelSampleEvent();

  virtual bool BootChk(GameSys::GameManager* gmgr);
  virtual void InitFunc(GameSys::GameManager* gmgr);
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:
  KawadaTestProc* m_proc;
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_TESTEVENT_KAWADATESTPOKEMODELSAMPLEEVENT_H_INCLUDED

