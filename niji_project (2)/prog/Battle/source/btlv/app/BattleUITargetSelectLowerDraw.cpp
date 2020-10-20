//=============================================================================
/**
 * @file BattleUITargetSelectLowerDraw.h
 * @brief バトル下画面 >> 「わざ対象選択」画面
 * @author yuto_uchida
 * @date 2015.03.31
 */
//=============================================================================
#include "BattleUITargetSelectLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "PokeTool/include/PokeTool.h"

#include "niji_conv_header/app/battle/res2d/trglow.h"
#include "niji_conv_header/app/battle/res2d/trglow_pane.h"
#include "niji_conv_header/app/battle/res2d/trglow_anim_list.h"
#include "niji_conv_header/message/msg_btl_app.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUITargetSelectLowerDraw::BattleUITargetSelectLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );
    GFL_ASSERT( param.pBtlAppMsgData );
    GFL_ASSERT( param.pBtlWordSet );

    _Clear();

    m_initParam = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUITargetSelectLowerDraw::~BattleUITargetSelectLowerDraw()
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItemObjectArray );
    for( u32 i = 0; i < loopNum; ++i )
    {
      GFL_SAFE_DELETE( m_pSelectItemObjectArray[ i ] );
    }

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void BattleUITargetSelectLowerDraw::Initialize()
  {
    //  2D環境作成
    _Initialize2D( m_initParam.pLytBuff );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );
    GFL_ASSERT( pLayoutWork );

    //  描画モードによって画面を変える
    {
      if( m_initParam.drawMode == DRAW_MODE_NORMAL )
      {
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_TRGLOW_BATTLE_TRG_LOW_000_NORMAL );
      }
      else
      if( m_initParam.drawMode == DRAW_MODE_ROYAL )
      {
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_TRGLOW_BATTLE_TRG_LOW_000_BR );
      }
    }

    //  対象選択項目のオブジェクト作成
    {
      SelectItemObject::INIT_SELECT_ITEM_PARAM  aInitParamBySelectItem[ SELECT_ITEM_POS_MAX ] =
      {
        /*
          SELECT_ITEM_POS_LEFT_DOWN = 0,
        */
        {
          m_initParam.aSelectItem[ SELECT_ITEM_POS_LEFT_DOWN ],

          //  項目のペインルートid
          PANENAME_BATTLE_TRG_LOW_000_PANE_BTNTRGT_02,

          //  プレイヤー名のテキストペインid(ロイヤル専用)
          PANENAME_BATTLE_TRG_LOW_000_PANE_BR_TEXT_02,

          //  アニメid

          //  性別
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02__PARTS_GENDER_ICON_MALE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02__PARTS_GENDER_ICON_FEMALE,

          //  仲間・敵の色
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_COLOR_00,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_COLOR_01,

          //  仲間・敵の選択不可能時
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_COLOR_02,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_COLOR_03,

          //  選択(味方・敵)
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_COLOR_04,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_COLOR_05,

          //  ボタンタッチ
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_TOUCH,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_CANSEL,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_TOUCH_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_PASSIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02_ACTIVE,

          //  効果アイコンアニメ
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02__PARTS_KOUKA_ACTIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02__PARTS_KOUKA_PASSIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02__PARTS_KOUKA_KOUKA_02,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02__PARTS_KOUKA_KOUKA_01,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02__PARTS_KOUKA_KOUKA_00,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_02__PARTS_KOUKA_KOUKA_03,

          //  グローブアニメ
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_02_BLUE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_02_YELLOW,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_02_GREEN,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_02_RED,
        },

        /*
          SELECT_ITEM_POS_RIGHT_DOWN,
        */
        {
          m_initParam.aSelectItem[ SELECT_ITEM_POS_RIGHT_DOWN ],

          //  項目のペインルートid
          PANENAME_BATTLE_TRG_LOW_000_PANE_BTNTRGT_03,

          //  プレイヤー名のテキストペインid(ロイヤル専用)
          PANENAME_BATTLE_TRG_LOW_000_PANE_BR_TEXT_03,

          //  アニメid

          //  性別
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03__PARTS_GENDER_ICON_MALE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03__PARTS_GENDER_ICON_FEMALE,

          //  仲間・敵の色
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_COLOR_00,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_COLOR_01,

          //  仲間・敵の選択不可能時
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_COLOR_02,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_COLOR_03,

          //  選択(味方・敵)
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_COLOR_04,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_COLOR_05,

          //  ボタンタッチ
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_TOUCH,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_CANSEL,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_TOUCH_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_PASSIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03_ACTIVE,

          //  効果アイコンアニメ
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03__PARTS_KOUKA_ACTIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03__PARTS_KOUKA_PASSIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03__PARTS_KOUKA_KOUKA_02,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03__PARTS_KOUKA_KOUKA_01,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03__PARTS_KOUKA_KOUKA_00,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_03__PARTS_KOUKA_KOUKA_03,

          //  グローブアニメ
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_03_BLUE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_03_YELLOW,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_03_GREEN,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_03_RED,
        },

        /*
          SELECT_ITEM_POS_LEFT_UP,
        */
        {
          m_initParam.aSelectItem[ SELECT_ITEM_POS_LEFT_UP ],

          //  項目のペインルートid
          PANENAME_BATTLE_TRG_LOW_000_PANE_BTNTRGT_00,

          //  プレイヤー名のテキストペインid(ロイヤル専用)
          PANENAME_BATTLE_TRG_LOW_000_PANE_BR_TEXT_00,

          //  アニメid

          //  性別
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00__PARTS_GENDER_ICON_MALE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00__PARTS_GENDER_ICON_FEMALE,

          //  仲間・敵の色
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_COLOR_00,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_COLOR_01,

          //  仲間・敵の選択不可能時
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_COLOR_02,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_COLOR_03,

          //  選択(味方・敵)
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_COLOR_04,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_COLOR_05,

          //  ボタンタッチ
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_TOUCH,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_CANSEL,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_TOUCH_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_PASSIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00_ACTIVE,

          //  効果アイコンアニメ
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00__PARTS_KOUKA_ACTIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00__PARTS_KOUKA_PASSIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00__PARTS_KOUKA_KOUKA_02,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00__PARTS_KOUKA_KOUKA_01,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00__PARTS_KOUKA_KOUKA_00,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_00__PARTS_KOUKA_KOUKA_03,

          //  グローブアニメ
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_00_BLUE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_00_YELLOW,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_00_GREEN,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_00_RED,
        },

        /*
          SELECT_ITEM_POS_RIGHT_UP,
        */
        {
          m_initParam.aSelectItem[ SELECT_ITEM_POS_RIGHT_UP ],

          //  項目のペインルートid
          PANENAME_BATTLE_TRG_LOW_000_PANE_BTNTRGT_01,

          //  プレイヤー名のテキストペインid(ロイヤル専用)
          PANENAME_BATTLE_TRG_LOW_000_PANE_BR_TEXT_01,

          //  アニメid
          //  性別
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01__PARTS_GENDER_ICON_MALE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01__PARTS_GENDER_ICON_FEMALE,

          //  仲間・敵の色
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_COLOR_00,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_COLOR_01,

          //  仲間・敵の選択不可能時
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_COLOR_02,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_COLOR_03,

          //  選択(味方・敵)
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_COLOR_04,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_COLOR_05,

          //  ボタンタッチ
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_TOUCH,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_CANSEL,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_TOUCH_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_PASSIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01_ACTIVE,

          //  効果アイコンアニメ
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01__PARTS_KOUKA_ACTIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01__PARTS_KOUKA_PASSIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01__PARTS_KOUKA_KOUKA_02,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01__PARTS_KOUKA_KOUKA_01,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01__PARTS_KOUKA_KOUKA_00,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BTNTRGT_01__PARTS_KOUKA_KOUKA_03,

          //  グローブアニメ
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_01_BLUE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_01_YELLOW,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_01_GREEN,
          LA_TRGLOW_BATTLE_TRG_LOW_000__GLOBE_01_RED,
        },
      };

      for( s32 i = 0; i < GFL_NELEMS( m_initParam.aSelectItem ); ++i )
      {
        m_pSelectItemObjectArray[ i ] = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) SelectItemObject();
        GFL_ASSERT( m_pSelectItemObjectArray[ i ] );

        m_pSelectItemObjectArray[ i ]->Initialize( aInitParamBySelectItem[ i ], this, LYT_WK_TYPE_SCREEN, 0, m_initParam.pUtilHeap, m_initParam.pMenuCursorLytBuff );
      }
    }

    //  ボタン作成
    {
      const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      gfl2::lyt::LytParts*  pBackBtnParts = pLayoutWork->GetPartsPane( PANENAME_BATTLE_TRG_LOW_000_PANE_BG_BACK );
      GFL_ASSERT( pBackBtnParts );

      //  ボタン設定データ定義一覧
      const ::app::ui::ButtonInfoEx    aButtonPaneIdxList[ BUTTON_ID_MAX ] =
      {
        //  戻るボタン
        {
          BUTTON_ID_BACK,
          pBackBtnParts,
          pLayoutWork->GetBoundingPane( pBackBtnParts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rMultiResId ),
          LA_TRGLOW_BATTLE_TRG_LOW_000__BG_BACK_TOUCH,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BG_BACK_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BG_BACK_CANSEL,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BG_BACK_TOUCH_RELEASE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BG_BACK_ACTIVE,
          LA_TRGLOW_BATTLE_TRG_LOW_000__BG_BACK_PASSIVE,
        },

        //  各項目のボタン
        m_pSelectItemObjectArray[ SELECT_ITEM_POS_LEFT_DOWN ]->CreateButtonInfoEx( BUTTON_ID_LEFT_DOWN ),
        m_pSelectItemObjectArray[ SELECT_ITEM_POS_LEFT_UP ]->CreateButtonInfoEx( BUTTON_ID_LEFT_UP ),
        m_pSelectItemObjectArray[ SELECT_ITEM_POS_RIGHT_DOWN ]->CreateButtonInfoEx( BUTTON_ID_RIGHT_DOWN ),
        m_pSelectItemObjectArray[ SELECT_ITEM_POS_RIGHT_UP ]->CreateButtonInfoEx( BUTTON_ID_RIGHT_UP ),
      };

      //  ボタン登録
      UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN ), aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
    }

    //  ボタンSE設定
    if( m_initParam.bUseInputSE )
    {
      const u32 aButtonSE[ BUTTON_ID_MAX ][ 2 ] =
      {
        { BUTTON_ID_BACK,       SEQ_SE_CANCEL1 },
        { BUTTON_ID_LEFT_DOWN,  SEQ_SE_DECIDE1 },
        { BUTTON_ID_LEFT_UP,    SEQ_SE_DECIDE1 },
        { BUTTON_ID_RIGHT_DOWN, SEQ_SE_DECIDE1 },
        { BUTTON_ID_RIGHT_UP,   SEQ_SE_DECIDE1 },
      };

      ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

      for( u32 i = 0; i < BUTTON_ID_MAX; ++i )
      {
        pButtonManager->SetButtonSelectSE( aButtonSE[ i ][ 0 ], aButtonSE[ i ][ 1 ] );
      }
    }

    //  各項目の状態設定をする
    {
      //  各項目が選択設定
      for( s32 i = 0 ; i < GFL_NELEMS( m_pSelectItemObjectArray ); ++i )
      {
        m_pSelectItemObjectArray[ i ]->SetEnable( m_initParam.aSelectItem[ i ].bSelectPermission );
      }

      //  リンクボタンidがあればリンクする
      {
        ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

        ::app::tool::ButtonManager::ButtonId aLinkButtonId[ SELECT_ITEM_POS_MAX ];

        //  ButtonId用のNULL値で初期化
        {
          u32 loopNum = GFL_NELEMS( aLinkButtonId );
          for( u32 i = 0; i < loopNum; ++i )
          {
            aLinkButtonId[ i ]  = ::app::tool::ButtonManager::BUTTON_ID_NULL;
          }
        }
        

        s32 linkDataNum = 0;
        {
          for( s32 i = 0; i < GFL_NELEMS( m_initParam.aLinkItemPos ); ++i )
          {
            if( m_initParam.aLinkItemPos[ i ] == SELECT_ITEM_POS_NONE )
            {
              break;
            }

            GFL_ASSERT_STOP( m_initParam.aLinkItemPos[ i ] < GFL_NELEMS( m_pSelectItemObjectArray ) );

            aLinkButtonId[ linkDataNum ]  = m_pSelectItemObjectArray[ m_initParam.aLinkItemPos[ i ] ]->GetButtonId();
            ++linkDataNum;
          }
        }

        if( 0 < linkDataNum )
        {
          pButtonManager->LinkButton( aLinkButtonId, linkDataNum );
        }
      }
    }

    //  いったんUpdateしないとアニメによる画面切り替えが更新しない
    UIView::Update();

    //  Start関数が呼ばれるまで非表示
    UIView::SetVisible( false );

    //  入力コールバック設定
    UIView::SetInputListener( this );
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void BattleUITargetSelectLowerDraw::Terminate()
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItemObjectArray );
    for( u32 i = 0; i < loopNum; ++i )
    {
      m_pSelectItemObjectArray[ i ]->Terminate();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   使える状態か
    */
  //------------------------------------------------------------------
  bool BattleUITargetSelectLowerDraw::IsReady()
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItemObjectArray );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItemObjectArray[ i ]->IsReady() == false )
      {
        return  false;
      }
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   削除可能か
    */
  //------------------------------------------------------------------
  bool BattleUITargetSelectLowerDraw::IsDelete()
  {
    bool  bRet  = true;

    u32 loopNum = GFL_NELEMS( m_pSelectItemObjectArray );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItemObjectArray[ i ]->IsDelete() == false )
      {
        bRet  = false;
        break;
      }
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
  void BattleUITargetSelectLowerDraw::Update( void )
  {
    _UpdateState();

    UIView::Update();

    {
      u32 loopNum = GFL_NELEMS( m_pSelectItemObjectArray );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pSelectItemObjectArray[ i ]->Update();
      }
    }
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUITargetSelectLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }

    {
      u32 loopNum = GFL_NELEMS( m_pSelectItemObjectArray );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pSelectItemObjectArray[ i ]->Draw( m_initParam.pRenderingManager, displayNo, LOW_VIEW_ORDER_OTHER );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   バックボタンを押す
    */
  //------------------------------------------------------------------
  void BattleUITargetSelectLowerDraw::PushBtnBack()
  {
    if( UIResponder::GetButtonManager()->IsButtonVisible( BUTTON_ID_BACK ) == false )
    {
      return;
    }

    //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
    UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_ID_BACK );
  }

  //------------------------------------------------------------------
  /**
    * @brief   カレントカーソル項目を決定
    */
  //------------------------------------------------------------------
  void BattleUITargetSelectLowerDraw::PushBtnCurrentCursor()
  {
    PushBtnSelectCursor( m_initParam.cursorItemPos );
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル項目を指定して決定
    */
  //------------------------------------------------------------------
  void BattleUITargetSelectLowerDraw::PushBtnSelectCursor( const SelectItemPos itemPos )
  {
    m_pSelectItemObjectArray[ itemPos ]->PushBtn();
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル移動
    */
  //------------------------------------------------------------------
  bool BattleUITargetSelectLowerDraw::MoveCursorUp()
  {
    //  移動データ
    const s32 moveDataNum = 2;
    const s32 aCursorMoveDataTable[ SELECT_ITEM_POS_MAX ][ moveDataNum ]  =
    {
      //  SELECT_ITEM_POS_LEFT_DOWN
      { SELECT_ITEM_POS_LEFT_UP, SELECT_ITEM_POS_RIGHT_UP },

      //  SELECT_ITEM_POS_RIGHT_DOWN
      { SELECT_ITEM_POS_RIGHT_UP, SELECT_ITEM_POS_LEFT_UP },

      //  SELECT_ITEM_POS_LEFT_UP(移動不可)
      { -1, -1 },

      //  SELECT_ITEM_POS_RIGHT_UP(移動不可)
      { -1, -1 },
    };

    return  _MoveCursor( aCursorMoveDataTable, m_initParam.cursorItemPos, SELECT_ITEM_POS_MAX );
  }

  bool BattleUITargetSelectLowerDraw::MoveCursorDown()
  {
    //  移動データ
    const s32 moveDataNum = 2;
    const s32 aCursorMoveDataTable[ SELECT_ITEM_POS_MAX ][ moveDataNum ]  =
    {
      //  SELECT_ITEM_POS_LEFT_DOWN(移動不可)
      { -1, -1 },

      //  SELECT_ITEM_POS_RIGHT_DOWN(移動不可)
      { -1, -1 },

      //  SELECT_ITEM_POS_LEFT_UP
      { SELECT_ITEM_POS_LEFT_DOWN, SELECT_ITEM_POS_RIGHT_DOWN },

      //  SELECT_ITEM_POS_RIGHT_UP
      { SELECT_ITEM_POS_RIGHT_DOWN, SELECT_ITEM_POS_LEFT_DOWN },
    };

    return  _MoveCursor( aCursorMoveDataTable, m_initParam.cursorItemPos, SELECT_ITEM_POS_MAX );
  }

  bool BattleUITargetSelectLowerDraw::MoveCursorLeft()
  {
    //  移動データ
    const s32 moveDataNum = 2;
    const s32 aCursorMoveDataTable[ SELECT_ITEM_POS_MAX ][ moveDataNum ]  =
    {
      //  SELECT_ITEM_POS_LEFT_DOWN(移動不可)
      { -1, -1 },

      //  SELECT_ITEM_POS_RIGHT_DOWN
      { SELECT_ITEM_POS_LEFT_DOWN, SELECT_ITEM_POS_LEFT_UP },

      //  SELECT_ITEM_POS_LEFT_UP(移動不可)
      { -1, -1 },

      //  SELECT_ITEM_POS_RIGHT_UP
      { SELECT_ITEM_POS_LEFT_UP, SELECT_ITEM_POS_LEFT_DOWN },
    };

    return  _MoveCursor( aCursorMoveDataTable, m_initParam.cursorItemPos, SELECT_ITEM_POS_MAX );
  }

  bool BattleUITargetSelectLowerDraw::MoveCursorRight()
  {
    //  移動データ
    const s32 moveDataNum = 2;
    const s32 aCursorMoveDataTable[ SELECT_ITEM_POS_MAX ][ moveDataNum ]  =
    {
      //  SELECT_ITEM_POS_LEFT_DOWN
      { SELECT_ITEM_POS_RIGHT_DOWN, SELECT_ITEM_POS_RIGHT_UP },

      //  SELECT_ITEM_POS_RIGHT_DOWN(移動不可)
      { -1, -1 },

      //  SELECT_ITEM_POS_LEFT_UP
      { SELECT_ITEM_POS_RIGHT_UP, SELECT_ITEM_POS_RIGHT_DOWN },

      //  SELECT_ITEM_POS_RIGHT_UP(移動不可)
      { -1, -1 },
    };

    return  _MoveCursor( aCursorMoveDataTable, m_initParam.cursorItemPos, SELECT_ITEM_POS_MAX );
  }

  //------------------------------------------------------------------
  /**
    * @brief   レイアウトワークを生成する
    */
  //------------------------------------------------------------------
  void BattleUITargetSelectLowerDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_NONE, },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
          //  選択画面
          {
            0,
            LYTRES_TRGLOW_BATTLE_TRG_LOW_000_BFLYT,
            LA_TRGLOW_BATTLE_TRG_LOW_000___NUM,
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
  void BattleUITargetSelectLowerDraw::_UpdateState()
  {
    if( m_state == STATE_TYPE_NONE )
    {
    }
    else if( m_state == STATE_TYPE_IN )
    {      
      //  アニメの終了を検知
      _SetState( STATE_TYPE_UPDATE );
    }
    else if( m_state == STATE_TYPE_UPDATE )
    {
    }
    else if( m_state == STATE_TYPE_OUT )
    {
      //  アニメの終了を検知
      _SetState( STATE_TYPE_END );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUITargetSelectLowerDraw::_SetState( const BattleUITargetSelectLowerDraw::StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      //  カーソル更新
      _SetCursor( m_initParam.cursorItemPos );
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
      //  カーソルはすべて非表示
      {
        u32 loopNum = GFL_NELEMS( m_pSelectItemObjectArray );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pSelectItemObjectArray[ i ]->SetVisibleCursor( false, ::app::tool::MenuCursor::CURSOR_ALL );
        }
      }
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル移動
    */
  //------------------------------------------------------------------
  bool  BattleUITargetSelectLowerDraw::_MoveCursor( const s32 aMoveDataTable[][ 2 ], const SelectItemPos currentCursorPos, const s32 rowNum )
  {
    GFL_ASSERT( currentCursorPos < rowNum );

    if( aMoveDataTable[ m_initParam.cursorItemPos ][ 0 ] == -1 )
    {
      return  false;
    }

    //  列数を取得
    const s32 colNum  = sizeof( aMoveDataTable[ 0 ] ) / sizeof( aMoveDataTable[ 0 ][ 0 ] );
    for( s32 i = 0; i < colNum; ++i )
    {
      //  現在のカーソル位置から移動できるかどうかチェック
      s32 chkIdx  = aMoveDataTable[ currentCursorPos ][ i ];
      if( chkIdx != -1 )
      {
        //  カーソル移動可能
        if( m_initParam.aSelectItem[ chkIdx ].bSelectPermission )
        {
          SelectItemPos oldCursorItemPos  = m_initParam.cursorItemPos;
          m_initParam.cursorItemPos = (SelectItemPos)aMoveDataTable[ currentCursorPos ][ i ];

          bool  bChangePosCursor  = false;
          if( oldCursorItemPos != m_initParam.cursorItemPos )
          {
            //  カーソル移動
            bChangePosCursor  = _SetCursor( m_initParam.cursorItemPos );
          }

          return  bChangePosCursor;
        }
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル設定
    */
  //------------------------------------------------------------------
  bool BattleUITargetSelectLowerDraw::_SetCursor( const SelectItemPos itemPos )
  {
    if( itemPos == SELECT_ITEM_POS_NONE )
    {
      GFL_ASSERT_STOP( 0 );
      return  false;
    }
 
    GFL_ASSERT( SELECT_ITEM_POS_MAX == GFL_NELEMS( m_pSelectItemObjectArray ) );

    bool bOldCursorActiveArray[ SELECT_ITEM_POS_MAX ];

    m_currentItemPos  = itemPos;

    //  一旦すべてのカーソルは非表示に
    {
      u32 loopNum = GFL_NELEMS( m_pSelectItemObjectArray );
      for( u32 i = 0; i < loopNum; ++i )
      {
        bOldCursorActiveArray[ i ]  = m_pSelectItemObjectArray[ i ]->IsVisibleCursor();

        m_pSelectItemObjectArray[ i ]->SetVisibleCursor( false, ::app::tool::MenuCursor::CURSOR_ALL );
      }
    }

    s32 linkItemChkIdx  = -1;
     //  単体カーソル設定か(項目がリンクしているのであれば配列の最初は-1にはならない)
    {
      if( m_initParam.aLinkItemPos[ 0 ] == -1 )
      {
        linkItemChkIdx  = -1;
      }
    }

    //  連結カーソル対象か
    {
      for( s32 i = 0; i < GFL_NELEMS( m_initParam.aLinkItemPos ); ++i )
      {
        //  リンク対象項目
        if( m_initParam.aLinkItemPos[ i ] == itemPos )
        {
          linkItemChkIdx  = i;
          break;
        }
      }
    }

    //  単体カーソル表示するか
    if( linkItemChkIdx == -1 )
    {
      m_pSelectItemObjectArray[ itemPos ]->SetVisibleCursor( true, ::app::tool::MenuCursor::CURSOR_ALL );
      return  ( bOldCursorActiveArray[ itemPos ] == false );
    }

    bool  bChangeMoveCursor = false;

    //  連結カーソル表示(連結している項目はすべてカーソル表示する)
    for( s32 i = 0; i < GFL_NELEMS( m_initParam.aLinkItemPos ); ++i )
    {
      //  配列の先頭から順に値が入っている。
      SelectItemPos nowUseItemPos = m_initParam.aLinkItemPos[ i ];

      //  値がない場合はこれ以上進んでもないので、ここで終了
      if( nowUseItemPos == SELECT_ITEM_POS_NONE )
      {
        break;
      }

      s32 visibleCursorType = ::app::tool::MenuCursor::CURSOR_ALL;

      //  連結項目が隣の場合はカーソルを一部削除
      for( s32 chk_i = 0; chk_i < GFL_NELEMS( m_initParam.aLinkItemPos ); ++chk_i )
      {
        SelectItemPos linkItemPos = m_initParam.aLinkItemPos[ chk_i ];
        if( linkItemPos == nowUseItemPos )
        {
          //  同じ項目はみない
          continue;;
        }

        //  ４つの項目で左右のどちらかのチャックしかないので条件がかかったらすぐにループから抜けることができる
        if( nowUseItemPos == SELECT_ITEM_POS_LEFT_DOWN )
        {
          if( linkItemPos == SELECT_ITEM_POS_RIGHT_DOWN )
          {
            visibleCursorType = ::app::tool::MenuCursor::CURSOR_LEFT;
            break;
          }
        }
        else
        if( nowUseItemPos == SELECT_ITEM_POS_LEFT_UP )
        {
          if( linkItemPos == SELECT_ITEM_POS_RIGHT_UP )
          {
            visibleCursorType = ::app::tool::MenuCursor::CURSOR_LEFT;
            break;
          }
        }
        else
        if( nowUseItemPos == SELECT_ITEM_POS_RIGHT_DOWN )
        {
          if( linkItemPos == SELECT_ITEM_POS_LEFT_DOWN )
          {
            visibleCursorType = ::app::tool::MenuCursor::CURSOR_LEFT;
            break;
          }
        }
        else
        if( nowUseItemPos == SELECT_ITEM_POS_RIGHT_UP )
        {
          if( linkItemPos == SELECT_ITEM_POS_LEFT_UP )
          {
            visibleCursorType = ::app::tool::MenuCursor::CURSOR_LEFT;
            break;
          }
        }
      }

      m_pSelectItemObjectArray[ nowUseItemPos ]->SetVisibleCursor( true, visibleCursorType );

      //  カーソルが変更前と同じかどうか
      if( bOldCursorActiveArray[ nowUseItemPos ] == false )
      {
        //  変更前と違う
        bChangeMoveCursor = true;
      }
    }

    return  bChangeMoveCursor;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタンidから選択項目の座標タイプを変換
    */
  //------------------------------------------------------------------
  BattleUITargetSelectLowerDraw::SelectItemPos BattleUITargetSelectLowerDraw::ConvButtonIdToSelectItemPos( const ButtonId buttonId )
  {
    const s32 aConvMap[ SELECT_ITEM_POS_MAX ][ 2 ]  =
    {
      { BUTTON_ID_LEFT_DOWN,  SELECT_ITEM_POS_LEFT_DOWN   },
      { BUTTON_ID_LEFT_UP,    SELECT_ITEM_POS_LEFT_UP     },
      { BUTTON_ID_RIGHT_DOWN, SELECT_ITEM_POS_RIGHT_DOWN  },
      { BUTTON_ID_RIGHT_UP,   SELECT_ITEM_POS_RIGHT_UP    },
    };

    for( s32 i = 0; i < SELECT_ITEM_POS_MAX; ++i )
    {
      if( aConvMap[ i ][ 0 ] == buttonId )
      {
        return  (SelectItemPos)aConvMap[ i ][ 1 ];
      }
    }

    return  SELECT_ITEM_POS_NONE;
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
  ::app::ui::UIInputListener::ListenerResult BattleUITargetSelectLowerDraw::OnLayoutPaneEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneEvent : painId = %d\n", buttonId );

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
  void BattleUITargetSelectLowerDraw::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneTouchTriggerEvent : buttonId = %d\n", buttonId );

    //  カーソル移動
    const SelectItemPos itemPos = ConvButtonIdToSelectItemPos( (ButtonId)buttonId );

    if( itemPos != SELECT_ITEM_POS_NONE )
    {
      _SetCursor( itemPos );
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
  ::app::ui::UIInputListener::ListenerResult BattleUITargetSelectLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

    if( m_initParam.pDelegate->OnKeyAction( this, pButton, pKey, pStick ) )
    {
      return  DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

  // ペイン選択アニメーションの開始を検知
  void BattleUITargetSelectLowerDraw::OnLayoutPaneStartingAnimeEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneStartingAnimeEvent : buttonId = %d\n", buttonId );
  }

  /** ここから選択項目のオブジェクトクラスの宣言 */

  BattleUITargetSelectLowerDraw::SelectItemObject::SelectItemObject()
  {
    _Clear();
  }

  BattleUITargetSelectLowerDraw::SelectItemObject::~SelectItemObject()
  {
    GFL_SAFE_DELETE( m_pKoukaIconParts );
    GFL_SAFE_DELETE( m_pMenuCursor );
    GFL_SAFE_DELETE( m_pPokeIcon );
  }

  /** 初期化 */
  void  BattleUITargetSelectLowerDraw::SelectItemObject::Initialize( 
    const INIT_SELECT_ITEM_PARAM& rInitParam, 
    UIView* pParentView,
    const s32 lytId,
    const s32 resId,
    ::app::util::Heap* pUseHeapUtil,
    void* pMenuCursorResBuf )
  {
    GFL_ASSERT( pUseHeapUtil );
    GFL_ASSERT( pParentView );
    GFL_ASSERT( pMenuCursorResBuf );

    m_pParentView = pParentView;
    m_lytId       = lytId;
    m_initParam   = rInitParam;

    ::app::util::G2DUtil* pG2DUtil    = pParentView->GetG2DUtil();
    gfl2::lyt::LytWk*     pLayoutWork = pParentView->GetLayoutWork( lytId );

    const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );
    //  項目のパーツペイン
    {
      m_pPartsPane  = pLayoutWork->GetPartsPane( rInitParam.root_parts_pane_id );
      GFL_ASSERT( m_pPartsPane );
    }

    //  プレイヤー名のテキストペイン取得して文字列を設定
    {
      gfl2::lyt::LytTextBox* pPlayerNameTextPane = pLayoutWork->GetTextBoxPane( rInitParam.player_text_pane_id );
      GFL_ASSERT( pPlayerNameTextPane );

      gfl2::str::StrBuf*  pTmpStrBuf  = pG2DUtil->GetTempStrBuf( 0 );

      pTmpStrBuf->SetStr( m_initParam.select_item_data.aPlayerName );
      if( m_initParam.select_item_data.bNPCName == true )
      {
        pG2DUtil->GetWordSet()->RegisterWord( 0, *pTmpStrBuf, m_initParam.select_item_data.playerNameAtribute );
        pG2DUtil->SetTextBoxPaneStringExpand( pPlayerNameTextPane, STR_TRAINER_NAME );
      }
      else
      {
        pG2DUtil->SetRegisterWord( 0, *pTmpStrBuf );
        pG2DUtil->SetTextBoxPaneStringExpand( pPlayerNameTextPane, STR_TRAINER_NAME );
      }
    }

    //  コーナー色をアニメ指定
    {
      u32 colorAnimeId  = m_initParam.grobe_anime_data.globe_blue_anime_idx;
      switch( m_initParam.select_item_data.cornerColor )
      {
        case CORRER_COLOR_BLUE:   colorAnimeId  = m_initParam.grobe_anime_data.globe_blue_anime_idx; break;
        case CORRER_COLOR_YELLOW: colorAnimeId  = m_initParam.grobe_anime_data.globe_yeloow_anime_idx; break;
        case CORRER_COLOR_GREEN:  colorAnimeId  = m_initParam.grobe_anime_data.globe_green_anime_idx; break;
        case CORRER_COLOR_RED:    colorAnimeId  = m_initParam.grobe_anime_data.globe_red_anime_idx; break;
      }

      pG2DUtil->StartAnime( lytId, colorAnimeId );
    }

    //  ポケモンアイコン
    gfl2::lyt::LytParts* pIconParts = pLayoutWork->GetPartsPane( m_pPartsPane, PANENAME_BATTLE_PLT_LOW_001_PANE_PARTS_POKEICON, &rMultiResId );
    GFL_ASSERT( pIconParts );

    //  インフォペイン
    gfl2::lyt::LytPane* pInfoPane = pLayoutWork->GetPane( m_pPartsPane, PANENAME_BATTLE_PLT_LOW_001_PANE_NULL_NAMECENTER, &rMultiResId );
    GFL_ASSERT( pInfoPane );

    //  カーソル位置のペイン
    m_pCursorNullPane = pLayoutWork->GetPane( m_pPartsPane, PANENAME_BATTLE_PLT_LOW_001_PANE_NULL_CUR, &rMultiResId );
    GFL_ASSERT( m_pCursorNullPane );

    //  項目色設定
    {
      u32 animeIdx  = m_initParam.friend_color_anime_idx;
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF_USE_WAZA )
      {

      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF )
      {

      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_ENEMY )
      {
        animeIdx  = m_initParam.enemy_color_anime_idx;
      }

      pG2DUtil->StartAnime( lytId, animeIdx );
    }

    //  カーソル作成
    {
      m_pMenuCursor = GFL_NEW( pUseHeapUtil->GetDeviceHeap() ) ::app::tool::MenuCursor( pUseHeapUtil, &pMenuCursorResBuf );
      GFL_ASSERT( m_pMenuCursor );

      m_pMenuCursor->CreateLayoutWork( m_pParentView->GetLayoutSystem(), m_pParentView->GetLytSetup( gfl2::lyt::DISPLAY_LOWER ), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_SMALL );
      m_pMenuCursor->Put( pLayoutWork, m_pCursorNullPane );

      SetVisibleCursor( false, ::app::tool::MenuCursor::CURSOR_ALL );
    }

    //  効果アイコンパーツインスタンス作成
    {
      parts::BattleUIKoukaIconPartsLayout::INIT_PARAM partsInitParam  =
      {
      /*
        //  パーツの細かい設定を行うのに必要
        ::app::util::G2DUtil*    p_g2d_util;

        //  パーツが配置されているLayoutId
        s32                     layout_id;

        //  パーツペイン
        gfl2::lyt::LytParts*    p_parts_pane;

        //  有効化アニメidx(アニメ名「active」)
        s32 active_anime_idx;

        //  無効化化アニメidx(アニメ名「passive」)
        s32 passive_anime_idx;

        //  効果なしアニメidx(アニメ名「kouka_02」)
        s32 kouka_none_anime_idx;

        //  効果いまひとつアニメidx(アニメ名「kouka_01」)
        s32 kouka_imahitotsu_anime_idx;

        //  効果ばつぐんアニメidx(アニメ名「kouka_00」)
        s32 kouka_batsugun_anime_idx;

        //  効果無記名アニメ名(アニメ名「kouka_03」)
        s32 kouka_normal_anime_idx;
      */
        m_pParentView->GetG2DUtil(),
        m_lytId,
        pLayoutWork->GetPartsPane( m_pPartsPane, PANENAME_BATTLE_PLT_LOW_001_PANE_PARTS_KOUKA, &rMultiResId ),
        m_initParam.kouka_icon_anime_data.active_anime_idx,
        m_initParam.kouka_icon_anime_data.passive_anime_idx,
        m_initParam.kouka_icon_anime_data.kouka_none_anime_idx,
        m_initParam.kouka_icon_anime_data.kouka_imahitotsu_anime_idx,
        m_initParam.kouka_icon_anime_data.kouka_batsugun_anime_idx,
        m_initParam.kouka_icon_anime_data.kouka_normal_anime_idx
      };

      m_pKoukaIconParts = GFL_NEW( pUseHeapUtil->GetDeviceHeap() ) parts::BattleUIKoukaIconPartsLayout( partsInitParam );
    }

    //  ポケモンデータがない場合は非選択対象に
    if( m_initParam.select_item_data.pBtlPokeParam == NULL )
    {
      //  項目の中身を非表示に
      pG2DUtil->SetPaneVisible( pIconParts, false );
      pG2DUtil->SetPaneVisible( pInfoPane, false );
      m_pKoukaIconParts->SetVisible( false );

      return;
    }

    pG2DUtil->SetPaneVisible( pIconParts, true );
    pG2DUtil->SetPaneVisible( pInfoPane, true );

    const pml::pokepara::PokemonParam*  pOrgPokemonParam  = m_initParam.select_item_data.pBtlPokeParam->GetSrcData();

    //  表示パラメータを取得
    const pml::pokepara::PokemonParam*  pViewPokemonParam = NULL;
    {
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF_USE_WAZA )
      {
        pViewPokemonParam = pOrgPokemonParam;
      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF )
      {
        pViewPokemonParam = pOrgPokemonParam;
      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_ENEMY )
      {
        pViewPokemonParam = m_initParam.select_item_data.pBtlPokeParam->GetViewSrcData();
      }
    }

    //  名前ペイン
    gfl2::lyt::LytTextBox* pNamePaneTextBox = pLayoutWork->GetTextBoxPane( m_pPartsPane, PANENAME_BATTLE_PLT_LOW_001_PANE_TEXTBOX_00, &rMultiResId );
    GFL_ASSERT( pNamePaneTextBox );

    //  名前表示
    {
      pG2DUtil->SetRegisterPokeNickName( 0, pViewPokemonParam );
      pG2DUtil->SetTextBoxPaneStringExpand( pNamePaneTextBox, STR_POKE_NAME );
    }

    //  性別表示
    {
      pml::Sex  sex = PokeTool::GetDrawSex( pViewPokemonParam );
      // オス
      if( sex == pml::SEX_MALE )
      {
        pG2DUtil->StartAnime( lytId, m_initParam.male_sex_anime_idx );
      }
      // メス
      else
      if( sex == pml::SEX_FEMALE )
      {
        pG2DUtil->StartAnime( lytId, m_initParam.female_sex_anime_idx );
      }
      else
      {
        //  非表示
        gfl2::lyt::LytParts*  pSexPartsPane = pLayoutWork->GetPartsPane( m_pPartsPane, PANENAME_BATTLE_PLT_LOW_001_PANE_PARTS_GENDER, &rMultiResId );
        GFL_ASSERT( pSexPartsPane );
        pG2DUtil->SetPaneVisible( pSexPartsPane, false );
      }
    }

    {
      ::app::tool::PokeIconToolEx::PokeIconToolParam  initParam;
      initParam.pUIView   = pParentView,
      initParam.pHeap     = pUseHeapUtil,
      initParam.layout_id = lytId,
      initParam.res_id    = resId,
      initParam.iconMax   = 1;
      initParam.sync      = false;

      m_pPokeIcon = GFL_NEW( pUseHeapUtil->GetDeviceHeap() ) ::app::tool::PokeIconToolEx( initParam );
      GFL_ASSERT( m_pPokeIcon );

      m_pPokeIcon->SetPokeIcon( 0, pViewPokemonParam, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, pIconParts );
    }

    //  効果種類更新
    _UpdateKoukaItem();
  }

  //  終了通知
  void BattleUITargetSelectLowerDraw::SelectItemObject::Terminate()
  {
  }

  //  使用準備OKか
  bool BattleUITargetSelectLowerDraw::SelectItemObject::IsReady()
  {
    if( m_initParam.select_item_data.pBtlPokeParam == NULL )
    {
      return  true;
    }

    //  ポケモンアイコンのテクスチャー貼り付けができているか
    return  m_pPokeIcon->IsReplaced( 0 );
  }

  /** 削除可能か */
  bool BattleUITargetSelectLowerDraw::SelectItemObject::IsDelete()
  {
    bool  bRes  = true;
    if( m_pPokeIcon != NULL )
    {
      if( m_pPokeIcon->EndFunc() == false )
      {
        bRes  = false;
      }
    }

    if( m_pMenuCursor != NULL )
    {
       //  リソースは共有しているのでリソースの破棄はしない
      if( m_pMenuCursor->DeleteLayoutWork() == false )
      {
        bRes  = false;
      }
      else
      {
        //  ここでカーソル破棄しないとUpdate()関数で参照しているカーソル関数でハング
        GFL_SAFE_DELETE( m_pMenuCursor );
      }
    }

    return  bRes;
  }

  //  ボタンExデータを作成
  const ::app::ui::ButtonInfoEx BattleUITargetSelectLowerDraw::SelectItemObject::CreateButtonInfoEx( const ButtonId buttonId )
  {
    GFL_ASSERT( m_pParentView );

    ::app::util::G2DUtil* pG2DUtil    = m_pParentView->GetG2DUtil();
    gfl2::lyt::LytWk*     pLayoutWork = m_pParentView->GetLayoutWork( m_lytId );

    const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    ::app::ui::ButtonInfoEx buttonInfoExData;

    buttonInfoExData.button_id      = buttonId;
    buttonInfoExData.picture_pane   = m_pPartsPane;
    GFL_ASSERT( buttonInfoExData.picture_pane );

    buttonInfoExData.bound_pane     = pLayoutWork->GetBoundingPane( m_pPartsPane, PANENAME_BATTLE_PLT_LOW_001_PANE_BOUND_00, &rMultiResId );
    GFL_ASSERT( buttonInfoExData.bound_pane );

    //!< タッチ時のアニメ
    buttonInfoExData.touch_anime_index      = m_initParam.btn_anime_data.touch_anime_index;

    //!< タッチ決定時のアニメ
    buttonInfoExData.release_anime_index    = m_initParam.btn_anime_data.release_anime_index;

    //!< タッチ状態からのキャンセルアニメ
    buttonInfoExData.cancel_anime_index     = m_initParam.btn_anime_data.cancel_anime_index;

    //!< キー決定時のアニメ ( touch => release の連続アニメ )
    buttonInfoExData.key_select_anime_index = m_initParam.btn_anime_data.key_select_anime_index;

    //!< アクティブアニメ
    buttonInfoExData.active_anime_index     = m_initParam.btn_anime_data.active_anime_index;

    //!< パッシブアニメ
    buttonInfoExData.passive_anime_index    = m_initParam.btn_anime_data.passive_anime_index;

    m_buttonInfoExData  = buttonInfoExData;

    return  m_buttonInfoExData;
  }

  //  更新
  void  BattleUITargetSelectLowerDraw::SelectItemObject::Update()
  {
    if( m_pPokeIcon != NULL )
    {
      m_pPokeIcon->Update();
    }
    else
    {
      //  m_pPokeIcon == NULL の場合選択対象のポケモンが死んでいるので効果表示は非表示
      m_pKoukaIconParts->SetVisible( false );
    }

    if( m_pMenuCursor != NULL )
    {
      m_pMenuCursor->Update();

      //  カーソルが表示している時は点滅アニメ設定
      u32 selectAnimeIdx  = 0;
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF_USE_WAZA )
      {
        selectAnimeIdx = m_initParam.friend_select_anime_idx;
      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF )
      {
        selectAnimeIdx = m_initParam.friend_select_anime_idx;
      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_ENEMY )
      {
        selectAnimeIdx = m_initParam.enemy_select_anime_idx;
      }

      ::app::util::G2DUtil* pG2DUtil    = m_pParentView->GetG2DUtil();
      if( IsVisibleCursor() == true )
      {
        //  カーソル選択している時は専用アニメを再生
        if( m_bSelectAnime == false )
        {
          m_bSelectAnime  = true;
          pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, selectAnimeIdx );
        }
      }
      else
      {
        //  カーソルが選択されていないので専用アニメは停止
        if( m_bSelectAnime == true )
        {
          m_bSelectAnime  = false;
          pG2DUtil->StopAnime( LYT_WK_TYPE_SCREEN, selectAnimeIdx, true );
        }
      }
    }

    //  アニメで切り替えている時に効果アイテムパーツの表示・非表示設定をしているので、ここで更新をかけないと設定が無効になる
    _UpdateKoukaItem();
  }

  //  有効設定
  void  BattleUITargetSelectLowerDraw::SelectItemObject::SetEnable( const bool bFlg )
  {
    m_pParentView->SetButtonEnable( m_buttonInfoExData.button_id, bFlg );

    ::app::util::G2DUtil* pG2DUtil    = m_pParentView->GetG2DUtil();

    s32 colorAnimeIdx = 0;

    //  選択/非選択を切り替え
    if( bFlg == true )
    {
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF_USE_WAZA )
      {
        colorAnimeIdx = m_initParam.friend_color_anime_idx;
      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF )
      {
        colorAnimeIdx = m_initParam.friend_color_anime_idx;
      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_ENEMY )
      {
        colorAnimeIdx = m_initParam.enemy_color_anime_idx;
      }
    }
    else
    {
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF_USE_WAZA )
      {
        colorAnimeIdx = m_initParam.friend_non_select_color_anime_idx;
      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_SELF )
      {
        colorAnimeIdx = m_initParam.friend_non_select_color_anime_idx;
      }
      else
      if( m_initParam.select_item_data.status == SELECT_ITEM_STATUS_ENEMY )
      {
        colorAnimeIdx = m_initParam.enemy_non_select_color_anime_idx;
      }
    }

    pG2DUtil->StartAnime( m_lytId, colorAnimeIdx );
  }

  void  BattleUITargetSelectLowerDraw::SelectItemObject::Draw( ::app::util::AppRenderingManager* pRenderManager, gfl2::gfx::CtrDisplayNo displayNo, u8 order )
  {
    if( m_pMenuCursor )
    {
      if( m_pParentView->IsInputEnabled() == true )
      {
        m_pMenuCursor->Draw( pRenderManager, displayNo, gfl2::lyt::DISPLAY_LOWER, order );
      }
    }
  }

  //  カーソル有効
  void BattleUITargetSelectLowerDraw::SelectItemObject::SetVisibleCursor( const bool bFlg, const s32 visibieType )
  {
    if( m_pMenuCursor != NULL )
    {
      m_pMenuCursor->SetVisible( bFlg, visibieType );

      if( bFlg == true )
      {
        gfl2::lyt::LytWk*     pLayoutWork = m_pParentView->GetLayoutWork( m_lytId );
        m_pMenuCursor->Put( pLayoutWork, m_pCursorNullPane );
        m_pMenuCursor->StartAnime();
      }
    }
  }

  //  カーソル表示しているか
  bool BattleUITargetSelectLowerDraw::SelectItemObject::IsVisibleCursor()
  {
    if( m_pMenuCursor != NULL )
    {
      if( m_pMenuCursor->IsVisible( ::app::tool::MenuCursor::CURSOR_LEFT ) ||
          m_pMenuCursor->IsVisible( ::app::tool::MenuCursor::CURSOR_RIGHT ) )
      {
        return  true;
      }
    }

    return  false;
  }

  //  ボタンを押す
  void BattleUITargetSelectLowerDraw::SelectItemObject::PushBtn()
  {
    m_pParentView->GetButtonManager()->StartSelectedAct( m_buttonInfoExData.button_id );
  }

  //  効果種類の設定
  void  BattleUITargetSelectLowerDraw::SelectItemObject::_UpdateKoukaItem()
  {
    m_pKoukaIconParts->SetType( m_initParam.select_item_data.koukaType );
    m_pKoukaIconParts->SetVisible( m_initParam.select_item_data.bKoukaTypePutFlg );
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
