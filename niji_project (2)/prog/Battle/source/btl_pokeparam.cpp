//=============================================================================================
/**
 * @file  btl_pokeparam.cpp
 * @brief ポケモンXY バトルシステム  参加ポケモン戦闘用データ
 * @author  taya
 *
 * @date  2008.10.08  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "pml/include/item/item.h"

#include "./btl_calc.h"
#include "./btl_sick.h"
#include "./btl_pokeparam.h"
#include "./btl_Capture.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/

//-----------------------------------------------------------------------
/*
 *  constructor / destructor
 */
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------
BTL_POKEPARAM::BTL_POKEPARAM( void )
{
}

//-----------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------
BTL_POKEPARAM::~BTL_POKEPARAM()
{

}

//-----------------------------------------------------------------------------
/**
 * @brief セットアップ
 * @param setupParam  セットアップに必要なパラメータ
 */
//-----------------------------------------------------------------------------
void BTL_POKEPARAM::Setup( const SetupParam& setupParam )
{
  bool bFastMode = setupParam.srcParam->StartFastMode();

#if PM_DEBUG
  m_pokeID_forDebugger = setupParam.pokeID;
#endif
  m_coreParam.myID   = setupParam.pokeID;
  m_coreParam.monsno = setupParam.srcParam->GetMonsNo();
  m_coreParam.ppSrc  = setupParam.srcParam;
  m_coreParam.hp     = setupParam.srcParam->GetHp();
  m_coreParam.hpMax  = setupParam.srcParam->GetPower( pml::pokepara::POWER_HP );
  m_coreParam.item   = setupParam.srcParam->GetItem();

  // 未来バージョン対策で想定外のアイテムナンバーは無効として扱う
  if( m_coreParam.item > ITEM_DATA_MAX )
  {
    m_coreParam.item = ITEM_DUMMY_DATA;
  }

  m_coreParam.usedItem              = ITEM_DUMMY_DATA;
  m_coreParam.fHensin               = false;
  m_coreParam.ppFake                = NULL;
  m_coreParam.fFakeEnable           = NULL;
  m_coreParam.totalTurnCount        = 0;
  m_coreParam.fakeViewTargetPokeId  = BTL_POKEID_NULL;
  m_coreParam.fBtlIn                = NULL;
  m_coreParam.defaultFormNo         = setupParam.srcParam->GetFormNo();
  m_coreParam.defaultTokusei        = setupParam.srcParam->GetTokuseiNo();
  m_coreParam.level                 = setupParam.srcParam->GetLevel();
  m_coreParam.mons_pow              = calc::PERSONAL_GetParam( m_coreParam.monsno, m_coreParam.defaultFormNo, pml::personal::PARAM_ID_basic_atk );
  m_coreParam.mons_agility          = calc::PERSONAL_GetParam( m_coreParam.monsno, m_coreParam.defaultFormNo, pml::personal::PARAM_ID_basic_agi );
  m_coreParam.dirtType              = pml::pokepara::DIRT_TYPE_NONE;
  m_coreParam.killCount             = 0;
  m_coreParam.fDontResetFormByByOut = false;
  this->ClearDeadCause();

  DEFAULT_POWERUP_DESC_Copy( &m_coreParam.defaultPowerUpDesc, setupParam.defaultPowerUpDesc );

  doryoku_InitParam( &m_doryokuParam, setupParam.srcParam );

  setupBySrcData( setupParam.srcParam, true, true );

  // 所有ワザデータ初期化
  m_wazaCnt = wazaWork_setupByPP( setupParam.srcParam, true );
  m_usedWazaCount = 0;

  // ランク効果初期化
  effrank_Init( &m_varyParam );

  // 状態異常ワーク初期化
  clearWazaSickWork( SICKWORK_CLEAR_ALL );

  // この時点でポケモン用状態異常になっていれば引き継ぎ
  {
    PokeSick sick = setupParam.srcParam->GetSick();
    if( sick != pml::pokepara::SICK_NULL )
    {
      m_coreParam.sickCont[ sick ] = btl::calc::MakeDefaultPokeSickCont( sick, this->GetID() );
      m_coreParam.wazaSickCounter[ sick ] = 0;
    }
  }

  // 各種ワーク領域初期化
  m_appearedTurn          = TURNCOUNT_NULL;
  m_turnCount             = 0;
  m_migawariHP            = 0;
  m_prevTargetPos         = BTL_POS_1ST_0; // ゼロでクリアする
  m_prevSelectWazaID      = WAZANO_NULL;
  m_prevActWazaID         = WAZANO_NULL;
  m_isPrevActWazaZenryoku = false;
  m_prevWazaType          = POKETYPE_NULL;
  m_wazaContCounter       = 0;
  m_combiWazaID           = WAZANO_NULL;
  m_combiPokeID           = BTL_POKEID_NULL;
  m_criticalRank          = 0;
  m_friendship            = setupParam.friendship;
  m_fldSim                = setupParam.fieldStatus;
  resetSpActPriority();

  flgbuf_clear( m_turnFlag, sizeof(m_turnFlag) );
  flgbuf_clear( m_contFlag, sizeof(m_contFlag) );
  flgbuf_clear( m_permFlag, sizeof(m_permFlag) );

  gfl2::std::MemClear( m_counter, sizeof(m_counter) );

  dmgrec_ClearWork();
  confrontRec_Clear();

  setupParam.srcParam->EndFastMode( bFastMode );
}


//----------------------------------------------------------------------------------
/**
 * 元になるポケモンパラメータ構造体から必要部分を構築
 *
 * @param   bpp
 * @param   srcPP
 * @param   fReflectHP
 * @param   fTokuseiUpdate
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::setupBySrcData( const pml::pokepara::PokemonParam* srcPP, bool fReflectHP, bool fTokuseiUpdate )
{
  // 2010.4.15 HP反映（レベルアップ＆死亡でHPが残ってしまう問題に対処）
  if( fReflectHP ){
    m_coreParam.hp = srcPP->GetHp();
    m_coreParam.hpMax = srcPP->GetPower( pml::pokepara::POWER_HP );
  }

  m_coreParam.exp = srcPP->GetExp();

  // 基本パラメタ初期化
  setupBySrcDataBase( srcPP, fTokuseiUpdate );

  // レベルアップ時の処理では、とくせいを戻さない（スキルスワップなどで書き換わるのに対処）
  if( fTokuseiUpdate ){
    m_tokusei = srcPP->GetTokuseiNo();
  }

  m_formNo = srcPP->GetFormNo();

  updateWeight();
}

/**
 * @brief 自身の重さを、現在のフォルムに応じた値に更新する
 */
void BTL_POKEPARAM::updateWeight( void )
{
  m_weight = calc::PERSONAL_GetParam( m_coreParam.monsno, m_formNo, pml::personal::PARAM_ID_weight );

  if( m_weight < BTL_POKE_WEIGHT_MIN ){
    m_weight = BTL_POKE_WEIGHT_MIN;
  }
}

//----------------------------------------------------------------------------------
/**
 * 元になるポケモンパラメータ構造体からバトルパラメータ部分のみ構築
 *
 * @param   bpp
 * @param   srcPP
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::setupBySrcDataBase( const pml::pokepara::PokemonParam* srcPP, bool fTypeUpdate )
{
  if( fTypeUpdate ){
    m_baseParam.type1         = srcPP->GetType1();
    m_baseParam.type2         = srcPP->GetType2();
    m_baseParam.type_ex       = POKETYPE_NULL;
    m_baseParam.type_ex_cause = EXTYPE_CAUSE_NONE;
  }
  m_baseParam.sex        = srcPP->GetSex();
  m_baseParam.attack     = srcPP->GetPower( pml::pokepara::POWER_ATK );
  m_baseParam.defence    = srcPP->GetPower( pml::pokepara::POWER_DEF );
  m_baseParam.sp_attack  = srcPP->GetPower( pml::pokepara::POWER_SPATK );
  m_baseParam.sp_defence = srcPP->GetPower( pml::pokepara::POWER_SPDEF );
  m_baseParam.agility    = srcPP->GetPower( pml::pokepara::POWER_AGI );
  
  //@バトル変更箇所 へんしん時にフーパを取るため追加。変数自体は前からあった。
  //他で参照もされていないので、使わせてもらう。
  m_baseParam.monsno     = srcPP->GetMonsNo();
}


/**
 *  srcPPを元にワザデータを設定
 */
u32 BTL_POKEPARAM::wazaWork_setupByPP( const pml::pokepara::PokemonParam* pp_src, bool fLinkSurface )
{
  pml::pokepara::PokemonParam* pp = const_cast<pml::pokepara::PokemonParam*>(pp_src);

  bool fastFlag = pp->StartFastMode();
  u32 i, cnt = 0;

  if( fLinkSurface )
  {
    for(i=0; i<PTL_WAZA_MAX; i++)
    {
      m_waza[i].truth.usedFlag = false;
      m_waza[i].truth.usedFlagFix = false;
    }
  }

  for(i=0; i<PTL_WAZA_MAX; i++)
  {
    if( wazaCore_SetupByPP( &m_waza[i].truth, pp, i ) ){
      ++cnt;
    }
  }

  if( fLinkSurface )
  {
    for(i=0; i<PTL_WAZA_MAX; ++i){
      m_waza[i].surface = m_waza[i].truth;
      m_waza[i].fLinked = true;
    }
  }

  pp->EndFastMode( fastFlag );

  return cnt;
}
/**
 *  srcPPにワザデータを反映させる
 */
void BTL_POKEPARAM::wazaWork_ReflectToPP( void )
{
  pml::pokepara::PokemonParam* pp = const_cast<pml::pokepara::PokemonParam*>(m_coreParam.ppSrc);

  for(u32 i=0; i<PTL_WAZA_MAX; ++i)
  {
    pp->SetWaza( i, m_waza[i].truth.number );
    pp->SetWazaPPUpCount( i, m_waza[i].truth.ppCnt );
    pp->SetWazaPP( i, m_waza[i].truth.pp );
  }
}
/**
 *  srcPP のワザデータを使って再設定（レベルアップ用）
 */
void BTL_POKEPARAM::wazaWork_ReflectFromPP( void )
{
  pml::pokepara::PokemonParam* pp = const_cast<pml::pokepara::PokemonParam*>(m_coreParam.ppSrc);

  m_wazaCnt = 0;

  for(u32 i=0; i<PTL_WAZA_MAX; ++i)
  {
    if( wazaCore_SetupByPP( &(m_waza[i].truth), pp, i) ){
      m_wazaCnt++;
    }
    if( m_waza[i].fLinked ){
      m_waza[i].surface = m_waza[i].truth;
    }
  }
}
void BTL_POKEPARAM::wazaWork_ClearSurface( void )
{
  m_wazaCnt = 0;
  for(u32 i=0; i<PTL_WAZA_MAX; ++i)
  {
    m_waza[i].surface = m_waza[i].truth;
    if( m_waza[i].surface.number != WAZANO_NULL ){
      m_wazaCnt++;
    }
    m_waza[i].fLinked = true;
  }
}

void BTL_POKEPARAM::wazaSet_ClearUsedFlag( WAZA_SET* waza )
{
  waza->surface.usedFlag = false;
  waza->truth.usedFlag = false;
}

bool BTL_POKEPARAM::wazaCore_SetupByPP( WAZA_CORE* core, pml::pokepara::PokemonParam* pp, u8 index )
{
  bool fExist = true;
  WazaID next_number;

  next_number = pp->GetWazaNo( index );

  if( core->number != next_number )
  {
    core->usedFlag = false;
    core->usedFlagFix = false;
  }

  core->number = next_number;
  if( core->number != WAZANO_NULL )
  {
    core->pp    = pp->GetWazaPP( index );
    core->ppMax = pp->GetWazaMaxPP( index );
    core->ppCnt = pp->GetWazaPPUpCount( index );
  }
  else{
    core->pp = 0;
    core->ppMax = 0;
    core->ppCnt = 0;
    fExist = false;
  }


  return fExist;
}


//-----------------------------------------------------------------------------
/**
 * @brief コピーする
 * @param srcParam  コピー元のパラメータ
 */
//-----------------------------------------------------------------------------
void BTL_POKEPARAM::CopyFrom( const BTL_POKEPARAM& srcParam )
{
  henshinCopyFrom( srcParam );
  CORE_PARAM_Copy( &m_coreParam, srcParam.m_coreParam );
}

/**
 * @brief コアパラメータをコピーする
 * @param[out] dest  コピー先のパラメータ
 * @param      src   コピー元のパラメータ
 */
void BTL_POKEPARAM::CORE_PARAM_Copy( CORE_PARAM* dest, const CORE_PARAM& src ) const
{
  dest->ppSrc->CopyFrom( *src.ppSrc );

  dest->ppFake         = src.ppFake;
  dest->exp            = src.exp;
  dest->monsno         = src.monsno;
  dest->hpMax          = src.hpMax;
  dest->hp             = src.hp;
  dest->item           = src.item;
  dest->usedItem       = src.usedItem;
  dest->defaultTokusei = src.defaultTokusei;
  dest->level          = src.level;
  dest->myID           = src.myID;
  dest->mons_pow       = src.mons_pow;
  dest->mons_agility   = src.mons_agility;
  dest->defaultFormNo  = src.defaultFormNo ;
  dest->fHensin        = src.fHensin;
  dest->fFakeEnable    = src.fFakeEnable;
  dest->fBtlIn         = src.fBtlIn;
  dest->dirtType       = src.dirtType;
  dest->deadCause      = src.deadCause;
  dest->deadCausePokeID = src.deadCausePokeID;
  dest->killCount       = src.killCount;

  dest->confrontRecCount     = src.confrontRecCount;
  dest->totalTurnCount       = src.totalTurnCount;
  dest->fakeViewTargetPokeId = src.fakeViewTargetPokeId;
  
  gfl2::std::MemCopy( src.sickCont,        dest->sickCont,        sizeof(dest->sickCont) );
  gfl2::std::MemCopy( src.wazaSickCounter, dest->wazaSickCounter, sizeof(dest->wazaSickCounter) );
  gfl2::std::MemCopy( src.confrontRec,     dest->confrontRec,     sizeof(dest->confrontRec) );

  DEFAULT_POWERUP_DESC_Copy( &(dest->defaultPowerUpDesc), src.defaultPowerUpDesc );
}



//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
//=============================================================================================
/**
 * ポケモンID（バトル参加全ポケモンに対し一意な値）を取得
 *
 * @retval  u8
 */
//=============================================================================================
u8  BTL_POKEPARAM::GetID( void ) const
{
  return m_coreParam.myID;
}
//=============================================================================================
/**
 * モンスターナンバーを返す
 *
 * @retval  u16
 */
//=============================================================================================
u16 BTL_POKEPARAM::GetMonsNo( void ) const
{
  return m_coreParam.monsno;
}

//=============================================================================================
/**
 * @brief モンスターナンバーを返す
 *       「へんしん」後の場合、変身後のモンスターナンバーを返します。
 *
 * @バトル変更箇所 へんしん時にフーパを取るため追加。
 */
//=============================================================================================
u16  BTL_POKEPARAM::GetHenshinMonsNo( void ) const
{
  if( HENSIN_Check() )
  {
    return m_baseParam.monsno;
  }
  return m_coreParam.monsno;
}

//=============================================================================================
/**
 * @brief デフォルト能力アップの設定を取得する
 */
//=============================================================================================
const DefaultPowerUpDesc& BTL_POKEPARAM::GetDefaultPowerUpDesc( void ) const
{
  return m_coreParam.defaultPowerUpDesc;
}

//=============================================================================================
/**
 * @brief 汚れの種類を取得する
 */
//=============================================================================================
DirtType BTL_POKEPARAM::GetDirtType( void ) const
{
  return m_coreParam.dirtType;
}

//=============================================================================================
/**
 * @brief 汚れをセットする
 * @param dirt  セットする汚れの種類
 */
//=============================================================================================
void BTL_POKEPARAM::SetDirtType( DirtType dirt )
{
  m_coreParam.dirtType = dirt;
}

//=============================================================================================
/**
 * @brief ひん死になった原因を取得する
 */
//=============================================================================================
DamageCause BTL_POKEPARAM::GetDeadCause( void ) const
{
  return m_coreParam.deadCause;
}

//=============================================================================================
/**
 * @brief ひん死の原因となったポケモンのIDを取得する
 */
//=============================================================================================
u8 BTL_POKEPARAM::GetDeadCausePokeID( void ) const
{
  return m_coreParam.deadCausePokeID;
}

//=============================================================================================
/**
 * @brief ひん死になった原因を設定する
 * @param damageCause        ひん死の原因
 * @param damageCausePokeID  ひん死の原因となったポケモンのID
 */
//=============================================================================================
void BTL_POKEPARAM::SetDeadCause( DamageCause damageCause, u8 damageCausePokeID )
{
  if( !this->IsDead() )
  {
    GFL_ASSERT(0);
    return;
  }
  m_coreParam.deadCause = damageCause;
  m_coreParam.deadCausePokeID = damageCausePokeID;
}

//=============================================================================================
/**
 * @brief ひん死の原因をクリアする
 */
//=============================================================================================
void BTL_POKEPARAM::ClearDeadCause( void )
{
  m_coreParam.deadCause = DAMAGE_CAUSE_OTHER;
  m_coreParam.deadCausePokeID = BTL_POKEID_NULL;
}

//=============================================================================================
/**
 * @brief 相手を倒した数を取得する
 */
//=============================================================================================
u8 BTL_POKEPARAM::GetKillCount( void ) const
{
  return m_coreParam.killCount;
}

//=============================================================================================
/**
 * @brief 相手を倒した数を設定する
 * @param killCount  設定する数
 */
//=============================================================================================
void BTL_POKEPARAM::SetKillCount( u8 killCount )
{
  m_coreParam.killCount = killCount;
}

//=============================================================================================
/**
 * @brief 相手を倒した数をインクリメントする
 */
//=============================================================================================
void BTL_POKEPARAM::IncKillCount( void )
{
  u8 killCount = this->GetKillCount();
  if( killCount < 0xff )
  {
    this->SetKillCount( killCount + 1 );
  }
}

//=============================================================================================
/**
 * @brief 行動の特殊優先度を取得する
 */
//=============================================================================================
u8 BTL_POKEPARAM::GetSpActPriority( void ) const
{
  return m_spActPriority;
}

//=============================================================================================
/**
 * @brief 行動の特殊優先度を設定する
 * @param priority  設定する特殊優先度
 */
//=============================================================================================
void BTL_POKEPARAM::SetSpActPriority( u8 priority )
{
  m_spActPriority = priority;
}

/**
 * @brief 行動の特殊優先度をデフォルトの状態にリセットする
 */
void BTL_POKEPARAM::resetSpActPriority( void )
{
  m_spActPriority = BTL_SPPRI_DEFAULT;
}

//=============================================================================================
/**
 * 元データポインタを取得
 *
 * @retval  const pml::pokepara::PokemonParam*
 */
//=============================================================================================
const pml::pokepara::PokemonParam* BTL_POKEPARAM::GetSrcData( void ) const
{
  return m_coreParam.ppSrc;
}
//=============================================================================================
/**
 * イリュージョン用の元データセット
 *
 * @param   fakeTargetPokeId  イリュージョン対象ポケモンのID
 * @param   fakePP
 */
//=============================================================================================
void BTL_POKEPARAM::SetViewSrcData( u8 fakeTargetPokeId, const pml::pokepara::PokemonParam* fakePP )
{
  m_coreParam.ppFake =  fakePP;
  m_coreParam.fFakeEnable = true;
  m_coreParam.fakeViewTargetPokeId = fakeTargetPokeId;
}
//=============================================================================================
/**
 * イリュージョン用の元データポインタ取得
 *
 * @retval  const pml::pokepara::PokemonParam*
 */
//=============================================================================================
const pml::pokepara::PokemonParam* BTL_POKEPARAM::GetViewSrcData( void ) const
{
  if( (m_coreParam.ppFake != NULL)
  &&  (m_coreParam.fFakeEnable)
  ){
    return m_coreParam.ppFake;
  }
  return m_coreParam.ppSrc;
}

//----------------------------------------------------------------------------------------------
// ステータスランク効果
//----------------------------------------------------------------------------------------------
void BTL_POKEPARAM::effrank_Init( VARIABLE_PARAM* rank )
{
  effrank_Reset( rank );
}
void BTL_POKEPARAM::effrank_Reset( VARIABLE_PARAM* rank )
{
  rank->attack = RANK_STATUS_DEFAULT;
  rank->defence = RANK_STATUS_DEFAULT;
  rank->sp_attack = RANK_STATUS_DEFAULT;
  rank->sp_defence = RANK_STATUS_DEFAULT;
  rank->agility = RANK_STATUS_DEFAULT;
  rank->hit = RANK_STATUS_DEFAULT;
  rank->avoid = RANK_STATUS_DEFAULT;
}
bool BTL_POKEPARAM::effrank_ResetRankUp( VARIABLE_PARAM* rank )
{
  bool fEffective = false;

  if( RANK_STATUS_DEFAULT < rank->attack )
  {
    rank->attack = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( RANK_STATUS_DEFAULT < rank->defence )
  {
    rank->defence = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( RANK_STATUS_DEFAULT < rank->sp_attack )
  {
    rank->sp_attack = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( RANK_STATUS_DEFAULT < rank->sp_defence )
  {
    rank->sp_defence = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( RANK_STATUS_DEFAULT < rank->agility )
  {
    rank->agility = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( RANK_STATUS_DEFAULT < rank->hit )
  {
    rank->hit = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( RANK_STATUS_DEFAULT < rank->avoid )
  {
    rank->avoid = RANK_STATUS_DEFAULT;
    fEffective = true;
  }

  return fEffective;
}
bool BTL_POKEPARAM::effrank_Recover( VARIABLE_PARAM* rank )
{
  bool fEffective = false;

  if( rank->attack < RANK_STATUS_DEFAULT ){
    rank->attack = RANK_STATUS_DEFAULT;
    fEffective = true;
  };
  if( rank->defence < RANK_STATUS_DEFAULT ){
    rank->defence = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( rank->sp_attack < RANK_STATUS_DEFAULT ){
    rank->sp_attack = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( rank->sp_defence < RANK_STATUS_DEFAULT ){
    rank->sp_defence = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( rank->agility < RANK_STATUS_DEFAULT ){
    rank->agility = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( rank->hit < RANK_STATUS_DEFAULT ){
    rank->hit = RANK_STATUS_DEFAULT;
    fEffective = true;
  }
  if( rank->avoid < RANK_STATUS_DEFAULT ){
    rank->avoid = RANK_STATUS_DEFAULT;
    fEffective = true;
  }

  return fEffective;
}


//----------------------------------------------------------------------------------------------
// ワザダメージレコード
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * ワザダメージレコード：ワーク初期化
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::dmgrec_ClearWork( void )
{
  gfl2::std::MemClear( m_wazaDamageRec, sizeof(m_wazaDamageRec) );
  gfl2::std::MemClear( m_dmgrecCount, sizeof(m_dmgrecCount) );
  m_dmgrecTurnPtr = 0;
  m_dmgrecPtr = 0;
}
//----------------------------------------------------------------------------------
/**
 * ワザダメージレコード：ターンチェック
 *
 * @param   pp
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::dmgrec_FwdTurn( void )
{
  if( ++(m_dmgrecTurnPtr) >= WAZADMG_REC_TURN_MAX ){
    m_dmgrecTurnPtr = 0;
  }
  m_dmgrecCount[ m_dmgrecTurnPtr ] = 0;
}

//---------------------------------------------------------------------------------------------
// 対面記録関連
//---------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * 初期化
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::confrontRec_Clear( void )
{
  m_coreParam.confrontRecCount = 0;
}

//=============================================================================================
/**
 * 対面記録セット
 *
 * @param   pokeID    対面したポケモンID
 */
//=============================================================================================
void BTL_POKEPARAM::Confront_Set( u8 pokeID )
{
  u32 i;
  for(i=0; i<m_coreParam.confrontRecCount; ++i)
  {
    if( m_coreParam.confrontRec[i] == pokeID ){
      return;
    }
  }
  if( i < GFL_NELEMS(m_coreParam.confrontRec) ){
//    BTL_Printf("ポケ[%d]が[%d]と対面した\n", m_coreParam.myID, pokeID);
    m_coreParam.confrontRec[i] = pokeID;
    m_coreParam.confrontRecCount++;
  }
}
//=============================================================================================
/**
 * 対面記録の件数取得
 *
 * @param   bpp
 *
 * @retval  u8    記録件数
 */
//=============================================================================================
u8 BTL_POKEPARAM::Confront_GetCount( void ) const
{
  return m_coreParam.confrontRecCount;
}
//=============================================================================================
/**
 * 対面記録されているポケモンIDを取得
 *
 * @param   idx   インデックス（0 <= idx < BPP_CONFRONT_REC_GetCount() )
 *
 * @retval  u8    ポケモンID
 */
//=============================================================================================
u8 BTL_POKEPARAM::Confront_GetPokeID( u8 idx ) const
{
  if( idx < m_coreParam.confrontRecCount ){
    return m_coreParam.confrontRec[ idx ];
  }
  return BTL_POKEID_NULL;
}


//---------------------------------------------------------------------------------------------
// パラメータ取得
//---------------------------------------------------------------------------------------------

//=============================================================================================
/**
 * 各種パラメータ取得
 *
 * @param   pp
 * @param   vid   必要なパラメータID
 *
 * @retval  int
 */
//=============================================================================================
int BTL_POKEPARAM::GetValue( ValueID vid ) const
{
  vid = convertValueID( vid );

  switch( vid ){
  case BPP_ATTACK:          return btl::calc::StatusRank( GetValue_Base(vid), m_varyParam.attack );
  case BPP_DEFENCE:         return btl::calc::StatusRank( GetValue_Base(vid), m_varyParam.defence );
  case BPP_SP_ATTACK:       return btl::calc::StatusRank( GetValue_Base(vid), m_varyParam.sp_attack );
  case BPP_SP_DEFENCE:      return btl::calc::StatusRank( GetValue_Base(vid), m_varyParam.sp_defence );
  case BPP_AGILITY:         return btl::calc::StatusRank( GetValue_Base(vid), m_varyParam.agility );

  case BPP_ATTACK_RANK:     return m_varyParam.attack;
  case BPP_DEFENCE_RANK:    return m_varyParam.defence;
  case BPP_SP_ATTACK_RANK:  return m_varyParam.sp_attack;
  case BPP_SP_DEFENCE_RANK: return m_varyParam.sp_defence;
  case BPP_AGILITY_RANK:    return m_varyParam.agility;

  case BPP_HIT_RATIO:       return m_varyParam.hit;
  case BPP_AVOID_RATIO:     return m_varyParam.avoid;

  case BPP_LEVEL:           return m_coreParam.level;
  case BPP_SEX:             return m_baseParam.sex;

  case BPP_HP:              return m_coreParam.hp;
  case BPP_MAX_HP:          return m_coreParam.hpMax;
  case BPP_MONS_POW:        return m_coreParam.mons_pow;
  case BPP_MONS_AGILITY:    return m_coreParam.mons_agility;

  case BPP_TOKUSEI_EFFECTIVE:
    if( CheckSick(pml::wazadata::WAZASICK_IEKI) ){
      return TOKUSEI_NULL;
    }
    /* fallthru */
  case BPP_TOKUSEI:         return m_tokusei;

  case BPP_EXP:             return m_coreParam.exp;

  case BPP_CALL_VALUE:
    {
      u16 monsno = this->GetMonsNo();
      u16 formno = this->GetFormNo();
      return calc::PERSONAL_GetParam( monsno, formno, pml::personal::PARAM_ID_sos_rate );
    }

  default:
    GFL_ASSERT(0);
    return 0;
  };
}
//=============================================================================================
/**
 * ランク補正フラットな状態のステータス取得
 *
 * @param   vid   必要なパラメータID
 *
 * @retval  int
 */
//=============================================================================================
int BTL_POKEPARAM::GetValue_Base( ValueID vid ) const
{
  vid = convertValueID( vid );

  switch( vid ){
  case BPP_ATTACK:            return m_baseParam.attack;
  case BPP_DEFENCE:           return m_baseParam.defence;
  case BPP_SP_ATTACK:         return m_baseParam.sp_attack;
  case BPP_SP_DEFENCE:        return m_baseParam.sp_defence;
  case BPP_AGILITY:           return m_baseParam.agility;

  case BPP_HIT_RATIO:         return RANK_STATUS_DEFAULT;
  case BPP_AVOID_RATIO:       return RANK_STATUS_DEFAULT;

  default:
    return GetValue( vid );
  };
}

//-----------------------------------------------------------------------------
/**
 * @brief 努力値を取得する
 * @param powerID  取得対象パラメータ
 */
//-----------------------------------------------------------------------------
u8 BTL_POKEPARAM::GetEffortValue( pml::pokepara::PowerID powerID ) const
{
  switch( powerID )
  {
  case pml::pokepara::POWER_ATK:    return m_doryokuParam.srcPow;
  case pml::pokepara::POWER_DEF:    return m_doryokuParam.srcDef;
  case pml::pokepara::POWER_SPATK:  return m_doryokuParam.srcSpPow;
  case pml::pokepara::POWER_SPDEF:  return m_doryokuParam.srcSpDef;
  case pml::pokepara::POWER_AGI:    return m_doryokuParam.srcAgi;
  }

  GFL_ASSERT(0);
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief 努力値の合計が最大か？
 * @retval true   努力値の合計が最大
 * @retval false  努力値の合計が最大でない
 */
//-----------------------------------------------------------------------------
bool BTL_POKEPARAM::IsEffortValueFull( void ) const
{
  return ( m_doryokuParam.srcSum == pml::MAX_TOTAL_EFFORT_POWER );
}

//----------------------------------------------------------------------------------
/**
 * 状態異常等の条件により、パラメータ取得用のIDを変更する
 * ※現状、ワンダールーム（全ポケモンの防御・特防を入れ替える）にのみ利用している。
 *
 * @param   ValueID    パラメータ取得用にリクエストされたパラメータID
 *
 * @retval  ValueID    実際に適用するパラメータID
 */
//----------------------------------------------------------------------------------
BTL_POKEPARAM::ValueID
BTL_POKEPARAM::convertValueID( BTL_POKEPARAM::ValueID vid ) const
{
  if( m_fldSim )
  {
    switch( vid ){
    case BPP_DEFENCE:
      if( m_fldSim->CheckEffect(FieldStatus::EFF_WONDERROOM) ){
        vid = BPP_SP_DEFENCE;
      }
      break;

    case BPP_SP_DEFENCE:
      if( m_fldSim->CheckEffect(FieldStatus::EFF_WONDERROOM) ){
        vid = BPP_DEFENCE;
      }
      break;
    }
  }
  return vid;
}


//---------------------------------------------------------------------------------------------
// HP状態チェック
//---------------------------------------------------------------------------------------------
//=============================================================================================
/**
 * HP満タンかチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::IsHPFull( void ) const
{
  return GetValue(BPP_HP) == GetValue(BPP_MAX_HP);
}
//=============================================================================================
/**
 * ひんし状態かチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::IsDead( void ) const
{
  return GetValue( BPP_HP ) == 0;
}
//=============================================================================================
/**
 * 戦える状態（ひん死でもタマゴでもない）かチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::IsFightEnable( void ) const
{
  if( m_coreParam.ppSrc->IsEgg(pml::pokepara::CHECK_BOTH_EGG) ){
    return false;
  }
  if( IsDead() ){
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------------------------
// 状態異常関連
//---------------------------------------------------------------------------------------------
//=============================================================================================
/**
 * 特定の状態異常にかかっているかチェック
 *
 * @param   sickType
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::CheckSick( WazaSick sickType ) const
{
  GFL_ASSERT(sickType < GFL_NELEMS(m_coreParam.sickCont));

  if( m_coreParam.sickCont[ sickType ].type != pml::wazadata::SICKCONT_NONE )
  {
    return true;
  }
  return false;
}

//=============================================================================================
/**
 * @brief ねむり状態かチェック
 *
 * @param checkMode  ねむりチェックモード
 *
 * @retval true   ねむり状態
 * @retval false  ねむり状態でない
 */
//=============================================================================================
bool BTL_POKEPARAM::CheckNemuri( NemuriCheckMode checkMode ) const
{
  if( checkMode == NEMURI_CHECK_INCLUDE_ZETTAINEMURI )
  {
    if( this->GetValue( BPP_TOKUSEI_EFFECTIVE ) == TOKUSEI_ZETTAINEMURI )
    {
      return true;
    }
  }

  return this->CheckSick( pml::wazadata::WAZASICK_NEMURI );
}

//=============================================================================================
/**
 * もうどく状態にかかっているかチェック
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::CheckMoudoku( void ) const
{
  WazaSick sickNo = pml::wazadata::WAZASICK_DOKU;

  if( m_coreParam.sickCont[ sickNo ].type != pml::wazadata::SICKCONT_NONE )
  {
    if( SICKCONT_IsMoudokuCont(m_coreParam.sickCont[sickNo]) ){
      return true;
    }
  }
  return false;
}

//=============================================================================================
/**
 * ワザロック状態なら、そのワザIDを返す
 *
 * @retval  WazaID  ワザロック状態の時、そのワザID／それ以外はWAZANO_NULL
 */
//=============================================================================================
WazaID BTL_POKEPARAM::GetWazaLockID( void ) const
{
  const BTL_SICKCONT* sick = &m_coreParam.sickCont[ pml::wazadata::WAZASICK_WAZALOCK ];

  switch( sick->type ){
  case pml::wazadata::SICKCONT_TURN:
    return (WazaID)(sick->turn.param);

  case pml::wazadata::SICKCONT_PERMANENT:
    return (WazaID)(sick->permanent.param);

  default:
    return WAZANO_NULL;
  }
}



//----------------------------------------------------------------------------------
/**
 * ワザ系状態異常のカウンタクリア
 *
 * @param   pp
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::clearWazaSickWork( uint32_t  clearCode )
{
  u32 i, start;
  u64 sleepRawCont, sleepCounter;

  start = ((clearCode & SICKWORK_CLEAR_ONLY_WAZASICK)==0)?   0 : pml::pokepara::SICK_MAX;

  sleepRawCont = m_coreParam.sickCont[ pml::wazadata::WAZASICK_NEMURI ].raw;
  sleepCounter = m_coreParam.wazaSickCounter[ pml::wazadata::WAZASICK_NEMURI ];

  for(i=start; i<GFL_NELEMS(m_coreParam.sickCont); ++i)
  {
    m_coreParam.sickCont[i].raw = 0;
    m_coreParam.sickCont[i].type = pml::wazadata::SICKCONT_NONE;
  }
  gfl2::std::MemClear( m_coreParam.wazaSickCounter, sizeof(m_coreParam.wazaSickCounter) );

  if( clearCode & SICKWORK_CLEAR_WITHOUT_SLEEP )
  {
    m_coreParam.sickCont[ pml::wazadata::WAZASICK_NEMURI ].raw    = sleepRawCont;
    m_coreParam.wazaSickCounter[ pml::wazadata::WAZASICK_NEMURI ] = sleepCounter;
  }
}

//=============================================================================================
/**
 * ポケモン状態異常にかかっているかチェック
 *
 * @retval  PokeSick    かかっている状態異常の識別子（かかっていない場合 pml::pokepara::SICK_NULL）
 */
//=============================================================================================
PokeSick  BTL_POKEPARAM::GetPokeSick( void ) const
{
  for(u32 i=POKESICK_ORIGIN; i<POKESICK_MAX; ++i)
  {
    if( m_coreParam.sickCont[i].type != pml::wazadata::SICKCONT_NONE ){
      return static_cast<PokeSick>( i );
    }
  }
  return pml::pokepara::SICK_NULL;
}
//=============================================================================================
/**
 * 特定状態異常に設定されたパラメータ値（8bit）を取得
 *
 * @param   sick
 *
 * @retval  u8
 */
//=============================================================================================
u16 BTL_POKEPARAM::GetSickParam( WazaSick sick ) const
{
  GFL_ASSERT(sick < GFL_NELEMS(m_coreParam.sickCont));

  switch(  m_coreParam.sickCont[sick].type ){
  case pml::wazadata::SICKCONT_TURN:
    return m_coreParam.sickCont[sick].turn.param;

  case pml::wazadata::SICKCONT_POKE:
    return m_coreParam.sickCont[sick].poke.ID;

  case pml::wazadata::SICKCONT_POKETURN:
    return m_coreParam.sickCont[sick].poketurn.pokeID;

  }

  GFL_ASSERT(0); // パラメ無いのに呼び出された
  return 0;
}
//=============================================================================================
/**
 * 特定状態異常の継続パラメータ取得
 *
 * @param   sick
 *
 * @retval  BTL_SICKCONT
 */
//=============================================================================================
BTL_SICKCONT  BTL_POKEPARAM::GetSickCont( WazaSick sick ) const
{
  GFL_ASSERT(sick < GFL_NELEMS(m_coreParam.sickCont));
  return m_coreParam.sickCont[ sick ];
}
//=============================================================================================
/**
 * 特定状態異常の継続ターンカウンタを取得
 *
 * @param   sick
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKEPARAM::GetSickTurnCount( WazaSick sick ) const
{
  GFL_ASSERT(sick < GFL_NELEMS(m_coreParam.sickCont));
  return m_coreParam.wazaSickCounter[ sick ];
}
//=============================================================================================
/**
 * 特定状態異常の残りターン数を取得
 *
 * @param   sick
 *
 * @retval  u8
 */
//=============================================================================================
bool BTL_POKEPARAM::IsSickLastTurn( WazaSick sickType ) const
{
  const BTL_SICKCONT& cont = this->GetSickCont( sickType );
  if( SICKCONT_IsNull(cont) ){
    return false;
  }

  u8 turnMax  = SICCONT_GetTurnMax( cont );
  u8 turnPast = this->GetSickTurnCount( sickType );

  int remain = turnMax - turnPast;
  if( remain > 1 ){
    return false;
  }

  return true;
}

//=============================================================================================
/**
 * 状態異常のターンチェックで減るHPの量を計算
 *
 * @param   pp
 *
 * @retval  int
 */
//=============================================================================================
int BTL_POKEPARAM::CalcSickDamage( WazaSick sick ) const
{
  if( CheckSick(sick) )
  {
    switch( sick ){
    case pml::wazadata::WAZASICK_DOKU:
      // 「どくどく」状態ならターン数でダメージ増加
      if( SICKCONT_IsMoudokuCont(m_coreParam.sickCont[sick]) ){
        return calc::QuotMaxHP(this, BTL_MOUDOKU_SPLIT_DENOM) * m_coreParam.wazaSickCounter[sick];
      }else{
        return calc::QuotMaxHP( this, BTL_DOKU_SPLIT_DENOM );
      }

    case pml::wazadata::WAZASICK_YAKEDO:
      return calc::QuotMaxHP( this, BTL_YAKEDO_SPLIT_DENOM );

    case pml::wazadata::WAZASICK_AKUMU:
      if( CheckNemuri( NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) ){
        return calc::QuotMaxHP( this, BTL_AKUMU_SPLIT_DENOM );
      }
      break;

    case pml::wazadata::WAZASICK_NOROI:
      return calc::QuotMaxHP( this, BTL_NOROI_SPLIT_DENOM );

    default:
      return 0;
    }
  }
  return 0;
}
//=============================================================================================
/**
 * こだわりロック状態なら、こだわっているワザのIDを返す
 *
 * @retval  WazaNo  こだわりロック状態なら、こだわっているワザID／それ以外は WAZANO_NULL
 */
//=============================================================================================
WazaNo BTL_POKEPARAM::GetKodawariWazaID( void ) const
{
  const BTL_SICKCONT& cont = this->GetSickCont( pml::wazadata::WAZASICK_KODAWARI );

  if( SICKCONT_IsNull(cont) ){
    return WAZANO_NULL;
  }

  return (WazaNo)(SICKCONT_GetParam( cont ));
}


//---------------------------------------------------------------------------------------------
// SrcPP との相互変換
//---------------------------------------------------------------------------------------------

//=============================================================================================
/**
 * Srcポケモンデータに現在のパラメータを反映
 *
 * @param   fDefaultForm
 */
//=============================================================================================
void BTL_POKEPARAM::ReflectToPP( bool fDefaultForm )
{
  pml::pokepara::PokemonParam* pp = (pml::pokepara::PokemonParam*)(m_coreParam.ppSrc);
  
  doryoku_PutToPP( &m_doryokuParam, pp );

  pp->SetExp( m_coreParam.exp );
  pp->SetHp( m_coreParam.hp );
  if( m_coreParam.hp )
  {
    PokeSick  sick = GetPokeSick();
    pp->SetSick( sick );
  }
  else{
    pp->SetSick( pml::pokepara::SICK_NULL );
  }

  wazaWork_ReflectToPP();

  {
    u8 formNo = ( m_coreParam.fHensin || fDefaultForm )? m_coreParam.defaultFormNo : m_formNo;
    pp->ChangeFormNo( formNo );
  }

  pp->SetItem( m_coreParam.item );
}



void BTL_POKEPARAM::wazaWork_UpdateNumber( WAZA_SET* waza, WazaID nextNumber, u8 ppMax, bool fPermenent )
{
  // スケッチ用
  if( fPermenent )
  {
    // 真ワザワークを更新、リンク中なら仮ワザワークも連動させる
    wazaCore_UpdateNumber( &waza->truth, nextNumber, ppMax );
    if( waza->fLinked ){
      waza->surface = waza->truth;
    }
  }
  // ものまね・へんしん用
  else
  {
    // 仮ワザワークのみを更新、リンクを切る
    wazaCore_UpdateNumber( &waza->surface, nextNumber, ppMax );
    waza->fLinked = false;
  }
}
/**
 *  ppMax  0ならデフォルト値
 */
void BTL_POKEPARAM::wazaCore_UpdateNumber( WAZA_CORE* core, WazaID nextID, u8 ppMax )
{
  core->number = nextID;
  core->usedFlag = false;
  core->usedFlagFix = false;

  if( nextID != WAZANO_NULL ){
    core->ppMax = WAZADATA_GetMaxPP( nextID, 0 );
  }
  else{
    core->ppMax = 0;
  }

  if( (ppMax !=0)
  &&  (core->ppMax > ppMax)
  ){
    core->ppMax = ppMax;
  }
  core->pp = core->ppMax;
}


//----------------------------------------------------------------------------------
/**
 * へんしん状態のクリア
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::clearHensin( void )
{
  if( m_coreParam.fHensin )
  {
    pml::pokepara::PokemonParam* ppSrc = reinterpret_cast<pml::pokepara::PokemonParam*>(m_coreParam.ppSrc);

    setupBySrcData( ppSrc, false, true );
    wazaWork_ClearSurface();

    m_coreParam.fHensin = false;
  }
}

//----------------------------------------------------------------------------------
/**
 * ワザ使用フラグのクリア
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::clearUsedWazaFlag( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_waza); ++i){
    wazaSet_ClearUsedFlag( &m_waza[i] );
  }

  m_prevSelectWazaID      = WAZANO_NULL;
  m_prevActWazaID         = WAZANO_NULL;
  m_isPrevActWazaZenryoku = false;
  m_prevWazaType          = POKETYPE_NULL;
  m_wazaContCounter       = 0;
}
//----------------------------------------------------------------------------------
/**
 * 各種カウンタ値をクリア
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::clearCounter( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_counter); ++i){
    m_counter[i] = 0;
  }
}


#if 0
//=============================================================================================
/**
 * バトル用ポケモンパラメータ削除
 *
 * @param   bpp
 *
 */
//=============================================================================================
void BTL_POKEPARAM_Delete( BTL_POKEPARAM* bpp )
{
  GFL_HEAP_FreeMemory( bpp );
}

//=============================================================================================
/**
 * データコピー
 *
 * @param   dst
 * @param   src
 *
 */
//=============================================================================================
void BTL_POKEPARAM_Copy( BTL_POKEPARAM* dst, const BTL_POKEPARAM* src )
{
  *dst = *src;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
#endif


/**
 *  ワザ所持数を返す（仮ワザワーク）
 */
u8 BTL_POKEPARAM::WAZA_GetCount( void ) const
{
  return m_wazaCnt;
}
/**
 *  ワザ所持数を返す（真ワザワーク）
 */
u8 BTL_POKEPARAM::WAZA_GetCount_Org( void ) const
{
  // 従来 for ループで処理していたが、ARMコンパイラのループ最適化処理にバグが見つかったため、
  // ループを使わないコードに変更している。 2016.06.08  taya
  if( m_waza[0].truth.number == WAZANO_NULL ){
    return 0;
  }
  if( m_waza[1].truth.number == WAZANO_NULL ){
    return 1;
  }
  if( m_waza[2].truth.number == WAZANO_NULL ){
    return 2;
  }
  if( m_waza[3].truth.number == WAZANO_NULL ){
    return 3;
  }
  return 4;

//  以下、従来のコード (PTL_WAZA_MAX = 4)
/*
    u32 cnt, i;
    for(i=0, cnt=0; i<PTL_WAZA_MAX; ++i)
    {
      if( m_waza[i].truth.number != WAZANO_NULL ){
        ++cnt;
      }else{
        break;
      }
    }
    return cnt;
*/
}

/**
 *  生きている間に使ったワザの数を返す（死んだらリセットされる）
 */
u8 BTL_POKEPARAM::WAZA_GetUsedCountInAlive( void ) const
{
  u8 cnt, i;
  for(i=0, cnt=0; i<m_wazaCnt; ++i)
  {
    if( m_waza[i].surface.usedFlag ){
      ++cnt;
    }
  }
  return cnt;
}

/**
 *  使ったワザの数を返す（死んでもカウントアップ）
 */
u8 BTL_POKEPARAM::WAZA_GetUsedCount( void ) const
{
  return m_usedWazaCount;
}

u8 BTL_POKEPARAM::WAZA_GetUsableCount( void ) const
{
  u8 cnt, i;
  for(i=0, cnt=0; i<m_wazaCnt; ++i)
  {
    if( m_waza[i].surface.pp ){
      ++cnt;
    }
  }
  return cnt;
}

WazaID BTL_POKEPARAM::WAZA_GetID( u8 idx ) const
{
  GFL_ASSERT(idx < m_wazaCnt);
  return m_waza[idx].surface.number;
}
WazaID BTL_POKEPARAM::WAZA_GetID_Org( u8 idx ) const
{
  return m_waza[idx].truth.number;
}
bool BTL_POKEPARAM::WAZA_CheckUsedInAlive( u8 idx ) const
{
  GFL_ASSERT(idx < m_wazaCnt);
  return m_waza[idx].surface.usedFlag;
}

void BTL_POKEPARAM::WAZA_Copy( BTL_POKEPARAM* bppDst )
{
  u32 i;
  for(i=0; i<GFL_NELEMS(bppDst->m_waza); ++i){
    bppDst->m_waza[i] = m_waza[i];
  }
  bppDst->m_wazaCnt = m_wazaCnt;
}

//=============================================================================================
/**
 * [ワザパラメータ] ID, 現PP, 最大PPを一度に取得
 *
 * @param   bpp
 * @param   idx     対象ワザインデックス
 * @param   PP      [out] 現PPを取得する変数アドレス
 * @param   PPMax   [out] 最大PPを取得する変数アドレス
 *
 * @retval  WazaID  ワザID
 */
//=============================================================================================
WazaID BTL_POKEPARAM::WAZA_GetParticular( u8 idx, u8* PP, u8* PPMax ) const
{
  GFL_ASSERT(idx < m_wazaCnt);

  *PP = m_waza[idx].surface.pp;
  *PPMax = m_waza[idx].surface.ppMax;

  return m_waza[idx].surface.number;
}

//=============================================================================================
/**
 * [ワザパラメータ] PP不足分を取得（仮ワザワーク）
 *
 * @param   bpp
 * @param   idx       対象ワザインデックス
 *
 * @retval  u8        PP不足分
 */
//=============================================================================================
u8 BTL_POKEPARAM::WAZA_GetPPShort( u8 idx ) const
{
  GFL_ASSERT(idx < m_wazaCnt);
  return (m_waza[idx].surface.ppMax - m_waza[idx].surface.pp);
}
//=============================================================================================
/**
 * [ワザパラメータ] PP不足分を取得（真ワザワーク）
 *
 * @param   bpp
 * @param   idx       対象ワザインデックス
 *
 * @retval  u8        PP不足分
 */
//=============================================================================================
u8 BTL_POKEPARAM::WAZA_GetPPShort_Org( u8 idx ) const
{
  GFL_ASSERT(m_waza[idx].truth.number != WAZANO_NULL);
  return (m_waza[idx].truth.ppMax - m_waza[idx].truth.pp);
}

//=============================================================================================
/**
 * [ワザパラメータ] PPが満タンでないワザスロットがあるかチェック（仮ワザワーク）
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::WAZA_CheckPPShortAny( void ) const
{
  u32 max = this->WAZA_GetCount();
  for(u32 i=0; i<max; ++i)
  {
    if( this->WAZA_GetPPShort(i) ){
      return true;
    }
  }
  return false;
}
//=============================================================================================
/**
 * [ワザパラメータ] PPが満タンでないワザスロットがあるかチェック（真ワザワーク）
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::WAZA_CheckPPShortAny_Org( void ) const
{
  u32 max = this->WAZA_GetCount_Org();
  for(u32 i=0; i<max; ++i)
  {
    if( this->WAZA_GetPPShort_Org(i) ){
      return true;
    }
  }
  return false;
}
//=============================================================================================
/**
 * ワザPP値を取得（仮ワザワーク）
 *
 * @param   pp
 * @param   wazaIdx
 *
 */
//=============================================================================================
u16 BTL_POKEPARAM::WAZA_GetPP( u8 wazaIdx ) const
{
  if( wazaIdx < m_wazaCnt ){
    return  m_waza[wazaIdx].surface.pp;
  }else{
    GFL_ASSERT_MSG(0,"wazaIdx:%d, wazaCnt:%d", wazaIdx, m_wazaCnt);
    return 0;
  }
}

//=============================================================================================
/**
 * ワザPP値をワザIDから取得（仮ワザワーク）
 *
 * @param   pp
 * @param   wazaIdx
 *
 */
//=============================================================================================
u16 BTL_POKEPARAM::WAZA_GetPP_ByNumber( WazaID waza ) const
{
  for(u32 i=0; i<m_wazaCnt; ++i)
  {
    if( m_waza[i].surface.number == waza ){
      return m_waza[i].surface.pp;
    }
  }
  return 0;
}
//=============================================================================================
/**
 * ワザPP値を取得（真ワザワーク）
 *
 * @param   pp
 * @param   wazaIdx
 *
 */
//=============================================================================================
u16 BTL_POKEPARAM::WAZA_GetPP_Org( u8 wazaIdx ) const
{
  GFL_ASSERT(m_waza[wazaIdx].truth.number != WAZANO_NULL);
  return  m_waza[wazaIdx].truth.pp;
}

//=============================================================================================
/**
 * ワザのPP最大値を取得（仮ワザワーク）
 *
 * @param   pp
 * @param   wazaIdx
 *
 */
//=============================================================================================
u16 BTL_POKEPARAM::WAZA_GetMaxPP( u8 wazaIdx ) const
{
  if( wazaIdx < m_wazaCnt ){
    return  m_waza[wazaIdx].surface.ppMax;
  }else{
    return 0;
  }
}

//=============================================================================================
/**
 * ワザPP最大値を取得（真ワザワーク）
 *
 * @param   pp
 * @param   wazaIdx
 *
 */
//=============================================================================================
u16 BTL_POKEPARAM::WAZA_GetMaxPP_Org( u8 wazaIdx ) const
{
  GFL_ASSERT(m_waza[wazaIdx].truth.number != WAZANO_NULL);
  return  m_waza[wazaIdx].truth.ppMax;
}

//=============================================================================================
/**
 * ワザPP値に回復の余地があるか判定
 *
 * @param   pp
 * @param   wazaIdx
 * @param   fOrg    trueなら真ワザワークの方を調べる
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::WAZA_IsPPFull( u8 wazaIdx, bool fOrg ) const
{
  const WAZA_CORE* core = (fOrg)? &(m_waza[wazaIdx].truth) : &(m_waza[wazaIdx].surface);

  if( (core->number != WAZANO_NULL)
  &&  (core->pp == core->ppMax)
  ){
    return true;
  }
  return false;
}
//=============================================================================================
/**
 * ワザPP値を減少
 *
 * @param   pp
 * @param   wazaIdx
 * @param   value
 *
 */
//=============================================================================================
void BTL_POKEPARAM::WAZA_DecrementPP( u8 wazaIdx, u8 value )
{
  GFL_ASSERT(wazaIdx < m_wazaCnt);

  if( m_waza[wazaIdx].surface.pp >= value )
  {
    m_waza[wazaIdx].surface.pp -= value;
  }
  else
  {
    m_waza[wazaIdx].surface.pp = 0;
  }

  if( m_waza[wazaIdx].fLinked )
  {
    m_waza[wazaIdx].truth.pp = m_waza[wazaIdx].surface.pp;
    m_coreParam.ppSrc->SetWazaPP( wazaIdx, m_waza[wazaIdx].truth.pp );
  }
}
//=============================================================================================
/**
 * ワザPP値を減少（ものまね等、一時的に上書きされている場合の上書き前の部分）
 *
 * @param   bpp
 * @param   wazaIdx
 * @param   value
 */
//=============================================================================================
void BTL_POKEPARAM::WAZA_DecrementPP_Org( u8 wazaIdx, u8 value )
{
  if( m_waza[wazaIdx].fLinked == false )
  {
    int pp =  m_waza[wazaIdx].truth.pp;
    pp -= value;
    if( pp < 0 ){
      pp = 0;
    }
    m_waza[wazaIdx].truth.pp = pp;
    m_coreParam.ppSrc->SetWazaPP( wazaIdx, m_waza[wazaIdx].truth.pp );
  }
}
//=============================================================================================
/**
 *
 *
 * @param   bpp
 * @param   wazaIdx
 */
//=============================================================================================
void BTL_POKEPARAM::WAZA_SetUsedFlag_Org( u8 wazaIdx )
{
  if( m_waza[wazaIdx].fLinked == false )
  {
    WAZA_SET* pWaza = &m_waza[ wazaIdx ];

    if( pWaza->truth.usedFlagFix == false )
    {
      pWaza->truth.usedFlagFix = true;
      m_usedWazaCount++;
    }
  }
}

//=============================================================================================
/**
 * ワザPP値を増加
 *
 * @param   pp
 * @param   wazaIdx
 * @param   value
 *
 * @retval  WazaID    回復したワザナンバー
 */
//=============================================================================================
WazaID BTL_POKEPARAM::WAZA_IncrementPP( u8 wazaIdx, u8 value )
{
  GFL_ASSERT(wazaIdx < PTL_WAZA_MAX);

  {
    WAZA_SET* wp = &m_waza[wazaIdx];

    wp->surface.pp += value;
    if( wp->surface.pp > wp->surface.ppMax )
    {
      wp->surface.pp = wp->surface.ppMax;
    }

    if( wp->fLinked ){
      wp->truth.pp = wp->surface.pp;
    }

    return wp->surface.number;
  }
}
//=============================================================================================
/**
 * ワザPP値を増加（真ワザワーク用）
 *
 * @param   pp
 * @param   wazaIdx
 * @param   value
 *
 * @retval  WazaID    回復したワザナンバー
 */
//=============================================================================================
WazaID BTL_POKEPARAM::WAZA_IncrementPP_Org( u8 wazaIdx, u8 value )
{
  GFL_ASSERT(wazaIdx < PTL_WAZA_MAX);

  {
    WAZA_SET* wp = &m_waza[wazaIdx];

    wp->truth.pp += value;
    if( wp->truth.pp > wp->truth.ppMax )
    {
      wp->truth.pp = wp->truth.ppMax;
    }

    if( wp->fLinked ){
      wp->surface.pp = wp->truth.pp;
    }

    return wp->truth.number;
  }
}
//=============================================================================================
/**
 * ワザが一時上書き状態にあるか判定
 *
 * @param   bpp
 * @param   wazaIdx   インデックス
 * @param   orgWaza   上書き前のワザID
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::WAZA_IsLinkOut( u8 wazaIdx ) const
{
  if( m_waza[wazaIdx].fLinked == false )
  {
    return true;
  }
  return false;
}
//=============================================================================================
/**
 * 使用したワザフラグを立てる
 *
 * @param   pp
 * @param   wazaIdx
 */
//=============================================================================================
void BTL_POKEPARAM::WAZA_SetUsedFlag( u8 wazaIdx )
{
  WAZA_SET* pWaza = &m_waza[ wazaIdx ];

  if( pWaza->surface.usedFlagFix == false )
  {
    pWaza->surface.usedFlagFix = true;
    m_usedWazaCount++;
  }
  pWaza->surface.usedFlag = true;

  if( pWaza->fLinked ){
    pWaza->truth.usedFlag = pWaza->surface.usedFlag;
    pWaza->truth.usedFlagFix = pWaza->surface.usedFlagFix;
  }
}
//=============================================================================================
/**
 * ワザナンバー上書き
 *
 * @param   pp
 * @param   wazaIdx       何番目のワザ[0-3]？
 * @param   waza          上書き後ワザナンバー
 * @param   ppMax         PP最大値の上限（最大でもデフォルト値まで。0ならデフォルト値になる）
 * @param   fPermenent    永続フラグ（trueならバトル後まで引き継ぐ／falseなら瀕死・入れかえで元に戻る）
 */
//=============================================================================================
void BTL_POKEPARAM::WAZA_UpdateID( u8 wazaIdx, WazaID waza, u8 ppMax, bool fPermenent )
{
  wazaWork_UpdateNumber( &m_waza[wazaIdx], waza, ppMax, fPermenent );
}
//=============================================================================================
/**
 * 指定ワザを覚えているかチェック
 *
 * @param   bpp
 * @param   waza
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::WAZA_IsUsable( WazaID waza ) const
{
  for(u32 i=0; i<GFL_NELEMS(m_waza); ++i)
  {
    if( m_waza[i].surface.number == waza ){
      return true;
    }
  }
  return false;
}



//=============================================================================================
/**
 * 指定ワザのindexを返す（自分が持っていないワザならPTL_WAZA_MAXを返す)
  *
 * @param   pp
 * @param   waza
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKEPARAM::WAZA_SearchIdx( WazaNo waza ) const
{
  if( waza != WAZANO_NULL )
  {
    for(u32 i=0; i<PTL_WAZA_MAX; ++i){
      if( m_waza[i].surface.number == waza ){
        return i;
      }
    }
  }
  return PTL_WAZA_MAX;
}



void BTL_POKEPARAM::splitTypeCore( pml::PokeType* type1, pml::PokeType* type2 ) const
{
  bool fHaneYasume = CheckSick( pml::wazadata::WAZASICK_HANEYASUME );

  // はねやすめで飛行タイプを無効にする
  if( (m_baseParam.type1 == POKETYPE_HIKOU) && (fHaneYasume) ){
    *type1 = POKETYPE_NULL;
  }else{
    *type1 = m_baseParam.type1;
  }

  if( (m_baseParam.type2 == POKETYPE_HIKOU) && (fHaneYasume) ){
    *type2 = POKETYPE_NULL;
  }else{
    *type2 = m_baseParam.type2;
  }

  // 技「もえつきる」使用後は、タイプ「なし」を許容する
  if( ( CONTFLAG_Get( CONTFLG_MOETUKIRU ) ) &&
      ( *type1 == POKETYPE_NULL ) &&
      ( *type2 == POKETYPE_NULL ) )
  {
    return;
  }

  // どちらか無効なら純粋タイプになる／元々純粋タイプならノーマルタイプになる
  if( (*type1) == POKETYPE_NULL )
  {
    if( (*type2) == POKETYPE_NULL ){
      *type2 = POKETYPE_NORMAL;
    }
    *type1 = *type2;
  }
  else if( (*type2) == POKETYPE_NULL )
  {
    *type2 = (*type1);
  }
}


PokeTypePair BTL_POKEPARAM::GetPokeType( void ) const
{
  PokeTypePair  typePair;
  pml::PokeType  type1, type2;

  splitTypeCore( &type1, &type2 );
  typePair = PokeTypePair_Make( type1, type2, m_baseParam.type_ex );

  return typePair;
}

//=============================================================================================
/**
 * ポケモンが持つ３タイプ（基本最大２タイプ＋拡張１タイプ）に、指定タイプが含まれているか判定
 *
 * @param   bpp
 * @param   type
 *
 * @retval  bool    含まれていればtrue
 */
//=============================================================================================
bool BTL_POKEPARAM::IsMatchType( pml::PokeType type ) const
{
  if( type != POKETYPE_NULL )
  {
    pml::PokeType  type1, type2;
    splitTypeCore( &type1, &type2 );

    if( (type1 == type) || (type2 == type) ){
      return true;
    }
    if( type == m_baseParam.type_ex ){
      return true;
    }
  }
  return false;
}

//=============================================================================================
/**
 * srcPPの各パラメータ取得（レベルアップ表示用）
 *
 * @param   bpp
 * @param   info
 */
//=============================================================================================
void BTL_POKEPARAM::GetPPStatus( BTL_LEVELUP_INFO* info ) const
{
  u8 fastFlag = m_coreParam.ppSrc->StartFastMode();

  info->hp     = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_HP );
  info->atk    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_ATK );
  info->def    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_DEF );
  info->sp_atk = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_SPATK );
  info->sp_def = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_SPDEF );
  info->agi    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_AGI );

  m_coreParam.ppSrc->EndFastMode( fastFlag );

}
//=============================================================================================
/**
 * 能力値を書き換え
 *
 * @param   bpp
 * @param   vid
 * @param   value
 */
//=============================================================================================
void BTL_POKEPARAM::SetBaseStatus( ValueID vid, u16 value )
{
  vid = convertValueID( vid );

  switch( vid ){
  case BPP_ATTACK:      m_baseParam.attack = value; break;
  case BPP_DEFENCE:     m_baseParam.defence = value; break;
  case BPP_SP_ATTACK:   m_baseParam.sp_attack = value; break;
  case BPP_SP_DEFENCE:  m_baseParam.sp_defence = value; break;
  case BPP_AGILITY:     m_baseParam.agility = value; break;

  default:
    GFL_ASSERT(0);
    break;
  }

  BTL_Printf("ポケ[%d]の能力(%d)を%dに書き換えた\n", m_coreParam.myID, vid, value);
}

//=============================================================================================
/**
 * クリティカルヒット時のパラメータ取得（攻撃側に不利なランク補正をフラットにする）
 *
 * @param   pp
 * @param   vid
 *
 * @retval  int
 */
//=============================================================================================
int BTL_POKEPARAM::GetValue_Critical( ValueID vid ) const
{
  bool fFlatParam = false;
  ValueID  vid_org = vid;

  vid = convertValueID( vid_org );

  switch( vid ){
  case BPP_ATTACK:     fFlatParam = (m_varyParam.attack < RANK_STATUS_DEFAULT); break;
  case BPP_SP_ATTACK:  fFlatParam = (m_varyParam.sp_attack < RANK_STATUS_DEFAULT); break;
  case BPP_DEFENCE:    fFlatParam = (m_varyParam.defence > RANK_STATUS_DEFAULT); break;
  case BPP_SP_DEFENCE: fFlatParam = (m_varyParam.sp_defence > RANK_STATUS_DEFAULT); break;

  default:
    break;
  }

  if( fFlatParam ){
    return GetValue_Base( vid_org );
  }else{
    return GetValue( vid_org );
  }
}

//=============================================================================================
/**
 * 所有アイテムIDを取得する
 * @retval  u16   所有アイテムID （未所持ならITEM_DUMMY_DATA）
 */
//=============================================================================================
u16 BTL_POKEPARAM::GetItem( void ) const
{
  return m_coreParam.item;
}
//=============================================================================================
/**
 * 所有アイテムIDを設定する
 * @param    itemID   所有させるアイテムID （未所持にするならITEM_DUMMY_DATA）
 */
//=============================================================================================
void BTL_POKEPARAM::SetItem( u16 itemID )
{
  m_coreParam.item = itemID;
}
//=============================================================================================
/**
 * 効果が発動する状態なら所有アイテムIDを返す
 * @param fldSim  フィールド状態管理
 * @return アイテムを所有しており、効果がある状態ならそのアイテムID／それ以外は ITEM_DUMMY_DATA
 */
//=============================================================================================
u16  BTL_POKEPARAM::GetItemEffective( const FieldStatus& fldSim ) const
{
  if( fldSim.CheckEffect(FieldStatus::EFF_MAGICROOM) ){
    return ITEM_DUMMY_DATA;
  }
  if( this->CheckSick(pml::wazadata::WAZASICK_SASIOSAE) ){
    return ITEM_DUMMY_DATA;
  }
  if( this->GetValue(BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_BUKIYOU ){
    return ITEM_DUMMY_DATA;
  }
  return this->GetItem();
}

//=============================================================================================
/**
 * @brief 場に出たターンの総数を取得する( １ターン中に何回場に出ても１回とみなす )
 */
//=============================================================================================
u16 BTL_POKEPARAM::GetTotalTurnCount( void ) const
{
  return m_coreParam.totalTurnCount;
}

//=============================================================================================
/**
 * @brief 場に出たターンの総数をインクリメントする
 */
//=============================================================================================
void BTL_POKEPARAM::IncTotalTurnCount( void )
{
  ++( m_coreParam.totalTurnCount );
}

//=============================================================================================
/**
 * 継続して場に出ているターン数を返す
 *
 * @param   pp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BTL_POKEPARAM::GetTurnCount( void ) const
{
  return m_turnCount;
}
//=============================================================================================
/**
 * 場に出た時のターン数を返す
 *
 * @param   pp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BTL_POKEPARAM::GetAppearTurn( void ) const
{
  return m_appearedTurn;
}

//=============================================================================================
/**
 * ターンフラグ値取得
 *
 * @param   pp
 * @param   flagID
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::TURNFLAG_Get( TurnFlag flagID ) const
{
  return flgbuf_get( m_turnFlag, flagID );
}
//=============================================================================================
/**
 * 場にいる間フラグ値取得
 *
 * @param   pp
 * @param   flagID
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::CONTFLAG_Get( ContFlag flagID ) const
{
  return flgbuf_get( m_contFlag, flagID );
}

//=============================================================================================
/**
 * 永続フラグ  値取得
 *
 * @param   flagID
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::PERMFLAG_Get( PermFlag flagID ) const
{
  return flgbuf_get( m_permFlag, flagID );
}
//=============================================================================================
/**
 * 永続フラグ  セット
 *
 * @param   flagID
 */
//=============================================================================================
void BTL_POKEPARAM::PERMFLAG_Set( PermFlag flagID )
{
  flgbuf_set( m_permFlag, flagID );
}


//=============================================================================================
/**
 * ワザ（そらをとぶ・あなをほる等）により画面から消えている状態チェック
 *
 * @param   bpp
 *
 * @retval  ContFlag
 */
//=============================================================================================
BTL_POKEPARAM::ContFlag  BTL_POKEPARAM::CONTFLAG_CheckWazaHide( void ) const
{
  static const ContFlag  hideState[] = {
    CONTFLG_SORAWOTOBU, CONTFLG_ANAWOHORU, CONTFLG_DIVING, CONTFLG_SHADOWDIVE,
  };

  for(u32 i=0; i<GFL_NELEMS(hideState); ++i)
  {
    if( CONTFLAG_Get(hideState[i]) )
    {
      return hideState[ i ];
    }
  }
  return CONTFLG_NULL;
}
//=============================================================================================
/**
 * ワザ（そらをとぶ・あなをほる等）により画面から消えている状態チェック
 *
 * @param   bpp
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::IsWazaHide( void ) const
{
  return (CONTFLAG_CheckWazaHide() != CONTFLG_NULL);
}
//=============================================================================================
/**
 * @brief フリーフォール使用中チェック
 * @retval true   フリーフォール使用中
 * @retval false  フリーフォール使用中でない
 */
//=============================================================================================
bool BTL_POKEPARAM::IsUsingFreeFall( void ) const
{
  u8 capturedPokeID = BTL_POKEPARAM::FreeFallCounterToPokeID( this->COUNTER_Get(BTL_POKEPARAM::COUNTER_FREEFALL) );
  if( capturedPokeID != BTL_POKEID_NULL ){
    return true;
  }
  return false;
}
//=============================================================================================
/**
 * 残りHP/MaxHP 比率（パーセンテージ）取得
 *
 * @param   pp
 *
 * @retval  fx32
 */
//=============================================================================================
fx32 BTL_POKEPARAM::GetHPRatio( void ) const
{
  double r = (double)(m_coreParam.hp * 100) / (double)(m_coreParam.hpMax);
  return FX32_CONST( r );
}

//-----------------------------
s8 BTL_POKEPARAM::getRankVaryStatus( ValueID type, s8* min, s8* max ) const
{
  const s8* ptr;

  *min = RANK_STATUS_MIN;
  *max = RANK_STATUS_MAX;

  switch( type ) {
  case BPP_ATTACK_RANK:     ptr = &m_varyParam.attack; break;
  case BPP_DEFENCE_RANK:    ptr = &m_varyParam.defence; break;
  case BPP_SP_ATTACK_RANK:  ptr = &m_varyParam.sp_attack; break;
  case BPP_SP_DEFENCE_RANK: ptr = &m_varyParam.sp_defence; break;
  case BPP_AGILITY_RANK:    ptr = &m_varyParam.agility; break;
  case BPP_HIT_RATIO:       ptr = &m_varyParam.hit; break;
  case BPP_AVOID_RATIO:     ptr = &m_varyParam.avoid; break;

  default:
    GFL_ASSERT_MSG(0, "illegal rank Type ->%d", type);
    return NULL;
  }
  return *ptr;
}

//=============================================================================================
/**
 * ランク増減効果が有効か？
 *
 * @param   rankType
 * @param   volume
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::IsRankEffectValid( ValueID rankType, int volume ) const
{
  s8  current;
  s8  min, max;

  current = getRankVaryStatus( rankType, &min, &max );
  BTL_Printf("  現状=%d, 最小=%d, 最大=%d, 効果値=%d\n", current, min, max, volume);
  if( volume > 0 ){
    return (current < max);
  }else{
    return (current > min);
  }
}
//=============================================================================================
/**
 * ランク増加効果があと何段階効くか判定
 *
 * @param   rankType
 *
 * @retval  int   段階数
 */
//=============================================================================================
int BTL_POKEPARAM::RankEffectUpLimit( ValueID rankType ) const
{
  s8  current;
  s8  min, max;

  current = getRankVaryStatus( rankType, &min, &max );
  BTL_Printf("ポケ[%d]のランク効果(%d) ... 現段階=%d, 上昇限界=%d\n", pp->coreParam.myID, rankType, *ptr, (max-(*ptr)));
  return max - current;
}
//=============================================================================================
/**
 * ランク減少効果があと何段階効くか判定
 *
 * @param   rankType
 *
 * @retval  int   段階数（マイナス）
 */
//=============================================================================================
int BTL_POKEPARAM::RankEffectDownLimit( ValueID rankType ) const
{
  s8  current;
  s8  min, max;

  current = getRankVaryStatus( rankType, &min, &max );
  return current - min;
}
//=============================================================================================
/**
 * フラットよりランクが下がっている能力が１つ以上あるかチェック
 *
 * @param   bpp
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::IsRankEffectDowned( void ) const
{
  if( m_varyParam.attack < RANK_STATUS_DEFAULT ){ return true; }
  if( m_varyParam.defence  < RANK_STATUS_DEFAULT ){ return true; }
  if( m_varyParam.sp_attack  < RANK_STATUS_DEFAULT ){ return true; }
  if( m_varyParam.sp_defence  < RANK_STATUS_DEFAULT ){ return true; }
  if( m_varyParam.agility  < RANK_STATUS_DEFAULT ){ return true; }
  if( m_varyParam.hit  < RANK_STATUS_DEFAULT ){ return true; }
  if( m_varyParam.avoid  < RANK_STATUS_DEFAULT ){ return true; }
  return false;
}
//=============================================================================================
/**
 * ランクアップ効果
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  u8 result   実際に上がった段階数
 */
//=============================================================================================
u8 BTL_POKEPARAM::RankUp( ValueID rankType, u8 volume )
{
  s8 *ptr;
  s8 max = RANK_STATUS_MAX;

  switch( rankType ){
  case BPP_ATTACK_RANK:     ptr = &m_varyParam.attack; break;
  case BPP_DEFENCE_RANK:    ptr = &m_varyParam.defence; break;
  case BPP_SP_ATTACK_RANK:  ptr = &m_varyParam.sp_attack; break;
  case BPP_SP_DEFENCE_RANK: ptr = &m_varyParam.sp_defence; break;
  case BPP_AGILITY_RANK:    ptr = &m_varyParam.agility; break;
  case BPP_HIT_RATIO:       ptr = &m_varyParam.hit; break;
  case BPP_AVOID_RATIO:     ptr = &m_varyParam.avoid; break;

  default:
    GFL_ASSERT(0);
    return false;
  }

  if( *ptr < max )
  {
    if( (*ptr + volume) > max ){
      volume = max - (*ptr);
    }
    *ptr += volume;
    return volume;
  }

  return 0;
}
//=============================================================================================
/**
 * ランクダウン効果
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  u8    実際に下がった段階数
 */
//=============================================================================================
u8 BTL_POKEPARAM::RankDown( ValueID rankType, u8 volume )
{
  s8 *ptr;
  s8 min = RANK_STATUS_MIN;

  switch( rankType ){
  case BPP_ATTACK_RANK:     ptr = &m_varyParam.attack; break;
  case BPP_DEFENCE_RANK:    ptr = &m_varyParam.defence; break;
  case BPP_SP_ATTACK_RANK:  ptr = &m_varyParam.sp_attack; break;
  case BPP_SP_DEFENCE_RANK: ptr = &m_varyParam.sp_defence; break;
  case BPP_AGILITY_RANK:    ptr = &m_varyParam.agility; break;
  case BPP_HIT_RATIO:       ptr = &m_varyParam.hit; break;
  case BPP_AVOID_RATIO:     ptr = &m_varyParam.avoid; break;

  default:
    GFL_ASSERT(0);
    return false;
  }

  if( *ptr > min )
  {
    if( (*ptr - volume) < min ){
      volume = (*ptr) - min;
    }
    *ptr -= volume;
    return volume;
  }
  return 0;
}
//=============================================================================================
/**
 * ランク値強制セット
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 */
//=============================================================================================
void BTL_POKEPARAM::RankSet( ValueID rankType, u8 value )
{
  s8 *ptr;
  u8 min = RANK_STATUS_MIN;
  u8 max = RANK_STATUS_MAX;

  switch( rankType ){
  case BPP_ATTACK_RANK:     ptr = &m_varyParam.attack; break;
  case BPP_DEFENCE_RANK:    ptr = &m_varyParam.defence; break;
  case BPP_SP_ATTACK_RANK:  ptr = &m_varyParam.sp_attack; break;
  case BPP_SP_DEFENCE_RANK: ptr = &m_varyParam.sp_defence; break;
  case BPP_AGILITY_RANK:    ptr = &m_varyParam.agility; break;
  case BPP_HIT_RATIO:       ptr = &m_varyParam.hit; break;
  case BPP_AVOID_RATIO:     ptr = &m_varyParam.avoid; break;

  default:
    GFL_ASSERT(0);
    return;
  }

  if( (min <= value) && (value <=max) ){
    *ptr = value;
  }else{
    GFL_ASSERT_MSG(0, "value=%d", value);
  }
}
//=============================================================================================
/**
 * ランク効果（７種）の内、下がっているものをフラットに戻す
 *
 * @param   bpp
 *
 * @retval  bool    有効だった（ランクが下がっている能力が１つ以上あった）場合、true
 */
//=============================================================================================
bool BTL_POKEPARAM::RankRecover( void )
{
  return effrank_Recover( &m_varyParam );
}
//=============================================================================================
/**
 * ランク効果（７種）をフラットに戻す
 */
//=============================================================================================
void BTL_POKEPARAM::RankReset( void )
{
  effrank_Reset( &m_varyParam );
}

//=============================================================================================
/**
 * 上昇しているランク効果（７種）をフラットに戻す
 * @retval true   能力ランクに変化があった
 * @retval false  能力ランクに変化がなかった
 */
//=============================================================================================
bool BTL_POKEPARAM::RankUpReset( void )
{
  return effrank_ResetRankUp( &m_varyParam );
}

//=============================================================================================
/**
 * クリティカルランク取得（気合いだめフラグも考慮）
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKEPARAM::GetCriticalRank( void ) const
{
  u8 result = this->GetCriticalRankPure();

  // きあいだめ
  if( CONTFLAG_Get(CONTFLG_KIAIDAME) )
  {
    result += 2;
  }

  // とぎすます
  if( CheckSick( pml::wazadata::WAZASICK_TOGISUMASU ) )
  {
    result += 3;
  }

  if( result > BTL_CALC_CRITICAL_MAX )
  {
    result = BTL_CALC_CRITICAL_MAX;
  }
  return result;
}
//=============================================================================================
/**
 * クリティカルランク取得（気合いだめフラグ、とぎすます状態を無視した純粋なクリティカルランク）
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKEPARAM::GetCriticalRankPure( void ) const
{
  return m_criticalRank;
}
//=============================================================================================
/**
 * クリティカルランク変動
 *
 * @param   bpp
 * @param   value
 *
 * @retval  bool    変動した場合true／既に上限（下限）で変動できない場合false
 */
//=============================================================================================
bool BTL_POKEPARAM::AddCriticalRank( int value )
{
  if( value > 0 )
  {
    if( m_criticalRank < BTL_CALC_CRITICAL_MAX )
    {
      m_criticalRank += value;
      if( m_criticalRank > BTL_CALC_CRITICAL_MAX ){
        m_criticalRank = BTL_CALC_CRITICAL_MAX;
      }
      return true;
    }
  }
  else
  {
    value *= -1;
    if( m_criticalRank )
    {
      if( m_criticalRank > value ){
        m_criticalRank -= value;
      }else{
        m_criticalRank = 0;
      }
      return true;
    }
  }
  return false;
}
//=============================================================================================
/**
 * クリティカルランクをセットする
 *
 * @param   rank
 */
//=============================================================================================
void BTL_POKEPARAM::SetCriticalRank( u8 rank )
{
  if( rank <= BTL_CALC_CRITICAL_MAX )
  {
    m_criticalRank = rank;
  }
}


//=============================================================================================
/**
 * HP値を減少
 *
 * @param   pp
 * @param   value
 *
 */
//=============================================================================================
void BTL_POKEPARAM::HpMinus( u16 value )
{
  if( m_coreParam.hp > value )
  {
    m_coreParam.hp -= value;
  }
  else
  {
    this->HpZero();
  }
}

//=============================================================================================
/**
 * HP値を増加
 *
 * @param   pp
 * @param   value
 *
 */
//=============================================================================================
void BTL_POKEPARAM::HpPlus( u16 value )
{
  m_coreParam.hp += value;
  if( m_coreParam.hp > m_coreParam.hpMax )
  {
    m_coreParam.hp = m_coreParam.hpMax;
  }
}
//=============================================================================================
/**
 * HP値をゼロにする
 *
 * @param   pp
 *
 */
//=============================================================================================
void BTL_POKEPARAM::HpZero( void )
{
  m_coreParam.hp = 0;
  m_coreParam.ppSrc->ResetMegaEvolve();
  ResetUltraBurst();
}

//=============================================================================================
/**
 * １ターン有効フラグのセット
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BTL_POKEPARAM::TURNFLAG_Set( TurnFlag flagID )
{
  flgbuf_set( m_turnFlag, flagID );
}
//=============================================================================================
/**
 * 永続フラグのセット
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BTL_POKEPARAM::CONTFLAG_Set( ContFlag flagID )
{
  flgbuf_set( m_contFlag, flagID );
}
//=============================================================================================
/**
 * 永続フラグのリセット
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BTL_POKEPARAM::CONTFLAG_Clear( ContFlag flagID )
{
  flgbuf_reset( m_contFlag, flagID );
}
//==================================================================================================
// 状態異常関連
//==================================================================================================

//=============================================================================================
/**
 * 状態異常を設定
 *
 * @param   pp
 * @param   sick
 * @param   contParam   継続パラメータ
 *
 */
//=============================================================================================
void BTL_POKEPARAM::SetWazaSick( WazaSick sick, const BTL_SICKCONT& contParam )
{
  if( calc::IsBasicSickID(sick) )
  {
    CurePokeSick();
  }

  m_coreParam.sickCont[ sick ] = contParam;
  m_coreParam.wazaSickCounter[sick] = 0;
}


//=============================================================================================
/**
 * 状態異常のターンチェック処理
 *
 * @param   bpp
 * @param   sick      チェックする状態異常コード
 * @param   pOldContDest    [out]チェック前の継続パラメータ（不要ならNULL）
 * @param   fCured          [out]このターンチェックで直った場合にtrue（不要ならNULL）
 *
 * @retval  bool    チェックした状態異常がこのチェック前まで継続されていた場合true
 */
//=============================================================================================
bool BTL_POKEPARAM::WazaSick_TurnCheck( WazaSick sick, BTL_SICKCONT* pOldContDest, bool* fCured )
{
  // ねむり・こんらんはアクション開始のタイミングで独自のカウンタデクリメント処理が入るので無視
  if( (sick == pml::wazadata::WAZASICK_NEMURI) || (sick == pml::wazadata::WAZASICK_KONRAN) ){
    return false;
  }

  if( m_coreParam.sickCont[sick].type != pml::wazadata::SICKCONT_NONE )
  {
    u32 turnMax = SICCONT_GetTurnMax( m_coreParam.sickCont[sick] );
    BTL_SICKCONT oldCont = m_coreParam.sickCont[ sick ];

    if( pOldContDest != NULL ){
      *pOldContDest = oldCont;
    }
    if( fCured != NULL ){
      *fCured = false;
    }

    // こだわりロックは、該当するワザを持っていないなら直る
    if( sick == pml::wazadata::WAZASICK_KODAWARI )
    {
      WazaNo  waza = static_cast<WazaNo>(SICKCONT_GetParam( oldCont ));
      if( !WAZA_IsUsable(waza) )
      {
        m_coreParam.sickCont[sick] = SICKCONT_MakeNull();;
        m_coreParam.wazaSickCounter[sick] = 0;
        if( fCured ){
          *fCured = true;
        }
      }
    }

    if( sick == pml::wazadata::WAZASICK_FUNJIN ){
      BTL_PRINT("ふんじんです 最大ターン数は %d, 現在カウンタは %d\n", turnMax, m_coreParam.wazaSickCounter[sick] );
    }

    // 継続ターン経過チェック
    if( turnMax )
    {
      m_coreParam.wazaSickCounter[sick] += 1;

      if( m_coreParam.wazaSickCounter[sick] >= turnMax )
      {
        BTL_PRINT("状態異常[%d] 経過ターンが最大ターンを越えたので治る\n", sick);
        m_coreParam.sickCont[sick] = SICKCONT_MakeNull();;
        m_coreParam.wazaSickCounter[sick] = 0;
        if( fCured ){
          *fCured = true;
        }
      }
    }

    // 永続型で最大ターン数が指定されているものはカウンタをインクリメント
    // （現状、この機構を利用しているのは「もうどく」のみ）
    else if( m_coreParam.sickCont[sick].type == pml::wazadata::SICKCONT_PERMANENT )
    {
      if( (m_coreParam.sickCont[sick].permanent.count_max != 0 )
      &&  (m_coreParam.wazaSickCounter[sick] < m_coreParam.sickCont[sick].permanent.count_max)
      ){
        m_coreParam.wazaSickCounter[sick]++;
      }
    }

    return true;
  }

  return false;
}
//=============================================================================================
/**
 * 「ねむり」独自のターンチェック処理
 * 状態異常カウンタをインクリメントし、起きる／起きないのチェックを行う
 *
 * @param   bpp
 *
 * @retval  bool    今回の呼び出しによって起きた場合はtrue／それ以外はfalse
 */
//=============================================================================================
bool BTL_POKEPARAM::CheckNemuriWakeUp( void )
{
  BTL_SICKCONT* cont = &(m_coreParam.sickCont[ pml::wazadata::WAZASICK_NEMURI ]);

  if( cont->type != pml::wazadata::SICKCONT_NONE )
  {
    u32 turnMax = SICCONT_GetTurnMax( *cont );
    if( turnMax )
    {
      // とくせい「はやおき」は眠りカウンタ２倍速
      u32 incValue = IsMatchTokusei(TOKUSEI_HAYAOKI)? 2 : 1;

      m_coreParam.wazaSickCounter[ pml::wazadata::WAZASICK_NEMURI ] += incValue;
      BTL_N_Printf( DBGSTR_BPP_NemuriWakeCheck, m_coreParam.myID, turnMax, m_coreParam.wazaSickCounter[ pml::wazadata::WAZASICK_NEMURI ] );
      if( m_coreParam.wazaSickCounter[ pml::wazadata::WAZASICK_NEMURI ] >= turnMax )
      {
        *cont = SICKCONT_MakeNull();;
        CureWazaSick( pml::wazadata::WAZASICK_NEMURI );
        return true;
      }
    }
  }
  return false;
}
//=============================================================================================
/**
 * 「こんらん」独自のターンチェック処理
 * 状態異常カウンタをインクリメントし、さめる／さめないのチェックを行う
 *
 * @param   bpp
 *
 * @retval  bool    今回の呼び出しによって覚めた場合はtrue／それ以外はfalse
 */
//=============================================================================================
bool BTL_POKEPARAM::CheckKonranWakeUp( void )
{
  enum {
    SICK_ID = pml::wazadata::WAZASICK_KONRAN,
  };
  BTL_SICKCONT* cont = &(m_coreParam.sickCont[ SICK_ID ]);

  if( cont->type != pml::wazadata::SICKCONT_NONE )
  {
    u32 turnMax = SICCONT_GetTurnMax( *cont );
    if( turnMax )
    {
      m_coreParam.wazaSickCounter[ SICK_ID ]++;
      if( m_coreParam.wazaSickCounter[ SICK_ID ] >= turnMax )
      {
        *cont = SICKCONT_MakeNull();;
        CureWazaSick( static_cast<WazaSick>(SICK_ID) );
        return true;
      }
    }
  }
  return false;
}
//=============================================================================================
/**
 * ポケモン系状態異常を回復させる
 *
 * @param   pp
 *
 */
//=============================================================================================
void BTL_POKEPARAM::CurePokeSick( void )
{
  for(u32 i=POKESICK_ORIGIN; i<POKESICK_MAX; ++i)
  {
    m_coreParam.sickCont[ i ] = SICKCONT_MakeNull();
    m_coreParam.wazaSickCounter[ i ] = 0;
    cureDependSick( static_cast<WazaSick>(i) );
  }
}

//----------------------------------------------------------------------------------
/**
 * ある状態異常が治った時に同時に治る状態の処理
 *
 * @param   bpp
 * @param   sickID
 */
//----------------------------------------------------------------------------------
void BTL_POKEPARAM::cureDependSick( WazaSick sickID  )
{
  switch( sickID ){
  case pml::wazadata::WAZASICK_NEMURI:
    m_coreParam.sickCont[ pml::wazadata::WAZASICK_AKUMU ] = SICKCONT_MakeNull();    // 眠りが治れば“あくむ”も治る
    m_coreParam.wazaSickCounter[ pml::wazadata::WAZASICK_AKUMU ] = 0;
    break;
  }
}
//=============================================================================================
/**
 * ワザ系状態異常を回復させる
 *
 * @param   pp
 * @param   sick
 *
 */
//=============================================================================================
void BTL_POKEPARAM::CureWazaSick( WazaSick sick )
{
  if( calc::IsBasicSickID(sick) )
  {
    CurePokeSick();
  }
  else
  {
    m_coreParam.sickCont[ sick ] = SICKCONT_MakeNull();
    m_coreParam.wazaSickCounter[ sick ] = 0;
  }
}

//=============================================================================================
/**
 * 特定ポケモンに依存している状態異常を回復させる
 *
 * @param   pp
 * @param   depend_pokeID
 */
//=============================================================================================
void BTL_POKEPARAM::CureWazaSickDependPoke( u8 depend_pokeID )
{
  if( depend_pokeID != BTL_POKEID_NULL )
  {
    for(u32 i=0; i<pml::wazadata::WAZASICK_MAX; ++i)
    {
      if( !SICKCONT_IsNull(m_coreParam.sickCont[i]) )
      {
        if( SICKCONT_GetPokeID(m_coreParam.sickCont[i]) == depend_pokeID )
        {
          m_coreParam.sickCont[i] = SICKCONT_MakeNull();
          cureDependSick( static_cast<WazaSick>(i) );
        }
      }
    }
  }
}

//=============================================================================================
/**
 * 場に入場した時のターンナンバーをセット
 *
 * @param   pp
 * @param   turn
 *
 */
//=============================================================================================
void BTL_POKEPARAM::SetAppearTurn( u16 turn )
{
  GFL_ASSERT(turn < BTL_TURNCOUNT_MAX);
  m_appearedTurn = turn;
  m_turnCount = 0;
  m_coreParam.fBtlIn = true;
  dmgrec_ClearWork();
}
//=============================================================================================
/**
 * ターン終了処理
 *
 * @param   pp
 *
 */
//=============================================================================================
void BTL_POKEPARAM::TurnCheck( void )
{
  flgbuf_clear( m_turnFlag, sizeof(m_turnFlag) );

  if( m_turnCount < BTL_TURNCOUNT_MAX )
  {
    m_turnCount++;
  }

  dmgrec_FwdTurn();
  resetSpActPriority();
}
//=============================================================================================
/**
 * ターンフラグの個別途中クリア
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BTL_POKEPARAM::TURNFLAG_ForceOff( TurnFlag flagID )
{
  flgbuf_reset( m_turnFlag, flagID );
}
//=============================================================================================
/**
 * 死亡による各種状態クリア
 *
 * @param   pp
 */
//=============================================================================================
void BTL_POKEPARAM::Clear_ForDead( void )
{
  flgbuf_clear( m_turnFlag, sizeof(m_turnFlag) );
  flgbuf_clear( m_contFlag, sizeof(m_contFlag) );

  clearHensin();
  wazaWork_ClearSurface();
  clearUsedWazaFlag();
  clearCounter();
  CombiWaza_ClearParam();
  FakeDisable();
  resetSpActPriority();

  MIGAWARI_Delete();
  clearWazaSickWork( SICKWORK_CLEAR_ALL );
  effrank_Init( &m_varyParam );

  m_formNo = m_coreParam.defaultFormNo;
  m_tokusei = m_coreParam.defaultTokusei;

  {
    pml::pokepara::PokemonParam* pp = m_coreParam.ppSrc;
    pp->ChangeFormNo( m_formNo );
    correctMaxHP();
    updateWeight();
  }
}

#if PM_DEBUG
void BTL_POKEPARAM::printHPDebug( u32 line )
{
  #if defined( DEBUG_ONLY_FOR_taya )
  u32 hp = this->GetValue( BPP_HP );
  u32 hpSrc = m_coreParam.ppSrc->GetHp();
  TAYA_PRINT("  [BPP] printHP  line=%d,   %d / %d\n", line, hp, hpSrc);
  #endif
}
#endif

//=============================================================================================
/**
 * 退場による各種状態クリア
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_POKEPARAM::Clear_ForOut( void )
{
  flgbuf_clear( m_turnFlag, sizeof(m_turnFlag) );

  /*
   * ※バトンタッチで引き継ぐ情報を記録しているため、
   * 退場時に継続フラグクリアはさせてはいけない
   */

  clearHensin();
  wazaWork_ClearSurface();
  clearUsedWazaFlag();
  clearCounter();
  CombiWaza_ClearParam();
  FakeDisable();
  resetSpActPriority();

  if( !CONTFLAG_Get(CONTFLG_BATONTOUCH) )
  {
    MIGAWARI_Delete();
    clearWazaSickWork( SICKWORK_CLEAR_ONLY_WAZASICK | SICKWORK_CLEAR_WITHOUT_SLEEP );
    effrank_Init( &m_varyParam );
    flgbuf_clear( m_contFlag, sizeof(m_contFlag) );
  }
  else
  {
    BTL_PRINT("ポケid=%d, バトンタッチ状態だから継続フラグをクリアしない\n", m_coreParam.myID);
  }

  if (!m_coreParam.ppSrc->IsMegaEvolved() && !IsGensiKaiki() && !IsUltraBurst() && !m_coreParam.fDontResetFormByByOut)
  {
    m_formNo = m_coreParam.defaultFormNo;
    m_tokusei = m_coreParam.defaultTokusei;
    m_coreParam.ppSrc->ChangeFormNo( m_formNo );
  }
  else
  {
    m_formNo  = m_coreParam.ppSrc->GetFormNo();
    m_tokusei = m_coreParam.ppSrc->GetTokuseiNo();
  }
}

//=============================================================================================
/**
 * 再入場による各種状態初期化
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_POKEPARAM::Clear_ForIn( void )
{
  setupBySrcData( m_coreParam.ppSrc, false, true );

  flgbuf_clear( m_contFlag, sizeof(m_contFlag) );
  flgbuf_clear( m_turnFlag, sizeof(m_turnFlag) );

  MIGAWARI_Delete();

  clearUsedWazaFlag();
  clearWazaSickWork( SICKWORK_CLEAR_ONLY_WAZASICK | SICKWORK_CLEAR_WITHOUT_SLEEP );
  effrank_Init( &m_varyParam );
}
//=============================================================================================
/**
 * バトンタッチによるパラメータ受け継ぎ
 *
 * @param   target    受け継ぐ側
 * @param   user      受け継がせる側
 */
//=============================================================================================
void BTL_POKEPARAM::CopyBatonTouchParams( BTL_POKEPARAM* user )
{
  m_varyParam = user->m_varyParam;

  BTL_N_Printf( DBGSTR_BPP_BatonTouch,
    user->m_coreParam.myID, m_coreParam.myID,
      m_varyParam.attack, m_varyParam.defence );

  m_migawariHP = user->m_migawariHP;

  // 特定の状態異常をそのまま引き継ぎ
  for(u32 i=0; i<pml::wazadata::WAZASICK_MAX; ++i)
  {
    if( (user->m_coreParam.sickCont[i].type != pml::wazadata::SICKCONT_NONE)
    &&  (BTL_SICK_CheckBatonTouchInherit(static_cast<WazaSick>(i), this))
    ){
      m_coreParam.sickCont[i] = user->m_coreParam.sickCont[i];
      m_coreParam.wazaSickCounter[i] = user->m_coreParam.wazaSickCounter[i];
    }
  }

  // 特定の継続フラグを引き継ぎ（今のところパワートリック＆きあいだめ）
  if( user->CONTFLAG_Get(CONTFLG_POWERTRICK) ){
    u16 atk, def;
    CONTFLAG_Set(CONTFLG_POWERTRICK);

    atk = GetValue_Base( BPP_ATTACK );
    def = GetValue_Base( BPP_DEFENCE );
    SetBaseStatus( BPP_ATTACK, def );
    SetBaseStatus( BPP_DEFENCE, atk );
  }
  if( user->CONTFLAG_Get(CONTFLG_KIAIDAME) ){
    CONTFLAG_Set(CONTFLG_KIAIDAME);
  }

  // 受け継がせたらクリア
  user->MIGAWARI_Delete();
  user->clearWazaSickWork( SICKWORK_CLEAR_ONLY_WAZASICK );
  effrank_Init( &user->m_varyParam );
  flgbuf_clear( user->m_contFlag, sizeof(user->m_contFlag) );
}

//=============================================================================================
/**
 * タイプかきかえ
 *
 * @param   type         新しいタイプ
 * @param   exTypeCause  拡張タイプが存在するなら、拡張の原因( 拡張タイプが存在しないなら、EXTYPE_CAUSE_NONE )
 * @return  bool   タイプに変化があった場合は true
 */
//=============================================================================================
bool BTL_POKEPARAM::ChangePokeType( PokeTypePair type, ExTypeCause exTypeCause )
{
  pml::PokeType  prev_type1  = m_baseParam.type1;
  pml::PokeType  prev_type2  = m_baseParam.type2;
  pml::PokeType  prev_typeEx = m_baseParam.type_ex;

  m_baseParam.type1         = PokeTypePair_GetType1( type );
  m_baseParam.type2         = PokeTypePair_GetType2( type );
  m_baseParam.type_ex       = PokeTypePair_GetTypeEx( type );
  m_baseParam.type_ex_cause = exTypeCause;

  if( (prev_type1 == m_baseParam.type1) && (prev_type2 == m_baseParam.type2)
  ||  (prev_type1 == m_baseParam.type2) && (prev_type2 == m_baseParam.type1)
  ){
    if( prev_typeEx == m_baseParam.type_ex ){
      return false;
    }
  }
  return true;
}
//=============================================================================================
/**
 * 拡張タイプ追加
 *
 * @param   pp
 * @param   type
 * @param   exTypeCause  タイプ拡張の原因
 */
//=============================================================================================
void BTL_POKEPARAM::ExPokeType( pml::PokeType type, ExTypeCause exTypeCause )
{
  if( (type != m_baseParam.type1)
  &&  (type != m_baseParam.type2)
  ){
    m_baseParam.type_ex = type;
    m_baseParam.type_ex_cause = exTypeCause;
  }
}

//=============================================================================================
/**
 * 設定されている拡張タイプを取得
 *
 * @retval  pml::PokeType
 */
//=============================================================================================
pml::PokeType  BTL_POKEPARAM::GetExType( void ) const
{
  return m_baseParam.type_ex;
}

//=============================================================================================
/**
 * @brief タイプが拡張されているか？
 * @retval true   タイプ拡張されている
 * @retval false  タイプ拡張されていない
 */
//=============================================================================================
bool BTL_POKEPARAM::HaveExType( void ) const
{
  pml::PokeType exType = this->GetExType();
  return ( exType != POKETYPE_NULL );
}

//=============================================================================================
/**
 * @brief タイプ拡張の原因を取得する
 *
 * @retval EXTYPE_CAUSE_NONE  タイプ拡張されていない
 */
//=============================================================================================
BTL_POKEPARAM::ExTypeCause  BTL_POKEPARAM::GetExTypeCause( void ) const
{
  return m_baseParam.type_ex_cause;
}


//=============================================================================================
/**
 * とくせい書き換え
 *
 * @param   pp
 * @param   tok
 */
//=============================================================================================
void BTL_POKEPARAM::ChangeTokusei( TokuseiNo tok )
{
  m_tokusei = tok;
}
//=============================================================================================
/**
 * フォルム変更
 *
 * @param   bpp
 * @param   formNo
 * @param   dontResetFormByOut  trueなら、場から退場してもデフォルトフォルムに戻らない
 */
//=============================================================================================
void BTL_POKEPARAM::ChangeForm( u8 formNo, bool dontResetFormByOut )
{
  pml::pokepara::PokemonParam* pp = const_cast<pml::pokepara::PokemonParam*>(m_coreParam.ppSrc);
  bool fFastMode;

  m_formNo = formNo;
  m_coreParam.fDontResetFormByByOut = dontResetFormByOut;

  fFastMode = pp->StartFastMode();
  pp->ChangeFormNo( formNo );

  setupBySrcDataBase( m_coreParam.ppSrc, true );
  correctMaxHP();
  updateWeight();

  if( (m_coreParam.monsno == MONSNO_SHEIMI) && (formNo == FORMNO_SHEIMI_LAND) )
  {
    m_coreParam.defaultFormNo = FORMNO_SHEIMI_LAND;
    m_tokusei = pp->GetTokuseiNo();
  }

  pp->EndFastMode( fFastMode );
}

/**
 * @brief 最大HPを更新する
 *
 * @note 最大HPの変動時応じて、残りHPも変化します
 */
void BTL_POKEPARAM::correctMaxHP( void )
{
  const u32 HP_old    = this->GetValue( BPP_HP );
  const u32 maxHP_old = this->GetValue( BPP_MAX_HP );
  const u32 maxHP_new = m_coreParam.ppSrc->GetMaxHp();

  m_coreParam.hpMax = maxHP_new;

  // 最大HPが増えた場合、最大HPの増加分を残りHPに加算する
  if( maxHP_old < maxHP_new )
  {
    this->HpPlus( maxHP_new - maxHP_old );
  }

  // 最大HPが減った場合、残りHPが最大HPを超えないようにする
  if( maxHP_new < maxHP_old )
  {
    if( maxHP_new < HP_old )
    {
      this->HpMinus( HP_old - maxHP_new );
    }
  }
}


//=============================================================================================
/**
 * 所有アイテム削除（所有アイテムが無くなるだけ）
 *
 * @param   pp
 */
//=============================================================================================
void BTL_POKEPARAM::RemoveItem( void )
{
  m_coreParam.usedItem = m_coreParam.item;
  m_coreParam.item = ITEM_DUMMY_DATA;
}
//=============================================================================================
/**
 * 所有アイテム消費（所有アイテムが無くなり、消費バッファに記憶する）
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_POKEPARAM::ConsumeItem( u16 itemID )
{
  m_coreParam.usedItem = itemID;
  m_coreParam.item = ITEM_DUMMY_DATA;
  BTL_PRINT("[BPP_]使用アイテム %d,  adrs=%p\n", m_coreParam.usedItem, &(m_coreParam.usedItem) );
}
//=============================================================================================
/**
 * アイテム消費情報のクリア
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_POKEPARAM::ClearConsumedItem( void )
{
  m_coreParam.usedItem = ITEM_DUMMY_DATA;
}
//=============================================================================================
/**
 * 消費したアイテムナンバーを返す
 *
 * @param   bpp
 *
 * @retval  u16   消費したアイテムナンバー（消費していなければITEM_DUMMY_DATA）
 */
//=============================================================================================
u16 BTL_POKEPARAM::GetConsumedItem( void ) const
{
  return m_coreParam.usedItem;
}
//=============================================================================================
/**
 * ワザプロセス終了通知
 *
 * @param   bpp
 * @param   actTargetPos  actWaza の対象位置
 * @param   fActEnable    actWaza が効果があったか（ワザエフェクトが出るか）
 * @param   fActZenryoku  実行されたワザがオーラ技か？
 * @param   actWaza       実行されたワザ
 * @param   orgWaza       クライアントが選択したワザ（ゆびをふる等、派生ワザの場合のみ actWaza と異なる）
 */
//=============================================================================================
void BTL_POKEPARAM::UpdateWazaProcResult( BtlPokePos actTargetPos, pml::PokeType actWazaType, bool fActEnable, bool fActZenryoku, WazaID actWaza, WazaID orgWaza )
{
  WazaID  prevActWaza = m_prevActWazaID;

  m_prevSelectWazaID      = orgWaza;
  m_prevActWazaID         = actWaza;
  m_isPrevActWazaZenryoku = fActZenryoku;
  m_prevTargetPos         = actTargetPos;
  m_prevWazaType          = actWazaType;

  if( prevActWaza == actWaza )
  {
    if( fActEnable ){
      m_wazaContCounter++;
    }else{
      m_wazaContCounter = 0;
    }
  }
  else{
    m_wazaContCounter = ( fActEnable )? 1 : 0;
  }
}

//=============================================================================================
/**
 * 同じワザを連続何回成功しているか返す（連続していない場合は0、連続中は1オリジンの値が返る）
 *
 * @param   pp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_POKEPARAM::GetWazaContCounter( void ) const
{
  return m_wazaContCounter;
}
//=============================================================================================
/**
 * 直近に実行されたワザナンバーを返す（場に出てから使ったワザが無い場合は WAZANO_NULL ）
 *
 * @param   pp
 *
 * @retval  WazaID
 */
//=============================================================================================
WazaID BTL_POKEPARAM::GetPrevWazaID( void ) const
{
  return m_prevActWazaID;
}
//=============================================================================================
/**
 * @brief 直近に実行されたワザがオーラ技かどうかを返す（場に出てから使ったワザが無い場合は false ）
 */
//=============================================================================================
bool BTL_POKEPARAM::IsPrevWazaZenryoku( void ) const
{
  return m_isPrevActWazaZenryoku;
}
//=============================================================================================
/**
 * 直近に実行されたワザタイプを返す（場に出てから使ったワザが無い場合は POKETYPE_NULL ）
 *
 * @param   bpp
 *
 * @retval  pml::PokeType
 */
//=============================================================================================
pml::PokeType BTL_POKEPARAM::GetPrevWazaType( void ) const
{
  return m_prevWazaType;
}
//=============================================================================================
/**
 * 直近に選択されたワザナンバーを返す（派生ワザを選択した場合など、BPP_GetPrevWazaIDと値が異なる場合がある）
 *
 * @param   bpp
 *
 * @retval  WazaID
 */
//=============================================================================================
WazaID BTL_POKEPARAM::GetPrevOrgWazaID( void ) const
{
  return m_prevSelectWazaID;
}

//=============================================================================================
/**
 * 直前に狙った相手の位置を返す
 *
 * @param   pp
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BTL_POKEPARAM::GetPrevTargetPos( void ) const
{
  return m_prevTargetPos;
}

//=============================================================================================
/**
 * バトルに一度でも参加したかどうか判定
 *
 * @param   bpp
 *
 * @retval  bool    参加していた場合true
 */
//=============================================================================================
bool BTL_POKEPARAM::GetBtlInFlag( void ) const
{
  return m_coreParam.fBtlIn;
}
//=============================================================================================
/**
 * 体重を設定
 *
 * @param   bpp
 * @param   weight
 */
//=============================================================================================
void BTL_POKEPARAM::SetWeight( u16 weight )
{
  if( weight < BTL_POKE_WEIGHT_MIN ){
    weight = BTL_POKE_WEIGHT_MIN;
  }
  m_weight = weight;
}
//=============================================================================================
/**
 * 体重を取得
 *
 * @param   bpp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BTL_POKEPARAM::GetWeight( void ) const
{
  return m_weight;
}
//=============================================================================================
/**
 * ワザダメージレコード：１件記録
 *
 * @param   pp
 * @param   rec
 */
//=============================================================================================
void BTL_POKEPARAM::WAZADMGREC_Add( const WAZADMG_REC* rec )
{
  u8 turn, idx;

  turn = m_dmgrecTurnPtr;
  idx = m_dmgrecCount[ turn ];

  // 最大記録数を超えていたら古いものを破棄する
  if( idx == WAZADMG_REC_MAX )
  {
    u32 i;
    for(i=0; i<(WAZADMG_REC_MAX-1); ++i){
      m_wazaDamageRec[ turn ][ i ] = m_wazaDamageRec[ turn ][ i+1 ];
    }
    --idx;
  }

  m_wazaDamageRec[ turn ][ idx ] = *rec;

  if( m_dmgrecCount[ turn ] < WAZADMG_REC_MAX ){
    m_dmgrecCount[ turn ]++;
  }
}
//=============================================================================================
/**
 * ワザダメージレコード：指定ターンの記録件数を取得
 *
 * @param   pp
 * @param   turn_ridx   遡るターン数（0なら当該ターンの記録）
 */
//=============================================================================================
u8 BTL_POKEPARAM::WAZADMGREC_GetCount( u8 turn_ridx ) const
{
  if( turn_ridx < WAZADMG_REC_TURN_MAX )
  {
    int turn = m_dmgrecTurnPtr - turn_ridx;
    if( turn < 0 ){
      turn += WAZADMG_REC_TURN_MAX;
    }
    return m_dmgrecCount[ turn ];
  }
  else
  {
    GFL_ASSERT( 0 );
    return 0;
  }
}
//=============================================================================================
/**
 * ワザダメージレコード：レコードを取得
 *
 * @param   bpp
 * @param   turn_ridx   遡るターン数（0なら当該ターンの記録）
 * @param   rec_ridx    何件目のレコードか？（0=最新）
 * @param   dst         [out] 取得先ワークアドレス
 *
 * @retval  bool        レコードを取得できたらtrue／それ以上の記録が無い場合などfalse
 */
//=============================================================================================
bool BTL_POKEPARAM::WAZADMGREC_Get( u8 turn_ridx, u8 rec_ridx, WAZADMG_REC* dst ) const
{
  if( turn_ridx < WAZADMG_REC_TURN_MAX )
  {
    int turn = m_dmgrecTurnPtr - turn_ridx;
    if( turn < 0 ){
      turn += WAZADMG_REC_TURN_MAX;
    }
    if( rec_ridx < m_dmgrecCount[ turn ] )
    {
      u8 idx = m_dmgrecCount[ turn ] - rec_ridx - 1;
      *dst = m_wazaDamageRec[ turn ][ idx ];
      return true;
    }
  }
  return false;
}

//=============================================================================================
/**
 * カウンタ値をセット
 *
 * @param   bpp
 * @param   cnt     カウンタ用途
 * @param   value   カウンタ値
 */
//=============================================================================================
void BTL_POKEPARAM::COUNTER_Set( Counter cnt, u8 value )
{
  GFL_ASSERT(cnt < COUNTER_MAX);
  m_counter[ cnt ] = value;
}
//=============================================================================================
/**
 * カウンタ値をインクリメント
 *
 * @param   cnt     カウンタ用途
 */
//=============================================================================================
void BTL_POKEPARAM::COUNTER_Inc( Counter cnt )
{
  GFL_ASSERT(cnt < COUNTER_MAX);

  if( m_counter[ cnt ] < 0xff )
  {
    m_counter[ cnt ]++;
  }
}
//=============================================================================================
/**
 * カウンタ値を取得
 *
 * @param   bpp
 * @param   cnt   カウンタ用途
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKEPARAM::COUNTER_Get( Counter cnt ) const
{
  GFL_ASSERT(cnt < COUNTER_MAX);
  return m_counter[ cnt ];
}
//=============================================================================================
/**
 * 経験値加算（サーバ用：Srcポケモンデータへの反映は行わない）
 *
 * @param   bpp
 * @param   expRest  [io] 加算する経験値／レベルアップ時は残りの加算経験値
 * @param   info     [out] レベルアップ時に各種パラメータの上昇値を格納する
 *
 * @retval  bool    レベルアップしたらtrue
 */
//=============================================================================================
bool BTL_POKEPARAM::AddExp( u32* expRest, BTL_LEVELUP_INFO* info )
{
  if( m_coreParam.level < pml::MAX_POKE_LEVEL )
  {
    pml::pokepara::PokemonParam* pp = const_cast<pml::pokepara::PokemonParam*>(m_coreParam.ppSrc);


    u32 expNow = m_coreParam.exp;
    u32 expSum = expNow + (*expRest);
    u32 expBorder = calc::PERSONAL_GetMinExp( m_coreParam.monsno, m_formNo, m_coreParam.level+1 );

    if( expSum >= expBorder )
    {
      u32 expAdd = (expBorder - expNow);
      u16 prevHP   = m_coreParam.hpMax;
      u16 diffHP;
      u16 atk, def, sp_atk, sp_def, agi;
      bool  fFastMode = pp->StartFastMode();

      // レベルアップ前パラメータ
      atk    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_ATK );
      def    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_DEF );
      sp_atk = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_SPATK );
      sp_def = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_SPDEF );
      agi    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_AGI );

      doryoku_PutToPP( &m_doryokuParam, pp );

      m_coreParam.exp = (expNow + expAdd);
      pp->SetExp( m_coreParam.exp );

      // レベルアップ後パラメータ
      m_coreParam.hpMax = m_coreParam.ppSrc->GetMaxHp();
      m_coreParam.level = pp->GetLevel();
      diffHP = m_coreParam.hpMax - prevHP;
      info->atk    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_ATK );
      info->def    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_DEF );
      info->sp_atk = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_SPATK );
      info->sp_def = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_SPDEF );
      info->agi    = m_coreParam.ppSrc->GetPower( pml::pokepara::POWER_AGI );

      if( !(m_coreParam.fHensin) )
      {
        m_baseParam.attack     = info->atk;
        m_baseParam.defence    = info->def;
        m_baseParam.sp_attack  = info->sp_atk;
        m_baseParam.sp_defence = info->sp_def;
        m_baseParam.agility    = info->agi;
      }

      info->level  = m_coreParam.level;
      info->hp     = diffHP;
      info->atk    -= atk;
      info->def    -= def;
      info->sp_atk -= sp_atk;
      info->sp_def -= sp_def;
      info->agi    -= agi;

      m_coreParam.hp += diffHP;
      pp->SetHp( m_coreParam.hp );

      wazaWork_ReflectToPP();

      pp->EndFastMode( fFastMode );

      *expRest -= expAdd;

      this->PERMFLAG_Set( PERMFLAG_LEVELUP );
      return true;
    }
    else
    {
      m_coreParam.exp = expSum;
      pp->SetExp( m_coreParam.exp );
    }
  }

  if( info ){
    gfl2::std::MemClear( info, sizeof(*info) );
  }
  return false;
}
//=============================================================================================
/**
 * レベル100になるまでに必要な経験値を取得
 *
 * @param   bpp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_POKEPARAM::GetExpMargin( void ) const
{
  return calc::PERSONAL_GetMinExp( m_coreParam.monsno, m_formNo, 100 );
}


//=============================================================================================
/**
 * Srcポケモンデータからパラメータを再構築（レベルアップ、ワザ覚え後）
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_POKEPARAM::ReflectByPP( void )
{
  if( !(m_coreParam.fHensin) )
  {
    // へんしん中でなければパラメータを全て反映
    setupBySrcData( m_coreParam.ppSrc, true, false );
    wazaWork_ReflectFromPP();
  }
  else{
    // へんしん中なら真ワザワークのみ反映
    wazaWork_setupByPP( m_coreParam.ppSrc, false );
  }
}


//=============================================================================================
/**
 * フェイクのPPが有効な状態かどうか判定
 *
 * @param   bpp
 */
//=============================================================================================
bool BTL_POKEPARAM::IsFakeEnable( void ) const
{
  return m_coreParam.fFakeEnable;
}
//=============================================================================================
/**
 * イリュージョン用のPPを無効にする
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_POKEPARAM::FakeDisable( void )
{
  m_coreParam.fFakeEnable = false;
  m_coreParam.ppFake = NULL;
  m_coreParam.fakeViewTargetPokeId = BTL_POKEID_NULL;
}
//=============================================================================================
/**
 * @brief イリュージョン対象ポケモンのIDを取得する
 * @retval BTL_POKEID_NULL  イリュージョンが有効でない場合
 */
//=============================================================================================
u8 BTL_POKEPARAM::GetFakeTargetPokeID( void ) const
{
  return m_coreParam.fakeViewTargetPokeId;
}


//=============================================================================================
/**
 * Srcポケモンデータポインタを差し替える
 *
 * @param   bpp
 * @param   pp
 */
//=============================================================================================
void BTL_POKEPARAM::SetSrcPP( pml::pokepara::PokemonParam* pp )
{
  {
    MonsNo monsno = pp->GetMonsNo();
    GFL_ASSERT_MSG( (monsno == m_coreParam.monsno), "bppMonsNo=%d, ppMonsNo=%d", m_coreParam.monsno, monsno );
  }
  m_coreParam.ppSrc = pp;
}

//---------------------------------------------------------------------------------------------
// 変身用ワーク処理
//---------------------------------------------------------------------------------------------

bool BTL_POKEPARAM::HENSIN_Set( const BTL_POKEPARAM* target )
{
  if( (m_coreParam.fHensin == false)
  &&  (target->m_coreParam.fHensin == false)
  &&  (target->m_migawariHP == 0)
  ){
    static CORE_PARAM  core = {0};
    static WAZA_SET  wazaWork[ PTL_WAZA_MAX ] = {};
    u16    migawariHP;
    u8     bKiaiDame;
    u8     friendship;
    BTL_SICKCONT sickcont_togisumasu;
    int i;

    // 不変部分をバックアップ
    gfl2::std::MemCopy( m_waza, wazaWork, sizeof(wazaWork) );
    core = m_coreParam;
    migawariHP = m_migawariHP;
    friendship = m_friendship;

    // 相手の気合いだめフラグをコピーする
    bKiaiDame  = target->CONTFLAG_Get( CONTFLG_KIAIDAME );
    // 相手の「とぎすます」状態をコピーする
    sickcont_togisumasu = target->GetSickCont( pml::wazadata::WAZASICK_TOGISUMASU );

    // 丸ごとコピー
    this->henshinCopyFrom( *target );

    // フラグクリア
    flgbuf_clear( m_turnFlag, sizeof(m_turnFlag) );
    flgbuf_clear( m_contFlag, sizeof(m_contFlag) );


    // 不変部分書き戻し
    m_coreParam = core;
    m_migawariHP = migawariHP;
    gfl2::std::MemCopy( wazaWork, m_waza, sizeof(wazaWork) );
    m_friendship = friendship;

    if( bKiaiDame ){
      this->CONTFLAG_Set( CONTFLG_KIAIDAME );
    }
    this->SetWazaSick( pml::wazadata::WAZASICK_TOGISUMASU, sickcont_togisumasu ); 


    // 仮ワザワークを全書き換え（MAXPPは最大5）
    for(i=0; i<PTL_WAZA_MAX; ++i)
    {
      wazaWork_UpdateNumber( &m_waza[i], target->m_waza[i].surface.number, 5, false );
    }


    m_appearedTurn = TURNCOUNT_NULL;
    m_turnCount = 0;
    m_prevSelectWazaID = WAZANO_NULL;
    m_prevActWazaID = WAZANO_NULL;
    m_isPrevActWazaZenryoku = false;
    m_prevWazaType = POKETYPE_NULL;
    m_wazaContCounter = 0;

    m_coreParam.fHensin = true;

    return true;
  }

  return false;
}

/**
 *
 */
void BTL_POKEPARAM::henshinCopyFrom( const BTL_POKEPARAM& src )
{
  m_baseParam = src.m_baseParam;
  m_varyParam = src.m_varyParam;

  m_tokusei                = src.m_tokusei;
  m_weight                 = src.m_weight;
  m_wazaCnt                = src.m_wazaCnt;
  m_formNo                 = src.m_formNo;
  m_friendship             = src.m_friendship;
  m_criticalRank           = src.m_criticalRank;
  m_usedWazaCount          = src.m_usedWazaCount;
  m_prevWazaType           = src.m_prevWazaType;
  m_turnCount              = src.m_turnCount;
  m_appearedTurn           = src.m_appearedTurn;
  m_wazaContCounter        = src.m_wazaContCounter;
  m_prevTargetPos          = src.m_prevTargetPos;
  m_prevActWazaID          = src.m_prevActWazaID;
  m_isPrevActWazaZenryoku  = src.m_isPrevActWazaZenryoku;
  m_prevSelectWazaID       = src.m_prevSelectWazaID;
  m_prevDamagedWaza        = src.m_prevDamagedWaza;
  m_dmgrecTurnPtr          = src.m_dmgrecTurnPtr;
  m_dmgrecPtr              = src.m_dmgrecPtr;
  m_migawariHP             = src.m_migawariHP;
  m_combiWazaID            = src.m_combiWazaID;
  m_combiPokeID            = src.m_combiPokeID;

  gfl2::std::MemCopy( src.m_waza, m_waza, sizeof(m_waza) );
  gfl2::std::MemCopy( src.m_turnFlag, m_turnFlag, sizeof(m_turnFlag) );
  gfl2::std::MemCopy( src.m_contFlag, m_contFlag, sizeof(m_contFlag) );
  gfl2::std::MemCopy( src.m_counter, m_counter, sizeof(m_counter) );
  gfl2::std::MemCopy( src.m_wazaDamageRec, m_wazaDamageRec, sizeof(m_wazaDamageRec) );
  gfl2::std::MemCopy( src.m_dmgrecCount, m_dmgrecCount, sizeof(m_dmgrecCount) );
}


bool BTL_POKEPARAM::HENSIN_Check( void ) const
{
  return m_coreParam.fHensin;
}

/**
 * @brief メガシンカしたポケモンに「へんしん」しているかチェックする
 * @retval true   メガシンカしたポケモンに「へんしん」している
 * @retvla false  メガシンカしたポケモンに「へんしん」していない
 */
bool BTL_POKEPARAM::HENSIN_Check_Mega( void ) const
{
  if( !this->HENSIN_Check() )
  {
    return false;
  }

  MonsNo view_monsno = static_cast<MonsNo>( this->GetHenshinMonsNo() );
  FormNo view_formno = this->GetFormNo();
  return pml::personal::CheckMegaForm( view_monsno, view_formno );
}

/**
 * @brief ゲンシカイキしたポケモンに「へんしん」しているかチェックする
 * @retval true   ゲンシカイキしたポケモンに「へんしん」している
 * @retvla false  ゲンシカイキしたポケモンに「へんしん」していない
 */
bool BTL_POKEPARAM::HENSIN_Check_GensiKaiki( void ) const
{
  if( !this->HENSIN_Check() )
  {
    return false;
  }

  MonsNo view_monsno = static_cast<MonsNo>( this->GetHenshinMonsNo() );
  FormNo view_formno = this->GetFormNo();
  return pml::personal::CheckGensiKaiki( view_monsno, view_formno );
}

/**
* @brief ウルトラバーストしたポケモンに「へんしん」しているかチェックする
* @retval true   ウルトラバーストしたポケモンに「へんしん」している
* @retvla false  ウルトラバーストしたポケモンに「へんしん」していない
*/
bool BTL_POKEPARAM::HENSIN_Check_UltraBurst(void) const
{
  if (!this->HENSIN_Check())
  {
    return false;
  }

  MonsNo view_monsno = static_cast<MonsNo>(this->GetHenshinMonsNo());
  FormNo view_formno = this->GetFormNo();
  return pml::personal::CheckUltraBurst(view_monsno, view_formno);
}

//---------------------------------------------------------------------------------------------
// みがわり関連
//---------------------------------------------------------------------------------------------

void BTL_POKEPARAM::MIGAWARI_Create( u16 migawariHP )
{
  GFL_ASSERT(m_migawariHP==0);
  m_migawariHP = migawariHP;
  CureWazaSick( pml::wazadata::WAZASICK_BIND );
}

void BTL_POKEPARAM::MIGAWARI_Delete( void )
{
  m_migawariHP = 0;
}
bool BTL_POKEPARAM::MIGAWARI_IsExist( void ) const
{
  return m_migawariHP != 0;
}
u32 BTL_POKEPARAM::MIGAWARI_GetHP( void ) const
{
  return m_migawariHP;
}
/**
 *  ダメージ加算
 *
 *  @param  [io] damage  与えるダメージ値 -> 与えたダメージ値
 *
 *  @retval  破壊されたらtrueが返る
 */
bool BTL_POKEPARAM::MIGAWARI_AddDamage( u16* damage )
{
  GFL_ASSERT(m_migawariHP!=0);

  if( m_migawariHP <= (*damage) ){
    (*damage) = m_migawariHP;
    m_migawariHP = 0;
    return true;
  }else{
    m_migawariHP -= (*damage);
    return false;
  }
}

//---------------------------------------------------------------------------------------------
// 対面記録関連
//---------------------------------------------------------------------------------------------

//=============================================================================================
/**
 * 対面記録セット
 *
 * @param   bpp
 * @param   pokeID    対面したポケモンID
 */
//=============================================================================================
void BTL_POKEPARAM::CONFRONT_REC_Set( u8 pokeID )
{
  u32 i;
  for(i=0; i<m_coreParam.confrontRecCount; ++i)
  {
    if( m_coreParam.confrontRec[i] == pokeID ){
      return;
    }
  }
  if( i < GFL_NELEMS(m_coreParam.confrontRec) ){
    BTL_Printf("ポケ[%d]が[%d]と対面した\n", m_coreParam.myID, pokeID);
    m_coreParam.confrontRec[i] = pokeID;
    m_coreParam.confrontRecCount++;
  }
}
//=============================================================================================
/**
 * 対面記録の件数取得
 *
 * @param   bpp
 *
 * @retval  u8    記録件数
 */
//=============================================================================================
u8 BTL_POKEPARAM::CONFRONT_REC_GetCount( void ) const
{
  return m_coreParam.confrontRecCount;
}
//=============================================================================================
/**
 * 対面記録されているポケモンIDを取得
 *
 * @param   bpp
 * @param   idx   インデックス（0 <= idx < BPP_CONFRONT_REC_GetCount() )
 *
 * @retval  u8    ポケモンID
 */
//=============================================================================================
u8 BTL_POKEPARAM::CONFRONT_REC_GetPokeID( u8 idx ) const
{
  if( idx < m_coreParam.confrontRecCount ){
    return m_coreParam.confrontRec[ idx ];
  }
  return BTL_POKEID_NULL;
}
//=============================================================================================
/**
 * 対面記録履歴に指定IDのポケモンが存在しているか判定
 *
 * @param   pokeID    ポケモンＩＤ指定
 *
 * @retval  bool    存在していたら true
 */
//=============================================================================================
bool BTL_POKEPARAM::CONFRONT_REC_IsMatch( u8 pokeID ) const
{
  for(u8 i=0; i<m_coreParam.confrontRecCount; ++i)
  {
    if( m_coreParam.confrontRec[i] == pokeID ){
      return true;
    }
  }
  return false;
}
//=============================================================================================
/**
 * 捕獲された時のボールアイテムIDをセットする
 *
 * @param   bpp
 * @param   ballItemID
 */
//=============================================================================================
void BTL_POKEPARAM::SetCaptureBallID( u16 ballItemID ) const
{
  item::BALL_ID  ballID = item::ITEM_GetBallID( ballItemID );
  if( ballID == item::BALLID_NULL ){
    ballID = item::BALLID_MONSUTAABOORU;
  }

  m_coreParam.ppSrc->SetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL, ballID );
  
  // フレンドボールで捕まえたなら、なつき度を変更する
  if( ballID == item::BALLID_HURENDOBOORU )
  {
    m_coreParam.ppSrc->SetFamiliarity( Capture::FRIENDBALL_FAMILIARITY );
  }
}


//---------------------------------------------------------------------------------------------
// 合体ワザ関連
//---------------------------------------------------------------------------------------------

//=============================================================================================
/**
 * 合体ワザ対象（後発）としてのパラメータセット
 *
 * @param   bpp
 * @param   combiPokeID     先発ポケモンID
 * @param   combiUsedWaza   先発ポケモンが使ったワザID
 */
//=============================================================================================
void BTL_POKEPARAM::CombiWaza_SetParam( u8 combiPokeID, WazaID combiUsedWaza )
{
  m_combiPokeID = combiPokeID;
  m_combiWazaID = combiUsedWaza;
}
//=============================================================================================
/**
 * 合体ワザパラメータ取得
 *
 * @param   bpp
 * @param   combiPokeID     [out]
 * @param   combiUsedWaza   [out]
 *
 * @retval  bool    セットされたパラメータがある場合、true
 */
//=============================================================================================
bool BTL_POKEPARAM::CombiWaza_GetParam( u8* combiPokeID, WazaID* combiUsedWaza ) const
{
  if( m_combiPokeID != BTL_POKEID_NULL )
  {
    *combiPokeID = m_combiPokeID;
    *combiUsedWaza = m_combiWazaID;
    return true;
  }
  return false;
}
//=============================================================================================
/**
 * 合体ワザパラメータが設定されているかチェック
 *
 * @param   bpp
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_POKEPARAM::CombiWaza_IsSetParam( void ) const
{
  return m_combiPokeID != BTL_POKEID_NULL;
}

//=============================================================================================
/**
 * 合体ワザパラメータクリア
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_POKEPARAM::CombiWaza_ClearParam( void )
{
  if( m_combiPokeID != BTL_POKEID_NULL)
  {
    m_combiPokeID = BTL_POKEID_NULL;
    m_combiWazaID = WAZANO_NULL;
  }
}

//---------------------------------------------------------------------------------------------
// とくせい・アイテム参照（いえき、さしおさえ等の影響を考慮して、直接参照しないようにするため）
//---------------------------------------------------------------------------------------------
bool BTL_POKEPARAM::IsMatchTokusei( TokuseiNo tokusei ) const
{
  if( CheckSick(pml::wazadata::WAZASICK_IEKI) ){
    return false;
  }
  return (m_tokusei == tokusei);
}


#if 0
bool BPP_IsHP41( const BTL_POKEPARAM* bpp )
{
  if( m_coreParam.hp == 41 )
  {
    BTL_PRINT("ID=%d, HP==41です\n", m_coreParam.myID);
    return true;
  }
  else
  {
    BTL_PRINT("ID=%d, HP41じゃないよ %dだよ\n", m_coreParam.myID, m_coreParam.hp);
    return false;
  }
}
#endif

//=============================================================================================
/**
 * メガ進化させる
 */
//=============================================================================================
void BTL_POKEPARAM::MegaEvo( void )
{
  this->ReflectToPP( false );

  pml::pokepara::PokemonParam* ppSrc = reinterpret_cast<pml::pokepara::PokemonParam*>(m_coreParam.ppSrc);

  ppSrc->MegaEvolve();
  setupBySrcData( ppSrc, true, true );
}

/**
 * @brief メガ進化できる状態か？（ポケモンナンバーと、合致するアイテム所持のチェック）
 * @retval true   メガ進化できる
 * @retval false  メガ進化できない
 */
bool BTL_POKEPARAM::IsAbleToMegaEvo( void ) const
{
  return m_coreParam.ppSrc->CanMegaEvolve();
}

/**
 * @brief メガ進化した状態か？
 * @retval true   メガ進化している
 * @retval false  メガ進化していない
 */
bool BTL_POKEPARAM::IsMegaEvolved( void ) const
{
  return m_coreParam.ppSrc->IsMegaEvolved();
}

//@バトル変更箇所
//=============================================================================================
//
// ゲンシカイキ
//
//=============================================================================================
void  BTL_POKEPARAM::GensiKaiki(u8 formNo)
{
  //処理がメガシンカに近いのでフォルムチェンジから独立。
  this->ReflectToPP( false );

  pml::pokepara::PokemonParam* pp = reinterpret_cast<pml::pokepara::PokemonParam*>(m_coreParam.ppSrc);

  pp->ChangeFormNo( formNo );
  setupBySrcData( pp, true, true );
}

/**
 *  ゲンシカイキした状態か？
 */
bool BTL_POKEPARAM::IsGensiKaiki(void ) const
{
  MonsNo monsno = static_cast<MonsNo>( GetMonsNo() );
  FormNo formno = GetFormNo();
  return pml::personal::CheckGensiKaiki( monsno, formno );
}

//@momijiバトル変更箇所
//=============================================================================================
//
// ウルトラバースト
//
//=============================================================================================
void  BTL_POKEPARAM::UltraBurst()
{
  if (!this->IsAbleToUltraBurst())
  {
    GFL_ASSERT_STOP(0);
  }

  //処理がメガシンカに近いのでフォルムチェンジから独立。
  this->ReflectToPP(false);

  pml::pokepara::PokemonParam* pp = reinterpret_cast<pml::pokepara::PokemonParam*>(m_coreParam.ppSrc);

  pp->ChangeFormNo(FORMNO_PURIZUMU_FORM_N);
  setupBySrcData(pp, true, true);
}

/**
*  ウルトラバーストした状態か？
*/
bool BTL_POKEPARAM::IsUltraBurst(void) const
{
  MonsNo monsno = static_cast<MonsNo>(GetMonsNo());
  FormNo formno = GetFormNo();
  return pml::personal::CheckUltraBurst(monsno, formno);
}
/**
*  ウルトラバーストを元に戻す
*/
void BTL_POKEPARAM::ResetUltraBurst()
{
  if (IsUltraBurst())
  {
    m_formNo = m_coreParam.defaultFormNo;
    m_tokusei = m_coreParam.defaultTokusei;
    m_coreParam.ppSrc->ChangeFormNo(m_formNo);
  }
}

/**
* @brief ウルトラバーストできる状態か？（ポケモンナンバーと、合致するアイテム所持のチェック）
* @retval true   ウルトラバーストできる
* @retval false  ウルトラバーストできない
*/
bool BTL_POKEPARAM::IsAbleToUltraBurst(void) const
{
  FormNo current_form = m_coreParam.ppSrc->GetFormNo();
  u16 itemId = m_coreParam.ppSrc->GetItem();
  
  if ((current_form == FORMNO_PURIZUMU_FORM_S || current_form == FORMNO_PURIZUMU_FORM_L) && 
      itemId == ITEM_NEKUROZUMAz_1)
  {
    return true;
  }

  return false;
}

//=============================================================================================
//
//  努力値関連
//
//=============================================================================================

//===========================================================================
/**
 *  ポケルス感染しているか判定
 */
 //===========================================================================
bool BTL_POKEPARAM::HavePokerus( void ) const
{
  return m_doryokuParam.bPokerus;
}

//===========================================================================
/**
 * ポケモンに努力値を加算する（予約）
 *
 * @param   id
 * @param   value
 */
//===========================================================================
void BTL_POKEPARAM::AddEffortPower( pml::pokepara::PowerID id, u8 value )
{
  doryoku_AddPower( &m_doryokuParam, id, value );
}

/**
 * 努力値ワークを初期化
 */
void BTL_POKEPARAM::doryoku_InitParam( DORYOKU_PARAM* work, pml::pokepara::PokemonParam* pp )
{
  work->srcSum = 0;

  for(u32 i=0; i<GFL_NELEMS(calc::PokePowerIDTable); ++i)
  {
    u8* ptr = doryoku_ParamIDtoValueAdrs( work, calc::PokePowerIDTable[i] );
    if( ptr ){
      *ptr = pp->GetEffortPower( calc::PokePowerIDTable[i] );
      work->srcSum += (*ptr);
    }
  }
  work->bPokerus = pp->HavePokerusUntilNow();
  work->bModified = false;
}
/**
 *  努力値ワークに加算する
 */
void BTL_POKEPARAM::doryoku_AddPower( DORYOKU_PARAM* work,  pml::pokepara::PowerID powID, u8 value )
{
  if( value == 0 ){
    return;
  }
  if( work->srcSum >= pml::MAX_TOTAL_EFFORT_POWER ){
    return;
  }

  u32 sum = work->srcSum + value;
  if( sum > pml::MAX_TOTAL_EFFORT_POWER ){
    value -= ( sum - pml::MAX_TOTAL_EFFORT_POWER );
  }

  if( value )
  {
    u8* ptr = doryoku_ParamIDtoValueAdrs( work, powID );
    if( ptr )
    {
      sum = (*ptr) + value;
      if( sum > pml::MAX_EFFORT_POWER ){
        value -= (sum - pml::MAX_EFFORT_POWER);
      }
      (*ptr) += value;
      work->srcSum += value;
      work->bModified = true;
      BTL_PRINT("努力値[%d] に値を %d 加算し、%d になる、合計値は%dになる\n", powID, value, (*ptr), work->srcSum );
    }
//    m_coreParam.ppSrc->AddEffortPower( id, value );
  }
}

/**
 *  努力値ワークの内容を PP に反映させる
 */
void BTL_POKEPARAM::doryoku_PutToPP( DORYOKU_PARAM* work, pml::pokepara::PokemonParam* pp )
{
  if( work->bModified )
  {
    bool bFastMode = pp->StartFastMode();

    for(u32 i=0; i<GFL_NELEMS(calc::PokePowerIDTable); ++i)
    {
      u8* ptr = doryoku_ParamIDtoValueAdrs( work, calc::PokePowerIDTable[i] );
      if( ptr ){
        pp->ChangeEffortPower( calc::PokePowerIDTable[i], *ptr );
        BTL_PRINT("努力値[%d]を %d に書き換え\n", i, *ptr );
      }
    }
    work->bModified = false;

    pp->EndFastMode( bFastMode );
  }
}

u8* BTL_POKEPARAM::doryoku_ParamIDtoValueAdrs( DORYOKU_PARAM* work, pml::pokepara::PowerID powID) const
{
  switch( powID ){
  case pml::pokepara::POWER_HP:     return &(work->srcHp);
  case pml::pokepara::POWER_ATK:    return &(work->srcPow);
  case pml::pokepara::POWER_DEF:    return &(work->srcDef);
  case pml::pokepara::POWER_AGI:    return &(work->srcAgi);
  case pml::pokepara::POWER_SPATK:  return &(work->srcSpPow);
  case pml::pokepara::POWER_SPDEF:  return &(work->srcSpDef);

  }
  GFL_ASSERT(0);
  return NULL;
}


GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif
