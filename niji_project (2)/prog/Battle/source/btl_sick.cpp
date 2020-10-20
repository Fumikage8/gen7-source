//=============================================================================================
/**
 * @file  btl_sick.cpp
 * @brief ポケモンXY バトルシステム 状態異常計算処理
 * @author  taya
 *
 * @date  2011.01.25  作成
 */
//=============================================================================================

#include "./btl_ServerFlow.h"
#include "./btl_event.h"
#include "./btl_str.h"
#include "./btl_sick.h"
#include "./btl_tables.h"
#include "./btl_pokeparam.h"

GFL_NAMESPACE_BEGIN(btl)


namespace sick {

int getCureStrID( WazaSick sick, bool fUseItem )
{
  static const struct {
    WazaSick  sick;
    s16       strID_notItem;
    s16       strID_useItem;
  }dispatchTbl[] = {
    { pml::wazadata::WAZASICK_DOKU,          BTL_STRID_SET_DokuCure,         BTL_STRID_SET_UseItem_CureDoku    },
    { pml::wazadata::WAZASICK_YAKEDO,        BTL_STRID_SET_YakedoCure,       BTL_STRID_SET_UseItem_CureYakedo  },
    { pml::wazadata::WAZASICK_NEMURI,        BTL_STRID_SET_NemuriWake,       BTL_STRID_SET_UseItem_CureNemuri  },
    { pml::wazadata::WAZASICK_KOORI,         BTL_STRID_SET_KoriMelt,         BTL_STRID_SET_UseItem_CureKoori   },
    { pml::wazadata::WAZASICK_MAHI,          BTL_STRID_SET_MahiCure,         BTL_STRID_SET_UseItem_CureMahi    },
    { pml::wazadata::WAZASICK_ENCORE,        BTL_STRID_SET_EncoreCure,       -1                                },
    { pml::wazadata::WAZASICK_KANASIBARI,    BTL_STRID_SET_KanasibariCure,   -1                                },
    { pml::wazadata::WAZASICK_SASIOSAE,      BTL_STRID_SET_SasiosaeCure,     -1                                },
    { pml::wazadata::WAZASICK_BIND,          BTL_STRID_SET_BindCure,         BTL_STRID_SET_BindCure            },
    { pml::wazadata::WAZASICK_YADORIGI,      BTL_STRID_SET_BindCure,         BTL_STRID_SET_BindCure            },
    { pml::wazadata::WAZASICK_TELEKINESIS,   BTL_STRID_SET_Telekinesis_End,  -1                                },
    { pml::wazadata::WAZASICK_TYOUHATSU,     BTL_STRID_SET_ChouhatuCure,     -1                                },
    { pml::wazadata::WAZASICK_FLYING,        BTL_STRID_SET_DenjiFuyuCure,    -1                                },
    { pml::wazadata::WAZASICK_KAIHUKUHUUJI,  BTL_STRID_SET_KaifukuFujiCure,  -1                                },
    { pml::wazadata::WAZASICK_ICHAMON,       BTL_STRID_SET_IchamonCure,      -1                                },

    { pml::wazadata::WAZASICK_KONRAN,        BTL_STRID_SET_KonranCure,       BTL_STRID_SET_UseItem_CureKonran  },
    { pml::wazadata::WAZASICK_MEROMERO,      BTL_STRID_SET_MeromeroCure,     BTL_STRID_SET_UseItem_CureMero    },
  };

  for(u32 i=0; i<GFL_NELEMS(dispatchTbl); ++i){
    if( dispatchTbl[i].sick == sick )
    {
      int strID = (fUseItem)? dispatchTbl[i].strID_useItem : dispatchTbl[i].strID_notItem;
      if( strID < 0 ){
        strID = dispatchTbl[i].strID_notItem;
      }
      return strID;
    }
  }

  return -1;
}

//=============================================================================================
/**
 * 状態異常にかかった時のデフォルトメッセージＩＤを返す
 *
 * @param   sickID
 * @param   cont
 *
 * @retval  int   メッセージID（存在しない場合-1）
 */
//=============================================================================================
int getDefaultSickStrID( WazaSick sickID, const BTL_SICKCONT& cont )
{
  int strID = -1;

  switch( sickID ){
  case pml::wazadata::WAZASICK_YAKEDO:       strID = BTL_STRID_SET_YakedoGet; break;
  case pml::wazadata::WAZASICK_MAHI:         strID = BTL_STRID_SET_MahiGet; break;
  case pml::wazadata::WAZASICK_KOORI:        strID = BTL_STRID_SET_KoriGet; break;
  case pml::wazadata::WAZASICK_NEMURI:       strID = BTL_STRID_SET_NemuriGet; break;
  case pml::wazadata::WAZASICK_KONRAN:       strID = BTL_STRID_SET_KonranGet; break;
  case pml::wazadata::WAZASICK_MEROMERO:     strID = BTL_STRID_SET_MeromeroGet; break;
  case pml::wazadata::WAZASICK_AKUMU:        strID = BTL_STRID_SET_AkumuGet;  break;
  case pml::wazadata::WAZASICK_TYOUHATSU:    strID = BTL_STRID_SET_Chouhatu; break;
  case pml::wazadata::WAZASICK_ICHAMON:      strID = BTL_STRID_SET_Ichamon; break;
  case pml::wazadata::WAZASICK_AKUBI:        strID = BTL_STRID_SET_Akubi; break;
  case pml::wazadata::WAZASICK_YADORIGI:     strID = BTL_STRID_SET_Yadorigi; break;
  case pml::wazadata::WAZASICK_MIYABURU:     strID = BTL_STRID_SET_Miyaburu; break;
  case pml::wazadata::WAZASICK_NEWOHARU:     strID = BTL_STRID_SET_NeWoHaru; break;
  case pml::wazadata::WAZASICK_SASIOSAE:     strID = BTL_STRID_SET_Sasiosae; break;
  case pml::wazadata::WAZASICK_KANASIBARI:   strID = BTL_STRID_SET_Kanasibari; break;
  case pml::wazadata::WAZASICK_KAIHUKUHUUJI: strID = BTL_STRID_SET_KaifukuFuji; break;

  case pml::wazadata::WAZASICK_DOKU:
    strID = SICKCONT_IsMoudokuCont(cont)? BTL_STRID_SET_MoudokuGet : BTL_STRID_SET_DokuGet;
    break;

  default:
    break;
  }

  BTL_Printf("sickID=%d, defMsgID=%d\n", sickID, strID);

  return strID;
}

/**
 *  状態異常によるダメージ時の標準メッセージID取得
 */
int getWazaSickDamageStrID( WazaSick sick )
{
  switch( sick ){
  case pml::wazadata::WAZASICK_DOKU:   return BTL_STRID_SET_DokuDamage;
  case pml::wazadata::WAZASICK_YAKEDO: return BTL_STRID_SET_YakedoDamage;
  case pml::wazadata::WAZASICK_AKUMU:  return BTL_STRID_SET_AkumuDamage;
  case pml::wazadata::WAZASICK_NOROI:  return BTL_STRID_SET_NoroiDamage;
  }
  return -1;
}
//==============================================================================================
//==============================================================================================

// ほろびのうたカウント表示共通処理
void putHorobiCounter( ServerFlow* flwk, const BTL_POKEPARAM* bpp, u8 count )
{
  u8 pokeID = bpp->GetID();

  BTL_HANDEX_PARAM_MESSAGE* param = (BTL_HANDEX_PARAM_MESSAGE*)(flwk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HorobiCountDown );
    HANDEX_STR_AddArg( &param->str, pokeID );
    HANDEX_STR_AddArg( &param->str, count );
  flwk->HANDEX_Pop( param );
}


/**
 *  ほろびのうた：継続
 */
void cont_HorobiNoUta( ServerFlow* flowWk, BTL_POKEPARAM* bpp )
{
  if( !bpp->IsDead() )
  {
    BTL_SICKCONT cont = bpp->GetSickCont( pml::wazadata::WAZASICK_HOROBINOUTA );
    u8 turnMax = SICCONT_GetTurnMax( cont );
    u8 turnNow = bpp->GetSickTurnCount( pml::wazadata::WAZASICK_HOROBINOUTA );
    int turnDiff = turnMax - turnNow;
    if( turnDiff > 0 ){
      putHorobiCounter( flowWk, bpp, turnDiff );
    }
  }
}
/**
 *  やどりぎのたね：継続
 */
void cont_Yadorigi( ServerFlow* flwk, BTL_POKEPARAM* bpp )
{
  BTL_SICKCONT  cont = bpp->GetSickCont( pml::wazadata::WAZASICK_YADORIGI );
  BtlPokePos  pos_to = static_cast<BtlPokePos>(SICKCONT_GetParam( cont ));
  u8 recoverPokeID = flwk->Hnd_GetExistPokeID( pos_to );

  if( recoverPokeID != BTL_POKEID_NULL )
  {
    BTL_HANDEX_PARAM_DAMAGE* dmg_param;
    u16 damage = calc::QuotMaxHP( bpp, 8 );
    u16 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
//    u16 que_reserve_pos = BTL_SVFTOOL_ReserveQuePos( flwk, SC_ACT_EFFECT_BYVECTOR );

    if( damage > hp ){
      damage = hp;
    }

    u8 pokeID = bpp->GetID();
    u8 causePokeID = SICKCONT_GetCausePokeID( cont );

    dmg_param = (BTL_HANDEX_PARAM_DAMAGE*)(flwk->HANDEX_Push( BTL_HANDEX_DAMAGE, causePokeID ));
      dmg_param->pokeID = pokeID;
      dmg_param->damage = damage;
      dmg_param->damageCause = DAMAGE_CAUSE_SICK_YADORIGINOTANE;
      dmg_param->damageCausePokeID = causePokeID;
      HANDEX_STR_Setup( &dmg_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_YadorigiTurn );
      HANDEX_STR_AddArg( &dmg_param->exStr, pokeID );
      // ダメージが成功したらエフェクト
      {
        // エフェクト側の from, to の解釈が逆っぽいのでこうする。
        dmg_param->fExEffect = true;
        dmg_param->effectNo = BTLEFF_YADORIGI;
        dmg_param->pos_from = pos_to;
        dmg_param->pos_to = flwk->Hnd_PokeIDtoPokePos( pokeID );
      }
    flwk->HANDEX_Pop( dmg_param );

    // 続けて回復
    if( recoverPokeID != BTL_POKEID_NULL )
    {
      BTL_HANDEX_PARAM_DRAIN* drain_param;
      drain_param = (BTL_HANDEX_PARAM_DRAIN*)flwk->HANDEX_Push( BTL_HANDEX_DRAIN, pokeID );
        drain_param->header.failSkipFlag = true;
        drain_param->recoverPokeID = recoverPokeID;
        drain_param->damagedPokeID = pokeID;
        drain_param->recoverHP = damage;
      flwk->HANDEX_Pop( drain_param );

      // @fix NMCat[37]「ヘドロえき」の相手に植えた「やどりぎのタネ」の効果で瀕死になった時、ポケモンが倒れず行動選択した状態で場に残る
      //
      // 原因：
      // 　勝敗判定の仕様変更によって、「ヘドロえき」のダメージ処理ではポケモンの退場処理を行わなくなった。
      // 　(「ヘドロえき」のダメージ処理の中で退場させるのではなく、技実行処理の中にある退場処理で退場させることで、
      // 　 「メガドレイン」などのドレイン技に対する「ヘドロえき」により、両陣営が全滅した場合の勝敗判定( 倒れた順番 )を正しく解釈する )
      // 　この変更により、上のドレイン処理において「ヘドロえき」のダメージで死亡しても、退場しなくなってしまっていた。
      //
      // 対処：
      // 　ここで、回復対象ポケモンの死亡チェックを行う。
      BTL_HANDEX_PARAM_DEAD_CHECK* dead_check_param;
      dead_check_param = (BTL_HANDEX_PARAM_DEAD_CHECK*)flwk->HANDEX_Push( BTL_HANDEX_DEAD_CHECK, pokeID );
        dead_check_param->pokeID = recoverPokeID;
      flwk->HANDEX_Pop( dead_check_param );
    }
  }
}
/**
 *  ねをはる：継続
 */
void cont_NeWoHaru( ServerFlow* flwk, BTL_POKEPARAM* bpp )
{
  if( !bpp->IsHPFull() )
  {
    u8 pokeID = bpp->GetID();
    // 「おおきなねっこ」対象にするため、ドレイン回復を使う
    BTL_HANDEX_PARAM_DRAIN* drain_param;
    drain_param = (BTL_HANDEX_PARAM_DRAIN*)(flwk->HANDEX_Push( BTL_HANDEX_DRAIN, pokeID ));
      drain_param->recoverPokeID = pokeID;
      drain_param->damagedPokeID = BTL_POKEID_NULL;
      drain_param->recoverHP = calc::QuotMaxHP( bpp, 16 );
      HANDEX_STR_Setup( &drain_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_NeWoHaruRecover );
      HANDEX_STR_AddArg( &drain_param->exStr, pokeID );
    flwk->HANDEX_Pop( drain_param );
  }
}
/**
 *  まきつく他：継続
 */
void cont_Bind( ServerFlow* flwk, BTL_POKEPARAM* bpp )
{
  if( !bpp->IsDead() )
  {
    u8 pokeID = bpp->GetID();

    BTL_HANDEX_PARAM_DAMAGE* param;
    BTL_SICKCONT cont = bpp->GetSickCont( pml::wazadata::WAZASICK_BIND );
    WazaID waza = (WazaID)(SICKCONT_GetParam( cont ));
    int effNo = -1;

    switch( waza ){
    case WAZANO_MAKITUKU:      effNo = BTLEFF_MAKITUKU; break;
    case WAZANO_SIMETUKERU:    effNo = BTLEFF_SIMETUKERU; break;
    case WAZANO_HONOONOUZU:    effNo = BTLEFF_HONOONOUZU; break;
    case WAZANO_MAGUMASUTOOMU: effNo = BTLEFF_MAGUMASUTOOMU; break;
    case WAZANO_KARADEHASAMU:  effNo = BTLEFF_KARADEHASAMU; break;
    case WAZANO_UZUSIO:        effNo = BTLEFF_UZUSIO; break;
    case WAZANO_SUNAZIGOKU:    effNo = BTLEFF_SUNAZIGOKU; break;
    case WAZANO_MATOWARITUKU:  effNo = BTLEFF_MATOWARITUKU; break;
    }

    u8 causePokeID = SICKCONT_GetCausePokeID( cont );
    param = (BTL_HANDEX_PARAM_DAMAGE*)(flwk->HANDEX_Push( BTL_HANDEX_DAMAGE, causePokeID ));

      param->pokeID = pokeID;
      param->damageCause = DAMAGE_CAUSE_SICK_BIND;
      param->damageCausePokeID = causePokeID;
      if( SICKCONT_GetFlag(cont) ){ // フラグONなら1/6（しめつけバンド対応）
        param->damage = calc::QuotMaxHP( bpp, 6 );
      }else{
        param->damage = calc::QuotMaxHP( bpp, 8 );
      }

      if( effNo != -1 )
      {
        param->fExEffect = true;
        param->effectNo = effNo;
        param->pos_from = flwk->Hnd_PokeIDtoPokePos( pokeID );
        param->pos_to = BTL_POS_NULL;
      }

      BTL_PRINT("バインドダメージテキスト : WAZA=%d\n", waza);

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Bind );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, waza );

    flwk->HANDEX_Pop( param );
  }
}
/**
 *  アクアリング：継続
 */
void cont_AquaRing( ServerFlow* flwk, BTL_POKEPARAM* bpp )
{
  if( !bpp->IsHPFull()
  &&  !bpp->IsDead()
  ){
    // 「おおきなねっこ」対象にするため、ドレイン回復を使う
    u8 pokeID = bpp->GetID();

    BTL_HANDEX_PARAM_DRAIN* drain_param;
    drain_param = (BTL_HANDEX_PARAM_DRAIN*)(flwk->HANDEX_Push( BTL_HANDEX_DRAIN, pokeID ));
      drain_param->recoverPokeID = pokeID;
      drain_param->damagedPokeID = BTL_POKEID_NULL;
      drain_param->recoverHP = calc::QuotMaxHP( bpp, 16 );
      HANDEX_STR_Setup( &drain_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRingRecover );
      HANDEX_STR_AddArg( &drain_param->exStr, pokeID );
    flwk->HANDEX_Pop( drain_param );
  }
}
/**
 *  アンコール：継続
 */
void cont_Encore( ServerFlow* flwk, BTL_POKEPARAM* bpp )
{
  BTL_SICKCONT cont = bpp->GetSickCont( pml::wazadata::WAZASICK_ENCORE );
  WazaID  encoreWaza = (WazaID)(SICKCONT_GetParam( cont ));

  if( bpp->WAZA_GetPP_ByNumber(encoreWaza) == 0 )
  {
    u8 pokeID = bpp->GetID();

    BTL_HANDEX_PARAM_CURE_SICK* param = (BTL_HANDEX_PARAM_CURE_SICK*)(flwk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      param->sickCode = (BtlWazaSickEx)(pml::wazadata::WAZASICK_ENCORE);
      param->pokeID[0] = pokeID;
      param->poke_cnt = 1;
    flwk->HANDEX_Pop( param );
  }
}




/**
 *  あくび：回復
 * @param akubiCont  あくびのBTL_SICKCONT
 */
void cure_Akubi( ServerFlow* flwk, BTL_POKEPARAM* bpp, const BTL_SICKCONT& akubiCont )
{
  int turns = calc::RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MAX );
  if( turns > 0 )
  {
    if( bpp->GetPokeSick() == pml::pokepara::SICK_NULL )
    {
      u8 akubi_causePokeID = SICKCONT_GetCausePokeID( akubiCont );
      u8 pokeID = bpp->GetID();
      BTL_HANDEX_PARAM_ADD_SICK* param = (BTL_HANDEX_PARAM_ADD_SICK*)(flwk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->fAlmost = true;
        param->sickID = pml::wazadata::WAZASICK_NEMURI;
        param->sickCont = SICKCONT_MakeTurn( akubi_causePokeID, turns );
        param->pokeID = pokeID;
      flwk->HANDEX_Pop( param );
    }
  }
}
/**
 *  ほろびのうた：回復
 * @param horobiNoUtaCont 「ほろびのうた」の BTL_SICKCONT
 */
void cure_HorobiNoUta( ServerFlow* flwk, BTL_POKEPARAM* bpp, const BTL_SICKCONT& horobiNoUtaCont )
{
  putHorobiCounter( flwk, bpp, 0 );
  {
    u8 pokeID = bpp->GetID();
    u8 horobiNoUta_causePokeID = SICKCONT_GetCausePokeID( horobiNoUtaCont );
    BTL_HANDEX_PARAM_KILL* param = (BTL_HANDEX_PARAM_KILL*)(flwk->HANDEX_Push( BTL_HANDEX_KILL, horobiNoUta_causePokeID ));
      param->pokeID = pokeID;
      param->deadCause = DAMAGE_CAUSE_SICK_HOROBINOUTA;
    flwk->HANDEX_Pop( param );
  }
}
/**
 *  さしおさえ：回復
 */
void cure_Sasiosae( ServerFlow* flwk, const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP* param = (BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP*)(flwk->HANDEX_Push( BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID ));
    param->pokeID = bpp->GetID();
    param->reactionType = BTL_ITEMREACTION_GEN;
  flwk->HANDEX_Pop( param );
}
/**
 *  かいふくふうじ：回復
 */
void cure_KaifukuFuji( ServerFlow* flwk, const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP* param = (BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP*)(flwk->HANDEX_Push(BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID));
    param->pokeID = bpp->GetID();
    param->reactionType = BTL_ITEMREACTION_HP;
  flwk->HANDEX_Pop( param );
}



//--------------------------------------------------------------------------------------
/**
 * 状態異常継続
*/
//--------------------------------------------------------------------------------------
void turncheck_contProc( ServerFlow* flowWk, BTL_POKEPARAM* bpp, WazaSick sick )
{
  if( !bpp->IsDead() )
  {
    u32 damage =  bpp->CalcSickDamage( sick );
    if( damage )
    {
      flowWk->SickDamageRecall( bpp, sick, damage );
    }

    BTL_N_Printf( DBGSTR_SICK_ContProc, bpp->GetID(), sick);

    switch( sick ){
    case pml::wazadata::WAZASICK_HOROBINOUTA:  cont_HorobiNoUta( flowWk, bpp ); break;
    case pml::wazadata::WAZASICK_YADORIGI:     cont_Yadorigi( flowWk, bpp ); break;
    case pml::wazadata::WAZASICK_NEWOHARU:     cont_NeWoHaru( flowWk, bpp ); break;
    case pml::wazadata::WAZASICK_BIND:         cont_Bind( flowWk, bpp ); break;
    case pml::wazadata::WAZASICK_AQUARING:     cont_AquaRing( flowWk, bpp ); break;
    case pml::wazadata::WAZASICK_ENCORE:       cont_Encore( flowWk, bpp ); break;
    }
  }
}


//----------------------------------------------------------------------------------------------
/**
 * 状態異常回復
*/
//----------------------------------------------------------------------------------------------
void turncheck_cureProc( ServerFlow* flwk, BTL_POKEPARAM* bpp, WazaSick sick, const BTL_SICKCONT& oldCont )
{
  BTL_HANDEX_STR_PARAMS  str;
  HANDEX_STR_Clear(&str);//Coverity[10974]初期化忘れ

  if( BTL_SICK_MakeDefaultCureMsg(sick, oldCont, bpp, ITEM_DUMMY_DATA, &str) )
  {
    u8 pokeID = bpp->GetID();
    BTL_HANDEX_PARAM_MESSAGE* param = (BTL_HANDEX_PARAM_MESSAGE*)(flwk->HANDEX_Push(BTL_HANDEX_MESSAGE, pokeID));
      param->str = str;
    flwk->HANDEX_Pop( param );
  }

  switch( sick ){
  case pml::wazadata::WAZASICK_AKUBI:        cure_Akubi( flwk, bpp, oldCont ); break;
  case pml::wazadata::WAZASICK_HOROBINOUTA:  cure_HorobiNoUta( flwk, bpp, oldCont ); break;
  case pml::wazadata::WAZASICK_SASIOSAE:     cure_Sasiosae( flwk, bpp ); break;
  case pml::wazadata::WAZASICK_KAIHUKUHUUJI: cure_KaifukuFuji( flwk, bpp ); break;
  }
}



} // end of namespace 'sick'







void BTL_SICK_TurnCheckCallback( BTL_POKEPARAM* bpp, WazaSick sick, const BTL_SICKCONT& oldCont, bool fCure, ServerFlow* flowWk )
{
  if( fCure ){
    sick::turncheck_cureProc( flowWk, bpp, sick, oldCont );
  }
  else{
    sick::turncheck_contProc( flowWk, bpp, sick );
  }
}


/**-------------------------------------------------------------
 * 飛行フラグチェックハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckFlying( const BTL_POKEPARAM* bpp )
{
  if( bpp->CheckSick(pml::wazadata::WAZASICK_NEWOHARU) ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
  if( bpp->CheckSick(pml::wazadata::WAZASICK_FLYING_CANCEL) ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }

  if( bpp->CheckSick(pml::wazadata::WAZASICK_TELEKINESIS)
  ||  bpp->CheckSick(pml::wazadata::WAZASICK_FLYING)
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}

/**-------------------------------------------------------------
 * 逃げ禁止チェックハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckEscapeForbit( const BTL_POKEPARAM* bpp )
{
  if( (bpp->CheckSick(pml::wazadata::WAZASICK_TOOSENBOU))
  ||  (bpp->CheckSick(pml::wazadata::WAZASICK_BIND))
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}

/**-------------------------------------------------------------
 * タイプごとの相性無効ハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckNotEffectByType( const BTL_POKEPARAM* defender )
{
  // とくていタイプを「みやぶる」状態なら、相性無効をキャンセルする
  if( defender->CheckSick(pml::wazadata::WAZASICK_MIYABURU) )
  {
    BTL_SICKCONT cont = defender->GetSickCont( pml::wazadata::WAZASICK_MIYABURU );
    pml::PokeType flatType = static_cast<pml::PokeType>(SICKCONT_GetParam( cont ));
    pml::PokeType pokeType = EVENTVAR_GetValue( BTL_EVAR_POKE_TYPE );
    if( flatType == pokeType )
    {
      if( !SICKCONT_GetFlag(cont) ){
        EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
      }else{
        EVENTVAR_RewriteValue( BTL_EVAR_FLATMASTER_FLAG, true );
      }
    }
  }

  // 「ねをはる」状態なら、地面ワザが等倍
  if( defender->CheckSick(pml::wazadata::WAZASICK_NEWOHARU) ){
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_JIMEN ){
      EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
    }
  }

  // 「うちおとす」状態なら、地面ワザが等倍
  if( defender->CheckSick(pml::wazadata::WAZASICK_FLYING_CANCEL) )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_JIMEN ){
      EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
    }
  }
}

/**-------------------------------------------------------------
 * 押し出しチェックハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckPushOutFail( ServerFlow* flowWk, const BTL_POKEPARAM* bpp )
{
  if( bpp->CheckSick(pml::wazadata::WAZASICK_NEWOHARU) )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_FAIL_FLAG, true) )
    {
      u8 pokeID = bpp->GetID();
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>
                                          (flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NeWoHaruStick );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

/**-------------------------------------------------------------
 * 攻撃ワザタイプ変更ハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckAttackType( const BTL_POKEPARAM* bpp )
{
  if( bpp->CheckSick(pml::wazadata::WAZASICK_FORCE_WAZATYPE) )
  {
    BTL_SICKCONT  cont = bpp->GetSickCont( pml::wazadata::WAZASICK_FORCE_WAZATYPE );
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, SICKCONT_GetParam(cont) );
  }
}

//=============================================================================================
/**
 * 回復時デフォルト文字列を生成
 *
 * @param   sickID
 * @param   cont
 * @param   bpp
 * @param   fUseItem
 * @param   str
 */
//=============================================================================================
bool BTL_SICK_MakeDefaultCureMsg( WazaSick sickID, const BTL_SICKCONT& oldCont, const BTL_POKEPARAM* bpp, u16 itemID, BTL_HANDEX_STR_PARAMS* str )
{
  bool fUseItem = (itemID != ITEM_DUMMY_DATA );
  int strID = sick::getCureStrID( sickID, fUseItem );
  if( strID >= 0 )
  {
    HANDEX_STR_Setup( str, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( str, bpp->GetID() );
    if( fUseItem ){
      HANDEX_STR_AddArg( str, itemID );
    }

    switch( sickID ){
    case pml::wazadata::WAZASICK_BIND:
      {
        WazaNo  waza = static_cast<WazaNo>( SICKCONT_GetParam(oldCont) );
        HANDEX_STR_AddArg( str, waza );
      }
      break;
    case pml::wazadata::WAZASICK_YADORIGI:
      HANDEX_STR_AddArg( str, WAZANO_YADORIGINOTANE );
      break;

    }
    return true;
  }
  return false;
}


//=============================================================================================
/**
 * 状態異常にかかった時のデフォルト文字列を生成
 *
 * @param   sickID
 * @param   cont
 * @param   bpp
 * @param   str
 */
//=============================================================================================
void BTL_SICK_MakeDefaultMsg( WazaSick sickID, const BTL_SICKCONT& cont, const BTL_POKEPARAM* bpp, BTL_HANDEX_STR_PARAMS* str )
{
  int strID = sick::getDefaultSickStrID( sickID, cont );
  if( strID >= 0 )
  {
    HANDEX_STR_Setup( str, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( str, bpp->GetID() );

    switch( sickID ){
    case pml::wazadata::WAZASICK_BIND:
    case pml::wazadata::WAZASICK_KANASIBARI:
      {
        WazaNo  waza = static_cast<WazaNo>( SICKCONT_GetParam(cont) );
        HANDEX_STR_AddArg( str, waza );
      }
      break;
    }
  }
  else
  {
    HANDEX_STR_Clear( str );
  }
}


//=============================================================================================
/**
 * バトンタッチで受け継ぐ状態異常コードかどうか判定
 *
 * @param   sick
 *
 * @retval  bool    受け継ぐならtrue
 */
//=============================================================================================
bool BTL_SICK_CheckBatonTouchInherit(WazaSick sick, const BTL_POKEPARAM* bpp)
{
  static const u16 sickTbl[] = {
    pml::wazadata::WAZASICK_KONRAN,
    pml::wazadata::WAZASICK_KAIHUKUHUUJI,
    pml::wazadata::WAZASICK_YADORIGI,
    pml::wazadata::WAZASICK_SASIOSAE,
    pml::wazadata::WAZASICK_HOROBINOUTA,
    pml::wazadata::WAZASICK_NEWOHARU,
    pml::wazadata::WAZASICK_TOOSENBOU,

    pml::wazadata::WAZASICK_FLYING,
    pml::wazadata::WAZASICK_TOOSENBOU,
    pml::wazadata::WAZASICK_NOROI,
    pml::wazadata::WAZASICK_AQUARING,

    pml::wazadata::WAZASICK_MUSTHIT_TARGET,
  };

  // 通常、テーブル内にあるものは引き継ぐ
  for(u32 i=0; i<GFL_NELEMS(sickTbl); ++i){
    if( sickTbl[i] == sick ){
      return true;
    }
  }

  // いえきは引き継がないポケモンがいるので特殊チェック
  if( sick == pml::wazadata::WAZASICK_IEKI )
  {
    // 絶対に無効化されてはいけない「とくせい」の持ち主は引き継がない
    TokuseiNo tokusei = static_cast<TokuseiNo>( bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ) );
    if( tables::IsNeverChangeTokusei(tokusei) ){
      return false;
    }
    return true;
  }

  // テレキネシスは引き継がないポケモンがいるので特殊チェック
  if( sick == pml::wazadata::WAZASICK_TELEKINESIS )
  {
    // メガゲンガーだけ引き継がない
    if( (bpp->GetMonsNo() == MONSNO_GENGAA)
    &&  (bpp->IsMegaEvolved())
    ){
      return false;
    }
    return true;
  }


  return false;
}


//=============================================================================================
/**
 * 状態異常継続ダメージを受ける際のデフォルトメッセージパラメータを生成
 *
 * @param   strParam
 * @param   bpp
 * @param   sickID
 *
 * @retval  BOOL
 */
//=============================================================================================
bool BTL_SICK_MakeSickDamageMsg( BTL_HANDEX_STR_PARAMS* strParam, const BTL_POKEPARAM* bpp, WazaSick sickID )
{
  int strID = sick::getWazaSickDamageStrID( sickID );
  if( strID > 0 )
  {
    HANDEX_STR_Setup( strParam, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( strParam, bpp->GetID() );
  }
  return false;
}
GFL_NAMESPACE_END(btl)

