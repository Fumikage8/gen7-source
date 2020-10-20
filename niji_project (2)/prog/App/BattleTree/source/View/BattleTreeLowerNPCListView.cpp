//==============================================================================
/**
 * @file    BattleTreeLowerNPCListView.h
 * @author  yuto_uchida
 * @date    2015.11.30
 * @brief   NPCトレーナーリストView
 */
//==============================================================================
#include "App/BattleTree/source/View/BattleTreeLowerNPCListView.h"

// niji
#include "App/BattleTree/source/BattleTreeResourceID.h"
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/Tool/CommonMessageObject.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_battletrainer_select.h>

#include <niji_conv_header/app/battletree/res2d/battletreeLyt.h>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt_anim_list.h>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt_pane.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(View)

  //------------------------------------------------------------------------------
  // @brief   定数
  //------------------------------------------------------------------------------
  const LowerNPCTrainerListView::PANE_SETUP_DATA LowerNPCTrainerListView::ma_sPaneSetupData[] =
  {
    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_00,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_00_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_00_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_00_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_00_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_01,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_01_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_01_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_01_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_01_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_02,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_02_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_02_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_02_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_02_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_03,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_03_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_03_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_03_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_03_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_04,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_04_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_04_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_04_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_04_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_05,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_05_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_05_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_05_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_05_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_06,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_06_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_06_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_06_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_06_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_07,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_07_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_07_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_07_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_07_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_08,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_08_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_08_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_08_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_08_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_10,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_10_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_10_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_10_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_10_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_11,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_11_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_11_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_11_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_11_TOUCH_RELEASE,
    },

    {
      PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_13,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_13_TOUCH,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_13_RELEASE,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_13_CANSEL,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_13_TOUCH_RELEASE,
    },

  };

  //------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------------------
  LowerNPCTrainerListView::LowerNPCTrainerListView( App::BattleTree::System::BattleTreeWork* pWork )
    : NetAppLib::System::NetApplicationViewBase(
        pWork,
        App::BattleTree::BATTLE_TREE_RESOURCE_ID_LAYOUT,
        LYTRES_BATTLETREELYT_BATTLETREE_MIN_LOW_001_BFLYT,
        LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001___NUM,
        app::util::G2DUtil::SETUP_LOWER,
        pWork->GetMessageUtility(),
        GARC_message_battletrainer_select_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    gfl2::heap::HeapBase* pDevHeap = m_pWork->GetAppHeap()->GetDeviceHeap();

    //  メニューカーソル作成
    {
      m_pMenuCursor = GFL_NEW( pDevHeap ) NetAppLib::UI::NetAppMenuCursor( m_pWork, GetG2DUtil(), NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, 0, LYT_WORK_ID );
    }

    app::util::G2DUtil* pG2DUtil            = UIView::GetG2DUtil();
    gfl2::lyt::LytWk*   pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_ID );
    gfl2::lyt::LytMultiResID& rMultiResID   = pG2DUtil->GetLayoutResourceID( 0 );

    //  ボタン作成
    {
      struct SETUP_BTN_DATA
      {
        gfl2::lyt::LytParts*  pPartsPane;
        u32 seId;
      };

      //  リストの順序が変わるとここを並べなおさないといけない
      const SETUP_BTN_DATA aSetupBtnData[ BUTTON_ID_MAX ]  =
      {
        { pLayoutWork->GetPartsPane( PANENAME_BATTLETREE_MIN_LOW_001_PANE_BACKBUTTON ), SEQ_SE_CANCEL1 },
      };

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
        //  Bキャンセル
        {
          BUTTON_ID_CANCEL,
          aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rMultiResID ),
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__BACKBUTTON_TOUCH,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__BACKBUTTON_RELEASE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__BACKBUTTON_CANSEL,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__BACKBUTTON_TOUCH_RELEASE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__BACKBUTTON_ACTIVE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__BACKBUTTON_PASSIVE
        }
      };

      UIResponder::CreateButtonManager( m_pWork->GetAppHeap(), pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
      //  入力コールバック設定
      UIView::SetInputListener( this );

      //  ボタンSEの設定
      {
        app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

        u32 loopNum = GFL_NELEMS( aSetupBtnData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pButtonManager->SetButtonSelectSE( i, aSetupBtnData[ i ].seId );
        }
      }
    }

    //  ウィンドウ作成
    {
      m_pLowerCommonMessageObject = GFL_NEW( pDevHeap ) app::tool::CommonMessageObject();

      app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResID );
      paneHelper.Push( PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_12 );

      app::tool::CommonMessageObject::INIT_PARAM  initParam;
      {
        initParam.lytId         = LYT_WORK_ID;
        initParam.pG2DUtil      = pG2DUtil;
        initParam.inAnimeId     = LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_12_IN;
        initParam.outAnimeId    = LA_BATTLETREELYT_BATTLETREE_MIN_LOW_001__PARTS_12_OUT;
        initParam.pWindowPane   = paneHelper.Peek();
        initParam.showWaitFrame = 0;

        //  テキストペイン設定
        {
          initParam.pTextBoxPaneArray[ 0 ]  = paneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00 );
          initParam.textBoxPaneNum  = 1;
        }
      }

      m_pLowerCommonMessageObject->InitFunc( initParam, pDevHeap );
      {
        gfl2::str::StrBuf* ppStrBuf[ 1 ] = { NULL };
        ppStrBuf[ 0 ] = &m_pWork->GetMessageUtility()->GetString( GARC_message_battletrainer_select_DAT, battle_trainer_select_01_02 );
        m_pLowerCommonMessageObject->ShowMessageTextPane( ppStrBuf, 1 );
      }
    }

    Setup();
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------------------
  LowerNPCTrainerListView::~LowerNPCTrainerListView( void )
  {
    m_pMenuCursor->Destroy();

    m_pLowerCommonMessageObject->EndFunc();

    GFL_SAFE_DELETE( m_pLowerCommonMessageObject );
    GFL_SAFE_DELETE( m_pMenuCursor );
    GFL_SAFE_DELETE( m_pScrollBar );

    DeleteManager();
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   セットアップ
   */
  //------------------------------------------------------------------------------
  void LowerNPCTrainerListView::Setup( void )
  {
    DeleteManager();

    gfl2::heap::HeapBase* pDevHeap = m_pWork->GetAppHeap()->GetDeviceHeap();

    app::util::G2DUtil* pG2DUtil            = UIView::GetG2DUtil();
    gfl2::lyt::LytWk*   pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_ID );
    gfl2::lyt::LytMultiResID& rMultiResID   = pG2DUtil->GetLayoutResourceID( 0 );

    u32 iconMax = GFL_NELEMS( ma_sPaneSetupData );

    m_pInfo         = GFL_NEW_LOW_ARRAY(pDevHeap) app::tool::ScrollPaneManager::PaneInfo[iconMax];
    m_pPaneDataList = GFL_NEW_ARRAY( pDevHeap ) PANE_DATA[ iconMax ];
    {
      for( u32 i=0; i < iconMax; ++i )
      {
        gfl2::lyt::LytParts* pIconParts = pLayoutWork->GetPartsPane( ma_sPaneSetupData[i].paneIndex );
        GFL_ASSERT( pIconParts != NULL );

        m_pInfo[i].cancel_anime_index   = ma_sPaneSetupData[i].cancel_anime_index;
        m_pInfo[i].hold_anime_index     = ma_sPaneSetupData[i].hold_anime_index;
        m_pInfo[i].release_anime_index  = ma_sPaneSetupData[i].release_anime_index;
        m_pInfo[i].select_anime_index   = ma_sPaneSetupData[i].select_anime_index;

        m_pInfo[i].root_pane      = pIconParts;
        m_pInfo[i].picture_pane   = pLayoutWork->GetPane( pIconParts, PANENAME_BT_BTN_LOW_000_PANE_PLAYERPHOTO, &rMultiResID );
        m_pInfo[i].bounding_pane  = pLayoutWork->GetPane( pIconParts, PANENAME_BT_BTN_LOW_000_PANE_BOUND_00, &rMultiResID );
        m_pInfo[i].cursor_pane    = pLayoutWork->GetPane( pIconParts, PANENAME_BT_BTN_LOW_000_PANE_NULL_CUR, &rMultiResID );

        m_pPaneDataList[ i ].pTrainerIcon       = (gfl2::lyt::LytPicture*)m_pInfo[i].picture_pane;
        m_pPaneDataList[ i ].pTrainerNameTexBox = pLayoutWork->GetTextBoxPane( pIconParts, PANENAME_BT_BTN_LOW_000_PANE_TEXTBOX_00, &rMultiResID );
        //  トレーナーアイコンデータ設定
        {
           //  デフォルトデータを作成
          app::tool::AppToolTrainerIconRendering::OutputDefaultIconObjectData( 1, &m_pPaneDataList[ i ].trainerIconObjData );

          m_pPaneDataList[ i ].trainerIconObjData.pReplacePane  = m_pPaneDataList[ i ].pTrainerIcon;
          m_pPaneDataList[ i ].trainerLoadId                    = m_pWork->GetAllocTrainerLoadID();
        }
      }
    }

    app::tool::ScrollPaneManager::Description param;
    {
      param.heap              = m_pWork->GetAppHeap();
      param.layout_work       = pLayoutWork;
      param.ui_device_manager = NULL; //  使わない(らしい)
      param.pane_table        = m_pInfo;
      param.pane_table_num    = iconMax;
      param.list_data_num     = m_pWork->GetAppParam()->in.npcSelectListNum;
      param.col               = 3;

      param.pane_width  = 104;
      param.pane_height = 100;

      param.pane_start_x = -102;
      param.pane_start_y = 67;

      param.pane_draw_area_top = 120;
      param.pane_draw_area_left = -138;
      param.pane_draw_area_down = -180;
      param.pane_draw_area_right = 138;

      param.pane_input_start_x = -160;
      param.pane_input_start_y = 110;

      // 入力できる横並びの個数
      param.pane_input_limit_col = 3;
      // 入力できる縦の個数
      param.pane_input_limit_row = 2;

      // @fix GFNMCat[1889] スライドしやすいようにフレームレートを上げて調整
      param.framerate = GameSys::FRAMEMODE_60;

      param.menu_cursor = m_pMenuCursor->GetAppToolMenuCursor();
      param.put_cursor_visible = true;

      param.listener = this;

      param.default_cursor_pos = 0;
      param.mode  = app::tool::ScrollPaneManager::MODE_NORMAL;
    }

    m_pScrollPaneManager = GFL_NEW( pDevHeap ) app::tool::ScrollPaneManager( param );

    UIView::AddSubView( m_pScrollPaneManager );
    SetVisibleCursor( false );

    //  スクロールバー作成
    {
      app::util::ScrollBar::SETUP_PARAM setup =
      {
        // @note  以下、[set1]を設定
        pDevHeap,
        app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
        m_pScrollPaneManager->GetScrollMax(),

        pLayoutWork,
        rMultiResID,
        PANENAME_BATTLETREE_MIN_LOW_001_PANE_PARTS_09,
        PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
        PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
        PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

        NULL,
        NULL,
        NULL,
        NULL,
      };

      m_pScrollBar = GFL_NEW( pDevHeap ) app::util::ScrollBar( &setup );
      m_pScrollBar->SetRequest( app::util::ScrollBar::REQ_BAR_ON );

      m_pScrollBar->ChangeParam( setup.max );
      if( setup.max == 0 )
      {
        m_pScrollBar->SetDrawEnable( false );
      }
      else
      {
        m_pScrollBar->SetDrawEnable( true );
      }
    }
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   スクロールペインマネージャの破棄
   */
  //------------------------------------------------------------------------------
  void LowerNPCTrainerListView::DeleteManager( void )
  {
    GFL_SAFE_DELETE_ARRAY( m_pPaneDataList );

    if( m_pScrollPaneManager )
    {
      m_pScrollPaneManager->RemoveFromSuperView();

      GFL_SAFE_DELETE( m_pScrollPaneManager );
    }

    GFL_SAFE_DELETE_ARRAY( m_pInfo );
  }

  //------------------------------------------------------------------
  /**
    * @brief   処理開始
    * @note    ポーズ状態から復帰する
    */
  //------------------------------------------------------------------
  void LowerNPCTrainerListView::Start()
  {
    _SetUpdateSeq( UPDATE_SEQ_IDLE );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの表示切替
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------------------
  void LowerNPCTrainerListView::SetVisibleCursor( bool visible )
  {
    if( m_pScrollPaneManager == NULL ) return;

    m_pScrollPaneManager->SetVisibleMenuCursor( visible );
  }

  //  押したボタンidを取得
  LowerNPCTrainerListView::ButtonId  LowerNPCTrainerListView::GetPushId()
  {
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {
      return m_pushId;
    }

    return  BUTTON_ID_NONE;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowerNPCTrainerListView::Update(void)
  {
    if( m_updateSeq == UPDATE_SEQ_IDLE )
    {
    }
    else
    if( m_updateSeq == UPDATE_SEQ_BTN_ANIM_WAIT )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        _SetUpdateSeq( UPDATE_SEQ_PAUSE );
      }
    }
    else
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {

    }

    m_pLowerCommonMessageObject->Update();

    UIView::Update();
    m_pMenuCursor->Update();
  }

  void LowerNPCTrainerListView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pScrollBar->Draw( m_pScrollPaneManager->GetScrollValue() );

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LYT_WORK_ID );

    if( UIView::IsInputEnabled() == true )
    {
      m_pMenuCursor->Draw( displayNo );
    }
  }

  bool LowerNPCTrainerListView::IsDrawing(void) const
  {
    bool result = false;
  
    if( m_pScrollPaneManager->IsDrawing() == true )
    {
      result  = true;
    }
    else
    if( UIView::IsDrawing() == true )
    {
      result = true;
    }
    else
    if( m_pMenuCursor->CanDestroy() == false )
    {
      result  = true;
    }

    return result;
  }

  void LowerNPCTrainerListView::_SetUpdateSeq( const UpdateSeq newSeq )
  {
    m_updateSeq = newSeq;

    if( m_updateSeq == UPDATE_SEQ_IDLE )
    {
      UIView::SetInputEnabledAllView( true );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_BTN_ANIM_WAIT )
    {
      UIView::SetInputEnabledAllView( false );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {
      UIView::SetInputEnabledAllView( false );
    }
  }

  //==============================================================================
  /**
   *      ↓　以下がリスナー　↓
   */
  //==============================================================================
  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowerNPCTrainerListView::OnLayoutPaneEvent( const u32 id )
  {
    m_pushId  = (ButtonId)id;
    if( m_pushId == BUTTON_ID_CANCEL )
    {
      _SetUpdateSeq( UPDATE_SEQ_BTN_ANIM_WAIT );
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
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
  app::ui::UIInputListener::ListenerResult LowerNPCTrainerListView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_CANCEL );

      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   
   */
  //------------------------------------------------------------------------------
  void LowerNPCTrainerListView::WriteTexture( u32 pane_index, u32 dataID, bool is_end_async )
  {
    UCHIDA_PRINT( "%s( pane_index[%d], dataID[%d], is_end_async[%d]\n", __FUNCTION__, pane_index, dataID, is_end_async );
  
    if( m_pWork->GetAppParam()->in.npcSelectListNum <= dataID )
    {
      GFL_ASSERT( 0 );
      return;
    }

    app::util::G2DUtil* pG2DUtil            = UIView::GetG2DUtil();

    App::BattleTree::NPC_SELECT_ITEM* pItem = &m_pWork->GetAppParam()->in.aNpcSelectItem[ dataID ];

    //  トレーナー名
    pG2DUtil->SetTextBoxPaneString( m_pPaneDataList[ pane_index ].pTrainerNameTexBox, pItem->aName );

    //  NPCトレーナーアイコンの貼り付け
    {
      m_pPaneDataList[ pane_index ].trainerIconObjData.trainerId  = pItem->type;
      m_pWork->GetTrainerIconUtilty()->SetIconObjectData( m_pPaneDataList[ pane_index ].trainerLoadId, m_pPaneDataList[ pane_index ].trainerIconObjData, false );
    }
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   決定ボタンの押下通知
   * @param   pane_index  選択されたボタンのインデックス
   * @param   dataID      選択されたデータのインデックス
   */
  //------------------------------------------------------------------------------
  void LowerNPCTrainerListView::Decide( u32 pane_index, u32 dataID )
  {
    UCHIDA_PRINT( "%s( pane_index[%d], dataID[%d]\n", __FUNCTION__, pane_index, dataID );

    m_selectDataId  = dataID;
    m_pushId        = BUTTON_ID_SELECT;

    _SetUpdateSeq( UPDATE_SEQ_BTN_ANIM_WAIT );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   カーソルの選択通知
   * @param   pane_index  選択されたボタンのインデックス
   * @param   dataID      選択されたデータのインデックス
   */
  //------------------------------------------------------------------------------
  void LowerNPCTrainerListView::OnCursor( u32 pane_index, u32 dataID )
  {
//    UCHIDA_PRINT( "%s( pane_index[%d], dataID[%d]\n", __FUNCTION__, pane_index, dataID );

    if( m_pDelegate != NULL )
    {
      m_pDelegate->LowerNPCTrainerListView_OnCursor( dataID );
    }

    m_pScrollPaneManager->SetVisibleMenuCursor( true );
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
