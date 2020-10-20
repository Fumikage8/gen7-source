//==============================================================================
/**
 @file    ZukanPicScrollDemo.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.02
 @brief   図鑑の絵巻デモ
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
#include "ZukanType.h"
#include "ZukanUtil.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanPokePicManager.h"
#include "ZukanPicScrollDataAccessor.h"

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
#include "ZukanPrismSheet.h"
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

#include "ZukanPicScrollDemo.h"




#define APP_ZUKAN_PROG_WAIT_ZERO (0)  // これが1のとき、プログラムで待ちをいれない。必ず0でコミット。


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanPicScrollDemo
//
//==============================================================================

ZukanPicScrollDemo::ZukanPicScrollDemo(
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
  gfl2::str::MsgData*                 pMsgData,
  print::WordSet*                     pWordSet,
  bool*                               pRegisterIsBingo,     // [out]
  u32*                                pRegisterLegendLevel  // [out]
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  , ZukanPrismSheet*                  pPrismSheet
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
)
  : m_pG2DUtil(pG2DUtil),
    m_lytRes(lytRes),
    m_pPicScrollDataAccessor(pPicScrollDataAccessor),
    m_pContents(pContents),
    m_pPokePicManager(pPokePicManager),
    m_pPokeExtendData(pPokeExtendData),
    m_pMsgData(pMsgData),
    m_pWordSet(pWordSet),
    m_state(STATE_WAIT_SETUP),
    m_placeType(PLACE_TYPE_NONE),
    m_pokeNum(0),
    m_currPokeIndex(0),
    m_isBingoStar(false),
    m_bingoStarCount(0),
    m_bingoBallAnimStartPosPane(NULL),
    m_bgInfo(),
    m_tourokuKanryouInfo(),
    m_pRegisterIsBingo(pRegisterIsBingo),
    m_pRegisterLegendLevel(pRegisterLegendLevel)
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    , m_pPrismSheet(pPrismSheet)
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
{
  m_pLytWk[LYT_WK_PTR_BACKMOST] = pLytWkBackmost;
  m_pLytWk[LYT_WK_PTR_BACK    ] = pLytWkBack    ;
  m_pLytWk[LYT_WK_PTR_CURTAIN ] = pLytWkCurtain ;
  m_pLytWk[LYT_WK_PTR_MAIN    ] = pLytWkMain    ;
  m_pLytWk[LYT_WK_PTR_FRONT   ] = pLytWkFront   ;

  m_lytWkId[LYT_WK_PTR_BACKMOST] = lytWkIdBackmost;
  m_lytWkId[LYT_WK_PTR_BACK    ] = lytWkIdBack    ;
  m_lytWkId[LYT_WK_PTR_CURTAIN ] = lytWkIdCurtain ;
  m_lytWkId[LYT_WK_PTR_MAIN    ] = lytWkIdMain    ;
  m_lytWkId[LYT_WK_PTR_FRONT   ] = lytWkIdFront   ;

  *m_pRegisterIsBingo     = false; 
  *m_pRegisterLegendLevel = 0;

  this->initLayout();
  this->initBg();
}
  
ZukanPicScrollDemo::~ZukanPicScrollDemo()
{}
 
void ZukanPicScrollDemo::Update(void)
{
  switch(m_state)
  {
  case STATE_WAIT_SETUP:
    break;
  case STATE_WAIT_SETUP_FINISH:
    {
      // 読み込みが完了しているか確認する
      bool isReady = true;
      for(u32 i=0; i<m_pokeNum; ++i)
      {
        if( !m_pPokePicManager->IsPokePicReady(&m_pokeInfo[i].pokeData, m_pokeInfo[i].figureDataIndex) )
        {
          isReady = false;
          break;
        }
      }
      // 読み込みが完了していたら開始待ち状態へ
      if(isReady)
      {
        m_state = STATE_WAIT_START;
      }
      break;
    }
  case STATE_WAIT_START:
    break;
  case STATE_START:
    {
      for(u32 i=0; i<m_pokeNum; ++i)
      {
        if( m_pokeInfo[i].pokeState == POKE_STATE_WAIT_START )
        {
          m_pokeInfo[i].pokeState = POKE_STATE_START;
        }
        else
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。既に開始しているのに再び開始を命令した。
        }
      }
      m_state = STATE_WAIT_END;
      break;
    }
  case STATE_WAIT_END:
    {
      this->updateForGrp000Evo3();
      break;
    }
  case STATE_END:
    break;
  }
}

void ZukanPicScrollDemo::Setup(const PokeTool::SimpleParam& sp)
{
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////
  //
  // どのポケモンを登録するか分かってはじめて行うことができる初期化
  //
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////

  GFL_ASSERT(m_state == STATE_WAIT_SETUP);  // 開発中に気付かせるためのASSERT。

  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  u32 alolaNo = m_pPokeExtendData->GetLocalNumber(sp.monsNo, PokeTool::ExtendData::LOCAL_AREA_A);
  if(alolaNo != PokeTool::ExtendData::LOCAL_NUMBER_NONE)
  {
    ZukanPicScrollDataAccessor::PokeData pokeData;
    m_pPicScrollDataAccessor->GetPokeData(alolaNo, &pokeData);

    m_placeType = pokeData.placeType;

    m_pokeNum = 0;
    u32 topAlolaNo = pokeData.placeGroupTopAlolaNo;
    for(u32 i=0; i<ZukanPicScrollDataAccessor::PLACE_POS_NUM[m_placeType]; ++i)
    {
      u32 groupAlolaNo = topAlolaNo + i;

      ZukanPicScrollDataAccessor::PokeData groupPokeData;
      m_pPicScrollDataAccessor->GetPokeData(groupAlolaNo, &groupPokeData);

      if(    groupPokeData.placeGroupTopAlolaNo == topAlolaNo
          && groupPokeData.placeType            == m_placeType )
      {
        if(m_pokeNum >= POKE_NUM_MAX)
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。1画面に出るポケモン数の最大値を越えてしまった。
          break;
        }
        
        m_pokeInfo[m_pokeNum].alolaNo   = groupAlolaNo;
      
        // @fix NMCat[2360] オドリドリのフォルム違いを捕獲した時、最初に見た別のフォルムが図鑑登録時に表示されてしまう
        //      →Savedata::ZukanData::GetDrawDataで得られるのは最初に見た姿の情報。ゲットしてもそれは(ユーザが図鑑で切り替えるまで)変更されない。
        //        よって、ゲットしたポケモンの姿を出すには、ゲットしたポケモンのSimpleParamから値を取らなければならない。
        if( groupAlolaNo == alolaNo )  // これから登録するポケモンは引数spから値を取る
        {
          m_pokeInfo[m_pokeNum].monsNo    = sp.monsNo;
          m_pokeInfo[m_pokeNum].sex       = sp.sex;
          m_pokeInfo[m_pokeNum].isRare    = sp.isRare;
          m_pokeInfo[m_pokeNum].formNo    = sp.formNo;
        }
        else  // それ以外のポケモンはセーブデータから値を取る
        {
          m_pokeInfo[m_pokeNum].monsNo    = static_cast<MonsNo>(m_pPokeExtendData->GetZenkokuNumber(m_pokeInfo[m_pokeNum].alolaNo, PokeTool::ExtendData::LOCAL_AREA_A));
          svZukanData->GetDrawData(m_pokeInfo[m_pokeNum].monsNo, &(m_pokeInfo[m_pokeNum].sex), &(m_pokeInfo[m_pokeNum].isRare), &(m_pokeInfo[m_pokeNum].formNo));
        }

        m_pokeInfo[m_pokeNum].isSee     = svZukanData->GetPokeSeeFlag(m_pokeInfo[m_pokeNum].monsNo);
        m_pokeInfo[m_pokeNum].isGet     = svZukanData->GetPokeGetFlag(m_pokeInfo[m_pokeNum].monsNo);
        m_pokeInfo[m_pokeNum].gradeVal  = groupPokeData.gradeVal;
        m_pContents->SetupPokeData(&m_pokeInfo[m_pokeNum].pokeData, &m_pokeInfo[m_pokeNum].figureDataIndex, m_pokeInfo[m_pokeNum].monsNo, m_pokeInfo[m_pokeNum].formNo, m_pokeInfo[m_pokeNum].sex, m_pokeInfo[m_pokeNum].isRare);
        m_pokeInfo[m_pokeNum].placePos  = groupPokeData.placePos;

        m_pokeInfo[m_pokeNum].pokeState = POKE_STATE_WAIT_START;
        m_pokeInfo[m_pokeNum].animCount = 0;

        {
          // これから登録するポケモンのgetフラグを立ててしまっているので、getフラグを立てる前の状態を取得する
              // @fix GFNMCat[891] 通信進化するポケモンを受け取った際の図鑑登録が正常に行われない
              //                   これから登録するポケモン以外もGetPokeRegisterShowFlagを使うことにした。
          bool seeFlag = false;
          bool getFlag = svZukanData->GetPokeRegisterShowFlag( m_pokeInfo[m_pokeNum].monsNo, &seeFlag );
          m_pokeInfo[m_pokeNum].isSee = seeFlag;
          m_pokeInfo[m_pokeNum].isGet = getFlag;

          if( groupAlolaNo == alolaNo )  // これから登録するポケモンだけの処理
          {
            // ここでセーブデータのZukanNotSaveDataに図鑑登録画面が呼ばれたことを伝えておく。
            // svZukanData->SetPokeRegisterShowを呼ぶのは、svZukanData->GetPokeRegisterShowFlagより後でなければうまくいかないので、ここにした。
            svZukanData->SetPokeRegisterShow( m_pokeInfo[m_pokeNum].monsNo );  // 図鑑登録画面を呼んだ(名前入力画面とかは関係ない)

            m_currPokeIndex = i;
            static const u32 REGISTER_LEGEND_LEVEL[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
            {
              ZukanType::REGISTER_LEGEND_LEVEL_COMMON      ,
              ZukanType::REGISTER_LEGEND_LEVEL_QUASI_LEGEND,
              ZukanType::REGISTER_LEGEND_LEVEL_LEGEND      ,
            };
            if(groupPokeData.ubFlag)
            {
              *m_pRegisterLegendLevel = ZukanType::REGISTER_LEGEND_LEVEL_UB;
            }
            else if(groupPokeData.tikiFlag)  // @fix NMCat[1107] タイプ：ヌル、シルヴァディの図鑑表示についての質問  →準伝説とティキを分けたので、ティキがUBのような扱いになった。
            {
              *m_pRegisterLegendLevel = ZukanType::REGISTER_LEGEND_LEVEL_TIKI;
            }
            else if(groupPokeData.maboroshiFlag)  // 幻  // @fix GFNMCat[3983] まぼろしポケモン用の評価コメントが表示されない  →まぼろしを伝説と分ける。
            {
              *m_pRegisterLegendLevel = ZukanType::REGISTER_LEGEND_LEVEL_MABOROSHI;
            }
            else
            {
              *m_pRegisterLegendLevel = REGISTER_LEGEND_LEVEL[m_pokeInfo[m_pokeNum].gradeVal];
            }
          }
        }

        ++m_pokeNum;
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。絵巻データがおかしい。
      }
    }

    m_state = STATE_WAIT_SETUP_FINISH;  // セットアップ完了待ちステートにしておく。
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。アローラのポケモンでない。
    m_placeType = PLACE_TYPE_NONE;
    m_pokeNum   = 0;
    m_currPokeIndex = 0;
    m_state     = STATE_END;  // 終了ステートにしておく。
  }

  // 配置タイプのセットアップ
  this->setupForGrp000Evo3();

  // 使わないものを消しておく
  this->disableUnusedObj();

  // ポケモンをスポットにあてはめる
  this->setupPokeSpot();
}

void ZukanPicScrollDemo::Start(void)
{
  if(m_state == STATE_WAIT_START)
  {
    m_state = STATE_START;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。既に開始しているのに再び開始を命令した。
  }
}

bool ZukanPicScrollDemo::IsReady(void) const
{
  return (m_state == STATE_WAIT_START);  
} 

bool ZukanPicScrollDemo::IsEnd(void) const
{
  return (m_state == STATE_END);
}

void ZukanPicScrollDemo::initLayout(void)
{
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////
  //
  // どのポケモンを登録するか分からなくても行うことができる初期化
  //
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////
  
  if(m_pLytWk[LYT_WK_PTR_BACK]) m_bingoBallAnimStartPosPane = m_pLytWk[LYT_WK_PTR_BACK]->GetPane(PANENAME_ZUKAN_MIN_UPP_001_PANE_BINGOBALLANIMSTARTPOS);

  m_placeTypeTopPane = m_pLytWk[LYT_WK_PTR_MAIN]->GetPartsPane(PANENAME_ZUKAN_MIN_UPP_000_PANE_GRP000_EVO3);

  // m_tourokuKanryouInfo
  {
    m_tourokuKanryouInfo.frontAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = LA_ALOLA_TOP_ZUKAN_MIN_UPP_002_COMPLETION;
    m_tourokuKanryouInfo.frontAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = LA_ALOLA_TOP_ZUKAN_MIN_UPP_002_COMPLETION_RARE;
    m_tourokuKanryouInfo.frontAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = LA_ALOLA_TOP_ZUKAN_MIN_UPP_002_COMPLETION_SR;
    m_tourokuKanryouInfo.backAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = LA_ALOLA_TOP_ZUKAN_MIN_UPP_003_N;
    m_tourokuKanryouInfo.backAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = LA_ALOLA_TOP_ZUKAN_MIN_UPP_003_RARE;
    m_tourokuKanryouInfo.backAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = LA_ALOLA_TOP_ZUKAN_MIN_UPP_003_SR;
    m_tourokuKanryouInfo.frontOffAnimId = LA_ALOLA_TOP_ZUKAN_MIN_UPP_002_OUT;
  }
}
void ZukanPicScrollDemo::initBg(void)
{
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////
  //
  // どのポケモンを登録するか分からなくても行うことができる初期化
  //
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////

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
    },
  };

  static const u32 BG_LYT_WK_PTR[BgInfo::BG_NUM] =
  {
    LYT_WK_PTR_CURTAIN,
    LYT_WK_PTR_BACKMOST,
  };

  //////////////////////////////////////////////
  // 背景
  //////////////////////////////////////////////
  for(s32 i=0; i<BgInfo::BG_NUM; ++i)
  {
    // ペインのアドレスを取得しておく
    m_bgInfo.bgPane[i][ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = m_pLytWk[BG_LYT_WK_PTR[i]]->GetPartsPane(BG_ID_INFO[i].bgNormalPaneId     );
    m_bgInfo.bgPane[i][ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = m_pLytWk[BG_LYT_WK_PTR[i]]->GetPartsPane(BG_ID_INFO[i].bgQuasiLegendPaneId);
    m_bgInfo.bgPane[i][ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = m_pLytWk[BG_LYT_WK_PTR[i]]->GetPartsPane(BG_ID_INFO[i].bgLegendPaneId     );
    // アニメを取得しておく
    m_bgInfo.bgInAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = BG_ID_INFO[i].bgNormalInAnimId     ;
    m_bgInfo.bgInAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = BG_ID_INFO[i].bgQuasiLegendInAnimId;
    m_bgInfo.bgInAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = BG_ID_INFO[i].bgLegendInAnimId     ;
    m_bgInfo.bgLoopAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = BG_ID_INFO[i].bgNormalLoopAnimId     ;
    m_bgInfo.bgLoopAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = BG_ID_INFO[i].bgQuasiLegendLoopAnimId;
    m_bgInfo.bgLoopAnimId[i][ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = BG_ID_INFO[i].bgLegendLoopAnimId     ;
  }
}


// SpotAnimName名前生成マクロ
#define SAN(parts,prefix,no,suffix) LA_ALOLA_TOP_ZUKAN_MIN_UPP_000__##parts##__##prefix##no##suffix
#define SAN9(parts,prefix,suffix) \
          {                               \
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


void ZukanPicScrollDemo::setupForGrp000Evo3(void)
{
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////
  //
  // Grp000_Evo3(Zukan_GRP_UPP_000.flyt)の初期化(定数列挙)
  //
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////

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
    SAN9(GRP000_EVO3,EVO3_POKEMON,_BASE)
    SAN9(GRP000_EVO3,EVO3_POKEMON,_RARE)
    SAN9(GRP000_EVO3,EVO3_POKEMON,_SR)

    SAN9(GRP000_EVO3,EVO3_NAME,_BALL_IN)
    SAN9(GRP000_EVO3,EVO3_NAME,_BALL_LOOP)
    SAN9(GRP000_EVO3,EVO3_NAME,_PLATE_WAIT)
    SAN9(GRP000_EVO3,EVO3_NAME,_PLATE_IN)
    SAN9(GRP000_EVO3,EVO3_NAME,_SHINE_N)
    SAN9(GRP000_EVO3,EVO3_NAME,_SHINE)
    SAN9(GRP000_EVO3,EVO3_NAME,_SHINE_SR)

    SAN9(GRP000_EVO3,EVO3_BALL,_FLARE)
    SAN9(GRP000_EVO3,EVO3_BALL,_FLARE_LOOP)
    SAN9(GRP000_EVO3,EVO3_BALL,_BASE_WAIT)
    SAN9(GRP000_EVO3,EVO3_BALL,_RARE_WAIT)
    SAN9(GRP000_EVO3,EVO3_BALL,_SR_WAIT)
    SAN9(GRP000_EVO3,EVO3_BALL,_BASE_CHANGE)
    SAN9(GRP000_EVO3,EVO3_BALL,_RARE_CHANGE)
    SAN9(GRP000_EVO3,EVO3_BALL,_SR_CHANGE)
  };

  this->internalSetup(
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

void ZukanPicScrollDemo::internalSetup(
    const u32       SPOT_NUM,
    const u32*      POKE_TOP_PANE_ID,
    const u32*      POKE_PARTS_PANE_ID,
    const u32*      BALL_TOP_PANE_ID,
    const u32*      BALL_PARTS_PANE_ID,
    const u32*      NAME_TOP_PANE_ID,
    const u32*      NAME_PARTS_PANE_ID,
    const u32*      SPOT_ANIM_ID
)
{
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////
  //
  // Grp000_Evo3(Zukan_GRP_UPP_000.flyt)の初期化(アドレス取得)
  //
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////

  //////////////////////////////////////////////
  // m_placeTypeの各スポット
  //////////////////////////////////////////////
  // ポケモンのペインのアドレスを取得しておく
  {
    for(u32 i=0; i<SPOT_NUM; ++i)
    {
      m_spotInfo[i].pokeTopPane   = m_pLytWk[LYT_WK_PTR_MAIN]->GetPane(m_placeTypeTopPane, POKE_TOP_PANE_ID[i], &m_lytRes);
      m_spotInfo[i].pokePartsPane = m_pLytWk[LYT_WK_PTR_MAIN]->GetPartsPane(m_placeTypeTopPane, POKE_PARTS_PANE_ID[i], &m_lytRes);
     
      // 残像３つ＋黒１つ
      m_spotInfo[i].pokePicPane[0] = m_pLytWk[LYT_WK_PTR_MAIN]->GetPicturePane(m_spotInfo[i].pokePartsPane, PANENAME_ZUKAN_ALA_UPP_001_PANE_00, &m_lytRes);
      m_spotInfo[i].pokePicPane[1] = m_pLytWk[LYT_WK_PTR_MAIN]->GetPicturePane(m_spotInfo[i].pokePartsPane, PANENAME_ZUKAN_ALA_UPP_001_PANE_01, &m_lytRes);
      m_spotInfo[i].pokePicPane[2] = m_pLytWk[LYT_WK_PTR_MAIN]->GetPicturePane(m_spotInfo[i].pokePartsPane, PANENAME_ZUKAN_ALA_UPP_001_PANE_02, &m_lytRes);
      m_spotInfo[i].pokePicPane[3] = m_pLytWk[LYT_WK_PTR_MAIN]->GetPicturePane(m_spotInfo[i].pokePartsPane, PANENAME_ZUKAN_ALA_UPP_001_PANE_03, &m_lytRes);
    }
  }

  // 捕獲前モンボマークのペインのアドレスを取得しておく
  {
    for(u32 i=0; i<SPOT_NUM; ++i)
    {
      m_spotInfo[i].ballTopPane   = m_pLytWk[LYT_WK_PTR_MAIN]->GetPane(m_placeTypeTopPane, BALL_TOP_PANE_ID[i], &m_lytRes);
      m_spotInfo[i].ballPartsPane = m_pLytWk[LYT_WK_PTR_MAIN]->GetPartsPane(m_placeTypeTopPane, BALL_PARTS_PANE_ID[i], &m_lytRes);
    }
  }

  // 名前プレートのペインのアドレスを取得しておく
  {
    for(u32 i=0; i<SPOT_NUM; ++i)
    {
      m_spotInfo[i].nameTopPane   = m_pLytWk[LYT_WK_PTR_MAIN]->GetPane(m_placeTypeTopPane, NAME_TOP_PANE_ID[i], &m_lytRes);
      m_spotInfo[i].namePartsPane = m_pLytWk[LYT_WK_PTR_MAIN]->GetPartsPane(m_placeTypeTopPane, NAME_PARTS_PANE_ID[i], &m_lytRes);
    
      m_spotInfo[i].nameBallPane     = m_pLytWk[LYT_WK_PTR_MAIN]->GetPane(m_spotInfo[i].namePartsPane, PANENAME_ZUKAN_ALA_UPP_002_PANE_BALL, &m_lytRes);
      m_spotInfo[i].nameNoTextPane   = m_pLytWk[LYT_WK_PTR_MAIN]->GetTextBoxPane(m_spotInfo[i].namePartsPane, PANENAME_ZUKAN_ALA_UPP_002_PANE_TEXTBOX_01, &m_lytRes);
      m_spotInfo[i].nameNameTextPane = m_pLytWk[LYT_WK_PTR_MAIN]->GetTextBoxPane(m_spotInfo[i].namePartsPane, PANENAME_ZUKAN_ALA_UPP_002_PANE_TEXTBOX_00, &m_lytRes);
    }
  }

  // アニメを取得しておく
  {
    for(u32 i=0; i<SPOT_NUM; ++i)
    {
      u32 j = 0;
      j = SPOT_ANIM_ID_POKE_WAIT                     ;  m_spotInfo[i].pokeWaitAnimId                                                            = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_POKE_IN                       ;  m_spotInfo[i].pokeInAnimId                                                              = SPOT_ANIM_ID[ SPOT_NUM * j + i ]; 
      j = SPOT_ANIM_ID_POKE_REGISTER_NORMAL_IN       ;  m_spotInfo[i].pokeRegisterInAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ]  = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_POKE_REGISTER_QUASI_LEGEND_IN ;  m_spotInfo[i].pokeRegisterInAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND]  = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_POKE_REGISTER_LEGEND_IN       ;  m_spotInfo[i].pokeRegisterInAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ]  = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_NAME_BALL_IN             ;  m_spotInfo[i].nameBallInAnimId                                                     = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_NAME_BALL_LOOP           ;  m_spotInfo[i].nameBallLoopAnimId                                                   = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_NAME_PLATE_WAIT          ;  m_spotInfo[i].namePlateWaitAnimId                                                  = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_NAME_PLATE_IN            ;  m_spotInfo[i].namePlateInAnimId                                                    = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_NAME_NORMAL_SHINE        ;  m_spotInfo[i].nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ]  = SPOT_ANIM_ID[ SPOT_NUM * j + i ]; 
      j = SPOT_ANIM_ID_NAME_QUASI_LEGEND_SHINE  ;  m_spotInfo[i].nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND]  = SPOT_ANIM_ID[ SPOT_NUM * j + i ]; 
      j = SPOT_ANIM_ID_NAME_LEGEND_SHINE        ;  m_spotInfo[i].nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ]  = SPOT_ANIM_ID[ SPOT_NUM * j + i ];  
      j = SPOT_ANIM_ID_BALL_FLARE_IN            ;  m_spotInfo[i].ballFlareInAnimId                                                    = SPOT_ANIM_ID[ SPOT_NUM * j + i ]; 
      j = SPOT_ANIM_ID_BALL_FLARE_LOOP          ;  m_spotInfo[i].ballFlareLoopAnimId                                                  = SPOT_ANIM_ID[ SPOT_NUM * j + i ]; 
      j = SPOT_ANIM_ID_BALL_NORMAL_WAIT         ;  m_spotInfo[i].ballWaitAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ]   = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_QUASI_LEGEND_WAIT   ;  m_spotInfo[i].ballWaitAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND]   = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_LEGEND_WAIT         ;  m_spotInfo[i].ballWaitAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ]   = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_NORMAL_CHANGE       ;  m_spotInfo[i].ballChangeAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL      ] = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_QUASI_LEGEND_CHANGE ;  m_spotInfo[i].ballChangeAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND] = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
      j = SPOT_ANIM_ID_BALL_LEGEND_CHANGE       ;  m_spotInfo[i].ballChangeAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND      ] = SPOT_ANIM_ID[ SPOT_NUM * j + i ];
    }
  }
}


// SpotAnimName名前生成マクロ
#undef SAN9
#undef SAN


void ZukanPicScrollDemo::disableUnusedObj(void)
{
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////
  //
  // 今回使わないものを消しておく
  //
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////

  for(u32 i=m_pokeNum; i<POKE_NUM_MAX; ++i)
  {
    if(m_spotInfo[i].pokeTopPane) m_spotInfo[i].pokeTopPane->SetVisible(false);
    if(m_spotInfo[i].ballTopPane) m_spotInfo[i].ballTopPane->SetVisible(false);
    if(m_spotInfo[i].nameTopPane) m_spotInfo[i].nameTopPane->SetVisible(false);
  }

  for(s32 j=0; j<BgInfo::BG_NUM; ++j)
  {
    for(s32 i=0; i<ZukanPicScrollDataAccessor::GRADE_VAL_NUM; ++i)
    {
      if(m_pokeInfo[m_currPokeIndex].gradeVal != i)
      {
        m_bgInfo.bgPane[j][i]->SetVisible(false);
      }
    }
  }
}


void ZukanPicScrollDemo::setupPokeSpot(void)
{
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////
  //
  // 今回使うスポットにポケモンをあてはめる
  //
  ////////////////////////////////////////////////
  ////////////////////////////////////////////////

  //////////////////////////////////////////////
  // 位置や表示内容を設定する
  //////////////////////////////////////////////
  m_pG2DUtil->SetMessageData( m_pMsgData, m_pWordSet );  // <niji_conv_header/message/msg_zukan.h>
  
  for(u32 i=0; i<m_pokeNum; ++i)
  {
    SpotInfo* pSpotInfo = &(m_spotInfo[ m_pokeInfo[i].placePos ]);
  
    // 位置
    ZukanPicScrollDataAccessor::TransData transData;
    m_pPicScrollDataAccessor->GetPokeTransData(m_pokeInfo[i].alolaNo, m_pokeInfo[i].formNo, m_pokeInfo[i].sex, &transData);

    gfl2::math::VEC2 pokePos(transData.pokeX, transData.pokeY);
    gfl2::math::VEC2 ballPos(transData.ballX, transData.ballY);
    gfl2::math::VEC2 namePos(transData.nameX, transData.nameY);
    pSpotInfo->pokeTopPane->SetTranslate(pokePos);
    pSpotInfo->ballTopPane->SetTranslate(ballPos);
    pSpotInfo->nameTopPane->SetTranslate(namePos);
 
    // アローラ番号
    m_pG2DUtil->SetTextBoxPaneNumber(pSpotInfo->nameNoTextPane, msg_zkn_03_01, m_pokeInfo[i].alolaNo, 3, 0, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU);
    // ポケモン種族名
    m_pG2DUtil->SetRegisterMonsNameNo(0, m_pokeInfo[i].monsNo);
    m_pG2DUtil->SetTextBoxPaneStringExpand(pSpotInfo->nameNameTextPane, msg_zkn_03_03);  // 見ていないときはネームプレートは表示されないので、??????を代入する必要はない。
    // 名前プレート演出
    m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->nameShineAnimId[ m_pokeInfo[i].gradeVal ] );

    // ポケモンのテクスチャを設定する
    // 残像３つ＋黒１つ
    m_pPokePicManager->SetupPicPane(pSpotInfo->pokePicPane[0], &m_pokeInfo[i].pokeData, m_pokeInfo[i].figureDataIndex); 
    m_pPokePicManager->SetupPicPane(pSpotInfo->pokePicPane[1], &m_pokeInfo[i].pokeData, m_pokeInfo[i].figureDataIndex); 
    m_pPokePicManager->SetupPicPane(pSpotInfo->pokePicPane[2], &m_pokeInfo[i].pokeData, m_pokeInfo[i].figureDataIndex); 
    m_pPokePicManager->SetupPicPane(pSpotInfo->pokePicPane[3], &m_pokeInfo[i].pokeData, m_pokeInfo[i].figureDataIndex); 
  }
 
  m_pG2DUtil->SetMessageData(NULL, NULL);


  //////////////////////////////////////////////
  // 今回の状態にあわせた初期設定
  //////////////////////////////////////////////
  for(u32 i=0; i<m_pokeNum; ++i)
  {
    SpotInfo* pSpotInfo = &(m_spotInfo[ m_pokeInfo[i].placePos ]);

    //////// トップノードの表示非表示 ////////
    // ポケモン(今捕まえたor前から居た)
    pSpotInfo->pokeTopPane->SetVisible( i == m_currPokeIndex || m_pokeInfo[i].isSee );
    // 名前プレート(今捕まえたor前から居た)
    pSpotInfo->nameTopPane->SetVisible( i == m_currPokeIndex || m_pokeInfo[i].isSee );
    // 捕獲前モンボマーク(今捕まえたorまだ居ない)
    pSpotInfo->ballTopPane->SetVisible( true );  // pSpotInfo->ballTopPane->SetVisible( i == m_currPokeIndex || !(m_pokeInfo[i].isSee) );  // アニメを再生しなかったら、表示はされないので、Visibleはtrueのままでいい。
        // ビンゴのフレアが捕獲前モンボマークの子になっているので、捕獲前モンボマークのVisibleはtrueにしておかなければならない。

    //////// アニメで初期状態をつくる ////////
    // アニメを再生しなかったら、表示はされない。
    
    if( i == m_currPokeIndex || m_pokeInfo[i].isSee )
    {
      // 後半 or 最初から
      // ポケモンを表示するので、色を暗くしておく必要があればやっておく
      
      // 見ていない or 見ただけで捕まえていない
      // そういう場合は、色を暗くしておく
      if( !m_pokeInfo[i].isGet )
      {
        nw::ut::Color8 vtxColor = ZukanType::POKE_SEE_COLOR;
        for(s32 vtxIdx=0; vtxIdx<nw::lyt::VERTEXCOLOR_MAX; ++vtxIdx)
        {
          // 残像３つ＋黒１つのうち、黒１つだけ色を暗くする
          pSpotInfo->pokePicPane[3]->SetVtxColor(vtxIdx, vtxColor);
        }
      }
    }
    
    if( m_pokeInfo[i].isSee )
    {
      // ポケモン表示
      m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->pokeWaitAnimId );
      
      if( i != m_currPokeIndex )
      {
        // 名前プレート表示
        m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->namePlateWaitAnimId );
      }
    }
    else
    {
      // 捕獲前モンボマーク表示
      m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->ballWaitAnimId[m_pokeInfo[i].gradeVal] );
    }
  }
}

void ZukanPicScrollDemo::updateForGrp000Evo3(void)
{
  this->internalUpdate();
}

void ZukanPicScrollDemo::internalUpdate(void)
{
  static const u32 SE_BG_IN[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
    SEQ_SE_SYS_NJ_065,  // 登録演出の背景が出る　ノーマル
    SEQ_SE_SYS_NJ_066,  // 登録演出の背景が出る　準伝
    SEQ_SE_SYS_NJ_067,  // 登録演出の背景が出る　伝説
  };
  static const u32 SE_POKE_IN[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
    SEQ_SE_ZUKAN_BALLSPIN,  // ポケモン2D絵が出てくる（ノーマル、準伝、伝説）
    SEQ_SE_ZUKAN_BALLSPIN,  // ポケモン2D絵が出てくる（ノーマル、準伝、伝説）
    SEQ_SE_ZUKAN_BALLSPIN,  // ポケモン2D絵が出てくる（ノーマル、準伝、伝説）
  };
  static const u32 SE_POKE_PUT[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
    SEQ_SE_ZUKAN_POKEFLASH,    // ポケモン2D絵がバン！と張り付く（ノーマル）
    SEQ_SE_ZUKAN_NAME,         // ポケモン2D絵がバン！と張り付く（準伝）
    SEQ_SE_ZUKAN_NAME_LEGEND,  // ポケモン2D絵がバン！と張り付く（伝説）
  };
  static const u32 SE_POKE_PUT_FRAME[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
     7,
     8,
    10,
  };
  //static const u32 SE_NAME_PLATE_IN[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  //{
    //なし,  // ポケモン名表示(ノーマル)
    //なし,  // ポケモン名表示(準伝)
    //なし,  // ポケモン名表示(伝説)
  //};
  static const u32 SE_TOUROKU_KANRYOU_CROSS[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
    SEQ_SE_SYS_NJ_030,  // なし
    SEQ_SE_SYS_NJ_030,  // とうろくかんりょう！が画面を横切る（準伝、伝説）
    SEQ_SE_SYS_NJ_030,  // とうろくかんりょう！が画面を横切る（準伝、伝説）
  };
  static const u32 SE_TOUROKU_KANRYOU_FLY[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
    SEQ_SE_ZUKAN_SLIDEIN,         // とうろくかんりょう！が飛んでくる（ノーマル）
    SEQ_SE_ZUKAN_SLIDEIN_LEGEND,  // とうろくかんりょう！が飛んでくる（準伝、伝説）
    SEQ_SE_ZUKAN_SLIDEIN_LEGEND,  // とうろくかんりょう！が飛んでくる（準伝、伝説）
  };
  static const u32 SE_TOUROKU_KANRYOU_PUT[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
    SEQ_SE_ZUKAN_TOUROKU,         // とうろくかんりょう！が張り付く（ノーマル）
    SEQ_SE_ZUKAN_TOUROKU_LEGEND,  // とうろくかんりょう！が張り付く（準伝）
    SEQ_SE_ZUKAN_BINGO_STAR,      // とうろくかんりょう！が張り付く（伝説）
  };
  static const u32 SE_TOUROKU_KANRYOU_FLY_FRAME[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
     1,
    41,
    41,
  };
  static const u32 SE_TOUROKU_KANRYOU_PUT_FRAME[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
  {
    16,
    56,
    56,
  };


  u32 judgeNum = 0;
  u32 endNum   = 0;
  for(u32 i=0; i<m_pokeNum; ++i)
  {
    SpotInfo* pSpotInfo = &(m_spotInfo[ m_pokeInfo[i].placePos ]);

    switch( m_pokeInfo[i].pokeState )
    {
    case POKE_STATE_WAIT_START:
      {
        break;
      }
    case POKE_STATE_START:
      {
        // 待ち
        m_pokeInfo[i].animCount = 0;
        m_pokeInfo[i].pokeState = POKE_STATE_WAIT_ANIM_COUNT_0;
        break;
      }
    case POKE_STATE_WAIT_ANIM_COUNT_0:
      {
        if( i== m_currPokeIndex )
        {
          ++m_pokeInfo[i].animCount;
#if APP_ZUKAN_PROG_WAIT_ZERO
          if(m_pokeInfo[i].animCount >= 0)
#else
          if(m_pokeInfo[i].animCount >= 10)
#endif
          {
            // 背景を出現させる
            m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_CURTAIN ], m_bgInfo.bgInAnimId[BgInfo::BG_CURTAIN ][m_pokeInfo[m_currPokeIndex].gradeVal] );
            m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_BACKMOST], m_bgInfo.bgInAnimId[BgInfo::BG_BACKMOST][m_pokeInfo[m_currPokeIndex].gradeVal] );
            Sound::PlaySE(SE_BG_IN[m_pokeInfo[m_currPokeIndex].gradeVal]);
            m_pokeInfo[i].pokeState = POKE_STATE_WAIT_BG_IN_END;
          }
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_BG_IN_END;
        }
        break;
      }
    case POKE_STATE_WAIT_BG_IN_END:
      {
        if( i== m_currPokeIndex )
        {
          if( 
                 m_pG2DUtil->IsAnimeEnd( m_lytWkId[LYT_WK_PTR_CURTAIN ], m_bgInfo.bgInAnimId[BgInfo::BG_CURTAIN ][m_pokeInfo[m_currPokeIndex].gradeVal] )
              && m_pG2DUtil->IsAnimeEnd( m_lytWkId[LYT_WK_PTR_BACKMOST], m_bgInfo.bgInAnimId[BgInfo::BG_BACKMOST][m_pokeInfo[m_currPokeIndex].gradeVal] )
          )  // BG_CURTAINとBG_BACKMOSTのインアニメの尺は同じはず
          {
            // 背景をループさせる
            m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_CURTAIN ], m_bgInfo.bgLoopAnimId[BgInfo::BG_CURTAIN ][m_pokeInfo[m_currPokeIndex].gradeVal] );
            m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_BACKMOST], m_bgInfo.bgLoopAnimId[BgInfo::BG_BACKMOST][m_pokeInfo[m_currPokeIndex].gradeVal] );
            if( m_pokeInfo[i].isSee )
            {
              // ポケモンを出現させる
              m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->pokeRegisterInAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] );
              Sound::PlaySE(SE_POKE_IN[m_pokeInfo[m_currPokeIndex].gradeVal]);
              m_pokeInfo[i].animCount = 0;
              m_pokeInfo[i].pokeState = POKE_STATE_WAIT_POKE_IN_END;
            }
            else
            {
              // 捕獲前モンボマークを消していく
              m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->ballChangeAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] );
              // ポケモンのシルエットを出現させる
              m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->pokeInAnimId );
              m_pokeInfo[i].pokeState = POKE_STATE_WAIT_SHADOW_IN_END;
            }
          }
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_SHADOW_IN_END;
        }
        break;
      }
    case POKE_STATE_WAIT_SHADOW_IN_END:
      {
        if( i== m_currPokeIndex )
        {
          if(    m_pG2DUtil->IsAnimeEnd( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->ballChangeAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] )
              && m_pG2DUtil->IsAnimeEnd( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->pokeInAnimId ) )
          {
            // ポケモンを出現させる
            m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->pokeRegisterInAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] );
            Sound::PlaySE(SE_POKE_IN[m_pokeInfo[m_currPokeIndex].gradeVal]);
            m_pokeInfo[i].animCount = 0;
            m_pokeInfo[i].pokeState = POKE_STATE_WAIT_POKE_IN_END;
          }
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_POKE_IN_END;
        }
        break;
      }
    case POKE_STATE_WAIT_POKE_IN_END:
      {
        if( i== m_currPokeIndex )
        {
          ++m_pokeInfo[i].animCount;
          if(m_pokeInfo[i].animCount == SE_POKE_PUT_FRAME[m_pokeInfo[m_currPokeIndex].gradeVal])
          {
            // ポケモンが張り付く
            Sound::PlaySE(SE_POKE_PUT[m_pokeInfo[m_currPokeIndex].gradeVal]);
          }
          if( m_pG2DUtil->IsAnimeEnd( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->pokeRegisterInAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] ) )
          {
            // 名前プレートのプレートを出現させる
            m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->namePlateInAnimId );
            //Sound::PlaySE(SE_NAME_PLATE_IN[m_pokeInfo[m_currPokeIndex].gradeVal]);→SEはなくなりました
            m_pokeInfo[i].pokeState = POKE_STATE_WAIT_NAME_PLATE_IN_END;
          }
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_NAME_PLATE_IN_END;
        }
        break;
      }
    case POKE_STATE_WAIT_NAME_PLATE_IN_END:
      {
        if( i== m_currPokeIndex )
        {
          if( m_pG2DUtil->IsAnimeEnd( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->namePlateInAnimId ) )
          {
            // 待ち
            m_pokeInfo[i].animCount = 0;
            m_pokeInfo[i].pokeState = POKE_STATE_WAIT_ANIM_COUNT_1;
          }
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_ANIM_COUNT_1;
        }
        break;
      }
    case POKE_STATE_WAIT_ANIM_COUNT_1:
      {
        if( i== m_currPokeIndex )
        {
          ++m_pokeInfo[i].animCount;
#if APP_ZUKAN_PROG_WAIT_ZERO
          if(m_pokeInfo[i].animCount >= 0)
#else
          if(m_pokeInfo[i].animCount >= 30)
#endif
          {
            // 登録完了を出現させる
            m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_FRONT], m_tourokuKanryouInfo.frontAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] );
            m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_CURTAIN], m_tourokuKanryouInfo.backAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] );
            if(    m_pokeInfo[m_currPokeIndex].gradeVal == ZukanPicScrollDataAccessor::GRADE_VAL_QUASI_LEGEND
                || m_pokeInfo[m_currPokeIndex].gradeVal == ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND       )
            {
              Sound::PlaySE( SE_TOUROKU_KANRYOU_CROSS[m_pokeInfo[m_currPokeIndex].gradeVal] );
            } 
            m_pokeInfo[i].animCount = 0;
            m_pokeInfo[i].pokeState = POKE_STATE_WAIT_REGISTERED_END;
          }
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_REGISTERED_END;
        }
        break;
      }
    case POKE_STATE_WAIT_REGISTERED_END:
      {
        if( i== m_currPokeIndex )
        {
          ++m_pokeInfo[i].animCount;
          if(m_pokeInfo[i].animCount == SE_TOUROKU_KANRYOU_FLY_FRAME[m_pokeInfo[m_currPokeIndex].gradeVal])
          {
            Sound::PlaySE( SE_TOUROKU_KANRYOU_FLY[m_pokeInfo[m_currPokeIndex].gradeVal] );
          }
          else if(m_pokeInfo[i].animCount == SE_TOUROKU_KANRYOU_PUT_FRAME[m_pokeInfo[m_currPokeIndex].gradeVal])
          {
            Sound::PlaySE( SE_TOUROKU_KANRYOU_PUT[m_pokeInfo[m_currPokeIndex].gradeVal] );
          }

          if( 
                 m_pG2DUtil->IsAnimeEnd( m_lytWkId[LYT_WK_PTR_FRONT], m_tourokuKanryouInfo.frontAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] )
              && m_pG2DUtil->IsAnimeEnd( m_lytWkId[LYT_WK_PTR_CURTAIN], m_tourokuKanryouInfo.backAnimId[m_pokeInfo[m_currPokeIndex].gradeVal] )
          )
          {
            // 待ち
            m_pokeInfo[i].animCount = 0;
            m_pokeInfo[i].pokeState = POKE_STATE_WAIT_ANIM_COUNT_2;
          }
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_ANIM_COUNT_2;
        }
        break;
      }
    case POKE_STATE_WAIT_ANIM_COUNT_2:
      {
        if( i== m_currPokeIndex )
        {
          ++m_pokeInfo[i].animCount;
#if APP_ZUKAN_PROG_WAIT_ZERO
          if(m_pokeInfo[i].animCount >= 0)
#else
          if(m_pokeInfo[i].animCount >= 30)
#endif
          {
            m_pokeInfo[i].pokeState = POKE_STATE_JUDGE;
          }
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_JUDGE;
        }
        break;
      }
    case POKE_STATE_JUDGE:
      {
        ++judgeNum;
        break;
      }
    case POKE_STATE_START_BINGO:
      {
        if( i== m_currPokeIndex )
        {
          // 登録完了を消していく
          m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_FRONT], m_tourokuKanryouInfo.frontOffAnimId );
          // ビンゴのバーを出現させる
          m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_BACK], LA_ALOLA_TOP_ZUKAN_MIN_UPP_001_BINGO );
          Sound::PlaySE( SEQ_SE_ZUKAN_BINGO_REDLINE );  // ビンゴ！レッドライン表示
          m_isBingoStar = true;
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_NAME_BALL_IN_START;
        }
        else
        {
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_NAME_BALL_IN_START;
        }
        break;
      }
    case POKE_STATE_WAIT_NAME_BALL_IN_START:
      {
        gfl2::math::Vector3 bingoBallAnimStartPos(0,0,0);
        if(m_bingoBallAnimStartPosPane) bingoBallAnimStartPos = ZukanUtil::GetPaneWorldPosIgnoringSR(m_bingoBallAnimStartPosPane);
        gfl2::math::Vector3 nameBallPos           = ZukanUtil::GetPaneWorldPosIgnoringSR(pSpotInfo->nameBallPane);

        gfl2::math::Vector2 bingoBeltDir(1.0f, 1.0f);
        gfl2::math::Vector2 nameBallDir(nameBallPos.x-bingoBallAnimStartPos.x, nameBallPos.y-bingoBallAnimStartPos.y);
        f32 bingoInnerProduct = bingoBeltDir.x*nameBallDir.x + bingoBeltDir.y*nameBallDir.y;
        
        //if(nameBallPos.x <= bingoBallAnimStartPos.x)
        if(bingoInnerProduct <= 0.0f)  // ビンゴの帯が斜めに移動するようになったので、移動方向に垂直な線を過ぎたら回すようにした。
        {
          // 名前プレートのモンボマークのイン
          m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->nameBallInAnimId );
          //Sound::PlaySE(なし);  // モンスターボール回転（１つごとに１つ再生）→SEなくなりました
          // 捕獲前モンボマークのフレアのイン
          m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->ballFlareInAnimId );
          //Sound::PlaySE(なし);  // ポケモンに光彩重なる（１体ごとに１つ再生）→SEなくなりました
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_NAME_BALL_LOOP_START;
        }
        break;
      }
    case POKE_STATE_WAIT_NAME_BALL_LOOP_START:
      {
        if( m_pG2DUtil->GetAnimeFrame( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->nameBallInAnimId )                    // IsAnimeEndで判定すると付け替えが1フレーム遅れるので
                >= m_pG2DUtil->GetAnimeMaxFrame( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->nameBallInAnimId ) -1.0f )  // このような判定にした。
        {
          m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->nameBallLoopAnimId );
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_BALL_FLARE_LOOP_START;
        }
        break;
      }
    case POKE_STATE_WAIT_BALL_FLARE_LOOP_START:  // BALL_INよりFLAREのほうが長いからこれでOK
      {
        if( m_pG2DUtil->GetAnimeFrame( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->ballFlareInAnimId )
                >= m_pG2DUtil->GetAnimeMaxFrame( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->ballFlareInAnimId ) -1.0f )
        {
          m_pG2DUtil->StartAnime( m_lytWkId[LYT_WK_PTR_MAIN], pSpotInfo->ballFlareLoopAnimId );
          m_pokeInfo[i].pokeState = POKE_STATE_WAIT_ANIM_COUNT_3;
        }
        break;
      }
    case POKE_STATE_WAIT_ANIM_COUNT_3:
      {
        // ここでは( i== m_currPokeIndex )だけでなく皆カウントする。
        ++m_pokeInfo[i].animCount;
#if APP_ZUKAN_PROG_WAIT_ZERO
        if(m_pokeInfo[i].animCount >= 0)
#else
        if(m_pokeInfo[i].animCount >= 30)
#endif
        {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
          m_pokeInfo[i].pokeState = POKE_STATE_PRISM;
#else  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
          m_pokeInfo[i].pokeState = POKE_STATE_END;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
        }
        break;
      }
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    case POKE_STATE_PRISM:
      {
        if( i== m_currPokeIndex )
        {
          if(m_pokeInfo[m_currPokeIndex].gradeVal == ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND)
          {
            if( *m_pRegisterIsBingo || m_pokeNum == 1 )  // レジェンドコンプ(1体でコンプなら1体でもOK)ならキラカード化
            {
              m_pPrismSheet->Req(ZukanPrismSheet::REQ_ID_PRISM_ON);
            }
          }
        }
        m_pokeInfo[i].pokeState = POKE_STATE_END;
        //breakしない
      }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    case POKE_STATE_END:
      {
        ++endNum;
        break;
      }
    }
  }

  // 個別対応
  if(m_isBingoStar)
  {
    ++m_bingoStarCount;
    if(m_bingoStarCount >= 24)
    {
      //Sound::PlaySE(なし);  // ビンゴ！★柄が左右から出てくる→SEなくなりました
      m_isBingoStar = false;
    }
  }

  // 皆揃って状態変更
  if(m_pokeNum == judgeNum)
  {
    bool isBingo = true;
    if(m_pokeNum == 1)
    {
      isBingo = false;  // 1匹で完成する画面はビンゴにしない
    }
    else
    {
      for(u32 i=0; i<m_pokeNum; ++i)
      {
        if( !m_pokeInfo[i].isGet ) 
        {
          if( i!= m_currPokeIndex )
          {
            isBingo = false;
          }
        }
      }
    }

    if(isBingo)
    {
      *m_pRegisterIsBingo = true;
      for(u32 i=0; i<m_pokeNum; ++i)
      {
        m_pokeInfo[i].pokeState = POKE_STATE_START_BINGO;
      }
    }
    else
    {
      for(u32 i=0; i<m_pokeNum; ++i)
      {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
        m_pokeInfo[i].pokeState = POKE_STATE_PRISM;
#else  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
        m_pokeInfo[i].pokeState = POKE_STATE_END;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
      }
    }
  }

  // 皆揃って終了
  if(m_pokeNum == endNum)
  {
    m_state = STATE_END;
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

