// ============================================================================
/*
 * @file JoinFestaSubScreenParentView.h
 * @brief ジョインフェスタ下画面のViewクラスを管理するクラスです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_VIEWMANAGER_H_INCLUDE )
#define JOINFESTASUBSCREEN_VIEWMANAGER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gfx/include/gfl2_types.h>   // for displayNo 
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)
class JoinFestaSubScreenWork;
GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)

// SubViewの前方宣言
class JoinFestaSubScreenBgLowerView;
class JoinFestaSubScreenMainLowerView;
class JoinFestaSubScreenSelectLowerView;
class JoinFestaSubScreenInvitationLowerView;
class JoinFestaSubScreenAttractionLowerView;
class JoinFestaSubScreenAlertLowerView;


/**
 * @brief ジョインフェスタ下画面親View
 */
class JoinFestaSubScreenParentView
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenParentView );
public:
  JoinFestaSubScreenParentView( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork );
  virtual~JoinFestaSubScreenParentView();

  void Initialize( NetAppLib::UI::NetAppMessageMenu* pNetAppMessageMenu );
  void Update();
  void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  /**
   * @brief Viewを破棄出来るかどうか
   *
   * @retval true：破棄出来る
   * @retval false：破棄出来ない
   */
  bool IsExitView();

  JoinFestaSubScreenBgLowerView*          GetBgLowerView()    { return m_pBgLowerView; }
  JoinFestaSubScreenMainLowerView*        GetMainLowerView()  { return m_pMainLowerView; }
  JoinFestaSubScreenSelectLowerView*      GetSelectLowerView(){ return m_pSelectLowerView; }
  JoinFestaSubScreenInvitationLowerView*  GetInvitationLowerView(){ return m_pInvitationLowerView; }
  JoinFestaSubScreenAttractionLowerView*  GetAttractionLowerView(){ return m_pAttractionLowerView; }
  JoinFestaSubScreenAlertLowerView*       GetAlertLowerView(){ return m_pAlertLowerView; }

  //! 全てのViewを非表示
  void DisableAllView();

  //! 一時停止
  void Suspend();
  //! 再起動
  void Resume();

private:
  JoinFestaSubScreenBgLowerView*          m_pBgLowerView;
  JoinFestaSubScreenMainLowerView*        m_pMainLowerView;
  JoinFestaSubScreenSelectLowerView*      m_pSelectLowerView;
  JoinFestaSubScreenInvitationLowerView*  m_pInvitationLowerView;
  JoinFestaSubScreenAttractionLowerView*  m_pAttractionLowerView;
  JoinFestaSubScreenAlertLowerView*       m_pAlertLowerView;


  ApplicationSystem::JoinFestaSubScreenWork*  m_pJoinFestaSubScreenWork;

};

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTASUBSCREEN_VIEWMANAGER_H_INCLUDE
