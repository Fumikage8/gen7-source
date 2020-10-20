//======================================================================
/**
 * @file TeamSelectUppEntryPokePartyView.cpp
 * @date 2015/07/02 19:29:11
 * @author uchida_yuto
 * @brief チーム選択の上画面選択中のポケモンパーティView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/View/TeamSelectUppEntryPokePartyView.h"

#include "niji_conv_header/app/teamselect/res2d/teamSelect.h"
#include "niji_conv_header/app/teamselect/res2d/teamSelect_anim_list.h"
#include "niji_conv_header/app/teamselect/res2d/teamSelect_pane.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  TeamSelectUppEntryPokePartyView::TeamSelectUppEntryPokePartyView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager ) :
  UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( pRenderManager );
    m_pHeap           = pHeap;
    m_pRenderManager  = pRenderManager;
  }

  TeamSelectUppEntryPokePartyView::~TeamSelectUppEntryPokePartyView()
  {
    GFL_SAFE_DELETE( m_pPokePartyLayout );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void TeamSelectUppEntryPokePartyView::Initialize( void* pLytBinData, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet, app::tool::PokeIcon*  pPokeIconTexManager )
  {
    GFL_ASSERT( pLytBinData );
    GFL_ASSERT( pAppMsgData );
    GFL_ASSERT( pWordSet );

    _Initialize2D( pLytBinData, pAppMsgData, pWordSet );

    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  ポケモンパーティのレイアウト作成
    {
      m_pPokePartyLayout  = GFL_NEW( m_pHeap->GetDeviceHeap() ) TeamSelectPokePartyLayoutParts();

      struct TeamSelectPokePartyLayoutParts::LAYOUT_PARAM layoutParam;
      layoutParam.lytWorkId             = LYT_WORK_NORMAL;
      layoutParam.pG2dUtil              = pG2DUtil;
      layoutParam.pLayoutWork           = pLayoutWork;
      layoutParam.pLytMuitiResID        = &rMultiResId;
      layoutParam.pPokeIconTexManager   = pPokeIconTexManager;

      //  パーツペイン
      layoutParam.pRootViewPartsPane    = pLayoutWork->GetPartsPane( PANENAME_TEAMSELECT_PRV_UPP_000_PANE_TEAMPLATE );

      //  初期デフォルト座標取得
      m_defaultPos  = gfl2::math::ConvertNwVec3ToGfVec3( layoutParam.pRootViewPartsPane->GetTranslate() );

      struct TeamSelectPokePartyLayoutParts::LAYOUT_ANIME_DATA  animeData  =
      {
        {
          LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE_PATERN_00,
          LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE_PATERN_01,
          LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE_COLOR_TEAM_00,
          LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE_COLOR_TEAM_01,
          LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE_COLOR_TEAM_02,
          LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE_COLOR_TEAM_03,
          LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE_COLOR_TEAM_04,
          LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE_COLOR_TEAM_05
        },

        {
          {
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_DIE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_REVIVE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_ITEM_CATCH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_ITEM_RELEASE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_JAMP_HIGH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_JAMP_MIDDLE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_JAMP_LOW,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_SICK,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_JAMP_SELECT,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_00_STOP,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_00_ICON_MALE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_00_ICON_FEMALE
          },

          {
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_DIE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_REVIVE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_ITEM_CATCH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_ITEM_RELEASE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_JAMP_HIGH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_JAMP_MIDDLE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_JAMP_LOW,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_SICK,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_JAMP_SELECT,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_01_STOP,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_01_ICON_MALE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_01_ICON_FEMALE
          },

          {
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_DIE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_REVIVE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_ITEM_CATCH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_ITEM_RELEASE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_JAMP_HIGH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_JAMP_MIDDLE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_JAMP_LOW,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_SICK,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_JAMP_SELECT,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_02_STOP,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_02_ICON_MALE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_02_ICON_FEMALE

          },

          {
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_DIE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_REVIVE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_ITEM_CATCH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_ITEM_RELEASE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_JAMP_HIGH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_JAMP_MIDDLE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_JAMP_LOW,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_SICK,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_JAMP_SELECT,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_03_STOP,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_03_ICON_MALE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_03_ICON_FEMALE
          },

          {
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_DIE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_REVIVE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_ITEM_CATCH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_ITEM_RELEASE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_JAMP_HIGH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_JAMP_MIDDLE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_JAMP_LOW,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_SICK,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_JAMP_SELECT,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_04_STOP,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_04_ICON_MALE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_04_ICON_FEMALE
          },

          {
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_DIE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_REVIVE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_ITEM_CATCH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_ITEM_RELEASE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_JAMP_HIGH,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_JAMP_MIDDLE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_JAMP_LOW,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_SICK,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_JAMP_SELECT,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__ICONPOKE_05_STOP,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_05_ICON_MALE,
            LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000__TEAMPLATE__GENDER_05_ICON_FEMALE
          }
        }
      };

      gfl2::std::MemCopy( &animeData, &layoutParam.animeData, sizeof( layoutParam.animeData ) );

      m_pPokePartyLayout->Initialize( &layoutParam );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void TeamSelectUppEntryPokePartyView::Terminate()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   破棄可能か
    */
  //------------------------------------------------------------------
  bool TeamSelectUppEntryPokePartyView::IsDelete()
  {
    return  ( UIView::IsDrawing() == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectUppEntryPokePartyView::Draw(  gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WORK_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderManager, displayNo, i );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      描画パラメータを設定反映
  * @param[in]  描画パラメータ
  * @param[in]  QRリーダーチェックをするか
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectUppEntryPokePartyView::SetViewParam( const struct TEAM_VIEW_PARAM* pViewParam, const bool bChkQRReader )
  {
    GFL_ASSERT( pViewParam );

    m_pPokePartyLayout->SetViewParam( pViewParam, bChkQRReader );
    m_pPokePartyLayout->CancelAllNGFrame();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ポケモン表示にNGフレームを表示させるか
  * @param[in]  pokePartyIdx  ポケモンパーティidx
  * @param[in]  bDrawFlg      表示させるかどうか
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectUppEntryPokePartyView::SetEnableNGFrame( const u32 pokePartyIdx, const bool bDrawFlg )
  {
    m_pPokePartyLayout->SetEnableNGFrame( pokePartyIdx, bDrawFlg );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ポケモンパーティーのフレームのオブジェクトY座標設定
  * @param[in]  yPos : Y座標設定
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectUppEntryPokePartyView::SetYPosPokePartyFrame( const f32 yPos )
  {
    m_pPokePartyLayout->SetYPos( yPos );
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void TeamSelectUppEntryPokePartyView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
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
        LYTRES_TEAMSELECT_TEAMSELECT_PRV_UPP_000_BFLYT,
        LA_TEAMSELECT_TEAMSELECT_PRV_UPP_000___NUM,
        pAppLytBuff,
        true,
        ::app::util::G2DUtil::SETUP_UPPER,
        true
      }
    };

    UIView::Create2D( m_pHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
