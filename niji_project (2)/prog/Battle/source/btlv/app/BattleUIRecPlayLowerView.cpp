//======================================================================
/**
 * @file BattleUIRecPlayLowerView.cpp
 * @date 2015/07/28 12:47:28
 * @author uchida_yuto
 * @brief バトル録画再生の下画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "BattleUIRecPlayLowerView.h"

//  nijiのインクルード
#include "AppLib/include/Tool/CommonMessageWindowView.h"

//  素材のインクルード
#include "niji_conv_header/app/battle/res2d/reclow.h"
#include "niji_conv_header/app/battle/res2d/reclow_pane.h"
#include "niji_conv_header/app/battle/res2d/reclow_anim_list.h"

//  arcヘッダーファイル
#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"
#include "arc_index/MenuWindow.gaix"

//  テキストのインクルード
//  バトル用のappテキストのインクルード
#include "niji_conv_header/message/msg_btl_app.h"

//  サウンドのインクルード
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  BattleUIRecPlayLowerView::BattleUIRecPlayLowerView( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    _Clear();

    m_initParam = param;
  }

  BattleUIRecPlayLowerView::~BattleUIRecPlayLowerView()
  {
    GFL_SAFE_DELETE( m_pMenuCursor );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  初期化処理
  * @return true 処理終了 / false 処理が終了していないので再度呼び出す
  * @note
  *         返り値が true になるまで呼び出す
  */
  //--------------------------------------------------------------------------------------------
  bool BattleUIRecPlayLowerView::InitFunc( const SETUP_PARAM& rSetupParam )
  {
    if( m_bInit == true )
    {
      return  true;
    }

    enum
    {
      SEQ_LOAD_START,
      SEQ_LOAD_WAIT,
      SEQ_INIT,
      SEQ_WAIT_LOAD
    };

    //  ファイルロードをする
    if( m_initSeq == SEQ_LOAD_START )
    {
      m_setupParam  = rSetupParam;

      gfl2::heap::HeapBase* pDevHeap  = m_initParam.pUtilHeap->GetDeviceHeap();
      {
        //  録画画面ロード
        m_pFileCtrlState  = GFL_NEW( pDevHeap ) ::app::util::AppUtilFileControlState( ARCID_BATTLE_LOWER_MENU, GARC_battleLowwerMenu_reclow_APPLYT, pDevHeap, false, 128 );
        m_pFileCtrlState->Start();

        //  共通メッセージロード
        m_pCommonMessageWindowFileCtrlState  = GFL_NEW( pDevHeap ) ::app::util::AppUtilFileControlState( ARCID_MENU_WINDOW, GARC_MenuWindow_MenuWindow_applyt_COMP, pDevHeap, true, 128 );
        m_pCommonMessageWindowFileCtrlState->Start();
      }

      ++m_initSeq;
    }

    if( m_initSeq == SEQ_LOAD_WAIT )
    {
      m_pFileCtrlState->Update();
      m_pCommonMessageWindowFileCtrlState->Update();
      if( ( m_pFileCtrlState->IsEnd() == true )
      &&  ( m_pCommonMessageWindowFileCtrlState->IsEnd() == true ) )
      {
        ++m_initSeq;
      }
    }

    if( m_initSeq == SEQ_INIT )
    {
      //  2D画面を生成
      _Init2D();

      //  初回カーソル位置設定
      _SetCursorPos( 0 );

      _SetUpdateSeq( UPDATE_SEQ_PAUSE );

      ++m_initSeq;
    }

    if( m_initSeq == SEQ_WAIT_LOAD )
    {
      bool  bNextSeq  = true;

      if( bNextSeq == true )
      {
        m_initSeq   = 0;
        m_endSeq    = 0;
        m_bInit     = true;

        return  true;
      }
    }

    return  false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  終了処理
  * @return true 処理終了 / false 処理が終了していないので再度呼び出す
  * @note
  *         返り値が true になるまで
  *         この関数を呼び出すとInitFunc()で初期化しないと使えないので注意
  *         使うケースとしては画面をもう使わないときの後処理に使う
  */
  //--------------------------------------------------------------------------------------------
  bool BattleUIRecPlayLowerView::EndFunc()
  {
    if( m_bInit == false )
    {
      return  true;
    }

    enum
    {
      SEQ_INIT  = 0,
      SEQ_WAIT,
      SEQ_END,
    };

    if( m_endSeq == SEQ_INIT )
    {
      ++m_endSeq;
    }
    else
    if( m_endSeq == SEQ_WAIT )
    {
      bool  bNextSeq  = true;
      //  メニューカーソルの破棄が終っているか
      if( m_pMenuCursor->DeleteLayoutWork() == false )
      {
        bNextSeq  = false;
      }

      if( UIView::IsDrawing() == true )
      {
        bNextSeq  = false;
      }

      if( bNextSeq == true )
      {
        ++m_endSeq;
      }
    }
    else
    if( m_endSeq == SEQ_END )
    {
      UIView::ReleaseButtonManager();
      UIView::Delete2D();

      m_pCommonMessageWindowView->RemoveFromSuperView();
      GFL_SAFE_DELETE( m_pCommonMessageWindowView );

      GFL_SAFE_DELETE( m_pCommonMessageWindowFileCtrlState );
      GFL_SAFE_DELETE( m_pFileCtrlState );
      GFL_SAFE_DELETE( m_pMenuCursor );
      
      m_endSeq   = 0;
      m_initSeq  = 0;
      m_bInit    = false;

      return true;
    }

    return false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  画面を開く
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIRecPlayLowerView::Open()
  {
    GFL_ASSERT( m_bInit == true );

    if( m_bOpen == true )
    {
      return;
    }

    m_bOpen = true;
    m_pMenuCursor->StartAnime();

    _SetUpdateSeq( UPDATE_SEQ_IDLE );
    UIView::SetVisible( true );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  画面を閉じる
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIRecPlayLowerView::Close()
  {
    GFL_ASSERT( m_bInit == true );

    UIView::SetVisible( false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIRecPlayLowerView::Update()
  {
    if( m_bInit == false )
    {
      return;
    }

    if( m_updateSeq == UPDATE_SEQ_IDLE )
    {
    }
    else
    if( m_updateSeq == UPDATE_SEQ_INPUT_ANIME )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        //  アニメを待つ
        //  押したボタンidを返す
        m_pushButtonId  = m_tmpPushButtonId;

        //  再び動作させる
        m_tmpPushButtonId = BUTTON_ID_NONE;
        _SetUpdateSeq( UPDATE_SEQ_IDLE );
      }
    }
    else
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {
      
    }

    m_pMenuCursor->Update();

    UIView::Update();
  }

  //  押したボタンidを返す
  BattleUIRecPlayLowerView::ButtonId BattleUIRecPlayLowerView::GetPushButtonId()
  {
    return m_pushButtonId;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIRecPlayLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( m_bInit == false )
    {
      return;
    }

    if( displayNo != gfl2::gfx::CtrDisplayNo::DOWN )
    {
      return;
    }

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WORK_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i );
    }

    //  入力できない時は描画はしない
    if( UIView::IsInputEnabled() == true )
    {
      m_pMenuCursor->Draw( m_initParam.pRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
    }
  }

  //  スキップモードを有効にする
  void BattleUIRecPlayLowerView::SetEnableSkipMode( const bool bFlg )
  {
    if( m_bSkipMode != bFlg )
    {
      if( bFlg == true )
      {
        _SetUpdateSeq( UPDATE_SEQ_SKIP_VIEW );
      }
      else
      {
        _SetUpdateSeq( UPDATE_SEQ_IDLE );
      }

      m_bSkipMode = bFlg;
    }
  }

  //  終了モード
  void BattleUIRecPlayLowerView::SetEnableQuitMode()
  {
    _SetUpdateSeq( UPDATE_SEQ_QUIT_VIEW );
  }

  //  チャプター最大数を取得
  void BattleUIRecPlayLowerView::SetChapterMax( const u16 maxChapter )
  {
    m_maxChapter  = maxChapter;
  }

  //  現在のチャプター数を設定
  void BattleUIRecPlayLowerView::SetChapterNum( const u16 currentChapter )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    //  現在のチャプター数
    u32 keta  = _GetNumberKeta( currentChapter );
    pG2DUtil->SetTextBoxPaneNumber( m_pCurrentChapterNumTexBox, battle_video_player_sys_01, currentChapter, keta, 0, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT );
  }

  //  スキップ設定をしている
  void BattleUIRecPlayLowerView::SetChapterNumSkipColor( const bool bFlg )
  {
    GFL_ASSERT( m_pCurrentChapterNumTexBox );

    ::app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    if( bFlg == true )
    {
      //  色を赤に
      pG2DUtil->SetTextBoxPaneColor( m_pCurrentChapterNumTexBox, gfl2::lyt::ColorU8::RED, gfl2::lyt::ColorU8::RED );
    }
    else
    {
      pG2DUtil->SetTextBoxPaneColor( m_pCurrentChapterNumTexBox, m_defaultCurrentChapterNumTexBoxTopColor, m_defaultCurrentChapterNumTexBoxBottomColor );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    */
  //------------------------------------------------------------------
  ::app::ui::UIInputListener::ListenerResult BattleUIRecPlayLowerView::OnLayoutPaneEvent( const u32 buttonId )
  {
    m_tmpPushButtonId  = (ButtonId)buttonId;

    if( m_tmpPushButtonId == BUTTON_ID_CANCEL )
    {
      _SetUpdateSeq( UPDATE_SEQ_INPUT_ANIME );
      return  DISABLE_ACCESS;
    }
    else
    if( m_tmpPushButtonId == BUTTON_ID_ADVANCE )
    {
      _SetUpdateSeq( UPDATE_SEQ_INPUT_ANIME );
      return  DISABLE_ACCESS;
    }
    else
    if( m_tmpPushButtonId == BUTTON_ID_RETURN )
    {
      _SetUpdateSeq( UPDATE_SEQ_INPUT_ANIME );
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIRecPlayLowerView::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    _SetCursorButtonId( (ButtonId)buttonId );
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力通知用リスナ
    */
  //------------------------------------------------------------------
  ::app::ui::UIInputListener::ListenerResult BattleUIRecPlayLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( UIView::IsButtonAnimation() == true )
    {
      //  ボタンアニメ中はボタン判定はしない
      return  ENABLE_ACCESS;
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      //  決定
      _PushEntry();
      return  DISABLE_ACCESS;
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      //  キャンセル
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_CANCEL );
      return  DISABLE_ACCESS;
    }
    else
    if( pKey->IsTrigger( gfl2::ui::DIR_LEFT ))
    {
      //  左に移動
      _SetCursorMove( -1 );
      return  DISABLE_ACCESS;
    }
    else
    if( pKey->IsTrigger( gfl2::ui::DIR_RIGHT ) )
    {
      //  右に移動
      _SetCursorMove( 1 );
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  2D初期化処理
  * @note
  *     レイアウトデータから描画に必要な生成をしている
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIRecPlayLowerView::_Init2D()
  {
    //  共通メッセージを作成
    {
      App::Tool::CommonMessageWindowView::PARAM param( m_initParam.pUtilHeap, m_initParam.pRenderingManager );
      m_pCommonMessageWindowView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowView( param );
      m_pCommonMessageWindowView->Setup( m_pCommonMessageWindowFileCtrlState->GetLoadDataBuffer() );
    }

    //  画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData aResTbl[] =
      {
        {
          m_pFileCtrlState->GetLoadDataBuffer(),
          8,
          ::app::util::G2DUtil::ATTACH_NONE
        },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
      {
        {
          0,
          LYTRES_RECLOW_BVP_MIN_LOW_001_BFLYT,
          LA_RECLOW_BVP_MIN_LOW_001___NUM,
          m_pFileCtrlState->GetLoadDataBuffer(),
          true,
          ::app::util::G2DUtil::SETUP_LOWER,
          true
        },
      };

      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), m_initParam.pMsgData, m_initParam.pWordSet );

      UIView::SetVisible( false );
    }

    ::app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );
    GFL_ASSERT( pLayoutWork );

    //  カーソル作成
    {
      m_pMenuCursor = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::MenuCursor( m_initParam.pUtilHeap, &m_setupParam.pMenuCursorLytBuffer );
      GFL_ASSERT( m_pMenuCursor );

      m_pMenuCursor->CreateLayoutWork( UIView::GetLayoutSystem(), UIView::GetLytSetup(gfl2::lyt::DISPLAY_LOWER), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_SMALL );
    }

    //  必要なペインを取得
    {
      m_pCurrentChapterNumTexBox  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_LOW_001_PANE_TEXTBOX_00 );
      GFL_ASSERT( m_pCurrentChapterNumTexBox );

      m_pChapterNumSlashTexBox    = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_LOW_001_PANE_TEXTBOX_02 );
      GFL_ASSERT( m_pChapterNumSlashTexBox );

      m_pMaxChapterNumTexBox      = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_LOW_001_PANE_TEXTBOX_01 );
      GFL_ASSERT( m_pChapterNumSlashTexBox );
    }

    //  必要なボタンを設定
    {
      struct SETUP_BTN_DATA
      {
        gfl2::lyt::LytParts*  pPartsPane;
        u32 seId;
      };

      SETUP_BTN_DATA  aSetupBtnData[ BUTTON_ID_MAX ] =
      {
        //  BUTTON_ID_CANCEL
        {
          pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_001_PANE_BTN_BACK ), SEQ_SE_CANCEL1
        },

        //  BUTTON_ID_ADVANCE
        {
          pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_001_PANE_PARTS_02 ), SEQ_SE_DECIDE1
        },

        //  BUTTON_ID_RETURN
        {
          pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_LOW_001_PANE_PARTS_01 ), SEQ_SE_DECIDE1
        },
      };

      //  固定テキスト設定
      {
        gfl2::lyt::LytTextBox* pTextBox  = NULL;

        //  進む
        {
          pTextBox  = pLayoutWork->GetTextBoxPane( aSetupBtnData[ BUTTON_ID_ADVANCE ].pPartsPane, PANENAME_BVP_BTN_LOW_003_PANE_TEXTBOX_00, &rResourceID );
          pG2DUtil->SetTextBoxPaneString( pTextBox, battle_video_player_sel_02 );
        }

        //  戻る
        {
          pTextBox  = pLayoutWork->GetTextBoxPane( aSetupBtnData[ BUTTON_ID_RETURN ].pPartsPane, PANENAME_BVP_BTN_LOW_003_PANE_TEXTBOX_00, &rResourceID );
          pG2DUtil->SetTextBoxPaneString( pTextBox, battle_video_player_sel_01 );
        }
      }

      //  再生ボタンの矢印設定
      {
        gfl2::lyt::LytPane* pArroyPane  = NULL;

        //  進む
        {
          pArroyPane  = pLayoutWork->GetPane( aSetupBtnData[ BUTTON_ID_ADVANCE ].pPartsPane, PANENAME_BVP_BTN_LOW_003_PANE_BVP_BUTTONICON_00, &rResourceID );
          pG2DUtil->SetPaneVisible( pArroyPane, true );
        }

        //  戻る
        {
          pArroyPane  = pLayoutWork->GetPane( aSetupBtnData[ BUTTON_ID_RETURN ].pPartsPane, PANENAME_BVP_BTN_LOW_003_PANE_BVP_BUTTONICON_01, &rResourceID );
          pG2DUtil->SetPaneVisible( pArroyPane, true );
        }
      }

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
        //  Bキャンセル
        {
          BUTTON_ID_CANCEL,
          aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_RECLOW_BVP_MIN_LOW_001__BTN_BACK_TOUCH,
          LA_RECLOW_BVP_MIN_LOW_001__BTN_BACK_RELEASE,
          LA_RECLOW_BVP_MIN_LOW_001__BTN_BACK_CANSEL,
          LA_RECLOW_BVP_MIN_LOW_001__BTN_BACK_TOUCH_RELEASE,
          LA_RECLOW_BVP_MIN_LOW_001__BTN_BACK_ACTIVE,
          LA_RECLOW_BVP_MIN_LOW_001__BTN_BACK_PASSIVE,
        },

        //  進める
        {
          BUTTON_ID_ADVANCE,
          aSetupBtnData[ BUTTON_ID_ADVANCE ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_ADVANCE ].pPartsPane, PANENAME_BVP_BTN_LOW_003_PANE_BOUND_00, &rResourceID ),
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_02_TOUCH,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_02_RELEASE,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_02_CANSEL,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_02_TOUCH_RELEASE,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_02_ACTIVE,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_02_PASSIVE,
        },

        //  戻す
        {
          BUTTON_ID_RETURN,
          aSetupBtnData[ BUTTON_ID_RETURN ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_RETURN ].pPartsPane, PANENAME_BVP_BTN_LOW_003_PANE_BOUND_00, &rResourceID ),
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_01_TOUCH,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_01_RELEASE,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_01_CANSEL,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_01_TOUCH_RELEASE,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_01_ACTIVE,
          LA_RECLOW_BVP_MIN_LOW_001__PARTS_01_PASSIVE,
        },
      };

      //  ボタン作成
      UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
      //  入力コールバック設定
      UIView::SetInputListener( this );

      //  ボタンSEの設定
      {
        ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

        u32 loopNum = GFL_NELEMS( aSetupBtnData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pButtonManager->SetButtonSelectSE( i, aSetupBtnData[ i ].seId );
        }
      }

      //  カーソルのボタンデータ作成
      {
        const MENU_CURSOR_POS_DATA  aSettingMenuCursorPosData[ MENU_CURSOR_POS_MAX ] =
        {
          {
            pLayoutWork->GetPane( aSetupBtnData[ BUTTON_ID_RETURN ].pPartsPane, PANENAME_BVP_BTN_LOW_003_PANE_NULL_CUR, &rResourceID ),
            BUTTON_ID_RETURN
          },

          {
            pLayoutWork->GetPane( aSetupBtnData[ BUTTON_ID_ADVANCE ].pPartsPane, PANENAME_BVP_BTN_LOW_003_PANE_NULL_CUR, &rResourceID ),
            BUTTON_ID_ADVANCE
          },
        };

        GFL_ASSERT( sizeof( aSettingMenuCursorPosData ) == sizeof( m_aMenuCursorPosData ) );
        gfl2::std::MemCopy( aSettingMenuCursorPosData, m_aMenuCursorPosData, sizeof( aSettingMenuCursorPosData ) );
      }
    }

    UIView::AddSubView( m_pCommonMessageWindowView );
    m_pCommonMessageWindowView->SetVisible( false );

    //  最大チャプター数
    {
      u32 keta  = _GetNumberKeta( m_maxChapter );
      pG2DUtil->SetTextBoxPaneNumber( m_pMaxChapterNumTexBox, battle_video_player_sys_01, m_maxChapter, keta );
    }

    //  現在チャプター数の色取得
    {
      pG2DUtil->GetTextBoxPaneColor( m_pCurrentChapterNumTexBox, &m_defaultCurrentChapterNumTexBoxTopColor, &m_defaultCurrentChapterNumTexBoxBottomColor );
    }
  }

  //  更新シーケンス設定
  void BattleUIRecPlayLowerView::_SetUpdateSeq( const UpdateSeq newSeq )
  {
    m_updateSeq = newSeq;

    if( m_updateSeq == UPDATE_SEQ_IDLE )
    {
      m_pCommonMessageWindowView->HideMessage();

      //  ボタンを入力を開始
      UIView::SetInputEnabledAllView( true );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_INPUT_ANIME )
    {
      //  ボタンを入力を停止
      UIView::SetInputEnabledAllView( false );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {
      UIView::SetInputEnabledAllView( false );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_QUIT_VIEW )
    {
      //  ウィンドウ表示
      {
        gfl2::str::StrBuf*  pText = UIView::GetG2DUtil()->GetTempStrBuf( 0 );
        m_initParam.pMsgData->GetString( battle_video_player_win_02, *pText );
        m_pCommonMessageWindowView->SetVisible( true );
        m_pCommonMessageWindowView->ShowMessage( pText, true, true, true );
      }

      UIView::SetInputEnabledAllView( false );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_SKIP_VIEW )
    {
      //  ウィンドウ表示
      {
        gfl2::str::StrBuf*  pText = UIView::GetG2DUtil()->GetTempStrBuf( 0 );
        m_initParam.pMsgData->GetString( battle_video_player_win_01, *pText );
        m_pCommonMessageWindowView->SetVisible( true );
        m_pCommonMessageWindowView->ShowMessage( pText, true, true, true );
      }

      //  スキップ画面
      UIView::SetInputEnabledAllView( false );

      m_tmpPushButtonId = m_pushButtonId  = BUTTON_ID_NONE;
    }
  }

  //  カーソル移動
  void BattleUIRecPlayLowerView::_SetCursorMove( const s32 move )
  {
    s32 maxNum = GFL_NELEMS( m_aMenuCursorPosData );
    s32 nowCursorPos  = (s32)m_menuCursorPos;

    nowCursorPos  = gfl2::math::Clamp<s32>( nowCursorPos + move, 0, maxNum - 1 );

    if( _SetCursorPos( (u32)nowCursorPos ) == true )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );
    }
  }

  //  カーソル位置設定
  bool BattleUIRecPlayLowerView::_SetCursorPos( const u32 pos )
  {
    ::app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );

    bool  bMove = ( m_menuCursorPos != pos );

    m_menuCursorPos = pos;
    GFL_ASSERT( m_menuCursorPos < GFL_NELEMS( m_aMenuCursorPosData ) );
    m_pMenuCursor->Put( pLayoutWork, m_aMenuCursorPosData[ m_menuCursorPos ].pPosPane );

    //  カーソル位置が変わった
    return  bMove;
  }

  //  カーソル位置設定(ボタンid版)
  void BattleUIRecPlayLowerView::_SetCursorButtonId( const ButtonId buttonId )
  {
    u32 loopNum = GFL_NELEMS( m_aMenuCursorPosData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_aMenuCursorPosData[ i ].buttonId == buttonId )
      {
        _SetCursorPos( i );
        return;
      }
    }
  }

  //  決定
  void BattleUIRecPlayLowerView::_PushEntry()
  {
    UIView::GetButtonManager()->StartSelectedAct( m_aMenuCursorPosData[ m_menuCursorPos ].buttonId );
  }

  //  数字の桁数取得
  u32 BattleUIRecPlayLowerView::_GetNumberKeta( const u32 num )
  {
    u32 keta = 1;
    u32 tmpNum = num;

    while( 10 <= tmpNum )
    {
      ++keta;
      tmpNum /= 10;
    }

    return  keta;
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
