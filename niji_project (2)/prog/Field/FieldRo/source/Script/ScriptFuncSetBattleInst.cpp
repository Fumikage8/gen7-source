//======================================================================
/**
 * @file ScriptFuncSetBattleInst.cpp
 * @date 2016/01/10 15:27:21
 * @author onoue_masayuki
 * @brief バトル施設命令セット
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Script/ScriptFuncSetBattleInst.h"

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptWork.h"

#include "niji_reference_files/script/FieldPawnTypes.h"

#include "Field/FieldStatic/include/BattleInst/BattleInst.h"
#include "Field/FieldRos/FieldGimmickBattleInst/include/FieldGimmickBattleInst.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "NetStatic/NetEvent/include/TeamSelectEvent.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "App/AppEvent/include/BattleTree/BattleTreeEvent.h"
#include "App/AppEvent/include/FieldPokeList/FieldPokeListEvent.h"
#include "niji_reference_files/script/FieldPawnTypes.h"
#include "Field/FieldBattleInstEvent/include/EventBattleInst.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstManager.h"

// リーリエ追加用
#include "Field/FieldStatic/include/BattleInst/BattleInstDefine.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( FieldScript );

//-----------------------------------------------------------------------------
/**
 *  @brief  命令テーブルセットを取得
 */
//-----------------------------------------------------------------------------
const GFL_PAWN_FUNC_TABLE* FuncSetBattleInst::GetTable( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] =
  {
    GFL_PAWN_FUNC( BattleInstWorkCreate )
    GFL_PAWN_FUNC( BattleInstWorkDelete )

    GFL_PAWN_FUNC( GetRank )
    GFL_PAWN_FUNC( SetTrainer )
    GFL_PAWN_FUNC( StartBattle )
    GFL_PAWN_FUNC( AddTrainerObj )
    GFL_PAWN_FUNC( AddDressupTrainerObj )
    GFL_PAWN_FUNC( AddWinCount )
    GFL_PAWN_FUNC( ClearWinCount )
    GFL_PAWN_FUNC( GetWinCount )
    GFL_PAWN_FUNC( GetMyWinCount )
    GFL_PAWN_FUNC( GetMyMaxWinCount )
    GFL_PAWN_FUNC( IsWin )
    GFL_PAWN_FUNC( IsBoss )
    GFL_PAWN_FUNC( GetCharacterID )
    GFL_PAWN_FUNC( GetPartnerCharacterID )
    GFL_PAWN_FUNC( GetPartnerTrainerID )
    GFL_PAWN_FUNC( WaitCommYesNo )
    GFL_PAWN_FUNC( BattleInst_GetBattleStartMsgId )
    GFL_PAWN_FUNC( BattleInst_SetBgm )
    GFL_PAWN_FUNC( BattleInst_GetBgm )
    GFL_PAWN_FUNC( BattleInst_End )
    GFL_PAWN_FUNC( BattleInst_IsComm )

    GFL_PAWN_FUNC(GetResultRegulationSelectApp_)
    GFL_PAWN_FUNC(GetResultPokeListApp_)
    GFL_PAWN_FUNC(GetResultNpcSelectApp_)
    GFL_PAWN_FUNC(GetResultQuickMatchApp_)
    
    GFL_PAWN_FUNC(GetBattleRule)
    GFL_PAWN_FUNC(WaitModelLoad)
    GFL_PAWN_FUNC(ClearModelData)
    
    GFL_PAWN_FUNC(SetupAiPartner)
    GFL_PAWN_FUNC(SetRegulation)

    GFL_PAWN_FUNC(AddWinCountRoyal)
    GFL_PAWN_FUNC(ClearWinCountRoyal)
    GFL_PAWN_FUNC(GetWinCountRoyal)
    GFL_PAWN_FUNC(ReleaseNextRankRoyal)
    GFL_PAWN_FUNC(GetReleaseRankRoyal)
    GFL_PAWN_FUNC(GetSelectRankRoyal)
    GFL_PAWN_FUNC(GetPlayerRankingRoyal)
    GFL_PAWN_FUNC(GetLastPokePartyIndexRoyal)
    GFL_PAWN_FUNC(IsBossRoyal)
    GFL_PAWN_FUNC(StartBattleRoyal)
    GFL_PAWN_FUNC(CreateForRoyal)
    GFL_PAWN_FUNC(DeleteForRoyal)
    
    GFL_PAWN_FUNC(SendCommonData)
    GFL_PAWN_FUNC(TradeTarinerData)

    GFL_PAWN_FUNC(BattleInstDisconnect)
    GFL_PAWN_FUNC(IsBinstFullScoutTrainerData)
    GFL_PAWN_FUNC(IsBinstAllreadyScoutTrainerData)
    GFL_PAWN_FUNC(IsBinstAllreadyLilieScout)
    GFL_PAWN_FUNC(ScoutCurrentTrainer)
    GFL_PAWN_FUNC(ScoutLilie)
    GFL_PAWN_FUNC(RemoveSelectScoutTrainer)

    GFL_PAWN_FUNC(WordSetBinstTrainerTypeNameFromTrainerNo)
    GFL_PAWN_FUNC(WordSetBinstTrainerNameFromTrainerNo)
    GFL_PAWN_FUNC(WordSetBinstTrainerTypeNameAndTrainerNameFromTrainerNo)

    GFL_PAWN_FUNC(SetRibbonEntryPokeAll)

    GFL_PAWN_FUNC( SetPrizeWinCount )
    GFL_PAWN_FUNC( GetPrizeWinCount )
    GFL_PAWN_FUNC( ClearPrizeWinCount )

    GFL_PAWN_FUNC( BattleInst_GetPartnerStartMsgId )
    GFL_PAWN_FUNC( BattleInst_GetPartnerLoseMsgId )

    GFL_PAWN_FUNC( OpenBattleInstGimmick )
    GFL_PAWN_FUNC( CloseBattleInstGimmick )

    GFL_PAWN_FUNC( SetBinstPokeDiary )
    GFL_PAWN_FUNC( BattleInst_IsCommExit )
    GFL_PAWN_FUNC( BattleInst_IsNetworkError )
    GFL_PAWN_FUNC( BattleInst_GetCommCheckResult )
    GFL_PAWN_FUNC( DEBUG_SetupSingleParty )

    GFL_PAWN_FUNC( WordSetSelectPartyMonsNickName )
    GFL_PAWN_FUNC( BattleInst_GetIsParent )

    GFL_PAWN_FUNC_END,
  };

  return s_FuncTable;
}

GameSys::GameManager* FuncSetBattleInst::GetGameManager(AMX * amx)
{
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  return p_obj->GetGameManager();
}


int FuncSetBattleInst::GetWinCountFromSave(GameSys::GameManager* p_gman,const Savedata::BattleTreeType type, const Savedata::BattleTreeRank rank)
{
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  int winCount = 0;
  if ( p_inst->GetIsComm() )
  {
    winCount = p_inst->GetCommWinCount();
  }
  else
  {
    winCount = sv->GetWinCount( type, rank );
  }
  return winCount;
}
//-----------------------------------------------------------------------------
/**
 *  @brief ユーティリティワークを生成
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInstWorkCreate(AMX * amx, const cell * ptr)
{
  GameSys::GameManager* p_gman = GetGameManager(amx);

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  BattleInst::BattleInstManager* pInstManager = GFL_NEW_LOW( p_heap ) BattleInst::BattleInstManager();
  p_gman->SetBattleInstManager( pInstManager );
  p_gman->GetBattleInstManager()->Create( p_gman );
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ユーティリティワークを解放
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInstWorkDelete( AMX *amx, const cell * )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInstManager* pInstManager = p_gman->GetBattleInstManager();
  pInstManager->Delete();
  p_gman->SetBattleInstManager( NULL );
  GFL_SAFE_DELETE(pInstManager);
  
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();
  pScriptSystem->Suspend();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ランク取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetRank( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return (inst->GetRank() == Savedata::BATTLE_TREE_RANK_SUPER);
}

//-----------------------------------------------------------------------------
/**
 *	@brief  トレーナーセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::SetTrainer( AMX *amx, const cell * ptr )
{
  u16 rule = ptr[1];
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();

  //トレーナー抽選
  if ( static_cast<Savedata::BattleTreeType>(rule) != Savedata::BATTLE_TREE_TYPE_MULTI )   //マルチ以外
  {
    int data_idx = inst->SelectTrainer(-1);
    inst->MakeTrainerDataWithBoss( 0, data_idx );
    inst->SetNoEntryTrainer( 0, data_idx );//次の抽選から外す
  }
  else                                                      //マルチ
  {
    int data_idx1 = inst->SelectTrainer(-1);
    int data_idx2 = inst->SelectTrainer(data_idx1);
    inst->MakeTrainerDataWithBoss( 0, data_idx1 );
    inst->MakeTrainerDataWithBoss( 1, data_idx2 );

    inst->SetNoEntryTrainer( 0, data_idx1 );//次の抽選から外す
    inst->SetNoEntryTrainer( 1, data_idx2 );//次の抽選から外す

  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  戦闘開始
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::StartBattle( AMX *amx, const cell * ptr )
{
  bool lv_lower_fix = ptr[1];
  bool bgm_change = ptr[2];
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  inst->StartBattle( lv_lower_fix, bgm_change );
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  トレーナーOBJ追加
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::AddTrainerObj( AMX *amx, const cell * ptr )
{
  u16 index = ptr[1];
  u16 objid = ptr[2];
  float x = amx_ctof(ptr[3]);
  float y = amx_ctof(ptr[4]);
  float z = amx_ctof(ptr[5]);
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();

  inst->AddTrainerObj( static_cast<Field::BATTLE_INST_POS_INDEX>(index),objid, x, y, z );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  着せ替えトレーナーOBJ追加
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::AddDressupTrainerObj( AMX *amx, const cell * ptr )
{
  u16 index = ptr[1];
  float x = amx_ctof(ptr[2]);
  float y = amx_ctof(ptr[3]);
  float z = amx_ctof(ptr[4]);
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();

  inst->AddDressupTrainerObj( static_cast<Field::BATTLE_INST_POS_INDEX>(index), x, y, z );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  モデル読み込み待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::WaitModelLoad( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return inst->WaitModelLoad();
}

//-----------------------------------------------------------------------------
/**
 *	@brief  モデル情報削除
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::ClearModelData( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  inst->ClearModelData();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  連勝数インクリメント
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::AddWinCount( AMX *amx, const cell * )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  inst->AddWinCount();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  連勝数クリア
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::ClearWinCount( AMX *amx, const cell * )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  Savedata::BattleTreeType type = static_cast<Savedata::BattleTreeType>(sv->GetBattleType());
  Savedata::BattleTreeRank rank = sv->GetRank();
  //連勝数クリア
  sv->ClearWinCount( type, rank );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  連勝数取得(通信時は自分の連勝数と不一致になる可能性がある)
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetWinCount( AMX *amx, const cell * ptr )
{
  int count = 0;
  Savedata::BattleTreeRank rank = static_cast<Savedata::BattleTreeRank>(ptr[1]);
  Savedata::BattleTreeType type = static_cast<Savedata::BattleTreeType>(ptr[2]);
  GameSys::GameManager* p_gman = GetGameManager(amx);
  return GetWinCountFromSave(p_gman,type,rank);
}

//-----------------------------------------------------------------------------
/**
 *	@brief  連勝数取得(セーブデータ)
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetMyWinCount( AMX *amx, const cell * ptr )
{
  int count = 0;
  Savedata::BattleTreeRank rank = static_cast<Savedata::BattleTreeRank>(ptr[1]);
  Savedata::BattleTreeType type = static_cast<Savedata::BattleTreeType>(ptr[2]);
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  count = sv->GetWinCount( type, rank );
  return count;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  最大連勝数取得(セーブデータ)
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetMyMaxWinCount( AMX *amx, const cell * ptr )
{
  int count = 0;
  Savedata::BattleTreeRank rank = static_cast<Savedata::BattleTreeRank>(ptr[1]);
  Savedata::BattleTreeType type = static_cast<Savedata::BattleTreeType>(ptr[2]);
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  count = sv->GetMaxWinCount( type, rank );
  return count;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  勝ったか？
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::IsWin( AMX *amx, const cell * )
{
  bool ret;
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  ret = inst->IsWin();
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  対戦相手はボスだった？
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::IsBoss( AMX *amx, const cell * )
{
  bool ret;
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  ret = inst->IsBoss();
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  キャラクターID取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetCharacterID( AMX *amx, const cell * ptr )
{
  u16 tr_no = ptr[1];
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return inst->GetCharacterID( tr_no );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  パートナーキャラクターID取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetPartnerCharacterID( AMX *amx, const cell * ptr )
{
  u16 tr_no = ptr[1];
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return inst->GetPartnerCharacterID();
}

//-----------------------------------------------------------------------------
/**
 *	@brief  パートナーキャラクター施設トレーナーID取得
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetPartnerTrainerID( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return inst->GetPartnerTrainerID();
}

//-----------------------------------------------------------------------------
/**
 *	@brief  通信ＹＥＳＮＯ待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::WaitCommYesNo( AMX *amx, const cell * ptr )
{
  u32 ret_wk = ptr[1];
  int answer = ptr[2];

  GameSys::GameManager* p_gman = GetGameManager(amx);
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_work = p_obj->GetWork();

  BattleInst::EventBattleInstCommYesNo::StartEvent( p_gman, p_work, ret_wk, answer );
  pScriptSystem->Suspend();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  戦闘開始メッセージのＩＤを返す
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_GetBattleStartMsgId( AMX *amx, const cell * ptr )
{
  int no = ptr[1];

  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return inst->GetBattleStartMsgId( no );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  パートナーの登場メッセージIDを返す
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_GetPartnerStartMsgId( AMX *amx, const cell * ptr )
{
  int no = ptr[1];

  return BattleInst::BattleInst::PartnerIDtoStartStrID( no );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  パートナーの負けメッセージIDを返す
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_GetPartnerLoseMsgId( AMX *amx, const cell * ptr )
{
  int no = ptr[1];

  return BattleInst::BattleInst::PartnerIDtoLoseStrID( no );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ＢＧＭセット
 *
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_SetBgm( AMX *amx, const cell * ptr )
{
  int bgm_no = ptr[1];//0～BattleInst::BattleInstDefine::BINST_BGM_COUNT
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  if ( bgm_no == 0 )
  {
    sv->ResetBattleBgm();
  }
  else if ( (0 < bgm_no) && ( bgm_no < BattleInst::BattleInstDefine::BINST_BGM_COUNT )  )
  {
    sv->SetBattleBgm( bgm_no );
  }
  else
  {
    GFL_ASSERT(0);
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ＢＧＭ取得
 *
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_GetBgm( AMX *amx, const cell * ptr )
{
  int bgm_no;
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  bgm_no = sv->GetBattleBgm();
  if ( bgm_no < 0 ) bgm_no = 0;
  else if ( bgm_no >= BattleInst::BattleInstDefine::BINST_BGM_COUNT ) bgm_no = 0;
  return bgm_no;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  バトルハウス終了処理
 *
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_End( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  //現在挑戦中のランク戦闘タイプのリセット
  sv->ResetBattleTypeAndRank();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  通信中か？
 *
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_IsComm( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return inst->GetIsComm();
}

//-----------------------------------------------------------------------------
/**
*	@brief  通信状態に問題が発生したか
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_IsCommExit( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return inst->GetIsCommExit();
}

//-----------------------------------------------------------------------------
/**
*	@brief  通信エラーが存在するかチェック
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_IsNetworkError( AMX *amx, const cell * ptr )
{
  return GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError();
}

//-----------------------------------------------------------------------------
/**
*	@brief  通信で受け取ったデータの整合性チェック結果を取得
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_GetCommCheckResult( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  return inst->GetCommCheckResult();
}

//-----------------------------------------------------------------------------
/**
 *  @brief レギュレーション選択アプリ結果取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetResultRegulationSelectApp_( AMX* amx, const cell* ptr)
{
  u32 is_select_ret_wk = ptr[1];

  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_work = p_obj->GetWork();
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

  p_work->SetTempWorkValue( is_select_ret_wk, p_inst->GetRegulationSelectResult()->bIsSelectedRegulation );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief ポケモン選択アプリ結果取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetResultPokeListApp_( AMX* amx, const cell* ptr)
{
  u32 is_select_ret_wk = ptr[1];

  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_work = p_obj->GetWork();
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

  App::PokeList::CONTEXT_PARAM* param = p_inst->GetPokeListContextParam();

  bool is_select = false;
  switch( param->end_mode)
  {
  case App::PokeList::END_MODE_DECIDE:
    is_select = true;
    //ワークに選択したメンバーをセット
    p_inst->SetPokeSelectResult();
    break;
  }
  p_work->SetTempWorkValue( is_select_ret_wk, is_select );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief NPC選択アプリ結果取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetResultNpcSelectApp_( AMX* amx, const cell* ptr)
{
  u32 is_select_ret_wk = ptr[1];

  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_work = p_obj->GetWork();
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

  bool is_select = (p_inst->GetBattleTreeAppParam()->out.npcSelectIdx != App::BattleTree::NPC_SELECT_IDX_ERROR);
  p_inst->SetPartnerNPCID(p_inst->GetBattleTreeAppParam()->out.npcSelectIdx);
  p_work->SetTempWorkValue( is_select_ret_wk, is_select );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief 通信クイック接続アプリ結果取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetResultQuickMatchApp_( AMX* amx, const cell* ptr)
{
  u32 is_connect_ret_wk = ptr[1];

  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_work = p_obj->GetWork();
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

  p_work->SetTempWorkValue( is_connect_ret_wk, p_inst->GetQuickMatchEventParam()->isConnected );

  return NULL;
}


//-----------------------------------------------------------------------------
/**
 *  @brief 選択したルールを取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetBattleRule( AMX* amx, const cell* ptr)
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();
  Regulation& reg = p_inst->GetRegulationDrawInfo()->GetRegulation();

  return reg.GetBattleRule();
}


//-----------------------------------------------------------------------------
/**
 *	@brief  ＡＩマルチ　相方セットアップ
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::SetupAiPartner( AMX* amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  inst->MakeAiPartner();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  レギュレーション設定
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::SetRegulation( AMX* amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);

  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  NetAppLib::UI::RegulationDrawInfo* info = inst->GetRegulationDrawInfo();
  Regulation& reg = info->GetRegulation();

  Regulation::BATTLE_CATEGORY battleCategory = reg.GetBattleCategory();
  Regulation::BATTLE_RULE battleRule = reg.GetBattleRule();
  inst->SetRule( reg.GetBattleCategory(), reg.GetBattleRule() );
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ギミックをオープンする
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::OpenBattleInstGimmick( AMX* amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  Fieldmap* p_fieldmap = p_gman->GetFieldmap();
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  Field::FieldGimmickBattleInst* pGimmick = static_cast<Field::FieldGimmickBattleInst*>( p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));

  return pGimmick->OpenGimmick();
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ギミックをクローズする
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::CloseBattleInstGimmick( AMX* amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  Fieldmap* p_fieldmap = p_gman->GetFieldmap();
  BattleInst::BattleInst* inst = p_gman->GetBattleInstManager()->GetBattleInst();
  Field::FieldGimmickBattleInst* pGimmick = static_cast<Field::FieldGimmickBattleInst*>( p_fieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE));

  return pGimmick->CloseGimmick();
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルマスターランク連勝数加算
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::AddWinCountRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save    = p_gdata->GetBattleInstSave();

  save->AddWinCountRoyal();

  return NULL;
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルマスターランク連勝数クリア
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::ClearWinCountRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save    = p_gdata->GetBattleInstSave();

  save->ClearWinCountRoyal();

  return NULL;
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルマスターランク連勝数取得
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetWinCountRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save    = p_gdata->GetBattleInstSave();

  return save->GetWinCountRoyal();
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルランク解放
 * @note マスターランクまで解放されていた場合は何もしない
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::ReleaseNextRankRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save    = p_gdata->GetBattleInstSave();

  save->ReleaseNextRankRoyal();

  return NULL;
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤル解放ランク取得
 * @return 解放されているランク。スーパーまで解放されていればスーパーが返る
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetReleaseRankRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save    = p_gdata->GetBattleInstSave();

  return save->GetReleaseRankRoyal();
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤル選択ランク取得
 * @return 選択されたランク。先にレギュレーション選択アプリを呼ぶこと
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetSelectRankRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  return p_inst->GetSelectRankRoyal();
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルプレイヤー順位取得
 * @return 順位(0オリジン) 1位:0、2位:1 …
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetPlayerRankingRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  return p_inst->GetBattleRoyalResult().clientRanking[BTL_CLIENT_PLAYER];
}

//--------------------------------------------------------------------------
/**
 * @brief 最後に場に出ていたポケモンのパーティー内番号取得
 * @return 最後に場に出ていたポケモンのパーティー内番号(全滅して場にポケモンがいなかったら-1)
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetLastPokePartyIndexRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  return p_inst->GetBattleRoyalResult().lastPokePartyIndex[BTL_CLIENT_PLAYER];
}

//--------------------------------------------------------------------------
/**
 * @brief ボス戦か
 * @retval true ボス戦
 * @retval false それ以外
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::IsBossRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save    = p_gdata->GetBattleInstSave();
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  return p_inst->IsBossRoyal();
}

//--------------------------------------------------------------------------
/**
 * @brief バトルロイヤル開始
 */
//--------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::StartBattleRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();
  FieldScriptSystem*        pScriptSystem = FieldScriptSystem::GetInstance();

  p_inst->StartBattleRoyal();

  pScriptSystem->Suspend();

  return NULL;
}

//--------------------------------------------------------------------------
/**
 * @brief バトルロイヤル用の生成処理
 */
//--------------------------------------------------------------------------
cell FuncSetBattleInst::AMX_NATIVE_CALL CreateForRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  p_inst->CreateForRoyal();

  return NULL;
}

//--------------------------------------------------------------------------
/**
 * @brief バトルロイヤル用の破棄処理
 */
//--------------------------------------------------------------------------
cell FuncSetBattleInst::AMX_NATIVE_CALL DeleteForRoyal( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  p_inst->DeleteForRoyal();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  基本データの送信
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::SendCommonData( AMX* amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::EventBattleInstConnect::StartEvent( p_gman );
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();
  pScriptSystem->Suspend();
  
  return NULL;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  対戦相手を交換
 *
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::TradeTarinerData( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::EventBattleInstTradeTrainerData::StartEvent( p_gman );
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();
  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  通信切断
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInstDisconnect( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::EventBattleInstDisconnect::StartEvent( p_gman );
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();
  pScriptSystem->Suspend();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  バトル施設スカウトトレーナーがいっぱいか
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::IsBinstFullScoutTrainerData( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save    = p_gdata->GetBattleInstSave();

  return save->IsFullScoutTrainerData();
}

//-----------------------------------------------------------------------------
/**
*	@brief  既にスカウト済みのバトル施設スカウトトレーナーか？
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::IsBinstAllreadyScoutTrainerData( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save    = p_gdata->GetBattleInstSave();
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  // 常に0番のトレーナーをスカウトするため0版をチェック
  u16 trainerID = p_inst->GetTrainerID( 0 );
  bool isExist  = save->IsExistScoutTrainerData( trainerID );

  return isExist;
}

//-----------------------------------------------------------------------------
/**
*	@brief  既にリーリエをスカウト済みか？
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::IsBinstAllreadyLilieScout(AMX *amx, const cell * ptr)
{
  GameSys::GameManager*     p_gman = GetGameManager(amx);
  GameSys::GameData*        p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* save = p_gdata->GetBattleInstSave();

  bool isExist = save->IsExistScoutTrainerData(BattleInst::BattleInstDefine::BINST_LILIE_DATA_ID);

  return isExist;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  現在の敵をスカウト
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::ScoutCurrentTrainer( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  p_inst->ScountCurrentTrainer();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*	@brief  リーリエをスカウト
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::ScoutLilie(AMX *amx, const cell * ptr)
{
  GameSys::GameManager*     p_gman = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst = p_gman->GetBattleInstManager()->GetBattleInst();
  p_inst->ScoutLilie();
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  選択したトレーナーを削除
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::RemoveSelectScoutTrainer( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman  = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* p_save = p_gdata->GetBattleInstSave();
  int selectIndex = p_inst->GetBattleTreeAppParam()->out.npcSelectIdx;

  p_save->ClearScoutTrainerIndex(selectIndex);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  トレーナータイプをワードセットに登録
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::WordSetBinstTrainerTypeNameFromTrainerNo( AMX *amx, const cell * ptr )
{
  u16 buf_id = ptr[1];
  u16 tr_no = ptr[2];
  
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();
  p_inst->WordSetBinstTrainerTypeNameFromTrainerNo(buf_id,tr_no);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  トレーナー名をワードセットに登録
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::WordSetBinstTrainerNameFromTrainerNo( AMX *amx, const cell * ptr )
{
  u16 buf_id = ptr[1];
  u16 tr_no = ptr[2];
  
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();
  p_inst->WordSetBinstTrainerNameFromTrainerNo(buf_id,tr_no);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  トレーナータイプ名とトレーナー名をワードセットに登録
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::WordSetBinstTrainerTypeNameAndTrainerNameFromTrainerNo( AMX *amx, const cell * ptr )
{
  u16 dst_buf_id = ptr[1];
  u16 type_name_buf_id = ptr[2];
  u16 name_buf_id = ptr[3];
  u16 tr_no = ptr[4];
  
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();
  p_inst->WordSetBinstTrainerTypeNameAndTrainerNameFromTrainerNo(dst_buf_id,type_name_buf_id,name_buf_id,tr_no);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*	@brief  バトルに参加したポケモン全員にリボンをセット
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::SetRibbonEntryPokeAll( AMX *amx, const cell * ptr )
{
  u16 ribbon_id = ptr[1];
  u16 mode      = ptr[2];

  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();

  bool isSetRibbon = p_inst->SetRibbon( ribbon_id, static_cast<Savedata::BattleInstMode>( mode));

  return isSetRibbon;
}

//-----------------------------------------------------------------------------
/**
*	@brief  バトル施設のご褒美用勝利カウンタをセット
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::SetPrizeWinCount( AMX *amx, const cell * ptr )
{
  u16 winCount = ptr[1];

  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();

  sv->SetPrizeWinCount( winCount );
  ONOUE_PRINT("ご褒美用勝利カウンタをセット:%d\n",winCount);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*	@brief  バトル施設のご褒美用勝利カウンタを取得
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::GetPrizeWinCount( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();

  return sv->GetPrizeWinCount();
}

//-----------------------------------------------------------------------------
/**
*	@brief  バトル施設のご褒美用勝利カウンタをリセット
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::ClearPrizeWinCount( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();

  sv->ResetPrizeWinCount();
  ONOUE_PRINT("ご褒美用勝利カウンタをリセット");
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*	@brief  バトル施設参加のダイアリーをセット
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::SetBinstPokeDiary( AMX *amx, const cell * ptr )
{
  GameSys::GameManager* p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BattleInstSave* sv = p_gdata->GetBattleInstSave();
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();
  Savedata::BattleTreeType type = static_cast<Savedata::BattleTreeType>(sv->GetBattleType());
  Savedata::BattleTreeRank rank = sv->GetRank();

  int winCount = GetWinCountFromSave(p_gman,type,rank);

  // レギュレーションを取得
  Regulation& reg = p_inst->GetRegulationDrawInfo()->GetRegulation();

  // @fix GFNMCat[4567]：ルールの数値が間違っていたので修正
  u16 data2 = 0;
  switch( reg.GetBattleRule() ){
  case Regulation::BATTLE_RULE_SINGLE: data2 = 0; break;
  case Regulation::BATTLE_RULE_DOUBLE: data2 = 1; break;  //! ダブル
  case Regulation::BATTLE_RULE_MULTI:  data2 = 2; break;  //! マルチ
  default:
    GFL_ASSERT(0);
    break;
  }

  //GFNMCat[4565] winCountは1試合目は"0"だが、ダイアリーでは1試合目を"1"としたい。最大9999
  if( winCount < sv->GetCountMax() )
  {
    ++winCount;
  }

  PokeDiary::Set( PDID_COUNT_WIN_BH_CHAMP , 
    static_cast<u16>( (p_inst->GetRank() == Savedata::BATTLE_TREE_RANK_SUPER) ), 
    data2, 
    static_cast<u16>( winCount ));
      
  ONOUE_PRINT("バトル施設ダイアリーをセット\n");
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetBattleInst::DEBUG_SetupSingleParty( AMX *amx, const cell * ptr )
{
#if PM_DEBUG
  GameSys::GameManager* p_gman = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();
  p_inst->DEBUG_SetupSingleParty();
#endif
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*	@brief  バトル施設用ポケモンニックネームワードセット
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::WordSetSelectPartyMonsNickName( AMX* amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman    = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst    = p_gman->GetBattleInstManager()->GetBattleInst();
  print::WordSet*           p_wordset = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 buff_id                 = ptr[1];
  u32 entry_party_poke_index  = ptr[2];

  s32 select_party_poke_index = p_inst->ConvertEntryPartyIndexToSelectPartyIndex( entry_party_poke_index );
  const pml::PokeParty* p_select_party = p_inst->GetSelectPokeParty();
  const pml::pokepara::CoreParam* p_poke = p_select_party->GetMemberPointerConst( select_party_poke_index );

  p_wordset->RegisterPokeNickName( buff_id, p_poke );

  return NULL;
}


//-----------------------------------------------------------------------------
/**
*	@brief  親かチェック
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetBattleInst::BattleInst_GetIsParent( AMX *amx, const cell * ptr )
{
  GameSys::GameManager*     p_gman    = GetGameManager(amx);
  BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();
  if ( p_inst->GetIsComm() )
  {
    return p_inst->GetIsParent();
  }else{
    return true;
  }
}

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );


