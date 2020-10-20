//==============================================================================
/**
 @file    ZukanTool.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.25
 @brief   図鑑で使うツール
 */
//==============================================================================


// gfl2のインクルード
#include <fs/include/gfl2_fs.h>
#include <Layout/include/gfl2_Layout.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <str/include/gfl2_MsgData.h>
#include <str/include/gfl2_StrBuf.h>

// poke_3d_modelのインクルード
#include <model/include/gfl2_BaseCamera.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>

#include  <arc_index/MenuCursor.gaix>
#include  <arc_index/MenuWindow2.gaix>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ZukanSave.h>
#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeToolExtendData.h>
#include <PokeTool/include/PokeToolPersonalSort.h>
#include <Print/include/WordSetLoader.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h>
#include <AppLib/include/Tool/app_tool_2ButtonsView.h>
#include <AppLib/include/Tool/GeneralView/GeneralLowerView.h>
#include <AppLib/include/Tool/TalkWindow.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>

// 図鑑のインクルード
#include "ZukanDataManager.h"
#include "ZukanTool.h"

#include "ZukanDebugWin.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


// インフレームの位置とスケールの調整値
struct InFrameUserValue
{
  s16                 monsNo;
  s16                 formNo;
  gfl2::math::Vector3 posOffset;
  gfl2::math::Vector3 scale;
};

static const InFrameUserValue INFRAME_USER_VALUE_TABLE[] =
{
  { MONSNO_RAITYUU   , 0, gfl2::math::Vector3(0.0f, 36.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  //  26 ライチュウ
  { MONSNO_MORUFON   , 0, gfl2::math::Vector3(0.0f, 21.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  //  49 モルフォン
  { MONSNO_BETOBETON , 0, gfl2::math::Vector3(0.0f, 13.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  //  89 ベトベトン
  { MONSNO_KENTAROSU , 0, gfl2::math::Vector3(0.0f, 28.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 128 ケンタロス
  { MONSNO_BUUSUTAA  , 0, gfl2::math::Vector3(0.0f, 26.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 136 ブースター
  { MONSNO_HURIIZAA  , 0, gfl2::math::Vector3(0.0f, 29.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 144 フリーザー
  { MONSNO_ARIADOSU  , 0, gfl2::math::Vector3(0.0f, 21.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 168 アリアドス
  { MONSNO_MARIRURI  , 0, gfl2::math::Vector3(0.0f, 14.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 184 マリルリ
  { MONSNO_WATAKKO   , 0, gfl2::math::Vector3(0.0f, 14.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 189 ワタッコ
  { MONSNO_DOKUKEIRU , 0, gfl2::math::Vector3(0.0f, 17.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 269 ドクケイル
  { MONSNO_RAIBORUTO , 1, gfl2::math::Vector3(0.0f, 38.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 310 ライボルト    メガ
  { MONSNO_KOOTASU   , 0, gfl2::math::Vector3(0.0f, 19.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 324 コータス
  { MONSNO_ZYUPETTA  , 1, gfl2::math::Vector3(0.0f, 25.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 354 ジュペッタ    メガ
  { MONSNO_REKKUUZA  , 0, gfl2::math::Vector3(0.0f, 18.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 384 レックウザ
  { MONSNO_DODAITOSU , 0, gfl2::math::Vector3(0.0f, 32.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 389 ドダイトス
  { MONSNO_MOUKAZARU , 0, gfl2::math::Vector3(0.0f, 24.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 391 モウカザル
  { MONSNO_GOUKAZARU , 0, gfl2::math::Vector3(0.0f, 38.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 392 ゴウカザル
  { MONSNO_GAAMEIRU  , 0, gfl2::math::Vector3(0.0f, 24.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 414 ガーメイル
  { MONSNO_PATIRISU  , 0, gfl2::math::Vector3(0.0f, 21.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 417 パチリス
  { MONSNO_ETEBOOSU  , 0, gfl2::math::Vector3(0.0f, 12.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 424 エテボース
  { MONSNO_MIMIRORU  , 0, gfl2::math::Vector3(0.0f, 17.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 427 ミミロル
  { MONSNO_NYARUMAA  , 0, gfl2::math::Vector3(0.0f, 28.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 431 ニャルマー
  { MONSNO_MIKARUGE  , 0, gfl2::math::Vector3(0.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 442 ミカルゲ
  { MONSNO_NEORANTO  , 0, gfl2::math::Vector3(0.0f, 27.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 457 ネオラント
  { MONSNO_GIRATHINA , 1, gfl2::math::Vector3(0.0f, 39.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 487 ギラティナ    オリジンフォルム
  { MONSNO_DAAKURAI  , 0, gfl2::math::Vector3(0.0f, 25.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 491 ダークライ
  { MONSNO_BIKUTHINI , 0, gfl2::math::Vector3(0.0f, 18.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 494 ビクティニ
  { MONSNO_REPARUDASU, 0, gfl2::math::Vector3(0.0f, 35.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 510 レパルダス
  { MONSNO_MUSYAANA  , 0, gfl2::math::Vector3(0.0f, 22.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 518 ムシャーナ
  { MONSNO_IWAPARESU , 0, gfl2::math::Vector3(0.0f, 13.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 558 イワパレス
  { MONSNO_SYANDERA  , 0, gfl2::math::Vector3(0.0f, 45.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 609 シャンデラ
  { MONSNO_KYUREMU   , 1, gfl2::math::Vector3(0.0f, 26.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 646 キュレム      ホワイトキュレム
  { MONSNO_MAFOKUSII , 0, gfl2::math::Vector3(0.0f,-14.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 655 マフォクシー
  { MONSNO_NYAONIKUSU, 0, gfl2::math::Vector3(0.0f, 18.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 678 ニャオニクス  オス
  { MONSNO_NYAONIKUSU, 1, gfl2::math::Vector3(0.0f, 18.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 678 ニャオニクス  メス
  { MONSNO_BUROSUTAA , 0, gfl2::math::Vector3(0.0f, 42.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 693 ブロスター
  { MONSNO_ASIKA3    ,  0, gfl2::math::Vector3(-6.0f, 26.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 730 アシレーヌ
  { MONSNO_GURIPUSU2 ,  0, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  1, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  2, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  3, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  4, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  5, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  6, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  7, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  8, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 ,  9, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 , 10, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 , 11, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 , 12, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 , 13, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 , 14, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 , 15, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 , 16, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_GURIPUSU2 , 17, gfl2::math::Vector3(13.0f, 11.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 773 シルヴァディ
  { MONSNO_HORAA     ,  1, gfl2::math::Vector3( 0.0f,-27.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // 778 ミミッキュ  首折れ(ヌシフォルムは「ヌシでないフォルム」に変更して図鑑(登録)に来るので「首折れヌシ」は考えなくてよい)
};

s32 GetInFrameUserValueTableIndex(s16 monsNo, s16 formNo)  // 0以上の値を返すとき、テーブルに該当するものあり。
{
  for(s32 i=0; i<GFL_NELEMS(INFRAME_USER_VALUE_TABLE); ++i)
  {
    if(monsNo == INFRAME_USER_VALUE_TABLE[i].monsNo && formNo == INFRAME_USER_VALUE_TABLE[i].formNo )
    {
      return i;
    }
  }
  return -1;
}




const s32 ZukanTool::POKE_MODEL_INFRAME_CENTER[POKE_MODEL_POS_TYPE_NUM][2] =
{
  { 200, 120 },
  { 110, 120 },
  { 323, 117 },  // レイアウト位置(123,3)
  { 200, 120 },//{ 200,  90 },
  
};
const s32 ZukanTool::POKE_MODEL_INFRAME_LENGTH[POKE_MODEL_SIZE_TYPE_NUM] =
{
  200,//220,
  200,//160,
  128,  // レイアウトサイズ(128,128)
};

const f32 ZukanTool::POKE_MODEL_INFRAME_SCALE_DEFAULT[POKE_MODEL_SCALE_TYPE_NUM] =
{
  1.0f,
  0.85f,
};
const f32 ZukanTool::POKE_MODEL_INFRAME_SCALE_SMALL_DEFAULT[POKE_MODEL_SCALE_TYPE_NUM] =
{
  0.75f,//0.61f,//0.61=122/200
  0.6375f,//0.5185f,
};
const f32 ZukanTool::POKE_MODEL_INFRAME_SCALE_MAX[POKE_MODEL_SCALE_TYPE_NUM] =
{
  1.4f,
  1.15f,
};
const f32 ZukanTool::POKE_MODEL_INFRAME_SCALE_MIN[POKE_MODEL_SCALE_TYPE_NUM] =
{
  0.5f,
  0.5f,
};
const f32 ZukanTool::POKE_MODEL_INFRAME_PITCH_MIN = 0.0f;  // [radian]  下を全く見せない角度




ZukanTool::ZukanTool(
    GameSys::GameManager*           gameManager,
    app::util::AppRenderingManager* appRenderingManager
)
  : m_gameManager(gameManager),
    m_appRenderingManager(appRenderingManager),
    m_gameLang(ZukanType::ZUKAN_LANG_JAPAN),
    m_talkWindowState(TalkWindowState::NONE),
    m_talkWindow(NULL),
    m_talkWindowIsOpen(false),
    m_menuCursorState(MenuCursorState::NONE),
    m_menuCursor(NULL),
    m_menuCursorBuf(NULL),
    m_messageMenuViewState(MessageMenuViewState::NONE),
    m_messageMenuView(NULL),
    m_messageMenuViewParam(NULL),
    m_viewBg(NULL),
    m_viewRegisterFade(NULL),
    m_pokeModelSysState(PokeModelSysState::NONE),
    m_pokeModelSys(NULL),
    m_pokeSimpleModelInFrame(NULL),
    m_drawEnvNodeOfCamera(NULL),
    m_camera(NULL),
    m_drawEnvNode(NULL),
    m_pokeModelSp(),
    m_pokeModelVoiceFlag(false),
    m_currPokeModelIsReady(false),
    m_currPokeModelIsVisible(false),
    m_pokeModelPauseFlag(false),
    m_pokeModelAnimeIndex(0),
    m_pokeModelCurrInFrameCenterX(POKE_MODEL_INFRAME_CENTER[POKE_MODEL_POS_TYPE_CENTER][0]),
    m_pokeModelCurrInFrameCenterY(POKE_MODEL_INFRAME_CENTER[POKE_MODEL_POS_TYPE_CENTER][1]),
    m_pokeModelCurrInFrameLength(POKE_MODEL_INFRAME_LENGTH[POKE_MODEL_SIZE_TYPE_NORMAL]),
    m_pokeModelCurrInFrameSizeType(POKE_MODEL_SIZE_TYPE_NORMAL),
    m_pokeModelCurrInFrameScale(POKE_MODEL_INFRAME_SCALE_DEFAULT[POKE_MODEL_SCALE_TYPE_NORMAL]),
    m_pokeModelCurrInFrameAddSize(0),
    m_pokeModelInFrameScaleDefault(POKE_MODEL_INFRAME_SCALE_DEFAULT[POKE_MODEL_SCALE_TYPE_NORMAL]),
    m_pokeModelInFrameScaleMax(POKE_MODEL_INFRAME_SCALE_MAX[POKE_MODEL_SCALE_TYPE_NORMAL]),
    m_pokeModelInFrameScaleMin(POKE_MODEL_INFRAME_SCALE_MIN[POKE_MODEL_SCALE_TYPE_NORMAL]),
    m_pokeModelGoalPosType(POKE_MODEL_POS_TYPE_CENTER),
    m_pokeModelPosAnimFlag(false),
    //m_pokeExtendData(NULL),
    m_pokePersonalSort(NULL),
    m_msgSysState(MsgSysState::NONE),
    m_wordSetLoader(NULL),
    m_wordSet(NULL),
    m_twoButtonsViewState(TwoButtonsViewState::NONE),
    m_twoButtonsView(NULL),
    m_multiButtonsViewState(MultiButtonsViewState::NONE),
    m_multiButtonsView(NULL),
    m_multiButtonsViewResourceProvider(NULL),
    m_commonGrpIconState(CommonGrpIconState::NONE),
    m_commonGrpIconLang(m_gameLang),
    m_langMsgDataManager(NULL),
    m_langFontManager(NULL)
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
    , m_monsNameStrBuf(NULL)
#endif
{
  STATIC_ASSERT(FORMNO_GURIPUSU2_MAX == 18);  // INFRAME_USER_VALUE_TABLEにシルヴァディがいるので、フォルム数が変更されたときに気付けるようにしておく。

  // @fix GFNMCat[5175] 図鑑登録：ゼルネアス、キュレムのアニメがかわいがりアニメになっていない。  →かわいがりアニメにした。
  STATIC_ASSERT( PokeTool::MODEL_ANIME_BT_WAIT_A == 0 );  // バトル用モーションと可愛がり用モーションの待機モーションは
  STATIC_ASSERT( PokeTool::MODEL_ANIME_KW_WAIT == 0 );    // どちらも0番という前提でつくっている。

  m_pokeModelSp.monsNo = MONSNO_NULL;

  for(u32 i=0; i<TEMP_STR_BUF_NUM; ++i)
  {
    m_tempStrBuf[i] = NULL;
  }
  for(u32 i=0; i<MSG_TYPE_NUM; ++i)
  {
    for(s32 j=0; j<ZukanType::ZUKAN_LANG_NUM; ++j)
    {
      m_msgData[i][j] = NULL;
    }
  }
  for(u32 i=0; i<FONT_TYPE_MAX; ++i)
  {
    m_fontState[i] = FontState::NONE;
  }
  for(s32 i=0; i<ZukanType::ZUKAN_LANG_NUM; ++i)
  {
    m_commonGrpIcon[i] = NULL;
  }

  m_gameLang = ZukanType::ConvertPokemonLang2ZukanLang(System::GetLang());
}

ZukanTool::~ZukanTool()
{
  // assert
  GFL_ASSERT(m_langFontManager==NULL);       // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(m_langMsgDataManager==NULL);    // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  for(s32 i=0; i<ZukanType::ZUKAN_LANG_NUM; ++i)
  {
    GFL_ASSERT(m_commonGrpIcon[i]==NULL);     // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  }
  for(u32 i=0; i<FONT_TYPE_MAX; ++i)
  {
    GFL_ASSERT(this->DestroyFont(static_cast<FontType>(i)));         // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  }
  GFL_ASSERT(this->DestroyMultiButtonsView());  // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(this->DestroyTwoButtonsView());  // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  for(u32 i=0; i<MSG_TYPE_NUM; ++i)
  {
    for(s32 j=0; j<ZukanType::ZUKAN_LANG_NUM; ++j)
    {
      GFL_ASSERT(m_msgData[i][j]==NULL);           // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
    }
  }
  GFL_ASSERT(this->DestroyMsgSys());          // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(m_pokePersonalSort==NULL);       // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  //GFL_ASSERT(m_pokeExtendData==NULL);         // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(m_pokeSimpleModelInFrame==NULL);        // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(this->DestroyPokeModelSys());    // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(m_viewRegisterFade==NULL);       // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(m_viewBg==NULL);                 // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(this->DestroyMenuCursor());      // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
  GFL_ASSERT(this->DestroyTalkWindow());      // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  GFL_ASSERT(m_monsNameStrBuf==NULL);  // 開発中に気付かせるためのASSERT。ASSERTを抜けたら強制的に破棄する。
#endif

  // delete
  this->DestroyLangFontManager();
  this->DestroyLangMsgDataManager();
  for(s32 i=0; i<ZukanType::ZUKAN_LANG_NUM; ++i)
  {
    GFL_SAFE_DELETE(m_commonGrpIcon[i]);
  }
  for(u32 i=0; i<FONT_TYPE_MAX; ++i)
  {
    this->DestroyFont(static_cast<FontType>(i));  // フォントはこの関数で破棄するしかない
  }
  GFL_SAFE_DELETE(m_multiButtonsViewResourceProvider);
  GFL_SAFE_DELETE(m_multiButtonsView);
  GFL_SAFE_DELETE(m_twoButtonsView);
  for(u32 i=0; i<MSG_TYPE_NUM; ++i)
  {
    for(s32 j=0; j<ZukanType::ZUKAN_LANG_NUM; ++j)
    {
      GFL_SAFE_DELETE(m_msgData[i][j]);
    }
  }
  for(u32 i=0; i<TEMP_STR_BUF_NUM; ++i)
  {
    GFL_SAFE_DELETE(m_tempStrBuf[i]);
  }
  GFL_SAFE_DELETE(m_wordSet);
  GFL_SAFE_DELETE(m_wordSetLoader);
  GFL_SAFE_DELETE(m_pokePersonalSort);
  //GFL_SAFE_DELETE(m_pokeExtendData);
  GFL_SAFE_DELETE(m_pokeSimpleModelInFrame);
  GFL_SAFE_DELETE(m_pokeModelSys);
  GFL_SAFE_DELETE(m_viewRegisterFade);
  GFL_SAFE_DELETE(m_viewBg);
  GFL_SAFE_DELETE(m_menuCursor);
  GFL_SAFE_DELETE(m_talkWindow);

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  GFL_SAFE_DELETE(m_monsNameStrBuf);
#endif
}

void ZukanTool::Update(void)
{
  if(m_pokeSimpleModelInFrame)
  {
    this->updatePokeModelPosAnim();
    m_pokeSimpleModelInFrame->Update();
    m_pokeSimpleModelInFrame->UpdateMegaGangarEffectState();

    // ポケモンの鳴き声  // @fix GFNMCat[5243] アローラ図鑑でリストからポケモンの詳細画面に入った時に鳴き声がなっていません。  →鳴らす
    {
      bool prevPokeModelIsReady   = m_currPokeModelIsReady;
      bool prevPokeModelIsVisible = m_currPokeModelIsVisible;
      m_currPokeModelIsReady   = m_pokeSimpleModelInFrame->IsReady();
      m_currPokeModelIsVisible = m_pokeSimpleModelInFrame->IsVisible();
      if(m_pokeModelVoiceFlag)
      {
        if(    m_currPokeModelIsReady && m_currPokeModelIsVisible
            && ( m_currPokeModelIsReady!=prevPokeModelIsReady || m_currPokeModelIsVisible!=prevPokeModelIsVisible ) )
        {
          // 表示された瞬間に鳴らす
          Sound::PlayVoice( 0, m_pokeModelSp.monsNo, m_pokeModelSp.formNo );  // voice_indexは0でいいか？
        }
      }
    }
  }
  if(m_pokeModelSys)
  {
    m_pokeModelSys->Update();
  }
}


//! @name  会話用ウィンドウ
//@{
bool ZukanTool::CreateTalkWindow(app::util::Heap* appHeap, void* buf)
{
  bool ret = false;
  switch(m_talkWindowState)
  {
  case TalkWindowState::NONE:
    {
      m_talkWindow = GFL_NEW(appHeap->GetDeviceHeap()) App::Tool::TalkWindow(appHeap, m_appRenderingManager);
      m_talkWindow->SetWindowType(App::Tool::WIN_TYPE_SYS_DOWN);
      m_talkWindow->Setup(buf);
      m_talkWindowState = TalkWindowState::WORKING;
      //breakしない
    }
  case TalkWindowState::WORKING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に生成しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyTalkWindow(void)
{
  bool ret = false;
  switch(m_talkWindowState)
  {
  case TalkWindowState::WORKING:
    {
      m_talkWindow->End();
      GFL_SAFE_DELETE(m_talkWindow);
      m_talkWindowState = TalkWindowState::NONE;
      //breakしない
    }
  case TalkWindowState::NONE:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}

void ZukanTool::OpenTalkWindow(
    app::ui::UIView*   parentView,
    gfl2::str::StrBuf* strBuf,
    bool               msgCursorVisible
)
{
  if(m_talkWindowState == TalkWindowState::WORKING)
  {
    //parentView->AddSubView(m_talkWindow);  // app::ui::UIViewのメンバ
    m_talkWindowIsOpen = true;  // @fix GFNMCat[1203] 「図鑑登録において、ウィンドウより手前にポケモンが表示されており、大きいポケモン時にウィンドウが見えない」に対処。
    m_talkWindow->SetMessage(strBuf);
    m_talkWindow->SetMsgCursorUserVisible(false, msgCursorVisible);
    m_talkWindow->CallMsgWin();
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていないので使い方が間違っている。
  }
}
void ZukanTool::CloseTalkWindow(void)
{
  if(m_talkWindowState == TalkWindowState::WORKING)
  {
    m_talkWindow->CloseMsgWin();
    //m_talkWindow->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ
    m_talkWindowIsOpen = false;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていないので使い方が間違っている。
  }
}
bool ZukanTool::IsTalkWindowWaiting(void) const
{
  if(m_talkWindowState == TalkWindowState::WORKING)
  {
    return ( m_talkWindow->GetStatus() == App::Tool::TalkWindow::WIN_STATUS_WIN_EXIT );
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていないので使い方が間違っている。
    return false;
  }
}
App::Tool::TalkWindow* ZukanTool::GetTalkWindowIfOpen(void) const
{
  if(m_talkWindowIsOpen)
  {
    return m_talkWindow;
  }
  else
  {
    return NULL;
  }
}
//@}


//! @name  メニューカーソル
//@{
bool ZukanTool::CreateMenuCursor(app::util::Heap* appHeap)
{
  bool ret = false;
  switch(m_menuCursorState)
  {
  case MenuCursorState::NONE:
    {
      m_menuCursor = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::MenuCursor(appHeap, &m_menuCursorBuf);
      m_menuCursorState = MenuCursorState::CREATING;
      //breakしない
    }
  case MenuCursorState::CREATING:
    {
      if( m_menuCursor->LoadResource() )
      {
        m_menuCursorState = MenuCursorState::WORKING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case MenuCursorState::WORKING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に生成しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyMenuCursor(void)
{
  bool ret = false;
  switch(m_menuCursorState)
  {
  case MenuCursorState::WORKING:
    {
      m_menuCursorState = MenuCursorState::DESTROYING;
      //breakしない
    }
  case MenuCursorState::DESTROYING:
    {
      if( m_menuCursor->IsModuleFree() )
      {
        m_menuCursor->DeleteResource();
        GFL_SAFE_DELETE(m_menuCursor);
        m_menuCursorBuf = NULL;
        m_menuCursorState = MenuCursorState::NONE;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case MenuCursorState::NONE:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}

void ZukanTool::CreateMenuCursorLayoutWork(app::util::G2DUtil* pG2DUtil, gfl2::lyt::DisplayType displayType, app::tool::MenuCursor::CursorSize cursorSize)
{
  m_menuCursor->CreateLayoutWork
  (
    pG2DUtil->GetLayoutSystem(),
    pG2DUtil->GetLayoutWorkSetup(displayType),
    pG2DUtil->GetAppLytAccessor(),
    cursorSize,
    false
  );
  m_menuCursor->StartAnime();
}
bool ZukanTool::DestroyMenuCursorLayoutWork(void)
{
  return m_menuCursor->DeleteLayoutWork();
}

app::tool::MenuCursor* ZukanTool::GetMenuCursor(void) const
{
  app::tool::MenuCursor* ret = NULL;
  if(m_menuCursorState == MenuCursorState::WORKING) ret = m_menuCursor;
  return ret;
}
//@}


//! @name  メッセージウィンドウ＆リストメニュー
//@{
bool ZukanTool::CreateMessageMenuView(app::util::Heap* appHeap, void* buf)
{
  bool ret = false;
  switch(m_messageMenuViewState)
  {
  case MessageMenuViewState::NONE:
    {
      GFL_ASSERT(m_menuCursorBuf != NULL);  // 開発中に気付かせるためのASSERT

      m_messageMenuViewParam = GFL_NEW(appHeap->GetDeviceHeap()) App::Tool::MessageMenuView::PARAM(appHeap, m_appRenderingManager);
      m_messageMenuView      = GFL_NEW(appHeap->GetDeviceHeap()) App::Tool::MessageMenuView(*m_messageMenuViewParam);
      m_messageMenuView->Setup(buf, m_menuCursorBuf);
      m_messageMenuViewState = MessageMenuViewState::CREATING;
      //breakしない
    }
  case MessageMenuViewState::CREATING:
    {
      if( m_messageMenuView->IsSetup() )
      {
        m_messageMenuViewState = MessageMenuViewState::WORKING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case MessageMenuViewState::WORKING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に生成しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyMessageMenuView(void)
{
  bool ret = false;
  switch(m_messageMenuViewState)
  {
  case MessageMenuViewState::WORKING:
    {
      m_messageMenuViewState = MessageMenuViewState::DESTROYING;
      //breakしない
    }
  case MessageMenuViewState::DESTROYING:
    {
      GFL_SAFE_DELETE(m_messageMenuView);
      GFL_SAFE_DELETE(m_messageMenuViewParam);
      m_messageMenuViewState = MessageMenuViewState::NONE;
      break;
    }
  case MessageMenuViewState::NONE:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}

App::Tool::MessageMenuView* ZukanTool::GetMessageMenuView(void) const
{
  App::Tool::MessageMenuView* ret = NULL;
  if(m_messageMenuViewState == MessageMenuViewState::WORKING) ret = m_messageMenuView;
  return ret;
}
//@}


//! @name  背景
//@{
void ZukanTool::CreateBg(const ZukanViewBg::Param& param)
{
  m_viewBg = GFL_NEW(param.appHeap->GetDeviceHeap()) ZukanViewBg(param);
}
void ZukanTool::DestroyBg(void)
{
  GFL_SAFE_DELETE(m_viewBg);
}
ZukanViewBg* ZukanTool::GetViewBg(void) const
{
  return m_viewBg;
}
//@}


//! @name  図鑑の登録画面に出すフェード層のビュー
//@{
void ZukanTool::CreateRegisterFade(const ZukanViewRegisterFade::Param& param)
{
  m_viewRegisterFade = GFL_NEW(param.appHeap->GetDeviceHeap()) ZukanViewRegisterFade(param);
}
void ZukanTool::DestroyRegisterFade(void)
{
  GFL_SAFE_DELETE(m_viewRegisterFade);
}
ZukanViewRegisterFade* ZukanTool::GetViewRegisterFade(void) const
{
  return m_viewRegisterFade;
}
//@}


//! @name  ポケモンモデル
//@{
bool ZukanTool::CreatePokeModelSys(app::util::Heap* appHeap, gfl2::heap::HeapBase* heapForComp) 
{
  bool ret = false;
  switch(m_pokeModelSysState)
  {
  case PokeModelSysState::NONE:
    {
      m_pokeModelSys = GFL_NEW(appHeap->GetDeviceHeap()) PokeTool::PokeModelSystem();
      //システムの非同期初期化開始
      m_pokeModelSys->InitSystemAsync(
        appHeap->GetDeviceHeap(),
        appHeap->GetDeviceHeap(),
        m_gameManager->GetAsyncFileManager(),
        1
      );
      m_pokeModelSysState = PokeModelSysState::CREATING;
      //breakしない
    }
  case PokeModelSysState::CREATING:
    {
      //初期化待ち
      if( m_pokeModelSys->IsFinishInitSystemAsync() )
      {
        // m_pokeModelSys
        {
          //システム初期化
          m_pokeModelSys->CreateSystem(false, NULL);
          //ポケモン用ヒープ作成
          PokeTool::PokeModelSystem::HeapOption opt;
          opt.animeType      = PokeTool::MODEL_ANIMETYPE_BATTLE;
          opt.useIdModel     = false;
          opt.useShadow      = false;
          opt.useColorShader = false;
          m_pokeModelSys->CreatePokeModelHeap(appHeap->GetDeviceHeap(), &opt);
        }

        // Camera, DrawEnv
        {
          app::util::AppRenderingManager::CreateDefaultCamera(appHeap, &m_drawEnvNodeOfCamera, &m_camera, app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
          m_drawEnvNode = app::util::AppRenderingManager::CreateDefaultDrawEnv(appHeap);
          m_camera->SetPosition(gfl2::math::Vector3(0,40,500));
          m_camera->SetTargetPosition(gfl2::math::Vector3(0,40,0));
          app::tool::PokeSimpleModelInFrame::SetLightParamForInFrame(m_drawEnvNode->GetLightSet(0)->GetLightNode(0), System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT);

          m_appRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_camera);
          m_appRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_drawEnvNode);
        }

        // m_pokeSimpleModelInFrame
        {
          app::tool::PokeSimpleModel::INIT_PARAM  initParam;
          {
            PokeTool::PokeModel::SetupOption* opt = &initParam.setupOpt;
            opt->dataHeap = appHeap->GetDeviceHeap();
            opt->workHeap = heapForComp;
          }
          initParam.pPokeModelSys     = m_pokeModelSys;
          initParam.pRenderingManager = m_appRenderingManager;
          initParam.renderPlace       = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
       
          m_pokeSimpleModelInFrame = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::PokeSimpleModelInFrame(initParam);

          m_pokeSimpleModelInFrame->InitInFrame(System::Camera::CModelInFrame::SCREEN_UPPER, System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT, System::Camera::CModelViewerInFrame::DRAW_POSITION_CENTER);
          this->setPokeModelInFrameFrame();
          this->resetPokeModelAnime();
        }

        m_pokeModelSysState = PokeModelSysState::WORKING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case PokeModelSysState::WORKING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に生成しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyPokeModelSys(void)
{
  bool ret = false;
  switch(m_pokeModelSysState)
  {
  case PokeModelSysState::WORKING:
    {
      m_pokeSimpleModelInFrame->Terminate();
      m_pokeModelSysState = PokeModelSysState::DESTROYING_MODEL;
      //breakしない
    }
  case PokeModelSysState::DESTROYING_MODEL:
    {
      if( m_pokeSimpleModelInFrame->IsDelete() )
      {
        // m_pokeSimpleModelInFrame
        {
          GFL_SAFE_DELETE(m_pokeSimpleModelInFrame);
        }

        // Camera, DrawEnv
        {
          m_appRenderingManager->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_drawEnvNode);
          m_appRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);

          GFL_SAFE_DELETE(m_drawEnvNodeOfCamera);
          GFL_SAFE_DELETE(m_camera);
          GFL_SAFE_DELETE(m_drawEnvNode);
        }

        // m_pokeModelSys
        {
          //ポケモン用ヒープ削除
          m_pokeModelSys->DeletePokeModelHeap();
          //システム開放開始
          m_pokeModelSys->TermSystemAsync();
          m_pokeModelSysState = PokeModelSysState::DESTROYING_SYS;
        }
        //breakしない
      }
      else
      {
        break;
      }
    }
  case PokeModelSysState::DESTROYING_SYS:
    {
      //システム開放待ち
      if( m_pokeModelSys->IsFinishTermSystemAsync() )
      {
        GFL_SAFE_DELETE(m_pokeModelSys);
        m_pokeModelSysState = PokeModelSysState::NONE;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case PokeModelSysState::NONE:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}

PokeTool::PokeModelSystem* ZukanTool::GetPokeModelSys(void) const
{
  if(m_pokeModelSysState == PokeModelSysState::WORKING)
  {
    return m_pokeModelSys;
  }
  else
  {
    return NULL;
  }
}
app::tool::PokeSimpleModelInFrame* ZukanTool::GetPokeSimpleModelInFrame(void) const
{
  if(m_pokeModelSysState == PokeModelSysState::WORKING)
  {
    return m_pokeSimpleModelInFrame;
  }
  else
  {
    return NULL;
  }
}

void ZukanTool::ChangePokeModel(const PokeTool::SimpleParam& sp)
{
  if(
         ( m_pokeModelSp.monsNo  != sp.monsNo  )
      || ( m_pokeModelSp.formNo  != sp.formNo  )
      || ( m_pokeModelSp.isEgg   != sp.isEgg   )
      || ( m_pokeModelSp.sex     != sp.sex     )
      || ( m_pokeModelSp.isRare  != sp.isRare  )
      || ( m_pokeModelSp.perRand != sp.perRand )
  )
  {
    if(m_pokeSimpleModelInFrame == NULL)  // @fix cov_ctr[11248]: NULLチェックを追加。
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。これを呼ぶときにはm_pokeSimpleModelInFrameは生成済みになっているはず。
      return;
    }

    m_pokeModelSp = sp;

    // @fix GFNMCat[5175] 図鑑登録：ゼルネアス、キュレムのアニメがかわいがりアニメになっていない。  →かわいがりアニメにした。
    if( this->checkUsingKawaigariInAppForZukan( sp.monsNo, sp.formNo ) )
    {
      m_pokeSimpleModelInFrame->SetAnimeType(PokeTool::MODEL_ANIMETYPE_KAWAIGARI);
    }
    else
    {
      m_pokeSimpleModelInFrame->SetAnimeType(PokeTool::MODEL_ANIMETYPE_BATTLE);
    }

    m_pokeSimpleModelInFrame->StartLoad( sp );
    this->pausePokeModelAnime(false);
    this->resetPokeModelAnime();

    if(m_pokeSimpleModelInFrame)
    {
      // ポケモン変更により変わるものを設定
      this->setPokeModelInFrameSRT();

      this->setPokeModelInFrameUserValue();

      this->setPokeModelInFrameFrame();
    }
  }
}
void ZukanTool::SetPokeModelVisible(bool visible)
{
  if(m_pokeSimpleModelInFrame)
  {
    m_pokeSimpleModelInFrame->SetVisible(visible);
  }
}
bool ZukanTool::IsPokeModelVisible(void) const
{
  if(m_pokeSimpleModelInFrame)
  {
    return m_pokeSimpleModelInFrame->IsVisible();
  }
  else
  {
    return false;
  }
}

void ZukanTool::ResetPokeModel(void)
{
  // アニメ、ポーズのリセット
  this->pausePokeModelAnime(false);
  this->resetPokeModelAnime();

  // 拡大縮小のリセット
  m_pokeModelCurrInFrameScale = m_pokeModelInFrameScaleDefault;
  m_pokeModelCurrInFrameAddSize = 0;
  this->setPokeModelInFrameFrame();

  // 回転のリセット
  m_pokeSimpleModelInFrame->GetInFrameSys()->ResetCameraYawPitch();
}

void ZukanTool::pausePokeModelAnime(bool isPause)
{
  if(m_pokeModelPauseFlag != isPause)
  {
    m_pokeModelPauseFlag = isPause;
    m_pokeSimpleModelInFrame->PauseAnime(m_pokeModelPauseFlag);
  }
}
void ZukanTool::SwitchPokeModelAnimePause(void)
{
  this->pausePokeModelAnime(!m_pokeModelPauseFlag);
}

void ZukanTool::changePokeModelAnime(PokeTool::MODEL_ANIME animeType)
{
  this->pausePokeModelAnime(false);  // アニメを変更したら、一時停止を解除する
  m_pokeSimpleModelInFrame->SetAnime( animeType, true );
}
void ZukanTool::SetNextPokeModelAnime(void)
{
  static const PokeTool::MODEL_ANIME POKE_MODEL_ANIME_TYPE[] =
  {
    PokeTool::MODEL_ANIME_BT_WAIT_A,  // 必ずresetPokeModelAnimeと合わせておくこと。必ずどのポケモンにも存在するモーションにしておくこと。かわいがり用モーションMODEL_ANIME_KW_WAITと同じ値のモーションにしておくこと。
    PokeTool::MODEL_ANIME_BT_WAIT_B,
    PokeTool::MODEL_ANIME_BT_WAIT_C,
    PokeTool::MODEL_ANIME_BUTURI1,
    PokeTool::MODEL_ANIME_BUTURI2,
    PokeTool::MODEL_ANIME_BUTURI3,
    PokeTool::MODEL_ANIME_BUTURI4,
    PokeTool::MODEL_ANIME_TOKUSHU1,
    PokeTool::MODEL_ANIME_TOKUSHU2,
    PokeTool::MODEL_ANIME_TOKUSHU3,
    PokeTool::MODEL_ANIME_TOKUSHU4,
  };
  u32 elemNum = sizeof(POKE_MODEL_ANIME_TYPE)/sizeof(POKE_MODEL_ANIME_TYPE[0]);

  // @fix GFNMCat[5175] 図鑑登録：ゼルネアス、キュレムのアニメがかわいがりアニメになっていない。  →かわいがりアニメにした。
  if( this->checkUsingKawaigariInAppForZukan( m_pokeModelSp.monsNo, m_pokeModelSp.formNo ) )
  {
    elemNum = 1;  // かわいがりアニメのときは待機しか再生しない。
  }

  u32 nextAnimeIndex = m_pokeModelAnimeIndex;
  if(m_pokeSimpleModelInFrame->IsReady())
  {
    // ポケモンモデルの準備が終わっているときしかアニメは変更できないことにする(リセットは別の関数で行うので準備ができていなくてもできる)
    for(u32 i=0; i<elemNum; ++i)
    {
      u32 animeIndex = (m_pokeModelAnimeIndex +i+1) % elemNum;
      if( m_pokeSimpleModelInFrame->IsAvailableAnimation(POKE_MODEL_ANIME_TYPE[animeIndex]) )
      {
        if( m_pokeSimpleModelInFrame->CheckCanOneshotMotion(POKE_MODEL_ANIME_TYPE[animeIndex]) )
        {
          // 再生してもよい
          nextAnimeIndex = animeIndex;
          break;
        }
      }
    }

    // 次のアニメに変更できていたら変更する
    if(nextAnimeIndex != m_pokeModelAnimeIndex)
    {
      m_pokeModelAnimeIndex = nextAnimeIndex;
      this->changePokeModelAnime(POKE_MODEL_ANIME_TYPE[m_pokeModelAnimeIndex]);

      // @fix NMCat[1132] バトルモーション再生中に拡大表示できてしまう
      // 待機モーション(0番)以外のモーションのときは、デフォルトサイズより拡大できないようにしておく。
      // これの記述箇所が2箇所に分かれてしまったのは失敗だったので、次回はsetPokeModelInFrameFrameをUpdate1箇所にまとめたい。
      if(m_pokeModelAnimeIndex != 0)
      {
        if(    m_pokeModelCurrInFrameScale > m_pokeModelInFrameScaleDefault
            || m_pokeModelCurrInFrameAddSize > 0
        )
        {
          m_pokeModelCurrInFrameScale = m_pokeModelInFrameScaleDefault;
          m_pokeModelCurrInFrameAddSize = 0;
          this->setPokeModelInFrameFrame();
        }
      }
    }
  }
}
void ZukanTool::resetPokeModelAnime(void)
{
  m_pokeModelAnimeIndex = 0;
  this->changePokeModelAnime(PokeTool::MODEL_ANIME_BT_WAIT_A);  // 必ずSetNextPokeModelAnimeと合わせておくこと。必ずどのポケモンにも存在するモーションにしておくこと。かわいがり用モーションMODEL_ANIME_KW_WAITと同じ値のモーションにしておくこと。
}

void ZukanTool::SetPokeModelPosType(PokeModelPosType posType, bool posAnimFlag)
{
  if(m_pokeModelGoalPosType == posType) return;

  if(!posAnimFlag)
  {
    // ユーザが「アニメしない」と指定したときに一緒に
    // 1辺の長さ、サイズタイプを設定する
    static const PokeModelSizeType POS_TYPE_TO_SIZE_TYPE[] =
    {
      POKE_MODEL_SIZE_TYPE_NORMAL,
      POKE_MODEL_SIZE_TYPE_NORMAL,
      POKE_MODEL_SIZE_TYPE_SMALL_RIGHT,
      POKE_MODEL_SIZE_TYPE_SMALL,
    };

    static const System::Camera::CModelViewerInFrame::EDrawPosition POS_TYPE_TO_INFRAME_DRAW_POSITION[] =
    {
      System::Camera::CModelViewerInFrame::DRAW_POSITION_CENTER,
      System::Camera::CModelViewerInFrame::DRAW_POSITION_CENTER,
      System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM,
      System::Camera::CModelViewerInFrame::DRAW_POSITION_CENTER,
    };

    m_pokeModelCurrInFrameLength   = POKE_MODEL_INFRAME_LENGTH[ POS_TYPE_TO_SIZE_TYPE[posType] ];
    m_pokeModelCurrInFrameSizeType = POS_TYPE_TO_SIZE_TYPE[posType];
     
    // PosType変更により変わるものを設定
    this->setPokeModelInFrameSRT();

    // インフレームの配置を変更する
    if(m_pokeSimpleModelInFrame)
    {
      m_pokeSimpleModelInFrame->SetInFrameDrawPosition(POS_TYPE_TO_INFRAME_DRAW_POSITION[posType]);
      // System::Camera::CModelViewerInFrame::EDrawPositionを設定したので、この関数の最後にUserPosOffsetとUserScaleを設定する
    }
  }
  else
  {
    // ユーザが「アニメする」と指定したときに
    // 変更できないタイプを設定していないか確認する
    if(m_pokeModelGoalPosType == POKE_MODEL_POS_TYPE_SMALL_CENTER)  // 小サイズ
    {
      GFL_ASSERT(posType == POKE_MODEL_POS_TYPE_SMALL_CENTER);  // 小サイズ→標準サイズはダメ  // 開発中に気付かせるためのASSERT
    }
    else  // 標準サイズ
    {
      GFL_ASSERT(posType != POKE_MODEL_POS_TYPE_SMALL_CENTER);  // 標準サイズ→小サイズはダメ  // 開発中に気付かせるためのASSERT
    }
  }


  bool l_posAnimFlag = posAnimFlag;
  if(posAnimFlag)
  {
    if(m_pokeModelPosAnimFlag == false)
    {
      if(m_pokeModelGoalPosType == posType)  // 既にその位置にいるなら移動しない。
      {
        l_posAnimFlag = false;
      }
    }
  }

  if(l_posAnimFlag)
  {
    m_pokeModelGoalPosType      = posType;
    m_pokeModelPosAnimFlag      = true;
  }
  else
  {
    m_pokeModelCurrInFrameCenterX = POKE_MODEL_INFRAME_CENTER[posType][0];
    m_pokeModelCurrInFrameCenterY = POKE_MODEL_INFRAME_CENTER[posType][1];
    m_pokeModelGoalPosType        = posType;
    m_pokeModelPosAnimFlag        = false;
    this->setPokeModelInFrameFrame();
  }

  // @fix NMCat[2241] 島図鑑で一部のポケモンの表示される位置が高い  →下揃えで表示するときはインフレームの位置とスケールの調整値を入れないようにする
  if(!posAnimFlag)
  {
    // System::Camera::CModelViewerInFrame::EDrawPositionを設定したとき、ここを通る
    this->setPokeModelInFrameUserValue();
  }
}
void ZukanTool::UpdatePokeModelController(bool onlyRotate)
{
  if(m_pokeSimpleModelInFrame)
  {
    gfl2::ui::DeviceManager* uiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

    const gfl2::ui::Button*       button   = uiDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    //const gfl2::ui::VectorDevice* crossKey = uiDeviceManager->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);
    const gfl2::ui::VectorDevice* stick    = uiDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);

    if(m_pokeSimpleModelInFrame->GetInFrameSys()->IsModelOnDisplay())
    {
      if(onlyRotate == false)
      {
        // ズーム
        bool zoomChange = false;

        /*
        f32 rate = (m_pokeModelCurrInFrameScale/2.0f+(1.0f-m_pokeModelInFrameScaleMin/2.0f));
        if( button->IsHold(gfl2::ui::BUTTON_L) )
        {
          // 大きくする
          m_pokeModelCurrInFrameScale += 0.02f*rate;
          if(m_pokeModelCurrInFrameScale > m_pokeModelInFrameScaleMax) m_pokeModelCurrInFrameScale = m_pokeModelInFrameScaleMax;
         
          // @fix NMCat[1132] バトルモーション再生中に拡大表示できてしまう
          // 待機モーション(0番)以外のモーションのときは、デフォルトサイズより拡大できないようにしておく。
          // これの記述箇所が2箇所に分かれてしまったのは失敗だったので、次回はsetPokeModelInFrameFrameをUpdate1箇所にまとめたい。
          if(m_pokeModelAnimeIndex != 0)
          {
            if(m_pokeModelCurrInFrameScale > m_pokeModelInFrameScaleDefault) m_pokeModelCurrInFrameScale = m_pokeModelInFrameScaleDefault;
          }

          zoomChange = true;
        }
        else if( button->IsHold(gfl2::ui::BUTTON_R) )
        {
          // 小さくする
          m_pokeModelCurrInFrameScale -= 0.02f*rate;
          if(m_pokeModelCurrInFrameScale < m_pokeModelInFrameScaleMin) m_pokeModelCurrInFrameScale = m_pokeModelInFrameScaleMin;
          zoomChange = true;
        }
        */

        // @fix NMCat[3712] 図鑑のポケモン詳細画面にて、特定の状況で拡縮する際に3Dモデルがカクつく事について
        //      ← 半径が2変化するときと半径が3変化するときが入り乱れていたためカクついた。半径が2変化するときで固定した。
        if( button->IsHold(gfl2::ui::BUTTON_L) )
        {
          // 大きくする
          s32 addSizeMax = m_pokeModelCurrInFrameLength/2 * ( m_pokeModelInFrameScaleMax - m_pokeModelInFrameScaleDefault );
          m_pokeModelCurrInFrameAddSize += 2;
          if(m_pokeModelCurrInFrameAddSize > addSizeMax) m_pokeModelCurrInFrameAddSize = addSizeMax;
         
          // @fix NMCat[1132] バトルモーション再生中に拡大表示できてしまう
          // 待機モーション(0番)以外のモーションのときは、デフォルトサイズより拡大できないようにしておく。
          // これの記述箇所が2箇所に分かれてしまったのは失敗だったので、次回はsetPokeModelInFrameFrameをUpdate1箇所にまとめたい。
          if(m_pokeModelAnimeIndex != 0)
          {
            if(m_pokeModelCurrInFrameAddSize > 0) m_pokeModelCurrInFrameAddSize = 0;
          }

          zoomChange = true;
        }
        else if( button->IsHold(gfl2::ui::BUTTON_R) )
        {
          // 小さくする
          s32 addSizeMin = m_pokeModelCurrInFrameLength/2 * ( m_pokeModelInFrameScaleMin - m_pokeModelInFrameScaleDefault );  // 負
          m_pokeModelCurrInFrameAddSize -= 2;
          if(m_pokeModelCurrInFrameAddSize < addSizeMin) m_pokeModelCurrInFrameAddSize = addSizeMin;
          zoomChange = true;
        }

        if(zoomChange)
        {
          this->setPokeModelInFrameFrame();
        }
      }

      // 回転
      {
        m_pokeSimpleModelInFrame->GetInFrameSys()->AddCameraYaw(-stick->GetX()/8.0f);
        m_pokeSimpleModelInFrame->GetInFrameSys()->AddCameraPitch(-stick->GetY()/16.0f);  // 縦の回転はゆっくりにしておく
        
        if(m_pokeSimpleModelInFrame->GetInFrameSys()->GetCameraPitch() < POKE_MODEL_INFRAME_PITCH_MIN)  // PITCHの上限はインフレームに任せる
        {
          m_pokeSimpleModelInFrame->GetInFrameSys()->SetCameraPitch(POKE_MODEL_INFRAME_PITCH_MIN);
        }
      }
    }
  }
}


#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
void ZukanTool::UpdatePokeModelControllerForPokeModelSRTModifyMode(void)
{
  if(m_pokeSimpleModelInFrame)
  {
    gfl2::ui::DeviceManager* uiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
    const gfl2::ui::VectorDevice* crossKey = uiDeviceManager->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);

    if(m_pokeSimpleModelInFrame->GetInFrameSys()->IsModelOnDisplay())
    {
      gfl2::math::Vector3 posOffset = m_pokeSimpleModelInFrame->GetInFrameSys()->GetUserPosOffset();
      gfl2::math::Vector3 scale     = m_pokeSimpleModelInFrame->GetInFrameSys()->GetUserScale();

      const f32 SMALL_VALUE = 0.0001f;
      s32 x = 0;
      s32 y = 0;
      if(posOffset.x < -SMALL_VALUE)     x = static_cast<s32>(posOffset.x - SMALL_VALUE);
      else if(posOffset.x > SMALL_VALUE) x = static_cast<s32>(posOffset.x + SMALL_VALUE);
      if(posOffset.y < -SMALL_VALUE)     y = static_cast<s32>(posOffset.y - SMALL_VALUE);
      else if(posOffset.y > SMALL_VALUE) y = static_cast<s32>(posOffset.y + SMALL_VALUE);

      const gfl2::str::STRCODE* monsNameStrCodePtr = L"";
      if(m_monsNameStrBuf)
      {
        pml::personal::GetMonsName(m_monsNameStrBuf, m_pokeModelSp.monsNo);
        monsNameStrCodePtr = m_monsNameStrBuf->GetPtr();
      }
      //GFL_RELEASE_PRINT("{ %3d, %2d, gfl2::math::Vector3(%d.0f, %d.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // %d %ls\n", m_pokeModelSp.monsNo, m_pokeModelSp.formNo, x, y, m_pokeModelSp.monsNo, monsNameStrCodePtr);
      GFL_RELEASE_PRINT("{ %3d, %2d, gfl2::math::Vector3(%d.0f, %d.0f,0.0f), gfl2::math::Vector3(1.0f,1.0f,1.0f) },  // %d\n", m_pokeModelSp.monsNo, m_pokeModelSp.formNo, x, y, m_pokeModelSp.monsNo);

      if( crossKey->IsRepeat(gfl2::ui::BUTTON_UP) )
      {
        y += 1;
      }
      else if( crossKey->IsRepeat(gfl2::ui::BUTTON_DOWN) )
      {
        y -= 1;
      }
      else if( crossKey->IsRepeat(gfl2::ui::BUTTON_LEFT) )
      {
        x -= 1;
      }
      else if( crossKey->IsRepeat(gfl2::ui::BUTTON_RIGHT) )
      {
        x += 1;
      }

      posOffset.x = x;
      posOffset.y = y;

      m_pokeSimpleModelInFrame->GetInFrameSys()->SetUserPosOffset(posOffset);
      m_pokeSimpleModelInFrame->GetInFrameSys()->SetUserScale(scale);
    }
  }
}
#endif


void ZukanTool::setPokeModelInFrameSRT(void)
{
  u32 scaleType = POKE_MODEL_SCALE_TYPE_NORMAL;
  if( m_pokeModelSp.monsNo == MONSNO_REKKUUZA && m_pokeModelSp.formNo == FORMNO_REKKUUZA_DELTA )  // @fix cov_ctr[10928]: formNoをmonsNoと間違えて書いていたのを修正
  {
    // メガレックウザだけ特別対応
    scaleType = POKE_MODEL_SCALE_TYPE_REKKUUZA_DELTA;
  }
  
  if(m_pokeModelCurrInFrameSizeType == POKE_MODEL_SIZE_TYPE_SMALL)
  {
    m_pokeModelInFrameScaleDefault = POKE_MODEL_INFRAME_SCALE_SMALL_DEFAULT[scaleType];
  }
  else
  {
    m_pokeModelInFrameScaleDefault = POKE_MODEL_INFRAME_SCALE_DEFAULT[scaleType];
  }
  m_pokeModelInFrameScaleMax     = POKE_MODEL_INFRAME_SCALE_MAX[scaleType];
  m_pokeModelInFrameScaleMin     = POKE_MODEL_INFRAME_SCALE_MIN[scaleType];
  
  m_pokeModelCurrInFrameScale = m_pokeModelInFrameScaleDefault;
  m_pokeModelCurrInFrameAddSize = 0;
} 

void ZukanTool::setPokeModelInFrameFrame(void)
{
  /*
  s32 left   = m_pokeModelCurrInFrameCenterX - m_pokeModelCurrInFrameLength/2 * m_pokeModelCurrInFrameScale;
  s32 top    = m_pokeModelCurrInFrameCenterY - m_pokeModelCurrInFrameLength/2 * m_pokeModelCurrInFrameScale;
  s32 right  = m_pokeModelCurrInFrameCenterX + m_pokeModelCurrInFrameLength/2 * m_pokeModelCurrInFrameScale;
  s32 bottom = m_pokeModelCurrInFrameCenterY + m_pokeModelCurrInFrameLength/2 * m_pokeModelCurrInFrameScale;
  */

  // @fix NMCat[3712] 図鑑のポケモン詳細画面にて、特定の状況で拡縮する際に3Dモデルがカクつく事について
  //      ← 半径が2変化するときと半径が3変化するときが入り乱れていたためカクついた。半径が2変化するときで固定した。
  s32 left   = m_pokeModelCurrInFrameCenterX - m_pokeModelCurrInFrameLength/2 * m_pokeModelCurrInFrameScale - m_pokeModelCurrInFrameAddSize;
  s32 top    = m_pokeModelCurrInFrameCenterY - m_pokeModelCurrInFrameLength/2 * m_pokeModelCurrInFrameScale - m_pokeModelCurrInFrameAddSize;
  s32 right  = m_pokeModelCurrInFrameCenterX + m_pokeModelCurrInFrameLength/2 * m_pokeModelCurrInFrameScale + m_pokeModelCurrInFrameAddSize;
  s32 bottom = m_pokeModelCurrInFrameCenterY + m_pokeModelCurrInFrameLength/2 * m_pokeModelCurrInFrameScale + m_pokeModelCurrInFrameAddSize;

  //GFL_PRINT("l=%d, t=%d, r=%d, b=%d, lr=%d, tb=%d\n", left, top, right, bottom, (left+right)/2, (top+bottom)/2);

  if(m_pokeSimpleModelInFrame) m_pokeSimpleModelInFrame->SetInFrameFrame(left, top, right, bottom);  // @fix cov_ctr[10998]: NULLチェック追加
}
void ZukanTool::updatePokeModelPosAnim(void)
{
  //static const s32 MOVE_VAL = 30;
  static const s32 MOVE_VAL = 3000;  // 酔うので一瞬で移動

  if(m_pokeModelPosAnimFlag)
  {
    bool nextFlag = false;
    s32 nextCenter[2] = { 0 };
    
    for(s32 i=0; i<2; ++i)
    {
      s32* pPokeModelCurrInFrameCenter = &m_pokeModelCurrInFrameCenterX;
      if(i == 1)
      {
        pPokeModelCurrInFrameCenter = &m_pokeModelCurrInFrameCenterY;
      }

      if(*pPokeModelCurrInFrameCenter > POKE_MODEL_INFRAME_CENTER[m_pokeModelGoalPosType][i])
      {
        nextCenter[i]  = *pPokeModelCurrInFrameCenter - MOVE_VAL;
        if(nextCenter[i] <= POKE_MODEL_INFRAME_CENTER[m_pokeModelGoalPosType][i])
        {
          nextCenter[i] = POKE_MODEL_INFRAME_CENTER[m_pokeModelGoalPosType][i];
        }
        else
        {
          nextFlag = true;
        }
      }
      else if(*pPokeModelCurrInFrameCenter < POKE_MODEL_INFRAME_CENTER[m_pokeModelGoalPosType][i])
      {
        nextCenter[i] = *pPokeModelCurrInFrameCenter + MOVE_VAL;
        if(nextCenter[i] >= POKE_MODEL_INFRAME_CENTER[m_pokeModelGoalPosType][i])
        {
          nextCenter[i] = POKE_MODEL_INFRAME_CENTER[m_pokeModelGoalPosType][i];
        }
        else
        {
          nextFlag = true;
        }
      }
      else
      {
        nextCenter[i] = POKE_MODEL_INFRAME_CENTER[m_pokeModelGoalPosType][i];
      }
    }

    m_pokeModelPosAnimFlag = nextFlag;
    m_pokeModelCurrInFrameCenterX = nextCenter[0];
    m_pokeModelCurrInFrameCenterY = nextCenter[1];

    this->setPokeModelInFrameFrame();
  }
}

void ZukanTool::setPokeModelInFrameUserValue(void)
{
  if(m_pokeSimpleModelInFrame)
  {
    if( m_pokeModelGoalPosType != POKE_MODEL_POS_TYPE_RIGHT )
    {
      // System::Camera::CModelViewerInFrame::DRAW_POSITION_CENTER
      //s32 tableIndex = -1;
      //if(!m_pokeModelSp.isEgg) tableIndex = GetInFrameUserValueTableIndex(m_pokeModelSp.monsNo, m_pokeModelSp.formNo);  // GFNMCat[5561] 手持ちボックス選択：タマゴの位置が中身によって変わる  ←このコメントアウトされているコードに変えれば修正されます。nijiでは修正しなかったのでコメントアウトしてあります。
      s32 tableIndex = GetInFrameUserValueTableIndex(m_pokeModelSp.monsNo, m_pokeModelSp.formNo);
      if(tableIndex >= 0)
      {
        m_pokeSimpleModelInFrame->GetInFrameSys()->SetUserPosOffset(INFRAME_USER_VALUE_TABLE[tableIndex].posOffset);
        m_pokeSimpleModelInFrame->GetInFrameSys()->SetUserScale(INFRAME_USER_VALUE_TABLE[tableIndex].scale);
      }
      else
      {
        m_pokeSimpleModelInFrame->GetInFrameSys()->ResetUserPosOffset();
        m_pokeSimpleModelInFrame->GetInFrameSys()->ResetUserScale();
      }
    }
    else
    {
      // System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM
      m_pokeSimpleModelInFrame->GetInFrameSys()->ResetUserPosOffset();
      m_pokeSimpleModelInFrame->GetInFrameSys()->ResetUserScale();
    }
  }
}

// @fix GFNMCat[5175] 図鑑登録：ゼルネアス、キュレムのアニメがかわいがりアニメになっていない。  →かわいがりアニメにした。
bool ZukanTool::checkUsingKawaigariInAppForZukan( MonsNo monsNo , pml::FormNo formNo )
{
  //////////////////////////////////////////////////////////////
  //
  // git_program/niji_project/prog/PokeTool/source/PokeTool.cpp
  // bool CheckUsingKawaigariInApp( MonsNo monsNo , pml::FormNo formNo )
  // を図鑑専用にした関数です。
  // PokeTool::CheckUsingKawaigariInAppの変更に目を光らせておいて下さい。
  //
  //////////////////////////////////////////////////////////////

  if( monsNo == MONSNO_ZERUNEASU && formNo == FORMNO_ZERUNEASU_NORMAL )
  {
    return true;
  }
  
  if( monsNo == MONSNO_KYUREMU &&
     (formNo == FORMNO_KYUREMU_WHITE || formNo == FORMNO_KYUREMU_BLACK ))
  {
    return true;
  } 
  return false;
}
//@}


//! @name  パーソナル外ポケモンデータ
//@{
/*
bool ZukanTool::CreatePokeExtendData(app::util::Heap* appHeap)
{
  if(m_pokeExtendData == NULL)
  {
    m_pokeExtendData = GFL_NEW(appHeap->GetDeviceHeap()) PokeTool::ExtendData(appHeap->GetDeviceHeap());
    m_pokeExtendData->Load(appHeap->GetDeviceHeap());
  }
  return true;
}
bool ZukanTool::DestroyPokeExtendData(void)
{
  if(m_pokeExtendData != NULL)
  {
    GFL_SAFE_DELETE(m_pokeExtendData);
  }
  return true;
}

const PokeTool::ExtendData* ZukanTool::GetPokeExtendData(void) const
{
  return m_pokeExtendData;
}
*/
const PokeTool::ExtendData* ZukanTool::GetPokeExtendData(void) const
{
  const PokeTool::ExtendData* pokeExtendData = NULL;
  if(m_pokePersonalSort) pokeExtendData = m_pokePersonalSort->GetPokemonExtendData();
  return pokeExtendData;
}
//@}
//! @name  パーソナルデータソートクラス
//@{
bool ZukanTool::CreatePokePersonalSort(app::util::Heap* appHeap)
{
  if(m_pokePersonalSort == NULL)
  {
    u32 lang = 0;  // languageは使わない
    Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
    m_pokePersonalSort = GFL_NEW(appHeap->GetDeviceHeap()) PokeTool::PersonalSort(lang, svZukanData, appHeap->GetDeviceHeap(), PokeTool::PersonalSort::LOAD_TYPE_FULL, PokeTool::PersonalSort::MODE_ZUKAN);
  }
  return true;
}
bool ZukanTool::DestroyPokePersonalSort(void)
{
  if(m_pokePersonalSort != NULL)
  {
    GFL_SAFE_DELETE(m_pokePersonalSort);
  }
  return true;
}

PokeTool::PersonalSort* ZukanTool::GetPokePersonalSort(void) const
{
  return m_pokePersonalSort;
}
//@}


//! @name  メッセージ
//@{
bool ZukanTool::CreateMsgSys(app::util::Heap* appHeap)
{
  bool ret = false;
  switch(m_msgSysState)
  {
  case MsgSysState::NONE:
    {
      u32 bitId = PRELOAD_PLACENAME;
      m_wordSetLoader = GFL_NEW(appHeap->GetDeviceHeap()) WordSetLoader(appHeap->GetDeviceHeap(), bitId);
      m_wordSetLoader->StartLoad(appHeap->GetDeviceHeap());
      m_msgSysState = MsgSysState::CREATING;
      //breakしない
    }
  case MsgSysState::CREATING:
    {
      if( m_wordSetLoader->WaitLoad() )
      {
        // m_wordSet
        m_wordSet = GFL_NEW(appHeap->GetDeviceHeap()) print::WordSet(print::WordSet::DEFAULT_WORD_MAX, print::WordSet::DEFAULT_BUF_LEN, appHeap->GetDeviceHeap());
        m_wordSet->SetWordSetLoader( m_wordSetLoader );

        // m_tempStrBuf
        for(u32 i=0; i<TEMP_STR_BUF_NUM; ++i)
        {
          m_tempStrBuf[i] = GFL_NEW(appHeap->GetDeviceHeap()) gfl2::str::StrBuf(TEMP_STR_BUF_CHAR_NUM_MAX, appHeap->GetDeviceHeap());
        }

        m_msgSysState = MsgSysState::WORKING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case MsgSysState::WORKING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に生成しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyMsgSys(void)
{
  bool ret = false;
  switch(m_msgSysState)
  {
  case MsgSysState::WORKING:
    {
      for(u32 i=0; i<TEMP_STR_BUF_NUM; ++i)
      {
        GFL_SAFE_DELETE(m_tempStrBuf[i]);
      }
      GFL_SAFE_DELETE(m_wordSet);
      GFL_SAFE_DELETE(m_wordSetLoader);
      m_msgSysState = MsgSysState::NONE;
      //breakしない
    }
  case MsgSysState::NONE:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}

print::WordSet*    ZukanTool::GetWordSet(void) const
{
  return m_wordSet;
}
//gfl2::str::StrBuf* ZukanTool::GetTempStrBuf(u32 index) const
//{
//  return m_tempStrBuf[index];
//}

void ZukanTool::CreateMsgData(MsgType msgType, const void* buf, gfl2::heap::HeapBase* heap, ZukanType::ZukanLang zukanLang)
{ 
  static const System::MSGLANGID SYSTEM_MSG_LANG_ID[ZukanType::ZUKAN_LANG_NUM] =  // git_program/niji_project/prog/System/include/PokemonLanguage.h
  {
    System::MSGLANGID_KANA,  // ひらがなに限定する        // 【sangoのひらがな/漢字判別方法】bool isKanji = m_gameData->GetConfig()->IsKanjiMode();    xy::MSGLANGID id = GetPmLanguageMsgLangId(langId, isKanji);    print::GetMessageArcId(id)
    System::MSGLANGID_ENGLISH,
    System::MSGLANGID_FRENCH,
    System::MSGLANGID_ITALIAN,
    System::MSGLANGID_GERMAN,
    System::MSGLANGID_SPANISH,
    System::MSGLANGID_KOREAN,
    System::MSGLANGID_CHINA,
    System::MSGLANGID_TAIWAN,
  };

  gfl2::fs::ArcFile::ARCDATID MSG_ARCDATID[MSG_TYPE_NUM] =
  {
    GARC_message_zukan_DAT,
    GARC_message_monsname_DAT,
    GARC_message_zkn_type_DAT,
    GARC_message_zkn_form_DAT,
    GARC_message_zkn_height_DAT,
    GARC_message_zkn_weight_DAT,
    GARC_message_zukan_comment_A_DAT,  // 後でカセットバージョンを見て書き換える
    GARC_message_initial_DAT,
    GARC_message_zukan_hyouka_DAT,
  };

  u8 cassetteVersion = System::GetVersion();
  if(cassetteVersion == VERSION_SUN2)
  {
    MSG_ARCDATID[MSG_TYPE_POKE_COMMENT] = GARC_message_zukan_comment_A_DAT;
  }
  else  // if(cassetteVersion == VERSION_MOON)
  {
    MSG_ARCDATID[MSG_TYPE_POKE_COMMENT] = GARC_message_zukan_comment_B_DAT;
  }

  ZukanType::ZukanLang lang = m_gameLang;
  if(zukanLang != ZukanType::ZUKAN_LANG_DEFAULT) lang = zukanLang;
 

  if(m_msgData[msgType][lang] == NULL)
  {
    if(buf != NULL)
    {
      m_msgData[msgType][lang] = GFL_NEW(heap) gfl2::str::MsgData(buf, heap);
    }
    else
    {
      m_msgData[msgType][lang] = GFL_NEW(heap) gfl2::str::MsgData(
          print::GetMessageArcId(SYSTEM_MSG_LANG_ID[lang]),
          MSG_ARCDATID[msgType],
          heap,
          gfl2::str::MsgData::LOAD_PART
      );  // 同期読み込み
    }
  }
}
void ZukanTool::DestroyMsgData(MsgType msgType, ZukanType::ZukanLang zukanLang)
{
  ZukanType::ZukanLang lang = m_gameLang;
  if(zukanLang != ZukanType::ZUKAN_LANG_DEFAULT) lang = zukanLang;

  GFL_SAFE_DELETE(m_msgData[msgType][lang]);
}
gfl2::str::MsgData* ZukanTool::GetMsgData(MsgType msgType, ZukanType::ZukanLang zukanLang) const
{
  ZukanType::ZukanLang lang = m_gameLang;
  if(zukanLang != ZukanType::ZUKAN_LANG_DEFAULT) lang = zukanLang;

  return m_msgData[msgType][lang];
}

void ZukanTool::ExpandStrBuf(const ExpandStrBufParam& param)
{
  gfl2::str::MsgData* msgData = this->GetMsgData(param.srcMsgType, param.zukanLang);

  msgData->GetString(param.srcMsgStrId, *(m_tempStrBuf[0]));

  for(u32 regBufId=0; regBufId<3; ++regBufId)
  {
    switch(param.srcRegBufType[regBufId])
    {
    case REG_BUF_TYPE_POKE_NICK_NAME:
      {
        m_wordSet->RegisterPokeNickName(regBufId, param.srcCP[regBufId]);
        break;
      }
    case REG_BUF_TYPE_POKE_MONS_NAME:
      {
        m_wordSet->RegisterPokeMonsName(regBufId, param.srcCP[regBufId]);
        break;
      }
    case REG_BUF_TYPE_PLACE_NAME:
      {
        m_wordSet->RegisterPlaceName(regBufId, param.srcPlaceNameID[regBufId]);
        break;
      }
    case REG_BUF_TYPE_ITEM_NAME:
      {
        m_wordSet->RegisterItemName(regBufId, param.srcItemID[regBufId], 1);  // アイテムの個数は1個限定
        break;
      }
    case REG_BUF_TYPE_PLAYER_NAME:
      {
        m_wordSet->RegisterPlayerName(regBufId, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus());
        break;
      }
    case REG_BUF_TYPE_PLAYER_NICKNAME:
      {
        m_wordSet->RegisterPlayerNickName(regBufId, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu());
        break;
      }
    }
  }

  m_wordSet->Expand(param.dstStrBuf, m_tempStrBuf[0]);
}
//@}


//! @name  2ボタンビュー
//@{
bool ZukanTool::CreateTwoButtonsView(app::util::Heap* appHeap, void* menuWindow2Buf)
{
  bool ret = false;
  switch(m_twoButtonsViewState)
  {
  /*
  case TwoButtonsViewState::NONE:
    {
      m_twoButtonsView = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::TwoButtonsView(appHeap, m_appRenderingManager);
      m_twoButtonsViewState = TwoButtonsViewState::CREATING;
      //breakしない
    }
  case TwoButtonsViewState::CREATING:
    {
      if( m_twoButtonsView->ObserveSetup() )
      {
        m_twoButtonsViewState = TwoButtonsViewState::WORKING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  */
  case TwoButtonsViewState::NONE:
    {
      GFL_ASSERT(m_menuCursorBuf != NULL);  // 開発中に気付かせるためのASSERT
      // ARCID_MENU_CURSORのGARC_MenuCursor_MenuCursor_applyt_COMPと同等のものを読み込み済みのバッファ
      
      m_twoButtonsView = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::TwoButtonsView(appHeap, m_appRenderingManager, false);
      m_twoButtonsView->Setup(menuWindow2Buf, m_menuCursorBuf);
      m_twoButtonsViewState = TwoButtonsViewState::CREATING;
      //breakしない
    }
  case TwoButtonsViewState::CREATING:
    {
      m_twoButtonsViewState = TwoButtonsViewState::WORKING;
      //breakしない
    }
  case TwoButtonsViewState::WORKING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に生成しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyTwoButtonsView(void)
{
  bool ret = false;
  switch(m_twoButtonsViewState)
  {
  case TwoButtonsViewState::WORKING:
    {
      GFL_SAFE_DELETE(m_twoButtonsView);
      m_twoButtonsViewState = TwoButtonsViewState::NONE;
      //breakしない
    }
  case TwoButtonsViewState::NONE:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}

void ZukanTool::OpenTwoButtonsView(
    app::ui::UIView*                          parentView,
    gfl2::str::StrBuf*                        strBuf1,
    gfl2::str::StrBuf*                        strBuf2,
    bool                                      button2UseBButton,
    app::tool::TwoButtonsView::IEventHandler* eventHandler
)
{
  if(m_twoButtonsViewState == TwoButtonsViewState::WORKING)
  {
    parentView->AddSubView(m_twoButtonsView);  // app::ui::UIViewのメンバ

    m_twoButtonsView->CursorMoveTo(0);  // 必ず上側をカーソルが指しているようにしておく

    m_twoButtonsView->SetTextBoxString(app::tool::TwoButtonsView::TEXTBOX_Button1, strBuf1);
    m_twoButtonsView->SetTextBoxString(app::tool::TwoButtonsView::TEXTBOX_Button2, strBuf2);
    if(button2UseBButton)
    {
      m_twoButtonsView->RegisterShortcut_B(app::tool::TwoButtonsView::TEXTBOX_Button2);
      m_twoButtonsView->SetButtonSE_Preset(app::tool::TwoButtonsView::SEPRESET_Decide_Cancel);
    }
    else
    {
      m_twoButtonsView->SetButtonSE_Preset(app::tool::TwoButtonsView::SEPRESET_Decide);
    }

    m_twoButtonsView->SetEventHandler(eventHandler);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていないので使い方が間違っている。
  }
}
void ZukanTool::CloseTwoButtonsView(void)
{
  if(m_twoButtonsViewState == TwoButtonsViewState::WORKING)
  {
    m_twoButtonsView->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていないので使い方が間違っている。
  }
}
//@} 


//! @name  マルチボタンビュー(GeneralLowerView, 共通下画面View)
//@{
ZukanTool::MultiButtonsViewResourceProvider::MultiButtonsViewResourceProvider(void)
  : app::tool::IResourceProvider(),
    m_menuWindow2Buf(NULL),
    m_menuCursorBuf(NULL)
{}
void* ZukanTool::MultiButtonsViewResourceProvider::IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID)
{
  void* buf = NULL;
  if(arcID==ARCID_MENU_WINDOW2 && arcDatID==GARC_MenuWindow2_MenuWindow2_applyt_COMP)
  {
    buf = m_menuWindow2Buf;
  }
  else if(arcID==ARCID_MENU_CURSOR && arcDatID==GARC_MenuCursor_MenuCursor_applyt_COMP)
  {
    buf = m_menuCursorBuf;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。こちらが用意していないリソースを求められている。
  }
  GFL_ASSERT(buf!=NULL);  // 開発中に気付かせるためのASSERT。バッファを設定していないときに呼び出された。
  return buf;
}

bool ZukanTool::CreateMultiButtonsView(app::util::Heap* appHeap, void* menuWindow2Buf)
{
  bool ret = false;
  switch(m_multiButtonsViewState)
  {
  /*
  case MultiButtonsViewState::NONE:
    {
      m_multiButtonsView = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::GeneralLowerView(appHeap, m_appRenderingManager);
      m_multiButtonsViewState = MultiButtonsViewState::CREATING;
      //breakしない
    }
  case MultiButtonsViewState::CREATING:
    {
      if( m_multiButtonsView->ObserveSetup() )
      {
        m_multiButtonsView->SetLayoutMode(app::tool::GeneralLowerView::LAYOUTMODE_4Buttons, false, true);
        m_multiButtonsViewState = MultiButtonsViewState::WORKING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  */
  case MultiButtonsViewState::NONE:
    {
      GFL_ASSERT(m_menuCursorBuf != NULL);  // 開発中に気付かせるためのASSERT
      // ARCID_MENU_CURSORのGARC_MenuCursor_MenuCursor_applyt_COMPと同等のものを読み込み済みのバッファ
    
      m_multiButtonsViewResourceProvider = GFL_NEW(appHeap->GetDeviceHeap()) MultiButtonsViewResourceProvider();
      m_multiButtonsView = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::GeneralLowerView(appHeap, m_appRenderingManager);
      m_multiButtonsViewState = MultiButtonsViewState::CREATING;

      m_multiButtonsViewResourceProvider->m_menuWindow2Buf = menuWindow2Buf;
      m_multiButtonsViewResourceProvider->m_menuCursorBuf  = m_menuCursorBuf;
      m_multiButtonsView->SetResourceProvider(m_multiButtonsViewResourceProvider);
      //breakしない
    }
  case MultiButtonsViewState::CREATING:
    {
      if( m_multiButtonsView->ObserveSetup() )
      {
        m_multiButtonsView->SetLayoutMode(app::tool::GeneralLowerView::LAYOUTMODE_4Buttons, false, true);
        m_multiButtonsViewState = MultiButtonsViewState::WORKING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case MultiButtonsViewState::WORKING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に生成しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyMultiButtonsView(void)
{
  bool ret = false;
  switch(m_multiButtonsViewState)
  {
  case MultiButtonsViewState::WORKING:
    {
      GFL_SAFE_DELETE(m_multiButtonsViewResourceProvider);
      GFL_SAFE_DELETE(m_multiButtonsView);
      m_multiButtonsViewState = MultiButtonsViewState::NONE;
      //breakしない
    }
  case MultiButtonsViewState::NONE:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}

void ZukanTool::OpenMultiButtonsView(
    app::ui::UIView*                            parentView,
    gfl2::str::StrBuf*                          strBuf0,
    gfl2::str::StrBuf*                          strBuf1,
    gfl2::str::StrBuf*                          strBuf2,
    gfl2::str::StrBuf*                          strBuf3,
    bool                                        button3UseBButton,
    bool                                        button1UseBButtonDecideSE,
    app::tool::GeneralLowerView::IEventHandler* eventHandler,
    s32                                         cursorPos
)
{
  if(m_multiButtonsViewState == MultiButtonsViewState::WORKING)
  {
    parentView->AddSubView(m_multiButtonsView);  // app::ui::UIViewのメンバ

    m_multiButtonsView->CursorMoveTo(cursorPos);  // カーソルが指しているところ

    m_multiButtonsView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button0, strBuf0);
    m_multiButtonsView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button1, strBuf1);
    m_multiButtonsView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button2, strBuf2);
    m_multiButtonsView->SetTextBoxString(app::tool::GeneralLowerView::TEXTBOX_Button3, strBuf3);

    m_multiButtonsView->SetButtonSEtoDefault();

    if(button3UseBButton)
    {
      m_multiButtonsView->RegisterShortcut_B(app::tool::GeneralLowerView::TEXTBOX_Button3);
      m_multiButtonsView->GetButtonManager()->SetButtonSelectSE(app::tool::GeneralLowerView::TEXTBOX_Button3, SEQ_SE_CANCEL1);
    }
    else if(button1UseBButtonDecideSE)
    {
      m_multiButtonsView->RegisterShortcut_B(app::tool::GeneralLowerView::TEXTBOX_Button1);
    }

    m_multiButtonsView->SetEventHandler(eventHandler);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていないので使い方が間違っている。
  }
}
void ZukanTool::CloseMultiButtonsView(void)
{
  if(m_multiButtonsViewState == MultiButtonsViewState::WORKING)
  {
    m_multiButtonsView->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていないので使い方が間違っている。
  }
}
//@} 


//! @name  フォント
//@{
bool ZukanTool::CreateFont(app::util::Heap* appHeap, FontType fontType)
{
  gfl2::str::Font* (*GetFont[])(void) =
  {
    print::SystemFont_GetNumFont,
  };
  void (*LoadFont[])(gfl2::heap::HeapBase* heapMem, gfl2::heap::HeapBase* devMem) =
  {
    print::SystemFont_LoadNumFont,
  };
  bool (*IsLoadFont[])(void) =
  {
    print::SystemFont_IsLoadNumFont,
  };
  void (*SetLoadFont[])(gfl2::heap::HeapBase* heapMem) =
  {
    print::SystemFont_SetLoadNumFont,
  };
  void (*SetLytSystemFont[])(void) =
  {
    print::SystemFont_SetLytSystemNumFont,
  };

  bool ret = false;
  switch(m_fontState[fontType])
  {
  case FontState::NONE:
    {
      if( GetFont[fontType]() == NULL )
      {
        LoadFont[fontType]( appHeap->GetDeviceHeap(), appHeap->GetDeviceHeap() );
        m_fontState[fontType] = FontState::CREATING;
      }
      else
      {
        //GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。他のところで生成されている。ケア済み。
        m_fontState[fontType] = FontState::ALREADY_EXISTING;
      }
      //breakしない
    }
  case FontState::CREATING:
    {
      if( IsLoadFont[fontType]() )
      {
        SetLoadFont[fontType]( appHeap->GetDeviceHeap() );
        SetLytSystemFont[fontType]();
        m_fontState[fontType] = FontState::WORKING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case FontState::WORKING:
    {
      ret = true;
      break;
    }
  case FontState::ALREADY_EXISTING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に生成しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyFont(FontType fontType)
{
  void (*ResetFontInfo[])(void) =
  {
    print::SystemFont_ResetNumFontInfo,
  };
  void (*DeleteFont[])(void) =
  {
    print::SystemFont_DeleteNumFont,
  };

  bool ret = false;
  switch(m_fontState[fontType])
  {
  case FontState::WORKING:
    {
      ResetFontInfo[fontType]();
      DeleteFont[fontType]();
      m_fontState[fontType] = FontState::NONE;
      //breakしない
    }
  case FontState::NONE:
    {
      ret = true;
      break;
    }
  case FontState::ALREADY_EXISTING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
//@}


//! @name  共通グラフィックアイコン
//@{
bool ZukanTool::CreateCommonGrpIcon(app::util::Heap* appHeap)
{
  static const gfl2::fs::ToolDefine::ArcLangType::Tag ARC_LANG[ZukanType::ZUKAN_LANG_NUM] =
  {
    gfl2::fs::ToolDefine::ArcLangType::JPN,
    gfl2::fs::ToolDefine::ArcLangType::USA,
    gfl2::fs::ToolDefine::ArcLangType::FRA,
    gfl2::fs::ToolDefine::ArcLangType::ITA,
    gfl2::fs::ToolDefine::ArcLangType::DEU,
    gfl2::fs::ToolDefine::ArcLangType::ESP,
    gfl2::fs::ToolDefine::ArcLangType::KOR,
    gfl2::fs::ToolDefine::ArcLangType::CHN,
    gfl2::fs::ToolDefine::ArcLangType::TWN,
  };

  bool ret = false;
  switch(m_commonGrpIconState)
  {
  case CommonGrpIconState::NONE:
    { 
      m_commonGrpIconLang = m_gameLang;  // まずはm_gameLangから処理を開始する
      m_commonGrpIcon[m_commonGrpIconLang] = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::AppCommonGrpIconData();
      m_commonGrpIcon[m_commonGrpIconLang]->Initialize(appHeap, ARC_LANG[m_commonGrpIconLang]);

      m_commonGrpIconState = CommonGrpIconState::CREATING;
      //breakしない
    }
  case CommonGrpIconState::CREATING:
    {
      if( m_commonGrpIcon[m_commonGrpIconLang]->IsReady() )
      {
        // 全言語分読み込んでいる
        // 次の言語
        if(m_commonGrpIconLang == m_gameLang)
        {
          m_commonGrpIconLang = static_cast<ZukanType::ZukanLang>(0);
        }
        else
        {
          m_commonGrpIconLang = static_cast<ZukanType::ZukanLang>( static_cast<s32>(m_commonGrpIconLang) +1 );
        }
          
        if(m_commonGrpIconLang == m_gameLang) m_commonGrpIconLang = static_cast<ZukanType::ZukanLang>( static_cast<s32>(m_commonGrpIconLang) +1 );  // m_gameLangは最初に済ませてある
        if(static_cast<s32>(m_commonGrpIconLang) >= static_cast<s32>(ZukanType::ZUKAN_LANG_NUM))  // 全部終わった
        {
          m_commonGrpIconState = CommonGrpIconState::WORKING;
          //breakしない
        }
        else
        {
          m_commonGrpIcon[m_commonGrpIconLang] = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::AppCommonGrpIconData();
          m_commonGrpIcon[m_commonGrpIconLang]->Initialize(appHeap, ARC_LANG[m_commonGrpIconLang]);
          break;
        }
      }
      else
      {
        break;
      }
    }
  case CommonGrpIconState::WORKING:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
bool ZukanTool::DestroyCommonGrpIcon(void)
{
  bool ret = false;
  switch(m_commonGrpIconState)
  {
  case CommonGrpIconState::WORKING:
    {
      for(s32 i=0; i<ZukanType::ZUKAN_LANG_NUM; ++i)
      {
        m_commonGrpIcon[i]->Terminate();
        GFL_SAFE_DELETE(m_commonGrpIcon[i]);
      }
      m_commonGrpIconState = CommonGrpIconState::NONE;
      //breakしない
    }
  case CommonGrpIconState::NONE:
    {
      ret = true;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしているので使い方が間違っている。
      break;
    }
  }
  return ret;
}
//app::tool::AppCommonGrpIconData* ZukanTool::GetCommonGrpIcon(void) const
//{
//  app::tool::AppCommonGrpIconData* ret = NULL;
//  if(m_commonGrpIconState == CommonGrpIconState::WORKING) ret = m_commonGrpIcon;
//  return ret;
//}

void ZukanTool::ReplaceTypeIconTextureByCommonGrpIcon(
    gfl2::lyt::LytPicture* pane, PokeType_tag type,
    ZukanType::ZukanLang zukanLang
) const
{
  ZukanType::ZukanLang lang = m_gameLang;
  if(zukanLang != ZukanType::ZUKAN_LANG_DEFAULT) lang = zukanLang;

  // 値チェック
  if(type<POKETYPE_ORIGIN || POKETYPE_MAX<=type)
  {
    type = POKETYPE_NULL;
  }
 
  // テクスチャを貼り替える
  if(type != POKETYPE_NULL)
  {
    pane->SetVisible(true);
    m_commonGrpIcon[lang]->ReplacePaneTextureByWazaTypeIcon(type, pane);
  }
  else
  {
    pane->SetVisible(false);
  }
}
void ZukanTool::ReplaceTypeIconTextureByCommonGrpIcon(
    gfl2::lyt::LytPicture* pane1, PokeType_tag type1, 
    gfl2::lyt::LytPicture* pane2, PokeType_tag type2,
    ZukanType::ZukanLang zukanLang
) const
{
  // 値チェック
  if(type1<POKETYPE_ORIGIN || POKETYPE_MAX<=type1)
  {
    type1 = POKETYPE_NULL;
  }
  if(type2<POKETYPE_ORIGIN || POKETYPE_MAX<=type2)
  {
    type2 = POKETYPE_NULL;
  }
  if(type1 == type2)
  {
    type2 = POKETYPE_NULL;
  }
  if(type1 == POKETYPE_NULL)
  {
    type1 = type2;
    type2 = POKETYPE_NULL;
  }

  // テクスチャを貼り替える
  this->ReplaceTypeIconTextureByCommonGrpIcon(pane1, type1, zukanLang);
  this->ReplaceTypeIconTextureByCommonGrpIcon(pane2, type2, zukanLang);
}

void ZukanTool::ReplaceLangIconTextureByCommonGrpIcon(
    gfl2::lyt::LytPicture* pane, u32 pokemonLang,
    ZukanType::ZukanLang zukanLang
) const
{
  ZukanType::ZukanLang lang = m_gameLang;
  if(zukanLang != ZukanType::ZUKAN_LANG_DEFAULT) lang = zukanLang;
  
  m_commonGrpIcon[lang]->ReplacePaneTextureByLangIcon(pokemonLang, pane);
}


//@}


//! @name  言語別MsgData
//@{
ZukanTool::LangMsgDataManager::LangMsgDataManager(gfl2::heap::HeapBase* heap, ZukanType::ZukanLang gameLang, ZukanTool* tool)
  : m_heap(heap),
    m_gameLang(gameLang),
    m_tool(tool),
    m_createStep(0)
{}

ZukanTool::LangMsgDataManager::~LangMsgDataManager()
{
  for(s32 i=0; i<ZukanTool::MSG_TYPE_NUM_FOR_INFO; ++i)
  {
    for(s32 j=0; j<ZukanType::ZUKAN_LANG_NUM; ++j)
    {
      if(m_gameLang != j)
      {
        m_tool->DestroyMsgData(static_cast<ZukanTool::MsgType>(i), static_cast<ZukanType::ZukanLang>(j));
      }
    }
  }
}

void ZukanTool::LangMsgDataManager::UpdateCreate(void)
{
  // この関数を1回呼ぶと、1個MsgDataを生成する
  bool continueFlag = false;
  do
  {
    continueFlag = false;
    if( this->IsCreated() ) break;
    s32 i = m_createStep / ZukanType::ZUKAN_LANG_NUM;
    s32 j = m_createStep % ZukanType::ZUKAN_LANG_NUM;
    ++m_createStep;
    if(m_gameLang != j)
    {
      m_tool->CreateMsgData(static_cast<ZukanTool::MsgType>(i), NULL, m_heap, static_cast<ZukanType::ZukanLang>(j));  // 同期読み込み
    }
    else
    {
      continueFlag = true;
    }
  }
  while(continueFlag);
}
bool ZukanTool::LangMsgDataManager::IsCreated(void) const
{
  return (m_createStep >= ZukanTool::MSG_TYPE_NUM_FOR_INFO * ZukanType::ZUKAN_LANG_NUM);
}
//@}

//! @name  言語別Font
//@{
ZukanTool::LangFontManager::LangFontManager(gfl2::heap::HeapBase* heap, ZukanType::ZukanLang gameLang, ZukanDataManager* dataManager)
  : m_heap(heap),
    m_gameLang(gameLang),
    m_dataManager(dataManager),
    m_createStep(0)
{
  for(s32 i=0; i<LANG_FONT_TYPE_NUM; ++i)
  {
    m_langFont[i] = NULL;
  }
  
  gfl2::str::Font* gameFont = GFL_SINGLETON_INSTANCE(print::SystemFont)->GetFont();
  switch(m_gameLang)
  {
  case ZukanType::ZUKAN_LANG_CHINA:
    {
      m_langFont[LANG_FONT_TYPE_CHINA ] = gameFont;
      break;
    }
  case ZukanType::ZUKAN_LANG_TAIWAN:
    {
      m_langFont[LANG_FONT_TYPE_TAIWAN] = gameFont;
      break;
    }
  default:
    {
      m_langFont[LANG_FONT_TYPE_JAPAN ] = gameFont;
      break;
    }
  }
}
ZukanTool::LangFontManager::~LangFontManager()
{
  gfl2::str::Font* gameFont = GFL_SINGLETON_INSTANCE(print::SystemFont)->GetFont();
  for(s32 i=0; i<LANG_FONT_TYPE_NUM; ++i)
  {
    if(m_langFont[i] != gameFont)
    {
      GFL_SAFE_DELETE(m_langFont[i]);
    }
    else
    {
      m_langFont[i] = NULL;
    }
  }
}

void ZukanTool::LangFontManager::UpdateCreate(void)
{
  // この関数を1回呼ぶと、1個Fontを生成する
  bool continueFlag = false;
  do
  {
    continueFlag = false;
    if( this->IsCreated() ) break;
    s32 i = m_createStep;
    ++m_createStep;
    if(m_langFont[i] == NULL)
    {
      gfl2::str::ResFont* resFont = GFL_NEW(m_heap) gfl2::str::ResFont();

      ZukanDataManager::DataId dataId = ZukanDataManager::DATA_ID_FONT_JAPAN;
      switch(i)
      {
      case LANG_FONT_TYPE_CHINA :
        {
          dataId = ZukanDataManager::DATA_ID_FONT_CHINA;
          break;
        }
      case LANG_FONT_TYPE_TAIWAN:
        {
          dataId = ZukanDataManager::DATA_ID_FONT_TAIWAN;
          break;
        }
      default:  // LANG_FONT_TYPE_JAPAN :
        {
          dataId = ZukanDataManager::DATA_ID_FONT_JAPAN;
          break;
        }
      }

      resFont->SetResource(m_dataManager->GetDataBuf(dataId));
      m_langFont[i] = resFont;
    }
    else
    {
      continueFlag = true;
    }
  }
  while(continueFlag);
}
bool ZukanTool::LangFontManager::IsCreated(void) const
{
  return (m_createStep >= LANG_FONT_TYPE_NUM);
}

gfl2::str::Font* ZukanTool::LangFontManager::GetLangFont(LangFontType langFontType) const
{
  return m_langFont[langFontType];
}
//@}


//! @name  言語別MsgData
//@{
void ZukanTool::CreateLangMsgDataManager(app::util::Heap* appHeapForInstance, gfl2::heap::HeapBase* heapForManager)
{
  if(m_langMsgDataManager == NULL)
  {
    m_langMsgDataManager = GFL_NEW(appHeapForInstance->GetDeviceHeap()) LangMsgDataManager(heapForManager, m_gameLang, this);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}
void ZukanTool::DestroyLangMsgDataManager(void)
{
  GFL_SAFE_DELETE(m_langMsgDataManager);
}
//@}
 
//! @name  言語別Font
//@{
void ZukanTool::CreateLangFontManager(app::util::Heap* appHeapForInstance, gfl2::heap::HeapBase* heapForManager, ZukanDataManager* dataManager)
{
  if(m_langFontManager == NULL)
  {
    m_langFontManager = GFL_NEW(appHeapForInstance->GetDeviceHeap()) LangFontManager(heapForManager, m_gameLang, dataManager);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}
void ZukanTool::DestroyLangFontManager(void)
{
  GFL_SAFE_DELETE(m_langFontManager);
}
//@}


#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
//! @name  デバッグ用インスタンス
//@{
void ZukanTool::CreateDebugInstance(gfl2::heap::HeapBase* heap)
{
  if(m_monsNameStrBuf == NULL)
  {
    m_monsNameStrBuf = GFL_NEW(heap) gfl2::str::StrBuf(32, heap);  // ポケモン名に使うのでそれに足りる文字数でいい
  }
}
void ZukanTool::DestroyDebugInstance(void)
{
  GFL_SAFE_DELETE(m_monsNameStrBuf);
}
//@}
#endif


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

