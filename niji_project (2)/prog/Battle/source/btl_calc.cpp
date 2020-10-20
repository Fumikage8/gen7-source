
#include <pml/include/item/item_manager.h>

#include <GameSys/include/GameData.h>
#include <Savedata/include/MiscSave.h>
#include <System/include/GflUse.h>

#include  "btl_common.h"
#include  "btl_calc.h"
#include  "btl_tables.h"
#include  "btl_mainmodule.h"

GFL_NAMESPACE_BEGIN(btl)
namespace calc {


static RandSys*     g_RandSys;
static WazaNo*      g_WazaStoreWork;
static itemman::ITEM_MANAGER*   g_ItemMan;
static bool         g_SakasaBtlFlag;
//static HeapHandle   g_HeapHandle;


//--------------------------------------------------------------
/**
 *  ポケモンの能力値IDテーブル
 *  ※各所、この順番で能力値・努力値などの操作をしているため順番を変えないようにお願いします
 */
 //--------------------------------------------------------------
const pml::pokepara::PowerID  PokePowerIDTable[ pml::pokepara::POWER_NUM ] = {
  pml::pokepara::POWER_HP,     // 最大HP
  pml::pokepara::POWER_ATK,    // こうげき
  pml::pokepara::POWER_DEF,    // ぼうぎょ
  pml::pokepara::POWER_SPATK,  // とくこう
  pml::pokepara::POWER_SPDEF,  // とくぼう
  pml::pokepara::POWER_AGI,    // すばやさ
};


//=============================================================================================
/**
 * システム初期化
 *
 * @param   randSys       乱数コンテキスト
 * @param   bSakasaBtl    true なら 相性さかさバトルモード
 * @param   heapHandle
 */
//=============================================================================================
void InitSys( RandSys* randSys, bool bSakasaBtl, HeapHandle heapHandle  )
{
  g_RandSys = randSys;
  g_WazaStoreWork = GFL_NEW_ARRAY( heapHandle ) WazaNo[ WAZANO_MAX ];
  g_ItemMan = GFL_NEW(heapHandle) itemman::ITEM_MANAGER( heapHandle, 1 );
  g_SakasaBtlFlag = bSakasaBtl;
//  g_HeapHandle = heapHandle;
}

//=============================================================================================
/**
 * システム（乱数系）リセット
 *
 * @param   randContext
 */
//=============================================================================================
void ResetSys( const u32 randSeed )
{
  g_RandSys->Initialize( randSeed );
}

//=============================================================================================
/**
 * システム終了
 */
//=============================================================================================
void QuitSys( void )
{
  if( g_ItemMan )
  {
    GFL_DELETE( g_ItemMan );
    g_ItemMan = NULL;
  }
  GFL_SAFE_DELETE_ARRAY( g_WazaStoreWork );
}


//=============================================================================================
/**
 *  タイプ相性基礎計算
 *
 * @param   wazaType    ワザのタイプ
 * @param   pokeType    ワザを受けるポケモンタイプ
 *
 * @retval  BtlTypeAff
 */
//=============================================================================================
BtlTypeAff TypeAff( pml::PokeType wazaType, pml::PokeType pokeType )
{
  return pml::battle::TypeAffinity::CalcAffinity( wazaType, pokeType, g_SakasaBtlFlag );
}
//=============================================================================================
/**
 * 相性値の掛け合わせ（２タイプあるポケモン対応）
 *
 * @param   aff1
 * @param   aff2
 *
 * @retval  BtlTypeAff    掛け合わせ後の相性値
 */
//=============================================================================================
BtlTypeAff TypeAffMul( BtlTypeAff aff1, BtlTypeAff aff2 )
{
  return pml::battle::TypeAffinity::MulAffinity( aff1, aff2 );
}
//=============================================================================================
/**
 * タイプ相性計算（ポケモンタイプそのまま使用可能）
 *
 * @param   wazaType
 * @param   pokeType
 *
 * @retval  BtlTypeAff
 */
//=============================================================================================
BtlTypeAff TypeAffPair( pml::PokeType wazaType, PokeTypePair pokeType )
{
  pml::PokeType type1, type2, typeEx;

  PokeTypePair_Split( pokeType, &type1, &type2, &typeEx );

  BtlTypeAff  result = TypeAff( wazaType, type1 );
  if( (type2 != POKETYPE_NULL) && (type2 != type1) )
  {
    result = TypeAffMul( result, TypeAff(wazaType, type2) );
  }

  if( (typeEx != POKETYPE_NULL) && (typeEx != type1) && (typeEx != type2) )
  {
    result = TypeAffMul( result, TypeAff(wazaType, typeEx) );
  }

  return result;
}

//=============================================================================================
/**
 *  抵抗相性を全て取得
 *
 * @param   type    タイプ
 * @param   dst     [out] 抵抗相性を持つタイプを格納する配列
 *
 * @retval  u8
 */
//=============================================================================================
u8 GetResistTypes( pml::PokeType type, pml::PokeType* dst )
{
  u8  cnt = 0;

  for(u32 i=0; i<POKETYPE_NUMS; ++i)
  {
    BtlTypeAff aff = TypeAff( type, (pml::PokeType)(i) );
    if( (aff==pml::battle::TypeAffinity::TYPEAFF_1_2) || (aff==pml::battle::TypeAffinity::TYPEAFF_0) ){
      dst[cnt++] = i;
    }
  }

  return cnt;
}


//=============================================================================================
/**
 * 基本ダメージ計算
 *
 * @param   wazaPow
 * @param   atkPower
 * @param   atkLevel
 * @param   defGuard
 *
 * @retval  u32
 */
//=============================================================================================
u32 DamageBase( u32 wazaPower, u32 atkPower, u32 atkLevel, u32 defGuard )
{
  u32 damage;

  damage = (wazaPower * atkPower * (atkLevel*2/5+2));
  damage = damage / defGuard / 50;
  damage += 2;
  return damage;
}
//=============================================================================================
/**
 *  相性ダメージ計算
 *
 * @param   rawDamage
 * @param   aff
 *
 * @retval  u32
 */
//=============================================================================================
u32 AffDamage( u32 rawDamage, BtlTypeAff aff )
{
  switch( aff ){
  case pml::battle::TypeAffinity::TYPEAFF_0:     return 0;
  case pml::battle::TypeAffinity::TYPEAFF_1_64:  return (rawDamage / 64);
  case pml::battle::TypeAffinity::TYPEAFF_1_32:  return (rawDamage / 32);
  case pml::battle::TypeAffinity::TYPEAFF_1_16:  return (rawDamage / 16);
  case pml::battle::TypeAffinity::TYPEAFF_1_8:   return (rawDamage /  8);
  case pml::battle::TypeAffinity::TYPEAFF_1_4:   return (rawDamage /  4);
  case pml::battle::TypeAffinity::TYPEAFF_1_2:   return (rawDamage /  2);
  case pml::battle::TypeAffinity::TYPEAFF_1:     return rawDamage;
  case pml::battle::TypeAffinity::TYPEAFF_2:     return rawDamage *  2;
  case pml::battle::TypeAffinity::TYPEAFF_4:     return rawDamage *  4;
  case pml::battle::TypeAffinity::TYPEAFF_8:     return rawDamage *  8;
  case pml::battle::TypeAffinity::TYPEAFF_16:    return rawDamage * 16;
  case pml::battle::TypeAffinity::TYPEAFF_32:    return rawDamage * 32;
  case pml::battle::TypeAffinity::TYPEAFF_64:    return rawDamage * 64;
  default:
    GFL_ASSERT(0);
    return rawDamage;
  }
}

//=============================================================================================
/**
 * 乱数値を返す
 *  ※返す値は 0〜(pattern-1) の範囲。pattern==0だと、32bit全範囲。
 *
 * @param   pattern   乱数の取りうる値の範囲
 *
 * @retval  u32
 */
//=============================================================================================
u32 GetRand( u32 range )
{
  u32 result;
  result = g_RandSys->Next( range );
  return result;
}

//=============================================================================================
/**
 *  min以上 〜 max以下の範囲内で乱数取得
 *
 * @param   min
 * @param   max
 *
 * @retval  u32
 */
//=============================================================================================
u32
RandRange( u32 min, u32 max )
{
  if( min > max ){
    u32 tmp = min;
    min = max;
    max = tmp;
  }
  {
    u32 range = 1 + (max-min);
    return min + GetRand( range );
  }
}

//=============================================================================================
/**
 *
 *
 * @param   value
 * @param   ratio   倍率（％）
 *
 * @retval  u32
 */
//=============================================================================================
u32
MulRatio( u32 value, fx32 ratio )
{
  u32 decimal;

  value *= ratio;
  decimal = value & ( (1 << FX32_SHIFT) -1 );
  value >>= FX32_SHIFT;
  if( decimal > (1 << (FX32_SHIFT-1)) ){
    ++value;
  }

  return value;
}
//=============================================================================================
/**
 * 倍率計算（整数x固定小数）※ゼロになった場合、最低１に切り上げ
 *
 * @param   value
 * @param   ratio   倍率（％）
 *
 * @retval  u32
 */
//=============================================================================================
u32 MulRatio_OverZero( u32 value, fx32 ratio )
{
  value = MulRatio( value, ratio );
  if( value == 0 ){
    value = 1;
  }
  return value;
}

//=============================================================================================
/**
 * 倍率計算（整数x整数）
 *
 * @param   value
 * @param   ratio   倍率（％）
 *
 * @retval  u32
 */
//=============================================================================================
u32 MulRatioInt( u32 value, u32 ratio )
{
  u32  rem;

  value *= ratio;
  rem = value % 100;
  value /= 100;
  if( rem >= 50 ){
    ++value;
  }
  return value;
}

//=============================================================================================
/**
 * ワザ系状態異常の、デフォルトの継続パラメータを作成
 *
 * @param   sick
 * @param   cont    [out]
 *
 */
//=============================================================================================
void MakeDefaultWazaSickCont( WazaSick sick, const BTL_POKEPARAM* attacker, BTL_SICKCONT* cont )
{
  if( sick < pml::pokepara::SICK_MAX ){
    *cont = MakeDefaultPokeSickCont( (PokeSick)sick, attacker->GetID() );
    return;
  }

  cont->raw = 0;

  switch( sick ){
  case pml::wazadata::WAZASICK_MEROMERO:
    {
      u8 causePokeID = attacker->GetID();
      u8 pokeID = attacker->GetID();
      *cont = SICKCONT_MakePoke( causePokeID, pokeID );
    }
    break;

  case pml::wazadata::WAZASICK_KONRAN:
    {
      u8 turns = RandRange( BTL_CONF_TURN_MIN, BTL_CONF_TURN_MAX );
      u8 causePokeID = attacker->GetID();
      *cont = SICKCONT_MakeTurn( causePokeID, turns );
    }
    break;

  default:
    *cont = SICKCONT_MakePermanent( attacker->GetID() );
    break;
  }
}

//=============================================================================================
/**
 * ワザ系状態異常の継続パラメータ（ポケモン依存型）を作成
 *
 * @param   pokeID
 * @param   causePokeID  状態異常の原因となったポケモンのID
 */
//=============================================================================================
extern BTL_SICKCONT MakeWazaSickCont_Poke( u8 pokeID, u8 causePokeID )
{
  BTL_SICKCONT cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_POKE;
  cont.causePokeID = causePokeID;
  cont.poke.ID = pokeID;
  return cont;
}


//=============================================================================================
/**
 * ポケモン系状態異常の、デフォルトの継続パラメータを作成
 *
 * @param   sick         状態異常コード
 * @param   causePokeID  状態異常の原因となったポケモンのID
 *
 * @retval  BTL_SICK_CONT
 */
//=============================================================================================
extern BTL_SICKCONT MakeDefaultPokeSickCont( pml::pokepara::Sick sick, u8 causePokeID )
{
  BTL_SICKCONT  cont;

  cont.raw = 0;
  switch( sick ){
  case pml::pokepara::SICK_DOKU:
  case pml::pokepara::SICK_YAKEDO:
  case pml::pokepara::SICK_MAHI:
  case pml::pokepara::SICK_KOORI:
    cont.type = pml::wazadata::SICKCONT_PERMANENT;
    cont.causePokeID = causePokeID;
    break;
  case pml::pokepara::SICK_NEMURI:
    cont.type = pml::wazadata::SICKCONT_TURN;
    cont.causePokeID = causePokeID;
    cont.turn.count = RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MAX );
    break;
  default:
    GFL_ASSERT_MSG(0, "illegal sick ID(%d)\n", sick);
    cont.type = pml::wazadata::SICKCONT_NONE;
    cont.causePokeID = causePokeID;
    break;
  }

  return cont;
}
//=============================================================================================
/**
 * ランク段階によるステータスの増減計算
 *
 * @param   defaultVal    ランクフラットな状態の値
 * @param   rank          ランク段階
 *
 * @retval  u16   ランク状態による変化後の値
 */
//=============================================================================================
u16
StatusRank( u16 defaultVal, u8 rank )
{
  //--------------------------------------------------------------
  /**
   *  ステータスランク補正テーブル
   */
  //--------------------------------------------------------------
  static const struct {
    u8  num;
    u8  denom;
  }StatusRankTable[] = {
    { 2, 8 }, // x(2/8) = 25%
    { 2, 7 }, // x(2/7) = 29%
    { 2, 6 }, // x(2/6) = 33%
    { 2, 5 }, // x(2/5) = 40%
    { 2, 4 }, // x(2/4) = 50%
    { 2, 3 }, // x(2/3) = 67%
    { 2, 2 }, // x(2/2) = 100%
    { 3, 2 }, // x(3/2) = 150%
    { 4, 2 }, // x(4/2) = 200%
    { 5, 2 }, // x(5/2) = 250%
    { 6, 2 }, // x(6/2) = 300%
    { 7, 2 }, // x(7/2) = 350%
    { 8, 2 }, // x(8/2) = 400%
  };


  if( rank < GFL_NELEMS(StatusRankTable) )
  {
    defaultVal = defaultVal * StatusRankTable[rank].num;
    defaultVal /= StatusRankTable[rank].denom;
    return defaultVal;
  }
  else{
    GFL_ASSERT(0);
    return defaultVal;
  }
}

//=============================================================================================
/**
 * ポケモンの最大HP * 1/n を計算（最低０）
 *
 * @param   bpp
 * @param   denom
 *
 * @retval  u32
 */
//=============================================================================================
u32 QuotMaxHP_Zero( const BTL_POKEPARAM* bpp, u32 denom )
{
  u32 ret = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP ) / denom;
  return ret;
}

//=============================================================================================
/**
 * ポケモンの最大HP * 1/n を計算（最低１になるように補正）
 *
 * @param   bpp
 * @param   denom
 *
 * @retval  u32
 */
//=============================================================================================
u32 QuotMaxHP( const BTL_POKEPARAM* bpp, u32 denom )
{
  u32 ret = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP ) / denom;
  if( ret == 0 ){ ret = 1; }
  return ret;
}


//=============================================================================================
/**
 * 命中率計算
 *
 * @param   defPer
 * @param   rank
 *
 * @retval  u8
 */
//=============================================================================================
u8 HitPer( u8 defPer, u8 rank )
{
  /**
   *  的中率補正テーブル
   */
  static const struct {
    u8  num;
    u8  denom;
  }HitRankTable[] = {
    {   6,  18 },
    {   6,  16 },
    {   6,  14 },
    {   6,  12 },
    {   6,  10 },
    {   6,   8 },
    {   6,   6 },
    {   8,   6 },
    {  10,   6 },
    {  12,   6 },
    {  14,   6 },
    {  16,   6 },
    {  18,   6 },
  };

  if( rank < GFL_NELEMS(HitRankTable) )
  {
    u32 per = defPer * HitRankTable[rank].num / HitRankTable[rank].denom;
    if( per > 100 )
    {
      per = 100;
    }
    return per;
  }
  else{
    GFL_ASSERT(0);
    return defPer;
  }
}

//=============================================================================================
/**
 * 乱数によるクリティカル判定
 *
 * @param   rank     クリティカルランク
 * @param   ratio    通常計算結果に補正する倍率（１：補正なし／２：クリティカル率２倍）
 *
 * @retval  bool     クリティカル発生したら true
 */
//=============================================================================================
bool CheckCritical( u8 rank, int ratio )
{
  static const u8 CriticalRankTable[] = {
    24, 8, 2, 1,
  };


  if( rank < GFL_NELEMS(CriticalRankTable) )
  {
    u8 rp = CriticalRankTable[ rank ] / ratio;
    if( rp == 0 ){
      rp = 1;
    }
    u8 ret = GetRand( rp );

    BTL_PRINT("Critical Rank:%d, result:%d\n", rp, (ret==0));

    return (ret == 0);
  }
  else{
    GFL_ASSERT(0);
    return false;
  }
}

//=============================================================================================
/**
 * アイテムパラメータ取得
 *
 * @param   item
 * @param   paramID
 *
 * @retval  s32
 */
//=============================================================================================
s32 ITEM_GetParam( u16 item, u16 paramID )
{
  return g_ItemMan->GetParam( item, (item::ITEM_DATA::ITEM_PRM_ID)paramID );
}

//=============================================================================================
/**
 * アイテムがメールかどうか判定
 *
 * @param   item
 *
 * @retval  bool    メールならtrue
 */
//=============================================================================================
bool ITEM_IsMail( u16 item )
{
  return ITEM_GetParam(item, item::ITEM_DATA::PRM_ID_ITEM_TYPE) == ITEMTYPE_MAIL;
}

//=============================================================================================
/**
 * 【パーソナル】パラメータ取得
 *
 * @param   mons_no
 * @param   form_no
 * @param   paramID
 *
 * @retval  u32
 */
//=============================================================================================
u32 PERSONAL_GetParam( int mons_no, int form_no, pml::personal::ParamID paramID )
{
  pml::personal::LoadPersonalData( (MonsNo)mons_no, form_no );
  return pml::personal::GetPersonalParam( paramID );
}

//=============================================================================================
/**
 * 【パーソナル】指定レベルになるための経験値取得
 *
 * @param   monsno
 * @param   formno
 * @param   level
 *
 * @retval  u32
 */
//=============================================================================================
u32 PERSONAL_GetMinExp( int monsno, int formno, u8 level )
{
  pml::personal::LoadGrowTable( (MonsNo)monsno, formno );
  return pml::personal::GetMinExp( level );
}

//=============================================================================================
/**
 * 【パーソナル】進化レベルを超えているポケモンかどうか判定
 *
 * @param   mons_no
 * @param   formno
 * @param   level
 *
 * @retval  bool
 */
//=============================================================================================
bool PERSONAL_IsEvoCancelPokemon( int mons_no, FormNo formno, u8 level )
{
  pml::personal::LoadEvolutionTable( (MonsNo)mons_no, formno );
  u32  num = pml::personal::GetEvolutionRouteNum();
  for(u32 i=0; i<num; ++i)
  {
    if( pml::personal::GetEvolutionCondition(i) == pml::personal::EVOLVE_COND_LEVELUP )
    {
      u8 evo_level = static_cast<u8>(pml::personal::GetEvolveEnableLevel( i ));
      if( level >= evo_level ){
        return true;
      }
    }
  }
  return false;
}

//=============================================================================================
/**
 * 基本状態異常か判定
 *
 * @param   sickID
 *
 * @retval  BOOL
 */
//=============================================================================================
bool IsBasicSickID( WazaSick sickID )
{
  return sickID < pml::pokepara::SICK_MAX;
}

//=============================================================================================
/**
 * 該当の天候によるターンチェックダメージを計算
 *
 * @param   bpp
 * @param   weather
 *
 * @retval  u16     ダメージ値
 */
//=============================================================================================
u16 RecvWeatherDamage( const BTL_POKEPARAM* bpp, BtlWeather weather )
{
  switch( weather ){
  case BTL_WEATHER_SAND:
    if( bpp->IsMatchType(POKETYPE_IWA) ){ return 0; }
    if( bpp->IsMatchType(POKETYPE_HAGANE) ){ return 0; }
    if( bpp->IsMatchType(POKETYPE_JIMEN) ){ return 0; }
    break;

  case BTL_WEATHER_SNOW:
    if( bpp->IsMatchType(POKETYPE_KOORI) ){ return 0; }
    break;

  default:
    return 0;
  }

  {
    u16 dmg = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP ) / 16;
    if( dmg == 0 ){ dmg = 1; }
    return dmg;
  }
}
//=============================================================================================
/**
 * 天候・ワザタイプ相性による威力倍率補正
 *
 * @param   weather
 * @param   wazaType
 *
 * @retval  fx32
 */
//=============================================================================================
fx32 GetWeatherDmgRatio( BtlWeather weather, pml::PokeType wazaType )
{
  switch( weather ){
  case BTL_WEATHER_SHINE:
  case BTL_WEATHER_DAY: //@バトル変更箇所
    if( wazaType == POKETYPE_HONOO ){
      return BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE;
    }
    if( wazaType == POKETYPE_MIZU ){
      return BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE;
     }
    break;

  case BTL_WEATHER_RAIN:
  case BTL_WEATHER_STORM: //@バトル変更箇所
    if( wazaType == POKETYPE_HONOO ){
      return BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE;
    }
    if( wazaType == POKETYPE_MIZU ){
      return BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE;
    }
    break;

  default:
    break;
  }
  return BTL_CALC_DMG_WEATHER_RATIO_NONE;
}

//=============================================================================================
/**
 * 天候が晴れの扱いか？(BTL_WEATHER_SHINEとBTL_WEATHER_DAY)
 *
 * @param   weather
 *
 * @retval  bool
 */
//=============================================================================================
//@バトル変更箇所
bool IsShineWeather( BtlWeather weather )
{
  if( weather == BTL_WEATHER_SHINE ||
      weather == BTL_WEATHER_DAY )
  {
    return true;
  }
  return false;
}

//=============================================================================================
/**
 * 天候が雨の扱いか？(BTL_WEATHER_RAINとBTL_WEATHER_STORM)
 *
 * @param   weather
 *
 * @retval  bool
 */
//=============================================================================================
//@バトル変更箇所
bool IsRainWeather( BtlWeather weather )
{
  if( weather == BTL_WEATHER_RAIN ||
      weather == BTL_WEATHER_STORM )
  {
    return true;
  }
  return false;
}

//=============================================================================================
/**
 * ワザデータの状態異常継続パラメータ値から、バトルで使う状態異常継続パラメータ値へ変換
 *
 * @param   wazaSickCont
 * @param   attacker    ワザを使ったポケモン
 * @param   sickCont    [out]
 *
 */
//=============================================================================================
void WazaSickContToBppSickCont( SICKCONT_PARAM wazaSickCont, const BTL_POKEPARAM* attacker, BTL_SICKCONT* sickCont )
{
  switch( wazaSickCont.type ){
  case pml::wazadata::SICKCONT_POKE:
    GFL_ASSERT_STOP(attacker!=NULL);
    {
      u8 causePokeID = attacker->GetID();
      *sickCont = SICKCONT_MakePoke( causePokeID, attacker->GetID() );
    }
    break;

  case pml::wazadata::SICKCONT_TURN:
    {
      u8 turn = RandRange( wazaSickCont.turnMin, wazaSickCont.turnMax );
      u8 causePokeID = attacker->GetID();
      *sickCont = SICKCONT_MakeTurn( causePokeID, turn );
    }
    break;

  case pml::wazadata::SICKCONT_PERMANENT:
    {
      u8 causePokeID = attacker->GetID();
      *sickCont = SICKCONT_MakePermanentIncParam( causePokeID, wazaSickCont.turnMax, wazaSickCont.turnMin );
    }
    break;

  case pml::wazadata::SICKCONT_POKETURN:
    GFL_ASSERT_STOP(attacker!=NULL);
    {
      u8 causePokeID = attacker->GetID();
      u8 pokeID = attacker->GetID();
      u8 turn   = RandRange( wazaSickCont.turnMin, wazaSickCont.turnMax );
      *sickCont = SICKCONT_MakePokeTurn( causePokeID, pokeID, turn );
    }
    break;
  }
}

//=============================================================================================
/**
 * 最大ヒット回数を受け取り、実際にヒットする回数（ランダム補正）を返す
 *
 * @param   numHitMax
 *
 * @retval  u8
 */
//=============================================================================================
u8 HitCountStd( u8 numHitMax )
{
  if( numHitMax == 5 )
  {
    static const u8 perTbl[] = {
      0, 0, 35, 70, 85, 100,
    };
    u8 rnd = GetRand( 100 );
    u8 i;

    for(i=0; i<GFL_NELEMS(perTbl); ++i)
    {
      if( rnd < perTbl[i] ){
        return i;
      }
    }
    return numHitMax;
  }
  else{
    return numHitMax;
  }
}
//=============================================================================================
/**
 * 「メンタルハーブ」で治る対象の状態異常チェック
 *
 * @param   bpp
 *
 * @retval  WazaSick
 */
//=============================================================================================
WazaSick CheckMentalSick( const BTL_POKEPARAM* bpp )
{
  WazaSick result = pml::wazadata::WAZASICK_NULL;
  u32 i = 0;
  for ever
  {
    result = tables::GetMentalSickID( i++ );
    if( result == pml::wazadata::WAZASICK_NULL ){
      break;
    }
    if( bpp->CheckSick(result) ){
      return result;
    }
  }
  return result;
}
//=============================================================================================
/**
 * タイプ相性（厳密）->タイプ相性（おおまか）に変更
 *
 * @param   aff
 *
 * @retval  BtlTypeAffAbout
 */
//=============================================================================================
BtlTypeAffAbout TypeAffAbout( BtlTypeAff aff )
{
  return pml::battle::TypeAffinity::ConvAboutAffinity( aff );
}

//=============================================================================================
/**
 * 確率事象チェック
 *
 * @param   per
 *
 * @retval  u32
 */
//=============================================================================================
u32 IsOccurPer( u32 per )
{
  return (GetRand(100) < per);
}

//=============================================================================================
/**
 * 数値切り上げ
 *
 * @param   value
 * @param   min
 *
 * @retval  int
 */
//=============================================================================================
int Roundup( int value, int min )
{
  if( value < min ){ value = min; }
  return value;
}

//=============================================================================================
/**
 * 選択対象タイプを返す
 *
 * @param waza      取得対象の技
 * @param attacker  技を撃つポケモン
 */
//=============================================================================================
WazaTarget GetWazaTarget( WazaID waza, const BTL_POKEPARAM* attacker )
{
  if( waza != WAZANO_NOROI )
  {
    return (WazaTarget)(WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_TARGET ));
  }
  else
  {
    if( attacker ){
      return calc::GetNoroiTargetType( attacker );
    }
    return (WazaTarget)WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_TARGET );
  }
}

//=============================================================================================
/**
 * 「のろい」の選択対象タイプを返す
 *
 * @param   attacker
 *
 * @retval  WazaTarget
 */
//=============================================================================================
WazaTarget GetNoroiTargetType( const BTL_POKEPARAM* attacker )
{
  return attacker->IsMatchType(POKETYPE_GHOST)? pml::wazadata::TARGET_OTHER_SELECT : pml::wazadata::TARGET_USER;
}

//=============================================================================================
/**
 * ワザターゲットをランダムで自動決定（サーバ計算用）
 *
 * @param   mainModule
 * @param   bpp
 * @param   waza
 *
 * @retval  BtlTargetPos
 */
//=============================================================================================
BtlPokePos DecideWazaTargetAuto( const MainModule* mainModule, const POKECON* pokeCon, const BTL_POKEPARAM* bpp, WazaID waza, b32 IsClient )
{
  BtlRule rule = mainModule->GetRule();
  BtlPokePos targetPos = BTL_POS_NULL;
  BtlPokePos myPos = mainModule->PokeIDtoPokePos( pokeCon, bpp->GetID() );
  WazaTarget  targetType = static_cast<WazaTarget>(WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_TARGET ));

  if( waza == WAZANO_NOROI ){
    targetType = GetNoroiTargetType( bpp );
  }

  // シングル
  if( rule == BTL_RULE_SINGLE )
  {
    switch( targetType ){
    case pml::wazadata::TARGET_OTHER_SELECT:  ///< 自分以外の１体（選択）
    case pml::wazadata::TARGET_ENEMY_SELECT:  ///< 敵１体（選択）
    case pml::wazadata::TARGET_ENEMY_RANDOM:  ///< 敵ランダム
    case pml::wazadata::TARGET_ENEMY_ALL:     ///< 敵側２体
    case pml::wazadata::TARGET_OTHER_ALL:     ///< 自分以外全部
      targetPos = MainModule::GetOpponentPokePosByRule( rule, myPos, 0 );
      return targetPos;

    case pml::wazadata::TARGET_USER:                ///< 自分１体のみ
    case pml::wazadata::TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体
      targetPos = myPos;
      return targetPos;

    case pml::wazadata::TARGET_ALL:
    case pml::wazadata::TARGET_UNKNOWN:
    default:
      break;
    }
    return BTL_POS_NULL;
  }
  // ダブル、乱入共通
  else
  {
    BtlExPos exPos;
    u8 pokeID[ BTL_POS_NUM ];
    u8 pokeCnt = 0;
    u8 fHitFar = false;

    switch( targetType ){
    case pml::wazadata::TARGET_OTHER_SELECT:       ///< 通常ポケ（１体選択）
    case pml::wazadata::TARGET_ENEMY_SELECT:       ///< 相手側ポケ（１体選択）
    case pml::wazadata::TARGET_ENEMY_RANDOM:       ///< 相手ポケ１体ランダム
      if( fHitFar ){
        exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ENEMY, myPos );
      }else{
        exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );
      }
      break;

    case pml::wazadata::TARGET_FRIEND_USER_SELECT: ///< 自分を含む味方ポケ（１体選択）
      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_MYTEAM, myPos );
      break;

    case pml::wazadata::TARGET_FRIEND_SELECT:      ///< 自分以外の味方ポケ（１体選択）
      if( fHitFar ){
        exPos = EXPOS_MAKE( BTL_EXPOS_FULL_FRIENDS, myPos );
      }else{
        exPos = EXPOS_MAKE( BTL_EXPOS_AREA_FRIENDS, myPos );
      }
      break;

    case pml::wazadata::TARGET_USER:               ///< 自分のみ
      return myPos;

    default:
      return BTL_POS_NULL;
    }

    // 生きてるポケモンがいればその中からランダム
    pokeCnt = mainModule->ExpandExistPokeID( pokeCon, exPos, pokeID );
    if( pokeCnt )
    {
      u8 rnd = IsClient? System::GflUse::GetPublicRand( pokeCnt ) : GetRand( pokeCnt );
      return mainModule->PokeIDtoPokePos( pokeCon, pokeID[rnd] );
    }

    // いなければ該当位置からランダム
    pokeCnt = mainModule->ExpandBtlPos( exPos, pokeID );
    if( pokeCnt )
    {
      u8 rnd = IsClient? System::GflUse::GetPublicRand( pokeCnt ) : GetRand( pokeCnt );
      return pokeID[ rnd ];
    }

    return BTL_POS_NULL;
  }
}

//=============================================================================================
/**
 * ポケモンID（max=31）６個分の配列を 32bit にpackする
 *
 * @param   pokeIDList
 *
 * @retval  u32
 */
//=============================================================================================
u32 PokeIDx6_Pack32bit( const u8* pokeIDList )
{
  u32 pack = 0;

  for(u32 i=0; i<BTL_EXIST_POS_MAX; ++i)
  {
    pack |= ((pokeIDList[i] & 0x1f) << (i*5));
  }

  return pack;
}
//=============================================================================================
/**
 * 32bit にpackするしたポケモンID（max=31）６個分を配列に書き戻し
 *
 * @param   pokeIDList
 *
 * @retval  u32
 */
//=============================================================================================
void PokeIDx6_Unpack32bit( u32 pack, u8* pokeIDList )
{
  for(u32 i=0; i<BTL_EXIST_POS_MAX; ++i)
  {
    pokeIDList[ i ] = (pack & 0x1f);
    pack >>= 5;
  }
}



//=============================================================================================
// ランダム技選択
//=============================================================================================

bool is_include( const WazaNo* tbl, u32 tblElems, WazaNo wazaID )
{
  u32 i;
  for(i=0; i<tblElems; ++i){
    if( tbl[i] == wazaID ){
      return true;
    }
  }
  return false;
}

//=============================================================================================
/**
 * 特定のワザを除外した中からランダムでワザを決定（ゆびをふる専用？）
 *
 * @param   omitWazaTbl       除外するワザIDテーブル
 * @param   tblElems          除外するワザIDテーブルの要素数
 *
 * @retval  WazaID    決定したワザID
 */
//=============================================================================================
WazaNo RandWaza(const WazaNo* omitWazaTbl, u16 tblElems )
{
  u16 waza, cnt, i;

  for(cnt=0, waza=1; waza<WAZANO_MAX; ++waza)
  {
    if( is_include(omitWazaTbl, tblElems, static_cast<WazaNo>(waza)) ){
      continue;
    }
    g_WazaStoreWork[ cnt++ ] = static_cast<WazaNo>(waza);
  }

  if( cnt ){
    i = GetRand( cnt );
    return g_WazaStoreWork[ i ];
  }
  return WAZANO_NULL;
}


//=============================================================================================
/**
 * ワザターゲットをランダムで自動決定
 *  （クライアント用：整合性チェックサーバの計算に影響を与えないよう乱数系の状態をキープする）
 *
 * @param   mainModule
 * @param   pokeCon
 * @param   bpp
 * @param   waza
 * @param   pRandContextSaveWork    乱数コンテキスト保存用の一時ワーク領域（クライアントが用意）
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos  DecideWazaTargetAutoForClient( const MainModule* mainModule, const POKECON* pokeCon,
      const BTL_POKEPARAM* bpp, WazaID waza, RandContext*  pRandContextSaveWork )
{
  BtlPokePos pos;

  pos = DecideWazaTargetAuto( mainModule, pokeCon, bpp, waza, true );

  return pos;
}


//=============================================================================================
// ルールごとに変わる要素
//=============================================================================================
//=============================================================================================
/**
 * ワザの対象を選択する必要があるか？
 */
//=============================================================================================
bool RULE_IsNeedSelectTarget( BtlRule rule )
{
  return ( rule != BTL_RULE_SINGLE );
}
//=============================================================================================
/**
 * 手持ちポケモンの開始インデックス
 *
 * @param   rule
 * @param   numCoverPos   クライアントの担当位置数（ダブル時、通常は２，マルチなら１になる）
 */
//=============================================================================================
u8 RULE_HandPokeIndex( BtlRule rule, u8 numCoverPos )
{
  GFL_UNUSED( rule );
  return numCoverPos;
}


//=============================================================================================
// 賞金計算
//=============================================================================================

u32 calcWinMoney_Sub( const BSP_TRAINER_DATA* trData, const pml::PokeParty* party )
{
  if( party && trData )
  {
    u32 poke_cnt = party->GetMemberCount();
    if( poke_cnt )
    {
      const pml::pokepara::PokemonParam *pp = party->GetMemberPointerConst( poke_cnt-1 );

      u32 money_ratio = trData->GetGoldParam();
      u32 level       = pp->GetLevel();

      return (level * money_ratio * 4);
    }
  }
  return 0;
}

/**
 *  対戦トレーナーデータ＆パーティデータから、勝ったときの賞金額を計算
 */
u32 CalcWinMoney( const BATTLE_SETUP_PARAM* sp )
{
  u32 money = 0;
  if( sp->competitor == BTL_COMPETITOR_TRAINER )
  {
    u8 clientID = BTL_CLIENT_ENEMY1;
    if( (sp->tr_data[clientID] != NULL) && (sp->party[clientID] != NULL) ){
      money += calcWinMoney_Sub( sp->tr_data[clientID], sp->party[clientID] );
    }

    clientID = BTL_CLIENT_ENEMY2;
    if( (sp->tr_data[clientID] != NULL) && (sp->party[clientID] != NULL) ){
      money += calcWinMoney_Sub( sp->tr_data[clientID], sp->party[clientID] );
    }
  }
  return money;
}

/**
 * 自分パーティーデータ＆Misc（バッジ数、手持ちお金）から、敗北時に失う金額を計算
 */
u32 CalcLoseMoney( const BATTLE_SETUP_PARAM* sp, const POKECON * pokeCon )
{
  const BTL_PARTY* party = pokeCon->GetPartyDataConst( BTL_CLIENT_PLAYER );
  u32 memberCount = party->GetMemberCount();
  u32 level_max = 0;
  for(u32 i=0; i<memberCount; ++i)
  {
    const BTL_POKEPARAM* member = party->GetMemberDataConst( i );
    u32 level = member->GetValue( BTL_POKEPARAM::BPP_LEVEL );
    if( level > level_max ) level_max = level;
  }

  u32 loseMoney = sp->gameData->GetMisc()->CalcPenaltyMoney( level_max );
  return loseMoney;
}
} // end of namespace 'calc'
GFL_NAMESPACE_END(btl)



