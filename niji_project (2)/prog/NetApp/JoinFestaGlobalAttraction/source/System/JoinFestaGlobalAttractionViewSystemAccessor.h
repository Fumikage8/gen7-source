// ============================================================================
/*
 * @file JoinFestaGlobalAttractionViewSystemAccessor.h
 * @brief グローバルアトラクションアプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTION_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTION_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaGlobalAttractionViewList;
class JoinFestaGlobalAttractionViewObjects;

class JoinFestaGlobalAttractionViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionViewSystemAccessor );

public:

  JoinFestaGlobalAttractionViewSystemAccessor(
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewList* pJoinFestaGlobalAttractionViewList,
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewObjects* pJoinFestaGlobalAttractionViewObjects );

  virtual~JoinFestaGlobalAttractionViewSystemAccessor();

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewList*  GetJoinFestaGlobalAttractionViewList() { return m_pJoinFestaGlobalAttractionViewList; }
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewObjects*  GetJoinFestaGlobalAttractionViewObjects() { return m_pJoinFestaGlobalAttractionViewObjects; }

private:

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewList*  m_pJoinFestaGlobalAttractionViewList;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewObjects*  m_pJoinFestaGlobalAttractionViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAGLOBALATTRACTION_VIEWSYSTEMACCESSOR_H_INCLUDE
