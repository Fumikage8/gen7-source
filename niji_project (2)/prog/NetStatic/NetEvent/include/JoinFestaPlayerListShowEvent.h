// ============================================================================
/*
 * @file JoinFestaPlayerListShowEvent.h
 * @brief 閲覧アプリのイベント
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOWEVENT_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOWEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaPlayerListShow/include/JoinFestaPlayerListShowAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

#include "./PhotoViewEvent.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
class JoinFestaPlayerListShowProc;
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)


class JoinFestaPlayerListShowEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, const bool bApplyPhotoFlag = false );

  JoinFestaPlayerListShowEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaPlayerListShowEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void  SetApplyPhotoFlag(const bool bApplyPhotoFlag);

private:
  void  ApplyPhotoFlagToOwnerList(void);

  NetApp::JoinFestaPlayerListShow::APP_PARAM                      m_AppParam;
  NetApp::JoinFestaPlayerListShow::JoinFestaPlayerListShowProc*   m_pJoinFestaPlayerListShowProc;

  PhotoView::PhotoViewEvent::Param                                m_PhotoViewEventParam;
};


GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAPLAYERLISTSHOWEVENT_H_INCLUDE
