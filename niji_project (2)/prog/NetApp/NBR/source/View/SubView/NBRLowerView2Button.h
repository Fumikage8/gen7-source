// ============================================================================
/*
 * @file NBRLowerView2Button.h
 * @brief 通信対戦受付アプリ 下画面：ルール誰が選ぶか画面
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_2BUTTON_H_INCLUDE )
#define NBR_LOWER_VIEW_2BUTTON_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Ui/UIInputListener.h"

// ビューベース 
#include "NetApp/NBR/source/View/NBRLowerViewBase.h"
// リスナー
#include "NetApp/NBR/source/View/SubView/NBRLowerView2ButtonListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


class NBRLowerView2Button : public NBRLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRLowerView2Button );
public:

  /**
   * @enum ButtonTextMode
   * ボタンテキストモード
   */
  enum ButtonTextMode{
    BUTTON_TEXT_MODE_WHO_DECIDE,  //! 「自分が決める/他の人に決める」のボタン
    BUTTON_TEXT_MODE_RULE,        //! 「同じルールで遊ぶ/ルールを変えて遊ぶ」のボタン
    BUTTON_TEXT_MODE_POKE_UNKNOWN,//! 「選びなおす/確認する」のボタン
  };

  /**
   * @enum ButtonId
   * ボタンID定義
   */
  enum ButtonId
  {
    BUTTON_ID_MYSELF,    //! 自分ルール選ぶボタン
    BUTTON_ID_OTHERS,    //! 他人ルール選ぶボタン
    BUTTON_ID_RETURN,    //! 戻るボタン
    BUTTON_MAX,

    BUTTON_ID_CONTINUE_SAME_RULE = BUTTON_ID_MYSELF, //! 同じルールで遊ぶ
    BUTTON_ID_CONTINUE_DIFF_RULE = BUTTON_ID_OTHERS, //! 違うルールで遊ぶ

    BUTTON_ID_RESELECT = BUTTON_ID_MYSELF, //! 選びなおす
    BUTTON_ID_CONFIRM  = BUTTON_ID_OTHERS, //! 確認する
  };

  /**
   * @enum VisibleMode
   * 表示モード定義
   */
  enum VisibleMode
  {
    ALL_VIEW_ON,            //! 全て表示
    ALL_VIEW_OFF,           //! 全て非表示
    RETURN_BUTTON_ONLY_ON,  //! 戻るボタンのみ表示
    TWO_BUTTON_ONLY_ON,     //! 2ボタンのみ非表示
    CURSOR_OFF,             //! カーソル非表示
    CURSOR_ON,              //! カーソル表示
  };


public:
  // コンストラクタ
  NBRLowerView2Button( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                       app::util::G2DUtil* pG2dUtil,
                       app::util::G2DUtil::LAYOUT_WORK* pLayoutWork );
  // デストラクタ
  virtual~NBRLowerView2Button();

  // イベントリスナー
  void SetListener( NBRLowerView2ButtonListener* pNBRLowerView2ButtonListener );
  void RemoveListener();

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

public:

  //--------------------------------------------------------------------------
  /**
  * @breif  ビュー全体の表示・非表示
  * @param  visibleMode   表示モード定義
  */
  //--------------------------------------------------------------------------
  void SetVisible( const VisibleMode visibleMode );

  //--------------------------------------------------------------------------
  /**
  * @breif  ボタンのテキスト設定
  * @param  mode  ButtonTextMode
  */
  //--------------------------------------------------------------------------
  void SetButtonText( ButtonTextMode mode );

private:

  void setupButton();

private:

  NBRLowerView2ButtonListener*    m_pListener;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_LOWER_VIEW_2BUTTON_H_INCLUDE
