// ============================================================================
/*
 * @file JoinFestaSelectWordBGLowerView.h
 * @brief 挨拶選択アプリの下画面を表示するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDBGLOWERVIEW_H_INCLUDE )
#define JOINFESTASELECTWORDBGLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSelectWordBGLowerView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordBGLowerView );
public:
  JoinFestaSelectWordBGLowerView( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork );
  virtual~JoinFestaSelectWordBGLowerView();


  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );



private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTASELECTWORD,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };


private:

  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*                         m_pJoinFestaSelectWordWork;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDBGLOWERVIEW_H_INCLUDE
