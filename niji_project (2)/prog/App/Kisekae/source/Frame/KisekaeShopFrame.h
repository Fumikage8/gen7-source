// ============================================================================
/*
 * @file ShopFrame.h
 * @brief きせかえアプリの購入フレーム
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_APP_SHOP_FRAME_H_INCLUDE )
#define KISEKAE_APP_SHOP_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  netlibのインクルード
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"

#include "App/Kisekae/source/System/KisekaeWork.h"
#include "App/Kisekae/source/View/KisekaeLowerItemSelectView.h"

#include "App/Kisekae/source/Tool/CharaModelViewer.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(Frame)

  class ShopFrame : 
    public NetAppLib::System::NetAppFrameBase,
    public App::Tool::MessageMenuView::IEventHandler,
    public App::Kisekae::View::LowerItemSelectViewDelegate,
    public app::tool::CharaModelViewerDelegate
  {
    GFL_FORBID_COPY_AND_ASSIGN( ShopFrame );
  public:
    ShopFrame(
      App::Kisekae::System::Work* pWork );
    virtual~ShopFrame();

  private:

    // NetAppFrameBase
    virtual bool startup();
    virtual bool cleanup();
    virtual void setListener();
    virtual void removeListener();
    virtual void updateSequence();

    virtual void  MessageMenuView_OnEvent(const EventCode event);

    //  きせかえ終了通知
    virtual void CharaModelViewer_OnDressupEnd();

    //  選択項目にカーソルが設定された時に呼ばれる
    virtual void LowerItemSelectViewDelegate_OnCursorItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView );

    //  キー入力
    virtual void LowerItemSelectViewDelegate_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:

    enum Seq
    {
      //  アイテム選択
      SEQ_ITEM_SELECT = 0,
      //  アイテム購入確認
      SEQ_BUY_CONFIRM,
      //  おかねが足りない
      SEQ_BUY_CONFIRM_SELECT_YES_NOT_MONEY,
      //  アイテム購入  
      SEQ_BUY_YES_SUCEESS,
      //  終了時の着せ替え確認
      SEQ_DRESS_UP_CONFIRM,
      //  購入済み
      SEQ_PURCHASED
    };

    //  変数初期化
    void _Clear()
    {
      m_pWork = NULL;
      m_seq = SEQ_ITEM_SELECT;
      m_messageMenuEventCode  = App::Tool::MessageMenuView::IEventHandler::EVENT_;
      m_nowHaveMoney  = 0;
      m_buyCount      = 0;
    }

    //  シーケンス設定
    void _SetSeq( const Seq newSeq );

    App::Kisekae::System::Work*                    m_pWork;

    App::Tool::MessageMenuView::IEventHandler::EventCode           m_messageMenuEventCode;

    Seq m_seq;

    u32 m_nowHaveMoney;

    poke_3d::model::DressUpParam  m_newDressUpParam;

    u32 m_buyCount;
  };

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_APP_SHOP_FRAME_H_INCLUDE
