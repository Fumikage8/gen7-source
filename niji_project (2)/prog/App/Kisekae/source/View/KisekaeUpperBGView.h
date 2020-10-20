// ============================================================================
/*
 * @file KisekaeUpperBGView.h
 * @brief きせかえアプリの下画面を表示するクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_UPPER_BG_VIEW_H_INCLUDE )
#define KISEKAE_UPPER_BG_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

#include "App/Kisekae/source/System/KisekaeWork.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(View)

class UpperBGView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperBGView );
public:
  enum MoneyMode
  {
    MONEY_MODE_TYPE_GOLD  = 0,
    MONEY_MODE_TYPE_FC,
  };

  UpperBGView( App::Kisekae::System::Work* pConnectionWork );
  virtual~UpperBGView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //  金額の単位モード設定
  void SetGoldMode( const MoneyMode mode );

  //  所持金表示
  void ShowMoneyWindow( const u32 initMoneyNum );
  //  所持金値を設定
  void SetMoneyNum( const u32 moneyNum );

  //  所持金非表示
  void HideMoneyWindow();

  //  アイテム関連の名前表示
  void SetItemNameViaible( const bool bFlg );

  //  アイテム関連の名前設定
  void SetItemName( const s32 itemTxtId, const s32 colorTxtId );

  //  ユーザー操作説明アイコン表示設定
  void SetVisibleUserInputTutorialIcon( const bool bFlg );

private:
  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

  void _Clear()
  {
    m_pWork                       = NULL;
    m_pMoneyWindowPane            = NULL;
    m_pMoenyWindowNumTextPane     = NULL;
    m_pUserInputTutorialIconPane  = NULL;
    m_pItemNameFramePane          = NULL;
    m_pItemNameTextPane           = NULL;
    m_pItemColorNameTextPane      = NULL;

    m_moneyMode = MONEY_MODE_TYPE_GOLD;
  }

  //  所持金をテキスト表示設定
  void _SetMoneyText( const u32 moneyNum );

  App::Kisekae::System::Work*     m_pWork;

  MoneyMode m_moneyMode;

  gfl2::lyt::LytPane*             m_pUserInputTutorialIconPane;
  gfl2::lyt::LytPane*             m_pMoneyWindowPane;
  gfl2::lyt::LytTextBox*          m_pMoenyWindowNumTextPane;

  gfl2::lyt::LytPane*             m_pItemNameFramePane;
  gfl2::lyt::LytTextBox*          m_pItemNameTextPane;
  gfl2::lyt::LytTextBox*          m_pItemColorNameTextPane;
};

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_UPPER_BG_VIEW_H_INCLUDE
