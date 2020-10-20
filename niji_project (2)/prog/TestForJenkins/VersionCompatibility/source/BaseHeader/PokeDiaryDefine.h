//==============================================================================
/**
 * @file	  PokeDiaryDefine.h
 * @brief   ポケモンダイアリー用定義
 * @date    2012.12.05
 * @author  tamada
 */
//==============================================================================

#if !defined  __POKEDAIARYDEFINE_H__
#define  __POKEDAIARYDEFINE_H__


//==============================================================================
/**
 * @brief   ポケモンダイアリー用イベント定義
 */
//==============================================================================
enum PDID{
  PDID_NULL       = 0,
  PDID_PERSONAL_NUM,

  PDID_LIVE_NULL = 0x100,
  PDID_LIVE_NUM,


  PDID_COUNT_NULL = 0x200,
  PDID_COUNT_FRIENDLY_SHOP,//0
  PDID_COUNT_KISEKAE,
  PDID_COUNT_GANBA_RIBBON,
  PDID_COUNT_NAME_CHANGE,
  PDID_COUNT_SODATEYA_TAMAGO,
  PDID_COUNT_ID_PRESENT,
  PDID_COUNT_CAMERA_OYAJI,
  PDID_COUNT_FOSSIL,
  PDID_COUNT_CHECK_NATSUKI,
  PDID_COUNT_PLANT_SEED,//10
  PDID_COUNT_GET_SEED,
  PDID_COUNT_BP_SHOP,
  PDID_COUNT_WIN_BH_CHAMP,
  PDID_COUNT_DENDOU,
  PDID_COUNT_GTS_POKEPUT,
  PDID_COUNT_GTS_POKETRADE,
  PDID_COUNT_MTRADE,
  PDID_COUNT_P2PTRADE,
  PDID_COUNT_BSPOT,
  PDID_COUNT_JF_ATTLACTION,
  PDID_COUNT_P2PBATTLE,//20
  PDID_COUNT_JF_SHOP,
  PDID_COUNT_RIDE,
  PDID_COUNT_EFFORT,
  PDID_COUNT_PARLE,
  PDID_COUNT_CAFE,
  PDID_COUNT_BATTLEROYAL,
  PDID_COUNT_LVUP,
  PDID_COUNT_MARASADA,
  PDID_COUNT_MONOHIROI,
  PDID_COUNT_NAKAYOSHI,//30
  PDID_COUNT_LOMILOMI,
  PDID_COUNT_TRAINING,
  PDID_COUNT_WAZAMASHIN,
  PDID_COUNT_BATTLEBIKING,
  PDID_COUNT_POKRESORT,
  PDID_COUNT_QR,
  PDID_COUNT_FIELDSCAN,
  PDID_COUNT_BATTLEMOVIE,

  PDID_COUNT_NUM,
};



#endif  //  __POKEDAIARYDEFINE_H__
