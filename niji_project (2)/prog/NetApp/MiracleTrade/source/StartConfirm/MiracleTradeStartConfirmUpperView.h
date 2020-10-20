// ============================================================================
/*
 * @file MiracleTradeStartConfirmUpperView.h
 * @brief ミラクル交換の開始確認を行う画面を表示するクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADESTARTCONFIRMUPPERVIEW_H_INCLUDE )
#define MIRACLETRADESTARTCONFIRMUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(StartConfirm)


class MiracleTradeStartConfirmUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeStartConfirmUpperView );
public:
  MiracleTradeStartConfirmUpperView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork );
  virtual~MiracleTradeStartConfirmUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_MIRACLE_TRADE,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

  void setupTextMessage();

private:

  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*     m_pMiracleTradeWork;

};


GFL_NAMESPACE_END(StartConfirm)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADESTARTCONFIRMUPPERVIEW_H_INCLUDE
