// ============================================================================
/*
 * @file JoinFestaAttractionListSelectFrameFactory.h
 * @brief アトラクション開催参加アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaAttractionListSelectViewSystemAccessor;


class JoinFestaAttractionListSelectFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectFrameFactory );
public:
  JoinFestaAttractionListSelectFrameFactory(
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor );
  virtual~JoinFestaAttractionListSelectFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork*                m_pJoinFestaAttractionListSelectWork;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor*  m_pJoinFestaAttractionListSelectViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECT_FRAMEFACTORY_H_INCLUDE
