// ============================================================================
/*
 * @file MiracleTradeFrameBase.h
 * @brief ミラクル交換のフレーム基底クラスです。
 * @date 2015.06.03
 */
// ============================================================================
#if !defined( MIRACLETRADEFRAMEBASE_H_INCLUDE )
#define MIRACLETRADEFRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)
class NetApplicationViewBase;
GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)


class MiracleTradeFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeFrameBase );

public:

  MiracleTradeFrameBase( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork );
  virtual~MiracleTradeFrameBase();

protected:

  //! フレームを終了します(自身の処理を終わらせます)
  virtual void exitFrame( NetAppLib::System::FrameResult result );

  // -----------------------------------------------------------------------------
  // 上画面用
  //! 現在アクティブなビューにビューを追加します。その際入力を有効にします。
  void addUpperViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView );
  //! 現在アクティブなビューにビューを追加します。その際入力を無効にします。
  void addUpperViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView );
  //! 現在アクティブなビューから子ビューをリムーブします。
  void removeUpperView();

  // -----------------------------------------------------------------------------
  // 下画面用
  //! 現在アクティブなビューにビューを追加します。その際入力を有効にします。
  void addLowerViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView );
  //! 現在アクティブなビューにビューを追加します。その際入力を無効にします。
  void addLowerViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView );
  //! 現在アクティブなビューから子ビューをリムーブします。
  void removeLowerView();
  
private:

  NetAppLib::UI::NetAppEmptyView*                         m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*                         m_pRootLowerView;
  app::ui::UIView*                                        m_pActiveUpperView;
  app::ui::UIView*                                        m_pActiveLowerView;

};


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEFRAMEBASE_H_INCLUDE
