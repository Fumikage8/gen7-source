// ============================================================================
/*
 * @file GTSFrameBase.h
 * @brief GTSのフレーム基底クラスです。
 * @date 2015.05.28
 */
// ============================================================================
#if !defined( GTSFRAMEBASE_H_INCLUDE )
#define GTSFRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


class GTSFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSFrameBase );

public:

  GTSFrameBase( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSFrameBase();

protected:

  // -----------------------------------------------------------------------------
  // 上画面用
  //! 現在アクティブなビューにビューを追加します。その際入力を有効にします。
  void addUpperViewInputEnable( app::ui::UIView* pView );
  //! 現在アクティブなビューにビューを追加します。その際入力を無効にします。
  void addUpperViewInputDisable( app::ui::UIView* pView );
  //! 現在アクティブなビューから子ビューをリムーブします。
  void removeUpperView();

  // -----------------------------------------------------------------------------
  // 下画面用
  //! 現在アクティブなビューにビューを追加します。その際入力を有効にします。
  void addLowerViewInputEnable( app::ui::UIView* pView );
  //! 現在アクティブなビューにビューを追加します。その際入力を無効にします。
  void addLowerViewInputDisable( app::ui::UIView* pView );
  //! 現在アクティブなビューから子ビューをリムーブします。
  void removeLowerView();
  
  void removeAllView();

private:

  NetAppLib::UI::NetAppEmptyView*                         m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*                         m_pRootLowerView;
  app::ui::UIView*                                        m_pActiveUpperView;
  app::ui::UIView*                                        m_pActiveLowerView;

};


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSFRAMEBASE_H_INCLUDE
