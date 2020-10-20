//======================================================================
/**
 * @file BattleVideoPlayerLowDataCtrlView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの下画面データ制御View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerLowDataCtrlView.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/tool/app_tool_MenuCursor.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_anim_list.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_pane.h"

#include <Sound/include/Sound.h>

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  //  選択して押せるボタン一覧
  const LowDataCtrlView::ButtonId  LowDataCtrlView::ms_aSelectButtonId[] =
  {
    BUTTON_ID_SELECT_01,
    BUTTON_ID_SELECT_02,
    BUTTON_ID_SELECT_03
  };

  //  カーソル移動のボタンidのルート
  const LowDataCtrlView::ButtonId LowDataCtrlView::ms_aCursorPosRootButtonId[] =
  {
    BUTTON_ID_PLAYER,
    BUTTON_ID_SELECT_01,
    BUTTON_ID_SELECT_02,
    BUTTON_ID_SELECT_03,
  };

  LowDataCtrlView::LowDataCtrlView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  LowDataCtrlView::~LowDataCtrlView()
  {
    GFL_SAFE_DELETE( m_pMenuCursor );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool LowDataCtrlView::InitFunc()
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

      //  固定テキスト表示
      {
        //  きりかえ
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          paneHelper.Push( PANENAME_BVP_MIN_LOW_003_PANE_PARTS_00 );

          gfl2::lyt::LytTextBox*  pTextBoxPane  = paneHelper.GetTextBoxPane( PANENAME_BVP_BTN_LOW_000_PANE_TEXTBOX_00 );
          GFL_ASSERT( pTextBoxPane );

          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, msg_bvp_sys_low_17 );
        }
      }

      {
        struct SETUP_BUTTON_DATA
        {
          u32 id;
          u32 seId;
          gfl2::lyt::LytParts*    pRootPane;
          gfl2::lyt::LytPaneIndex cursorPaneIndex;
        };

        const SETUP_BUTTON_DATA aButtonData[ BUTTON_ID_MAX ]  =
        {
          //  再生ボタン
          {
            BUTTON_ID_PLAYER,
            SEQ_SE_DECIDE1,

            pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_003_PANE_PARTS_04 ),
            PANENAME_BVP_BTN_LOW_004_PANE_NULL_CUR,
          },

          //  選択ボタン１
          {
            BUTTON_ID_SELECT_01,
            SEQ_SE_DECIDE1,

            pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_003_PANE_PARTS_01 ),
            PANENAME_BVP_BTN_LOW_005_PANE_NULL_CUR,
          },

          //  選択ボタン２
          {
            BUTTON_ID_SELECT_02,
            SEQ_SE_DECIDE1,

            pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_003_PANE_PARTS_02 ),
            PANENAME_BVP_BTN_LOW_005_PANE_NULL_CUR,
          },

          //  選択ボタン３
          {
            BUTTON_ID_SELECT_03,
            SEQ_SE_DECIDE1,

            pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_003_PANE_PARTS_03 ),
            PANENAME_BVP_BTN_LOW_005_PANE_NULL_CUR,
          },

          //  きりかえボタン
          {
            BUTTON_ID_CHNAGE_INFO,
            SEQ_SE_DECIDE1,

            pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_003_PANE_PARTS_00 ),
            gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
          }
        };

        const app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
        {
          //  再生ボタン
          {
            aButtonData[ BUTTON_ID_PLAYER ].id,
            aButtonData[ BUTTON_ID_PLAYER ].pRootPane,
            pLayoutWork->GetBoundingPane( aButtonData[ BUTTON_ID_PLAYER ].pRootPane, PANENAME_BVP_BTN_LOW_004_PANE_BOUND_00, &rMultiResId ),
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_04_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_04_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_04_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_04_TOUCH_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_04_ACTIVE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_04_PASSIVE,
          },

          //  選択ボタン１
          {
            aButtonData[ BUTTON_ID_SELECT_01 ].id,
            aButtonData[ BUTTON_ID_SELECT_01 ].pRootPane,
            pLayoutWork->GetBoundingPane( aButtonData[ BUTTON_ID_SELECT_01 ].pRootPane, PANENAME_BVP_BTN_LOW_005_PANE_BOUND_00, &rMultiResId ),
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_01_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_01_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_01_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_01_TOUCH_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_01_ACTIVE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_01_PASSIVE,
          },

          //  選択ボタン２
          {
            aButtonData[ BUTTON_ID_SELECT_02 ].id,
            aButtonData[ BUTTON_ID_SELECT_02 ].pRootPane,
            pLayoutWork->GetBoundingPane( aButtonData[ BUTTON_ID_SELECT_02 ].pRootPane, PANENAME_BVP_BTN_LOW_005_PANE_BOUND_00, &rMultiResId ),
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_02_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_02_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_02_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_02_TOUCH_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_02_ACTIVE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_02_PASSIVE,
          },

          //  選択ボタン３
          {
            aButtonData[ BUTTON_ID_SELECT_03 ].id,
            aButtonData[ BUTTON_ID_SELECT_03 ].pRootPane,
            pLayoutWork->GetBoundingPane( aButtonData[ BUTTON_ID_SELECT_03 ].pRootPane, PANENAME_BVP_BTN_LOW_005_PANE_BOUND_00, &rMultiResId ),
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_03_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_03_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_03_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_03_TOUCH_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_03_ACTIVE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_03_PASSIVE,
          },

          //  切り替えボタン
          {
            aButtonData[ BUTTON_ID_CHNAGE_INFO ].id,
            aButtonData[ BUTTON_ID_CHNAGE_INFO ].pRootPane,
            pLayoutWork->GetBoundingPane( aButtonData[ BUTTON_ID_CHNAGE_INFO ].pRootPane, PANENAME_BVP_BTN_LOW_000_PANE_BOUND_00, &rMultiResId ),
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_TOUCH,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_CANSEL,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_TOUCH_RELEASE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_ACTIVE,
            LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_PASSIVE,
          },
        };

        UIResponder::CreateButtonManager( m_initParam.pHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
        //  入力コールバック設定
        UIView::SetInputListener( this );

        //  ボタンの詳細設定
        {
          app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

          for( u32 i = 0; i < BUTTON_ID_MAX; ++i )
          {
            //  SE設定
            pButtonManager->SetButtonSelectSE( aButtonData[ i ].id, aButtonData[ i ].seId );

            m_aButtonData[ i ].pRootPane    = aButtonData[ i ].pRootPane;
            GFL_ASSERT( m_aButtonData[ i ].pRootPane );

            //  カーソルペイン取得
            if( aButtonData[ i ].cursorPaneIndex != gfl2::lyt::LYTSYS_PANEINDEX_ERROR )
            {
              m_aButtonData[ i ].pCursorPane  = pLayoutWork->GetPane( m_aButtonData[ i ].pRootPane, aButtonData[ i ].cursorPaneIndex, &rMultiResId );
              GFL_ASSERT( m_aButtonData[ i ].pCursorPane );
            }
          }
        }
      }

      //  初回カーソル位置設定
      _SetCursorPos( 0, false );

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
  bool LowDataCtrlView::EndFunc()
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
  void LowDataCtrlView::Update()
  {
    if( m_state == STATE_IDLE )
    {
    }
    else
    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        SetState( STATE_INPUT_STOP );
      }
    }

    m_pMenuCursor->Update();

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowDataCtrlView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderManager, displayNo, i );
      }

      m_pMenuCursor->Draw( m_initParam.pRenderManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   インフォモード設定
    * @param[in] mode 設定するモード
    */
  //------------------------------------------------------------------
  void LowDataCtrlView::SetInfoMode( const InfoMode mode )
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();

    if( mode == INFO_MODE_DETAIL )
    {
      // @fix GFNMCat[1096] 結果モード用の切り替えを無効にしないときれいに切り替わらない
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_ON_KEKKA, false );
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_OFF_KEKKA, true, false );
    }
    else
    if( mode == INFO_MODE_RESULT )
    {
      // @fix GFNMCat[1096] 詳細モード用の切り替えを無効にしないときれいに切り替わらない
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_OFF_KEKKA, false );
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003__PARTS_00_ON_KEKKA, true, false );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void LowDataCtrlView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
      m_pushButtonId  = BUTTON_ID_NONE;

      // @fix GFNMCat[1331] カーソル保存を初期化
      m_activeSelectButtonId = BUTTON_ID_NONE;

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
  }

  //------------------------------------------------------------------
  /**
    * @brief   選択ボタンを非表示
    */
  //------------------------------------------------------------------
  void LowDataCtrlView::SetContentButtonNonVisible()
  {
    u32 loopNum = GFL_NELEMS( ms_aSelectButtonId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      UIView::GetButtonManager()->SetButtonInvisible( ms_aSelectButtonId[ i ] );
    }

    //  選択できるのが一つしかなくなるのでカーソル位置は初期化
    _SetCursorPos( 0, false );
  }

  void LowDataCtrlView::SetContentButtonNonVisible( const ButtonId btnId )
  {
    u32 loopNum = GFL_NELEMS( ms_aSelectButtonId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( ms_aSelectButtonId[ i ] == btnId )
      {
        UIView::GetButtonManager()->SetButtonInvisible( ms_aSelectButtonId[ i ] );
        break;
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタン内容を設定
    * @param[in] buttonId 表示設定するボタンid
    * @param[in: content   設定内容
    */
  //------------------------------------------------------------------
  void LowDataCtrlView::SetContentButton( const ButtonId id, const ButtonContent content, const u32 attachId )
  {
    u32 loopNum = GFL_NELEMS( ms_aSelectButtonId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      ButtonId  chkButtonId  = ms_aSelectButtonId[ i ];
      if( chkButtonId == id )
      {
        //  ボタン表示切り替え
        const u32 aContent[ BUTTON_CONTENT_MAX ] =
        {
          PANENAME_BVP_BTN_LOW_005_PANE_BVP_BUTTONICON_00,
          PANENAME_BVP_BTN_LOW_005_PANE_BVP_BUTTONICON_03,
          PANENAME_BVP_BTN_LOW_005_PANE_BVP_BUTTONICON_02,
          PANENAME_BVP_BTN_LOW_005_PANE_BVP_BUTTONICON_01,
        };

        app::util::G2DUtil* pG2DUtil          = UIView::GetG2DUtil();
        gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
        gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

        u32 contentNum = GFL_NELEMS( aContent );
        for( u32 content_idx = 0; content_idx < contentNum; ++content_idx )
        {
          gfl2::lyt::LytPane* pIconPane = pLayoutWork->GetPane( m_aButtonData[ chkButtonId ].pRootPane, aContent[ content_idx ], &rMultiResId );
          GFL_ASSERT( pIconPane );

          pG2DUtil->SetPaneVisible( pIconPane, false );
          if( ( ButtonContent )content_idx == content )
          {
            //  表示
            pG2DUtil->SetPaneVisible( pIconPane, true );
          }
        }

        UIView::GetButtonManager()->SetButtonVisible( ms_aSelectButtonId[ i ] );
        m_aButtonData[ ms_aSelectButtonId[ i ] ].id = attachId;

        break;
      }
    }

    //  表示するボタン個数に合わせて画面構成を変える
    {
      u32 visibleCount  = 0;
      u32 loop  = GFL_NELEMS( ms_aSelectButtonId );
      for( u32 i = 0; i < loop; ++i )
      {
        if( UIView::GetButtonManager()->IsButtonVisible( ms_aSelectButtonId[ i ] ) == true )
        {
          ++visibleCount;
        }
      }

      app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003_PATERN_00 );
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003_PATERN_01 );

      if( visibleCount <= 2 )
      {
        pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003_PATERN_01 );
      }
      else
      {
        pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003_PATERN_00 );
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return none
  * 
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  * 
  *                 TRIGER判定として使いたい場合は、ここを使わずに
  *                 ボタンマネージャーモードをTRIGGERにして
  *                 OnLayoutPaneEventを使うべき。
  */
  //--------------------------------------------------------------------------------------------
  void LowDataCtrlView::OnLayoutPaneTouchTriggerEvent( const u32 id )
  {
    u32 loopNum = GFL_NELEMS( ms_aCursorPosRootButtonId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( ms_aCursorPosRootButtonId[ i ] == (s32)id )
      {
        if( _SetCursorPos( i, true ) == true )
        {
        }

        break;
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
  app::ui::UIInputListener::ListenerResult LowDataCtrlView::OnLayoutPaneEvent( const u32 id )
  {
    if( m_state != STATE_IDLE )
    {
      return  DISABLE_ACCESS;
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
  app::ui::UIInputListener::ListenerResult LowDataCtrlView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_state != STATE_IDLE )
    {
      return  DISABLE_ACCESS;
    }

    s32 newCursorPos  = m_cursorPos;

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      //  現在アクティブになっているカーソルのidを押す
      UIView::GetButtonManager()->StartSelectedAct( _GetButtonIdFromCursorPos( newCursorPos ) );
    }
    else
    //  上を押した
    if( pKey->IsTrigger( gfl2::ui::BUTTON_UP )
    ||  pKey->IsRepeat( gfl2::ui::BUTTON_UP ) )
    {
      if( newCursorPos == 0 )
      {
        //  再生ボタン位置なら変えない
      }
      else
      {
        _SetCursorMovePos( MOVE_CURSOR_TYPE_SUB );
      }
    }
    else
    //  下を押した
    if( pKey->IsTrigger( gfl2::ui::BUTTON_DOWN )
    ||  pKey->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
    {
      if( newCursorPos == 0 )
      {
        //  再生ボタン位置なら変えない
      }
      else
      {
        _SetCursorMovePos( MOVE_CURSOR_TYPE_ADD );
      }
    }
    else
    //  左を押した
    if( pKey->IsTrigger( gfl2::ui::BUTTON_LEFT )
    ||  pKey->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      if( 0 < newCursorPos )
      {
        //  再生ボタン以外にカーソルがいる
        _ChangeCursorAreaMove();
      }
    }
    else
    //  右を押した
    if( pKey->IsTrigger( gfl2::ui::BUTTON_RIGHT )
    ||  pKey->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      if( 0 == newCursorPos )
      {
        //  再生ボタンにカーソルがいる
        _ChangeCursorAreaMove();
      }
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_CHNAGE_INFO );
    }
    else
    {
      return  ENABLE_ACCESS;
    }

    return  DISABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief    カーソル位置を設定
    * @note     指定した位置になければ設定できる位置に補正
    */
  //------------------------------------------------------------------
  void LowDataCtrlView::_SetCursorPosFit( const s32 pos )
  {
    s32 cursorPosMax  = GFL_NELEMS( ms_aCursorPosRootButtonId );
    s32 newPos        = pos;

    //  進む場合
    for( s32 i = 0; i < cursorPosMax; ++i )
    {
      if( cursorPosMax <= newPos )
      {
        newPos  = 0;
      }

      if( _SetCursorPos( newPos, true ) == true )
      {
        break;
      }
      else
      {
        ++newPos;
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル位置を設定
    * @return   設定成功: true / 失敗: false
    */
  //------------------------------------------------------------------
  bool LowDataCtrlView::_SetCursorPos( const s32 pos, const bool bChangeSE )
  {
    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    app::util::G2DUtil* pG2DUtil          = UIView::GetG2DUtil();

    u32 newPos  = pos;

    ButtonId  id  = _GetButtonIdFromCursorPos( newPos );
    GFL_ASSERT( id != BUTTON_ID_NONE );

    if( pG2DUtil->GetPaneVisible( m_aButtonData[ id ].pRootPane ) == true )
    {
      m_pMenuCursor->Put( pLayoutWork, m_aButtonData[ id ].pCursorPane );

      //  選択エリアのボタンなら保存する
      {
        u32 loopNum = GFL_NELEMS( ms_aSelectButtonId );
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( ms_aSelectButtonId[ i ] == id )
          {
            m_activeSelectButtonId  = id;
            break;
          }
        }
      }

      /*
        ここで初期化するとカーソル保存がなくなる
      // @fix GFNMCat[1331] 選択エリア内のカーソル位置に設定でない場合は選択エリア内でのカーソル位置を初期化
      if( bSelectArea == false )
      {
        m_activeSelectButtonId = BUTTON_ID_NONE;
      }
      */
    }
    else
    {
      return  false;
    }

    if( bChangeSE == true )
    {
      if( m_cursorPos != newPos )
      {
        //  変わっているのでＳＥを再生
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
    }

    m_cursorPos = newPos;

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル位置移動を設定
    */
  //------------------------------------------------------------------
  void LowDataCtrlView::_SetCursorMovePos( const MoveCursorType type )
  {
    s32 cursorPosMax  = GFL_NELEMS( ms_aCursorPosRootButtonId );
    s32 newPos  = m_cursorPos;
    s32 oldPos  = newPos;

    if( type == MOVE_CURSOR_TYPE_SUB )
    {
      --newPos;

      //  戻る場合
      for( s32 i = cursorPosMax - 1; 0 <= i; --i )
      {
        if( newPos < 0 )
        {
          newPos  = cursorPosMax - 1;
        }

        if( newPos == 0 )
        {
          //  再生ボタンへは移動しない
          --newPos;
          continue;
        }

        if( _SetCursorPos( newPos, true ) == true )
        {
          break;
        }
        else
        {
          --newPos;
        }
      }
    }
    else
    if( type == MOVE_CURSOR_TYPE_ADD )
    {
      ++newPos;

      //  進む場合
      for( s32 i = 0; i < cursorPosMax; ++i )
      {
        if( cursorPosMax <= newPos )
        {
          newPos  = 0;
        }

        if( newPos == 0 )
        {
          //  再生ボタンへは移動しない
          ++newPos;
          continue;
        }

        if( _SetCursorPos( newPos, true ) == true )
        {
          break;
        }
        else
        {
          ++newPos;
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル位置のエリア切り替え
    * @note    再生ボタンと選択ボタンを二つのエリアとみなして切り替え
    */
  //------------------------------------------------------------------
  void LowDataCtrlView::_ChangeCursorAreaMove()
  {
    ButtonId  activeButtonId  = _GetButtonIdFromCursorPos( m_cursorPos );
    if( activeButtonId == BUTTON_ID_PLAYER )
    {
      //  再生ボタンのエリア から 選択ボタンのエリアへ
      if( m_activeSelectButtonId == BUTTON_ID_NONE )
      {
        //  デフォルトは選択１のボタンへ
        _SetCursorPosFit( _GetCursorPosFromButtonId( BUTTON_ID_SELECT_01 ) );
      }
      else
      {
        //  以前エリアで移動した場合はそこに移動
        _SetCursorPosFit( _GetCursorPosFromButtonId( m_activeSelectButtonId ) );
      }
    }
    else
    {
      //  選択ボタンのエリア から 再生ボタンのエリアへ
      _SetCursorPosFit( _GetCursorPosFromButtonId( BUTTON_ID_PLAYER ) );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル位置からボタンidを取得
    */
  //------------------------------------------------------------------
  LowDataCtrlView::ButtonId LowDataCtrlView::_GetButtonIdFromCursorPos( const s32 pos )
  {
    if( GFL_NELEMS( ms_aCursorPosRootButtonId ) <= pos )
    {
      GFL_ASSERT( 0 );
      return  BUTTON_ID_NONE;
    }

    return  ms_aCursorPosRootButtonId[ pos ];
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタンidからカーソル位置を取得
    */
  //------------------------------------------------------------------
  s32 LowDataCtrlView::_GetCursorPosFromButtonId( const ButtonId id )
  {
    u32 loopNum = GFL_NELEMS( ms_aCursorPosRootButtonId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( ms_aCursorPosRootButtonId[ i ] == id )
      {
        return  (s32)i;
      }
    }

    GFL_ASSERT( 0 );

    return  0;
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void LowDataCtrlView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
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
        LYTRES_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003_BFLYT,
        LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_003___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
