//=============================================================================================
/**
 * @file  btl_action.cpp
 * @brief ポケモンXYバトル プレイヤーが選択した動作内容の受け渡し構造体定義
 * @author  taya
 *
 * @date  2010.12.29  作成
 */
//=============================================================================================
#include "./btl_pokeparam.h"
#include "./btl_ZenryokuWaza.h"
#include "./btl_action.h"

GFL_NAMESPACE_BEGIN(btl)

// たたかうアクション設定
void BTL_ACTION_SetFightParam( BTL_ACTION_PARAM* p, WazaID waza, u8 targetPos, bool isZenryokuWaza )
{
  p->raw = 0;
  p->fight.cmd = BTL_ACTION_FIGHT;
  p->fight.targetPos = targetPos;
  p->fight.waza = waza;
  p->fight.zenryokuWazaFlag = isZenryokuWaza;
}
void BTL_ACTION_ChangeFightTargetPos( BTL_ACTION_PARAM* p, u8 nextTargetPos )
{
  if( p->gen.cmd == BTL_ACTION_FIGHT )
  {
    if( nextTargetPos != BTL_POS_NULL ){
      p->fight.targetPos = nextTargetPos;
    }
  }
}

// たたかうアクション -> ワザ説明モードへ
void BTL_ACTION_FightParamToWazaInfoMode( BTL_ACTION_PARAM* p )
{
  if( p->gen.cmd == BTL_ACTION_FIGHT ){
    p->fight.wazaInfoFlag = true;
  }
}
// ワザ説明モード設定かどうか判定
bool BTL_ACTION_IsWazaInfoMode( const BTL_ACTION_PARAM* p )
{
  if( p->gen.cmd == BTL_ACTION_FIGHT )
  {
    return p->fight.wazaInfoFlag;
  }
  return false;
}
// メガ進化指定されているかどうか判定
bool BTL_ACTION_IsFightWithMegaEvo( const BTL_ACTION_PARAM* p )
{
  if( p->gen.cmd == BTL_ACTION_FIGHT )
  {
    return p->fight.megaEvoFlag;
  }
  return false;
}
// メガ進化指定をオフ
void BTL_ACTION_OffMegaEvo( BTL_ACTION_PARAM* p )
{
  if( p->gen.cmd == BTL_ACTION_FIGHT )
  {
    p->fight.megaEvoFlag = false;
  }
}

// ウルトラバースト指定されているかどうか判定
bool BTL_ACTION_IsFightWithUltraBurst(const BTL_ACTION_PARAM* p)
{
  if (p->gen.cmd == BTL_ACTION_FIGHT)
  {
    return p->fight.urtlaBurstFlag;
  }
  return false;
}
// ウルトラバースト指定をオフ
void BTL_ACTION_OffUltraBurst(BTL_ACTION_PARAM* p)
{
  if (p->gen.cmd == BTL_ACTION_FIGHT)
  {
    p->fight.urtlaBurstFlag = false;
  }
}

/**
 * @brief 技No.取得
 * @param act         取得対象のアクション
 * @param attackPoke  攻撃するポケモンのパラメータ
 *
 * @note 「たたかう」以外のアクションが設定されている場合、WAZANO_NULL を返します。
 * @note オーラ技を使用するアクションの場合、オーラ技の技No.を返します。
 *
 * @caution オーラ技を使用する場合、このメソッドが返す技No.は、
 *            ポケモンパラメータが持つ技スロットに含まれない　ということに注意してください。
 */
WazaNo BTL_ACTION_GetWazaID( const BTL_ACTION_PARAM* act, const BTL_POKEPARAM* attackPoke )
{
  if( act->gen.cmd != BTL_ACTION_FIGHT ) {
    return WAZANO_NULL;
  }

  if( act->fight.zenryokuWazaFlag ) 
  {
    WazaNo wazaNo = static_cast<WazaNo>( act->fight.waza );
    return ZenryokuWaza::GetZenryokuWazaNo( *attackPoke, wazaNo );
  }

  return static_cast<WazaNo>( act->fight.waza );
}

/**
 * @brief  ワザ対象位置を取得
 * @param[in]   act   対象アクション
 *
 * @return      ワザの対象位置
 */
BtlPokePos  BTL_ACTION_GetWazaTargetPos( const BTL_ACTION_PARAM* act )
{
  if( act->gen.cmd != BTL_ACTION_FIGHT ){
    GFL_ASSERT(false);
    return BTL_POS_NULL;
  }
  return act->fight.targetPos;
}


/**
 * @brief 技No.取得
 * @param act         取得対象のアクション
 * @param attackPoke  攻撃するポケモンのパラメータ
 *
 * @note 「たたかう」以外のアクションが設定されている場合、WAZANO_NULL を返します。
 * @note オーラ技を使用するアクションの場合でも、オーラ技に変化する前の技の技No.を返します。
 */
WazaNo BTL_ACTION_GetOriginalWazaID( const BTL_ACTION_PARAM* act )
{
  if( act->gen.cmd != BTL_ACTION_FIGHT ) {
    return WAZANO_NULL;
  }
  return static_cast<WazaNo>( act->fight.waza );
}


/**
 * @brief オーラ技指定がされているか？
 * @param act  チェック対象のアクションパラメータ
 * @retval true   オーラ技指定がされている
 * @retval false  オーラ技指定がされていない
 */
bool BTL_ACTION_IsFightWithZenryokuWaza( const BTL_ACTION_PARAM* act )
{
  if( act->gen.cmd == BTL_ACTION_FIGHT ) {
    return act->fight.zenryokuWazaFlag;
  }
  return false;
}



// アイテムつかうアクション設定
void BTL_ACTION_SetItemParam( BTL_ACTION_PARAM* p, u16 itemNumber, u8 targetIdx, u8 wazaIdx )
{
  p->raw = 0;
  p->item.cmd = BTL_ACTION_ITEM;
  p->item.number = itemNumber;
  p->item.targetIdx = targetIdx;
  p->item.param = wazaIdx;
}



// 入れ替えポケモン選択アクション（選択対象は未定）
void BTL_ACTION_SetChangeBegin( BTL_ACTION_PARAM* p )
{
  p->raw = 0;
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.posIdx = 0;
  p->change.memberIdx = 0;
  p->change.depleteFlag = 0;
}

// 入れ替えポケモン選択アクション（通常）
void BTL_ACTION_SetChangeParam( BTL_ACTION_PARAM* p, u8 posIdx, u8 memberIdx )
{
  p->raw = 0;
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.posIdx = posIdx;
  p->change.memberIdx = memberIdx;
  p->change.depleteFlag = 0;
}

// 入れ替えポケモン選択アクション（もう戦えるポケモンがいない）
void BTL_ACTION_SetChangeDepleteParam( BTL_ACTION_PARAM* p )
{
  p->raw = 0;
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.memberIdx = 0;
  p->change.depleteFlag = 1;
}
// 入れ替えられるポケモンがもういない判定
bool BTL_ACTION_IsDeplete( const BTL_ACTION_PARAM* p )
{
  return ((p->change.cmd == BTL_ACTION_CHANGE) && (p->change.depleteFlag == 1));
}

// メガ進化フラグ
void BTL_ACTION_SetMegaEvoParam( BTL_ACTION_PARAM* p, u8 pokeIdx )
{
  p->raw = 0;
  p->mega_evo.cmd = BTL_ACTION_MEGAEVO;
  p->mega_evo.pokeIdx = pokeIdx;
  BTL_PRINT("MegaEvo(adrs=%p) pokeIdx=%d\n", p, p->mega_evo.pokeIdx);
}
bool BTL_ACTION_IsMegaEvoParam( const BTL_ACTION_PARAM* p )
{
  if( p->mega_evo.cmd == BTL_ACTION_MEGAEVO )
  {
    return true;
  }
  return false;
}


void BTL_ACTION_SetEscapeParam( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_ESCAPE;
}

void BTL_ACTION_SetNULL( BTL_ACTION_PARAM* p )
{
  p->raw = 0;
  p->gen.cmd = BTL_ACTION_NULL;
  p->gen.param = 0;
}

void BTL_ACTION_SetSkip( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_SKIP;
}

/**
 *  アクションコマンド取得
 */
BtlAction BTL_ACTION_GetAction( const BTL_ACTION_PARAM* p )
{
  return static_cast<BtlAction>( p->gen.cmd );
}

/**
 *  録画バッファあふれコマンド
 */
void BTL_ACTION_SetRecPlayOver( BTL_ACTION_PARAM* act )
{
  act->gen.cmd = BTL_ACTION_RECPLAY_TIMEOVER;
  act->gen.param = 0;
}

/**
 *  読み込み失敗通知コマンド
 */
void BTL_ACTION_SetRecPlayError( BTL_ACTION_PARAM* act )
{
  act->gen.cmd = BTL_ACTION_RECPLAY_ERROR;
  act->gen.param = 0;
}

GFL_NAMESPACE_END(btl)
