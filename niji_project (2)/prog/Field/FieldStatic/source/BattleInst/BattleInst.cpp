//======================================================================
/**
 * @file BgattleInst.cpp
 * @date 2015/01/12
 * @author saita_kazuki
 * @brief バトル施設 (sangoから移植)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldStatic/include/BattleInst/BattleInst.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_FsArcFile.h>
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Field/FieldStatic/include/Event/EventBattleCall.h"
#include "Field/FieldStatic/include/Event/EventProcessCall.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"

#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include "Savedata/include/BattleInstSave.h"
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/Record.h"
#include "Savedata/include/BoxSave.h"
#include "Savedata/include/BoxPokemonSave.h"

#include "PokeTool/include/PokeToolRibbon.h"

#include <niji_conv_header/trainer/trtype_group_def.h>
#include <niji_conv_header/field/chara/chara_model_id.h>

#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "System/include/HomeSleepAndPowerSystem.h"

#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"

#include <arc_index/message.gaix>
#include <arc_def_index/arc_def.h>


#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRos/FieldGimmickBattleInst/include/BattleInst/BattleInstModelManager.h"
#include "Field/FieldRos/FieldGimmickBattleInst/include/FieldGimmickBattleInst.h"


#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptWork.h"

#include "savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// ロトムお題用
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"

GFL_NAMESPACE_BEGIN( BattleInst )

//----------------------------------------------------------------------------
/**
 *  @brief  ＡＩビットセット
 *
 *  @param  const int base_ai_bit ビット
 *  @param  int btl_type          バトルタイプ
 *  @param  dst                   格納先
 *
 */
//-----------------------------------------------------------------------------
static void SetAiBit( const int base_ai_bit, const int btl_type, BSP_TRAINER_DATA* dst)
{
  int ai_bit = base_ai_bit;
  if ( (btl_type == Regulation::BATTLE_RULE_DOUBLE) || (btl_type == Regulation::BATTLE_RULE_MULTI) )
  {
    ai_bit |= BTL_AISCRIPT_BIT_WAZA_DOUBLE;
  }
  dst->SetAIBit( ai_bit );
}

//--------------------------------------------------------------
/**
 * バトル施設トレーナーの持ちポケモンのパワー乱数を決定する ノーマルランク
 * @param  tr_no  トレーナーナンバー
 * @return  パワー乱数
 */
//--------------------------------------------------------------
static u8 GetPowerRndNormal( u16 tr_no )
{
  u8  pow_rnd;
  if(tr_no<50){
    pow_rnd=(0x1f*5)/8;
  }
  else if(tr_no<70){
    pow_rnd=(0x1f*6)/8;
  }
  else if(tr_no<90){
    pow_rnd=(0x1f*7)/8;
  }
  else if(tr_no<110){
    pow_rnd=(0x1f*8)/8;
  }
  else if(tr_no<130){
    pow_rnd=(0x1f*8)/8;
  }
  else if(tr_no<190){
    pow_rnd=(0x1f*8)/8;
  }
  else{
    // ボス
    pow_rnd=(0x1f*8)/8;
  }
  return pow_rnd;
}


//--------------------------------------------------------------
/**
 * ボス戦かどうかチェック
 */
//--------------------------------------------------------------
static bool CheckIsBoss( int winCount )
{
  // @fix GFNMCat[1440] スーパー50戦目のボス判定時時の勝利数計算に誤りがあった問題を修正
  int bossIndex = (winCount + 1) % 10;
  if( bossIndex == 0 )
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------
/**
 * バトル施設トレーナーの持ちポケモンのパワー乱数を決定する スーパーランク
 * @param  tr_no  トレーナーナンバー
 * @return  パワー乱数
 * //@fix BTS[4027] //条件判定変更
 */
//--------------------------------------------------------------
static u8 GetPowerRndSuper( u16 tr_no )
{
  u8  pow_rnd;
  if(tr_no<50){
    pow_rnd=(0x1f*5)/8;
  }
  else if(tr_no<70){
    pow_rnd=(0x1f*6)/8;
  }
  else if(tr_no<90){
    pow_rnd=(0x1f*7)/8;
  }
  else{
    pow_rnd=0x1f;
  }
  return pow_rnd;
}

//--------------------------------------------------------------
/**
 * バトル施設トレーナーの持ちポケモンのパワー乱数を決定する
 * @param  tr_no  トレーナーナンバー
 * @param  super  スーパーランクか？
 * @return  パワー乱数
 */
//--------------------------------------------------------------
static u8 GetPowerRnd( u16 tr_no )
{
  return GetPowerRndNormal( tr_no );
}

//----------------------------------------------------------------------------
/**
 *  @brief 生成
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::Create()
{
  for ( int i = 0; i < BattleInstDefine::BINST_MODEL_DISP_MAX ;i++)
  {
    mModelData[i].isUsing = false;
    mModelData[i].index = static_cast<Field::BATTLE_INST_POS_INDEX>(i);
  }

  InitTrainerMsgData();
}

//----------------------------------------------------------------------------
/**
 *  @brief 削除
 *
 *  @param  GameSys::GameManager *p_gman 
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::Delete()
{
  FinalizeTrainerMsgData();
}

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
BattleInst::BattleInst( GameSys::GameManager *p_gman, gfl2::heap::HeapBase* p_heap )
  : mData( p_heap)
{
  mp_heap = p_heap;
  mp_heap_tmp = p_heap->GetLowerHandle();
  mp_gman = p_gman;
  mData.TrainerParam.romData[0] = NULL;
  mData.TrainerParam.romData[1] = NULL;
  mData.TrainerParam.rule = BTL_RULE_SINGLE;
  mData.SuperRank = false;

  gfl2::std::MemClear( &mData.EntryPokeData, sizeof(EntryPokeData));

  mData.PartnerData.pokeSex = pml::SEX_MALE; //@fix BTS[2522]
  for ( int i = 0; i < BattleInstTool::PARTNER_POKE_CNT; i++ )
  {
    mData.PartnerData.tokuseiIndex[i] = 0;
  }
  mData.PartnerData.trID = 0;
  mData.PartnerData.romData = NULL;
  mData.PartnerData.pokeData[0] = NULL;
  mData.PartnerData.pokeData[1] = NULL;
  mData.RecData = GFL_NEW( p_heap ) ExtSavedata::BattleRecorderSaveData( p_heap, p_gman );

  mCommDataMine.Party = reinterpret_cast<pml::PokeParty*>( GFL_NEW(mp_heap) pml::PokeParty(mp_heap) );
  mCommDataYours.Data.Party = reinterpret_cast<pml::PokeParty*>( GFL_NEW(mp_heap) pml::PokeParty(mp_heap) );
  mCommDataYours.MyStatus = GFL_NEW(mp_heap) Savedata::MyStatus();
  mData.Comm = false;
  mData.CommExit = false;
  mData.BtlResultPoint = 0;
  mData.PokeTotalHpMax = 0;
  mData.CommWinCount = 0;

  mIsParent = false;//マルチプレイ時、親かどうか

  mPastTrNo[0] = -1;
  mPastTrNo[1] = -1;

  //通信用にポケパーティ確保
  mCommVsTrainer.Party1 = reinterpret_cast<pml::PokeParty*>( GFL_NEW(mp_heap) pml::PokeParty(mp_heap) );
  mCommVsTrainer.Party2 = reinterpret_cast<pml::PokeParty*>( GFL_NEW(mp_heap) pml::PokeParty(mp_heap) );

  //選択出力用にポケパーティ確保
  mpSelectParty = GFL_NEW(mp_heap) pml::PokeParty(mp_heap);

  mpTrainerNameMsgData = NULL;
  mpTrainerNameMsgDataRoyal = NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief デストラクタ
 *
 */
//-----------------------------------------------------------------------------
BattleInst::~BattleInst()
{
  GFL_SAFE_DELETE( mpTrainerNameMsgData );
  GFL_SAFE_DELETE( mpTrainerNameMsgDataRoyal );
  GFL_SAFE_DELETE( mpSelectParty );
  GFL_SAFE_DELETE( mCommVsTrainer.Party1 );
  GFL_SAFE_DELETE( mCommVsTrainer.Party2 );
  GFL_SAFE_DELETE( mCommDataMine.Party );
  GFL_SAFE_DELETE( mCommDataYours.Data.Party );
  GFL_SAFE_DELETE( mCommDataYours.MyStatus );

  BATTLE_SETUP_Clear( &(mData.SetupParam) );

  GFL_SAFE_DELETE( mData.RecData );

  for (int i=0;i<2;i++)
  {
    if ( mData.TrainerParam.romData[i] )
    {
      GflHeapFreeMemory( mData.TrainerParam.romData[i] );
    }
  }

  if ( mData.PartnerData.romData )
  {
    GflHeapFreeMemory( mData.PartnerData.romData );
  }
  for (int i=0;i<2;i++)
  {
    if ( mData.PartnerData.pokeData[i] )
    {
      GflHeapFreeMemory( mData.PartnerData.pokeData[i] );
    }
  }

  for( int i = 0; i < ROYAL_ENEMY_NUM; ++i)
  {
    GflHeapSafeFreeMemory( mData.RoyalEnemyTrainerParam.romData[i] );
  }

  for( u32 cntClient = 0; cntClient < BTL_CLIENT_ROYAL_NUM; ++cntClient)
  {
    GFL_SAFE_DELETE( mData.BattleRoyalResultParam.aTrainerData[ cntClient ].pNameBuf);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ルールセット
 *
 *  @param  Regulation::BATTLE_CATEGORY category カテゴリ
 *  @param  rule              ルール
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::SetRule( Regulation::BATTLE_CATEGORY category, Regulation::BATTLE_RULE rule )
{
  switch (rule)
  {
  case Regulation::BATTLE_RULE_SINGLE:
    mData.TrainerParam.rule = BTL_RULE_SINGLE;
    break;
  case Regulation::BATTLE_RULE_DOUBLE:
    mData.TrainerParam.rule = BTL_RULE_DOUBLE;
    break;
  case Regulation::BATTLE_RULE_ROYAL:
    mData.TrainerParam.rule = BTL_RULE_ROYAL;
    break;
  case Regulation::BATTLE_RULE_MULTI:
    mData.TrainerParam.rule = BTL_RULE_DOUBLE;
    break;
  default:
    GFL_ASSERT(0);
    break;
  }

  //スーパーランクか？
  this->SetRank( category == Regulation::BATTLE_CATEGORY_TREE_SUPER );

  //セーブデータに現在挑戦中項目を格納
  {
    GameSys::GameData* p_gdata = mp_gman->GetGameData();
    Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
    sv->SetBattleTypeAndRank( this->GetBattleTreeType(), this->GetBattleTreeRank() );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  スーパーランクフラグセット
 *
 *  @param  bool super スーパーか？
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::SetRank( bool super )
{
  mData.SuperRank = super;
}

//----------------------------------------------------------------------------
/**
 *  @brief  対戦相手の決定
 *
 *  @param  int set_slot_idx
 *  @param  data_idx 
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::MakeTrainerData( int set_slot_idx, int data_idx )
{
  //トレーナーデータセット
  mData.TrainerParam.trID[set_slot_idx] = data_idx;

  {
    int arc_id;
    s32 size_align = 4;
    arc_id = ARCID_BATTLE_TREE_TRAINER;

    size_t size = BattleInstTool::GetTrainerRomDataSize( mp_heap, mp_gman->GetAsyncFileManager(), arc_id, data_idx);
    if( size > 0)
    {
      if ( mData.TrainerParam.romData[set_slot_idx] )
      {
        GflHeapFreeMemory( mData.TrainerParam.romData[set_slot_idx] );
        mData.TrainerParam.romData[set_slot_idx] = NULL;
      }
      mData.TrainerParam.romData[set_slot_idx] = (BINST_TRAINER_ROM_DATA  *)( GflHeapAllocMemoryLowAlign( mp_heap, size, size_align ) );
      BattleInstTool::GetTrainerRomData( mp_heap, mp_gman->GetAsyncFileManager(), arc_id, data_idx, mData.TrainerParam.romData[set_slot_idx], size);
    }
    else
    {
      GFL_ASSERT_MSG( 0, "data size <= 0\n");
    }
  }

  mData.TrainerParam.isBoss = false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  対戦相手の決定
 *
 *  @param  int set_slot_idx  スロットインデックス
 *  @param  data_idx          データインデックス
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::MakeTrainerDataWithBoss( int set_slot_idx, int data_idx )
{
  MakeTrainerData( set_slot_idx, data_idx );
  //ボスチェック
  {
    GameSys::GameData* p_gdata = mp_gman->GetGameData();
    Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();

    int win;
    //連勝数取得
    if ( mData.Comm )
    {
      win = mData.CommWinCount;
    }
    else
    {
      win = sv->GetWinCount( this->GetBattleTreeType(), this->GetBattleTreeRank() );
    }
    if ( mData.SuperRank )
    {
      if (win == 49) mData.TrainerParam.isBoss = true;
    }
    else
    {
      if (win == 19) mData.TrainerParam.isBoss = true;
    }
  }

  ONOUE_PRINT("対戦相手は trid = %d ボス %d\n",data_idx, mData.TrainerParam.isBoss);

}

//----------------------------------------------------------------------------
/**
 *  @brief  バトル開始
 *
 *  @param  bool inLvLowerFix レベル補正をかけるか？
 *  @param  bgm_change ＢＧＭ変更するか？
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::StartBattle( bool inLvLowerFix, bool bgm_change )
{
  BATTLE_SETUP_Clear( &(mData.SetupParam) );
  // @fix GFNMcat[1884] バトルツリー、バトルロイヤルではお手入れ禁止
  BATTLE_PARAM_SetBtlStatusFlag( &(mData.SetupParam), BTL_STATUS_FLAG_OTEIRE_DISABLE );

  pml::PokeParty     *my_party = reinterpret_cast<pml::PokeParty*>( GFL_NEW_LOW(mp_heap) pml::PokeParty(mp_heap) );
  pml::PokeParty     *partner_party = reinterpret_cast<pml::PokeParty*>( GFL_NEW_LOW(mp_heap) pml::PokeParty(mp_heap) );

  BATTLE_SETUP_PARAM *setup_param = &(mData.SetupParam);
  BattlelInstTrainerParam *param = &(mData.TrainerParam);
  EntryPokeData *entry_data = &(mData.EntryPokeData);

  //自分のポケパーティを作成する
  MakeMyPokeParty( inLvLowerFix, entry_data ,my_party );

  const Field::ZoneDataLoader * cp_zone_loader = mp_gman->GetGameData()->GetFieldZoneDataLoader();
  BTL_FIELD_SITUATION sit;
  Field::SetUpFieldSituation( &sit, mp_gman, &cp_zone_loader, Field::ATTR_ERROR, btl::bg::FAR_TYPE_NONE, btl::bg::NEAR_TYPE_NONE ); // @note アトリビュートに任せるのでBGはTYPE_NONEを指定

  //バトルルールでセットアップ関数を分岐
  switch( this->GetBattleRule() )
  {
  case Regulation::BATTLE_RULE_MULTI:  // AIマルチ
    if ( !mData.Comm )
    {
      BTL_SETUP_BattleHouseTrainer_Multi( setup_param, mp_gman, &sit, param->rule, my_party, mp_heap );
    }
    else
    {
      // 自分とパートナーセットアップ
      BATTLE_SETUP_BattleHouseComm( setup_param, mp_gman, &sit, param->rule, my_party, mCommDataYours.Data.Party, mIsParent, mp_heap );
      // 敵のトレーナーセットアップ
      BATTLE_SETUP_ToAIMultiModeForBattleHouse( setup_param, mp_gman, mp_heap );
    }
    break;
  case Regulation::BATTLE_RULE_DOUBLE: // ダブル
  case Regulation::BATTLE_RULE_SINGLE: // シングル
    BTL_SETUP_BattleHouseTrainer( setup_param, mp_gman, &sit, param->rule, my_party, mp_heap );
    break;
  default : GFL_ASSERT(0);
  }

  //コピー終了。パーティ解放
  GFL_SAFE_DELETE( partner_party );
  GFL_SAFE_DELETE( my_party );

  //BGM変更
  if ( bgm_change )
  {
    GameSys::GameData* p_gdata = mp_gman->GetGameData();
    Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
    if ( sv->IsSetBattleBgm() ){
      int bgm_no = sv->GetBattleBgm();

      // momiji追加
      // [fix]GFCHECK[8230] 一次元配列の記述方法を修正
      // 選べる戦闘曲_仕様書のリストに準拠した並べ順
      static const u32 SELECT_BGM_TABLE[BattleInstDefine::BINST_BGM_COUNT] =
      {
        STRM_BGM_VS_TRAINER_MJ,
        STRM_BGM_VS_TRAINER_NJ,
        STRM_BGM_MJ_M02,
        STRM_BGM_VS_NORAPOKE_02,
        STRM_BGM_VS_GIVER,
        STRM_BGM_VS_FRIEND_NJ,
        STRM_BGM_VS_RIVAL_NJ,
        STRM_BGM_VS_SKULL,
        STRM_BGM_VS_SKULLELITE,
        STRM_BGM_VS_SKULLBOSS,
        STRM_BGM_MJ_VS07,
        STRM_BGM_VS_ETHER,
        STRM_BGM_VS_MOTHER,
        STRM_BGM_VS_BEAST,
        STRM_BGM_MJ_VS08,
        STRM_BGM_MJ_VS09,
        STRM_BGM_VS_LEGEND02_NJ,
        STRM_BGM_VS_BROYAL,
        STRM_BGM_VS_LEGEND01_NJ,
        STRM_BGM_VS_SHITENNO_NJ,
        STRM_BGM_GAME_SYNC,
        STRM_BGM_VS_CHAMP_NJ,
        STRM_BGM_VS_REDGREEN,
        STRM_BGM_MJ_VS01,
        STRM_BGM_MJ_VS02,
        STRM_BGM_MJ_VS03,
        STRM_BGM_MJ_VS04,
        STRM_BGM_MJ_VS05,
        STRM_BGM_MJ_VS06,
        STRM_BGM_MJ_VSDIVEZONE02,
        STRM_BGM_MJ_VSDIVEZONE01,
        STRM_BGM_MJ_VSDIVEZONE08,
        STRM_BGM_MJ_VSDIVEZONE06,
        STRM_BGM_MJ_VSDIVEZONE07,
        STRM_BGM_MJ_VSDIVEZONE03,
        STRM_BGM_MJ_VSDIVEZONE05,
        STRM_BGM_MJ_VSDIVEZONE04,
        STRM_BGM_MJ_VSDIVEZONE11,
        STRM_BGM_MJ_VSDIVEZONE09,
        STRM_BGM_MJ_VSDIVEZONE10,
        STRM_BGM_MJ_VSDIVEZONE12,
        STRM_BGM_MJ_VSDIVEZONE13,
        STRM_BGM_MJ_VSDIVEZONE14,
        STRM_BGM_MJ_VSDIVEZONE16
        //STRM_BGM_VS_NORAPOKE_NJ,
        //STRM_BGM_VS_MOTHERBEAST,
      };

      if ( bgm_no == 0 || IsBoss() ){
        // 0番、ボスの場合デフォルトBGMを使う
      } else if ( bgm_no < BattleInstDefine::BINST_BGM_COUNT ){
        setup_param->btlEffData.bgm_default = SELECT_BGM_TABLE[bgm_no];
      } else {
        GFL_ASSERT_MSG(0,"BGM ERROR %d",bgm_no);
        setup_param->btlEffData.bgm_default = STRM_BGM_VS_TRAINER_MJ;
      }
    }
  }

  // @fix MMCat[363] ロトムのお願い「バトルツリー　お願い」で、バトルツリーをしなくてもお願いが完了される
  // ロトムお題達成関数差し込み
  App::FieldMenu::PlayData::SetRotomRequestAchieved(Savedata::FieldMenu::ROTOM_REQ_BATTLE_TREE);

  {
    Field::EventBattleInstBattleCall* p_event =
      reinterpret_cast<Field::EventBattleInstBattleCall*>(GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventBattleInstBattleCall>( mp_gman->GetGameEventManager() ));
    p_event->SetUpBattleParam( &(mData.SetupParam) );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  戦うトレーナーをバトルセットアップパラメータにセットする
 *
 *  @param  int tr_no       ハウストレーナーナンバー
 *  @param  dst             格納先トレーナーデータポインタ
 *  @param  pHeap           ヒープ
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::SetVsTrainer( int tr_no, BSP_TRAINER_DATA* dst, gfl2::heap::HeapBase* pHeap )
{
  if ( tr_no == BTL_INST_PARTNER_NO )   //対戦相手ではなく、相方の時、対象がＡＩ相方の時
  {
    if( !GetIsComm() )
    {
      SetAiBit( AI_BIT, this->GetBattleRule(), dst );

      // パートナートレイナー
      dst->SetTrainerID( mData.PartnerData.trID );
      // @fix GFMMcat[174]バトルツリーのククイがバトル中Zリングを持ってない
      trainer::TrType tr_type = static_cast<trainer::TrType>(mData.PartnerData.romData->tr_type);
      if (tr_type == trainer::TRTYPE_DOCTORNRBT)
      {
        tr_type = trainer::TRTYPE_DOCTOR;
      }
      dst->LoadTrTypeData(pHeap, tr_type);
      dst->SetName( mpTrainerNameMsgData, mData.PartnerData.trID );
    }
    // パートナーに勝ち負けメッセージセットは無い
  }
  else //対戦相手のとき
  {
    u32 tr_id    = mData.TrainerParam.trID[tr_no];
    SetAiBit( AI_BIT, this->GetBattleRule(), dst );
    dst->SetTrainerID( tr_id );
    // @fix GFMMcat[174]バトルツリーのククイがバトル中Zリングを持ってない
    trainer::TrType tr_type = static_cast<trainer::TrType>(mData.TrainerParam.romData[tr_no]->tr_type);
    if (tr_type == trainer::TRTYPE_DOCTORNRBT)
    {
      tr_type = trainer::TRTYPE_DOCTOR;
    }
    dst->LoadTrTypeData(pHeap, tr_type);
    // @fix GFNMCat[1155] 敵トレーナーの名前にパートナー名がセットされていた問題を修正
    dst->SetName( mpTrainerNameMsgData, tr_id );

    // 勝ち負けメッセージセット
    dst->SetMsgID( GARC_message_tower_trainer_DAT,
        TrainerIDtoWinStrID( tr_id ),
        TrainerIDtoLoseStrID( tr_id ) );
  }
  //トレーナーはアイテム所持していない
}

//--------------------------------------------------------------
/**
 * @brief 対戦ポケモンセット
 * @param tr_no     0～2
 * @param party     ポケパーティ
 * @param pHeap  ヒープ
 */
//--------------------------------------------------------------
void BattleInst::SetVsPokemon( int tr_no, pml::PokeParty* party, gfl2::heap::HeapBase* pHeap )
{
  int poke_cnt = 0;
  pml::pokepara::PokemonParam *pp_ary[6];

  if ( tr_no == BTL_INST_PARTNER_NO ) //対象がＡＩ相方の時
  {
    poke_cnt = BattleInstTool::PARTNER_POKE_CNT;
    MakePartnerPokemon( pHeap, pp_ary );
  }
  else
  {
    int arc_id;
    switch( this->GetBattleRule() )
    {
    case Regulation::BATTLE_RULE_MULTI:
      // AIマルチ
      poke_cnt = 2;
      break;
    case Regulation::BATTLE_RULE_DOUBLE:
      // ダブル
      poke_cnt = 4;
      break;
    case Regulation::BATTLE_RULE_SINGLE:
      // シングル
      poke_cnt = 3;
      break;
    default:
      GFL_ASSERT_STOP(0);
      break;
    }

    arc_id = ARCID_BATTLE_TREE_POKE;

    if ( tr_no == BTL_INST_ENEMY1_NO ) //一人目
    {
      MakeTrainerPokemon( arc_id,
                          mData.TrainerParam.romData[tr_no],
                          mData.TrainerParam.trID[tr_no],
                          poke_cnt,
                          NULL,
                          NULL,
                          pHeap,
                          pp_ary,
                          mData.TrainerParam.select_poke_index );
    }
    else              //二人目 ( tr_no == BTL_INST_ENEMY2_NO )
    {
      MakeTrainerPokemon( arc_id,
                          mData.TrainerParam.romData[tr_no],
                          mData.TrainerParam.trID[tr_no],
                          poke_cnt,
                          mData.TrainerParam.check_poke,
                          mData.TrainerParam.check_item,
                          pHeap,
                          pp_ary,
                          NULL );
    }
  }


  //パーティにセット
  for(int i=0;i<poke_cnt;i++)
  {
    party->AddMember( *pp_ary[i] );
    GFL_SAFE_DELETE( pp_ary[i] );
  }
}

//--------------------------------------------------------------
/**
 * @brief 自分のポケパーティを作成
 * @param inLvLowFix      レベル５０未満の自分のポケモンのレベルを５０まで引き上げるか？
 * @param entry_data      エントリーデータ
 * @param outParty        ポケパーティ格納場所
 */
//--------------------------------------------------------------
void BattleInst::MakeMyPokeParty( bool inLvLowerFix, const EntryPokeData *entry_data, pml::PokeParty* outParty )
{
  MakePokeParty( inLvLowerFix, mpSelectParty, entry_data, outParty );

  //自分のポケパーティの最大ＨＰのトータルを計算（トライアルハウス用）
  mData.PokeTotalHpMax = 0;
  for(int i=0;i<entry_data->entry_num;i++)
  {
    pml::pokepara::PokemonParam* param = outParty->GetMemberPointer( i );
    mData.PokeTotalHpMax += param->GetHp();
  }
  ONOUE_PRINT("TOTAL_POKE_HP = %d\n",mData.PokeTotalHpMax);
}


//--------------------------------------------------------------
/**
 * @brief ポケパーティを作成
 * @param inLvLowFix      レベル５０未満の自分のポケモンのレベルを５０まで引き上げるか？
 * @param inParty        元のパーティー
 * @param entry_data      エントリーデータ
 * @param outParty        ポケパーティ格納場所
 */
//--------------------------------------------------------------
void BattleInst::MakePokeParty( bool inLvLowerFix, pml::PokeParty* inParty, const EntryPokeData *entry_data, pml::PokeParty* outParty )
{
  //まず回復する
  inParty->RecoverAll();
  ONOUE_PRINT("entry_data->entry_num:%d\n",entry_data->entry_num);
  for(int i=0;i<entry_data->entry_num;i++)
  {
    pml::pokepara::PokemonParam* param = inParty->GetMemberPointer( entry_data->entry_poke_pos[i] );
    outParty->AddMember( *(param) );
  }

  // スーパーランクならレベル補正
  if (GetRank())
  {
    for (int i = 0; i < entry_data->entry_num; i++)
    {
      pml::pokepara::PokemonParam* param = outParty->GetMemberPointer(i);
      //LV50調整
      AdjustLevel(inLvLowerFix, param);
    }
  }

  //@fix GFBTS2629【ポケモンパラメータの不正検査の不足】
  for(int i=0;i<entry_data->entry_num;i++)
  {
    pml::pokepara::PokemonParam* param = outParty->GetMemberPointer( i );
    //不正パラメーター修正
    param->RecalculateCalcData();
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  AI相方ポケモン作成
 *
 *  @param  pHeap   ヒープ
 *  @param  pp_ary  ポケパラポインタ配列
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::MakePartnerPokemon( gfl2::heap::HeapBase* pHeap,
                                     pml::pokepara::PokemonParam** pp_ary )
{
  for( int i=0; i<BattleInstTool::PARTNER_POKE_CNT; i++ )
  {
    //格納配列にセット
    pml::pokepara::PokemonParam* pp;
    u8 pow = 0x1f;  //最強設定
    //特性を指定する @fix BTS[1911] 性別を指定する  @fix BTS[2522]
    pp = BattleInstTool::MakePokemonParam(
        mData.PartnerData.pokeData[i], 50, pow, pHeap, mData.PartnerData.tokuseiIndex[i], mData.PartnerData.pokeSex );
    pp_ary[i] = pp;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  対戦ポケモン作成
 *
 *  @param arc_id      アーカイブＩＤ
 *  @param td         ＲＯＭトレーナーデータポインタ
 *  @param tr_id      トレーナーＩＤ
 *  @param poke_cnt   ポケモン数
 *  @param set_poke_no  決定したポケモン配列
 *  @param set_item_no　決定したアイテム配列
 *  @param pHeap        ヒープ
 *  @param pp_ary       ポケパラポインタ配列
 *
 *  @return bool 50回抽選をした場合 true
 */
//-----------------------------------------------------------------------------
bool BattleInst::MakeTrainerPokemon( int arc_id,
                                     BINST_TRAINER_ROM_DATA *td,
                                     int tr_id,
                                     int poke_cnt,
                                     u16 *set_poke_no,
                                     u16 *set_item_no,
                                     gfl2::heap::HeapBase* pHeap,
                                     pml::pokepara::PokemonParam** pp_ary ,
                                     u16 *select_poke_index )
{
  //手持ちポケモンのMAXは6体まで
  if ( poke_cnt > 6 )
  {
    GFL_ASSERT(0);
    poke_cnt = 6;
  }

  bool ret = false;

  int set_count=0;
  int loop_count = 0;
  int entry_mons_no[6] = {0,0,0,0,0,0};
  int entry_item_no[6] = {0,0,0,0,0,0};
  while( set_count != poke_cnt )
  {
    int i;
    BINST_POKEMON_ROM_DATA  pd;
    int poke_index = System::GflUse::GetPublicRand(td->use_poke_cnt);
    int set_index = td->use_poke_table[ poke_index ];
    BattleInstTool::GetPokemonRomData(mp_heap, mp_gman->GetAsyncFileManager(), arc_id, set_index, &pd);

    //モンスターナンバーのチェック（同一のポケモンは持たない）
    for(i=0;i<set_count;i++)
    {
      GFL_ASSERT(pd.mons_no != 0);  //@checked
      if( entry_mons_no[i] == pd.mons_no )
      {
        break;      //抽選済み
      }
    }
    if( i != set_count )
    {
      continue;
    }

    //ペアを組んでいるトレーナーの持ちポケモンとのチェック
    if(set_poke_no != NULL)
    {
      //モンスターナンバーのチェック（同一のポケモンは持たない）
      for(i=0;i<poke_cnt;i++){
        if( set_poke_no[i] == pd.mons_no )
        {
          break; //抽選済み
        }
      }
      if( i != poke_cnt )
      {
        continue;
      }
    }

    //50回まわして、決まらないようなら、同一アイテムチェックはしない
    if(loop_count<50){
      //装備アイテムのチェック（同一のアイテムは持たない）
      for(i=0;i<set_count;i++)
      {
        if((pd.item_no)&&(entry_item_no[i] == pd.item_no)){
          break; //アイテム重複
        }
      }
      if( i != set_count )
      {
        loop_count++;
        continue;
      }

      //ペアを組んでいるトレーナーの持ちポケモンの装備アイテムとのチェック
      if(set_item_no!=NULL)
      {
        //装備アイテムのチェック（同一のアイテムは持たない）
        for(i=0;i<poke_cnt;i++)
        {
          if((set_item_no[i] == pd.item_no) && ( set_item_no[i] != 0 ))
          {
            break;
          }
        }
        if( i != poke_cnt )
        {
          loop_count++;
          continue;
        }
      }
    }

    entry_mons_no[ set_count ] = pd.mons_no;
    entry_item_no[ set_count ] = pd.item_no;
    if( select_poke_index != NULL )
    {
      select_poke_index [ set_count ] = td->use_poke_table[ poke_index ];
      ONOUE_PRINT("poke_index[%d]=%d\n",set_count,td->use_poke_table[ poke_index ]);
    }
    //二人目の重複用に保存
    {
      if(set_poke_no == NULL) mData.TrainerParam.check_poke[ set_count ] = pd.mons_no;
      if(set_item_no == NULL) mData.TrainerParam.check_item[ set_count ] = pd.item_no;
    }

    //格納配列にセット
    {
      pml::pokepara::PokemonParam* pp;
      u8 pow = GetPowerRnd( tr_id );
      pp = BattleInstTool::MakePokemonParam( &pd, 50, pow, pHeap );//特性, 性別指定しない

      // NMCat[747] バトルツリーで相手トレーナーの登場演出と実際に投げるボールが異なる
      // トレーナータイプデータから取得したボールIDをセットする
      item::BALL_ID ball_id = this->GetBallIDByTrainerRomData( pHeap->GetLowerHandle(), td );
      pp->SetGetBall( ball_id );

      pp_ary[set_count] = pp;
    }
    set_count++;
  } //end while

  if( loop_count >= 50 )
  {
    ret = true;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *  @brief    レベル調整
 *
 *  @param  bool lv_lower_fix レベル調整するか？
 *  @param  pp    対象ポケモンパラム
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void BattleInst::AdjustLevel( bool lv_lower_fix, pml::pokepara::PokemonParam* pp )
{
  int level = pp->GetLevel();
  if ( (level > 50) || lv_lower_fix )
  {
    level = 50;
    pml::personal::LoadGrowTable( pp->GetMonsNo() , pp->GetFormNo() );  // 成長曲線テーブルをロード
    u32 exp = pml::personal::GetMinExp( level );  // 設定したいレベルになるための最小経験値を取得
    pp->SetExp( exp );  // 経験値を設定（内部でパラメータを再計算・技は変化なし）
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  対戦トレーナーをグリッドに追加
 *
 *  @param  pos_index  モデルのインデックス
 *  @param  charID
 *  @param  x                                 Ｘ座標
 *  @param  y                                 Ｙ座標
 *  @param  z                                 Ｚ座標
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void BattleInst::AddTrainerObj( Field::BATTLE_INST_POS_INDEX pos_index, int charID, float x, float y, float z )
{
  Field::Fieldmap* p_fieldmap = mp_gman->GetFieldmap();
  Field::FieldGimmickBattleInst* pGimmick = static_cast<Field::FieldGimmickBattleInst*>( p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));
  BattleInstModelManager* pModelManager = pGimmick->GetModelManager();

  mModelData[pos_index].pos = gfl2::math::Vector3(x,y,z);
  mModelData[pos_index].rot.RadianYToQuaternion(0);
  mModelData[pos_index].charaID = charID;
  mModelData[pos_index].isDressup = false;
  pModelManager->Request(&mModelData[pos_index]);
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
void BattleInst::AddDressupTrainerObj( Field::BATTLE_INST_POS_INDEX pos_index, float x, float y, float z )
{
  Field::Fieldmap* p_fieldmap = mp_gman->GetFieldmap();
  Field::FieldGimmickBattleInst* pGimmick = static_cast<Field::FieldGimmickBattleInst*>( p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));
  BattleInstModelManager* pModelManager = pGimmick->GetModelManager();

  mModelData[pos_index].pos = gfl2::math::Vector3(x,y,z);
  mModelData[pos_index].rot.RadianYToQuaternion(0);
  mModelData[pos_index].charaID = 0;
  mModelData[pos_index].pMyStatus = mCommDataYours.MyStatus;
  mModelData[pos_index].isDressup = true;
  pModelManager->Request(&mModelData[pos_index]);
}

//----------------------------------------------------------------------------
/**
 *  @brief  モデル読み込み待ち
 *
 *  @return trueで完了
 */
//--------------------------------------------------------------------------
bool BattleInst::WaitModelLoad()
{
  Field::Fieldmap* p_fieldmap = mp_gman->GetFieldmap();
  Field::FieldGimmickBattleInst* pGimmick = static_cast<Field::FieldGimmickBattleInst*>( p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));
  BattleInstModelManager* pModelManager = pGimmick->GetModelManager();

  return pModelManager->IsRequestEnd(false);
}

//----------------------------------------------------------------------------
/**
 *  @brief  モデル情報を破棄
 */
//--------------------------------------------------------------------------
void BattleInst::ClearModelData()
{
  for (int i = 0; i < BattleInstDefine::BINST_MODEL_DISP_MAX; i++)
  {
    mModelData[i].isUsing = false;
    mModelData[i].isDressup = false;
    mModelData[i].pMyStatus = NULL;
    mModelData[i].pos.x = 0;
    mModelData[i].pos.y = 0;
    mModelData[i].pos.z = 0;

    mModelData[i].rot.RadianYToQuaternion(0);
    mModelData[i].charaID = 0;
    mModelData[i].index = static_cast<Field::BATTLE_INST_POS_INDEX>(i);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief    対戦トレーナー決定
 *
 *  @param  int data_idx データインデックス
 *
 *  @return no  対戦トレーナーナンバー
 */
//-----------------------------------------------------------------------------
int BattleInst::SelectTrainer( int data_idx )
{
  int no = 0;
  Regulation::BATTLE_RULE rule = this->GetBattleRule();

  GameSys::GameData* p_gdata = mp_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();

  //連勝数取得
  int win;
  if ( mData.Comm )
  {
    win = mData.CommWinCount;
  }
  else
  {
    win = sv->GetWinCount( this->GetBattleTreeType(), this->GetBattleTreeRank() );
  }

  // @fix NMCat[1105] トレーナーの抽選範囲が、ノーマルとスーパーで異なっていた問題を修正
  if ( mData.SuperRank )
  {
    int bossTrNo = SelectSuperBoss( data_idx, win, mData.Comm, rule );

    if( bossTrNo != -1 ){
      no = bossTrNo;
    }else{
      no = SelectRandamTrainer( win, data_idx );
    }
  }
  else
  {
    if( win == 19 )
    {
      // @fix NMCat[1111] ノーマルとスーパーでボスの強さが変わるように調整
      if( rule == Regulation::BATTLE_RULE_SINGLE ){
        no = 203;
      }else if ( rule == Regulation::BATTLE_RULE_DOUBLE ){
        no = 204;
      }else if( rule == Regulation::BATTLE_RULE_MULTI ){
        if ( !mData.Comm )      //非通信
        {
          if ( data_idx == -1 ) no = 203;   //  一人目
          else no = 204;    // 二人目
        }
        else                    //通信
        {
          if ( data_idx == -1 ) no = 203;   //  一人目
          else no = 204;    // 二人目
        }
      }else{
        GFL_ASSERT_MSG(0,"win = %d",win);
      }
    }else{
      no = SelectRandamTrainer( win, data_idx );
    }
  }

  if ( mData.Comm )
  {
    if ( data_idx == -1 ) mCommVsTrainer.no[0] = no;
    else mCommVsTrainer.no[1] = no;
  }

  return no;
}

//----------------------------------------------------------------------------
/**
*  @brief トレーナーロムデータから捕獲ボールIDを取得
*  @param win_count 勝利数
*  @param tr_no トレーナーインデックス（一人目もしくは二人目）
*  @return トレーナーID
*/
//-----------------------------------------------------------------------------
int BattleInst::SelectRandamTrainer( int win_count, int tr_no )
{
  int no = 0;
  if ( win_count < 10 ) no = SelectTrainerCore( tr_no, 0, 50 );
  else if( win_count < 20 ) no = SelectTrainerCore( tr_no, 30, 40 );
  else if( win_count < 30 ) no = SelectTrainerCore( tr_no, 50, 40 );
  else if( win_count < 40 ) no = SelectTrainerCore( tr_no, 70, 40 );
  else if( win_count < 49 ) no = SelectTrainerCore( tr_no, 90, 40 );
  else no = SelectTrainerCore( tr_no, 90, 100 );

  return no;
}

//----------------------------------------------------------------------------
/**
 *  @brief    連勝数加算
 *
 */
//-----------------------------------------------------------------------------
void BattleInst::AddWinCount( void )
{
  GameSys::GameData* p_gdata = mp_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  Regulation::BATTLE_RULE rule = this->GetBattleRule();

  Savedata::BattleTreeType type = this->GetBattleTreeType();
  Savedata::BattleTreeRank rank = this->GetBattleTreeRank();

  if ( mData.Comm )
  {
    //通信連勝数インクリメント
    mData.CommWinCount++;
    //値オーバー用の対処
    if ( mData.CommWinCount > sv->GetCountMax() ) mData.CommWinCount = sv->GetCountMax();
  }
  //連勝数インクリメント
  sv->AddWinCount( type, rank );

  //取得してレコードにセット
  {
    Savedata::Record* rec = p_gdata->GetSaveData()->GetRecordSaveData();

    int normal = sv->GetWinCount( type, Savedata::BATTLE_TREE_RANK_NORMAL);
    int super = sv->GetWinCount( type, Savedata::BATTLE_TREE_RANK_SUPER);
    int count;
    if ( normal > super ) count = normal;
    else count = super;

    Savedata::Record::RECORD_ID id;
    switch(rule)
    {
    case Regulation::BATTLE_RULE_SINGLE:
      id = Savedata::Record::RECID_BTREE_WIN_SINGLE;
      break;
    case Regulation::BATTLE_RULE_DOUBLE:
      id = Savedata::Record::RECID_BTREE_WIN_DOUBLE;
      break;
    case Regulation::BATTLE_RULE_MULTI:
      id = Savedata::Record::RECID_BTREE_WIN_MULTI;
      break;
    default:
      return; //@fix KW[1395]
    }
    rec->SetRecIfLarge( id, count );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief バトルに勝ったか？
 *
 *  @return　bool trueで勝利
 */
//-----------------------------------------------------------------------------
bool BattleInst::IsWin( void )
{
  BtlResult result = mData.SetupParam.result;
  if ( result == BTL_RESULT_WIN )
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  対戦者はボスか？
 *
 *  @return bool trueでボス
 */
//-----------------------------------------------------------------------------
bool BattleInst::IsBoss( void )
{
  return mData.TrainerParam.isBoss;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ＡＩパートナー作成
 *
 */
//-----------------------------------------------------------------------------
void BattleInst::MakeAiPartner( void )
{
  //ポケモンデータ領域確保
  for (int i=0; i<BattleInstTool::PARTNER_POKE_CNT; i++)
  {
    s32 size_align = 4;
    int size = sizeof(BINST_POKEMON_ROM_DATA);
    mData.PartnerData.pokeData[i] =
      (BINST_POKEMON_ROM_DATA  *)( GflHeapAllocMemoryLowAlign( mp_heap, size, size_align ) );
  }


  GameSys::GameData* p_gdata = mp_gman->GetGameData();
  Savedata::BattleInstSave* p_save = p_gdata->GetBattleInstSave();

  // 選択したパートナーのデータを取得
  u32 selectNpcIndex  = mData.PartnerNPCID;
  u16 selectTrainerID = 0;
  u16 selectPokeID1 = 0;
  u16 selectPokeID2 = 0;
  u8 selectPokeTokuseiIndex1 = 0;
  u8 selectPokeTokuseiIndex2 = 0;
  p_save->GetScoutTrainerData(selectNpcIndex,&selectTrainerID,&selectPokeID1,&selectPokeID2,&selectPokeTokuseiIndex1,&selectPokeTokuseiIndex2);
  ONOUE_PRINT("select selectNpcIndex:%d\n",selectNpcIndex);
  ONOUE_PRINT("select selectTrainerID:%d\n",selectTrainerID);
  ONOUE_PRINT("select selectPokeID1:%d\n",selectPokeID1);
  ONOUE_PRINT("select selectPokeID2:%d\n",selectPokeID2);
  ONOUE_PRINT("select selectTokuseiIndex1:%d\n",selectPokeTokuseiIndex1);
  ONOUE_PRINT("select selectTokuseiIndex2:%d\n",selectPokeTokuseiIndex2);

  int selectPokeData[BattleInstTool::PARTNER_POKE_CNT] = {selectPokeID1,selectPokeID2};
  int selectPokeTokuseiIndex[BattleInstTool::PARTNER_POKE_CNT] = {selectPokeTokuseiIndex1,selectPokeTokuseiIndex2};

  // ポケモンを選択
  {
    int poke_index = 0;
    int set_count=0;
    while( set_count != BattleInstTool::PARTNER_POKE_CNT )
    {
      BINST_POKEMON_ROM_DATA  pd;
      BattleInstTool::GetPokemonRomData(mp_heap, mp_gman->GetAsyncFileManager(), ARCID_BATTLE_TREE_POKE, selectPokeData[set_count], &pd);
      *(mData.PartnerData.pokeData[ set_count ]) = pd;
      mData.PartnerData.tokuseiIndex[ set_count ] = selectPokeTokuseiIndex[set_count];
      set_count++;
    }
  }


  // パートナーデータを生成
  {
    mData.PartnerData.trID = selectTrainerID;  //データインデックス兼ＩＤ
    const int arc_id = ARCID_BATTLE_TREE_TRAINER;
    s32 size_align = 4;

    size_t size = BattleInstTool::GetTrainerRomDataSize( mp_heap, mp_gman->GetAsyncFileManager(), arc_id, selectTrainerID);
    if( size > 0)
    {
      if ( mData.PartnerData.romData )
      {
        GflHeapFreeMemory( mData.PartnerData.romData );
        mData.PartnerData.romData = NULL;
      }
      mData.PartnerData.romData = (BINST_TRAINER_ROM_DATA  *)( GflHeapAllocMemoryLowAlign( mp_heap, size, size_align ) );
      BattleInstTool::GetTrainerRomData( mp_heap, mp_gman->GetAsyncFileManager(), arc_id, selectTrainerID, mData.PartnerData.romData, size);
    }
    else
    {
      GFL_ASSERT_MSG( 0, "data size <= 0\n");
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  トレーナーキャラクターID取得
*
*  @param  int tr_no トレーナーナンバー
*
*  @return キャラクターID
*/
//----------------------------------------------------------------------------- 
int BattleInst::GetCharacterID( int tr_no )
{
  int charaID;
  trainer::TrainerTypeData trData;
  trainer::TrType tr_type = static_cast<trainer::TrType>(mData.TrainerParam.romData[tr_no]->tr_type);

  trData.LoadData( mp_heap->GetLowerHandle() , tr_type );
  charaID = trData.GetFieldMainModelID();

  ONOUE_PRINT("指定tr_type %d 取得ＯＢＪコード %d\n",tr_type, charaID);

  return charaID;
}

//----------------------------------------------------------------------------
/**
*  @brief  パートナートレーナーキャラクターID取得
*
*  @return キャラクターID
*/
//----------------------------------------------------------------------------- 
int BattleInst::GetPartnerCharacterID()
{
  int charaID;
  trainer::TrainerTypeData trData;
  trainer::TrType tr_type = static_cast<trainer::TrType>(mData.PartnerData.romData->tr_type);

  trData.LoadData( mp_heap->GetLowerHandle() , tr_type );
  charaID = trData.GetFieldMainModelID();

  ONOUE_PRINT("指定tr_type %d 取得ＯＢＪコード %d\n",tr_type, charaID);

  return charaID;
}

//----------------------------------------------------------------------------
/**
 *  @brief      トレーナーＩＤ取得
 *
 *  @param  int tr_no  選出トレーナー番号　0～1
 *
 *  @return トレーナーＩＤ
 */
//-----------------------------------------------------------------------------
int BattleInst::GetTrainerID( int tr_no )
{
  return mData.TrainerParam.trID[tr_no];
}

//----------------------------------------------------------------------------
/**
 *  @brief  戦闘録画できるかチェック
 *  @return 録画OKのときにtrue
 *
 */
//-----------------------------------------------------------------------------
bool BattleInst::IsCanSaveBattleRecord()
{
  //通信切断されたら戦闘記録しない @fix GFBTS[1426]
  BtlResult result = mData.SetupParam.result;
  if ( result == BTL_RESULT_COMM_ERROR )
  {
    ONOUE_PRINT("通信エラーだったので戦闘録画はスキップ");
    return false;
  }
   return true;
}

//----------------------------------------------------------------------
/*
 *  @brief  戦闘開始メッセージＩＤを返す
 *  @param  no  1 or 0
 *  @return 文字列ＩＤ
 */
//----------------------------------------------------------------------
int BattleInst::GetBattleStartMsgId( int no )
{
  if( no > 0)
  {
    no = 1;
  }
  else
  {
    no = 0;
  }

  int str_id;
  str_id = GetTrainerID( no ) * BINST_MSG_OFFSET;
  ONOUE_PRINT("str_id = %d trid = %d\n",str_id, GetTrainerID( 0 ));
  return str_id;
}

//--------------------------------------------------------------
/**
 * バトル施設トレーナー抽選
 * @param   tr_no   マルチの一人目トレーナーナンバー
 * @param   start   開始インデックス
 * @param   width   抽選幅
 *
 * @return  抽選結果トレーナーナンバー
 *
 * @fix     BTS[2176]:連戦している間に同じトレーナが立て続けに出ないように処理を変更。連戦ででなければＯＫとする
 */
//--------------------------------------------------------------
int BattleInst::SelectTrainerCore( int tr_no, int start, int width )
{
  int no;
  if ( tr_no == -1 )
  {
    int i = 0;
    //５０回トライアル
    for( i=0;i<50;i++ )
    {
      no = start + System::GflUse::GetPublicRand(width);
      if ( (no != tr_no)&&(no != mPastTrNo[0])&&(no != mPastTrNo[1]) && (!IsPartner(no)) )
      {
        break;  //選出終了
      }
    }

    //５０回トライアルしても重複した場合
    if ( i>=50 )
    {
      //捜査対象を降順にしておく
      int comp[2];
      if ( mPastTrNo[0] <= mPastTrNo[1] )
      {
        comp[0] = mPastTrNo[0];
        comp[1] = mPastTrNo[1];
      }
      else
      {
        comp[1] = mPastTrNo[0];
        comp[0] = mPastTrNo[1];
      }

      no = start;
      //過去トレーナーがいれば過去トレーナーの次のインデックスにセット（インデックス巻きかえりつき）
      if ( (comp[0] >= 0) && (no == comp[0]) )  //過去トレーナー1人目と比較
      {
        no = start + ( ((comp[0] - start) + 1 ) % width );
      }
      if ( (comp[1] >= 0) && no == comp[1] )  //過去トレーナー2人目と比較
      {
        no = start + ( ((comp[1] - start) + 1 ) % width );
      }
      //@note 抽選幅が２以下はない前提でこれ以上捜査しない
    }
    
#if PM_DEBUG

    if ( Field::Debug::DebugTypes::s_battleInstDebugBtlInstTrainer1 > 0)
    {
      no = Field::Debug::DebugTypes::s_battleInstDebugBtlInstTrainer1 - 1;
    }
#endif
  }
  else
  {
    int i = 0;
    //５０回トライアル
    for( i=0;i<50;i++ )
    {
      no = start + System::GflUse::GetPublicRand(width);
      if ( (no != tr_no)&&(no != mPastTrNo[0])&&(no != mPastTrNo[1]) && (!IsPartner(no)) )
      {
        break; //選出終了
      }
    }

    //５０回トライアルしても重複した場合
    if ( i>=50 )
    {
      //捜査対象を降順にしておく
      int comp[3];
      if ( mPastTrNo[0] <= mPastTrNo[1] )
      {
        comp[0] = mPastTrNo[0];
        comp[1] = mPastTrNo[1];
      }
      else
      {
        comp[1] = mPastTrNo[0];
        comp[0] = mPastTrNo[1];
      }

      int tgt = tr_no;
      for(int j=0;j<2;j++)
      {
        if ( tgt <= comp[j] )
        {
          int tmp = comp[j];
          comp[j] = tgt;
          tgt = tmp;
        }
      }
      comp[2] = tgt;

      //過去トレーナーがいれば過去トレーナーの次のインデックスにセット（インデックス巻きかえりつき）
      no = start;
      if ( (comp[0] >= 0) && (no == comp[0]) )
      {
        no = start + ( ((comp[0] - start) + 1 ) % width );
      }
      if ( (comp[1] >= 0) && no == comp[1] )
      {
        no = start + ( ((comp[1] - start) + 1 ) % width );
      }
      if ( (comp[2] >= 0) && no == comp[2] )
      {
        no = start + ( ((comp[2] - start) + 1 ) % width );
      }
      //@note 抽選幅が２以下はない前提でこれ以上捜査しない
    }

#if PM_DEBUG
    if ( Field::Debug::DebugTypes::s_battleInstDebugBtlInstTrainer2 > 0)
    {
      no = Field::Debug::DebugTypes::s_battleInstDebugBtlInstTrainer2 - 1;
    }
#endif
  }

  return no;
}

//----------------------------------------------------------------------------
/**
*  @brief パートナーかチェック
*  @param no チェックするトレーナーNO
*  @retval パートナーの場合にtrue
*/
//-----------------------------------------------------------------------------
bool BattleInst::IsPartner(int no)
{
  return (GetBattleRule() == Regulation::BATTLE_RULE_MULTI && !GetIsComm() && mData.PartnerData.trID == no);
}

//--------------------------------------------------------------
/**
 * ボスを選択
 * @param   data_index  データ番号
 * @param   winCount    勝利数
 * @param   isComm      通信かどうか
 * @param   rule        ルール
 * 
 * @return ボスと戦えない場合は、-1
 */
//--------------------------------------------------------------
int BattleInst::SelectSuperBoss( int data_idx, int winCount, bool isComm, Regulation::BATTLE_RULE rule )
{
  static BossData bossSun[]  = {
    {197,-1},
    {201,-1},
    {199,-1},
    {192,-1},
    {194,-1},
    {195,-1},
    {196,-1},
    {205,-1},
    {193,FE_BINST_BOSS_ENABLE_RIRA}
  };


  static BossData bossMoon[]  = {
    {198,-1},
    {202,-1},
    {200,-1},
    {192,-1},
    {194,-1},
    {195,-1},
    {196,-1},
    {205,-1},
    {193,FE_BINST_BOSS_ENABLE_RIRA}
  };

  if( !CheckIsBoss(winCount) )
  {
    return -1;
  }

  BossData* bossArray = NULL;
  const bool isSun = (GET_VERSION() == VERSION_SUN2 );
  if( isSun )
  {
    bossArray = bossSun;
  }else{
    bossArray = bossMoon;
  } 

  // @fix GFNMCat[1440] スーパー50戦目の抽選される敵トレーナーが正しくない問題を修正
  if( winCount == 49 )
  {
    if ( data_idx == -1 ) 
    {
      if( rule == Regulation::BATTLE_RULE_DOUBLE )
      {
        return 191;   //  ダブル一人目
      }else{
        return 190;   //  シングル一人目
      }
    }else {
      return 191;    // 二人目
    }
  }else{
    // @momiji変更箇所
    int index = System::GflUse::GetPublicRand(114)/14;
    if( index > 8 ){ index = 8; }

    GameSys::GameData* p_gdata = mp_gman->GetGameData();
    Field::EventWork* pEventWork = p_gdata->GetEventWork();

    int retIndex = -1;
    // リラのみフラグ管理
    if (bossArray[index].flag_no == -1 || pEventWork->CheckEventFlag(bossArray[index].flag_no))
    {
      retIndex = bossArray[index].tr_no;
    }

    // AIパートナーの場合は抽選しない
    if( !isComm )
    {
      if( mData.PartnerData.trID == retIndex )
      {
        retIndex = -1;
      }
    }

    if( retIndex == data_idx )
    {
      return -1;
    }else{
      return retIndex;
    }
  }
}

//--------------------------------------------------------------
/**
 * 抽選の対象外とするトレーナーをセットする
 * @param   target  0 or 1
 * @param   tr_no   施設トレーナーナンバー
 */
//--------------------------------------------------------------
void BattleInst::SetNoEntryTrainer( int target, int trno )
{
  mPastTrNo[target] = trno;
}

//----------------------------------------------------------------------------
/**
 *  @brief    通信用対戦ポケモンセット
 *
 */
//-----------------------------------------------------------------------------
void BattleInst::SelectVsPokemonForComm( void )
{
  //送信前に内容をクリア
  mCommVsTrainer.Party1->Clear();
  mCommVsTrainer.Party2->Clear();
  //セット
  SetVsPokemon( 0, mCommVsTrainer.Party1, mp_heap );
  SetVsPokemon( 1, mCommVsTrainer.Party2, mp_heap );
}

//----------------------------------------------------------------------------
/**
 *  @brief ポケパーティに通信用対戦ポケモンをセット
 *
 *  @param  int tr_no   トレーナー番号0～1
 *  @param  party       格納ポケパーティ
 *
 */
//-----------------------------------------------------------------------------
void BattleInst::SetVsCommPokemonToParty( int tr_no, pml::PokeParty* party )
{
  if (tr_no == 0)
  {
    party->CopyFrom( *mCommVsTrainer.Party1 );
  }
  else
  {
    party->CopyFrom( *mCommVsTrainer.Party2 );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief    通信用連勝数セット
 *
 *  @param  int count 連勝カウンタ
 *
 */
//-----------------------------------------------------------------------------
void BattleInst::SetCommWinCount( int count )
{
  mData.CommWinCount = count;
  if ( !mData.SuperRank )
  {
    if ( mData.CommWinCount > 19 )
    {
      GFL_ASSERT_MSG(0,"COMM_WINCNT_OVER %d",mData.CommWinCount);
      mData.CommWinCount = 0;
    }
  }
}

//--------------------------------------------------------------
/**
 * 参加ポケモンにリボンをつける
 * @param ribbon_id
 * @param mode ツリーかロイヤルか
 */
//--------------------------------------------------------------
bool BattleInst::SetRibbon( u32 ribbon_id, Savedata::BattleInstMode mode )
{
  // @fix 森本さん要望：バトルロイヤルとツリーを分ける必要がなくなったのでUNUSE
  GFL_UNUSED( mode );

  //バトルハウス系のリボン以外は無視する
  if( (ribbon_id != PokeTool::RIBBON_NO_NIJI_BTLTOWER) &&
      (ribbon_id != PokeTool::RIBBON_NO_NIJI_MASTER) &&
      (ribbon_id != PokeTool::RIBBON_NO_NIJI_ROYAL)
     )
  {
    return false;
  }

  // @fix 森本さん要望：以下の処理は、バトルロイヤルもチーム選択の結果からポケモンを選択するのでツリーの処理と同じになりました

  u32 selectTeamIdx = mData.TeamSelectEventAppParam.out.teamIdx;

  // 選択チーム番号が不正かチェック
  if( selectTeamIdx == NetApp::TeamSelect::TEAM_SELECT_ERROR)
  {
    GFL_ASSERT( 0);
    return false;
  }

  // 選択チーム番号がQRかチェック
  if( selectTeamIdx == NetApp::TeamSelect::TEAM_SELECT_QR)
  {
    // 何もしない
    return false;
  }

  // 手持ち
  if( selectTeamIdx == NetApp::TeamSelect::TEAM_SELECT_TEMOCHI)
  {
    return this->SetRibbon_MyParty( ribbon_id);
  }
  // バトルチーム
  else 
  {
    return this->SetRibbon_BattleTeam( ribbon_id, selectTeamIdx);
  }
}

//--------------------------------------------------------------
/**
 * @brief バトルツリータイプの取得
 */
//--------------------------------------------------------------
Savedata::BattleTreeType BattleInst::GetBattleTreeType()
{
  switch( this->GetBattleRule())
  {
  case Regulation::BATTLE_RULE_SINGLE:
    return Savedata::BATTLE_TREE_TYPE_SINGLE;

  case Regulation::BATTLE_RULE_DOUBLE:
    return Savedata::BATTLE_TREE_TYPE_DOUBLE;

  case Regulation::BATTLE_RULE_MULTI:
    return Savedata::BATTLE_TREE_TYPE_MULTI;

  default:
    GFL_ASSERT_MSG( 0, "Invalid battle rule");
    return Savedata::BATTLE_TREE_TYPE_SINGLE;
  }
}

//--------------------------------------------------------------
/**
 * @brief バトルツリーランクの取得
 */
//--------------------------------------------------------------
Savedata::BattleTreeRank BattleInst::GetBattleTreeRank()
{
  if( mData.SuperRank)
  {
    return Savedata::BATTLE_TREE_RANK_SUPER;
  }
  return Savedata::BATTLE_TREE_RANK_NORMAL;
}


//----------------------------------------------------------------------
/*
 *  @brief  参加メンバー選択結果取得
 *  @param	app::pokelist::Result* p_result 結果
 */
//----------------------------------------------------------------------
bool BattleInst::SetPokeSelectResult()
{
  App::PokeList::CONTEXT_PARAM* p_result = GetPokeListContextParam();
  if( p_result->end_mode != App::PokeList::END_MODE_DECIDE ) return false; //キャンセル

  mData.EntryPokeData.entry_num = 0;
  for( int i = 0;i < pml::PokeParty::MAX_MEMBERS; ++i ){
    if( p_result->join_order[i] >= 0 ){
      mData.EntryPokeData.entry_poke_pos[mData.EntryPokeData.entry_num++] = p_result->join_order[i];
    }
  }
  return true;
}

//----------------------------------------------------------------------
/*
*  @brief  ロイヤル選択ランク取得
*/
//----------------------------------------------------------------------
Savedata::BattleRoyalRank BattleInst::GetSelectRankRoyal()
{
  switch( mData.RegulationDrawInfo.GetRegulation().GetBattleCategory())
  {
  case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_NORMAL:
    return Savedata::BATTLE_ROYAL_RANK_NORMAL;
  case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_SUPER:
    return Savedata::BATTLE_ROYAL_RANK_SUPER;
  case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_HYPER:
    return Savedata::BATTLE_ROYAL_RANK_HYPER;
  case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_MASTER:
    return Savedata::BATTLE_ROYAL_RANK_MASTER;
  default:
    GFL_ASSERT( 0);
    return Savedata::BATTLE_ROYAL_RANK_NORMAL;
  }
}

//----------------------------------------------------------------------
/*
 *  @brief  トレーナー名メッセージデータを作成
 */
//----------------------------------------------------------------------
void BattleInst::InitTrainerMsgData( void )
{
  GFL_ASSERT(mpTrainerNameMsgData==NULL);

  mpTrainerNameMsgData = GFL_NEW(mp_heap) gfl2::str::MsgData( print::GetMessageArcId(),
                                                                             GARC_message_tower_trainer_name_DAT, mp_heap,
                                                                             gfl2::str::MsgData::LOAD_FULL );
}

//----------------------------------------------------------------------
/*
 *  @brief  トレーナー名メッセージデータを削除
 */
//----------------------------------------------------------------------
void BattleInst::FinalizeTrainerMsgData( void )
{
  GFL_SAFE_DELETE( mpTrainerNameMsgData );
}

//----------------------------------------------------------------------
/*
*  @brief ランク別トレーナーIDオフセット取得 ロイヤル
*/
//----------------------------------------------------------------------
u32 BattleInst::GetTrainerIDRankOffsetRoyal( Savedata::BattleRoyalRank rank ) const
{
  switch( rank)
  {
  case Savedata::BATTLE_ROYAL_RANK_NORMAL:
    return 0;
  case Savedata::BATTLE_ROYAL_RANK_SUPER:
    return 10;
  case Savedata::BATTLE_ROYAL_RANK_HYPER:
    return 20;
  case Savedata::BATTLE_ROYAL_RANK_MASTER:
    return 20;
  default:
    GFL_ASSERT( 0);
    return 0;
  }
}

//----------------------------------------------------------------------
/*
*  @brief ランク別トレーナーID範囲サイズ取得 ロイヤル
*/
//----------------------------------------------------------------------
u32 BattleInst::GetTrainerIDRankRangeSizeRoyal( Savedata::BattleRoyalRank rank ) const
{
  switch( rank)
  {
  case Savedata::BATTLE_ROYAL_RANK_NORMAL:
    return 20;
  case Savedata::BATTLE_ROYAL_RANK_SUPER:
    return 20;
  case Savedata::BATTLE_ROYAL_RANK_HYPER:
    return 20;
  case Savedata::BATTLE_ROYAL_RANK_MASTER:
    return 30;
  default:
    GFL_ASSERT( 0);
    return 0;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief ボス戦かどうか ロイヤル
 *  @retval true ボス戦
 *  @retval false それ以外
 */
//-----------------------------------------------------------------------------
bool BattleInst::IsBossRoyal()
{
  GameSys::GameData* p_gdata = mp_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();

  Savedata::BattleRoyalRank rank = this->GetSelectRankRoyal();
  bool isWinCountMax = sv->IsMaxWinCountRoyal();

  return ( rank == Savedata::BATTLE_ROYAL_RANK_MASTER && isWinCountMax );
}

//----------------------------------------------------------------------------
/**
 *  @brief バトル開始 ロイヤル
 */
//-----------------------------------------------------------------------------
void BattleInst::StartBattleRoyal( void )
{
  GameSys::GameData * gameData = mp_gman->GetGameData();

  // トレーナーの選出とROMデータ作成
  this->SelectTrainerAllRoyal();

  // バトルセットアップパラメーターをクリアしておく
  BATTLE_SETUP_Clear( &(mData.SetupParam) );
  // @fix GFNMcat[1884] バトルツリー、バトルロイヤルではお手入れ禁止
  BATTLE_PARAM_SetBtlStatusFlag( &(mData.SetupParam), BTL_STATUS_FLAG_OTEIRE_DISABLE );

  // 一時的なMyパーティー
  pml::PokeParty* my_party = reinterpret_cast<pml::PokeParty*>( GFL_NEW_LOW(mp_heap) pml::PokeParty(mp_heap) );

  //自分のポケパーティを作成する
  this->SetPokeSelectResult();
  MakeMyPokeParty( false, &(mData.EntryPokeData), my_party );

  // フィールドシチュレーション設定
  const Field::ZoneDataLoader* cp_zone_loader = gameData->GetFieldZoneDataLoader();
  BTL_FIELD_SITUATION sit;
  Field::SetUpFieldSituation( &sit, mp_gman, &cp_zone_loader, Field::ATTR_ERROR, btl::bg::FAR_TYPE_NONE, btl::bg::NEAR_TYPE_NONE );

  // セットアップ
  BATTLE_SETUP_PARAM* bp = &(mData.SetupParam);
  BTL_SETUP_BattleRoyalInst( bp, mp_gman, &sit, my_party, mp_heap);

  // レギュレーションに合わせる
  BATTLE_PARAM_SetRegulation( bp, &(mData.RegulationDrawInfo.GetRegulation()), mp_heap);

  //コピー終了。パーティ解放
  GFL_DELETE( my_party );

  // バトル施設イベント呼び出し
  Field::EventBattleInstBattleCall* p_event =
    reinterpret_cast<Field::EventBattleInstBattleCall*>(GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventBattleInstBattleCall>( mp_gman->GetGameEventManager() ));
  p_event->SetUpBattleParam( &(mData.SetupParam) );
}

//----------------------------------------------------------------------
/*
 *  @brief ロイヤル用の生成処理
*/
//----------------------------------------------------------------------
void BattleInst::CreateForRoyal( void )
{
  if( mpTrainerNameMsgDataRoyal == NULL)
  {
    mpTrainerNameMsgDataRoyal = GFL_NEW(mp_heap) gfl2::str::MsgData(
      print::GetMessageArcId(),
      GARC_message_royal_trainer_name_DAT,
      mp_heap,
      gfl2::str::MsgData::LOAD_FULL
      );
  }
}

//----------------------------------------------------------------------
/*
 *  @brief ロイヤル用の破棄処理
*/
//----------------------------------------------------------------------
void BattleInst::DeleteForRoyal( void )
{
  GFL_SAFE_DELETE( mpTrainerNameMsgDataRoyal );
}

//----------------------------------------------------------------------------
/**
 *  @brief バトルロイヤル結果アプリ用パラメーターの設定
 */
//-----------------------------------------------------------------------------
void BattleInst::SetupBattleRoyalResultParam( void )
{
  const BattleRoyalResult& result = mData.SetupParam.battleRoyalResult;

  // 自分の順位を設定
  if( result.clientRanking[BTL_CLIENT_PLAYER] == 0)
  {
    mData.BattleRoyalResultParam.selfRanking = 1;
  }
  else
  {
    mData.BattleRoyalResultParam.selfRanking = 0;
  }

  // 各トレーナーごとの設定
  for( u32 cntClient = BTL_CLIENT_ENEMY1; cntClient < BTL_CLIENT_ROYAL_NUM; ++cntClient)
  {
    const BSP_TRAINER_DATA* trData = mData.SetupParam.tr_data[ cntClient ];

    App::BattleRoyalResult::TRINER_BTL_RESULT_DATA* pTrResultData = &(mData.BattleRoyalResultParam.aTrainerData[ cntClient ]);

    pTrResultData->ranking = result.clientRanking[cntClient] + 1; // アプリ側は1オリジンなのでプラス1
    pTrResultData->trainerType = static_cast<trainer::TrType>( trData->GetTrainerType());
    pTrResultData->correrColor = GetCornerColorFromBtlClientId( static_cast<BTL_CLIENT_ID>( cntClient) );
    pTrResultData->pNameBuf = GFL_NEW( mp_heap) gfl2::str::StrBuf( *(trData->GetName()), mp_heap);

    // バトルロイヤル仕様変更対応：倒したポケモン数の設定
    pTrResultData->killCount = result.killCount[ cntClient ];

    for( u32 cntPoke = 0; cntPoke < App::BattleRoyalResult::POKE_DATA_MAX; ++cntPoke)
    {
      App::BattleRoyalResult::POKE_DATA* pPokeData = &pTrResultData->aPokeData[cntPoke];

      pPokeData->hp = result.restHp[ cntClient ][ cntPoke ];
      pPokeData->maxHp = result.maxHp[ cntClient ][ cntPoke ];

      pml::pokepara::PokemonParam* param = mData.SetupParam.party[cntClient]->GetMemberPointer( cntPoke );
      pPokeData->simpleParam.monsNo = param->GetMonsNo();
      pPokeData->simpleParam.formNo = param->GetFormNo();
      pPokeData->simpleParam.sex = param->GetSex();
    }
  }

  // プレイヤーの設定
  {
    App::BattleRoyalResult::TRINER_BTL_RESULT_DATA* pTrResultData = &(mData.BattleRoyalResultParam.aTrainerData[ BTL_CLIENT_PLAYER ]);
    const Savedata::MyStatus* pMyStatus = mp_gman->GetGameData()->GetPlayerStatusConst();

    pTrResultData->ranking = result.clientRanking[ BTL_CLIENT_PLAYER ] + 1; // アプリ側は1オリジンなのでプラス1
    if( pMyStatus->GetSex() == PM_MALE)
    {
      pTrResultData->trainerType = trainer::TRTYPE_HERO;
    }
    else
    {
      pTrResultData->trainerType = trainer::TRTYPE_HEROINE;
    }
    pTrResultData->iconObjData = pMyStatus->GetIcon();
    pTrResultData->correrColor = GetCornerColorFromBtlClientId( BTL_CLIENT_PLAYER );

    gfl2::str::STRCODE strcode[ System::STRLEN_PLAYER_NAME + System::EOM_LEN ];
    pMyStatus->GetNameString( strcode);
    pTrResultData->pNameBuf = GFL_NEW( mp_heap) gfl2::str::StrBuf( strcode, mp_heap);

    // バトルロイヤル仕様変更対応：倒したポケモン数の設定
    pTrResultData->killCount = result.killCount[ BTL_CLIENT_PLAYER ];

    for( u32 cntPoke = 0; cntPoke < App::BattleRoyalResult::POKE_DATA_MAX; ++cntPoke)
    {
      App::BattleRoyalResult::POKE_DATA* pPokeData = &pTrResultData->aPokeData[cntPoke];

      s32 pos = mData.EntryPokeData.entry_poke_pos[cntPoke];

      pPokeData->hp = result.restHp[ BTL_CLIENT_PLAYER ][ cntPoke ];
      pPokeData->maxHp = result.maxHp[BTL_CLIENT_PLAYER][ cntPoke ];

      pml::pokepara::PokemonParam* param = mpSelectParty->GetMemberPointer( pos );
      pPokeData->simpleParam.monsNo = param->GetMonsNo();
      pPokeData->simpleParam.formNo = param->GetFormNo();
      pPokeData->simpleParam.sex = param->GetSex();
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief バトルロイヤルでエントリーした中で先頭のモンスターNoを取得
 */
//-----------------------------------------------------------------------------
u16 BattleInst::GetBattleRoyalEntoryFirstMonsNo( void )
{
  u32 pos = mData.EntryPokeData.entry_poke_pos[0];

  // @fix GFNMCat[5451] 手持ち限定になっていたため、選択パーティーを使うように修正
  return mpSelectParty->GetMemberPointer( pos)->GetMonsNo();
}

//----------------------------------------------------------------------------
/**
 *  @brief 戦うトレーナーをバトルセットアップパラメータにセットする ロイヤル
 *  @param int tr_no トレーナーナンバー
 *  @param dst 格納先トレーナーデータポインタ
 *  @param pHeap ヒープ
 */
//-----------------------------------------------------------------------------
void BattleInst::SetVsTrainerRoyal( int tr_no, BSP_TRAINER_DATA* dst, gfl2::heap::HeapBase* pHeap )
{
  u32 tr_id = mData.RoyalEnemyTrainerParam.trID[tr_no];

  dst->SetTrainerID( tr_id );
  dst->SetAIBit( (AI_BIT | BTL_AISCRIPT_BIT_WAZA_ROYAL) );
  dst->LoadTrTypeData( pHeap, static_cast<trainer::TrType>(mData.RoyalEnemyTrainerParam.romData[tr_no]->tr_type) );
  dst->SetName( mpTrainerNameMsgDataRoyal, tr_id );

  // @fix NMCat[2453] メッセージID変換にオフセットは不要。tr_id(タワートレーナーデータインデックス)だけで良い
  // 勝ち負けメッセージセット
  dst->SetMsgID(
    GARC_message_royal_trainer_DAT,
    TrainerIDtoWinStrID( tr_id ),
    TrainerIDtoLoseStrID( tr_id )
    );

  //トレーナーはアイテム所持していない
}

//--------------------------------------------------------------
/**
 * @brief 対戦ポケモンセット ロイヤル
 * @param tr_no 0～2
 * @param party ポケパーティ
 * @param pHeap ヒープ
 */
//--------------------------------------------------------------
void BattleInst::SetVsPokemonRoyal( int tr_no, pml::PokeParty* party, gfl2::heap::HeapBase* pHeap )
{
  pml::pokepara::PokemonParam* pp_ary[ ROYAL_TEMOCHI_NUM ] = { NULL };

  MakeTrainerPokemonRoyal(
    mData.RoyalEnemyTrainerParam.romData[tr_no],
    mData.RoyalEnemyTrainerParam.trID[tr_no],
    pHeap,
    pp_ary
    );

  // パーティにセット
  for( u32 i = 0; i < ROYAL_TEMOCHI_NUM; ++i)
  {
    party->AddMember( *pp_ary[i] );
    GFL_DELETE( pp_ary[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief 対戦トレーナー決定 ロイヤル
 *  @param int data_idx データインデックス
 *  @param bool isBoss ボスかどうか
 *  @return no 対戦トレーナーナンバー
 */
//-----------------------------------------------------------------------------
int BattleInst::SelectTrainerRoyal( int data_idx, bool isBoss )
{
   int no = 0;

  // ボスは固定
  if( isBoss)
  {
    no = 50; // ロイヤルマスク
  }
  // それ以外はランクから求める
  else
  {
    Savedata::BattleRoyalRank rank = this->GetSelectRankRoyal();
    u32 offset = this->GetTrainerIDRankOffsetRoyal( rank);
    u32 width = this->GetTrainerIDRankRangeSizeRoyal( rank);
    no = this->SelectTrainerCore( data_idx, offset, width);
  }

  return no;
}

//----------------------------------------------------------------------------
/**
 *  @brief 対戦相手の決定
 *  @param int set_slot_idx
 *  @param data_idx 
 */
//-----------------------------------------------------------------------------
void BattleInst::MakeTrainerDataRoyal( int set_slot_idx, int data_idx )
{
  //トレーナーデータセット
  mData.RoyalEnemyTrainerParam.trID[set_slot_idx] = data_idx;

  int arc_id = ARCID_BATTLE_ROYAL_TRAINER;
  s32 size_align = 4;
  size_t size = BattleInstTool::GetTrainerRomDataSize( mp_heap, mp_gman->GetAsyncFileManager(), arc_id, data_idx);
  if( size > 0)
  {
    if( mData.RoyalEnemyTrainerParam.romData[set_slot_idx] )
    {
      GflHeapFreeMemory( mData.RoyalEnemyTrainerParam.romData[set_slot_idx] );
      mData.RoyalEnemyTrainerParam.romData[set_slot_idx] = NULL;
    }
    mData.RoyalEnemyTrainerParam.romData[set_slot_idx] = (BINST_TRAINER_ROM_DATA  *)( GflHeapAllocMemoryLowAlign( mp_heap, size, size_align ) );
    BattleInstTool::GetTrainerRomData( mp_heap, mp_gman->GetAsyncFileManager(), arc_id, data_idx, mData.RoyalEnemyTrainerParam.romData[set_slot_idx], size);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "data size <= 0\n");
  }

  mData.RoyalEnemyTrainerParam.isBoss = false;
}

//----------------------------------------------------------------------------
/**
 *  @brief ボス戦フラグのセット ロイヤル
 */
//-----------------------------------------------------------------------------
void BattleInst::SetBossFlagRoyal( bool isBoss)
{
  mData.RoyalEnemyTrainerParam.isBoss = isBoss;
}

//----------------------------------------------------------------------------
/**
 *  @brief 対戦トレーナー全ての決定 ロイヤル
 */
//-----------------------------------------------------------------------------
void BattleInst::SelectTrainerAllRoyal( void )
{
  bool is_boss_join = this->IsBossRoyal();

  int data_index = 0;
  data_index = this->SelectTrainerRoyal( -1, is_boss_join);
#if PM_DEBUG
  data_index = this->Debug_GetRoyalForceVsTrainerNo( 0, data_index );
#endif // PM_DEBUG
  this->MakeTrainerDataRoyal( 0, data_index);
  this->SetNoEntryTrainer( 0, data_index);

  data_index = this->SelectTrainerRoyal( -1, false);
#if PM_DEBUG
  data_index = this->Debug_GetRoyalForceVsTrainerNo( 1, data_index );
#endif // PM_DEBUG
  this->MakeTrainerDataRoyal( 1, data_index);
  this->SetNoEntryTrainer( 1, data_index);

  data_index = this->SelectTrainerRoyal( -1, false);
#if PM_DEBUG
  data_index = this->Debug_GetRoyalForceVsTrainerNo( 2, data_index );
#endif // PM_DEBUG
  this->MakeTrainerDataRoyal( 2, data_index);
}

//----------------------------------------------------------------------------
/**
 *  @brief 対戦ポケモン作成 ロイヤル
 *  @param td ROMトレーナーデータポインタ
 *  @param tr_id トレーナーID
 *  @param set_poke_no 決定したポケモン配列
 *  @param set_item_no 決定したアイテム配列
 *  @param pHeap ヒープ
 *  @param pp_ary ポケパラポインタ配列
 *  @return bool 50回抽選をした場合 true
 */
//-----------------------------------------------------------------------------
bool BattleInst::MakeTrainerPokemonRoyal(
  BINST_TRAINER_ROM_DATA *td,
  int tr_id,
  gfl2::heap::HeapBase* pHeap,
  pml::pokepara::PokemonParam** pp_ary
  )
{
  const s32 LOOP_MAX = 500;
  s32 loop_count = 0;

  s32 set_count = 0;
  s32 entry_mons_no[ ROYAL_TEMOCHI_NUM ] = { MONSNO_NULL };
  s32 entry_item_no[ ROYAL_TEMOCHI_NUM ] = { ITEM_DUMMY_DATA };

  bool isUseObon = false;
  bool isUseKiainotasuki = false;
  bool isUseRam = false;

  while( set_count < ROYAL_TEMOCHI_NUM )
  {
    BINST_POKEMON_ROM_DATA pd;
    int poke_index = System::GflUse::GetPublicRand(td->use_poke_cnt);
    int set_index = td->use_poke_table[ poke_index ];
    BattleInstTool::GetPokemonRomData(mp_heap, mp_gman->GetAsyncFileManager(), ARCID_BATTLE_ROYAL_POKE, set_index, &pd);

    //モンスターナンバーのチェック（同一のポケモンは持たない）
    bool isMonsOverlap = false;
    for( s32 i = 0; i < set_count; ++i)
    {
      GFL_ASSERT(pd.mons_no != MONSNO_NULL);
      if( entry_mons_no[i] == pd.mons_no )
      {
        isMonsOverlap = true;
        break; //抽選済み
      }
    }
    // 重複したら再抽選。ただしループ上限に達していたら重複チェックはしない
    if( isMonsOverlap && loop_count < LOOP_MAX )
    {
      loop_count++;
      continue;
    }

    // 装備アイテムのチェック（同一のアイテムは持たない）
    bool isItemOverlap = false;
    for( s32 i = 0; i < set_count; ++i)
    {
      if( pd.item_no &&
          (entry_item_no[i] == pd.item_no)
        )
      {
        isItemOverlap = true;
        break; //アイテム重複
      }
    }

    // アイテムが重複していたらオボンのみorきあいのたすきorラムのみに変更
    s16 fix_item_no = pd.item_no;
    if( isItemOverlap)
    {
      if( isUseObon == false)
      {
        fix_item_no = ITEM_OBONNOMI;
      }
      else if( isUseKiainotasuki == false)
      {
        fix_item_no = ITEM_KIAINOTASUKI;
      }
      else if( isUseRam == false)
      {
        fix_item_no = ITEM_RAMUNOMI;
      }
    }

    entry_mons_no[ set_count ] = pd.mons_no;
    entry_item_no[ set_count ] = fix_item_no;

    // 重複したときにセットするアイテムの使用をチェック
    if( fix_item_no == ITEM_OBONNOMI)
    {
      isUseObon = true;
    }
    else if( fix_item_no == ITEM_KIAINOTASUKI)
    {
      isUseKiainotasuki = true;
    }
    else if( fix_item_no == ITEM_RAMUNOMI)
    {
      isUseRam = true;
    }

    //格納配列にセット
    {
      pml::pokepara::PokemonParam* pp;
      u8 pow = this->GetPowerRndRoyal( tr_id );
      pp = BattleInstTool::MakePokemonParam( &pd, 50, pow, pHeap );//特性, 性別指定しない
      pp->SetItem( fix_item_no); // ここでアイテムを上書きする

      // NMCat[747] バトルツリーで相手トレーナーの登場演出と実際に投げるボールが異なる
      // トレーナータイプデータから取得したボールIDをセットする
      item::BALL_ID ball_id = this->GetBallIDByTrainerRomData( pHeap->GetLowerHandle(), td );
      pp->SetGetBall( ball_id );

      pp_ary[set_count] = pp;
    }

    set_count++;
  } //end while

  if( loop_count >= LOOP_MAX )
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------
/*
*  @brief  送信データ取得
*/
//----------------------------------------------------------------------
void BattleInst::GetSendCommData( CommSendData *pDest )
{
  GameSys::GameData* p_gdata = mp_gman->GetGameData();
  Savedata::MyStatus* pMyStatus = p_gdata->GetPlayerStatus();
  pMyStatus->Serialize( pDest->SerializeMyStatus );

  mCommDataMine.Party->SerializeFull( &pDest->Party );
  pDest->SuperRank = mCommDataMine.SuperRank;
  pDest->WinNum = mCommDataMine.WinNum;
}

//----------------------------------------------------------------------
/*
*  @brief  通信マルチ対戦トレーナーの取得
*  @return データの出力先
*/
//----------------------------------------------------------------------
void BattleInst::SetCommVsTrainer( CommSendVsTrainer* pSrc )
{
  SetCommVsTrainerNo( pSrc->no[0], pSrc->no[1] );
  mCommVsTrainer.Party1->DeserializeFull(&pSrc->Party1);
  mCommVsTrainer.Party2->DeserializeFull(&pSrc->Party2);
}

//----------------------------------------------------------------------
/*
*  @brief  通信マルチ対戦トレーナーの取得
*  @return データの出力先
*/
//----------------------------------------------------------------------
void BattleInst::GetCommVsTrainer( CommSendVsTrainer* pDest )
{
  GetCommVsTrainerNo( &pDest->no[0], &pDest->no[1] );
  mCommVsTrainer.Party1->SerializeFull(&pDest->Party1);
  mCommVsTrainer.Party2->SerializeFull(&pDest->Party2);
}

//----------------------------------------------------------------------
/*
 *  @brief  送信バッファにデータセット
 */
//----------------------------------------------------------------------
void  BattleInst::SetupSendData( void )
{
  //自分のポケパーティを作成する
  MakeMyPokeParty( false, &mData.EntryPokeData, mCommDataMine.Party );
  ONOUE_PRINT("送信したパーティ:%d_%d - isParent = %d\n",mCommDataMine.Party->GetMemberPointer(0)->GetMonsNo(),mCommDataMine.Party->GetMemberPointer(1)->GetMonsNo(),mIsParent);
  //ランクセット
  mCommDataMine.SuperRank = mData.SuperRank;
  //通信マルチ連勝数セット
  {
    GameSys::GameData* p_gdata = mp_gman->GetGameData();
    Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
    //連勝数取得
    int win = sv->GetWinCount( this->GetBattleTreeType(), this->GetBattleTreeRank() );
    mCommDataMine.WinNum = win;
    if ( !mData.SuperRank )
    {
      if ( mCommDataMine.WinNum > 19 )
      {
        GFL_ASSERT_MSG(0,"COMM_SEND WINCNT_OVER %d",mData.CommWinCount);
        mCommDataMine.WinNum = 0;
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
*  @brief  受信データを反映
*/
//----------------------------------------------------------------------
void BattleInst::SetCommData( CommSendData *pSrc )
{
  mCommDataYours.MyStatus->Deserialize( pSrc->SerializeMyStatus );
  mCommDataYours.Data.Party->DeserializeFull( &pSrc->Party );
  mCommDataYours.Data.SuperRank = pSrc->SuperRank;
  mCommDataYours.Data.WinNum = pSrc->WinNum;

  ONOUE_PRINT("受信したパーティ:%d_%d - isParent = %d\n",mCommDataYours.Data.Party->GetMemberPointer(0)->GetMonsNo(),mCommDataYours.Data.Party->GetMemberPointer(1)->GetMonsNo(),mIsParent);
}

//----------------------------------------------------------------------
/*
*  @brief  現在の対戦相手をスカウトする
*/
//----------------------------------------------------------------------
void BattleInst::ScountCurrentTrainer()
{
  const int tr_no = 0; // 常に0番トレーナーをスカウト
  Savedata::BattleInstSave* p_save    = mp_gman->GetGameData()->GetBattleInstSave();
  BINST_TRAINER_ROM_DATA* pRomData = mData.TrainerParam.romData[tr_no];

  pml::pokepara::PokemonParam* pokeParam0 = mData.SetupParam.party[BTL_CLIENT_ENEMY1]->GetMemberPointer(0);
  pml::pokepara::PokemonParam* pokeParam1 = mData.SetupParam.party[BTL_CLIENT_ENEMY1]->GetMemberPointer(1);
  ONOUE_PRINT("SCOUNT0_MONS_NO = %d\n",pokeParam0->GetMonsNo());
  ONOUE_PRINT("SCOUNT1_MONS_NO = %d\n",pokeParam1->GetMonsNo());
  ONOUE_PRINT("SCOUNT0_TOKUSEI_INDEX = %d\n",pokeParam0->GetTokuseiIndex());
  ONOUE_PRINT("SCOUNT1_TOKUSEI_INDEX = %d\n",pokeParam1->GetTokuseiIndex());
  // @fix GFNMCat[1157] パートーナーのもちポケモンが抽選結果によらず上位の2匹になっていた問題を修正
  p_save->AddScoutTrainerData(
    GetTrainerID(tr_no),
    mData.TrainerParam.select_poke_index[0],mData.TrainerParam.select_poke_index[1],
    pokeParam0->GetTokuseiIndex(),pokeParam1->GetTokuseiIndex());
}

//----------------------------------------------------------------------
/*
*  @brief  リーリエをスカウトする
*/
//----------------------------------------------------------------------
void BattleInst::ScoutLilie()
{
  Savedata::BattleInstSave* p_save = mp_gman->GetGameData()->GetBattleInstSave();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  
  //トレーナーデータ作成
  BINST_TRAINER_ROM_DATA* pRomData = NULL;
  {
    s32 size_align = 4;
    size_t size = BattleInstTool::GetTrainerRomDataSize(p_heap, mp_gman->GetAsyncFileManager(), ARCID_BATTLE_TREE_TRAINER, BattleInstDefine::BINST_LILIE_DATA_ID);
    if (size > 0)
    {
      pRomData = (BINST_TRAINER_ROM_DATA  *)(GflHeapAllocMemoryLowAlign(p_heap, size, size_align));
      BattleInstTool::GetTrainerRomData(p_heap, mp_gman->GetAsyncFileManager(), ARCID_BATTLE_TREE_TRAINER, BattleInstDefine::BINST_LILIE_DATA_ID, pRomData, size);
    }
    else
    {
      GFL_ASSERT_MSG(0, "data size <= 0\n");
    }
  }

  if (pRomData == NULL)
  {
    return;
  }

  // ポケモンデータ作成
  // リーリエは２体しかポケモンを持っていない特殊トレーナー
  const u32 POKEMON_NUM = 2;
  pml::pokepara::PokemonParam *pp_ary[POKEMON_NUM];
  for (u32 index(0); index < POKEMON_NUM; ++index)
  {
    BINST_POKEMON_ROM_DATA  pd;
    BattleInstTool::GetPokemonRomData(p_heap, mp_gman->GetAsyncFileManager(), ARCID_BATTLE_TREE_POKE, pRomData->use_poke_table[index], &pd);

    //格納配列にセット
    pml::pokepara::PokemonParam* pp = NULL;
    // 個体値
    u8 pow = 0x1f;
    pp = BattleInstTool::MakePokemonParam(&pd, 50, pow, p_heap);//特性, 性別指定しない

    // NMCat[747] バトルツリーで相手トレーナーの登場演出と実際に投げるボールが異なる
    // トレーナータイプデータから取得したボールIDをセットする
    item::BALL_ID ball_id = this->GetBallIDByTrainerRomData(p_heap->GetLowerHandle(), pRomData);
    pp->SetGetBall(ball_id);

    pp_ary[index] = pp;
  }

  MUNAKATA_PRINT("SCOUNT0_MONS_NO = %d\n", pp_ary[0]->GetMonsNo());
  MUNAKATA_PRINT("SCOUNT1_MONS_NO = %d\n", pp_ary[1]->GetMonsNo());
  MUNAKATA_PRINT("SCOUNT0_TOKUSEI_INDEX = %d\n", pp_ary[0]->GetTokuseiIndex());
  MUNAKATA_PRINT("SCOUNT1_TOKUSEI_INDEX = %d\n", pp_ary[1]->GetTokuseiIndex());
  // リーリエ追加
  p_save->AddScoutTrainerData(
    BattleInstDefine::BINST_LILIE_DATA_ID,
    pRomData->use_poke_table[0], pRomData->use_poke_table[1],
    pp_ary[0]->GetTokuseiIndex(), pp_ary[1]->GetTokuseiIndex());

  for (u32 index(0); index < POKEMON_NUM; ++index)
  {
    GFL_SAFE_DELETE(pp_ary[index]);
  }
  GflHeapSafeFreeMemory(pRomData);
}

//----------------------------------------------------------------------
/*
*  @brief  対戦相手のトレーナータイプ名を登録
*/
//----------------------------------------------------------------------
void BattleInst::WordSetBinstTrainerTypeNameFromTrainerNo(u16 buf_id,u16 tr_no)
{
  print::WordSet  *wset = mp_gman->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  wset->RegisterTrTypeName( buf_id, mData.TrainerParam.romData[tr_no]->tr_type );
}

//----------------------------------------------------------------------
/*
*  @brief  対戦相手のトレーナー名を登録
*/
//----------------------------------------------------------------------
void BattleInst::WordSetBinstTrainerNameFromTrainerNo(u16 buf_id,u16 tr_no)
{
  print::WordSet  *wset = mp_gman->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  wset->RegisterWord( buf_id, mpTrainerNameMsgData, mData.TrainerParam.trID[tr_no] );
}

//----------------------------------------------------------------------
/*
 * @brief  対戦相手のトレーナータイプ名とトレーナー名を登録
 */
//----------------------------------------------------------------------
void BattleInst::WordSetBinstTrainerTypeNameAndTrainerNameFromTrainerNo(u16 dst_buf_id,u16 type_name_buf_id,u16 name_buf_id,u16 tr_no)
{
  this->WordSetBinstTrainerTypeNameFromTrainerNo( type_name_buf_id, tr_no );
  this->WordSetBinstTrainerNameFromTrainerNo( name_buf_id, tr_no );

  print::WordSet  *wset = mp_gman->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  wset->RegisterTrTypeNameAndTrainerName( dst_buf_id, type_name_buf_id, name_buf_id );
}

//----------------------------------------------------------------------------
/**
 *  @brief 参加ポケモンパーティー(バトルに渡したパーティー)内のポケモンindexをSelectParty内のポケモンindexに変換する
 *  @param 参加ポケモンパーティー(バトルに渡したパーティー)内のポケモンindex
 *  @return SelectParty内のポケモンindex
 */
//-----------------------------------------------------------------------------
s32 BattleInst::ConvertEntryPartyIndexToSelectPartyIndex( u32 index ) const
{
  if( index >= pml::PokeParty::MAX_MEMBERS )
  {
    GFL_ASSERT( 0 );
    return mData.EntryPokeData.entry_poke_pos[ 0 ];
  }

  return mData.EntryPokeData.entry_poke_pos[ index ];
}

//----------------------------------------------------------------------------
/**
 *  @brief トレーナーIDに応じたパワー乱数の取得 ロイヤル
 */
//-----------------------------------------------------------------------------
u8 BattleInst::GetPowerRndRoyal( u16 trID)
{
  u8  pow_rnd = 0;
  const u8 pow_max = pml::MAX_TALENT_POWER;
  if( trID < 10)
  {
    // 最大値の5/8
    pow_rnd = ( pow_max * 6 ) / 8;
  }
  else if( trID < 20)
  {
    // 最大値の7/8
    pow_rnd = ( pow_max * 7 ) / 8;
  }
  else if( trID < 50)
  {
    // 最大値の8/8
    pow_rnd = ( pow_max * 8 ) / 8;
  }
  else
  {
    // ボス
    // 最大値の8/8
    pow_rnd = ( pow_max * 8 ) / 8;
  }

  return pow_rnd;
}

//----------------------------------------------------------------------------
/**
 *  @brief リボンのセット バトルチーム
 *  @param ribbonID リボンID
 *  @param selectTeamIdx 選択チーム番号
 *  @retval true 1匹でもリボンをセットした
 *  @retval false どのポケモンにもリボンをセットしていない(既にセットされていた)
 */
//-----------------------------------------------------------------------------
bool BattleInst::SetRibbon_BattleTeam( u32 ribbonID, u32 selectTeamIdx)
{
  const EntryPokeData& entry_data = mData.EntryPokeData;
  Savedata::BOX* pBoxSave = mp_gman->GetGameData()->GetBOX();
  Savedata::BoxPokemon* pBoxPokemonSave = mp_gman->GetGameData()->GetBoxPokemon();

  bool isLock = pBoxSave->IsTeamLock( selectTeamIdx);
  if( isLock)
  {
    // ロックされているバトルチームは施設で使用できない
    GFL_ASSERT( 0);
    return false;
  }

  bool isSetRibbon = false;

  for( int i = 0; i < entry_data.entry_num; i++)
  {
    u32 partyPos = entry_data.entry_poke_pos[i];
    u16 teamPokePos = pBoxSave->GetTeamPokePos( selectTeamIdx, partyPos);

    if( teamPokePos == Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE)
    {
      // チームにポケモンがいない
      GFL_ASSERT( 0);
      continue;
    }

    u8 tray = (teamPokePos >> 8) & 0xFF;
    u8 pos  = (teamPokePos     ) & 0xFF;

    pml::pokepara::CoreParam* pp;
    pp = pBoxPokemonSave->GetPokemon( tray, pos, mp_heap->GetLowerHandle());

    if( pp->HaveRibbon( ribbonID) == false && pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG) == false)
    {
      pp->SetRibbon( ribbonID);
      isSetRibbon = true;
    }

    pBoxPokemonSave->UpdatePokemonConst( pp, tray, pos);

    GFL_DELETE( pp);
  }

  return isSetRibbon;
}

//----------------------------------------------------------------------------
/**
 *  @brief リボンのセット 自分のパーティ
 *  @param ribbonID リボンID
 *  @retval true 1匹でもリボンをセットした
 *  @retval false どのポケモンにもリボンをセットしていない(既にセットされていた)
 */
//-----------------------------------------------------------------------------
bool BattleInst::SetRibbon_MyParty( u32 ribbonID)
{
  const EntryPokeData& entry_data = mData.EntryPokeData;
  pml::PokeParty* party = mp_gman->GetGameData()->GetPlayerParty();

  bool isSetRibbon = false;

  for( s32 i = 0; i < entry_data.entry_num; i++)
  {
    u32 pos = entry_data.entry_poke_pos[i];
    pml::pokepara::PokemonParam* pp = party->GetMemberPointer( pos);

    if( pp->HaveRibbon( ribbonID) == false && pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG) == false)
    {
      pp->SetRibbon( ribbonID);
      isSetRibbon = true;
    }
  }

  return isSetRibbon;
}

//----------------------------------------------------------------------------
/**
 *  @brief トレーナーロムデータから捕獲ボールIDを取得
 *  @param p_heap 一時確保用ヒープ
 *  @param td トレーナーデータ
 *  @return 捕獲ボールID
 */
//-----------------------------------------------------------------------------
item::BALL_ID BattleInst::GetBallIDByTrainerRomData( gfl2::heap::HeapBase* p_heap, const BINST_TRAINER_ROM_DATA* td )
{
  trainer::TrType tr_type = static_cast<trainer::TrType>( td->tr_type );

  trainer::TrainerTypeData trData;
  trData.LoadData( p_heap->GetLowerHandle(), tr_type );

  return trData.GetBallID();
}

#if PM_DEBUG

void BattleInst::DEBUG_SetupSingleParty()
{
  mData.PokeListContextParam.end_mode = App::PokeList::END_MODE_DECIDE;
  mData.PokeListContextParam.join_order[0] = 0;
  mData.PokeListContextParam.join_order[1] = 1;
  mData.PokeListContextParam.join_order[2] = 2;

  const Savedata::MyStatus* pMyStatus = mp_gman->GetGameData()->GetPlayerStatusConst();
  GameSys::GameData* p_gdata = mp_gman->GetGameData();
  mpSelectParty->CopyFrom( *(p_gdata->GetPlayerParty()));
  mData.RegulationDrawInfo.GetRegulation().LoadData( Regulation::PRESET_BATTLE_TREE_SUPER_S );
  SetRule(Regulation::BATTLE_CATEGORY_TREE_SUPER,Regulation::BATTLE_RULE_SINGLE);
  SetPokeSelectResult();
}

//----------------------------------------------------------------------------
/**
 *  @brief デバッグ用。強制トレーナーNo取得
 *  @param slotIndex データ格納スロット番号
 *  @param dataIndex 抽選されたトレーナーNo
 *  @return 強制トレーナーNoが有効ならそれを返す。無効ならslotIndexを返す
 */
//-----------------------------------------------------------------------------
s32 BattleInst::Debug_GetRoyalForceVsTrainerNo( s32 slotIndex, s32 dataIndex )
{
  if( slotIndex >= GFL_NELEMS( Field::Debug::DebugTypes::s_battleRoyalInstDebugForceVsTrainerNo ) )
  {
    return dataIndex;
  }

  s32 forceDataIndex = Field::Debug::DebugTypes::s_battleRoyalInstDebugForceVsTrainerNo[ slotIndex ];
  if( forceDataIndex > 0 && forceDataIndex <= 51 )
  {
    dataIndex = forceDataIndex - 1;
  }
  return dataIndex;
}

#endif // PM_DEBUG

GFL_NAMESPACE_END( BattleInst )

