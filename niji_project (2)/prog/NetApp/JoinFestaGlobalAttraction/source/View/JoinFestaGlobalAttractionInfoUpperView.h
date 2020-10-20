// ============================================================================
/*
 * @file JoinFestaGlobalAttractionInfoUpperView.h
 * @brief グローバルアトラクションアプリの上画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONINFOUPPERVIEW_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONINFOUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionInfoUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionInfoUpperView );
public:
  JoinFestaGlobalAttractionInfoUpperView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork );
  virtual~JoinFestaGlobalAttractionInfoUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetGlobalAttractionInfo( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo );
  void UpdateGlobalAttractionInfo();

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JoinFestaGlobalAttraction,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*                         m_pJoinFestaGlobalAttractionWork;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo*   m_pActiveGlobalAttractionInfo;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONINFOUPPERVIEW_H_INCLUDE
