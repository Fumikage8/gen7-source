//=============================================================================
/**
 * @file   btl_BattleResult.cpp
 * @date   2015/08/24 11:45:12
 * @author obata_toshihiro
 * @brief  戦闘結果の構築
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <Trainer/Trainer/include/TrainerTypeData.h>
#include <PokeTool/include/NatsukiManager.h>
#include "./btl_ui.h"
#include "./btl_rec.h"
#include "./btl_BattleResult.h"
#include "./btl_PokeID.h"
#include "./btl_mainmodule.h"
#include "./btl_ServerFlow.h"
#include "./btl_pokecon.h"
#include "./btl_client.h"
#include "./btl_RoyalRankingContainer.h"
#include "./btl_ZenryokuWazaStatus.h"
#include "./btl_UltraBeast.h"

GFL_NAMESPACE_BEGIN( btl )


 //-------------------------------------------------------------------------
 /**
  * @brief バトル内パーティデータを、結果としてセットアップパラメータに書き戻す
  *
  * @param[in,out] setupParam        結果の出力先
  * @param[in,out] pokeconForServer  サーバー側の POKECON
  * @param[in,out] pokeconForClient  クライアント側の POKECON
  * @param         mainModule        参照する MainModule
  * @param         myClientId        プレイヤのクライアントID
  */
 //-------------------------------------------------------------------------
 void BattleResult::ApplyBattlePartyData( 
   BATTLE_SETUP_PARAM* setupParam, 
   POKECON*            pokeconForServer, 
   POKECON*            pokeconForClient, 
   const MainModule*   mainModule,
   u8                  myClientId )
{
  // 野生or通常トレーナー戦：経験値・レベルアップ・なつき度を反映
  if( mainModule->IsCompetitorScenarioMode() )
  {
    PokeParty* srcParty;

    // ↓ウルトラバーストを元に戻す処理は、ここでなされている
    pokeconForServer->RefrectBtlPartyStartOrder( myClientId, mainModule->GetClientBasePokeID( myClientId ), setupParam->fightPokeIndex  );
    srcParty = pokeconForServer->GetSrcParty( myClientId );

    // ジムリーダー、四天王、チャンピオンならなつき度アップ
    if ( setupParam->competitor == BTL_COMPETITOR_TRAINER )
    {
      trainer::TrTypeGroup trGroup = mainModule->GetClientTrainerGroup( BTL_CLIENT_ENEMY1 );
      if ( trainer::TrainerTypeData::IsBossGroup( trGroup ) )
      {
        u32  pokeCnt = srcParty->GetMemberCount();
        const BTL_FIELD_SITUATION* sit = mainModule->GetFieldSituation();
        for(u32 i = 0; i < pokeCnt; ++i)
        {
          pml::pokepara::PokemonParam *pp;
          pp = srcParty->GetMemberPointer( i );
          PokeTool::NatsukiManager::Calc( pp, PokeTool::NATSUKI_TYPE_BOSS_BATTLE, sit->place_id );
        }
      }
    }

    pml::pokepara::PokemonParam *ppOrg, *ppResult;
    u32  pokeCnt = srcParty->GetMemberCount();

    // トレーナー戦は装備アイテムを元に戻す
    if( (setupParam->competitor == BTL_COMPETITOR_TRAINER) )
    {
      u16  OrgItemNo;
      for(u32 i=0; i<pokeCnt; ++i)
      {
        ppOrg     = setupParam->party[BTL_CLIENT_PLAYER]->GetMemberPointer( i );
        ppResult  = srcParty->GetMemberPointer( i );
        OrgItemNo = ppOrg->GetItem();

        // 消費アイテムでなければ、ただ元に戻すだけ
        if( !calc::ITEM_GetParam(OrgItemNo, item::ITEM_DATA::PRM_ID_SPEND) ){
          ppResult->SetItem( OrgItemNo );
        // 消費アイテムであれば、他のアイテムに変わっていた場合に消す
        }else if( ppResult->GetItem() != OrgItemNo ){
          ppResult->SetItem( ITEM_DUMMY_DATA );
        }
      }
    }

    // メガ進化、Ｘのフォルムチェンジを元に戻す
    for(u32 i=0; i<pokeCnt; ++i)
    {
      ppResult  = srcParty->GetMemberPointer( i );
      ppResult->ResetMegaEvolve();
    }
    BattleResult::srcParty_FormReset_OnBattleEnd( srcParty, *setupParam->party[BTL_CLIENT_PLAYER] );

    // Levelが変わってないのに HP最大値が増えていたら、その分HPも加算（努力値によるもの）
    for(u32 i=0; i<pokeCnt; ++i)
    {
      ppOrg     = setupParam->party[BTL_CLIENT_PLAYER]->GetMemberPointer( i );
      ppResult  = srcParty->GetMemberPointer( i );
      if( !ppResult->IsHpZero() )
      {
        u32 hpMaxOrg = ppOrg->GetMaxHp();
        u32 hpMax    = ppResult->GetMaxHp();
        if( (hpMaxOrg < hpMax) && (ppOrg->GetLevel() == ppResult->GetLevel()) ){
          u32 diff = hpMax - hpMaxOrg;
          ppResult->RecoverHp( diff );
        }
      }
    }

    setupParam->party[ BTL_CLIENT_PLAYER ]->CopyFrom( *srcParty );
  }

  // 野生戦：捕獲ポケモン情報
  if( setupParam->competitor == BTL_COMPETITOR_WILD )
  {
    u8 clientID = mainModule->GetOpponentClientID( myClientId, 0 );
    PokeParty* srcParty;

    pokeconForServer->RefrectBtlParty( clientID, true );
    srcParty = pokeconForServer->GetSrcParty( clientID );
    srcParty_FormReset_OnBattleEnd( srcParty, *setupParam->party[BTL_CLIENT_ENEMY1] );
    clearUnknownUBNickName( mainModule, srcParty );
    
    setupParam->party[ BTL_CLIENT_ENEMY1 ]->CopyFrom( *srcParty );
  }

  // デモ以外全て：状態異常コード書き戻し
  if( setupParam->competitor != BTL_COMPETITOR_DEMO_CAPTURE )
  {
    u32 clientID;

    for(u32 id=0; id<BTL_CLIENT_MAX; ++id)
    {
      clientID = mainModule->ClientIDtoRelation( myClientId, id );
      for(u32 p=0; p<BTL_PARTY_MEMBER_MAX; ++p){
        setupParam->party_state[ clientID ][ p ] = BTL_POKESTATE_NORMAL;
      }

//      BTL_PRINT("myClientID=%d, idx=%d , clientID=%d ... \n", myClientId, id, clientID );

      if( mainModule->IsExistClient(clientID) )
      {
        BTL_PARTY* party = pokeconForClient->GetPartyData( clientID );

        u32 numMembers = party->GetMemberCount();
        u32 HPSum, MaxHPSum;
        u8 orgPokeID = mainModule->GetClientBasePokeID( clientID );
        u8 idx;

        HPSum = MaxHPSum = 0;
        for(u32 p=0; p<numMembers; ++p)
        {
          BTL_POKEPARAM* bpp = party->GetMemberData( p );
          idx = bpp->GetID() - orgPokeID;
          HPSum += bpp->GetValue( BTL_POKEPARAM::BPP_HP );
          MaxHPSum += bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP );

          if( bpp->IsDead() ){
            setupParam->party_state[ clientID ][ idx ] = BTL_POKESTATE_DEAD;
          }
          else if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL ){
            setupParam->party_state[ clientID ][ idx ] = BTL_POKESTATE_SICK;
          }
        }

        setupParam->restHPRatio[ clientID ] = (HPSum * 100) / MaxHPSum;
      }
    }
  }

  // ポケモン毎の戦闘結果
  if( mainModule->IsCompetitorScenarioMode() )
  {
    const PokeParty* srcParty = pokeconForClient->GetSrcParty( myClientId );
    const BTL_PARTY* btlParty = pokeconForClient->GetPartyDataConst( myClientId );   // pokeconForServer ではダメ！詳しくは @fix NMCat[1778] を参照してください。

    const u32 basePokeId = mainModule->GetClientBasePokeID( myClientId );
    const u32 memberCount = srcParty->GetMemberCount();    

    for( u32 memberIndex=0; memberIndex<memberCount; ++memberIndex )
    {
      for( u32 i=0; i<memberCount; ++i )
      {
        const BTL_POKEPARAM* bpp = btlParty->GetMemberDataConst( i );
        if( bpp->GetID() == ( basePokeId + memberIndex ) )
        {
          setupParam->pokeResult[ memberIndex ].totalTurnCount = bpp->GetTotalTurnCount();
          setupParam->pokeResult[ memberIndex ].dirtType       = bpp->GetDirtType();
          BTL_PRINT( "[BattleResult] ポケモン毎の戦闘結果( pokeId=%d )\n", bpp->GetID() );
          BTL_PRINT( "　場に出たターンの総数 %d\n", setupParam->pokeResult[ memberIndex ].totalTurnCount );
          BTL_PRINT( "　汚れの種類           %d\n", setupParam->pokeResult[ memberIndex ].dirtType );
          break;
        }
      }
    }
  }
}



/**
 * @brief 戦闘終了時のフォルムチェンジ
 * @param[out] party     操作対象のパーティ
 * @param      orgParty  戦闘開始時のパーティ
 */
void BattleResult::srcParty_FormReset_OnBattleEnd( PokeParty* party, const PokeParty& orgParty )
{
  u32 memberCount = party->GetMemberCount();
  for( u32 i=0; i<memberCount; ++i )
  {
    pml::pokepara::PokemonParam* param = party->GetMemberPointer( i );
    const pml::pokepara::PokemonParam* orgParam = orgParty.GetMemberPointerConst( i );
    srcParty_FormReset_OnBattleEnd( param, *orgParam );
  }
}

/**
 * @brief 戦闘終了時のフォルムチェンジ
 * @param[out] pokeParam  フォルムチェンジさせるポケモン
 * @param      orgParam   戦闘開始時のパラメータ
 */
void BattleResult::srcParty_FormReset_OnBattleEnd( pml::pokepara::PokemonParam* pokeParam, const pml::pokepara::PokemonParam& orgParam )
{
  const MonsNo monsno = pokeParam->GetMonsNo();

  // ゼルネアス
  if( monsno == MONSNO_ZERUNEASU )
  {
    pokeParam->ChangeFormNo( FORMNO_ZERUNEASU_NORMAL );
  }

  // カイオーガ
  if( monsno == MONSNO_KAIOOGA )
  {
    pokeParam->ChangeFormNo( FORMNO_KAIOOGA_NORMAL );
  }

  // グラードン
  if( monsno == MONSNO_GURAADON )
  {
    pokeParam->ChangeFormNo( FORMNO_GURAADON_NORMAL );
  }

  // ヨワシ
  if( monsno == MONSNO_YOWASI )
  {
    pokeParam->ChangeFormNo( FORMNO_YOWASI_NORMAL );
  }

  // メテノ
  if( monsno == MONSNO_NAGAREBOSI )
  {
    FormNo form_old  = pokeParam->GetFormNo();
    FormNo form_next = pml::personal::METENO_GetCoreFormNo( form_old );
    pokeParam->ChangeFormNo( form_next );
  }

  // ジガルデ
  if( monsno == MONSNO_ZIGARUDE )
  {
    FormNo form_org = orgParam.GetFormNo();
    pokeParam->ChangeFormNo( form_org );
  }
}

/**
 * @brief 未発見ウルトラビーストのニックネームをクリアする
 */
void BattleResult::clearUnknownUBNickName( const MainModule* mainModule, PokeParty* party )
{
  u32 memberCount = party->GetMemberCount();
  for( u32 i=0; i<memberCount; ++i )
  {
    pml::pokepara::PokemonParam* param = party->GetMemberPointer( i );
    const MonsNo monsno = param->GetMonsNo();

    if( UltraBeast::IsUnknownNamePokemon( *mainModule, monsno ) )
    {
      param->SetDefaultNickName();
    }
  }
}


//-------------------------------------------------------------------------
/**
 * @brief バトルロイヤルの結果を、セットアップパラメータに書き戻す
 *
 * @param[in,out] setupParam   結果の出力先
 * @param         mainModule   参照する MainModule
 * @param         pokecon      参照する POKECON
 * @param         myClientId   プレイヤのクライアントID
 */
//-------------------------------------------------------------------------
void BattleResult::ApplyBattleRoyalResult( 
  BATTLE_SETUP_PARAM* setupParam, 
  const MainModule*   mainModule,
  const POKECON*      pokecon,
  u8                  myClientId )
{
  ApplyBattleRoyalResult_Ranking( &setupParam->battleRoyalResult, mainModule, myClientId );
  ApplyBattleRoyalResult_KillCount( &setupParam->battleRoyalResult, pokecon );
  ApplyBattleRoyalResult_Hp( &setupParam->battleRoyalResult, pokecon );
  ApplyBattleRoyalResult_LastPoke( &setupParam->battleRoyalResult, pokecon );

#if PM_DEBUG
  BTL_PRINT( "[BattleResult] バトルロイヤルの結果\n" );
  for( u8 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    BTL_PRINT( "clientId=%d, lastPokePartyIndex=%d, killCount=%d, ranking=%d \n",
      clientId, 
      setupParam->battleRoyalResult.lastPokePartyIndex[ clientId ], 
      setupParam->battleRoyalResult.killCount[ clientId ],
      setupParam->battleRoyalResult.clientRanking[ clientId ] );

    for( u8 memberIndex=0; memberIndex<pml::PokeParty::MAX_MEMBERS; ++memberIndex )
    {
      BTL_PRINT( "[%d] hp=%d/%d\n", 
        memberIndex, 
        setupParam->battleRoyalResult.restHp[ clientId ][ memberIndex ],
        setupParam->battleRoyalResult.maxHp[ clientId ][ memberIndex ] );
    }
  }
#endif
}

/**
 * @brief バトルロイヤルの結果を、セットアップパラメータに書き戻す( 順位 )
 *
 * @param[out] result       結果の出力先
 * @param      mainModule   参照する MainModule
 * @param      myClientId   プレイヤのクライアントID
 */
void BattleResult::ApplyBattleRoyalResult_Ranking( BattleRoyalResult* result, const MainModule* mainModule, u8 myClientId )
{
  const BTL_CLIENT* myClient = mainModule->GetClient( myClientId );
  const RoyalRankingContainer& rankingContainer = myClient->GetRoyalRankingContainer();
  for( u8 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    const u8 ranking = rankingContainer.GetClientRanking( clientId );
    result->clientRanking[ clientId ]= ranking;
  }

#if PM_DEBUG
  if( mainModule->IsCompetitorScenarioMode() )
  {
    u8 playerClientID = mainModule->GetPlayerClientID();

    // Rボタンで勝ち
    if( ui::CheckKeyCont( BUTTON_R ) )
    {
      result->clientRanking[ playerClientID ] = 0;
      GFL_PRINT( "[BattleResult] デバッグ機能で、バトルロイヤルの結果を書き換えました( 勝ち )\n" );
    }

    // Lボタンで負け
    if( ui::CheckKeyCont( BUTTON_L ) )
    {
      result->clientRanking[ playerClientID ] = 3;
      GFL_PRINT( "[BattleResult] デバッグ機能で、バトルロイヤルの結果を書き換えました( 負け )\n" );
    }
  }
#endif
}

/**
 * @brief バトルロイヤルの結果を、セットアップパラメータに書き戻す( 倒したポケモンの数 )
 *
 * @param[out] result   結果の出力先
 * @param      pokecon  参照する POKECON
 */
void BattleResult::ApplyBattleRoyalResult_KillCount( BattleRoyalResult* result, const POKECON* pokecon )
{
  for( u8 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    const BTL_PARTY* party = pokecon->GetPartyDataConst( clientId );
    result->killCount[ clientId ] = party->GetTotalKillCount();
  }
}

/**
 * @brief バトルロイヤルの結果を、セットアップパラメータに書き戻す( HP )
 *
 * @param[out] result   結果の出力先
 * @param      pokecon  参照する POKECON
 */
void BattleResult::ApplyBattleRoyalResult_Hp( BattleRoyalResult* result, const POKECON* pokecon )
{
  // 初期化
  for( u8 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    for( u8 memberIndex=0; memberIndex<pml::PokeParty::MAX_MEMBERS; ++memberIndex )
    {
      result->maxHp[ clientId ][ memberIndex ]  = 0;
      result->restHp[ clientId ][ memberIndex ] = 0;
    }
  }

  for( u8 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    const BTL_PARTY* party = pokecon->GetPartyDataConst( clientId );
    const u8 memberCount = party->GetMemberCount();
    for( u8 memberIndex=0; memberIndex<memberCount; ++memberIndex )
    {
      const BTL_POKEPARAM* pokeParam =party->GetMemberDataConst( memberIndex );
      const u8 startMemberIndex = PokeID::PokeIdToStartMemberIndex( pokeParam->GetID() );   // 戦闘開始時のパーティの並び順に格納する
      result->maxHp[ clientId ][ startMemberIndex ]  = pokeParam->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
      result->restHp[ clientId ][ startMemberIndex ] = pokeParam->GetValue( BTL_POKEPARAM::BPP_HP );
    }
  }
}

/**
 * @brief バトルロイヤルの結果を、セットアップパラメータに書き戻す( 最後に場に残っていたポケモン )
 *
 * @param[out] result   結果の出力先
 * @param      pokecon  参照する POKECON
 */
void BattleResult::ApplyBattleRoyalResult_LastPoke( BattleRoyalResult* result, const POKECON* pokecon )
{
  for( u8 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    const BTL_POKEPARAM* pokeParam = pokecon->GetClientPokeDataConst( clientId, 0 );
    const u8 pokeID = pokeParam->GetID();
    result->lastPokePartyIndex[ clientId ] = ( pokeParam->IsDead() ) ? ( -1 ) : ( PokeID::PokeIdToStartMemberIndex( pokeID ) );
  }
}


//-------------------------------------------------------------------------
/**
 * @brief * @brief ぜんりょくパワー関連の結果を、セットアップパラメータに書き戻す
 *
 * @param[out] setupParam                結果の出力先
 * @param      playerZenryokuWazaStatus  プレイヤの全力技ステータス管理オブジェクト
 */
//-------------------------------------------------------------------------
void BattleResult::ApplyZenryokuPowerResult( 
  BATTLE_SETUP_PARAM*       setupParam, 
  const ZenryokuWazaStatus& playerZenryokuWazaStatus )
{
  setupParam->zenryokuWazaUsedFlag = playerZenryokuWazaStatus.IsZenryokuWazaUsed();
}


//-------------------------------------------------------------------------
/**
 * @brief お手入れ対象となるポケモンを、セットアップパラメータに書き戻す
 *
 * @param[out] setupParam   結果の出力先
 * @param      mainModule   参照する MainModule
 * @param      pokecon      参照する POKECON
 * @param      myClientId   プレイヤのクライアントID
 */
//-------------------------------------------------------------------------
void BattleResult::ApplyOteireTarget(
  BATTLE_SETUP_PARAM* setupParam, 
  const MainModule*   mainModule,
  const POKECON*      pokecon,
  u8                  myClientId )
{
  setupParam->oteireFlag = false;
  setupParam->oteireTargetIndex = 0;

  // お手入れボタンが押されていない
  if( !mainModule->IsOteireButtonPressed() )
  {
    return;
  }

  // お手入れ対象が見当たらない
  const u8 oteireTargetID = mainModule->GetOteireTargetPokeID();
  if( oteireTargetID == BTL_POKEID_NULL )
  {
    return;
  }

  setupParam->oteireFlag = true;
  setupParam->oteireTargetIndex = PokeID::PokeIdToStartMemberIndex( oteireTargetID );
  BTL_PRINT( "[BattleResult] お手入れ対象インデックス %d\n", setupParam->oteireTargetIndex );
}

//-------------------------------------------------------------------------
/**
  * @brief バトル録画のヘッダ情報を、セットアップパラメータに格納する
  *
  * @param[out] setupParam   結果の出力先
  * @param      mainModule   参照する MainModule
  * @param      myClientId   プレイヤのクライアントID
  * @param      result       バトルの結果
  */
//-------------------------------------------------------------------------
void BattleResult::ApplyRecordHeader(
  BATTLE_SETUP_PARAM* setupParam, 
  const MainModule*   mainModule,
  u8                  myClientId,
  BtlResult           result )
{
  setupParam->recHeader.turnNum = BattleResult::GetRecTurnCount( setupParam->recBuffer, setupParam->recDataSize );
  setupParam->recHeader.result1 = BattleResult::GetRecResult1( result );
  setupParam->recHeader.result2 = BattleResult::GetRecResult2( mainModule, result );
  BTL_PRINT( "[BattleResult] バトル録画のヘッダ情報\n" );
  BTL_PRINT( "　ターン数 %d\n", setupParam->recHeader.turnNum );
  BTL_PRINT( "　結果１   %d\n", setupParam->recHeader.result1 );
  BTL_PRINT( "　結果２   %d\n", setupParam->recHeader.result2 );
}

/**
 * @brief バトル録画ヘッダ用のターン数を取得する
 * @param recordData      録画データ
 * @param recordDataSize  録画データのサイズ
 */
u32 BattleResult::GetRecTurnCount( const void* recordData, u32 recordDataSize )
{
  if( recordData == NULL )
  {
    return 0;
  }

  rec::Reader reader;
  reader.Init( recordData, recordDataSize );
  return reader.GetTurnCount();
}

/**
 * @brief バトル録画ヘッダ用の結果１( 勝敗 )を取得する
 * @param result  バトルの結果
 */
BtlRecordResult1 BattleResult::GetRecResult1( BtlResult result )
{
  switch( result )
  {
  case BTL_RESULT_LOSE:
  case BTL_RESULT_RUN:
    return BTL_RECORD_RESULT_1_LOSE;

  case BTL_RESULT_WIN:
  case BTL_RESULT_RUN_ENEMY:
  case BTL_RESULT_CAPTURE:
    return BTL_RECORD_RESULT_1_WIN;

  case BTL_RESULT_DRAW:
  case BTL_RESULT_COMM_ERROR:
    return BTL_RECORD_RESULT_1_DRAW;

  default:
    GFL_ASSERT(0);
    return BTL_RECORD_RESULT_1_DRAW;
  }
}

/**
 * @brief バトル録画ヘッダ用の結果２( 決着理由 )を取得する
 * @param mainModule   参照する MainModule
 * @param result       バトルの結果
 */
BtlRecordResult2 BattleResult::GetRecResult2( const MainModule* mainModule, BtlResult result )
{
  // 制限時間オーバー
  if( mainModule->CheckGameLimitTimeOver() ||
      mainModule->CheckClientLimitTimeOver() )
  {
    return BTL_RECORD_RESULT_2_TIMEOVER;
  }

  // どちらかが降参
  if( ( result == BTL_RESULT_RUN ) ||
      ( result == BTL_RESULT_RUN_ENEMY ) )
  {
    return BTL_RECORD_RESULT_2_SURRENDER;
  }

  // 全滅
  return BTL_RECORD_RESULT_2_DEAD;
}



GFL_NAMESPACE_END( btl )