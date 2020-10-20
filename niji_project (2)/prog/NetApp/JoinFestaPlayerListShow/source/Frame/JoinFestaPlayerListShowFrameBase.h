// ============================================================================
/*
 * @file JoinFestaPlayerListShowFrameBase.h
 * @brief 閲覧アプリのフレームの基底クラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_FRAMEBASE_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowWork.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowViewList.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaPlayerListShowViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerListShowFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowFrameBase );
public:
  JoinFestaPlayerListShowFrameBase(
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor );
  virtual~JoinFestaPlayerListShowFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork*            m_pJoinFestaPlayerListShowWork;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewList*        m_pJoinFestaPlayerListShowViewList;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewObjects*     m_pJoinFestaPlayerListShowViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOW_FRAMEBASE_H_INCLUDE
