// ============================================================================
/*
 * @file JoinFestaSelectWordUpperView.h
 * @brief 挨拶選択アプリの上画面を表示するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDUPPERVIEW_H_INCLUDE )
#define JOINFESTASELECTWORDUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSelectWordUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordUpperView );
public:
  JoinFestaSelectWordUpperView( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork );
  virtual~JoinFestaSelectWordUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetCategory( u32 categoryMessageID, u32 selectMessageID );
  void SetMessage( u32 categoryMessageID, u32 selectMessageID );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JoinFestaSelectWord,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

  void setTextMessage( u32 windowMessageID, u32 categoryMessageID, u32 selectMessageID );

private:

  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*   m_pJoinFestaSelectWordWork;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDUPPERVIEW_H_INCLUDE
