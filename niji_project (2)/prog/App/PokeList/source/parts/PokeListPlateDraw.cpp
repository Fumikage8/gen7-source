//==============================================================================
/**
 * @file    PokeListPlateDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.06
 * @brief   ポケモンリスト画面プレート描画クラス
 */
//==============================================================================


// niji
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListPlateDraw.h"
#include "App/PokeList/source/parts/PokeListPokeInfoDraw.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Tool/app_tool_Gauge.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/Util/app_util_Controller.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList_anim_list.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//------------------------------------------------------------------------------
// @brief   変数
//------------------------------------------------------------------------------
//! @brief  プレートペインID
const u32 c_PlatePaneIndex[PLATE_NUM] = {
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_00,  //!< 左上
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_03,  //!< 右上
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_01,  //!< 左中
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_04,  //!< 右中
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_02,  //!< 左下
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_05,  //!< 右下
};


const u32 c_DisableExchange[] = {
  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_DIE,
  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_DIE,
  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_DIE,
  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_DIE,
  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_DIE,
  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_DIE,
};

//! @brief    プレートの状態アニメ
const u32 c_PlateStateAnim[PLATE_NUM][PlateDraw::PLATE_STATE_NUM] = {
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_PARTNER,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_DIE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_PASSIVE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_OWN_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_PARTNER_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_DIE_SELECTED,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_PARTNER,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_DIE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_PASSIVE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_OWN_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_PARTNER_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_DIE_SELECTED,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_PARTNER,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_DIE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_PASSIVE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_OWN_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_PARTNER_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_DIE_SELECTED,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_PARTNER,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_DIE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_PASSIVE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_OWN_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_PARTNER_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_DIE_SELECTED,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_PARTNER,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_DIE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_PASSIVE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_OWN_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_PARTNER_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_DIE_SELECTED,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_PARTNER,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_DIE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_PASSIVE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_OWN_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_PARTNER_SELECTED,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_DIE_SELECTED,
  }
};

//! @brief    メッセージウィンドウのタイプアニメ
const u32 c_PlateMsgWndTypeAnim[PLATE_NUM][PlateMessageWindow::PLATE_MSG_WND_TYPE_NUM] = {
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_DESC_ITEM,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_DESC_SANKA,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_DESC_ITEM,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_DESC_SANKA,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_DESC_ITEM,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_DESC_SANKA,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_DESC_ITEM,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_DESC_SANKA,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_DESC_ITEM,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_DESC_SANKA,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_DESC_ITEM,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_DESC_SANKA,
  },
};

//! @brief    メッセージウィンドウイン・アウトアニメ
const u32 c_PlateMsgWndInOutAnim[PLATE_NUM][2] = {
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_DESC_IN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_DESC_OUT,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_DESC_IN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_DESC_OUT,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_DESC_IN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_DESC_OUT,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_DESC_IN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_DESC_OUT,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_DESC_IN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_DESC_OUT,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_DESC_IN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_DESC_OUT,
  },
};



//==============================================================================
// @brief   プレートの位置管理クラス
//==============================================================================

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  PlatePosition::PlatePosition( DrawerBase* pBase, const CursorPos cpos, gfl2::lyt::LytParts* pParts )
    : m_pBase( pBase )
    , m_cCursorPos( cpos )
    , m_InitPos( 0, 0, 0 )
    , m_PlateSubVec( 0, 0, 0 )
    , m_pPlateParts( pParts )
  {
    // 初期位置の座標を取得
    m_InitPos = GetNowPos();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デフォルト位置へのバインド
   * @note    デフォルト位置へ戻す処理
   */
  //----------------------------------------------------------------------------
  PlatePosition::UpdateResult PlatePosition::UpdatePlateBind( void )
  {
    UpdateResult result = UPDATE_RESULT_CONTINUE;

    gfl2::math::Vector3 init_pos = GetInitPos();
    gfl2::math::Vector3 now_pos = GetNowPos();

    //if( !m_isCatch ) return;  //!< キャッチ状態でなければ何もしない

    // 元の位置までのベクトル
    gfl2::math::Vector3 move_vec = init_pos - now_pos;
    // 次の座標の計算
    gfl2::math::Vector3 next_pos = now_pos + (move_vec * 0.3f);

    // 補正
    u32 flg = 0;
    f32 endVec = 0.5f;
    if( gfl2::math::FAbs(move_vec.x) < endVec )
    {
      next_pos.x = init_pos.x;
      flg |= 1;
    }
    if( gfl2::math::FAbs(move_vec.y) < endVec )
    {
      next_pos.y = init_pos.y;
      flg |= 2;
    }
    next_pos.z = 0.0f;

    // 初期位置まで戻った
    if( flg == 3 )
    {
      app::util::G2DUtil* pG2D = m_pBase->GetG2D();
      u32 lytID = m_pBase->GetLytID();

      gfl2::lyt::LytPaneIndex parent_index = PANENAME_POKELIST_MIN_LOW_000_PANE_ALL;
      gfl2::lyt::LytPaneIndex child_index = c_PlatePaneIndex[m_cCursorPos];
      gfl2::lyt::LytPaneIndex offset_index = PANENAME_POKELIST_MIN_LOW_000_PANE_NULL_00;
      u32 mode = app::util::G2DUtil::APPEND_CHILD_MODE_INSERT;

      pG2D->AppendChild( lytID, parent_index, child_index, mode, offset_index );
      //m_isCatch = false;
      result = UPDATE_RESULT_FINISH;
    }

    // 座標を更新
    SetNowPos( next_pos );

    return result;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ドラッグによるプレートの座標変更
   */
  //----------------------------------------------------------------------------
  void PlatePosition::UpdatePlateDrag( gfl2::math::Vector3 pos )
  {
    gfl2::math::Vector3 now_pos = GetNowPos();
    gfl2::math::Vector3 touch = gfl2::math::Vector3( pos.x - 160, 120 - pos.y, 0 );

    now_pos = touch + m_PlateSubVec;

    // 座標を更新
    SetNowPos( now_pos );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   初期位置の取得
   */
  //----------------------------------------------------------------------------
  gfl2::math::Vector3 PlatePosition::GetInitPos( void )
  {
    return m_InitPos;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   現在位置の更新
   */
  //----------------------------------------------------------------------------
  void PlatePosition::SetNowPos( gfl2::math::Vector3 pos )
  {
    m_pBase->SetPanePos( m_pPlateParts, pos );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   現在位置の取得
   */
  //----------------------------------------------------------------------------
  gfl2::math::Vector3 PlatePosition::GetNowPos( void )
  {
    return m_pBase->GetPanePos( m_pPlateParts );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   タッチ点とプレートの中心点との差分の設定
   * @param   vec   タッチ点
   */
  //----------------------------------------------------------------------------
  void PlatePosition::SetPlateSubVec( gfl2::math::Vector3 vec )
  {
    gfl2::math::Vector3 touch = gfl2::math::Vector3( vec.x - 160, 120 - vec.y, 0 );
    gfl2::math::Vector3 init_pos = GetInitPos();

    m_PlateSubVec = init_pos - touch;
  }



//==============================================================================
// @brief   プレート専用メッセージウィンドウクラス
//==============================================================================

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  PlateMessageWindow::PlateMessageWindow( DrawerBase* pBase, const CursorPos cpos )
    : m_pBase( pBase )
    , m_cCursorPos( cpos )
    , m_PlateMode( PLATE_MODE_NUM )
    , m_State( STATE_NULL )
    , m_isOpen( false )
    , m_pItemTextBox( NULL )
    , m_pSankaTextBox( NULL )
    , m_pHPGauge( NULL )
  {
    gfl2::lyt::LytParts* pParts = GetPlateParts( m_pBase, m_cCursorPos );

    // テキストボックス
    m_pItemTextBox = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_000_PANE_TEXTBOX_03, pParts );
    m_pSankaTextBox = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_000_PANE_TEXTBOX_04, pParts  );
    m_pHPGauge = m_pBase->GetPane( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_HP, pParts  );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  PlateMessageWindow::~PlateMessageWindow( void )
  {
    ;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //----------------------------------------------------------------------------
  void PlateMessageWindow::Update( void )
  {
    switch( m_State )
    {
    case STATE_OPEN:
      {
        if( IsEndOpened() )
        {
          m_State = STATE_MAIN;
        }
      } break;

    case STATE_CLOSE:
      {
        if( IsEndClosed() )
        {
          m_State = STATE_NULL;
          //m_isOpen = false;
        }
      } break;
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウのセットアップ
   */
  //----------------------------------------------------------------------------
  void PlateMessageWindow::SetupMessageWindow( PlateMode plate_mode )
  {
    m_PlateMode = plate_mode;

    const u32* pTypeAnim = c_PlateMsgWndTypeAnim[m_cCursorPos];

    // アニメーションを停止
    for( u8 i=0; i<PLATE_MSG_WND_TYPE_NUM; ++i )
    {
      m_pBase->StopAnim( pTypeAnim[i] );
    }

    // 各モードごとの対応
    switch( m_PlateMode )
    {
    case PLATE_MODE_ITEM:
      {
        m_pBase->StartAnim( pTypeAnim[PLATE_MSG_WND_TYPE_ITEM] );

        // アイテムのセットアップ
        SetupItem();
      } break;

    case PLATE_MODE_JOIN:
      {
        m_pBase->StartAnim( pTypeAnim[PLATE_MSG_WND_TYPE_SANKA] );

        // 参加選択のセットアップ
        SetupJoin();
      } break;

    case PLATE_MODE_SIMPLE2:
      {
        m_pBase->StartAnim( pTypeAnim[PLATE_MSG_WND_TYPE_ITEM] );

        // 対象選択２セットアップ
        SetupSimple2();
      } break;

    case PLATE_MODE_WAZAOSHIE:
      {
        m_pBase->StartAnim( pTypeAnim[PLATE_MSG_WND_TYPE_ITEM] );

        // 技教えのセットアップ
        SetupWazaOshie();
      } break;
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウを開く
   */
  //----------------------------------------------------------------------------
  void PlateMessageWindow::OpenWindow( void )
  {
    if( m_isOpen ) return;

    const u32* pInOutAnim = c_PlateMsgWndInOutAnim[m_cCursorPos];

    // HPゲージ
    m_pBase->SetVisiblePane( m_pHPGauge, false );

    if( !m_pBase->IsAnimEnd(pInOutAnim[1]) )
    {
      m_pBase->StopAnim( pInOutAnim[1] );
    }

    // アニメーションの再生
    m_pBase->StartAnim( pInOutAnim[0] );

    m_State = STATE_OPEN;
    m_isOpen = true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   開くアニメーションが終了したか？
   * @return  true = 終了、false = 継続
   */
  //----------------------------------------------------------------------------
  bool PlateMessageWindow::IsEndOpened( void )
  {
    const u32* pInOutAnim = c_PlateMsgWndInOutAnim[m_cCursorPos];

    if( !m_isOpen ) return true;

    return m_pBase->IsAnimEnd( pInOutAnim[0] );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   開いているか？
   * @return  true = 開いている、false = 閉じている
   */
  //----------------------------------------------------------------------------
  bool PlateMessageWindow::IsOpened( void )
  {
    return m_isOpen;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウを閉じる
   */
  //----------------------------------------------------------------------------
  void PlateMessageWindow::CloseWindow( void )
  {
    const u32* pInOutAnim = c_PlateMsgWndInOutAnim[m_cCursorPos];

    if( !m_isOpen ) return;

    // HPゲージ
    m_pBase->SetVisiblePane( m_pHPGauge, true );

    if( !m_pBase->IsAnimEnd(pInOutAnim[0]) )
    {
      m_pBase->StopAnim( pInOutAnim[0] );
    }

    // アニメーションの再生
    m_pBase->StartAnim( pInOutAnim[1] );

    m_State = STATE_CLOSE;

    m_isOpen = false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   閉じるアニメーションが終了したか？
   * @return  true = 終了、false = 継続
   */
  //----------------------------------------------------------------------------
  bool PlateMessageWindow::IsEndClosed( void )
  {
    const u32* pInOutAnim = c_PlateMsgWndInOutAnim[m_cCursorPos];

    bool result = m_pBase->IsAnimEnd( pInOutAnim[1] );

    if( !m_isOpen ) return true;

    return result;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   アイテムのセットアップ
   */
  //----------------------------------------------------------------------------
  void PlateMessageWindow::SetupItem( void )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();
    {
      // 所持アイテムIDの取得
      u16 itemNo = staticWork->GetPokeItem( m_cCursorPos );

      if( itemNo != ITEM_DUMMY_DATA )
      {
        app::util::G2DUtil* g2d = m_pBase->GetG2D();
        {
          g2d->SetRegisterItemName( 0, itemNo );
          g2d->SetTextBoxPaneStringExpand( m_pItemTextBox, mes_pokelist_item_name );
        }

        // ウィンドウをオープン
        OpenWindow();
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   参加選択のセットアップ
   */
  //----------------------------------------------------------------------------
  void PlateMessageWindow::SetupJoin( void )
  {
    const u32 c_JoinMsgIDTbl[] = {
      mes_pokelist_06_01,
      mes_pokelist_06_02,
      mes_pokelist_06_03,
      mes_pokelist_06_04,
      mes_pokelist_06_05,
      mes_pokelist_06_06,
    };

    StaticWork* staticWork = m_pBase->GetStaticWork();

    // インデックス
    u8 poke_index = staticWork->GetPokeIndex( m_cCursorPos );
    // 参加番号の取得
    s8 order      = staticWork->GetJoinOrder( poke_index );
    // 参加可否の取得
    bool enable   = staticWork->IsJoinEnable( poke_index );

    // 参加可能で未参加の場合何も表示しない
    if( enable && (order == -1 || order == 0xFF ) ) return;

    u32 strID = mes_pokelist_05_10;   //!< 出られません

    if( enable )
    {
      if( order >= GFL_NELEMS(c_JoinMsgIDTbl) )
      {
        GFL_ASSERT(0);
        order = 0;
      }
      strID = c_JoinMsgIDTbl[order];
    }

    // メッセージの設定
    app::ui::UIView* view = m_pBase->GetUIView();
    {
      view->SetTextboxPaneMessage( m_pSankaTextBox, strID );
    }

    if( !IsOpened() )
    {
      // ウィンドウをオープン
      OpenWindow();
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   対象選択２のセットアップ
   */
  //----------------------------------------------------------------------------
  void PlateMessageWindow::SetupSimple2( void )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();

    bool isEgg = false;

    // タマゴかどうかを取得
    const pml::pokepara::PokemonParam* pp = staticWork->GetPokeConst( m_cCursorPos );
    {
      bool fastmode_flag = pp->StartFastMode();
      {
        isEgg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
      }
      pp->EndFastMode( fastmode_flag );
    }

    if( !isEgg )
    {
      // 選択可否状況を取得
      u8 poke_index = staticWork->GetPokeIndex( m_cCursorPos );
      u8 enable     = staticWork->GetSimple2SelectEnable( poke_index );

      if( enable == 1 )
      {
        return;
      }

      // メッセージの設定
      app::ui::UIView* view = m_pBase->GetUIView();
      {
        view->SetTextboxPaneMessage( m_pItemTextBox, mes_pokelist_08_01 );
      }

      // ウィンドウをオープン
      OpenWindow();
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   技教えのセットアップ
   */
  //----------------------------------------------------------------------------
  void PlateMessageWindow::SetupWazaOshie( void )
  {
    const u32 c_WazaStateMsgID[] = {
      mes_pokelist_06_11,
      mes_pokelist_06_10,
      mes_pokelist_06_12,
    };

    StaticWork* staticWork = m_pBase->GetStaticWork();

    bool isEgg = false;

    // タマゴかどうかを取得
    const pml::pokepara::PokemonParam* pp = staticWork->GetPokeConst( m_cCursorPos );
    {
      bool fastmode_flag = pp->StartFastMode();
      {
        isEgg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
      }
      pp->EndFastMode( fastmode_flag );
    }

    if( !isEgg )
    {
      // 技習得状況を取得
      u8 poke_index = staticWork->GetPokeIndex( m_cCursorPos );
      u8 waza_state = staticWork->GetWazaSetParam( poke_index );

      // メッセージの設定
      app::ui::UIView* view = m_pBase->GetUIView();
      {
        view->SetTextboxPaneMessage( m_pItemTextBox, c_WazaStateMsgID[waza_state] );
      }

      // ウィンドウをオープン
      OpenWindow();
    }
  }





//==============================================================================
// @brief   ポケモンアイコンアニメクラス
//==============================================================================
  //----------------------------------------------------------------------------
  // @brief   変数
  //----------------------------------------------------------------------------
  //! @brief  ポケモンアイコンアニメ(CursorPos順)
  const u32 c_PokeIconAnimTbl[PLATE_NUM][PokeIconAnim::ICON_ANIM_NUM] = {
    {
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_HIGH,      //!< 元気な時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_MIDDLE,    //!< 体力が半分以下
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_LOW,       //!< 体力が少ない
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_DIE,            //!< 瀕死時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_SICK,           //!< 異常状態時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_SELECT,    //!< 選択時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_STOP,           //!< 色戻し用
    },
    {
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_JAMP_HIGH,      //!< 元気な時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_JAMP_MIDDLE,    //!< 体力が半分以下
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_JAMP_LOW,       //!< 体力が少ない
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_DIE,            //!< 瀕死時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_SICK,           //!< 異常状態時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_JAMP_SELECT,    //!< 選択時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_STOP,           //!< 色戻し用
    },
    {
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_JAMP_HIGH,      //!< 元気な時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_JAMP_MIDDLE,    //!< 体力が半分以下
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_JAMP_LOW,       //!< 体力が少ない
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_DIE,            //!< 瀕死時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_SICK,           //!< 異常状態時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_JAMP_SELECT,    //!< 選択時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_STOP,           //!< 色戻し用
    },
    {
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_JAMP_HIGH,      //!< 元気な時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_JAMP_MIDDLE,    //!< 体力が半分以下
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_JAMP_LOW,       //!< 体力が少ない
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_DIE,            //!< 瀕死時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_SICK,           //!< 異常状態時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_JAMP_SELECT,    //!< 選択時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_STOP,           //!< 色戻し用
    },
    {
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_JAMP_HIGH,      //!< 元気な時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_JAMP_MIDDLE,    //!< 体力が半分以下
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_JAMP_LOW,       //!< 体力が少ない
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_DIE,            //!< 瀕死時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_SICK,           //!< 異常状態時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_JAMP_SELECT,    //!< 選択時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_STOP,           //!< 色戻し用
    },
    {
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_JAMP_HIGH,      //!< 元気な時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_JAMP_MIDDLE,    //!< 体力が半分以下
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_JAMP_LOW,       //!< 体力が少ない
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_DIE,            //!< 瀕死時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_SICK,           //!< 異常状態時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_JAMP_SELECT,    //!< 選択時
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_STOP,           //!< 色戻し用
    },
  };

  static const char* c_IconAnimName[] = {
    "ANIM_JAMP_HIGH",
    "ANIM_JAMP_MIDDLE",
    "ANIM_JAMP_LOW",
    "ANIM_DIE",
    "ANIM_SICK",
    "ANIM_JAMP_SELECT",
    "ANIM_STOP",
    "ANIM_NULL",
    "ANIM_NULL",
  };


  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  PokeIconAnim::PokeIconAnim( DrawerBase* pBase, const CursorPos cpos )
    : m_pBase( pBase )
    , m_cCursorPos( cpos )
    , m_PrevAnimID( ICON_ANIM_NULL )
    , m_isSelected( false )
  {
    ;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   ループアニメーションの開始
   */
  //----------------------------------------------------------------------------
  void PokeIconAnim::StartLoopAnim( const pml::pokepara::PokemonParam* pPoke )
  {
    FUKUSHIMA_PRINT( "StartLoopAnim\n" );

    if( !m_pBase->GetPokeIcon() ) return;

    bool isEgg = false;
    bool isSick = false;
    u16 hpNow = 0;
    u16 hpMax = 0;

    bool fastmode_flag = pPoke->StartFastMode();
    {
      isEgg = pPoke->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
      isSick = pPoke->HaveSick();

      hpNow = pPoke->GetHp();
      hpMax = pPoke->GetMaxHp();
    }
    pPoke->EndFastMode( fastmode_flag );

    m_pBase->StopAnim( c_PokeIconAnimTbl[m_cCursorPos][ICON_ANIM_JUMP_SELECT] );

    // タマゴ
    if( isEgg )
    {
      StartAnim( ICON_ANIM_STOP );
      return;
    }
    // 状態異常アニメ
    else if( isSick )
    {
      StartAnim( ICON_ANIM_SICK );
    }
    // 体力ゲージの色に応じたアニメを再生する
    else {
      switch( app::tool::Gauge::GetDottoColor( hpNow, hpMax ) )
      {
      case app::tool::Gauge::GAUGE_COLOR_GREEN:   //!< 緑
        {
          StartAnim( ICON_ANIM_JUMP_HIGH );
        } break;
      case app::tool::Gauge::GAUGE_COLOR_YELLOW:  //!< 黄
        {
          StartAnim( ICON_ANIM_JUMP_MIDDLE );
        } break;
      case app::tool::Gauge::GAUGE_COLOR_RED:     //!< 赤
        {
          StartAnim( ICON_ANIM_JUMP_LOW );
        } break;
      case app::tool::Gauge::GAUGE_COLOR_NULL:    //!< 瀕死
        {
          StartAnim( ICON_ANIM_DIE );
        } break;
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   プレート選択時のアニメーションの変更
   *
   * @param   cpos    カーソル位置
   */
  //----------------------------------------------------------------------------
  void PokeIconAnim::StartSelectAnim( CursorPos cpos )
  {
    FUKUSHIMA_PRINT( "StartSelectAnim\n" );

    // 未選択時
    if( !m_isSelected )
    {
      // 選択された
      if( m_cCursorPos == cpos )
      {
        StartSelectAnim( true );

        m_isSelected = true;
      }
    }
    // 選択時
    else {
      // 選択が外れた
      if( m_cCursorPos != cpos )
      {
        StartSelectAnim( false );

        m_isSelected = false;
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケアイコン選択アニメの再生・停止
   * @param   play    再生フラグ(true = 再生、false = 停止)
   */
  //----------------------------------------------------------------------------
  void PokeIconAnim::StartSelectAnim( bool play )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();

    bool isDie  = false;
    bool isSick = false;
    bool isEgg  = false;  // @fix GFNMCat[876]

    const pml::pokepara::PokemonParam* pPoke = staticWork->GetPokeConst( m_cCursorPos );
    bool fastmode_flag = pPoke->StartFastMode();
    {
      isDie  = (pPoke->GetHp() == 0);
      isSick = pPoke->HaveSick();
      isEgg  = pPoke->IsEgg(pml::pokepara::CHECK_BOTH_EGG);
    }
    pPoke->EndFastMode( fastmode_flag );

    const u32* pAnimTable = c_PokeIconAnimTbl[m_cCursorPos];
    const u32 anmID = pAnimTable[ICON_ANIM_JUMP_SELECT];

    if( play )
    {
      // 状態異常、瀕死状態でなければ選択アニメを開始する
      if( isEgg || (!isSick && !isDie) )
      {
        m_pBase->StartAnim( anmID );
      }
    }
    else {
      m_pBase->StopAnim( anmID, isEgg );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アニメーションの再生
   */
  //----------------------------------------------------------------------------
  void PokeIconAnim::StartAnim( IconAnim icon_anim_id )
  {
    if( icon_anim_id == m_PrevAnimID ) return;

    const u32* pAnimTable = c_PokeIconAnimTbl[m_cCursorPos];

    // アニメーションの停止
    if( m_PrevAnimID != ICON_ANIM_NULL )
    {
      m_pBase->StopAnim( pAnimTable[m_PrevAnimID] );
    }

    // アニメーションの再生
    if( icon_anim_id != ICON_ANIM_NULL )
    {
      m_pBase->StartAnim( pAnimTable[icon_anim_id] );
    }

    FUKUSHIMA_PRINT( "StopAnim[%s] -> PlayAnim[%s]\n",
      c_IconAnimName[m_PrevAnimID], c_IconAnimName[icon_anim_id] );

    m_PrevAnimID = icon_anim_id;
  }





//==============================================================================
// @brief   プレートクラス
//==============================================================================

  //! @brief  通常時のプレートステート
  const PlateDraw::PlateState c_DefaultPlate[PLATE_NUM] = {
    PlateDraw::PLATE_STATE_DEFAULT, PlateDraw::PLATE_STATE_DEFAULT,
    PlateDraw::PLATE_STATE_DEFAULT, PlateDraw::PLATE_STATE_DEFAULT,
    PlateDraw::PLATE_STATE_DEFAULT, PlateDraw::PLATE_STATE_DEFAULT,
  };

  //! @brief  マルチ戦闘のプレートステート(自分が左)
  const PlateDraw::PlateState c_MultiPlateLeft[PLATE_NUM] = {
    PlateDraw::PLATE_STATE_DEFAULT, PlateDraw::PLATE_STATE_PARTNER,
    PlateDraw::PLATE_STATE_DEFAULT, PlateDraw::PLATE_STATE_PARTNER,
    PlateDraw::PLATE_STATE_DEFAULT, PlateDraw::PLATE_STATE_PARTNER,
  };

  //! @brief  マルチ戦闘のプレートステート(自分が右)
  const PlateDraw::PlateState c_MultiPlateRight[PLATE_NUM] = {
    PlateDraw::PLATE_STATE_PARTNER, PlateDraw::PLATE_STATE_DEFAULT,
    PlateDraw::PLATE_STATE_PARTNER, PlateDraw::PLATE_STATE_DEFAULT,
    PlateDraw::PLATE_STATE_PARTNER, PlateDraw::PLATE_STATE_DEFAULT,
  };

  //! @brief  ポケモンのインデックス
  const u32 c_CursorPosIndex[PLATE_NUM] = {
    0, 1, 2, 0, 1, 2
  };


  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  PlateDraw::PlateDraw( DrawerBase* pBase, const CursorPos cpos )
    : m_pBase( pBase )
    , m_cCursorPos( cpos )
    , m_pPokeInfoDraw( NULL )
    , m_pPokeIconAnim( NULL )
    , m_pPlatePosition( NULL )
    , m_pPlateMsgWindow( NULL )
    , m_PlateState( PLATE_STATE_DEFAULT )
    , m_PrevPos( CPOS_NULL )
    , m_pPlateParts( NULL )
    , m_pPlateBound( NULL )
    , m_isCatch( false )
    , m_isDrag( false )
  {
    Init();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  PlateDraw::~PlateDraw( void )
  {
    GFL_SAFE_DELETE( m_pPlateMsgWindow );
    GFL_SAFE_DELETE( m_pPlatePosition );
    GFL_SAFE_DELETE( m_pPokeIconAnim );
    GFL_SAFE_DELETE( m_pPokeInfoDraw );
  }



  //----------------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //----------------------------------------------------------------------------
  void PlateDraw::Init( void )
  {
    gfl2::heap::HeapBase* pDevHeap = m_pBase->GetDeviceHeap();

    // ペインの設定
    {
      m_pPlateParts = GetPlateParts( m_pBase, m_cCursorPos );
      m_pPlateBound = m_pBase->GetPane( PANENAME_POKELIST_PLT_LOW_000_PANE_BOUND_00, m_pPlateParts );
    }

    // ポケモン情報表示
    m_pPokeInfoDraw = GFL_NEW(pDevHeap) PokeInfoDraw( m_pBase, m_cCursorPos );
    GFL_ASSERT( m_pPokeInfoDraw );

    // ポケモンアイコンアニメ
    m_pPokeIconAnim = GFL_NEW(pDevHeap) PokeIconAnim( m_pBase, m_cCursorPos );
    GFL_ASSERT( m_pPokeIconAnim );

    StaticWork* staticWork = m_pBase->GetStaticWork();
    Mode mode = staticWork->GetProcMode();

    if( mode == MODE_FIELD || mode == MODE_JOIN || mode == MODE_WAZAOSHIE || mode == MODE_SIMPLE2 )
    {
      // 
      m_pPlatePosition = GFL_NEW(pDevHeap) PlatePosition( m_pBase, m_cCursorPos, m_pPlateParts );
      GFL_ASSERT( m_pPlatePosition );

      // 
      m_pPlateMsgWindow = GFL_NEW(pDevHeap) PlateMessageWindow( m_pBase, m_cCursorPos );
      GFL_ASSERT( m_pPlateMsgWindow );
    }

    // 先頭のポケモンを現すプレートの表示
    if( mode == MODE_BATTLE )
    {
      const ModeContext context = staticWork->GetModeContext();
      {
        if( context.battle.btl_mode == BTL_MODE_MULTI_LEFT || context.battle.btl_mode == BTL_MODE_MULTI_RIGHT )
        {
          gfl2::lyt::LytPane* pane = m_pBase->GetPane( PANENAME_POKELIST_MIN_LOW_000_PANE_BG_TOPPOKE_R );

          m_pBase->SetVisiblePane( pane, true );
        }
      }
    }

    //if( m_cCursorPos == CPOS_PLATE_TL )
    //{
    //  gfl2::lyt::LytPane* plateFramePane =
    //    m_pBase->GetPane( PANENAME_POKELIST_PLT_LOW_000_PANE_COMMONPOKELIST_BUTTON_06, m_pPlateParts );
    //  {
    //    m_pBase->SetVisiblePane( plateFramePane, true );
    //  }
    //}
    //else if( m_cCursorPos == CPOS_PLATE_TR )
    //{
    //  gfl2::lyt::LytPane* plateFramePane =
    //    m_pBase->GetPane( PANENAME_POKELIST_PLT_LOW_000_PANE_COMMONPOKELIST_BUTTON_06, m_pPlateParts );
    //  {
    //    m_pBase->SetVisiblePane( plateFramePane, true );
    //  }
    //}
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //----------------------------------------------------------------------------
  void PlateDraw::Update( void )
  {
    if( m_pPokeInfoDraw ) m_pPokeInfoDraw->Update();
    if( m_pPlateMsgWindow ) m_pPlateMsgWindow->Update();

    // ポケモンアイコンアニメの更新
    UpdatePokeIconAnim();

    if( m_isCatch && !m_isDrag )
    {
      PlatePosition::UpdateResult result;
      result = m_pPlatePosition->UpdatePlateBind();

      if( result == PlatePosition::UPDATE_RESULT_FINISH )
      {
        m_isCatch = false;
      }
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンアニメの更新
   */
  //------------------------------------------------------------------
  void PlateDraw::UpdatePokeIconAnim( void )
  {
    if( m_pPokeIconAnim == NULL )
    {
      return;
    }

    CursorPos cpos = CPOS_NULL;
    {
      StaticWork* staticWork = m_pBase->GetStaticWork();
      {
        // カーソル位置の取得
        cpos = staticWork->GetCursorPos( staticWork->GetCursorID() );
      }
    }

    if( cpos == CPOS_NULL )
    {
      return;
    }

    if( m_PrevPos != cpos )
    {
      FUKUSHIMA_PRINT( "StartSelectAnim : pos[%d], prev[%d]\n", cpos, m_PrevPos );

      m_pPokeIconAnim->StartSelectAnim( cpos );

      m_PrevPos = cpos;
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   セットアップ
   * @param   plate_mode    プレート表示モード
   */
  //----------------------------------------------------------------------------
  void PlateDraw::Setup( PlateMode plate_mode )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();
    const pml::pokepara::PokemonParam* pPoke = staticWork->GetPokeConst( m_cCursorPos );
    const PlateDraw::PlateState* cpPlateState = c_DefaultPlate;

    Mode mode = staticWork->GetProcMode();

    // プレートのステートの変更
    if( mode == MODE_BATTLE )
    {
      ModeContext context = staticWork->GetModeContext();
      if( context.battle.btl_mode == BTL_MODE_MULTI_LEFT )
      {
        cpPlateState = c_MultiPlateLeft;
      }
      else if( context.battle.btl_mode == BTL_MODE_MULTI_RIGHT )
      {
        cpPlateState = c_MultiPlateRight;
      }
    }

    SetupPlatePokeDisplay( pPoke );
    SetModePlate( plate_mode );
    SetPlateState( cpPlateState[m_cCursorPos], plate_mode );
  }



  //----------------------------------------------------------------------------
  /**
   * @brief   プレートの状態を変更する
   * @param   state       セットする状態
   * @param   plate_mode  プレートの表示モード
   */
  //----------------------------------------------------------------------------
  void PlateDraw::SetPlateState( PlateState state, PlateMode plate_mode )
  {
    const u32* pAnimTable = c_PlateStateAnim[m_cCursorPos];

    for( u8 i=0; i<PLATE_STATE_NUM; ++i )
    {
      m_pBase->StopAnim( pAnimTable[i] );
    }

    const StaticWork* staticWork = m_pBase->GetStaticWork();
    const pml::pokepara::PokemonParam* pPoke = staticWork->GetPokeConst( m_cCursorPos );

    bool isEgg = false;   //!< タマゴフラグ
    bool isDie = false;   //!< 瀕死フラグ

    bool fastmode_flag = pPoke->StartFastMode();
    {
      isEgg = pPoke->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
      isDie = pPoke->GetHp() == 0;
    }
    pPoke->EndFastMode( fastmode_flag );

    // 瀕死時
    if( isDie && !isEgg )   // @fix GFNMCat[4232]
    {
      switch( state )
      {
      case PLATE_STATE_DEFAULT:
      case PLATE_STATE_PARTNER:
        {
          state = PLATE_STATE_DIE;
        } break;

      case PLATE_STATE_SELECTED:
      case PLATE_STATE_PARTNER_SELECTED:
        {
          state = PLATE_STATE_DIE_SELECTED;
        } break;
      }
    }

    // モードごとの対応
    switch(plate_mode)
    {
    case PLATE_MODE_SIMPLE2:
    case PLATE_MODE_WAZAOSHIE:
    case PLATE_MODE_ITEM:
      {
        if( isEgg ) state = PLATE_STATE_DISABLE;
      } break;
    }

    app::tool::ButtonManager* pBtnMan = m_pBase->GetButtonManager();

    if( state != PLATE_STATE_DISABLE )
    {
      m_pBase->StartAnim( pAnimTable[state] );
      pBtnMan->SetButtonActive( BTN_ID_PLATE_TL + m_cCursorPos );
    }
    else {
      pBtnMan->SetButtonPassive( BTN_ID_PLATE_TL + m_cCursorPos );
    }

    m_PlateState = state;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   プレートの状態を取得する
   * @return  PlateMode
   */
  //------------------------------------------------------------------------------
  PlateDraw::PlateState PlateDraw::GetPlateState( void )
  {
    return m_PlateState;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   表示を更新
   * @param   pPokeParam    プレートに表示するポケモン
   */
  //------------------------------------------------------------------------------
  void PlateDraw::SetupPlatePokeDisplay( const pml::pokepara::PokemonParam* pPokeParam )
  {
    if( pPokeParam == NULL || pPokeParam->IsNull() )
    {
      SetVisible( false );  //!< プレートを非表示
      //m_button_manager[ BTNGRP_PLATE ]->SetButtonPassive( plateID );  // ボタンを無効化
    }
    else {
      SetVisible( true );  //!< プレートを表示

      m_pPokeInfoDraw->SetupPokeDisplay( pPokeParam );

      m_pPokeIconAnim->StartLoopAnim( pPokeParam );
    }
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   モードごとの表示を設定
   * @param   plate_mode    プレートの表示タイプ
   */
  //------------------------------------------------------------------------------
  void PlateDraw::SetModePlate( PlateMode plate_mode )
  {
    m_pPokeInfoDraw->SetupPlateMode( plate_mode );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   ポケアイコンアニメの再生
   */
  //------------------------------------------------------------------------------
  void PlateDraw::StartPokeIconAnim( void )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();

    const pml::pokepara::PokemonParam* pPoke = staticWork->GetPokeConst( m_cCursorPos );

    m_pPokeIconAnim->StartLoopAnim( pPoke ); //!< ポケモンアイコンのアニメの再生
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   ポケアイコン選択アニメの再生・停止
   * @param   play    再生フラグ(true = 再生、false = 停止)
   */
  //------------------------------------------------------------------------------
  void PlateDraw::StartPokeIconSelectAnim( bool play )
  {
    if( m_pPokeIconAnim ) m_pPokeIconAnim->StartSelectAnim( play );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   プレートをキャッチ状態にする
   */
  //------------------------------------------------------------------------------
  void PlateDraw::SetPlateCatch( void )
  {
    m_isCatch = true;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   プレートをキャッチ状態かどうか
   * @return  true = キャッチ中
   */
  //------------------------------------------------------------------------------
  bool PlateDraw::IsPlateCatch( void )
  {
    return m_isCatch;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   プレートをドラッグ状態を設定する
   * @param   flag    ドラッグフラグ
   */
  //------------------------------------------------------------------------------
  void PlateDraw::SetPlateDrag( bool flag )
  {
    m_isDrag = flag;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   プレートをドラッグ状態かどうか
   * @return  true = ドラッグ中
   */
  //------------------------------------------------------------------------------
  bool PlateDraw::IsPlateDrag( void )
  {
    return m_isDrag;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   表示切替
   * @param   visible   表示フラグ(true=表示、false=非表示)
   */
  //------------------------------------------------------------------------------
  void PlateDraw::SetVisible( bool visible )
  {
    m_pBase->SetVisiblePane( m_pPlateParts, visible );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   プレートのタッチ判定
   * @param   pTouchPanel   gfl2::ui::TouchPanelクラスポインタ
   * @return  true = タッチ中、false = タッチされていない
   */
  //------------------------------------------------------------------------------
  bool PlateDraw::IsTouchPlate( gfl2::ui::TouchPanel* pTouchPanel )
  {
    return m_pBase->IsHoldPane( pTouchPanel, m_pPlateBound );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   HPの更新
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------------------
  void PlateDraw::UpdateHP( pml::pokepara::PokemonParam* pp )
  {
    m_pPokeInfoDraw->SetParamHp( pp );
    m_pPokeIconAnim->StartLoopAnim( pp );
  }


  //------------------------------------------------------------------------------
  /**
   * @brief   プレートの座標管理クラスの取得
   * @param   PlatePositionクラスのポインタ
   */
  //------------------------------------------------------------------------------
  PlatePosition* PlateDraw::GetPlatePosition( void )
  {
    return m_pPlatePosition;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   プレートのメッセージウィンドウクラスの取得
   * @param   PlateMessageWindowクラスのポインタ
   */
  //------------------------------------------------------------------------------
  PlateMessageWindow* PlateDraw::GetPlateMessageWindow( void )
  {
    return m_pPlateMsgWindow;
  }



GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
