// ============================================================================
/*
 * @file NBRLowerViewReturnButton.h
 * @brief 通信対戦受付アプリ 下画面：戻るボタンのみ
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_RETURNBUTTON_H_INCLUDE )
#define NBR_LOWER_VIEW_RETURNBUTTON_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Ui/UIInputListener.h"

// ビューベース 
#include "NetApp/NBR/source/View/NBRLowerViewBase.h"
// リスナー
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButtonListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


class NBRLowerViewReturnButton : public NBRLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRLowerViewReturnButton );
public:
  // コンストラクタ
  NBRLowerViewReturnButton( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                       app::util::G2DUtil* pG2dUtil,
                       app::util::G2DUtil::LAYOUT_WORK* pLayoutWork );
  // デストラクタ
  virtual~NBRLowerViewReturnButton();

  // イベントリスナー
  void SetListener( NBRLowerViewReturnButtonListener* pNBRLowerViewReturnButtonListener );
  void RemoveListener();

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

public:

  //--------------------------------------------------------------------------
  /**
  * @breif  ビュー全体の表示・非表示
  * @param  isVisible         true = 表示, false = 非表示
  */
  //--------------------------------------------------------------------------
  void SetAllVisible( const bool isVisible );

private:

  //-------------------------------------
  /// ボタンID定義
  //=====================================
  enum ButtonId
  {
    BUTTON_ID_RETURN,    ///< 戻るボタン
    BUTTON_MAX,
  };

  void setupButton();

private:

  NBRLowerViewReturnButtonListener*    m_pListener;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_LOWER_VIEW_RETURNBUTTON_H_INCLUDE
