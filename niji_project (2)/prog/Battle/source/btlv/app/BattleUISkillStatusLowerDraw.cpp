//=============================================================================
/**
 * @file BattleUISkillStatusLowerDraw.h
 * @brief バトル下画面 >> 「わざスタータス」画面
 * @author yuto_uchida
 * @date 2015.03.19
 */
//=============================================================================
#include "BattleUISkillStatusLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/dsclow.h"
#include "niji_conv_header/app/battle/res2d/dsclow_pane.h"
#include "niji_conv_header/app/battle/res2d/dsclow_anim_list.h"
#include "niji_conv_header/message/msg_btl_app.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUISkillStatusLowerDraw::BattleUISkillStatusLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );
    GFL_ASSERT( param.pWazaSetumeiString );
    GFL_ASSERT( param.pAppCommonGrpIconData );

    //  初期化
    {
      m_state     = STATE_TYPE_NONE;
      m_initParam = param;
    }

    _Initialize2D( m_initParam.pLytBuff );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );
    GFL_ASSERT( pLayoutWork );

    //  保管するペインを取得
    {
      m_pOpenEventStartPosPane = pLayoutWork->GetPane( PANENAME_BATTLE_DSC_LOW_000_PANE_NULL_SCALE );
      m_pOpenEventEndPosPane   = pLayoutWork->GetPane( PANENAME_BATTLE_DSC_LOW_000_PANE_NULL_PIVOT );
    }

    //  ループアニメ再生
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_DSCLOW_BATTLE_DSC_LOW_000_BG_KEEP );
    }

    //  各種項目名の設定
    {
      //  技名
      if( m_initParam.bZenryoku == false )
      {
        pG2DUtil->SetRegisterWazaName( 0, m_initParam.wazaId );
        pG2DUtil->SetTextBoxPaneStringExpand( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_TEXTBOX_00 ), STR_WAZA_NAME );

        //  通常わざのバターンアニメ再生
        {
          pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_DSCLOW_BATTLE_DSC_LOW_000_BG_BATTLE );
        }
      }
      else
      {
        //  通常技の項目
        {
          pG2DUtil->SetRegisterWazaName( 0, m_initParam.wazaId );
          pG2DUtil->SetTextBoxPaneStringExpand( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_TEXTBOX_00 ), STR_WAZA_NAME );
        }

        //  全力わざのバターンアニメ再生
        {
          pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_DSCLOW_BATTLE_DSC_LOW_000_BG_ZENRYOKU );
        }

        //  全力わざ名を設定
        {
          GFL_ASSERT( m_initParam.pZenryokuWazaName );
          pG2DUtil->SetRegisterWord( 0, *m_initParam.pZenryokuWazaName );
          pG2DUtil->SetTextBoxPaneStringExpand( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_TEXTBOX_05 ), STR_WAZA_FULLPOWER );
        }

        //  追加効果テキスト
        //  全力技の追加効果のメッセージid
        {
          // @fix 岩尾さん要望　全力技で追加効果があれば効果説明を表示
          s32 msgId = _GetZenryokuWazaEffectMsgId( WAZADATA_GetZenryokuWazaEffect( m_initParam.zenryokuWazaId ) );
          gfl2::lyt::LytTextBox*  pDesc01Pane = pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_DESC_01 );
          GFL_ASSERT( pDesc01Pane );
          //  -1ならメッセージがないか効果がない
          if( msgId == -1 )
          {
            pG2DUtil->SetTextBoxPaneString( pDesc01Pane, L"" );
          }
          else
          {
            UIView::SetTextboxPaneMessage( pDesc01Pane, msgId );
          }
        }
      }

      //  PP
      {
        //  項目
        UIView::SetTextboxPaneMessage( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_TEXTBOX_01 ), STR_PP );

        //  now
        UIView::SetTextboxPaneNumber( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_TEXTBOX_02 ), STR_VALUE_2, param.nowPPNum, 2 );

        //  max
        UIView::SetTextboxPaneNumber( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_TEXTBOX_03 ), STR_VALUE_2, param.maxPPNum, 2 );;
      }

      //  分類
      {
        //  項目
        UIView::SetTextboxPaneMessage( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_PALAM_00 ), STR_WAZA_KIND );

        const s32 aBunruiAnimeidList[ pml::wazadata::DMG_MAX ]  =
        {
          //  へんか
          LA_DSCLOW_BATTLE_DSC_LOW_000__PARTS_SKILLTYPE_TEXTURE_00,
          //  物理
          LA_DSCLOW_BATTLE_DSC_LOW_000__PARTS_SKILLTYPE_TEXTURE_01,
          //  特殊
          LA_DSCLOW_BATTLE_DSC_LOW_000__PARTS_SKILLTYPE_TEXTURE_02
        };

        GFL_ASSERT( m_initParam.wazaBunruiType < GFL_NELEMS( aBunruiAnimeidList ) );
        //  ぶんるいアイコンを変更
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, aBunruiAnimeidList[ m_initParam.wazaBunruiType ] );
      }

      //  威力
      {
        //  項目
        UIView::SetTextboxPaneMessage( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_PALAM_01 ), STR_WAZA_POWER );

        //  数字
        if( 1 < param.powerNum )
        {
          UIView::SetTextboxPaneNumber( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_PALAM_03 ), STR_VALUE_3, param.powerNum, 3 );
        }
        else
        {
          UIView::SetTextboxPaneMessage( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_PALAM_03 ), str_btl_poke_hyphen );
        }
      }

      //  命中
      {
        //  項目
        UIView::SetTextboxPaneMessage( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_PALAM_02 ), STR_WAZA_HIT );

        //  数字
        bool  bPutRet  = false;
        if( 0 < param.hitNum )
        {
          bPutRet  = ( pml::wazadata::IsAlwaysHit( m_initParam.wazaId ) == false );
        }

        if( bPutRet == true )
        {
          UIView::SetTextboxPaneNumber( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_PALAM_04 ), STR_VALUE_3, param.hitNum, 3 );
        }
        else
        {
          UIView::SetTextboxPaneMessage( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_PALAM_04 ), str_btl_poke_hyphen );
        }
      }

      //  技説明
      {
        UIView::SetTextboxPaneMessage( pLayoutWork->GetTextBoxPane( PANENAME_BATTLE_DSC_LOW_000_PANE_DESC_00 ), param.pWazaSetumeiString );
      }

      //  タイプ
      {
        //  プレートの色を変える(全力技は全力技プレート変更で変えているので呼ばない)
        if( m_initParam.bZenryoku == false )
        {
          const s32 aPokeTypeToColorAnimIdMap[] =
          {
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_00,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_01,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_02,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_03,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_04,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_05,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_06,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_07,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_08,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_09,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_10,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_11,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_12,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_13,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_14,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_15,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_16,
            LA_DSCLOW_BATTLE_DSC_LOW_000_COLOR_17
          };

          GFL_ASSERT( param.wazaType < GFL_NELEMS( aPokeTypeToColorAnimIdMap ) );
          pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, aPokeTypeToColorAnimIdMap[ param.wazaType ] );
        }

        //  タイプアイコンのtex差し替え
        {
          const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

          gfl2::lyt::LytParts*  pPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_DSC_LOW_000_PANE_PARTS_WAZATYPE );
          GFL_ASSERT( pPartsPane );

          gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( pPartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMultiResId );
          m_initParam.pAppCommonGrpIconData->ReplacePaneTextureByWazaTypeIcon( param.wazaType, pPicPane );
        }
      }
    }

    //  ボタン関連の設定
    {
      //  入力コールバック設定
      UIView::SetInputListener( this );
    }

    UIView::SetVisible( false );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUISkillStatusLowerDraw::~BattleUISkillStatusLowerDraw()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUISkillStatusLowerDraw::Update( void )
  {
    _UpdateState();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUISkillStatusLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  タッチパネルイベントの検知
  *
  * @param  pTouchPanel タッチパネル
  * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
  *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUISkillStatusLowerDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

    if( pTouchPanel->IsTouchRelease() )
    {
      if( m_initParam.bUseInputSE )
      {
        Sound::PlaySE( SEQ_SE_CANCEL1 );
      }

      m_initParam.pDelegate->OnEnterInput( this, BUTTON_ID_CLOSE );

      return DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
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
  ::app::ui::UIInputListener::ListenerResult BattleUISkillStatusLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

    // @fix NMCat[2418] インフォウィンドウの閉じる操作と同じにするように仕様が変わった
    if( ( pButton->IsTrigger( gfl2::ui::BUTTON_B ) ) || ( pButton->IsTrigger( gfl2::ui::BUTTON_A ) ) )
    {
      if( m_initParam.bUseInputSE )
      {
        Sound::PlaySE( SEQ_SE_CANCEL1 );
      }

      m_initParam.pDelegate->OnEnterInput( this, BUTTON_ID_CLOSE );

      return DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUISkillStatusLowerDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_ROOT, },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
        //  選択画面
        {
          0,
          LYTRES_DSCLOW_BATTLE_DSC_LOW_000_BFLYT,
          LA_DSCLOW_BATTLE_DSC_LOW_000___NUM,
          pAppLytBuff,
          true,
          ::app::util::G2DUtil::SETUP_LOWER,
          true
        },
      };

      //  項目名など固定なので描画側で直接メッセージを指定
      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), m_initParam.pBtlAppMsgData, m_initParam.pBtlWordSet );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUISkillStatusLowerDraw::_UpdateState()
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      //  アニメの終了を検知
      if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, m_startAnimeId ) )
      {
        _SetState( STATE_TYPE_INPUT_IDLE );
      }
    }

    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }

    if( m_state == STATE_TYPE_OUT )
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
  void BattleUISkillStatusLowerDraw::_SetState( const BattleUISkillStatusLowerDraw::StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );

      if( m_windowType == WINDOW_TYPE_INFO )
      {
        gfl2::math::VEC3  pos = gfl2::math::ConvertGfVec3ToNwVec3( m_openEventStartPos );

        m_pOpenEventStartPosPane->SetTranslate( pos );
        m_pOpenEventEndPosPane->SetTranslate( -pos );

        m_startAnimeId  = LA_DSCLOW_BATTLE_DSC_LOW_000_IN_INFO;
      }
      else
      if( m_windowType == WINDOW_TYPE_NORMAL )
      {
        m_startAnimeId  = LA_DSCLOW_BATTLE_DSC_LOW_000_IN;
      }

      //  インフォ画面を開くSEを再生
      if( m_initParam.bUseInputSE == true )
      {
        Sound::PlaySE( SEQ_SE_INFO );
      }

      //  開始アニメをする
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_startAnimeId );
    }
    else
    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

  //  全力技の追加効果メッセージidを取得
  s32 BattleUISkillStatusLowerDraw::_GetZenryokuWazaEffectMsgId( const ZenryokuWazaEffect effectId )
  {
    const s32 aDataTable[ pml::wazadata::ZENRYOKUWAZA_EFFECT_NUM ][ 2 ]  =
    {
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_NONE,               -1 }, // なし
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_ATTACK_UP1,         STR_ZPINFO_02  }, // 攻撃↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_ATTACK_UP2,         STR_ZPINFO_09  }, // 攻撃↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_ATTACK_UP3,         STR_ZPINFO_16  }, // 攻撃↑↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_DEFENSE_UP1,        STR_ZPINFO_03  }, // 防御↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_DEFENSE_UP2,        STR_ZPINFO_10  }, // 防御↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_DEFENSE_UP3,        STR_ZPINFO_17  }, // 防御↑↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP1,      STR_ZPINFO_04  }, // 特攻↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP2,      STR_ZPINFO_11  }, // 特攻↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP3,      STR_ZPINFO_18  }, // 特攻↑↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP1,     STR_ZPINFO_05  }, // 特防↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP2,     STR_ZPINFO_12  }, // 特防↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP3,     STR_ZPINFO_19  }, // 特防↑↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP1,     STR_ZPINFO_06  }, // 素早↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP2,     STR_ZPINFO_13  }, // 素早↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP3,     STR_ZPINFO_20  }, // 素早↑↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_HIT_UP1,         STR_ZPINFO_07  }, // 命中↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_HIT_UP2,         STR_ZPINFO_14  }, // 命中↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_HIT_UP3,         STR_ZPINFO_21  }, // 命中↑↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AVOID_UP1,       STR_ZPINFO_08  }, // 回避↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AVOID_UP2,       STR_ZPINFO_15  }, // 回避↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AVOID_UP3,       STR_ZPINFO_22  }, // 回避↑↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP1,    STR_ZPINFO_23  }, // 攻撃防御特攻特防素早さ↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP2,    STR_ZPINFO_24  }, // 攻撃防御特攻特防素早さ↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP3,    STR_ZPINFO_25  }, // 攻撃防御特攻特防素早さ↑↑↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_CRITICAL_RATIO_UP1, STR_ZPINFO_01  }, // 急所↑
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_RESET_RANK_DOWN,    STR_ZPINFO_26  }, // 能力↓リセット
      // @fix NMCat[1776] msgIDの指定ミス
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_RECOVER_HP,         STR_ZPINFO_27  }, // 自分のHP回復
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_RECOVER_HP_POS,     STR_ZPINFO_28  }, // 次に場に出たポケモンのHPを回復
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_KONOYUBITOMARE,     STR_ZPINFO_29  }, // このゆびとまれ
      { pml::wazadata::ZENRYOKUWAZA_EFFECT_BY_POKETYPE,        STR_ZPINFO_30  }, // 使用するポケモンのタイプで変化する( のろい )
    };

    for( u32 i = 0; i < pml::wazadata::ZENRYOKUWAZA_EFFECT_NUM; ++i )
    {
      if( aDataTable[ i ][ 0 ] == effectId )
      {
        return  aDataTable[ i ][ 1 ];
      }
    }

    return  -1;
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
