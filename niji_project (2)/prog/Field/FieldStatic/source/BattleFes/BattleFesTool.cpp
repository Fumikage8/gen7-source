//======================================================================
/**
* @file BattleFesTool.h
* @date 2016/11/25
* @author munakata_kai
* @brief バトルフェスの生成ツール郡
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "Field/FieldStatic/include/BattleFes/BattleFesTool.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstTool.h"
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"

#include <debug/include/gfl2_Assert.h>

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_FsArcFile.h>
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "System/include/GflUse.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include <pml/include/item/item.h>

#include "Battle/include/battle_def.h"
#include <arc_def_index/arc_def.h>

#include "Field/FieldStatic/include/Time/EvTime.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/BattleFes/BattleFesDebug.h"
#endif

GFL_NAMESPACE_BEGIN( BattleFesTool )

//------------------------------------------------------------------
/**
*  @brief    pokeparaが所持しているアイテムを、Trainerは利用可能であるかを判定する
*            対象はZワザ、メガシンカ
*/
//------------------------------------------------------------------
PokeparaItemAvailabilityCheckResult  CheckPokeparaItemAvailability(const pml::pokepara::CoreParam* const pPokeCoreParam, const bool has_mega_ring, const bool has_z_ring)
{
  GFL_ASSERT_STOP(pPokeCoreParam);
  const u16                               itemID = pPokeCoreParam->GetItem();

  if (itemID != ITEM_DUMMY_DATA)
  {
    /*  メガシンカ  */
    if ((item::ITEM_CheckMegaStone(itemID)))
    {
      if (!has_mega_ring) return  CHECK_RESULT_NotAvailable;   /*  メガシンカはメガリングを所持しているTrainerでないと発動できない */
    }

    /*  Zワザ       */
    if (item::ITEM_CheckPiece(itemID))
    {
      if (!has_z_ring)   return  CHECK_RESULT_NotAvailable;    /*  ZワザはZリングを所持しているTrainerでないと発動できない         */
    }

    /*  使用条件を満たしているか、使用条件のないアイテムを所持している  */
    return CHECK_RESULT_Available;
  }

  /*  アイテムを所持していない  */
  return CHECK_RESULT_NoItem;
}

//-----------------------------------------------------------------------------
/**
* @brief バトルツリーポケモン生成 返値のPokemonParam*は開放してください
* param[in] gfl2::heap::HeapBase*
* param[in] u16 バトルツリーＩＤ
* param[in] u8  性別
* param[in] u8　特性
* param[in] u8　ランク
* param[out] pml::pokepara::PokemonParam*
*/
//-----------------------------------------------------------------------------
pml::pokepara::PokemonParam* CreateBattleTreePokemon(gfl2::heap::HeapBase* heap, const u16 battle_tree_id, const int sex, const int tokusei, const u8 rank, const u8 has_mega_ring, const u8 has_z_ring)
{
  GFL_ASSERT_STOP(battle_tree_id < BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX);
  GFL_ASSERT_STOP(sex < BattleFes::BattleFesDefine::POKEMON_SEX_END || sex == pml::pokepara::INIT_SPEC_SEX_BOTH);
  GFL_ASSERT_STOP(tokusei < BattleFes::BattleFesDefine::POKEMON_TOKUSEI_END || tokusei == -1);
  BattleInst::BINST_POKEMON_ROM_DATA  pd;
  BattleInstTool::GetPokemonRomData(heap, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), ARCID_BATTLE_TREE_POKE, static_cast<int>(battle_tree_id), &pd);
  pml::pokepara::PokemonParam* pp = BattleInstTool::MakePokemonParam(&pd, BattleFes::BattleFesDefine::POKEMON_INIT_LEVEL + rank, BattleFes::BattleFesDefine::POWER, heap, tokusei, sex);
  if (CheckPokeparaItemAvailability(pp, (has_mega_ring != 0), (has_z_ring != 0)) == BattleFesTool::CHECK_RESULT_NotAvailable)
  {
    // 使えないアイテムだったので持ち物なし状態に
    pp->RemoveItem();
  }
  return pp;
}

//------------------------------------------------------------------
/**
  *  @brief    JoinFestaPersonalData から BatleFesCircle用のPokeparaを生成する
  */
//------------------------------------------------------------------
pml::pokepara::PokemonParam* CreateBattleTreePokemon(gfl2::heap::HeapBase* pHeap, const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonalData)
{
  // バトルフェスのデータ受信しているかチェック
  if( IsReceiveBattleFesPacket( rJFPersonalData ) == false )
  { // パケット受信していない
    return NULL;
  }

  NetAppLib::JoinFesta::PacketGameStatus1 val;
  
  if(rJFPersonalData.GetPacketGameStatus1(&val))
  {
    return CreateBattleTreePokemon(pHeap, val.battleFesPokeId, val.battleFesPokeSex, val.battleFesPokeTokuseiIndex, val.battleFesRank, val.has_mega_ring, val.has_zenryoku_ring);
  }

  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @brief ランダムでバトルツリーポケモン生成 返値のPokemonParam*は開放してください※フェスファンポケモン作成限定
* param[in] gfl2::heap::HeapBase*
* param[out] BATTLE_TREE_DATA*
* return pml::pokepara::PokemonParam*
*/
//-----------------------------------------------------------------------------
pml::pokepara::PokemonParam* CreateRandomBattleTreePokemon(gfl2::heap::HeapBase* pHeap, BattleFes::BATTLE_TREE_DATA* pBattleTreeData)
{
  u32 idx = 0;
  // 空きデータない仕様確認済み
  idx = System::GflUse::GetPublicRand(BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX);
  pml::pokepara::PokemonParam* pPokeParam = CreateBattleTreePokemon(pHeap, idx, pml::pokepara::INIT_SPEC_SEX_BOTH, -1, 0, static_cast<u8>(true), static_cast<u8>(true));
  if (pBattleTreeData != NULL)
  {
    pBattleTreeData->pokeID = idx;
    pBattleTreeData->Sex = static_cast<u8>(pPokeParam->GetSex());
    pBattleTreeData->Tokusei = pPokeParam->GetTokuseiIndex();
  }
  return pPokeParam;
}

/* ----------------------------------------------------------------------------------------- */
/**
* @brief   ランダムで着せ替えパラメータ取得
* @param[in]  性別
* @param[in]  ヒープ
* @param[in]  セットする着せ替えのパターン
*/
/* ----------------------------------------------------------------------------------------- */
void GetDressUpParamRandom(const u32 sex, gfl2::heap::HeapBase* pHeap, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager = GFL_NEW_LOW(pHeap) poke_3d::model::DressUpModelResourceManager();
  pDressUpModelResourceManager->Initialize(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), pHeap->GetLowerHandle(), System::DressUp::GetDressUpArcIdListField());

  static const s16 sc_bodyCountList[] =
  {
    -1,                           // 性別
    DRESSUP_COLOR_ID_SKIN_COUNT,  // 体の色
    DRESSUP_COLOR_ID_EYE_COUNT,   // 目の色
    DRESSUP_COLOR_ID_HAIR_COUNT,  // 髪の色
    DRESSUP_COLOR_ID_LIP_COUNT,   // リップの色
  };

  static const s16 sc_itemCountList_Hero[] =
  {
    DRESSUP_ITEM_ID_HERO_FACE_COUNT,          //ITEM_CATEGORY_FACE,                   // 顔
    DRESSUP_ITEM_ID_HERO_HAIR_COUNT,          //ITEM_CATEGORY_HAIR_STYLE,             // 髪型
    -1,                                       //ITEM_CATEGORY_ACCBADGE,               // バッジアクセサリ
    DRESSUP_ITEM_ID_HERO_ACCEYE_COUNT,        //ITEM_CATEGORY_ACCEYE,                 // アイアクセサリ
    -1,                                       //ITEM_CATEGORY_ACCHAIR,                // ヘアアクセサリ
    DRESSUP_ITEM_ID_HERO_BAG_COUNT,           //ITEM_CATEGORY_BAG,                    // バッグ
    -1,                                       //ITEM_CATEGORY_BNGL,                   // バングル
    -1,                                       //ITEM_CATEGORY_BODY,                   // ボディ
    DRESSUP_ITEM_ID_HERO_BOTTOMS_COUNT,       //ITEM_CATEGORY_BOTTOMS,                // ボトムス
    DRESSUP_ITEM_ID_HERO_HAT_COUNT,           //ITEM_CATEGORY_HAT,                    // 帽子
    DRESSUP_ITEM_ID_HERO_LEGS_COUNT,          //ITEM_CATEGORY_LEGS,                   // レッグ
    DRESSUP_ITEM_ID_HERO_SHOES_COUNT,         //ITEM_CATEGORY_SHOES,                  // シューズ
    DRESSUP_ITEM_ID_HERO_TOPS_COUNT,          //ITEM_CATEGORY_TOPS,                   // トップス
    -1,                                       //ITEM_CATEGORY_SITU,                   // シチュエーション着替え
  };

  static const s16 sc_itemCountList_Heroine[] =
  {
    DRESSUP_ITEM_ID_HEROINE_FACE_COUNT,       //ITEM_CATEGORY_FACE,                   // 顔
    DRESSUP_ITEM_ID_HEROINE_HAIR_COUNT,       //ITEM_CATEGORY_HAIR_STYLE,             // 髪型
    -1,                                       //ITEM_CATEGORY_ACCBADGE,               // バッジアクセサリ
    DRESSUP_ITEM_ID_HEROINE_ACCEYE_COUNT,     //ITEM_CATEGORY_ACCEYE,                 // アイアクセサリ
    DRESSUP_ITEM_ID_HEROINE_ACCHAIR_COUNT,    //ITEM_CATEGORY_ACCHAIR,                // ヘアアクセサリ
    DRESSUP_ITEM_ID_HEROINE_BAG_COUNT,        //ITEM_CATEGORY_BAG,                    // バッグ
    -1,                                       //ITEM_CATEGORY_BNGL,                   // バングル
    -1,                                       //ITEM_CATEGORY_BODY,                   // ボディ
    DRESSUP_ITEM_ID_HEROINE_BOTTOMS_COUNT,    //ITEM_CATEGORY_BOTTOMS,                // ボトムス
    DRESSUP_ITEM_ID_HEROINE_HAT_COUNT,        //ITEM_CATEGORY_HAT,                    // 帽子
    DRESSUP_ITEM_ID_HEROINE_LEGS_COUNT,       //ITEM_CATEGORY_LEGS,                   // レッグ
    DRESSUP_ITEM_ID_HEROINE_SHOES_COUNT,      //ITEM_CATEGORY_SHOES,                  // シューズ
    DRESSUP_ITEM_ID_HEROINE_TOPS_COUNT,       //ITEM_CATEGORY_TOPS,                   // トップス
    -1,                                       //ITEM_CATEGORY_SITU,                   // シチュエーション着替え
  };

  // ボディ設定
  for (u32 i = 0; i < poke_3d::model::DressUpParam::BODY_CATEGORY_COUNT; ++i)
  {
    out_pDressUpParam->bodyParams[i] = -1;

    if (sc_bodyCountList[i] < 0)
    {
      continue;
    }

    out_pDressUpParam->bodyParams[i] = System::GflUse::GetPublicRand(sc_bodyCountList[i]);
  }

  const s16* pItemCountList = NULL;

  // 性別ごとの設定
  if (sex == PM_MALE)
  {
    out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] = poke_3d::model::DressUpParam::SEX_MALE;
    out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF; // 男ならリップは無し
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P1_BNGL_ZENBRESS01;         // バングルは必ず付ける
    pItemCountList = sc_itemCountList_Hero;
  }
  else
  {
    out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] = poke_3d::model::DressUpParam::SEX_FEMALE;
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS01;         // バングルは必ず付ける
    pItemCountList = sc_itemCountList_Heroine;
  }

  // アイテム設定
  for (u32 i = 0; i < poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT; ++i)
  {
    out_pDressUpParam->itemParams[i] = -1;

    if (pItemCountList[i] < 0)
    {
      continue;
    }

    out_pDressUpParam->itemParams[i] = System::GflUse::GetPublicRand(pItemCountList[i]);
  }

  u32 rand = 0;

  // ランダムで帽子、アクセサリーを外す
  rand = System::GflUse::GetPublicRand();
  if (rand % 2 == 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
  }
  rand = System::GflUse::GetPublicRand();
  if (rand % 2 == 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE] = -1;
  }
  rand = System::GflUse::GetPublicRand();
  if (rand % 2 == 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
  }

  // 帽子がかぶれない髪型なら帽子を外す
  poke_3d::model::DressUpModelResourceManagerCore* pDumrmc = pDressUpModelResourceManager->GetDressUpModelResourceManagerCore(static_cast<poke_3d::model::DressUpParam::Sex>(out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]));
  if (!pDumrmc->CanDressUpHairWearHat(out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE]))
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
  }
  // 帽子をかぶっているならヘアアクセサリーは外す
  if (out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] >= 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
  }

  // 念のため組み合わせチェックをして問題があればデフォルトに戻す
  if (!pDumrmc->CheckDressUpCombination(*out_pDressUpParam))
  {
    System::DressUp::GetDressUpParamDefault(sex, out_pDressUpParam);
  }

  pDressUpModelResourceManager->Finalize();
  GFL_SAFE_DELETE(pDressUpModelResourceManager);
}

/* ----------------------------------------------------------------------------------------- */
/**
* @brief   サングラス着用着せ替えパラメータ取得
* @param[in]  着せ替えのパラメータ
*/
/* ----------------------------------------------------------------------------------------- */
void GetSunglassDressUpParam(poke_3d::model::DressUpParam* out_pDressUpParam)
{
  // 黒サングラス
  if (out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == PM_MALE)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE] = DRESSUP_ITEM_ID_P1_ACCEYE_MMJ_AGENT01;
  }
  else
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE] = DRESSUP_ITEM_ID_P2_ACCEYE_MMJ_AGENT01;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool IsRecentPartner(const u32 elapsedHour)
{
  return (elapsedHour < BattleFes::BattleFesDefine::PARTNER_LIFETIME_HOUR);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool IsRecentPartner(const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonal)
{
  return IsRecentPartner(rJFPersonal.GetUpdateElapsedDate());
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief       バトルフェス用のパケットデータを受信しているかチェックする
 * @param[in]   rJFPersonal      チェックしたいパーソナルデータ
 * @return      trueで受信している
 */
/* ----------------------------------------------------------------------------------------- */
bool IsReceiveBattleFesPacket( const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonalData )
{
  NetAppLib::JoinFesta::PacketGameStatus1 val;

  // 受信データの有効性チェック
  if(rJFPersonalData.GetPacketGameStatus1(&val))
  {
    bool  bValid  = true;
    
    bValid  &= (val.battleFesPokeId < BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX);
    bValid  &= (val.battleFesPokeSex < BattleFes::BattleFesDefine::POKEMON_SEX_END);
    bValid  &= (val.battleFesPokeTokuseiIndex < BattleFes::BattleFesDefine::POKEMON_TOKUSEI_END);

    return bValid;
  }

  // そもそも受信していない
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief       通信人物の中から、バトルフェスに誘える人数を取得する
 * @return      誘う事が出来る人数
 */
/* ----------------------------------------------------------------------------------------- */
u32 GetBattleFesValidPatherCount()
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* jfPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  u32 count = 0;
  
  for( u32 i = 0; i < NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = jfPersonalManager->GetPersonalDataList( static_cast<NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    GFL_ASSERT( list ); //@fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);
          // memo
          // (*it)->IsOnline() は見なくて良い。バトルフェスはオンライン関係なく、経過時間で有効なデータになる。
          // バトルフェス用のパケットデータ受信している　かつ　経過時間が72時間以内なら誘える
          if( pPersonalData && IsReceiveBattleFesPacket( *pPersonalData ) && IsRecentPartner( *pPersonalData ) )
          {
            count++;
          }
        }
      }
    }
  }

  return count;
}


//------------------------------------------------------------------
/**
  *  @brief    JoinFestaPersonalDataの内容をBattleFesSaveにセットする
  */
//------------------------------------------------------------------
void SetJFPersonalToBattleFesSave(Savedata::BattleFesSave& rDst, const u32 partnerIndex, const NetAppLib::JoinFesta::JoinFestaPersonalData& rSrc, gfl2::heap::HeapBase* pTempHeapBase)
{
  GFL_ASSERT(pTempHeapBase);

  /*  pokepara  */
  {
    pml::pokepara::PokemonParam* pPokemonParam = BattleFesTool::CreateBattleTreePokemon(pTempHeapBase, rSrc);

    rDst.SetPartnerPokemonData(partnerIndex, pPokemonParam);
    GFL_SAFE_DELETE(pPokemonParam);
  }

  /*  trainerName  */
  {
    gfl2::str::STRCODE name[System::STRLEN_PLAYER_NAME + System::EOM_LEN];

    rSrc.GetName(name);
    rDst.SetPartnerName(partnerIndex, name);
  }

  rDst.SetPartnerHello(partnerIndex, rSrc.GetPattern(JoinFestaScript::PATTERN_HELLO));
  rDst.SetPartnerEmotion(partnerIndex, rSrc.GetPattern(JoinFestaScript::PATTERN_EMOTION));

  /*  Icon  */
  {
    rDst.SetPartnerIcon(partnerIndex, rSrc.GetDressUpIcon());
  }

  {
    NetAppLib::JoinFesta::PacketGameStatus1 packet1;
    const bool                              isValid = rSrc.GetPacketGameStatus1(&packet1);

    GFL_ASSERT(isValid);

    if(isValid)
    {
      rDst.SetPartnerRank(          partnerIndex, packet1.battleFesRank);
      rDst.SetPartnerBallThrowType( partnerIndex, packet1.ballThrowType);
      rDst.SetPartnerMegaRing(      partnerIndex, packet1.has_mega_ring);
      rDst.SetPartnerZenryokuRing(  partnerIndex, packet1.has_zenryoku_ring);
    }
  }

  rDst.SetPartnerSex(partnerIndex, rSrc.GetSex());
  // 不使用
  rDst.SetPartnerRegionCode(partnerIndex, rSrc.GetRegionCode());
  rDst.SetPartnerRomCode(partnerIndex, rSrc.GetRomVersion());
  rDst.SetPartnerLanguageId(partnerIndex, rSrc.GetLanguageId());
#if defined(GF_PLATFORM_CTR)
  rDst.SetPartnerAddressId(partnerIndex, rSrc.GetSimpleAddressId().id);
#else
  rDst.SetPartnerAddressId(partnerIndex, 0);
#endif
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  BFCTrainerInfo::Setup(const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonal)
{
  NetAppLib::JoinFesta::PacketGameStatus1 pgs1;
  const bool                              isValid = rJFPersonal.GetPacketGameStatus1(&pgs1);

  if(isValid)
  {
    rJFPersonal.GetName(name);
    iconData  = *(rJFPersonal.GetDressUpIcon());
    grade     = pgs1.battleFesRank;
  }

  return isValid;
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  BFCTrainerInfo::Setup(const Savedata::BattleFesSave& rBFSave, const u32 partnerIndex)
{
  rBFSave.GetPartnerName(partnerIndex, name);
  iconData = *(rBFSave.GetPartnerIcon(partnerIndex));
  grade = rBFSave.GetPartnerRank(partnerIndex);
}

//--------------------------------------------------------------
/**
* @brief 今日のBtlFesField取得
* @return BtlFesField
*/
//-------------------------------------------------------------- 
BtlFesField GetBtlFieldType(const u8 rank)
{
  gfl2::system::Date date;
  Field::EvTime::GetDeviceDateTime(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData(), &date);
  u32 battleFieldID = static_cast<u32>(date.GetDateTimeToDay());
  // 41~
  u32 MaxBattleFieldID = BATTLE_FES_FIELD_MAX;
  GFL_ASSERT(rank >= BattleFes::BattleFesDefine::BATTLE_FES_FIELD_LIMIT_RANK);
  const u32 FIRST_LIMIT_RANK = 15;
  const u32 SECOND_LIMIT_RANK = 20;
  const u32 THIRD_LIMIT_RANK = 30;
  const u32 FOURTH_LIMIT_RANK = 40;
  // @fix MMCat[223] 【フェスサークル】バトルエージェントにて「バトル設定」の天候が資料と相違している
  if (rank <= FIRST_LIMIT_RANK) MaxBattleFieldID = BATTLE_FES_FIELD_SNOW;       // 10~15
  else if (rank <= SECOND_LIMIT_RANK) MaxBattleFieldID = BATTLE_FES_FIELD_ELEKI;// 16~20
  else if (rank <= THIRD_LIMIT_RANK) MaxBattleFieldID = BATTLE_FES_FIELD_PHYCHO;// 21~30
  else if (rank <= FOURTH_LIMIT_RANK) MaxBattleFieldID = BATTLE_FES_FIELD_DAY;  // 31~40
#if PM_DEBUG
  if (BattleFesDebug::GetFieldType() > 0)
  {
    return static_cast<BtlFesField>(BattleFesDebug::GetFieldType());
  }
#endif
  return static_cast<BtlFesField>(battleFieldID % MaxBattleFieldID);
}

GFL_NAMESPACE_END( BattleFesTool )

