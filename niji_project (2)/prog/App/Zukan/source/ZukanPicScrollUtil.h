#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLUTIL_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLUTIL_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPicScrollUtil.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.02
 @brief   図鑑の絵巻の便利機能
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

// 図鑑のインクルード
#include "ZukanPicScrollDataAccessor.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class MsgData;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
  class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(PokeTool)
  class ExtendData;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class G2DUtil;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  //class ZukanPicScrollDataAccessor;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPicScrollUtil
 @brief     図鑑の絵巻の便利機能
 */
//==============================================================================
class ZukanPicScrollUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPicScrollUtil);

public:
  ZukanPicScrollUtil(
    app::util::G2DUtil*                 pG2DUtil,
    gfl2::lyt::LytMultiResID            lytRes,
    gfl2::lyt::LytWk*                   pLytWk,
    u32                                 lytWkId,
    const ZukanPicScrollDataAccessor*   pPicScrollDataAccessor,
    const PokeTool::ExtendData*         pPokeExtendData,
    gfl2::str::MsgData*                 pMsgData,  // <niji_conv_header/message/msg_zukan.h>
    print::WordSet*                     pWordSet
  );
  virtual ~ZukanPicScrollUtil();

public:
  enum
  {
    POKE_NUM_MAX    = 9,           // 1画面に出るポケモン数の最大値
  };
  static const u32 ANIM_ID_NULL = 0xFFFFFFFF;  // u32で扱っているアニメIDのNULL値

  // 背景情報
  struct BgIdInfo
  {
    u32 bgNormalPaneId;            // 背景普通のペイン
    u32 bgQuasiLegendPaneId;       // 背景準伝説のペイン
    u32 bgLegendPaneId;            // 背景伝説のペイン
    u32 bgNormalInAnimId;          // 背景普通のインアニメ
    u32 bgQuasiLegendInAnimId;     // 背景準伝説のインアニメ
    u32 bgLegendInAnimId;          // 背景伝説のインアニメ
    u32 bgNormalLoopAnimId;        // 背景普通のループアニメ
    u32 bgQuasiLegendLoopAnimId;   // 背景準伝説のループアニメ
    u32 bgLegendLoopAnimId;        // 背景伝説のループアニメ
    u32 bgNormalOutAnimId;         // 背景普通のアウトアニメ
    u32 bgQuasiLegendOutAnimId;    // 背景準伝説のアウトアニメ
    u32 bgLegendOutAnimId;         // 背景伝説のアウトアニメ
  };
  class BgInfo
  { 
  public:
    enum
    {
      BG_CURTAIN,
      BG_BACKMOST,
      BG_NUM
    };
    gfl2::lyt::LytParts*  bgPane[BG_NUM][ZukanPicScrollDataAccessor::GRADE_VAL_NUM];        // 背景のペイン
    u32                   bgInAnimId[BG_NUM][ZukanPicScrollDataAccessor::GRADE_VAL_NUM];    // 背景のインアニメ
    u32                   bgLoopAnimId[BG_NUM][ZukanPicScrollDataAccessor::GRADE_VAL_NUM];  // 背景のループアニメ
    u32                   bgOutAnimId[BG_NUM][ZukanPicScrollDataAccessor::GRADE_VAL_NUM];   // 背景のアウトアニメ
    BgInfo(void)
    {
      for(s32 j=0; j<BG_NUM; ++j)
      {
        for(s32 i=0; i<ZukanPicScrollDataAccessor::GRADE_VAL_NUM; ++i)
        {
          bgPane[j][i]       = NULL;
          bgInAnimId[j][i]   = ANIM_ID_NULL;
          bgLoopAnimId[j][i] = ANIM_ID_NULL;
          bgOutAnimId[j][i]  = ANIM_ID_NULL;
        }
      }
    }
  };

  // 使っているアニメだけ列挙しておく
  enum SpotAnimId
  {
    SPOT_ANIM_ID_POKE_WAIT,        // ポケモンの待機アニメ
    SPOT_ANIM_ID_POKE_IN,          // ポケモンのインアニメ

    SPOT_ANIM_ID_NAME_PLATE_WAIT,          // 名前プレートのプレートの待機アニメ
    SPOT_ANIM_ID_NAME_NORMAL_SHINE,        // 名前プレートのキラキラ普通のアニメ
    SPOT_ANIM_ID_NAME_QUASI_LEGEND_SHINE,  //               キラキラ準伝説のアニメ
    SPOT_ANIM_ID_NAME_LEGEND_SHINE,        //               キラキラ伝説のアニメ

    SPOT_ANIM_ID_BALL_NORMAL_WAIT,               // 捕獲前モンボマークのモンボマーク普通の待機アニメ
    SPOT_ANIM_ID_BALL_QUASI_LEGEND_WAIT,         //                     モンボマーク準伝説の待機アニメ
    SPOT_ANIM_ID_BALL_LEGEND_WAIT,               //                     モンボマーク伝説の待機アニメ
    SPOT_ANIM_ID_BALL_NORMAL_CHANGE,             // 捕獲前モンボマークのモンボマーク普通のチェンジアニメ
    SPOT_ANIM_ID_BALL_QUASI_LEGEND_CHANGE,       //                     モンボマーク準伝説のチェンジアニメ
    SPOT_ANIM_ID_BALL_LEGEND_CHANGE,             //                     モンボマーク伝説のチェンジアニメ
    SPOT_ANIM_ID_BALL_EFFECT_QUASI_LEGEND_LOOP,  // 捕獲前モンボマークのエフェクト準伝説のループアニメ
    SPOT_ANIM_ID_BALL_EFFECT_LEGEND_LOOP,        //                     エフェクト伝説のループアニメ

    SPOT_ANIM_ID_NUM,
  };

  // ポケモン１体分のペインやアニメ１塊をスポットと呼ぶことにする
  class SpotInfo
  {
  public:
     // ポケモン
    gfl2::lyt::LytPane*    pokeTopPane;
    gfl2::lyt::LytParts*   pokePartsPane;
    gfl2::lyt::LytPicture* pokePicPane[4];  // 残像３つ＋黒１つ

    // 捕獲前モンボマーク
    gfl2::lyt::LytPane*    ballTopPane;
    gfl2::lyt::LytParts*   ballPartsPane;

    // 名前プレート
    gfl2::lyt::LytPane*    nameTopPane;
    gfl2::lyt::LytParts*   namePartsPane;
    gfl2::lyt::LytPane*    nameBallPane;      // 名前プレートのモンボマーク
    gfl2::lyt::LytTextBox* nameNoTextPane;    // 名前プレートの番号テキスト
    gfl2::lyt::LytTextBox* nameNameTextPane;  // 名前プレートの名前テキスト

    // アニメ
    u32 pokeWaitAnimId;
    u32 pokeInAnimId;
    u32 namePlateWaitAnimId;
    u32 nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];
    u32 ballWaitAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];
    u32 ballChangeAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];
    u32 ballEffectLoopAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];

  public:
    SpotInfo(void)
      : pokeTopPane(NULL),
        pokePartsPane(NULL),
        ballTopPane(NULL),
        ballPartsPane(NULL),
        nameTopPane(NULL),
        namePartsPane(NULL),
        nameBallPane(NULL),
        nameNoTextPane(NULL),
        nameNameTextPane(NULL),
        pokeWaitAnimId(ANIM_ID_NULL),
        pokeInAnimId(ANIM_ID_NULL),
        namePlateWaitAnimId(ANIM_ID_NULL)
    {
      for(u32 i=0; i<4; ++i)
      {
        pokePicPane[i] = NULL;
      } 
      for(s32 i=0; i<ZukanPicScrollDataAccessor::GRADE_VAL_NUM; ++i)
      {
        nameShineAnimId[i]      = ANIM_ID_NULL;
        ballWaitAnimId[i]       = ANIM_ID_NULL;
        ballChangeAnimId[i]     = ANIM_ID_NULL;
        ballEffectLoopAnimId[i] = ANIM_ID_NULL;
      }
    }
  };

  // 配置タイプ１つ分の情報をプレイスインフォと呼ぶことにする
  class PlaceInfo
  {
  public:
    SpotInfo              spotInfo[POKE_NUM_MAX];
  public:
    PlaceInfo(void)
    {}
  };

  // 絵巻の１レイアウトをシート(ページ、１画面)と呼ぶことにする
  class SheetInfo
  {
  public:
    PlaceInfo*            placeInfo;
    gfl2::lyt::LytParts*  placeTypeTopPane;
    gfl2::lyt::LytPane*   sheetTopPane;
  public:
    SheetInfo(void)
      : placeInfo(NULL),
        placeTypeTopPane(NULL),
        sheetTopPane(NULL)
    {}
  };

public:
  static void SetupBgInfo(
      BgInfo*           bgInfo,
      gfl2::lyt::LytWk* pCurtainLytWk,
      u32               curtainLytWkId,
      gfl2::lyt::LytWk* pBackmostLytWk,
      u32               backmostLytWkId
  );
  
  void SetupSheetInfo(SheetInfo* sheetInfo) const;  // PlaceInfoは外部で設定しておくこと。この関数内ではPlaceInfoのセットアップはしない。
  void SetupPlaceInfo(PlaceInfo* placeInfo, gfl2::lyt::LytParts* placeTypeTopPane) const;

private:
  static const u32 PANE_ID_NULL = 0xFFFFFFFF;  // u32で扱っているペインIDのNULL値

private:
  void setupPlaceInfoForGrp000Evo3(PlaceInfo* placeInfo, gfl2::lyt::LytParts* placeTypeTopPane) const;

  void internalSetupPlaceInfo(
      PlaceInfo* placeInfo,
      gfl2::lyt::LytParts* placeTypeTopPane,
      const u32  SPOT_NUM,
      const u32* POKE_TOP_PANE_ID,
      const u32* POKE_PARTS_PANE_ID,
      const u32* BALL_TOP_PANE_ID,
      const u32* BALL_PARTS_PANE_ID,
      const u32* NAME_TOP_PANE_ID,
      const u32* NAME_PARTS_PANE_ID,
      const u32* SPOT_ANIM_ID  // SPOT_ANIM_ID[SPOT_ANIM_ID_NUM][SPOT_NUM]
  ) const;

private:
  app::util::G2DUtil*                 m_pG2DUtil;
  gfl2::lyt::LytMultiResID            m_lytRes;
  gfl2::lyt::LytWk*                   m_pLytWk;
  u32                                 m_lytWkId;
  const ZukanPicScrollDataAccessor*   m_pPicScrollDataAccessor;
  const PokeTool::ExtendData*         m_pPokeExtendData;
  gfl2::str::MsgData*                 m_pMsgData;
  print::WordSet*                     m_pWordSet;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLUTIL_H_INCLUDED

