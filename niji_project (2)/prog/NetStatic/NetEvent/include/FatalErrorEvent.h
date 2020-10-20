// ============================================================================
/*
 * @file FatalErrorEvent.h
 * @brief フェイタルエラーアプリのイベント
 * @date 2015.12.25
 * @author endo_akira
 */
// ============================================================================
#if !defined( FATALERROREVENT_H_INCLUDE )
#define FATALERROREVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(FatalError)


enum FatalErrorNo
{
  FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED,      // SDカードが認識できない
  FATAL_ERROR_NO_NET_ERROR                    // 通信エラー
};

typedef struct
{
  FatalErrorNo    eFatalErrorNo;
} EventParam;


class FatalErrorEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( FatalErrorEvent );

public:

  static void StartEvent( GameSys::GameManager* pGameManager, const EventParam& eventParam );

  FatalErrorEvent( gfl2::heap::HeapBase* pHeap );
  virtual~FatalErrorEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetEventParam( const EventParam& eventParam ){ m_EventParam = eventParam; }

private:

  EventParam      m_EventParam;

};


GFL_NAMESPACE_END(FatalError)
GFL_NAMESPACE_END(NetEvent)

#endif // FATALERROREVENT_H_INCLUDE
