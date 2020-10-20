// ============================================================================
/*
 * @file JoinFestaSelectWordBGUpperView.h
 * @brief 挨拶選択アプリの上画面を表示するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDBGUPPERVIEW_H_INCLUDE )
#define JOINFESTASELECTWORDBGUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSelectWordBGUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordBGUpperView );
public:
  JoinFestaSelectWordBGUpperView( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork );
  virtual~JoinFestaSelectWordBGUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JoinFestaSelectWord,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:

  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*                         m_pJoinFestaSelectWordWork;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDBGUPPERVIEW_H_INCLUDE
