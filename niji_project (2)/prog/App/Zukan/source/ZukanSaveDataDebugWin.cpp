#if PM_DEBUG


//==============================================================================
/**
 @file    ZukanSaveDataDebugWin.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.05.25
 @brief   図鑑セーブデータのデバッグウィンドウ
 */
//==============================================================================


// gfl2のインクルード
#include <str/include/gfl2_StrBuf.h>

#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>

// pmlのインクルード
#include <pml/include/personal/pml_PersonalData.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/poke_lib/monsno_def.h>

// nijiのインクルード
#include <PokeTool/include/PokeToolExtendData.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ZukanSave.h>
#include <System/include/PokemonVersion.h>
#include <System/include/GflUse.h>

// メッセージのインデックス
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// 図鑑のインクルード
#include "ZukanSaveDataDebugWin.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanSaveDataDebugWinImpl
 @brief     図鑑セーブデータのデバッグウィンドウの実装
 */
//==============================================================================
class ZukanSaveDataDebugWinImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanSaveDataDebugWinImpl);

public:
  ///////////////////////////////////////
  // コンストラクタ、デストラクタ
  ///////////////////////////////////////
  ZukanSaveDataDebugWinImpl(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup, gfl2::str::MsgData* pMsgData, Savedata::ZukanData* svZukanData);
  virtual ~ZukanSaveDataDebugWinImpl();

private:
  ///////////////////////////////////////
  // 初期化処理、終了処理
  ///////////////////////////////////////
  static void Initialize(ZukanSaveDataDebugWinImpl* inst);
  static void Terminate(void);

  ///////////////////////////////////////
  // 更新、描画
  ///////////////////////////////////////
  static void     MonsNo_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* MonsNo_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     FormNo_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* FormNo_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static void     None_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* None_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Str_Draw   (void* userWork, gfl2::debug::DebugWinItem* item);
  
  static wchar_t* MonsGetSee_Draw(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* MonsLang_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* FormSee_Draw   (void* userWork, gfl2::debug::DebugWinItem* item);
 
  ///////////////////////////////////////
  // 便利関数
  ///////////////////////////////////////

  //! @brief  s32の更新
  //! @param[in] minVal  取り得る最小値
  //! @param[in] maxVal  取り得る最大値
  //! @param[in] isLoop  trueのとき、minVal, maxValに到達したときに反対側に回り込む
  //! @param[in] isJump  trueのとき、Lボタンを押しながらで10ずつ増減、Rボタンを押しながらで100ずつ増減
  static s32 Util_S32_Update(s32 currVal, s32 minVal, s32 maxVal, bool isLoop, bool isJump);

  static void Poke_UpdateSexVector(s32 prevMonsNo, s32 prevFormNo, s32 currMonsNo, s32 currFormNo);

private:
  ///////////////////////////////////////
  // static変数
  ///////////////////////////////////////
  static s32 s_monsNo;
  static s32 s_formNo;
  static u32 s_sexVector;

  static ZukanSaveDataDebugWinImpl* s_inst;

private:
  gfl2::heap::HeapBase*       m_heap;
  gfl2::debug::DebugWinGroup* m_parentGroup;
  gfl2::str::MsgData*         m_pMsgData;
  Savedata::ZukanData*        m_svZukanData;

  gfl2::debug::DebugWinGroup* m_zukanSaveDataGroup;

  static const s32 STR_BUF_NUM = 4;
  gfl2::str::StrBuf* m_strBuf[STR_BUF_NUM];
  static const s32 SMALL_STR_BUF_NUM = 9;
  gfl2::str::StrBuf* m_smallStrBuf[SMALL_STR_BUF_NUM];
};




//==============================================================================
//
//  ZukanSaveDataDebugWinImpl
//
//==============================================================================

///////////////////////////////////////
// static変数
///////////////////////////////////////
s32 ZukanSaveDataDebugWinImpl::s_monsNo = 0;
s32 ZukanSaveDataDebugWinImpl::s_formNo = 0;
u32 ZukanSaveDataDebugWinImpl::s_sexVector = 0;

ZukanSaveDataDebugWinImpl* ZukanSaveDataDebugWinImpl::s_inst = NULL;

///////////////////////////////////////
// コンストラクタ、デストラクタ
///////////////////////////////////////
ZukanSaveDataDebugWinImpl::ZukanSaveDataDebugWinImpl(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup, gfl2::str::MsgData* pMsgData, Savedata::ZukanData* svZukanData)
  : m_heap(heap),
    m_parentGroup(parentGroup),
    m_pMsgData(pMsgData),
    m_svZukanData(svZukanData),
    m_zukanSaveDataGroup(NULL)
{
  Initialize(this);
  
  // 親
  m_zukanSaveDataGroup = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_zukan_sv_top, parentGroup);

  // 子
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_00, NULL, MonsNo_Update, MonsNo_Draw);
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_01, NULL, FormNo_Update, FormNo_Draw);
  
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_00, NULL, None_Update, None_Draw);
  
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_00, NULL, None_Update, Str_Draw       );
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_00, NULL, None_Update, MonsGetSee_Draw);
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_00, reinterpret_cast<void*>(0), None_Update, MonsLang_Draw  );
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_00, reinterpret_cast<void*>(1), None_Update, MonsLang_Draw  );

  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_01, NULL, None_Update, Str_Draw    );
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSaveDataGroup, m_heap, m_pMsgData, msg_dmenu_zukan_sv_00, NULL, None_Update, FormSee_Draw);

  // インスタンス
  for(s32 i=0; i<STR_BUF_NUM; ++i)
  {
    m_strBuf[i] = GFL_NEW(m_heap) gfl2::str::StrBuf(24, m_heap);
  }
  for(s32 i=0; i<SMALL_STR_BUF_NUM; ++i)
  {
    m_smallStrBuf[i] = GFL_NEW(m_heap) gfl2::str::StrBuf(8, m_heap);
  }
}
ZukanSaveDataDebugWinImpl::~ZukanSaveDataDebugWinImpl()
{
  for(s32 i=0; i<SMALL_STR_BUF_NUM; ++i)
  {
    GFL_SAFE_DELETE(m_smallStrBuf[i]);
  }
  for(s32 i=0; i<STR_BUF_NUM; ++i)
  {
    GFL_SAFE_DELETE(m_strBuf[i]);
  }

  gfl2::debug::DebugWin_RemoveGroup(m_zukanSaveDataGroup);
  
  Terminate();
}

///////////////////////////////////////
// 初期化処理、終了処理
///////////////////////////////////////
void ZukanSaveDataDebugWinImpl::Initialize(ZukanSaveDataDebugWinImpl* inst)
{
  s_monsNo = MONSNO_HUSIGIDANE;
  s_formNo = 0;

  // s_sexVector
  Poke_UpdateSexVector(MONSNO_NULL, 0, s_monsNo, s_formNo);  // 最初は必ず設定されるような値を渡す

  s_inst = inst;
}
void ZukanSaveDataDebugWinImpl::Terminate(void)
{
  s_inst = NULL;
}

///////////////////////////////////////
// 更新、描画
///////////////////////////////////////
void     ZukanSaveDataDebugWinImpl::MonsNo_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  s32 prevMonsNo = s_monsNo;
  s32 prevFormNo = s_formNo;

  s_monsNo = Util_S32_Update( s_monsNo, MONSNO_HUSIGIDANE, MONSNO_END, true, true );

  if(prevMonsNo != s_monsNo) s_formNo = 0;  // monsNoを変更したらformNoは0にする

  Poke_UpdateSexVector(prevMonsNo, prevFormNo, s_monsNo, s_formNo);
}

wchar_t* ZukanSaveDataDebugWinImpl::MonsNo_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
 
  MonsNo monsNo = static_cast<MonsNo>(s_monsNo);
  pml::personal::GetMonsName(s_inst->m_strBuf[0], monsNo);

  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_17, *(s_inst->m_strBuf[1]));  // アローラ図鑑番号

  s32 alolaNo = PokeTool::ExtendData::GetLocalNumberStatic(s_monsNo, PokeTool::ExtendData::LOCAL_AREA_A);

  swprintf(
      workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%03d:%ls](%ls %03d)",
      item->GetName(), s_monsNo, s_inst->m_strBuf[0]->GetPtr(),
      s_inst->m_strBuf[1]->GetPtr(), alolaNo
  );

  return workStr;
}

void     ZukanSaveDataDebugWinImpl::FormNo_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  s32 prevMonsNo = s_monsNo;
  s32 prevFormNo = s_formNo;

  s32 formNum = s_inst->m_svZukanData->GetFormMax(static_cast<MonsNo>(s_monsNo));
  if(s_formNo >= formNum) s_formNo = 0;

  s32 minVal = 0;
  s32 maxVal = formNum-1;

  s_formNo = Util_S32_Update( s_formNo, minVal, maxVal, true, true );
  
  Poke_UpdateSexVector(prevMonsNo, prevFormNo, s_monsNo, s_formNo);
}

wchar_t* ZukanSaveDataDebugWinImpl::FormNo_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  
  s32 formNum = s_inst->m_svZukanData->GetFormMax(static_cast<MonsNo>(s_monsNo));

  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_02, *(s_inst->m_strBuf[0]));  // オス
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_03, *(s_inst->m_strBuf[1]));  // メス
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_05, *(s_inst->m_strBuf[2]));  // 性別なし

  if(    s_sexVector == pml::personal::SEX_VECTOR_ONLY_MALE
      || s_sexVector == pml::personal::SEX_VECTOR_ONLY_FEMALE
      || s_sexVector == pml::personal::SEX_VECTOR_UNKNOWN     )
  {
    s32 strBufIndex = 2;
    if( s_sexVector == pml::personal::SEX_VECTOR_ONLY_MALE )        strBufIndex = 0;
    else if( s_sexVector == pml::personal::SEX_VECTOR_ONLY_FEMALE ) strBufIndex = 1;
    
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%02d]/%02d(%ls)", item->GetName(), s_formNo, formNum, s_inst->m_strBuf[strBufIndex]->GetPtr() );
  }
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%02d]/%02d(%ls %ls)", item->GetName(), s_formNo, formNum, s_inst->m_strBuf[0]->GetPtr(), s_inst->m_strBuf[1]->GetPtr() );
  }
  
  return workStr;
}

void     ZukanSaveDataDebugWinImpl::None_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{}

wchar_t* ZukanSaveDataDebugWinImpl::None_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );
  return workStr;
}
wchar_t* ZukanSaveDataDebugWinImpl::Str_Draw   (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );
  return workStr;
}

wchar_t* ZukanSaveDataDebugWinImpl::MonsGetSee_Draw(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_07, *(s_inst->m_strBuf[0]));  // Get
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_06, *(s_inst->m_strBuf[1]));  // See

  MonsNo monsNo = static_cast<MonsNo>(s_monsNo);
  bool getFlag = s_inst->m_svZukanData->GetPokeGetFlag(monsNo);
  bool seeFlag = s_inst->m_svZukanData->GetPokeSeeFlag(monsNo);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"    %ls=%d %ls=%d",
      s_inst->m_strBuf[0]->GetPtr(), getFlag,
      s_inst->m_strBuf[1]->GetPtr(), seeFlag
  );

  return workStr;
}

wchar_t* ZukanSaveDataDebugWinImpl::MonsLang_Draw  (void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  static const u32 POKEMON_LANG_NUM = 9;
  static const u8 POKEMON_LANG_LIST[POKEMON_LANG_NUM] =  // git_program/niji_project/prog/System/include/PokemonVersion.h  POKEMON_LANG_JAPANなど
  {
    POKEMON_LANG_JAPAN  ,
    POKEMON_LANG_ENGLISH,
    POKEMON_LANG_FRANCE ,
    POKEMON_LANG_GERMANY,
    POKEMON_LANG_ITALY  ,
    POKEMON_LANG_KOREA  ,
    POKEMON_LANG_SPAIN  ,
    POKEMON_LANG_CHINA  ,
    POKEMON_LANG_TAIWAN ,
  };

  bool langFlag[POKEMON_LANG_NUM] = { 0 };
  
  s32 val = reinterpret_cast<s32>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  MonsNo monsNo = static_cast<MonsNo>(s_monsNo);
  for(u32 i=0; i<POKEMON_LANG_NUM; ++i)
  {
    langFlag[i] = s_inst->m_svZukanData->GetTextVersionUpFlag(monsNo, POKEMON_LANG_LIST[i]);
  } 
  
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_07, *(s_inst->m_strBuf[0]));  // Get

  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_08, *(s_inst->m_smallStrBuf[0]));  // JPN
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_09, *(s_inst->m_smallStrBuf[1]));  // ENG
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_10, *(s_inst->m_smallStrBuf[2]));  // FRA
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_11, *(s_inst->m_smallStrBuf[3]));  // GER
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_12, *(s_inst->m_smallStrBuf[4]));  // ITA
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_13, *(s_inst->m_smallStrBuf[5]));  // KOR
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_14, *(s_inst->m_smallStrBuf[6]));  // SPA
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_15, *(s_inst->m_smallStrBuf[7]));  // CHS
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_16, *(s_inst->m_smallStrBuf[8]));  // CHT

  if(val == 0)
  {
    swprintf(
        workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"    %ls={%ls=%d %ls=%d %ls=%d %ls=%d",
        s_inst->m_strBuf[0]->GetPtr(),
        s_inst->m_smallStrBuf[0]->GetPtr(), langFlag[0],
        s_inst->m_smallStrBuf[1]->GetPtr(), langFlag[1],
        s_inst->m_smallStrBuf[2]->GetPtr(), langFlag[2],
        s_inst->m_smallStrBuf[3]->GetPtr(), langFlag[3]
    );
  }
  else
  {
    swprintf(
        workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"        %ls=%d %ls=%d %ls=%d %ls=%d %ls=%d}",
        s_inst->m_smallStrBuf[4]->GetPtr(), langFlag[4],
        s_inst->m_smallStrBuf[5]->GetPtr(), langFlag[5],
        s_inst->m_smallStrBuf[6]->GetPtr(), langFlag[6],
        s_inst->m_smallStrBuf[7]->GetPtr(), langFlag[7],
        s_inst->m_smallStrBuf[8]->GetPtr(), langFlag[8]
    );
  }
  
  return workStr;
}

wchar_t* ZukanSaveDataDebugWinImpl::FormSee_Draw   (void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_06, *(s_inst->m_strBuf[0]));  // See

  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_02, *(s_inst->m_strBuf[1]));  // オス
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_03, *(s_inst->m_strBuf[2]));  // メス
  s_inst->m_pMsgData->GetString(msg_dmenu_zukan_sv_04, *(s_inst->m_strBuf[3]));  // レア

  MonsNo monsNo = static_cast<MonsNo>(s_monsNo);
  bool seeMaleFlag       = false;
  bool seeMaleRareFlag   = false;
  bool seeFemaleFlag     = false;
  bool seeFemaleRareFlag = false;

  if( s_sexVector == pml::personal::SEX_VECTOR_UNKNOWN )  // 性別不明の情報はオスのところに書かれている。
  {
    seeMaleFlag       = s_inst->m_svZukanData->CheckPokeSeeForm(monsNo, pml::SEX_UNKNOWN, false, s_formNo);
    seeMaleRareFlag   = s_inst->m_svZukanData->CheckPokeSeeForm(monsNo, pml::SEX_UNKNOWN, true , s_formNo);
  }
  else  // オスしかいないorメスしかいないときもこの判定でいい。図鑑セーブデータ側でいないほうのフラグは立てないので。
  {
    seeMaleFlag       = s_inst->m_svZukanData->CheckPokeSeeForm(monsNo, pml::SEX_MALE   , false, s_formNo);
    seeMaleRareFlag   = s_inst->m_svZukanData->CheckPokeSeeForm(monsNo, pml::SEX_MALE   , true , s_formNo);
    seeFemaleFlag     = s_inst->m_svZukanData->CheckPokeSeeForm(monsNo, pml::SEX_FEMALE , false, s_formNo);
    seeFemaleRareFlag = s_inst->m_svZukanData->CheckPokeSeeForm(monsNo, pml::SEX_FEMALE , true , s_formNo);
  }

  swprintf(
      workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"    %ls={%ls=%d %ls%ls=%d %ls=%d %ls%ls=%d}",
      s_inst->m_strBuf[0]->GetPtr(),
      s_inst->m_strBuf[1]->GetPtr(), seeMaleFlag  , s_inst->m_strBuf[1]->GetPtr(), s_inst->m_strBuf[3]->GetPtr(), seeMaleRareFlag  ,
      s_inst->m_strBuf[2]->GetPtr(), seeFemaleFlag, s_inst->m_strBuf[2]->GetPtr(), s_inst->m_strBuf[3]->GetPtr(), seeFemaleRareFlag
  );
  
  return workStr;
}

///////////////////////////////////////
// 便利関数
///////////////////////////////////////
s32 ZukanSaveDataDebugWinImpl::Util_S32_Update(s32 currVal, s32 minVal, s32 maxVal, bool isLoop, bool isJump)
{
  gfl2::ui::Button* uiButton = gfl2::debug::DebugWin_GetUiButton();

  s32 retVal = currVal;

  s32 delta = 1;
  if(isJump)
  {
    if( uiButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      delta = 100;
    }
    else if( uiButton->IsHold( gfl2::ui::BUTTON_L ) )
    {
      delta = 10;
    }
  }

  if(currVal == minVal)
  {
    if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
    {
      if(isLoop)
      {
        retVal = maxVal;
      }
    }
    else if( uiButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      retVal = currVal + delta;
    }
  }
  else if(currVal == maxVal)
  {
    if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
    {
      if(isLoop)
      {
        retVal = minVal;
      }
    }
    else if( uiButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      retVal = currVal - delta;
    }
  }
  else
  {
    if( uiButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      retVal = currVal + delta;
    }
    else if( uiButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      retVal = currVal - delta;
    }
  }

  if(retVal < minVal)
  {
    retVal = minVal;
  }
  else if(retVal > maxVal)
  {
    retVal = maxVal;
  }

  return retVal;
}

void ZukanSaveDataDebugWinImpl::Poke_UpdateSexVector(s32 prevMonsNo, s32 prevFormNo, s32 currMonsNo, s32 currFormNo)
{
  if(prevMonsNo != currMonsNo || prevFormNo != currFormNo)
  {
    pml::personal::LoadPersonalData( static_cast<MonsNo>(currMonsNo), currFormNo );
    s_sexVector = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex );
  }
}




//==============================================================================
//
//  ZukanSaveDataDebugWin
//
//==============================================================================
ZukanSaveDataDebugWin::ZukanSaveDataDebugWin(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup)
  : m_heap(heap),
    m_parentGroup(parentGroup),
    m_pMsgData(NULL),
    m_svZukanData(NULL),
    m_impl(NULL)
{
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  if( devWork ) m_pMsgData = devWork->GetMessageData();

  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  m_svZukanData = svZukanData;

  m_impl = GFL_NEW(heap) ZukanSaveDataDebugWinImpl(m_heap, m_parentGroup, m_pMsgData, m_svZukanData);
}
ZukanSaveDataDebugWin::~ZukanSaveDataDebugWin()
{
  GFL_SAFE_DELETE(m_impl);
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // PM_DEBUG

