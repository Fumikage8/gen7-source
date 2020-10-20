//=============================================================================
/**
 * @file BattleUISkillSelectLowerDraw.cpp
 * @brief バトル下画面 >> 「技選択画面」画面
 * @author yuto_uchida
 * @date 2015.03.13
 */
//=============================================================================
#include "BattleUISkillSelectLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

//  nijiのインクルード
#include "AppLib/include/Util/app_util_Common.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "AppLib/include/Tool/CommonMessageObject.h"
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  サウンドのインクルード
#include <Sound/include/Sound.h>
#include "niji_conv_header/sound/SoundMiddleID.h"

//  リソースのインクルード
#include "niji_conv_header/app/battle/res2d/skllow.h"
#include "niji_conv_header/app/battle/res2d/skllow_pane.h"
#include "niji_conv_header/app/battle/res2d/skllow_anim_list.h"
#include "niji_conv_header/message/msg_btl_app.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  const BattleUISkillSelectLowerDraw::CursorType  BattleUISkillSelectLowerDraw::m_saListCursorType[] =
  {
    CURSOR_POS_TYPE_SKILL_00,
    CURSOR_POS_TYPE_SKILL_01,
    CURSOR_POS_TYPE_SKILL_02,
    CURSOR_POS_TYPE_SKILL_03,
  };

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUISkillSelectLowerDraw::BattleUISkillSelectLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );
    GFL_ASSERT( param.skillSelectItemNum <= SKILL_LIST_MAX );
    GFL_ASSERT( 0 < param.skillSelectItemNum );
    GFL_ASSERT( param.pMenuCursorLytBuff );

    _Clear();

     m_initParam = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUISkillSelectLowerDraw::~BattleUISkillSelectLowerDraw()
  {
    m_pMenuCursor->DeleteLayoutWork();

    m_pCommonMessageObject->EndFunc();
    {
      GFL_SAFE_DELETE( m_pCommonMessageObject );
      GFL_SAFE_DELETE( m_pMenuCursor );
      GFL_SAFE_DELETE( m_pMegaButtonInst);
      GFL_SAFE_DELETE(m_pUltraBurstButtonInst);
      GFL_SAFE_DELETE( m_pZenryokuButtonInst);
    }

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 初期化
    */
  //-------------------------------------------------------------------------
  void BattleUISkillSelectLowerDraw::Initialize()
  {
    _Initialize2D( m_initParam.pLytBuff );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork   = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    m_pSceneLayoutWork              = pLayoutWork;

    //  わざ選択項目作成
    {
      SkillButton::NORMAL_SKILL_SETUP_DATA    normalButtonSetupData;
      SkillButton::ZENRYOKU_SKILL_SETUP_DATA  zenryokuButtonSetupData;

      u32 loopNum = GFL_NELEMS( m_aSkillButtonInst );
      for( u32 i = 0; i < loopNum; ++i )
      {
        //  インスタンス作成に必要なパラメータを出力
        _OutputSkillItemInitParam( &normalButtonSetupData, &zenryokuButtonSetupData, i );

        //  パラメータで初期化
        m_aSkillButtonInst[ i ].Initialize( normalButtonSetupData, zenryokuButtonSetupData, m_initParam.aSkillList[ i ], m_initParam.pAppCommonGrpIconData, m_initParam.pUtilHeap->GetDeviceHeap(), pG2DUtil );
      }
    }

    //  全力ボタン作成
    {
      ZenryokuButton::INIT_PARAM  initParam =
      {
      /*
        //  ボタンID
        s32 panel_button_id;
        //  選択項目のペインID
        s32 select_item_pane_id;
        //  2D制御
        ::app::util::G2DUtil* p_g2d_util;
        //  レイアウトid
        s32 lyt_wk_id;
        //  リソースid
        s32 res_id;
      */
        BUTTON_ID_ZENRYOKU,
        PANENAME_BATTLE_SKL_LOW_000_PANE_BTN_ZENRYOKU,
        pG2DUtil,
        LYT_WK_TYPE_SCREEN,
        0,
      };

      m_pZenryokuButtonInst = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ZenryokuButton();
      m_pZenryokuButtonInst->Initialize( initParam );
    }

    //  メガ進化ボタン作成
    {
      MegaButton::INIT_PARAM  initParam =
      {
        /*
        //  ボタンID
        s32 panel_button_id;
        //  選択項目のペインID
        s32 select_item_pane_id;
        //  2D制御
        ::app::util::G2DUtil* p_g2d_util;
        //  レイアウトid
        s32 lyt_wk_id;
        //  リソースid
        s32 res_id;
        */
        BUTTON_ID_MEGA,
        PANENAME_BATTLE_SKL_LOW_000_PANE_BTN_MEGA,
        pG2DUtil,
        LYT_WK_TYPE_SCREEN,
        0,
      };

      m_pMegaButtonInst = GFL_NEW(m_initParam.pUtilHeap->GetDeviceHeap()) MegaButton();
      m_pMegaButtonInst->Initialize(initParam);
    }

    //  ヒカリドラゴン進化ボタン作成
    {
      UltraBurstButton::INIT_PARAM  initParam =
      {
        /*
        //  ボタンID
        s32 panel_button_id;
        //  選択項目のペインID
        s32 select_item_pane_id;
        //  2D制御
        ::app::util::G2DUtil* p_g2d_util;
        //  レイアウトid
        s32 lyt_wk_id;
        //  リソースid
        s32 res_id;
        */
        BUTTON_ID_ULTRABURST,
        PANENAME_BATTLE_SKL_LOW_000_PANE_BTN_UB,  //@todo
        pG2DUtil,
        LYT_WK_TYPE_SCREEN,
        0,
      };

      m_pUltraBurstButtonInst = GFL_NEW(m_initParam.pUtilHeap->GetDeviceHeap()) UltraBurstButton();
      m_pUltraBurstButtonInst->Initialize(initParam);
    }

    //  ボタン入力関連の設定
    {
      gfl2::lyt::LytParts*  pBackBtnParts = pLayoutWork->GetPartsPane( PANENAME_BATTLE_SKL_LOW_000_PANE_BTN_BACK );
      GFL_ASSERT( pBackBtnParts );

      //  ボタン設定データ定義一覧
      const ::app::ui::ButtonInfoEx    aButtonPaneIdxList[ BUTTON_ID_MAX ] =
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
        //  パネルのインフォアイコンボタン登録
        m_aSkillButtonInst[ 0 ].GetNormal()->GetPanelInfoIconButtonInfoEx(),
        m_aSkillButtonInst[ 1 ].GetNormal()->GetPanelInfoIconButtonInfoEx(),
        m_aSkillButtonInst[ 2 ].GetNormal()->GetPanelInfoIconButtonInfoEx(),
        m_aSkillButtonInst[ 3 ].GetNormal()->GetPanelInfoIconButtonInfoEx(),

        //  パネルのボタン登録
        m_aSkillButtonInst[ 0 ].GetNormal()->GetPanelButtonInfoEx(),
        m_aSkillButtonInst[ 1 ].GetNormal()->GetPanelButtonInfoEx(),
        m_aSkillButtonInst[ 2 ].GetNormal()->GetPanelButtonInfoEx(),
        m_aSkillButtonInst[ 3 ].GetNormal()->GetPanelButtonInfoEx(),

        //  全力わざのインフォアイコンボタン登録
        m_aSkillButtonInst[ 0 ].GetZenryoku()->GetPanelInfoIconButtonInfoEx(),
        m_aSkillButtonInst[ 1 ].GetZenryoku()->GetPanelInfoIconButtonInfoEx(),
        m_aSkillButtonInst[ 2 ].GetZenryoku()->GetPanelInfoIconButtonInfoEx(),
        m_aSkillButtonInst[ 3 ].GetZenryoku()->GetPanelInfoIconButtonInfoEx(),

        //  全力わざのパネルのボタン登録
        m_aSkillButtonInst[ 0 ].GetZenryoku()->GetPanelButtonInfoEx(),
        m_aSkillButtonInst[ 1 ].GetZenryoku()->GetPanelButtonInfoEx(),
        m_aSkillButtonInst[ 2 ].GetZenryoku()->GetPanelButtonInfoEx(),
        m_aSkillButtonInst[ 3 ].GetZenryoku()->GetPanelButtonInfoEx(),

        //  全力ボタンの登録
        m_pZenryokuButtonInst->GetButtonInfoEx(),

        //  メガ進化ボタンの登録
        m_pMegaButtonInst->GetButtonInfoEx(),

        //  ヒカリドラゴン進化ボタンの登録
        m_pUltraBurstButtonInst->GetButtonInfoEx(),

        //  戻るボタン
        {
          BUTTON_ID_BACK,
          pLayoutWork->GetPane( PANENAME_BATTLE_SKL_LOW_000_PANE_BTN_BACK ),
          pLayoutWork->GetBoundingPane( pBackBtnParts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rMultiResId ),
          LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_BACK_TOUCH,
          LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_BACK_RELEASE,
          LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_BACK_CANSEL,
          LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_BACK_TOUCH_RELEASE,
          ::app::tool::ButtonManager::ANIMATION_NULL,
          ::app::tool::ButtonManager::ANIMATION_NULL,
        }
      };

      //  ボタン登録
      UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN ), aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );

      //  入力コールバック設定
      UIView::SetInputListener( this );

      //  Start関数が呼ばれるまで非表示
      UIView::SetVisible( false );
    }

    //  SEボタン設定
    {
      ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

      if( m_initParam.bUseInputSE )
      {
        const s32 aSetupSeTable[ BUTTON_ID_MAX ][ 2 ] =
        {
          { m_aSkillButtonInst[ 0 ].GetNormal()->GetPanelInfoIconButtonInfoEx().button_id,      ::app::tool::ButtonManager::SE_NULL },
          { m_aSkillButtonInst[ 1 ].GetNormal()->GetPanelInfoIconButtonInfoEx().button_id,      ::app::tool::ButtonManager::SE_NULL },
          { m_aSkillButtonInst[ 2 ].GetNormal()->GetPanelInfoIconButtonInfoEx().button_id,      ::app::tool::ButtonManager::SE_NULL },
          { m_aSkillButtonInst[ 3 ].GetNormal()->GetPanelInfoIconButtonInfoEx().button_id,      ::app::tool::ButtonManager::SE_NULL },

          // @fix GFNMCat[4595] ＰＰが０はエラーＳＥを出すように修正(通常技)
          { m_aSkillButtonInst[ 0 ].GetNormal()->GetPanelButtonInfoEx().button_id,              ( m_aSkillButtonInst[ 0 ].GetNormal()->GetNowPPNum() <= 0 ) ? SEQ_SE_BEEP : SEQ_SE_DECIDE1 },
          { m_aSkillButtonInst[ 1 ].GetNormal()->GetPanelButtonInfoEx().button_id,              ( m_aSkillButtonInst[ 1 ].GetNormal()->GetNowPPNum() <= 0 ) ? SEQ_SE_BEEP : SEQ_SE_DECIDE1 },
          { m_aSkillButtonInst[ 2 ].GetNormal()->GetPanelButtonInfoEx().button_id,              ( m_aSkillButtonInst[ 2 ].GetNormal()->GetNowPPNum() <= 0 ) ? SEQ_SE_BEEP : SEQ_SE_DECIDE1 },
          { m_aSkillButtonInst[ 3 ].GetNormal()->GetPanelButtonInfoEx().button_id,              ( m_aSkillButtonInst[ 3 ].GetNormal()->GetNowPPNum() <= 0 ) ? SEQ_SE_BEEP : SEQ_SE_DECIDE1 },

          { m_aSkillButtonInst[ 0 ].GetZenryoku()->GetPanelInfoIconButtonInfoEx().button_id,    ::app::tool::ButtonManager::SE_NULL },
          { m_aSkillButtonInst[ 1 ].GetZenryoku()->GetPanelInfoIconButtonInfoEx().button_id,    ::app::tool::ButtonManager::SE_NULL },
          { m_aSkillButtonInst[ 2 ].GetZenryoku()->GetPanelInfoIconButtonInfoEx().button_id,    ::app::tool::ButtonManager::SE_NULL },
          { m_aSkillButtonInst[ 3 ].GetZenryoku()->GetPanelInfoIconButtonInfoEx().button_id,    ::app::tool::ButtonManager::SE_NULL },

          // @fix GFNMCat[4595] ＰＰが０はエラーＳＥを出すように修正(Ｚ技)
          { m_aSkillButtonInst[ 0 ].GetZenryoku()->GetPanelButtonInfoEx().button_id,            ( m_aSkillButtonInst[ 0 ].GetNormal()->GetNowPPNum() <= 0 ) ? SEQ_SE_BEEP : SEQ_SE_DECIDE1 },
          { m_aSkillButtonInst[ 1 ].GetZenryoku()->GetPanelButtonInfoEx().button_id,            ( m_aSkillButtonInst[ 1 ].GetNormal()->GetNowPPNum() <= 0 ) ? SEQ_SE_BEEP : SEQ_SE_DECIDE1 },
          { m_aSkillButtonInst[ 2 ].GetZenryoku()->GetPanelButtonInfoEx().button_id,            ( m_aSkillButtonInst[ 2 ].GetNormal()->GetNowPPNum() <= 0 ) ? SEQ_SE_BEEP : SEQ_SE_DECIDE1 },
          { m_aSkillButtonInst[ 3 ].GetZenryoku()->GetPanelButtonInfoEx().button_id,            ( m_aSkillButtonInst[ 3 ].GetNormal()->GetNowPPNum() <= 0 ) ? SEQ_SE_BEEP : SEQ_SE_DECIDE1 },

          //  下の２つは必ず再生するので、あとで設定（配列の個数で枠は必要だったので空で用意）
          // momijiで1個追加
          { BUTTON_ID_ZENRYOKU,           ::app::tool::ButtonManager::SE_NULL },
          { BUTTON_ID_MEGA,               ::app::tool::ButtonManager::SE_NULL },
          { BUTTON_ID_ULTRABURST,             ::app::tool::ButtonManager::SE_NULL },
          { BUTTON_ID_BACK, SEQ_SE_CANCEL1 },
        };

        for( s32 i = 0; i < BUTTON_ID_MAX; ++i )
        {
          pButtonManager->SetButtonSelectSE( aSetupSeTable[ i ][ 0 ], aSetupSeTable[ i ][ 1 ] );
        }

        // @fix GFNMCat[1363] 通信対戦ではSEを出さない
        pButtonManager->SetButtonSelectSE( BUTTON_ID_MEGA, SEQ_SE_BT_040 );
        pButtonManager->SetButtonSelectSE( BUTTON_ID_ZENRYOKU, SEQ_SE_BT_ZENRYOKUWAZA );
        pButtonManager->SetButtonSelectSE(BUTTON_ID_ULTRABURST, SEQ_SE_W741);
      }
    }

    //  メニューカーソル作成
    {
      //  すでに作成したカーソルレイアウトリソースを使っているのでロードはしなくてもいい
      m_pMenuCursor = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::MenuCursor( m_initParam.pUtilHeap, &m_initParam.pMenuCursorLytBuff );
      GFL_ASSERT( m_pMenuCursor );

      m_pMenuCursor->CreateLayoutWork( UIView::GetLayoutSystem(), UIView::GetLytSetup( gfl2::lyt::DISPLAY_LOWER ), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_SMALL );
      m_pMenuCursor->StartAnime();
    }

    //  特別ボタンの設定
    {
      m_pMegaButtonInst->SetVislble( false );
      m_pZenryokuButtonInst->SetVislble( false );
      m_pUltraBurstButtonInst->SetVislble(false);

      if( m_initParam.specialSelectParam.type == SPECIAL_BUTTON_TYPE_NONE )
      {
      }
      else if( m_initParam.specialSelectParam.type == SPECIAL_BUTTON_TYPE_MEGA )
      {
        m_pMegaButtonInst->SetVislble( true );
        m_pProcSpecialButtonInst  = m_pMegaButtonInst;
      }
      else if( m_initParam.specialSelectParam.type == SPECIAL_BUTTON_TYPE_ZENRYOKU )
      {
        m_pZenryokuButtonInst->SetVislble( true );
        m_pProcSpecialButtonInst  = m_pZenryokuButtonInst;
      }
      else if (m_initParam.specialSelectParam.type == SPECIAL_BUTTON_TYPE_ULTRABURST)
      {
        m_pUltraBurstButtonInst->SetVislble(true);
        m_pProcSpecialButtonInst = m_pUltraBurstButtonInst;
      }
    }

    //  わざ選択の初期設定
    {
      s32 loopNum = GFL_NELEMS( m_aSkillButtonInst );
      for( s32 i = 0; i < loopNum; ++i )
      {
        m_aSkillButtonInst[ i ].Change( SkillButton::CHANGE_TYPE_NORMAL, UIView::GetButtonManager() );
        if( i < m_initParam.skillSelectItemNum )
        {
          m_aSkillButtonInst[ i ].SetVisible( true );
        }
        else
        {
          m_aSkillButtonInst[ i ].SetVisible( false );
        }
      }
    }

    //  下側のウィンドウ作成
    {
      m_pCommonMessageObject = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::CommonMessageObject();

      ::app::util::GetPaneHelper  paneHelper( m_pSceneLayoutWork, &rMultiResId );
      paneHelper.Push( PANENAME_BATTLE_SKL_LOW_000_PANE_MESSAGE_L1 );

      ::app::tool::CommonMessageObject::INIT_PARAM  initParam;
      {
        initParam.lytId         = LYT_WK_TYPE_SCREEN;
        initParam.pG2DUtil      = pG2DUtil;
        initParam.inAnimeId     = LA_SKLLOW_BATTLE_SKL_LOW_000__MESSAGE_L1_IN;
        initParam.outAnimeId    = LA_SKLLOW_BATTLE_SKL_LOW_000__MESSAGE_L1_OUT;
        initParam.pWindowPane   = paneHelper.Peek();
        initParam.showWaitFrame = 0;

        //  テキストペイン設定
        {
          initParam.pTextBoxPaneArray[ 0 ]  = paneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00 );
          initParam.textBoxPaneNum  = 1;
        }
      }

      m_pCommonMessageObject->InitFunc( initParam, m_initParam.pUtilHeap->GetDeviceHeap() );
    }

    // @fix NMCat[1749] 技説明のショートカットキーテキストの場合はインフォボタンが一つもなければショートカットキーテキストはフラグをoff
    {
      m_bPutWazaInfoShortCutKeyText = false;
      for( s32 i = 0; i < m_initParam.skillSelectItemNum; ++i )
      {
        if( m_initParam.aSkillList[ i ].bInfoIconVisible == true )
        {
          m_bPutWazaInfoShortCutKeyText = true;
          break;
        }
      }
    }
  }

  /**
  * @brief  終了
  */        
  void BattleUISkillSelectLowerDraw::Terminate()
  {
  }

  /**
  * @brief  クラス破棄可能か
  */
  bool BattleUISkillSelectLowerDraw::IsDelete()
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

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUISkillSelectLowerDraw::Update()
  {
    _UpdateState();

    m_pCommonMessageObject->Update();
    m_pMenuCursor->Update();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUISkillSelectLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
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

  //------------------------------------------------------------------
  /**
    * @brief  技リストのカレント位置を更新
    * @return カーソル位置が変わった -> true / 変わっていない -> false
    */
  //------------------------------------------------------------------
  bool BattleUISkillSelectLowerDraw::SetCurrentIdxSkillList( const s32 idx )
  {
    GFL_ASSERT( idx < m_initParam.skillSelectItemNum );
    
    bool  bActive     = false;
    bool  bChangePos  = ( m_skillListCursorIdx != idx );

    for( s32 i = 0; i < m_initParam.skillSelectItemNum; ++i )
    {
      bActive = ( idx == i );

      //  カーソル位置配置
      if( bActive == true )
      {
        //  わざ選択のカーソル設定なので、わざ選択リストへ移動
        //  カーソル移動前に下記処理を呼ばないとカーソル位置更新タイミング順序が狂い、位置がおかしくなる。
        MoveCursorSkillList();

        //  すでに同じなのでなにもしない
        if( bChangePos == false )
        {
          return  false;
        }

        return  _SetCurrentIdxSkillList( idx );
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   特殊ボタンへカーソル移動
    */
  //------------------------------------------------------------------
  bool BattleUISkillSelectLowerDraw::MoveCursorSpecialButton()
  {
    //  特殊ボタンがなければ移動しない
    if( m_initParam.specialSelectParam.type == SPECIAL_BUTTON_TYPE_NONE )
    {
      return  false;
    }

    if( m_state == STATE_TYPE_INPUT_SKILL_SELECT_LIST )
    {
      _SetState( STATE_TYPE_INPUT_SPECIAL_BUTTON );
      return  true;;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   技リストへカーソル移動
    */
  //------------------------------------------------------------------
  bool BattleUISkillSelectLowerDraw::MoveCursorSkillList()
  {
    if( m_state == STATE_TYPE_INPUT_SPECIAL_BUTTON )
    {
      _SetState( STATE_TYPE_INPUT_SKILL_SELECT_LIST );

      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   スキル選択リストのカーソルが一番上か
    */
  //------------------------------------------------------------------
  bool BattleUISkillSelectLowerDraw::IsSkillSelectListCurrentUpperLimit()
  {
    return ( m_skillListCursorIdx <= 0 );
  }

  //------------------------------------------------------------------
  /**
    * @brief   スキル選択リストのカーソルが一番下か
    */
  //------------------------------------------------------------------
  bool BattleUISkillSelectLowerDraw::IsSkillSelectListCurrentUnderLimit()
  {
    //  まず一番下のlistのidxを取得
    s32 limitIdx  = 0;
    {
      for( s32 i = 0; i < m_initParam.skillSelectItemNum; ++i )
      {
        gfl2::lyt::LytPane* pCursorNullPane = m_aSkillButtonInst[ i ].GetCursorNullPane();
        if( pCursorNullPane == NULL )
        {
          continue;
        }

        limitIdx  = i;
      }
    }

    return  ( limitIdx <= m_skillListCursorIdx );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタンidからカーソルリストidxを取得
    */
  //------------------------------------------------------------------
  s32 BattleUISkillSelectLowerDraw::GetButtonIdToCursorSelectListIdx( const ButtonId id )
  {
    s32 loopNum = GFL_NELEMS( m_aSkillButtonInst );
    for( s32 i = 0; i < loopNum; ++i )
    {
      if( ( m_aSkillButtonInst[ i ].GetPanelButtonId() == id ) ||
          ( m_aSkillButtonInst[ i ].GetInfoButtonId() == id ) )
      {
        return  i;
      }
    }

    return  -1;
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUISkillSelectLowerDraw::_Initialize2D( void* pAppLytBuff )
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
            LYTRES_SKLLOW_BATTLE_SKL_LOW_000_BFLYT,
            LA_SKLLOW_BATTLE_SKL_LOW_000___NUM,
            pAppLytBuff,
            true,
            ::app::util::G2DUtil::SETUP_LOWER,
            true
          },
      };

      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData),  m_initParam.pBtlAppMsgData, m_initParam.pBtlWordSet );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUISkillSelectLowerDraw::_UpdateState()
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      //  初回カーソルを設定
      {
        u32 listMax = GFL_NELEMS( m_saListCursorType );

        if( m_initParam.firstCursorType == CURSOR_POS_TYPE_SPECIAL )
        {
          if( m_initParam.specialSelectParam.type == SPECIAL_BUTTON_TYPE_NONE )
          {
            m_initParam.firstCursorType = CURSOR_POS_TYPE_SKILL_00;
          }
        }
        else
        {
          //  リストを超えていないか
          if( m_initParam.skillSelectItemNum <= (s32)listMax )
          {
          }
          else
          {
            m_initParam.firstCursorType = CURSOR_POS_TYPE_SKILL_00;
          }
        }

        if( m_initParam.firstCursorType == CURSOR_POS_TYPE_SPECIAL )
        {
          //  初回ステート設定なので MoveCursorSpecialButton を使わないで直接ステートを変える
          //  なぜなら関数内部でステートチェックをして意図しないステートであれば遷移しないから

          _SetState( STATE_TYPE_INPUT_SPECIAL_BUTTON );
        }
        else
        {
          bool  bHitFirstCursor = false;
          //  技は必ず先頭から順に配置されているので、歯抜けにはなっていないのが前提
          //  仕様でも歯抜けになることはない（なったらそれはバグ）
          for( s32 i = 0; i < m_initParam.skillSelectItemNum; ++i )
          {
            if( m_saListCursorType[ i ] == m_initParam.firstCursorType )
            {
              //  リストにカーソルを設定
              m_skillListCursorIdx  = i;
              bHitFirstCursor = true;
              break;
            }
          }

          // @fix GFNMCat[3609] カーソルが見つからなかったら先頭の初期位置にする（技は必ず１つ以上あり、先頭のはある前提）
          if( bHitFirstCursor == false )
          {
            m_initParam.firstCursorType = CURSOR_POS_TYPE_SKILL_00;
            m_skillListCursorIdx  = 0;
          }

          _SetState( STATE_TYPE_INPUT_SKILL_SELECT_LIST );
        }
      }
    }

    if( m_state == STATE_TYPE_INPUT_ZENRYOKU_SKILL_SELECT_LIST )
    {
      //  全力わざ発動イベント
      if( m_subStateZenryokuSelectType == SUB_STATE_ZENRYOKU_SELECT_TYPE_EVENT )
      {
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_TOUCHANIME ) )
        {
          _SetSubStateZenryokuSelect( SUB_STATE_ZENRYOKU_SELECT_TYPE_IN );
        }
      }

      //  全力わざリスト登場
      if( m_subStateZenryokuSelectType == SUB_STATE_ZENRYOKU_SELECT_TYPE_IN )
      {
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_SKLLOW_BATTLE_SKL_LOW_000_IN_ZENRYOKU ) )
        {
          _SetSubStateZenryokuSelect( SUB_STATE_ZENRYOKU_SELECT_TYPE_INPUT );
        }
      }

      //  全力わざリスト選択中
      if( m_subStateZenryokuSelectType == SUB_STATE_ZENRYOKU_SELECT_TYPE_INPUT )
      {
        
      }

      //  全力わざリスト退場
      if( m_subStateZenryokuSelectType == SUB_STATE_ZENRYOKU_SELECT_TYPE_OUT )
      {
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_SKLLOW_BATTLE_SKL_LOW_000_OUT_ZENRYOKU ) )
        {
          _SetSubStateZenryokuSelect( SUB_STATE_ZENRYOKU_SELECT_TYPE_END );
        }
      }
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
  void BattleUISkillSelectLowerDraw::_SetState( const StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    StateType oldState  = m_state;

    m_state = newStateType;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );

      SetHelpMessageId( str_LA_waza );
    }
    else
    if( m_state == STATE_TYPE_INPUT_SKILL_SELECT_LIST )
    {
      SetHelpMessageId( str_LA_waza );

      //  カーソルを技リストに移動
      _SetCurrentIdxSkillList( m_skillListCursorIdx );
    }
    else
    if( m_state == STATE_TYPE_INPUT_SPECIAL_BUTTON )
    {
      SetHelpMessageId( str_LA_waza );

      //  ここのステートにくるのであれば必ず特別ボタン制御クラスがある！
      GFL_ASSERT( m_pProcSpecialButtonInst );

      //  カーソルに特殊ボタンを設定
      m_cursorSelectType  = CURSOR_POS_TYPE_SPECIAL;

      //  特別ボタンを表示
      m_pProcSpecialButtonInst->SetVislble( true );

      //  カーソルを特殊ボタンへ移動
      m_pMenuCursor->Put( m_pSceneLayoutWork, m_pProcSpecialButtonInst->GetCursorNullPane() );

      //  前のステートが全力わざ選択なら、全力わざ選択用の画面が開いているので、閉じる
      if( oldState == STATE_TYPE_INPUT_ZENRYOKU_SKILL_SELECT_LIST )
      {
      }
      else
      {
      }
    }
    else
    if( m_state == STATE_TYPE_INPUT_ZENRYOKU_SKILL_SELECT_LIST )
    {
      _SetSubStateZenryokuSelect( SUB_STATE_ZENRYOKU_SELECT_TYPE_EVENT );
    }
    else
    if( m_state == STATE_TYPE_ENTER_ANIM_WAIT )
    {
      
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
      //  カーソル非表示
      m_pMenuCursor->SetVisible( false );
    }
    else if( m_state == STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   全力わざ選択サブステート設定
    */
  //------------------------------------------------------------------
  void BattleUISkillSelectLowerDraw::_SetSubStateZenryokuSelect( const SubStateZenryokuSelectType newStateType )
  {
    m_subStateZenryokuSelectType  = newStateType;

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( newStateType == SUB_STATE_ZENRYOKU_SELECT_TYPE_EVENT )
    {
      //  全力ボタンを押したたら専用アニメを再生
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_TOUCHANIME );

      //  いったんカーソルを非表示
      m_pMenuCursor->SetVisible( false );

      UIView::SetInputEnabledAllView( false );
    }
    else
    if( newStateType == SUB_STATE_ZENRYOKU_SELECT_TYPE_IN )
    {
      m_pProcSpecialButtonInst->SetVislble( false );

      //  いったんカーソルを非表示
      m_pMenuCursor->SetVisible( false );

      //  全力わざ選択用の画面を開く
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_SKLLOW_BATTLE_SKL_LOW_000_IN_ZENRYOKU );

      //  全力ボタンを押せるようにする
      {
        u32 loopNum = GFL_NELEMS( m_aSkillButtonInst );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_aSkillButtonInst[ i ].Change( SkillButton::CHANGE_TYPE_ZENRYOKU, UIView::GetButtonManager() );
        }
      }

      UIView::SetInputEnabledAllView( false );
    }
    else
    if( newStateType == SUB_STATE_ZENRYOKU_SELECT_TYPE_INPUT )
    {
      GFL_ASSERT( 0 <= m_skillListCursorIdx );
      GFL_ASSERT( m_skillListCursorIdx < GFL_NELEMS( m_aSkillButtonInst ) );

      bool  bSelect       = false;
      s32   nowCursorIdx  = m_skillListCursorIdx;

      //  選択できる項目にカーソルidxを設定
      {
        s32 loopNum = GFL_NELEMS( m_aSkillButtonInst );
        for( s32 i = 0; i < loopNum; ++i )
        {
          if( m_aSkillButtonInst[ nowCursorIdx ].GetCursorNullPane() == NULL )
          {
            ++nowCursorIdx;
            if( loopNum <= nowCursorIdx )
            {
              nowCursorIdx  = 0;
            }
          }
          else
          {
            //  全力わざの項目が見つかったので終了
            bSelect = true;
            break;
          }
        }
      }

      if( bSelect )
      {
        //  選択位置を更新
        m_skillListCursorIdx = nowCursorIdx;

        m_pMenuCursor->SetVisible( true );
        m_pMenuCursor->Put( m_pSceneLayoutWork, m_aSkillButtonInst[ m_skillListCursorIdx ].GetCursorNullPane() );
      }
      else
      {
        //  選択するのがない場合はヘルプメッセージに注意表示
        gfl2::str::StrBuf*  pTmpStrBuf  = UIView::GetG2DUtil()->GetTempStrBuf( 0 );
        m_initParam.pBtlAppMsgData->GetString( STR_WAZA_FULLPOWER_NG, *pTmpStrBuf );
        gfl2::str::StrBuf*  pMessageArray[] =
        {
          pTmpStrBuf
        };

        m_pCommonMessageObject->ShowMessageTextPane( pMessageArray, GFL_NELEMS( pMessageArray ) );
      }

      // @fix GFNMCat[1552] 外部設定で入力フラグを見て設定する（強制的にtrueにしないため）
      UIView::SetInputEnabledAllView( m_initParam.bControllable );
    }
    else
    if( newStateType == SUB_STATE_ZENRYOKU_SELECT_TYPE_OUT )
    {
      m_pCommonMessageObject->HideMessage();

      //  いったんカーソルを非表示
      m_pMenuCursor->SetVisible( false );

      //  全力わざ選択用の画面を閉じる
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_SKLLOW_BATTLE_SKL_LOW_000_OUT_ZENRYOKU );

      UIView::SetInputEnabledAllView( false );
    }
    else
    if( newStateType == SUB_STATE_ZENRYOKU_SELECT_TYPE_END )
    {
      //  通常ボタンを押せるようにする
      {
        u32 loopNum = GFL_NELEMS( m_aSkillButtonInst );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_aSkillButtonInst[ i ].Change( SkillButton::CHANGE_TYPE_NORMAL, UIView::GetButtonManager() );
        }
      }

      //  カーソルを表示して位置を合わせる
      {
        m_pMenuCursor->SetVisible( true );
        m_pMenuCursor->Put( m_pSceneLayoutWork, m_aSkillButtonInst[ m_skillListCursorIdx ].GetCursorNullPane() );
      }

      UIView::SetInputEnabledAllView( m_initParam.bControllable );
      _SetState( STATE_TYPE_INPUT_SPECIAL_BUTTON );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief    わざ選択のカレントidx設定
    * @return   移動した -> true / 移動していない -> false
    */
  //------------------------------------------------------------------
  bool BattleUISkillSelectLowerDraw::_SetCurrentIdxSkillList( const s32 idx )
  {
    gfl2::lyt::LytPane* pCursorNullPane = m_aSkillButtonInst[ idx ].GetCursorNullPane();
    if( pCursorNullPane == NULL )
    {
      return false;
    }
    
    m_skillListCursorIdx  = idx;
    m_cursorSelectType    = m_saListCursorType[ idx ];

    m_pMenuCursor->Put( m_pSceneLayoutWork, pCursorNullPane );

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   SkillButtonクラスに必要ならパラメーターを出力
    */
  //------------------------------------------------------------------
  void BattleUISkillSelectLowerDraw::_OutputSkillItemInitParam( SkillButton::NORMAL_SKILL_SETUP_DATA* pOutputNormalButtonParam, SkillButton::ZENRYOKU_SKILL_SETUP_DATA* pOutputZenryokuButtonParam, const u32 selecListIdx )
  {
    GFL_ASSERT( pOutputNormalButtonParam );
    GFL_ASSERT( pOutputZenryokuButtonParam );
    GFL_ASSERT( selecListIdx < SKILL_LIST_MAX );

    _OutputNormalSkillItemInitParam( &pOutputNormalButtonParam->init, &pOutputNormalButtonParam->pane, selecListIdx );
    _OutputZenryokuSkillItemInitParam( &pOutputZenryokuButtonParam->init, &pOutputZenryokuButtonParam->pane, selecListIdx );
  }

  //------------------------------------------------------------------
  /**
    * @brief   parts::BattleUISkillButtonPartsLayoutのインスタンス作成に必要なパラメータ出力(通常わざ用)
    */
  //------------------------------------------------------------------
  void BattleUISkillSelectLowerDraw::_OutputNormalSkillItemInitParam( parts::BattleUISkillButtonPartsLayout::INIT_PARAM* pOutInitParam, parts::BattleUISkillButtonPartsLayout::INIT_PANE_PARAM* pOutInitPaneParam, const u32 selecListIdx )
  {
    GFL_ASSERT( selecListIdx < SKILL_LIST_MAX );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork   = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  初期パラメータ
    {
      const parts::BattleUISkillButtonPartsLayout::INIT_PARAM aSkillItemInitParam[ SKILL_LIST_MAX ]  =
      {
        /*
          //  リストidx( 0 〜 3 )
          s32 list_idx;

          //  ボタンID
          s32 button_id;

          //  アニメidリスト
          s32 a_anim_id_list[ ANIM_TYPE_MAX ];
        */
        //  1列目
        {
          BUTTON_ID_SKILL_ITEM_00,
          BUTTON_ID_SKILL_ITEM_INFO_00,

          {
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_04,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_05,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_06,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_07,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_08,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_09,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_10,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_11,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_12,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_13,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_14,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_15,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_16,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_COLOR_17,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_TEXCOLOR_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_TEXCOLOR_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_TEXCOLOR_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00_TEXCOLOR_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_INFO_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_INFO_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_INFO_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_INFO_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_INFO_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_INFO_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_KOUKA_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_KOUKA_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_KOUKA_KOUKA_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_KOUKA_KOUKA_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_KOUKA_KOUKA_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_00__PARTS_KOUKA_KOUKA_03,
          }
        },

        //  2列目
        {
          BUTTON_ID_SKILL_ITEM_01,
          BUTTON_ID_SKILL_ITEM_INFO_01,

          {
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_04,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_05,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_06,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_07,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_08,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_09,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_10,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_11,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_12,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_13,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_14,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_15,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_16,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_COLOR_17,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_TEXCOLOR_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_TEXCOLOR_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_TEXCOLOR_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01_TEXCOLOR_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_INFO_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_INFO_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_INFO_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_INFO_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_INFO_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_INFO_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_KOUKA_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_KOUKA_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_KOUKA_KOUKA_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_KOUKA_KOUKA_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_KOUKA_KOUKA_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_01__PARTS_KOUKA_KOUKA_03,
          }
        },

        //  3列目
        {
          BUTTON_ID_SKILL_ITEM_02,
          BUTTON_ID_SKILL_ITEM_INFO_02,

          {
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_04,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_05,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_06,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_07,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_08,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_09,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_10,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_11,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_12,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_13,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_14,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_15,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_16,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_COLOR_17,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_TEXCOLOR_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_TEXCOLOR_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_TEXCOLOR_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02_TEXCOLOR_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_INFO_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_INFO_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_INFO_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_INFO_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_INFO_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_INFO_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_KOUKA_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_KOUKA_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_KOUKA_KOUKA_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_KOUKA_KOUKA_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_KOUKA_KOUKA_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_02__PARTS_KOUKA_KOUKA_03,
          }
        },

        //  4列目
        {
          BUTTON_ID_SKILL_ITEM_03,
          BUTTON_ID_SKILL_ITEM_INFO_03,

          {
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_04,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_05,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_06,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_07,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_08,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_09,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_10,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_11,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_12,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_13,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_14,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_15,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_16,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_COLOR_17,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_TEXCOLOR_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_TEXCOLOR_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_TEXCOLOR_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03_TEXCOLOR_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_INFO_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_INFO_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_INFO_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_INFO_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_INFO_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_INFO_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_KOUKA_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_KOUKA_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_KOUKA_KOUKA_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_KOUKA_KOUKA_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_KOUKA_KOUKA_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSSKILL_03__PARTS_KOUKA_KOUKA_03,
          }
        }
      };

      *pOutInitParam = aSkillItemInitParam[ selecListIdx ];
    }

    //  ペイン初期パラメータ
    {
      const s32 aRootPaneId[ SKILL_LIST_MAX ] =
      {
        PANENAME_BATTLE_SKL_LOW_000_PANE_PARTSSKILL_00,
        PANENAME_BATTLE_SKL_LOW_000_PANE_PARTSSKILL_01,
        PANENAME_BATTLE_SKL_LOW_000_PANE_PARTSSKILL_02,
        PANENAME_BATTLE_SKL_LOW_000_PANE_PARTSSKILL_03,
      };

      //  パーツルートペイン
      pOutInitPaneParam->pRootPartsPane  = pLayoutWork->GetPartsPane( aRootPaneId[ selecListIdx ] );
      GFL_ASSERT( pOutInitPaneParam->pRootPartsPane );

      //  カーソルNULLペイン
      pOutInitPaneParam->pCursorNullPane        = pLayoutWork->GetPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_NULL_CUR, &rMultiResId );

      //  ボタンのヒット判定のボックスペイン
      pOutInitPaneParam->pButtonHitBoxPane      = pLayoutWork->GetBoundingPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_BOUND_00, &rMultiResId );

      //  インフォアイコンのパーツペイン
      pOutInitPaneParam->pInfoIconPartsPane     = pLayoutWork->GetPartsPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_PARTS_INFO, &rMultiResId );

      //  インフォアイコンのヒット判定のボックスペイン
      pOutInitPaneParam->pInfoIconHitBoxPane    = pLayoutWork->GetBoundingPane( pOutInitPaneParam->pInfoIconPartsPane, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00, &rMultiResId );

      //  現在PP値のテキストペイン
      pOutInitPaneParam->pNowPPTextBoxPane      = pLayoutWork->GetTextBoxPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_05, &rMultiResId );
      //  最大PP値のテキストペイン
      pOutInitPaneParam->pMaxPPNumTextBoxPane   = pLayoutWork->GetTextBoxPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_03, &rMultiResId );
      /** @fix  MMCat[171]   つよさをみる画面のPP値の文字色に関する質問:  "PP","/"も色変更の対象とする  */
      //  "PP"
      pOutInitPaneParam->pPPLavelTextBoxPane    = pLayoutWork->GetTextBoxPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_01, &rMultiResId );
      //  "/"
      pOutInitPaneParam->pPPSlashTextBoxPane    = pLayoutWork->GetTextBoxPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_04, &rMultiResId );

      //  技名のテキストペイン
      pOutInitPaneParam->pSkillNameTextBoxPane  = pLayoutWork->GetTextBoxPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_00, &rMultiResId );

      //  効果アイコンペイン
      pOutInitPaneParam->pKoukaIconPartsPane    = pLayoutWork->GetPartsPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_PARTS_KOUKA, &rMultiResId );

      //  PP表記テキストペイン
      pOutInitPaneParam->pPPTitleTextPane       = pLayoutWork->GetTextBoxPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_01, &rMultiResId );

      //  タイプアイコン画像ペイン
      {
        gfl2::lyt::LytParts* pPartsPane         = pLayoutWork->GetPartsPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_PARTS_TYPE, &rMultiResId );
        GFL_ASSERT( pPartsPane );

        pOutInitPaneParam->pTypeIconPicPane     = pLayoutWork->GetPicturePane( pPartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMultiResId );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   parts::BattleUIZenryokuSkillButtonPartsLayoutのインスタンス作成に必要なパラメータ出力(全力わざ用)
    */
  //------------------------------------------------------------------
  //  parts::BattleUISkillButtonPartsLayoutのインスタンス作成に必要なパラメータ出力(全力わざ用)
  void BattleUISkillSelectLowerDraw::_OutputZenryokuSkillItemInitParam( parts::BattleUIZenryokuSkillButtonPartsLayout::INIT_PARAM* pOutInitParam, parts::BattleUIZenryokuSkillButtonPartsLayout::INIT_PANE_PARAM* pOutInitPaneParam, const u32 selecListIdx )
  {
    GFL_ASSERT( selecListIdx < SKILL_LIST_MAX );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork   = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  初期パラメータ
    {
      const parts::BattleUIZenryokuSkillButtonPartsLayout::INIT_PARAM aSkillItemInitParam[ SKILL_LIST_MAX ]  =
      {
        /*
          //  リストidx( 0 〜 3 )
          s32 list_idx;

          //  ボタンID
          s32 button_id;

          //  アニメidリスト
          s32 a_anim_id_list[ ANIM_TYPE_MAX ];
        */
        //  1列目
        {
          BUTTON_ID_ZENRYOKU_SKILL_ITEM_00,
          BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_00,

          {
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_INFO_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_INFO_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_INFO_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_INFO_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_INFO_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_INFO_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_KOUKA_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_KOUKA_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_KOUKA_KOUKA_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_KOUKA_KOUKA_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_KOUKA_KOUKA_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00__PARTS_KOUKA_KOUKA_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00_LOOP,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_00_LOOP_NORMAL
          }
        },

        //  2列目
        {
          BUTTON_ID_ZENRYOKU_SKILL_ITEM_01,
          BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_01,

          {
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_INFO_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_INFO_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_INFO_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_INFO_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_INFO_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_INFO_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_KOUKA_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_KOUKA_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_KOUKA_KOUKA_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_KOUKA_KOUKA_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_KOUKA_KOUKA_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01__PARTS_KOUKA_KOUKA_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01_LOOP,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_01_LOOP_NORMAL
          }
        },

        //  3列目
        {
          BUTTON_ID_ZENRYOKU_SKILL_ITEM_02,
          BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_02,

          {
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_INFO_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_INFO_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_INFO_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_INFO_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_INFO_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_INFO_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_KOUKA_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_KOUKA_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_KOUKA_KOUKA_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_KOUKA_KOUKA_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_KOUKA_KOUKA_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02__PARTS_KOUKA_KOUKA_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02_LOOP,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_02_LOOP_NORMAL
          }
        },

        //  4列目
        {
          BUTTON_ID_ZENRYOKU_SKILL_ITEM_03,
          BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_03,

          {
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_INFO_TOUCH,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_INFO_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_INFO_CANSEL,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_INFO_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_INFO_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_INFO_TOUCH_RELEASE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_KOUKA_ACTIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_KOUKA_PASSIVE,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_KOUKA_KOUKA_00,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_KOUKA_KOUKA_01,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_KOUKA_KOUKA_02,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03__PARTS_KOUKA_KOUKA_03,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03_LOOP,
            LA_SKLLOW_BATTLE_SKL_LOW_000__PARTSZENRYOKU_03_LOOP_NORMAL
          }
        }
      };

      *pOutInitParam = aSkillItemInitParam[ selecListIdx ];
    }

    //  ペイン初期パラメータ
    {
      const s32 aRootPaneId[ SKILL_LIST_MAX ] =
      {
        PANENAME_BATTLE_SKL_LOW_000_PANE_PARTSZENRYOKU_00,
        PANENAME_BATTLE_SKL_LOW_000_PANE_PARTSZENRYOKU_01,
        PANENAME_BATTLE_SKL_LOW_000_PANE_PARTSZENRYOKU_02,
        PANENAME_BATTLE_SKL_LOW_000_PANE_PARTSZENRYOKU_03,
      };

      //  パーツルートペイン
      pOutInitPaneParam->pRootPartsPane  = pLayoutWork->GetPartsPane( aRootPaneId[ selecListIdx ] );
      GFL_ASSERT( pOutInitPaneParam->pRootPartsPane );

      //  カーソルNULLペイン
      pOutInitPaneParam->pCursorNullPane        = pLayoutWork->GetPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_UPP_008_PANE_NULL_CUR, &rMultiResId );

      //  ボタンのヒット判定のボックスペイン
      pOutInitPaneParam->pButtonHitBoxPane      = pLayoutWork->GetBoundingPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_UPP_008_PANE_BOUND_00, &rMultiResId );

      //  インフォアイコンのパーツペイン
      pOutInitPaneParam->pInfoIconPartsPane     = pLayoutWork->GetPartsPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_UPP_008_PANE_PARTS_INFO, &rMultiResId );
      //  インフォアイコンのヒット判定のボックスペイン(インフォアイコンパーツペインを参照)
      {
        pOutInitPaneParam->pInfoIconHitBoxPane  = pLayoutWork->GetBoundingPane( pOutInitPaneParam->pInfoIconPartsPane, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00, &rMultiResId );
      }

      //  技名のテキストペイン
      pOutInitPaneParam->pSkillNameTextBoxPane  = pLayoutWork->GetTextBoxPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_UPP_008_PANE_TEXTBOX_00, &rMultiResId );

      //  効果アイコンペイン
      pOutInitPaneParam->pKoukaIconPartsPane    = pLayoutWork->GetPartsPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_UPP_008_PANE_PARTS_KOUKA, &rMultiResId );

      //  タイプアイコンペイン
      {
        gfl2::lyt::LytParts*  pPartsPane    = pLayoutWork->GetPartsPane( pOutInitPaneParam->pRootPartsPane, PANENAME_BATTLE_PLT_UPP_008_PANE_PARTS_TYPE, &rMultiResId );
        GFL_ASSERT( pPartsPane );
        pOutInitPaneParam->pTypeIconPicPane = pLayoutWork->GetPicturePane( pPartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMultiResId );
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  buttonId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUISkillSelectLowerDraw::OnLayoutPaneEvent( const u32 buttonId )
  {
    UCHIDA_PRINT( "OnLayoutPaneEvent : painId = %d\n", buttonId );

    if( m_initParam.pDelegate == NULL )
    {
      return  ENABLE_ACCESS;
    }

    if( m_initParam.pDelegate->OnEnterInput( this, buttonId ) )
    {
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
  void BattleUISkillSelectLowerDraw::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    //  カーソル対象のボタンを設定しているか
    SkillButton*  pSkillItemButton  = NULL;

    s32 loopNum = GFL_NELEMS( m_aSkillButtonInst );
    for( s32 i = 0; i < loopNum; ++i )
    {
      pSkillItemButton  = &m_aSkillButtonInst[ i ];
      if( pSkillItemButton->GetPanelButtonId() == (s32)buttonId )
      {
        //  カーソル位置を更新
        if( SetCurrentIdxSkillList( i ) )
        {
          return;
        }
      }
    }

    //  特殊ボタンの場合は特殊ボタンへ移動
    if ((buttonId == BUTTON_ID_MEGA) ||
        (buttonId == BUTTON_ID_ZENRYOKU) ||
        (buttonId == BUTTON_ID_ULTRABURST))
    {
      MoveCursorSpecialButton();
      return;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力の検知
    *
    * @param  pButton ボタン
    * @param  pKey    十字キー
    * @param  pStick  アナログスティック
    *
    * @return 同フレーム内での他入力イベントを許可するならtrue、
    *         他のイベントを排除するならfalseを返却すること。
    */
  //------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUISkillSelectLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

		if(m_initParam.pDelegate->OnKeyAction( this, pButton, pKey, pStick ))
    {
      return  DISABLE_ACCESS;
	  }

		return ENABLE_ACCESS;
  }

  /**
  * @brief  カーソル選択している項目を押す
  */
  void BattleUISkillSelectLowerDraw::PushBtnCurrentCursorItem()
  {
    //  コールバックで入力結果を渡す
    if( IsSkillSelectState() )
    {
      //  わざ選択項目の押した時のアニメ再生
      SkillButton*  pSkillItemButton  = &m_aSkillButtonInst[ m_skillListCursorIdx ];
      UIResponder::GetButtonManager()->StartSelectedAct( pSkillItemButton->GetPanelButtonId() );

      //  わざ選択項目を押したことを通知
      GFL_ASSERT( m_skillListCursorIdx < GFL_NELEMS( m_aSkillButtonInst ) );

      //  入力が無効な時は入力の入力のコールバッグが動かないので手動で動かす
      if( UIResponder::IsInputEnabled() == false )
      {
        OnLayoutPaneEvent( pSkillItemButton->GetPanelButtonId() );
      }
    }
    else if( m_state == STATE_TYPE_INPUT_SPECIAL_BUTTON )
    {
      GFL_ASSERT( m_pProcSpecialButtonInst );
 
      ButtonId  buttonId  = (ButtonId)m_pProcSpecialButtonInst->GetInitParam().button_id;

      UIResponder::GetButtonManager()->StartSelectedAct( buttonId );

      //  入力が無効な時は入力の入力のコールバッグが動かないので手動で動かす
      if( UIResponder::IsInputEnabled() == false )
      {
        OnLayoutPaneEvent( buttonId );
      }
    }
  }

  /**
  * @brief  わざ選択カーソルしている項目を押す
  */
  bool BattleUISkillSelectLowerDraw::PushBtnCurrentCursorItemInfoIcon()
  {
    if( IsSkillSelectState() )
    {
      //  インフォアイコンの押した時のアニメ再生
      SkillButton*  pSkillItemButton  = &m_aSkillButtonInst[ m_skillListCursorIdx ];
      if( pSkillItemButton->IsInfoIconVisible() )
      {
        //  コールバックで入力結果を渡す
        if( m_initParam.pDelegate )
        {
          //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
          UIResponder::GetButtonManager()->StartSelectedAct( pSkillItemButton->GetInfoButtonId() );

          return true;
        }
      }
    }

    return false;
  }

  /**
  * @brief  指定したボタンを押す
  */
  void BattleUISkillSelectLowerDraw::PushBtn( ButtonId buttonId )
  {
    //  コールバックで入力結果を渡す
    if( m_initParam.pDelegate == NULL )
    {
      return;
    }
    
    //  ボタンのタッチリリースアニメをする
    UIResponder::GetButtonManager()->StartSelectedAct( buttonId );
  }

  /**
  * @brief  特殊ボタンを発動状態にさせるかどうか
  */
  void BattleUISkillSelectLowerDraw::InvokeSpecialBtn( SpecialButtonType type, bool bFlg )
  {
    GFL_ASSERT( m_pProcSpecialButtonInst );

    m_pProcSpecialButtonInst->InvokeSpecial( bFlg );

    //  全力ボタンありなら
    if( m_initParam.specialSelectParam.type == SPECIAL_BUTTON_TYPE_ZENRYOKU )
    {
      //  項目を通常と全力で切り替え
      if( bFlg )
      {
        _SetState( STATE_TYPE_INPUT_ZENRYOKU_SKILL_SELECT_LIST );
      }
      else
      {
        if( m_state == STATE_TYPE_INPUT_ZENRYOKU_SKILL_SELECT_LIST )
        {
          _SetSubStateZenryokuSelect( SUB_STATE_ZENRYOKU_SELECT_TYPE_OUT );
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }
    else
    if (m_initParam.specialSelectParam.type == SPECIAL_BUTTON_TYPE_ULTRABURST)
    {
      ::app::tool::ButtonManager* pButtonManager = UIView::GetButtonManager();
      if (bFlg == true)
      {
        pButtonManager->SetButtonSelectSE(BUTTON_ID_ULTRABURST, SEQ_SE_W742);
      }
      else
      {
        pButtonManager->SetButtonSelectSE(BUTTON_ID_ULTRABURST, SEQ_SE_W741);
      }
    }
  }

  /**
  * @brief  ヘルプメッセージテキスト設定
  */
  void BattleUISkillSelectLowerDraw::SetHelpMessageId( const s32 msgId )
  {
    bool  bBlankMsg = false;

    //  ショートカットキーテキストラベルの場合は表示するかフラグしだい
    if( msgId == str_LA_waza )
    {
      //  表示フラグが立っていないなら空文字フラグを立てる
      bBlankMsg = ( m_bPutWazaInfoShortCutKeyText == false );
    }
    
    if( ( msgId != -1 ) && ( bBlankMsg == false ) )
    {
      UIView::SetTextboxPaneMessage( LYT_WK_TYPE_SCREEN, PANENAME_BATTLE_SKL_LOW_000_PANE_TEXTBOX_00, msgId );
    }
    else
    {
      //  空文字入れる
      UIView::SetTextboxPaneString( LYT_WK_TYPE_SCREEN, PANENAME_BATTLE_SKL_LOW_000_PANE_TEXTBOX_00, L"" );
    }
  }

//  ここからは SkillButtonクラスの定義
  BattleUISkillSelectLowerDraw::SkillButton::~SkillButton()
  {
    GFL_SAFE_DELETE( m_pZenryokuButtonInst );
    GFL_SAFE_DELETE( m_pNormalButtonInst );
  }

  void BattleUISkillSelectLowerDraw::SkillButton::Initialize( 
    NORMAL_SKILL_SETUP_DATA& rNormalSetupParam, ZENRYOKU_SKILL_SETUP_DATA& rZenryokuSetupParam, 
    const SKILL_SELECT_PARAM& rSkillParam, ::app::tool::AppCommonGrpIconData* pAppCommonGrpIconData,
    gfl2::heap::HeapBase* pHeap, ::app::util::G2DUtil* pG2DUtil )
  {
    GFL_ASSERT( pG2DUtil );
    GFL_ASSERT( pHeap );

    m_skillParam  = rSkillParam;

    //  通常と全力用のインスタンスを用意
    m_pNormalButtonInst   = GFL_NEW( pHeap ) parts::BattleUISkillButtonPartsLayout( pG2DUtil, LYT_WK_TYPE_SCREEN );
    m_pZenryokuButtonInst = GFL_NEW( pHeap ) parts::BattleUIZenryokuSkillButtonPartsLayout( pG2DUtil, LYT_WK_TYPE_SCREEN );

    m_pNormalButtonInst->Initialize( rNormalSetupParam.init, rNormalSetupParam.pane, pAppCommonGrpIconData, pHeap );
    m_pZenryokuButtonInst->Initialize( rZenryokuSetupParam.init, rZenryokuSetupParam.pane, pAppCommonGrpIconData, pHeap );

    //  通常わざのボタン設定
    {
      m_pNormalButtonInst->SetId( rSkillParam.normal.base.wazaId );
      m_pNormalButtonInst->SetPPNum( rSkillParam.normal.ppNowNum, rSkillParam.normal.ppMaxNum );
      m_pNormalButtonInst->SetInfoIconVisible( rSkillParam.bInfoIconVisible );
      m_pNormalButtonInst->SetKoukaType( rSkillParam.normal.base.koukaType );
      m_pNormalButtonInst->SetKoukaTypeVisible( rSkillParam.normal.base.bKoukaTypePutFlg );
    }

    //  全力わざのボタン設定
    {
      if( rSkillParam.zenryoku.base.wazaId == WAZANO_NULL )
      {
        m_pZenryokuButtonInst->SetVisible( false );
      }
      else
      {
        m_pZenryokuButtonInst->SetName( rSkillParam.zenryoku.base.wazaId, rSkillParam.zenryoku.pName, ( rSkillParam.normal.ppNowNum == 0 ) );
        m_pZenryokuButtonInst->SetInfoIconVisible( rSkillParam.bInfoIconVisible );
        m_pZenryokuButtonInst->SetKoukaType( rSkillParam.zenryoku.base.koukaType );
        m_pZenryokuButtonInst->SetKoukaTypeVisible( rSkillParam.zenryoku.base.bKoukaTypePutFlg );
        m_pZenryokuButtonInst->SetWazaType( rSkillParam.zenryoku.type );

        //  効果が高い時は専用エフェクト使用
        if( rSkillParam.zenryoku.bNo1Power == true )
        {
          m_pZenryokuButtonInst->OnEffectPower();
        }
        else
        {
          m_pZenryokuButtonInst->OnEffectNormal();
        }
      }
    }
  }

  void BattleUISkillSelectLowerDraw::SkillButton::Change( const ChangeType type, ::app::tool::ButtonManager* pButtonManager )
  {
    GFL_ASSERT( pButtonManager );

    if( type == CHANGE_TYPE_NORMAL )
    {
      m_pNormalButtonInst->SetActive( true );
      m_pZenryokuButtonInst->SetActive( false );
      m_pZenryokuButtonInst->SetVisible( false );

      pButtonManager->SetButtonInputEnable( m_pNormalButtonInst->GetPanelButtonInfoEx().button_id, true );
      pButtonManager->SetButtonInputEnable( m_pNormalButtonInst->GetPanelInfoIconButtonInfoEx().button_id, true );

      pButtonManager->SetButtonInputEnable( m_pZenryokuButtonInst->GetPanelButtonInfoEx().button_id, false );
      pButtonManager->SetButtonInputEnable( m_pZenryokuButtonInst->GetPanelInfoIconButtonInfoEx().button_id, false );
    }
    else
    if( type == CHANGE_TYPE_ZENRYOKU )
    {
      bool  bZenryokuFlg  = false;
      //  全力わざがない場合は表示しない
      if( m_skillParam.zenryoku.base.wazaId == WAZANO_NULL )
      {
        bZenryokuFlg  = false;
      }
      else
      {
        bZenryokuFlg  = true;
      }

      m_pNormalButtonInst->SetActive( false );
      m_pZenryokuButtonInst->SetActive( true );
      m_pZenryokuButtonInst->SetVisible( bZenryokuFlg );

      pButtonManager->SetButtonInputEnable( m_pNormalButtonInst->GetPanelButtonInfoEx().button_id, false );
      pButtonManager->SetButtonInputEnable( m_pNormalButtonInst->GetPanelInfoIconButtonInfoEx().button_id, false );

      pButtonManager->SetButtonInputEnable( m_pZenryokuButtonInst->GetPanelButtonInfoEx().button_id, bZenryokuFlg );
      pButtonManager->SetButtonInputEnable( m_pZenryokuButtonInst->GetPanelInfoIconButtonInfoEx().button_id, bZenryokuFlg );
    }

    m_type  = type;
  }

  gfl2::lyt::LytPane*  BattleUISkillSelectLowerDraw::SkillButton::GetCursorNullPane()
  {
    if( m_type == CHANGE_TYPE_NORMAL )
    {
      if( m_pNormalButtonInst->IsVisible() )
      {
        return  m_pNormalButtonInst->GetCursorNullPane();
      }
    }
    else
    if( m_type == CHANGE_TYPE_ZENRYOKU )
    {
      if( m_pZenryokuButtonInst->IsVisible() )
      {
        return  m_pZenryokuButtonInst->GetCursorNullPane();
      }
    }

    return  NULL;
  }

  void BattleUISkillSelectLowerDraw::SkillButton::SetActive( const bool bFlg )
  {
    if( m_type == CHANGE_TYPE_NORMAL )
    {
      m_pNormalButtonInst->SetActive( bFlg );
    }
    else if( m_type == CHANGE_TYPE_ZENRYOKU )
    {
      m_pZenryokuButtonInst->SetActive( bFlg );
    }
  }

  void BattleUISkillSelectLowerDraw::SkillButton::SetVisible( const bool bFlg )
  {
    if( m_type == CHANGE_TYPE_NORMAL )
    {
    }
    else if( m_type == CHANGE_TYPE_ZENRYOKU )
    {
      if( m_skillParam.zenryoku.base.wazaId == WAZANO_NULL )
      {
        m_pZenryokuButtonInst->SetVisible( false );
      }
      else
      {
        m_pZenryokuButtonInst->SetVisible( bFlg );
      }
    }

    m_pNormalButtonInst->SetVisible( bFlg );
  }

  bool BattleUISkillSelectLowerDraw::SkillButton::IsInfoIconVisible()
  {
    if( m_type == CHANGE_TYPE_NORMAL )
    {
      if( m_pNormalButtonInst->IsVisible() )
      {
        return  m_pNormalButtonInst->IsInfoIconVisible();
      }
    }
    else if( m_type == CHANGE_TYPE_ZENRYOKU )
    {
      if( m_pZenryokuButtonInst->IsVisible() )
      {
        return  m_pZenryokuButtonInst->IsInfoIconVisible();
      }
    }

    return  false;
  }

  s32  BattleUISkillSelectLowerDraw::SkillButton::GetPanelButtonId()
  {
    if( m_type == CHANGE_TYPE_NORMAL )
    {
      return  m_pNormalButtonInst->GetPanelButtonInfoEx().button_id;
    }
    else if( m_type == CHANGE_TYPE_ZENRYOKU )
    {
      return  m_pZenryokuButtonInst->GetPanelButtonInfoEx().button_id;
    }

    return  -1;
  }

  s32  BattleUISkillSelectLowerDraw::SkillButton::GetInfoButtonId()
  {
    if( m_type == CHANGE_TYPE_NORMAL )
    {
      return  m_pNormalButtonInst->GetPanelInfoIconButtonInfoEx().button_id;
    }
    else if( m_type == CHANGE_TYPE_ZENRYOKU )
    {
      return  m_pZenryokuButtonInst->GetPanelInfoIconButtonInfoEx().button_id;
    }

    return  -1;
  }

  //  ここからはZenryokuButtonクラスの定義

  /** 全力ボタン */
  void BattleUISkillSelectLowerDraw::ZenryokuButton::Initialize( const INIT_PARAM& initParam )
  {
    SpecialButtonBase::Initialize( initParam );

    gfl2::lyt::LytWk* pLayoutWork = initParam.p_g2d_util->GetLayoutWork( initParam.lyt_wk_id );
    GFL_ASSERT( pLayoutWork );

    gfl2::lyt::LytParts*  pRootPartsPane  = pLayoutWork->GetPartsPane( initParam.pane_id );

    const gfl2::lyt::LytMultiResID& rMultiResId = initParam.p_g2d_util->GetLayoutResourceID( initParam.res_id );

    //  カーソルNullペイン取得
    {
      m_pCursorNullPane = pLayoutWork->GetPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_002_PANE_NULL_CUR, &rMultiResId );
      GFL_ASSERT( m_pCursorNullPane );
    }

    //  ALL
    {
      m_pAllPane  = pLayoutWork->GetPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_002_PANE_ALL, &rMultiResId );
      GFL_ASSERT( m_pAllPane );
    }

    //  ボタン登録データ作成
    {
      m_buttonInfoEx.button_id  = initParam.button_id;
      m_buttonInfoEx.picture_pane = pLayoutWork->GetPane( initParam.pane_id );
      GFL_ASSERT( m_buttonInfoEx.picture_pane );

      m_buttonInfoEx.bound_pane  = pLayoutWork->GetBoundingPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_002_PANE_BOUND_00, &rMultiResId );
      GFL_ASSERT( m_buttonInfoEx.bound_pane );

      //!< タッチ時のアニメ
      m_buttonInfoEx.touch_anime_index      = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_TOUCH;

      //!< タッチ決定時のアニメ
      m_buttonInfoEx.release_anime_index    = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_RELEASE;

      //!< タッチ状態からのキャンセルアニメ
      m_buttonInfoEx.cancel_anime_index     = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_CANSEL;

      //!< キー決定時のアニメ ( touch => release の連続アニメ )
      m_buttonInfoEx.key_select_anime_index = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_TOUCH_RELEASE;

      //!< アクティブアニメ
      m_buttonInfoEx.active_anime_index     = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_ACTIVE;

      //!< パッシブアニメ
      m_buttonInfoEx.passive_anime_index    = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_PASSIVE;
    }
  }

  void BattleUISkillSelectLowerDraw::ZenryokuButton::SetActive( const bool bFlg )
  {
  }

  void BattleUISkillSelectLowerDraw::ZenryokuButton::SetVislble( const bool bFlg )
  {
    //  表示する時はエフェクトアニメ再生
    if( bFlg == true )
    {
      //  ボタンのスケールを元に戻す必要がある。
      gfl2::math::VEC2  scale = gfl2::math::VEC2( 1.0f, 1.0f );
      m_initParam.p_g2d_util->SetScale( m_pAllPane, scale );

      //  再生しないのであれば再生する
      if( m_initParam.p_g2d_util->IsAnimeEnd( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_EFFECT ) == true )
      {
        m_initParam.p_g2d_util->StartAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_EFFECT );
      }
    }

    m_initParam.p_g2d_util->SetPaneVisible( m_buttonInfoEx.picture_pane, bFlg );
  }

  void BattleUISkillSelectLowerDraw::ZenryokuButton::InvokeSpecial( const bool bFlg )
  {
    if( bFlg )
    {
      m_initParam.p_g2d_util->StartAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_SELECT );
    }
    else
    {
      m_initParam.p_g2d_util->StartAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_ZENRYOKU_SELECTED_CANSEL );
    }

    m_bInvoke = bFlg;
  }

  //  ここからはMegaButtonクラスの定義

  /** メガ進化ボタン */
  void BattleUISkillSelectLowerDraw::MegaButton::Initialize( const INIT_PARAM& initParam )
  {
    SpecialButtonBase::Initialize( initParam );

    SpecialButtonBase::Initialize( initParam );

    gfl2::lyt::LytWk* pLayoutWork = initParam.p_g2d_util->GetLayoutWork( initParam.lyt_wk_id );
    GFL_ASSERT( pLayoutWork );

    gfl2::lyt::LytParts*  pRootPartsPane  = pLayoutWork->GetPartsPane( initParam.pane_id );

    const gfl2::lyt::LytMultiResID& rMultiResId = initParam.p_g2d_util->GetLayoutResourceID( initParam.res_id ); 

    //  カーソルNullペイン取得
    {
      m_pCursorNullPane = pLayoutWork->GetPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_001_PANE_NULL_CUR, &rMultiResId );
      GFL_ASSERT( m_pCursorNullPane );
    }

    //  ALL
    {
      m_pAllPane  = pLayoutWork->GetPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_001_PANE_ALL, &rMultiResId );
      GFL_ASSERT( m_pAllPane );
    }

    //  ボタン登録データ作成
    {
      m_buttonInfoEx.button_id  = initParam.button_id;
      m_buttonInfoEx.picture_pane = pLayoutWork->GetPane( initParam.pane_id );
      GFL_ASSERT( m_buttonInfoEx.picture_pane );

      m_buttonInfoEx.bound_pane  = pLayoutWork->GetBoundingPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_001_PANE_BOUND_00, &rMultiResId );
      GFL_ASSERT( m_buttonInfoEx.bound_pane );

      //!< タッチ時のアニメ
      m_buttonInfoEx.touch_anime_index      = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_TOUCH;

      //!< タッチ決定時のアニメ
      m_buttonInfoEx.release_anime_index    = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_RELEASE;

      //!< タッチ状態からのキャンセルアニメ
      m_buttonInfoEx.cancel_anime_index     = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_CANSEL;

      //!< キー決定時のアニメ ( touch => release の連続アニメ )
      m_buttonInfoEx.key_select_anime_index = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_TOUCH_RELEASE;

      //!< アクティブアニメ
      m_buttonInfoEx.active_anime_index     = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_ACTIVE;

      //!< パッシブアニメ
      m_buttonInfoEx.passive_anime_index    = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_PASSIVE;
    }
  }

  void BattleUISkillSelectLowerDraw::MegaButton::SetActive( const bool bFlg )
  {
  }

  void BattleUISkillSelectLowerDraw::MegaButton::SetVislble( const bool bFlg )
  {
    //  表示する時はエフェクトアニメ再生
    if( bFlg == true )
    {
      //  発動していない時はボタンのスケールを元に戻す必要がある。
      if( m_bInvoke == false )
      {
        gfl2::math::VEC2  scale = gfl2::math::VEC2( 1.0f, 1.0f );
        m_initParam.p_g2d_util->SetScale( m_pAllPane, scale );

        SetEnableEffect( false );
      }
      else
      {
        SetEnableEffect( true );
      }
    }

    m_initParam.p_g2d_util->SetPaneVisible( m_buttonInfoEx.picture_pane, bFlg );
  }

  void BattleUISkillSelectLowerDraw::MegaButton::InvokeSpecial( const bool bFlg )
  {
    if( bFlg )
    {
      //  発動する
      if( m_bInvoke == false )
      {
        m_initParam.p_g2d_util->StartAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_TOUCHANIME );
        m_initParam.p_g2d_util->StartAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_SELECT );

        //  発動していない時はエフェクトを出す
        SetEnableEffect( true );
      }
    }
    else
    {
      if( m_bInvoke )
      {
        m_initParam.p_g2d_util->StartAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_SELECT_CANSEL );

        //  発動中はエフェクトを消す
        SetEnableEffect( false );
      }
    }

    m_bInvoke = bFlg;
  }

  void BattleUISkillSelectLowerDraw::MegaButton::SetEnableEffect( const bool bFlg )
  {
    if( bFlg == true )
    {
      m_initParam.p_g2d_util->StopAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_OFF_LOOP );
      if( m_initParam.p_g2d_util->IsAnimeEnd( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_EFFECT ) == true )
      {
        m_initParam.p_g2d_util->StartAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_EFFECT );
      }
    }
    else
    {
      m_initParam.p_g2d_util->StopAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_EFFECT );
      if( m_initParam.p_g2d_util->IsAnimeEnd( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_OFF_LOOP ) == true )
      {
        m_initParam.p_g2d_util->StartAnime( m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_MEGA_OFF_LOOP );
      }
    }
  }


  //  ここからはUltraBurstButtonクラスの定義

  /** ウルトラバーストボタン */
  void BattleUISkillSelectLowerDraw::UltraBurstButton::Initialize(const INIT_PARAM& initParam)
  {
    SpecialButtonBase::Initialize(initParam);

    SpecialButtonBase::Initialize(initParam);

    gfl2::lyt::LytWk* pLayoutWork = initParam.p_g2d_util->GetLayoutWork(initParam.lyt_wk_id);
    GFL_ASSERT(pLayoutWork);

    gfl2::lyt::LytParts*  pRootPartsPane = pLayoutWork->GetPartsPane(initParam.pane_id);

    const gfl2::lyt::LytMultiResID& rMultiResId = initParam.p_g2d_util->GetLayoutResourceID(initParam.res_id);

    //  カーソルNullペイン取得
    {
      m_pCursorNullPane = pLayoutWork->GetPane(pRootPartsPane, PANENAME_BATTLE_BTN_LOW_001_PANE_NULL_CUR, &rMultiResId);
      GFL_ASSERT(m_pCursorNullPane);
    }

    //  ALL
    {
      m_pAllPane = pLayoutWork->GetPane(pRootPartsPane, PANENAME_BATTLE_BTN_LOW_001_PANE_ALL, &rMultiResId);
      GFL_ASSERT(m_pAllPane);
    }

    //  ボタン登録データ作成
    {
      m_buttonInfoEx.button_id = initParam.button_id;
      m_buttonInfoEx.picture_pane = pLayoutWork->GetPane(initParam.pane_id);
      GFL_ASSERT(m_buttonInfoEx.picture_pane);

      m_buttonInfoEx.bound_pane = pLayoutWork->GetBoundingPane(pRootPartsPane, PANENAME_BATTLE_BTN_LOW_001_PANE_BOUND_00, &rMultiResId);
      GFL_ASSERT(m_buttonInfoEx.bound_pane);

      //!< タッチ時のアニメ
      m_buttonInfoEx.touch_anime_index = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_TOUCH;

      //!< タッチ決定時のアニメ
      m_buttonInfoEx.release_anime_index = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_RELEASE;

      //!< タッチ状態からのキャンセルアニメ
      m_buttonInfoEx.cancel_anime_index = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_CANSEL;

      //!< キー決定時のアニメ ( touch => release の連続アニメ )
      m_buttonInfoEx.key_select_anime_index = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_TOUCH_RELEASE;

      //!< アクティブアニメ
      m_buttonInfoEx.active_anime_index = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_ACTIVE;

      //!< パッシブアニメ
      m_buttonInfoEx.passive_anime_index = LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_PASSIVE;
    }
  }

  void BattleUISkillSelectLowerDraw::UltraBurstButton::SetActive(const bool bFlg)
  {
  }

  void BattleUISkillSelectLowerDraw::UltraBurstButton::SetVislble(const bool bFlg)
  {
    //  表示する時はエフェクトアニメ再生
    if (bFlg == true)
    {
      //  発動していない時はボタンのスケールを元に戻す必要がある。
      if (m_bInvoke == false)
      {
        gfl2::math::VEC2  scale = gfl2::math::VEC2(1.0f, 1.0f);
        m_initParam.p_g2d_util->SetScale(m_pAllPane, scale);

        SetEnableEffect(false);
      }
      else
      {
        SetEnableEffect(true);
      }
    }

    m_initParam.p_g2d_util->SetPaneVisible(m_buttonInfoEx.picture_pane, bFlg);
  }

  void BattleUISkillSelectLowerDraw::UltraBurstButton::InvokeSpecial(const bool bFlg)
  {
    if (bFlg)
    {
      //  発動する
      if (m_bInvoke == false)
      {
        //m_initParam.p_g2d_util->StartAnime(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_TOUCHANIME);
        m_initParam.p_g2d_util->StartAnime(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_MODE_ON);

        //  発動していない時はエフェクトを出す
        SetEnableEffect(true);
      }
    }
    else
    {
      if (m_bInvoke)
      {
        m_initParam.p_g2d_util->StartAnime(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_MODE_CANCEL);

        //  発動中はエフェクトを消す
        SetEnableEffect(false);
      }
    }

    m_bInvoke = bFlg;
  }

  void BattleUISkillSelectLowerDraw::UltraBurstButton::SetEnableEffect(const bool bFlg)
  {
    //常駐エフェクトのみ
    if (m_initParam.p_g2d_util->IsAnimeEnd(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_EFFECTLOOP) == true)
    {
      m_initParam.p_g2d_util->StartAnime(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_EFFECTLOOP);
    }
    /*
    if (bFlg == true)
    {
      m_initParam.p_g2d_util->StopAnime(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_OFF_LOOP);
      if (m_initParam.p_g2d_util->IsAnimeEnd(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_EFFECTLOOP) == true)
      {
        m_initParam.p_g2d_util->StartAnime(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_EFFECTLOOP);
      }
    }
    else
    {
      m_initParam.p_g2d_util->StopAnime(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_EFFECTLOOP);
      if (m_initParam.p_g2d_util->IsAnimeEnd(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_OFF_LOOP) == true)
      {
        m_initParam.p_g2d_util->StartAnime(m_initParam.lyt_wk_id, LA_SKLLOW_BATTLE_SKL_LOW_000__BTN_UB_OFF_LOOP);
      }
    }
    */
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

