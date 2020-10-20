//=============================================================================
/**
 * @file BattleUIYesNoLowerDraw.cpp
 * @brief バトル下画面 >> 「二者択一」画面
 * @author yuto_uchida
 * @date 2015.03.04
 */
//=============================================================================
#include "BattleUIYesNoLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/ysnlow.h"
#include "niji_conv_header/app/battle/res2d/ysnlow_pane.h"
#include "niji_conv_header/app/battle/res2d/ysnlow_anim_list.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIYesNoLowerDraw::BattleUIYesNoLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );
    GFL_ASSERT( param.pBtlAppMsgData );
    GFL_ASSERT( param.pBtlWordSet );
    GFL_ASSERT( param.pMenuCursorLytBuff );

    _Clear();

    m_initParam = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIYesNoLowerDraw::~BattleUIYesNoLowerDraw()
  {
    GFL_SAFE_DELETE( m_pMenuCursor );

    {
      for( s32 i = 0; i < GFL_NELEMS( m_pSelectItemList ); ++i )
      {
        GFL_SAFE_DELETE( m_pSelectItemList[ i ] );
      }
    }

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 初期化
    */
  //-------------------------------------------------------------------------
  bool BattleUIYesNoLowerDraw::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBuff );

      //  選択項目初期化
      {
        gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

        SelectItem::INIT_PARAM  aInitParam[ SELECT_ITEM_MAX ]  =
        {
          /*
            //  基本設定
            SELECT_ITEM_CONFIG_PARAM r_config_param;
            //  ボタン登録データ
            ::app::ui::ButtonInfoEx   button_info_ex;
            //  レイアウト
            gfl2::lyt::LytWk*         p_layout_work;
            //  参照するリソース
            gfl2::lyt::LytMultiResID* p_lyt_muitl_resId;
            //  2d制御
            ::app::util::G2DUtil*     p_g2d_util;
          */
          {
            {
              PANENAME_BATTLE_YSN_LOW_000_PANE_BTN_00,
            },

            {
              0,
              NULL,
              NULL,

              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_00_TOUCH,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_00_RELEASE,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_00_CANSEL,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_00_TOUCH_RELEASE,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_00_ACTIVE,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_00_PASSIVE,
            },

            pLayoutWork,
            &UIView::GetG2DUtil()->GetLayoutResourceID( 0 ),
            UIView::GetG2DUtil(),
          },

          {
            {
              PANENAME_BATTLE_YSN_LOW_000_PANE_BTN_01,
            },

            //
            {
              1,
              NULL,
              NULL,

              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_01_TOUCH,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_01_RELEASE,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_01_CANSEL,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_01_TOUCH_RELEASE,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_01_ACTIVE,
              LA_YSNLOW_BATTLE_YSN_LOW_000__BTN_01_PASSIVE
            },

            pLayoutWork,
            &UIView::GetG2DUtil()->GetLayoutResourceID( 0 ),
            UIView::GetG2DUtil(),
          },
        };

        for( s32 i = 0; i < GFL_NELEMS( m_pSelectItemList ); ++i )
        {
          m_pSelectItemList[ i ]  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) SelectItem( aInitParam[ i ] );
          m_pSelectItemList[ i ]->SetText( m_initParam.aItemTextIdList[ i ], this );
        }
      }

      //  ボタン関連の設定
      {
        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

        const ::app::ui::ButtonInfoEx aButtonPaneIdxList[] =
        {
          m_pSelectItemList[ 0 ]->GetInitParam().button_info_ex,
          m_pSelectItemList[ 1 ]->GetInitParam().button_info_ex,
        };

        UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN ), aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );

        //  入力コールバック設定
        UIView::SetInputListener( this );
      }

      //  ボタンSE設定
      if( m_initParam.bUseInputSE )
      {
        ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

        u32 loopNum = GFL_NELEMS( m_pSelectItemList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pButtonManager->SetButtonSelectSE( m_pSelectItemList[ i ]->GetInitParam().button_info_ex.button_id, SEQ_SE_DECIDE1 );
        }
      }

      //  カーソル作成
      {
        //  すでにリソースバッファを作成して持っているのでロードは不要
        m_pMenuCursor = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::MenuCursor( m_initParam.pUtilHeap, &m_initParam.pMenuCursorLytBuff );
        GFL_ASSERT( m_pMenuCursor );

        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
        m_pMenuCursor->CreateLayoutWork( UIView::GetLayoutSystem(), UIView::GetLytSetup( gfl2::lyt::DISPLAY_LOWER ), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_SMALL );
      }

      //  初回選択位置設定
      {
        _SetActiveSelectItemCursor( m_initParam.selectItemIdx );
      }

      UIView::SetVisible( false );

      ++m_initSeq;
    }

    if( m_initSeq == 1 )
    {
      return  true;
    }

    return  false;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 終了
    */
  //-------------------------------------------------------------------------
  bool BattleUIYesNoLowerDraw::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bRet  = true;
      if( m_pMenuCursor->DeleteLayoutWork() == false )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++m_endSeq;
      }
    }

    if( m_endSeq == 1 )
    {
      if( UIView::IsDrawing() == false )
      {
        return  true;
      }
    }

    return  false;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 指定した選択項目をプッシュ
    */
  //-------------------------------------------------------------------------
  void BattleUIYesNoLowerDraw::PushActiveSelectItem( const s32 selectItemIdx )
  {
    GFL_ASSERT( selectItemIdx < GFL_NELEMS( m_pSelectItemList ) );

    if( m_initParam.pDelegate != NULL )
    {
      m_initParam.pDelegate->OnPushSelectItem( selectItemIdx );
    }

    //  現在選択項目とは別の項目を指定しているかもしれないのでまずacitve設定を先に
    _SetActiveSelectItemCursor( selectItemIdx );

    //  プッシュアニメさせる
    {
      UIResponder::GetButtonManager()->StartSelectedAct( m_pSelectItemList[ selectItemIdx ]->GetInitParam().button_info_ex.button_id );
    }
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 上カーソル移動
    */
  //-------------------------------------------------------------------------
  bool BattleUIYesNoLowerDraw::MoveUpCursor()
  {
    s32 nowCursorIdx  = m_initParam.selectItemIdx;

    --nowCursorIdx;
    if( nowCursorIdx < 0 )
    {
      nowCursorIdx  = SELECT_ITEM_MAX - 1;
    }

    m_initParam.selectItemIdx = nowCursorIdx;
    _SetActiveSelectItemCursor( m_initParam.selectItemIdx );

    return  true;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 下カーソル移動
    */
  //-------------------------------------------------------------------------
  bool BattleUIYesNoLowerDraw::MoveDownCursor()
  {
    s32 nowCursorIdx  = m_initParam.selectItemIdx;

    ++nowCursorIdx;
    if( SELECT_ITEM_MAX <= nowCursorIdx )
    {
      nowCursorIdx  = 0;
    }

    m_initParam.selectItemIdx = nowCursorIdx;
    _SetActiveSelectItemCursor( m_initParam.selectItemIdx );

    return  true;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIYesNoLowerDraw::Update( void )
  {
    _UpdateState();

    m_pMenuCursor->Update();
    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIYesNoLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }

    if( UIView::IsInputEnabled() == true )
    {
      m_pMenuCursor->Draw( m_initParam.pRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER, LOW_VIEW_ORDER_OTHER );
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
  ::app::ui::UIInputListener::ListenerResult BattleUIYesNoLowerDraw::OnLayoutPaneEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneEvent : buttonId = %d\n", buttonId );

    if( m_initParam.pDelegate == NULL )
    {
      return  ENABLE_ACCESS;
    }

    //  項目を選択した
    //  現在選択項目とは別の項目を指定しているかもしれないのでまずacitve設定を先に
    if( m_initParam.pDelegate->OnPushSelectItem( buttonId ) )
    {
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
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
  void BattleUIYesNoLowerDraw::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneTouchTriggerEvent : buttonId = %d\n", buttonId );

    //  カーソル移動
    _SetActiveSelectItemCursor( buttonId );
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
  ::app::ui::UIInputListener::ListenerResult BattleUIYesNoLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

    m_initParam.pDelegate->OnKeyAction( this, pButton, pKey, pStick );

    return ENABLE_ACCESS;
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUIYesNoLowerDraw::_Initialize2D( void* pAppLytBuff )
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
            LYTRES_YSNLOW_BATTLE_YSN_LOW_000_BFLYT,
            LA_YSNLOW_BATTLE_YSN_LOW_000___NUM,
            pAppLytBuff,
            true,
            ::app::util::G2DUtil::SETUP_LOWER,
            true
          },
      };

      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), m_initParam.pBtlAppMsgData, m_initParam.pBtlWordSet );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIYesNoLowerDraw::_UpdateState()
  {
    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      _SetState( STATE_TYPE_INPUT_IDLE );
    }

    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }

    if( m_state == STATE_TYPE_OUT )
    {
      _SetState( STATE_TYPE_END );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIYesNoLowerDraw::_SetState( const StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );
    }
    else if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }
    else if( m_state == STATE_TYPE_OUT )
    {
      //  カーソルを非表示
      m_pMenuCursor->SetVisible( false );
    }
    else if( m_state ==  STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   選択項目のカーソルactive設定
    */
  //------------------------------------------------------------------
  void BattleUIYesNoLowerDraw::_SetActiveSelectItemCursor( const s32 selectItemIdx )
  {
    GFL_ASSERT( m_pMenuCursor );
    GFL_ASSERT( selectItemIdx < GFL_NELEMS( m_pSelectItemList ) );

    m_initParam.selectItemIdx = selectItemIdx;
    for( s32 i = 0; i < GFL_NELEMS( m_pSelectItemList ); ++i )
    {
      if( i == selectItemIdx )
      {
        //  カーソル位置設定
        m_pMenuCursor->StartAnime();
        m_pMenuCursor->Put( UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN ), m_pSelectItemList[ i ]->GetCursorPane() );

        m_pSelectItemList[ i ]->SetActive( true );
      }
      else
      {
        m_pSelectItemList[ i ]->SetActive( false );
      }
    }
  }

  /** ここから 選択項目オブジェクトクラスの定義 */

  //------------------------------------------------------------------
  /**
    * @brief  コンストラクタ
    */
  //------------------------------------------------------------------
  BattleUIYesNoLowerDraw::SelectItem::SelectItem( const INIT_PARAM& rInitParam )
  {
    {
      m_buttonInfo.button_id  = 0;
      m_buttonInfo.picture_pane = NULL;
      m_buttonInfo.bound_pane   = NULL;
      m_buttonInfo.touch_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;       
      m_buttonInfo.release_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;     
      m_buttonInfo.cancel_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;      
      m_buttonInfo.key_select_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;  
      m_buttonInfo.active_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;      
      m_buttonInfo.passive_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;     
    }

    GFL_ASSERT( rInitParam.p_g2d_util );
    GFL_ASSERT( rInitParam.p_layout_work );
    GFL_ASSERT( rInitParam.p_lyt_muitl_resId );

    //  部品ペインを辿ってテキストペインを取得して表示設定
    m_pPartsPane        = rInitParam.p_layout_work->GetPartsPane( rInitParam.r_config_param.root_pane_id );
    GFL_ASSERT( m_pPartsPane );

    m_pCursorNullPane        = rInitParam.p_layout_work->GetPane( m_pPartsPane, PANENAME_COMMON_BTN_LOW_005_PANE_NULL_CUR, rInitParam.p_lyt_muitl_resId );
    GFL_ASSERT( m_pCursorNullPane );

    m_pTextBox   = rInitParam.p_layout_work->GetTextBoxPane( m_pPartsPane, PANENAME_COMMON_BTN_LOW_005_PANE_TEXTBOX_00, rInitParam.p_lyt_muitl_resId );
    GFL_ASSERT( m_pTextBox );

    m_initParam = rInitParam;

    m_bActive = true;
    SetActive( false );

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
      m_initParam.button_info_ex.picture_pane = m_pPartsPane;
      m_initParam.button_info_ex.bound_pane    = rInitParam.p_layout_work->GetBoundingPane( m_pPartsPane, PANENAME_COMMON_BTN_LOW_005_PANE_BOUND_00, rInitParam.p_lyt_muitl_resId );
      GFL_ASSERT( m_initParam.button_info_ex.bound_pane );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief  テキスト設定
    */
  //------------------------------------------------------------------
  void BattleUIYesNoLowerDraw::SelectItem::SetText( const s32 messageID, UIView* pView )
  {
    GFL_ASSERT( pView );
    pView->SetTextboxPaneMessage( m_pTextBox, messageID );
  }

  //------------------------------------------------------------------
  /**
    * @brief  アクティブ設定
    */
  //------------------------------------------------------------------
  void BattleUIYesNoLowerDraw::SelectItem::SetActive( bool bFlg )
  {
    m_bActive = bFlg;
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
