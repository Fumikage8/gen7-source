// ============================================================================
/*
 * @file JoinFestaAttractionListSelectFrameBase.h
 * @brief アトラクション開催参加アプリのフレームの基底クラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_FRAMEBASE_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectWork.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectViewList.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaAttractionListSelectViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaAttractionListSelectFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectFrameBase );
public:
  JoinFestaAttractionListSelectFrameBase(
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor );
  virtual~JoinFestaAttractionListSelectFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork*            m_pJoinFestaAttractionListSelectWork;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewList*        m_pJoinFestaAttractionListSelectViewList;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewObjects*     m_pJoinFestaAttractionListSelectViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECT_FRAMEBASE_H_INCLUDE
