//============================================================================================
/**
 * @file		box_debug_pokepara.cpp
 * @brief		デバッグ用ポケモンパラメータ全表示
 * @author	Akito Mori -> yuto uchidaが移植
 * @date		13.03.24
 *
 * namespace	app::box::
 *
 * BOX画面のR+Xデバッグメニュー中に「ポケモン全パラメータ」という欄を表示し、
 * カーソルが指しているポケモンのパラメータを表示するメニュー。
 * Xボタンを押すとBOXが終了してしまうので、BOXトレイ名のところを選択してから開始する必要が
 * ある。メニュー起動後はボタンが効かなくなる。
 */
//============================================================================================
#include "App/Box/include/BoxProc.h"

#if PM_DEBUG
// debug
#include "Debug/DebugWin/include/DebugWinSystem.h"

#include "pml/include/pokepara/pml_PokemonParamType.h"

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "GameSys/include/GameData.h"

#endif

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )


#if PM_DEBUG
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static void DebugWinFunc_BoxPokepara1Update( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara1Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara2Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara3Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara4Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara5Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara6Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara7Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara9Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara10Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara11Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara12Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara13Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara14Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara15Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara16Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara17Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara20Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara21Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara22Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara23Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara24Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara25Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara26Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara27Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara28Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara29Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara30Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara31Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara32Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara33Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara35Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara38Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara39Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara40Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara41Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara42Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara43Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara45Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara46Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara47Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara48Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara49Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara50Draw( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_BoxPokePara51Draw( void* userWork, gfl2::debug::DebugWinItem* item );


static void teamlock_Update(void* userWork, gfl2::debug::DebugWinItem* );
static void teamlock1_Update(void* userWork, gfl2::debug::DebugWinItem* );
static void teamlock2_Update(void* userWork, gfl2::debug::DebugWinItem* );
static void teamlock3_Update(void* userWork, gfl2::debug::DebugWinItem* );
static void teamlock4_Update(void* userWork, gfl2::debug::DebugWinItem* );
static void teamlock5_Update(void* userWork, gfl2::debug::DebugWinItem* );
static void teamlock6_Update(void* userWork, gfl2::debug::DebugWinItem* );
static wchar_t* teamlock_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item, int teamno);
static wchar_t* teamlock1_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item);
static wchar_t* teamlock2_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item);
static wchar_t* teamlock3_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item);
static wchar_t* teamlock4_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item);
static wchar_t* teamlock5_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item);
static wchar_t* teamlock6_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item);

//---------------------------------------------------------------------------------
// デバッグメニュー作成
//---------------------------------------------------------------------------------
void Proc::DebugMenu_Init( gfl2::heap::HeapBase *heap )
{
  m_debug_DeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

  // グループ作成
  m_debug_topGroup = gfl2::debug::DebugWin_AddGroup( heap, L"ポケモン全パラメータ", NULL, 51 );

  m_debug_LockGroup     = gfl2::debug::DebugWin_AddGroup(heap, L"LockTeam", gfl2::debug::DebugWin_GetTopGroup() );
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_debug_LockGroup, heap, L"Team1", NULL, &teamlock1_Update, &teamlock1_Draw);
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_debug_LockGroup, heap, L"Team2", NULL, &teamlock2_Update, &teamlock2_Draw);
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_debug_LockGroup, heap, L"Team3", NULL, &teamlock3_Update, &teamlock3_Draw);
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_debug_LockGroup, heap, L"Team4", NULL, &teamlock4_Update, &teamlock4_Draw);
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_debug_LockGroup, heap, L"Team5", NULL, &teamlock5_Update, &teamlock5_Draw);
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_debug_LockGroup, heap, L"Team6", NULL, &teamlock6_Update, &teamlock6_Draw);

  
  // 変数初期化
  m_dump.value[0]=0;
  m_dump.value[1]=10;
  m_coreParam = GFL_NEW_LOW(heap) pml::pokepara::CoreParam(heap);
  
  
  // アイテム追加
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"１", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara1Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"２", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara2Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"３", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara3Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"４", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara4Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"５", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara5Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"６", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara6Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"７", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara7Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"９", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara9Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ａ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara10Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｂ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara11Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｃ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara12Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｄ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara13Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｅ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara14Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｆ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara15Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｇ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara16Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｈ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara17Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｋ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara20Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｌ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara21Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｍ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara22Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｎ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara23Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｏ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara24Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｐ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara25Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｑ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara26Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｒ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara27Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｓ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara28Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｔ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara29Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｕ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara30Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｖ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara31Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｗ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara32Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｘ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara33Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"Ｚ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara35Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｃ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara38Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｄ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara39Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｅ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara40Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｆ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara41Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｇ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara42Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｈ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara43Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｊ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara45Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｋ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara46Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｌ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara47Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｍ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara48Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｎ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara49Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｏ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara50Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_debug_topGroup , heap, L"ｐ", &m_dump, DebugWinFunc_BoxPokepara1Update, DebugWinFunc_BoxPokePara51Draw );
}

//---------------------------------------------------------------------------------
// デバッグメニュー破棄
//---------------------------------------------------------------------------------
void Proc::DebugMenu_Exit( void )
{
  // グループ破棄
  gfl2::debug::DebugWin_RemoveGroup( m_debug_LockGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_debug_topGroup );
  GFL_DELETE m_coreParam;
}

//---------------------------------------------------------------------------------
// アップデート処理
//---------------------------------------------------------------------------------
void Proc::DebugMenu_Update( void )
{
  u32 tray  = m_debug_appparam->nowDispTray;
  u32 pos   = m_debug_appparam->cursorPos;

  // パラメータ更新
  if(m_dump.value[0]!=tray || m_dump.value[1]!=pos){
//    GFL_PRINT("カーソル位置が変わったので更新 tray=%d, pos=%d\n", tray, pos);
    pml::pokepara::CoreParam *poke = GetCoreParamPointer( tray, pos );
    // ポケモンパラメータの取得ができなかったら更新無
    if(poke==NULL){
      return;
    }
    
    //高速モード開始
    bool flag = poke->StartFastMode();

    m_dump.value[2] = poke->GetMonsNo();  // ポケモンNO
    m_dump.value[3] = static_cast<u32>(poke->GetFormNo());  //フォルムNO
    m_dump.value[4] = poke->GetType1();   // タイプ１
    m_dump.value[5] = poke->GetType2();   // タイプ２
    m_dump.value[6] = poke->GetItem();    // どうぐ

    m_dump.value[7] = poke->GetLevel();   // レベル
    m_dump.value[8] = poke->GetExp();     // 経験値
    m_dump.value[9] = poke->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG ); // タマゴフラグ
    m_dump.value[10] = poke->IsEgg( pml::pokepara::CHECK_ONLY_ILLEGAL_EGG );    // ダメタマゴ


    m_dump.value[11] = (poke->GetID()&0xffff0000)>>16;  // トレーナーID上位
    m_dump.value[12] = poke->GetID()&0xffff;            // トレーナーID下位
    m_dump.value[13] = (poke->GetPersonalRnd()&0xffff0000)>>16; // 個性乱数上位
    m_dump.value[14] = poke->GetPersonalRnd()&0xffff;           // 個性乱数下位
    m_dump.value[15] = (poke->GetRareRnd()&0xffff0000)>>16;     // レア乱数上位
    m_dump.value[16] = poke->GetRareRnd()&0xffff;               // レア乱数下位
    m_dump.value[17] = poke->IsRare();                          // レアか？

    m_dump.value[18] = poke->GetSick();  // 状態異常
    m_dump.value[19] = poke->GetSex(); ///< 性別
    m_dump.value[20] = poke->GetSexVector();  ///< 性別ベクトル
    m_dump.value[21] = poke->GetMaxHp();  ///< 最大HP

    m_dump.value[22] = poke->GetPower( pml::pokepara::POWER_HP    ); // 能力値：最大HP
    m_dump.value[23] = poke->GetPower( pml::pokepara::POWER_ATK   ); // 能力値：こうげき
    m_dump.value[24] = poke->GetPower( pml::pokepara::POWER_DEF   ); // 能力値：ぼうぎょ
    m_dump.value[25] = poke->GetPower( pml::pokepara::POWER_SPATK ); // 能力値：とくこう
    m_dump.value[26] = poke->GetPower( pml::pokepara::POWER_SPDEF ); // 能力値：とくぼう
    m_dump.value[27] = poke->GetPower( pml::pokepara::POWER_AGI   ); // 能力値：すばやさ

    m_dump.value[28] = poke->GetTalentPower( pml::pokepara::POWER_HP    ); ///< 個体値:最大HP
    m_dump.value[29] = poke->GetTalentPower( pml::pokepara::POWER_ATK   ); ///< 個体値:こうげき
    m_dump.value[30] = poke->GetTalentPower( pml::pokepara::POWER_DEF   ); ///< 個体値:ぼうぎょ
    m_dump.value[31] = poke->GetTalentPower( pml::pokepara::POWER_SPATK ); ///< 個体値:とくこう
    m_dump.value[32] = poke->GetTalentPower( pml::pokepara::POWER_SPDEF ); ///< 個体値:とくぼう
    m_dump.value[33] = poke->GetTalentPower( pml::pokepara::POWER_AGI   ); ///< 個体値:すばやさ

    m_dump.value[34] = poke->GetEffortPower( pml::pokepara::POWER_HP    ); ///< 努力値:最大HP
    m_dump.value[35] = poke->GetEffortPower( pml::pokepara::POWER_ATK   ); ///< 努力値:こうげき
    m_dump.value[36] = poke->GetEffortPower( pml::pokepara::POWER_DEF   ); ///< 努力値:ぼうぎょ
    m_dump.value[37] = poke->GetEffortPower( pml::pokepara::POWER_SPATK ); ///< 努力値:とくこう
    m_dump.value[38] = poke->GetEffortPower( pml::pokepara::POWER_SPDEF ); ///< 努力値:とくぼう
    m_dump.value[39] = poke->GetEffortPower( pml::pokepara::POWER_AGI   ); ///< 努力値:すばやさ
    m_dump.value[40] = poke->GetTotalEffortPower();                        ///< 努力値合計

    for(int i=0;i<4;i++){
      m_dump.value[41+3*i] = poke->GetWazaPP( i );        ///< 技N:PP
      m_dump.value[42+3*i] = poke->GetWazaMaxPP( i );     ///< 技N:PP_MAX
      m_dump.value[43+3*i] = poke->GetWazaPPUpCount( i ); ///< 技N:ポイントアップ使用回数
    }
    
    poke->GetNickName(m_dump.poke_nickname,13);                // ニックネーム
    m_dump.value[53] = poke->HaveNickName();                   // ニックネームフラグ
    poke->GetParentName( m_dump.parent_name, 13 );             // 親名を取得する
    m_dump.value[54] = static_cast<u32>(poke->GetParentSex()); // 親の性別を取得する
    poke->GetPastParentsName( m_dump.other_parent_name, 13);   // 過去の親名を取得する
    m_dump.value[55] = poke->GetPastParentsSex();              // 過去の親の性別を取得する
    
    m_dump.value[61] = poke->GetFamiliarTrainerID();            // 親以外のなつきトレーナーＩＤ
    m_dump.value[62] = poke->GetOriginalFamiliarity();          // 最初の親のなつき度を取得する
    m_dump.value[63] = poke->GetOthersFamiliarity();            // 親以外とのなつき度
    
    m_dump.value[64] = poke->GetFriendlyTrainerID();            // 親以外の仲良しトレーナーＩＤ
    m_dump.value[65] = poke->GetFriendship();                   // 最初の親の仲良し度を取得する
    m_dump.value[66] = poke->GetOthersFriendship();             // 親以外との仲良し度


    m_dump.value[67] = poke->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_YEAR)*10000 +  // タマゴをもらった年
                       poke->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_MONTH)*100  +  // タマゴをもらった月
                       poke->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_DAY);          // タマゴをもらった日
    m_dump.value[68] = poke->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE);        // タマゴをもらった場所
    m_dump.value[69] = poke->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR)*10000 +  // 孵化した or 捕獲した年
                       poke->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH)*100  +  // タマゴが孵化した or 捕獲した月
                       poke->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY);          // タマゴが孵化した or 捕獲した日
    m_dump.value[70] = poke->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE);        // タマゴが孵化した or 捕獲した場所
  
    m_dump.value[71] = poke->GetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL);        // 捕まえたボール
    m_dump.value[72] = poke->GetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL);       // 捕まえたレベル
    m_dump.value[74] = poke->GetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_PARENT);    // 親との思い出レベル
    m_dump.value[75] = poke->GetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT);     // 親との思い出事象コード
    m_dump.value[76] = poke->GetMemories( pml::pokepara::MEMORIES_DATA_WITH_PARENT);     // 親との思い出内容データ
    m_dump.value[77] = poke->GetMemories( pml::pokepara::MEMORIES_FEEL_WITH_PARENT);     // 親との思い出気持ちデータ
    m_dump.value[78] = poke->GetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_OTHERS);    // 親以外のトレーナーとの思い出レベル
    m_dump.value[79] = poke->GetMemories( pml::pokepara::MEMORIES_CODE_WITH_OTHERS);     // 親以外のトレーナーとの思い出事象コード
    m_dump.value[80] = poke->GetMemories( pml::pokepara::MEMORIES_DATA_WITH_OTHERS);     // 親以外のトレーナーとの思い出内容データ
    m_dump.value[81] = poke->GetMemories( pml::pokepara::MEMORIES_FEEL_WITH_OTHERS);     // 親以外のトレーナーとの思い出気持ちデータ

    m_dump.value[82] = poke->GetCondition( pml::pokepara::CONDITION_STYLE );      // かっこよさ
    m_dump.value[83] = poke->GetCondition( pml::pokepara::CONDITION_BEAUTIFUL );  // うつくしさ
    m_dump.value[84] = poke->GetCondition( pml::pokepara::CONDITION_CUTE );       // かわいさ
    m_dump.value[85] = poke->GetCondition( pml::pokepara::CONDITION_CLEVER );     // かしこさ
    m_dump.value[86] = poke->GetCondition( pml::pokepara::CONDITION_STRONG );     // たくましさ
    m_dump.value[87] = poke->GetCondition( pml::pokepara::CONDITION_FUR );        // 毛艶
    
    m_dump.value[88] = poke->GetCountryCode();     // 国コード
    m_dump.value[89] = poke->GetHWRegionCode();    // 本体リージョン
    m_dump.value[90] = poke->GetLangId();          // 言語コードを取得する
    m_dump.value[91] = poke->GetCassetteVersion(); // 捕まえたカセットバージョンを取得する

    m_dump.value[95] = poke->GetGetBall();    // 捕まえたボールを取得する

    m_dump.value[96] = poke->GetFeed();  // 満腹度を取得する
    m_dump.value[97] = poke->GetNadenadeValue();  // なでなで度を取得する

    m_dump.value[105] = poke->GetTrainingBadge1();           ///<新規・トレーニング勲章
    m_dump.value[106] = poke->GetTrainingBadge2();           ///<新規・トレーニング勲章
    m_dump.value[107] = poke->IsTrainingBadge( 0 )*10 +      ///<新規・トレーニング勲章(チュートリアル）
                        poke->IsTrainingBadge( 1 );          ///<
    m_dump.value[108] = poke->IsTrainingBadge( 2 ) * 100000 +   ///<トレーニング勲章(1面～6面）
                        poke->IsTrainingBadge( 3 ) * 10000 +    
                        poke->IsTrainingBadge( 4 ) * 1000 +     
                        poke->IsTrainingBadge( 5 ) * 100 +      
                        poke->IsTrainingBadge( 6 ) * 10 +       
                        poke->IsTrainingBadge( 7 ) * 1 ;        
                                                                
    m_dump.value[109] = poke->IsTrainingBadge(  8 ) * 100000 +  ///<トレーニング勲章(7～12面） 
                        poke->IsTrainingBadge(  9 ) * 10000 +   
                        poke->IsTrainingBadge( 10 ) * 1000 +    
                        poke->IsTrainingBadge( 11 ) * 100 +     
                        poke->IsTrainingBadge( 12 ) * 10 +      
                        poke->IsTrainingBadge( 13 ) * 1 ;       
                                                                
    m_dump.value[110] = poke->IsTrainingBadge( 14 ) * 100000 +  ///<トレーニング勲章(13～18面） 
                        poke->IsTrainingBadge( 15 ) * 10000 +   
                        poke->IsTrainingBadge( 16 ) * 1000 +    
                        poke->IsTrainingBadge( 17 ) * 100 +     
                        poke->IsTrainingBadge( 18 ) * 10 +      
                        poke->IsTrainingBadge( 19 ) * 1 ;       
                                                                
    m_dump.value[111] = poke->IsTrainingBadge( 20 ) * 100000 +  ///<トレーニング勲章(19～24面） 
                        poke->IsTrainingBadge( 21 ) * 10000 +   
                        poke->IsTrainingBadge( 22 ) * 1000 +    
                        poke->IsTrainingBadge( 23 ) * 100 +     
                        poke->IsTrainingBadge( 24 ) * 10 +      
                        poke->IsTrainingBadge( 25 ) * 1 ;       
                                                                
    m_dump.value[112] = poke->IsTrainingBadge( 26 ) * 100000 +  ///<トレーニング勲章(25～30面） 
                        poke->IsTrainingBadge( 27 ) * 10000 + 
                        poke->IsTrainingBadge( 28 ) * 1000 +  
                        poke->IsTrainingBadge( 29 ) * 100 +   
                        poke->IsTrainingBadge( 30 ) * 10 +   
                        poke->IsTrainingBadge( 31 ) * 1 ;   

    m_dump.value[113] = poke->IsTrainingBadge( 32 ) * 100000 +  /// トレーニング勲章(31～36面） 
                        poke->IsTrainingBadge( 33 ) * 10000 + 
                        poke->IsTrainingBadge( 34 ) * 1000 +  
                        poke->IsTrainingBadge( 35 ) * 100 +   
                        poke->IsTrainingBadge( 36 ) * 10 +   
                        poke->IsTrainingBadge( 37 ) * 1 ;   

    m_dump.value[114] = poke->IsTrainingBadge( 38 ) * 1000000000 +   ///<トレーニング勲章(あまり）
                        poke->IsTrainingBadge( 39 ) * 100000000 + 
                        poke->IsTrainingBadge( 39 ) * 10000000 + 
                        poke->IsTrainingBadge( 39 ) * 1000000 + 
                        poke->IsTrainingBadge( 39 ) * 100000 + 
                        poke->IsTrainingBadge( 39 ) * 10000 + 
                        poke->IsTrainingBadge( 40 ) * 1000 + 
                        poke->IsTrainingBadge( 41 ) * 100 + 
                        poke->IsTrainingBadge( 42 ) * 10 +  
                        poke->IsTrainingBadge( 43 ) * 1 ;   
    m_dump.value[115] = 0;  // トレーニング勲章の総数
    for(int i=0;i<48;i++){
      m_dump.value[115] += poke->IsTrainingBadge( i );
    }

    m_dump.value[118] = poke->GetMezapaType();  ///< 「めざめるパワー」のタイプを取得
    m_dump.value[119] = poke->GetMezapaPower();      ///< 「めざめるパワー」の威力を取得

    m_dump.value[120] = poke->JudgeTaste( pml::pokepara::TASTE_KARAI  );  // 好き嫌いを調べる
    m_dump.value[121] = poke->JudgeTaste( pml::pokepara::TASTE_SIBUI  );  // 好き嫌いを調べる
    m_dump.value[122] = poke->JudgeTaste( pml::pokepara::TASTE_AMAI   );  // 好き嫌いを調べる
    m_dump.value[123] = poke->JudgeTaste( pml::pokepara::TASTE_NIGAI  );  // 好き嫌いを調べる
    m_dump.value[124] = poke->JudgeTaste( pml::pokepara::TASTE_SUPPAI );  // 好き嫌いを調べる

    m_dump.value[125] = poke->HavePokerusJustNow();   // 今ポケルスに感染しているか？
    m_dump.value[126] = poke->HavePokerusPast();      // 今までにポケルスに感染したことがあるか？
    m_dump.value[127] = poke->GetPokerus(); ///< DEBUG用！ポケルスカウンタを取得
    m_dump.value[128] = poke->GetEventPokeFlag();  // 配布ポケモンかどうか
    m_dump.value[129] = static_cast<u32>(poke->GetTokuseiNo());  // 特性
    m_dump.value[130] = poke->GetTokuseiIndex();  // 特性INDEX
    
    //  すごい特訓フラグ
    {
      for( u32 i = 0; i < pml::pokepara::POWER_NUM; ++i )
      {
        m_dump.value[ 131 + i ] = poke->IsTrainingDone( ( pml::pokepara::PowerID )( pml::pokepara::POWER_HP + i ) );
      }
    }

    m_dump.value[137] = poke->GetDebugEditFlag();   // デバッグ生成されたポケモンか

    // 高速モード終了
    poke->EndFastMode(flag);

    m_dump.value[0] = tray; //カーソル位置保存
    m_dump.value[1] = pos;
  }
  
}

//---------------------------------------------------------------------------------
// トレイ位置とカーソル位置からポケモンのパラメータを取得する（できない事もある）
//---------------------------------------------------------------------------------
pml::pokepara::CoreParam *Proc::GetCoreParamPointer( int tray, int pos )
{
  pml::pokepara::CoreParam *core = NULL;

  if(pos>=30 && pos<36){  // 手持ち
    if((pos-30)<m_debug_appparam->pTemochi2->GetMemberCount()){
      core = m_debug_appparam->pTemochi2->GetMemberPointer(pos-30);
    }
  }
  // カーソル位置からポケモンパラメータを取得
  if(pos>=0 && pos<30){
    if(tray>=0 && tray<32){ // BOX内
      m_debug_appparam->pBoxPokemon->GetPokemon( m_coreParam, tray, pos );
      core = m_coreParam;
    }
  }

  return core;
}

// デバッグパラメータ更新処理（ここでは未使用）
static void DebugWinFunc_BoxPokepara1Update( void* userWork, gfl2::debug::DebugWinItem* item ){ return ;}

//---------------------------------------------------------------------------------
// カーソル位置表示
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara1Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"トレイ[%d], カーソル[%d]",dump->value[0], dump->value[1] );
  return workStr;
}

//---------------------------------------------------------------------------------
// ポケモンNO,フォルムNO,属性NO1,NO2,どうぐ
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara2Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"ポケモン%d, フォルム%d, タイプ[%d,%d], どうぐ%d",
            dump->value[2], dump->value[3], dump->value[4], dump->value[5], dump->value[6]  );
  return workStr;
}

//---------------------------------------------------------------------------------
// レベル、経験値、タマゴ、ダメタマゴ
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara3Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"レベル%d, 経験値=%d, 卵=%d, ダメ卵=%d",
            dump->value[7], dump->value[8], dump->value[9], dump->value[10] );
  return workStr;
}

//---------------------------------------------------------------------------------
// トレーナーID
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara4Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"トレーナーID High[%d] Low[%d]",
            dump->value[11], dump->value[12]);
  return workStr;
}
//---------------------------------------------------------------------------------
// 個性乱数
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara5Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"個性乱数 High[%d] Low[%d]",
            dump->value[13], dump->value[14]);
  return workStr;
}
//---------------------------------------------------------------------------------
// レア乱数
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara6Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"レア乱数 High[%d] Low[%d] レア=%d",
            dump->value[15], dump->value[16], dump->value[17]);
  return workStr;
}

//---------------------------------------------------------------------------------
// 状態異常,性別,性別ベクトル,努力値合計
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara7Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"状態異常=%d 性別=%d 性別VEC=%d 努力値合計=%d",
            dump->value[18], dump->value[19], dump->value[20], dump->value[40]);
  return workStr;
}

//---------------------------------------------------------------------------------
// 個体値
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara9Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"個 HP%3d 攻%3d 防%3d 特攻%3d 特防%3d 速%3d",
            dump->value[28], dump->value[29], dump->value[30], dump->value[31], dump->value[32], dump->value[33]);
  return workStr;
}

//---------------------------------------------------------------------------------
// 努力値
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara10Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"努 HP%3d 攻%3d 防%3d 特攻%3d 特防%3d 速%0d",
            dump->value[34], dump->value[35], dump->value[36], dump->value[37], dump->value[38], dump->value[39] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 技１のＰＰ情報 と 特性NO
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara11Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"技１ PP%d MAX%d PPアップ回数%d   特性NO=%d",
            dump->value[41], dump->value[42], dump->value[43], dump->value[129] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 技2のＰＰ情報 と 特性INDEX
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara12Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"技２ PP%d MAX%d PPアップ回数%d   特性IDX=%d",
            dump->value[44], dump->value[45], dump->value[46], dump->value[130] );
  return workStr;
}
//---------------------------------------------------------------------------------
// 技３のＰＰ情報
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara13Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"技３ PP%d MAX%d PPアップ回数%d",
            dump->value[47], dump->value[48], dump->value[49]);
  return workStr;
}

//---------------------------------------------------------------------------------
// 技4のＰＰ情報
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara14Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"技４ PP%d MAX%d PPアップ回数%d",
            dump->value[50], dump->value[51], dump->value[52]);
  return workStr;
}

//---------------------------------------------------------------------------------
// ポケモンのニックネームとニックネームフラグ
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara15Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"ニックーネーム「%ls」：フラグ=%d",
            dump->poke_nickname, dump->value[53] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 親名と性別
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara16Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"親名「%ls」：性別=%d",
            dump->parent_name, dump->value[54] );
  return workStr;
}
//---------------------------------------------------------------------------------
// 別親名と性別
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara17Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"別親名「%ls」：性別=%d",
            dump->other_parent_name, dump->value[55] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 親以外のなつきトレーナーＩＤ
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara20Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"親以外のなつきＩＤ [%d]",
            dump->value[61] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 捕獲親なつき度、親以外のなつき度
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara21Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"捕獲親なつき度 %d 親以外なつき度 %d",
            dump->value[62], dump->value[63] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 親以外の仲良しトレーナーＩＤ
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara22Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"親以外の仲良しＩＤ [%d]",
            dump->value[64] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 捕獲親の仲良し度、親以外の仲良し度
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara23Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"捕獲親仲良し度 %d 親以外仲良し度 %d",
            dump->value[65], dump->value[66] );
  return workStr;
}

//---------------------------------------------------------------------------------
// タマゴ受取年月日、受取場所
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara24Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"卵受取年月日 %d 場所 %d",
            dump->value[67], dump->value[68] );
  return workStr;
}

//---------------------------------------------------------------------------------
// タマゴ孵化or捕獲年月日、孵化or捕獲場所
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara25Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"卵孵化or捕獲年月日 %d 場所 %d",
            dump->value[69], dump->value[70] );
  return workStr;
}


//---------------------------------------------------------------------------------
// 捕まえたボール、レベル、地形アトリビュート
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara26Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"思い出：捕獲ボール=%d LV=%d",
            dump->value[71], dump->value[72] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 親との思いでレベル、思い出事象コード
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara27Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"親との思い出LV=%d コード=%d",
            dump->value[74], dump->value[75]);
  return workStr;
}

//---------------------------------------------------------------------------------
// 親との思いで内容、気持ち
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara28Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"親思い出内容=%d 気持ち=%d",
            dump->value[76], dump->value[77] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 親との思いでレベル、思い出事象コード
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara29Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"別親との思い出LV=%d コード=%d",
            dump->value[78], dump->value[79] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 親との思いで内容、気持ち
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara30Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"別親思い出内容=%d 気持ち=%d",
            dump->value[80], dump->value[81]);
  return workStr;
}

//---------------------------------------------------------------------------------
// コンディション：かっこよさ、うつくしさ、かわいさ
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara31Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"格好=%d 美=%d 可愛=%d",
            dump->value[82], dump->value[83], dump->value[84]);
  return workStr;
}

//---------------------------------------------------------------------------------
// コンディション：かしこさ、たくましさ、けづや
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara32Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"賢さ=%d 逞=%d 毛艶=%d",
            dump->value[85], dump->value[86], dump->value[87]);
  return workStr;
}

//---------------------------------------------------------------------------------
// 国コード、本体リージョン、言語コード、捕獲カセット
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara33Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"国=%d 本体R=%d 言語=%d 捕獲カセット=%d",
            dump->value[88], dump->value[89], dump->value[90], dump->value[91]);
  return workStr;
}

//---------------------------------------------------------------------------------
// 捕獲ボール、満腹度、なでなで度
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara35Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"捕獲ボール=%d 満腹度=%d なでなで度=%d",
            dump->value[95], dump->value[96], dump->value[97]);
  return workStr;
}

//---------------------------------------------------------------------------------
// 所持しているトレーニンググッズ、勲章１、勲章2
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara38Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"トレグッズ=%d トレ勲章１=%d トレ勲章2=%d",
            dump->value[105], dump->value[106], dump->value[107]);
  return workStr;
}

//---------------------------------------------------------------------------------
// トレーニング勲章総数、チュートリアル勲章ビット
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara39Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"トレ勲章総数=%d チュート勲章(bit)=%02d",
            dump->value[115], dump->value[107] );
  return workStr;
}

//---------------------------------------------------------------------------------
// トレーニング１～６面、７～１２面
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara40Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"bit)LV1 1～6面=%06d LV2 1～6面=%06d",
            dump->value[108], dump->value[109]);
  return workStr;
}

//---------------------------------------------------------------------------------
// トレーニング13～18面、19～24面
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara41Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"bit)LV3 1～6面=%06d 裏1 1～6面=%06d",
            dump->value[110], dump->value[111]);
  return workStr;
}

//---------------------------------------------------------------------------------
// トレーニング25～30面、31～36面
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara42Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"bit)裏2 1～6面=%06d 配信 1～6面=%06d",
            dump->value[112], dump->value[113]);
  return workStr;
}

//---------------------------------------------------------------------------------
// トレーニングあまりbit
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara43Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"bit)トレ余り=%010d",
            dump->value[114]);
  return workStr;
}

//---------------------------------------------------------------------------------
// めざめるパワーのタイプ、めざめるパワーの威力
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara45Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"めざパタイプ=%d めざパ威力=%d",
            dump->value[118], dump->value[119] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 味：辛い、渋い、甘い、苦い、酸っぱい
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara46Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"味：辛%d 渋%d 甘%d 苦%d 酸%d",
            dump->value[120], dump->value[121], dump->value[122], dump->value[123], dump->value[124] );
  return workStr;
}

//---------------------------------------------------------------------------------
// ポケルス  今、過去、カウンタ
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara47Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"ポケルス：今%d 過去%d カウンタ%d",
            dump->value[125], dump->value[126], dump->value[127] );
  return workStr;
}

//---------------------------------------------------------------------------------
// 配布ポケモンか
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara48Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"配布ポケモン=%d",
            dump->value[128] );
  return workStr;
}

//---------------------------------------------------------------------------------
// すごい特訓フラグ( 1 )
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara49Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"すごい特訓 HP=%d 攻撃=%d 防御=%d", 
    dump->value[ 131 + pml::pokepara::POWER_HP ],
    dump->value[ 131 + pml::pokepara::POWER_ATK ],
    dump->value[ 131 + pml::pokepara::POWER_DEF ] );

  return workStr;
}

//---------------------------------------------------------------------------------
// すごい特訓フラグ( 2 )
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara50Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"すごい特訓 特攻=%d 特防=%d 素早さ=%d", 
    dump->value[ 131 + pml::pokepara::POWER_SPATK ],
    dump->value[ 131 + pml::pokepara::POWER_SPDEF ],
    dump->value[ 131 + pml::pokepara::POWER_AGI ] );

  return workStr;
}

//---------------------------------------------------------------------------------
// デバッグ生成フラグ
//---------------------------------------------------------------------------------
static wchar_t* DebugWinFunc_BoxPokePara51Draw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::Box::Proc::DebugPokeParaDump *dump = static_cast<App::Box::Proc::DebugPokeParaDump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"デバッグ生成フラグ=%d", dump->value[ 137 ] );

  return workStr;
}




static void teamlock_Update(void* userWork, gfl2::debug::DebugWinItem* , int teamno)
{
  Savedata::BOX* pBoxData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();

  u8 lockno = pBoxData->IsTeamLock(teamno);

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    int lockTeamIndex = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleSpotData()->GetFriendLockTeamIndex();
    if( lockTeamIndex == teamno )
    {
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleSpotData()->SetFriendLockTeamIndex( 0xFF );
    }

    lockTeamIndex = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleSpotData()->GetInternetLockTeamIndex();
    if( lockTeamIndex == teamno )
    {
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleSpotData()->SetInternetLockTeamIndex( 0xFF );
    }

    lockTeamIndex = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetLiveMatchData()->GetPartyIndex();
    if( lockTeamIndex == teamno )
    {
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetLiveMatchData()->InvalidatePartyIndex();
    }

    pBoxData->SetTeamLock(teamno,false);
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    pBoxData->SetTeamLock(teamno,true);
  }
}


static void teamlock1_Update(void* userWork, gfl2::debug::DebugWinItem* )
{
  teamlock_Update(userWork, NULL , 0);
}

static void teamlock2_Update(void* userWork, gfl2::debug::DebugWinItem* )
{
  teamlock_Update(userWork, NULL , 1);
}

static void teamlock3_Update(void* userWork, gfl2::debug::DebugWinItem* )
{
  teamlock_Update(userWork, NULL , 2);
}

static void teamlock4_Update(void* userWork, gfl2::debug::DebugWinItem* )
{
  teamlock_Update(userWork, NULL , 3);
}

static void teamlock5_Update(void* userWork, gfl2::debug::DebugWinItem* )
{
  teamlock_Update(userWork, NULL , 4);
}

static void teamlock6_Update(void* userWork, gfl2::debug::DebugWinItem* )
{
  teamlock_Update(userWork, NULL , 5);
}


static wchar_t* teamlock_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item, int teamno)
{
  Savedata::BOX* pBoxData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();
  u8 lockno = pBoxData->IsTeamLock(teamno);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"t%d[ %d ]",teamno, lockno );
  return workStr;
}



static wchar_t* teamlock1_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  return teamlock_Draw(NULL,item, 0);
}
static wchar_t* teamlock2_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  return teamlock_Draw(NULL,item, 1);
}
static wchar_t* teamlock3_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  return teamlock_Draw(NULL,item, 2);
}
static wchar_t* teamlock4_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  return teamlock_Draw(NULL,item, 3);
}
static wchar_t* teamlock5_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  return teamlock_Draw(NULL,item, 4);
}
static wchar_t* teamlock6_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  return teamlock_Draw(NULL,item, 5);
}

#endif

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )
