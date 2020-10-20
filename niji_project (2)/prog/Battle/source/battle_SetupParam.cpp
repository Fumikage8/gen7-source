//=============================================================================================
/**
 * @file  battle_SetupParam.cpp
 * @brief ポケモンXY バトルシステム セットアップパラメータ構造＆ツール関数
 * @author  taya
 *
 * @date  2011.4.14  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include <pml/include/pml_PokePara.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Field/FieldStatic/include/Encount/FieldEncountPokeSet.h>
#include <Field/FieldStatic/include/Debug/FieldDebugTypes.h>
#include <Field/FieldStatic/include/BattleInst/BattleInst.h>
#include <Field/FieldStatic/include/BattleInst/BattleInstManager.h>
#include <Field/FieldStatic/include/BattleFes/BattleFes.h>
#include <Field/FieldStatic/include/BattleFes/BattleFesManager.h>
#include <Trainer/Trainer/include/tr_tool.h>

#include  "Battle/include/battle_SetupParam.h"

#include  "btl_const.h"
#include  "btl_DebugPrint.h"

#include "Savedata/include/MyStatus.h"
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/MiscSave.h"
#include "Savedata/include/BoxPokemonSave.h"

// レギュレーション検査
#include "Battle/Regulation/include/PokeRegulation.h"

//ロトム用
#include <AppLib/include/Tool/AppToolRotomFriendship.h>

//BSP_BATTLE_EFFECT設定用
#include "niji_conv_header/sound/SoundMiddleID.h"
#include "Battle/include/battle_encount_msg_def.h"
#include "niji_conv_header/battle/battle_cutin.h"
#include "niji_conv_header/battle/battle_effect.cdat"

#include "Battle/Background/include/BgSetupUtil.h"
#include "niji_conv_header/battle/background/bgsys_far_type_def.h"
#include "niji_conv_header/battle/background/bgsys_near_type_def.h"

#if defined(GF_PLATFORM_CTR)
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#endif

int BATTLE_SETUP_PARAM::pokeDecoType = 1;

#if PM_DEBUG
  bool BATTLE_SETUP_PARAM::isDebugUseMomijiOptimize = true;
#endif

GFL_NAMESPACE_BEGIN(btl)
  namespace setup {


    u8 GetMaxFollowPokeLevel( const Savedata::Misc& misc );
    void BATTLE_SETUP_PARAM_InitPartyParam( BATTLE_SETUP_PARAM* setupParam );


    static const u8   SAKASA_BATTLE_ENEMY_POKEMON_PARTY_SIZE = 3;   ///< さかさバトル、相手トレーナーのポケモン数
    static const u16  SAKASA_BATTLE_ENEMY_POKEMON_PATTERN = 36;     ///< さかさバトル、相手トレーナーのポケモンのパターン数

    //=============================================================================================
    /**
     * セットアップ共通処理
     *
     * @param[out]   dst
     * @param[in]    gameMan
     * @param[in]    sit
     */
    //=============================================================================================
    void common_base( BATTLE_SETUP_PARAM* dst, GameSys::GameManager* gameMan, const BTL_FIELD_SITUATION* sit )
    {
      u32 i;
      GameSys::GameData* gameData = gameMan->GetGameData();


     OHNO_PRINT("バトルセットアップのライブ大会用タイマー %d\n", dst->bEnableTimeStop);



      dst->commNetIDBit        = 0xffff; // デフォルトでは全BitをONにしておく
      dst->commMode            = BTL_COMM_NONE;
      dst->commPos             = 0;
      dst->multiMode           = BTL_MULTIMODE_NONE;
      dst->getMoney            = 0;
      dst->moneyRate           = 1.0f;
      dst->recBuffer           = NULL;
      dst->recordDataMode      = BTL_RECORDDATA_NONE;
      dst->gameData            = gameData;
      dst->cmdIllegalFlag      = false;
      dst->isDisconnectOccur   = false;
      dst->isLiveRecSendEnable = false;
#if PM_DEBUG
      dst->isLiveRecPlayMode   = false;
#endif
      dst->recPlayCompleteFlag = false;
      dst->oteireFlag          = false;
      dst->bSkyBattle          = false;
      dst->bSakasaBattle       = false;
      dst->bNoGainBattle       = false;
      gfl2::std::MemClear( dst->conventionInfo, sizeof(BATTLE_CONVENTION_INFO)*2 );

      for(i=0; i<BTL_CLIENT_NUM; ++i){
        dst->party[i] = NULL;
        dst->playerStatus[i] = NULL;
        dst->tr_data[i] = NULL;
        dst->miseaiData[i] = NULL;
        dst->playerRating[i] = 0;
      }

      for(i=0; i<GFL_NELEMS(dst->fightPokeIndex); ++i){
        dst->fightPokeIndex[i] = false;
      }

      for(i=0; i<GFL_NELEMS(dst->turnedLvUpPokeIndex); ++i){
        dst->turnedLvUpPokeIndex[i] = false;
      }

      dst->shooterBitWork.shooter_use = true;
      for(i=0; i<GFL_NELEMS(dst->shooterBitWork.bit_tbl); ++i){
        dst->shooterBitWork.bit_tbl[i] = 0;//0xff;このBitは使用禁止bitなので0だと全て使えるの間違いでしたnagihashi 5/3
      }

      dst->playerStatus[BTL_CLIENT_PLAYER] = gameData->GetPlayerStatus();

      dst->itemData     = gameData->GetMyItem();
      dst->zukanData    = gameData->GetZukanData();
      dst->configData   = gameData->GetConfig();

      const Savedata::Misc* pMisc = gameData->GetMisc();
      if( dst->itemData->Check( ITEM_GAKUSYUUSOUTI, 1 ) == false ){
        dst->bGakusyuSouti = false;
      } else {
        dst->bGakusyuSouti = pMisc->GetGakusyuusoutiFlag();
      }
      pMisc->GetMegaSeeFlags( dst->megaFlags );

      {
        const Savedata::Misc* misc = gameData->GetMisc();
        dst->maxFollowPokeLevel = GetMaxFollowPokeLevel( *misc );
      }

      BATTLE_SETUP_PARAM_InitPartyParam( dst );
      INTRUDE_BATTLE_PARAM_Clear( &( dst->intrudeParam ) );

#if PM_DEBUG
      if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::BATTLE_INTRUDE_FORCE_CALL ) )
      {
        BTL_SETUP_SetDebugFlag( dst, BTL_DEBUGFLAG_INTRUDE_FORCE_CALL );
      }
      if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::BATTLE_INTRUDE_FORCE_APPEAR ) )
      {
        BTL_SETUP_SetDebugFlag( dst, BTL_DEBUGFLAG_INTRUDE_FORCE_APPEAR );
      }
#endif

      //--------------------------------------------------------

      gfl2::std::MemCopy( sit, &dst->fieldSituation, sizeof(BTL_FIELD_SITUATION) );
      gfl2::std::MemClear( &(dst->PGL_Record), sizeof(dst->PGL_Record) );
      for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i){
        dst->PGL_Record.myParty[ i ].deadPokeNumber = MONSNO_NULL;
        dst->PGL_Record.enemysParty[ i ].deadPokeNumber = MONSNO_NULL;
      }

      dst->result = BTL_RESULT_WIN;

      //イベントフラグチェック
      {
        Field::EventWork* evwork = gameData->GetEventWork();
        //ボックス開発者に会っているかフラグ
        if( evwork->CheckEventFlag( SYS_FLAG_PCNAME_OPEN )){
          BATTLE_PARAM_SetBtlStatusFlag( dst, BTL_STATUS_FLAG_PCNAME_OPEN );
        }
      }
     //--------------------------------------------------------
      //ロトム系設定
      //イベントフラグチェック
      {
        Field::EventWork* evwork = gameData->GetEventWork();
        //ロトム図鑑入手
        if (evwork->CheckEventFlag(SYS_FLAG_BOUKEN_NAVI_GET))
        {
          dst->bEnableRotom = true;
        }
        
        dst->rotomRank = gameData->GetFieldMenu()->GetRotomRank(App::Tool::RotomFriendship::GetDataIndex(evwork));
      }
      // バトルフェスフィールド効果設定
      dst->btlFesFieldType = BATTLE_FES_FIELD_NONE;

     //--------------------------------------------------------
    }

     /**
      * @brief 命令無視をしないで言うことを聞くポケモンの最大レベルを取得する
      * @param misc  参照するスタンプデータ
      */
     u8 GetMaxFollowPokeLevel( const Savedata::Misc& misc )
     {
       static const u8 MAX_LEVEL[ Savedata::Misc::ISLAND_STAMP_NUM + 1 ] = 
       {
         20, 35, 50, 65, 80, 100,
       };

       u8 stampNum = misc.GetIslandStampNum();
       u8 maxLevel = MAX_LEVEL[ stampNum ];
       return maxLevel;
     }

    /**
     * @brief パーティごとのパラメータを初期化する
     * @param setupParam  初期化対象のインスタンス
     */
    void BATTLE_SETUP_PARAM_InitPartyParam( BATTLE_SETUP_PARAM* setupParam )
    {
      for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
      {
        PartyParam* partyParam = &( setupParam->partyParam[ clientId ] );
        for( u32 memberIndex=0; memberIndex<pml::PokeParty::MAX_MEMBERS; ++memberIndex )
        {
          DefaultPowerUpDesc* powerUpDesc = &( partyParam->defaultPowerUpDesc[ memberIndex ] );
          DEFAULT_POWERUP_DESC_Clear( powerUpDesc );
        }
      }
    }

    //=============================================================================================
    /**
     * @brief  プレイヤーパラメータセット
     *
     * @param[out]   dst
     * @param[in]    gameData
     * @param[in]    heapHandle
     */
    //=============================================================================================
    void player_param( BATTLE_SETUP_PARAM* dst, GameSys::GameData* gameData, gfl2::heap::HeapBase *pHeap )
    {
      dst->party[ BTL_CLIENT_PLAYER ] = GFL_NEW(pHeap) pml::PokeParty( pHeap );
      pml::PokeParty* playerParty = gameData->GetPlayerParty();
      BTL_PRINT("[BTL_SETUP] PlayerParty=%p, Members=%d\n", playerParty, playerParty->GetMemberCount());
      dst->party[ BTL_CLIENT_PLAYER ]->CopyFrom( *playerParty );
    }

    //=============================================================================================
    /**
     *  トレーナー手持ちポケモンレベル調整
     */
    //=============================================================================================
    void adjustTrPokeLevel( pml::PokeParty* party, int adjust_level )
    {
      int num;

      BTL_PRINT("AdjustLevel =%d\n", adjust_level );
      if( adjust_level == 0 ) return;

      num = party->GetMemberCount();
      for( int i = 0;i < num; ++i ){
        pml::pokepara::PokemonParam* pp = party->GetMemberPointer( i );
        int level = pp->GetLevel() + adjust_level;

        if( level > 100 ) level = 100;
        else if( level < 1 ) level = 1;

        //レベル再設定
        PokeRegulation::MakeLevelRevise( pp, level );
        //補正後レベルのデフォ技を設定
        pp->SetDefaultWaza();
      }
    }

    //=============================================================================================
    /**
     *  トレーナー手持ちポケモンレベル調整
     */
    //=============================================================================================
    void setTrPokeLevelCap( pml::PokeParty* party, int level, b32 setDefaultWaza )
    {
      int num;

      BTL_PRINT("level =%d\n", level );
      if( level < 1 ||  level > 100 ) return;

      num = party->GetMemberCount();
      for( int i = 0;i < num; ++i ){
        pml::pokepara::PokemonParam* pp = party->GetMemberPointer( i );
        int now_level = pp->GetLevel();

        if( now_level > level )
        {
          //レベル再設定
          PokeRegulation::MakeLevelRevise( pp, level );
        }

        // 補正後レベルのデフォ技を設定、念のため補正が実行されなくても行う
        if( setDefaultWaza )
        {
          pp->SetDefaultWaza();
        }
      }
    }

    //=============================================================================================
    /**
     *  トレーナー手持ちポケモンレベル調整
     */
    //=============================================================================================
    void setTrPokeLevel( pml::PokeParty* party, int level, b32 setDefaultWaza )
    {
      int num;

      BTL_PRINT("level =%d\n", level );
      if( level < 1 ||  level > 100 ) return;

      num = party->GetMemberCount();
      for( int i = 0;i < num; ++i ){
        pml::pokepara::PokemonParam* pp = party->GetMemberPointer( i );

        //レベル再設定
        PokeRegulation::MakeLevelRevise( pp, level );

        //補正後レベルのデフォ技を設定
        if( setDefaultWaza )
        {
          pp->SetDefaultWaza();
        }
      }
    }

    //=============================================================================================
    /**
     * @brief  ゲーム内トレーナーパラメータセット
     *
     * @param[out]   dst        パラメータセットアップ先構造体
     * @param[in]    gameData
     * @param[in]    clientID
     * @param[in]    party
     * @param[in]    tr_id      トレーナーID
     * @param[in]    pHeap
     */
    //=============================================================================================
    void normalTrainer( BATTLE_SETUP_PARAM* dst, GameSys::GameData* gameData, BTL_CLIENT_ID clientID,
            pml::PokeParty* party, trainer::TrainerID tr_id, gfl2::heap::HeapBase* pHeap )
    {
      if(tr_id == trainer::TRID_NULL) return;

      trainer::EncountTrainerPersonalDataMake( tr_id, dst->tr_data[clientID], pHeap );
      trainer::EncountTrainerPokeDataMake( tr_id, party, pHeap );

      //トレーナー手持ちポケモンレベル調整
      adjustTrPokeLevel( party, dst->fieldSituation.trPokeLvAdjust );
    }


    /**
     *  トレーナーデータに格納されている BGM設定などを、バトルセットアップパラメータに引き渡し
     */
    void settingByTrainerData( BATTLE_SETUP_PARAM* bp, const BSP_TRAINER_DATA* trData )
    {
      BATTLE_PARAM_SetBattleEffect( bp, trData->GetBattleEffectID() );
    }

    //=============================================================================================
    /**
    * @brief  バトルフェストレーナーパラメータセット
    *
    * @param[out]   dst        パラメータセットアップ先構造体
    * @param[in]    gameData
    * @param[in]    clientID
    * @param[in]    party
    * @param[in]    pHeap
    */
    //=============================================================================================
    void btlfesTrainer(BATTLE_SETUP_PARAM* dst, GameSys::GameManager *gameMan, BTL_CLIENT_ID clientID,
      pml::PokeParty* party, gfl2::heap::HeapBase* pHeap)
    {
      BattleFes::BattleFes* fes = gameMan->GetGameData()->GetBattleFesManager()->GetBattleFes();
      fes->SetVsTrainer(dst->tr_data[clientID], pHeap);
      fes->SetVsPokemon(party);
    }

    //=============================================================================================
    /**
     * @brief  バトルハウストレーナーパラメータセット
     *
     * @param[out]   dst        パラメータセットアップ先構造体
     * @param[in]    gameData
     * @param[in]    clientID
     * @param[in]    party
     * @param[in]    btl_tr_no      バトルハウストレーナー 0 〜 2   2はＡＩマルチ相方
     * @param[in]    pHeap
     */
    //=============================================================================================
    void btlhouseTrainer( BATTLE_SETUP_PARAM* dst, GameSys::GameManager *gameMan, BTL_CLIENT_ID clientID,
            pml::PokeParty* party, int btl_tr_no, gfl2::heap::HeapBase* pHeap )
    {
      BattleInst::BattleInst* inst = gameMan->GetBattleInstManager()->GetBattleInst();
      inst->SetVsTrainer( btl_tr_no, dst->tr_data[clientID], pHeap );
      inst->SetVsPokemon( btl_tr_no, party, pHeap );
    }

    //=============================================================================================
    /**
     * @brief  バトルハウストレーナーパラメータセット
     *
     * @param[out]   dst        パラメータセットアップ先構造体
     * @param[in]    gameData
     * @param[in]    clientID
     * @param[in]    party
     * @param[in]    btl_tr_no      バトルハウストレーナー 0 〜 2   2はＡＩマルチ相方
     * @param[in]    pHeap
     */
    //=============================================================================================
    void btlhouseTrainerComm( BATTLE_SETUP_PARAM* dst, GameSys::GameManager *gameMan, BTL_CLIENT_ID clientID,
            pml::PokeParty* party, int btl_tr_no, gfl2::heap::HeapBase* pHeap )
    {
      BattleInst::BattleInst* inst = gameMan->GetBattleInstManager()->GetBattleInst();
      inst->SetVsTrainer( btl_tr_no, dst->tr_data[clientID], pHeap );
      inst->SetVsCommPokemonToParty( btl_tr_no, party );
    }

    //=============================================================================================
    /**
     * @brief  バトルロイヤル施設トレーナーパラメータセット
     *
     * @param[out]   dst        パラメータセットアップ先構造体
     * @param[in]    gameData
     * @param[in]    clientID
     * @param[in]    party
     * @param[in]    btl_tr_no      バトルハウストレーナー 0 〜 2   2はＡＩマルチ相方
     * @param[in]    pHeap
     */
    //=============================================================================================
    void royalinstTrainer( BATTLE_SETUP_PARAM* dst, GameSys::GameManager *gameMan, BTL_CLIENT_ID clientID,
            pml::PokeParty* party, int btl_tr_no, gfl2::heap::HeapBase* pHeap )
    {
      BattleInst::BattleInst* inst = gameMan->GetBattleInstManager()->GetBattleInst();
      inst->SetVsTrainerRoyal( btl_tr_no, dst->tr_data[clientID], pHeap);
      inst->SetVsPokemonRoyal( btl_tr_no, party, pHeap);
    }

  } // end of namespace 'setup'
GFL_NAMESPACE_END(btl)

//=============================================================================================
/**
 * バトルエフェクトパラメータ取得
 *
 * @param[in]    btl_eff_id
 * @param[out]   pBtlEff
 */
//=============================================================================================
void BATTLE_EFFECT_GetParam( BattleEffectId btl_eff_id, BSP_BATTLE_EFFECT *pBtlEff )
{
  const BSP_BATTLE_EFFECT* bbe = &DATA_BattleEffectTbl[btl_eff_id];

  gfl2::std::MemCopy( bbe, pBtlEff, sizeof(BSP_BATTLE_EFFECT));
}

//=============================================================================================
/**
 *  @brief  戦闘パラメータワークの内部初期化処理
 *  @param  bp  確保済みのBATTLE_SETUP_PARAM構造体型ワークへのポインタ
 *
 *  ＊デフォルトのパラメータで構造体を初期化します。バトルタイプに応じて、必要な初期化を追加で行ってください
 *  ＊使い終わったら必ずBATTLE_PARAM_Release()関数で解放処理をしてください
 */
//=============================================================================================
void BATTLE_SETUP_Init( BATTLE_SETUP_PARAM* bp )
{
  gfl2::std::MemClear( bp, sizeof(*bp) );
#if PM_DEBUG
  InitializeBattleDebugParam(&bp->btlvDebugParam);
	bp->btlvDebugParam.mIsOptimizeMode = BATTLE_SETUP_PARAM::isDebugUseMomijiOptimize;
#endif

  // フィールドシチュエーションデータデフォルト設定
  bp->fieldSituation.hour   = 10;
  bp->fieldSituation.minute = 0;

  bp->moneyRate = 1.0f;
}

/**
 * パラメータクリア（動的に確保したメンバポインタをfreeなど）
 *
 * @param[out]   bp
 */

void BATTLE_SETUP_Clear( BATTLE_SETUP_PARAM  *bp )
{
  for(u32 i=0; i<GFL_NELEMS(bp->party); ++i)
  {
    if( bp->party[i] ){
      GFL_SAFE_DELETE( bp->party[i] );
    }
  }

  for(u32 i=0; i<GFL_NELEMS(bp->tr_data); ++i)
  {
    if ( bp->tr_data[i] ){
      GFL_SAFE_DELETE( bp->tr_data[i] );
    }
  }

  for( int i=0; i<BTL_CLIENT_NUM; i++ )
  {
    if( (i != BTL_CLIENT_PLAYER) && (bp->playerStatus[ i ] != NULL) ){
      GFL_SAFE_DELETE( bp->playerStatus[i] );
    }
  }

  if( bp->recBuffer )
  {
    GFL_SAFE_DELETE( bp->recBuffer );
    ICHI_PRINT("録画バッファ削除\n");
  }

  for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
  {
    GFL_SAFE_DELETE( bp->reinforceParam.reinforcePoke[i].pokeParam );
  }
}



void setup_reinforce( ReinforceBattleParam* reinforceParam, Field::Encount::PokeSet* reinforcePokeSet, gfl2::heap::HeapBase* pHeap );

//=============================================================================================
/**
 * 野生戦用パラメータセット
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    partyEnemy
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    reinforcePokeSet  援軍がない場合NULL, 援軍がある場合, エンカウントデータを持っている PokeSet
 * @param[in]    pHeap
 */
//=============================================================================================
void BATTLE_SETUP_Wild(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const pml::PokeParty         *partyEnemy,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  BattleEffectId                btl_eff_id,
  Field::Encount::PokeSet*      reinforcePokeSet,
  gfl2::heap::HeapBase          *pHeap )
{
  GFL_ASSERT_STOP( partyEnemy );

  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );
  btl::setup::player_param( bp, gameMan->GetGameData(), pHeap );
  BATTLE_PARAM_SetBattleEffect( bp, btl_eff_id );
  BTL_PRINT("[BtlSetup] eff=%d\n", btl_eff_id );

  bp->party[ BTL_CLIENT_ENEMY1 ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->party[ BTL_CLIENT_ENEMY1 ]->CopyFrom( *partyEnemy );

  bp->competitor = BTL_COMPETITOR_WILD;
  bp->rule = rule;

  // ボックス＆手持ち満杯フラグのチェック
  if( bp->party[BTL_CLIENT_PLAYER]->IsFull() )
  {
    Savedata::BoxPokemon* box_poke = gameMan->GetGameData()->GetBoxPokemon();
    if( box_poke->GetSpaceCountAll(pHeap->GetLowerHandle()) == 0 ){ //ボックスに空きがない
      BATTLE_PARAM_SetBtlStatusFlag( bp, BTL_STATUS_FLAG_BOXFULL );
    }
  }

  
  // 援軍
  for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
  {
    bp->reinforceParam.reinforcePoke[i].pokeParam = GFL_NEW( pHeap ) pml::pokepara::PokemonParam( pHeap );
  }
  setup_reinforce( &bp->reinforceParam, reinforcePokeSet, pHeap );
}

/**
 * @brief 援軍のパラメータをセットアップする
 * @param[out] reinforceParam    セットアップ対象のパラメータ
 * @param      reinforcePokeSet  援軍がない場合NULL, 援軍がある場合, エンカウントデータを持っている PokeSet
 * @param      pHeap             援軍ポケモンパラメータのバッファ確保に使用するヒープ
 */
void setup_reinforce( ReinforceBattleParam* reinforceParam, Field::Encount::PokeSet* reinforcePokeSet, gfl2::heap::HeapBase* pHeap )
{
  reinforceParam->reinforceEnableFlag = false;

  if( reinforcePokeSet == NULL )
  {
    return;  // 援軍なし
  }

  const u32 reinforcePokeNum = reinforcePokeSet->GetReinforcePokeNum();
  if( reinforcePokeNum <= 0 )
  {
    return;  // 援軍なし
  }


  reinforceParam->reinforceEnableFlag = true;


  // 援軍ポケモンのパラメータを取得
  for( u32 i=0; i<reinforcePokeNum; ++i )
  {
    ReinforcePokeParam* reinforcePoke = &( reinforceParam->reinforcePoke[i] );

    int reinforceCount = 0;
    Field::Encount::ReinforcePokeType reinforceType = Field::Encount::REINFORCE_POKE_TYPE_MAX;
    bool success = reinforcePokeSet->GetReinforcePokeParam( i, reinforcePoke->pokeParam, &reinforceType, &reinforceCount );

    if( success )
    {
      switch( reinforceType )
      {
      case Field::Encount::REINFORCE_POKE_TYPE_MUST:  reinforcePoke->condition = REINFORCE_CONDITION_MUST;   break;
      case Field::Encount::REINFORCE_POKE_TYPE_HP:    reinforcePoke->condition = REINFORCE_CONDITION_PINCH;  break;
      default:                                        reinforcePoke->condition = REINFORCE_CONDITION_NULL;   break; 
      }

      reinforcePoke->maxAppearCount = reinforceCount;
    }
    else
    {
      GFL_ASSERT(0);
      reinforceParam->reinforceEnableFlag = false;  // パラメータの取得に失敗したら、援軍をキャンセル
    }
  }
}

//=============================================================================================
/**
 * トレーナー戦（基本）用パラメータセット
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    trID
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_Trainer(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  trainer::TrainerID            trID,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  gfl2::heap::HeapBase          *pHeap )
{
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY1]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY1]==NULL);
  GameSys::GameData * gameData = gameMan->GetGameData();

  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );
  btl::setup::player_param( bp, gameData, pHeap );

  bp->party[ BTL_CLIENT_ENEMY1 ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY1 ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  btl::setup::normalTrainer( bp, gameData, BTL_CLIENT_ENEMY1, bp->party[BTL_CLIENT_ENEMY1], trID, pHeap );

  btl::setup::settingByTrainerData( bp, bp->tr_data[ BTL_CLIENT_ENEMY1 ] );

/*
  bp->party[ BTL_CLIENT_ENEMY1 ] = reinterpret_cast<pml::PokeParty*>( GFL_NEW(pHeap) pml::PokeParty(pHeap) );
  setup_trainer_param( dst, gameData, BTL_CLIENT_ENEMY1, dst->party[ BTL_CLIENT_ENEMY1 ], trID, pHeap );
*/
  bp->competitor = BTL_COMPETITOR_TRAINER;
  bp->rule = rule;
}

//=============================================================================================
/**
 * トレーナー戦（タッグ）用パラメータセット  /  タッグ = プレイヤー vs AIx2
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    trID_1
 * @param[in]    trID_2
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_Trainer_Tag(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  trainer::TrainerID            trID_1,
  trainer::TrainerID            trID_2,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  gfl2::heap::HeapBase          *pHeap )
{
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY1]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY1]==NULL);

  GameSys::GameData * gameData = gameMan->GetGameData();
  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );
  btl::setup::player_param( bp, gameData, pHeap );

  bp->party[ BTL_CLIENT_ENEMY1 ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->party[ BTL_CLIENT_ENEMY2 ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY1 ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY2 ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);

  btl::setup::normalTrainer( bp, gameData, BTL_CLIENT_ENEMY1, bp->party[BTL_CLIENT_ENEMY1], trID_1, pHeap );
  btl::setup::normalTrainer( bp, gameData, BTL_CLIENT_ENEMY2, bp->party[BTL_CLIENT_ENEMY2], trID_2, pHeap );
/*
  bp->party[ BTL_CLIENT_ENEMY1 ] = reinterpret_cast<pml::PokeParty*>( GFL_NEW(pHeap) pml::PokeParty(pHeap) );
  setup_trainer_param( dst, gameData, BTL_CLIENT_ENEMY1, dst->party[ BTL_CLIENT_ENEMY1 ], trID, pHeap );
*/
  bp->competitor = BTL_COMPETITOR_TRAINER;
  bp->rule = rule;
  bp->multiMode = BTL_MULTIMODE_P_AA;

  btl::setup::settingByTrainerData( bp, bp->tr_data[ BTL_CLIENT_ENEMY1 ] );
}

//=============================================================================================
/**
 * トレーナー戦（AIマルチ）用パラメータセット  /  AIマルチ = プレイヤー&AI vs AIx2
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    trID_friend
 * @param[in]    trID_1
 * @param[in]    trID_2
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_Trainer_Multi(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  trainer::TrainerID            trID_friend,
  trainer::TrainerID            trID_1,
  trainer::TrainerID            trID_2,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  gfl2::heap::HeapBase          *pHeap )
{
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY1]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY1]==NULL);

  GameSys::GameData * gameData = gameMan->GetGameData();
  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );
  btl::setup::player_param( bp, gameData, pHeap );

  bp->party[ BTL_CLIENT_PARTNER ]   = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->party[ BTL_CLIENT_ENEMY1 ]    = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->party[ BTL_CLIENT_ENEMY2 ]    = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->tr_data[ BTL_CLIENT_PARTNER ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY1  ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY2  ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);

  btl::setup::normalTrainer( bp, gameData, BTL_CLIENT_PARTNER, bp->party[BTL_CLIENT_PARTNER], trID_friend, pHeap );
  btl::setup::normalTrainer( bp, gameData, BTL_CLIENT_ENEMY1,  bp->party[BTL_CLIENT_ENEMY1], trID_1, pHeap );
  btl::setup::normalTrainer( bp, gameData, BTL_CLIENT_ENEMY2,  bp->party[BTL_CLIENT_ENEMY2], trID_2, pHeap );
/*
  bp->party[ BTL_CLIENT_ENEMY1 ] = reinterpret_cast<pml::PokeParty*>( GFL_NEW(pHeap) pml::PokeParty(pHeap) );
  setup_trainer_param( dst, gameData, BTL_CLIENT_ENEMY1, dst->party[ BTL_CLIENT_ENEMY1 ], trID, pHeap );
*/
  bp->competitor = BTL_COMPETITOR_TRAINER;
  bp->rule = rule;
  bp->multiMode = BTL_MULTIMODE_PA_AA;

  btl::setup::settingByTrainerData( bp, bp->tr_data[ BTL_CLIENT_ENEMY1 ] );
}



//=============================================================================================
/**
 * 通信対戦（マルチ以外）セットアップ
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    sit
 * @param[in]    pHeap
 */
//=============================================================================================
void BATTLE_SETUP_Comm(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  BtlCommMode                  commMode,
  u8                           commPos,
  gfl2::heap::HeapBase          *pHeap )
{
  GFL_ASSERT_STOP(commMode!=BTL_COMM_NONE);

  GameSys::GameData * gameData = gameMan->GetGameData();
  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );
  btl::setup::player_param( bp, gameData, pHeap );

  bp->competitor = BTL_COMPETITOR_COMM;
  BATTLE_PARAM_SetBattleEffect( bp, BATTLE_EFFECT_COMM_NORMAL );

  bp->commMode = commMode;
  bp->commPos = commPos;
  BTL_PRINT("[BtlSetup] CommPos=%d にセット\n", bp->commPos);

  // 録画データ生成の為、対戦相手のMYSTATU、POKEPARTYを受け取るバッファとして確保します。
  {
    for( int i=0; i<BTL_CLIENT_NUM; ++i)
    {
      if( i != BTL_CLIENT_PLAYER )
      {
        if( bp->party[ i ] == NULL ){
          bp->party[ i ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
        }
        if( bp->playerStatus[ i ] == NULL ){
          bp->playerStatus[ i ] = GFL_NEW(pHeap) Savedata::MyStatus();
        }
      }
    }
  }

  // 録画用バッファ生成
  BATTLE_PARAM_AllocRecBuffer( bp, pHeap );

  // 念の為初期値を通信エラーにしておく
  bp->result = BTL_RESULT_COMM_ERROR;
}


//=============================================================================================
/**
 * 通信マルチモード（通常）へ変換
 * ※ BATTLE_SETUP_Comm の直後に呼び出すこと
 *
 * @param   bp
 */
//=============================================================================================
void BATTLE_SETUP_ToMultiMode( BATTLE_SETUP_PARAM *bp )
{
  GFL_ASSERT_STOP(bp->competitor == BTL_COMPETITOR_COMM);

  bp->multiMode = BTL_MULTIMODE_PP_PP;
  BATTLE_PARAM_SetBattleEffect( bp, BATTLE_EFFECT_COMM_MULTI );
}
//=============================================================================================
/**
 * 通信マルチモード（AIマルチ）へ変換
 *
 * @param   bp
 * @param   trid_enemy1
 * @param   trid_enemy2
 * @param   pHeap
 */
//=============================================================================================
void BATTLE_SETUP_ToAIMultiMode(
  BATTLE_SETUP_PARAM* bp,
  GameSys::GameData *gameData,
  trainer::TrainerID trid_enemy1,
  trainer::TrainerID trid_enemy2,
  BtlCompetitor      competitor,
  gfl2::heap::HeapBase* pHeap )
{
  GFL_ASSERT_STOP(bp->rule == BTL_RULE_DOUBLE);

  bp->multiMode  = BTL_MULTIMODE_PP_AA;
  bp->competitor = competitor;

  bp->party[ BTL_CLIENT_ENEMY1 ]    = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->party[ BTL_CLIENT_ENEMY2 ]    = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY1  ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY2  ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);

  btl::setup::normalTrainer( bp, gameData, BTL_CLIENT_ENEMY1,  bp->party[BTL_CLIENT_ENEMY1], trid_enemy1, pHeap );
  btl::setup::normalTrainer( bp, gameData, BTL_CLIENT_ENEMY2,  bp->party[BTL_CLIENT_ENEMY2], trid_enemy2, pHeap );

  BATTLE_PARAM_SetBattleEffect( bp, BATTLE_EFFECT_TR_MULTI2D );
}

//=============================================================================================
/**
 * バトルハウス通信対戦セットアップ
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    my_party
 * @param[in]    comm_party
 * @param[in]    isParent
 * @param[in]    pHeap
 */
//=============================================================================================
void BATTLE_SETUP_BattleHouseComm(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  pml::PokeParty               *my_party,
  pml::PokeParty               *comm_party,
  u8                           isParent,
  gfl2::heap::HeapBase          *pHeap )
{
  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );
 
  // 自分の位置
  u8 commPos = BTL_CLIENT_PLAYER;

  //初期値は通信エラー @fix BTS[5004]
  bp->result = BTL_RESULT_COMM_ERROR;
  bp->party[ BTL_CLIENT_PLAYER ] = GFL_NEW(pHeap) pml::PokeParty( pHeap );
  bp->party[ BTL_CLIENT_PARTNER ] = GFL_NEW(pHeap) pml::PokeParty( pHeap );
  bp->playerStatus[ BTL_CLIENT_PARTNER ] = GFL_NEW(pHeap) Savedata::MyStatus();

  pml::PokeParty* playerParty = NULL;
  pml::PokeParty* partnerParty = NULL;
  ONOUE_PRINT("[BtlSetup] isParent=%d\n", isParent);
  if( isParent )
  {
    commPos = BTL_CLIENT_PLAYER;
    playerParty = my_party;
    partnerParty = comm_party;
  }else{
    commPos = BTL_CLIENT_PARTNER;
    playerParty = comm_party;
    partnerParty = my_party;
  }

  bp->party[ BTL_CLIENT_PLAYER ]->CopyFrom( *playerParty );
  bp->party[ BTL_CLIENT_PARTNER ]->CopyFrom( *partnerParty );
  ONOUE_PRINT("[BTL_CLIENT_PLAYER] PlayerParty=%p, Members=%d MonsNo1 = %d, MonsNo2 = %d\n", 
    playerParty, playerParty->GetMemberCount(),
    playerParty->GetMemberPointer(0)->GetMonsNo(),
    playerParty->GetMemberPointer(1)->GetMonsNo());

  ONOUE_PRINT("[BTL_CLIENT_PARTNER] partnerParty=%p, Members=%d MonsNo1 = %d, MonsNo2 = %d\n", 
    partnerParty, partnerParty->GetMemberCount(),
    partnerParty->GetMemberPointer(0)->GetMonsNo(),
    partnerParty->GetMemberPointer(1)->GetMonsNo());

  bp->competitor = BTL_COMPETITOR_INST;
  bp->rule = rule;
  GFL_ASSERT_STOP(bp->rule == BTL_RULE_DOUBLE);
  //敵は2Dトレーナーのはずなので、エフェクトのタイプはMULTI2D
  BATTLE_PARAM_SetBattleEffect( bp, BATTLE_EFFECT_TR_MULTI2D );
  bp->multiMode = BTL_MULTIMODE_PP_AA;
  bp->commMode = BTL_COMM_WIRELESS;
  bp->commPos = commPos;
  ONOUE_PRINT("[BtlSetup] CommPos=%d にセット\n", bp->commPos);

  // 録画用バッファ生成
  BATTLE_PARAM_AllocRecBuffer( bp, pHeap );
}

//=============================================================================================
/**
 * バトルハウス通信マルチモード（AIマルチ）へ変換
 *
 * @param   bp
 * @param   pHeap
 */
//=============================================================================================
void BATTLE_SETUP_ToAIMultiModeForBattleHouse(
  BATTLE_SETUP_PARAM* bp,
  GameSys::GameManager      *gameMan,
  gfl2::heap::HeapBase* pHeap )
{
  GFL_ASSERT(bp->rule == BTL_RULE_DOUBLE);


  bp->multiMode = BTL_MULTIMODE_PP_AA;
  bp->competitor = BTL_COMPETITOR_INST;

  bp->party[ BTL_CLIENT_ENEMY1 ]    = reinterpret_cast<pml::PokeParty*>( GFL_NEW(pHeap) pml::PokeParty(pHeap) );
  bp->party[ BTL_CLIENT_ENEMY2 ]    = reinterpret_cast<pml::PokeParty*>( GFL_NEW(pHeap) pml::PokeParty(pHeap) );
  bp->tr_data[ BTL_CLIENT_ENEMY1  ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY2  ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);

  btl::setup::btlhouseTrainerComm( bp, gameMan, BTL_CLIENT_ENEMY1, bp->party[BTL_CLIENT_ENEMY1], BattleInst::BattleInst::BTL_INST_ENEMY1_NO, pHeap );
  btl::setup::btlhouseTrainerComm( bp, gameMan, BTL_CLIENT_ENEMY2, bp->party[BTL_CLIENT_ENEMY2], BattleInst::BattleInst::BTL_INST_ENEMY2_NO, pHeap );

  if( bp->rule != BTL_RULE_DOUBLE ){
    GFL_ASSERT(0);
    bp->rule = BTL_RULE_DOUBLE;
  }
  if( bp->multiMode != BTL_MULTIMODE_PP_AA ){
    GFL_ASSERT(0);
    bp->multiMode = BTL_MULTIMODE_PP_AA;
  }

  BATTLE_PARAM_SetBattleEffect( bp, BATTLE_EFFECT_TR_MULTI2D );
}


/**
 * バトルエフェクトパラメータ セット
 *
 * @param[out]   bp
 * @param[in]    btl_eff_id
 *
 * @note  BATTLE_SETUP_系の呼び出し時に初期値は決定されているため
 *        後から上書きしたい時のみ呼び出す
 */
void BATTLE_PARAM_SetBattleEffect( BATTLE_SETUP_PARAM* bp, BattleEffectId btl_eff_id )
{
  if( BATTLE_EFFECT_MAX <= btl_eff_id )
  {
    GFL_ASSERT(0);
    btl_eff_id = BATTLE_EFFECT_DEFAULT;
  }
  BATTLE_EFFECT_GetParam( btl_eff_id, &bp->btlEffData );
}


/**
 *  制限時間設定
 *
 * @param[out]  bsp
 * @param[in]   GameLimitSec      試合時間制限（秒／０なら無制限）
 * @param[in]   ClientLimitSec    持ち時間（秒／０なら無制限）
 * @param[in]   CommandLimitSec   コマンド選択時間制限（秒／０なら無制限）
 */
void BATTLE_PARAM_SetTimeLimit( BATTLE_SETUP_PARAM* bsp, u32 GameLimitSec, u32 ClientLimitSec, u32 CommandLimitSec )
{
  // 試合時間と持ち時間の両方が設定されることは無いはず
  if( ( GameLimitSec != 0 ) &&
      ( ClientLimitSec != 0 ) )
  {
    GFL_PRINT( "引数が不正です。試合時間と持ち時間の両方が設定されています。\n" );
    GFL_ASSERT(0);
    return;
  }

  bsp->LimitTimeGame    = GameLimitSec;
  bsp->LimitTimeClient  = ClientLimitSec;
  bsp->LimitTimeCommand = CommandLimitSec;
}

/**
 *  スカイバトル設定
 *
 * @param[out]  bsp
 */
void BATTLE_PARAM_SetSkyBattle( BATTLE_SETUP_PARAM* bsp )
{
  bsp->bSkyBattle = true;
}

/**
 *  さかさバトル設定
 *
 * @param[out]  bsp
 */
void BATTLE_PARAM_SetSakasaBattle( BATTLE_SETUP_PARAM* bsp )
{
  bsp->bSakasaBattle = true;
}

/**
 *  必ず捕獲モード設定
 *
 * @param[out]  bsp
 */
void BATTLE_PARAM_SetMustCaptureMode( BATTLE_SETUP_PARAM* bsp )
{
  bsp->bMustCapture = true;
}

/**
 * 経験値・お金が入らないモード設定
 *
 * @param[out]  bsp
 */
void BATTLE_PARAM_SetNoGainMode( BATTLE_SETUP_PARAM* bsp )
{
  bsp->bNoGainBattle = true;
}

/**
 *  賞金レート設定
 *
 * @param[out]  bsp
 *
 * @note  野生戦は賞金が発生しないので関係ない(ネコにこばんには影響しない)
 */
void BATTLE_PARAM_SetMoneyRate( BATTLE_SETUP_PARAM* bsp, f32 rate )
{
  bsp->moneyRate = rate;
}


/**
 * ポケモンパーティを外部からセット
 *
 * @param[out]  bsp
 * @param[in]   party
 * @param[in]   id
 */
void BATTLE_PARAM_SetPokeParty( BATTLE_SETUP_PARAM* bsp, const pml::PokeParty * party, BTL_CLIENT_ID id )
{
  GFL_ASSERT_STOP( id < BTL_CLIENT_NUM );
  GFL_ASSERT_STOP( bsp->party[ id ] );
  bsp->party[ id ]->CopyFrom( *party );

#if PM_DEBUG
#if defined( DEBUG_ONLY_FOR_ichiraku_katsuhiko )
  // 中身見たかったので ichiraku
  {
    GFL_PRINT(">Battle BATTLE_PARAM_SetPokeParty\n");
    GFL_PRINT(">BTL_CLIENT_ID = ");
    switch(id){
    case BTL_CLIENT_PLAYER: GFL_PRINT("BTL_CLIENT_PLAYER\n"); break;
    case BTL_CLIENT_ENEMY1: GFL_PRINT("BTL_CLIENT_ENEMY1\n"); break;
    case BTL_CLIENT_PARTNER:GFL_PRINT("BTL_CLIENT_PARTNER\n"); break;
    case BTL_CLIENT_ENEMY2: GFL_PRINT("BTL_CLIENT_ENEMY2\n"); break;
    }
    for( int i=0; i<bsp->party[id]->GetMemberCount(); i++ )
    {
      pml::pokepara::PokemonParam* pp = bsp->party[id]->GetMemberPointer( i );
      GFL_PRINT("ポケモン %d-------------------\n", i);
      GFL_PRINT("MonsNo:%d\n", pp->GetMonsNo() );
      GFL_PRINT("FormNo:%d\n", pp->GetFormNo() );
      gfl2::str::STRCODE name[32];
      pp->GetNickName( name, sizeof(name) );
      gfl2::str::PrintCode( name, "NickName:" );
    }
  }
#endif
#endif
}

/**
 * @breif   バトル引数にレギュレーションを設定
 *          必ずすべてのバトルパラメータを設定した後に呼んでください
 *
 * @param[out]  bsp
 * @param[in]   reg
 * @param[in]   heap
 *
 * @note 内部で行っていること
 *        ・制限時間設定
 *        ・ニックネーム補正
 *        ・レベル補正
 *        ・カメラモード設定
 *        ・逆さバトル
 */
void BATTLE_PARAM_SetRegulation( BATTLE_SETUP_PARAM* bsp, Regulation* reg, gfl2::heap::HeapBase* heap )
{
  {
    // レギュレーション検査クラス作成
    PokeRegulation *pokeReg;
    pokeReg = GFL_NEW_LOW( heap ) PokeRegulation( heap );

    // レギュレーションセット
    pokeReg->SetRegulation( reg );

    for( int i=0; i<BTL_CLIENT_NUM; ++i)
    {
      // レベル補正
      pokeReg->ModifyLevelPokeParty( bsp->party[i] );
      // ニックネーム補正
      pokeReg->ModifyNickName( bsp->party[i] );
    }

    GFL_DELETE pokeReg;
  }

  // 対戦ルール
  switch( reg->GetBattleRule() ){
  case Regulation::BATTLE_RULE_SINGLE:
    bsp->rule = BTL_RULE_SINGLE;
    break;
  case Regulation::BATTLE_RULE_DOUBLE:
    bsp->rule = BTL_RULE_DOUBLE;
    break;
  case Regulation::BATTLE_RULE_ROYAL:
    bsp->rule = BTL_RULE_ROYAL;
    break;
  case Regulation::BATTLE_RULE_MULTI:
    // マルチはダブル
    bsp->rule = BTL_RULE_DOUBLE;
    break;
  }

  // 制限時間を設定
  {
    u32 GameLimitSec    = 0;    // 試合制限時間
    u32 ClientLimitSec  = 0;    // 持ち時間
    u32 CommandLimitSec = 0;    // コマンド選択時間制限

    if( reg->GetTimeLimitType() == Regulation::TIME_LIMIT_TOTAL )
    {
      GameLimitSec = reg->GetValueParam( Regulation::TIME_VS ) * 60; // 分を秒に変換して渡す
    }
    else
    {
      ClientLimitSec = reg->GetValueParam( Regulation::TIME_VS ) * 60; // 分を秒に変換して渡す
    }
    CommandLimitSec = reg->GetValueParam( Regulation::TIME_COMMAND );

    BATTLE_PARAM_SetTimeLimit( bsp, GameLimitSec, ClientLimitSec, CommandLimitSec );
    GFL_PRINT("BATTLE_PARAM_SetRegulation LimitTimeCommand %d\n", bsp->LimitTimeCommand);
    GFL_PRINT("BATTLE_PARAM_SetRegulation LimitTimeGame %d\n", bsp->LimitTimeGame);
  }

  // 逆さバトルフラグがONか
  if( reg->GetBoolParam( Regulation::REVERSE ) )
  {
    bsp->bSakasaBattle = true;
    GFL_PRINT("BATTLE_PARAM_SetRegulation 逆さバトル設定\n");
  }

  // ライブ大会か
  if( reg->GetBattleCategory() == Regulation::BATTLE_CATEGORY_LIVE_CUP )
  {
    bsp->bEnableTimeStop = true;

    // 観戦機能がONか
    if( reg->GetBoolParam( Regulation::WATCH ) )
    {
      bsp->isLiveRecSendEnable = true;
    }
    GFL_PRINT("BATTLE_PARAM_SetRegulation ライブ大会設定\n");
  }

  // 戦闘背景
  if( reg->IsWcs() )
  {
    // 背景タイプを指定する
    if( reg->GetBackGroundType() == Regulation::BACKGROUND_WCS1 )
    {//WCSバージョン1
      btl::GetBgComponentData(btl::bg::FAR_TYPE_NONE,btl::bg::NEAR_TYPE_WCS1,&(bsp->fieldSituation.bgComponent));
      BATTLE_PARAM_SetBattleEffect( bsp, BATTLE_EFFECT_COMM_WCS );
    }
    else if( reg->GetBackGroundType() == Regulation::BACKGROUND_WCS2 )
    {//WCSバージョン２
      btl::GetBgComponentData(btl::bg::FAR_TYPE_NONE,btl::bg::NEAR_TYPE_WCS2,&(bsp->fieldSituation.bgComponent));
      BATTLE_PARAM_SetBattleEffect( bsp, BATTLE_EFFECT_COMM_WCSFINAL );

      // ステータスフラグをWCS決勝に設定
      BATTLE_PARAM_SetBtlStatusFlag( bsp, BTL_STATUS_FLAG_WCS_FINAL );
    }
    GFL_PRINT("BATTLE_PARAM_SetRegulation WCS設定\n");
  }
  else if( reg->GetBattleRule() == Regulation::BATTLE_RULE_ROYAL )
  {
    btl::GetBgComponentData( btl::bg::FAR_TYPE_NONE, btl::bg::NEAR_TYPE_BTLROYAL, &bsp->fieldSituation.bgComponent );
  }
  else
  {
    btl::GetBgComponentData( btl::bg::FAR_TYPE_NONE, btl::bg::NEAR_TYPE_COMM, &bsp->fieldSituation.bgComponent );
  }
}

/**
 * @breif   レート値をセット
 *
 * @param[out]  bsp
 * @param[in]   rate1   BTL_CLIENT_PLAYERのレート値
 * @param[in]   rate2   BTL_CLIENT_ENEMY1のレート値
 */
void BATTLE_PARAM_SetRatingValue( BATTLE_SETUP_PARAM* bsp, u16 rate1, u16 rate2 )
{
  bsp->playerRating[ BTL_CLIENT_PLAYER ] = rate1;
  bsp->playerRating[ BTL_CLIENT_ENEMY1 ] = rate2;
  bsp->isPlayerRatingDisplay = true;
  GFL_PRINT("BATTLE_PARAM_SetRegulation レーティングバトル設定 %d %d \n", rate1, rate2);
}

/**
 * @brief セットアップ済みパラメータに録画バッファを生成
 * @param[out]  bsp
 * @param[in]   heap
 */
void BATTLE_PARAM_AllocRecBuffer( BATTLE_SETUP_PARAM* bsp, gfl2::heap::HeapBase* heap )
{
  if( bsp->recBuffer == NULL )
  {
    bsp->recBuffer = GFL_NEW( heap ) u8[ BTLREC_OPERATION_BUFFER_SIZE ];
    ICHI_PRINT("録画バッファ生成\n");
  }
}

/*
 *  @brief デバッグ用フラグリセット
 */
void BTL_SETUP_ResetDebugFlag( BATTLE_SETUP_PARAM* bsp )
{
  bsp->DebugFlagBit = 0;
}
/*
 *  @brief デバッグ用フラグセット
 */
void BTL_SETUP_SetDebugFlag( BATTLE_SETUP_PARAM* bsp, BtlDebugFlag flag )
{
  GFL_ASSERT(flag < BTL_DEBUGFLAG_MAX); // @check

  bsp->DebugFlagBit |= (1 << flag);
}
/*
 *  @brief デバッグ用フラグ取得
 */
bool BTL_SETUP_GetDebugFlag( const BATTLE_SETUP_PARAM* bsp, BtlDebugFlag flag )
{
  GFL_ASSERT(flag < BTL_DEBUGFLAG_MAX); // @check

  #if PM_DEBUG
  return (bsp->DebugFlagBit & (1 << flag)) != 0;
  #else
  return false;
  #endif
}

/*
 *  @brief バトルハウス
 */

void BTL_SETUP_BattleHouseTrainer(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  pml::PokeParty               *my_party,
  gfl2::heap::HeapBase          *pHeap )
{
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY1]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY1]==NULL);

  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );
  //初期値は負け
  bp->result = BTL_RESULT_LOSE;
  {
     bp->party[ BTL_CLIENT_PLAYER ] = GFL_NEW(pHeap) pml::PokeParty( pHeap );
     pml::PokeParty* playerParty = my_party;
     BTL_PRINT("[BTL_SETUP_BTL_INST] PlayerParty=%p, Members=%d\n", playerParty, playerParty->GetMemberCount());
     bp->party[ BTL_CLIENT_PLAYER ]->CopyFrom( *playerParty );
  }

  bp->party[ BTL_CLIENT_ENEMY1 ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY1 ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  btl::setup::btlhouseTrainer( bp, gameMan, BTL_CLIENT_ENEMY1, bp->party[BTL_CLIENT_ENEMY1], BattleInst::BattleInst::BTL_INST_ENEMY1_NO, pHeap );

  btl::setup::settingByTrainerData( bp, bp->tr_data[ BTL_CLIENT_ENEMY1 ] );

  bp->competitor = BTL_COMPETITOR_INST;
  bp->rule = rule;

  // 録画用バッファ生成
  BATTLE_PARAM_AllocRecBuffer( bp, pHeap );
}

/*
 *  @brief バトルハウス　マルチ
 */
void BTL_SETUP_BattleHouseTrainer_Multi(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  pml::PokeParty               *my_party,
  gfl2::heap::HeapBase          *pHeap )
{
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY1]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY1]==NULL);
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY2]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY2]==NULL);
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_PARTNER]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_PARTNER]==NULL);

  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );
  //初期値は負け
  bp->result = BTL_RESULT_LOSE;

  {
     bp->party[ BTL_CLIENT_PLAYER ] = GFL_NEW(pHeap) pml::PokeParty( pHeap );
     BTL_PRINT("[BTL_SETUP_BTL_INST] PlayerParty=%p, Members=%d\n", my_party, my_party->GetMemberCount());
     bp->party[ BTL_CLIENT_PLAYER ]->CopyFrom( *my_party );
  }

  bp->party[ BTL_CLIENT_ENEMY1  ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->party[ BTL_CLIENT_ENEMY2  ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->party[ BTL_CLIENT_PARTNER ] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY1  ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  bp->tr_data[ BTL_CLIENT_ENEMY2  ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  bp->tr_data[ BTL_CLIENT_PARTNER ] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);

  btl::setup::btlhouseTrainer( bp, gameMan, BTL_CLIENT_ENEMY1, bp->party[BTL_CLIENT_ENEMY1], BattleInst::BattleInst::BTL_INST_ENEMY1_NO, pHeap );
  btl::setup::btlhouseTrainer( bp, gameMan, BTL_CLIENT_ENEMY2, bp->party[BTL_CLIENT_ENEMY2], BattleInst::BattleInst::BTL_INST_ENEMY2_NO, pHeap );
  btl::setup::btlhouseTrainer( bp, gameMan, BTL_CLIENT_PARTNER, bp->party[BTL_CLIENT_PARTNER], BattleInst::BattleInst::BTL_INST_PARTNER_NO, pHeap );

  btl::setup::settingByTrainerData( bp, bp->tr_data[ BTL_CLIENT_ENEMY1 ] );

  bp->competitor = BTL_COMPETITOR_INST;
  bp->rule = rule;
  bp->multiMode = BTL_MULTIMODE_PA_AA;

  // 録画用バッファ生成
  BATTLE_PARAM_AllocRecBuffer( bp, pHeap );
}

/**
 * トレーナー戦　ロイヤル施設　用パラメータセット
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    sit
 * @param[in]    my_party
 * @param[in]    pHeap
 */
void BTL_SETUP_BattleRoyalInst(
  BATTLE_SETUP_PARAM            *bp,
  GameSys::GameManager          *gameMan,
  const BTL_FIELD_SITUATION     *sit,
  pml::PokeParty                *my_party,
  gfl2::heap::HeapBase          *p_heap )
{
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY1]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY1]==NULL);
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY2]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY2]==NULL);
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_PARTNER]==NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_PARTNER]==NULL);

  BATTLE_SETUP_Init( bp );

  btl::setup::common_base( bp, gameMan, sit );

  {
    bp->party[ BTL_CLIENT_PLAYER ] = GFL_NEW(p_heap) pml::PokeParty(p_heap);
    pml::PokeParty* playerParty = my_party;
    bp->party[ BTL_CLIENT_PLAYER ]->CopyFrom( *playerParty );
  }

  bp->party[ BTL_CLIENT_PARTNER ]   = GFL_NEW(p_heap) pml::PokeParty(p_heap);
  bp->party[ BTL_CLIENT_ENEMY1 ]    = GFL_NEW(p_heap) pml::PokeParty(p_heap);
  bp->party[ BTL_CLIENT_ENEMY2 ]    = GFL_NEW(p_heap) pml::PokeParty(p_heap);
  bp->tr_data[ BTL_CLIENT_PARTNER ] = GFL_NEW(p_heap) BSP_TRAINER_DATA(p_heap);
  bp->tr_data[ BTL_CLIENT_ENEMY1  ] = GFL_NEW(p_heap) BSP_TRAINER_DATA(p_heap);
  bp->tr_data[ BTL_CLIENT_ENEMY2  ] = GFL_NEW(p_heap) BSP_TRAINER_DATA(p_heap);

  btl::setup::royalinstTrainer( bp, gameMan, BTL_CLIENT_PARTNER, bp->party[BTL_CLIENT_PARTNER], BattleInst::BattleInst::BTL_INST_ROYAL_ENEMEY_INDEX_0, p_heap );
  btl::setup::royalinstTrainer( bp, gameMan, BTL_CLIENT_ENEMY1, bp->party[BTL_CLIENT_ENEMY1], BattleInst::BattleInst::BTL_INST_ROYAL_ENEMEY_INDEX_1, p_heap );
  btl::setup::royalinstTrainer( bp, gameMan, BTL_CLIENT_ENEMY2, bp->party[BTL_CLIENT_ENEMY2], BattleInst::BattleInst::BTL_INST_ROYAL_ENEMEY_INDEX_2, p_heap );

  // @fix GFNMCat[3568] ロイヤルは誰が相手でも同じバトルエフェクト・曲にするため、敵トレーナーの設定ではなく、専用ラベルの設定を適用する
  BATTLE_PARAM_SetBattleEffect( bp, BATTLE_EFFECT_TR_ROYAL );

  // @fix GFNMCat[4935] 対戦相手を施設トレーナーにする
  bp->competitor  = BTL_COMPETITOR_INST;
  bp->rule        = BTL_RULE_ROYAL;
  bp->multiMode   = BTL_MULTIMODE_PA_AA;

  // 録画用バッファ生成
  BATTLE_PARAM_AllocRecBuffer( bp, p_heap );
}

/*
 *  @brief ボイスチャットする場合
 */
void BTL_SETUP_VoiceChatOn(BATTLE_SETUP_PARAM *bp)
{
  bp->bVoiceChat= true;
}

/**
 *  @brief バトルクライントIDからコーナー色を取得
 */
CornerColor GetCornerColorFromBtlClientId( BTL_CLIENT_ID btlClietnId )
{
  switch( btlClietnId ){
  case BTL_CLIENT_ROYAL1: return CORRER_COLOR_GREEN;
  case BTL_CLIENT_ROYAL2: return CORRER_COLOR_YELLOW; 
  case BTL_CLIENT_ROYAL3: return CORRER_COLOR_RED;
  case BTL_CLIENT_ROYAL4: return CORRER_COLOR_BLUE; 
  }
  GFL_ASSERT(0);
  return CORRER_COLOR_GREEN;
}



//=============================================================================================
/**
 * @brief  録画データから構築した BATTLE_SETUP_PARAM を、録画対戦（模擬戦）モードにコンバートする
 * @param[io]  bsp              録画データから構築した BATTLE_SETUP_PARAM （コンバートされ、内容が書き換わります）
 * @param[in]  targetClientID   模擬戦で戦いたいトレーナーデータが格納されている ClientID （0 or 1）
 * @param[in]  gameData         プレイヤーのMyStatus, Partyデータを取得するためのデータ
 * @param[in]  pHeap            必要に応じて対戦相手用の MyStatus を構築するためのヒープ
 * @retval     bool     正しくコンバートされたら true
 */
//=============================================================================================
bool BTL_SETUP_ConvertToRecordFightMode( BATTLE_SETUP_PARAM* bsp, u8 targetClientID, GameSys::GameData* gameData, gfl2::heap::HeapBase* pHeap )
{
  if( bsp->recordDataMode != BTL_RECORDDATA_PLAY ){
    GFL_ASSERT(0);
    return false;
  }
  
  //BTS3834 施設のBSPはエンカウントが2Dになっている。
  //そのため3Dに直さなければならない。
  //ただ、BGMはそのままが良いという事なので、
  //eff_kindのみ書き換える。
  if( bsp->playerStatus[targetClientID] != NULL )
  {
    // エンカウントをトレーナー3Dに。
    bsp->btlEffData.enceff.eff_kind = BTL_CUTIN_COMM;
  }
  else
  {
    // エンカウントをトレーナー2Dに。
    bsp->btlEffData.enceff.eff_kind = BTL_CUTIN_TRAINER;
  }

  // ClientID=BTL_CLIENT_PLAYER の相手と戦いたい場合、そのデータを BTL_CLIENT_ENEMY1 にコピーしておく
  BTL_PRINT("[CONV] TargetClient=%d, \n", targetClientID);
  if( targetClientID == BTL_CLIENT_PLAYER )
  {
    // target が プレイヤーデータの場合
    if( bsp->playerStatus[BTL_CLIENT_PLAYER] != NULL )
    {
      BTL_PRINT("[CONV] TargetClientData = Player's Data\n");
      if( bsp->playerStatus[BTL_CLIENT_ENEMY1] == NULL ){
        BTL_PRINT("[CONV] プレイヤーデータのウツワが無いのでnewします\n");
        bsp->playerStatus[BTL_CLIENT_ENEMY1] = GFL_NEW(pHeap) Savedata::MyStatus();
      }
      bsp->playerStatus[BTL_CLIENT_ENEMY1]->CopyFrom( *(bsp->playerStatus[BTL_CLIENT_PLAYER]) );
    }
    // target 施設トレーナーの場合
    else
    {
      if( bsp->tr_data[BTL_CLIENT_ENEMY1] != NULL )
      {
        BTL_PRINT("[CONV] 施設トレーナーデータの中身を書き換える手段が無いのでポインタをスワップする\n");
        BSP_TRAINER_DATA* tmp = bsp->tr_data[ BTL_CLIENT_ENEMY1 ];
        bsp->tr_data[ BTL_CLIENT_ENEMY1 ] = bsp->tr_data[ BTL_CLIENT_PLAYER ];
        bsp->tr_data[ BTL_CLIENT_PLAYER ] = tmp;
      }else{
        GFL_ASSERT(0);
        return false;
      }
    }

    // Partyデータをコピー
    if( (bsp->party[BTL_CLIENT_PLAYER] != NULL) && (bsp->party[BTL_CLIENT_ENEMY1] != NULL) ){
      bsp->party[BTL_CLIENT_ENEMY1]->CopyFrom( *(bsp->party[BTL_CLIENT_PLAYER]) );
    }else{
      GFL_ASSERT(0);
      return false;
    }
  }

  // BTL_CLIENT_PLAYER に、現プレイヤーのデータをコピーしておく
  if( bsp->playerStatus[BTL_CLIENT_PLAYER] ){
    bsp->playerStatus[BTL_CLIENT_PLAYER]->CopyFrom( *(gameData->GetPlayerStatus()) );
  }else{
    GFL_ASSERT(0);
    return false;
  }
  bsp->party[BTL_CLIENT_PLAYER]->CopyFrom( *(gameData->GetPlayerParty()) );


  // Competitor は 施設トレーナー扱いに、録画データモードは模擬戦モードにする
  bsp->commPos = BTL_CLIENT_PLAYER;
  bsp->competitor = BTL_COMPETITOR_INST;
  bsp->recordDataMode = BTL_RECORDDATA_FIGHT;


  return true;
}

void BTL_SETUP_BattleFesTrainer(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  pml::PokeParty               *my_party,
  gfl2::heap::HeapBase          *pHeap)
{
  GFL_ASSERT_STOP(bp->party[BTL_CLIENT_ENEMY1] == NULL);
  GFL_ASSERT_STOP(bp->tr_data[BTL_CLIENT_ENEMY1] == NULL);
  GFL_ASSERT_STOP(bp->playerStatus[BTL_CLIENT_PARTNER] == NULL);
  GFL_ASSERT_STOP(bp->playerStatus[BTL_CLIENT_ENEMY2] == NULL);

  BATTLE_SETUP_Init(bp);

  btl::setup::common_base(bp, gameMan, sit);
  //初期値は負け
  bp->result = BTL_RESULT_LOSE;
  {
    bp->party[BTL_CLIENT_PLAYER] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
    pml::PokeParty* playerParty = my_party;
    BTL_PRINT("[BTL_SETUP_BTL_INST] PlayerParty=%p, Members=%d\n", playerParty, playerParty->GetMemberCount());
    bp->party[BTL_CLIENT_PLAYER]->CopyFrom(*playerParty);
  }

  bp->party[BTL_CLIENT_ENEMY1] = GFL_NEW(pHeap) pml::PokeParty(pHeap);
  bp->tr_data[BTL_CLIENT_ENEMY1] = GFL_NEW(pHeap) BSP_TRAINER_DATA(pHeap);
  btl::setup::btlfesTrainer(bp, gameMan, BTL_CLIENT_ENEMY1, bp->party[BTL_CLIENT_ENEMY1], pHeap);

  btl::setup::settingByTrainerData(bp, bp->tr_data[BTL_CLIENT_ENEMY1]);

  bp->competitor = BTL_COMPETITOR_INST;
  bp->rule = BTL_RULE_SINGLE;

  // 録画用バッファ生成
  BATTLE_PARAM_AllocRecBuffer(bp, pHeap);
}
