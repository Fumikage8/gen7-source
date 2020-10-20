// ============================================================================
/*
 * @file SuperTrainingFrame.cpp
 * @brief すごい特訓アプリのプロセスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include "App/SuperTraining/source/Frame/SuperTrainingFrame.h"
#include "App/SuperTraining/source/SuperTrainingFrameResult.h"

//  nijiのインクルード
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "pml/include/pokepara/pml_PokemonParamType.h"

//  リソースのインクルード
#include <arc_index/message.gaix>

//  メッセージのインクルード
#include <niji_conv_header/message/msg_tokkun.h>
#include <niji_conv_header/message/msg_itemname.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(Frame)

  const App::SuperTraining::View::LowerItemListView::ButtonId SuperTrainingFrame::ms_aChkButtonIdList[] =
  {
    App::SuperTraining::View::LowerItemListView::BUTTON_ID_ATK,
    App::SuperTraining::View::LowerItemListView::BUTTON_ID_DEF,
    App::SuperTraining::View::LowerItemListView::BUTTON_ID_HP,
    App::SuperTraining::View::LowerItemListView::BUTTON_ID_SP_ATK,
    App::SuperTraining::View::LowerItemListView::BUTTON_ID_SP_DEF,
    App::SuperTraining::View::LowerItemListView::BUTTON_ID_AGI
  };

  SuperTrainingFrame::SuperTrainingFrame(
    App::SuperTraining::System::SuperTrainingWork* pSuperTrainingWork ) :
    NetAppLib::System::NetAppFrameBase( pSuperTrainingWork )
  {
    _Clear();

    m_pSuperTrainingWork  = pSuperTrainingWork;
  }


  SuperTrainingFrame::~SuperTrainingFrame()
  {
  }


  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  bool SuperTrainingFrame::startup()
  {
    App::SuperTraining::System::ViewSystemAccessor* pViewSysAccessor  = m_pSuperTrainingWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor );

    App::SuperTraining::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );
    pViewObjects->InputDisableAll();

    App::SuperTraining::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );
    pViewList->RemoveAllView();

    App::SuperTraining::APP_PARAM*  pAppParam = m_pSuperTrainingWork->GetAppParam();

    pml::pokepara::PokemonParam*  pPokeParam  = pAppParam->in.pPokeParam;
    GFL_ASSERT( pPokeParam );

    //  上Viewの構築
    {
      App::Tool::FieldStatusUpperDraw*  pUpperStatusView  = pViewObjects->GetUpperStatusView();
      pUpperStatusView->SetVisibleRadarChart( true );
      pUpperStatusView->SetPokemonParam( pPokeParam );

      pViewList->AddUpperView( pUpperStatusView );
    }

    //  ポケモンパラメータを高速アクセスモードに
    bool  bPokeFatMode  = pPokeParam->StartFastMode();
    {
      //  下Viewの構築
      {
        App::SuperTraining::View::LowerItemListView*  pItemListView = pViewObjects->GetLowerItemListView();
        GFL_ASSERT( pItemListView );

        pItemListView->Start();
        pViewList->AddLowerView( pItemListView );

        //  チェック項目をいったんはずす
        {
          u32 loopNum = GFL_NELEMS( ms_aChkButtonIdList );
          pml::pokepara::PowerID  powerId = pml::pokepara::POWER_HP;

          for( u32 i = 0; i < loopNum; ++i )
          {
            //  ボタンidから能力idを取得
            powerId = _GetPokeParaPowerIdFromChkButtonId( ms_aChkButtonIdList[ i ] );

            //  すでに最大値かチェック
            if( pml::MAX_TALENT_POWER <= pPokeParam->GetTalentPower( powerId ) )
            {
              //  最大値なのでチェックボックスは不要
              pItemListView->SetEnableChkBoxSelect( ms_aChkButtonIdList[ i ], false );
            }
            else
            {
              pItemListView->SetChkSelect( ms_aChkButtonIdList[ i ], false );
            }
          }
        }

        //  金の王冠ならアイテムインフォを非表示
        Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();
        if( pAppParam->in.useCrown == App::SuperTraining::USE_CROWN_GOLD )
        {
          Savedata::MyItem::ITEM_ST*  pItemData = pMyItem->GetItem( ITEM_KINNOOUKAN );
          if( pItemData->num <= 0 )
          {
            //  金の王冠が０なのはありえない
            GFL_ASSERT( 0 );
          }

          // @fix NMCat[180] 金の王冠の時は名前と最大個数のみ表示設定
          pItemListView->SetVisibleItemInfo( true );
          pItemListView->SetItemInfoNumberItemLeftPos( pItemData->num );

          //  すべてにチェックをつける
          {
            u32 loopNum = GFL_NELEMS( ms_aChkButtonIdList );
            for( u32 i = 0; i < loopNum; ++i )
            {
              pItemListView->SetEnableChkBoxSelect( ms_aChkButtonIdList[ i ], true );
              pItemListView->SetChkSelect( ms_aChkButtonIdList[ i ], true );
            }

            //  いきなり特訓ボタンに移動する。
            //  そして上下のチェック項目のカーソル移動もしない。
            pItemListView->LockCursorTrainigButton();
          }
        }
        else
        {
          //  銀の王冠の所持値と最大値を設定
          Savedata::MyItem::ITEM_ST*  pItemData = pMyItem->GetItem( ITEM_GINNOOUKAN );
          if( pItemData->num <= 0 )
          {
            //  銀の王冠が０なのはありえない
            GFL_ASSERT( 0 );
          }

          pItemListView->SetVisibleItemInfo( true );
          //  所持値を設定
          pItemListView->SetItemInfoNumber( pItemData->num, pItemData->num );

          //  最初は何もチェックがついていない状態なのでとっくかいしボタンは非表示
          pItemListView->SetVisibleTrainingButton( false );
        }
      }
    }
    pPokeParam->EndFastMode( bPokeFatMode );

    _SetSeq( SEQ_ITEM_SELECT );

    return true;
  }


  bool SuperTrainingFrame::cleanup()
  {
    App::SuperTraining::System::ViewSystemAccessor* pViewSysAccessor  = m_pSuperTrainingWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor );

    App::SuperTraining::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::SuperTraining::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    App::SuperTraining::View::LowerItemListView*  pItemListView = pViewObjects->GetLowerItemListView();

    //  チェックがついている項目のフラグを立てる
    {
      App::SuperTraining::APP_PARAM*  pAppParam = m_pSuperTrainingWork->GetAppParam();

      u32 loopNum = GFL_NELEMS( ms_aChkButtonIdList );
      pml::pokepara::PowerID  powerId = pml::pokepara::POWER_HP;
      for( u32 i = 0; i < loopNum; ++i )
      {
        powerId = _GetPokeParaPowerIdFromChkButtonId( ms_aChkButtonIdList [ i ] );
        GFL_ASSERT( powerId < pml::pokepara::POWER_NUM );

        pAppParam->out.bPowerUpArray[ powerId ] = pItemListView->IsChkSelect( ms_aChkButtonIdList[ i ] );
      }
    }

    return true;
  }


  void SuperTrainingFrame::setListener()
  {
  }


  void SuperTrainingFrame::removeListener()
  {
  }


  void SuperTrainingFrame::updateSequence()
  {
    App::SuperTraining::System::ViewSystemAccessor* pViewSysAccessor  = m_pSuperTrainingWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor );

    App::SuperTraining::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::SuperTraining::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    App::SuperTraining::View::LowerItemListView*  pItemListView = pViewObjects->GetLowerItemListView();

    u32 seq = CellSuper::GetSubSeq();

    //  選択
    if( seq == SEQ_ITEM_SELECT )
    {
      //  上Viewのチャートモード変更
      {
        gfl2::ui::DeviceManager* pUIDevManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
        const gfl2::ui::Button*       button   = pUIDevManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

        //  レーダーチャート切り替え
        if( button->IsTrigger( gfl2::ui::BUTTON_Y ) )
        {
          App::Tool::FieldStatusUpperDraw*  pUpperStatusView  = pViewObjects->GetUpperStatusView();
          pUpperStatusView->ChangeRadarChartMode();
        }
      }


      App::SuperTraining::View::LowerItemListView::ButtonId pushButtonId  = pItemListView->GetPushButtonId();
      if( pushButtonId == App::SuperTraining::View::LowerItemListView::BUTTON_ID_NONE )
      {
        //  何にも押していない
      }
      else
      {
        //  何か押した
        if( pushButtonId == App::SuperTraining::View::LowerItemListView::BUTTON_ID_CANCEL )
        {
          App::SuperTraining::APP_PARAM*  pAppParam = m_pSuperTrainingWork->GetAppParam();
          pAppParam->out.result = App::SuperTraining::APP_RESULT_CANCEL;

          exitFrame( FRAME_RESULT_BACK );
        }
        else
        if( pushButtonId == App::SuperTraining::View::LowerItemListView::BUTTON_ID_START_TRAINIG )
        {
          //  特訓開始
          seq = _SetSeq( SEQ_TRAINING );
        }
        else
        {
          //  チェックボックスが有効か
          if( pItemListView->IsEnableChkBoxSelect( pushButtonId ) == false )
          {
            //  チェックボックスがついていないのはもうあげることができないので注意喚起を出す。

            //  注意喚起表示
            seq = _SetMessageSeq( tokkun_10_01 );
          }
          else
          {
            //  チェックボックスを選択
            if( pItemListView->ChkChangeSelect( pushButtonId ) == false )
            {
              //  ありえない
              GFL_ASSERT( 0 );
            }

            //  新しくチェックがついた
            if( pItemListView->IsChkSelect( pushButtonId ) == true )
            {
              //  チェックをつけて銀の王冠がすでに０個である場合はもうチェックをつけることはできない
              if( pItemListView->GetItemInfoNowNumber() <= 0 )
              {
                //  新しく追加したのはチェックを消す
                pItemListView->SetChkSelect( pushButtonId, false );

                //  注意喚起する
                seq = _SetMessageSeq( tokkun_10_02 );
              }
            }

            //  とっくんボタンを出すか判定
            u32   chkMarkCount  = 0;
            {
              bool  bTrainingButton = false;
              u32 loopNum = GFL_NELEMS( ms_aChkButtonIdList );
              for( u32 i = 0; i < loopNum; ++i )
              {
                if( pItemListView->IsChkSelect( ms_aChkButtonIdList[ i ] ) == true )
                {
                  ++chkMarkCount;
                  bTrainingButton = true;
                }
              }

              pItemListView->SetVisibleTrainingButton( bTrainingButton );
            }

            //  銀の王冠の所持値と最大値を設定
            {
              Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();
              Savedata::MyItem::ITEM_ST*  pItemData = pMyItem->GetItem( ITEM_GINNOOUKAN );

              s32 newCrownNum = (s32)( pItemData->num - chkMarkCount );

              pItemListView->SetItemInfoNumber( newCrownNum, pItemData->num );
            }

            if( seq == SEQ_ITEM_SELECT )
            {
              //  元の選択に戻す
              seq = _SetSeq( seq );
            }
            else
            {
              //  メッセージシーケンスを設定している
            }
          }
        }
      }
    }
    else
    //  特訓開始
    if( seq == SEQ_TRAINING )
    {
    }
    //  メッセージ
    else
    if( seq == SEQ_MESSAGE )
    {
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnFinish )
      {
        seq = _SetSeq( SEQ_ITEM_SELECT );
      }
    }

    CellSuper::SetSubSeq( seq );

    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();  // @fix[956] すごいオヤジのステータス画面でグラフにちらつきが発生している  // SRT更新が1フレーム遅れていたのでここで更新。
  }

  //  シーケンス設定
  u32 SuperTrainingFrame::_SetSeq( const u32 newSeq )
  {
    App::SuperTraining::System::ViewSystemAccessor* pViewSysAccessor  = m_pSuperTrainingWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor );

    App::SuperTraining::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::SuperTraining::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    App::SuperTraining::View::LowerItemListView*  pItemListView = pViewObjects->GetLowerItemListView();

    NetAppLib::UI::NetAppMessageMenu* pMessageMenu  = pViewObjects->GetMessageMenu();
    GFL_ASSERT( pMessageMenu );
    pMessageMenu->SetEventHandler( NULL );
    m_messageMenuEventCode  = App::Tool::MessageMenuView::IEventHandler::EVENT_;

    //  必ずはずす
    {
      pMessageMenu->RemoveFromSuperView();
    }

    u32 seq = newSeq;
    if( seq == SEQ_ITEM_SELECT )
    {
      pItemListView->Start();
    }
    else
    if( seq == SEQ_TRAINING )
    {
      App::SuperTraining::APP_PARAM*  pAppParam = m_pSuperTrainingWork->GetAppParam();
      pAppParam->out.result = App::SuperTraining::APP_RESULT_GO_TRAINING;
      
      exitFrame( FRAME_RESULT_BACK );
    }
    else
    if( seq == SEQ_MESSAGE )
    {
    }
    else
    {
      GFL_ASSERT( 0 );
    }

    return  seq;
  }

  //  メッセージシーケンス設定
  u32 SuperTrainingFrame::_SetMessageSeq( const u32 strId )
  {
    u32 newSeq  = _SetSeq( SEQ_MESSAGE );
    {
      App::SuperTraining::System::ViewSystemAccessor* pViewSysAccessor  = m_pSuperTrainingWork->GetViewSystemAccessor();
      GFL_ASSERT( pViewSysAccessor );

      App::SuperTraining::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
      GFL_ASSERT( pViewObjects );

      App::SuperTraining::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
      GFL_ASSERT( pViewList );

      NetAppLib::UI::NetAppMessageMenu* pMessageMenu  = pViewObjects->GetMessageMenu();

      pMessageMenu->SetEventHandler( this );
      pMessageMenu->SetYNListMode( false );
      if( strId != tokkun_10_02 )
      {
        pMessageMenu->ShowMessage( GARC_message_tokkun_DAT, strId, false, false, true );
      }
      else
      {
        App::Tool::MessageMenuView* pMessageOrgView  = pMessageMenu;

        //  作業用文字列バッファを別Viewから取得（一時的使用なので問題ない
        app::util::G2DUtil* pG2DUtil  = NULL;
        {
          App::SuperTraining::View::LowerItemListView*  pItemListView = pViewObjects->GetLowerItemListView();
          pG2DUtil  = pViewObjects->GetLowerItemListView()->GetG2DUtil();
          GFL_ASSERT( pG2DUtil );
        }
        gfl2::str::StrBuf*  pTmpSrcStrBuf  = pG2DUtil->GetTempStrBuf( 0 );
        gfl2::str::StrBuf*  pTmpDstStrBuf  = pG2DUtil->GetTempStrBuf( 1 );

        //  ここのメッセージは銀の王冠使用時にしかこない
        /*
          @attention
            アイテム名の取得がルール違反だが、nijiで問題ないとのことなので、momijiでも変更しない。
            RegisterItemName()を使用するべき。
        */
        print::WordSet* pWordSet  = pG2DUtil->GetWordSet();
        pWordSet->RegisterWord( 0, m_pSuperTrainingWork->GetMessageUtility()->GetData( GARC_message_itemname_DAT ), ITEMNAME_795 );

        gfl2::str::MsgData* pMsgData  = m_pSuperTrainingWork->GetMessageUtility()->GetData( GARC_message_tokkun_DAT );
        pMsgData->GetString( strId, *pTmpSrcStrBuf );
        pWordSet->Expand( pTmpDstStrBuf, pTmpSrcStrBuf );
        
        //  派生元にしないと設定できない
        pMessageOrgView->ShowMessage( pTmpDstStrBuf, false, false, true );
      }
      pMessageMenu->SetFinishMode_UserInput();

      pViewList->AddLowerViewInputEnable( pMessageMenu );
    }

    return  newSeq;
  }

  void  SuperTrainingFrame::MessageMenuView_OnEvent(const EventCode event)
  {
    m_messageMenuEventCode  = event;
  }

  //  チェック項目ボタンidからポケモンパラメータの能力idに変換
  pml::pokepara::PowerID  SuperTrainingFrame::_GetPokeParaPowerIdFromChkButtonId( const App::SuperTraining::View::LowerItemListView::ButtonId buttonId )
  {
    switch( buttonId )
    {
      case App::SuperTraining::View::LowerItemListView::BUTTON_ID_ATK:      return  pml::pokepara::POWER_ATK;
      case App::SuperTraining::View::LowerItemListView::BUTTON_ID_DEF:      return  pml::pokepara::POWER_DEF;
      case App::SuperTraining::View::LowerItemListView::BUTTON_ID_HP:       return  pml::pokepara::POWER_HP;
      case App::SuperTraining::View::LowerItemListView::BUTTON_ID_SP_ATK:   return  pml::pokepara::POWER_SPATK;
      case App::SuperTraining::View::LowerItemListView::BUTTON_ID_SP_DEF:   return  pml::pokepara::POWER_SPDEF;
      case App::SuperTraining::View::LowerItemListView::BUTTON_ID_AGI:      return  pml::pokepara::POWER_AGI;
    }

    return  pml::pokepara::POWER_AGI;
  }

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)
