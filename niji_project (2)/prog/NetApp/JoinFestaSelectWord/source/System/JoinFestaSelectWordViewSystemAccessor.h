// ============================================================================
/*
 * @file JoinFestaSelectWordViewSystemAccessor.h
 * @brief 挨拶選択アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTASELECTWORD_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaSelectWordViewList;
class JoinFestaSelectWordViewObjects;

class JoinFestaSelectWordViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordViewSystemAccessor );

public:

  JoinFestaSelectWordViewSystemAccessor(
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewList* pJoinFestaSelectWordViewList,
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewObjects* pJoinFestaSelectWordViewObjects );

  virtual~JoinFestaSelectWordViewSystemAccessor();

  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewList*  GetJoinFestaSelectWordViewList() { return m_pJoinFestaSelectWordViewList; }
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewObjects*  GetJoinFestaSelectWordViewObjects() { return m_pJoinFestaSelectWordViewObjects; }

private:

  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewList*  m_pJoinFestaSelectWordViewList;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewObjects*  m_pJoinFestaSelectWordViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTASELECTWORD_VIEWSYSTEMACCESSOR_H_INCLUDE
