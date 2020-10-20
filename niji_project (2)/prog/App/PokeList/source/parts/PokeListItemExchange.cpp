//==============================================================================
/**
 * @file    PokeListItemExchange.cpp
 * @author  fukushima_yuya
 * @date    2015.03.27
 * @brief   アイテムの並び替え
 */
//==============================================================================

// niji
#include "App/PokeList/source/parts/PokeListCursorDraw.h"
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListItemExchange.h"
#include "App/PokeList/source/parts/PokeListPlateDraw.h"
#include "App/PokeList/source/parts/PokeListWindow.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Tool/AppToolItemEffect.h>
#include <AppLib/include/Ui/UIView.h>
#include <Sound/include/Sound.h>
#include <Savedata/include/ZukanSave.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList_anim_list.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//==============================================================================
// @brief   フォルムチェンジクラス
//==============================================================================
  //----------------------------------------------------------------------------
  // @brief 定数
  //----------------------------------------------------------------------------
  enum FormChangeSeq {
    FC_SEQ_NONE,
    FC_SEQ_CHECK,
    FC_SEQ_EFFECT_BEGIN,
    FC_SEQ_EFFECT_END,
    FC_SEQ_REPLACE,
    FC_SEQ_MESSAGE_WAIT,
    FC_SEQ_END,
    FC_SEQ_NUM
  };

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  FormChange::FormChange( DrawerBase* pBase, PlateDraw** ppPlate, PokeListWindow* pWindow, u8 memberCount, App::Tool::ItemEffect* itemEffect )
    : m_pBase( pBase )
    , m_ppPlate( ppPlate )
    , m_pWindow( pWindow )
    , m_pItemEffect(itemEffect)
    , m_FormNo( 0xFF )
    , m_MemberCount( memberCount )
    , m_Seq( FC_SEQ_NONE )
    , m_AddCnt( 0 )
    , m_TargetCount(0)
    , m_LoopCnt( 0 )
    , m_isEffect( false )
    , m_isInput( false )
    , m_CreateEffectSeq(0)
    , m_DeleteEffectSeq(0)
  {
    for( u8 i=0; i<2; ++i )
    {
      m_CursorPos[i] = CPOS_NULL;
    }

    SetPokeIconPos();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  FormChange::~FormChange( void )
  {
    ;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   破棄処理
   *
   * @return  "true  == 破棄完了"
   * @return  "false == 破棄中"
   */
  //----------------------------------------------------------------------------
  bool FormChange::DeleteFunc( void )
  {
    if( m_pItemEffect )
    {
      if( m_DeleteEffectSeq == 0 )
      {
        m_pItemEffect->End();
        m_DeleteEffectSeq++;
      }
      else if( m_DeleteEffectSeq == 1 )
      {
        m_pItemEffect->Delete( 0 );
        m_DeleteEffectSeq++;
      }
      else {
        return true;
      }

      return false;
    }
    
    return true;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   更新
   * @return  true = 継続、false = 終了
   */
  //----------------------------------------------------------------------------
  bool FormChange::Update( void )
  {
    if( m_Seq != FC_SEQ_MESSAGE_WAIT )
    {
      m_isInput = false;
    }

    switch( m_Seq )
    {
    case FC_SEQ_CHECK:
      {
        if( CheckFormChange() )
        {
          if( CheckEffect() )
          {
            m_CreateEffectSeq = 0;
            m_DeleteEffectSeq = 0;

            m_Seq = FC_SEQ_EFFECT_BEGIN;  //!< エフェクトの開始
          }
          else {
            m_Seq = FC_SEQ_REPLACE;       //!< ポケモンアイコンの差し替え
          }
        }
        else {
          m_Seq = FC_SEQ_END;
        }
      } break;

    case FC_SEQ_EFFECT_BEGIN:
      {
        if( CreateEffect() )
        {
          // エフェクトの表示
          m_Seq = FC_SEQ_REPLACE;
        }
      } break;

    case FC_SEQ_REPLACE:
      {
        // テクスチャの差し替えタイミングかどうか
        if( IsReplaceIcon() )
        {
          // フォルムチェンジ＋テクスチャの差し替え
          FormChangeProc();

          m_Seq = FC_SEQ_EFFECT_END;
        }
      } break;

    case FC_SEQ_EFFECT_END:
      {
        // エフェクトの終了か？
        if( DeleteEffect() )
        {
          // 泣き声の再生
          PlayPokeVoice();

          //m_pWindow->SetMessage( PokeListWindow::MSG_TYPE_MSG_NAME, mes_pokelist_04_09, m_ActCPos );
          //m_pWindow->OpenMsgOnly();
          bool disable = ((m_LoopCnt + 1) < m_TargetCount);
          m_pWindow->OpenMsgOnly(PokeListWindow::MSG_TYPE_MSG_NAME, mes_pokelist_04_09, m_ActCPos, disable);
          m_pWindow->RequestButtonDisable();    // @fix GFNMCat[5661]

          //SetWindowEnable( true );

          m_Seq = FC_SEQ_MESSAGE_WAIT;
        }
      } break;

    case FC_SEQ_MESSAGE_WAIT:
      {
        // 入力があった
        if( !m_pWindow->IsWindowIdle() ) break;

        //SetWindowEnable( false );
        //m_pWindow->CloseMsgOnly();

        m_Seq = FC_SEQ_END;
      } break;

    case FC_SEQ_END:
      {
        m_LoopCnt++;
        m_FormNo = 0xFF;

        if( m_LoopCnt < m_AddCnt )
        {
          m_Seq = FC_SEQ_CHECK;
        }
        else {
          End();
          return false;
        }
      } break;
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの開始
   *
   * @param   once    １回だけの処理かどうか
   */
  //----------------------------------------------------------------------------
  void FormChange::Start( bool once )
  {
    m_isOnce = once;
    m_Seq = FC_SEQ_CHECK;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの終了
   */
  //----------------------------------------------------------------------------
  void FormChange::End( void )
  {
    ItemNameFade( !m_isOnce );

    m_AddCnt = 0;
    m_LoopCnt = 0;
    m_TargetCount = 0;
    m_Seq = FC_SEQ_NONE;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの対象の追加
   * @param   cpos  プレート位置
   */
  //----------------------------------------------------------------------------
  void FormChange::AddFormChangeTarget( CursorPos cpos )
  {
    if( cpos == CPOS_NULL ) return;

    if( m_AddCnt < 2 )
    {
      m_CursorPos[m_AddCnt] = cpos;

      m_AddCnt++;

      if( _CheckFormChange(cpos) )
      {
        if( CheckEffect() )
        {
          m_TargetCount++;
        }
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   入力フラグのセット
   * @param   flag    true = 入力あり、false = 入力なし
   */
  //----------------------------------------------------------------------------
  void FormChange::SetInputFlag( bool input )
  {
    m_isInput = input;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アクティブなプレート位置
   * @return  対象のプレート位置
   */
  //----------------------------------------------------------------------------
  CursorPos FormChange::GetActCursorPos( void )
  {
    return m_ActCPos;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジのチェック
   * @return  true = フォルムチェンジあり、false = フォルムチェンジなし
   */
  //----------------------------------------------------------------------------
  bool FormChange::CheckFormChange( void )
  {
    if( m_AddCnt == 0 ) return false;

    m_ActCPos = m_CursorPos[m_LoopCnt];

    return _CheckFormChange( m_ActCPos );
  }

  bool FormChange::_CheckFormChange( CursorPos cpos )
  {
    bool result = false;

    // 対象位置のポケモンを取得
    pml::pokepara::PokemonParam* pp = m_pBase->GetStaticWork()->GetPoke( cpos );
    {
      // SimpleParamの取得
      PokeTool::GetSimpleParam( &m_SimpleParam, pp );

      bool fastmode = pp->StartFastMode();
      {
        // 所持アイテムNoの取得
        m_ItemNo = pp->GetItem();
        // 変更フォルムNoの取得
        pml::FormNo form = pp->GetNextFormNoFromHoldItem( m_ItemNo );

        FUKUSHIMA_PRINT( "NowFormNo=[%d], NextFormNo=[%d]\n", pp->GetFormNo(), form );

        if( pp->GetFormNo() != form )
        {
          // フォルムチェンジのスキップ
          if( IsSkipFormChangePerformance( pp, form ) )
          {
            FUKUSHIMA_PRINT( "SkipFormChangePerformance\n" );
            pp->EndFastMode( fastmode );    //!< @fix CrossCheck[8270]
            return false;
          }

          m_FormNo = form;
          result = true;
        }
      }
      pp->EndFastMode( fastmode );
    }

    // フォルムチェンジあり
    if( result )
    {
      // 道具名欄のフェード処理
      ItemNameFade( false );
    }

    return result;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   エフェクトの有無のチェック
   * @return  true = あり、false = なし
   */
  //----------------------------------------------------------------------------
  bool FormChange::CheckEffect( void )
  {
    m_isEffect = false;

    if( m_SimpleParam.monsNo == MONSNO_487 )  //!< ギラティナのみエフェクトあり
    {
      m_isEffect = true;   //!< エフェクトあり
    }

    return m_isEffect;     //!< エフェクトなし
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジ処理
   */
  //----------------------------------------------------------------------------
  void FormChange::FormChangeProc( void )
  {
    StaticWork* pStaticWork = m_pBase->GetStaticWork();
    pml::pokepara::PokemonParam* pp = pStaticWork->GetPoke( m_ActCPos );

    // フォルムチェンジ
    _ChangeForm( pp, m_FormNo );

    m_ppPlate[m_ActCPos]->SetupPlatePokeDisplay( pp );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   道具名欄の一括処理
   * @brief   in    true = フェードイン、false = フェードアウト
   */
  //----------------------------------------------------------------------------
  void FormChange::ItemNameFade( bool in )
  {
    StaticWork* pStaticWork = m_pBase->GetStaticWork();
    LocalWork* pLocalWork = m_pBase->GetLocalWork();

    if( pLocalWork->GetDispMode() != DISP_MODE_ITEM_EXCHANGE ) return;

    for( u8 i=0; i<m_MemberCount; ++i )
    {
      if( pStaticWork->IsEgg( static_cast<CursorPos>(i) ) ) continue;

      PlateMessageWindow* pMsgWnd = m_ppPlate[i]->GetPlateMessageWindow();

      if( in )
      {
        pMsgWnd->SetupMessageWindow( PLATE_MODE_ITEM );
      }
      else {
        pMsgWnd->CloseWindow();
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウの入力有効設定
   * @param   enable    true = 有効、false = 無効
   */
  //----------------------------------------------------------------------------
  void FormChange::SetWindowEnable( bool enable )
  {
    app::ui::UIView* pUIView = m_pBase->GetUIView();
    pUIView->SetInputEnabled( enable );

    app::tool::ButtonManager* pBtnMan = m_pBase->GetButtonManager();
    
    for( u8 i=0; i<BTN_ID_NUM; ++i )
    {
      if( i == BTN_ID_DECIDE ) continue;

      pBtnMan->SetButtonInputEnable( i, !enable );
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジ
   *
   * @param   pp    ポケパラム
   * @param   form  フォルム
   */
  //----------------------------------------------------------------------------
  void FormChange::_ChangeForm( pml::pokepara::PokemonParam* pp, pml::FormNo form )
  {
    // フォルムチェンジ
    _SetFormNo( pp, form );

    // 図鑑に反映
    Savedata::ZukanData* zukan = m_pBase->GetStaticWork()->GetZukanData();
    {
      if( zukan )
      {
        zukan->SetPokeSee( *pp );   // @fix GFNMCat[4178]
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   フォルム設定
   *
   * @param   pp    ポケパラム
   * @param   form  フォルム
   */
  //----------------------------------------------------------------------------
  void FormChange::_SetFormNo( pml::pokepara::PokemonParam* pp, pml::FormNo form )
  {
    bool fastmode = pp->StartFastMode();
    {
      // フォルムチェンジ
      pp->ChangeFormNo( form );
    }
    pp->EndFastMode( fastmode );
  }



  //----------------------------------------------------------------------------
  /**
   * @brief   エフェクトの生成
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool FormChange::CreateEffect( void )
  {
    if( m_pItemEffect == NULL ) return true;

    switch( m_CreateEffectSeq )
    {
    case 0:
      {
        gfl2::math::VEC3 pos = m_pPokeIconPos[m_ActCPos];
        App::Tool::ItemEffect::Type effType = m_pItemEffect->GetFormChangeEffectType( m_SimpleParam.monsNo, m_SimpleParam.formNo, m_ItemNo );
        m_pItemEffect->Create( 0, effType, &pos, false );
        m_CreateEffectSeq++;
      } break;

    case 1:
      {
        if( m_pItemEffect->IsCreate( 0 ) )
        {
          m_CreateEffectSeq++;
        }
      } break;

    case 2:
      {
        m_pItemEffect->Start();
        m_CreateEffectSeq++;
      } break;

    case 3:
      {
        return true;
      }
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   エフェクトの破棄
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool FormChange::DeleteEffect( void )
  {
    if( m_pItemEffect == NULL ) return true;

    switch( m_DeleteEffectSeq )
    {
    case 0:
      {
        if( m_pItemEffect->IsEnd( 0 ) )
        {
          m_DeleteEffectSeq++;
        }
      } break;

    case 1:
      {
        m_pItemEffect->End();
        m_DeleteEffectSeq++;
      } break;

    case 2:
      {
        m_pItemEffect->Delete( 0 );
        m_DeleteEffectSeq++;
      } break;

    case 3:
      {
        return true;
      }
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アイコンの差し替えが可能か？
   *
   * @return  "true  = 可"
   * @return  "false = 不可"
   */
  //----------------------------------------------------------------------------
  bool FormChange::IsReplaceIcon( void )
  {
    if( m_pItemEffect )
    {
      if( !m_pItemEffect->IsUpdateTiming( 0 ) ) return false;
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコン座標のセット
   */
  //----------------------------------------------------------------------------
  void FormChange::SetPokeIconPos( void )
  {
    const u32 c_PlateIndex[pml::PokeParty::MAX_MEMBERS] = {
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_00,  //!< 左上
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_03,  //!< 右上
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_01,  //!< 左中
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_04,  //!< 右中
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_02,  //!< 左下
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_05,  //!< 右下
    };

    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      gfl2::lyt::LytParts* parts = m_pBase->GetParts( c_PlateIndex[i] );
      {
        gfl2::lyt::LytPane* pane = m_pBase->GetPane( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_POKEICON, parts );
        {
          m_pPokeIconPos[i] = parts->GetTranslate() + pane->GetTranslate();
        }
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの演出をスキップ
   *
   * @param   pp    ポケモンパラメータ
   * @param   form  変更フォルムNo
   */
  //----------------------------------------------------------------------------
  bool FormChange::IsSkipFormChangePerformance( pml::pokepara::PokemonParam* pp, pml::FormNo form )
  {
    static const MonsNo c_FormChangeSkipMonsNo[] = {
      MONSNO_ARUSEUSU,      //!< アルセウス
      MONSNO_GURIPUSU2,     //!< グリプス２
      MONSNO_GENOSEKUTO,    //!< ゲノセクト
    };

    MonsNo monsNo = m_SimpleParam.monsNo;

    for( u32 i=0; i<GFL_NELEMS(c_FormChangeSkipMonsNo); ++i )
    {
      if( monsNo == c_FormChangeSkipMonsNo[i] )
      {
        // フォルムチェンジ
        _ChangeForm( pp, form );

        return true;    //!< フォルムチェンジ演出をスキップ
      }
    }

    return false;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   泣き声の再生
   */
  //----------------------------------------------------------------------------
  void FormChange::PlayPokeVoice( void )
  {
    pml::pokepara::PokemonParam* pp = NULL;
    {
      StaticWork* staticWork = m_pBase->GetStaticWork();
      {
        pp = staticWork->GetPoke( m_ActCPos );
      }
    }

    bool fastmode = pp->StartFastMode();
    {
      // ギラティナの場合、泣き声を再生
      if( pp->GetMonsNo() == MONSNO_GIRATHINA )
      {
        Sound::PlayVoice( 0, pp );
      }
    }
    pp->EndFastMode( fastmode );
  }





//==============================================================================
// @brief   道具アイコンのドラッグ処理クラス
//==============================================================================
  //----------------------------------------------------------------------------
  // @brief   定数
  //----------------------------------------------------------------------------
  //! @brief  アイコンのドラッグアニメ
  const u32 c_IconDragAnim = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_ICON_POS_NORMAL;
  //! @brief  アイコンのキャッチアニメ
  const u32 c_IconCatchAnim = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_ICON_CATCH;
  //! @brief  アイコンのリリースアニメ
  const u32 c_IconReleaseAnim = LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_ICON_RELEASE;

  //! @brief  プレートの道具キャッチアニメ
  const u32 c_ItemCatchAnim[] = {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_ITEM_CATCH,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_ITEM_CATCH,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_ITEM_CATCH,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_ITEM_CATCH,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_ITEM_CATCH,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_ITEM_CATCH,
  };
  //! @brief  プレートの道具リリースアニメ
  const u32 c_ItemReleaseAnim[] = {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_ITEM_RELEASE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_POKEICON_ITEM_RELEASE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_POKEICON_ITEM_RELEASE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_POKEICON_ITEM_RELEASE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_POKEICON_ITEM_RELEASE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_POKEICON_ITEM_RELEASE,
  };

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pBase   DrawerBaseクラスポインタ
   */
  //----------------------------------------------------------------------------
  ItemIconDrag::ItemIconDrag( DrawerBase* pBase, CursorDraw* pCursor )
    : m_pBase( pBase )
    , m_pCursor( pCursor )
    , m_MovePos( CPOS_NULL )
    , m_pIconParts( NULL )
    , m_pIconPicture( NULL )
    , m_CatchAnim( 0 )
  {
    m_pIconParts = m_pBase->GetParts( PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_ICON );
    m_pIconPicture = m_pBase->GetPicture( PANENAME_COMMON_ICN_006_PANE_ICON_ITEM, m_pIconParts );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  ItemIconDrag::~ItemIconDrag( void )
  {
    ;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ドラッグの開始
   * @param   movePos   移動元
   * @param   itemNo    道具No
   */
  //----------------------------------------------------------------------------
  void ItemIconDrag::StartDrag( CursorPos movePos, u16 itemNo )
  {
    app::tool::ItemIconToolEx* pItemIcon = m_pBase->GetItemIcon();

    m_MovePos = movePos;

    // カーソルの非表示
    m_pCursor->SetVisible( false );

    // 道具アイコンのセット
    pItemIcon->SetItemIcon( 0, itemNo, m_pIconPicture );
    // アイコンの表示
    m_pBase->SetVisiblePane( m_pIconParts, true );

    // アニメーションの再生
    {
      m_CatchAnim   = c_ItemCatchAnim[m_MovePos];
      m_ReleaseAnim = c_ItemReleaseAnim[m_MovePos];

      m_pBase->StartAnim( m_CatchAnim );
      m_pBase->StartAnim( c_IconCatchAnim );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ドラッグ処理
   * @param   tp  タッチ座標
   */
  //----------------------------------------------------------------------------
  void ItemIconDrag::UpdateDrag( gfl2::math::Vector3& tp )
  {
    gfl2::math::VEC3 vec = m_pIconParts->GetTranslate();
    gfl2::math::Vector3 pos = gfl2::math::ConvertNwVec3ToGfVec3( vec );
    //gfl2::math::Vector3 touch = gfl2::math::Vector3( tp.x - 160, 120 - tp.y - 20, 0 );
    gfl2::math::Vector3 touch = gfl2::math::Vector3( tp.x - 160, 120 - tp.y, 0 );    // @fix NMCat[339]

    pos = touch;

    vec = gfl2::math::ConvertGfVec3ToNwVec3( pos );
    m_pIconParts->SetTranslate( vec );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ドラッグの終了
   */
  //----------------------------------------------------------------------------
  void ItemIconDrag::EndDrag( CursorPos destPos, u16 moveItemNo )
  {
    m_pBase->StopAnim( m_CatchAnim );
    if( moveItemNo != ITEM_DUMMY_DATA )
    {
      m_pBase->StartAnim( m_ReleaseAnim );
    }

    if( destPos < GFL_NELEMS(c_ItemReleaseAnim) )
    {
      m_pBase->StartAnim( c_ItemReleaseAnim[destPos] );
    }

    m_pBase->ChangeAnim( c_IconCatchAnim, c_IconReleaseAnim );
  }





//==============================================================================
// @brief   道具入れ替えクラス
//==============================================================================


  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pBase         DrawerBaseクラスポインタ
   * @param   ppPokePlate   PlateDrawクラスポインタ配列
   * @param   pCursor       CursorDrawクラスポインタ
   * @param   pMsgWindow    MessageWindowクラスポインタ
   * @param   memberCount   メンバー数
   */
  //----------------------------------------------------------------------------
  ItemExchange::ItemExchange( DrawerBase* pBase, PlateDraw** ppPokePlate, CursorDraw* pCursor, MessageWindow* pMsgWindow, u8 memberCount )
    : m_pBase( pBase )
    , m_ppPokePlate( ppPokePlate )
    , m_pCursor( pCursor )
    , m_pMsgWindow( pMsgWindow )
    , m_MemberCount( memberCount )
    , m_LoopExchange( true )
    , m_DragMode( false )
    , m_Seq( SEQ_ITEM_NONE )
    , m_MovePos( CPOS_NULL )
    , m_DestPos( CPOS_NULL )
    , m_pMovePlate( NULL )
    , m_pDestPlate( NULL )
    , m_pMoveMsgWnd( NULL )
    , m_pDestMsgWnd( NULL )
    , m_MoveItem( ITEM_DUMMY_DATA )
    , m_DestItem( ITEM_DUMMY_DATA )
    , m_pNullVisible( NULL )
    , m_SelectPlate( CPOS_NULL )
    , m_DecidePlate( CPOS_NULL )
    , m_pItemDrag( NULL )
    , m_pFormChange( NULL )
  {
    m_TriggerBtnID  = BTN_ID_NUM;
    m_DestBtnID     = BTN_ID_NUM;

    Init();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  ItemExchange::~ItemExchange( void )
  {
    GFL_SAFE_DELETE( m_pItemDrag );
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //----------------------------------------------------------------------------
  void ItemExchange::Init( void )
  {
    gfl2::heap::HeapBase* pDevHeap = m_pBase->GetDeviceHeap();
    {
      // アイテムアイコンのドラッグ
      m_pItemDrag = GFL_NEW(pDevHeap) ItemIconDrag( m_pBase, m_pCursor );
    }

    // 機能ボタンの表示切替用ペイン
    m_pNullVisible = m_pBase->GetPane( PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_00 );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //----------------------------------------------------------------------------
  void ItemExchange::Update( void )
  {
    switch( m_Seq )
    {
    case SEQ_ITEM_EXCHANGE:
      {
        m_Seq = SEQ_ITEM_EXCHANGE_END;

        if( m_MovePos == CPOS_NULL ) break;
        if( m_DestPos == CPOS_NULL ) break;

        if( m_MovePos != m_DestPos )
        {
          // ウィンドウを閉じる
          if( m_pMoveMsgWnd ) m_pMoveMsgWnd->CloseWindow();
          if( m_pDestMsgWnd ) m_pDestMsgWnd->CloseWindow();

          m_Seq = SEQ_ITEM_EXCHANGE_CLOSE;
        }
      } break;

    case SEQ_ITEM_EXCHANGE_CLOSE:
      {
        if( m_pMoveMsgWnd && !m_pMoveMsgWnd->IsEndClosed() ) break;
        if( m_pDestMsgWnd && !m_pDestMsgWnd->IsEndClosed() ) break;

        Exchange();   //!< 入れ替え

        m_Seq = SEQ_ITEM_EXCHANGE_OPEN;
      } break;

    case SEQ_ITEM_EXCHANGE_OPEN:
      {
        if( m_pMoveMsgWnd && !m_pMoveMsgWnd->IsEndOpened() ) break;
        if( m_pDestMsgWnd && !m_pDestMsgWnd->IsEndOpened() ) break;

        m_pDestPlate->StartPokeIconSelectAnim( true );  // @fix GFNMcat[1566]

        m_Seq = SEQ_ITEM_FORMCHANGE_BEGIN;
      } break;

    case SEQ_ITEM_EXCHANGE_END:
      {
        Reset();
        SetPokeListEnable( true );

        if( m_LoopExchange )
        {
          m_pBase->GetButtonManager()->SetInputMode( app::tool::ButtonManager::INPUT_MODE_KEY_ONLY );

          m_pMsgWindow->SetMessage( mes_pokelist_02_03 );
          m_Seq = SEQ_ITEM_MOVE_SELECT;
        }
        else {
          EndExchange();
          m_Seq = SEQ_ITEM_NONE;
        }

        m_pCursor->SetVisible( true );
      } break;

    case SEQ_ITEM_FORMCHANGE_BEGIN:
      {
        if( m_pFormChange )
        {
          m_pFormChange->AddFormChangeTarget( m_MovePos );
          m_pFormChange->AddFormChangeTarget( m_DestPos );
          m_pFormChange->Start( !m_LoopExchange );
        }

        m_Seq = SEQ_ITEM_FORMCHANGE_END;
      } break;

    case SEQ_ITEM_FORMCHANGE_END:
      {
        if( m_pFormChange && m_pFormChange->Update() ) break;

        m_Seq = SEQ_ITEM_EXCHANGE_END;
      } break;
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   道具の入れ替えの開始
   */
  //----------------------------------------------------------------------------
  void ItemExchange::StartExchange( bool loop )
  {
#if 1
    // @fix MMCat[121]: m_LoopExchangeを退避
    bool tmp_loop = m_LoopExchange;
    m_LoopExchange = true;
    Reset();
    m_LoopExchange = tmp_loop;
#else
    Reset();
#endif

    // プレートの変更
    {
      PlateMode plateMode = PLATE_MODE_ITEM;

      for( u8 i=0; i<m_MemberCount; ++i )
      {
        PlateMessageWindow* pMsgWindow = m_ppPokePlate[i]->GetPlateMessageWindow();
        pMsgWindow->SetupMessageWindow( plateMode );
        //m_ppPokePlate[i]->SetPlateState( PlateDraw::PLATE_STATE_DEFAULT, plateMode );
      }
    }

    // 機能ボタンの非表示
    app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
    {
      m_pBase->SetVisiblePane( m_pNullVisible, false );

      btnMan->SetButtonInputEnable( BTN_ID_X, false );
      btnMan->SetButtonInputEnable( BTN_ID_Y, false );

      // ボタンの入力モードをキーのみに変更
      btnMan->SetInputMode( app::tool::ButtonManager::INPUT_MODE_KEY_ONLY );
    }

    // 下部メッセージウィンドウの表示
    {
      m_pMsgWindow->SetMessage( mes_pokelist_02_03 );
      m_pMsgWindow->Open();
    }

    // カーソル
    CursorPos cpos = m_pCursor->GetCursorPos();
    m_pCursor->SetCursorID( CID_MOVE, cpos );

    m_LoopExchange = loop;
    m_Seq = SEQ_ITEM_MOVE_SELECT;

    FUKUSHIMA_PRINT("ItemExchange LoopMode[%s]\n", (loop) ? "true" : "false" );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   道具の入れ替えの終了
   */
  //----------------------------------------------------------------------------
  void ItemExchange::EndExchange( void )
  {
    Reset();

    // 下部メッセージウィンドウの非表示
    m_pMsgWindow->Close();

    // 機能ボタンの表示
    app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
    {
      m_pBase->SetVisiblePane( m_pNullVisible, true );

      btnMan->SetButtonInputEnable( BTN_ID_X, true );
      btnMan->SetButtonInputEnable( BTN_ID_Y, true );

      // ボタンの入力モードをデフォルトに戻す
      btnMan->SetInputMode( app::tool::ButtonManager::INPUT_MODE_BOTH );
    }

    // プレートの変更
    {
      StaticWork* pStaticWork = m_pBase->GetStaticWork();
      PlateMode plateMode = PLATE_MODE_PARAM;

      for( u8 i=0; i<m_MemberCount; ++i )
      {
        PlateMessageWindow* pMsgWindow = m_ppPokePlate[i]->GetPlateMessageWindow();
        pMsgWindow->CloseWindow();

        CursorPos cpos = static_cast<CursorPos>(i);
        const pml::pokepara::PokemonParam* pPoke = pStaticWork->GetPokeConst( cpos );
        {
          m_ppPokePlate[i]->SetupPlatePokeDisplay( pPoke );
          m_ppPokePlate[i]->SetModePlate( plateMode );
          m_ppPokePlate[i]->SetPlateState( PlateDraw::PLATE_STATE_DEFAULT, plateMode );
        }
      }
    }

    // 画面モードの設定
    LocalWork* localWork = m_pBase->GetLocalWork();
    {
      localWork->SetDispMode( DISP_MODE_NORMAL );
    }

    // カーソル位置の更新
    UpdateCursorPos();  // @fix GFNMcat[1566]

    m_Seq = SEQ_ITEM_NONE;
  }

  // @fix GFNMcat[1566]
  //----------------------------------------------------------------------------
  /**
   * @brief   カーソル位置の更新
   */
  //----------------------------------------------------------------------------
  void ItemExchange::UpdateCursorPos( void )
  {
    // カーソル位置の取得
    CursorPos cpos = m_pCursor->GetCursorPos( CID_POKE );
    {
      if( m_Seq == SEQ_ITEM_MOVE_SELECT )
      {
        cpos = m_pCursor->GetCursorPos( CID_MOVE );
      }
      else if( m_Seq == SEQ_ITEM_DEST_SELECT )
      {
        cpos = m_pCursor->GetCursorPos( CID_DEST );
      }
    }

    // カーソル位置を更新
    m_pCursor->SetCursorID( CID_POKE, cpos );

    if( cpos <= CPOS_PLATE_BR )
    {
      // ポケモンアイコン選択アニメの再生
      m_ppPokePlate[cpos]->StartPokeIconSelectAnim( true );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ボタン選択処理
   * @param   cpos    カーソル位置
   */
  //----------------------------------------------------------------------------
  void ItemExchange::Decide( CursorPos cpos )
  {
    if( m_Seq == SEQ_ITEM_MOVE_SELECT )
    {
      if( GetItemNo( cpos ) == ITEM_DUMMY_DATA )
      {
        // SE再生：ビープ音
        Sound::PlaySE( SEQ_SE_BEEP );
      }

      SetMovePos( cpos );
    }
    else if( m_Seq == SEQ_ITEM_DEST_SELECT )
    {
      if( m_MovePos != cpos )
      {
        SetDestPos( cpos );
      }
      else {
        Cancel(); //!< 一致しているのでキャンセル扱い
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   キャンセル処理
   * @param   true = 入れ替えの終了、false = 入れ替え継続中
   */
  //----------------------------------------------------------------------------
  bool ItemExchange::Cancel( void )
  {
    if( m_LoopExchange )
    {
      if( m_Seq == SEQ_ITEM_MOVE_SELECT )
      {
        EndExchange();

        return true;    //!< ポケモン入れ替えの終了を通知
      }
      else if( m_Seq == SEQ_ITEM_DEST_SELECT )
      {
        StartExchange();
      }
    }
    else {
      EndExchange();

      return true;    //!< ポケモン入れ替えの終了を通知
    }

    return false;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   移動元のセット
   * @param   cpos    選択したプレート位置
   * @param   isDrag  ドラッグでの入れ替えかどうか(デフォルトは選択入れ替え)
   * @return  "true  = セット完了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool ItemExchange::SetMovePos( CursorPos cpos, bool isDrag )
  {
    GFL_ASSERT( m_Seq == SEQ_ITEM_MOVE_SELECT );

    // プレートの選択が出来るかどうか
    if( !IsSelectPlate( cpos ) )
    {
      return false;   //!< 選択出来ないプレートだった
    }

    StaticWork* pStaticWork = m_pBase->GetStaticWork();
    if( pStaticWork->GetPokeItem( cpos ) == ITEM_DUMMY_DATA )
    {
      return false;   //!< 移動元は必ず道具を所持していなければならない
    }

    // 移動元の位置
    m_MovePos = cpos;

    // カーソル
    //CursorPos nowPos = m_pCursor->GetCursorPos();
    m_pCursor->SetCursorPos( cpos );
    //m_pCursor->SetCursorID( CID_DEST, nowPos );
    m_pCursor->SetCursorID( CID_DEST, cpos );   // @fix NMCat[275]

    // 移動元プレート
    m_pMovePlate = m_ppPokePlate[cpos];
    m_pMoveMsgWnd = m_pMovePlate->GetPlateMessageWindow();
    m_pMovePlate->SetPlateState( PlateDraw::PLATE_STATE_SELECTED );

    // ドラッグでの入れ替えかどうか
    m_DragMode = isDrag;

    app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();

    // ドラッグモードなら道具アイコンの表示
    if( m_DragMode )
    {
      u16 item_no = pStaticWork->GetPokeItem( m_MovePos );

      // 道具名欄をクローズ
      m_pMoveMsgWnd->CloseWindow();

      // 道具アイコンの表示
      m_pItemDrag->StartDrag( m_MovePos, item_no );

      m_pCursor->SetVisible( false );

      btnMan->ResetHoldState();
    }

    // ボタンの入力モードをデフォルトに戻す
    btnMan->SetInputMode( app::tool::ButtonManager::INPUT_MODE_BOTH );

    // 下部メッセージの変更
    m_pMsgWindow->SetMessage( mes_pokelist_02_04 );

    m_Seq = SEQ_ITEM_DEST_SELECT;

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   移動先のセット
   * @param   cpos    選択したプレート位置
   */
  //----------------------------------------------------------------------------
  void ItemExchange::SetDestPos( CursorPos cpos )
  {
    GFL_ASSERT( m_Seq == SEQ_ITEM_DEST_SELECT );

    // @fix NMCat[687]
    //// カーソル
    //{
    //  CursorID cid = (m_LoopExchange) ? CID_MOVE : CID_POKE;
    //  CursorPos now_cpos = (cpos != CPOS_NULL) ? cpos : m_MovePos;
    //  m_pCursor->SetCursorID( cid, now_cpos );
    //  m_pCursor->SetVisible( false );
    //}

    // プレートの選択が出来るかどうか
    if( !IsSelectPlate( cpos ) )
    {
      if( m_LoopExchange )
      {
        StartExchange( m_LoopExchange );  //!< キャンセル扱いとなり移動元の選択からやり直す
      }
      return;   //!< 選択出来ないプレートだった
    }

    // @fix NMCat[687]
    // カーソル
    {
      CursorID cid = (m_LoopExchange) ? CID_MOVE : CID_POKE;
      CursorPos now_cpos = (cpos != CPOS_NULL) ? cpos : m_MovePos;
      m_pCursor->SetCursorID( cid, now_cpos );
      m_pCursor->SetVisible( false );
    }

    // 移動先の位置
    m_DestPos = cpos;
    //m_pCursor->SetCursorPos( cpos );

    // 移動先プレート
    if( cpos != CPOS_NULL )
    {
      m_pDestPlate = m_ppPokePlate[cpos];
      m_pDestPlate->StartPokeIconSelectAnim( false ); // @fix GFNMcat[1566]

      m_pDestMsgWnd = m_pDestPlate->GetPlateMessageWindow();
    }

    // 移動元プレートの状態を戻す
    m_pMovePlate->SetPlateState( PlateDraw::PLATE_STATE_DEFAULT );

    // ポケモンリストの入力を無効
    SetPokeListEnable( false );

    m_Seq = SEQ_ITEM_EXCHANGE;
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   データの初期化
   */
  //----------------------------------------------------------------------------
  void ItemExchange::Reset( void )
  {
    m_MovePos = CPOS_NULL;
    m_DestPos = CPOS_NULL;

    m_pMovePlate = NULL;
    m_pDestPlate = NULL;

    if( m_LoopExchange )
    {
      for( u8 i=0; i<m_MemberCount; ++i )
      {
        m_ppPokePlate[i]->SetPlateState( PlateDraw::PLATE_STATE_DEFAULT, PLATE_MODE_ITEM );
      }
    }

    // カーソルの表示を戻す
    if( m_DragMode )
    {
      m_pCursor->SetVisible( true );
    }
    m_DragMode = false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   プレートが選択できるかどうか
   * @return  true = 選択可、false = 選択不可
   */
  //----------------------------------------------------------------------------
  bool ItemExchange::IsSelectPlate( CursorPos cpos )
  {
    // 選択された箇所がプレートではない
    if( cpos > CPOS_PLATE_BR ) return false;

    // 選択された箇所がメンバー数以上の位置
    if( cpos >= m_MemberCount ) return false;

    // プレートの状態がDISABLE
    if( m_ppPokePlate[cpos]->GetPlateState() == PlateDraw::PLATE_STATE_DISABLE ) return false;

    // 移動元と同じプレート
    if( m_MovePos == cpos ) return false;

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   道具入れ替え
   */
  //----------------------------------------------------------------------------
  void ItemExchange::Exchange( void )
  {
    StaticWork* pStaticWork = m_pBase->GetStaticWork();

    pml::pokepara::PokemonParam* pPokeMove = pStaticWork->GetPoke( m_MovePos );
    pml::pokepara::PokemonParam* pPokeDest = pStaticWork->GetPoke( m_DestPos );

    bool fastmode_flag_m = pPokeMove->StartFastMode();
    bool fastmode_flag_d = pPokeDest->StartFastMode();
    {
      // 所持道具の取得
      m_MoveItem = pPokeMove->GetItem();
      m_DestItem = pPokeDest->GetItem();

      // 道具の入れ替え
      pPokeMove->SetItem( m_DestItem );
      pPokeDest->SetItem( m_MoveItem );

      // プレート表示の更新
      m_pMovePlate->SetupPlatePokeDisplay( pPokeMove );
      m_pDestPlate->SetupPlatePokeDisplay( pPokeDest );
    }
    pPokeDest->EndFastMode( fastmode_flag_d );
    pPokeMove->EndFastMode( fastmode_flag_m );

    // ウィンドウを開く
    if( m_DestItem != ITEM_DUMMY_DATA )
    {
      if( m_pMoveMsgWnd ) m_pMoveMsgWnd->SetupMessageWindow( PLATE_MODE_ITEM );
    }
    if( m_MoveItem != ITEM_DUMMY_DATA )
    {
      if( m_pDestMsgWnd ) m_pDestMsgWnd->SetupMessageWindow( PLATE_MODE_ITEM );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの入力の有効設定の変更
   * @param   enable    有効フラグ(true = 有効、false = 無効)
   */
  //----------------------------------------------------------------------------
  void ItemExchange::SetPokeListEnable( bool enable )
  {
    app::ui::UIView* pUIView = m_pBase->GetUIView();

    pUIView->SetInputEnabled( enable );
  }



  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジのセット
   * @param   pFormChange   FormChangeクラスポインタ
   */
  //----------------------------------------------------------------------------
  void ItemExchange::SetFormChange( FormChange* pFormChange )
  {
    m_pFormChange = pFormChange;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの開始
   * @param   cpos    プレート位置
   */
  //----------------------------------------------------------------------------
  void ItemExchange::StartFormChange( CursorPos cpos )
  {
    if( m_pFormChange )
    {
      SetPokeListEnable( false );

      m_pCursor->SetVisible( false );

      m_pFormChange->AddFormChangeTarget( cpos );

      // フォルムチェンジの開始
      m_pFormChange->Start();

      m_LoopExchange = false;
      m_Seq = SEQ_ITEM_FORMCHANGE_END;
    }
  }

  




//------------------------------------------------------------------------------
/**
 * @brief   タッチ処理
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == 入れ替えイベント開始"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool ItemExchange::TouchEvent( gfl2::ui::TouchPanel* touchPanel )
{
  LocalWork* localWork = m_pBase->GetLocalWork();

  switch( localWork->GetDispMode() )
  {
  case DISP_MODE_ITEM_EXCHANGE:
    return TouchEvent_Exchange( touchPanel );
    // no break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：リスト入替時
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool ItemExchange::TouchEvent_Exchange( gfl2::ui::TouchPanel* touchPanel )
{
  u32 btnID = BTN_ID_NUM;

  // 移動元選択
  if( GetSequence() == SEQ_ITEM_MOVE_SELECT )
  {
    // タッチイベント：トリガー
    if( TouchEvent_Trigger( touchPanel, &btnID ) )
    {
      // 道具を未所持の場合、ビープ音を鳴らす
      if( GetItemNo( btnID ) == ITEM_DUMMY_DATA )
      {
        // SE再生：ビープ音
        Sound::PlaySE( SEQ_SE_BEEP );

        return false;
      }

      // タッチ座標の取得
      gfl2::math::Vector3 pos = GetTouchPos( touchPanel );
      {
        // 道具アイコンの位置を更新
        m_pItemDrag->UpdateDrag( pos );
      }

      m_TriggerBtnID = btnID;

      // 移動元の設定
      SetMovePos( static_cast<CursorPos>(m_TriggerBtnID), true );

      // SE再生：キャッチ音
      Sound::PlaySE( SEQ_SE_CATCH );

      m_DestBtnID = BTN_ID_NUM;

      return true;
    }
  }
  // 移動先選択
  else if( GetSequence() == SEQ_ITEM_DEST_SELECT )
  {
    if( !IsDragMode() )
    {
      // ドラッグモードで無ければタッチ処理は必要ない
      return false;
    }

    // タッチイベント：リリース
    if( TouchEvent_Release( touchPanel ) )
    {
      // 所持道具Noの取得
      u16 itemNo = GetItemNo( m_TriggerBtnID );

      if( m_DestBtnID != BTN_ID_NUM )
      {
        // ドラッグの終了
        m_pItemDrag->EndDrag( static_cast<CursorPos>(m_DestBtnID), itemNo );

        // 移動先の設定
        SetDestPos( static_cast<CursorPos>(m_DestBtnID) );
      }
      else {
        // ドラッグの終了
        m_pItemDrag->EndDrag( CPOS_NULL, itemNo );

        // 移動先の設定
        SetDestPos( CPOS_NULL );
      }

      // SE再生：リリース音
      Sound::PlaySE( SEQ_SE_RELEASE );

      m_TriggerBtnID = BTN_ID_NUM;
      m_DestBtnID    = BTN_ID_NUM;
    }
    else {
      // タッチイベント：ホールド
      TouchEvent_Hold( touchPanel, &btnID );
      // 移動先のボタンIDを設定
      m_DestBtnID = btnID;

      // タッチ座標の取得
      gfl2::math::Vector3 pos = GetTouchPos( touchPanel );
      {
        // 道具アイコンの位置を更新
        m_pItemDrag->UpdateDrag( pos );
      }
    }

    return true;
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：トリガー
 *
 * @param   touchPanel    タッチパネル
 * @param   destID        選択ボタンID格納先
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool ItemExchange::TouchEvent_Trigger( gfl2::ui::TouchPanel* touchPanel, u32* destID )
{
  if( touchPanel->IsTouchTrigger() )
  {
    // タッチしているボタンIDを取得
    u32 btnID = GetTouchBtnID( touchPanel );

    if( btnID == BTN_ID_NUM )
    {
      // タッチイベントなし
      return false;
    }

    // タッチしたボタンのIDを渡す
    if( destID )
    {
      *destID = btnID;
    }

    return true;
  }

  // タッチイベントなし
  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：ホールド
 *
 * @param   touchPanel    タッチパネル
 * @param   destID        選択ボタンID格納先
 * @param   except        除外ボタンID
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool ItemExchange::TouchEvent_Hold( gfl2::ui::TouchPanel* touchPanel, u32* destID, u32 except )
{
  if( touchPanel->IsTouch() )
  {
    // タッチしているボタンIDを取得
    u32 btnID = GetTouchBtnID( touchPanel, except );

    if( btnID == BTN_ID_NUM )
    {
      // タッチイベントなし
      return false;
    }

    // タッチしたボタンのIDを渡す
    if( destID )
    {
      *destID = btnID;
    }

    return true;
  }

  // タッチイベントなし
  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：リリース
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool ItemExchange::TouchEvent_Release( gfl2::ui::TouchPanel* touchPanel )
{
  if( touchPanel->IsTouchRelease() )
  {
    return true;
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   タッチしているボタンIDの取得
 *
 * @param   touchPanel    タッチパネル
 * @param   except        除外ボタンID
 *
 * @return  ボタンID
 */
//------------------------------------------------------------------------------
u32 ItemExchange::GetTouchBtnID( gfl2::ui::TouchPanel* touchPanel, u32 except )
{
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
  {
    // タッチ座標
    u16 tx = touchPanel->GetX();
    u16 ty = touchPanel->GetY();

    // 検索ボタンIDの開始と終端
    u32 start = BTN_ID_PLATE_TL;
    u32 end   = BTN_ID_PLATE_BR;

    for( u32 i=start; i<=end; ++i )
    {
      // 除外ボタンIDならタッチ判定しない
      if( except == i ) continue;
      // ボタンがパッシブ状態ならタッチ判定しない
      if( !btnMan->IsButtonActive( i ) ) continue;

      // ボタンのタッチ判定
      if( btnMan->IsButtonHit( i, tx, ty ) )
      {
        // ボタンあり
        return i;
      }
    }
  }

  // ボタンなし
  return BTN_ID_NUM;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチ座標の取得
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  タッチ座標
 */
//------------------------------------------------------------------------------
gfl2::math::Vector3 ItemExchange::GetTouchPos( gfl2::ui::TouchPanel* touchPanel )
{
  gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );
  {
    pos.x = static_cast<f32>( touchPanel->GetX() );
    pos.y = static_cast<f32>( touchPanel->GetY() );
  }

  return pos;
}


//------------------------------------------------------------------------------
/**
 * @brief   所持道具IDの取得
 *
 * @param   btnID   ボタンID
 *
 * @return  道具ID
 */
//------------------------------------------------------------------------------
u16 ItemExchange::GetItemNo( u32 btnID )
{
  u16 itemNo = ITEM_DUMMY_DATA;

  StaticWork* staticWork = m_pBase->GetStaticWork();
  {
    // 所持道具Noの取得
    itemNo = staticWork->GetPokeItem( static_cast<CursorPos>(btnID) );
  }

  return itemNo;
}

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
