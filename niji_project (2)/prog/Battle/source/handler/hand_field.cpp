//=============================================================================================
/**
 * @file  hand_field.cpp
 * @brief ポケモンXY バトルシステム イベントファクター [フィールドエフェクト]
 * @author  taya
 *
 * @date  2010.12.21  作成
 */
//=============================================================================================
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>

#include "../btl_pokeparam.h"
#include "../btl_ServerFlow.h"
#include "hand_field.h"
#include "hand_common.h"

GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace field {

/**
 *  proto types
 */
const BtlEventHandlerTable* ADD_Fld_Weather( u32* numElems, u8 sub_param );
void handler_fld_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
const BtlEventHandlerTable* ADD_Fld_TrickRoom( u32* numElems, u8 sub_param );
void handler_fld_TrickRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
const BtlEventHandlerTable* ADD_Fld_Juryoku( u32* numElems, u8 sub_param );
void handler_fld_Jyuryoku_AdjustDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_fld_Jyuryoku_CheckAff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
const BtlEventHandlerTable* ADD_Fld_Sawagu( u32* numElems, u8 sub_param );
const BtlEventHandlerTable* ADD_Fld_MizuAsobi( u32* numElems, u8 sub_param );
void handler_fld_MizuAsobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
const BtlEventHandlerTable* ADD_Fld_DoroAsobi( u32* numElems, u8 sub_param );
void handler_fld_DoroAsobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
const BtlEventHandlerTable* ADD_Fld_WonderRoom( u32* numElems, u8 sub_param );
void handler_fld_WonderRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
const BtlEventHandlerTable* ADD_Fld_Fuin( u32* numElems, u8 sub_param );
const BtlEventHandlerTable* ADD_Fld_MagicRoom( u32* numElems, u8 sub_param );
const BtlEventHandlerTable* ADD_Fld_FairyLock( u32* numElems, u8 sub_param );
void handler_fld_dummy( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
const BtlEventHandlerTable* ADD_Fld_Ground( u32* numElems, u8 sub_param );
void handler_grass_power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_grass_turncheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_mist_power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_mist_guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_mist_checkFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_mist_sickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_elec_power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_elec_checkFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_elec_sickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_phycho_power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void handler_phycho_noEffectCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
const BtlEventHandlerTable* ADD_Fld_PlasmaShower( u32* numElems, u8 sub_param );
void handler_plasma_shower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work );
void common_field_sickGuardSucceed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BTL_POKEPARAM::ContFlag pokeContFlag, u16 strID );

  //@バトル変更箇所sango追加分
void handler_ooame_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_oohideri_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_rankiryuu_CheckAff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_rankiryuu_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );

//=============================================================================================
/**
 * フィールドエフェクトハンドラをシステムに追加
 *
 * @param   effect      エフェクトタイプ
 * @param   sub_param   エフェクトサブパラメータ（現状、天候の種類を指定するのみ）
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  Add( FieldStatus::EffectType effect, u8 sub_param )
{
  typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numHandlers, u8 sub_param );

  static const struct {
    FieldStatus::EffectType   eff;
    pEventAddFunc             func;
  }funcTbl[] = {
    { FieldStatus::EFF_WEATHER,      ADD_Fld_Weather      }, ///< 各種天候
    { FieldStatus::EFF_TRICKROOM,    ADD_Fld_TrickRoom    }, ///< トリックルーム
    { FieldStatus::EFF_JURYOKU,      ADD_Fld_Juryoku      }, ///< じゅうりょく
    { FieldStatus::EFF_FUIN,         ADD_Fld_Fuin         }, ///< ふういん
    { FieldStatus::EFF_MIZUASOBI,    ADD_Fld_MizuAsobi    }, ///< みずあそび
    { FieldStatus::EFF_DOROASOBI,    ADD_Fld_DoroAsobi    }, ///< どろあそび
    { FieldStatus::EFF_WONDERROOM,   ADD_Fld_WonderRoom   }, ///< ワンダールーム
    { FieldStatus::EFF_MAGICROOM,    ADD_Fld_MagicRoom    }, ///< マジックルーム
    { FieldStatus::EFF_FAIRY_LOCK,   ADD_Fld_FairyLock    }, ///< フェアリーロック
    { FieldStatus::EFF_PLASMASHOWER, ADD_Fld_PlasmaShower }, ///< プラズマシャワー
    { FieldStatus::EFF_GROUND,       ADD_Fld_Ground       }, ///< 各種グラウンド
  };

  {
    for(u32 i=0; i<GFL_NELEMS(funcTbl); ++i)
    {
      if( funcTbl[i].eff == effect )
      {
        const BtlEventHandlerTable* handlerTable;
        u32 numHandlers;

        handlerTable = funcTbl[i].func( &numHandlers, sub_param );
        return EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect,
                BTL_EVPRI_FIELD_DEFAULT, 0, sub_param, handlerTable, numHandlers );
      }
    }
  }
  return NULL;
}

//=============================================================================================
/**
 * フィールドエフェクトハンドラを削除
 *
 * @param   factor
 *
 */
//=============================================================================================
void  Remove( BTL_EVENT_FACTOR* factor )
{
  EVENT_FACTOR_Remove( factor );
}


//--------------------------------------------------------------------------------------
/**
 *  天候
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_Weather( u32* numElems, u8 sub_param )
{
  //@バトル変更箇所
  //sangoまで呼ばれてなかった。中身はダミーっぽかったのでグラウンドを参考に大幅修正

  static const BtlEventHandlerTable HandlerTable_Ooame[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_ooame_CheckExe   },    // ワザ出し成功判定
  };
  static const BtlEventHandlerTable HandlerTable_Oohideri[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_oohideri_CheckExe   },    // ワザ出し成功判定
  };
  static const BtlEventHandlerTable HandlerTable_Rankiryuu[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_rankiryuu_CheckAff    }, // 相性計算
    { BTL_EVENT_CHECK_AFFINITY_ONLY_ATTACKER, handler_rankiryuu_CheckAff    }, // 相性計算( 攻撃側のイベントハンドラのみが反応し、防御側の特性等を考慮しない )
    { BTL_EVENT_WAZA_DMG_PROC_END,            handler_rankiryuu_AfterDamage }, // ダメージ処理後(メッセージの表示
  };

  // ダミー
  static const BtlEventHandlerTable DammyHandlerTable[] = {
    { BTL_EVENT_NULL, handler_fld_dummy },
  };

  switch( sub_param ){
  case BTL_WEATHER_STORM:    ///< おおあめ
    *numElems = GFL_NELEMS( HandlerTable_Ooame );
    return HandlerTable_Ooame;

  case BTL_WEATHER_DAY:      ///< おおひでり
    *numElems = GFL_NELEMS( HandlerTable_Oohideri );
    return HandlerTable_Oohideri;

  case BTL_WEATHER_TURBULENCE:  ///< 乱気流
    *numElems = GFL_NELEMS( HandlerTable_Rankiryuu );
    return HandlerTable_Rankiryuu;

  default:
    *numElems = GFL_NELEMS( DammyHandlerTable );
    return DammyHandlerTable;
  }
}

//さわぐで使っている(?)ので取っておく
void handler_fld_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
}

//--------------------------------------------------------------------------------------
/**
 *  おおあめ用
 */
//--------------------------------------------------------------------------------------
// 成否チェック（炎の攻撃技を失敗させる）
void handler_ooame_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  //エアロックのチェックのため
  if( flowWk->Hnd_GetWeather() == BTL_WEATHER_STORM )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_HONOO )
    {
      if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC ||
          EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_SPECIAL )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OOAME );
      }
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  おおひでり用
 */
//--------------------------------------------------------------------------------------
// 成否チェック（水の攻撃技を失敗させる）
void handler_oohideri_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  //エアロックのチェックのため
  if( flowWk->Hnd_GetWeather() == BTL_WEATHER_DAY )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_MIZU )
    {
      if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC ||
          EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_SPECIAL )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OOHIDERI );
      }
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  乱気流用
 */
//--------------------------------------------------------------------------------------
// ひこうタイプへの弱点属性を等倍に
void handler_rankiryuu_CheckAff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  //エアロックのチェックのため
  if( flowWk->Hnd_GetWeather() == BTL_WEATHER_TURBULENCE )
  {
    //対象が飛行
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_TYPE) == POKETYPE_HIKOU )
    {
      //飛行に対してバツグンの場合(逆さバトル対応
      if( calc::TypeAff( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) , POKETYPE_HIKOU ) == pml::battle::TypeAffinity::TYPEAFF_2 )
      {
        // 等倍に書き換え
        EVENTVAR_RewriteValue( BTL_EVAR_TYPEAFF, pml::battle::TypeAffinity::TYPEAFF_1 );
        
        //ここでメッセージを出すとちきゅうなげ等の固定ダメージが拾えないので、
        //技カテゴリ分岐をやめ、メッセージは別ハンドらで出す。
      }
    }
  }
}

void handler_rankiryuu_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  //エアロックのチェックのため
  if( flowWk->Hnd_GetWeather() == BTL_WEATHER_TURBULENCE )
  {
    u8 defPoke = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( defPoke );
    //対象が飛行
    if( bpp->IsMatchType(POKETYPE_HIKOU) )
    {
      //飛行に対してバツグンの場合(逆さバトル対応
      if( calc::TypeAff( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) , POKETYPE_HIKOU ) == pml::battle::TypeAffinity::TYPEAFF_2 )
      {
        //ちきゅうなげ、ナイトヘッド対策
        if( EVENTVAR_GetValue( BTL_EVAR_FIX_DAMAGE) == 0 )
        {
          //メッセージを出す
          BTL_HANDEX_PARAM_MESSAGE* msg_param;
          msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, BTL_POKEID_NULL ));
            HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_TurbulenceFail );
          flowWk->HANDEX_Pop( msg_param );
        }
      }
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  トリックルーム
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_TrickRoom( u32* numElems, u8 sub_param )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,  handler_fld_TrickRoom   },  // ダメージ補正
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_fld_TrickRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  EVENTVAR_RewriteValue( BTL_EVAR_TRICK_FLAG, true );
}
//--------------------------------------------------------------------------------------
/**
 *  じゅうりょく
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_Juryoku( u32* numElems, u8 sub_param )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,                handler_fld_Jyuryoku_AdjustDmg },  // 命中率補正ハンドラ
    { BTL_EVENT_CHECK_AFFINITY,                handler_fld_Jyuryoku_CheckAff  },  // 相性計算
    { BTL_EVENT_CHECK_AFFINITY_ONLY_ATTACKER,  handler_fld_Jyuryoku_CheckAff  },  // 相性計算( 攻撃側のイベントハンドラのみが反応し、防御側の特性等を考慮しない )
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 命中率補正ハンドラ
void handler_fld_Jyuryoku_AdjustDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.67) );
}
// 相性チェックハンドラ
void handler_fld_Jyuryoku_CheckAff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_JIMEN )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
  }
}


//--------------------------------------------------------------------------------------
/**
 *  さわぐ
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_Sawagu( u32* numElems, u8 sub_param )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,  handler_fld_Weather   },  // ダメージ補正
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//--------------------------------------------------------------------------------------
/**
 *  みずあそび
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_MizuAsobi( u32* numElems, u8 sub_param )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,  handler_fld_MizuAsobi   },  // ダメージ補正
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_fld_MizuAsobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_HONOO ){
    EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.33) );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  どろあそび
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_DoroAsobi( u32* numElems, u8 sub_param )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,  handler_fld_DoroAsobi   },  // ダメージ補正
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_fld_DoroAsobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DENKI ){
    EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.33) );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ワンダールーム
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_WonderRoom( u32* numElems, u8 sub_param )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD_PREV,  handler_fld_WonderRoom   },  // 防御側能力値計算の前
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_fld_WonderRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  int swap_cnt = EVENTVAR_GetValue( BTL_EVAR_VID_SWAP_CNT);
  EVENTVAR_RewriteValue( BTL_EVAR_VID_SWAP_CNT, (swap_cnt+1) );
}
//--------------------------------------------------------------------------------------
/**
 *  ふういん
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_Fuin( u32* numElems, u8 sub_param )
{
  // 今のところ、何もハンドラを用意する必要が無くなってしまった…
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NULL, handler_fld_dummy },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//--------------------------------------------------------------------------------------
/**
 *  マジックルーム
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_MagicRoom( u32* numElems, u8 sub_param )
{
  // 現状、マジックルームは登録しておくことで効果は各所で判断するため、
  // こいつのハンドラ自体は何もしない。
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NULL, handler_fld_dummy },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//--------------------------------------------------------------------------------------
/**
 *  フェアリーロック
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_FairyLock( u32* numElems, u8 sub_param )
{
  // 現状、フェアリーロックは登録しておくことで効果は各所で判断するため、
  // こいつのハンドラ自体は何もしない。
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NULL, handler_fld_dummy },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_fld_dummy( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{

}
//--------------------------------------------------------------------------------------
/**
 *  プラズマシャワー
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_PlasmaShower( u32* numElems, u8 sub_param )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM_2ND, handler_plasma_shower },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_plasma_shower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_NORMAL )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, POKETYPE_DENKI );
  }
}




//--------------------------------------------------------------------------------------
/**
 *  各種グラウンド
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Fld_Ground( u32* numElems, u8 sub_param )
{
  // グラスフィールド
  static const BtlEventHandlerTable GrassHandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,      handler_grass_power     },  // 威力決定ハンドラ
    { BTL_EVENT_TURNCHECK_BEGIN, handler_grass_turncheck },  // ターンチェックハンドラ
  };
  // ミストフィールド
  static const BtlEventHandlerTable MistHandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,        handler_mist_power      },  // 威力決定ハンドラ
    { BTL_EVENT_ADDSICK_CHECKFAIL, handler_mist_checkFail  },  // 状態異常失敗チェックハンドラ
    { BTL_EVENT_ADDSICK_FAILED,    handler_mist_sickFailed },  // 状態異常失敗確定ハンドラ
  };
  // エレキフィールド
  static const BtlEventHandlerTable ElecHandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,        handler_elec_power         },  // 威力決定ハンドラ
    { BTL_EVENT_ADDSICK_CHECKFAIL, handler_elec_checkFail     },  // 状態異常失敗チェックハンドラ
    { BTL_EVENT_ADDSICK_FAILED,    handler_elec_sickFailed    },  // 状態異常失敗確定ハンドラ
  };
  // サイコフィールド
  static const BtlEventHandlerTable PhychoHandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,           handler_phycho_power         }, // 威力決定ハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_FIELD, handler_phycho_noEffectCheck }, // フィールド効果による技無効化チェック
  };

  // ダミー
  static const BtlEventHandlerTable DammyHandlerTable[] = {
    { BTL_EVENT_NULL, handler_fld_dummy },
  };

  switch( sub_param ){
  case BTL_GROUND_GRASS:
    *numElems = GFL_NELEMS( GrassHandlerTable );
    return GrassHandlerTable;

  case BTL_GROUND_MIST:
    *numElems = GFL_NELEMS( MistHandlerTable );
    return MistHandlerTable;

  case BTL_GROUND_ELEKI:
    *numElems = GFL_NELEMS( ElecHandlerTable );
    return ElecHandlerTable;

  case BTL_GROUND_PHYCHO:
    *numElems = GFL_NELEMS( PhychoHandlerTable );
    return PhychoHandlerTable;

  default:
    *numElems = GFL_NELEMS( DammyHandlerTable );
    return DammyHandlerTable;
  }
}
//--------------------------------------------------------------
// 各種グラウンド系の効果がおよぶポケモンかどうか判定
//--------------------------------------------------------------

bool common_isGroundEffective( ServerFlow* flowWk, u8 pokeID )
{
  // ふゆう状態にある場合は効かない
  if( flowWk->Hnd_IsFloatingPoke(pokeID) ){
    return false;
  }

  // そらをとぶなどで消えているポケモンにも効かない
  if( flowWk->Hnd_IsTameHidePoke(pokeID) ){
    return false;
  }

  return true;
}

//-----------------------------------------------
//  グラスフィールド
//-----------------------------------------------
// グラス：威力決定ハンドラ
void handler_grass_power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  // 着地しているポケモンの草ワザなら威力1.5倍
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_KUSA )
  {
    u8 pokeID = EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK);
    if( common_isGroundEffective(flowWk, pokeID) ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );
    }
  }
  // じしん・マグニチュード・じならしは威力半分
  {
    WazaID  waza = (WazaID)(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( (waza == WAZANO_ZISIN)
    ||  (waza == WAZANO_MAGUNITYUUDO)
    ||  (waza == WAZANO_ZINARASI)
    ){
      u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
      if( common_isGroundEffective(flowWk, pokeID) ){
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5f) );
      }
    }
  }
}
// グラス：ターンチェックハンドラ
void handler_grass_turncheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  u8 pokeID = EVENTVAR_GetValue(BTL_EVAR_POKEID);
  if( (pokeID != BTL_POKEID_NULL)
  &&  (common_isGroundEffective(flowWk, pokeID))
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    BTL_HANDEX_PARAM_RECOVER_HP*  param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP*>
                                            (flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
      param->pokeID = pokeID;
      param->recoverHP = calc::QuotMaxHP( bpp, 16 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//-----------------------------------------------
//  ミストフィールド
//-----------------------------------------------
// ミスト：威力決定ハンドラ
void handler_mist_power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  // 着地しているポケモンに対するドラゴンワザなら威力半分
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DRAGON )
  {
    u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    if( common_isGroundEffective(flowWk, pokeID) ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5f) );
    }
  }
}
// ミスト：状態異常失敗チェックハンドラ
void handler_mist_checkFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( common_isGroundEffective(flowWk, pokeID) )
  {
    WazaSick  sickID = (WazaSick)(EVENTVAR_GetValue(BTL_EVAR_SICKID));
    if( calc::IsBasicSickID(sickID) ||
        ( sickID == pml::wazadata::WAZASICK_KONRAN ) )
    {
      bool bGuard = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      if( bGuard )
      {
        // ミストフィールドで防いだ記録をフラグで残す
        BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>(flowWk->HANDEX_Push(BTL_HANDEX_SET_CONTFLAG, pokeID));
          param->pokeID = pokeID;
          param->flag = BTL_POKEPARAM::CONTFLG_MISTFIELD_GUARD;
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
// ミスト：状態異常失敗確定ハンドラ
void handler_mist_sickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  common_field_sickGuardSucceed( myHandle, flowWk, BTL_POKEPARAM::CONTFLG_MISTFIELD_GUARD, BTL_STRID_SET_MistFieldEffect );
}


//-----------------------------------------------
//  エレキフィールド
//-----------------------------------------------

// エレキ：威力計算ハンドラ
void handler_elec_power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  // 着地しているポケモンのでんきワザは1.5倍
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DENKI )
  {
    u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( common_isGroundEffective(flowWk, pokeID) ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5) );
    }
  }
}

// エレキ：状態異常チェックハンドラ
void handler_elec_checkFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( common_isGroundEffective(flowWk, pokeID) )
  {
    bool bGuard = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_NEMURI );
    if( !bGuard ){
      bGuard = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_AKUBI );
    }

    if( bGuard )
    {
      // エレキフィールドで防いだ記録をフラグで残す
      BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>(flowWk->HANDEX_Push(BTL_HANDEX_SET_CONTFLAG, pokeID));
        param->pokeID = pokeID;
        param->flag = BTL_POKEPARAM::CONTFLG_ELECFIELD_GUARD;
      flowWk->HANDEX_Pop( param );
    }
  }
}
// エレキ：状態異常失敗確定ハンドラ
void handler_elec_sickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  common_field_sickGuardSucceed( myHandle, flowWk, BTL_POKEPARAM::CONTFLG_ELECFIELD_GUARD, BTL_STRID_SET_ElecFieldEffect );
}

void common_field_sickGuardSucceed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BTL_POKEPARAM::ContFlag pokeContFlag, u16 strID )
{
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->CONTFLAG_Get(pokeContFlag) )
  {
    // フィールド効果で防いだ記録を落とす
    BTL_HANDEX_PARAM_RESET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = pokeContFlag;
    flowWk->HANDEX_Pop( param );

    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, strID );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//-----------------------------------------------
// サイコフィールド
//-----------------------------------------------

/**
 * @brief サイコフィールド：威力計算ハンドラ
 *
 * 接地しているポケモンの「エスパー」技は1.5倍
 */
void handler_phycho_power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  // 使用する技のタイプが「エスパー」でない
  if( EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE ) != POKETYPE_ESPER )
  {
    return;
  }

  // 接地していない
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  if( !common_isGroundEffective( flowWk, pokeID ) ) 
  {
    return;
  }

  EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5) );
}

/** 
 * @brief サイコフィールド：フィールド効果による技無効化チェック
 *
 * 接地しているポケモンには、先制技が効かない
 */
void handler_phycho_noEffectCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 subParam, int* work )
{
  const s32 wazaPriority = EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );
  const u8  atkPokeID    = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  const u8  defPokeID    = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

  // 先制技でない
  if( wazaPriority <= 0 )
  {
    return;
  }

  // ターゲットが味方
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID( atkPokeID, defPokeID ) )
  {
    return;
  }

  // ターゲットが接地していない
  if( !common_isGroundEffective( flowWk, defPokeID ) )
  {
    return;
  }

  if( EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true ) )
  {
    BTL_HANDEX_STR_PARAMS* strParam = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
    HANDEX_STR_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_PhychoField_Protect );
    HANDEX_STR_AddArg( strParam, defPokeID );
  }
}


} // end of namespace 'field'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)


