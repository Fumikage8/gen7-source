// ============================================================================
/*
 * @file JoinFestaPlayerListSelectFrameBase.h
 * @brief プレイヤー単体選択アプリのフレームの基底クラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_FRAMEBASE_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectWork.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectViewList.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaPlayerListSelectViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerListSelectFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectFrameBase );
public:
  JoinFestaPlayerListSelectFrameBase(
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork,
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor* pJoinFestaPlayerListSelectViewSystemAccessor );
  virtual~JoinFestaPlayerListSelectFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork*            m_pJoinFestaPlayerListSelectWork;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewList*        m_pJoinFestaPlayerListSelectViewList;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewObjects*     m_pJoinFestaPlayerListSelectViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECT_FRAMEBASE_H_INCLUDE
