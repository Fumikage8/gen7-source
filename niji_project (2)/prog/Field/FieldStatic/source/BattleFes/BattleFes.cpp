//======================================================================
/**
* @file BattleFes.h
* @date 2016/11/23
* @author munakata_kai
* @brief バトルフェスデータ管理
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "Field/FieldStatic/include/BattleFes/BattleFes.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_FsArcFile.h>
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/BattleFesSave.h"
#include "Savedata/include/EventWork.h"
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"

#include "Field/FieldStatic/include/Event/EventBattleCall.h"
#include "Savedata/include/MyStatus.h"

#include <niji_conv_header/trainer/trtype_group_def.h>
#include <niji_conv_header/field/chara/chara_model_id.h>

#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"
// arc
#include <arc_index/battle_tree_trainer.gaix>
#include <arc_index/message.gaix>
#include <arc_def_index/arc_def.h>

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesModelManager.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/FieldGimmickBattleFes.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"
#include <Battle/Background/include/BgSetupUtil.h>

// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/BattleFes/BattleFesDebug.h"
#endif

GFL_NAMESPACE_BEGIN(BattleFes)

// 1体のバトルツリートレーナーリソースヒープサイズ
// 敵トレーナーのバトルツリーリソースに関しては、最大使用量でLocalHeapを作成し、リソース読み込み先とする。
// R:\home\momiji\resource\trainer\trtree\convert\tmpのデータサイズがリソースのサイズとなっている。
// 256B＋ファイルアクセス用ヒープサイズがあれば全てのトレーナーのバトルツリーリソースを読み込むことが出来る（現時点）ツリートレーナーの抽選ポケモン数が増えるようなことがあれば再度確認する必要がある。
static const u32 BFES_TRAINER_RESOURCE_HEAP_SIZE = 0x00400;

//----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 *
 *  @param  p_gman    マネージャポインタ
 *  @param p_heap     ヒープ
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
BattleFes::BattleFes(GameSys::GameManager *pGman, gfl2::heap::HeapBase* pHeap)
  : m_pGm(pGman)
  , m_Data()
  , m_pTrainerNameMsgData(NULL)
  , m_isEvent(false)
{
  for (u32 index(0); index < BattleFesDefine::FIGHT_TRAINER_MAX; ++index)
  {
    m_Data.TrainerParam[index].trID = 0;
    m_Data.TrainerParam[index].pRomData = NULL;
    m_Data.TrainerParam[index].pPokeParty = GFL_NEW(pHeap) pml::PokeParty(pHeap);
    m_Data.TrainerParam[index].pLocalHeapForTrainerResource = GFL_CREATE_LOCAL_HEAP_NAME(pHeap, BFES_TRAINER_RESOURCE_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "BFES_TRAINER_RESOURCE_HEAP");
  }

  Field::EventWork* ev = pGman->GetGameData()->GetEventWork();
  m_isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);

  // 同期オープン
  app::util::FileAccessor::FileOpenSync(ARCID_BATTLE_TREE_TRAINER, pHeap);

  if (m_isEvent)
  {
    createEventTrainer();
  }
  else
  {
    createTrainer();
  }

  for (u32 index(0); index < BattleFesDefine::BFES_MODEL_DISP_MAX; ++index)
  {
    ClearModelData(index);
    m_ModelData[index].pMyStatus = GFL_NEW(pHeap) Savedata::MyStatus();
  }

  initTrainerMsgData(pHeap);

  // リクエスト終了処理
  allWaitCreatedTrainer();

  app::util::FileAccessor::FileCloseSync(ARCID_BATTLE_TREE_TRAINER);
}

//----------------------------------------------------------------------------
/**
 *  @brief デストラクタ
 *
 */
//-----------------------------------------------------------------------------
BattleFes::~BattleFes()
{ 
  finalizeTrainerMsgData();

  for (u32 index(0); index < BattleFesDefine::BFES_MODEL_DISP_MAX; ++index)
  {
    ClearModelData(index);
    GFL_SAFE_DELETE(m_ModelData[index].pMyStatus);
  }

  for (u32 index(0); index < BattleFesDefine::FIGHT_TRAINER_MAX; ++index)
  {
    if (m_Data.TrainerParam[index].pRomData != NULL)
    {
      GflHeapFreeMemory(m_Data.TrainerParam[index].pRomData);
    }
    GFL_SAFE_DELETE(m_Data.TrainerParam[index].pPokeParty);
    GFL_DELETE_HEAP(m_Data.TrainerParam[index].pLocalHeapForTrainerResource);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief 敵トレーナーのポケモンパーティ取得
*
*/
//-----------------------------------------------------------------------------
pml::PokeParty* BattleFes::GetEnemyTrainerPokeParty(const u32 tr_count) const
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);
  return m_Data.TrainerParam[tr_count].pPokeParty;
}

//----------------------------------------------------------------------------
/**
*  @brief  対戦トレーナーをグリッドに追加
*
*  @param  pos_index  モデルのインデックス
*  @param  char_id
*  @param  x                                 Ｘ座標
*  @param  y                                 Ｙ座標
*  @param  z                                 Ｚ座標
*
*  @return none
*/
//--------------------------------------------------------------------------
void BattleFes::AddTrainerObj(const Field::BATTLE_FES_POS_INDEX pos_index, const int char_id, const float x, const float y, const float z)
{
  GFL_ASSERT(pos_index == Field::BATTLE_FES_POS_INDEX_ENEMY);
  Field::Fieldmap* p_fieldmap = m_pGm->GetFieldmap();
  Field::FieldGimmickBattleFes* pGimmick = static_cast<Field::FieldGimmickBattleFes*>(p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));
  GFL_ASSERT(pGimmick != NULL);
  BattleFesModelManager* pModelManager = pGimmick->GetModelManager();
  GFL_ASSERT(pModelManager != NULL);
  
  m_ModelData[pos_index].pos = gfl2::math::Vector3(x, y, z);
  m_ModelData[pos_index].rot.RadianYToQuaternion(0);
  m_ModelData[pos_index].charaID = char_id;
  m_ModelData[pos_index].index = pos_index;
  m_ModelData[pos_index].isDressup = false;
  pModelManager->Request(&m_ModelData[pos_index]);
}

//----------------------------------------------------------------------------
/**
*  @brief  NPCを削除申請
*
*  @return none
*/
//--------------------------------------------------------------------------
void BattleFes::DeleteObj()
{
  Field::Fieldmap* p_fieldmap = m_pGm->GetFieldmap();
  Field::FieldGimmickBattleFes* pGimmick = static_cast<Field::FieldGimmickBattleFes*>(p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));
  GFL_ASSERT(pGimmick != NULL);
  BattleFesModelManager* pModelManager = pGimmick->GetModelManager();
  GFL_ASSERT(pModelManager != NULL);
  pModelManager->RequestDelete(Field::BATTLE_FES_POS_INDEX_ENEMY);
  pModelManager->Delete();
}

//----------------------------------------------------------------------------
/**
*  @brief  NPCを削除待ち
*
*  @return bool
*/
//--------------------------------------------------------------------------
bool BattleFes::WaitModelDelete()
{
  Field::Fieldmap* p_fieldmap = m_pGm->GetFieldmap();
  Field::FieldGimmickBattleFes* pGimmick = static_cast<Field::FieldGimmickBattleFes*>(p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));
  GFL_ASSERT(pGimmick != NULL);
  BattleFesModelManager* pModelManager = pGimmick->GetModelManager();
  GFL_ASSERT(pModelManager != NULL);
  return pModelManager->Delete();
}

//----------------------------------------------------------------------------
/**
*  @brief  パートナートレーナーをグリッドに追加
*
*  @param  pos_index  モデルのインデックス
*  @param  x                                 Ｘ座標
*  @param  y                                 Ｙ座標
*  @param  z                                 Ｚ座標
*
*  @return none
*/
//--------------------------------------------------------------------------
void BattleFes::AddDressupTrainerObj(const Field::BATTLE_FES_POS_INDEX pos_index, const float x, const float y, const float z)
{
  GFL_ASSERT(pos_index < Field::BATTLE_FES_POS_INDEX_ENEMY);
  Field::Fieldmap* p_fieldmap = m_pGm->GetFieldmap();
  Field::FieldGimmickBattleFes* pGimmick = static_cast<Field::FieldGimmickBattleFes*>(p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));
  GFL_ASSERT(pGimmick != NULL);
  BattleFesModelManager* pModelManager = pGimmick->GetModelManager();
  GFL_ASSERT(pModelManager != NULL);
  // ModelData作成
  m_ModelData[pos_index].pos = gfl2::math::Vector3(x, y, z);
  m_ModelData[pos_index].rot.RadianYToQuaternion(0);
  m_ModelData[pos_index].charaID = 0;
  // MyStatus作成
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesSave(m_isEvent);
  GFL_ASSERT(m_ModelData[pos_index].pMyStatus != NULL);
  m_ModelData[pos_index].pMyStatus->SetIcon(*pBattleFesSave->GetPartnerIcon(pos_index));
  m_ModelData[pos_index].pMyStatus->SetBallThrowType(pBattleFesSave->GetPartnerBallThrowType(pos_index));
  m_ModelData[pos_index].pMyStatus->SetMegaRing((pBattleFesSave->GetPartnerMegaRing(pos_index) != 0));
  m_ModelData[pos_index].pMyStatus->SetZenryokuRing((pBattleFesSave->GetPartnerZenryokuRing(pos_index) != 0));
  m_ModelData[pos_index].pMyStatus->SetSex(pBattleFesSave->GetPartnerSex(pos_index));
  gfl2::str::STRCODE name[System::STRLEN_PLAYER_NAME + System::EOM_LEN];
  pBattleFesSave->GetPartnerName(pos_index, name);
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  gfl2::str::StrBuf* pName = GFL_NEW_LOW(p_heap) gfl2::str::StrBuf(name, p_heap->GetLowerHandle());
  m_ModelData[pos_index].pMyStatus->SetNameString(pName);
  // MyStatus作成完了
  m_ModelData[pos_index].isDressup = true;
  m_ModelData[pos_index].index = pos_index;
  // ModelData作成完了
  pModelManager->Request(&m_ModelData[pos_index]);
  GFL_SAFE_DELETE(pName);
}

//----------------------------------------------------------------------------
/**
*  @brief  モデル読み込み待ち
*
*  @return trueで完了
*/
//--------------------------------------------------------------------------
bool BattleFes::WaitModelLoad()
{
  Field::Fieldmap* p_fieldmap = m_pGm->GetFieldmap();
  Field::FieldGimmickBattleFes* pGimmick = static_cast<Field::FieldGimmickBattleFes*>(p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));
  BattleFesModelManager* pModelManager = pGimmick->GetModelManager();
  GFL_ASSERT(pModelManager != NULL);
  return pModelManager->IsRequestEnd(false);
}

//----------------------------------------------------------------------------
/**
*  @brief  モデル情報の破棄
*/
//--------------------------------------------------------------------------
void BattleFes::ClearModelData(const u32 index)
{
  m_ModelData[index].isUsing = false;
  m_ModelData[index].isDressup = false;
  m_ModelData[index].pos.x = 0;
  m_ModelData[index].pos.y = 0;
  m_ModelData[index].pos.z = 0;

  m_ModelData[index].rot.RadianYToQuaternion(0);
  m_ModelData[index].charaID = 0;
  m_ModelData[index].index = static_cast<Field::BATTLE_FES_POS_INDEX>(index);
}

//----------------------------------------------------------------------------
/**
*  @brief  トレーナーキャラクターID取得
*
*  @param  u16 tr_count トレーナーナンバー
*
*  @return キャラクターID
*/
//----------------------------------------------------------------------------- 
u16 BattleFes::GetCharacterID(const u16 tr_count)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);
  GFL_ASSERT(WaitCreatedTrainer(tr_count));
  u32 charaID;
  trainer::TrainerTypeData trData;
  trainer::TrType tr_type = static_cast<trainer::TrType>(m_Data.TrainerParam[tr_count].pRomData->tr_type);

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  trData.LoadData(p_heap->GetLowerHandle(), tr_type);
  charaID = trData.GetFieldMainModelID();

  MUNAKATA_PRINT("指定tr_type %d 取得ＯＢＪコード %d\n", tr_type, charaID);

  return charaID;
}

//----------------------------------------------------------------------
/*
*  @brief  対戦相手のトレーナータイプ名を登録
*/
//----------------------------------------------------------------------
void BattleFes::WordSetTrainerTypeNameFromTrainerNo(const u16 buf_id, const u16 tr_count)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);
  GFL_ASSERT(WaitCreatedTrainer(tr_count));
  print::WordSet* pWordSet = m_pGm->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  pWordSet->RegisterTrTypeName(buf_id, m_Data.TrainerParam[tr_count].pRomData->tr_type);
}

//----------------------------------------------------------------------
/*
*  @brief  対戦相手のトレーナー名を登録
*/
//----------------------------------------------------------------------
void BattleFes::WordSetTrainerNameFromTrainerNo(const u16 buf_id, const u16 tr_count)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);
  GFL_ASSERT(WaitCreatedTrainer(tr_count));
  print::WordSet* pWordSet = m_pGm->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  pWordSet->RegisterWord(buf_id, m_pTrainerNameMsgData, m_Data.TrainerParam[tr_count].trID);
}

//----------------------------------------------------------------------------
/**
*  @brief  バトル開始
*
*  @return none
*/
//-----------------------------------------------------------------------------
void BattleFes::StartBattle()
{
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  BATTLE_SETUP_PARAM *p_setup_param = GFL_NEW(p_heap) BATTLE_SETUP_PARAM();
  m_pGm->GetGameData()->GetBattleFesManager()->SetSetupParam(p_setup_param);
  pml::PokeParty     *p_my_party = GFL_NEW_LOW(p_heap) pml::PokeParty(p_heap->GetLowerHandle());

  //自分のポケパーティを作成する
  pml::pokepara::CoreParam* p_rentalpokemon = GFL_NEW_LOW(p_heap) pml::pokepara::CoreParam(p_heap->GetLowerHandle());
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesSave(m_isEvent);
  pBattleFesSave->GetRentalPokemonData(p_rentalpokemon);
  pml::pokepara::PokemonParam* p_pokemon = GFL_NEW_LOW(p_heap) pml::pokepara::PokemonParam(p_heap->GetLowerHandle(), *p_rentalpokemon);
  p_my_party->AddMember(*p_pokemon);
  GFL_SAFE_DELETE(p_pokemon);

  for (u32 index(0); index < BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    // 仲間のポケモン取得
    pBattleFesSave->GetPartnerPokemonData(index, p_rentalpokemon);
    p_pokemon = GFL_NEW_LOW(p_heap) pml::pokepara::PokemonParam(p_heap->GetLowerHandle(), *p_rentalpokemon);
    // 仲間のポケモンをパーティ登録
    p_my_party->AddMember(*p_pokemon);
    GFL_SAFE_DELETE(p_pokemon);
  }

  GFL_SAFE_DELETE(p_rentalpokemon);

  BTL_FIELD_SITUATION sit;
  // @fix アトリビュート指定をしないと予期せぬ大きさの背景（フィールドに依存）を読み込もうとして落ちるので指定する
  //構造体初期化
  BATTLE_SETUP_FIELD_SITUATION_Init(&sit);
  //バトル背景パラメータセット
  btl::GetBgComponentData(btl::bg::FAR_TYPE_Z_F0601JF0103, btl::bg::NEAR_TYPE_FES, &(sit.bgComponent));

  // ※読み込み速度が遅いならEventBattleFesBattleCallに移す
  u32 win_count = pBattleFesSave->GetWinCount();
  createTrainerPokemon(p_heap, win_count);
  // ※ここでp_setup_paramが初期化されるので注意
  BTL_SETUP_BattleFesTrainer(p_setup_param, m_pGm, &sit, p_my_party, p_heap);

  u8 rank = pBattleFesSave->GetRank();
  if (rank >= BattleFesDefine::BATTLE_FES_FIELD_LIMIT_RANK)
  {
    // バトルフェス環境設定
    p_setup_param->btlFesFieldType = BattleFesTool::GetBtlFieldType(rank);
  }

  GFL_SAFE_DELETE(p_my_party);

  // @fix GFNMcat[1884] バトルツリー、バトルロイヤルではお手入れ禁止
  BATTLE_PARAM_SetBtlStatusFlag(p_setup_param, BTL_STATUS_FLAG_OTEIRE_DISABLE);
  // バトルフェスフラグ設定
  BATTLE_PARAM_SetBtlStatusFlag(p_setup_param, BTL_STATUS_FLAG_BATTLE_FES);

  // 主人公の黒サングラスデータ登録（SavedataのMyStatusを参照させないため、MyStatusをここで生成）
  Savedata::MyStatus* pPlayerStatus = p_setup_param->playerStatus[BTL_CLIENT_PLAYER];
  p_setup_param->playerStatus[BTL_CLIENT_PLAYER] = GFL_NEW(p_heap) Savedata::MyStatus();
  p_setup_param->playerStatus[BTL_CLIENT_PLAYER]->CopyFrom(*pPlayerStatus);
  poke_3d::model::DressUpParam dressUpParam = p_setup_param->playerStatus[BTL_CLIENT_PLAYER]->GetDressup();
  BattleFesTool::GetSunglassDressUpParam(&dressUpParam);
  p_setup_param->playerStatus[BTL_CLIENT_PLAYER]->SetDressup(dressUpParam);

  // 仲間登録
  p_setup_param->playerStatus[BTL_CLIENT_PARTNER] = GFL_NEW(p_heap) Savedata::MyStatus();
  p_setup_param->playerStatus[BTL_CLIENT_PARTNER]->CopyFrom(*m_ModelData[Field::BATTLE_FES_POS_INDEX_PARTNER1].pMyStatus);
  p_setup_param->playerStatus[BTL_CLIENT_ENEMY2] = GFL_NEW(p_heap) Savedata::MyStatus();
  p_setup_param->playerStatus[BTL_CLIENT_ENEMY2]->CopyFrom(*m_ModelData[Field::BATTLE_FES_POS_INDEX_PARTNER2].pMyStatus);

  //勝利時メッセージ
  p_setup_param->tr_data[0] = GFL_NEW(p_heap)BSP_TRAINER_DATA(p_heap);
  p_setup_param->tr_data[0]->SetMsgID(0, GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData()->GetPattern(JoinFestaScript::PATTERN_EMOTION), 0);
  p_setup_param->tr_data[2] = GFL_NEW(p_heap)BSP_TRAINER_DATA(p_heap);
  p_setup_param->tr_data[2]->SetMsgID(0, pBattleFesSave->GetPartnerEmotion(0), 0);
  p_setup_param->tr_data[3] = GFL_NEW(p_heap)BSP_TRAINER_DATA(p_heap);
  p_setup_param->tr_data[3]->SetMsgID(0, pBattleFesSave->GetPartnerEmotion(1), 0);

  // BGM設定
  switch (pBattleFesSave->GetTrainerData(win_count).trID)
  {
  case BattleFesDefine::TR_ID_MAMANE:
    p_setup_param->btlEffData.bgm_default = STRM_BGM_VS_TRAINER_NJ;
    break;
  case BattleFesDefine::TR_ID_SAKAKI:
    p_setup_param->btlEffData.bgm_default = STRM_BGM_MJ_VS02;
    break;
  case BattleFesDefine::TR_ID_SITAPPA:
    p_setup_param->btlEffData.bgm_default = STRM_BGM_MJ_VS01;
    break;
  }

  // バトルエージェントのバトルパラメータチェック
  // 自分のポケモンと敵のポケモンのレベルは同じ
#if PM_DEBUG
  // @fix MMCat Contact[151] 【バトルエージェント】デバッグモード使用時の不具合へのご対応要望
  // 敵ポケモンレベル固定ならば無視
  if (!BattleFesDebug::GetFixedLevelFlg())
#endif
  {
    GFL_ASSERT(p_setup_param->party[BTL_CLIENT_PLAYER]->GetMemberPointer(0)->GetLevel() == p_setup_param->party[BTL_CLIENT_ENEMY1]->GetMemberPointer(0)->GetLevel());
  }
  // 3 vs 3　である
  GFL_ASSERT(p_setup_param->party[BTL_CLIENT_PLAYER]->GetMemberCount() == p_setup_param->party[BTL_CLIENT_ENEMY1]->GetMemberCount());
  GFL_ASSERT(p_setup_param->party[BTL_CLIENT_PLAYER]->GetMemberCount() == BattleFesDefine::POKEMON_MAX);
  // シングルバトルである
  GFL_ASSERT(p_setup_param->rule == BTL_RULE_SINGLE);
  // 施設バトルである
  GFL_ASSERT(p_setup_param->competitor == BTL_COMPETITOR_INST);

  // バトル施設イベント呼び出し
  Field::EventBattleFesBattleCall* p_event =
    reinterpret_cast<Field::EventBattleFesBattleCall*>(GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventBattleFesBattleCall>(m_pGm->GetGameEventManager()));
  p_event->SetUpBattleParam(p_setup_param);
}

//----------------------------------------------------------------------------
/**
*  @brief バトルに勝ったか？
*
*  @return　bool trueで勝利
*/
//-----------------------------------------------------------------------------
bool BattleFes::IsWin()
{
  BATTLE_SETUP_PARAM *p_setup_param = m_pGm->GetGameData()->GetBattleFesManager()->GetSetupParam();
  GFL_ASSERT(p_setup_param != NULL);
  if (p_setup_param->result == BTL_RESULT_WIN)
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  バトル終了
*
*  @return none
*/
//-----------------------------------------------------------------------------
void BattleFes::EndBattle()
{
  BATTLE_SETUP_PARAM *p_setup_param = m_pGm->GetGameData()->GetBattleFesManager()->GetSetupParam();
  BATTLE_SETUP_Clear(p_setup_param);
  // BATTLE_SETUP_ClearではBTL_CLIENT_PLAYERの破棄は行わないため
  GFL_SAFE_DELETE(p_setup_param->playerStatus[BTL_CLIENT_PLAYER]);
  GFL_SAFE_DELETE(p_setup_param);
  m_pGm->GetGameData()->GetBattleFesManager()->SetSetupParam(NULL);
}

//----------------------------------------------------------------------------
/**
*  @brief  戦うトレーナーをバトルセットアップパラメータにセットする
*
*  @param  dst             格納先トレーナーデータポインタ
*  @param  pHeap           ヒープ
*
*  @return none
*/
//-----------------------------------------------------------------------------
void BattleFes::SetVsTrainer(BSP_TRAINER_DATA* dst, gfl2::heap::HeapBase* pHeap)
{
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesSave(m_isEvent);
  u32 win_count = pBattleFesSave->GetWinCount();
  u32 tr_id = m_Data.TrainerParam[win_count].trID;
  dst->SetTrainerID(tr_id);
  dst->LoadTrTypeData(pHeap, static_cast<trainer::TrType>(m_Data.TrainerParam[win_count].pRomData->tr_type));
  // @fix GFNMCat[1155] 敵トレーナーの名前にパートナー名がセットされていた問題を修正
  dst->SetName(m_pTrainerNameMsgData, tr_id);
  // AIセット
  setAiBit(BattleFes::AI_BIT, pBattleFesSave->GetRank(), pBattleFesSave->IsBoss(win_count), dst);
}

//--------------------------------------------------------------
/**
* @brief 対戦ポケモンセット
* @param party     ポケパーティ
*/
//--------------------------------------------------------------
void BattleFes::SetVsPokemon(pml::PokeParty* party)
{
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesSave(m_isEvent);
  u32 win_count = pBattleFesSave->GetWinCount();
  //パーティにセット
  for(u32 index(0); index < BattleFesDefine::POKEMON_MAX; ++index)
  {
    party->AddMember(*m_Data.TrainerParam[win_count].pPokeParty->GetMemberPointer(index));
  }
}

//--------------------------------------------------------------
/**
* @brief トレーナーデータ読み込み待ち
* @param tr_count     トレーナーカウント
*/
//-------------------------------------------------------------- 
bool BattleFes::WaitCreatedTrainer(const u32 tr_count)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);
  // トレーナーデータ読み込み完了待ち
  return app::util::FileAccessor::IsFileLoad(reinterpret_cast<void**>(&m_Data.TrainerParam[tr_count].pRomData));
}

#if PM_DEBUG
//----------------------------------------------------------------------------
/**
*  @brief ５人の敵トレーナーデータ＆ポケモンデータ作成
*
*/
//-----------------------------------------------------------------------------
void BattleFes::DebugCreateEnemy(gfl2::heap::HeapBase* pHeap)
{
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetSaveData()->GetBattleFesSave();
  // トレーナーセーブデータがあるならトレーナーデータ復元
  if (pBattleFesSave->GetTrainerData(0).trID < GARC_battle_tree_trainer_DATA_NUM)
  {
    for (u32 index(0); index < BattleFesDefine::FIGHT_TRAINER_MAX; ++index)
    {
      TRAINER_DATA trainer = pBattleFesSave->GetTrainerData(index);
      debugMakeTrainerData(pHeap, index, trainer.trID);
    }
  }
  else
  {
    // トレーナーデータ作成
    TRAINER_DATA trainer[BattleFesDefine::FIGHT_TRAINER_MAX];
    for (u32 index(0); index < BattleFesDefine::FIGHT_TRAINER_MAX; ++index)
    {
      u32 tr_id = 0;
      // ボスチェック
      if (pBattleFesSave->IsBoss(index))
      {
        tr_id = selectBoss();
        // ※念のため、ここには来ない設計
        if (tr_id == 0)
        {
          GFL_ASSERT(0);
          tr_id = selectRandamTrainer(index);
        }
      }
      else
      {
        tr_id = selectRandamTrainer(index);
      }

      debugMakeTrainerData(pHeap, index, tr_id);

      // トレーナーデータ保存
      trainer[index].trID = tr_id;
      for (u32 poke_count(0); poke_count < BattleFesDefine::POKEMON_MAX; ++poke_count)
      {
        trainer[index].battleTreeData[poke_count].pokeID = BattleFesDefine::BATTLE_TREE_ID_MAX;
        trainer[index].battleTreeData[poke_count].Sex = 0;
        trainer[index].battleTreeData[poke_count].Tokusei = 0;
      }
      pBattleFesSave->SetTrainerData(index, trainer[index]);
    }
  }
}
#endif

// 非公開関数
//--------------------------------------------------------------
/**
* トレーナー抽選
* @param   start   開始インデックス
* @param   width   抽選幅
*
* @return  抽選結果トレーナーナンバー
*
*/
//--------------------------------------------------------------
u32 BattleFes::selectTrainerCore(const u32 tr_count, const u32 start, const u32 width)
{
  u32 no = 0;
  int rand_count = 0;
  static const int RAND_COUNT_MAX = 100;
  //RAND_COUNT_MAX回トライアル
  for (rand_count = 0; rand_count < RAND_COUNT_MAX; ++rand_count)
  {
    bool is_same_no = false;
    no = start + System::GflUse::GetPublicRand( width );
    for (u32 index(0); index < tr_count; ++index)
    {
      if (no == m_Data.TrainerParam[index].trID)
      {
        // 被ったのでやり直し
        is_same_no = true;
        break;
      }
    }

    if (!is_same_no)
    {
      // 選別終了
      break;
    }
  }

  //RAND_COUNT_MAX回トライアルしても重複した場合
  if (rand_count >= RAND_COUNT_MAX)
  {
    no = 0;
    for (u32 index(0); index < tr_count; ++index)
    {
      no += m_Data.TrainerParam[index].trID;
    }
    no = (no % width) + start;

    for (rand_count = 0; rand_count < RAND_COUNT_MAX; ++rand_count)
    {
      bool is_same_no = false;
      for (u32 index(0); index < tr_count; ++index)
      {
        if (no == m_Data.TrainerParam[index].trID)
        {
          // 被ったのでやり直し
          is_same_no = true;
          break;
        }
      }
      if (!is_same_no)
      {
        // 選別終了
        break;
      }
      ++no;
      no = (no % width) + start;
    }
    //絶対来ない設計
    if (rand_count >= RAND_COUNT_MAX)
    {
      GFL_ASSERT_MSG(0, "%d回トライアルしても重複した！\n", RAND_COUNT_MAX);
    }
  }
  return no;
}

//----------------------------------------------------------------------------
/**
*  @brief ランクに応じてトレーナーＩＤを決定
*  @return トレーナーID
*/
//-----------------------------------------------------------------------------
u32 BattleFes::selectRandamTrainer(const u32 tr_count)
{
  u32 rank = m_pGm->GetGameData()->GetSaveData()->GetBattleFesSave()->GetRank();
  u32 no = 0;
  if (rank < 10) no = selectTrainerCore(tr_count, 0, 50);
  else if (rank < 20) no = selectTrainerCore(tr_count, 30, 40);
  else if (rank < 30) no = selectTrainerCore(tr_count, 50, 40);
  else if (rank < 40) no = selectTrainerCore(tr_count, 70, 40);
  else if (rank < 50) no = selectTrainerCore(tr_count, 90, 40);
  else no = selectTrainerCore(tr_count, 90, 100);

  // トレーナーデータが変更されたら修正必須
  const u32 TRAINER_DATA_MAX = 189;
  STATIC_ASSERT(GARC_battle_tree_trainer_DATA_NUM == 210);
  if (no > TRAINER_DATA_MAX)
  {
    GFL_ASSERT_MSG(0, "TrainerID Error ID = %d\n", no);
    no = gfl2::math::clamp(no, static_cast<u32>(GARC_battle_tree_trainer_bttd_000_BIN), static_cast<u32>(TRAINER_DATA_MAX));
  }

  return no;
}

//--------------------------------------------------------------
/**
* ボスＩＤ取得
*/
//--------------------------------------------------------------
u32 BattleFes::selectBoss()
{
  // ２５戦目はボス（サカキかマーマネ）
  // ボスデータが変更されたら修正必須
  STATIC_ASSERT(GARC_battle_tree_trainer_DATA_NUM == 210);
  static u32 bossDataID[] = {
    BattleFesDefine::TR_ID_MAMANE, BattleFesDefine::TR_ID_SAKAKI
  };
  Field::EventWork* pEventWork = m_pGm->GetGameData()->GetEventWork();
  return bossDataID[static_cast<u32>(pEventWork->CheckEventFlag(SYS_FLAG_BFES_SAKAKI_OPEN))];
}

//----------------------------------------------------------------------------
/**
*  @brief  対戦相手の決定
*
*  @param  tr_count
*  @param  data_idx
*
*  @return none
*/
//-----------------------------------------------------------------------------
void BattleFes::makeTrainerData(const u32 tr_count, const u32 data_idx)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);

  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesSave(m_isEvent);

  //トレーナーデータセット
  m_Data.TrainerParam[tr_count].trID = data_idx;

  if (m_Data.TrainerParam[tr_count].pRomData != NULL)
  {
    GFL_ASSERT(0);
  }
  // 非同期読み込みリクエスト
  app::util::FileAccessor::FileLoad(ARCID_BATTLE_TREE_TRAINER, data_idx, reinterpret_cast<void**>(&m_Data.TrainerParam[tr_count].pRomData), m_Data.TrainerParam[tr_count].pLocalHeapForTrainerResource, false);
}

//----------------------------------------------------------------------------
/**
*  @brief  対戦ポケモン作成→ポケモンセット
*
*  @param gfl2::heap::HeapBase*        ヒープ
*  @param const u32       何番目のトレーナー
*  @param const u32      トレーナーＩＤ（dataID）
*  @param BATTLE_TREE_DATA* バトルツリーデータ
*
*  @return bool 50回抽選をした場合 true
*/
//-----------------------------------------------------------------------------
bool BattleFes::makeTrainerPokemon(gfl2::heap::HeapBase* pHeap, const u32 tr_count, BATTLE_TREE_DATA* pBattleTreeData)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);
  bool ret = false;

  int set_count = 0;
  int loop_count = 0;
  int entry_mons_no[BattleFesDefine::POKEMON_MAX] = { 0, 0, 0 };
  int entry_item_no[BattleFesDefine::POKEMON_MAX] = { 0, 0, 0 };

  const BattleInst::BINST_TRAINER_ROM_DATA *td = m_Data.TrainerParam[tr_count].pRomData;
  pml::PokeParty* p_pokeParty = m_Data.TrainerParam[tr_count].pPokeParty;
  gfl2::math::SFMTRandom       randSys; // 乱数
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesSave(m_isEvent);
  randSys.Initialize(pBattleFesSave->GetTrainerData(tr_count).pokeMakeSeed);
  while (set_count != BattleFesDefine::POKEMON_MAX)
  {
    int i;
    BattleInst::BINST_POKEMON_ROM_DATA  pd;
    int poke_index = randSys.Next(td->use_poke_cnt);
    int set_index = td->use_poke_table[poke_index];
    BattleInstTool::GetPokemonRomData(pHeap->GetLowerHandle(), m_pGm->GetAsyncFileManager(), ARCID_BATTLE_TREE_POKE, set_index, &pd);

    //モンスターナンバーのチェック（同一のポケモンは持たない）
    for (i = 0; i<set_count; i++)
    {
      GFL_ASSERT(pd.mons_no != 0);  //@checked
      if (entry_mons_no[i] == pd.mons_no)
      {
        break;      //抽選済み
      }
    }
    if (i != set_count)
    {
      continue;
    }

    //50回まわして、決まらないようなら、同一アイテムチェックはしない
    if (loop_count<50){
      //装備アイテムのチェック（同一のアイテムは持たない）
      for (i = 0; i<set_count; i++)
      {
        if ((pd.item_no) && (entry_item_no[i] == pd.item_no)){
          break; //アイテム重複
        }
      }
      if (i != set_count)
      {
        loop_count++;
        continue;
      }
    }

    entry_mons_no[set_count] = pd.mons_no;
    entry_item_no[set_count] = pd.item_no;

    //格納配列にセット
    {
      pml::pokepara::PokemonParam* pp;
      // ランク＋５０レベル
      u32 rank = pBattleFesSave->GetRank();
#if PM_DEBUG
      if (BattleFesDebug::GetFixedLevelFlg())
      {
        rank = 0;
      }
#endif
      pp = BattleInstTool::MakePokemonParam(&pd, BattleFesDefine::POKEMON_INIT_LEVEL + rank, BattleFesDefine::POWER, pHeap->GetLowerHandle());//特性, 性別指定しない

      // トレーナータイプデータから取得したボールIDをセットする
      item::BALL_ID ball_id = this->getBallIDByTrainerRomData(pHeap, td);
      pp->SetGetBall(ball_id);

      // バトルツリーポケモン生成に必要なパラメータ保存
      if (pBattleTreeData != NULL)
      {
        pBattleTreeData[set_count].pokeID = set_index;
        pBattleTreeData[set_count].Sex = static_cast<u8>(pp->GetSex());
        pBattleTreeData[set_count].Tokusei = pp->GetTokuseiIndex();
      }
      // ポケモンセット
      p_pokeParty->AddMember(*pp);
      GFL_SAFE_DELETE(pp);
    }
    set_count++;
  } //end while

  if (loop_count >= 50)
  {
    ret = true;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
*  @brief  対戦ポケモン作成（セーブデータからポケモン作成）
*  @param gfl2::heap::HeapBase*        ヒープ
*  @param const u32     何番目のトレーナー
*  @param const BATTLE_TREE_DATA*   バトルツリーデータ 3個分必須
*
*/
//-----------------------------------------------------------------------------
void BattleFes::makeTrainerPokemonFromSaveData(gfl2::heap::HeapBase* pHeap, const u32 tr_count, const BATTLE_TREE_DATA* pBattleTreeData)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);
  GFL_ASSERT(pBattleTreeData != NULL);
#if PM_DEBUG
  // デバッグ時のみ使用
  // 既にポケモンを所持している
  if (m_Data.TrainerParam[tr_count].pPokeParty->GetMemberCount() > 0)
  {
    // ポケモンデータ削除
    m_Data.TrainerParam[tr_count].pPokeParty->Clear();
  }
#endif

  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesSave(m_isEvent);

  for (u32 index(0); index < BattleFesDefine::POKEMON_MAX; ++index)
  {
    BattleInst::BINST_POKEMON_ROM_DATA  pd;
    BattleInstTool::GetPokemonRomData(pHeap->GetLowerHandle(), m_pGm->GetAsyncFileManager(), ARCID_BATTLE_TREE_POKE, pBattleTreeData[index].pokeID, &pd);
    pml::pokepara::PokemonParam* pp;
    // ランク＋５０レベル
    u32 rank = pBattleFesSave->GetRank();
#if PM_DEBUG
    if (BattleFesDebug::GetFixedLevelFlg())
    {
      rank = 0;
    }
#endif
    pp = BattleInstTool::MakePokemonParam(&pd, BattleFesDefine::POKEMON_INIT_LEVEL + rank, BattleFesDefine::POWER, pHeap->GetLowerHandle(), pBattleTreeData[index].Tokusei, pBattleTreeData[index].Sex);//特性, 性別指定

    // トレーナータイプデータから取得したボールIDをセットする
    item::BALL_ID ball_id = this->getBallIDByTrainerRomData(pHeap, m_Data.TrainerParam[tr_count].pRomData);
    pp->SetGetBall(ball_id);
    m_Data.TrainerParam[tr_count].pPokeParty->AddMember(*pp);
    GFL_SAFE_DELETE(pp);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief トレーナーロムデータから捕獲ボールIDを取得
*  @param pHeap 一時確保用ヒープ
*  @param td トレーナーデータ
*  @return 捕獲ボールID
*/
//-----------------------------------------------------------------------------
item::BALL_ID BattleFes::getBallIDByTrainerRomData(gfl2::heap::HeapBase* pHeap, const BattleInst::BINST_TRAINER_ROM_DATA* td)
{
  GFL_ASSERT(td != NULL);
  trainer::TrType tr_type = static_cast<trainer::TrType>(td->tr_type);

  trainer::TrainerTypeData trData;
  trData.LoadData(pHeap, tr_type);

  return trData.GetBallID();
}

//----------------------------------------------------------------------
/*
*  @brief  トレーナー名メッセージデータを作成
*/
//----------------------------------------------------------------------
void BattleFes::initTrainerMsgData(gfl2::heap::HeapBase* pHeap)
{
  GFL_ASSERT(m_pTrainerNameMsgData == NULL);

  m_pTrainerNameMsgData = GFL_NEW(pHeap) gfl2::str::MsgData(print::GetMessageArcId(),
    GARC_message_tower_trainer_name_DAT, pHeap,
    gfl2::str::MsgData::LOAD_FULL);
}

//----------------------------------------------------------------------
/*
*  @brief  トレーナー名メッセージデータを削除
*/
//----------------------------------------------------------------------
void BattleFes::finalizeTrainerMsgData()
{
  GFL_SAFE_DELETE(m_pTrainerNameMsgData);
}

//----------------------------------------------------------------------------
/**
*  @brief トレーナーデータ作成
*/
//-----------------------------------------------------------------------------
void BattleFes::createTrainer()
{
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetSaveData()->GetBattleFesSave();

  // トレーナーセーブデータがあるならトレーナーデータ復元
  if (pBattleFesSave->GetTrainerData(0).trID < GARC_battle_tree_trainer_DATA_NUM)
  {
    // リクエストを優先順位の高いものからやる
    u32 tr_count_array[BattleFesDefine::FIGHT_TRAINER_MAX];
    u32 win_count = pBattleFesSave->GetWinCount();
    for (u32 index(0); index < BattleFesDefine::FIGHT_TRAINER_MAX; ++index)
    {
      tr_count_array[index] = (win_count + index) % BattleFesDefine::FIGHT_TRAINER_MAX;
    }
    for (u32 index(0); index < BattleFesDefine::FIGHT_TRAINER_MAX; ++index)
    {
      TRAINER_DATA trainer = pBattleFesSave->GetTrainerData(tr_count_array[index]);
      makeTrainerData(tr_count_array[index], trainer.trID);
    }
  }
  else
  {
    // トレーナーデータ作成
    TRAINER_DATA trainer[BattleFesDefine::FIGHT_TRAINER_MAX];
    for (u32 index(0); index < BattleFesDefine::FIGHT_TRAINER_MAX; ++index)
    {
      u32 tr_id = 0;
      // ボスチェック
      if (pBattleFesSave->IsBoss(index))
      {
        tr_id = selectBoss();
        // ※念のため、ここには来ない設計
        if (tr_id == 0)
        {
          GFL_ASSERT(0);
          tr_id = selectRandamTrainer(index);
        }
      }
      else
      {
        tr_id = selectRandamTrainer(index);
      }

      makeTrainerData(index, tr_id);
      // トレーナーデータ保存
      trainer[index].trID = tr_id;
      // ポケモン選定固定化のためのシード値保存
      trainer[index].pokeMakeSeed = System::GflUse::GetPublicRand();
      for (u32 poke_count(0); poke_count < BattleFesDefine::POKEMON_MAX; ++poke_count)
      {
        trainer[index].battleTreeData[poke_count].pokeID = BattleFesDefine::BATTLE_TREE_ID_MAX;
        trainer[index].battleTreeData[poke_count].Sex = 0;
        trainer[index].battleTreeData[poke_count].Tokusei = 0;
      }
      pBattleFesSave->SetTrainerData(index, trainer[index]);
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief イベントトレーナーデータ作成
*/
//-----------------------------------------------------------------------------
void BattleFes::createEventTrainer()
{
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesManager()->GetBattleFesSave();

  // トレーナーセーブデータがあるならトレーナーデータ復元
  if (pBattleFesSave->GetTrainerData(0).trID < GARC_battle_tree_trainer_DATA_NUM)
  {
    TRAINER_DATA trainer = pBattleFesSave->GetTrainerData(0);
    makeTrainerData(0, trainer.trID);
  }
  else
  {
    // トレーナーデータ作成
    TRAINER_DATA trainer;
    // したっぱ
    u32 tr_id = BattleFesDefine::TR_ID_SITAPPA;
    makeTrainerData(0, tr_id);
    // トレーナーデータ保存
    trainer.trID = tr_id;
    // ポケモン選定固定化のためのシード値保存
    trainer.pokeMakeSeed = System::GflUse::GetPublicRand();
    for (u32 poke_count(0); poke_count < BattleFesDefine::POKEMON_MAX; ++poke_count)
    {
      trainer.battleTreeData[poke_count].pokeID = BattleFesDefine::BATTLE_TREE_ID_MAX;
      trainer.battleTreeData[poke_count].Sex = 0;
      trainer.battleTreeData[poke_count].Tokusei = 0;
    }
    pBattleFesSave->SetTrainerData(0, trainer);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief トレーナーポケモン作成
*  @param pHeap ヒープ
*  @param tr_count トレーナーカウント
*/
//-----------------------------------------------------------------------------
void BattleFes::createTrainerPokemon(gfl2::heap::HeapBase* pHeap, const u32 tr_count)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);
  GFL_ASSERT(WaitCreatedTrainer(tr_count));
  Savedata::BattleFesSave* pBattleFesSave = m_pGm->GetGameData()->GetBattleFesSave(m_isEvent);
  u32 winCount = pBattleFesSave->GetWinCount();

  // トレーナーポケモンセーブデータがあるなら復元
  if (pBattleFesSave->GetTrainerData(tr_count).battleTreeData[0].pokeID < BattleFesDefine::BATTLE_TREE_ID_MAX)
  {
    TRAINER_DATA trainer = pBattleFesSave->GetTrainerData(tr_count);
    makeTrainerPokemonFromSaveData(pHeap, tr_count, trainer.battleTreeData);
  }
  else
  {
    TRAINER_DATA trainer = pBattleFesSave->GetTrainerData(tr_count);
    makeTrainerPokemon(pHeap, tr_count, trainer.battleTreeData);
    // トレーナーポケモンデータ保存
    pBattleFesSave->SetTrainerData(tr_count, trainer);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief 全トレーナーデータ作成リクエスト終了処理
*/
//-----------------------------------------------------------------------------
void BattleFes::allWaitCreatedTrainer()
{
  bool loop_max_flg = false;
  const u32 loop_max = (m_isEvent) ? 1 : BattleFesDefine::FIGHT_TRAINER_MAX;
  for (u32 index(0); index < loop_max; ++index)
  {
    u32 loop_count = 0;
    const u32 LOOP_MAX = 500;
    while (loop_count < LOOP_MAX)
    {
      if (WaitCreatedTrainer(index))
      {
        break;
      }
      ++loop_count;
    }
    if (loop_count >= LOOP_MAX)
    {
      loop_max_flg = true;
    }
  }
  // 
  if (loop_max_flg)
  {
    GFL_ASSERT_MSG(0, "※リクエスト終了ができていないものがあります！\n");
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()->IsAllReqFinished();
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  ＡＩビットセット
*
*  @param  const int base_ai_bit ビット
*  @param  int rank
*  @param  bool is_boss
*  @param  dst                   格納先
*
*/
//-----------------------------------------------------------------------------
void BattleFes::setAiBit(const int base_ai_bit, const u32 rank, const bool is_boss, BSP_TRAINER_DATA* dst)
{
  int ai_bit = base_ai_bit;
  const int EASY_AI_RANK = 10;
  if (rank < EASY_AI_RANK && !is_boss)
  {
    ai_bit = BTL_AISCRIPT_BIT_WAZA_BASIC;
  }
  dst->SetAIBit(ai_bit);
}

#if PM_DEBUG
//----------------------------------------------------------------------------
/**
*  @brief  対戦相手の決定
*
*  @param  tr_count
*  @param  data_idx
*
*  @return none
*/
//-----------------------------------------------------------------------------
void BattleFes::debugMakeTrainerData(gfl2::heap::HeapBase* pHeap, const u32 tr_count, const u32 data_idx)
{
  GFL_ASSERT(tr_count < BattleFesDefine::FIGHT_TRAINER_MAX);

  // デバッグ時のみ使用
  if (m_Data.TrainerParam[tr_count].trID == data_idx)
  {
    // 同じデータなので再生成不要
    return;
  }

  //トレーナーデータセット
  m_Data.TrainerParam[tr_count].trID = data_idx;

  size_t size = BattleInstTool::GetTrainerRomDataSize(pHeap, m_pGm->GetAsyncFileManager(), ARCID_BATTLE_TREE_TRAINER, data_idx);

  if (size > 0)
  {
    if (m_Data.TrainerParam[tr_count].pRomData != NULL)
    {
      GflHeapFreeMemory(m_Data.TrainerParam[tr_count].pRomData);
      m_Data.TrainerParam[tr_count].pRomData = NULL;
    }
    s32 size_align = 4;
    m_Data.TrainerParam[tr_count].pRomData = static_cast<BattleInst::BINST_TRAINER_ROM_DATA*>(GflHeapAllocMemoryLowAlign(m_Data.TrainerParam[tr_count].pLocalHeapForTrainerResource, size, size_align));
    // 同期読み込みリクエスト
    BattleInstTool::GetTrainerRomData(pHeap, m_pGm->GetAsyncFileManager(), ARCID_BATTLE_TREE_TRAINER, data_idx, m_Data.TrainerParam[tr_count].pRomData, size);
  }
  else
  {
    GFL_ASSERT_MSG(0, "data size <= 0\n");
  }
}
#endif

GFL_NAMESPACE_END( BattleFes )

