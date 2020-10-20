// ============================================================================
/*
 * @file ColorDressUpFrame.h
 * @brief きせかえアプリのいろそめきせかえフレーム
 * @date 2015.10.19
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_APP_COLOR_DRESS_UP_FRAME_H_INCLUDE )
#define KISEKAE_APP_COLOR_DRESS_UP_FRAME_H_INCLUDE
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

  class ColorDressUpFrame : 
    public NetAppLib::System::NetAppFrameBase,
    public App::Tool::MessageMenuView::IEventHandler,
    public App::Kisekae::View::LowerItemSelectViewDelegate,
    public app::tool::CharaModelViewerDelegate
  {
    GFL_FORBID_COPY_AND_ASSIGN( ColorDressUpFrame );
  public:
    ColorDressUpFrame(
      App::Kisekae::System::Work* pWork );
    virtual~ColorDressUpFrame();

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

    //  項目内のアイテムはきせかえ可能なのものか
    virtual bool LowerItemSelectViewDelegate_OnIsDressupOk( App::Kisekae::System::ItemDataBase* pItemData );

    //  項目更新時に呼ばれる
    virtual void LowerItemSelectViewDelegate_OnUpdateItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView );

    //  キー入力
    virtual void LowerItemSelectViewDelegate_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:

    enum Seq
    {
      //  アイテム選択
      SEQ_ITEM_SELECT = 0,

      //  いろそめ確認
      SEQ_SETTING_COLOR_CONFIRM,

      //  金額確認
      SEQ_USE_MONEY_CONFIRM,

      //  そめる
      SEQ_SET_COLOR,

      //  そめる完了
      SEQ_SET_COLOR_COMPLETE,

      //  おかねが足りない
      SEQ_MESSAGE_NOT_MONEY,

      //  データ変更確認
      SEQ_CHANGE_CONFIRM
    };

    void _Clear()
    {
      m_seq = SEQ_ITEM_SELECT;
      m_messageMenuEventCode  = App::Tool::MessageMenuView::IEventHandler::EVENT_;

      m_pWork = NULL;
      m_nowFesCoin  = 0;
      m_waitCnt     = 0;
    }

    //  シーケンス設定
    void _SetSeq( const Seq newSeq );

    App::Kisekae::System::Work*                    m_pWork;

    App::Tool::MessageMenuView::IEventHandler::EventCode           m_messageMenuEventCode;

    Seq m_seq;

    s32 m_waitCnt;
    u32 m_nowFesCoin;

    poke_3d::model::DressUpParam  m_newDressUpParam;
  };

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_APP_COLOR_DRESS_UP_FRAME_H_INCLUDE
