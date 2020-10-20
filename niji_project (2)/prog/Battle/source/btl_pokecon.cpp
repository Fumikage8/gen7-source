#include <PokeTool/include/NakayoshiManager.h>

#include "btl_common.h"
#include "btl_mainmodule.h"
#include "btl_pokecon.h"
#include "btl_PokeID.h"

#include "btl_SideEffectManager.h"
#include "btl_SideEffect.h"
#include "btl_PosEffectManager.h"
#include "btl_PosEffect.h"


GFL_NAMESPACE_BEGIN(btl)

//=======================================================================================================
// statics
//=======================================================================================================
PokeParty* POKECON::s_tmpPokeParty = NULL;

void POKECON::InitSystem( HeapHandle heap )
{
  if( s_tmpPokeParty == NULL )
  {
    s_tmpPokeParty = GFL_NEW(heap) PokeParty(heap);
  }
}
void POKECON::QuitSystem( void )
{
  if( s_tmpPokeParty )
  {
    GFL_DELETE s_tmpPokeParty;
    s_tmpPokeParty = NULL;
  }
}

//=======================================================================================================
// BTL_POKE_CONTAINER
//=======================================================================================================

POKECON::POKECON( HeapHandle heap  )
 : m_mainModule( NULL )
 , m_latestIntrudePokeID(BTL_POKEID_NULL)
{
  m_sideEffectManager = GFL_NEW( heap ) SideEffectManager( heap );
  m_posEffectManager = GFL_NEW( heap ) PosEffectManager( heap );

  for( u32 i=0; i<GFL_NELEMS(m_storedPokeParam); ++i )
  {
    m_storedPokeParam[i] = GFL_NEW( heap ) BTL_POKEPARAM();
  }
}

POKECON::~POKECON( void )
{
  GFL_SAFE_DELETE( m_posEffectManager );
  GFL_SAFE_DELETE( m_sideEffectManager );
  
  for( u32 i=0; i<GFL_NELEMS(m_activePokeParam); ++i )
  {
    GFL_SAFE_DELETE( m_activePokeParam[i] );
  }

  for( u32 i=0; i<GFL_NELEMS(m_storedPokeParam); ++i )
  {
    GFL_SAFE_DELETE( m_storedPokeParam[i] );
  }
}


bool POKECON::IsInitialized( void )  const
{
  return (m_mainModule != NULL);
}


void POKECON::Init( MainModule* mainModule, bool fForServer )
{
  int i;

  m_mainModule = mainModule;
  m_fForServer = fForServer;
  m_fieldStatus.Init( fForServer );
  m_sideEffectManager->RemoveAllSideEffect();
  m_posEffectManager->RemoveAllPosEffect();

  for(i=0; i<GFL_NELEMS(m_srcParty); ++i){
    m_srcParty[i] = NULL;
  }

  storeAllActivePokeParam();
}

void POKECON::Clear( void )
{
  storeAllActivePokeParam();

  for(u32 i=0; i<GFL_NELEMS(m_party); ++i){
    m_party[i].Initialize();
  }

  m_fieldStatus.Init( m_fForServer );
  m_sideEffectManager->RemoveAllSideEffect();
  m_posEffectManager->RemoveAllPosEffect();
}

/**
 * @brief 全ての使用中のポケモンパラメータを、待機用配列に格納する
 */
void POKECON::storeAllActivePokeParam( void )
{
  for( u32 i=0; i<GFL_NELEMS(m_activePokeParam); ++i )
  {
    if( m_activePokeParam[i] == NULL )
    {
      continue;
    }

    GFL_ASSERT_STOP( m_storedPokeParam[i] == NULL );

    m_storedPokeParam[i] = m_activePokeParam[i];
    m_activePokeParam[i] = NULL;
  }
}

//=============================================================================================
/**
 * @brief パーティデータを登録
 * 
 * @param clientID      追加対象クライアントのID
 * @param srcParty      追加するパーティデータ
 * @param partyParam    追加パーティの付加パラメータ
 * @param playerData    プレイヤーデータ( NPCなら NULL )
 */
//=============================================================================================
void POKECON::SetParty( 
  u8                        clientID, 
  PokeParty*                srcParty,
  const PartyParam&         partyParam,
  const Savedata::MyStatus* playerData )
{
  m_srcParty[ clientID ] = srcParty;

  const u32 memberCount = srcParty->GetMemberCount();
  for( u32 i=0; i<memberCount; ++i )
  {
    const u8                     pokeID             = PokeID::GetClientPokeId( static_cast<BTL_CLIENT_ID>( clientID ), i );
    pml::pokepara::PokemonParam* srcParam           = srcParty->GetMemberPointer( i );
    const DefaultPowerUpDesc*    defaultPowerUpDesc = &( partyParam.defaultPowerUpDesc[ i ] );
    this->addPokeParam( clientID, pokeID, srcParam, *defaultPowerUpDesc, playerData );
  }

  this->updateIllusionTarget( clientID );

  m_party[ clientID ].MoveAlivePokeToFirst();
}

/**
 * @brief ポケモンパラメータを追加する
 * @param clientID            追加対象クライアントのID
 * @param pokeID              追加ポケモンのID
 * @param srcParam            追加ポケモンの元となるパラメータ
 * @param defaultPowerUpDesc  追加ポケモンのデフォルトパワーアップ設定
 * @param playerData          プレイヤーデータ( NPCなら NULL )
 */
void POKECON::addPokeParam( 
  u8                           clientID, 
  u8                           pokeID, 
  pml::pokepara::PokemonParam* srcParam, 
  const DefaultPowerUpDesc&    defaultPowerUpDesc,
  const Savedata::MyStatus*    playerData )
{
  GFL_ASSERT_STOP( m_storedPokeParam[ pokeID ] != NULL );
  GFL_ASSERT_STOP( m_activePokeParam[ pokeID ] == NULL );
  m_activePokeParam[ pokeID ] = m_storedPokeParam[ pokeID ];
  m_storedPokeParam[ pokeID ] = NULL;

  this->setupPokeParam( m_activePokeParam[ pokeID ], pokeID, srcParam, defaultPowerUpDesc, playerData );

  m_party[ clientID ].AddMember( m_activePokeParam[ pokeID ] );
}

/**
 * @brief ポケモンパラメータをセットアップする
 * @param[out] pokeParam           セットアップ対象のパラメータ
 * @param      pokeID              ポケモンのID
 * @param      srcParam            元となるパラメータ
 * @param      defaultPowerUpDesc  ポケモンのデフォルトパワーアップ設定
 * @param      playerData          プレイヤーデータ( NPCなら NULL )
 */
void POKECON::setupPokeParam( 
  BTL_POKEPARAM*               pokeParam,
  u8                           pokeID, 
  pml::pokepara::PokemonParam* srcParam, 
  const DefaultPowerUpDesc&    defaultPowerUpDesc, 
  const Savedata::MyStatus*    playerData )
{
  u8 friendship = 0;

  if( playerData != NULL )
  {
    friendship = PokeTool::nakayoshi::GetFriendship( srcParam, playerData );
  }

  BTL_POKEPARAM::SetupParam setupParam;
  setupParam.srcParam    = srcParam;
  setupParam.pokeID      = pokeID;
  setupParam.friendship  = friendship;
  setupParam.fieldStatus = &m_fieldStatus;
  DEFAULT_POWERUP_DESC_Copy( &setupParam.defaultPowerUpDesc, defaultPowerUpDesc );

  pokeParam->Setup( setupParam );
}

/**
 * @brief 特性「イリュージョン」の対象ポケモンを更新する
 * @param clientID  更新対象クライアントのID
 *
 * 最後以外のポケモンがイリュージョン使いなら、最後のSrcPPを見せかけデータにする
 */
void POKECON::updateIllusionTarget( u8 clientID )
{
  BTL_PARTY* party = &m_party[ clientID ];

  int lastPokeIndex = party->GetIllusionTargetMemberIndex();
  if( lastPokeIndex > 0 )
  {
    for( int i=0; i<lastPokeIndex; ++i )
    {
      u8 pokeID = PokeID::GetClientPokeId( static_cast<BTL_CLIENT_ID>( clientID ), i );
      if( m_activePokeParam[pokeID]->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_IRYUUZYON )
      {
        const BTL_POKEPARAM*               illusionTargetPoke    = party->GetMemberDataConst( lastPokeIndex );
        const u8                           illusionTargetPokeId  = illusionTargetPoke->GetID();
        const pml::pokepara::PokemonParam* illusionTargetPokeSrc = illusionTargetPoke->GetSrcData(); 
        m_activePokeParam[ pokeID ]->SetViewSrcData( illusionTargetPokeId, illusionTargetPokeSrc );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモンを追加する
 * @param clientID            追加対象クライアントのID
 * @param pokeID              追加ポケモンのID
 * @param srcParam            追加ポケモンの元となるパラメータ
 * @param defaultPowerUpDesc  追加ポケモンのデフォルトパワーアップ設定
 * @param playerData          プレイヤーデータ( NPCなら NULL )
 * @retval true   追加できた
 * @retval false  追加できなかった
 */
//-----------------------------------------------------------------------------
bool POKECON::AddPokemon( 
  u8                                 clientID, 
  u8                                 pokeID,
  const pml::pokepara::PokemonParam& srcParam, 
  const DefaultPowerUpDesc&          defaultPowerUpDesc, 
  const Savedata::MyStatus*          playerData )
{
  PokeParty* srcParty    = this->GetSrcParty( clientID );
  BTL_PARTY* battleParty = this->GetPartyData( clientID );

  // これ以上追加できない
  if( ( srcParty->IsFull() ) ||
      ( battleParty->IsFull() ) ) 
  {
    GFL_ASSERT(0);
    return false;
  }

  // 既に存在する
  if( this->IsExistPokemon( pokeID ) )
  {
    GFL_ASSERT(0);
    return false;
  }

  const u8 addMemberIndex = srcParty->GetMemberCount();
  srcParty->AddMember( srcParam );                                                           // srcParam がコピーされる
  pml::pokepara::PokemonParam* srcParamClone = srcParty->GetMemberPointer( addMemberIndex ); // srcParam のコピーを取得
  this->addPokeParam( clientID, pokeID, srcParamClone, defaultPowerUpDesc, playerData );

  this->updateIllusionTarget( clientID );

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 既存のポケモンを新しいパラメータで置き換える
 * @param clientID            追加対象クライアントのID
 * @param pokeID              追加ポケモンのID
 * @param srcParam            追加ポケモンの元となるパラメータ
 * @param defaultPowerUpDesc  追加ポケモンのデフォルトパワーアップ設定
 * @param playerData          プレイヤーデータ( NPCなら NULL )
 * @retval true   置換できた
 * @retval false  置換できなかった
 */
//-----------------------------------------------------------------------------
bool POKECON::ReplacePokemon(
  u8                                 clientID,
  u8                                 pokeID, 
  const pml::pokepara::PokemonParam& srcParam, 
  const DefaultPowerUpDesc&          defaultPowerUpDesc,
  const Savedata::MyStatus*          playerData )
{
  // 置換対象のポケモンが存在しない
  if( !( this->IsExistPokemon( pokeID ) ) )
  {
    GFL_ASSERT(0);
    return false;
  }

  PokeParty*                         srcParty           = this->GetSrcParty( clientID );
  const BTL_POKEPARAM*               replacePokeParam   = this->GetPokeParamConst( pokeID );
  const pml::pokepara::PokemonParam* replaceSrcParam    = replacePokeParam->GetSrcData();
  const u8                           replaceMemberIndex = srcParty->GetMemberIndex( replaceSrcParam );

  // 置換対象の元パラメータが存在しない
  if( PokeParty::MAX_MEMBERS <= replaceMemberIndex )
  {
    GFL_ASSERT(0);
    return false;
  }

  srcParty->ReplaceMember( replaceMemberIndex, srcParam );                                       // srcParam がコピーされる
  pml::pokepara::PokemonParam* srcParamClone = srcParty->GetMemberPointer( replaceMemberIndex ); // srcParam のコピーを取得
  this->replacePokeParam( clientID, pokeID, srcParamClone, defaultPowerUpDesc, playerData );

  this->updateIllusionTarget( clientID );

  return true;
}

/**
 * @brief 既存のポケモンパラメータを置換する
 * @param clientID            追加対象クライアントのID
 * @param pokeID              追加ポケモンのID
 * @param srcParam            追加ポケモンの元となるパラメータ
 * @param defaultPowerUpDesc  追加ポケモンのデフォルトパワーアップ設定
 * @param playerData          プレイヤーデータ( NPCなら NULL )
 */
void POKECON::replacePokeParam( 
  u8                           clientID, 
  u8                           pokeID, 
  pml::pokepara::PokemonParam* srcParam, 
  const DefaultPowerUpDesc&    defaultPowerUpDesc,
  const Savedata::MyStatus*    playerData )
{
  GFL_ASSERT_STOP( m_storedPokeParam[ pokeID ] == NULL );
  GFL_ASSERT_STOP( m_activePokeParam[ pokeID ] != NULL );

  this->setupPokeParam( m_activePokeParam[ pokeID ], pokeID, srcParam, defaultPowerUpDesc, playerData );
}

//------------------------------------------------------------------------------
/**
 * 指定クライアントのパーティデータを返す
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  const PokeParty*
 */
//------------------------------------------------------------------------------
BTL_PARTY* POKECON::GetPartyData( u32 clientID )
{
  GFL_ASSERT(clientID < BTL_CLIENT_MAX);
  return &m_party[ clientID ];
}
const BTL_PARTY* POKECON::GetPartyDataConst( u32 clientID ) const
{
  GFL_ASSERT(clientID < BTL_CLIENT_MAX);
  return &m_party[ clientID ];
}



//=============================================================================================
/**
 * クライアントポケモンデータの取得 ( const )
 *
 * @param   wk
 * @param   clientID
 * @param   memberIdx
 *
 * @retval  const BTL_POKEPARAM*
 */
//=============================================================================================
const BTL_POKEPARAM* POKECON::GetClientPokeDataConst( u8 clientID, u8 memberIdx ) const
{
  const BTL_PARTY* party = &m_party[ clientID ];
  return party->GetMemberDataConst( memberIdx );
}
//=============================================================================================
/**
 * ポケモンデータの取得
 *
 * @param   pokeID
 *
 * @retval  BTL_POKEPARAM*
 */
//=============================================================================================
BTL_POKEPARAM*  POKECON::GetPokeParam( u8 pokeID )
{
  GFL_ASSERT(pokeID<BTL_POKEID_MAX);
  GFL_ASSERT_MSG(m_activePokeParam[pokeID], "invalid pokeID(%d)", pokeID);
  return m_activePokeParam[ pokeID ];
}
//=============================================================================================
/**
 * ポケモンデータの取得（ const ）
 *
 * @param   pokeID
 *
 * @retval  const BTL_POKEPARAM*
 */
//=============================================================================================
const BTL_POKEPARAM* POKECON::GetPokeParamConst( u8 pokeID ) const
{
  GFL_ASSERT(pokeID<BTL_POKEID_MAX);
  GFL_ASSERT_MSG(m_activePokeParam[pokeID], "pokeID=%d", pokeID);

  return m_activePokeParam[ pokeID ];
}
/**
 * 指定IDのポケモンが存在しているかどうか判定
 * @param pokeID
 * @return
 */
bool POKECON::IsExistPokemon( u8 pokeID ) const
{
  if( pokeID < BTL_POKEID_MAX )
  {
    return m_activePokeParam[pokeID] != NULL;
  }
  else{
    GFL_ASSERT(0);
    return false;
  }
}


// バトルパーティの内容をオリジナルパーティデータに反映させる
void POKECON::RefrectBtlParty( u8 clientID, bool fDefaultForm )
{
  PokeParty* srcParty = m_srcParty[ clientID ];
  BTL_PARTY* btlParty = GetPartyData( clientID );
  u32 memberCount = srcParty->GetMemberCount();

  pml::pokepara::PokemonParam *pp;
  BTL_POKEPARAM* bpp;
  u8  illusionTargetPokeId[ BTL_PARTY_MEMBER_MAX ];
  u32 i;

  s_tmpPokeParty->Clear();

  // 無効値でクリア
  for(i=0; i<GFL_NELEMS(illusionTargetPokeId); ++i){
    illusionTargetPokeId[ i ] = BTL_POKEID_NULL;
  }

  for(i=0; i<memberCount; ++i)
  {
    bpp = btlParty->GetMemberData( i );
    bpp->ReflectToPP( fDefaultForm );

    const pml::pokepara::PokemonParam* const_pp = bpp->GetSrcData();

    s_tmpPokeParty->AddMember( *const_pp );

    // イリュージョン中の場合、自分のイリュージョン対象ポケIndexを記録する
    if( bpp->IsFightEnable() )
    {
      illusionTargetPokeId[i] = bpp->GetFakeTargetPokeID();
    }
  }

  srcParty->CopyFrom( *s_tmpPokeParty );

  // SrcPP ポインタを再設定
  for(i=0; i<memberCount; ++i)
  {
    pp = srcParty->GetMemberPointer( i );
    bpp = btlParty->GetMemberData( i );
    bpp->SetSrcPP( pp );
  }

  // イリュージョンを再設定
  for(i=0; i<memberCount; ++i)
  {
    if( illusionTargetPokeId[i] != BTL_POKEID_NULL )
    {
      const u8                           illusionTargetPokeMemberIndex = btlParty->GetMemberIndex( illusionTargetPokeId[i] );
      const BTL_POKEPARAM*               illusionTargetPoke            = btlParty->GetMemberDataConst( illusionTargetPokeMemberIndex );
      const pml::pokepara::PokemonParam* illusionTargetPokeSrc         = illusionTargetPoke->GetSrcData();
      bpp = btlParty->GetMemberData( i );
      bpp->SetViewSrcData( illusionTargetPokeId[i], illusionTargetPokeSrc );
    }
  }
}
void POKECON::RefrectBtlPartyStartOrder( u8 clientID, u8 orgPokeID, u8* fightPokeIdx )
{
  PokeParty* srcParty = m_srcParty[ clientID ];
  BTL_PARTY* btlParty = GetPartyData( clientID );

  u32 memberCount = srcParty->GetMemberCount();
  const pml::pokepara::PokemonParam* pp;
  BTL_POKEPARAM* bpp;

  s_tmpPokeParty->Clear();

  for(u32 i=0; i<memberCount; ++i)
  {
    for(u32 j=0; j<memberCount; ++j)
    {
      bpp = btlParty->GetMemberData( j );
      if( bpp->GetID() == (orgPokeID+i) )
      {
        bpp->ReflectToPP( true );
        pp = bpp->GetSrcData();
        s_tmpPokeParty->AddMember( *pp );

        fightPokeIdx[ i ] = bpp->GetBtlInFlag();
        break;
      }
    }
  }

  srcParty->CopyFrom( *s_tmpPokeParty );
}
PokeParty* POKECON::GetSrcParty( u8 clientID )
{
  return m_srcParty[ clientID ];
}

//----------------------------------------------------------------------------------
/**
 * 指定ポケモンのパーティ内位置を返す
 *
 * @param   clientID
 * @param   pokeID
 *
 * @retval  int
 */
//----------------------------------------------------------------------------------
int POKECON::FindPokemon( u8 clientID, u8 pokeID ) const
{
  const BTL_POKEPARAM* bpp;
  const BTL_PARTY* party = &m_party[ clientID ];

  u32 max = party->GetMemberCount();

  for(u32 i=0; i<max; ++i)
  {
    bpp = party->GetMemberDataConst( i );
    if( bpp->GetID() == pokeID )
    {
      return static_cast<int>( i );
    }
  }
  return -1;
}


//=============================================================================================
/**
 * 戦闘位置からポケモンデータの取得
 *
 * @param   wk    メインモジュールのハンドラ
 * @param   pos   立ち位置
 *
 * @retval  BTL_POKEPARAM*
 */
//=============================================================================================
BTL_POKEPARAM* POKECON::GetFrontPokeData( BtlPokePos pos )
{
  return const_cast<BTL_POKEPARAM*>( GetFrontPokeDataConst(pos) );
}
//=============================================================================================
/**
 * 戦闘位置からポケモンデータの取得（const）
 *
 * @param   wk    メインモジュールのハンドラ
 * @param   pos   立ち位置
 *
 * @retval  const BTL_POKEPARAM*（パーティのメンバーが足りない場合はNULL）
 */
//=============================================================================================
const BTL_POKEPARAM* POKECON::GetFrontPokeDataConst( BtlPokePos pos ) const
{
  const BTL_PARTY* party;
  u8 clientID, posIdx, memberCount;

  m_mainModule->BtlPosToClientID_and_PosIdx( pos, &clientID, &posIdx );
  if( clientID == BTL_CLIENT_NULL )
  {
    return NULL;
  }

  party = &m_party[ clientID ];
  memberCount = party->GetMemberCount();

  if( posIdx < memberCount ){
//    BTL_N_PrintfSimple( DBGSTR_MAIN_GetFrontPokeDataResult, clientID, posIdx );
    return party->GetMemberDataConst( posIdx );
  }else{
    BTL_N_PrintfSimple( DBGSTR_MAIN_PokeConGetByPos, pos, clientID, posIdx );
    return NULL;
  }
}
//=============================================================================================
/**
 * クライアントIDからポケモンデータの取得
 *
 * @param   wk
 * @param   clientID
 * @param   posIdx
 *
 * @retval  const BTL_POKEPARAM*
 */
//=============================================================================================
BTL_POKEPARAM* POKECON::GetClientPokeData( u8 clientID, u8 posIdx )
{
  BTL_PARTY* party;
  party = &m_party[ clientID ];
  return party->GetMemberData( posIdx );
}
#if 0
//=============================================================================================
/**
 * クライアント指定で戦えるポケモン数を取得
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKECON_GetClientAlivePokeCount( const BTL_POKE_CONTAINER* pokeCon, u8 clientID )
{
  const BTL_PARTY* party = BTL_POKECON_GetPartyDataConst( pokeCon, clientID );
  return BTL_PARTY_GetAliveMemberCount( party );
}

#endif

//=============================================================================================
/**
 * フィールド状態管理クラスのポインタを取得
 *
 * @retval  FieldStatus*
 */
//=============================================================================================
FieldStatus*   POKECON::GetFieldStatus( void )
{
  return &m_fieldStatus;
}
const FieldStatus*   POKECON::GetFieldStatusConst( void ) const
{
  return &m_fieldStatus;
}



//-------------------------------------------------------------------------
/**
 * @brief サイドエフェクトの状態を取得する
 * @param side    取得したい陣営
 * @param effect  取得したいサイドエフェクト
 * @retval NULL  指定したサイドエフェクトが存在しない場合
 */
//-------------------------------------------------------------------------
SideEffectStatus* POKECON::GetSideEffectStatus( BtlSide side, BtlSideEffect effect )
{
  return m_sideEffectManager->GetSideEffectStatus( side, effect );
}

//-------------------------------------------------------------------------
/**
 * @brief サイドエフェクトの状態を取得する
 * @param side    取得したい陣営
 * @param effect  取得したいサイドエフェクト
 * @retval NULL  指定したサイドエフェクトが存在しない場合
 */
//-------------------------------------------------------------------------
const SideEffectStatus* POKECON::GetSideEffectStatusConst( BtlSide side, BtlSideEffect effect ) const
{
  return m_sideEffectManager->GetSideEffectStatusConst( side, effect );
}

//-------------------------------------------------------------------------
/**
 * @brief 位置エフェクトの状態を取得する
 * @param pos        取得したい位置
 * @param posEffect  取得したい位置エフェクト
 * @retval NULL  指定した位置エフェクトが存在しない場合
 */
//-------------------------------------------------------------------------
PosEffectStatus* POKECON::GetPosEffectStatus( BtlPokePos pos, BtlPosEffect posEffect )
{
  return m_posEffectManager->GetPosEffectStatus( pos, posEffect );
}

//-------------------------------------------------------------------------
/**
 * @brief 位置エフェクトの状態を取得する
 * @param pos        取得したい位置
 * @param posEffect  取得したい位置エフェクト
 * @retval NULL  指定した位置エフェクトが存在しない場合
 */
//-------------------------------------------------------------------------
const PosEffectStatus* POKECON::GetPosEffectStatusConst( BtlPokePos pos, BtlPosEffect posEffect ) const
{
  return m_posEffectManager->GetPosEffectStatusConst( pos, posEffect );
}

//-------------------------------------------------------------------------
/**
 * @brief 乱入により新たに追加されたポケモンIDを記録しておく（ServerFlowで対象ポケモンの追加Indexを調べるために使う）
 * @param[in]  pokeID   新たに追加されたポケモンのID
 */
//-------------------------------------------------------------------------
void POKECON::IntrudePokeID_Set( u8 pokeID )
{
  m_latestIntrudePokeID = pokeID;
}

//-------------------------------------------------------------------------
/**
 * @brief 乱入により新たに追加されたポケモンIDの記録を消去する（記録領域は１件分なのでClient, Server 双方不要になったタイミングで消去する）
 */
//-------------------------------------------------------------------------
void POKECON::IntrudePokeID_Clear( void )
{
  m_latestIntrudePokeID = BTL_POKEID_NULL;
}

//-------------------------------------------------------------------------
/**
 * @brief 乱入により新たに追加されたポケモンIDを取得する
 * @return  乱入により新たに追加されたポケモンID
 */
//-------------------------------------------------------------------------
u8 POKECON::IntrudePokeID_Get( void ) const
{
  return m_latestIntrudePokeID;
}



GFL_NAMESPACE_END(btl)




