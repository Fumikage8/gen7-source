//==============================================================================
/**
 @file    ZukanPicScrollUtil.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.02
 @brief   図鑑の絵巻の便利機能
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ZukanSave.h>

#include <PokeTool/include/PokeToolExtendData.h>

#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/alola_top.h>
#include <niji_conv_header/app/zukan/alola_top_pane.h>
#include <niji_conv_header/app/zukan/alola_top_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑のインクルード
#include "ZukanPicScrollDataAccessor.h"

#include "ZukanPicScrollUtil.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanPicScrollUtil
//
//==============================================================================

ZukanPicScrollUtil::ZukanPicScrollUtil(
  app::util::G2DUtil*                 pG2DUtil,
  gfl2::lyt::LytMultiResID            lytRes,
  gfl2::lyt::LytWk*                   pLytWk,
  u32                                 lytWkId,
  const ZukanPicScrollDataAccessor*   pPicScrollDataAccessor,
  const PokeTool::ExtendData*         pPokeExtendData,
  gfl2::str::MsgData*                 pMsgData,
  print::WordSet*                     pWordSet
)
  : m_pG2DUtil(pG2DUtil),
    m_lytRes(lytRes),
    m_pLytWk(pLytWk),
    m_lytWkId(lytWkId),
    m_pPicScrollDataAccessor(pPicScrollDataAccessor),
    m_pPokeExtendData(pPokeExtendData),
    m_pMsgData(pMsgData),
    m_pWordSet(pWordSet)
{}
  
ZukanPicScrollUtil::~ZukanPicScrollUtil()
{}

void ZukanPicScrollUtil::SetupBgInfo(
    BgInfo*           bgInfo,
    gfl2::lyt::LytWk* pCurtainLytWk,
    u32               curtainLytWkId,
    gfl2::lyt::LytWk* pBackmostLytWk,
    u32               backmostLytWkId
)
{
  static const BgIdInfo BG_ID_INFO[BgInfo::BG_NUM] =
  {
    {
      PANENAME_ZUKAN_MIN_UPP_003_PANE_BG_EVO3,
      PANENAME_ZUKAN_MIN_UPP_003_PANE_BG_RARE,
      PANENAME_ZUKAN_MIN_UPP_003_PANE_BG_SR,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_EVO3_IN,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_RARE_IN,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_SR_IN,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_EVO3_BG_LOOP,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_RARE_BG_LOOP,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_SR_BG_LOOP,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_EVO3_OUT,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_RARE_OUT,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_003__BG_SR_OUT,
    },
    {
      PANENAME_ZUKAN_MIN_UPP_004_PANE_BG_EVO3,
      PANENAME_ZUKAN_MIN_UPP_004_PANE_BG_RARE,
      PANENAME_ZUKAN_MIN_UPP_004_PANE_BG_SR,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_EVO3_IN,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_RARE_IN,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_SR_IN,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_EVO3_BG_LOOP,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_RARE_BG_LOOP,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_SR_BG_LOOP,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_EVO3_OUT,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_RARE_OUT,
      LA_ALOLA_TOP_ZUKAN_MIN_UPP_004__BG_SR_OUT,
    },
  };

  gfl2::lyt::LytWk* pBgLytWk[BgInfo::BG_NUM] =
  {
    pCurtainLytWk,
    pBackmostLytWk,
  };

  //////////////////////////////////////////////
  // 背景
  //////////////////////////////////////////////
  for(s32 i=0; i<BgInfo::BG_NUM; ++i)
  {
    // ペインのアドレスを取得しておく
    bgInfo->bgPane[i][ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = pBgLytWk[i]->GetPartsPane(BG_ID_INFO[i].bgNormalPaneId     );
    bgInfo->bgPane[i][ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = pBgLytWk[i]->GetPartsPane(BG_ID_INFO[i].bgQuasiLegendPaneId);
    bgInfo->bgPane[i][ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = pBgLytWk[i]->GetPartsPane(BG_ID_INFO[i].bgLegendPaneId     );
    // アニメを取得しておく
    bgInfo->bgInAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = BG_ID_INFO[i].bgNormalInAnimId     ;
    bgInfo->bgInAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = BG_ID_INFO[i].bgQuasiLegendInAnimId;
    bgInfo->bgInAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = BG_ID_INFO[i].bgLegendInAnimId     ;
    bgInfo->bgLoopAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = BG_ID_INFO[i].bgNormalLoopAnimId     ;
    bgInfo->bgLoopAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = BG_ID_INFO[i].bgQuasiLegendLoopAnimId;
    bgInfo->bgLoopAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = BG_ID_INFO[i].bgLegendLoopAnimId     ;
    bgInfo->bgOutAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = BG_ID_INFO[i].bgNormalOutAnimId     ;
    bgInfo->bgOutAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = BG_ID_INFO[i].bgQuasiLegendOutAnimId;
    bgInfo->bgOutAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = BG_ID_INFO[i].bgLegendOutAnimId     ;
  }
}

void ZukanPicScrollUtil::SetupSheetInfo(SheetInfo* sheetInfo) const  // PlaceInfoは外部で設定しておくこと。この関数内ではPlaceInfoのセットアップはしない。
{
  sheetInfo->placeTypeTopPane = m_pLytWk->GetPartsPane(PANENAME_ZUKAN_MIN_UPP_000_PANE_GRP000_EVO3);
  sheetInfo->sheetTopPane = m_pLytWk->GetPane(PANENAME_ZUKAN_MIN_UPP_000_PANE_ALL);
}

void ZukanPicScrollUtil::SetupPlaceInfo(PlaceInfo* placeInfo, gfl2::lyt::LytParts* placeTypeTopPane) const
{
  this->setupPlaceInfoForGrp000Evo3 (placeInfo, placeTypeTopPane);
}


// SpotAnimName名前生成マクロ
#define SAN(parts,prefix,no,suffix) LA_ALOLA_TOP_ZUKAN_MIN_UPP_000__##parts##__##prefix##no##suffix
#define SAN9(parts,prefix,suffix) \
          {                              \
            SAN(parts,prefix,_00,suffix), \
            SAN(parts,prefix,_01,suffix), \
            SAN(parts,prefix,_02,suffix), \
            SAN(parts,prefix,_03,suffix), \
            SAN(parts,prefix,_04,suffix), \
            SAN(parts,prefix,_05,suffix), \
            SAN(parts,prefix,_06,suffix), \
            SAN(parts,prefix,_07,suffix), \
            SAN(parts,prefix,_08,suffix), \
          },


void ZukanPicScrollUtil::setupPlaceInfoForGrp000Evo3(PlaceInfo* placeInfo, gfl2::lyt::LytParts* placeTypeTopPane) const
{
  static const u32 POKE_TOP_PANE_ID[9] =
  {
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_00,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_01,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_02,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_03,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_04,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_05,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_06,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_07,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_POKEMON_08,
  };
  static const u32 POKE_PARTS_PANE_ID[9] =
  {
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_00,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_01,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_02,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_03,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_04,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_05,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_06,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_07,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_POKEMON_08,
  };

  static const u32 BALL_TOP_PANE_ID[9] =
  {
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_00,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_01,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_02,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_03,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_04,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_05,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_06,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_07,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_BALL_08,
  };
  static const u32 BALL_PARTS_PANE_ID[9] =
  {
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_00,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_01,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_02,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_03,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_04,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_05,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_06,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_07,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_BALL_08,
  };
 
  static const u32 NAME_TOP_PANE_ID[9] =
  {
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_00,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_01,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_02,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_03,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_04,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_05,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_06,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_07,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_NAME_08,
  };
  static const u32 NAME_PARTS_PANE_ID[9] =
  {
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_00,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_01,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_02,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_03,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_04,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_05,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_06,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_07,
    PANENAME_ZUKAN_GRP_UPP_000_PANE_EVO3_NAME_08,
  };

  static const u32 SPOT_ANIM_ID[SPOT_ANIM_ID_NUM][9] =
  {
    SAN9(GRP000_EVO3,EVO3_POKEMON,_WAIT)
    SAN9(GRP000_EVO3,EVO3_POKEMON,_IN)

    SAN9(GRP000_EVO3,EVO3_NAME,_PLATE_WAIT)
    SAN9(GRP000_EVO3,EVO3_NAME,_SHINE_N)
    SAN9(GRP000_EVO3,EVO3_NAME,_SHINE)
    SAN9(GRP000_EVO3,EVO3_NAME,_SHINE_SR)

    SAN9(GRP000_EVO3,EVO3_BALL,_BASE_WAIT)
    SAN9(GRP000_EVO3,EVO3_BALL,_RARE_WAIT)
    SAN9(GRP000_EVO3,EVO3_BALL,_SR_WAIT)
    SAN9(GRP000_EVO3,EVO3_BALL,_BASE_CHANGE)
    SAN9(GRP000_EVO3,EVO3_BALL,_RARE_CHANGE)
    SAN9(GRP000_EVO3,EVO3_BALL,_SR_CHANGE)
    SAN9(GRP000_EVO3,EVO3_BALL,_RARE_EFFECT_LOOP)
    SAN9(GRP000_EVO3,EVO3_BALL,_SR_EFFECT_LOOP)
  };

  this->internalSetupPlaceInfo(
      placeInfo,
      placeTypeTopPane,
      9,
      POKE_TOP_PANE_ID,
      POKE_PARTS_PANE_ID,
      BALL_TOP_PANE_ID,
      BALL_PARTS_PANE_ID,
      NAME_TOP_PANE_ID,
      NAME_PARTS_PANE_ID,
      reinterpret_cast<const u32*>(SPOT_ANIM_ID)
  );
}

void ZukanPicScrollUtil::internalSetupPlaceInfo(
    PlaceInfo* placeInfo,
    gfl2::lyt::LytParts* placeTypeTopPane,
    const u32  SPOT_NUM,
    const u32* POKE_TOP_PANE_ID,
    const u32* POKE_PARTS_PANE_ID,
    const u32* BALL_TOP_PANE_ID,
    const u32* BALL_PARTS_PANE_ID,
    const u32* NAME_TOP_PANE_ID,
    const u32* NAME_PARTS_PANE_ID,
    const u32* SPOT_ANIM_ID
) const
{
  //////////////////////////////////////////////
  // [固定処理]  placeTypeの各スポット
  //////////////////////////////////////////////
  // ポケモンのペインのアドレスを取得しておく
  {
    for(u32 i=0; i<SPOT_NUM; ++i)
    {
      placeInfo->spotInfo[i].pokeTopPane   = m_pLytWk->GetPane(placeTypeTopPane, POKE_TOP_PANE_ID[i], &m_lytRes);
      placeInfo->spotInfo[i].pokePartsPane = m_pLytWk->GetPartsPane(placeTypeTopPane, POKE_PARTS_PANE_ID[i], &m_lytRes);
     
      // 残像３つ＋黒１つ
      placeInfo->spotInfo[i].pokePicPane[0] = m_pLytWk->GetPicturePane(placeInfo->spotInfo[i].pokePartsPane, PANENAME_ZUKAN_ALA_UPP_001_PANE_00, &m_lytRes);
      placeInfo->spotInfo[i].pokePicPane[1] = m_pLytWk->GetPicturePane(placeInfo->spotInfo[i].pokePartsPane, PANENAME_ZUKAN_ALA_UPP_001_PANE_01, &m_lytRes);
      placeInfo->spotInfo[i].pokePicPane[2] = m_pLytWk->GetPicturePane(placeInfo->spotInfo[i].pokePartsPane, PANENAME_ZUKAN_ALA_UPP_001_PANE_02, &m_lytRes);
      placeInfo->spotInfo[i].pokePicPane[3] = m_pLytWk->GetPicturePane(placeInfo->spotInfo[i].pokePartsPane, PANENAME_ZUKAN_ALA_UPP_001_PANE_03, &m_lytRes);
    }
  }

  // 捕獲前モンボマークのペインのアドレスを取得しておく
  {
    for(u32 i=0; i<SPOT_NUM; ++i)
    {
      placeInfo->spotInfo[i].ballTopPane   = m_pLytWk->GetPane(placeTypeTopPane, BALL_TOP_PANE_ID[i], &m_lytRes);
      placeInfo->spotInfo[i].ballPartsPane = m_pLytWk->GetPartsPane(placeTypeTopPane, BALL_PARTS_PANE_ID[i], &m_lytRes);
    }
  }

  // 名前プレートのペインのアドレスを取得しておく
  {
    for(u32 i=0; i<SPOT_NUM; ++i)
    {
      placeInfo->spotInfo[i].nameTopPane   = m_pLytWk->GetPane(placeTypeTopPane, NAME_TOP_PANE_ID[i], &m_lytRes);
      placeInfo->spotInfo[i].namePartsPane = m_pLytWk->GetPartsPane(placeTypeTopPane, NAME_PARTS_PANE_ID[i], &m_lytRes);
    
      placeInfo->spotInfo[i].nameBallPane     = m_pLytWk->GetPane(placeInfo->spotInfo[i].namePartsPane, PANENAME_ZUKAN_ALA_UPP_002_PANE_BALL, &m_lytRes);
      placeInfo->spotInfo[i].nameNoTextPane   = m_pLytWk->GetTextBoxPane(placeInfo->spotInfo[i].namePartsPane, PANENAME_ZUKAN_ALA_UPP_002_PANE_TEXTBOX_01, &m_lytRes);
      placeInfo->spotInfo[i].nameNameTextPane = m_pLytWk->GetTextBoxPane(placeInfo->spotInfo[i].namePartsPane, PANENAME_ZUKAN_ALA_UPP_002_PANE_TEXTBOX_00, &m_lytRes);
    }
  }

  // アニメを取得しておく
  {
    for(u32 i=0; i<SPOT_NUM; ++i)
    {
      u32 j = 0;

      j = SPOT_ANIM_ID_POKE_WAIT                     ;  placeInfo->spotInfo[i].pokeWaitAnimId      = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_POKE_IN                       ;  placeInfo->spotInfo[i].pokeInAnimId        = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
                                                                                                                                               
      j = SPOT_ANIM_ID_NAME_PLATE_WAIT               ;  placeInfo->spotInfo[i].namePlateWaitAnimId                                                       = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_NAME_NORMAL_SHINE             ;  placeInfo->spotInfo[i].nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL]             = SPOT_ANIM_ID[ SPOT_NUM * j + i ]; 
      j = SPOT_ANIM_ID_NAME_QUASI_LEGEND_SHINE       ;  placeInfo->spotInfo[i].nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND]       = SPOT_ANIM_ID[ SPOT_NUM * j + i ]; 
      j = SPOT_ANIM_ID_NAME_LEGEND_SHINE             ;  placeInfo->spotInfo[i].nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ]       = SPOT_ANIM_ID[ SPOT_NUM * j + i ];  
                                                                                                                                               
      j = SPOT_ANIM_ID_BALL_NORMAL_WAIT              ;  placeInfo->spotInfo[i].ballWaitAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ]        = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_QUASI_LEGEND_WAIT        ;  placeInfo->spotInfo[i].ballWaitAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND]        = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_LEGEND_WAIT              ;  placeInfo->spotInfo[i].ballWaitAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ]        = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_NORMAL_CHANGE            ;  placeInfo->spotInfo[i].ballChangeAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ]      = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_QUASI_LEGEND_CHANGE      ;  placeInfo->spotInfo[i].ballChangeAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND]      = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_LEGEND_CHANGE            ;  placeInfo->spotInfo[i].ballChangeAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ]      = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      // SPOT_ANIM_ID_BALL_EFFECT_NORMAL_LOOPはない
      j = SPOT_ANIM_ID_BALL_EFFECT_QUASI_LEGEND_LOOP ;  placeInfo->spotInfo[i].ballEffectLoopAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND]  = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_EFFECT_LEGEND_LOOP       ;  placeInfo->spotInfo[i].ballEffectLoopAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ]  = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
    }
  }
}


// SpotAnimName名前生成マクロ
#undef SAN9
#undef SAN


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

