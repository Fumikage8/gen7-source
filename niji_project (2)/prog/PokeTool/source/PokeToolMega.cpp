//==============================================================================
/**
 * @file    poke_tool_Mega.cpp
 * @brief   メガ進化関連ユーティリティ
 * @date    2013.01.26
 * @author  tamada
 *          2015.01.13 niji移植ariizumi
 */
//==============================================================================
#include "debug/include/gfl2_Assert.h"
#include "pml/include/pmlib.h"
#include "PokeTool/include/PokeToolMega.h"


GFL_NAMESPACE_BEGIN(PokeTool)
GFL_NAMESPACE_BEGIN(Mega)



//------------------------------------------------------------------------
//------------------------------------------------------------------------
struct MEGA_TABLE {
  u16     itemno;
  MonsNo  monsno; 
  u8      formno;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
static const MEGA_TABLE MegaTable[] =
{
  { ITEM_GENGANAITO,      MONSNO_GENGAA,      FORMNO_GENGAA_MEGA },
  { ITEM_SAANAITONAITO,   MONSNO_SAANAITO,    FORMNO_SAANAITO_MEGA },
  { ITEM_DENRYUUNAITO,    MONSNO_DENRYUU,     FORMNO_DENRYUU_MEGA },
  { ITEM_HUSIGIBANAITO,   MONSNO_HUSIGIBANA,  FORMNO_HUSIGIBANA_MEGA },
  { ITEM_RIZAADONAITOx,   MONSNO_RIZAADON,    FORMNO_RIZAADON_MEGAX },
  { ITEM_RIZAADONAITOy,   MONSNO_RIZAADON,    FORMNO_RIZAADON_MEGAY },
  { ITEM_KAMEKKUSUNAITO,  MONSNO_KAMEKKUSU,   FORMNO_KAMEKKUSU_MEGA },
  { ITEM_MYUUTUNAITOx,    MONSNO_MYUUTUU,     FORMNO_MYUUTUU_MEGAX },
  { ITEM_MYUUTUNAITOy,    MONSNO_MYUUTUU,     FORMNO_MYUUTUU_MEGAY },
  { ITEM_BASYAAMONAITO,   MONSNO_BASYAAMO,    FORMNO_BASYAAMO_MEGA },
  { ITEM_TYAAREMUNAITO,   MONSNO_TYAAREMU,    FORMNO_TYAAREMU_MEGA },
  { ITEM_HERUGANAITO,     MONSNO_HERUGAA,     FORMNO_HERUGAA_MEGA },
  { ITEM_BOSUGODORANAITO, MONSNO_BOSUGODORA,  FORMNO_BOSUGODORA_MEGA },
  { ITEM_ZYUPETTANAITO,   MONSNO_ZYUPETTA,    FORMNO_ZYUPETTA_MEGA },
  { ITEM_BANGIRASUNAITO,  MONSNO_BANGIRASU,   FORMNO_BANGIRASU_MEGA },
  { ITEM_HASSAMUNAITO,    MONSNO_HASSAMU,     FORMNO_HASSAMU_MEGA },
  { ITEM_KAIROSUNAITO,    MONSNO_KAIROSU,     FORMNO_KAIROSU_MEGA },
  { ITEM_PUTERANAITO,     MONSNO_PUTERA,      FORMNO_PUTERA_MEGA },
  { ITEM_RUKARIONAITO,    MONSNO_RUKARIO,     FORMNO_RUKARIO_MEGA },
  { ITEM_YUKINOONAITO,    MONSNO_YUKINOOO,    FORMNO_YUKINOOO_MEGA },
  { ITEM_GARUURANAITO,    MONSNO_GARUURA,     FORMNO_GARUURA_MEGA },
  { ITEM_GYARADOSUNAITO,  MONSNO_GYARADOSU,   FORMNO_GYARADOSU_MEGA },
  { ITEM_ABUSORUNAITO,    MONSNO_ABUSORU,     FORMNO_ABUSORU_MEGA },
  { ITEM_HUUDHINAITO,     MONSNO_HUUDHIN,     FORMNO_HUUDHIN_MEGA },
  { ITEM_HERAKUROSUNAITO, MONSNO_HERAKUROSU,  FORMNO_HERAKUROSU_MEGA },
  { ITEM_KUTIITONAITO,    MONSNO_KUTIITO,     FORMNO_KUTIITO_MEGA },
  { ITEM_RAIBORUTONAITO,  MONSNO_RAIBORUTO,   FORMNO_RAIBORUTO_MEGA },
  { ITEM_GABURIASUNAITO,  MONSNO_GABURIASU,   FORMNO_GABURIASU_MEGA },
  { ITEM_RATHIASUNAITO,   MONSNO_RATHIASU,    FORMNO_RATHIASU_MEGA },
  { ITEM_RATHIOSUNAITO,   MONSNO_RATHIOSU,    FORMNO_RATHIOSU_MEGA },
  //sango追加分
  {ITEM_RAGURAAZINAITO ,MONSNO_RAGURAAZI ,FORMNO_RAGURAAZI_MEGA},
  {ITEM_ZYUKAINNAITO   ,MONSNO_ZYUKAIN   ,FORMNO_ZYUKAIN_MEGA},
  {ITEM_YAMIRAMINAITO  ,MONSNO_YAMIRAMI  ,FORMNO_YAMIRAMI_MEGA},
  {ITEM_TIRUTARISUNAITO,MONSNO_TIRUTARISU,FORMNO_TIRUTARISU_MEGA},
  {ITEM_ERUREIDONAITO  ,MONSNO_ERUREIDO  ,FORMNO_ERUREIDO_MEGA},
  {ITEM_TABUNNENAITO   ,MONSNO_TABUNNE   ,FORMNO_TABUNNE_MEGA},
  {ITEM_METAGUROSUNAITO,MONSNO_METAGUROSU,FORMNO_METAGUROSU_MEGA},
  {ITEM_SAMEHADANAITO  ,MONSNO_SAMEHADAA ,FORMNO_SAMEHADAA_MEGA},
  {ITEM_YADORANNAITO   ,MONSNO_YADORAN   ,FORMNO_YADORAN_MEGA},
  {ITEM_HAGANEERUNAITO ,MONSNO_HAGANEERU ,FORMNO_HAGANEERU_MEGA},
  {ITEM_PIZYOTTONAITO  ,MONSNO_PIZYOTTO  ,FORMNO_PIZYOTTO_MEGA},
  {ITEM_ONIGOORINAITO  ,MONSNO_ONIGOORI  ,FORMNO_ONIGOORI_MEGA},
  {ITEM_DHIANSINAITO   ,MONSNO_DHIANSII  ,FORMNO_DHIANSII_MEGA},
  {ITEM_BAKUUDANAITO   ,MONSNO_BAKUUDA   ,FORMNO_BAKUUDA_MEGA},
  {ITEM_MIMIROPPUNAITO ,MONSNO_MIMIROPPU ,FORMNO_MIMIROPPU_MEGA},
  {ITEM_BOOMANDANAITO  ,MONSNO_BOOMANDA  ,FORMNO_BOOMANDA_MEGA},
  {ITEM_SUPIANAITO     ,MONSNO_SUPIAA    ,FORMNO_SUPIAA_MEGA},

  {ITEM_DUMMY_DATA     ,MONSNO_REKKUUZA  ,FORMNO_REKKUUZA_DELTA},
  //グラードン、カイオーガはメガシンカではないので入れない
  //レックウザは扱いがメガシンカだが、特殊な進化条件を持つので注意。
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int GetMegaIndex( MonsNo monsno, u8 formno )
{
  for (int i = 0; i < GFL_NELEMS(MegaTable); ++ i )
  {
    if ( MegaTable[i].monsno == monsno && MegaTable[i].formno == formno ) return i;
  }
  GFL_ASSERT(0);
  return -1;
}
int GetMegaIndex( u16 itemno )
{
  for (int i = 0; i < GFL_NELEMS(MegaTable); ++ i )
  {
    if ( MegaTable[i].itemno == itemno ) return i;
  }
  GFL_ASSERT(0);
  return -1;
}
//sangoではレックウザがアイテムなしで進化できる。つまり関係ない○○ナイトを持って進化できる・・・
int GetMegaIndexItem( MonsNo monsno, u16 itemno )
{
  for (int i = 0; i < GFL_NELEMS(MegaTable); ++ i )
  {
    if ( MegaTable[i].monsno == monsno && 
        (MegaTable[i].itemno == itemno || monsno == MONSNO_REKKUUZA)) 
    {
      return i;
    }
  }
  GFL_ASSERT(0);
  return -1;
}

//nijiではLong分岐は無いので不要
#if 0
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SetSeeFlag( u8 * flagarea, MonsNo monsno, u8 formno )
{
  int index = GetMegaIndex( monsno, formno );
  if ( index < 0 ) return;
  flagarea[ index / 8 ] |= ( 1 << ( index % 8 ) );
}

void SetSeeFlag( u8 * flagarea, u16 itemno )
{
  int index = GetMegaIndex( itemno );
  if ( index < 0 ) return;
  flagarea[ index / 8 ] |= ( 1 << ( index % 8 ) );
}
void SetSeeFlagItem( u8 * flagarea, MonsNo monsno, u16 itemno )
{
  int index = GetMegaIndexItem( monsno,itemno );
  if ( index < 0 ) return;
  flagarea[ index / 8 ] |= ( 1 << ( index % 8 ) );
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool GetSeeFlag( const u8 * flagarea, MonsNo monsno, u8 formno )
{
  int index = GetMegaIndex( monsno, formno );
  if ( index < 0 ) return false;
  return ( ( flagarea[ index / 8 ] & ( 1 << (index % 8) ) ) != 0 );
}

bool GetSeeFlag( const u8 * flagarea, u16 itemno )
{
  int index = GetMegaIndex( itemno );
  if ( index < 0 ) return false;
  bool result = ( ( flagarea[ index / 8 ] & ( 1 << (index % 8) ) ) != 0 );
  return result;
}
bool GetSeeFlagItem( const u8 * flagarea, MonsNo monsno, u16 itemno )
{
  int index = GetMegaIndexItem( monsno,itemno );
  if ( index < 0 ) return false;
  bool result = ( ( flagarea[ index / 8 ] & ( 1 << (index % 8) ) ) != 0 );
  return result;
}

bool IsAlreadySeen( const u8 * flagarea )
{
  for (int index = 0; index < GFL_NELEMS(MegaTable); ++ index )
  {
    GFL_PRINT("IsAlreadySeen::index=%d, value=%d\n", index,
        flagarea[ index / 8 ] & ( 1 << (index % 8) ) );
    if ( ( flagarea[ index / 8 ] & ( 1 << (index % 8) ) ) != 0 ) return true;
  }
  return false;
}
#endif

//==============================================================================
//==============================================================================
//------------------------------------------------------------------------
/**
 * @brief メガ進化状態のフォルムをノーマルに戻す
 * @param my_party
 */
//------------------------------------------------------------------------
void NormalizeMegaForm( pml::PokeParty * my_party )
{
  int max = my_party->GetMemberCount();
  for ( int i = 0; i < max; i ++ )
  {
    pml::pokepara::PokemonParam * pp = my_party->GetMemberPointer(i);
    
    //GFSBTS[1352]グラカイのみ例外的に対応
    //ポワルン等のバトル専用フォルムは保留。
    if( pp->GetMonsNo() == MONSNO_GURAADON &&
        pp->GetFormNo() == FORMNO_GURAADON_OMEGA )
    {
      //フォルムをノーマルに戻す
      pp->ChangeFormNo( FORMNO_GURAADON_NORMAL );
    }
    if( pp->GetMonsNo() == MONSNO_KAIOOGA &&
        pp->GetFormNo() == FORMNO_KAIOOGA_ALPHA )
    {
      //フォルムをノーマルに戻す
      pp->ChangeFormNo( FORMNO_KAIOOGA_NORMAL );
    }
    
    if ( IsMegaForm( pp ) == false ) continue;

    //フォルムをノーマルに戻す
    pp->ChangeFormNo( 0 );
  }
}

//------------------------------------------------------------------------
/**
 * @brief   メガ進化状態かどうかをチェック
 * @param   poke
 * @retval  true  メガ進化フォルムの状態
 * @retval  false メガ進化フォルムでない
 *
 * @note    タマゴであってもフォルムをチェックしている。
 * @caution 同名のpmlib::pokepara::CoreParamの関数とは同じ判定でないので注意
 */
//------------------------------------------------------------------------
bool IsMegaForm( pml::pokepara::CoreParam * poke )
{
  //メガ進化を持たないポケモンはスルー
  if ( poke->GetMegaFormCount() == 0 ) return false;
  //フォルムがノーマルの場合もスルー
  if ( poke->GetFormNo() == 0 ) return false;
  return true;
}


GFL_NAMESPACE_END(Mega)
GFL_NAMESPACE_END(PokeTool)
