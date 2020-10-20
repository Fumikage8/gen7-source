//=============================================================================
/**
 * @file BattleUIItemConfirmLowerDraw.h
 * @brief バトル下画面 >> 「アイテム確認」画面
 * @author yuto_uchida
 * @date 2015.04.07
 */
//=============================================================================
#include "BattleUIItemConfirmLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/itmlow.h"
#include "niji_conv_header/app/battle/res2d/itmlow_pane.h"
#include "niji_conv_header/app/battle/res2d/itmlow_anim_list.h"
#include "niji_conv_header/message/msg_btl_app.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  const BattleUIItemConfirmLowerDraw::BUTTON_DATA  BattleUIItemConfirmLowerDraw::m_aButtonDataTable[ BUTTON_ID_MAX ] =
  {
    { BUTTON_ID_CANCEL,           SEQ_SE_CANCEL1 },
    { BUTTON_ID_ENTER,            SEQ_SE_DECIDE1 },
    { BUTTON_ID_MOVE_LEFT_LIST,   SEQ_SE_DECIDE1 },
    { BUTTON_ID_MOVE_RIGHT_LIST,  SEQ_SE_DECIDE1 },
  };

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIItemConfirmLowerDraw::BattleUIItemConfirmLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );
    GFL_ASSERT( param.pBtlAppMsgData );
    GFL_ASSERT( param.pBtlWordSet );
    GFL_ASSERT( param.pLytMenuCursorBuff );

    //  初期化
    _Clear();

    m_initParam       = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIItemConfirmLowerDraw::~BattleUIItemConfirmLowerDraw()
  {
    GFL_SAFE_DELETE( m_pCommonMessageWindow );

    GFL_ASSERT( m_pItemIcon );
    GFL_SAFE_DELETE( m_pItemIcon );

    GFL_ASSERT( m_pMenuCursor );

    GFL_SAFE_DELETE( m_pMenuCursor );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::Initialize()
  {
    //  レイアウト生成
    _Initialize2D( m_initParam.pLytBuff );

    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );
    ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    gfl2::lyt::LytParts*  pEnterBtnPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_ITM_LOW_000_PANE_PARTS_USE );
    GFL_ASSERT( pEnterBtnPartsPane );

    //  ボタン関連の設定
    {
      gfl2::lyt::LytParts*  pBackBtnPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_ITM_LOW_000_PANE_PARTS_BACK );
      GFL_ASSERT( pBackBtnPartsPane );

      gfl2::lyt::LytParts*  pLeftArrowBtnPartsPane   = pLayoutWork->GetPartsPane( PANENAME_BATTLE_ITM_LOW_000_PANE_PARTSCUR_L );
      GFL_ASSERT( pLeftArrowBtnPartsPane );

      gfl2::lyt::LytParts*  pRightArrowBtnPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_ITM_LOW_000_PANE_PARTSCUR_R );
      GFL_ASSERT( pRightArrowBtnPartsPane );

      const ::app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
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
        //  もどるボタン
        {
          BUTTON_ID_CANCEL,
          pBackBtnPartsPane,
          pLayoutWork->GetBoundingPane( pBackBtnPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rMultiResId ),
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_BACK_TOUCH,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_BACK_RELEASE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_BACK_CANSEL,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_BACK_TOUCH_RELEASE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_BACK_ACTIVE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_BACK_PASSIVE
        },

        //  決定ボタン
        {
          BUTTON_ID_ENTER,
          pEnterBtnPartsPane,
          pLayoutWork->GetBoundingPane( pEnterBtnPartsPane, PANENAME_COMMON_BTN_LOW_005_PANE_BOUND_00, &rMultiResId ),
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_USE_TOUCH,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_USE_RELEASE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_USE_CANSEL,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_USE_TOUCH_RELEASE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_USE_ACTIVE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_USE_PASSIVE
        },

        //  左矢印
        {
          BUTTON_ID_MOVE_LEFT_LIST,
          pLeftArrowBtnPartsPane,
          pLayoutWork->GetBoundingPane( pLeftArrowBtnPartsPane, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &rMultiResId ),
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_L_TOUCH,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_L_RELEASE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_L_CANSEL,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_L_TOUCH_RELEASE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_L_ACTIVE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_L_PASSIVE
        },

        //  右矢印
        {
          BUTTON_ID_MOVE_RIGHT_LIST,
          pRightArrowBtnPartsPane,
          pLayoutWork->GetBoundingPane( pRightArrowBtnPartsPane, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &rMultiResId ),
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_R_TOUCH,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_R_RELEASE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_R_CANSEL,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_R_TOUCH_RELEASE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_R_ACTIVE,
          LA_ITMLOW_BATTLE_ITM_LOW_000__PARTSCUR_R_PASSIVE
        }
      };

      UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN ), aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );

      //  入力コールバック設定
      UIView::SetInputListener( this );
    }

    //  ボタンSEの設定
    {
      ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

      const BattleUIItemConfirmLowerDraw::BUTTON_DATA*  pButtonData = NULL;
      for( s32 i = 0; i < BUTTON_ID_MAX; ++i )
      {
        pButtonData = &m_aButtonDataTable[ i ];
        pButtonManager->SetButtonSelectSE( pButtonData->id, pButtonData->seId );
      }
    }

    //  固定テキスト設定
    {
      //  使用ボタン名
      {
        gfl2::lyt::LytTextBox*  pTextBoxPane  = pLayoutWork->GetTextBoxPane( pEnterBtnPartsPane, PANENAME_COMMON_BTN_LOW_005_PANE_TEXTBOX_00, &rMultiResId );
        GFL_ASSERT( pTextBoxPane );

        UIView::SetTextboxPaneMessage( pTextBoxPane, str_btl_bag_06 );
      }
    }

    //  カーソル作成
    {
      m_pMenuCursor = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::MenuCursor( m_initParam.pUtilHeap, &m_initParam.pLytMenuCursorBuff );
      m_pMenuCursor->CreateLayoutWork( pG2DUtil->GetLayoutSystem(), pG2DUtil->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_SMALL );
      m_pMenuCursor->SetVisible( false );

      //  ペインにくっつける
      m_pMenuCursor->Put( pLayoutWork, pLayoutWork->GetPane( pEnterBtnPartsPane, PANENAME_COMMON_BTN_LOW_005_PANE_NULL_CUR, &rMultiResId ) );
    }

    //  アイテムアイコン
    {
      m_pItemIcon = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::ItemIcon( m_initParam.pUtilHeap->GetSystemHeap(), m_initParam.pUtilHeap->GetDeviceHeap(), 1 );
    }

    //  使用不可能時のエラーメッセージウィンドウ作成
    {
       m_pCommonMessageWindow = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowManipulator();
       App::Tool::CommonMessageWindowManipulator::SSetupInfo  setupInfo;
       // 設定内容を決める
       {
        setupInfo.layoutWorkID        = LYT_WK_TYPE_SCREEN;

        setupInfo.paneID_MsgCursor    = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
        setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
        setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1A_OF_TEXTBOX_1A;
        setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
        setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1B_OF_TEXTBOX_1B;
        setupInfo.paneID_WindowParts  = PANENAME_BATTLE_ITM_LOW_000_PANE_PARTS_MSG;
        setupInfo.pHandler            = this;
        setupInfo.pHeap               = m_initParam.pUtilHeap;
        setupInfo.pTargetView         = this;
       }

       m_pCommonMessageWindow->Setup( setupInfo );
       m_pCommonMessageWindow->SetFinishMode_Timer( 30 );
    }

    //  アイテムカテゴリー名
    {
      m_pItemCategoryNameTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_ITM_LOW_000_PANE_TEXTBOX_00 );
      GFL_ASSERT( m_pItemCategoryNameTextPane );
    }

    //  アイテム名
    {
      m_pItemNameTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_ITM_LOW_000_PANE_TEXTBOX_01 );
      GFL_ASSERT( m_pItemNameTextPane );
    }

    //  アイテム所持数
    {
      m_pItemHaveNumTextPane  = pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_ITM_LOW_000_PANE_TEXTBOX_03 );
      GFL_ASSERT( m_pItemHaveNumTextPane );
    }

    //  アイテム詳細
    {
      m_pItemDescriptionTextPane  = pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_ITM_LOW_000_PANE_TEXTBOX_02 );
      GFL_ASSERT( m_pItemDescriptionTextPane );
    }

    //  アイコン設定
    {
      gfl2::lyt::LytParts*  pLytPartsPane = pLayoutWork->GetPartsPane( PANENAME_BATTLE_ITM_LOW_000_PANE_PARTS_ITEM );
      GFL_ASSERT( pLytPartsPane );

      m_pItemIconPicturePane  = pLayoutWork->GetPicturePane( pLytPartsPane, PANENAME_COMMON_ICN_006_PANE_ICON_ITEM, &rMultiResId );
      GFL_ASSERT( m_pItemIconPicturePane );
    }

    //  BGペイン取得
    {
      m_pBGPane = pLayoutWork->GetPane( PANENAME_BATTLE_ITM_LOW_000_PANE_BG_00 );
      GFL_ASSERT( m_pBGPane );
    }

    //  左矢印オブジェクトのセットアップ
    {
      ArrowObject::SETUP_DATA setupData;
      setupData.pView       = this;
      setupData.lytId       = LYT_WK_TYPE_SCREEN;

      m_aArrowObject[ ARROW_OBJECT_LEFT ].Setup( setupData );
    }

    //  右矢印オブジェクトのセットアップ
    {
      ArrowObject::SETUP_DATA setupData;
      setupData.pView       = this;
      setupData.lytId       = LYT_WK_TYPE_SCREEN;

      m_aArrowObject[ ARROW_OBJECT_RIGHT ].Setup( setupData );
    }

    //  初回は非表示
    UIView::SetVisible( false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::Terminate()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   クラス破棄可能か
    */
  //------------------------------------------------------------------
  bool BattleUIItemConfirmLowerDraw::IsDelete()
  {
    bool  bRet  = true;

    //  リソースバッファを参照しているだけなので、m_pMenuCursor->DeleteResource()は呼ぶと他の画面表示時に壊れる
    if( UIView::IsDrawing() == true )
    {
      bRet  = false;
    }

    if( m_pMenuCursor->DeleteLayoutWork() == false )
    {
      bRet  = false;
    }

    return  bRet;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::Update( void )
  {
    _UpdateState();

    //  アイテムアイコンのテクスチャーをペイン非表示にせずに、すぐに差し替える処理をすると
    //  連続で何度も切り替える場面では表示がバグることがあるのを防ぐため非表示にしてから差し替えて再表示している。
    if( m_itemChangeId != -1 )
    {
      enum
      {
        BTL_UI_ITEM_CONFIRM_ICON_STEP_INIT = 0,
        BTL_UI_ITEM_CONFIRM_ICON_STEP_LOAD,
        BTL_UI_ITEM_CONFIRM_ICON_STEP_WAIT
      };

      ::app::util::G2DUtil* pG2dUtil  = UIView::GetG2DUtil();

      if( m_itemChangeStep == BTL_UI_ITEM_CONFIRM_ICON_STEP_INIT )
      {
        pG2dUtil->SetPaneVisible( m_pItemIconPicturePane, false );
        ++m_itemChangeStep;
      }
      else
      if( m_itemChangeStep == BTL_UI_ITEM_CONFIRM_ICON_STEP_LOAD )
      {
        m_pItemIcon->ReplacePaneTexture( 0, m_itemChangeId, m_pItemIconPicturePane );
        ++m_itemChangeStep;
      }
      else
      if( m_itemChangeStep == BTL_UI_ITEM_CONFIRM_ICON_STEP_WAIT )
      {
        pG2dUtil->SetPaneVisible( m_pItemIconPicturePane, true );
        ++m_itemChangeStep;

        m_itemChangeId  = -1;
      }
    }

    m_pMenuCursor->Update();
    m_pCommonMessageWindow->UpdateWindow();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }

    //  入力できない時は描画はしない
    if( UIView::IsInputEnabled() == true )
    {
      m_pMenuCursor->Draw( m_initParam.pRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER, LOW_VIEW_ORDER_OTHER );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージイベント開始
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::EventMessage( gfl2::str::StrBuf*  pMessageStrBuf )
  {
    GFL_ASSERT( pMessageStrBuf );

    m_pMesssageStrBuf  = pMessageStrBuf;
    _SetUpdateSubState( UPDATE_SUB_STATE_MSG_OPEN );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 指定したidのボタンを押す
    * @param   buttonId    : 押すボタンのid
    * @param   bForcePush  : 入力がfalseになっても押すかどうか（捕獲チュートリアルでは入力がfalseになっているのでその時に使用する）
    */
  //-------------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::PushBtn( ButtonId buttonId, const bool bForcePush )
  {
    ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

    if( pButtonManager->IsButtonVisible( buttonId ) == false )
    {
      return;
    }

    //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
    pButtonManager->StartSelectedAct( buttonId );

    if( bForcePush == true )
    {
      //  入力が無効の場合はOnLayoutPaneEvent()関数が呼ばれないので手動で呼び出す
      if( UIView::IsInputEnabled() == false )
      {
        OnLayoutPaneEvent( buttonId );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   情報表示設定
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::SetInfoData( const s32 itemId, const s32 itemNum, const item::BtlPocket itemPocketIdx )
  {
    ::app::util::G2DUtil* pG2dUtil  = UIView::GetG2DUtil();

    //  アイテムカテゴリー名
    {
      const s32 aPocketInfoData[ item::BTLPOCKET_NUM ][ 2 ]  =
      {
        //  BTLPOCKET_ROTOPON,  // ロトムパワー
        { str_btl_bag_18, LA_ITMLOW_BATTLE_ITM_LOW_000_CATEGORY_04 },

        //  BTLPOCKET_HP,      // HP/PP 回復
        { str_btl_bag_00, LA_ITMLOW_BATTLE_ITM_LOW_000_CATEGORY_00 },

        //  BTLPOCKET_STATUS,  // 状態異常 回復
        { str_btl_bag_01, LA_ITMLOW_BATTLE_ITM_LOW_000_CATEGORY_01 },

        //  BTLPOCKET_BALL,    // ボール
        { str_btl_bag_03, LA_ITMLOW_BATTLE_ITM_LOW_000_CATEGORY_02 },

        //  BTLPOCKET_BATTLE,  // 戦闘用
        { str_btl_bag_04, LA_ITMLOW_BATTLE_ITM_LOW_000_CATEGORY_03 }
      };

      //  カテゴリ-名を設定
      pG2dUtil->SetTextBoxPaneString( m_pItemCategoryNameTextPane, aPocketInfoData[ itemPocketIdx ][ 0 ] );

      //  カテゴリーごとのカラーアニメ設定
      pG2dUtil->StartAnime( LYT_WK_TYPE_SCREEN, aPocketInfoData[ itemPocketIdx ][ 1 ] );
    }

    //  アイテム名
    {
      m_initParam.pBtlWordSet->RegisterItemName( 0, itemId, itemNum );
      pG2dUtil->SetTextBoxPaneStringExpand( m_pItemNameTextPane, STR_ITEM_NAME );
    }

    //  アイテム所持数
    {
      pG2dUtil->SetTextBoxPaneNumber( m_pItemHaveNumTextPane, STR_VALUE_3, itemNum, 3, 0, print::NUM_DISP_LEFT );
    }

    //  アイテム詳細
    {
      m_initParam.pItemMessage->GetInfoMessage( itemId, pG2dUtil->GetTempStrBuf( 1 ) );
      pG2dUtil->SetTextBoxPaneString( m_pItemDescriptionTextPane, pG2dUtil->GetTempStrBuf( 1 ) );
    }

    //  アイコンテクスチャ差し替え処理を開始
    //  すぐに差し替えないのは短いフレーム間で差し替えると表示が一瞬バグるのを防ぐため
    //  いったんアイコンを非表示の状態で差し替えて、差し替え終わった次のフレームで表示するようにしている
    {
      m_itemChangeId = itemId;
      m_itemChangeStep  = 0;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief     背景表示設定
    * @param[in] bFlg : 表示するかのフラグ
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::SetViaibleBG( const bool bFlg )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pBGPane, bFlg );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタン表示設定
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::SetVisibleButton( const ButtonId id, const bool bFlg )
  {
    ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
    if( bFlg == true )
    {
      pButtonManager->SetButtonVisible( id );
    }
    else
    {
      pButtonManager->SetButtonInvisible( id );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUIItemConfirmLowerDraw::OnLayoutPaneEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneEvent : buttonId = %d\n", buttonId );

    if( m_initParam.pDelegate == NULL )
    {
      return  ENABLE_ACCESS;
    }

    if( m_initParam.pDelegate->OnEnterInput( this, buttonId ) )
    {
      if( buttonId == BUTTON_ID_MOVE_LEFT_LIST )
      {
        m_aArrowObject[ ARROW_OBJECT_LEFT ].Push();
      }
      else
      if( buttonId == BUTTON_ID_MOVE_RIGHT_LIST )
      {
        m_aArrowObject[ ARROW_OBJECT_RIGHT ].Push();
      }

      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力の検知
    *
    * @param  pKey    十字キー
    * @param  pStick  アナログスティック
    *
    * @return 同フレーム内での他入力イベントを許可するならtrue、
    *         他のイベントを排除するならfalseを返却すること。
    */
  //------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUIItemConfirmLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

    m_initParam.pDelegate->OnKeyAction( this, pButton, pKey, pStick );

    return ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   汎用メッセージのコールバッグ
    */
  //------------------------------------------------------------------
  void  BattleUIItemConfirmLowerDraw::CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event )
  {
    if( event == App::Tool::CommonMessageWindowManipulator::EVENT_OnFinish )
    {
      m_bFinishCommomMessageWindow  = true;
    }
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUIItemConfirmLowerDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_ROOT },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
          //  選択画面
          {
            0,
            LYTRES_ITMLOW_BATTLE_ITM_LOW_000_BFLYT,
            LA_ITMLOW_BATTLE_ITM_LOW_000___NUM,
            pAppLytBuff,
            true,
            ::app::util::G2DUtil::SETUP_LOWER,
            true
          }
      };

      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), m_initParam.pBtlAppMsgData, m_initParam.pBtlWordSet );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::_UpdateState()
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      //  アニメの終了を検知
      _SetState( STATE_TYPE_UPDATE );
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
      if( m_updateSubState == UPDATE_SUB_STATE_INFO )
      {

      }

      if( m_updateSubState == UPDATE_SUB_STATE_MSG_OPEN )
      {
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_MSG_IN ) )
        {
          _SetUpdateSubState( UPDATE_SUB_STATE_MSG_UPDATE );
        }
      }

      if( m_updateSubState == UPDATE_SUB_STATE_MSG_UPDATE )
      {
        if( m_bFinishCommomMessageWindow == true )
        {
          _SetUpdateSubState( UPDATE_SUB_STATE_MSG_CLOSE );
        }
      }

      if( m_updateSubState == UPDATE_SUB_STATE_MSG_CLOSE )
      {
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_MSG_OUT ) )
        {
          _SetUpdateSubState( UPDATE_SUB_STATE_INFO );
        }
      }
    }

    if( m_state == STATE_TYPE_OUT )
    {
      //  アニメの終了を検知
      _SetState( STATE_TYPE_END );
    }

    {
      u32 loopNum = GFL_NELEMS( m_aArrowObject );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aArrowObject[ i ].Update();
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::_SetState( const BattleUIItemConfirmLowerDraw::StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else if( m_state == STATE_TYPE_IN )
    {
      //  表示
      UIView::SetVisible( true );

      m_pMenuCursor->SetVisible( true );
      m_pMenuCursor->StartAnime();

      //  カーソル表示
      m_pMenuCursor->SetVisible( true );
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      _SetUpdateSubState( UPDATE_SUB_STATE_INFO );
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
      //  カーソルを非表示
      m_pMenuCursor->SetVisible( false );
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      //  もう終了しているので表示はしない
      UIView::SetVisible( false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   更新時のサブステート
    */
  //------------------------------------------------------------------
  void BattleUIItemConfirmLowerDraw::_SetUpdateSubState( const UpdateSubState newState )
  {
    GFL_ASSERT_STOP( m_state == STATE_TYPE_UPDATE );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    m_updateSubState  = newState;

    if( newState == UPDATE_SUB_STATE_INFO )
    {
      m_pMenuCursor->SetVisible( true );

      pG2DUtil->SetPaneVisible( LYT_WK_TYPE_SCREEN, PANENAME_BATTLE_ITM_LOW_000_PANE_PARTS_MSG, false );
    }
    else
    if( newState == UPDATE_SUB_STATE_MSG_OPEN )
    {
      m_pMenuCursor->SetVisible( false );

      pG2DUtil->SetPaneVisible( LYT_WK_TYPE_SCREEN, PANENAME_BATTLE_ITM_LOW_000_PANE_PARTS_MSG, true );
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_MSG_IN );

      m_bFinishCommomMessageWindow  = false;
      m_pCommonMessageWindow->ShowMessage( m_pMesssageStrBuf );
    }
    else
    if( newState == UPDATE_SUB_STATE_MSG_UPDATE )
    {
    }
    else
    if( newState == UPDATE_SUB_STATE_MSG_CLOSE )
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_ITMLOW_BATTLE_ITM_LOW_000__PARTS_MSG_OUT );
    }
  }

//  ここから先は クラス BattleUIItemConfirmLowerDraw::ArrowObject の定義
  BattleUIItemConfirmLowerDraw::ArrowObject::ArrowObject()
  {
    _Clear();
  }

  void BattleUIItemConfirmLowerDraw::ArrowObject::Setup( const SETUP_DATA& rSetupData )
  {
    GFL_ASSERT( rSetupData.pView );

    m_setupData = rSetupData;

    _SetState( ARROW_STATE_TYPE_IDLE );
  }

  void BattleUIItemConfirmLowerDraw::ArrowObject::Push()
  {
    _SetState( ARROW_STATE_TYPE_PUSH );
  }

  void BattleUIItemConfirmLowerDraw::ArrowObject::Update()
  {
    if( m_state == ARROW_STATE_TYPE_IDLE )
    {
    }

    if( m_state == ARROW_STATE_TYPE_PUSH )
    {
      _SetState( ARROW_STATE_TYPE_IDLE );
    }
  }

  void BattleUIItemConfirmLowerDraw::ArrowObject::_SetState( const ArrowStateType newState )
  {
    m_state = newState;

    if( m_state == ARROW_STATE_TYPE_IDLE )
    {
    }
    else
    if( m_state == ARROW_STATE_TYPE_PUSH )
    {
    }
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
