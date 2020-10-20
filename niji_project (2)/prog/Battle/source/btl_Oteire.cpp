//=============================================================================
/**
 * @file   btl_Oteire.cpp
 * @date   2016/01/20 20:27:30
 * @author obata_toshihiro
 * @brief  お手入れ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <PokeTool/include/KawaigariParamCareCoreManager.h>
#include "./btl_Oteire.h"
#include "./btl_mainmodule.h"
#include "./btl_pokecon.h"
#include "./btl_PokeID.h"
#include "./btl_pokeparam.h"
#include "./btl_party.h"

GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief お手入れ対象となるポケモンのIDを取得する
 * @param[out] isFound          お手入れ対象が見つかったか？
 * @param[out] oteireTargetID   お手入れ対象ポケモンのID( 対象が見つからない場合 BTL_POKEID_NULL )
 * @param      mainModule       参照する MainModule
 * @param      pokecon          参照する POKECON
 * @param      myClientId       プレイヤのクライアントID
 */
 //-----------------------------------------------------------------------------
void Oteire::GetOteireTarget(
  bool*               isFound,
  u8*                 oteireTargetID,
  const MainModule*   mainModule,
  const POKECON*      pokecon,
  u8                  myClientId )
{
  // 最後に場にいたポケモンから選択
  GetOteireTargetInBattleField( isFound, oteireTargetID, mainModule, pokecon, myClientId );

  // 場に対象となるポケモンが存在しないなら、パーティ全体から選択
  if( *isFound == false )
  {
    GetOteireTargetInParty( isFound, oteireTargetID, mainModule, pokecon, myClientId );
  }
}

/**
 * @brief 場にいるポケモンの中から、お手入れ対象となるポケモンのIDを取得する
 * @param[out] isFound          お手入れ対象が見つかったか？
 * @param[out] oteireTargetID   お手入れ対象ポケモンのID
 * @param      mainModule       参照する MainModule
 * @param      pokecon          参照する POKECON
 * @param      myClientId       プレイヤのクライアントID
 */
void Oteire::GetOteireTargetInBattleField(
  bool*               isFound,
  u8*                 oteireTargetID,
  const MainModule*   mainModule,
  const POKECON*      pokecon,
  u8                  myClientId )
{
  *isFound = false;
  *oteireTargetID = BTL_POKEID_NULL;

  // 最後に場にいたポケモンのうち、
  // 戦闘開始時点のパーティにおいて、より前にいたポケモンを「お手入れ対象」とする
  u8 oteireTargetIndex = 0;
  const u8 coverPosNum = mainModule->GetClientCoverPosNum( myClientId );
  for( u32 posIndex=0; posIndex<coverPosNum; ++posIndex )
  {
    const BTL_POKEPARAM* poke = pokecon->GetClientPokeDataConst( myClientId, posIndex );
    if( !Oteire::IsOteireTarget( *poke ) )
    {
      continue;
    }

    const u8 pokeId = poke->GetID();
    const u8 startMemberIndex = PokeID::PokeIdToStartMemberIndex( pokeId );
    if( !( *isFound ) || 
         ( startMemberIndex < oteireTargetIndex ) )
    {
      *isFound = true;
      *oteireTargetID = pokeId;
      oteireTargetIndex = startMemberIndex;
    }
  }
}


/**
 * @brief パーティの中から、お手入れ対象となるポケモンのIDを取得する
 * @param[out] isFound          お手入れ対象が見つかったか？
 * @param[out] oteireTargetID   お手入れ対象ポケモンのID
 * @param      mainModule       参照する MainModule
 * @param      pokecon          参照する POKECON
 * @param      myClientId       プレイヤのクライアントID
 */
void Oteire::GetOteireTargetInParty(
  bool*               isFound,
  u8*                 oteireTargetID,
  const MainModule*   mainModule,
  const POKECON*      pokecon,
  u8                  myClientId )
{
  *isFound = false;
  *oteireTargetID = BTL_POKEID_NULL;

  // パーティ内の生きているポケモンのうち、
  // 戦闘開始時点のパーティにおいて、より前にいるポケモンを「お手入れ対象」とする
  u8 oteireTargetIndex = 0;
  const BTL_PARTY* party = pokecon->GetPartyDataConst( myClientId );
  const u8 memberCount = party->GetMemberCount();
  for( u32 battleMemberIndex=0; battleMemberIndex<memberCount; ++battleMemberIndex )
  {
    const BTL_POKEPARAM* poke = party->GetMemberDataConst( battleMemberIndex );
    if( !Oteire::IsOteireTarget( *poke ) )
    {
      continue;
    }

    const u8 pokeId = poke->GetID();
    const u8 startMemberIndex = PokeID::PokeIdToStartMemberIndex( pokeId );
    if( !( *isFound ) || 
         ( startMemberIndex < oteireTargetIndex ) )
    {
      *isFound = true;
      *oteireTargetID = pokeId;
      oteireTargetIndex = startMemberIndex;
    }
  }
}

//---------------------------------------------------------------------------
/**
 * @brief お手入れ対象ポケモンか存在するかチェックする
 * @param poke  チェック対象のパーティ
 * @retval true    お手入れ対象ポケモンが存在する
 * @retval false   お手入れ対象ポケモンが存在しない
 */
//---------------------------------------------------------------------------
bool Oteire::IsOteireTargetPokeExist( const BTL_PARTY& party )
{
  const u32 memberNum = party.GetMemberCount();

  for( u32 i=0; i<memberNum; ++i )
  {
    const BTL_POKEPARAM* poke = party.GetMemberDataConst( i );
    if( ( poke != NULL ) &&
        ( Oteire::IsOteireTarget( *poke ) ) )
    {
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief お手入れ対象ポケモンかどうかをチェックする
 * @param poke  チェック対象のポケモン
 * @retval true    お手入れ対象
 * @retval false   お手入れ対象でない
 */
//-----------------------------------------------------------------------------
bool Oteire::IsOteireTarget( const BTL_POKEPARAM& poke )
{
  if( poke.IsDead() )
  {
    return false;
  }

  const bool sick = Oteire::IsOteireTarget_Sick( poke );
  const bool dirt = Oteire::IsOteireTarget_Dirt( poke );
  return ( sick || dirt );
}

/**
 * @brief お手入れ対象ポケモンかどうかをチェックする( 状態異常 )
 * @param poke  チェック対象のポケモン
 * @retval true    お手入れ対象
 * @retval false   お手入れ対象でない
 */
bool Oteire::IsOteireTarget_Sick( const BTL_POKEPARAM& poke )
{
  const pml::pokepara::Sick sick = poke.GetPokeSick();

  if( sick == pml::pokepara::SICK_NULL )
  {
    return false;
  }

  // 状態異常以外の眠り( ぜったいねむり等 )は除外
  if( ( sick == pml::pokepara::SICK_NEMURI ) &&
     !( poke.CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_ONLY_SICK ) ) )
  {
    return false;
  }

  return true;
}

/**
 * @brief お手入れ対象ポケモンかどうかをチェックする( 汚れ )
 * @param poke  チェック対象のポケモン
 * @retval true    お手入れ対象
 * @retval false   お手入れ対象でない
 */
bool Oteire::IsOteireTarget_Dirt( const BTL_POKEPARAM& poke )
{
  const DirtType dirt = poke.GetDirtType();
  return ( dirt != pml::pokepara::DIRT_TYPE_NONE );
}

//---------------------------------------------------------------------------
/**
 * @brief 設定上、付いてはいけない汚れが付いていたら、汚れを取り除く
 * @param party          操作対象のパーティ
 * @param kawaigariData  汚れが付いて良いかのデータ
 */
//---------------------------------------------------------------------------
void Oteire::RemoveIllegalDirt( BTL_PARTY* party, PokeTool::KawaigariParamCareCoreManager* kawaigariData )
{
  const u32 memberNum = party->GetMemberCount();

  for( u32 i=0; i<memberNum; ++i )
  {
    BTL_POKEPARAM* poke = party->GetMemberData( i );
    Oteire::RemoveIllegalDirt( poke, kawaigariData );
  }
}

//---------------------------------------------------------------------------
/**
 * @brief 設定上、付いてはいけない汚れが付いていたら、汚れを取り除く
 * @param poke           操作対象のポケモン
 * @param kawaigariData  汚れが付いて良いかのデータ
 */
//---------------------------------------------------------------------------
void Oteire::RemoveIllegalDirt( BTL_POKEPARAM* poke, PokeTool::KawaigariParamCareCoreManager* kawaigariData )
{
  if( ( poke == NULL ) ||
      ( kawaigariData == NULL ) ||
      ( !kawaigariData->IsLoaded() ) )
  {
    GFL_ASSERT(0);
    return;
  }

  if( Oteire::IsIllegalDirt( *poke, kawaigariData ) )
  {
    BTL_PRINT( "[Oteire] 設定上、付着してはいけない汚れをクリアします( monsno=%d, formno=%d, sex=%d, dirtType=%d )\n", poke->GetMonsNo(), poke->GetFormNo(), poke->GetValue( BTL_POKEPARAM::BPP_SEX ), poke->GetDirtType() );
    poke->SetDirtType( pml::pokepara::DIRT_TYPE_NONE );
  }
}

/**
 * @brief 設定上、付いてはいけない汚れが付いているか？
 * @param poke           チェック対象のポケモン
 * @param kawaigariData  汚れが付いて良いかのデータ
 * @retval true   付いてはいけない汚れが付いている
 * @retval false  付いてはいけない汚れが付いていない
 */
bool Oteire::IsIllegalDirt( const BTL_POKEPARAM& poke, PokeTool::KawaigariParamCareCoreManager* kawaigariData )
{
  // 瀕死なら、汚れは付かない
  if( poke.IsDead() )
  {
    return true;
  }

  const MonsNo   monsno   = static_cast<MonsNo>( poke.GetMonsNo() );
  const FormNo   formno   = poke.GetFormNo();
  const Sex      sex      = static_cast<Sex>( poke.GetValue( BTL_POKEPARAM::BPP_SEX ) );
  const DirtType dirtType = poke.GetDirtType();

  switch( dirtType )
  {
  case pml::pokepara::DIRT_TYPE_HAIR:   return !kawaigariData->TypeEnabled_Hair( monsno, formno, sex );
  case pml::pokepara::DIRT_TYPE_SAND:   return !kawaigariData->TypeEnabled_Dust( monsno, formno, sex );
  case pml::pokepara::DIRT_TYPE_STAIN:  return !kawaigariData->TypeEnabled_Dirt( monsno, formno, sex );
  case pml::pokepara::DIRT_TYPE_WATER:  return !kawaigariData->TypeEnabled_Water( monsno, formno, sex );
  }

  return false;
}


GFL_NAMESPACE_END( btl )