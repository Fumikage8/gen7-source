// ============================================================================
/*
 * @file ShopFrame.h
 * @brief きせかえアプリの購入フレーム
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/Frame/KisekaeShopFrame.h"
#include "App/Kisekae/source/KisekaeFrameResult.h"

//  gflib2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/MiscSave.h"

//  Viewのインクルード
#include "App/Kisekae/source/System/KisekaeViewSystemAccessor.h"
#include "App/Kisekae/source/Tool/CharaModelViewer.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_kisekae.h>
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(Frame)

  ShopFrame::ShopFrame(
    App::Kisekae::System::Work* pWork ) :
    NetAppLib::System::NetAppFrameBase( pWork )    
  {
    _Clear();

    m_pWork = pWork;
  }

  ShopFrame::~ShopFrame()
  {
  }

  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  bool ShopFrame::startup()
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    Savedata::Misc* pMiscSaveData = pGameManager->GetGameData()->GetMisc();
    GFL_ASSERT( pMiscSaveData);

    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );
    pViewObjects->InputDisableAll();

    App::Kisekae::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );
    pViewList->RemoveAllView();

    //  モデルビューアーの設定
    {
      app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
      GFL_ASSERT( pCharaModelViewr );

      pCharaModelViewr->SetDelegate( this );

      //  回転入力イベント
      {
        pCharaModelViewr->StartRotYInputEvent();
      }

      //  現在のキャラモデルの最新の着せ替えパラメータを取得
      m_newDressUpParam = pCharaModelViewr->GetFirstCharaModelDressUpParam();
    }

    //  上Viewの構築
    {
      App::Kisekae::View::UpperBGView*  pBGView = pViewObjects->GetUpperBGView();
      GFL_ASSERT( pBGView );

      pViewList->AddUpperView( pBGView );

      //  初回所持金値を設定
      m_nowHaveMoney  = pMiscSaveData->GetGold();
      pBGView->ShowMoneyWindow( m_nowHaveMoney );

      //  ユーザー操作チュートリアル表示
      pBGView->SetVisibleUserInputTutorialIcon( true );
    }

    //  下Viewの構築
    {
      App::Kisekae::View::LowerItemSelectView*  pItemSelectView = pViewObjects->GetLowerItemSelectView();
      GFL_ASSERT( pItemSelectView );

      pViewList->AddLowerView( pItemSelectView );

      pItemSelectView->SetDelgate( this );

      //  固定メッセージを指定
      {
        pItemSelectView->ShowMessageLower( &m_pWork->GetMessageUtility()->GetString( GARC_message_kisekae_DAT, kisekae_01_04 ), true );
      }

      //  初回リスト設定
      pItemSelectView->CreateList( m_pWork->GetItemList(), 1, App::Kisekae::View::LowerItemSelectView::MODE_SHOP, true, true );

      pItemSelectView->StartEvent();
    }

    _SetSeq( SEQ_ITEM_SELECT );

    return true;
  }

  bool ShopFrame::cleanup()
  {
    m_pWork->GetCharaModelViewer()->StartIdleingEvent();

    //  購入した回数をダイアリーに記録
    // @fix GFNMCat[5370] きせかえ購入をしていない場合はダイアリー記録をしない
    if( 0 < m_buyCount )
    {
      PokeDiary::Set( PDID_COUNT_KISEKAE, m_buyCount );
    }

    return true;
  }

  void ShopFrame::setListener()
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    //  下画面入力
    {
      App::Kisekae::View::LowerItemSelectView*  pSelectView = pViewObjects->GetLowerItemSelectView();

      pSelectView->SetInputEnabledAllView( true );
    }
  }

  void ShopFrame::removeListener()
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    NetAppLib::UI::NetAppMessageMenu* pMessageMenu  = pViewObjects->GetMessageMenu();
    GFL_ASSERT( pMessageMenu );
    pMessageMenu->SetEventHandler( NULL );

    App::Kisekae::View::LowerItemSelectView*  pSelectView = pViewObjects->GetLowerItemSelectView();
    pSelectView->SetDelgate( NULL );

    {
      app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
      GFL_ASSERT( pCharaModelViewr );

      pCharaModelViewr->SetDelegate( NULL );
    }
  }

  void ShopFrame::updateSequence()
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Kisekae::View::LowerItemSelectView*  pLowerItemSelectView  = pViewObjects->GetLowerItemSelectView();
    GFL_ASSERT( pLowerItemSelectView );

    if( m_seq == SEQ_ITEM_SELECT )
    {
      //  アイテム選択
      App::Kisekae::View::LowerItemSelectView::ButtonId pushButtonId  = pLowerItemSelectView->GetPushId();

      //  Bキャンセル
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CANCEL )
      {
        _SetSeq( SEQ_DRESS_UP_CONFIRM );
      }
      //  すがたを見る
      else
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_VIEWR )
      {
        //  ショップでは押すことはない
        GFL_ASSERT( 0 );
      }
      //  きがえる
      else
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_DRESS_UP )
      {
        //  ショップでは押すことはない
        GFL_ASSERT( 0 );
      }
      //  アイテムを押した
      else
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_ITEM )
      {
        _SetSeq( SEQ_BUY_CONFIRM );
      }
      else
      {
        //  アイドリング中
        Savedata::Fashion::Category category  = m_pWork->ConvCategoryIdFromLowerItemSelectViewButtonId( pushButtonId );
        if( category == Savedata::Fashion::CATEGORY_MAX )
        {
        }
        else
        {
          pLowerItemSelectView->StartIdling();

          //  リストを変える
          {
            App::Kisekae::System::ItemList* pItemList  = m_pWork->GetItemList();

            //  リストのカーソル位置を保存
            {
              pItemList->SetListPos( pLowerItemSelectView->GetListPos() );
              pItemList->SetListScroll( pLowerItemSelectView->GetListScroll() );
            }

            //  カテゴリー変更
            pItemList->SetActiveCategory( category );

            pLowerItemSelectView->CreateList( pItemList, 1, App::Kisekae::View::LowerItemSelectView::MODE_SHOP, true, false );
          }
        }
      }
    }
    else
    if( m_seq == SEQ_BUY_CONFIRM )
    {
      //  アイテム購入確認
      /**<  Yes/NoモードでYesが選択された                               */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
      {
        //  お金が足りているかチェック
        App::Kisekae::System::ItemList* pItemList  = m_pWork->GetItemList();
        App::Kisekae::System::ItemDataBase* pItemDataBase = NULL;
        {
          pItemDataBase = pItemList->GetData( pLowerItemSelectView->GetPushItemSelectIdx() );
        }

        if( ( m_nowHaveMoney <= 0 ) || ( m_nowHaveMoney < pItemDataBase->GetMoney() ) )
        {
          //  お金が足りない
          _SetSeq( SEQ_BUY_CONFIRM_SELECT_YES_NOT_MONEY );
        }
        else
        {
          //  お金が足りた
          _SetSeq( SEQ_BUY_YES_SUCEESS );
        }
      }
      else
      /**<  Yes/NoモードでNoが選択された                                */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
      {
        _SetSeq( SEQ_ITEM_SELECT );
      }
    }
    else
    if( m_seq == SEQ_BUY_CONFIRM_SELECT_YES_NOT_MONEY )
    {
      //  おかねが足りない
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnFinish )
      {
        _SetSeq( SEQ_ITEM_SELECT );
      }
    }
    else
    if( m_seq == SEQ_BUY_YES_SUCEESS )
    {
      //  アイテム購入
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnFinish )
      {
        _SetSeq( SEQ_ITEM_SELECT );
      }
    }
    else
    if( m_seq == SEQ_DRESS_UP_CONFIRM )
    {
      //  終了時の購入アイテムの着せ替え確認
      /**<  Yes/NoモードでYesが選択された                               */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
      {
        //  MyStatusの着せ替えを反映
        {
          //  プレイヤーのトレーナー情報を取得してモデル表示
          Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
          pMyStatus->SetDressup( m_newDressUpParam );
        }

        m_pWork->OnDressup();

        //  きせかえ完了SE
        Sound::PlaySE( SEQ_SE_COMPLETE1 );

        exitFrame( RESULT_CODE_RESULT_BACK );
      }
      else
      /**<  Yes/NoモードでNoが選択された                                */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
      {
        exitFrame( RESULT_CODE_RESULT_BACK );
      }
    }
    else
    if( m_seq == SEQ_PURCHASED )
    {
      //  購入済み
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnFinish )
      {
        _SetSeq( SEQ_ITEM_SELECT );
      }
    }
  }

  //  シーケンス設定
  void ShopFrame::_SetSeq( const Seq newSeq )
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Kisekae::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    NetAppLib::UI::NetAppMessageMenu* pMessageMenu  = pViewObjects->GetMessageMenu();
    GFL_ASSERT( pMessageMenu );
    //  メッセージメニューを初期化
    {
      pMessageMenu->SetEventHandler( NULL );
      m_messageMenuEventCode  = App::Tool::MessageMenuView::IEventHandler::EVENT_;
    }

    //  必ずはずす
    pMessageMenu->RemoveFromSuperView();

    m_seq = newSeq;

    if( m_seq == SEQ_ITEM_SELECT )
    {
      //  アイテム選択

      //  下画面開始
      {
        App::Kisekae::View::LowerItemSelectView*  pItemSelectView = pViewObjects->GetLowerItemSelectView();
        GFL_ASSERT( pItemSelectView );

        pItemSelectView->StartIdling();
      }
    }
    else
    if( m_seq == SEQ_BUY_CONFIRM )
    {
      App::Kisekae::View::LowerItemSelectView*  pItemSelectView = pViewObjects->GetLowerItemSelectView();

      App::Kisekae::System::ItemList* pItemList  = m_pWork->GetItemList();
      App::Kisekae::System::ItemDataBase* pItemDataBase = NULL;
      {
        pItemDataBase = pItemList->GetData( pItemSelectView->GetPushItemSelectIdx() );
      }

      //  すでに購入しているかチェック
      if( pItemDataBase->IsBuy() == true )
      {
        //  すでに購入しているので別シーケンスを呼ぶ
        _SetSeq( SEQ_PURCHASED );
      }
      else
      //  アイテム購入確認
      {
        App::Tool::MessageMenuView* pMessageMenuView  = pMessageMenu;

        gfl2::str::StrBuf*  pTmpStrBuf  = m_pWork->GetTmpStrBuf( 0 );

        //  メッセージ
        u32 strId = kisekae_03_01;
        {
          //  きせかえパラメータをチェックするために購入するアイテムidに切り替え
          //  @fix GFNMCat[1410] 現在選択しているアイテムidを反映してきせかえ可能かチェックして不可能であれば専用メッセージを出す
          poke_3d::model::DressUpParam  chkDressUpParam;
          m_pWork->OutputChkDressUpParam( &chkDressUpParam, m_pWork->GetCharaModelViewer()->GetNowCharaModelDressUpParam(), pItemDataBase );
          if( m_pWork->GetCharaModelViewer()->CheckDressUpCombination( chkDressUpParam ) == false )
          {
            //  きせかえることができないが購入可能
            strId = kisekae_03_09;
          }
        }

        gfl2::str::StrBuf& rStrBuf = m_pWork->GetMessageUtility()->GetString( GARC_message_kisekae_DAT, strId );
        //  文字列作成
        {
          print::WordSet*  pWordSet  = m_pWork->GetMessageUtility()->GetWordSet();
          // expand
          pWordSet->RegisterNumber( 0, pItemDataBase->GetMoney(), Savedata::Misc::MY_GOLD_KETA, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
          pWordSet->Expand( pTmpStrBuf, &rStrBuf );
        }

        pMessageMenu->SetEventHandler( this );

        pMessageMenu->SetYNListMode( true );
        pMessageMenu->SetFinishMode_None();

        pMessageMenuView->ShowMessageWithListMenu(
          pTmpStrBuf,
          false,
          *m_pWork->GetMessageUtility()->GetData( GARC_message_kisekae_DAT ),
          select_yes,
          select_no );
      }

      pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
    else
    if( m_seq == SEQ_BUY_CONFIRM_SELECT_YES_NOT_MONEY )
    {
      //  おかねが足りない
      pMessageMenu->SetEventHandler( this );
      pMessageMenu->SetYNListMode( false );
      pMessageMenu->ShowMessage( GARC_message_kisekae_DAT, kisekae_03_05, false, false, true );
      pMessageMenu->SetFinishMode_UserInput();

      pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
    else
    if( m_seq == SEQ_BUY_YES_SUCEESS )
    {
      //  アイテム購入
      pMessageMenu->SetEventHandler( this );
      pMessageMenu->SetYNListMode( false );
      pMessageMenu->ShowMessage( GARC_message_kisekae_DAT, kisekae_03_04, false, false, true );
      pMessageMenu->SetFinishMode_UserInput();

      //  購入SE
      Sound::PlaySE( SEQ_SE_SYS_006 );

      //  購入カウント
      ++m_buyCount;

      //  現在選択している項目をセーブに追加
      App::Kisekae::System::ItemDataBase* pItemDataBase = NULL;
      {
        App::Kisekae::View::LowerItemSelectView*  pItemSelectView = pViewObjects->GetLowerItemSelectView();

        //  データ取得
        {
          App::Kisekae::System::ItemList* pItemList  = m_pWork->GetItemList();

          pItemDataBase = pItemList->GetData( pItemSelectView->GetPushItemSelectIdx() );
        }

        //  セーブ変更
        {
          GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
          GFL_ASSERT( pGameManager );

          //  お金を減らす
          {
            Savedata::Misc* pMiscSaveData = pGameManager->GetGameData()->GetMisc();
            GFL_ASSERT( pMiscSaveData );

            pMiscSaveData->SubGold( pItemDataBase->GetMoney() );
            m_nowHaveMoney  = pMiscSaveData->GetGold();
          }

          //  アイテム追加
          {
            //  内部でセーブしている
            pItemDataBase->SetBuy();

            //  着せ替え反映パラメータに設定
            {
              //  きせかえ可能かチェック
              //  @fix GFNMCat[1410] 現在選択しているアイテムidを反映してきせかえ可能かチェック
              poke_3d::model::DressUpParam  chkDressUpParam;
              m_pWork->OutputChkDressUpParam( &chkDressUpParam, m_pWork->GetCharaModelViewer()->GetNowCharaModelDressUpParam(), pItemDataBase );
              if( m_pWork->GetCharaModelViewer()->CheckDressUpCombination( chkDressUpParam ) == true )
              {
                poke_3d::model::DressUpParam::ItemCategory  category  = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( pItemDataBase->GetCategory() );
                m_newDressUpParam.itemParams[ category ]  = pItemDataBase->GetItemId();
              }
            }
          }
        }

        //  画面更新
        {
          //  上画面更新
          {
            App::Kisekae::View::UpperBGView*  pBGView = pViewObjects->GetUpperBGView();
            GFL_ASSERT( pBGView );

            pBGView->ShowMoneyWindow( m_nowHaveMoney );
          }

          //  下画面更新
          {
            pItemSelectView->Refresh();
          }
        }
      }

      pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
    else
    if( m_seq == SEQ_DRESS_UP_CONFIRM )
    {
      //  終了時の購入アイテムの着せ替え確認
      poke_3d::model::DressUpParam nowEquipDressUpParam;
      {
        Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
        nowEquipDressUpParam = pMyStatus->GetDressup();
      }

      bool  bChgDressUpParam  = Savedata::Fashion::IsChangeDressUpParam( m_newDressUpParam, nowEquipDressUpParam );

      //  なにも買っていない場合はそのまま終了
      if( bChgDressUpParam == true )
      {
        pMessageMenu->SetEventHandler( this );
        pMessageMenu->SetYNListMode( true );
        pMessageMenu->SetFinishMode_None();

        pMessageMenu->ShowMessageWithListMenu(
          GARC_message_kisekae_DAT,
          kisekae_03_02,
          false,
          GARC_message_kisekae_DAT,
          select_yes,
          select_no );

        pViewList->AddLowerViewInputEnable( pMessageMenu );
      }
      else
      {
        exitFrame( RESULT_CODE_RESULT_BACK );
      }
    }
    else
    if( m_seq == SEQ_PURCHASED )
    {
      //  購入済み
      pMessageMenu->SetEventHandler( this );
      pMessageMenu->SetYNListMode( false );
      pMessageMenu->ShowMessage( GARC_message_kisekae_DAT, kisekae_03_07, false, false, true );
      pMessageMenu->SetFinishMode_UserInput();

      pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
  }

  void  ShopFrame::MessageMenuView_OnEvent(const EventCode event)
  {
    m_messageMenuEventCode  = event;
  }

  //  選択項目にカーソルが設定された時に呼ばれる
  void ShopFrame::LowerItemSelectViewDelegate_OnCursorItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView )
  {
    poke_3d::model::DressUpParam  nowDressUpParam = m_newDressUpParam;

    if( pItemData == NULL )
    {
      //  リストが空の場合は変更前のをきせかえ
    }
    else
    {
      //  セーブデータが知っているカテゴリーとモデル側で知っているカテゴリーが異なるので変換する
      const poke_3d::model::DressUpParam::ItemCategory  dressUpCategory = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( pItemData->GetCategory() );
      nowDressUpParam.itemParams[ dressUpCategory ]  = pItemData->GetItemId();
    }

    //  ロード開始
    {
      m_pWork->OutputDressupParam( &nowDressUpParam, nowDressUpParam, m_newDressUpParam );

      app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
      GFL_ASSERT( pCharaModelViewr );

      pCharaModelViewr->StartDressupEvent( nowDressUpParam );
    }
  }

  //  キー入力
  void ShopFrame::LowerItemSelectViewDelegate_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
    GFL_ASSERT( pCharaModelViewr );

    pCharaModelViewr->NoticeKeyAction( pButton, pKey, pStick );
  }

  //  きせかえ終了通知
  void ShopFrame::CharaModelViewer_OnDressupEnd()
  {
    //  キャラの着せ替え終了
    app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
    GFL_ASSERT( pCharaModelViewr );

    //  回転入力再開
    pCharaModelViewr->StartRotYInputEvent();
  }

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
