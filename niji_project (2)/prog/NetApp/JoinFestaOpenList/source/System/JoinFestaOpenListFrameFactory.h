// ============================================================================
/*
 * @file JoinFestaOpenListFrameFactory.h
 * @brief 開催選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTAOPENLIST_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaOpenListViewSystemAccessor;


class JoinFestaOpenListFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListFrameFactory );
public:
  JoinFestaOpenListFrameFactory(
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork,
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor* pJoinFestaOpenListViewSystemAccessor );
  virtual~JoinFestaOpenListFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork*                m_pJoinFestaOpenListWork;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor*  m_pJoinFestaOpenListViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLIST_FRAMEFACTORY_H_INCLUDE
