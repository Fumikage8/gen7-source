// ============================================================================
/*
 * @file JoinFestaPlayerDetailFrameBase.h
 * @brief 詳細アプリのフレームの基底クラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_FRAMEBASE_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailWork.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailViewList.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaPlayerDetailViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerDetailFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailFrameBase );
public:
  JoinFestaPlayerDetailFrameBase(
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork,
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor* pJoinFestaPlayerDetailViewSystemAccessor );
  virtual~JoinFestaPlayerDetailFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork*            m_pJoinFestaPlayerDetailWork;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewList*        m_pJoinFestaPlayerDetailViewList;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewObjects*     m_pJoinFestaPlayerDetailViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERDETAIL_FRAMEBASE_H_INCLUDE
