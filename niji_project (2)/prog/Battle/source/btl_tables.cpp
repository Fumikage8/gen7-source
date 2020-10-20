//=============================================================================================
/**
 * @file  btl_tables.cpp
 * @brief ポケモンXY てきとうなサイズのテーブルをVRAM_Hに置いてアクセスする
 * @author  taya
 *
 * @date  2011.01.25  作成
 */
//=============================================================================================

#include "./btl_tables.h"
#include "btl_SideEff.h"
#include "btlv/btlv_effect_def.h"

GFL_NAMESPACE_BEGIN(btl)
namespace tables {

//----------------------------------------------------------------------------------
/**
 * テーブル要素に該当値が存在するか判定
 *
 * @param   value       判定する値
 * @param   table       テーブルアドレス
 * @param   tableElems  テーブル要素数
 *
 * @retval  bool    該当値がテーブルに含まれたらtrue
 */
//----------------------------------------------------------------------------------
bool checkTableElems( u16 value, const u16* table, u32 tableElems )
{
  u32 i;
  for(i=0; i<tableElems; ++i)
  {
    if( table[i] == value ){
      return true;
    }
  }
  return false;
}

/**
 *  アンコール除外対象のワザか判定
 */
bool IsMatchEncoreFail( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL,    WAZANO_ANKOORU,  WAZANO_OUMUGAESI,
    WAZANO_HENSIN,  WAZANO_MONOMANE, WAZANO_SUKETTI, 

    WAZANO_URUTORADASSYUATAKKU,       WAZANO_NOOMARUZENRYOKU,
    WAZANO_ZENRYOKUMUSOUGEKIRETUKEN,  WAZANO_KAKUTOUZENRYOKU,
    WAZANO_FAINARUDAIBUATAKKU,        WAZANO_HIKOUZENRYOKU,
    WAZANO_ASIDDOPOIZUNDERIITO,       WAZANO_DOKUZENRYOKU,
    WAZANO_RAIZINGURANDOOOBAA,        WAZANO_ZIMENZENRYOKU,
    WAZANO_WAARUZUENDOFOORU,          WAZANO_IWAZENRYOKU,
    WAZANO_ZETTAIHOSYOKUKAITENZAN,    WAZANO_MUSIZENRYOKU,
    WAZANO_MUGENANYAHENOIZANAI,       WAZANO_GOOSUTOZENRYOKU,
    WAZANO_TYOUZETURASENRENGEKI,      WAZANO_HAGANEZENRYOKU,
    WAZANO_DAINAMIKKUHURUHUREIMU,     WAZANO_HONOOZENRYOKU,
    WAZANO_HAIPAAAKUATORUNEEDO,       WAZANO_MIZUZENRYOKU,
    WAZANO_BURUUMUSYAINEKUSUTORA,     WAZANO_KUSAZENRYOKU,
    WAZANO_SUPAAKINGUGIGABORUTO,      WAZANO_DENKIZENRYOKU,
    WAZANO_MAKISIMAMUSAIBUREIKAA,     WAZANO_ESUPAAZENRYOKU,
    WAZANO_REIZINGUZIOHURIIZU,        WAZANO_KOORIZENRYOKU,
    WAZANO_ARUTHIMETTODORAGONBAAN,    WAZANO_DORAGONZENRYOKU,
    WAZANO_BURAKKUHOORUIKURIPUSU,     WAZANO_AKUZENRYOKU,
    WAZANO_RABURIISUTAAINPAKUTO,      WAZANO_FEARIIZENRYOKU,
    WAZANO_HISSATUNOPIKATYUUTO,       WAZANO_SYADOUAROOZUSUTORAIKU,   WAZANO_HAIPAADAAKUKURASSYAA,
    WAZANO_WADATUMINOSINFONIA,        WAZANO_GAADHIANDEAROORA,        WAZANO_HAISINSENYOUZENRYOKU,
    WAZANO_RAITONINGUSAAHURAIDO,      WAZANO_HONKIWODASUKOUGEKI,      WAZANO_SEBUNEBORUBUUSUTO,
    WAZANO_ORIZINZUSUUPAANOVHA,       WAZANO_SATOPIKASENYOUZENRYOKU,
    WAZANO_ZENRYOKUSUTOONEZZI,        WAZANO_SINNEKUROZUMAZETTO,      WAZANO_ZENRYOKUMETEODORAIBU,
    WAZANO_ZENRYOKUSYADOOREI,         WAZANO_ZENRYOKUZYARETUKU,       WAZANO_ZENRYOKUSUKEIRUNOIZU,

    // @fix GFNMCat[5768] バトル自動テストでとまり
    // 派生技に対するアンコールは失敗する
    WAZANO_YUBIWOHURU,  WAZANO_SIZENNOTIKARA,  WAZANO_NEKONOTE,
    WAZANO_NEGOTO,      WAZANO_OUMUGAESI,      WAZANO_SAKIDORI,
    WAZANO_MANEKKO,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}
/**
 *  さきどり失敗ワザ判定
 */
bool IsSakidoriFailWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL, WAZANO_MIRAAKOOTO, WAZANO_KAUNTAA, WAZANO_METARUBAASUTO,
    WAZANO_KIAIPANTI, WAZANO_SAKIDORI, WAZANO_DOROBOU, WAZANO_HOSIGARU,
    WAZANO_OSYABERI, WAZANO_WARUAGAKI, WAZANO_GEPPU,
    WAZANO_TORAPPUSHERU, WAZANO_KUTIBASIKYANON,
    WAZANO_HISSATUNOPIKATYUUTO,       WAZANO_SYADOUAROOZUSUTORAIKU,   WAZANO_HAIPAADAAKUKURASSYAA,
    WAZANO_WADATUMINOSINFONIA,        WAZANO_GAADHIANDEAROORA,        WAZANO_HAISINSENYOUZENRYOKU,
    WAZANO_RAITONINGUSAAHURAIDO,      WAZANO_HONKIWODASUKOUGEKI,      WAZANO_SEBUNEBORUBUUSUTO,
    WAZANO_ORIZINZUSUUPAANOVHA,       WAZANO_SATOPIKASENYOUZENRYOKU,  WAZANO_ZENRYOKUSUTOONEZZI,

  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  時間差攻撃ワザ判定
 */
bool IsDelayAttackWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL, WAZANO_MIRAIYOTI, WAZANO_HAMETUNONEGAI,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  Zワザ判定
 */
bool IsZWaza( WazaID waza )
{
  if( IsZWaza_General( waza ) ||
      IsZWaza_Special( waza ) )
  {
    return true;
  }

  return false;
}

/**
 * @brief 汎用ｚ技判定
 */
bool IsZWaza_General( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_URUTORADASSYUATAKKU,       WAZANO_NOOMARUZENRYOKU,
    WAZANO_ZENRYOKUMUSOUGEKIRETUKEN,  WAZANO_KAKUTOUZENRYOKU,
    WAZANO_FAINARUDAIBUATAKKU,        WAZANO_HIKOUZENRYOKU,
    WAZANO_ASIDDOPOIZUNDERIITO,       WAZANO_DOKUZENRYOKU,
    WAZANO_RAIZINGURANDOOOBAA,        WAZANO_ZIMENZENRYOKU,
    WAZANO_WAARUZUENDOFOORU,          WAZANO_IWAZENRYOKU,
    WAZANO_ZETTAIHOSYOKUKAITENZAN,    WAZANO_MUSIZENRYOKU,
    WAZANO_MUGENANYAHENOIZANAI,       WAZANO_GOOSUTOZENRYOKU,
    WAZANO_TYOUZETURASENRENGEKI,      WAZANO_HAGANEZENRYOKU,
    WAZANO_DAINAMIKKUHURUHUREIMU,     WAZANO_HONOOZENRYOKU,
    WAZANO_HAIPAAAKUATORUNEEDO,       WAZANO_MIZUZENRYOKU,
    WAZANO_BURUUMUSYAINEKUSUTORA,     WAZANO_KUSAZENRYOKU,
    WAZANO_SUPAAKINGUGIGABORUTO,      WAZANO_DENKIZENRYOKU,
    WAZANO_MAKISIMAMUSAIBUREIKAA,     WAZANO_ESUPAAZENRYOKU,
    WAZANO_REIZINGUZIOHURIIZU,        WAZANO_KOORIZENRYOKU,
    WAZANO_ARUTHIMETTODORAGONBAAN,    WAZANO_DORAGONZENRYOKU,
    WAZANO_BURAKKUHOORUIKURIPUSU,     WAZANO_AKUZENRYOKU,
    WAZANO_RABURIISUTAAINPAKUTO,      WAZANO_FEARIIZENRYOKU,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 * @brief 専用ｚ技判定
 */
bool IsZWaza_Special( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_HISSATUNOPIKATYUUTO,       WAZANO_SYADOUAROOZUSUTORAIKU,   WAZANO_HAIPAADAAKUKURASSYAA,
    WAZANO_WADATUMINOSINFONIA,        WAZANO_GAADHIANDEAROORA,        WAZANO_HAISINSENYOUZENRYOKU,
    WAZANO_RAITONINGUSAAHURAIDO,      WAZANO_HONKIWODASUKOUGEKI,      WAZANO_SEBUNEBORUBUUSUTO,
    WAZANO_ORIZINZUSUUPAANOVHA,       WAZANO_SATOPIKASENYOUZENRYOKU,
    // @fix MMCat[58] ぽかぼかフレンドタイムの威力表示が不適切
    WAZANO_SINNEKUROZUMAZETTO, WAZANO_ZENRYOKUMETEODORAIBU, WAZANO_ZENRYOKUSYADOOREI,
    WAZANO_ZENRYOKUZYARETUKU, WAZANO_ZENRYOKUSUTOONEZZI, WAZANO_ZENRYOKUSUKEIRUNOIZU,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  プレッシャー対象ワザ判定
 */
bool IsPressureEffectiveWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_YOKODORI, WAZANO_HUUIN,
    WAZANO_MAKIBISI, WAZANO_DOKUBISI, WAZANO_SUTERUSUROKKU,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}
/**
 * フェイントなどで無効化できるタイプのサイドエフェクトかどうかを判定
 */
bool IsGuardTypeSideEffect( BtlSideEffect eff )
{
  static const u16 table[] = {
    BTL_SIDEEFF_WIDEGUARD,    BTL_SIDEEFF_FASTGUARD,
    BTL_SIDEEFF_TATAMIGAESHI, BTL_SIDEEFF_TRICKGUARD,
  };

  return checkTableElems( eff, table, GFL_NELEMS(table) );
}

/**
 *  合体ワザ判定
 */
bool IsCombiWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_HONOONOTIKAI, WAZANO_KUSANOTIKAI, WAZANO_MIZUNOTIKAI,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  AIClientがアイテム使用する際の優先度を返す（0:最低）
 */
u32 GetAIItemPriority( u16 itemID )
{
// ↑優先度 高
  static const u16 Pri_6[] = { ITEM_KAIHUKUNOKUSURI, ITEM_DUMMY_DATA };
  static const u16 Pri_5[] = { ITEM_NEMUKEZAMASI,    ITEM_KOORINAOSI,      ITEM_DUMMY_DATA };
  static const u16 Pri_4[] = { ITEM_KIZUGUSURI,      ITEM_SUGOIKIZUGUSURI, ITEM_MANTANNOKUSURI, ITEM_DUMMY_DATA };
  static const u16 Pri_3[] = { ITEM_NANDEMONAOSI,    ITEM_DUMMY_DATA };
  static const u16 Pri_2[] = { ITEM_MAHINAOSI,       ITEM_DUMMY_DATA };
  static const u16 Pri_1[] = { ITEM_DOKUKESI,        ITEM_YAKEDONAOSI, ITEM_DUMMY_DATA };
// ↓優先度 低

  static const u16* table[] = {
    Pri_1,  Pri_2,  Pri_3,  Pri_4, Pri_5, Pri_6,
  };

  for(u32 i=0; i<GFL_NELEMS(table); ++i)
  {
    for(u32 j=0; table[i][j] != ITEM_DUMMY_DATA; ++j)
    {
      if( table[i][j] == itemID ){
        return 1 + i;
      }
    }
  }

  return 0;
}


/**
 *  アイテムコールで発動してはいけないアイテムか判定
 */
bool CheckItemCallNoEffect( u16 itemID )
{
  static const u16 table[] = {
    ITEM_KIAINOHATIMAKI,  ITEM_PAWAHURUHAABU,   ITEM_DASSYUTUBOTAN,
    ITEM_ZYAPONOMI,       ITEM_RENBUNOMI,       ITEM_IBANNOMI,
    ITEM_SENSEINOTUME,    ITEM_KYUUKON,         ITEM_ZYUUDENTI,
  };

  return checkTableElems( itemID, table, GFL_NELEMS(table) );
}


/**
 *  使用する対象ポケモンの選択を行う必要が無いアイテムか判別
 *
 *  ※トレーナー戦は使用できないという判別に使用させてもらいます 2013/04/23 by nakahiro
 */
bool IsNoTargetItem( u16 itemID )
{
  static const u16 table[] = {
    ITEM_ENEKONOSIPPO,  ITEM_PIPPININGYOU, ITEM_POKEZYARASI,
  };

  return checkTableElems( itemID, table, GFL_NELEMS(table) );
}

/**
*  使用する対象ポケモンの選択を行う必要が無いロトポンアイテムか判別
*  //@momiji追加分
*/
bool IsRotoponItem(u16 itemID)
{
  static const u16 table[] = {
    ITEM_KAIHUKUPON, ITEM_PIIPIIPON, ITEM_HOZYOPON, ITEM_TUKAMAEPON,
  };

  return checkTableElems(itemID, table, GFL_NELEMS(table));
}

/**
 *  「なりきり」に失敗するとくせい判定
 */
bool CheckNarikiriFailTokusei( TokuseiNo tokuseiID )
{
  static const u16 table[] = {
    TOKUSEI_TOREESU,       TOKUSEI_TENKIYA,         TOKUSEI_MARUTITAIPU,
    TOKUSEI_HURAWAAGIHUTO, TOKUSEI_DARUMAMOODO,     TOKUSEI_IRYUUZYON,
    TOKUSEI_KAWARIMONO,    TOKUSEI_HUSIGINAMAMORI,  TOKUSEI_BATORUSUITTI,
    TOKUSEI_DARUMAMOODO,   TOKUSEI_arSISUTEMU,      TOKUSEI_BAKENOKAWA,
    TOKUSEI_GYOGUN,        TOKUSEI_KIZUNAHENGE,     TOKUSEI_SUWAAMUTHENZI,
    TOKUSEI_ZETTAINEMURI,  TOKUSEI_RIMITTOSIIRUDO,  TOKUSEI_RESIIBAA,
    TOKUSEI_KAGAKUNOTIKARA,
  };

  if( IsNeverChangeTokusei(tokuseiID) ){
    return true;
  }

  return checkTableElems( tokuseiID, table, GFL_NELEMS(table) );
}

/**
 *  「レシーバー」に失敗するとくせい判定
 */

bool CheckReceiverFailTokusei( TokuseiNo tokuseiID )
{
  // 「なりきり」と一緒
  return CheckNarikiriFailTokusei( tokuseiID );
}

/**
 *  「なかまづくり」に失敗するとくせい判定
 */

bool CheckNakamaDukuriFailTokusei( TokuseiNo tokuseiID )
{
  // 「トレース」と一緒
  return CheckTraceFailTokusei( tokuseiID );
}

/**
 *  トレースできないとくせい判定
 */
bool CheckTraceFailTokusei( TokuseiNo tokuseiID )
{
  // トレースの場合は「ふしぎなまもり」を許可する
  if( tokuseiID == TOKUSEI_HUSIGINAMAMORI ){
    return false;
  }

  // あとは「なりきり」と一緒
  return CheckNarikiriFailTokusei( tokuseiID );
}

/**
 *  スキルスワップできないとくせい判定（相手・自分のどちらか一方でも該当したらダメ）
 */
bool CheckSkillSwapFailTokusei( TokuseiNo tokuseiID )
{
  static const u16 table[] = {
    TOKUSEI_HUSIGINAMAMORI, TOKUSEI_MARUTITAIPU,   TOKUSEI_IRYUUZYON,
    TOKUSEI_BATORUSUITTI,   TOKUSEI_DARUMAMOODO,   TOKUSEI_arSISUTEMU,  
    TOKUSEI_BAKENOKAWA,     TOKUSEI_GYOGUN,        TOKUSEI_KIZUNAHENGE, 
    TOKUSEI_SUWAAMUTHENZI,  TOKUSEI_ZETTAINEMURI,  TOKUSEI_RIMITTOSIIRUDO,

  };

  if( IsNeverChangeTokusei(tokuseiID) ){
    return true;
  }

  return checkTableElems( tokuseiID, table, GFL_NELEMS(table) );
}

/**
 *  絶対に書き換わってはいけない・無効化されてはいけないとくせい判定
 */
bool IsNeverChangeTokusei( TokuseiNo tokuseiID )
{
  static const u16 table[] = {
    TOKUSEI_MARUTITAIPU,  TOKUSEI_BATORUSUITTI,   TOKUSEI_DARUMAMOODO,    
    TOKUSEI_arSISUTEMU,   TOKUSEI_BAKENOKAWA,     TOKUSEI_GYOGUN,         
    TOKUSEI_KIZUNAHENGE,  TOKUSEI_SUWAAMUTHENZI,  TOKUSEI_ZETTAINEMURI,   
    TOKUSEI_RIMITTOSIIRUDO,

  };
  return checkTableElems( tokuseiID, table, GFL_NELEMS(table) );
}

/**
 *  アルセウスのプレート判定
 */
bool IsMatchAruseusPlate( u16 itemID )
{
  static const u16 table[] = {
    ITEM_HINOTAMAPUREETO,
    ITEM_SIZUKUPUREETO,
    ITEM_IKAZUTIPUREETO,
    ITEM_MIDORINOPUREETO,
    ITEM_TURARANOPUREETO,
    ITEM_KOBUSINOPUREETO,
    ITEM_MOUDOKUPUREETO,
    ITEM_DAITINOPUREETO,
    ITEM_AOZORAPUREETO,
    ITEM_HUSIGINOPUREETO,
    ITEM_TAMAMUSIPUREETO,
    ITEM_GANSEKIPUREETO,
    ITEM_MONONOKEPUREETO,
    ITEM_RYUUNOPUREETO,
    ITEM_KOWAMOTEPUREETO,
    ITEM_KOUTETUPUREETO,
    ITEM_SEIREIPUREETO,
  };

  return checkTableElems( itemID, table, GFL_NELEMS(table) );
}

/**
 *  グリプス２のチップ判定
 */
bool IsMatchGuripusu2Chip( u16 itemID )
{
  static const u16 table[] = {
    ITEM_FAITOMEMORI,
    ITEM_HURAINGUMEMORI,
    ITEM_POIZUNMEMORI,
    ITEM_GURAUNDOMEMORI,
    ITEM_ROKKUMEMORI,
    ITEM_BAGUMEMORI,
    ITEM_GOOSUTOMEMORI,
    ITEM_SUTIIRUMEMORI,
    ITEM_FAIYAAMEMORI,
    ITEM_UOOTAAMEMORI,
    ITEM_GURASUMEMORI,
    ITEM_EREKUTOROMEMORI,
    ITEM_SAIKIKKUMEMORI,
    ITEM_AISUMEMORI,
    ITEM_DORAGONMEMORI,
    ITEM_DAAKUMEMORI,
    ITEM_FEARIIMEMORI,
  };

  return checkTableElems( itemID, table, GFL_NELEMS(table) );
}

/**
 *  インセクタのカセット判定
 */
bool IsMatchInsectaCasette( u16 itemID )
{
  static const u16 table[] = {
    ITEM_AKUAKASETTO,
    ITEM_INAZUMAKASETTO,
    ITEM_BUREIZUKASETTO,
    ITEM_HURIIZUKASETTO,
  };

  return checkTableElems( itemID, table, GFL_NELEMS(table) );
}


/**
 *  かたやぶりが無効化する対象のとくせい判定
 */
bool IsMatchKatayaburiTarget( TokuseiNo tokuseiID )
{
  static const u16 table[] = {
    TOKUSEI_HUSIGINAMAMORI, TOKUSEI_BOUON,        TOKUSEI_HUYUU,
    TOKUSEI_SUNAGAKURE,     TOKUSEI_YUKIGAKURE,   TOKUSEI_TYOSUI,
    TOKUSEI_KABUTOAAMAA,    TOKUSEI_HIRAISIN,     TOKUSEI_YOBIMIZU,
    TOKUSEI_SHERUAAMAA,     TOKUSEI_TENNEN,       TOKUSEI_HIRAISIN,
    TOKUSEI_YOBIMIZU,       TOKUSEI_KYUUBAN,      TOKUSEI_TANZYUN,
    TOKUSEI_TIDORIASI,      TOKUSEI_HAADOROKKU,   TOKUSEI_FIRUTAA,
    TOKUSEI_MORAIBI,        TOKUSEI_DENKIENZIN,   TOKUSEI_NENTYAKU,
    TOKUSEI_HUSIGINAUROKO,  TOKUSEI_ATUISIBOU,    TOKUSEI_TAINETU,
    TOKUSEI_SIROIKEMURI,    TOKUSEI_KURIABODHI,   TOKUSEI_SURUDOIME,
    TOKUSEI_KAIRIKIBASAMI,  TOKUSEI_SEISINRYOKU,  TOKUSEI_RINPUN,
    TOKUSEI_GANZYOU,        TOKUSEI_SIMERIKE,     TOKUSEI_DONKAN,
    TOKUSEI_ZYUUNAN,        TOKUSEI_MAIPEESU,     TOKUSEI_MIZUNOBEERU,
    TOKUSEI_RIIHUGAADO,     TOKUSEI_HUMIN,        TOKUSEI_HATOMUNE,
    TOKUSEI_YARUKI,         TOKUSEI_MENEKI,       TOKUSEI_MAGUMANOYOROI,
    TOKUSEI_AMANOZYAKU,     TOKUSEI_HURENDOGAADO, TOKUSEI_MARUTISUKEIRU,
    TOKUSEI_TEREPASII,      TOKUSEI_HATOMUNE,     TOKUSEI_MIRAKURUSUKIN,
    TOKUSEI_MAZIKKUMIRAA,   TOKUSEI_SOUSYOKU,     TOKUSEI_TIKUDEN,
    TOKUSEI_KANSOUHADA,     TOKUSEI_HURAWAAGIHUTO,TOKUSEI_HEVHIMETARU,
    TOKUSEI_RAITOMETARU,
    TOKUSEI_AROMABEERU,     TOKUSEI_HURAWAABEERU, TOKUSEI_FAAKOOTO,
    TOKUSEI_BOUDAN,         TOKUSEI_SUIITOBEERU,  TOKUSEI_KUSANOKEGAWA,
    TOKUSEI_DAAKUOORA,      TOKUSEI_FEARIIOORA,   TOKUSEI_OORABUREIKU,
    TOKUSEI_BOUZIN,         TOKUSEI_MOHUMOHU,     TOKUSEI_BAKENOKAWA,     
    TOKUSEI_SUIHOU,         TOKUSEI_ZYOOUNOIGEN,  TOKUSEI_BIBIDDOBODHI,

  };

  return checkTableElems( tokuseiID, table, GFL_NELEMS(table) );

}

/**
 *  タイプ書き換え失敗するモンスターナンバー判定
 */
bool IsTypeChangeForbidPoke( u16 monsno )
{
  static const u16 table[] = {
    MONSNO_ARUSEUSU, MONSNO_GURIPUSU2,
  };
  return checkTableElems( monsno, table, GFL_NELEMS(table) );
}

/**
 *  ユーザコンフィグ「わざエフェクト：みない」でスキップするエフェクト指定
 */
bool IsEffectDisableType( u16 effectNo )
{
  static const u16 table[] = {
    BTLEFF_NEMURI,         BTLEFF_MAHI,          BTLEFF_DOKU,       BTLEFF_YAKEDO,
    BTLEFF_KONRAN,         BTLEFF_MEROMERO,      BTLEFF_NOROI,      BTLEFF_AKUMU,
    BTLEFF_SHOOTER_EFFECT, BTLEFF_USE_ITEM,      BTLEFF_HP_RECOVER, BTLEFF_SIMETUKERU,
    BTLEFF_YADORIGI,       BTLEFF_MAKITUKU,      BTLEFF_KOORI,      BTLEFF_HONOONOUZU,
    BTLEFF_MAGUMASUTOOMU,  BTLEFF_KARADEHASAMU,  BTLEFF_UZUSIO,     BTLEFF_SUNAZIGOKU,
    BTLEFF_RAINBOW,        BTLEFF_MOOR,          BTLEFF_BURNING,    BTLEFF_GUARD,
    
    //@バトル変更箇所
    //SBTS[69] kujira残留バグのまとわりつくがエフェクトスキップで表示される
    BTLEFF_MATOWARITUKU,
    // momiji追加分
    BTLEFF_ROTOM_POWER, BTLEFF_Z_ROTOM_POWER, BTLEFF_PP_RECOVER,
  };

  return checkTableElems( effectNo, table, GFL_NELEMS(table) );
}
//----------------------------------------------------------------
/**
 *  メンタルハーブで治すことのできる状態異常テーブル
 */
//----------------------------------------------------------------
static const WazaSick MentalSickTbl[] = {
  pml::wazadata::WAZASICK_MEROMERO,      pml::wazadata::WAZASICK_ICHAMON,   pml::wazadata::WAZASICK_KANASIBARI,
  pml::wazadata::WAZASICK_KAIHUKUHUUJI,  pml::wazadata::WAZASICK_ENCORE,    pml::wazadata::WAZASICK_TYOUHATSU,
};

//=============================================================================================
/**
 * メンタルハーブで治す状態異常テーブル順番アクセス
 *
 * @param   idx   0〜
 *
 * @retval  WazaSick    有効Idxの時、状態異常コード／それ以外 pml::wazadata::WAZASICK_NULL
 */
//=============================================================================================
WazaSick  GetMentalSickID( u32 idx )
{
  if( idx < GFL_NELEMS(MentalSickTbl) ){
    return MentalSickTbl[idx];
  }
  return pml::wazadata::WAZASICK_NULL;
}
//=============================================================================================
/**
 * メンタルハーブで治すことの出来る状態異常かどうか判定
 *
 * @param   sickID
 *
 * @retval  bool
 */
//=============================================================================================
bool   IsMentalSickID( WazaSick sickID  )
{
  for(uint32_t i=0; i<GFL_NELEMS(MentalSickTbl); ++i)
  {
    if( MentalSickTbl[i] == sickID ){
      return true;
    }
  }
  return false;
}



//=============================================================================================
/**
 * ターンチェック処理順に状態異常コード取得
 *
 * @param   idx   処理順（0〜）
 *
 * @retval  WazaSick    有効idxの時、状態異常コード／それ以外 pml::wazadata::WAZASICK_NULL;
 */
//=============================================================================================
WazaSick  GetTurnCheckWazaSickByOrder( u32 idx )
{
  static const WazaSick table[] = {

    // 回復系
    pml::wazadata::WAZASICK_AQUARING,        ///< アクアリング
    pml::wazadata::WAZASICK_NEWOHARU,        ///< ねをはる

    // ドレイン系
    pml::wazadata::WAZASICK_YADORIGI,        ///< やどりぎのたね

    // ダメージ系
    pml::wazadata::WAZASICK_DOKU,
    pml::wazadata::WAZASICK_YAKEDO,
    pml::wazadata::WAZASICK_AKUMU,
    pml::wazadata::WAZASICK_NOROI,
    pml::wazadata::WAZASICK_BIND,

    // 効果切れチェック系
    pml::wazadata::WAZASICK_TYOUHATSU,
    pml::wazadata::WAZASICK_ICHAMON,
    pml::wazadata::WAZASICK_ENCORE,          ///< 直前に使ったワザしか出せない
    pml::wazadata::WAZASICK_KANASIBARI,
    pml::wazadata::WAZASICK_FLYING,          ///< でんじふゆう
    pml::wazadata::WAZASICK_TELEKINESIS,     ///< テレキネシス
    pml::wazadata::WAZASICK_KAIHUKUHUUJI,    ///< かいふくふうじ
    pml::wazadata::WAZASICK_SASIOSAE,        ///< さしおさえ（回復封じが切れてから差し押さえが切れると、木の実とかで回復できるかも）
    pml::wazadata::WAZASICK_MUSTHIT,         ///< ワザ必中状態
    pml::wazadata::WAZASICK_MUSTHIT_TARGET,  ///< 特定相手に対してのみ必中状態
    pml::wazadata::WAZASICK_FUNJIN,          ///< ふんじん
    pml::wazadata::WAZASICK_FORCE_WAZATYPE,  ///< ワザが強制的に特定タイプになる
    pml::wazadata::WAZASICK_SAWAGU,          ///< さわぐ
    pml::wazadata::WAZASICK_ABARERU,         ///< あばれる・はなびらのまい・げきりん
    pml::wazadata::WAZASICK_TOGISUMASU,      ///< とぎすます
    pml::wazadata::WAZASICK_ZIGOKUDUKI,      ///< じごくづき

    // 特殊効果系
    pml::wazadata::WAZASICK_AKUBI,

    // ほろびのうた
    pml::wazadata::WAZASICK_HOROBINOUTA,     ///< ほろびのうた

    // ターンチェック処理が無いはず系
    pml::wazadata::WAZASICK_MAHI,
    pml::wazadata::WAZASICK_NEMURI,
    pml::wazadata::WAZASICK_KOORI,
    pml::wazadata::WAZASICK_MEROMERO,
    pml::wazadata::WAZASICK_KONRAN,
    pml::wazadata::WAZASICK_IEKI,            ///< とくせい無効化
    pml::wazadata::WAZASICK_MIYABURU,        ///< みやぶられて相性ゼロでも等倍で殴られる＆回避率上昇が無効
    pml::wazadata::WAZASICK_TOOSENBOU,       ///< にげる・いれかえが出来ない
    pml::wazadata::WAZASICK_HANEYASUME,      ///< はねやすめ（飛行タイプ->無属性化）
    pml::wazadata::WAZASICK_WAZALOCK,        ///< 直前に使ったワザしか出せない上、行動も選択できない
    pml::wazadata::WAZASICK_TAMELOCK,        ///< 直前に使ったワザしか出せない上、行動も選択できない（溜めワザ専用）
    pml::wazadata::WAZASICK_KODAWARI,        ///< 最初に使ったワザしか選択できない
    pml::wazadata::WAZASICK_FLYING_CANCEL,   ///< ふゆう状態になれない
    pml::wazadata::WAZASICK_FREEFALL,        ///< フリーフォール
    pml::wazadata::WAZASICK_HITRATIO_UP,     ///< 命中率上昇

  };

  if( idx < GFL_NELEMS(table) ){
    return table[idx];
  }
  return pml::wazadata::WAZASICK_NULL;
}

/**
 *  派生ワザ失敗共通（※注意：ゆびをふるは別テーブルです！）
 */
bool checkHaseiOmmitCommon( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_YUBIWOHURU,    WAZANO_NEGOTO,
    WAZANO_NEKONOTE,      WAZANO_MANEKKO,       WAZANO_SAKIDORI,
    WAZANO_OUMUGAESI,     WAZANO_SIZENNOTIKARA, WAZANO_OSYABERI,
    WAZANO_WARUAGAKI,     WAZANO_SUKETTI,       WAZANO_MONOMANE,
    WAZANO_GEPPU,         WAZANO_OIWAI,         WAZANO_TEWOTUNAGU,
    WAZANO_HISSATUNOPIKATYUUTO,       WAZANO_SYADOUAROOZUSUTORAIKU,   WAZANO_HAIPAADAAKUKURASSYAA,
    WAZANO_WADATUMINOSINFONIA,        WAZANO_GAADHIANDEAROORA,        WAZANO_HAISINSENYOUZENRYOKU,
    WAZANO_RAITONINGUSAAHURAIDO,      WAZANO_HONKIWODASUKOUGEKI,      WAZANO_SEBUNEBORUBUUSUTO,
    WAZANO_ORIZINZUSUUPAANOVHA,       WAZANO_SATOPIKASENYOUZENRYOKU,  WAZANO_ZENRYOKUSUTOONEZZI,
    WAZANO_SINNEKUROZUMAZETTO,        WAZANO_ZENRYOKUMETEODORAIBU,    WAZANO_ZENRYOKUSYADOOREI,
    WAZANO_ZENRYOKUZYARETUKU,         WAZANO_ZENRYOKUSUKEIRUNOIZU,

  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  「ねごと」で出ないワザ判定
 */
bool IsNegotoOmmit( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_SAWAGU,      WAZANO_ROKETTOZUTUKI, WAZANO_GODDOBAADO,
    WAZANO_SOORAABIIMU, WAZANO_GAMAN,         WAZANO_KAMAITATI,
    WAZANO_SORAWOTOBU,  WAZANO_TOBIHANERU,    WAZANO_SYADOODAIBU,
    WAZANO_KIAIPANTI,   WAZANO_HURIIFOORU,    WAZANO_GOOSUTODAIBU,
    WAZANO_TORAPPUSHERU,WAZANO_KUTIBASIKYANON,WAZANO_ANAWOHORU,
    WAZANO_DAIBINGU,    WAZANO_HURIIZUBORUTO, WAZANO_KOORUDOHUREA,
    WAZANO_ZIOKONTOROORU, WAZANO_SOORAABUREEDO,
  };

  if( checkHaseiOmmitCommon(waza) ){
    return true;
  }

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  「ネコのて」で出ないワザ判定
 */
bool IsNekoNoteOmmit( WazaID waza )
{
  static const u16 table[] = {
      WAZANO_KAUNTAA,       WAZANO_MIRAAKOOTO,  WAZANO_MAMORU,      WAZANO_MIKIRI,
      WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE, WAZANO_IKARINOKONA,
      WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
      WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_SURIKAE,
      WAZANO_FEINTO,        WAZANO_KIAIPANTI,   WAZANO_HENSIN,
      WAZANO_GIHUTOPASU,
      WAZANO_DORAGONTEERU,  WAZANO_TOMOENAGE,   // この二つは「いたずらごころ」との組み合わせでヤバそうなので
      WAZANO_HOERU,         WAZANO_HUKITOBASI,  // この二つもです（XYより）
      WAZANO_NIIDORUGAADO,  WAZANO_KINGUSIIRUDO, WAZANO_TATAMIGAESI,
      WAZANO_SORAWOTOBU,    WAZANO_HURIIFOORU,   WAZANO_ANAWOHORU, WAZANO_DAIBINGU,
      WAZANO_TOBIHANERU,    WAZANO_SYADOODAIBU,  WAZANO_GOOSUTODAIBU,
      WAZANO_SUPOTTORAITO,  WAZANO_TORAPPUSHERU, WAZANO_KUTIBASIKYANON,
      WAZANO_TOOTIKA,

  };

  if( checkHaseiOmmitCommon(waza) ){
    return true;
  }

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  「まねっこ」で出ないワザ判定
 */
bool IsManekkoOmmit( WazaID waza )
{
  static const u16 table[] = {
      WAZANO_KAUNTAA,       WAZANO_MIRAAKOOTO,  WAZANO_MAMORU,      WAZANO_MIKIRI,
      WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE,  WAZANO_IKARINOKONA,
      WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
      WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_SURIKAE,
      WAZANO_FEINTO,        WAZANO_KIAIPANTI,   WAZANO_HENSIN,
      WAZANO_GIHUTOPASU,
      WAZANO_DORAGONTEERU,  WAZANO_TOMOENAGE,   // この二つは「いたずらごころ」との組み合わせでヤバそうなので
      WAZANO_HOERU,         WAZANO_HUKITOBASI,  // この二つもです（XYより）
      WAZANO_NIIDORUGAADO,  WAZANO_KINGUSIIRUDO, WAZANO_TATAMIGAESI,
      WAZANO_SUPOTTORAITO,  WAZANO_TORAPPUSHERU, WAZANO_KUTIBASIKYANON,
      WAZANO_TOOTIKA,

  };

  if( checkHaseiOmmitCommon(waza) ){
    return true;
  }

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  ものまね失敗対象のワザ判定
 */
bool IsMatchMonomaneFail( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL,    WAZANO_SUKETTI,   WAZANO_MONOMANE,
    WAZANO_HENSIN,  WAZANO_WARUAGAKI, WAZANO_OSYABERI,

    WAZANO_URUTORADASSYUATAKKU,       WAZANO_NOOMARUZENRYOKU,
    WAZANO_ZENRYOKUMUSOUGEKIRETUKEN,  WAZANO_KAKUTOUZENRYOKU,
    WAZANO_FAINARUDAIBUATAKKU,        WAZANO_HIKOUZENRYOKU,
    WAZANO_ASIDDOPOIZUNDERIITO,       WAZANO_DOKUZENRYOKU,
    WAZANO_RAIZINGURANDOOOBAA,        WAZANO_ZIMENZENRYOKU,
    WAZANO_WAARUZUENDOFOORU,          WAZANO_IWAZENRYOKU,
    WAZANO_ZETTAIHOSYOKUKAITENZAN,    WAZANO_MUSIZENRYOKU,
    WAZANO_MUGENANYAHENOIZANAI,       WAZANO_GOOSUTOZENRYOKU,
    WAZANO_TYOUZETURASENRENGEKI,      WAZANO_HAGANEZENRYOKU,
    WAZANO_DAINAMIKKUHURUHUREIMU,     WAZANO_HONOOZENRYOKU,
    WAZANO_HAIPAAAKUATORUNEEDO,       WAZANO_MIZUZENRYOKU,
    WAZANO_BURUUMUSYAINEKUSUTORA,     WAZANO_KUSAZENRYOKU,
    WAZANO_SUPAAKINGUGIGABORUTO,      WAZANO_DENKIZENRYOKU,
    WAZANO_MAKISIMAMUSAIBUREIKAA,     WAZANO_ESUPAAZENRYOKU,
    WAZANO_REIZINGUZIOHURIIZU,        WAZANO_KOORIZENRYOKU,
    WAZANO_ARUTHIMETTODORAGONBAAN,    WAZANO_DORAGONZENRYOKU,
    WAZANO_BURAKKUHOORUIKURIPUSU,     WAZANO_AKUZENRYOKU,
    WAZANO_RABURIISUTAAINPAKUTO,      WAZANO_FEARIIZENRYOKU,
    WAZANO_HISSATUNOPIKATYUUTO,       WAZANO_SYADOUAROOZUSUTORAIKU,   WAZANO_HAIPAADAAKUKURASSYAA,
    WAZANO_WADATUMINOSINFONIA,        WAZANO_GAADHIANDEAROORA,        WAZANO_HAISINSENYOUZENRYOKU,
    WAZANO_RAITONINGUSAAHURAIDO,      WAZANO_HONKIWODASUKOUGEKI,      WAZANO_SEBUNEBORUBUUSUTO,
    WAZANO_ORIZINZUSUUPAANOVHA,       WAZANO_SATOPIKASENYOUZENRYOKU,  WAZANO_ZENRYOKUSUTOONEZZI,
    WAZANO_SINNEKUROZUMAZETTO,        WAZANO_ZENRYOKUMETEODORAIBU,    WAZANO_ZENRYOKUSYADOOREI,
    WAZANO_ZENRYOKUZYARETUKU,         WAZANO_ZENRYOKUSUKEIRUNOIZU,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  「さいはい」で出ないワザ判定
 */
bool IsSaihaiOmmit( WazaID waza )
{
  static const u16 table[] = 
  {
    WAZANO_SAIHAI,         WAZANO_GAMAN,          WAZANO_KAMAITATI, 
    WAZANO_SORAWOTOBU,     WAZANO_SOORAABIIMU,    WAZANO_ANAWOHORU,
    WAZANO_ROKETTOZUTUKI,  WAZANO_GODDOBAADO,     WAZANO_DAIBINGU,
    WAZANO_TOBIHANERU,     WAZANO_SYADOODAIBU,    WAZANO_HURIIFOORU,
    WAZANO_HURIIZUBORUTO,  WAZANO_KOORUDOHUREA,   WAZANO_GOOSUTODAIBU,
    WAZANO_ZIOKONTOROORU,  WAZANO_SOORAABUREEDO,  WAZANO_TORAPPUSHERU,   
    WAZANO_KUTIBASIKYANON, WAZANO_KINGUSIIRUDO,   WAZANO_HENSIN,
    WAZANO_ABARERU,        WAZANO_GEKIRIN,        WAZANO_HANABIRANOMAI,
    WAZANO_SAWAGU,         WAZANO_KOROGARU,       WAZANO_AISUBOORU,
    WAZANO_GAMAN,          WAZANO_KIAIPANTI,

    WAZANO_URUTORADASSYUATAKKU,       WAZANO_NOOMARUZENRYOKU,
    WAZANO_ZENRYOKUMUSOUGEKIRETUKEN,  WAZANO_KAKUTOUZENRYOKU,
    WAZANO_FAINARUDAIBUATAKKU,        WAZANO_HIKOUZENRYOKU,
    WAZANO_ASIDDOPOIZUNDERIITO,       WAZANO_DOKUZENRYOKU,
    WAZANO_RAIZINGURANDOOOBAA,        WAZANO_ZIMENZENRYOKU,
    WAZANO_WAARUZUENDOFOORU,          WAZANO_IWAZENRYOKU,
    WAZANO_ZETTAIHOSYOKUKAITENZAN,    WAZANO_MUSIZENRYOKU,
    WAZANO_MUGENANYAHENOIZANAI,       WAZANO_GOOSUTOZENRYOKU,
    WAZANO_TYOUZETURASENRENGEKI,      WAZANO_HAGANEZENRYOKU,
    WAZANO_DAINAMIKKUHURUHUREIMU,     WAZANO_HONOOZENRYOKU,
    WAZANO_HAIPAAAKUATORUNEEDO,       WAZANO_MIZUZENRYOKU,
    WAZANO_BURUUMUSYAINEKUSUTORA,     WAZANO_KUSAZENRYOKU,
    WAZANO_SUPAAKINGUGIGABORUTO,      WAZANO_DENKIZENRYOKU,
    WAZANO_MAKISIMAMUSAIBUREIKAA,     WAZANO_ESUPAAZENRYOKU,
    WAZANO_REIZINGUZIOHURIIZU,        WAZANO_KOORIZENRYOKU,
    WAZANO_ARUTHIMETTODORAGONBAAN,    WAZANO_DORAGONZENRYOKU,
    WAZANO_BURAKKUHOORUIKURIPUSU,     WAZANO_AKUZENRYOKU,
    WAZANO_RABURIISUTAAINPAKUTO,      WAZANO_FEARIIZENRYOKU,
    WAZANO_HISSATUNOPIKATYUUTO,       WAZANO_SYADOUAROOZUSUTORAIKU,   WAZANO_HAIPAADAAKUKURASSYAA,
    WAZANO_WADATUMINOSINFONIA,        WAZANO_GAADHIANDEAROORA,        WAZANO_HAISINSENYOUZENRYOKU,
    WAZANO_RAITONINGUSAAHURAIDO,      WAZANO_HONKIWODASUKOUGEKI,      WAZANO_SEBUNEBORUBUUSUTO,
    WAZANO_ORIZINZUSUUPAANOVHA,       WAZANO_SATOPIKASENYOUZENRYOKU,  WAZANO_ZENRYOKUSUTOONEZZI,
    WAZANO_SINNEKUROZUMAZETTO,        WAZANO_ZENRYOKUMETEODORAIBU,    WAZANO_ZENRYOKUSYADOOREI,
    WAZANO_ZENRYOKUZYARETUKU,         WAZANO_ZENRYOKUSUKEIRUNOIZU,
  };

  if( checkHaseiOmmitCommon(waza) ){
    return true;
  }


  return checkTableElems( waza, table, GFL_NELEMS(table) );
}


//--------------------------------------------------------------------------
/**
 *  ゆびをふるで出ないワザテーブル
 */
//--------------------------------------------------------------------------
static const WazaID YubiFuruOmmit[] = {
  WAZANO_YUBIWOHURU,      WAZANO_NEGOTO,        WAZANO_WARUAGAKI,
  WAZANO_NEKONOTE,        WAZANO_MANEKKO,       WAZANO_SAKIDORI,
  WAZANO_OUMUGAESI,       WAZANO_SIZENNOTIKARA, WAZANO_OSYABERI,

  WAZANO_SUKETTI,         WAZANO_MONOMANE,      WAZANO_KAUNTAA,
  WAZANO_MIRAAKOOTO,      WAZANO_MAMORU,        WAZANO_MIKIRI,
  WAZANO_KORAERU,         WAZANO_MITIDURE,      WAZANO_HENSIN,
  WAZANO_KONOYUBITOMARE,  WAZANO_IKARINOKONA,   WAZANO_KIAIPANTI,
  WAZANO_YOKODORI,        WAZANO_TEDASUKE,      WAZANO_DOROBOU,
  WAZANO_HOSIGARU,        WAZANO_TORIKKU,       WAZANO_SURIKAE,
  WAZANO_FEINTO,          WAZANO_SAKIOKURI,     WAZANO_GIHUTOPASU,

  WAZANO_TEKUNOBASUTAA,   WAZANO_INISIENOUTA,   WAZANO_SINPINOTURUGI,
  WAZANO_HURIIZUBORUTO,   WAZANO_KOORUDOHUREA,  WAZANO_BAAKUAUTO,
  WAZANO_vJENEREETO,      WAZANO_IBIKI,
  WAZANO_FASUTOGAADO,     WAZANO_WAIDOGAADO,    WAZANO_OSAKINIDOUZO,

  WAZANO_DAIYASUTOOMU,    WAZANO_SUTIIMUBAASUTO, WAZANO_IZIGENHOORU,
  WAZANO_SAUZANAROO,      WAZANO_SAUZANWHEEBU,

  WAZANO_GEPPU,           WAZANO_HAMETUNOHIKARI, WAZANO_TATAMIGAESI,
  WAZANO_TORIKKUGAADO,    WAZANO_KINGUSIIRUDO,   WAZANO_NIIDORUGAADO,
  WAZANO_OIWAI,           WAZANO_TEWOTUNAGU,
  
  WAZANO_KONGENNOHADOU,   WAZANO_DANGAINOTURUGI, WAZANO_GARYOUTENSEI,
  WAZANO_IZIGENRASSYU,    
  
  WAZANO_MABOROSIWAZA,
  // ↓通信互換のためmomijiにおいては新技２つを禁止にしました
  WAZANO_HANABISENYOU,    WAZANO_HIKARIDORAGON,

  WAZANO_URUTORADASSYUATAKKU,       WAZANO_NOOMARUZENRYOKU,
  WAZANO_ZENRYOKUMUSOUGEKIRETUKEN,  WAZANO_KAKUTOUZENRYOKU,
  WAZANO_FAINARUDAIBUATAKKU,        WAZANO_HIKOUZENRYOKU,
  WAZANO_ASIDDOPOIZUNDERIITO,       WAZANO_DOKUZENRYOKU,
  WAZANO_RAIZINGURANDOOOBAA,        WAZANO_ZIMENZENRYOKU,
  WAZANO_WAARUZUENDOFOORU,          WAZANO_IWAZENRYOKU,
  WAZANO_ZETTAIHOSYOKUKAITENZAN,    WAZANO_MUSIZENRYOKU,
  WAZANO_MUGENANYAHENOIZANAI,       WAZANO_GOOSUTOZENRYOKU,
  WAZANO_TYOUZETURASENRENGEKI,      WAZANO_HAGANEZENRYOKU,
  WAZANO_DAINAMIKKUHURUHUREIMU,     WAZANO_HONOOZENRYOKU,
  WAZANO_HAIPAAAKUATORUNEEDO,       WAZANO_MIZUZENRYOKU,
  WAZANO_BURUUMUSYAINEKUSUTORA,     WAZANO_KUSAZENRYOKU,
  WAZANO_SUPAAKINGUGIGABORUTO,      WAZANO_DENKIZENRYOKU,
  WAZANO_MAKISIMAMUSAIBUREIKAA,     WAZANO_ESUPAAZENRYOKU,
  WAZANO_REIZINGUZIOHURIIZU,        WAZANO_KOORIZENRYOKU,
  WAZANO_ARUTHIMETTODORAGONBAAN,    WAZANO_DORAGONZENRYOKU,
  WAZANO_BURAKKUHOORUIKURIPUSU,     WAZANO_AKUZENRYOKU,
  WAZANO_RABURIISUTAAINPAKUTO,      WAZANO_FEARIIZENRYOKU,
  WAZANO_HISSATUNOPIKATYUUTO,       WAZANO_SYADOUAROOZUSUTORAIKU,   WAZANO_HAIPAADAAKUKURASSYAA,
  WAZANO_WADATUMINOSINFONIA,        WAZANO_GAADHIANDEAROORA,        WAZANO_HAISINSENYOUZENRYOKU,
  WAZANO_RAITONINGUSAAHURAIDO,      WAZANO_HONKIWODASUKOUGEKI,      WAZANO_SEBUNEBORUBUUSUTO,
  WAZANO_ORIZINZUSUUPAANOVHA,       WAZANO_SATOPIKASENYOUZENRYOKU,  WAZANO_ZENRYOKUSUTOONEZZI,
  WAZANO_SINNEKUROZUMAZETTO,        WAZANO_ZENRYOKUMETEODORAIBU,    WAZANO_ZENRYOKUSYADOOREI,
  WAZANO_ZENRYOKUZYARETUKU,         WAZANO_ZENRYOKUSUKEIRUNOIZU,

  WAZANO_SUPOTTORAITO,    WAZANO_TORAPPUSHERU,  WAZANO_KUTIBASIKYANON,
  WAZANO_TOOTIKA,         WAZANO_SAIHAI,        WAZANO_HURUURUKANON,
  WAZANO_SYADOOSUTIIRU,

};

//=============================================================================================
/**
 * ゆびをふる出ないワザテーブル情報取得
 *
 * @param   elems   [out] テーブル要素数
 *
 * @retval  const WazaID*   テーブルアドレス
 */
//=============================================================================================
const WazaID* GetYubiFuruOmmitTable( u32* elems )
{
  *elems = GFL_NELEMS(YubiFuruOmmit);
  return YubiFuruOmmit;
}
//=============================================================================================
/**
 * ゆびをふるで出ないワザか判定
 *
 * @param   waza
 *
 * @retval  bool
 */
//=============================================================================================
bool IsYubiFuruOmmit( WazaID waza )
{
  u32 i;
  for(i=0; i<GFL_NELEMS(YubiFuruOmmit); ++i)
  {
    if( YubiFuruOmmit[i] == waza ){
      return true;
    }
  }
  return false;
}


/**
 *  「ぼうじん」でガードできる対象のワザチェック
 */
bool IsBoujinGuardWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_SIBIREGONA,    WAZANO_NEMURIGONA,    WAZANO_DOKUNOKONA,
    WAZANO_KINOKONOHOUSI, WAZANO_HUNZIN,        WAZANO_WATAHOUSI,
  };
  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  「がんじょうあご」で威力増加するワザチェック
 */
bool IsAgoBoostWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_KAMITUKU,       WAZANO_KAMIKUDAKU,    WAZANO_DOKUDOKUNOKIBA,
    WAZANO_KAMINARINOKIBA, WAZANO_KOORINOKIBA,   WAZANO_HONOONOKIBA,
    WAZANO_HISSATUMAEBA,   WAZANO_SAIKOFANGU,
  };
  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  「ぼうだん」で防ぐことができるワザかチェック
 */
bool IsBoudanWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_TAMANAGE,       WAZANO_KIAIDAMA,       WAZANO_SYADOOBOORU,
    WAZANO_AISUBOORU,      WAZANO_WHEZAABOORU,    WAZANO_ZYAIROBOORU,
    WAZANO_ENAZIIBOORU,    WAZANO_EREKIBOORU,     WAZANO_TAMAGOBAKUDAN,
    WAZANO_HEDOROBAKUDAN,  WAZANO_HADOUDAN,       WAZANO_TANEBAKUDAN,
    WAZANO_DOROBAKUDAN,    WAZANO_KAENDAN,        WAZANO_MISUTOBOORU,
    WAZANO_TANEMASINGAN,   WAZANO_MAGUNETTOBOMU,  WAZANO_ASIDDOBOMU,
    WAZANO_OKUTANHOU,      WAZANO_DENZIHOU,       WAZANO_GANSEKIHOU,
    WAZANO_ROKKUBURASUTO,  WAZANO_KUTIBASIKYANON, WAZANO_KAHUNDANGO,

  };
  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 *  「おやこあい」が無効なワザかチェック
 */
bool IsOyakoaiOmitWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_GAMUSYARA,      WAZANO_NAGETUKERU,
    WAZANO_KOROGARU,       WAZANO_AISUBOORU,
    WAZANO_INOTIGAKE,      WAZANO_DAIBAKUHATU, WAZANO_ZIBAKU,

    WAZANO_URUTORADASSYUATAKKU,       WAZANO_NOOMARUZENRYOKU,
    WAZANO_ZENRYOKUMUSOUGEKIRETUKEN,  WAZANO_KAKUTOUZENRYOKU,
    WAZANO_FAINARUDAIBUATAKKU,        WAZANO_HIKOUZENRYOKU,
    WAZANO_ASIDDOPOIZUNDERIITO,       WAZANO_DOKUZENRYOKU,
    WAZANO_RAIZINGURANDOOOBAA,        WAZANO_ZIMENZENRYOKU,
    WAZANO_WAARUZUENDOFOORU,          WAZANO_IWAZENRYOKU,
    WAZANO_ZETTAIHOSYOKUKAITENZAN,    WAZANO_MUSIZENRYOKU,
    WAZANO_MUGENANYAHENOIZANAI,       WAZANO_GOOSUTOZENRYOKU,
    WAZANO_TYOUZETURASENRENGEKI,      WAZANO_HAGANEZENRYOKU,
    WAZANO_DAINAMIKKUHURUHUREIMU,     WAZANO_HONOOZENRYOKU,
    WAZANO_HAIPAAAKUATORUNEEDO,       WAZANO_MIZUZENRYOKU,
    WAZANO_BURUUMUSYAINEKUSUTORA,     WAZANO_KUSAZENRYOKU,
    WAZANO_SUPAAKINGUGIGABORUTO,      WAZANO_DENKIZENRYOKU,
    WAZANO_MAKISIMAMUSAIBUREIKAA,     WAZANO_ESUPAAZENRYOKU,
    WAZANO_REIZINGUZIOHURIIZU,        WAZANO_KOORIZENRYOKU,
    WAZANO_ARUTHIMETTODORAGONBAAN,    WAZANO_DORAGONZENRYOKU,
    WAZANO_BURAKKUHOORUIKURIPUSU,     WAZANO_AKUZENRYOKU,
    WAZANO_RABURIISUTAAINPAKUTO,      WAZANO_FEARIIZENRYOKU,
    WAZANO_HISSATUNOPIKATYUUTO,       WAZANO_SYADOUAROOZUSUTORAIKU,   WAZANO_HAIPAADAAKUKURASSYAA,
    WAZANO_WADATUMINOSINFONIA,        WAZANO_GAADHIANDEAROORA,        WAZANO_HAISINSENYOUZENRYOKU,
    WAZANO_RAITONINGUSAAHURAIDO,      WAZANO_HONKIWODASUKOUGEKI,      WAZANO_SEBUNEBORUBUUSUTO,
    WAZANO_ORIZINZUSUUPAANOVHA,       WAZANO_SATOPIKASENYOUZENRYOKU,  WAZANO_ZENRYOKUSUTOONEZZI,
    WAZANO_SINNEKUROZUMAZETTO,        WAZANO_ZENRYOKUMETEODORAIBU,    WAZANO_ZENRYOKUSYADOOREI,
    WAZANO_ZENRYOKUZYARETUKU,         WAZANO_ZENRYOKUSUKEIRUNOIZU,
  };
  return checkTableElems( waza, table, GFL_NELEMS(table) );
}


/**
 * @brief UIにダメージ相性を表示しない技か？
 */
bool IsNotDisplayUiAffinity( WazaID waza )
{
  static const u16 table[]  =
  {
    WAZANO_HASAMIGIROTIN,  WAZANO_TUNODORIRU,    WAZANO_SONIKKUBUUMU,
    WAZANO_KAUNTAA,        WAZANO_TIKYUUNAGE,    WAZANO_RYUUNOIKARI,
    WAZANO_ZIWARE,         WAZANO_NAITOHEDDO,    WAZANO_GAMAN,
    WAZANO_SAIKOWHEEBU,    WAZANO_IKARINOMAEBA,  WAZANO_MIRAAKOOTO,
    WAZANO_GAMUSYARA,      WAZANO_ZETTAIREIDO,   WAZANO_METARUBAASUTO,
    WAZANO_INOTIGAKE,      WAZANO_SIZENNOIKARI,  WAZANO_GAADHIANDEAROORA,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}

/**
 * @brief とくせい「ちからづく」の対象ワザとするか？
 */
bool IsTikarazukuEffeciveWaza( WazaID waza )
{
  static const u16 table[]  =
  {
    WAZANO_HIMITUNOTIKARA, WAZANO_UTAKATANOARIA, WAZANO_KAGENUI,
    WAZANO_ANKAASYOTTO,    WAZANO_ORIZINZUSUUPAANOVHA,
  };

  return checkTableElems( waza, table, GFL_NELEMS(table) );
}


/*--------------------------------------------------------------------------------------------*/
/* デバッグ用「ゆびをふる」コントロール                                                       */
/*--------------------------------------------------------------------------------------------*/

#if PM_DEBUG

/**
 *  デバッグ「ゆびをふるテーブル（グローバル）」
 */
int g_YubiFuruDebugNumber[ BTL_POS_NUM ];

/**
 *  「ゆびをふるテーブル」リセット
 */
void YubifuruDebugReset( void )
{
  u32 i;
  for(i=0; i<GFL_NELEMS(g_YubiFuruDebugNumber); ++i){
    g_YubiFuruDebugNumber[i] = 0;
  }
}
/**
 *  外部から適当に操作した「ゆびをふるテーブル」に禁止ワザが設定されていたら補正
 */
void YubifuruDebugSetEnd( void )
{
  u32 i;
  for(i=0; i<GFL_NELEMS(g_YubiFuruDebugNumber); ++i){
    if( IsYubiFuruOmmit( (WazaID)(g_YubiFuruDebugNumber[i])) )
    {
      YubifuruDebugInc(i);
    }
  }
}
/**
 *  「ゆびをふるテーブル」のナンバーをインクリメント（禁止ワザはスキップ）
 */
void YubifuruDebugInc( u8 pos )
{
  for ever
  {
    g_YubiFuruDebugNumber[pos]++;
    if( g_YubiFuruDebugNumber[pos] >= WAZANO_MAX )
    {
      g_YubiFuruDebugNumber[pos] = 0;
    }
    if( !IsYubiFuruOmmit( (WazaID)(g_YubiFuruDebugNumber[pos]) ) ){
      break;
    }
  };
}
/**
 *「ゆびをふるテーブル」に設定されているナンバーを取得
 */
int  YubifuruDebug_GetNumber( u8 pos )
{
  if( pos < GFL_NELEMS(g_YubiFuruDebugNumber) ){
    return g_YubiFuruDebugNumber[ pos ];
  }
  return 0;
}



#endif


}   // end of namespace 'tables'
GFL_NAMESPACE_END(btl)


