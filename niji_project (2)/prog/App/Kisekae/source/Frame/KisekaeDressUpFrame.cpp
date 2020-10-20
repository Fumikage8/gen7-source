// ============================================================================
/*
 * @file KisekaeDressUpFrame.h
 * @brief きせかえアプリの着せ替えフレーム
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/Frame/KisekaeDressUpFrame.h"
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

  DressUpFrame::DressUpFrame(
    App::Kisekae::System::Work* pWork ) :
    NetAppLib::System::NetAppFrameBase( pWork )
  {
    _Clear();

    m_pWork = pWork;
  }

  DressUpFrame::~DressUpFrame()
  {
  }

  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  bool DressUpFrame::startup()
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

      // @fix 大田哲司さん要望　回転入力イベントにする
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

      //  金額表示を消す
      pBGView->HideMoneyWindow();

      //  ユーザー操作チュートリアル表示
      pBGView->SetVisibleUserInputTutorialIcon( true );
    }

    //  下Viewの構築
    {
      App::Kisekae::View::LowerItemSelectView*  pItemSelectView = pViewObjects->GetLowerItemSelectView();
      GFL_ASSERT( pItemSelectView );

      pViewList->AddLowerView( pItemSelectView );

      pItemSelectView->SetDelgate( this );

      // @fix 大田哲司さん要望  下のメッセージを消して「きがえる」ボタンを初回は押せないようにする
      {
        pItemSelectView->HideMessageLower();
        pItemSelectView->SetActionButton( App::Kisekae::View::LowerItemSelectView::BUTTON_ID_DRESS_UP, false );
      }

      //  初回リスト設定
      pItemSelectView->CreateList( m_pWork->GetItemList(), 1, App::Kisekae::View::LowerItemSelectView::MODE_DRESS_UP, false, true );

      pItemSelectView->StartEvent();
    }

    //  現在カレントになっているカテゴリで装備しているアイテム名の更新
    {
      App::Kisekae::System::ItemList* pItemList  = m_pWork->GetItemList();
      _UpdateEquipItemInfo( pItemList->GetActiveCategory() );
    }

    _SetSeq( SEQ_ITEM_SELECT );

    return true;
  }

  bool DressUpFrame::cleanup()
  {
    m_pWork->GetCharaModelViewer()->StartIdleingEvent();

    return true;
  }

  void DressUpFrame::setListener()
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    //  下画面入力
    {
      App::Kisekae::View::LowerItemSelectView*  pSelectView = pViewObjects->GetLowerItemSelectView();
      App::Kisekae::View::LowerInfoView*        pInfoView = pViewObjects->GetLowerInfoView();

      pSelectView->SetInputEnabledAllView( true );
      pInfoView->SetDelegate( this );
    }
  }

  void DressUpFrame::removeListener()
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

    App::Kisekae::View::LowerInfoView*        pInfoView = pViewObjects->GetLowerInfoView();
    pInfoView->SetDelegate( NULL );

    {
      app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
      GFL_ASSERT( pCharaModelViewr );

      pCharaModelViewr->SetDelegate( NULL );
    }
  }

  void DressUpFrame::updateSequence()
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Kisekae::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    App::Kisekae::View::LowerItemSelectView*  pLowerItemSelectView  = pViewObjects->GetLowerItemSelectView();
    GFL_ASSERT( pLowerItemSelectView );

    App::Kisekae::View::LowerInfoView*  pLowerInfoView  = pViewObjects->GetLowerInfoView();
    GFL_ASSERT( pLowerInfoView );

    App::Kisekae::View::UpperBGView*  pBGView = pViewObjects->GetUpperBGView();
    GFL_ASSERT( pBGView );

    if( m_seq == SEQ_ITEM_SELECT )
    {
      //  // @fix NMCat[1484] UIの表示設定と３Ｄのカメラ設定との同期を取るためにＵＩ表示設定を１フレーム遅らせた
      {
        //  「すがたをみる」のViewを外す
        pLowerInfoView->RemoveFromSuperView();

        pLowerItemSelectView->SetVisible( true );
        pBGView->SetItemNameViaible( true );
        pBGView->SetVisibleUserInputTutorialIcon( true );
      }

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
        _SetSeq( SEQ_MODEL_SHOW );
      }
      //  きがえる
      else
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_DRESS_UP )
      {
        if( _IsChangeDressup() == true )
        {
          m_pWork->OnDressup();
        }

        APP_PARAM* appParam = m_pWork->GetAppParam();

        if( appParam->runType != EVENT_RUN_TYPE_FINDER_STUDIO )
        {
          //  MyStatusの着せ替えを反映
          {
            //  プレイヤーのトレーナー情報を取得してモデル表示
            Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
            pMyStatus->SetDressup( m_newDressUpParam );
          }
        }
        else {
          // 着せ替え情報外部に出力
          appParam->output.dressUpParam = m_newDressUpParam;
        }

        exitFrame( RESULT_CODE_RESULT_BACK );
      }
      //  アイテムを押した
      else
      if( pushButtonId == App::Kisekae::View::LowerItemSelectView::BUTTON_ID_ITEM )
      {
        //  きせかえをする
        App::Kisekae::System::ItemDataBase* pItemData = m_pWork->GetItemList()->GetData( pLowerItemSelectView->GetPushItemSelectIdx() );

        // @fix 大田哲司さん要望  アイテムを選択すると「きがえる」ボタンを押せるようにする
        {
          pLowerItemSelectView->SetActionButton( App::Kisekae::View::LowerItemSelectView::BUTTON_ID_DRESS_UP, true );
        }

        {
          app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
          GFL_ASSERT( pCharaModelViewr );

          //  セーブデータが知っているカテゴリーとモデル側で知っているカテゴリーが異なるので変換する
          const poke_3d::model::DressUpParam::ItemCategory  dressUpCategory = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( pItemData->GetCategory() );

          const poke_3d::model::DressUpParam* pDefaultDressUpParam = m_pWork->GetItemList()->GetDefaultIndispensableDressUpParam();
          GFL_ASSERT( pDefaultDressUpParam );

          //  最新きせかえパラメータに書き込み
          if( m_newDressUpParam.itemParams[ dressUpCategory ] != pItemData->GetItemId() )
          {
            m_newDressUpParam.itemParams[ dressUpCategory ]  = pItemData->GetItemId();
          }
          else
          {
            //  すでに装備している着せ替えをはずすして、デフォルトのに差し替え
            m_newDressUpParam.itemParams[ dressUpCategory ] = pDefaultDressUpParam->itemParams[ dressUpCategory ];
          }

          if( pCharaModelViewr->StartDressupEvent( m_newDressUpParam ) == true )
          {
            //  モデルロードが入るので
            //  モデルロード中はすがたをみせるが押せない
            pLowerItemSelectView->SetActionButton( App::Kisekae::View::LowerItemSelectView::BUTTON_ID_VIEWR, false );

            //  きせかえに変化あったので
            //  リストを更新してチェックマークをつける
            pLowerItemSelectView->Refresh();

            _UpdateEquipItemInfo( pItemData->GetCategory() );
          }
        }

        //  元の選択に戻す
        pLowerItemSelectView->StartIdling();
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
          //  カテゴリーが切り替わった

          {
            //  現在装備しているきせかえ名を表示
            _UpdateEquipItemInfo( category );
          }

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

            pLowerItemSelectView->CreateList( pItemList, 1, App::Kisekae::View::LowerItemSelectView::MODE_DRESS_UP, false, false );
          }
        }
      }
    }
    else
    if( m_seq == SEQ_MODEL_SHOW )
    {
      //  // @fix NMCat[1484] UIの表示設定と３Ｄのカメラ設定との同期を取るためにＵＩ表示設定を１フレーム遅らせた
      {
        pBGView->SetItemNameViaible( false );
        pBGView->SetVisibleUserInputTutorialIcon( false );

        //  選択リスト画面は非表示(Bボタンキャンセルがかぶっていてアニメする時にかぶっているボタンがあるせいで見た目がだめ)
        pLowerItemSelectView->SetVisible( false );

        if( pLowerInfoView->GetSuperView() == NULL )
        {
          pViewList->AddLowerViewInputEnable( pLowerInfoView );
        }
      }

      App::Kisekae::View::LowerInfoView::ButtonId pushId  = pLowerInfoView->GetPushButtonId();
      if( pushId == App::Kisekae::View::LowerInfoView::BUTTON_ID_CANCEL )
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
        APP_PARAM* appParam = m_pWork->GetAppParam();

        if( appParam->runType != EVENT_RUN_TYPE_FINDER_STUDIO )
        {
          //  MyStatusの着せ替えを反映
          {
            //  プレイヤーのトレーナー情報を取得してモデル表示
            Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
            pMyStatus->SetDressup( m_newDressUpParam );
          }
        }
        else {
          // 着せ替え情報外部に出力
          appParam->output.dressUpParam = m_newDressUpParam;
        }

        //  きせかえしたフラグを立てる
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
  }

  //  シーケンス設定
  void DressUpFrame::_SetSeq( const Seq newSeq )
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Kisekae::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    App::Kisekae::View::LowerInfoView*  pLowerInfoView  = pViewObjects->GetLowerInfoView();
    GFL_ASSERT( pLowerInfoView );

    App::Kisekae::View::LowerItemSelectView*  pItemSelectView = pViewObjects->GetLowerItemSelectView();
    GFL_ASSERT( pItemSelectView );

    NetAppLib::UI::NetAppMessageMenu* pMessageMenu  = pViewObjects->GetMessageMenu();
    GFL_ASSERT( pMessageMenu );
    pMessageMenu->SetEventHandler( NULL );
    m_messageMenuEventCode  = App::Tool::MessageMenuView::IEventHandler::EVENT_;

    App::Kisekae::View::UpperBGView*  pBGView = pViewObjects->GetUpperBGView();
    GFL_ASSERT( pBGView );

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
        pItemSelectView->StartIdling();
      }

      //  上画面のモデルビューアー環境を回転入力イベントに再設定
      {
        app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
        GFL_ASSERT( pCharaModelViewr );
        
        pCharaModelViewr->StartRotYInputEvent();
      }
    }
    else
    if( m_seq == SEQ_MODEL_SHOW )
    {
      app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
      GFL_ASSERT( pCharaModelViewr );

      //  ショーイベント開始
      pCharaModelViewr->StartShowEvent();

      pLowerInfoView->StartIdling();
    }
    else
    if( m_seq == SEQ_CHANGE_CONFIRM )
    {
      //  終了時の購入アイテムの着せ替え確認

      //  装備変更している場合は反映させるかチェック
      if( _IsChangeDressup() == true )
      {
        pMessageMenu->SetEventHandler( this );
        pMessageMenu->SetYNListMode( true );
        pMessageMenu->SetFinishMode_None();

        pMessageMenu->ShowMessageWithListMenu(
          GARC_message_kisekae_DAT,
          kisekae_03_03,
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

  void  DressUpFrame::MessageMenuView_OnEvent(const EventCode event)
  {
    m_messageMenuEventCode  = event;
  }

  //  指定したカテゴリーで装備しているきせかえ情報を更新
  void DressUpFrame::_UpdateEquipItemInfo( const Savedata::Fashion::Category cateogyr )
  {
    App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Kisekae::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );
    
    App::Kisekae::View::UpperBGView*  pBGView = pViewObjects->GetUpperBGView();
    GFL_ASSERT( pBGView );

    //  セーブ内のカテゴリーからキャラきせかえで定義しているカテゴリー値に変換
    poke_3d::model::DressUpParam::ItemCategory  dressUpParamCategory  = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( cateogyr );

    //  きせかえデータベースからきせかえデータを取得
    const table_data_GoodsData* pGoodsData  = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( 
                                                              (poke_3d::model::DressUpParam::Sex)m_newDressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ], 
                                                              (Savedata::Fashion::Category)cateogyr,
                                                              m_newDressUpParam.itemParams[ dressUpParamCategory ] );

    if( pGoodsData == NULL )
    {
      // @fix 守屋さん要望　着せ替え画面中にカーテン表示するために今まで表示フレームを非表示設定しているところをテキストのみ非表示するように変更
      pBGView->SetItemName( -1, -1 );
      return;
    }

    s32 itemTxtId   = pGoodsData->goods_txt_id;
    s32 colorTxtId  = pGoodsData->color_txt_id;

    pBGView->SetItemName( itemTxtId, colorTxtId );
    pBGView->SetItemNameViaible( true );
  }

  //  現在装備しているきせかえが変わっているいるか
  bool DressUpFrame::_IsChangeDressup()
  {
    //  終了時の購入アイテムの着せ替え確認
    poke_3d::model::DressUpParam nowEquipDressUpParam;
    {
      APP_PARAM* appParam = m_pWork->GetAppParam();

      if( appParam->runType != EVENT_RUN_TYPE_FINDER_STUDIO )
      {
        /// ファインダースタジオイベント以外は現在の衣装
        Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
        nowEquipDressUpParam = pMyStatus->GetDressup();
      }
      else {
        /// ファインダースタジオイベントの場合は着せ替え前の衣装
        nowEquipDressUpParam = appParam->inputDressUpParam;
      }
    }

    return  Savedata::Fashion::IsChangeDressUpParam( m_newDressUpParam, nowEquipDressUpParam );
  }

  //  選択項目にカーソルが設定された時に呼ばれる
  void DressUpFrame::LowerItemSelectViewDelegate_OnCursorItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView )
  {
    if( pItemData == NULL )
    {
      return;
    }

    if( pItemView == NULL )
    {
      return;
    }

    //  newマークを消すカウントを設定
    m_newFlgDownCount = 2;

    {
      pItemView->SetEnableNewMark( pItemData->IsNew() );
    }
  }

  //  選択項目にカーソルがヒットし続けている間はずっと呼ばれる
  void DressUpFrame::LowerItemSelectViewDelegate_OnCursorHitItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView )
  {
    if( pItemData == NULL )
    {
      return;
    }

    if( pItemView == NULL )
    {
      return;
    }

    if( 0 < m_newFlgDownCount )
    {
      --m_newFlgDownCount;

      if( m_newFlgDownCount <= 0 )
      {
        pItemData->Alrady();
        pItemView->SetEnableNewMark( pItemData->IsNew() );
      }
    }
  }

  //  項目内のアイテムはきせかえ可能なのものか
  bool DressUpFrame::LowerItemSelectViewDelegate_OnIsDressupOk( App::Kisekae::System::ItemDataBase* pItemData )
  {
    if( pItemData == NULL )
    {
      return  false;
    }

    app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();

    poke_3d::model::DressUpParam  chkDressUpParam;
    m_pWork->OutputChkDressUpParam( &chkDressUpParam, m_newDressUpParam, pItemData );
    bool  bFlg  = pCharaModelViewr->CheckDressUpCombination( chkDressUpParam );

    return  bFlg;
  }

  //  選択項目を押した
  bool DressUpFrame::LowerItemSelectViewDelegate_OnPushItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView )
  {
    if( pItemView->IsSelectActive() == false )
    {
      return  false;
    }

    return  true;
  }

  //  項目更新時に呼ばれる
  void DressUpFrame::LowerItemSelectViewDelegate_OnUpdateItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView )
  {
    if( pItemView == NULL )
    {
      return;
    }

    pItemView->SetEnableChkMark( false );

    if( pItemData == NULL )
    {
      return;
    }

    //  現在モデルが着ているドレスと同じならチェックマークをつける
    {
      poke_3d::model::DressUpParam::ItemCategory  dressUpParamCategory  = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( (Savedata::Fashion::Category)pItemData->GetCategory() );
      if( m_newDressUpParam.itemParams[ dressUpParamCategory ] == (s16)pItemData->GetItemId() )
      {
        //  マークをつける
        pItemView->SetEnableChkMark( true );
      }
      else
      {
        pItemView->SetEnableChkMark( false );
      }
    }
  }

  //  キー入力取得
  bool DressUpFrame::LowerInfoViewDelegata_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
    GFL_ASSERT( pCharaModelViewr );

    return  pCharaModelViewr->NoticeKeyAction( pButton, pKey, pStick );
  }

  //  キー入力
  void DressUpFrame::LowerItemSelectViewDelegate_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
    GFL_ASSERT( pCharaModelViewr );

    pCharaModelViewr->NoticeKeyAction( pButton, pKey, pStick );
  }

  //  きせかえ終了通知
  void DressUpFrame::CharaModelViewer_OnDressupEnd()
  {
    //  キャラの着せ替え終了
    app::tool::CharaModelViewer*  pCharaModelViewr  = m_pWork->GetCharaModelViewer();
    GFL_ASSERT( pCharaModelViewr );

    //  すがたを見るボタンを押せる
    {
      App::Kisekae::System::ViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
      GFL_ASSERT( pViewSysAccessor != NULL );

      App::Kisekae::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
      GFL_ASSERT( pViewObjects );

      App::Kisekae::View::LowerItemSelectView*  pLowerItemSelectView  = pViewObjects->GetLowerItemSelectView();
      GFL_ASSERT( pLowerItemSelectView );

      pLowerItemSelectView->SetActionButton( App::Kisekae::View::LowerItemSelectView::BUTTON_ID_VIEWR, true );
    }

    //  回転入力イベント再開
    pCharaModelViewr->StartRotYInputEvent();
  }

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
