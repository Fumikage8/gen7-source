// ============================================================================
/*
 * @file KisekaeColorDressUpFrame.h
 * @brief きせかえアプリのいろそめきせかえフレーム
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/Frame/KisekaeColorDressUpFrame.h"
#include "App/Kisekae/source/KisekaeFrameResult.h"

//  gflib2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/MiscSave.h"
#include "Savedata/include/JoinFestaDataSave.h"

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

  ColorDressUpFrame::ColorDressUpFrame(
    App::Kisekae::System::Work* pWork ) :
    NetAppLib::System::NetAppFrameBase( pWork )
  {
    _Clear();
    
    m_pWork = pWork;
  }

  ColorDressUpFrame::~ColorDressUpFrame()
  {
  }

  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  bool ColorDressUpFrame::startup()
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    Savedata::JoinFestaDataSave* pJoinFestaDataSave = pGameManager->GetGameData()->GetJoinFestaData();
    GFL_ASSERT( pJoinFestaDataSave );

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

      pBGView->SetGoldMode( App::Kisekae::View::UpperBGView::MONEY_MODE_TYPE_FC );
      pViewList->AddUpperView( pBGView );

      //  所持しているフェスコインを設定
      m_nowFesCoin  = pJoinFestaDataSave->GetCoin();
      pBGView->ShowMoneyWindow( m_nowFesCoin );

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
        pItemSelectView->ShowMessageLower( &m_pWork->GetMessageUtility()->GetString( GARC_message_kisekae_DAT, kisekae_10_03 ), true );
      }

      //  初回リスト設定
      pItemSelectView->CreateList( m_pWork->GetItemList(), 1, App::Kisekae::View::LowerItemSelectView::MODE_COLOR_DRESS_UP, true, true );

      pItemSelectView->StartEvent();
    }

    _SetSeq( SEQ_ITEM_SELECT );

    return true;
  }

  bool ColorDressUpFrame::cleanup()
  {
    m_pWork->GetCharaModelViewer()->StartIdleingEvent();

    return true;
  }

  void ColorDressUpFrame::setListener()
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

  void ColorDressUpFrame::removeListener()
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

  void ColorDressUpFrame::updateSequence()
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
        _SetSeq( SEQ_CHANGE_CONFIRM );
      }
      //  すがたを見る
      else
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_VIEWR )
      {
        GFL_ASSERT( 0 );
      }
      //  きがえる
      else
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_DRESS_UP )
      {
        GFL_ASSERT( 0 );
      }
      //  アイテムを押した
      else
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_ITEM )
      {
        _SetSeq( SEQ_SETTING_COLOR_CONFIRM );
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

            pLowerItemSelectView->CreateList( pItemList, 1, App::Kisekae::View::LowerItemSelectView::MODE_COLOR_DRESS_UP, true, false );
          }
        }
      }
    }
    else
    if( m_seq == SEQ_SETTING_COLOR_CONFIRM )
    {
      //  そめるアイテムの確認
      /**<  Yes/NoモードでYesが選択された                               */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
      {
        //  金額チェック
        _SetSeq( SEQ_USE_MONEY_CONFIRM );
      }
      else
      /**<  Yes/NoモードでNoが選択された                                */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
      {
        //  そめずに戻る
        _SetSeq( SEQ_ITEM_SELECT );
      }
    }
    else
    if( m_seq == SEQ_USE_MONEY_CONFIRM )
    {
      //  金額確認
      /**<  Yes/NoモードでYesが選択された                               */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
      {
        App::Kisekae::System::ItemDataBase* pItemData = m_pWork->GetItemList()->GetData( pLowerItemSelectView->GetPushItemSelectIdx() );

        //  金額チェック
        if( pItemData->GetMoney() <= m_nowFesCoin )
        {
          //  染める処理開始
          _SetSeq( SEQ_SET_COLOR );
        }
        else
        {
          _SetSeq( SEQ_MESSAGE_NOT_MONEY );
        }
      }
      else
      /**<  Yes/NoモードでNoが選択された                                */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
      {
        //  そめずに戻る
        _SetSeq( SEQ_ITEM_SELECT );
      }
    }
    else
    if( m_seq == SEQ_SET_COLOR )
    {
      //  そめる
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
      {
        {
          App::Kisekae::System::ItemDataBase* pItemData = m_pWork->GetItemList()->GetData( pLowerItemSelectView->GetPushItemSelectIdx() );

          //  フェスコインを消耗
          {
            GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
            GFL_ASSERT( pGameManager );

            Savedata::JoinFestaDataSave* pJoinFestaDataSave = pGameManager->GetGameData()->GetJoinFestaData();
            GFL_ASSERT( pJoinFestaDataSave );

            // コイン消費
            pJoinFestaDataSave->SubCoin( pItemData->GetMoney() );
            m_nowFesCoin = pJoinFestaDataSave->GetCoin();
          }

          //  フェスコイン値反映
          {
            App::Kisekae::View::UpperBGView*  pBGView = pViewObjects->GetUpperBGView();
            GFL_ASSERT( pBGView );

            pBGView->ShowMoneyWindow( m_nowFesCoin );
          }

          //  データに色染め設定
          {
            pItemData->SettingColor();
          }

          //  染めた色が白orライトグレー以外の場合は設定
          {
            App::Kisekae::APP_PARAM*  pAppParam = m_pWork->GetAppParam();
            GFL_ASSERT( pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_COLOR_SHOP );

            //  木の実使用時限定
            if( pAppParam->typeData.colorShop.bUseKinomiDiscount == true )
            {
              pAppParam->output.bKinomiChangeColor  = ( pItemData->IsColorDefault() == false );
            }
          }

          //  色を染めた着せ替えがすでにプレイヤーが装備している場合はステータスに色染め時の値を設定する必要がある
          {
            //  パーツidから判別可能
            poke_3d::model::DressUpModelResourceManagerCore*  pDressUpResCore = m_pWork->GetDressUpModelResManagerCore();
            GFL_ASSERT( pDressUpResCore );

            const poke_3d::model::DressUpParam::ItemCategory  dressUpCategory = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( pItemData->GetCategory() );

            s16 newEquipPartsId  = 0;
            //  新しく装備した色つきのアイテムidのパーツidを取得
            {
              newEquipPartsId = pDressUpResCore->GetDressUpPartsIndex( dressUpCategory, pItemData->GetItemId() );
            }

            Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
            poke_3d::model::DressUpParam nowEquipDressUpParam = pMyStatus->GetDressup();

            s16 nowEquipPartsId = -1;
            //  現在装備している色つきのアイテムidのパーツidを取得
            {
              // @fix NMCat[672] きていない状態を想定していなかった
              if( nowEquipDressUpParam.itemParams[ dressUpCategory ] != -1 )
              {
                nowEquipPartsId = pDressUpResCore->GetDressUpPartsIndex( dressUpCategory, nowEquipDressUpParam.itemParams[ dressUpCategory ] );
              }
            }

            //  装備しているのが同じなのであればMyStatusのitemidを更新する
            //  itemidは変わっているので
            if( newEquipPartsId == nowEquipPartsId )
            {
              //  同じパーツidでも色染め用のでなければ差分設定する
              if( App::Kisekae::System::ItemList::IsColorDressup( pDressUpResCore->GetDressUpColorIndex( dressUpCategory, nowEquipDressUpParam.itemParams[ dressUpCategory ] ) ) == false )
              {
                //  今装備しているのが色染めでない。
              }
              else
              {
                //  今装備しているのが色染めで同じパーツなので差分設定は不要
                nowEquipDressUpParam.itemParams[ dressUpCategory ]  = pItemData->GetItemId();
                pMyStatus->SetDressup( nowEquipDressUpParam );
              }
              
              //  @fix GFNMCat[1410] 現在選択しているアイテムidを反映してきせかえ可能かチェック可能であれば変更の最新として設定
              poke_3d::model::DressUpParam  chkDressUpParam;
              m_pWork->OutputChkDressUpParam( &chkDressUpParam, m_pWork->GetCharaModelViewer()->GetNowCharaModelDressUpParam(), pItemData );
              if( m_pWork->GetCharaModelViewer()->CheckDressUpCombination( chkDressUpParam ) == true )
              {
                m_newDressUpParam.itemParams[ dressUpCategory ]  = pItemData->GetItemId();
              }
            }
            else
            {
              //  partsIdが変わっている場倍は装備変更した

              //  @fix GFNMCat[1410] 現在選択しているアイテムidを反映してきせかえ可能かチェック可能であれば変更の最新として設定
              poke_3d::model::DressUpParam  chkDressUpParam;
              m_pWork->OutputChkDressUpParam( &chkDressUpParam, m_pWork->GetCharaModelViewer()->GetNowCharaModelDressUpParam(), pItemData );
              if( m_pWork->GetCharaModelViewer()->CheckDressUpCombination( chkDressUpParam ) == true )
              {
                m_newDressUpParam.itemParams[ dressUpCategory ]  = pItemData->GetItemId();
              }
            }
          }

          _SetSeq( SEQ_SET_COLOR_COMPLETE );
        }
      }
    }
    else
    if( m_seq == SEQ_SET_COLOR_COMPLETE )
    {
      if( Sound::IsSEFinished( SEQ_SE_COMPLETE1 ) != 0 )
      {
        App::Kisekae::APP_PARAM*  pAppParam = m_pWork->GetAppParam();
        GFL_ASSERT( pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_COLOR_SHOP );

        if( pAppParam->typeData.colorShop.bUseKinomiDiscount == false )
        {
          //  リスト構築しなおし
          App::Kisekae::System::ItemList* pItemList  = m_pWork->GetItemList();
          {
            //  リストのカーソル位置を保存
            {
              pItemList->SetListPos( pLowerItemSelectView->GetListPos() );
              pItemList->SetListScroll( pLowerItemSelectView->GetListScroll() );
            }

            //  リストの構築しなおし
            pItemList->Refresh();

            //  下画面を更新ではなく作りなおす
            {
              pLowerItemSelectView->CreateList( m_pWork->GetItemList(), 1, App::Kisekae::View::LowerItemSelectView::MODE_COLOR_DRESS_UP, true, true );
            }
          }

          _SetSeq( SEQ_ITEM_SELECT );
        }
        else
        {
          //  木の実使用時には色をそめたら即時終了
          _SetSeq( SEQ_CHANGE_CONFIRM );
        }
      }
    }
    else
    if( m_seq == SEQ_MESSAGE_NOT_MONEY )
    {
      //  金額足りない
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnFinish )
      {
        _SetSeq( SEQ_ITEM_SELECT );
      }
    }
    else
    if( m_seq == SEQ_CHANGE_CONFIRM )
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

        exitFrame( RESULT_CODE_RESULT_BACK );
      }
      else
      /**<  Yes/NoモードでNoが選択された                                */
      if( m_messageMenuEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
      {
        exitFrame( RESULT_CODE_RESULT_BACK );
      }
    }
  }

  //  シーケンス設定
  void ColorDressUpFrame::_SetSeq( const Seq newSeq )
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Kisekae::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    App::Kisekae::View::LowerItemSelectView*  pItemSelectView = pViewObjects->GetLowerItemSelectView();
    GFL_ASSERT( pItemSelectView );

    NetAppLib::UI::NetAppMessageMenu* pMessageMenu  = pViewObjects->GetMessageMenu();
    GFL_ASSERT( pMessageMenu );
    //  メッセージメニューの初期設定
    {
      GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      GFL_ASSERT( pGameManager );

      Savedata::ConfigSave* pConfigSaveData = pGameManager->GetGameData()->GetConfig();
      GFL_ASSERT( pConfigSaveData );

      pMessageMenu->SetEventHandler( NULL );
      pMessageMenu->SetMessageSpeed( pConfigSaveData->GetTalkingSpeed() );
      pMessageMenu->SetMessageAutoFlg( false );
      m_messageMenuEventCode  = App::Tool::MessageMenuView::IEventHandler::EVENT_;
    }

    //  必ずはずす
    {
      pMessageMenu->RemoveFromSuperView();
    }

    m_seq = newSeq;

    if( m_seq == SEQ_ITEM_SELECT )
    {
      //  アイテム選択

      //  下画面開始
      {
        pItemSelectView->SetVisible( true );
        pItemSelectView->StartIdling();
      }
    }
    else
    if( m_seq == SEQ_SETTING_COLOR_CONFIRM )
    {
      //  指定したアイテムを色染めするか

      App::Kisekae::System::ItemList* pItemList  = m_pWork->GetItemList();
      App::Kisekae::System::ItemDataBase* pItemDataBase = pItemList->GetData( pItemSelectView->GetPushItemSelectIdx() );

      App::Tool::MessageMenuView* pMessageMenuView  = pMessageMenu;

      gfl2::str::StrBuf*  pMesssgeStrBuf  = m_pWork->GetTmpStrBuf( 0 );

      //  メッセージ
      gfl2::str::StrBuf& rStrBuf = m_pWork->GetMessageUtility()->GetString( GARC_message_kisekae_DAT, kisekae_11_01 );

      //  文字列作成
      {
        print::WordSet*  pWordSet  = m_pWork->GetMessageUtility()->GetWordSet();

        // expand
        //  きせかえの購入個数は常に１つなので第３引数の購入個数値は１でよい
        pWordSet->RegisterKisekaeItemName( 0, pItemDataBase->GetItemTxtID(), 1 );
        pWordSet->Expand( pMesssgeStrBuf, &rStrBuf );
      }

      pMessageMenu->SetEventHandler( this );

      pMessageMenu->SetYNListMode( true );
      pMessageMenu->SetFinishMode_None();

      pMessageMenuView->ShowMessageWithListMenu(
        pMesssgeStrBuf,
        false,
        *m_pWork->GetMessageUtility()->GetData( GARC_message_kisekae_DAT ),
        select_yes,
        select_no );

      pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
    else
    if( m_seq == SEQ_USE_MONEY_CONFIRM )
    {
      //  金銭確認
        App::Tool::MessageMenuView* pMessageMenuView  = pMessageMenu;

        gfl2::str::StrBuf*  pTmpStrBuf  = m_pWork->GetTmpStrBuf( 0 );

        App::Kisekae::System::ItemDataBase* pItemData = m_pWork->GetItemList()->GetData( pItemSelectView->GetPushItemSelectIdx() );

        //  メッセージ
        gfl2::str::StrBuf& rStrBuf = m_pWork->GetMessageUtility()->GetString( GARC_message_kisekae_DAT, kisekae_11_02 );
        //  文字列作成
        {
          print::WordSet*  pWordSet  = m_pWork->GetMessageUtility()->GetWordSet();
          // expand
          pWordSet->RegisterNumber( 0, pItemData->GetMoney(), Savedata::Misc::MY_GOLD_KETA, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
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

        pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
    else
    if( m_seq == SEQ_SET_COLOR )
    {
      //  購入SE
      Sound::PlaySE( SEQ_SE_SYS_006 );

      //  そめる
      pMessageMenu->SetEventHandler( this );
      pMessageMenu->SetYNListMode( false );

      //  メッセージの自動処理設定
      pMessageMenu->SetMessageSpeed( Savedata::ConfigSave::MSG_NORMAL );
      pMessageMenu->SetMessageAutoFlg( false );
      pMessageMenu->ShowMessage( GARC_message_kisekae_DAT, kisekae_11_03, false, false, true );

      pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
    else
    if( m_seq == SEQ_SET_COLOR_COMPLETE )
    {
       //  きせかえ完了SE
       Sound::PlaySE( SEQ_SE_COMPLETE1 );

       pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
    else
    if( m_seq == SEQ_MESSAGE_NOT_MONEY )
    {
      // フェスコインが足りない
      pMessageMenu->SetEventHandler( this );
      pMessageMenu->SetYNListMode( false );
      pMessageMenu->ShowMessage( GARC_message_kisekae_DAT, kisekae_11_05, false, false, true );
      pMessageMenu->SetFinishMode_UserInput();

      pViewList->AddLowerViewInputEnable( pMessageMenu );
    }
    else
    if( m_seq == SEQ_CHANGE_CONFIRM )
    {
      //  終了時の購入アイテムの着せ替え確認
      poke_3d::model::DressUpParam nowEquipDressUpParam;
      {
        Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
        nowEquipDressUpParam = pMyStatus->GetDressup();
      }

      bool  bChgDressUpParam  = Savedata::Fashion::IsChangeDressUpParam( m_newDressUpParam, nowEquipDressUpParam );

      //  装備変更している場合は反映させるかチェック
      if( bChgDressUpParam == true )
      {
        pMessageMenu->SetEventHandler( this );
        pMessageMenu->SetYNListMode( true );
        pMessageMenu->SetFinishMode_None();

        pMessageMenu->ShowMessageWithListMenu(
          GARC_message_kisekae_DAT,
          kisekae_11_04,
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
  }

  void  ColorDressUpFrame::MessageMenuView_OnEvent(const EventCode event)
  {
    m_messageMenuEventCode  = event;
  }

  //  選択項目にカーソルが設定された時に呼ばれる
  void ColorDressUpFrame::LowerItemSelectViewDelegate_OnCursorItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView )
  {
    poke_3d::model::DressUpParam  nowDressUpParam = m_newDressUpParam;

    if( pItemData == NULL )
    {
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

  //  項目内のアイテムはきせかえ可能なのものか
  bool ColorDressUpFrame::LowerItemSelectViewDelegate_OnIsDressupOk( App::Kisekae::System::ItemDataBase* pItemData )
  {
    return  true;
  }

  //  項目更新時に呼ばれる
  void ColorDressUpFrame::LowerItemSelectViewDelegate_OnUpdateItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView )
  {
    if( pItemData == NULL )
    {
      return;
    }

    if( pItemView == NULL )
    {
      return;
    }
  }

  //  キー入力
  void ColorDressUpFrame::LowerItemSelectViewDelegate_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
    GFL_ASSERT( pCharaModelViewr );

    pCharaModelViewr->NoticeKeyAction( pButton, pKey, pStick );
  }

  //  きせかえ終了通知
  void ColorDressUpFrame::CharaModelViewer_OnDressupEnd()
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
