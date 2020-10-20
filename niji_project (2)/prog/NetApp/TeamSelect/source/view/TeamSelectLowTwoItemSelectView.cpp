//======================================================================
/**
 * @file TeamSelectLowTwoItemSelectView.cpp
 * @date 2015/07/02 19:29:11
 * @author uchida_yuto
 * @brief 2選択下画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/View/TeamSelectLowTwoItemSelectView.h"

#include "Applib/include/Tool/app_tool_MenuCursor.h"

#include "niji_conv_header/app/teamselect/res2d/teamSelect.h"
#include "niji_conv_header/app/teamselect/res2d/teamSelect_anim_list.h"
#include "niji_conv_header/app/teamselect/res2d/teamSelect_pane.h"

#include "niji_conv_header/message/msg_battleteam_select.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  TeamSelectLowTwoItemSelectView::TeamSelectLowTwoItemSelectView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager ) :
  UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( pRenderManager );
    m_pHeap           = pHeap;
    m_pRenderManager  = pRenderManager;
  }

  TeamSelectLowTwoItemSelectView::~TeamSelectLowTwoItemSelectView()
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
  void TeamSelectLowTwoItemSelectView::Initialize( void* pLytBinData, void* pLytMenuCursorBinData, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    GFL_ASSERT( pLytBinData );
    GFL_ASSERT( pAppMsgData );
    GFL_ASSERT( pWordSet );
    GFL_ASSERT( pLytMenuCursorBinData );

    _Initialize2D( pLytBinData, pAppMsgData, pWordSet );

    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    app::util::G2DUtil* pG2DUtil          = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  カーソル作成
    {
      m_pMenuCursor = GFL_NEW( m_pHeap->GetDeviceHeap() ) app::tool::MenuCursor( m_pHeap, &pLytMenuCursorBinData );

      m_pMenuCursor->CreateLayoutWork( pG2DUtil->GetLayoutSystem(), pG2DUtil->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_SMALL );
      m_pMenuCursor->SetVisible( false );
    }

    //  ボタン関連の設定
    {
      struct ButtonData aItemBtnData[ BUTTON_ID_MAX ]  = 
      {
        { pLayoutWork->GetPartsPane( PANENAME_TEAMSELECT_SLC_LOW_000_PANE_BUTTONS_00 ), NULL },
        { pLayoutWork->GetPartsPane( PANENAME_TEAMSELECT_SLC_LOW_000_PANE_BUTTONS_01 ), NULL }
      };

      //  テキストペイン取得
      {
        u32 loopNum = GFL_NELEMS( aItemBtnData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          aItemBtnData[ i ].pTextboxPane  = pLayoutWork->GetTextBoxPane( aItemBtnData[ i ].pItemBtnPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &rMultiResId );
        }
      }

      gfl2::std::MemCopy( aItemBtnData, m_aButtonData, sizeof( m_aButtonData ) );

      const ::app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
      {
        //  項目1ボタン
        {
          BUTTON_ID_ONE_SELECT,
          aItemBtnData[ BUTTON_ID_ONE_SELECT ].pItemBtnPartsPane,
          pLayoutWork->GetBoundingPane( aItemBtnData[ BUTTON_ID_ONE_SELECT ].pItemBtnPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &rMultiResId ),
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_00_TOUCH,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_00_RELEASE,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_00_CANSEL,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_00_TOUCH_RELEASE,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_00_ACTIVE,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_00_PASSIVE
        },

        //  項目2ボタン
        {
          BUTTON_ID_TWO_SELECT,
          aItemBtnData[ BUTTON_ID_TWO_SELECT ].pItemBtnPartsPane,
          pLayoutWork->GetBoundingPane( aItemBtnData[ BUTTON_ID_TWO_SELECT ].pItemBtnPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &rMultiResId ),
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_01_TOUCH,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_01_RELEASE,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_01_CANSEL,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_01_TOUCH_RELEASE,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_01_ACTIVE,
          LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000__BUTTONS_01_PASSIVE
        }
      };

      UIResponder::CreateButtonManager( m_pHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
      //  入力コールバック設定
      UIView::SetInputListener( this );

      {
        app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

        u32 loopNum = GFL_NELEMS( aButtonPaneIdxList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pButtonManager->SetButtonSelectSE( BUTTON_ID_ONE_SELECT + i, SEQ_SE_DECIDE1 );
        }
      }
    }

    SetCursorPos( BUTTON_ID_ONE_SELECT );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void TeamSelectLowTwoItemSelectView::Terminate()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   破棄可能か
    */
  //------------------------------------------------------------------
  bool TeamSelectLowTwoItemSelectView::IsDelete()
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
  void TeamSelectLowTwoItemSelectView::Update()
  {
    if( m_state == STATE_IDLE )
    {
    }

    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        //  ボタンアニメ終了

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
  void TeamSelectLowTwoItemSelectView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WORK_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderManager, displayNo, i );
    }

    //  入力できない時は描画はしない
    if( UIView::IsInputEnabled() == true )
    {
      m_pMenuCursor->Draw( m_pRenderManager,displayNo, gfl2::lyt::DISPLAY_LOWER );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult TeamSelectLowTwoItemSelectView::OnLayoutPaneEvent( const u32 id )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    //  入力演出待ちに（このときはすべての入力はOFF）
    m_pushButtonId  = (ButtonId)id;

    SetState( STATE_INPUT_EVENT_WAIT );

    return  DISABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  *       【ボタンマネージャー関係】
  *
  * @param[in] id ボタンid
  *
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  * 
  *                 TRIGER判定として使いたい場合は、ここを使わずに
  *                 ボタンマネージャーモードをTRIGGERにして
  *                 OnLayoutPaneEventを使うべき。
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectLowTwoItemSelectView::OnLayoutPaneTouchTriggerEvent( const u32 id )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return;
    }

    SetCursorPos( (ButtonId)id );
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
  app::ui::UIInputListener::ListenerResult TeamSelectLowTwoItemSelectView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( m_selectIdx );
      return  DISABLE_ACCESS;
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) == true )
    {
      // @fix GFNMCat[2145] Bを押したら２番目を強制的に押せるようにする
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_TWO_SELECT );
      return  DISABLE_ACCESS;
    }
    else
    if( ( m_selectIdx < ( BUTTON_ID_MAX - 1 ) ) && ( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) == true ) )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );

      ++m_selectIdx;
      SetCursorPos( (ButtonId)m_selectIdx );
    }
    else
    if( ( ( BUTTON_ID_MAX - 1 ) <= m_selectIdx ) && ( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) == true ) )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );

      m_selectIdx = 0;
      SetCursorPos( (ButtonId)m_selectIdx );
    }
    else
    if( ( 0 < m_selectIdx ) && ( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) == true ) )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );

      --m_selectIdx;
      SetCursorPos( (ButtonId)m_selectIdx );
    }
    else
    if( ( m_selectIdx <= 0 ) && ( pButton->IsTrigger( gfl2::ui::BUTTON_UP ) == true ) )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );

      m_selectIdx = BUTTON_ID_MAX - 1;
      SetCursorPos( (ButtonId)m_selectIdx );
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void TeamSelectLowTwoItemSelectView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
      m_lastPushButtonId  = BUTTON_ID_NONE;
      m_pushButtonId      = BUTTON_ID_NONE;

      //  カーソル設定
      {
        m_pMenuCursor->SetVisible( true );
      }

      UIView::SetInputEnabledAllView( true );
    }
    else
    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      //  親がいるので親の入力もOFF
      if( UIView::GetSuperView() != NULL )
      {
        UIView::GetSuperView()->SetInputEnabledAllView( false );
      }
    }
    else
    if( m_state == STATE_INPUT_STOP )
    {
      m_lastPushButtonId  = m_pushButtonId;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル位置をボタンに合わせる
    * @param[in]  btnId カーソル位置を合わせるボタンid
    */
  //------------------------------------------------------------------
  void TeamSelectLowTwoItemSelectView::SetCursorPos( const ButtonId btnId )
  {
    GFL_ASSERT( btnId < GFL_NELEMS( m_aButtonData ) );

    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    app::util::G2DUtil* pG2DUtil          = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    gfl2::lyt::LytPane* pCurPane  = pLayoutWork->GetPane( m_aButtonData[ btnId ].pItemBtnPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR, &rMultiResId );
    GFL_ASSERT( pCurPane );

    //  カーソル配置
    m_pMenuCursor->Put( pLayoutWork, pCurPane );

    //  カーソルアニメ開始
    m_pMenuCursor->StartAnime();

    m_selectIdx = (s32)btnId;
  }

  //------------------------------------------------------------------
  /**
    * @brief    ボタンのテキスト設定
    * @param[in]  btnid テキスト設定するボタンid
    * @param[in]  msgId 設定するテキストid
    */
  //------------------------------------------------------------------
  void TeamSelectLowTwoItemSelectView::SetTextBtn( const ButtonId btnId, const u32 msgId )
  {
    GFL_ASSERT( btnId < GFL_NELEMS( m_aButtonData ) );

    ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();

    //  テキスト設定
    pG2DUtil->SetTextBoxPaneString( m_aButtonData[ btnId ].pTextboxPane, msgId );
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void TeamSelectLowTwoItemSelectView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_TEAMSELECT_TEAMSELECT_SLC_LOW_000_BFLYT,
        LA_TEAMSELECT_TEAMSELECT_SLC_LOW_000___NUM,
        pAppLytBuff,
        true,
        ::app::util::G2DUtil::SETUP_LOWER,
        true
      }
    };

    UIView::Create2D( m_pHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
