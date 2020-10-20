#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLDEMO_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLDEMO_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPicScrollDemo.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.02
 @brief   図鑑の絵巻デモ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanPicScrollDataAccessor.h"
#include "ZukanContents.h"


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
  class ZukanPokePicManager;
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  class ZukanPrismSheet;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPicScrollDemo
 @brief     図鑑の絵巻デモ
 */
//==============================================================================
class ZukanPicScrollDemo
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPicScrollDemo);

public:
  ZukanPicScrollDemo(
    app::util::G2DUtil*                 pG2DUtil,
    gfl2::lyt::LytMultiResID            lytRes,
    gfl2::lyt::LytWk*                   pLytWkBackmost,
    gfl2::lyt::LytWk*                   pLytWkBack,
    gfl2::lyt::LytWk*                   pLytWkCurtain,
    gfl2::lyt::LytWk*                   pLytWkMain,
    gfl2::lyt::LytWk*                   pLytWkFront,
    u32                                 lytWkIdBackmost,
    u32                                 lytWkIdBack,
    u32                                 lytWkIdCurtain,
    u32                                 lytWkIdMain,
    u32                                 lytWkIdFront,
    const ZukanPicScrollDataAccessor*   pPicScrollDataAccessor,
    const ZukanContents*                pContents,
    ZukanPokePicManager*                pPokePicManager, 
    const PokeTool::ExtendData*         pPokeExtendData,
    gfl2::str::MsgData*                 pMsgData,  // <niji_conv_header/message/msg_zukan.h>
    print::WordSet*                     pWordSet,
    bool*                               pRegisterIsBingo,     // [out]
    u32*                                pRegisterLegendLevel  // [out]
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    , ZukanPrismSheet*                  pPrismSheet
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  );
  virtual ~ZukanPicScrollDemo();

  void Update(void);
  
  void Setup(const PokeTool::SimpleParam& sp);
  bool IsReady(void) const;
  void Start(void);
  bool IsEnd(void) const;

private:
  enum State
  {
    STATE_WAIT_SETUP,
    STATE_WAIT_SETUP_FINISH,
    STATE_WAIT_START,
    STATE_START,
    STATE_WAIT_END,
    STATE_END
  };

  enum
  {
    POKE_NUM_MAX    = 9,           // 1画面に出るポケモン数の最大値
    PLACE_TYPE_NONE = 0xFFFF,      // PicScrollDataAccessor::PlaceType
  };
  static const u32 PANE_ID_NULL = 0xFFFFFFFF;  // u32で扱っているペインIDのNULL値
  static const u32 ANIM_ID_NULL = 0xFFFFFFFF;  // u32で扱っているアニメIDのNULL値

  // LytWkポインタ  // G2DUtilに渡すIDと勘違いされないようにポインタという名前にした。G2DUtilに渡すIDとしては使えない。
  enum LytWkPtr
  {
    LYT_WK_PTR_MAIN,      // ポケモン、捕獲前モンボマーク、名前プレート
    LYT_WK_PTR_BACK,      // ビンゴ背景
    LYT_WK_PTR_FRONT,     // 登録完了文字列
    LYT_WK_PTR_BACKMOST,  // 最背面
    LYT_WK_PTR_CURTAIN,   // カーテン背景
    LYT_WK_PTR_NUM,
  };

  enum PokeState
  {
    // 開始待ち
    POKE_STATE_WAIT_START,
    // 登録演出
    POKE_STATE_START,                        // 登録演出開始
    POKE_STATE_WAIT_ANIM_COUNT_0,            // 登録演出開始前の待ち
    POKE_STATE_WAIT_BG_IN_END,               // 背景の出現が完了するのを待つ
    POKE_STATE_WAIT_SHADOW_IN_END,           // 黒ポケモンの出現が完了するのを待つ
    POKE_STATE_WAIT_POKE_IN_END,             // ポケモンの出現が完了するのを待つ
    POKE_STATE_WAIT_NAME_PLATE_IN_END,       // 名前プレートのプレートの出現が完了するのを待つ
    POKE_STATE_WAIT_ANIM_COUNT_1,            // 登録完了表示開始前の待ち
    POKE_STATE_WAIT_REGISTERED_END,          // 登録完了表示が完了するのを待つ
    POKE_STATE_WAIT_ANIM_COUNT_2,            // ビンゴ演出or終了の前の待ち
    POKE_STATE_JUDGE,                        // ビンゴ演出or終了どちらになるか判定する
    // ビンゴ演出
    POKE_STATE_START_BINGO,                  // ビンゴ演出開始
    POKE_STATE_WAIT_NAME_BALL_IN_START,      // 名前プレートのモンボマークのインの開始を待つ
    POKE_STATE_WAIT_NAME_BALL_LOOP_START,    // 名前プレートのモンボマークのループの開始を待つ
    POKE_STATE_WAIT_BALL_FLARE_LOOP_START,   // 捕獲前モンボマークのフレアのループの開始を待つ
    POKE_STATE_WAIT_ANIM_COUNT_3,            // 終了の前の待ち
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    // キラカード判定
    POKE_STATE_PRISM,
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    // 終了
    POKE_STATE_END
  };

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
    BgInfo(void)
    {
      for(s32 j=0; j<BG_NUM; ++j)
      {
        for(s32 i=0; i<ZukanPicScrollDataAccessor::GRADE_VAL_NUM; ++i)
        {
          bgPane[j][i]       = NULL;
          bgInAnimId[j][i]   = ANIM_ID_NULL;
          bgLoopAnimId[j][i] = ANIM_ID_NULL;
        }
      }
    }
  };

  // 登録完了演出情報
  class TourokuKanryouInfo
  {
  public:
    u32 frontAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];  // 前面のアニメ
    u32 backAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];   // 背面のアニメ
    u32 frontOffAnimId;  // 前面のアニメをオフにする
    TourokuKanryouInfo(void)
    {
      for(s32 i=0; i<ZukanPicScrollDataAccessor::GRADE_VAL_NUM; ++i)
      {
        frontAnimId[i] = ANIM_ID_NULL;
        backAnimId[i]  = ANIM_ID_NULL;
        frontOffAnimId = ANIM_ID_NULL;
      }
    }
  };

  // 使っているアニメだけ列挙しておく
  enum SpotAnimId
  {
    SPOT_ANIM_ID_POKE_WAIT,                      // ポケモンの待機アニメ
    SPOT_ANIM_ID_POKE_IN,                        // ポケモンのインアニメ
    SPOT_ANIM_ID_POKE_REGISTER_NORMAL_IN,        // ポケモンの登録普通のインアニメ
    SPOT_ANIM_ID_POKE_REGISTER_QUASI_LEGEND_IN,  //           登録準伝説のインアニメ
    SPOT_ANIM_ID_POKE_REGISTER_LEGEND_IN,        //           登録伝説のインアニメ

    SPOT_ANIM_ID_NAME_BALL_IN,             // 名前プレートのモンボマークのインアニメ
    SPOT_ANIM_ID_NAME_BALL_LOOP,           //               モンボマークの回転ループアニメ
    SPOT_ANIM_ID_NAME_PLATE_WAIT,          // 名前プレートのプレートの待機アニメ
    SPOT_ANIM_ID_NAME_PLATE_IN,            //               プレートのインアニメ
    SPOT_ANIM_ID_NAME_NORMAL_SHINE,        // 名前プレートのキラキラ普通のアニメ
    SPOT_ANIM_ID_NAME_QUASI_LEGEND_SHINE,  //               キラキラ準伝説のアニメ
    SPOT_ANIM_ID_NAME_LEGEND_SHINE,        //               キラキラ伝説のアニメ

    SPOT_ANIM_ID_BALL_FLARE_IN,             // 捕獲前モンボマークのフレアのインアニメ
    SPOT_ANIM_ID_BALL_FLARE_LOOP,           //                             回転ループアニメ
    SPOT_ANIM_ID_BALL_NORMAL_WAIT,          // 捕獲前モンボマークのモンボマーク普通の待機アニメ
    SPOT_ANIM_ID_BALL_QUASI_LEGEND_WAIT,    //                     モンボマーク準伝説の待機アニメ
    SPOT_ANIM_ID_BALL_LEGEND_WAIT,          //                     モンボマーク伝説の待機アニメ
    SPOT_ANIM_ID_BALL_NORMAL_CHANGE,        // 捕獲前モンボマークのモンボマーク普通のチェンジアニメ
    SPOT_ANIM_ID_BALL_QUASI_LEGEND_CHANGE,  //                     モンボマーク準伝説のチェンジアニメ
    SPOT_ANIM_ID_BALL_LEGEND_CHANGE,        //                     モンボマーク伝説のチェンジアニメ
    
    SPOT_ANIM_ID_NUM,
  };

  class PokeInfo
  {
  public:
    u32                                  alolaNo;
    MonsNo                               monsNo;
    pml::FormNo                          formNo;
    pml::Sex                             sex;
    bool                                 isRare;
    bool                                 isSee;
    bool                                 isGet;
    ZukanPicScrollDataAccessor::GradeVal gradeVal;
    ZukanContents::PokeData              pokeData;
    u32                                  figureDataIndex;
    u8                                   placePos;
    PokeState                            pokeState;
    u32                                  animCount;
  public:
    PokeInfo(void)
      : alolaNo(0),
        monsNo(MONSNO_NULL),
        formNo(0),
        sex(pml::SEX_MALE),
        isRare(false),
        isSee(false),
        isGet(false),
        gradeVal(ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL),
        pokeData(),
        figureDataIndex(0),
        placePos(0),
        pokeState(POKE_STATE_START),
        animCount(0)
    {}
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
    u32 pokeRegisterInAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];
    u32 nameBallInAnimId;
    u32 nameBallLoopAnimId;
    u32 namePlateWaitAnimId;
    u32 namePlateInAnimId;
    u32 nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM]; 
    u32 ballFlareInAnimId; 
    u32 ballFlareLoopAnimId; 
    u32 ballWaitAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];
    u32 ballChangeAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NUM];

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
        nameBallInAnimId(ANIM_ID_NULL),
        nameBallLoopAnimId(ANIM_ID_NULL),
        namePlateWaitAnimId(ANIM_ID_NULL),
        namePlateInAnimId(ANIM_ID_NULL),
        ballFlareInAnimId(ANIM_ID_NULL),
        ballFlareLoopAnimId(ANIM_ID_NULL)
    {
      for(u32 i=0; i<4; ++i)
      {
        pokePicPane[i] = NULL;
      } 
      for(s32 i=0; i<ZukanPicScrollDataAccessor::GRADE_VAL_NUM; ++i)
      {
        pokeRegisterInAnimId[i] = ANIM_ID_NULL;
        nameShineAnimId[i]      = ANIM_ID_NULL;
        ballWaitAnimId[i]       = ANIM_ID_NULL;
        ballChangeAnimId[i]     = ANIM_ID_NULL;
      }
    }
  };

private:
  void initLayout(void);
  void initBg(void);
  
  void setupForGrp000Evo3(void);

  void internalSetup(
      const u32       SPOT_NUM,
      const u32*      POKE_TOP_PANE_ID,
      const u32*      POKE_PARTS_PANE_ID,
      const u32*      BALL_TOP_PANE_ID,
      const u32*      BALL_PARTS_PANE_ID,
      const u32*      NAME_TOP_PANE_ID,
      const u32*      NAME_PARTS_PANE_ID,
      const u32*      SPOT_ANIM_ID  // SPOT_ANIM_ID[SPOT_ANIM_ID_NUM][SPOT_NUM]
  );
 
  void disableUnusedObj(void);

  void setupPokeSpot(void);

  void updateForGrp000Evo3(void);

  void internalUpdate(void);

private:
  app::util::G2DUtil*                 m_pG2DUtil;
  gfl2::lyt::LytMultiResID            m_lytRes;
  gfl2::lyt::LytWk*                   m_pLytWk[LYT_WK_PTR_NUM];
  u32                                 m_lytWkId[LYT_WK_PTR_NUM];
  const ZukanPicScrollDataAccessor*   m_pPicScrollDataAccessor;
  const ZukanContents*                m_pContents;
  ZukanPokePicManager*                m_pPokePicManager;
  const PokeTool::ExtendData*         m_pPokeExtendData;
  gfl2::str::MsgData*                 m_pMsgData;
  print::WordSet*                     m_pWordSet;

  State      m_state;
  s32        m_placeType;  // PicScrollDataAccessor::PlaceType
  u32        m_pokeNum;
  SpotInfo   m_spotInfo[POKE_NUM_MAX];  // m_pokeInfoやm_currPokeIndexの影響を受けない固定のもの
  PokeInfo   m_pokeInfo[POKE_NUM_MAX];  // m_spotInfo[m_pokeInfo[i].placePos]がm_pokeInfo[i]のスポット
  u32        m_currPokeIndex;           // m_pokeInfo[m_currPokeIndex]が今捕まえたポケモン
  bool       m_isBingoStar;
  u32        m_bingoStarCount;

  gfl2::lyt::LytPane*   m_bingoBallAnimStartPosPane;  // ビンゴ先端ペイン
  gfl2::lyt::LytParts*  m_placeTypeTopPane;
  BgInfo                m_bgInfo;
  TourokuKanryouInfo    m_tourokuKanryouInfo;

  // 図鑑登録用一時記憶
  bool*  m_pRegisterIsBingo;
  u32*   m_pRegisterLegendLevel;

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  ZukanPrismSheet*                    m_pPrismSheet;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLDEMO_H_INCLUDED

