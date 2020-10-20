// ============================================================================
/*
 * @file DressUpFrame.h
 * @brief きせかえアプリの所持きせかえフレーム
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_APP_DRESS_UP_FRAME_H_INCLUDE )
#define KISEKAE_APP_DRESS_UP_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  netlibのインクルード
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"

#include "App/Kisekae/source/System/KisekaeWork.h"
#include "App/Kisekae/source/View/KisekaeLowerItemSelectView.h"
#include "App/Kisekae/source/View/KisekaeLowerInfoView.h"

#include "App/Kisekae/source/Tool/CharaModelViewer.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(Frame)

  class DressUpFrame : 
    public NetAppLib::System::NetAppFrameBase,
    public App::Tool::MessageMenuView::IEventHandler,
    public App::Kisekae::View::LowerItemSelectViewDelegate,
    public App::Kisekae::View::LowerInfoViewDelegata,
    public app::tool::CharaModelViewerDelegate
  {
    GFL_FORBID_COPY_AND_ASSIGN( DressUpFrame );
  public:
    DressUpFrame(
      App::Kisekae::System::Work* pWork );
    virtual~DressUpFrame();

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

    //  選択項目を押した
    virtual bool LowerItemSelectViewDelegate_OnPushItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView );

    //  選択項目にカーソルが設定された時に呼ばれる
    virtual void LowerItemSelectViewDelegate_OnCursorItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView );

    //  選択項目にカーソルがヒットし続けている間はずっと呼ばれる
    virtual void LowerItemSelectViewDelegate_OnCursorHitItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView );

    //  項目内のアイテムはきせかえ可能なのものか
    virtual bool LowerItemSelectViewDelegate_OnIsDressupOk( App::Kisekae::System::ItemDataBase* pItemData );

    //  項目更新時に呼ばれる
    virtual void LowerItemSelectViewDelegate_OnUpdateItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView );

    //  キー入力取得(InfoView)
    virtual bool LowerInfoViewDelegata_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //  キー入力取得(ItemSelectView)
    virtual void LowerItemSelectViewDelegate_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:

    enum Seq
    {
      //  アイテム選択
      SEQ_ITEM_SELECT = 0,

      //  モデルショー
      SEQ_MODEL_SHOW,

      //  データ変更確認
      SEQ_CHANGE_CONFIRM,
    };

    void _Clear()
    {
      m_seq = SEQ_ITEM_SELECT;
      m_messageMenuEventCode  = App::Tool::MessageMenuView::IEventHandler::EVENT_;

      m_newFlgDownCount = 0;
      m_pWork = NULL;
    }

    //  現在装備しているきせかえが変わっているいるか
    bool _IsChangeDressup();

    //  指定したカテゴリーで装備しているきせかえ情報を更新
    void _UpdateEquipItemInfo( const Savedata::Fashion::Category cateogyr );

    //  シーケンス設定
    void _SetSeq( const Seq newSeq );

    App::Kisekae::System::Work*                    m_pWork;

    u32 m_newFlgDownCount;
    App::Tool::MessageMenuView::IEventHandler::EventCode           m_messageMenuEventCode;

    Seq m_seq;

    poke_3d::model::DressUpParam  m_newDressUpParam;
  };

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_APP_DRESS_UP_FRAME_H_INCLUDE
