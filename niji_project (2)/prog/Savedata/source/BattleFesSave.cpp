//===================================================================
/**
* @file    BattleFesSave.cpp
* @brief   バトルフェスサークルのデータ管理
* @author  munakata_kai
* @date    2016/11/15
*/
//===================================================================
#include <Savedata/include/BattleFesSave.h>
#include <util/include/gfl2_std_string.h>
#include <System/include/PokemonVersion.h>
#include "Savedata/include/MiscSave.h"
// msgId
#include  <niji_conv_header/message/msg_jf_phrase.h>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#include "System/include/HeapDefine.h"
// フェスファン名用
#include <arc_index/message.gaix>
#include <arc_index/battle_tree_trainer.gaix>

// 定型文用msgId
#include  <niji_conv_header/message/msg_jf_phrase.h>

#include <Print/include/PrintSystem.h>

#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "System/include/DressUp.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesTool.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/BattleFes/BattleFesDebug.h"
#endif

GFL_NAMESPACE_BEGIN(Savedata)

//--------------------------------------------------------------------------------------------
/**
* @brief    コンストラクタ
*/
//--------------------------------------------------------------------------------------------
BattleFesSave::BattleFesSave()
{
  STATIC_ASSERT(sizeof(BattleFesData) == 924);
  Reset();
}


//--------------------------------------------------------------------------------------------
/**
* @brief    デストラクタ
*/
//--------------------------------------------------------------------------------------------
BattleFesSave::~BattleFesSave()
{
  Reset();
}


//--------------------------------------------------------------------------------------------
/**
* @brief   全データをリセットする
*/
//--------------------------------------------------------------------------------------------
void BattleFesSave::Reset()
{
  gfl2::std::MemClear(&m_battleFesData, sizeof(BattleFesData));
}

//------------------------------------------------------------------------------------------
/**
* @brief    読み込んだデータをセットする関数
*
* @param    pData    先頭のポインタ
*/
//------------------------------------------------------------------------------------------
// @override
void BattleFesSave::SetData(void* pData)
{
  gfl2::std::MemCopy(pData, reinterpret_cast<void*>(&m_battleFesData), sizeof(BattleFesData));
}


//------------------------------------------------------------------------------------------
/**
* @brief    データの先頭ポインタを得る関数
*
* @return  先頭のポインタ
*/
//------------------------------------------------------------------------------------------
// @override
void* BattleFesSave::GetData()
{
  return reinterpret_cast<void*>(&m_battleFesData);
}


//------------------------------------------------------------------------------------------
/**
* @brief    データサイズ
*
* @return  バイト
*/
//------------------------------------------------------------------------------------------
// @override
size_t BattleFesSave::GetDataSize()
{
  return sizeof(BattleFesData);
}

//--------------------------------------------------------------------------
/**
* @brief セーブデータのクリア
* @param heap　クリアに使用するテンポラリヒープ
*/
//--------------------------------------------------------------------------
void BattleFesSave::Clear(gfl2::heap::HeapBase * heap)
{
  Reset();
  InitRentalPokemonData(heap);
  ClearBattleTreeData();
  for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    ClearPartnerData(index, heap);
  }
  ClearTrainerData();
}

//--------------------------------------------------------------------------
/**
* @brief バトルセーブデータのクリア
* @param heap　クリアに使用するテンポラリヒープ
*/
//--------------------------------------------------------------------------
void BattleFesSave::ClearBattleData(gfl2::heap::HeapBase * heap)
{
  InitRentalPokemonData(heap);
  ClearBattleTreeData();
  for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    ClearPartnerData(index, heap);
  }
  ClearTrainerData();
}

//-----------------------------------------------------------------------------
/**
* @brief   初期化
*
* @param   heap  ヒープ
*/
//-----------------------------------------------------------------------------
void BattleFesSave::InitRentalPokemonData(gfl2::heap::HeapBase* heap)
{
  pml::pokepara::CoreParam* cp = GFL_NEW_LOW(heap->GetLowerHandle()) pml::pokepara::CoreParam(heap->GetLowerHandle(), MONSNO_NULL, 0, 0);
  // 自分のポケモンデータを初期化
  cp->Serialize_Core(m_battleFesData.rentalPokemon.pokemonCoreParam);
  m_battleFesData.isRegistPoke = false;
  GFL_DELETE cp;
}

//-----------------------------------------------------------------------------
/**
* @brief   CoreParamをセット
*
* @param   pp  セットするCoreParam
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetRentalPokemonData(const pml::pokepara::CoreParam* cp)
{
  GFL_ASSERT_STOP(cp->IsNull() == false);
  cp->Serialize_Core(&m_battleFesData.rentalPokemon);
  m_battleFesData.isRegistPoke = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   自分のレンタルポケモンデータの取得 CoreParamを取得
* @param[in] pml::pokepara::CoreParam* cp 取得場所
* @param[out] bool 取得成功/取得失敗
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::GetRentalPokemonData(pml::pokepara::CoreParam* cp) const
{
  if(m_battleFesData.isRegistPoke)
  {
    cp->Deserialize_Core(&m_battleFesData.rentalPokemon);
  }
  return (m_battleFesData.isRegistPoke != 0);  ///< u8 -> bool WarnningC4800解決.
}

//-----------------------------------------------------------------------------
/**
* @brief 自分のレンタルポケモンデータの有無
* @param[out] bool true : データ有り false : データ無し
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsExistRentalPokemonData() const
{
  return (m_battleFesData.isRegistPoke != 0);  ///< u8 -> bool WarnningC4800解決.
}

//-----------------------------------------------------------------------------
/**
* @brief   バトルツリーデータの登録
* @param[in]
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetBattleTreeData(const BattleFes::BATTLE_TREE_DATA battle_tree_data)
{
  SetBattleTreeID(battle_tree_data.pokeID);
  SetBattleTreeSex(battle_tree_data.Sex);
  SetBattleTreeTokusei(battle_tree_data.Tokusei);
}

//-----------------------------------------------------------------------------
/**
* @brief   バトルツリーＩＤの登録
* @param[in] u16 battle_tree_id
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetBattleTreeID(const u16 battle_tree_id)
{
  GFL_ASSERT_STOP(battle_tree_id < BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX);
  m_battleFesData.battle_tree_id = battle_tree_id;
}

//-----------------------------------------------------------------------------
/**
* @brief   バトルツリーＩＤの取得
* @param[in] index
* @param[out] u16 battle_tree_id　0は異常
*/
//-----------------------------------------------------------------------------
u16 BattleFesSave::GetBattleTreeID() const
{
  return m_battleFesData.battle_tree_id;
}

//-----------------------------------------------------------------------------
/**
* @brief   バトルツリー性別の登録
* @param[in] u8 sex
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetBattleTreeSex(const u8 battle_tree_sex)
{
  GFL_ASSERT_STOP(battle_tree_sex < BattleFes::BattleFesDefine::POKEMON_SEX_END);
  m_battleFesData.battle_tree_sex = battle_tree_sex;
}

//-----------------------------------------------------------------------------
/**
* @brief   バトルツリー性別の取得
* @param[out] u8 sex
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetBattleTreeSex() const
{
  return m_battleFesData.battle_tree_sex;
}

//-----------------------------------------------------------------------------
/**
* @brief   バトルツリー特性の登録
* @param[in] u8
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetBattleTreeTokusei(const u8 battle_tree_tokusei)
{
  GFL_ASSERT_STOP(battle_tree_tokusei < BattleFes::BattleFesDefine::POKEMON_TOKUSEI_END);
  m_battleFesData.battle_tree_tokusei = battle_tree_tokusei;
}

//-----------------------------------------------------------------------------
/**
* @brief   バトルツリー特性の取得
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetBattleTreeTokusei() const
{
  return m_battleFesData.battle_tree_tokusei;
}

//-----------------------------------------------------------------------------
/**
* @brief   バトルツリー関連データ削除
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearBattleTreeData()
{
  m_battleFesData.battle_tree_id = BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX;
  m_battleFesData.battle_tree_sex = 0;
  m_battleFesData.battle_tree_tokusei = 0;
}

//-----------------------------------------------------------------------------
/**
* @brief   仲間のレンタルポケモンデータの初期化
* @param[in] index 0~1
* @param[in] heap  ヒープ
*/
//-----------------------------------------------------------------------------
void BattleFesSave::InitPartnerPokemonData(const u32 index, gfl2::heap::HeapBase* heap)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  pml::pokepara::CoreParam* cp = GFL_NEW_LOW(heap->GetLowerHandle()) pml::pokepara::CoreParam(heap->GetLowerHandle(), MONSNO_NULL, 0, 0);
  // 仲間のポケモンデータを初期化
  cp->Serialize_Core(m_battleFesData.partner[index].rentalPokemon.pokemonCoreParam);
  m_battleFesData.partner[index].isRegistPoke = false;
  GFL_DELETE cp;
}

//-----------------------------------------------------------------------------
/**
* @brief   仲間のレンタルポケモンデータの登録 CoreParamをセット
* @param[in] index 0~1
* @param[in] pml::pokepara::CoreParam* cp
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerPokemonData(const u32 index, const pml::pokepara::CoreParam* cp)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(cp->IsNull() == false);
  cp->Serialize_Core(&m_battleFesData.partner[index].rentalPokemon);
  m_battleFesData.partner[index].isRegistPoke = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   仲間のレンタルポケモンデータの取得 CoreParamを取得
* @param[in] index 0~1
* @param[in] pml::pokepara::CoreParam* cp
*/
//-----------------------------------------------------------------------------
void BattleFesSave::GetPartnerPokemonData(const u32 index, pml::pokepara::CoreParam* cp)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistPoke);
  cp->Deserialize_Core(&m_battleFesData.partner[index].rentalPokemon);
}

//-----------------------------------------------------------------------------
/**
* @brief   仲間が登録されているか
* @param[in] index
* @param[out] bool
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsPartner(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  if (m_battleFesData.partner[index].isRegistName &&
      m_battleFesData.partner[index].isRegistIcon &&
      m_battleFesData.partner[index].isRegistMegaRing &&
      m_battleFesData.partner[index].isRegistZenryokuRing &&
      m_battleFesData.partner[index].isRegistBall &&
      m_battleFesData.partner[index].isRegistSex  &&
      m_battleFesData.partner[index].isRegistPoke && 
      m_battleFesData.partner[index].isRegistHello &&
      m_battleFesData.partner[index].isRegistEmotion &&
      m_battleFesData.partner[index].isRegistRegionCode &&
      m_battleFesData.partner[index].isRegistRomCode &&
      m_battleFesData.partner[index].isRegistLanguageId &&
      m_battleFesData.partner[index].isRegistAddressId &&
      m_battleFesData.partner[index].isRegistRank)
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
* @brief   仲間の名前登録
* @param[in] index
* @param[in] gfl2::str::STRCODE*
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerName(const u32 index, const gfl2::str::STRCODE* name)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  gfl2::std::MemCopy(name, m_battleFesData.partner[index].name, sizeof(m_battleFesData.partner[index].name));
  m_battleFesData.partner[index].isRegistName = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   仲間の名前取得
* @param[in] index
* @param[in] gfl2::str::STRCODE* (２６バイト以上のサイズ必須)
*/
//-----------------------------------------------------------------------------
void BattleFesSave::GetPartnerName(const u32 index, gfl2::str::STRCODE* name) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistName);
  gfl2::std::MemCopy(m_battleFesData.partner[index].name, name, sizeof(m_battleFesData.partner[index].name));
}

//-----------------------------------------------------------------------------
/**
* @brief   定型文挨拶登録
* @param[in] index
* @param[in] u16
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerHello(const u32 index, const u16 patternHello)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(patternHello < msg_jf_phrase_max);
  m_battleFesData.partner[index].patternHello = patternHello;
  m_battleFesData.partner[index].isRegistHello = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   定型文挨拶取得
* @param[in] index
* @param[out] u16
*/
//-----------------------------------------------------------------------------
u16 BattleFesSave::GetPartnerHello(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistHello);
  return m_battleFesData.partner[index].patternHello;
}

//-----------------------------------------------------------------------------
/**
* @brief   定型文感動登録
* @param[in] index
* @param[in] u16
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerEmotion(const u32 index, const u16 patternEmotion)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(patternEmotion < msg_jf_phrase_max);
  m_battleFesData.partner[index].patternEmotion = patternEmotion;
  m_battleFesData.partner[index].isRegistEmotion = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   定型文感動取得
* @param[in] index
* @param[out] u16
*/
//-----------------------------------------------------------------------------
u16 BattleFesSave::GetPartnerEmotion(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistEmotion);
  return m_battleFesData.partner[index].patternEmotion;
}

//-----------------------------------------------------------------------------
/**
* @brief   ボール投げ種類の登録
* @param[in] index
* @param[in] u8 (0~7)
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerBallThrowType(const u32 index, const u8 ballThrowType)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(ballThrowType < Savedata::Misc::BALL_THROW_TYPE_NUM);
  m_battleFesData.partner[index].ballThrowType = ballThrowType;
  m_battleFesData.partner[index].isRegistBall = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   ボール投げ種類の取得
* @param[in] index
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerBallThrowType(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistBall);
  return m_battleFesData.partner[index].ballThrowType;
}

//-----------------------------------------------------------------------------
/**
* @brief   メガリングフラグの登録
* @param[in] index
* @param[in] u8
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerMegaRing(const u32 index, const u8 has_mega_ring)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(has_mega_ring <= static_cast<u8>(true));
  m_battleFesData.partner[index].has_mega_ring = has_mega_ring;
  m_battleFesData.partner[index].isRegistMegaRing = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   メガリングフラグの取得
* @param[in] index
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerMegaRing(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistMegaRing);
  return m_battleFesData.partner[index].has_mega_ring;
}

//-----------------------------------------------------------------------------
/**
* @brief   ぜんりょくリングフラグの登録
* @param[in] index
* @param[in] u8
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerZenryokuRing(const u32 index, const u8 has_zenryoku_ring)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(has_zenryoku_ring <= static_cast<u8>(true));
  m_battleFesData.partner[index].has_zenryoku_ring = has_zenryoku_ring;
  m_battleFesData.partner[index].isRegistZenryokuRing = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   ぜんりょくリングフラグの取得
* @param[in] index
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerZenryokuRing(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistZenryokuRing);
  return m_battleFesData.partner[index].has_zenryoku_ring;
}

//-----------------------------------------------------------------------------
/**
* @brief   性別の登録
* @param[in] index
* @param[in] u8
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerSex(const u32 index, const u8 sex)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(sex <= PM_FEMALE);
  m_battleFesData.partner[index].sex = sex;
  m_battleFesData.partner[index].isRegistSex = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   性別の取得
* @param[in] index
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerSex(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistSex);
  return m_battleFesData.partner[index].sex;
}

//-----------------------------------------------------------------------------
/**
* @brief 仲間のレンタルポケモンデータの有無
* @param[in] index 0~1
* @param[in] pml::pokepara::CoreParam* cp
* @param[out] bool true : データ有り false : データ無し
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsExistPartnerPokemonData(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  return (m_battleFesData.partner[index].isRegistPoke != 0);
}

//-----------------------------------------------------------------------------
/**
* @brief 仲間が最大人数いるか否か
* @param[out] bool true : 最大人数登録されている false : 空きがある
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsFullPartner()
{
  for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    if (!IsPartner(index))
    {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
* @brief 指定の仲間データを削除
* @param[in] index
* @param[in] heap クリアに使用するテンポラリヒープ
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearPartnerData(const u32 index, gfl2::heap::HeapBase* heap)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  InitPartnerPokemonData(index, heap);
  gfl2::std::MemClear(&m_battleFesData.partner[index].name, sizeof(m_battleFesData.partner[index].name));
  m_battleFesData.partner[index].patternHello = 0;
  m_battleFesData.partner[index].patternEmotion = 0;

  m_battleFesData.partner[index].isRegistName = false;
  m_battleFesData.partner[index].isRegistIcon = false;
  m_battleFesData.partner[index].isRegistMegaRing = false;
  m_battleFesData.partner[index].isRegistZenryokuRing = false;
  m_battleFesData.partner[index].isRegistBall = false;
  m_battleFesData.partner[index].isRegistSex = false;
  m_battleFesData.partner[index].isRegistPoke = false;
  m_battleFesData.partner[index].isRegistHello = false;

  m_battleFesData.partner[index].isRegistEmotion = false;
  m_battleFesData.partner[index].isFesFun = false;
  m_battleFesData.partner[index].isRegistRegionCode = false;
  m_battleFesData.partner[index].isRegistRomCode = false;
  m_battleFesData.partner[index].isRegistLanguageId = false;
  m_battleFesData.partner[index].isRegistAddressId = false;
  m_battleFesData.partner[index].isRegistRank = false;

  m_battleFesData.partner[index].regionCode = 0;
  m_battleFesData.partner[index].romCode = 0;
  m_battleFesData.partner[index].languageId = 0;
  m_battleFesData.partner[index].addressId = 0;

  m_battleFesData.partner[index].ballThrowType = 0;
  m_battleFesData.partner[index].has_mega_ring = 0;
  m_battleFesData.partner[index].has_zenryoku_ring = 0;
  m_battleFesData.partner[index].sex = 0;
  gfl2::std::MemClear(&m_battleFesData.partner[index].icon, sizeof(m_battleFesData.partner[index].icon));
}

//-----------------------------------------------------------------------------
/**
* @brief   フェスファンフラグ（ＴＲＵＥ）設定
* @param[in] index
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerFesFun(const u32 index)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].isFesFun = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   フェスファンフラグの取得
* @param[in] index
* @param[out] bool
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsFesFun(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  return (m_battleFesData.partner[index].isFesFun != 0);
}

//-----------------------------------------------------------------------------
/**
* @brief 今のバトルセットカウント取得（０～５）
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetNowBattleSetCount() const
{
  return static_cast<u8>(m_battleFesData.winCount / BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
}

//-----------------------------------------------------------------------------
/**
* @brief ランクアップに必要なバトルセットカウント取得（１～５）
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetRankUpBattleSetCount() const
{
  const u8 RANKUP_BATTLE_SET_COUNT_MAX = 5;
  u8 setCount = gfl2::math::clamp(static_cast<u8>((m_battleFesData.rank / BattleFes::BattleFesDefine::BATTLE_SET_COUNT_PAUSE) + 1), static_cast<u8>(1), RANKUP_BATTLE_SET_COUNT_MAX);
  return setCount;
}

//-----------------------------------------------------------------------------
/**
* @brief 連勝数取得（０～２）
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetWinCount() const
{
  return (m_battleFesData.winCount % BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
}

//-----------------------------------------------------------------------------
/**
* @brief 連勝数カウントアップ（＋１）
*/
//-----------------------------------------------------------------------------
void BattleFesSave::AddWinCount()
{
  m_battleFesData.winCount++;
  m_battleFesData.winCount = gfl2::math::clamp(m_battleFesData.winCount, static_cast<u32>(0), static_cast<u32>(GetRankUpBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX));
#if PM_DEBUG
  BattleFesDebug::SetWinCount();
#endif
}

//-----------------------------------------------------------------------------
/**
* @brief 連勝数０クリア
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ZeroClearWinCount()
{
  m_battleFesData.winCount = 0;
#if PM_DEBUG
  BattleFesDebug::SetWinCount();
#endif
}

//-----------------------------------------------------------------------------
/**
* @brief 連勝数クリア（※０にするわけではない）
*        0~2→0
*        3~5→3
*        6~9→6
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearWinCount()
{
  m_battleFesData.winCount = GetNowBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX;
#if PM_DEBUG
  BattleFesDebug::SetWinCount();
#endif
}

//-----------------------------------------------------------------------------
/**
* @brief 全乱数回数取得
* @param[out] u16
*/
//-----------------------------------------------------------------------------
u16 BattleFesSave::GetTotalRandCount() const
{
  return m_battleFesData.totalRandCount;
}

//-----------------------------------------------------------------------------
/**
* @brief 全乱数回数クリア
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearTotalRandCount()
{
  m_battleFesData.totalRandCount = 0;
}

//-----------------------------------------------------------------------------
/**
* @brief 全乱数回数更新
*/
//-----------------------------------------------------------------------------
void BattleFesSave::UpdateTotalRandCount()
{
  m_battleFesData.totalRandCount += m_battleFesData.activeRandCount;
  const u16 max = 0xffff;
  m_battleFesData.totalRandCount = gfl2::math::clamp(m_battleFesData.totalRandCount, static_cast<u16>(0), max);
  ClearActiveRandCount();
}

//-----------------------------------------------------------------------------
/**
* @brief 今抽選されたポケモンに使用した乱数回数アップ（＋１）
*/
//-----------------------------------------------------------------------------
void BattleFesSave::AddActiveRandCount()
{
  ++m_battleFesData.activeRandCount;
  const u16 max = 0xffff;
  m_battleFesData.activeRandCount = gfl2::math::clamp(m_battleFesData.activeRandCount, static_cast<u16>(0), max);
}

//-----------------------------------------------------------------------------
/**
* @brief 今抽選されたポケモンに使用した乱数回数クリア
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearActiveRandCount()
{
  m_battleFesData.activeRandCount = 0;
}

//-----------------------------------------------------------------------------
/**
* @brief ランク回数取得
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetRank() const
{
  return m_battleFesData.rank;
}

//-----------------------------------------------------------------------------
/**
* @brief ランクアップ（＋１）
*/
//-----------------------------------------------------------------------------
void BattleFesSave::AddRank()
{
  m_battleFesData.rank++;
  m_battleFesData.rank = gfl2::math::clamp(m_battleFesData.rank, static_cast<u32>(0), static_cast<u32>(BattleFes::BattleFesDefine::BFES_RANK_MAX));
#if PM_DEBUG
  BattleFesDebug::SetRank();
#endif
}

//-----------------------------------------------------------------------------
/**
* @brief 日にち更新
*/
//-----------------------------------------------------------------------------
void BattleFesSave::UpdateBattleFesDayEvent()
{
  // 連勝していないならバトルフェスデータ削除
  if (GetWinCount() == 0)
  {
    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    // @fix GFMMCat[809]０時直前にすりかえ屋を利用しても受付で選ぶ前に０時を過ぎると抽選したポケモンが消えてしまう
    // 自分のポケモンは消さない
    for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
    {
      ClearPartnerData(index, pHeap);
    }
    ClearTrainerData();
  }
  ClearTotalRandCount();
  ClearActiveRandCount();
  ClearUseShop();
}

//-----------------------------------------------------------------------------
/**
* @brief バトルツリートレーナーデータ（ポケモン情報含む）設定
* param[in] u32 (0~4)
* param[in] const BattleFes::TRAINER_DATA&
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetTrainerData(const u32 index, const BattleFes::TRAINER_DATA trainer)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
  GFL_ASSERT_STOP(trainer.trID < GARC_battle_tree_trainer_DATA_NUM);
  m_battleFesData.trainer[index].trID = trainer.trID;
  m_battleFesData.trainer[index].pokeMakeSeed = trainer.pokeMakeSeed;
  for (u32 poke_count(0); poke_count < BattleFes::BattleFesDefine::POKEMON_MAX; ++poke_count)
  {
    m_battleFesData.trainer[index].battleTreeData[poke_count].pokeID = trainer.battleTreeData[poke_count].pokeID;
    m_battleFesData.trainer[index].battleTreeData[poke_count].Sex = trainer.battleTreeData[poke_count].Sex;
    m_battleFesData.trainer[index].battleTreeData[poke_count].Tokusei = trainer.battleTreeData[poke_count].Tokusei;
  }
}

//-----------------------------------------------------------------------------
/**
* @brief バトルツリートレーナーデータ（ポケモン情報含む）取得
* param[in] u32 (0~4)
* param[out] const BattleFes::TRAINER_DATA&
*/
//-----------------------------------------------------------------------------
const BattleFes::TRAINER_DATA& BattleFesSave::GetTrainerData(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
  return m_battleFesData.trainer[index];
}

//-----------------------------------------------------------------------------
/**
* @brief バトルツリートレーナーデータ（ポケモン情報含む）全削除
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearTrainerData()
{
  for (u32 tr_count(0); tr_count < BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX; ++tr_count)
  {
    m_battleFesData.trainer[tr_count].trID = GARC_battle_tree_trainer_DATA_NUM;
    m_battleFesData.trainer[tr_count].pokeMakeSeed = 0;
    for (u32 poke_count(0); poke_count < BattleFes::BattleFesDefine::POKEMON_MAX; ++poke_count)
    {
      m_battleFesData.trainer[tr_count].battleTreeData[poke_count].pokeID = BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX;
      m_battleFesData.trainer[tr_count].battleTreeData[poke_count].Sex = 0;
      m_battleFesData.trainer[tr_count].battleTreeData[poke_count].Tokusei = 0;
    }
  }
}

//-----------------------------------------------------------------------------
/**
* @brief 仲間にフェスファンを追加する
* param[in] u32 (0~1)
*/
//-----------------------------------------------------------------------------
void BattleFesSave::AddFesFunPartner(const u32 index, const u32 sex, const pml::pokepara::CoreParam* cp, poke_3d::model::DressUpParam* dressUpParam)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  // 登録
  SetPartnerPokemonData(index, cp);
  SetPartnerFesFun(index);

  // 名前
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  gfl2::str::MsgData* pMsgData = GFL_NEW_LOW(pHeap) gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_jf_window_DAT, pHeap->GetLowerHandle());
  gfl2::str::StrBuf tempStrBuf(256, pHeap->GetLowerHandle());
  // プリセットNPCの名前男女分
  pMsgData->GetString((1 + sex), tempStrBuf);
  gfl2::str::STRCODE name[::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN];
  tempStrBuf.PutStr(name, ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN);
  GFL_SAFE_DELETE(pMsgData);

  SetPartnerName(index, name);

  // 性別がFEMALEの場合は、オフセットずらす
  const int offsetSex = (sex == PM_MALE) ? 0 : msg_jf_phrase_max / 2;
  const u32 tblIndex = System::GflUse::GetPublicRand(NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::INIT_PATTENR_TABLE_NUM);
  // あいさつ
  SetPartnerHello(index, static_cast<u16>(NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitPatternMsg(tblIndex, 0) + offsetSex));
  // かんどう
  SetPartnerEmotion(index, static_cast<u16>(NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitPatternMsg(tblIndex, 2) + offsetSex));

  // アイコン
  Savedata::MyStatus::ICON_DATA icon;
  app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData(sex, &icon);
  icon.dressUpParam.ConvertFromDressUpParam(&icon.dressUpParam, *dressUpParam);
  icon.bDressDefault = false;
  icon.dressUpParam.hasZRing = true;
  icon.dressUpParam.hasBagCharm = true;
  SetPartnerIcon(index, &icon);
  SetPartnerRank(index, 0);

  SetPartnerMegaRing(index, static_cast<u8>(true));
  SetPartnerZenryokuRing(index, static_cast<u8>(true));
  SetPartnerBallThrowType(index, System::GflUse::GetPublicRand(Savedata::Misc::BALL_THROW_TYPE_NUM));
  SetPartnerSex(index, static_cast<u8>(sex));
  // 不使用のため適当
#if defined(GF_PLATFORM_CTR)
  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  SetPartnerRegionCode(index, pMyStatus->GetRegionCode());
  SetPartnerRomCode(index, pMyStatus->GetRomCode());
  SetPartnerLanguageId(index, pMyStatus->GetPokeLanguageId());
  SetPartnerAddressId(index, pMyStatus->GetSimpleAddressId().id);
#else
  SetPartnerRegionCode(index, 0);
  SetPartnerRomCode(index, 0);
  SetPartnerLanguageId(index, 0);
  SetPartnerAddressId(index, 0);
#endif
}

//----------------------------------------------------------------------------
/**
*  @brief ボスか否か
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsBoss(const u32 tr_count) const
{
  // BattleFes::BattleFesDefine::BATTLE_SET_COUNT_PAUSE-1セット目のFIGHT_TRAINER_MAX戦目はボス（サカキかマーマネ）
  // ５０ランクの５セット目のFIGHT_TRAINER_MAX戦目はボス（サカキかマーマネ）
  bool isBossSetCount = false;
  if (
      ((m_battleFesData.rank % BattleFes::BattleFesDefine::BATTLE_SET_COUNT_PAUSE) / (BattleFes::BattleFesDefine::BATTLE_SET_COUNT_PAUSE - 1) == 1) ||
      m_battleFesData.rank == BattleFes::BattleFesDefine::BFES_RANK_MAX
     )
  {
    isBossSetCount = true;
  }
  
  if (tr_count == BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX - 1 && 
      isBossSetCount &&
      // @fix MMCat[164]【フェスサークル】バトルエージェントにてボスが1セット目から出てくる
      // 最後のバトルセットであるか？を追加
      GetNowBattleSetCount() == (GetRankUpBattleSetCount() - 1))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerRegionCode(const u32 index, const u8 regionCode)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].regionCode = regionCode;
  m_battleFesData.partner[index].isRegistRegionCode = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerRegionCode(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistRegionCode);
  return m_battleFesData.partner[index].regionCode;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerRomCode(const u32 index, const u8 romCode)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].romCode = romCode;
  m_battleFesData.partner[index].isRegistRomCode = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerRomCode(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistRomCode);
  return m_battleFesData.partner[index].romCode;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerLanguageId(const u32 index, const u8 languageId)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].languageId = languageId;
  m_battleFesData.partner[index].isRegistLanguageId = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerLanguageId(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistLanguageId);
  return m_battleFesData.partner[index].languageId;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerAddressId(const u32 index, const u32 addressId)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].addressId = addressId;
  m_battleFesData.partner[index].isRegistAddressId = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u32 BattleFesSave::GetPartnerAddressId(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistAddressId);
  return m_battleFesData.partner[index].addressId;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerIcon(const u32 index, const Savedata::MyStatus::ICON_DATA* icon)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  gfl2::std::MemCopy(icon, &m_battleFesData.partner[index].icon, sizeof(m_battleFesData.partner[index].icon));
  // 黒サングラス装着
  poke_3d::model::DressUpParam dressUpParam;
  System::DressUpParamSaveData::ConvertToDressUpParam(&dressUpParam, m_battleFesData.partner[index].icon.dressUpParam);
  BattleFesTool::GetSunglassDressUpParam(&dressUpParam);
  System::DressUpParamSaveData::ConvertFromDressUpParam(&m_battleFesData.partner[index].icon.dressUpParam, dressUpParam);
  m_battleFesData.partner[index].isRegistIcon = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
const Savedata::MyStatus::ICON_DATA* BattleFesSave::GetPartnerIcon(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistIcon);
  return &m_battleFesData.partner[index].icon;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerRank(const u32 index, const u8 rank)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(rank <= BattleFes::BattleFesDefine::BFES_RANK_MAX);
  m_battleFesData.partner[index].rank = rank;
  m_battleFesData.partner[index].isRegistRank = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerRank(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistRank);
  return m_battleFesData.partner[index].rank;
}

//----------------------------------------------------------------------------
/**
*  @brief お店使用フラグ取得
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsUseShop() const
{
  return (m_battleFesData.isUseShop != 0);
}

//----------------------------------------------------------------------------
/**
*  @brief お店使用
*/
//-----------------------------------------------------------------------------
void BattleFesSave::UseShop()
{
  m_battleFesData.isUseShop = 1;
}

//----------------------------------------------------------------------------
/**
*  @brief サカキフラグON＆マーマネ→サカキ対応
*/
//-----------------------------------------------------------------------------
void BattleFesSave::OpenSakaki()
{
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  pEventWork->SetEventFlag(SYS_FLAG_BFES_SAKAKI_OPEN);
  // 最終トレーナーチェック
  u32 tr_count = BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX - 1;
  BattleFes::TRAINER_DATA trainer = GetTrainerData(tr_count);

  // @fix MMCat[197] バトルエージェントの連勝中に、レインボーロケット団イベントをクリアすると、ボス戦の「マーマネ」が「サカキ」のセリフを話す
  if (trainer.trID == BattleFes::BattleFesDefine::TR_ID_MAMANE)
  {
    // マーマネのデータをセーブしながら、RRイベントをクリアした場合、マーマネをサカキに作成し直す
    trainer.trID = BattleFes::BattleFesDefine::TR_ID_SAKAKI;
    // ポケモン選定固定化のためのシード値変更
    trainer.pokeMakeSeed = System::GflUse::GetPublicRand();
    // ポケモンデータクリア
    for (u32 poke_count(0); poke_count < BattleFes::BattleFesDefine::POKEMON_MAX; ++poke_count)
    {
      trainer.battleTreeData[poke_count].pokeID = BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX;
      trainer.battleTreeData[poke_count].Sex = 0;
      trainer.battleTreeData[poke_count].Tokusei = 0;
    }
    // トレーナーデータ保存
    SetTrainerData(tr_count, trainer);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief お店使用フラグクリア
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearUseShop()
{
  m_battleFesData.isUseShop = 0;
}

#if PM_DEBUG
u32 BattleFesSave::GetDebugWinCount()
{
  return m_battleFesData.winCount;
}

void BattleFesSave::SetWinCount(const u32 win_count)
{
  m_battleFesData.winCount = win_count;
  m_battleFesData.winCount = gfl2::math::clamp(m_battleFesData.winCount, static_cast<u32>(0), static_cast<u32>(GetRankUpBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX));
}

void BattleFesSave::SetRank(const u32 rank)
{
  m_battleFesData.rank = rank;
  m_battleFesData.rank = gfl2::math::clamp(m_battleFesData.rank, static_cast<u32>(0), static_cast<u32>(BattleFes::BattleFesDefine::BFES_RANK_MAX));
}

void BattleFesSave::SetTotalRandCount(const u16 count)
{
  m_battleFesData.totalRandCount = count;
  const u16 max = 0xffff;
  m_battleFesData.totalRandCount = gfl2::math::clamp(m_battleFesData.totalRandCount, static_cast<u16>(0), max);
}
#endif

GFL_NAMESPACE_END(Savedata)

