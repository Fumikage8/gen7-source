//======================================================================
/**
 * @file BattleVideoPlayerLowDataListView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの下画面データリストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerLowDataListView.h"

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_anim_list.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_pane.h"

#include <Sound/include/Sound.h>

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  LowDataListView::LowDataListView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  LowDataListView::~LowDataListView()
  {
    if( m_pPaneListView != NULL )
    {
      m_pPaneListView->RemoveFromSuperView();
      m_pPaneListView->DeleteScrollBar();
      m_pPaneListView->DeletePaneList();
    }

    GFL_SAFE_DELETE( m_pPaneListView );
    GFL_SAFE_DELETE( m_pPaneListViewHeap );
    GFL_SAFE_DELETE( m_pMenuCursor );
    GFL_SAFE_DELETE_ARRAY( m_pPaneListData );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool LowDataListView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBinData, m_initParam.pAppMsgData, m_initParam.pWordSet );

      gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
      app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
      gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      //  カーソル作成
      //  すでにバイナリを設定しているのでロードは不要
      {
        m_pMenuCursor = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) app::tool::MenuCursor( m_initParam.pHeap, &m_initParam.pMenuCursorBinData );
        m_pMenuCursor->CreateLayoutWork( UIView::GetLayoutSystem(), UIView::GetLytSetup( gfl2::lyt::DISPLAY_LOWER ), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_SMALL );

        //  最初から再生
        m_pMenuCursor->StartAnime();
      }

      //  PaneListViewを作成
      {
        //  PaneLisstView専用のヒープを作成
        //  なぜなら何度も読み変えるから
        {
          m_pPaneListViewHeap = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) app::util::Heap();
          m_pPaneListViewHeap->LocalHeapCreate( m_initParam.pHeap->GetSystemHeap(), m_initParam.pHeap->GetDeviceHeap(), 1024 * 100, 1024 * 500 );
        }

        m_pPaneListView = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) app::tool::PaneListView( m_pPaneListViewHeap, true );
      }

      // -----------------------------------------------------------------------
      { // m_pPaneListData 初期化 ここから

        struct SETUP_ITEM_DATA
        {
          u32 partsPaneId;
          u32 holdAnimeId;
          u32 releaseAnimeId;
          u32 cancelAnimeId;
          u32 selectAnimeId;
        };

        const SETUP_ITEM_DATA aSetupItemData[ LIST_ITEM_PANE_MAX ]  =
        {
          //  menu01
          {
            PANENAME_BVP_MIN_LOW_002_PANE_MENU_01,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_01_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_01_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_01_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_01_TOUCH_RELEASE
          },

          //  menu02
          {
            PANENAME_BVP_MIN_LOW_002_PANE_MENU_02,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_02_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_02_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_02_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_02_TOUCH_RELEASE
          },

          //  menu03
          {
            PANENAME_BVP_MIN_LOW_002_PANE_MENU_03,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_03_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_03_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_03_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_03_TOUCH_RELEASE
          },

          //  menu04
          {
            PANENAME_BVP_MIN_LOW_002_PANE_MENU_04,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_04_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_04_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_04_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_04_TOUCH_RELEASE
          },

          //  menu05
          {
            PANENAME_BVP_MIN_LOW_002_PANE_MENU_05,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_05_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_05_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_05_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_05_TOUCH_RELEASE
          },

          //  menu06
          {
            PANENAME_BVP_MIN_LOW_002_PANE_MENU_06,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_06_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_06_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_06_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_06_TOUCH_RELEASE
          },

          //  menu07
          {
            PANENAME_BVP_MIN_LOW_002_PANE_MENU_07,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_07_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_07_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_07_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_07_TOUCH_RELEASE
          },

          //  menu08
          {
            PANENAME_BVP_MIN_LOW_002_PANE_MENU_08,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_08_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_08_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_08_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__MENU_08_TOUCH_RELEASE
          }
        };

        m_pPaneListData = GFL_NEW_LOW_ARRAY( m_initParam.pHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];

        for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
        {
          const SETUP_ITEM_DATA*  pSetupItemData  = &aSetupItemData[ i ];

          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );

          paneHelper.Push( pSetupItemData->partsPaneId );

          //  paneList用のデータ
          {
            m_pPaneListData[i].base       = paneHelper.Peek();
            m_pPaneListData[i].text       = paneHelper.GetTextBoxPane( PANENAME_BVP_BTN_LOW_002_PANE_TEXTBOX_00 );
            m_pPaneListData[i].bound      = paneHelper.GetBoundingPane( PANENAME_BVP_BTN_LOW_002_PANE_BOUND_00 );
            m_pPaneListData[i].cursor_pos = paneHelper.GetPane( PANENAME_BVP_BTN_LOW_002_PANE_NULL_CUR );

            m_pPaneListData[i].holdAnime    = pSetupItemData->holdAnimeId;
            m_pPaneListData[i].releaseAnime = pSetupItemData->releaseAnimeId;
            m_pPaneListData[i].cancelAnime  = pSetupItemData->cancelAnimeId;
            m_pPaneListData[i].selectAnime  = pSetupItemData->selectAnimeId;
          }

          //  選択項目個別のデータ
          {
            m_aSelectItem[ i ].pChkMarkBGPane   = paneHelper.GetPane( PANENAME_BVP_BTN_LOW_002_PANE_CHECKMARK_BG_00 );
            GFL_ASSERT( m_aSelectItem[ i ].pChkMarkBGPane );

            m_aSelectItem[ i ].pChkMarkPane     = paneHelper.GetPane( PANENAME_BVP_BTN_LOW_002_PANE_CHECKMARK_CHECK_00 );
            GFL_ASSERT( m_aSelectItem[ i ].pChkMarkPane );

            m_aSelectItem[ i ].pUploadMarkPane     = paneHelper.GetPane( PANENAME_BVP_BTN_LOW_002_PANE_BVP_BUTTONICON_00 );
            GFL_ASSERT( m_aSelectItem[ i ].pUploadMarkPane );
          }
        }
      } // m_pPaneListData 初期化 ここまで
      // -----------------------------------------------------------------------

      //  ボタン作成
      {
        gfl2::lyt::LytParts*  pChangeBtnPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_002_PANE_BTN_01 );
        GFL_ASSERT( pChangeBtnPartsPane );

        gfl2::lyt::LytParts*  pDelBtnPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_002_PANE_BTN_02 );
        GFL_ASSERT( pDelBtnPartsPane );

        const app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
        {
        /*
          app::tool::ButtonManager::ButtonId button_id;
          gfl2::lyt::LytPane* picture_pane;
          gfl2::lyt::LytPane* bound_pane;
          u32                 touch_anime_index;        //!< タッチ時のアニメ
          u32                 release_anime_index;      //!< タッチ決定時のアニメ
          u32                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
          u32                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
          u32                 active_anime_index;       //!< アクティブアニメ
          u32                 passive_anime_index;      //!< パッシブアニメ
        */
          //  情報切り替えボタン
          {
            BUTTON_ID_DATA_CHANGE,
            pChangeBtnPartsPane,
            pLayoutWork->GetBoundingPane( pChangeBtnPartsPane, PANENAME_BVP_BTN_LOW_000_PANE_BOUND_00, &rMultiResId ),
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_TOUCH_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_ACTIVE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_PASSIVE,
          },

          //  削除ボタン
          {
            BUTTON_ID_DATA_DEL,
            pDelBtnPartsPane,
            pLayoutWork->GetBoundingPane( pDelBtnPartsPane, PANENAME_BVP_BTN_LOW_001_PANE_BOUND_00, &rMultiResId ),
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_02_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_02_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_02_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_02_TOUCH_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_02_ACTIVE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_02_PASSIVE,
          },
        };

        UIResponder::CreateButtonManager( m_initParam.pHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
        //  入力コールバック設定
        UIView::SetInputListener( this );

        //  ボタンSEの設定
        {
          app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
          pButtonManager->SetButtonSelectSE( BUTTON_ID_DATA_CHANGE, SEQ_SE_DECIDE1 );
          pButtonManager->SetButtonSelectSE( BUTTON_ID_DATA_DEL, SEQ_SE_DECIDE1 );
        }
      }

      //  テキスト設定のペイン取得
      {
        //  リスト選択のテキスト
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          m_pSelectListNumTextPane  = paneHelper.GetTextBoxPane( PANENAME_BVP_MIN_LOW_002_PANE_TEXTBOX_00 );
        }

        //  リスト選択の数字との間のテキスト
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          m_pListNumSlashTextPane = paneHelper.GetTextBoxPane( PANENAME_BVP_MIN_LOW_002_PANE_TEXTBOX_02 );
          pG2DUtil->SetTextBoxPaneString( m_pListNumSlashTextPane, msg_bvp_sys_low_01 );
        }

        //  リスト最大数のテキスト
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          m_pListMaxNumTextPane = paneHelper.GetTextBoxPane( PANENAME_BVP_MIN_LOW_002_PANE_TEXTBOX_01 );
        }

        //  Yボタンテキスト
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          paneHelper.Push( PANENAME_BVP_MIN_LOW_002_PANE_BTN_02 );

          m_pYBtnTextPane = paneHelper.GetTextBoxPane( PANENAME_BVP_BTN_LOW_001_PANE_TEXTBOX_00 );
        }

        //  Xボタンテキスト
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          paneHelper.Push( PANENAME_BVP_MIN_LOW_002_PANE_BTN_01 );

          m_pXBtnTextPane = paneHelper.GetTextBoxPane( PANENAME_BVP_BTN_LOW_000_PANE_TEXTBOX_00 );

          pG2DUtil->SetTextBoxPaneString( m_pXBtnTextPane, msg_bvp_sys_low_17 );
        }
      }

      //  初期化設定でリスト数０の指定をする
      {
        LIST_SCROLL_DATA  dummyScrollData;
        SetListData( NULL, dummyScrollData );
      }

      //  各モードの初期指定
      SetDelMode( DEL_MODE_NONE );
      SetInfoMode( INFO_MODE_DETAIL );

      ++m_initSeq;
    }

    if( m_initSeq == 1 )
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  bool LowDataListView::EndFunc()
  {
    bool  bRet  = true;
    if( m_pMenuCursor->DeleteLayoutWork() == false )
    {
      bRet  = false;
    }

    if( UIView::IsDrawing() == true )
    {
      bRet  = false;
    }

    return  bRet;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowDataListView::Update()
  {
    if( m_state == STATE_IDLE )
    {
    }

    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      if( ( UIView::IsButtonAnimation() == false )
      &&  ( m_pPaneListView->IsButtonAnimation() == false ) )
      {
        SetState( STATE_INPUT_STOP );
      }
    }

    if( m_state == STATE_INPUT_STOP )
    {
    }

    UIView::Update();

    m_pMenuCursor->Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowDataListView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderManager, displayNo, i );
      }

      //  カーソル描画
      m_pMenuCursor->Draw( m_initParam.pRenderManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void LowDataListView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
      m_pushButtonId  = BUTTON_ID_NONE;

      if( UIView::GetSuperView() == NULL )
      {
        UIView::SetInputEnabledAllView( true );
      }
      else
      {
        UIView::GetSuperView()->SetInputEnabledAllView( true );
      }
    }
    else
    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      if( UIView::GetSuperView() == NULL )
      {
        UIView::SetInputEnabledAllView( false );
      }
      else
      {
        UIView::GetSuperView()->SetInputEnabledAllView( false );
      }
    }
    else
    if( m_state == STATE_INPUT_STOP )
    {
      
    }
  }
  
  //------------------------------------------------------------------
  /**
    * @brief   削除モード設定
    */
  //------------------------------------------------------------------
  void LowDataListView::SetDelMode( const DelMode newMode )
  {
    m_delMode  = newMode;

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    //  通常モード
    if( m_delMode == DEL_MODE_NONE )
    {
      //  Yボタンのテキスト設定
      pG2DUtil->SetTextBoxPaneString( m_pYBtnTextPane, msg_bvp_sys_low_15 );

      //  すべての項目にチェックマークをはずす
      {
        for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
        {
          pG2DUtil->SetPaneVisible( m_aSelectItem[ i ].pChkMarkBGPane, false );
          pG2DUtil->SetPaneVisible( m_aSelectItem[ i ].pChkMarkPane, false );
          pG2DUtil->SetPaneVisible( m_aSelectItem[ i ].pUploadMarkPane, m_aSelectItem[ i ].bUploadMarkFlg );
        }
      }

      UIView::GetButtonManager()->SetButtonActive( BUTTON_ID_DATA_DEL );
    }
    else
    if( m_delMode == DEL_MODE_SELECT )
    {
      //  Yボタンのテキスト設定
      pG2DUtil->SetTextBoxPaneString( m_pYBtnTextPane, msg_bvp_sys_low_16 );

      if( m_pVideoDataListManager != NULL )
      {
        //  削除マークをすべてクリア
        u32 loopNum = m_pVideoDataListManager->GetListDataNum();
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pVideoDataListManager->SetDataDelFlg( i, false );
        }
      }

      //  すべての項目にチェックマークをつける準備
      {
        for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
        {
          pG2DUtil->SetPaneVisible( m_aSelectItem[ i ].pChkMarkBGPane, true );
          pG2DUtil->SetPaneVisible( m_aSelectItem[ i ].pChkMarkPane, false );
          pG2DUtil->SetPaneVisible( m_aSelectItem[ i ].pUploadMarkPane, false );
        }
      }

      //  削除マークが一つもついていないので削除ボタンを押せないようにする
      UIView::GetButtonManager()->SetButtonPassive( BUTTON_ID_DATA_DEL );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   情報モード設定
    */
  //------------------------------------------------------------------
  void LowDataListView::SetInfoMode( const InfoMode newMode )
  {
    m_infoMode  = newMode;

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_infoMode == INFO_MODE_DETAIL )
    {
      //  詳細モード

      // @fix GFNMCat[1096] 結果モード用の切り替えを無効にしないときれいに切り替わらない
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_ON_KEKKA, false );
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_OFF_KEKKA, true, false );
    }
    else
    if( m_infoMode == INFO_MODE_RESULT )
    {
      //  結果モード

      // @fix GFNMCat[1096] 詳細モード用の切り替えを無効にしないときれいに切り替わらない
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_OFF_KEKKA, false );
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002__BTN_01_ON_KEKKA, true, false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   リストデータ設定
    */
  //------------------------------------------------------------------
  void LowDataListView::SetListData( NetAppLib::BattleVideoPlayer::VideoDataManager* pLinkVideoDataManager, const LIST_SCROLL_DATA& rScrollData )
  {
    app::util::G2DUtil* pG2DUtil          = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    u32 dataNum = 0;
    //  参照したデータをコピーする(ポインターは危険！)
    if( pLinkVideoDataManager != NULL )
    {
      m_pVideoDataListManager  = pLinkVideoDataManager;
      dataNum = m_pVideoDataListManager->GetListDataNum();
    }

    //  ペインリストデータを作成
    app::tool::PaneList::SETUP_PARAM list_setup;
    {
      list_setup.heap           = m_pPaneListViewHeap->GetDeviceHeap();  // ヒープ

      list_setup.pLytWk         = UIView::GetLayoutWork( LYT_WORK_NORMAL );    // レイアウトワーク
      list_setup.paneList       = m_pPaneListData;              // ペインデータ
      list_setup.paneListMax    = LIST_ITEM_PANE_MAX;           // ペインデータ数

      list_setup.valMax         = dataNum;      // 実際に使用するリストの項目数（ペインの数ではない）
      list_setup.listPosMax     = 6;            // カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
      list_setup.listPos        = rScrollData.listPos;            // 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
      list_setup.listScroll     = rScrollData.listScroll;            // 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                              //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                              //   valMax-1になるようにする設定する。

      // スクロール値補正/カーソル位置補正
      app::tool::PaneList::CommonAdjustmentListScrollAndPos( &list_setup );

      list_setup.defPY          = 100.0f;    // ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )

      //   この座標で指定した場所にあるペインが描画開始位置のペインになる
      list_setup.defSX          = 238.0f;   // ペインのXサイズ(レイアウトデディターで厳密に確認）
      list_setup.defSY          = 34.0f;    // ペインのYサイズ(レイアウトデディターで厳密に確認）

      list_setup.writeOffsetY   = -1;      // 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

      list_setup.listStartPane  = NULL;   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

      list_setup.cbFunc         = this;   // コールバック関数
      list_setup.cbWork         = NULL;   // コールバック関数に渡すワーク

      list_setup.uiDevMan       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(); // UIデバイスマネージャ
      list_setup.menuCursor     = m_pMenuCursor;  // カーソル
    }

    //  スクロールデータを作成
    app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
    {
      // @note  以下、[set1]を設定
      m_pPaneListViewHeap->GetDeviceHeap(),
      app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
      list_setup.valMax - list_setup.listPosMax,

      pLayoutWork,
      rMultiResId,
      PANENAME_BVP_MIN_LOW_002_PANE_SCL_01,
      PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

      NULL,
      NULL,
      NULL,
      NULL,
    };
    
    //  スクロールバー作成
    {
      //  いったん削除
      m_pPaneListView->DeleteScrollBar();
      //  新しく作成
      m_pPaneListView->CreateScrollBar( &scroll_bar_setup );
    }

    //  リスト作成
    {
      //  リストの削除と生成を同時にやっている
      m_pPaneListView->ResetPaneList( &list_setup );

      m_pPaneListView->SetListener( this );
    }

    //  リストの初期設定
    app::tool::PaneList*  pPaneList = m_pPaneListView->GetPaneList();
    {
      // @fix NMCat[2144] リストをループする仕様になった
      pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_BUTTON, true, false, true, false );
      pPaneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );
    }

    AddSubView( m_pPaneListView );

    //  リスト数字設定
    {
      //  リスト現在値
      {
        u32 nowListPos  = pPaneList->GetItemPos() + 1;
        pG2DUtil->SetTextBoxPaneNumber( m_pSelectListNumTextPane, msg_bvp_sys_up_12, nowListPos, 3, 0, print::NUM_DISP_ZERO );
      }

      //  リスト最大値
      pG2DUtil->SetTextBoxPaneNumber( m_pListMaxNumTextPane, msg_bvp_sys_up_12, dataNum, 3, 0, print::NUM_DISP_ZERO );
    }

    if( NetAppLib::BattleVideoPlayer::VideoDataManager::VIDEO_DATA_MAX <= dataNum )
    {
      //  テキスト色を赤にする
      gfl2::lyt::ColorU8  redColor  = gfl2::lyt::ColorU8( 255, 0, 0, 255 );
      pG2DUtil->SetTextBoxPaneColor( m_pListNumSlashTextPane, redColor, redColor ); 
      pG2DUtil->SetTextBoxPaneColor( m_pSelectListNumTextPane, redColor, redColor ); 
      pG2DUtil->SetTextBoxPaneColor( m_pListMaxNumTextPane, redColor, redColor ); 
    }
    else
    {
      // @fix GFNMCat[1641] 赤にしてから白にするケースがぬけていた。
      gfl2::lyt::ColorU8  redColor  = gfl2::lyt::ColorU8( 255, 255, 255, 255 );
      pG2DUtil->SetTextBoxPaneColor( m_pListNumSlashTextPane, redColor, redColor ); 
      pG2DUtil->SetTextBoxPaneColor( m_pSelectListNumTextPane, redColor, redColor ); 
      pG2DUtil->SetTextBoxPaneColor( m_pListMaxNumTextPane, redColor, redColor ); 
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   リストのスクロールデータ取得
    */
  //------------------------------------------------------------------
  const LowDataListView::LIST_SCROLL_DATA LowDataListView::GetListScrollData()
  {
    LIST_SCROLL_DATA  data;
    m_pPaneListView->GetPaneList()->GetCursorData( &data.listPos, &data.listScroll );

    return  data;
  }

  void LowDataListView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
  {
    UCHIDA_PRINT( "CallBack_CursorMove DataList ->pos %d\n", pos );

    //  選択項目に合わせてデータ更新
    _UpdateData( pos );
  }

  void LowDataListView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
  {
  }

  void LowDataListView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
  {
    GFL_ASSERT( pane );
    
    UCHIDA_PRINT( "CallBack_ListWrite DataList ->pos %d\n", pos );

    NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData  = m_pVideoDataListManager->GetListData( pos );

    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    // @fix GFNMCat[5267] 選択項目用の名前に変更
    pG2DUtil->SetTextBoxPaneString( pane[ pane_index ].text, pVideoData->selectMsgId );

    gfl2::lyt::LytPane* pUploadMarkPane  = m_aSelectItem[ pane_index ].pUploadMarkPane;
    m_aSelectItem[ pane_index ].bUploadMarkFlg  = pVideoData->sdCardData.upload_flag;

    if( m_delMode == DEL_MODE_SELECT )
    {
      gfl2::lyt::LytPane* pChkMarkPane  = m_aSelectItem[ pane_index ].pChkMarkPane;
      pG2DUtil->SetPaneVisible( pChkMarkPane, m_pVideoDataListManager->GetDataDelFlg( pos ) );

      pG2DUtil->SetPaneVisible( pUploadMarkPane, false );
    }
    else
    {
      
      pG2DUtil->SetPaneVisible( pUploadMarkPane, m_aSelectItem[ pane_index ].bUploadMarkFlg );
    }
  }

  void LowDataListView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
  {
  }

  void LowDataListView::PaneListItemSelectListener( u32 pos )
  {
    UCHIDA_PRINT( "Push DataList\n" );

    //  選択項目に合わせてデータ更新
    _UpdateData( pos );

    u32  cursorPos  = 0;
    m_pPaneListView->GetPaneList()->GetCursorData( &cursorPos, NULL );
    u32 paneListIndex = m_pPaneListView->GetPaneList()->GetPosPaneIndex( cursorPos );

    app::util::G2DUtil* pG2DUtil          = UIView::GetG2DUtil();

    //  通常モード
    if( m_delMode == DEL_MODE_NONE )
    {
      //  選択
      m_pushButtonId  = BUTTON_ID_DATA_SELECT;
      SetState( STATE_INPUT_EVENT_WAIT );
    }
    else
    //  削除モード
    if( m_delMode == DEL_MODE_SELECT )
    {
      //  削除チェックをつける

      bool  bNowDelFlg  = m_pVideoDataListManager->GetDataDelFlg( pos );
      bool  bNewDelFlg  = false;
      if( bNowDelFlg == false )
      {
        bNewDelFlg  = true;
      }
      else
      {
        bNewDelFlg  = false;
      }

      //  チャックマーク表示変更
      {
        gfl2::lyt::LytPane* pChkMarkPane  = m_aSelectItem[ paneListIndex ].pChkMarkPane;
        pG2DUtil->SetPaneVisible( pChkMarkPane, bNewDelFlg );

        m_pVideoDataListManager->SetDataDelFlg( pos, bNewDelFlg );
      }

      //  削除フラグがついている個数を取得
      //  １以上であれば削除ボタンを押せるようにする
      u32 delFlgCnt = m_pVideoDataListManager->GetDataDelFlgCount();
      if( 1 <= delFlgCnt )
      {
        UIView::GetButtonManager()->SetButtonActive( BUTTON_ID_DATA_DEL );
      }
      else
      {
        UIView::GetButtonManager()->SetButtonPassive( BUTTON_ID_DATA_DEL );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowDataListView::OnLayoutPaneEvent( const u32 id )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    m_pushButtonId  = (ButtonId)id;

    SetState( STATE_INPUT_EVENT_WAIT );

    return  DISABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力通知用リスナ
    * @param[in] pButton  ボタン入力の情報
    * @param[in] pKey     キー入力の情報
    * @param[in] pStick   スティック入力の情報
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowDataListView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    //  削除ボタン
    if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_DATA_DEL );
      return  DISABLE_ACCESS;
    }
    //  切り替えボタン
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_DATA_CHANGE );
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void LowDataListView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  pAppLytBuff, 16, app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002_BFLYT,
        LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_002___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

  //------------------------------------------------------------------
  /**
    * @brief    データの更新
    * @@param   現在選択している選択pos
    */
  //------------------------------------------------------------------
  void LowDataListView::_UpdateData( const u32 currentPos )
  {
    app::util::G2DUtil* pG2DUtil          = UIView::GetG2DUtil();
    pG2DUtil->SetTextBoxPaneNumber( m_pSelectListNumTextPane, msg_bvp_sys_up_12, currentPos + 1, 3, 0, print::NUM_DISP_ZERO );

    if( m_pDelegate != NULL )
    {
      if( m_pDelegate->LowDataListViewDelegate_OnActiveCursor( currentPos ) == LowDataListViewDelegate::ACTIVE_ITEM_RESULT_INFO_OFF )
      {
        //  infoデータがないので切り替えをパッシブにする
        UIView::GetButtonManager()->SetButtonPassive( BUTTON_ID_DATA_CHANGE );
      }
      else
      {
        //  infoデータがあるので切り替えをパッシブにする
        UIView::GetButtonManager()->SetButtonActive( BUTTON_ID_DATA_CHANGE );
      }
    }
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
