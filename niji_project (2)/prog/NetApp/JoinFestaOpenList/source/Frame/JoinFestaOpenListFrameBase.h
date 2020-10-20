// ============================================================================
/*
 * @file JoinFestaOpenListFrameBase.h
 * @brief 開催選択アプリのフレームの基底クラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_FRAMEBASE_H_INCLUDE )
#define JOINFESTAOPENLIST_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListWork.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewList.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaOpenListViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaOpenListFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListFrameBase );
public:
  JoinFestaOpenListFrameBase(
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork,
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor* pJoinFestaOpenListViewSystemAccessor );
  virtual~JoinFestaOpenListFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork*            m_pJoinFestaOpenListWork;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewList*        m_pJoinFestaOpenListViewList;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewObjects*     m_pJoinFestaOpenListViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLIST_FRAMEBASE_H_INCLUDE
