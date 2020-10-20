//==============================================================================
/**
 * @file	BattleFesPokeLotterySystem.cpp
 * @brief	フィールドギミック：バトルフェス　ポケモン抽選管理
 * @author	munakata_kai
 * @date	2016/11/18
 */
// =============================================================================
#include "Field/FieldStatic/include/BattleFes/BattleFesPokeLotterySystem.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/BattleFesSave.h"
#include "Savedata/include/EventWork.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battle_tree_poke.gaix"

#include <math/include/gfl2_math.h>
#include "Field/FieldStatic/include/BattleInst/BattleInstData.h"

// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include "Field/FieldStatic/include/BattleFes/BattleFesTool.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"


GFL_NAMESPACE_BEGIN(BattleFes)

BattleFesPokeLotterySystem::BattleFesPokeLotterySystem(gfl2::heap::HeapBase* pHeap)
{
  for (u32 index(0); index < BattleFesDefine::POKEMON_MAX; ++index)
  {
    m_pPokeParam[index] = GFL_NEW(pHeap) pml::pokepara::PokemonParam(pHeap, MONSNO_NULL, 0, 0);
    m_BattleTreeData[index].pokeID = 0;
    m_BattleTreeData[index].Sex = 0;
    m_BattleTreeData[index].Tokusei = 0;
  }
}

BattleFesPokeLotterySystem::~BattleFesPokeLotterySystem()
{
  for (u32 index(0); index < BattleFesDefine::POKEMON_MAX; ++index)
  {
    GFL_SAFE_DELETE(m_pPokeParam[index]);
  }
}

//-----------------------------------------------------------------------------
/**
* @brief ポケモン抽選（三匹決定）
* @param[in] randSeed シード値
*/
//-----------------------------------------------------------------------------
void BattleFesPokeLotterySystem::PokemonLottery(const u32 randSeed, gfl2::heap::HeapBase* pHeap, const bool isShop)
{
  // 乱数初期化
  m_randSys.Initialize(randSeed);

  GameSys::GameData*       pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Field::EventWork*        ev = pGameData->GetEventWork();
  bool                     isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave(isEvent);

  u16 totalRandCount = pBattleFesSave->GetTotalRandCount();
  for (u16 index(0); index < totalRandCount; ++index)
  {
    m_randSys.Next();
  }

  pBattleFesSave->ClearActiveRandCount();

  app::util::FileAccessor::FileOpenSync(ARCID_BATTLE_TREE_POKE, pHeap);

  for (u32 index(0); index < BattleFesDefine::POKEMON_MAX; ++index)
  {
    if (isShop)
    {
      // お店抽選
      createShopPokemon(pHeap, index);
    }
    else
    {
      createPokemon(pHeap, index);
    }
  }

  app::util::FileAccessor::FileCloseSync(ARCID_BATTLE_TREE_POKE);

  if (isShop)
  {
    pBattleFesSave->UseShop();
  }
}

//-----------------------------------------------------------------------------
/**
* @brief 抽選されたポケモンを取得
* @param[in] index (0~2)
* @param[out] pml::pokepara::PokemonParam*
*/
//-----------------------------------------------------------------------------
pml::pokepara::PokemonParam* BattleFesPokeLotterySystem::GetPokemon(const u32 index) const
{
  GFL_ASSERT(index < BattleFesDefine::POKEMON_MAX);
  return m_pPokeParam[index];
}

//-----------------------------------------------------------------------------
/**
* @brief 抽選されたポケモンのバトルツリーデータ取得
* @param[in] index (0~2)
* @param[out] BATTLE_TREE_DATA
*/
//-----------------------------------------------------------------------------
const BATTLE_TREE_DATA& BattleFesPokeLotterySystem::GetBattleTreeData(const u32 index) const
{
  GFL_ASSERT(index < BattleFesDefine::POKEMON_MAX);
  return m_BattleTreeData[index];
}

//-----------------------------------------------------------------------------
/**
* @brief 抽選されたポケモンのパラメータを再計算（レベル更新用）
*/
//-----------------------------------------------------------------------------
void BattleFesPokeLotterySystem::UpdatePokemonData(gfl2::heap::HeapBase* pHeap)
{
  GameSys::GameData*       pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Field::EventWork*        ev = pGameData->GetEventWork();
  bool                     isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  u8                       rank = pBattleFesSave->GetRank();
  // メガリング所持の有無取得
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
  NetAppLib::JoinFesta::PacketGameStatus1 val;
  if (pMyData->GetPacketGameStatus1(&val) == false)
  {
    // 情報取得失敗時
    val.has_mega_ring = 0;
    val.has_zenryoku_ring = 1;
  }
  for (u32 index(0); index < BattleFesDefine::POKEMON_MAX; ++index)
  {
    pml::pokepara::PokemonParam* pPokeParam = BattleFesTool::CreateBattleTreePokemon(pHeap->GetLowerHandle(), m_BattleTreeData[index].pokeID, static_cast<int>(m_BattleTreeData[index].Sex), static_cast<int>(m_BattleTreeData[index].Tokusei), rank, val.has_mega_ring, val.has_zenryoku_ring);
    // レベルしか変わっていないか確認
    GFL_ASSERT(m_pPokeParam[index]->GetMonsNo() == pPokeParam->GetMonsNo());
    GFL_ASSERT(m_pPokeParam[index]->GetFormNo() == pPokeParam->GetFormNo());
    GFL_ASSERT(m_pPokeParam[index]->GetTokuseiNo() == pPokeParam->GetTokuseiNo());
    GFL_ASSERT(m_pPokeParam[index]->GetSex() == pPokeParam->GetSex());
    GFL_ASSERT(m_pPokeParam[index]->GetItem() == pPokeParam->GetItem());
#if PM_DEBUG
    GFL_ASSERT(m_pPokeParam[index]->GetWazaCount() == pPokeParam->GetWazaCount());
    for (u8 waza_index(0); waza_index < m_pPokeParam[index]->GetWazaCount(); ++waza_index)
    {
      GFL_ASSERT(m_pPokeParam[index]->GetWazaNo(waza_index) == pPokeParam->GetWazaNo(waza_index));
    }
#endif
    m_pPokeParam[index]->CopyFrom(*pPokeParam);
    GFL_SAFE_DELETE(pPokeParam);
  }
}

// 非公開
//-----------------------------------------------------------------------------
/**
* @brief ポケモン作成（被りなし）
* @param[in] gfl2::heap::HeapBase*
* @param[in] u32 抽選番号（０～２）
*/
//-----------------------------------------------------------------------------
void BattleFesPokeLotterySystem::createPokemon(gfl2::heap::HeapBase* pHeap, const u32 index)
{
  GFL_ASSERT(index < BattleFesDefine::POKEMON_MAX);
  BattleInst::BINST_POKEMON_ROM_DATA  pd;
  pd.mons_no = MONSNO_NULL;
  const u32 LOOP_MAX = 100;
  u32 loop = 0;
  bool pokemon_same_check_flg = false;
  u32 idx = 0;

  GameSys::GameData*       pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Field::EventWork*        ev = pGameData->GetEventWork();
  bool                     isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave(isEvent);

  do
  {
    pokemon_same_check_flg = false;
    idx = m_randSys.Next(BattleFesDefine::BATTLE_TREE_ID_MAX);
    pBattleFesSave->AddActiveRandCount();
    getPokemonRomData(pHeap->GetLowerHandle(), idx, &pd);
    // ポケモン被り判定
    for (u32 poke_index(0); poke_index < index; ++poke_index)
    {
      if (pd.mons_no == m_pPokeParam[poke_index]->GetMonsNo())
      {
        pokemon_same_check_flg = true;
        break;
      }
    }
    ++loop;
  } while (pd.mons_no == MONSNO_NULL || (pokemon_same_check_flg && loop < LOOP_MAX));

  u8 tokusei = getPokemonRandomTokusei();
  pBattleFesSave->AddActiveRandCount();
  u8 sex = getPokemonRandomSex();
  pBattleFesSave->AddActiveRandCount();
  m_BattleTreeData[index].pokeID = idx;
  m_BattleTreeData[index].Sex = sex;
  m_BattleTreeData[index].Tokusei = tokusei;
  // メガリング所持の有無取得
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
  NetAppLib::JoinFesta::PacketGameStatus1 val;
  if (pMyData->GetPacketGameStatus1(&val) == false)
  {
    // 情報取得失敗時
    val.has_mega_ring = 0;
    val.has_zenryoku_ring = 1;
  }
  pml::pokepara::PokemonParam* pPokeParam = BattleFesTool::CreateBattleTreePokemon(pHeap->GetLowerHandle(), idx, static_cast<int>(sex), static_cast<int>(tokusei), pBattleFesSave->GetRank(), val.has_mega_ring, val.has_zenryoku_ring);
  m_pPokeParam[index]->CopyFrom(*pPokeParam);
  GFL_SAFE_DELETE(pPokeParam);
}

#include "BattleFesNoLotteryList.cdat"

//-----------------------------------------------------------------------------
/**
* @brief お店抽選によるポケモン作成（被りなし）
* @param[in] gfl2::heap::HeapBase*
* @param[in] u32 抽選番号（０～２）
*/
//-----------------------------------------------------------------------------
void BattleFesPokeLotterySystem::createShopPokemon(gfl2::heap::HeapBase* pHeap, const u32 index)
{
  GFL_ASSERT(index < BattleFesDefine::POKEMON_MAX);
  BattleInst::BINST_POKEMON_ROM_DATA  pd;
  pd.mons_no = MONSNO_NULL;
  const u32 LOOP_MAX = 100;
  u32 loop = 0;
  bool pokemon_same_check_flg = false;
  u32 idx = 0;

  GameSys::GameData*       pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Field::EventWork*        ev = pGameData->GetEventWork();
  bool                     isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  u32 rank = pBattleFesSave->GetRank();
  // フィールド抽選
  const u32* p_exclusion_array = NULL;
  u32 exclusion_array_size = 0;
  BtlFesField field_type = (rank < BattleFesDefine::BATTLE_FES_FIELD_LIMIT_RANK) ? BATTLE_FES_FIELD_NONE : BattleFesTool::GetBtlFieldType(rank);

  switch (field_type)
  {
  case BATTLE_FES_FIELD_NONE:
    p_exclusion_array = s_None_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_None_NoPoke);
    break;
  case BATTLE_FES_FIELD_SHINE:
  case BATTLE_FES_FIELD_DAY:
    p_exclusion_array = s_Shine_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Shine_NoPoke);
    break;
  case BATTLE_FES_FIELD_RAIN:
  case BATTLE_FES_FIELD_STORM:
    p_exclusion_array = s_Rain_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Rain_NoPoke);
    break;
  case BATTLE_FES_FIELD_SNOW:
    p_exclusion_array = s_Snow_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Snow_NoPoke);
    break;
  case BATTLE_FES_FIELD_SAND:
    p_exclusion_array = s_Sand_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Sand_NoPoke);
    break;
  case BATTLE_FES_FIELD_ELEKI:
    p_exclusion_array = s_Eleki_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Eleki_NoPoke);
    break;
  case BATTLE_FES_FIELD_MIST:
    p_exclusion_array = s_Mist_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Mist_NoPoke);
    break;
  case BATTLE_FES_FIELD_PHYCHO:
    p_exclusion_array = s_Phycho_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Phycho_NoPoke);
    break;
  case BATTLE_FES_FIELD_GRASS:
    p_exclusion_array = s_Grass_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Grass_NoPoke);
    break;
  case BATTLE_FES_FIELD_TURBULENCE:
    p_exclusion_array = s_Turbulence_NoPoke;
    exclusion_array_size = GFL_NELEMS(s_Turbulence_NoPoke);
    break;
  default:
    // 来てはいけない
    GFL_ASSERT_STOP(0);
    break;
  }

  do
  {
    pokemon_same_check_flg = false;
    idx = m_randSys.Next(BattleFesDefine::BATTLE_TREE_ID_MAX);
    pBattleFesSave->AddActiveRandCount();
    // 除外リスト判定
    if (idx >= (BattleFesDefine::BATTLE_TREE_ID_MAX / 2))
    {
      // ＩＤ高い方から検索
      for (s32 poke_index(exclusion_array_size - 1); poke_index >= 0; --poke_index)
      {
        if (idx == p_exclusion_array[poke_index])
        {
          pokemon_same_check_flg = true;
          GFL_PRINT("BattleFesPokeLottery Exclusion BattleTreeID = %d\n", idx);
          break;
        }
      }
    }
    else
    {
      // ＩＤ低い方から検索
      for (u32 poke_index(0); poke_index < exclusion_array_size; ++poke_index)
      {
        if (idx == p_exclusion_array[poke_index])
        {
          pokemon_same_check_flg = true;
          GFL_PRINT("BattleFesPokeLottery Exclusion BattleTreeID = %d\n", idx);
          break;
        }
      }
    }
    // 除外リストにない場合
    if (!pokemon_same_check_flg)
    {
      getPokemonRomData(pHeap->GetLowerHandle(), idx, &pd);
      // ポケモン被り判定
      for (u32 poke_index(0); poke_index < index; ++poke_index)
      {
        if (pd.mons_no == m_pPokeParam[poke_index]->GetMonsNo())
        {
          pokemon_same_check_flg = true;
          break;
        }
      }
      // 除外された場合、ループ回数を増加させないため、ここで増加
      ++loop;
    }
  } while (pd.mons_no == MONSNO_NULL || (pokemon_same_check_flg && loop < LOOP_MAX));

  u8 tokusei = getPokemonRandomTokusei();
  pBattleFesSave->AddActiveRandCount();
  u8 sex = getPokemonRandomSex();
  pBattleFesSave->AddActiveRandCount();
  m_BattleTreeData[index].pokeID = idx;
  m_BattleTreeData[index].Sex = sex;
  m_BattleTreeData[index].Tokusei = tokusei;
  // メガリング所持の有無取得
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
  NetAppLib::JoinFesta::PacketGameStatus1 val;
  if (pMyData->GetPacketGameStatus1(&val) == false)
  {
    // 情報取得失敗時
    val.has_mega_ring = 0;
    val.has_zenryoku_ring = 1;
  }
  pml::pokepara::PokemonParam* pPokeParam = BattleFesTool::CreateBattleTreePokemon(pHeap->GetLowerHandle(), idx, static_cast<int>(sex), static_cast<int>(tokusei), rank, val.has_mega_ring, val.has_zenryoku_ring);
  m_pPokeParam[index]->CopyFrom(*pPokeParam);
  GFL_SAFE_DELETE(pPokeParam);
}

//-----------------------------------------------------------------------------
/**
* @brief ポケモンの性別取得
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesPokeLotterySystem::getPokemonRandomSex()
{
  // BattleFesDefine::POKEMON_SEX_END - 1 →性別不明は除外
  return m_randSys.Next(BattleFesDefine::POKEMON_SEX_END - 1);
}

//-----------------------------------------------------------------------------
/**
* @brief ポケモンの特性取得
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesPokeLotterySystem::getPokemonRandomTokusei()
{
  return m_randSys.Next(BattleFesDefine::POKEMON_TOKUSEI_END);
}

//--------------------------------------------------------------
/**
* @brief   バトル施設モンスターデータを取得する
* @param   pHeap    ヒープ
* @param   data_idx  データインデックス
* @param   pd　格納バッファ
*/
//--------------------------------------------------------------
void BattleFesPokeLotterySystem::getPokemonRomData(gfl2::heap::HeapBase* pHeap, int data_idx, BattleInst::BINST_POKEMON_ROM_DATA *pd)
{
  size_t size = 0;
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()->GetArcFile(ARCID_BATTLE_TREE_POKE)->GetDataSize(&size, data_idx, NULL);
  if (size > 0)
  {
    app::util::FileAccessor::FileLoadBufSync(ARCID_BATTLE_TREE_POKE, data_idx, reinterpret_cast<void*>(pd), size, pHeap, false);
  }
  else
  {
    GFL_ASSERT_MSG(0, "data size <= 0\n");
  }
}

GFL_NAMESPACE_END(BattleFes)
