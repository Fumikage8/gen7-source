//======================================================================
/**
 * @file TeamSelectPokePartyLayoutParts.cpp
 * @date 2015/07/03 20:35:54
 * @author uchida_yuto
 * @brief チーム選択のポケモンパーティレイアウトパーツ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/Parts/TeamSelectPokePartyLayoutParts.h"
#include "NetApp/TeamSelect/source/TeamSelectShareParam.h"

#include "AppLib/include/Util/app_util_2d.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "PokeTool/include/PokeToolBattleParty.h"

#include "niji_conv_header/app/teamselect/res2d/teamSelect_pane.h"
#include "niji_conv_header/message/msg_battleteam_select.h"

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //  コンストラクタ
  TeamSelectPokePartyLayoutParts::TeamSelectPokePartyLayoutParts()
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
    * @brief      動作に必要な引数を登録
    * @param[in]  チームの描画なパラメータ
    * @param[in]  レイアウト描画に必要なパラメータ
    * @note
    *    必ず最初に呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TeamSelectPokePartyLayoutParts::Initialize( struct LAYOUT_PARAM* pLayoutParam )
  {
    GFL_ASSERT( pLayoutParam );
    GFL_ASSERT( pLayoutParam->pG2dUtil );

    m_layoutParam = *pLayoutParam;

    //  デフォルトpos
    {
      m_defaultPos          = gfl2::math::ConvertNwVec3ToGfVec3( m_layoutParam.pRootViewPartsPane->GetTranslate() );
    }

    //  チーム名テキストボックスペイン取得
    {
      m_pTeamNameTextBoxPane  = m_layoutParam.pLayoutWork->GetTextBoxPane( m_layoutParam.pRootViewPartsPane, PANENAME_TEAMSELECT_PLT_000_PANE_TEXT_TEAMNAME_00, m_layoutParam.pLytMuitiResID );
      GFL_ASSERT( m_pTeamNameTextBoxPane );

      m_layoutParam.pG2dUtil->SetPaneVisible( m_pTeamNameTextBoxPane, false );
    }

    //  チームロックペイン取得
    {
      m_pLockIconPane = m_layoutParam.pLayoutWork->GetPane( m_layoutParam.pRootViewPartsPane, PANENAME_TEAMSELECT_PLT_000_PANE_LOCKICON, m_layoutParam.pLytMuitiResID );
      GFL_ASSERT( m_pLockIconPane );

      m_layoutParam.pG2dUtil->SetPaneVisible( m_pLockIconPane, false );
    }

    //  QEコードの説明テキストペイン取得
    {
      m_pQRMessageTextBoxPane = m_layoutParam.pLayoutWork->GetTextBoxPane( m_layoutParam.pRootViewPartsPane, PANENAME_TEAMSELECT_PLT_000_PANE_TEXTBOX_00, m_layoutParam.pLytMuitiResID );
      GFL_ASSERT( m_pQRMessageTextBoxPane );

      m_layoutParam.pG2dUtil->SetTextBoxPaneString( m_pQRMessageTextBoxPane, msg_bts_sys_08 );
      m_layoutParam.pG2dUtil->SetPaneVisible( m_pQRMessageTextBoxPane, false );
    }

    //  アイコンペイン設定
    {
      u32 loopNum = GFL_NELEMS( m_aPokeIconParam );

      struct SETUP_DATA
      {
        gfl2::lyt::LytPaneIndex pokeIconPaneIdx;
        gfl2::lyt::LytPaneIndex genderPaneIdx;
        gfl2::lyt::LytPaneIndex maskPaneIdx;
        gfl2::lyt::LytPaneIndex lvTitleNamePaneIdx;
        gfl2::lyt::LytPaneIndex lvNumberPaneIdx;
        gfl2::lyt::LytPaneIndex NGIconPaneIdx;
      };

      const struct SETUP_DATA aSetupData[]  =
      {
        {
          PANENAME_TEAMSELECT_PLT_000_PANE_ICONPOKE_00,
          PANENAME_TEAMSELECT_PLT_000_PANE_GENDER_00,
          PANENAME_TEAMSELECT_PLT_000_PANE_MASK_00,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLV_00,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLVNUMB_00,
          PANENAME_TEAMSELECT_PLT_000_PANE_NOICON_00,
        },

        {
          PANENAME_TEAMSELECT_PLT_000_PANE_ICONPOKE_01,
          PANENAME_TEAMSELECT_PLT_000_PANE_GENDER_01,
          PANENAME_TEAMSELECT_PLT_000_PANE_MASK_01,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLV_01,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLVNUMB_01,
          PANENAME_TEAMSELECT_PLT_000_PANE_NOICON_01,
        },

        {
          PANENAME_TEAMSELECT_PLT_000_PANE_ICONPOKE_02,
          PANENAME_TEAMSELECT_PLT_000_PANE_GENDER_02,
          PANENAME_TEAMSELECT_PLT_000_PANE_MASK_02,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLV_02,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLVNUMB_02,
          PANENAME_TEAMSELECT_PLT_000_PANE_NOICON_02,
        },

        {
          PANENAME_TEAMSELECT_PLT_000_PANE_ICONPOKE_03,
          PANENAME_TEAMSELECT_PLT_000_PANE_GENDER_03,
          PANENAME_TEAMSELECT_PLT_000_PANE_MASK_03,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLV_03,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLVNUMB_03,
          PANENAME_TEAMSELECT_PLT_000_PANE_NOICON_03,
        },

        {
          PANENAME_TEAMSELECT_PLT_000_PANE_ICONPOKE_04,
          PANENAME_TEAMSELECT_PLT_000_PANE_GENDER_04,
          PANENAME_TEAMSELECT_PLT_000_PANE_MASK_04,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLV_04,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLVNUMB_04,
          PANENAME_TEAMSELECT_PLT_000_PANE_NOICON_04,
        },

        {
          PANENAME_TEAMSELECT_PLT_000_PANE_ICONPOKE_05,
          PANENAME_TEAMSELECT_PLT_000_PANE_GENDER_05,
          PANENAME_TEAMSELECT_PLT_000_PANE_MASK_05,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLV_05,
          PANENAME_TEAMSELECT_PLT_000_PANE_TEXTLVNUMB_05,
          PANENAME_TEAMSELECT_PLT_000_PANE_NOICON_05,
        }
      };

      GFL_ASSERT( GFL_NELEMS( aSetupData ) == loopNum );

      //  いったんすべて非表示
      struct LAYOUT_POKE_ICON_PARAM*  pPokeIconParam  = NULL;

      for( u32 i = 0; i < loopNum; ++i )
      {
        const struct SETUP_DATA*  pSetupData  = &aSetupData[ i ];

        pPokeIconParam  = &m_aPokeIconParam[ i ];

        //  ポケモンアイコンのペイン取得
        {
          pPokeIconParam->pPokeIconPartsPane  = m_layoutParam.pLayoutWork->GetPartsPane( m_layoutParam.pRootViewPartsPane, pSetupData->pokeIconPaneIdx, m_layoutParam.pLytMuitiResID );
          GFL_ASSERT( pPokeIconParam->pPokeIconPartsPane );

          //  ポケモンアイコンの画像ペイン取得
          pPokeIconParam->pPokeIconPicture  = m_layoutParam.pLayoutWork->GetPicturePane( pPokeIconParam->pPokeIconPartsPane, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, m_layoutParam.pLytMuitiResID );
          GFL_ASSERT( pPokeIconParam->pPokeIconPicture );

          //  ポケモンアイコンペインの部品ペインアイテムアイコンペインを取得
          pPokeIconParam->pHaveItemIconPane = m_layoutParam.pLayoutWork->GetPane( pPokeIconParam->pPokeIconPartsPane, PANENAME_COMMON_BTN_LOW_001_PANE_ICON_ITEM, m_layoutParam.pLytMuitiResID );
          GFL_ASSERT( pPokeIconParam->pHaveItemIconPane );
        }

        //  性別のペイン取得
        pPokeIconParam->pGenderPartsPane  = m_layoutParam.pLayoutWork->GetPartsPane( m_layoutParam.pRootViewPartsPane, pSetupData->genderPaneIdx, m_layoutParam.pLytMuitiResID );
        GFL_ASSERT( pPokeIconParam->pGenderPartsPane );

        //  レベルタイトルテキストボックスペイン取得
        pPokeIconParam->pLvTitleTextPane  = m_layoutParam.pLayoutWork->GetTextBoxPane( m_layoutParam.pRootViewPartsPane, pSetupData->lvTitleNamePaneIdx, m_layoutParam.pLytMuitiResID );

        //  レベル値テキストボックスペイン取得
        pPokeIconParam->pLvNumberTextBox  = m_layoutParam.pLayoutWork->GetTextBoxPane( m_layoutParam.pRootViewPartsPane, pSetupData->lvNumberPaneIdx, m_layoutParam.pLytMuitiResID );

        //  マスクペイン取得
        pPokeIconParam->pMaskPane         = m_layoutParam.pLayoutWork->GetPane( m_layoutParam.pRootViewPartsPane, pSetupData->maskPaneIdx, m_layoutParam.pLytMuitiResID );

        //  NGアイコンペイン取得
        pPokeIconParam->pNGIconPane       = m_layoutParam.pLayoutWork->GetPane( m_layoutParam.pRootViewPartsPane, pSetupData->NGIconPaneIdx, m_layoutParam.pLytMuitiResID );
        GFL_ASSERT( pPokeIconParam->pNGIconPane );

        //  ペインはすべて非表示
        {
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pPokeIconPartsPane, false );
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pGenderPartsPane, false );
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pMaskPane, false );
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pLvTitleTextPane, false );
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pLvNumberTextBox, false );
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pNGIconPane, false );
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief      レイアウト内容を設定
    * @param[in]  チームの描画なパラメータ
    */
  //------------------------------------------------------------------
  void TeamSelectPokePartyLayoutParts::SetViewParam( const struct TEAM_VIEW_PARAM* pTeamViewParam, const bool bChkQRReader )
  {
    GFL_ASSERT( pTeamViewParam );
    GFL_ASSERT( pTeamViewParam->pTeamData );
    GFL_ASSERT( pTeamViewParam->pTeamData->pPokeParty );
    GFL_ASSERT( m_layoutParam.pPokeIconTexManager );

    struct LAYOUT_POKE_ICON_PARAM*  pPokeIconParam  = NULL;

    u32 loopNum = GFL_NELEMS( m_aPokeIconParam );

    //  いったんすべてのペインを非表示
    {
      for( u32 i = 0; i < loopNum; ++i )
      {
        pPokeIconParam  = &m_aPokeIconParam[ i ];
        m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pPokeIconPartsPane, false );
        m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pGenderPartsPane, false );
        m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pMaskPane, false );
        m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pLvTitleTextPane, false );
        m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pLvNumberTextBox, false );
        m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pNGIconPane, false );
      }
    }

    m_layoutParam.pG2dUtil->SetPaneVisible( m_pLockIconPane, false );
    m_layoutParam.pG2dUtil->SetPaneVisible( m_pTeamNameTextBoxPane, false );

    //  データがない空
    if( pTeamViewParam->pTeamData->status == TEAM_STATUS_NONE )
    {
      m_layoutParam.pG2dUtil->StartAnime( m_layoutParam.lytWorkId, m_layoutParam.animeData.aAnimeIdx[ ANIME_ID_POKE_PARTY_VISIBLE ] );
      return;
    }
    //  QRコード画面なら専用設定
    else
    if( pTeamViewParam->pTeamData->status == TEAM_STATUS_QR_READ_REQUEST )
    {
      bool  bPokeSet  = false;
      if( bChkQRReader == true )
      {
        if( pTeamViewParam->pTeamData->bQRReaderData == true )
        {
          //  ポケモンがいる
          if( 0 < pTeamViewParam->pTeamData->pPokeParty->GetPokeParty()->GetMemberCount() )
          {
            bPokeSet  = true;
          }
        }
      }

      //  ポケモン設定ができない場合はQR表示に
      if( bPokeSet == false )
      {
        //  QE用のアニメ再生
        m_layoutParam.pG2dUtil->StartAnime( m_layoutParam.lytWorkId, m_layoutParam.animeData.aAnimeIdx[ ANIME_ID_QR_VISIBLE ] );
        m_layoutParam.pG2dUtil->SetPaneVisible( m_pQRMessageTextBoxPane, true );
        return;
      }
    }
    
    //  通常ポケモンパーティーのアニメ再生
    m_layoutParam.pG2dUtil->StartAnime( m_layoutParam.lytWorkId, m_layoutParam.animeData.aAnimeIdx[ ANIME_ID_POKE_PARTY_VISIBLE ] );

    //  QR用の説明テキストは非表示
    m_layoutParam.pG2dUtil->SetPaneVisible( m_pQRMessageTextBoxPane, false );

    //  ロックアイコンを出すか
    {
      if( _IsLoak( pTeamViewParam->pTeamData ) == true )
      {
        m_layoutParam.pG2dUtil->SetPaneVisible( m_pLockIconPane, true );
      }
    }

    //  チーム名表示
    {
      m_layoutParam.pG2dUtil->SetPaneVisible( m_pTeamNameTextBoxPane, true );
      m_layoutParam.pG2dUtil->SetTextBoxPaneString( m_pTeamNameTextBoxPane, pTeamViewParam->pTeamData->pPokeParty->GetTeamName() );
    }

    //  必要なアイコンだけ表示して内容を設定
    u32 memberCnt = pTeamViewParam->pTeamData->pPokeParty->GetPokeParty()->GetMemberCount();
    for( u32 i = 0; i < memberCnt; ++i )
    {
      pPokeIconParam  = &m_aPokeIconParam[ i ];

      const pml::pokepara::PokemonParam*  pPokeParam  = pTeamViewParam->pTeamData->pPokeParty->GetPokeParty()->GetMemberPointerConst( i );
      GFL_ASSERT( pPokeParam );

      //  パラメータアクセスを高速モードに
      bool  bParamFastMode  = pPokeParam->StartFastMode();

      //  IDがなければ空白にする
      if( pPokeParam->IsNull() == false )
      {
        m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pPokeIconPartsPane, true );

        //  性別設定アニメで
        {
          u32 animeId  = gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR;

          pml::Sex sex = pPokeParam->GetSex();
          // オス
          if( sex == pml::SEX_MALE )
          {
            animeId = m_layoutParam.animeData.aPokeIconAnimeId[ i ][ POKE_ICON_ANIME_ID_MALE ];
          }
          // メス
          else
          if( sex == pml::SEX_FEMALE )
          {
            animeId = m_layoutParam.animeData.aPokeIconAnimeId[ i ][ POKE_ICON_ANIME_ID_FEMALE ];
          }
          // 不明
          else
          if( sex == pml::SEX_UNKNOWN )
          {
          }

          if( animeId != gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR )
          {
            m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pGenderPartsPane, true );
            m_layoutParam.pG2dUtil->StartAnime( m_layoutParam.lytWorkId, animeId );
          }
          else
          {
            m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pGenderPartsPane, false );
          }
        }

        // @fix GFNMCat[834] タマゴの時はレベルと性別関連のパラメータは非表示
        if( pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == true )
        {
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pLvTitleTextPane, false );
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pLvNumberTextBox, false );
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pGenderPartsPane, false );
        }
        else
        {
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pLvTitleTextPane, true );
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pLvNumberTextBox, true );
        }

        //  テクスチャー差し替え(すでに事前にアイコンはロードしている前提)
        m_layoutParam.pPokeIconTexManager->ReplaceReadTexture( pTeamViewParam->aPokeIconDataId[ i ], pPokeIconParam->pPokeIconPicture );

        //  レベル表示
        {
          //  タイトル
          m_layoutParam.pG2dUtil->SetTextBoxPaneString ( pPokeIconParam->pLvTitleTextPane, msg_bts_sys_09 );

          //  レベル値
          m_layoutParam.pG2dUtil->SetTextBoxPaneNumber( pPokeIconParam->pLvNumberTextBox, msg_bts_sys_05, pPokeParam->GetLevel(), 3 );
        }

        //  アイテムを持っているか
        if( pPokeParam->GetItem() != ITEM_DUMMY_DATA )
        {
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pHaveItemIconPane, true );
        }
        else
        {
          m_layoutParam.pG2dUtil->SetPaneVisible( pPokeIconParam->pHaveItemIconPane, false );
        }
      }

      pPokeParam->EndFastMode( bParamFastMode );
    }

    //  チーム番号で色を変える
    {
      static const AnimeId aColorAnimeIdx[] =
      {
        ANIME_ID_TEME_01_COLOR,
        ANIME_ID_TEME_02_COLOR,
        ANIME_ID_TEME_03_COLOR,
        ANIME_ID_TEME_04_COLOR,
        ANIME_ID_TEME_05_COLOR,
        ANIME_ID_TEME_06_COLOR
      };

      AnimeId animeIdx = ANIME_ID_TEME_01_COLOR;

      u32 chkNum  = GFL_NELEMS( aColorAnimeIdx );
      if( pTeamViewParam->pTeamData->no < chkNum )
      {
        animeIdx = aColorAnimeIdx[ pTeamViewParam->pTeamData->no ];
      }
      else
      {
        GFL_ASSERT( 0 );
      }

      m_layoutParam.pG2dUtil->StartAnime( m_layoutParam.lytWorkId, m_layoutParam.animeData.aAnimeIdx[ animeIdx ] );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      NGフレームを表示させるか
  * @param[in]  pokePartyIdx  ポケモンパーティidx
  * @param[in]  bFlg          NGかどうか
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectPokePartyLayoutParts::SetEnableNGFrame( const u32 pokePartyIdx, const bool bFlg )
  {
    GFL_ASSERT( pokePartyIdx < GFL_NELEMS( m_aPokeIconParam ) );
    
    if( bFlg == false )
    {
      m_layoutParam.pG2dUtil->SetPaneVisible( m_aPokeIconParam[ pokePartyIdx ].pNGIconPane, false );
      m_layoutParam.pG2dUtil->SetPaneVisible( m_aPokeIconParam[ pokePartyIdx ].pMaskPane, true );
    }
    else
    {
      m_layoutParam.pG2dUtil->SetPaneVisible( m_aPokeIconParam[ pokePartyIdx ].pNGIconPane, true );
      m_layoutParam.pG2dUtil->SetPaneVisible( m_aPokeIconParam[ pokePartyIdx ].pMaskPane, false );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      全ポケモン表示にＮＧフレームを設定をキャンセル
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectPokePartyLayoutParts::CancelAllNGFrame()
  {
    u32 loopNum = GFL_NELEMS( m_aPokeIconParam );
    for( u32 i = 0; i < loopNum; ++i )
    {
      m_layoutParam.pG2dUtil->SetPaneVisible( m_aPokeIconParam[ i ].pNGIconPane, false );
      m_layoutParam.pG2dUtil->SetPaneVisible( m_aPokeIconParam[ i ].pMaskPane, false );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ルートオブジェクトのY座標設定
  * @param[in]  yPos  : 設定するY座標
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectPokePartyLayoutParts::SetYPos( const f32 yPos )
  {
    const gfl2::math::VEC3& nowPos  = m_layoutParam.pRootViewPartsPane->GetTranslate();
    gfl2::math::VEC3  newPos  = nowPos;

    newPos.y  = yPos;
    m_layoutParam.pRootViewPartsPane->SetTranslate( newPos );
  }

  //  指定したチームがロック中か
  bool TeamSelectPokePartyLayoutParts::_IsLoak( const struct TEAM_DATA* pTeamData )
  {
    if( pTeamData->lockStatus == LOCK_STATUS_NONE )
    {
      return  false;
    }

    return  true;
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
