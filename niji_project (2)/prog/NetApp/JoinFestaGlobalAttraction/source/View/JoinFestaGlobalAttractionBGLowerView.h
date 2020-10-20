// ============================================================================
/*
 * @file JoinFestaGlobalAttractionBGLowerView.h
 * @brief グローバルアトラクションアプリの下画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONBGLOWERVIEW_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONBGLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionBGLowerView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionBGLowerView );
public:
  JoinFestaGlobalAttractionBGLowerView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork );
  virtual~JoinFestaGlobalAttractionBGLowerView();


  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );



private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAGLOBALATTRACTION,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };


private:

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*                         m_pJoinFestaGlobalAttractionWork;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONBGLOWERVIEW_H_INCLUDE
