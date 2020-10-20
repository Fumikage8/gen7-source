// ============================================================================
/*
 * @file JoinFestaGlobalAttractionBGUpperView.h
 * @brief グローバルアトラクションアプリの上画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONBGUPPERVIEW_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONBGUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionBGUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionBGUpperView );
public:
  JoinFestaGlobalAttractionBGUpperView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork );
  virtual~JoinFestaGlobalAttractionBGUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

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

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONBGUPPERVIEW_H_INCLUDE
