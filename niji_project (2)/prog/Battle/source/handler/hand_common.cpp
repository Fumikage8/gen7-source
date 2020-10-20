//=============================================================================================
/**
 * @file  hand_common.cpp
 * @brief ポケモンXY バトルシステム イベントハンドラ共有関数群
 * @author  taya
 *
 * @date  2011.02.03  作成
 */
//=============================================================================================

#include "../btl_common.h"
#include "../btl_tables.h"
#include "./hand_common.h"

GFL_NAMESPACE_BEGIN(btl)
/**
 *  複数体ターゲットポケモンIDが設定されているハンドラで、指定IDのポケモンがターゲットに含まれるか判定
 */
bool HandCommon_CheckTargetPokeID( u8 pokeID )
{
  u32 i, target_cnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  BTL_PRINT("ターゲット%d対、pokeID:%d がいるか？\n", target_cnt, pokeID);
  for(i=0; i<target_cnt; ++i)
  {
    if( EVENTVAR_GetValue(static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i)) == pokeID ){
      BTL_PRINT("いました\n");
      return true;
    }
  }
  BTL_PRINT("いまへん\n");
  return false;
}
/**
 * @brief 複数体ターゲットポケモンIDが設定されているハンドラで、指定した陣営のポケモンがターゲットに含まれるか判定
 * @param side  チェック対象の陣営
 * @retval true   指定した陣営のポケモンが含まれている
 * @retval false  指定した陣営のポケモンが含まれていない
 */
bool HandCommon_CheckTargetSide( ServerFlow* flowWk, BtlSide side )
{
  const u32 targetCount = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );

  for( u32 i=0; i<targetCount; ++i )
  {
    u8 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>( BTL_EVAR_POKEID_TARGET1+i ) );
    BtlSide targetSide = flowWk->Hnd_GetMainModule()->PokeIDtoSide( targetPokeID );
    if( targetSide == side )
    {
      return true;
    }
  }

  return false;
}
/**
 *  指定ポケモンの行動順が最後かどうか判定
 */
bool HandCommon_IsPokeOrderLast( ServerFlow* flowWk, u8 pokeID )
{
  if( flowWk->Hnd_IsMyActOrderLast(pokeID) )
  {
    return true;
  }
  return false;
}
/**
 *  ねこだまし失敗判定（場に出た直後の行動かどうか）
 */
bool HandCommon_IsNekodamashiFail( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  return bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_ACTION_DONE );
}


/**
 *  分離できない特定のポケモンとアイテムの組み合わせかどうか判定
 */
bool HandCommon_IsUnbreakablePokeItem( ServerFlow* flowWk, u16 monsno, u16 itemID )
{
  switch( monsno ){
  case MONSNO_GIRATHINA:
    if( itemID == ITEM_HAKKINDAMA ){  // ギラティナのはっきんだまはNG
      return true;
    }
    break;
  case MONSNO_ARUSEUSU:
    if( tables::IsMatchAruseusPlate(itemID) ){  // アルセウスのプレートはNG
      return true;
    }
    break;
  case MONSNO_GURIPUSU2:
    if( tables::IsMatchGuripusu2Chip(itemID) ){  // グリプス２のチップはNG
      return true;
    }
    break;
  case MONSNO_CN_INSEKUTA:
    if( tables::IsMatchInsectaCasette(itemID) ){  // インセクタのカセットはNG
      return true;
    }
    break;
  //@バトル変更箇所(カイグラアイテム対応)
  case MONSNO_KAIOOGA:
    if( itemID == ITEM_AIIRONOTAMA ){  // カイオーガののあいいろのたまはNG
      return true;
    }
    break;
  case MONSNO_GURAADON:
    if( itemID == ITEM_BENIIRONOTAMA ){  // グラードンののべにいろのたまはNG
      return true;
    }
    break;

  default:
    // メガ進化可能な組み合わせなら NG
    //@バトル変更箇所 レックウザは何持っていても進化できるし、何も持ってなくても進化できるのではじく。
    if( flowWk->Hnd_IsMegaEvolveItem((MonsNo)(monsno), itemID) &&
        monsno != MONSNO_REKKUUZA )
    {
      return true;
    }
    // オーブは分離できない 
    if( ::item::ITEM_CheckPiece( itemID ) ) {
      return true;
    }

  }
  return false;
}


/**
 *  絶対にアイテムを書き換えてはいけないポケモン判定
 */
bool HandCommon_CheckCantChangeItemPoke( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  u16 monsno = bpp->GetMonsNo();
  u16 itemID = bpp->GetItem();

  if( HandCommon_IsUnbreakablePokeItem(flowWk, monsno, itemID) ){
    return true;
  }

  return false;
}

/**
 *  ルール上、相手のどうぐを失わせる行為が禁止されているポケのチェック（野生戦で相手側チェック）
 */
bool HandCommon_CheckCantStealWildPoke( ServerFlow* flowWk, u8 attackPokeID )
{
  // 野生戦で相手の場合をチェック
  if( flowWk->Hnd_GetCompetitor() == BTL_COMPETITOR_WILD )
  {
    u8 clientID = MainModule::PokeIDtoClientID( attackPokeID );
    if( (clientID == BTL_CLIENT_ENEMY1) || (clientID == BTL_CLIENT_ENEMY2) ){
      return true;
    }
  }
  return false;
}
/**
 *  ルール上、相手のどうぐを失わせる行為が禁止されているポケのチェック
 */
bool HandCommon_CheckCantStealPoke( ServerFlow* flowWk, u8 attackPokeID, u8 targetPokeID )
{
  // 野生戦で相手の場合をチェック
  if( HandCommon_CheckCantStealWildPoke(flowWk, attackPokeID) )
  {
    return true;
  }
  // ギラティナ・アルセウス・インセクタのチェック
  if( HandCommon_CheckCantChangeItemPoke(flowWk, targetPokeID) )
  {
    return true;
  }

  {
    const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( attackPokeID );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    u16 atkMonsNo = attacker->GetMonsNo();
    u16 tgtItemID = target->GetItem();
    if( HandCommon_IsUnbreakablePokeItem(flowWk, atkMonsNo, tgtItemID) ){
      return true;
    }
  }
  return false;
}
/**
 *  マジックコート（相手が使うサイドエフェクトでマジックコート対象ワザは失敗
 */
void HandCommon_MagicCoat_CheckSideEffWaza( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );

  // 自分自身が画面から消えていたら失敗
  if( flowWk->Hnd_IsTameHidePoke(pokeID) ){
    return;
  }

  u8 wazaUserPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

  if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(wazaUserPokeID, pokeID) )
  {
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));

    if( (WAZADATA_GetCategory(waza) == pml::wazadata::CATEGORY_SIDE_EFFECT)
    &&  (WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_MagicCoat))
    ){
      if( EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_NO_REACTION) ){
        flowWk->Hnd_AddMagicCoatAction( pokeID, wazaUserPokeID );
      }
    }
  }
}
/**
 *  マジックコート（自分がワザ対象なら無効化）
 */
void HandCommon_MagicCoat_Wait( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MAGICCOAT_FLAG) == false)
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsWazaHide() )
    {
      WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));

      if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_MagicCoat) )
      {
        if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
        {
          // マジックコート反応呼び出し
          u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
          flowWk->Hnd_AddMagicCoatAction( pokeID, atkPokeID );

          // この時点でリアクションなし
          EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
        }
      }
    }
  }
}

/**
 *  マジックコート（跳ね返し確定反応）
 */
void HandCommon_MagicCoat_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID) )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsWazaHide() )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>
                                          (flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MagicCoatExe );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_AddArg( &param->str, EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
      flowWk->HANDEX_Pop( param );
    }
  }
}

/**
 *  ワザ基本威力値をn倍に
 */
void HandCommon_MulWazaBasePower( u32 ratio )
{
  u32 pow = EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER ) * ratio;
  EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
}

//------------------------------------------------
/**
 * 逃げ計算スキップ共通（にげあし・けむりだま）
 */
//------------------------------------------------
void HandCommon_NigeruCalcSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );

  u8 escapePokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(escapePokeID, pokeID) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
/**
 *  逃げ特殊メッセージ表示予約
 */
bool HandCommon_CheckNigeruExMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );

  u8 escapePokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(escapePokeID, pokeID) )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, true) )
    {
      return true;
    }
  }
  return false;
}


// 自分が他ポケモンに対し、ひん死になるダメージを与えたか判定（じしんかじょう、とどめばり @ BTL_EVENT_DAMAGEPROC_END_HIT_REAL）
u32 HandCommon_GetKillCount( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で

  u32 count = 0;
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 死んだポケいるなら攻撃ランクアップ
    const BTL_POKEPARAM* bpp;
    u32 i, targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    for(i=0; i<targetCnt; ++i)
    {
      bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i)) );
      if( bpp->IsDead() )
      {
        ++count;
      }
    }
  }
  return count;
}

// 天候ダメージリアクション（ダメージ無効化する：とくせい「ぼうじん」、アイテム「ぼうじんゴーグル」用
bool HandCommon_Boujin_WeatherReaction( u8 pokeID )
{
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_DAMAGE) > 0)
  ){
    return EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
  return false;
}
// ワザ無効化チェック：とくせい「ぼうじん」、アイテム「ぼうじんゴーグル」用
bool HandCommon_Boujin_WazaNoEffect( u8 pokeID )
{
  // しびれごな、ねむりごな、どくのこな、いかりのこな、キノコのほうし、ふんじん が無効
  WazaNo waza = static_cast<WazaNo>(EVENTVAR_GetValue(BTL_EVAR_WAZAID));
  if( tables::IsBoujinGuardWaza(waza) )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
    {
      return true;
    }
  }
  return false;
}

// めざめるパワーのワザタイプを取得
pml::PokeType HandCommon_GetMezapaType( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp )
  {
    const pml::pokepara::PokemonParam* pp = bpp->GetSrcData();
    return pp->GetMezapaType();
  }
  GFL_ASSERT(0);
  return POKETYPE_NORMAL;
}



//----------------------------------------------------------------------------------
/**
 * 状態異常を受け付けない共通処理（BTL_EVENT_ADDSICK_CHECKFAIL時）
 *
 * @param   flowWk
 * @param   pokeID    状態異常を受けようとしているポケモンID
 * @param   sick      状態異常コード
 *
 * @retval  bool      この処理により受け付け拒否した場合true
 */
//----------------------------------------------------------------------------------
bool HandCommon_GuardWazaSick( ServerFlow* flowWk, u8 pokeID, WazaSick guardSick )
{
  GFL_UNUSED(flowWk);
  // くらう側が
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // くらう病気が指定通り
    WazaSick  attackSick = static_cast<WazaSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
    if( guardSick == attackSick ){
      return EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
  return false;
}
//===============================================================================
/**
 * 「どろぼう」等 ダメージプロセス開始処理
 */
//================================================================================
void HandCommon_DorobouStart( ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    work[0] = (bpp->GetItem() != ITEM_DUMMY_DATA);  // work[0] を処理開始時点のアイテム所持フラグとしておく
  }
}

//=============================================================================
/**
 * 「どろぼう」の追加効果を発動できるか判定
 */
//=============================================================================
bool HandCommon_IsDorobouEnable( ServerFlow* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( (bpp->GetItem() == ITEM_DUMMY_DATA)
  &&  (work[0] == false)
  ){
    return true;
  }
  return false;
}


//=============================================================================================
/**
 * 指定ポケモンIDを素早さ順にソートする
 *
 * @param       flowWk
 * @param[io]   targetPokeIDAry
 * @param       targetCnt
 * @param       myPokeID
 * @param       bDownFriendPriority   味方のプライオリティを下げる（敵より確実に下になる）
 */
//=============================================================================================
void  HandCommon_SortTargetByAgility( ServerFlow* flowWk, u8* targetPokeIDAry, uint32_t targetCnt, u8 myPokeID, bool bDownFriendPriority )
{
  static u16  rank[ BTL_FRONT_POKE_MAX ];

  if( targetCnt >= GFL_NELEMS(rank) ){
    GFL_ASSERT( 0 );
    return;
  }

  for(u32 i=0; i<targetCnt; ++i)
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeIDAry[i] );
    rank[i] = flowWk->Hnd_CalcAgilityRank( bpp, true );
    if( bDownFriendPriority
    &&  flowWk->Hnd_GetMainModule()->IsFriendPokeID(myPokeID, targetPokeIDAry[i]) ){
      rank[i] += BTL_POSIDX_MAX;
    }
  }

  for(u32 i=0; i<targetCnt; ++i)
  {
    for(u32 j=i+1; j<targetCnt; ++j)
    {
      if( rank[j] < rank[i] )
      {
        u16  tmp_rank = rank[ i ];
        rank[ i ] = rank[ j ];
        rank[ j ] = tmp_rank;

        u8 tmpID = targetPokeIDAry[ i ];
        targetPokeIDAry[ i ] = targetPokeIDAry[ j ];
        targetPokeIDAry[ j ] = tmpID;
      }
    }
  }
}


//=============================================================================================
/**
 * ポケモンID をキーにして状態異常を取得する
 *
 * @param     flowWk
 * @param     pokeID
 *
 * @retval    pml::pokepara::Sick     状態異常コード
 */
//=============================================================================================
pml::pokepara::Sick  HandCommon_CheckPokeSick( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp )
  {
    return bpp->GetPokeSick();
  }
  return pml::pokepara::SICK_NULL;
}

//=============================================================================================
/**
 * ワザがプラスの効果しかない「シンプルエフェクト」タイプかどうかを判定
 * @param waza
 * @return  プラス効果しかない「シンプルエフェクト」なら true
 */
//=============================================================================================
bool HandCommon_IsSimpleEffectOnlyAdvantage( WazaID waza )
{
  if( WAZADATA_GetCategory(waza) != pml::wazadata::CATEGORY_SIMPLE_EFFECT ){
    return false;
  }

  u32 eff_count = WAZADATA_GetRankEffectCount( waza );
  if( eff_count == 0 ){
    return false;
  }

  for(u32 i=0; i<eff_count; ++i)
  {
    int eff_value;
    WAZADATA_GetRankEffect( waza, i, &eff_value );
    if( eff_value < 0 ){
      return false;
    }
  }


  return true;
}




//=============================================================================================
/**
 * @brief 技「このゆびとまれ」による、技ターゲットの誘導
 *
 * @param myHandle
 * @param flowWk
 * @param konoyubiPokeId  「このゆびとまれ」を使用しているポケモンのID
 * @param temptPriority   ターゲット誘導の優先度
 * @param temptCause      ターゲット誘導の要因
 * @param factorWork      イベントファクターの作業領域
 */
//=============================================================================================
void HandCommon_KonoyubiTomare_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 konoyubiPokeId, TemptTargetPriority temptPriority, TemptTargetCause temptCause, int* factorWork )
{
  GFL_UNUSED( factorWork );

  if( flowWk->Hnd_GetRule() == BTL_RULE_SINGLE ) {
    return;
  }

  u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

  // 攻撃するポケモンが味方なら、無効
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID( konoyubiPokeId, atkPokeID ) ) {
    return; 
  }

  // 技「フリーフォール」には無効
  if( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) == WAZANO_HURIIFOORU ) { 
    return; 
  }

  // 「このゆびとまれ」中のポケモンがフリーフォールされているなら、無効
  const BTL_POKEPARAM* konoyubiPoke = flowWk->Hnd_GetPokeParam( konoyubiPokeId );
  if( konoyubiPoke->CheckSick( pml::wazadata::WAZASICK_FREEFALL ) ) { 
    return; 
  }

  // メンバー入れ替え時の割り込みアクション中なら、無効
  if( flowWk->Hnd_IsMemberOutIntr() ) {
    return; 
  }

  HandCommon_TemptTarget( konoyubiPokeId, temptPriority, temptCause );
}

//=============================================================================================
/**
 * @brief 技ターゲットの誘導
 *
 * @param temptPokeId     誘導先ポケモンのＩＤ
 * @param temptPriority   ターゲット誘導の優先度
 * @param temptCause      ターゲット誘導の要因
 */
//=============================================================================================
void HandCommon_TemptTarget( u8 temptPokeId, TemptTargetPriority temptPriority, TemptTargetCause temptCause )
{
  // 現時点でのターゲット誘導優先度より高くないなら、無効
  TemptTargetPriority currentTemptPriority = static_cast<TemptTargetPriority>( EVENTVAR_GetValue( BTL_EVAR_TEMPT_TARGET_PRIORITY ) );
  if( temptPriority <= currentTemptPriority )
  {
    return;
  }

  EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, temptPokeId );
  EVENTVAR_RewriteValue( BTL_EVAR_TEMPT_TARGET_PRIORITY, temptPriority );
  EVENTVAR_RewriteValue( BTL_EVAR_TEMPT_TARGET_CAUSE, temptCause );
}

//=============================================================================================
/**
 * @brief 特性「かたやぶり」のスキップチェックハンドラ
 */
//=============================================================================================
bool HandCommon_Katayaburi_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(eventType);
  GFL_UNUSED(pokeID);
  if( factorType == BTL_EVENT_FACTOR_TOKUSEI )
  {
    TokuseiNo tokusei = static_cast<TokuseiNo>( subID );
    if( tables::IsMatchKatayaburiTarget( tokusei ) )
    {
      return true;
    }
  }
  return false;
}


//=============================================================================================
/**
 * @brief 乱入時に味方が残ってるか判定
 *
 * @param flowWk    サーバーフローハンドラ
 * @param pokeID    対象ポケモンID（野生側）
 * @retval  bool    乱入線であり、pokeID の味方が場に存在している場合に true 
 */
//=============================================================================================
bool HandCommon_IsExistIntrudeFriend( ServerFlow* flowWk, u8 pokeID )
{
  if( flowWk->Hnd_GetRule() != BTL_RULE_INTRUDE ){
    return false;
  }

  BtlPokePos  myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
  if( myPos == BTL_POS_NULL ){
    return false;
  }

  const MainModule* mainModule = flowWk->Hnd_GetMainModule();
  BtlPokePos friendPos = mainModule->GetFriendPokePos( myPos, 1 );
  if( friendPos == BTL_POS_NULL ){
    return false;
  }

  if( flowWk->Hnd_GetExistPokeID(friendPos) == BTL_POKEID_NULL ){
    return false;
  }

  return true;
}

//=============================================================================================
/**
 * @brief  自分が対処すべきイベントかどうか判定（ポケモンID、ワザIDの両チェック）
 *
 * @param[in]  myHandle     イベントハンドラ
 * @param[in]  pokeID_label ポケモンIDを取得するためのラベルナンバー
 * @param[in]  pokeID       自分のポケモンID
 * @return     自分が対処すべきイベントなら true 
 *
 * @detail  従来ならポケモンIDの一致のみを確認すればよかったケースでも、
 *          今作（niji）より、（おどりこ・さいはい効果）１ターン中に複数のワザを出すことが起こりうるようになったため、
 *          張り付いているハンドラが反応すべき処理か否かを判定するために、ワザIDの一致も確認する必要が生じた。
 *          そのようなケースで使うことを想定している。
 */
//=============================================================================================
bool HandCommon_IsMyEvent( BTL_EVENT_FACTOR* myHandle, BtlEvVarLabel pokeID_label, u8 pokeID )
{
  if( EVENTVAR_GetValue(pokeID_label) != pokeID ){
    return false;
  }
  if( EVENTVAR_GetValue(BTL_EVAR_WAZAID) != EVENT_FACTOR_GetSubID(myHandle) ){
    return false;
  }
  return true;
}


GFL_NAMESPACE_END(btl)

