//=============================================================================================
/**
 * @file  btl_sickcont.cpp
 * @brief ポケモンXY バトルシステム 状態異常などの継続パラメータ
 * @author  taya
 *
 * @date  2010.12.21  作成
 */
//=============================================================================================

#include "btl_common.h"
#include "btl_sickcont.h"

GFL_NAMESPACE_BEGIN(btl)


/**
 *  NULL型 作成
 */
BTL_SICKCONT  SICKCONT_MakeNull( void )
{
  BTL_SICKCONT  cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_NONE;
  cont.causePokeID = BTL_POKEID_NULL;
  return cont;
}
/**
 *  NULL型かどうか判別
 */
bool SICKCONT_IsNull( const BTL_SICKCONT& cont )
{
  return cont.type == pml::wazadata::SICKCONT_NONE;
}

/*
 *  ターン数型 作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param turn         継続ターン数
 */
BTL_SICKCONT  SICKCONT_MakeTurn( u8 causePokeID, u8 turns )
{
  BTL_SICKCONT  cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_TURN;
  cont.causePokeID = causePokeID;
  cont.turn.count = turns;
  cont.turn.param = 0;
  return cont;
}
/*
 *  ターン数型（パラメータ付き）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param turn         継続ターン数
 * @param param        パラメータ
 */
BTL_SICKCONT  SICKCONT_MakeTurnParam( u8 causePokeID, u8 turns, u16 param )
{
  BTL_SICKCONT  cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_TURN;
  cont.causePokeID = causePokeID;
  cont.turn.count = turns;
  cont.turn.param = param;
  return cont;
}
/*
 *  ポケ依存型作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param pokeID       依存対象ポケモンのID
 */
BTL_SICKCONT  SICKCONT_MakePoke( u8 causePokeID, u8 pokeID )
{
  BTL_SICKCONT  cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_POKE;
  cont.causePokeID = causePokeID;
  cont.poke.ID = pokeID;
  return cont;
}
/*
 *  永続型作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 */
BTL_SICKCONT  SICKCONT_MakePermanent( u8 causePokeID )
{
  BTL_SICKCONT cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_PERMANENT;
  cont.causePokeID = causePokeID;
  return cont;
}
/*
 *  永続型（ターン数カウントアップ）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param count_max    ターンカウントの最大値
 */
BTL_SICKCONT  SICKCONT_MakePermanentInc( u8 causePokeID, u8 count_max )
{
  BTL_SICKCONT cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_PERMANENT;
  cont.causePokeID = causePokeID;
  cont.permanent.count_max = count_max;
  return cont;
}
/*
 *  永続型（パラメータ付き）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param param        パラメータ
 */
BTL_SICKCONT  SICKCONT_MakePermanentParam( u8 causePokeID, u16 param )
{
  BTL_SICKCONT cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_PERMANENT;
  cont.causePokeID = causePokeID;
  cont.permanent.param = param;
  return cont;
}
/*
 *  永続型（ターン数カウントアップ＆パラメータ付き）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param count_max    ターンカウントの最大値
 * @param param        パラメータ
 */
BTL_SICKCONT  SICKCONT_MakePermanentIncParam( u8 causePokeID, u8 count_max, u16 param )
{
  BTL_SICKCONT cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_PERMANENT;
  cont.causePokeID = causePokeID;
  cont.permanent.count_max = count_max;
  cont.permanent.param = param;
  return cont;
}
/*
 *  ターン数・ポケ依存混合型作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param pokeID       依存対象ポケモンのID
 * @param turn         継続ターン数
 */
BTL_SICKCONT  SICKCONT_MakePokeTurn( u8 causePokeID, u8 pokeID, u8 turns )
{
  BTL_SICKCONT  cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_POKETURN;
  cont.causePokeID = causePokeID;
  cont.poketurn.count = turns;
  cont.poketurn.pokeID = pokeID;
  return cont;
}
/*
 *  ターン数・ポケ依存混合型（パラメータ付き）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param pokeID       依存対象ポケモンのID
 * @param turn         継続ターン数
 * @param param        パラメータ
 */
BTL_SICKCONT  SICKCONT_MakePokeTurnParam( u8 causePokeID, u8 pokeID, u8 turns, u16 param )
{
  BTL_SICKCONT  cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_POKETURN;
  cont.causePokeID = causePokeID;
  cont.poketurn.count = turns;
  cont.poketurn.pokeID = pokeID;
  cont.poketurn.param = param;
  return cont;
}
/**
 * もうどく用型作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 */
BTL_SICKCONT  SICKCONT_MakeMoudokuCont( u8 causePokeID )
{
  BTL_SICKCONT cont;
  cont.raw = 0;
  cont.type = pml::wazadata::SICKCONT_PERMANENT;
  cont.causePokeID = causePokeID;
  cont.permanent.count_max = BTL_MOUDOKU_COUNT_MAX;
  return cont;
}
/**
 * もうどく用型かどうか判別
 */
bool  SICKCONT_IsMoudokuCont( const BTL_SICKCONT& cont )
{
  if( (cont.type == pml::wazadata::SICKCONT_PERMANENT)
  &&  (cont.permanent.count_max == BTL_MOUDOKU_COUNT_MAX)
  ){
    return true;
  }
  return false;
}
/*
 *  依存ポケIDを取得（ポケ依存でなければ BTL_POKEID_NULL）
 */
u8  SICKCONT_GetPokeID( const BTL_SICKCONT& cont )
{
  if( cont.type == pml::wazadata::SICKCONT_POKE ){
    return cont.poke.ID;
  }
  if( cont.type == pml::wazadata::SICKCONT_POKETURN ){
    return cont.poketurn.pokeID;
  }
  return BTL_POKEID_NULL;
}
/*
 *  依存ポケIDを書き換え（ポケ依存でなければ何もしないL）
 */
void  SICKCONT_SetPokeID( BTL_SICKCONT* cont, u8 pokeID )
{
  if( cont->type == pml::wazadata::SICKCONT_POKE ){
    cont->poke.ID = pokeID;
  }
  else if( cont->type == pml::wazadata::SICKCONT_POKETURN ){
    cont->poketurn.pokeID = pokeID;
  }
}
/*
 *  有効ターン数を取得（ターン数制限がなければ 0）
 */
u8  SICCONT_GetTurnMax( const BTL_SICKCONT& cont )
{
  if( cont.type == pml::wazadata::SICKCONT_TURN ){
    return cont.turn.count;
  }
  if( cont.type == pml::wazadata::SICKCONT_POKETURN ){
    return cont.poketurn.count;
  }
  return 0;
}
/*
 *  汎用パラメータを設定
 */
void  SICKCONT_AddParam( BTL_SICKCONT* cont, u16 param )
{
  if( cont->type == pml::wazadata::SICKCONT_PERMANENT ){
    cont->permanent.param = param;
    return;
  }
  else if( cont->type == pml::wazadata::SICKCONT_POKE ){
    cont->poke.param = param;
    return;
  }
  else if( cont->type == pml::wazadata::SICKCONT_POKETURN ){
    cont->poketurn.param = param;
    return;
  }
  else if( cont->type == pml::wazadata::SICKCONT_TURN ){
    cont->turn.param = param;
    return;
  }
}
/*
 *  設定した汎用パラメータを取得
 */
u16  SICKCONT_GetParam( const BTL_SICKCONT& cont )
{
  if( cont.type == pml::wazadata::SICKCONT_PERMANENT ){
    return cont.permanent.param;
  }
  if( cont.type == pml::wazadata::SICKCONT_POKE ){
    return cont.poke.param;
  }
  if( cont.type == pml::wazadata::SICKCONT_POKETURN ){
    return cont.poketurn.param;
  }
  if( cont.type == pml::wazadata::SICKCONT_TURN ){
    return cont.turn.param;
  }
  return 0;
}
/*
 *  汎用フラグセット
 */
void  SICKCONT_SetFlag( BTL_SICKCONT* cont, bool flag )
{
  if( cont->type == pml::wazadata::SICKCONT_PERMANENT ){
    cont->permanent.flag = flag;
    return;
  }
  if( cont->type == pml::wazadata::SICKCONT_POKE ){
    cont->poke.flag = flag;
    return;
  }
  if( cont->type == pml::wazadata::SICKCONT_POKETURN ){
    cont->poketurn.flag = flag;
    return;
  }
  if( cont->type == pml::wazadata::SICKCONT_TURN ){
    cont->turn.flag = flag;
    return;
  }
}
/*
 *  汎用フラグ取得
 */
bool  SICKCONT_GetFlag( const BTL_SICKCONT& cont )
{
  if( cont.type == pml::wazadata::SICKCONT_PERMANENT ){
    return cont.permanent.flag;
  }
  if( cont.type == pml::wazadata::SICKCONT_POKE ){
    return cont.poke.flag;
  }
  if( cont.type == pml::wazadata::SICKCONT_POKETURN ){
    return cont.poketurn.flag;
  }
  if( cont.type == pml::wazadata::SICKCONT_TURN ){
    return cont.turn.flag;
  }
  return 0;

}
/*
 *  継続ターン数増加
 */
void  SICKCONT_IncTurn( BTL_SICKCONT* cont, u8 inc )
{
  if( cont->type == pml::wazadata::SICKCONT_TURN ){
    if( cont->turn.count < 8 ){
      cont->turn.count += inc;
    }
  }
  if( cont->type == pml::wazadata::SICKCONT_POKETURN ){
    if( cont->poketurn.count < 8 ){
      cont->poketurn.count += inc;
    }
  }
}
/*
 *  継続ターン数セット
 */
void  SICKCONT_SetTurn( BTL_SICKCONT* cont, u8 turn )
{
  if( cont->type == pml::wazadata::SICKCONT_TURN ){
    if( cont->turn.count < 8 ){
      cont->turn.count = turn;
    }
  }
  if( cont->type == pml::wazadata::SICKCONT_POKETURN ){
    if( cont->poketurn.count < 8 ){
      cont->poketurn.count = turn;
    }
  }
}

/**
 * @brief 状態異常の原因となったポケモンのIDを取得する
 * @retval [0, 31]          原因となったポケモンが存在する場合
 * @retval BTL_POKEID_NULL  原因となったポケモンが存在しない場合
 */
u8 SICKCONT_GetCausePokeID( const BTL_SICKCONT& cont )
{
  return cont.causePokeID;
}

/**
 * @brief 状態異常の原因となったポケモンのIDを設定する
 * @param[in]  pokeID   原因ポケモンID
 */
void SICKCONT_SetCausePokeID( BTL_SICKCONT* cont, u8 pokeID )
{
  cont->causePokeID = pokeID;
}

/**
 * @brief 32ビット値に分割する
 * @param[in]  cont   分割対象の BTL_SICKCONT
 * @param[out] high   上位32ビットの格納先
 * @param[out] low    下位32ビットの格納先
 */
void SICKCONT_Split32bit( const BTL_SICKCONT& cont, u32* high, u32* low )
{
  *high = static_cast<u32>( cont.raw >> 32 );
  *low  = static_cast<u32>( cont.raw & 0x00000000ffffffff );
}

/**
 * @brief 32ビット値を結合する
 * @param[out] cont   結合した値の格納先
 * @param[in]  high   上位32ビット
 * @param[in]  low    下位32ビット
 */
void SICKCONT_Unite32bit( BTL_SICKCONT* cont, u32 high, u32 low )
{
  u64 high_64 = static_cast<u64>( high ) << 32;
  u64 low_64  = static_cast<u64>( low );
  cont->raw = ( high_64 | low_64 );
}



GFL_NAMESPACE_END(btl)


