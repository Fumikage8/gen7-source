//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file EncountPokeSet.cpp
 *  @brief エンカウントするポケモンセットを取得
 *  @author tomoya takahashi / hosaka genya
 *  @date 2011.12.21
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.25
 */
//=============================================================================

#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldStatic/include/Time/EvTime.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

// gamesys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/TimeZone.h"

// savedata
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/MiscSave.h"

// system
#include "System/include/GflUse.h"

//conv_header
#include "niji_conv_header/field/encount/FieldEncountDataTypes.h"

// ロトムパワー
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )


//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

///アイテムを持たせる確率定義
const u8 PokeSet::DATA_ItemRangeTable[ITEM_RANGE_TBL_NUM][pml::personal::ITEM_SLOT_NUM] = {
 {50,5,0},  //通常
 {60,20,0}, //通常+ふくがん
 {50,5,1},  //特別
 {60,20,5}, //特別+ふくがん
};

static const u8 RARE_TRY_COUNT_HIKARUOMAMORI = 2; ///<「ひかるおまもり」レア抽選+2回

enum{
  TALENT_V_NUM_LEGEND = 3,
  TALENT_V_NUM_POKETRACER_LV1 = 1,
  TALENT_V_NUM_POKETRACER_LV2 = 2,
  TALENT_V_NUM_POKETRACER_LV3 = 3,
};

//----------------------------------------------------------------------------
/**
 *  @brief  EncountPokeParam
 */
//-----------------------------------------------------------------------------
EncountPokeParam::EncountPokeParam()
{
  Clear();
}

void EncountPokeParam::Clear()
{
  monsNo = 0;
  item = ITEM_REQ_DEFAULT;
  level = 1;
  form = 0;
  rare = RARE_REQ_DEFAULT;
  spabi = SPABI_REQ_DEFAULT;
  sex = SEX_REQ_DEFAULT;
  talentVNum = 0;
  seikaku = pml::pokepara::INIT_SPEC_SEIKAKU_RANDOM;

  gfl2::std::MemClear(waza,sizeof(u16)*4);

  for( u32 i=0; i< pml::pokepara::POWER_NUM; i++ ) {
    talent[i] = pml::pokepara::INIT_SPEC_TALENT_RANDOM;
    effort[i] = 0;
  }
  encount_mode = WILDENC_MODE_NORMAL;
  aura_mode = WILDENC_AULA_MODE_NONE;
  aura_color_r = 0;
  aura_color_g = 0;
  aura_color_b = 0;
  aura_color_a = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ReinforcePokeParam
 */
//-----------------------------------------------------------------------------
ReinforcePokeParam::ReinforcePokeParam()
{
  Clear();
}

void ReinforcePokeParam::Clear()
{
  reinforce_count = 0;
  reinforce_type = REINFORCE_POKE_TYPE_MUST;
  poke.Clear();
}

//----------------------------------------------------------------------------
/**
 *  @brief  PokeSetコンストラクタ 
 */
//-----------------------------------------------------------------------------
PokeSet::PokeSet(void):
  m_enc_num(ENC_POKENUM_NORMAL),
  m_tbl_num(0),
  m_element_num(0),
  m_attr(ATTR_ERROR),
  m_enc_type(Encount::TYPE_NORMAL),
  m_prob_calctype(ENCPROB_CALCTYPE_NORMAL),
  m_far_bg(btl::bg::FAR_TYPE_NONE),
  m_near_bg(btl::bg::NEAR_TYPE_NONE),
  m_btl_eff(BATTLE_EFFECT_MAX),
  m_btl_mode(WILDENC_MODE_NORMAL),
  m_my_id(0),
  m_real_region_formno(0),
  m_intruderIndex(0),
  m_weather( weather::SUNNY)
{
  this->Clear();
}

//----------------------------------------------------------------------------
/**
 *  @brief  PokeSet 初期化 
 *
 *  @param  p_gman    ゲームマネージャー
 *  @param  enc_type  エンカウントタイプ
 *  @param  weather   天気
 */
//-----------------------------------------------------------------------------
void PokeSet::Initialize(GameSys::GameManager* p_gman, Encount::Type enc_type, Attr attr, weather::WeatherKind weather, bool isFishing )
{
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  const Savedata::MyStatus* p_myst = p_gdata->GetPlayerStatusConst();

  m_attr = attr;
  m_enc_type = enc_type;
  m_my_id = p_myst->GetID();
  m_real_region_formno = p_gdata->GetRealRegionFormNo();

  if( m_enc_type == Encount::TYPE_FORCE ){
    m_flags.enc_force_f = true;
  }

  m_flags.fishing_f = isFishing;

  //イベントフラグチェック
  {
    EventWork* evwork = p_gdata->GetEventWork();
    m_flags.gameclear_f = evwork->CheckEventFlag(SYS_FLAG_GAME_CLEAR);
    m_flags.spray_f = !evwork->CanUseMushiyoke(); //スプレーが使えない=スプレー使用中

    //「ひかるおまもり」レア率アップチェック
    m_flags.rare_up_f = p_gdata->GetMyItem()->Check( ITEM_HIKARUOMAMORI, 1 );

    m_flags.flash_f = false;
    m_flags.kuroi_biidoro_f = false;
    m_flags.siroi_biidoro_f = false;
  }

  // 道具、アイテムでの特性効果適用フラグ更新
  {
    pml::PokeParty* party = p_gdata->GetPlayerParty();
    pml::pokepara::PokemonParam* p_pp;
    int  idx;

    //タマゴでない先頭ポケモン(瀕死は可)のパラメータチェック
    idx = party->GetMemberTopIndex(pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG);
    if( idx != pml::PokeParty::MEMBER_INDEX_ERROR ){

      p_pp = party->GetMemberPointer(idx);
      SetEffectApplyCheckInfo( &m_topPokeInfo, *p_pp);

      //特性効果発揮チェック
      CheckMonsSpa( &m_flags, weather, NULL, m_topPokeInfo );
      
      //スプレー及び低レベルエンカウント回避チェックに用いるレベルを取得
      if( m_flags.spa_low_lv_rm || m_flags.spray_f ){
        m_topPokeInfo.spray_lv = p_pp->GetLevel();
      }
    }
  }

  // 共通フラグを乱入にコピーしておく
  m_flagsIntruder.common_flags = m_flags.common_flags;

  // 天候定義を保存しておく
  m_weather = weather;
}

//----------------------------------------------------------------------------
/**
 *  @brief  クリア処理
 */
//-----------------------------------------------------------------------------
void PokeSet::Clear()
{
  for( int i=0; i < pml::PokeParty::MAX_MEMBERS;i++ ){
    m_pokeTbl[i].Clear();
  }
  for( int i=0; i < REINFORCE_POKE_NUM;i++ ){
    m_reinforceTbl[i].Clear();
  }
  gfl2::std::MemClear( &m_topPokeInfo, sizeof(EffectApplyCheckInfo) );
  gfl2::std::MemClear( &m_topPokeInfoIntruder, sizeof(EffectApplyCheckInfo) );
  // @fix GFNMCat[1268] エンカウントデータの有無が必要なため共通関数を用意
  this->ResetEncountData();
  m_enc_num = ENC_POKENUM_NORMAL;
  m_tbl_num = 0;
  m_element_num = 0;
  m_reinforce_num = 0;
  m_attr = ATTR_ERROR;
  m_enc_type = Encount::TYPE_NORMAL;
  m_prob_calctype = ENCPROB_CALCTYPE_NORMAL;
  m_far_bg = btl::bg::FAR_TYPE_NONE;
  m_near_bg = btl::bg::NEAR_TYPE_NONE;
  m_btl_eff = BATTLE_EFFECT_MAX;
  m_btl_mode = WILDENC_MODE_NORMAL;
  m_my_id = 0;
  m_real_region_formno = 0;
  m_flags.common_flags = 0;
  m_flags.spa_flags = 0;
  m_flagsIntruder.common_flags = 0;
  m_flagsIntruder.spa_flags = 0;
  m_intruderIndex = 0;
  m_weather = weather::SUNNY;
}

//----------------------------------------------------------------------------
/**
 *  @brief  特性/道具使用によるエンカウント率変動計算
 *
 *  @param  in_prob 計算元の確率
 *  @param  cp_player プレイヤークラス
 *
 *  @retval out_prob  計算後の確率
 */
//-----------------------------------------------------------------------------
u32 PokeSet::EncountProbCalc( const u32 in_prob, MoveModel::FieldMoveModelPlayer * cp_player )
{
   u32 new_prob = in_prob;

  //特性による確率変更
  if( m_flags.spa_rate_up ){
    new_prob *= 2;
  }else if(m_flags.spa_rate_down){
    new_prob /= 2;
  }else if(m_topPokeInfo.mons_item == ITEM_KIYOMENOOHUDA || m_topPokeInfo.mons_item == ITEM_KIYOMENOOKOU)
  {
    // 装備アイテム(きよめのおふだ/きよめのおこう)による確率変動
    // sangoではkujiraに引き続きアイテムは特性を打ち消す挙動にした。次回作では要検討。
    // nijiでは特性が優先(森本さんに確認しました)
    new_prob = (in_prob * 2) / 3;
  }

  //フラッシュによる確率変動
  if( m_flags.flash_f )
  {
    new_prob /= 4;
  }

  const u32 max_prob = 100;
  new_prob = this->GetRideAdjustEncountProb( new_prob, max_prob, cp_player);

  // ロトムパワー補正
  new_prob = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->CalcEncount(new_prob);

  // 上限チェック
  if(new_prob > max_prob)
  {
    new_prob = max_prob;
  }
  
  return new_prob;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ROMのエンカウントテーブルからポケモンを抽選
 *
 * @param data            エンカウントデータ
 *
 * @retval true    取得できた。
 * @retval false   取得できない。
 */
/* -------------------------------------------------------------------------*/
bool PokeSet::SetNormalEncountData( const ENCOUNT_DATA& data, BattleEffectId btl_eff )
{
  const COMMON_TIME_ZONE_DATA& now_time_enc_poke_block = GetNowTimeZoneEncountData( data);

  //エンカウントデータからテーブルを取得
  const COMMON_POKE_DATA* enc_poke_block = NULL; 
  enc_poke_block = GetEncountPokeBlockData( now_time_enc_poke_block );

  if( enc_poke_block == NULL ) return false;
  
  m_tbl_num = MONS_NUM_NORMAL; //エンカウントテーブル数
  m_element_num = 1; 
  m_enc_num = ENC_POKENUM_NORMAL;

  // @fix GFNMCat[1268] エンカウントデータの有無が必要なため共通関数を用意
  // エンカウントデータを保持しておく
  this->SetEncountData( data);

  //エンカウントポケモン抽選
  if( LotteryEncountPoke( enc_poke_block, now_time_enc_poke_block.minLevel, now_time_enc_poke_block.maxLevel ) == false ) return false;

  m_btl_eff = btl_eff;
  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  シンボルエンカウントパラメータを 汎用エンカウントポケモンデータ型にコンバート
 *
 *  @retval true    取得できた。
 *  @retval false   取得できない。
 */
//-----------------------------------------------------------------------------
bool PokeSet::ConvSymbolEncountPokeParam( const SymbolEncountParam* param, EncountPokeParam* epp )
{
  epp->Clear();

  epp->monsNo = param->monsNo;
  epp->item = param->item;
  epp->level = param->level;
  epp->form = param->form;
  if (epp->form == MONS_FORM_REGION_CODE)
  {
    epp->form = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetRealRegionFormNo();
  }
  epp->rare = param->rare;
  epp->spabi = param->spabi;
  epp->sex = param->sex;
  epp->seikaku = param->seikaku;  //性格 

  epp->waza[0] = param->waza[0];
  epp->waza[1] = param->waza[1];
  epp->waza[2] = param->waza[2];
  epp->waza[3] = param->waza[3];

  if( param->talent_hp < -1 ){  //-2以下が指定されている時、符号反転してV数指定に使う
    epp->talentVNum = (param->talent_hp*-1)-1;
  }else{
    epp->talentVNum = pml::pokepara::INIT_SPEC_TALENT_V_NUM_NONE;

    ///< 個体値：HP(-1～31)
    if( param->talent_hp >= 0 && param->talent_hp <= 31 ){
      epp->talent[pml::pokepara::POWER_HP] = param->talent_hp;
    }
    ///< 個体値：ATK(-1～31)
    if( param->talent_atk >= 0 && param->talent_atk <= 31 ){
      epp->talent[pml::pokepara::POWER_ATK] = param->talent_atk;
    }
    ///< 個体値：DEF(-1～31)
    if( param->talent_def >= 0 && param->talent_def <= 31 ){
      epp->talent[pml::pokepara::POWER_DEF] = param->talent_def;
    }
    ///< 個体値：SPATK(-1～31)
    if( param->talent_spatk >= 0 && param->talent_spatk <= 31 ){
      epp->talent[pml::pokepara::POWER_SPATK] = param->talent_spatk;
    }
    ///< 個体値：SPDEF(-1～31)
    if( param->talent_spdef >= 0 && param->talent_spdef <= 31 ){
      epp->talent[pml::pokepara::POWER_SPDEF] = param->talent_spdef;
    }
    ///< 個体値：AGI (-1～31)
    if( param->talent_agi >= 0 && param->talent_agi <= 31 ){
      epp->talent[pml::pokepara::POWER_AGI] = param->talent_agi;
    }
  }
  epp->effort[pml::pokepara::POWER_HP] = param->effort_hp;       ///< 努力値：HP(0-252 かつ 全ての努力値の合計が510以下)
  epp->effort[pml::pokepara::POWER_ATK] = param->effort_atk;      ///< 努力値：ATK(0-252 かつ 全ての努力値の合計が510以下)
  epp->effort[pml::pokepara::POWER_DEF] = param->effort_def;      ///< 努力値：DEF(0-252 かつ 全ての努力値の合計が510以下)
  epp->effort[pml::pokepara::POWER_SPATK] = param->effort_spatk;    ///< 努力値：SPATK(0-252 かつ 全ての努力値の合計が510以下)
  epp->effort[pml::pokepara::POWER_SPDEF] = param->effort_spdef;    ///< 努力値：SPDEF(0-252 かつ 全ての努力値の合計が510以下)
  epp->effort[pml::pokepara::POWER_AGI] = param->effort_agi;      ///< 努力値：AGI (0-252 かつ 全ての努力値の合計が510以下)

  epp->encount_mode = param->mode;    //エンカウントモード WildEncMode
  epp->aura_mode = param->aura_mode;  //オーラ効果 WildEncAulaMode

  epp->aura_color_r = param->aura_color_r;  //オーラの色 R(0-255)
  epp->aura_color_g = param->aura_color_g;  //オーラの色 G(0-255)
  epp->aura_color_b = param->aura_color_b;  //オーラの色 B(0-255)
  epp->aura_color_a = param->aura_color_a;  //オーラの色 A(0-255)
  return true;
}


//----------------------------------------------------------------------------
/**
 *  @brief  シンボルエンカウントデータをセット
 *
 *  @retval true    取得できた。
 *  @retval false   取得できない。
 */
//-----------------------------------------------------------------------------
bool PokeSet::SetSymbolEncountData( const SymbolEncountParam* param )
{
  EncountPokeParam* epp = &m_pokeTbl[0];
  
  epp->Clear();

  m_tbl_num = 1;
  m_element_num = 1; 
  m_enc_num = ENC_POKENUM_NORMAL;

  m_far_bg = static_cast<btl::bg::bgsys_far_typeID>(param->far_bg);
  m_near_bg = static_cast<btl::bg::bgsys_near_typeID>(param->near_bg);
  m_btl_eff = static_cast<BattleEffectId>(param->btl_eff);

  SetBattleMode(static_cast<WildEncMode>(param->mode));

  ConvSymbolEncountPokeParam(param, epp);
  
  //１００％レア、１００％ノーマルの書き換えを行う(momiji仕様)
  if (epp->rare == RARE_REQ_MUST)//レア固定設定がされている
  {
    if (IsMustRareEncZone())//対象ゾーンである
    {
      GameSys::GameData* p_gdata = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
      Savedata::Misc* p_misc = p_gdata->GetMisc();
      if (!p_misc->CheckMustRareEncTrigger())//１００％フラグのトリガーがひかれていない
      {
        epp->rare = RARE_REQ_NOT;
      }
      //抽選終了したら、誤動作防止のためレアトリガーをリセット
      //p_misc->ResetMustRareEncTrigger(); << @fix MMCat[497] UB世界のシングルエンカウントのポケモン抽選に関して >> 必ずレアになるフラグをリセットする場所をEventSymbolEncount.cppへ移動
    }
    else
    {
      //@note　momijiでRARE_REQ_MUSTを設定しているのは特定ゾーンのシンボルポケモンだけのため、他で設定がされている場合はレアが出ない設定に書き換える
      epp->rare = RARE_REQ_NOT;
    }
  }

  return true;
}

bool PokeSet::IsMustRareEncZone(void)
{
  GameSys::GameData* p_gdata = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Field::ZoneID zone_id = p_gdata->GetNowZoneID();
  if (zone_id == Z_F0701PH0901) return true;
  if (zone_id == Z_F0701PH1001) return true;
  if (zone_id == Z_F0701PH1101) return true;
  if (zone_id == Z_F0701PH1201) return true;

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  パラメータ指定でエンカウントデータをセット
 */
//-----------------------------------------------------------------------------
bool PokeSet::SetEncountDataFromParam( MonsNo monsNo, FormNo formNo, u32 level, BattleEffectId btl_eff )
{
  EncountPokeParam param;
 
  param.monsNo = monsNo;
  param.level = level;
  param.form = formNo;

  return SetEncountDataFromParam( param, btl_eff );
}

//----------------------------------------------------------------------------
/**
 *  @brief  パラメータ指定でエンカウントデータをセット(データクラスによる指定)
 */
//-----------------------------------------------------------------------------
bool PokeSet::SetEncountDataFromParam( const EncountPokeParam& param, BattleEffectId btl_eff )
{
  m_tbl_num = 1;
  m_element_num = 1; 
  m_enc_num = ENC_POKENUM_NORMAL;
  m_btl_eff = btl_eff;

  m_pokeTbl[0] = param;
  
  //低レベルエンカウント回避チェック
  if( CheckLowLevelEncountAvoid( param.level ) )
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief 抽選したポケモンの情報を使用して引数で渡したポケモンパラメーターのセットアップをする
 *  @param pPokeParam セットアップするポケモンパラメーター
 *  @param pRandom 乱数管理インスタンス
 *  @param intruder_type 乱入種類
 *  @param rare_try_count レアになるまで乱数を引き直す回数
 *  @retval true 抽選成功
 *  @retval true 抽選失敗
 */
//-----------------------------------------------------------------------------
bool PokeSet::SetupLotteryIntruderPokeParam( pml::pokepara::PokemonParam* pPokeParam, gfl2::math::SFMTRandom* pRandom, IntruderType intruder_type, u8 rare_try_count)
{
  GFL_ASSERT_MSG( pRandom != NULL, "PokeSet::CheckEffectApplyIntruder : pRandomがNULLです\n");

  // 現在の時間帯のエンカウントデータ取得
  const COMMON_TIME_ZONE_DATA& now_time_enc_poke_block = GetNowTimeZoneEncountData( m_encount_data);

  // エンカウントポケモンデータを初期化
  EncountPokeParam enc_poke_param;

  bool isLotterySuccess = false;

  if( intruder_type == INTRUDER_TYPE_NORMAL)
  {
    // ハガネタイプ優先エンカウント
    if( m_flagsIntruder.spa_hagane_up )
    {
      isLotterySuccess = LotFixTypeEncountIntruder( &enc_poke_param, now_time_enc_poke_block, POKETYPE_HAGANE, pRandom );
    }
    // 電気タイプ優先エンカウント
    else if( m_flagsIntruder.spa_denki_up )
    {
      isLotterySuccess = LotFixTypeEncountIntruder( &enc_poke_param, now_time_enc_poke_block, POKETYPE_DENKI, pRandom );
    }
  }
  else
  {
    // ハガネタイプ優先エンカウント
    if( m_flagsIntruder.spa_hagane_up )
    {
      isLotterySuccess = LotFixTypeEncountIntruderEx( &enc_poke_param, now_time_enc_poke_block, POKETYPE_HAGANE, pRandom, intruder_type );
    }
    // 電気タイプ優先エンカウント
    else if( m_flagsIntruder.spa_denki_up )
    {
      isLotterySuccess = LotFixTypeEncountIntruderEx( &enc_poke_param, now_time_enc_poke_block, POKETYPE_DENKI, pRandom, intruder_type );
    }
  }

  // 通常乱入抽選
  if( isLotterySuccess == false)
  {
    // ブロック取得
    const COMMON_POKE_DATA* enc_poke_block = NULL; 
    IntruderType intruder_type_fix;
    s8 block_index = this->LotteryIntruderPokeBlock( &intruder_type_fix, pRandom, intruder_type);
    enc_poke_block = GetEncountIntruderPokeBlockData( now_time_enc_poke_block, intruder_type_fix, block_index );
    if( enc_poke_block == NULL )
    {
      return false;
    }

    // @fix GFNMCat[3757] 天候乱入に抽選したが、ポケモンが登録されていないケースは通常乱入で再抽選する
    if( intruder_type_fix != INTRUDER_TYPE_NORMAL )
    {
      if( enc_poke_block[ 0 ].monsNo == MONSNO_NULL )
      {
        block_index = this->LotteryIntruderPokeBlock( &intruder_type_fix, pRandom, INTRUDER_TYPE_NORMAL);
        enc_poke_block = GetEncountIntruderPokeBlockData( now_time_enc_poke_block, intruder_type_fix, block_index );
        if( enc_poke_block == NULL )
        {
          return false;
        }
      }
    }

    // NMCat[259] 最大レベルに最小レベルを渡していたので最大レベルを渡すように修正
    isLotterySuccess = LotteryEncountIntruderPoke( &enc_poke_param, enc_poke_block, intruder_type_fix, now_time_enc_poke_block.minLevel, now_time_enc_poke_block.maxLevel, pRandom);
  }

  // 抽選失敗
  if( isLotterySuccess == false )
  {
    return false;
  }

  // ポケパラ設定
  pml::pokepara::InitialSpec spec;
  InitializePokeParamInitialSpecIntruder( &spec, &enc_poke_param, m_flagsIntruder, rare_try_count);
  pPokeParam->Setup( spec);
  InitializePokeParam( pPokeParam, spec, &enc_poke_param, m_flagsIntruder, pRandom);

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief シンボル援軍ポケモンのデータ数を取得する
 *
 *  @retval 援軍ポケモンの数
 */
//-----------------------------------------------------------------------------
int PokeSet::GetReinforcePokeNum( void )
{
  return m_reinforce_num;
}


//----------------------------------------------------------------------------
/**
 *  @brief  シンボルエンカウント援軍データをセット
 */
//-----------------------------------------------------------------------------
bool PokeSet::SetReinforcePokeParam( const SymbolEncountParam* param, const ReinforcePokeType reinforce_type, const int reinforce_count )
{
  if( m_reinforce_num >= REINFORCE_POKE_NUM ){
    GFL_ASSERT(0);
    return false;
  }
  ReinforcePokeParam* rpp = &m_reinforceTbl[m_reinforce_num];
  rpp->Clear();

  ConvSymbolEncountPokeParam( param, &rpp->poke );
  rpp->reinforce_type = reinforce_type;
  rpp->reinforce_count = reinforce_count;

  m_reinforce_num++;
  return true;
}


//----------------------------------------------------------------------------
/**
 *  @brief シンボル援軍ポケモンの情報を取得する
 *
 *  @param index   援軍ポケモンデータIndex
 *  @param pPokeParam セットアップするポケモンパラメーター
 *  @param reinforce_type  援軍抽選タイプ
 *  @param reinforce_count 援軍抽選カウント上限
 *  @retval true 取得成功
 *  @retval true 取得失敗
 */
//-----------------------------------------------------------------------------
bool PokeSet::GetReinforcePokeParam( int index,
  pml::pokepara::PokemonParam* pPokeParam, ReinforcePokeType* reinforce_type, int* reinforce_count )
{
  if( index >= m_reinforce_num ){
    GFL_ASSERT_MSG( 0, "PokeSet::GetReinforcePokeParam : %d >= %d\n", index, m_reinforce_num );
    return false;
  }

  // ポケパラ設定
  pml::pokepara::InitialSpec spec;
  InitializePokeParamInitialSpec( &spec, &m_reinforceTbl[index].poke, m_flagsIntruder);
  pPokeParam->Setup( spec);
  InitializePokeParam( pPokeParam, spec, &m_reinforceTbl[index].poke, m_flagsIntruder, NULL );

  if ( reinforce_type != NULL )
  {
    *reinforce_type = m_reinforceTbl[index].reinforce_type;
  }
  if ( reinforce_count != NULL )
  {
    *reinforce_count = m_reinforceTbl[index].reinforce_count;
  }
  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief 指定ポケモンパラメーターから特性、アイテム効果のfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff発動をチェックする
 *  @param pPokeParam チェックするポケモンパラメーター
 *  @param pRandom 乱数管理インスタンス
 *  @note SetupLotteryIntruderPokeParam()の直前に呼んでください
 */
//-----------------------------------------------------------------------------
void PokeSet::CheckEffectApplyIntruder( const pml::pokepara::PokemonParam& pokeParam, gfl2::math::SFMTRandom* pRandom)
{
  GFL_ASSERT_MSG( pRandom != NULL, "PokeSet::CheckEffectApplyIntruder : pRandomがNULLです\n");

  // 特性フラグ初期化
  m_flagsIntruder.spa_flags = 0;

  SetEffectApplyCheckInfo( &m_topPokeInfoIntruder, pokeParam);
  CheckMonsSpa( &m_flagsIntruder, m_weather, pRandom, m_topPokeInfoIntruder );

  //スプレー及び低レベルエンカウント回避チェックに用いるレベルを取得
  if( m_flagsIntruder.spa_low_lv_rm || m_flagsIntruder.spray_f )
  {
    m_topPokeInfoIntruder.spray_lv = pokeParam.GetLevel();
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  メンバにセットされたENC_POKE_PARAM[]を用いてPokePartyをセット
 *
 *  @retval true    取得できた。
 *  @retval false   取得できない。
 */
//-----------------------------------------------------------------------------
bool PokeSet::GetPokeParty( gfl2::heap::HeapBase* p_heap, pml::PokeParty* p_party ) const
{
  if( m_enc_num == 0 )
  {
    GFL_ASSERT(0);
    return false;
  }

  for( int i = 0;i < m_enc_num; ++i )
  {
    AddPoke( p_heap, &m_pokeTbl[i], p_party );
  }

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  メンバにセットされたENC_POKE_PARAM[]を用いてDefaultPowerUpDescをセット
 *
 *  @retval true    取得できた。
 *  @retval false   取得できない。
 */
//-----------------------------------------------------------------------------
bool PokeSet::GetDefaultPowerUp( gfl2::heap::HeapBase* p_heap, DefaultPowerUpDesc* p_powerUp ) const
{
  GFL_UNUSED(p_heap);

  if( m_enc_num == 0 )
  {
    GFL_ASSERT(0);
    return false;
  }

  for( int i = 0;i < m_enc_num; ++i )
  {
    SetPowerUp( &m_pokeTbl[i], &p_powerUp[i] );
  }

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  セットされたパラメータからバトルエフェクトIDを取得
 */
//-----------------------------------------------------------------------------
BattleEffectId PokeSet::GetBattleEffectId( void ) const
{
  // 明示指定がある場合は指定を優先する
  if( m_btl_eff != BATTLE_EFFECT_MAX )
  {
    return m_btl_eff;
  }

  // 明示指定がない場合はアトリビュートから取得
  return static_cast<BattleEffectId>( Attribute::GetBattleEffectID( m_attr));
}

/* -------------------------------------------------------------------------*/
/**
 * @brief セットされているポケモンが指定したポケモンと同じか
 *
 * @return trueの時同じ
 */
/* -------------------------------------------------------------------------*/ 
bool PokeSet::IsSame( u32 mons_no ) const
{
  for( int i = 0; i< m_enc_num; ++i )
  {
    const EncountPokeParam* epp = &m_pokeTbl[i];
    if( epp->monsNo != mons_no )
    {
      return false;
    }
  }
  return true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief セットされているポケモンがすべて同じか
 *
 * @return trueの時同じ
 *
 * @note 1匹の時は必ずtrue
 */
/* -------------------------------------------------------------------------*/ 
bool PokeSet::IsAllSame( void ) const
{
  u32 mons_no = m_pokeTbl[0].monsNo;
  for( int i = 0; i< m_enc_num; ++i )
  {
    const EncountPokeParam* epp = &m_pokeTbl[i];
    if( epp->monsNo != mons_no )
    {
      return false;
    }
  }
  return true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief エンカウントするポケモンナンバー（先頭のポケモンナンバー）
 *
 * @return モンスターナンバー
 */
/* -------------------------------------------------------------------------*/
u32 PokeSet::GetTopMonsNo( void ) const
{
  return m_pokeTbl[0].monsNo;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief エンカウントするポケモンデータ（先頭のポケモンデータ）
 *
 * @return モンスターデータ
 */
/* -------------------------------------------------------------------------*/
const EncountPokeParam* PokeSet::GetTopMonsEncountParam( void ) const
{
  return &m_pokeTbl[0];
}

/* -------------------------------------------------------------------------*/
/**
 * @brief エンカウト数を取得
 *
 * @return エンカウント数
 */
/* -------------------------------------------------------------------------*/
int PokeSet::GetEncountNum( void ) const
{
  return m_enc_num;
}

/* -------------------------------------------------------------------------*/
/**
 * @breif エンカウントデータからMonsNoとFormNoを取得する
 * @param pEncData エンカウントデータ
 * @parma pOutMonsNo モンスターNoが格納される
 * @parma pOutFormNo フォルムNoが格納される
 */
/* -------------------------------------------------------------------------*/
void PokeSet::GetFirstMonsInfo(const ENCOUNT_DATA& pEncData, MonsNo* pOutMonsNo, FormNo* pOutFormNo) const
{
  const COMMON_POKE_DATA* encPokeData = this->GetEncountPokeBlockData(this->GetNowTimeZoneEncountData(pEncData));
  *pOutMonsNo = static_cast<MonsNo>(encPokeData[0].monsNo);
  *pOutFormNo = static_cast<FormNo>(encPokeData[0].form);
}

/* -------------------------------------------------------------------------*/
/**
 * @breif エンカウントデータのセット
 * @param data エンカウントデータ
 */
/* -------------------------------------------------------------------------*/
void PokeSet::SetEncountData( const ENCOUNT_DATA& data)
{
  gfl2::std::MemCopy( &data, &m_encount_data, sizeof(ENCOUNT_DATA) );
  m_is_encount_data_exist = true;
}

/* -------------------------------------------------------------------------*/
/**
 * @breif エンカウントデータのリセット
 */
/* -------------------------------------------------------------------------*/
void PokeSet::ResetEncountData()
{
  gfl2::std::MemClear( &m_encount_data, sizeof(ENCOUNT_DATA) );
  m_is_encount_data_exist = false;
}

/* -------------------------------------------------------------------------*/
/**
 * @breif エンカウントデータが存在するか
 */
/* -------------------------------------------------------------------------*/
bool PokeSet::IsEncountDataExist() const
{
  return m_is_encount_data_exist;
}

/* -------------------------------------------------------------------------*/
/**
 * @breif ポケモンサーチエンカウントを回避するか
 * @param encPokeLevel エンカウントポケモンのレベル
 * @retval 回避する
 * @retval 回避しない
 * @fix GFNMCat[1222][1223] ポケモンサーチのエンカウント回避チェックで外部からレベルを渡してチェックする処理を追加
 */
/* -------------------------------------------------------------------------*/
bool PokeSet::IsPokemonSearchEncountAvoid( u32 encPokeLevel)
{
  // 低レベルエンカウント回避チェック
  if( this->CheckLowLevelEncountAvoid( encPokeLevel) )
  {
    return true;
  }

  return false;
}

//============================================================================
//非公開関数
//============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  特性効果発揮チェック 
 *
 *  @param  pFlag 書き変えるフラグ
 *  @param  weather 天候コード 
 *  @param  pRandom 乱数
 *  @param  checkPokeParam チェックする情報
 */
//-----------------------------------------------------------------------------
void PokeSet::CheckMonsSpa( EffectApplyCheckFlag* pFlag, weather::WeatherKind weather, gfl2::math::SFMTRandom* pRandom, const EffectApplyCheckInfo& info)
{
  u32 rand = GetRandom( 100, pRandom);

  //------------------------------

  // 発動率100%
  switch( info.mons_spa)
  {
  // エンカウント率2倍(ありじごく他)
  case TOKUSEI_ARIZIGOKU:
  case TOKUSEI_NOOGAADO:
  case TOKUSEI_HAKKOU:
    pFlag->spa_rate_up = true;
    return;

  // エンカウント率1/2(あくしゅう他)
  case TOKUSEI_AKUSYUU:
  case TOKUSEI_SIROIKEMURI:
  case TOKUSEI_HAYAASI:
    pFlag->spa_rate_down = true;
    return;

  // 砂嵐エンカウント率1/2（すながくれ他)
  case TOKUSEI_SUNAGAKURE:
    if(weather == weather::SANDSTORM )
    {
      pFlag->spa_rate_down = true;
    }
    return;

  // あられエンカウント率1/2（ゆきがくれ他）
  case TOKUSEI_YUKIGAKURE:
    if( weather == weather::SNOW ||
        weather == weather::SNOWSTORM ||
        weather == weather::DIAMONDDUST
      )
    {
      pFlag->spa_rate_down = true;
    }
    return;

  // アイテムが手に入りやすい(ふくがん他)
  case TOKUSEI_HUKUGAN:
    pFlag->spa_item_rate_up = true;
    return;

  default:
    // 何もしない
    break;
  }

  //------------------------------

  // 発動率2/3
  if( rand < 67 )
  {
    switch( info.mons_spa)
    {
    // 手持ちと異なる性別(メロメロボディ他)
    case TOKUSEI_MEROMEROBODHI:
      if( info.mons_sex != pml::SEX_UNKNOWN )
      {
        pFlag->spa_sex_fix = true;
      }
      return;

    default:
      // 何もしない
      break;
    }
  }

  //------------------------------

  //発動率1/2
  if( rand >= 50 )
  {
    switch( info.mons_spa)
    {
    // ハガネタイプとのエンカウント率アップ
    case TOKUSEI_ZIRYOKU:
      pFlag->spa_hagane_up = true;
      return;

    // 電気タイプとのエンカウント率アップ
    case TOKUSEI_SEIDENKI:
      pFlag->spa_denki_up = true;
      return;

    // レベルの高いポケモンとエンカウントしやすい(やるき他)
    case TOKUSEI_HARIKIRI:
    case TOKUSEI_YARUKI:
    case TOKUSEI_PURESSYAA:
      pFlag->spa_high_lv_hit = true;
      return;

    // レベルの低いポケモンとエンカウントしない(威嚇他)
    case TOKUSEI_IKAKU:
    case TOKUSEI_SURUDOIME:
      pFlag->spa_low_lv_rm = true;
      return;

    // 手持ちと同じ性格(シンクロ他)
    case TOKUSEI_SINKURO:
      pFlag->spa_chr_fix = true;
      return;

    default:
      // 何もしない
      break;
    }
  }

  //------------------------------
}

//----------------------------------------------------------------------------
/**
 *  @brief  エンカウントデータの通常エンカウントポケモンデータブロックを取得
 *  @param now_time_data 現在の時間帯のエンカウントデータ
 *
 *  @return テーブルポインタ
 */
//----------------------------------------------------------------------------
const COMMON_POKE_DATA* PokeSet::GetEncountPokeBlockData( const COMMON_TIME_ZONE_DATA& now_time_data ) const
{
  // 通常エンカウントブロックを返す
  return now_time_data.normalMons[0];
}

//----------------------------------------------------------------------------
/**
 *  @brief  エンカウントデータの乱入エンカウントポケモンデータブロックを取得
 *  @param now_time_data 現在の時間帯のエンカウントデータ
 *  @param intruder_type 乱入種類
 *  @param block_index ブロック番号(通常乱入…0～6、雨・雪・砂嵐乱入…0～1)
 *
 *  @return テーブルポインタ
 */
//-----------------------------------------------------------------------------
const COMMON_POKE_DATA* PokeSet::GetEncountIntruderPokeBlockData( const COMMON_TIME_ZONE_DATA& now_time_data, IntruderType intruder_type, int block_index ) const
{
  switch( intruder_type)
  {
    case INTRUDER_TYPE_NORMAL:
      GFL_ASSERT_MSG( block_index < BLOCK_NUM_NORMAL_INTRUDER, "PokeSet::GetEncountIntruderPokeBlockData ブロック番号が不正\n");
      return now_time_data.normalIntruderMons[block_index];
    case INTRUDER_TYPE_RAIN:
      GFL_ASSERT_MSG( block_index < BLOCK_NUM_RAIN_INTRUDER, "PokeSet::GetEncountIntruderPokeBlockData ブロック番号が不正\n");
      return now_time_data.rainIntruderMons[block_index];
    case INTRUDER_TYPE_SNOW:
      GFL_ASSERT_MSG( block_index < BLOCK_NUM_SNOW_INTRUDER, "PokeSet::GetEncountIntruderPokeBlockData ブロック番号が不正\n");
      return now_time_data.snowIntruderMons[block_index];
    case INTRUDER_TYPE_SAND:
      GFL_ASSERT_MSG( block_index < BLOCK_NUM_SAND_INTRUDER, "PokeSet::GetEncountIntruderPokeBlockData ブロック番号が不正\n");
      return now_time_data.sandIntruderMons[block_index];
    default:
      GFL_ASSERT_MSG( 0, "PokeSet::GetEncountIntruderPokeBlockData 乱入種類の指定が不正\n");
      return NULL;
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief  エンカウントデータ抽選
 *
 * @param enc_poke_block  エンカウトデータ
 *
 * @return 抽選に成功した時true
 */
/* -------------------------------------------------------------------------*/
bool PokeSet::LotteryEncountPoke( const COMMON_POKE_DATA* enc_poke_block, int minLevel, int maxLevel)
{
  int ( PokeSet::*ENC_PROB_CALC_FUNC[ ENCPROB_CALCTYPE_MAX ] )() =
  {
    &PokeSet::LotNormalEncount,
  }; 

  int idx = -1;
  if( m_flags.spa_hagane_up ){  //ハガネタイプ優先エンカウント
    idx = LotFixTypeEncount( enc_poke_block, POKETYPE_HAGANE );
  }else if( m_flags.spa_denki_up ){ //電気タイプ優先エンカウント
    idx = LotFixTypeEncount( enc_poke_block, POKETYPE_DENKI );
  }
  if( idx < 0 ){  //タイプ優先エンカウントに失敗したら通常抽選
    idx = ( this->*ENC_PROB_CALC_FUNC[ m_prob_calctype ] )();
  }

#if PM_DEBUG
  // 抽選番号を指定番号に変更
  int debugIndex = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENCOUNT_LOTTERY_INDEX );
  if( debugIndex != 0 )
  {
    idx = debugIndex - 1;
  }
#endif // PM_DEBUG

  // 乱入ポケモンは抽選されたポケモンと同じ番号
  m_intruderIndex = idx;

  //エンカウントモンスターパラメータセット
  const COMMON_POKE_DATA *poke_data;
  poke_data = &enc_poke_block[idx];

  // @fix 抽選されたモンスターNoが不正ではないかチェック
  if( poke_data->monsNo == MONSNO_NULL)
  {
    GFL_ASSERT( 0);
    return false;
  }

  int max_level = 0;
  for( int i = 0; i< m_enc_num; ++i )
  {
    EncountPokeParam* epp = &m_pokeTbl[i];
    GetEncountPokeParam( epp, &max_level, poke_data, minLevel, maxLevel, m_flags, NULL);
    ++poke_data;
  }

  //低レベルエンカウント回避チェック
  if( CheckLowLevelEncountAvoid( max_level ) ) return false;

  return true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 乱入エンカウントデータ抽選
 *
 * @param p_out_epp エンカウントポケモンパラメータ出力先
 * @param enc_poke_block  エンカウトデータ
 * @param intruder_type 乱入種類
 * @param minLevel 最小レベル
 * @param maxLevel 最大レベル
 * @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
 *
 * @return 抽選に成功した時true
 */
/* -------------------------------------------------------------------------*/
bool PokeSet::LotteryEncountIntruderPoke( EncountPokeParam* p_out_epp, const COMMON_POKE_DATA* enc_poke_block, IntruderType intruder_type, int minLevel, int maxLevel, gfl2::math::SFMTRandom* pRandom)
{
  s32 idx = m_intruderIndex;
  if( intruder_type != INTRUDER_TYPE_NORMAL)
  {
    // 通常乱入以外は1匹分のデータしかないので0
    idx = 0;
  }

  //エンカウントモンスターパラメータセット
  const COMMON_POKE_DATA *poke_data;
  poke_data = &enc_poke_block[ idx ];
  
  // @fix GFNMCat[1268] しげみの洞窟：ヤングースが乱入ポケモンを呼び出そうとするとハング
  if( poke_data[0].monsNo == MONSNO_NULL )
  {
    return false;
  }
  
  int max_level = 0;
  GetEncountPokeParam( p_out_epp, &max_level, poke_data, minLevel, maxLevel, m_flagsIntruder, pRandom);

  return true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief  エンカウントポケモンデータ取得
 *
 * @param p_out_ep 格納先
 * @param p_max_level 格納したポケモンの最大レベル
 * @param p_poke_data 格納するエンカウントポケモンデータ
 * @param flags 特性、道具効果適用チェックフラグ
 * @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
 * @param minLevel 最小レベル
 * @param maxLevel 最大レベル
 */
/* -------------------------------------------------------------------------*/
void PokeSet::GetEncountPokeParam( EncountPokeParam* p_out_epp, int* p_max_level, const COMMON_POKE_DATA* p_poke_data, int minLevel, int maxLevel, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom)
{
  p_out_epp->level = PokeSet::LotEncountLevel( p_poke_data, minLevel, maxLevel, flags, pRandom );
  if( *p_max_level < p_out_epp->level ){
    *p_max_level = p_out_epp->level; //最大レベルを更新
  }
  p_out_epp->monsNo = p_poke_data->monsNo;

  pml::personal::LoadPersonalData( static_cast <MonsNo>(p_poke_data->monsNo), static_cast <FormNo>(0) );
  int form_max = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_max );
  
  switch( p_poke_data->monsNo )
  {
  // メテノ
  case MONSNO_NAGAREBOSI:
    {
      // エンカウントデータ設定の手間とFileMakerフォーマット拡張を回避するため特殊処理にした

      // @attention メテノはバトル中に殻なしフォルム[7~13]から殻ありフォルム[0~6]へフォルムチェンジする演出がある。
      //  殻ありフォルムはバトル後、殻なしフォルムへ変更される。バトル外は殻なしフォルムを表示しなければならない。
      //  ここで、「殻あり」にすることで、バトル中のフォルムチェンジ演出が行われなくなるが、
      //  これは「野生エンカウント時は中身がわからないようにしたい」（貫田さん談）とのことで仕様
      u32 form_max = FORMNO_NAGAREBOSI_MAX / 2;
      if( pRandom)
      {
        p_out_epp->form = pRandom->Next( form_max);
      }
      else
      {
        p_out_epp->form = System::GflUse::GetPublicRand( form_max);
      }
    }
    break;

  // その他のポケモン全て
  default:
    {
      if( p_poke_data->form == MONS_FORM_RND_CODE ){
        p_out_epp->form = pml::pokepara::INIT_SPEC_FORM_RANDOM; //フォルムをランダムで取る特殊コード
      }else if(p_poke_data->form == MONS_FORM_REGION_CODE ){
        p_out_epp->form = m_real_region_formno;
      }else if(p_poke_data->form>=form_max){
        GFL_ASSERT_MSG(0,"エンカウントデータのフォルム指定がパーソナルのフォルム最大数を越えています \n mons = %d form = %d > form_max = %d\n", p_poke_data->monsNo, p_poke_data->form, form_max); 
        p_out_epp->form = 0;
      }else{
        p_out_epp->form = p_poke_data->form;
      }
    }
    break;
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ビードロレベル補正
 *
 * @param level レベル
 * @param flags 特性、道具効果適用チェックフラグ
 * @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
 *
 * @return 補正後のレベル
 */
/* ------------------------------------------------------------------------- */
int PokeSet::CalcLevelFixBiidoro( int level, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom ) const
{
  static const u8 sc_ProbTable[] = { 40, 30, 20, 10 };

  u32 rand = GetRandom( 100, pRandom);

  u32 prob = 0;
  u32 count = GFL_NELEMS(sc_ProbTable);
  u32 point = 0;

  for (u32 i = 0; i < count; i++)
  {
    prob += sc_ProbTable[ i ];
    if (rand < prob)
    {
      point = i+1;
      break;
    }
  }

  int out_level = level;

  if (flags.kuroi_biidoro_f) 
  {
    out_level += point;
  }
  else if (flags.siroi_biidoro_f)
  {
    out_level -= point;
  }

  if (out_level < 1) { out_level = 1; }
  if (out_level > 100) { out_level = 100; }


  return out_level;
}

//--------------------------------------------------------------
/**
 * @brief 隠し特性込みのエンカウントレベル取得

 * @param enc_poke_block エンカウントポケモンデータブロック
 * @param minLevel 最小レベル
 * @param maxLevel 最大レベル
 * @param flags 特性、道具効果適用チェックフラグ
 * @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)

 * @return  レベル
 */
//--------------------------------------------------------------
int PokeSet::LotEncountLevel( const COMMON_POKE_DATA* enc_poke_block, int minLevel, int maxLevel, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom)
{
  int level = minLevel;
  level += GetRandom( (maxLevel - minLevel) + 1, pRandom );

  if( flags.spa_high_lv_hit )
  {
    // 高レベルエンカウント
    // nijiでは最大レベルにする
    level = maxLevel;
  }
  
  // ロトムパワー補正
  level = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->CalcLevel(level, maxLevel);

  // ビードロ補正
  level = CalcLevelFixBiidoro(level, flags, pRandom);

  return level;
}

//--------------------------------------------------------------
/**
 * @brief 低レベルエンカウント回避チェック
 *
 * @param   max_level 同時にエンカウントするポケモンで一番高いレベル
 * @retval  true  回避する
 * @retval  false 回避しない
 */
//--------------------------------------------------------------
bool PokeSet::CheckLowLevelEncountAvoid( int max_level )
{
  if( m_flags.enc_force_f ){
    return false; //強制エンカウントなので回避しない
  }
  //特性によるレベル差戦闘回避
  IWASAWA_PRINT("LowLvEncAvoid spa = %d, m_lv=%d, spray_lv = %d\n", m_flags.spa_low_lv_rm, max_level, m_topPokeInfo.spray_lv);
  SAITA_PRINT("LowLvEncAvoid spa = %d, m_lv=%d, spray_lv = %d\n", m_flags.spa_low_lv_rm, max_level, m_topPokeInfo.spray_lv);
  if( m_flags.spa_low_lv_rm && ( m_topPokeInfo.spray_lv >= (max_level+5)) ){
    return true;
  }
  //スプレーチェック
  if( m_flags.fishing_f ) return false; //釣りではスプレーチェックしない(BW継承)

  if( m_flags.spray_f && (max_level < m_topPokeInfo.spray_lv)){
    return true;
  }

  // ロトムパワー補正
  if (GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->IsEncountAvoid())
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  特性による特定タイプの優先エンカウント
 *
 *  @retval 0から 優先エンカウントするテーブルIndex
 *  @retval -1  優先エンカウントする対象が見つからなかった
 */
//-----------------------------------------------------------------------------
int PokeSet::LotFixTypeEncount( const COMMON_POKE_DATA* enc_poke_block, int type )
{
  int num = 0;
  u8 idx_tbl[MONS_NUM_MAX];

  for(int i = 0;i < m_tbl_num; ++i){
    for(int j = 0;j < m_element_num; ++j ){
      int idx = i*m_element_num+j;
      int type1,type2;

      //@fix GFNMCat[1268] しげみの洞窟：ヤングースが乱入ポケモンを呼び出そうとするとハング
      if( enc_poke_block[idx].monsNo == MONSNO_NULL )
      {
        continue;
      }

      if( enc_poke_block[idx].form == MONS_FORM_RND_CODE ){
        //フォルムNoがランダムのものは特性の効果対象外(bw継承仕様)
        continue;
      }
      pml::personal::LoadPersonalData( static_cast < MonsNo >(enc_poke_block[idx].monsNo), static_cast <FormNo>(enc_poke_block[idx].form) );
      type1 = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type1 );
      type2 = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type2 );
      if( type1==type || type2==type ){
        idx_tbl[num++] = i;
      }
    }
  }
  if( num == 0 ) return -1; //エラー値
  return idx_tbl[System::GflUse::GetPublicRand( num )];
}

//--------------------------------------------------------------
/**
 * @brief テーブル内番号の抽選共通処理
 * @param 確率テーブルポインタ
 * @param 確率テーブル長
 * @return 抽選されたテーブル内番号
 */
//--------------------------------------------------------------
int PokeSet::LotRandomEncountCommon( const u8* tbl, u32 tbl_len )
{
  u32 rnd = System::GflUse::GetPublicRand( 100 ); 
  s8 index = this->LotteryCommon( rnd, tbl, tbl_len);
  if( index == -1)
  {
    GFL_ASSERT(0); //テーブル指定が間違っていなければ、ここに抜けてくることはないはず
    index = 0;
  }
  return index;
}

//--------------------------------------------------------------
/**
 * @brief 抽選共通処理
 * @param rnd 乱数
 * @param tbl 確率が格納された配列
 * @param tbl_len tblサイズ
 */
//--------------------------------------------------------------
s8 PokeSet::LotteryCommon( u32 rnd, const u8* tbl, u32 tbl_len )
{
  u32 key = 0;
  for( u32 i = 0; i < static_cast<u32>(tbl_len); i++ ){
    key += tbl[i];
    if( rnd < key ){
      return i;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
/**
 *  @brief  通常エンカウントブロック内Index抽選
 *
 *  @retval 0～ エンカウントデータブロック内Index
 */
//-----------------------------------------------------------------------------
int PokeSet::LotNormalEncount()
{
  const COMMON_TIME_ZONE_DATA& now_time_enc_poke_block = GetNowTimeZoneEncountData( m_encount_data);
  return LotRandomEncountCommon( now_time_enc_poke_block.prob, sizeof(now_time_enc_poke_block.prob) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  パーソナルから持たせるアイテムを抽選
 *  @param monsno モンスターNo
 *  @param form フォルムNo
 *  @param flags 特性、道具効果適用チェックフラグ
 *  @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
 */
//-----------------------------------------------------------------------------
u16 PokeSet::ItemGetFromPersonal( u32 monsno, u8 form, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom ) const
{
  u16 slot[pml::personal::ITEM_SLOT_NUM] = {0};
 
  for(int i = 0;i < pml::personal::ITEM_SLOT_NUM; ++i){
    slot[i] = pml::personal::GetPersonalParam( static_cast<pml::personal::ParamID>(pml::personal::PARAM_ID_item1+i) );
  }
  //2スロットが同じアイテムなら必ず持たせる
  if( slot[0] == slot[1] ){
    return slot[0];
  }
  {
    u8 rnd = GetRandom( 100, pRandom); 
    u8 tmp = 0;
    const u8* range = DATA_ItemRangeTable[ flags.spa_item_rate_up ];
    for( int i = 0;i < pml::personal::ITEM_SLOT_NUM; ++i ){
      tmp += range[i];
      if(rnd < tmp ) return slot[i];
    }
  }
  return ITEM_DUMMY_DATA; //無し
}

//----------------------------------------------------------------------------
/**
 *  @brief ポケモン初期化パラメーターを初期化
 *  @param p_out_spec 初期化するポケモン初期化パラメーター
 *  @param enc_poke_param 元になるエンカウントポケモンパラメーター
 *  @param flags 特性、道具効果適用チェックフラグ
 */
//-----------------------------------------------------------------------------
void PokeSet::InitializePokeParamInitialSpec( pml::pokepara::InitialSpec* p_out_spec, const EncountPokeParam* enc_poke_param, const EffectApplyCheckFlag& flags) const
{
  p_out_spec->monsno = static_cast<MonsNo>(enc_poke_param->monsNo);
  p_out_spec->level = enc_poke_param->level;
  p_out_spec->formno = static_cast<FormNo>(enc_poke_param->form);
  p_out_spec->id = m_my_id;
  
  //パーソナルロード
  pml::personal::LoadPersonalData( p_out_spec->monsno, p_out_spec->formno );

  if(enc_poke_param->sex == SEX_REQ_MALE){
    p_out_spec->sex = pml::SEX_MALE;
  }else if(enc_poke_param->sex == SEX_REQ_FEMALE){
    p_out_spec->sex = pml::SEX_FEMALE;
  }else{
    // @fix GFNMCat[1205] 特性「メロメロ」のフィールド効果が反映されていない。
    // 正しくは、シンボルエンカウントの性別指定がされていないときにメロメロが反映されていない
    // 全てのエンカウントが共通で通る関数に移動
    // 種族としての性別制限はPokemonParam生成時にチェックするので、ここでは気にしない
    if( flags.spa_sex_fix ){
      if( m_topPokeInfo.mons_sex == pml::SEX_MALE ){
        p_out_spec->sex = pml::SEX_FEMALE;
      }else if(m_topPokeInfo.mons_sex == pml::SEX_FEMALE){
        p_out_spec->sex = pml::SEX_MALE;
      }
    }
  }

  if(enc_poke_param->rare == RARE_REQ_MUST ){
    p_out_spec->rare_rnd = pml::pokepara::INIT_SPEC_RARE_TRUE;
  }else if(enc_poke_param->rare == RARE_REQ_NOT ){
    p_out_spec->rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE;
  }else{
    // @fix レア抽選回数の計算を共通化
    p_out_spec->rare_try_count = this->CalcRareTryCount( 1, flags); //レアになるまで乱数を引き直す回数（標準は1回）
  }

  // @debug 必ずレアカラーにする
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENCOUNT_POKE_COLOR_RARE ) == 1 )
  {
    p_out_spec->rare_rnd = pml::pokepara::INIT_SPEC_RARE_TRUE;
  }
#endif

    
  //個体値V抽選数
  if (flags.legend_f)
  {
    p_out_spec->talentVNum = TALENT_V_NUM_LEGEND;  //伝説系は3V 
  }else{
    p_out_spec->talentVNum = enc_poke_param->talentVNum;
    for( int i = 0;i < pml::pokepara::POWER_NUM;i++ ){
       p_out_spec->talentPower[i] = enc_poke_param->talent[i];
    }
  }

  switch(enc_poke_param->spabi){
  case SPABI_REQ_01:
  case SPABI_REQ_02:
  case SPABI_REQ_03:
    p_out_spec->tokusei_index = enc_poke_param->spabi-1;
  default:
    break;
  }
  if( enc_poke_param->seikaku > 0 && enc_poke_param->seikaku < pml::pokepara::SEIKAKU_NUM + 1 ){
    p_out_spec->seikaku = enc_poke_param->seikaku - 1;
  }else if( flags.spa_chr_fix ){
    p_out_spec->seikaku = m_topPokeInfo.mons_chr;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief ポケモン初期化パラメーターを初期化
 *  @param p_out_spec 初期化するポケモン初期化パラメーター
 *  @param enc_poke_param 元になるエンカウントポケモンパラメーター
 *  @param flags 特性、道具効果適用チェックフラグ
 *  @param rare_try_count レアになるまで乱数を引き直す回数
 */
//-----------------------------------------------------------------------------
void PokeSet::InitializePokeParamInitialSpecIntruder( pml::pokepara::InitialSpec* p_out_spec, const EncountPokeParam* enc_poke_param, const EffectApplyCheckFlag& flags, u8 rare_try_count) const
{
  this->InitializePokeParamInitialSpec( p_out_spec, enc_poke_param, flags);

  // 色違い抽選回数の設定
  // @fix 特性、どうぐ(nijiではひかるおまもり)の効果適用をしていなかった
  p_out_spec->rare_try_count = this->CalcRareTryCount( rare_try_count, flags);
}

//----------------------------------------------------------------------------
/**
 *  @brief ポケモンパラメーターを生成
 *  @param p_out_param 生成したポケモンパラメーターの格納先
 *  @param spec ポケモン初期化パラメーター
 *  @param enc_poke_param エンカウントポケモンパラメーター
 *  @param flags 特性、道具効果フラグ
 *  @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
 */
//-----------------------------------------------------------------------------
void PokeSet::InitializePokeParam( pml::pokepara::PokemonParam* p_out_param, const pml::pokepara::InitialSpec& spec, const EncountPokeParam* enc_poke_param, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom) const
{
  //所持アイテム抽選
  // @fix ITEM_DATA_MAXにはアイテムとして有効な値なので<=とする
  if( enc_poke_param->item <= ITEM_DATA_MAX){
    if( enc_poke_param->item == ITEM_REQ_DEFAULT ){  //パーソナル依存
      p_out_param->SetItem( ItemGetFromPersonal( enc_poke_param->monsNo, enc_poke_param->form, flags, pRandom) );
    }else{
      p_out_param->SetItem( enc_poke_param->item ); //指定アイテムセット
    }
  }

  //努力値指定反映
  for( int i = 0;i < pml::pokepara::POWER_NUM;i++ ){
    p_out_param->ChangeEffortPower(static_cast<pml::pokepara::PowerID>(i),enc_poke_param->effort[i]);
  }
  //技反映
  if( enc_poke_param->waza[0] != 0 ){
    for( int i = 0; i<4; i++){
      p_out_param->SetWaza( i, static_cast<WazaNo>(enc_poke_param->waza[i]));
      p_out_param->SetTamagoWazaNo( i, static_cast<WazaNo>(enc_poke_param->waza[i]) ); //NMCat[719] タマゴ技スロットへのセットも行う
    }
    p_out_param->CloseUpWazaPos(); //技前詰め
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  PokemonParam設定
 *
 *  @param  p_heap    ヒープ
 *  @param  data      ポケモンデータ
 *  @param  p_party   ポケparty
 */
//-----------------------------------------------------------------------------
void PokeSet::AddPoke( gfl2::heap::HeapBase* p_heap, const EncountPokeParam* param, pml::PokeParty* p_party ) const
{
  pml::pokepara::InitialSpec spec;
  InitializePokeParamInitialSpec( &spec, param, m_flags);

  pml::pokepara::PokemonParam* pp;
  pp = GFL_NEW( p_heap ) pml::pokepara::PokemonParam( p_heap, spec );
  InitializePokeParam( pp, spec, param, m_flags, NULL);

  p_party->AddMember( *pp );
  GFL_DELETE pp;
}

//----------------------------------------------------------------------------
/**
 *  @brief  設定
 *
 *  @param  p_heap    ヒープ
 *  @param  data      ポケモンデータ
 *  @param  p_party   ポケparty
 */
//-----------------------------------------------------------------------------
void PokeSet::SetPowerUp( const EncountPokeParam* param, DefaultPowerUpDesc* p_powerup ) const
{
  switch(param->aura_mode){
  case WILDENC_AULA_MODE_ATK1:
  case WILDENC_AULA_MODE_ATK2:
  case WILDENC_AULA_MODE_ATK3:
    p_powerup->rankUp_Attack = (param->aura_mode-WILDENC_AULA_MODE_ATK1)+1;
    break;
  case WILDENC_AULA_MODE_DEF1:
  case WILDENC_AULA_MODE_DEF2:
  case WILDENC_AULA_MODE_DEF3:
    p_powerup->rankUp_Defense = (param->aura_mode-WILDENC_AULA_MODE_DEF1)+1;
    break;
  case WILDENC_AULA_MODE_SPATK1:
  case WILDENC_AULA_MODE_SPATK2:
  case WILDENC_AULA_MODE_SPATK3:
    p_powerup->rankUp_SpAttack = (param->aura_mode-WILDENC_AULA_MODE_SPATK1)+1;
    break;

  case WILDENC_AULA_MODE_SPDEF1:
  case WILDENC_AULA_MODE_SPDEF2:
  case WILDENC_AULA_MODE_SPDEF3:
    p_powerup->rankUp_SpDefense = (param->aura_mode-WILDENC_AULA_MODE_SPDEF1)+1;
    break;
  case WILDENC_AULA_MODE_AGI1:
  case WILDENC_AULA_MODE_AGI2:
  case WILDENC_AULA_MODE_AGI3:
    p_powerup->rankUp_Agility = (param->aura_mode-WILDENC_AULA_MODE_AGI1)+1;
    break;
  case WILDENC_AULA_MODE_ALL1:
  case WILDENC_AULA_MODE_ALL2:
  case WILDENC_AULA_MODE_ALL3:
    {
      u8 plus = (param->aura_mode-WILDENC_AULA_MODE_ALL1)+1;
      p_powerup->rankUp_Attack = plus;
      p_powerup->rankUp_Defense = plus;
      p_powerup->rankUp_SpAttack = plus;
      p_powerup->rankUp_SpDefense = plus;
      p_powerup->rankUp_Agility = plus;
    }
    break;
  default:
    return;
  }
  switch( param->encount_mode )
  {
  case WILDENC_MODE_BEAST:
    p_powerup->reason = DEFAULT_POWERUP_REASON_ULTRA_BEAST;
    break;
  case WILDENC_MODE_NUSI:
  default:
    p_powerup->reason = DEFAULT_POWERUP_REASON_NUSI;
    break;
  }
  p_powerup->aura_color.Set( param->aura_color_r*1.0f, param->aura_color_g*1.0f,param->aura_color_b*1.0f,param->aura_color_a*1.0f);
}

/* ------------------------------------------------------------------------- */
/**
 * @brief バトルモードをセット
 *
 * @param mode WildEncMode
 */
/* ------------------------------------------------------------------------- */
void PokeSet::SetBattleMode( WildEncMode mode )
{
  m_btl_mode = mode;

  switch (mode)
  {
  case WILDENC_MODE_LEGEND:
  case WILDENC_MODE_LEGEND_IMMORTAL:
  case WILDENC_MODE_LEGEND_NOLOSE:
    m_flags.legend_f = true;
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 現在の時間帯のエンカウントデータを取得
 * @param エンカウントデータ
 *
 * @return 現在の時間帯のエンカウントデータ
 */
/* ------------------------------------------------------------------------- */
const COMMON_TIME_ZONE_DATA& PokeSet::GetNowTimeZoneEncountData( const ENCOUNT_DATA& data ) const
{
  // @fix GFNMCat[4305] どの時間帯で抽選するかは、EvTime(イベント中は停止)を使用する
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  GameSys::TimeZone::Zone timeZone = EvTime::GetAlolaTimeZone( pGameData );
  switch( timeZone)
  {
  case GameSys::TimeZone::MORNING:
  case GameSys::TimeZone::NOON:
  case GameSys::TimeZone::EVENING:
    return data.day;
  }
  return data.night;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 乱入ポケモンのブロック番号を抽選
 * @param pOutFixType ブロック番号抽選後の乱入種類
 * @param pRandom 乱数管理インスタンス
 * @type type 乱入種類
 * @return 抽選されたブロック番号
 */
/* ------------------------------------------------------------------------- */
s8 PokeSet::LotteryIntruderPokeBlock( IntruderType* pOutFixType, gfl2::math::SFMTRandom* pRandom, IntruderType type)
{
  s8 index = -1;

  const u8 probNormalTable[ BLOCK_NUM_NORMAL_INTRUDER ] = { 1, 1, 1, 10, 10, 10, 67 };
  const u8 probWeatherTable[ BLOCK_NUM_RAIN_INTRUDER ] = { 1, 10 };

  *pOutFixType = type;

  u32 rand = pRandom->Next( 100 ); 

  switch( type)
  {
  case INTRUDER_TYPE_NORMAL:
    {
      index = this->LotteryCommon( rand, probNormalTable, BLOCK_NUM_NORMAL_INTRUDER);
    }
    break;

  case INTRUDER_TYPE_RAIN:
  case INTRUDER_TYPE_SNOW:
  case INTRUDER_TYPE_SAND:
    {
      index = this->LotteryCommon( rand, probWeatherTable, BLOCK_NUM_RAIN_INTRUDER);

      // 天候優先で抽選漏れしたら通常から抽選
      if( index == -1)
      {
        rand = pRandom->Next( 100 ); 
        index = this->LotteryCommon( rand, probNormalTable, BLOCK_NUM_NORMAL_INTRUDER);
        *pOutFixType = INTRUDER_TYPE_NORMAL;
      }
    }
    break;

  default:
    GFL_ASSERT_MSG( 0, "PokeSet::LotteryIntruderPokeBlock 乱入種類が不正です\n");
    break;
  }

  if( index == -1)
  {
    GFL_ASSERT( 0);
    index = 0;
  }

  return index;
}

//-------------------------------------------------------------------------
/**
 * @brief 特性、道具効果チェック用データセット
 * @param pInfo セットするインスタンスのポインタ
 * @param pPokeParam 情報を取得するポケモンパラメーター
 */
//-------------------------------------------------------------------------
void PokeSet::SetEffectApplyCheckInfo( EffectApplyCheckInfo* pInfo, const pml::pokepara::PokemonParam& pPokeParam)
{
  pInfo->mons_no   = pPokeParam.GetMonsNo();
  pInfo->mons_item = pPokeParam.GetItem();
  pInfo->mons_sex  = pPokeParam.GetSex();
  pInfo->mons_spa  = pPokeParam.GetTokuseiNo();
  pInfo->mons_chr  = pPokeParam.GetSeikaku();
}

//----------------------------------------------------------------------------
/**
 *  @brief  特性による特定タイプの優先エンカウント (乱入用)
 *  @param pOutEncPokeParam 抽選したポケモンデータ格納用
 *  @param pNowTimeEncPokeBlock 現在の時間帯のブロック配列
 *  @param type ポケモンタイプ(はがね、でんきとか)
 *  @param pRandom 乱数インスタンス
 *  @retval true 抽選した
 *  @retval false 抽選していない
 */
//-----------------------------------------------------------------------------
bool PokeSet::LotFixTypeEncountIntruder( EncountPokeParam* pOutEncPokeParam, const COMMON_TIME_ZONE_DATA& nowTimeEncPokeBlock, int type, gfl2::math::SFMTRandom* pRandom )
{
  s32 num = 0;
  s32 idx_tbl[ BLOCK_NUM_NORMAL_INTRUDER ] = {};

  // ブロック内番号は通常抽選で決定した番号を使う

  // タイプが一致するポケモンが存在するブロック番号をリストアップ
  for( u32 i = 0; i < BLOCK_NUM_NORMAL_INTRUDER; ++i)
  {
    const COMMON_POKE_DATA* pEncPokeBlock = nowTimeEncPokeBlock.normalIntruderMons[ i ]; 

    // @fix GFNMCat[1268] しげみの洞窟：ヤングースが乱入ポケモンを呼び出そうとするとハング
    if( pEncPokeBlock[ m_intruderIndex ].monsNo == MONSNO_NULL )
    {
      continue;
    }

    if( pEncPokeBlock[ m_intruderIndex ].form == MONS_FORM_RND_CODE )
    {
      //フォルムNoがランダムのものは特性の効果対象外(bw継承仕様)
      continue;
    }

    pml::personal::LoadPersonalData( static_cast < MonsNo >(pEncPokeBlock[ m_intruderIndex ].monsNo), static_cast <FormNo>(pEncPokeBlock[ m_intruderIndex ].form) );
    s32 type1 = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type1 );
    s32 type2 = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type2 );

    if( type1 == type || type2 == type )
    {
      idx_tbl[ num ] = i;
      num++;
    }
  }

  // 指定タイプはいなかった
  if( num == 0)
  {
    return false;
  }

  // 使用するブロックの決定
  u32 rand = pRandom->Next( num );
  s32 pokeBlockIndexFix = idx_tbl[ rand ];
  const COMMON_POKE_DATA* pEncPokeBlockFix = nowTimeEncPokeBlock.normalIntruderMons[ pokeBlockIndexFix ]; 

  // ポケモン情報取得
  s32 max_level = 0;
  GetEncountPokeParam( pOutEncPokeParam, &max_level, &pEncPokeBlockFix[ m_intruderIndex ], nowTimeEncPokeBlock.minLevel, nowTimeEncPokeBlock.maxLevel, m_flagsIntruder, pRandom);

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  特性による特定タイプの優先エンカウント (乱入用、乱入種類指定)
 *  @param pOutEncPokeParam 抽選したポケモンデータ格納用
 *  @param pNowTimeEncPokeBlock 現在の時間帯のブロック配列
 *  @param type ポケモンタイプ(はがね、でんきとか)
 *  @param pRandom 乱数インスタンス
 *  @retval true 抽選した
 *  @retval false 抽選していない
 */
//-----------------------------------------------------------------------------
bool PokeSet::LotFixTypeEncountIntruderEx( EncountPokeParam* pOutEncPokeParam, const COMMON_TIME_ZONE_DATA& nowTimeEncPokeBlock, int type, gfl2::math::SFMTRandom* pRandom, IntruderType intruder_type )
{
  s32 num = 0;
  s32 idx_tbl[ BLOCK_NUM_RAIN_INTRUDER ] = {};

  // ブロック内番号は通常抽選で決定した番号を使う

  // タイプが一致するポケモンが存在するブロック番号をリストアップ
  for( u32 i = 0; i < BLOCK_NUM_RAIN_INTRUDER; ++i)
  {
    const COMMON_POKE_DATA* pEncPokeBlock; 
    switch( intruder_type)
    {
    case INTRUDER_TYPE_RAIN:
      pEncPokeBlock = nowTimeEncPokeBlock.rainIntruderMons[ i ]; 
      break;
    case INTRUDER_TYPE_SNOW:
      pEncPokeBlock = nowTimeEncPokeBlock.snowIntruderMons[ i ]; 
      break;
    case INTRUDER_TYPE_SAND:
      pEncPokeBlock = nowTimeEncPokeBlock.sandIntruderMons[ i ]; 
      break;
    default:
      GFL_ASSERT_MSG( 0, "PokeSet::LotFixTypeEncountIntruderEx : 乱入種類が不正\n");
      pEncPokeBlock = NULL;
      return false;
      //break;
    }

    // @fix GFNMCat[1268] しげみの洞窟：ヤングースが乱入ポケモンを呼び出そうとするとハング
    if( pEncPokeBlock[ 0 ].monsNo == MONSNO_NULL )
    {
      continue;
    }

    if( pEncPokeBlock[ 0 ].form == MONS_FORM_RND_CODE )
    {
      //フォルムNoがランダムのものは特性の効果対象外(bw継承仕様)
      continue;
    }

    pml::personal::LoadPersonalData( static_cast < MonsNo >(pEncPokeBlock[ 0 ].monsNo), static_cast <FormNo>(pEncPokeBlock[ 0 ].form) );
    s32 type1 = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type1 );
    s32 type2 = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type2 );

    if( type1 == type || type2 == type )
    {
      idx_tbl[ num ] = i;
      num++;
    }
  }

  // 指定タイプがいなかったら通常乱入を見る
  if( num == 0)
  {
    return LotFixTypeEncountIntruder( pOutEncPokeParam, nowTimeEncPokeBlock, type, pRandom);
  }

  // 使用するブロックの決定
  u32 rand = pRandom->Next( num );
  s32 pokeBlockIndexFix = idx_tbl[ rand ];
  const COMMON_POKE_DATA* pEncPokeBlockFix = nowTimeEncPokeBlock.normalIntruderMons[ pokeBlockIndexFix ]; 

  // ポケモン情報取得
  s32 max_level = 0;
  GetEncountPokeParam( pOutEncPokeParam, &max_level, &pEncPokeBlockFix[ m_intruderIndex ], nowTimeEncPokeBlock.minLevel, nowTimeEncPokeBlock.maxLevel, m_flagsIntruder, pRandom);

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief 乱数取得
 *  @param max 最大値
 *  @param pRandom 乱数インスタンス
 *  @return 乱数 (maxは含まれない, 0～max-1が返る)
 *  @note pRandomがNULLならGetPublicRandを使用する
 */
//-----------------------------------------------------------------------------
u32 PokeSet::GetRandom( u32 max, gfl2::math::SFMTRandom* pRandom) const
{
  u32 rnd = 0;
  if( pRandom == NULL)
  {
    rnd = System::GflUse::GetPublicRand( max); 
  }
  else
  {
    rnd = pRandom->Next( max); 
  }
  return rnd;
}

//----------------------------------------------------------------------------
/**
 *  @brief ライドごとに調整したエンカウント率を返す
 */
//-----------------------------------------------------------------------------
u32 PokeSet::GetRideAdjustEncountProb( const u32 in_prob, const u32 max_prob, MoveModel::FieldMoveModelPlayer * cp_player ) const
{
  u32 prob_rate = 100;

#if PM_DEBUG
  if( Field::Debug::DebugTypes::s_isEncountDebugEnable)
  {
    prob_rate = Field::Debug::DebugTypes::s_encountRideAdjustRate;
    return in_prob * prob_rate / max_prob;
  }
#endif // PM_DEBUG

  // ライドごとにエンカウント率を調整
  // prob_rate％分減らす
  RIDE_POKEMON_ID id = EventPokemonRideTool::GetPokemonRideOnID( cp_player);
  switch( id)
  {
  // ケンタロス
  case RIDE_KENTAROSU:
    prob_rate = 50;
    break;

  // サメハダー
  case RIDE_SAMEHADA:
    prob_rate = 15;
    break;

  // ムーランド
  case RIDE_MUURANDO:
    prob_rate = 55;
    break;

  // ラプラス
  case RIDE_LAPLACE:
    prob_rate = 20;
    break;

  // カイリキー
  case RIDE_KAIRIKY:
    prob_rate = 100;
    break;

  // バンバドロ
  case RIDE_ROBA2:
    prob_rate = 65;
    break;
  }

  return in_prob * prob_rate / max_prob;
}

//----------------------------------------------------------------------------
/**
 *  @brief レア抽選回数計算
 *  @param src 元の抽選回数
 *  @param flags 特性、道具効果適用チェックフラグ
 *  @return srcに特性、どうぐの効果を適用したレア抽選回数
 *  @fix 特性、どうぐの効果適用をしたレア抽選回数の計算処理を共通化
 */
//-----------------------------------------------------------------------------
u8 PokeSet::CalcRareTryCount( u8 src, const EffectApplyCheckFlag& flags) const
{
  u8 hikaruomamori_count = RARE_TRY_COUNT_HIKARUOMAMORI * flags.rare_up_f;
  u8 ret = src + hikaruomamori_count;
  return ret;
}

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )
