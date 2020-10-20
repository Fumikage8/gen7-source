//==============================================================================
/**
 * @file   VersionCompatibility.cpp
 * @date   2016/10/26 Wed. 13:05:53
 * @author muto_yu
 * @brief  niji-momiji間でのpreprocessor symbol値一致チェック
 * @attention シンボルの単純な追加は基本的に検出できない（MAX値をチェックすることである程度であれば間接的に検知することは可能）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <gflnet2/include/beacon/gflnet2_Beacon.h>
#include  <str/include/gfl2_StrSys.h>

#include  <System/include/GameStrLen.h>

#ifndef USE_PAWN
  #define USE_PAWN  (0)
#endif


#include  "./VersionCompatibility.h"
#include  "./CheckMacro.h"

//  #include  "./BaseHeader/needRedefine/redefine_battle_dressup.h"   /*  別ソースで  */
//  #include  "./BaseHeader/needRedefine/redefine_field_dressup.h"    /*  別ソースで  */
#include  "./BaseHeader/needRedefine/redefine_msg_jf_phrase.h"
#include  "./BaseHeader/needRedefine/redefine_msg_place_name.h"
#include  "./BaseHeader/needRedefine/redefine_msg_place_name_out.h"
#include  "./BaseHeader/needRedefine/redefine_msg_place_name_per.h"
#include  "./BaseHeader/needRedefine/redefine_msg_place_name_spe.h"
#include  "./BaseHeader/needRedefine/redefine_itemsym.h"
#include  "./BaseHeader/needRedefine/redefine_monsno_def.h"


/*
  break include-guard
*/
#undef  __SOUNDMIDDLEID_H__
#undef  __RECORD_DEFINE_H__
#undef  __POKEDAIARYDEFINE_H__
#undef  __WAZANODEF_H__
#undef  __TOKUSEIDEF_H__
#undef  __SEIKAKU_H__
#undef  __JOIN_FESTA_PAWN_DEFINE_H__


#include  <niji_conv_header/sound/SoundMiddleID.h>
#include  <Savedata/include/MyStatus.h>
#include  <Savedata/include/Record.h>
#include  <niji_reference_files/savedata/PokeDiaryDefine.h>
#include  <niji_conv_header/poke_lib/monsno_def.h>
#include  <niji_conv_header/poke_lib/wazano_def.h>
#include  <niji_conv_header/poke_lib/tokusei_def.h>
#include  <niji_conv_header/poke_lib/seikaku.h>
#include  <niji_conv_header/poke_lib/item/itemsym.h>
#include  <NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h>
#include  <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h>
#include  <niji_reference_files/script/JoinFestapawnDefine.h>
#include  <niji_conv_header/message/msg_jf_phrase.h>
#include  <niji_conv_header/message/msg_jf_phrase.h>
#include  <niji_conv_header/message/msg_jf_phrase.h>
#include  <niji_conv_header/message/msg_place_name.h>
#include  <niji_conv_header/message/msg_place_name_out.h>
#include  <niji_conv_header/message/msg_place_name_per.h>
#include  <niji_conv_header/message/msg_place_name_spe.h>


namespace TestForJenkins  {
namespace VersionCompatibility  {
// =============================================================================
/**/
//==============================================================================


/*
  　■MyStatusデータ (MyStatus.h）
  　■JoinFestaPersonalデータ(JoinFestaPersonalData.h)
  　■レコード定義 (RecordDefine.h）
  　■ポケダイアリー定義（PokeDiaryDefine.h)
  　■フェスサークルの定義（
  　　■JoinFestanetDefine.h
  　　■JoinFestaPawnDefine.h
  　　■jf_phrase.mstxt
  　）
  　■通信対戦の選べる戦闘曲（BgmSelect\NetworkBattleBgmSelect.h）
  　■トレーナーメモに表示される地名（place_name*.mstxt）
  　■きせかえデータ（各パーツのアイテムヘッダ）
  　　■niji_conv_header/battle/dressup/ *.h
  　　■niji_conv_header/field/dressup/ *.h
  　■ポケモンの種族No、フォルムNo（monsno_def.h）
  　■技No（wazano_def.h）
  　■特性No（tokusei_def.h）
  　■せいかく（seikaku.h）
  　■アイテムNo（itemsym.h）
  △□CalcDataが変わるような種族値変更（ポケモンのパーソナルデータ）

  2017/03/30
    deleted:
      RECID_LARGE_70,  ///<
      RECID_LARGE_71,  ///<
      RECID_LARGE_72,  ///<
      (
        RECID_ROTOPON_CNT,            ///< ロトポンした回数                                   999,999,999
        RECID_RR_EVENT_FLAG,          ///< レインボーロケット団イベントクリアフラグ           999,999,999
        RECID_NUSIIRU_CNT,            ///< ヌシール取得数                                     999,999,999
      )
      
    changed:
      #define OTHER_FORM_MAX  ( 269 )
        #define OTHER_FORM_MAX  ( 309 )

  -------

  remove:
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_88                      );
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_89                      );
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_90                      );
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_91                      );
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_92                      );
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_93                      );
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_94                      );
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_95                      );
    IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_96                      );


  Revision: 1168
  Author: nishino_koji
  Date: 2017年2月3日 17:46:03
  Message:
  イワンコ、ルガルガン、ガラガラの別フォルムを追加
  ----
    source\VersionCompatibility.cpp(1406)
    source\VersionCompatibility.cpp(1918)
    source\VersionCompatibility.cpp(2006)
      added:
        #define		FORMNO_GARAGARA_NUSHI				( 2 )
        #define		FORMNO_OOKAMI1_NORMAL				( 0 )
        #define		FORMNO_OOKAMI1_TWILIGHT				( 1 )
        #define		FORMNO_OOKAMI2_TWILIGHT				( 2 )
      changed:
        #define		FORMNO_GARAGARA_MAX				( 2 )
          #define		FORMNO_GARAGARA_MAX				( 3 )
        #define		FORMNO_OOKAMI2_MAX				( 2 )
          #define		FORMNO_OOKAMI2_MAX				( 3 )
        #define OTHER_FORM_MAX  ( 266 )
          #define OTHER_FORM_MAX  ( 269 )


  Revision: 1175
  Author: niji_ci
  Date: 2017年2月3日 20:01:59
  Message:
  [modify] jenkins-niji_text-14533 auto commit
  ----
    source\VersionCompatibility.cpp(5627)
      deleted:
        #define	PLACENAME_Z_00_00		(194)
        #define	PLACENAME_Z_00_01		(195)
        #define	PLACENAME_Z_00_02		(196)
        #define	PLACENAME_Z_00_03		(197)      
      added:
        #define	PLACENAME_Z_01_01		(194)
        #define	PLACENAME_Z_02_01		(195)
        #define	PLACENAME_Z_03_01		(196)
        #define	PLACENAME_Z_04_01		(197)
        #define	PLACENAME_Z_F0102_S0801_01		(198)
        #define	PLACENAME_F0102D0701_01		(199)
        #define	PLACENAME_F0201_S1101_01		(200)
        #define	PLACENAME_F0401_S0801_01		(201)
        #define	PLACENAME_F0701PH0201_01		(202)
        #define	PLACENAME_F0701PH0203_01		(203)
        #define	PLACENAME_F0701PH0301_01		(204)
        #define	PLACENAME_F0701PH0401_01		(205)
        #define	PLACENAME_F0701PH0501_01		(206)
        #define	PLACENAME_F0701PH0601_01		(207)
        #define	PLACENAME_F0701PH0701_01		(208)
        #define	PLACENAME_F0701PH0801_01		(209)
        #define	PLACENAME_F0701PH0901_01		(210)
        #define	PLACENAME_F0801R0101_01		(211)
      changed:
        #define	msg_place_name_max		(198)
          #define	msg_place_name_max		(212)


  Revision: 39045
  Author: morimoto
  Date: 2016年12月11日 16:39:55
  Message:
  新特性追加
  ----
    source\VersionCompatibility.cpp(2947,3): error #94: the size of an array must be greater than zero
    source\VersionCompatibility.cpp(2948,3): error #94: the size of an array must be greater than zero
      added:
        TOKUSEI_BUREINPURIZUMU			 = 233,  // ブレインプリズム
      changed:
        TOKUSEI_MAX						 = 232,
        TOKUSEI_NUM						 = 233,
          TOKUSEI_MAX						 = 233,
          TOKUSEI_NUM						 = 234,


  Revision: 39143
  Author: nakamura
  Date: 2016年12月13日 17:00:37
  Message:
  更新
  ----
    source\VersionCompatibility.cpp(3900,3): error #94: the size of an array must be greater than zero
      added:
        #define ITEM_SORUPURASU_1		( 945 )		// ソルプラス
        #define ITEM_RUNAPURASU_1		( 946 )		// ルナプラス
      changed:
        #define ITEM_DATA_MAX		( 944 )		// アイテム最大数
          #define ITEM_DATA_MAX		( 946 )		// アイテム最大数




*/

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  DummyFunction(void)
{
  /*
    NetworkBattleBgmSelect
  */
  IS_COMPATIBLE_CC(STRM_BGM_VS_TRAINER_NJ    );
  IS_COMPATIBLE_CC(STRM_BGM_VS_NORAPOKE_NJ   );
  IS_COMPATIBLE_CC(STRM_BGM_VS_NORAPOKE_02   );
  IS_COMPATIBLE_CC(STRM_BGM_VS_GIVER         );
  IS_COMPATIBLE_CC(STRM_BGM_VS_FRIEND_NJ     );
  IS_COMPATIBLE_CC(STRM_BGM_VS_RIVAL_NJ      );
  IS_COMPATIBLE_CC(STRM_BGM_VS_SKULL         );
  IS_COMPATIBLE_CC(STRM_BGM_VS_SKULLELITE    );
  IS_COMPATIBLE_CC(STRM_BGM_VS_SKULLBOSS     );
  IS_COMPATIBLE_CC(STRM_BGM_VS_ETHER         );
  IS_COMPATIBLE_CC(STRM_BGM_VS_MOTHER        );
  IS_COMPATIBLE_CC(STRM_BGM_VS_MOTHERBEAST   );
  IS_COMPATIBLE_CC(STRM_BGM_VS_BEAST         );
  IS_COMPATIBLE_CC(STRM_BGM_VS_SHITENNO_NJ   );
  IS_COMPATIBLE_CC(STRM_BGM_VS_CHAMP_NJ      );
  IS_COMPATIBLE_CC(STRM_BGM_VS_LEGEND02_NJ   );
  IS_COMPATIBLE_CC(STRM_BGM_VS_BROYAL        );
  IS_COMPATIBLE_CC(STRM_BGM_VS_LEGEND01_NJ   );
  IS_COMPATIBLE_CC(STRM_BGM_VS_REDGREEN      );


  /*
    Record.h
  */
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WALK_COUNT                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_REPORT_COUNT                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_CLEAR_TIME                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BATTLE_COUNT                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BTL_ENCOUNT                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BTL_TRAINER                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_CAPTURE_POKE                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FISHING_POKEMON               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_TAMAGO_HATCHING               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKE_EVOLUTION                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_RECOVERY_PC                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_COMM_TRADE                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_COMM_BATTLE                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_COMM_BTL_WIN                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_COMM_BTL_LOSE                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WIFI_BATTLE                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WIFI_BTL_WIN                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WIFI_BTL_LOSE                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SHOPPING_CNT                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SHOPPING_MONEY                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SODATEYA_CNT                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_KILL_POKE                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYMAX_EXP                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYCNT_EXP                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GTS_PUT                       );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_NICKNAME                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_PREMIUM_BALL                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WIN_BP                        );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_USE_BP                        );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WEAK_ATTACK                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_CLOTHING_CNT                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_HAIR_ARRANGE                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BERRY_HARVEST                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GTS_TRADE_COUNT               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_MIRACLE_TRADE_COUNT           );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_QUICKMATCH_COUNT              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_RIDE_POKE                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_MAME_PRESENT_CNT              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_JF_USE_JCOIN                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_MAME_GET_CNT                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BTREE_CHALLENGE               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_ZWAZA_CNT                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_THROW_BALL                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_STEAL_ITEM                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WAZA_CNT                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SODATEYA_LVUP_CNT             );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_ESCAPE                        );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BRAKE_ITEM                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_HEAL_KIDOU                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_NEKONIKOBAN                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FINDER_POINT                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GURUGURU                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FINDER_RECORD                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_PARLE_NADE                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKERESORT                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKEMAME_TRADE                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKEMON_TOUCH                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKEMAME_IN                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKERESORT_LVUP               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKERESORT_NATSUKI            );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKERESORT_TAMAGOFUKA         );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POKERESORT_KISO               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_QR_READ_BATTLE                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BATTLEMOVIE                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BATTLEMOVIE_FIGHT             );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_ROTOM                         );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_JF_GEST_TALK_CNT              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_NUTS                          );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_MANY_NUTS                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FISHING_ITEM                  );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_75                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_76                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_77                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_78                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_79                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_80                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_81                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_82                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_83                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_84                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_85                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_86                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_87                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_88                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_89                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_90                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_91                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_92                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_93                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_94                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_95                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_96                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_97                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_98                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_99                      );

  IS_COMPATIBLE_CC(Savedata::Record::RECID_LARGE_MAX                     );

  // ここから２バイトレコード
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DENDOU_CNT                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_RECOVERY_HOME                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WAZA_HANERU                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WAZA_WARUAGAKI                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WAZA_MUKOU                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_TEMOTI_KIZETU                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_TEMOTI_MAKIZOE                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_NIGERU_SIPPAI                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_NIGERARETA_FLAG               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FISHING_FAILURE               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYMAX_KILL                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYCNT_KILL                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYMAX_CAPTURE                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYCNT_CAPTURE                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYMAX_TRAINER_BATTLE         );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYCNT_TRAINER_BATTLE         );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYMAX_EVOLUTION              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DAYCNT_EVOLUTION              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FOSSIL_RESTORE                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_CAMERA_CNT                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BTREE_WIN_SINGLE              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BTREE_WIN_DOUBLE              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BTREE_WIN_MULTI               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_IDKUJI_ATARI_CNT              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_POINTUP_USE                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_KAWAIGARI_CNT                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FISHING_SUCCESSION            );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_RARE_COLOUR_ENC_COUNT         );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_JF_ATT_CNT                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_JF_SHOP_GET_CNT               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_QR_SCAN                       );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LERAN_WAZA                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_CAFE                          );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FINDER                        );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_CANCEL_EVOLUTION              );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_RANNNYU                       );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_NAKAYOSHI                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BATTLEROYAL                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_MONOHIROI                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_MARASADA                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SUGOI_TRAINING                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BATTLE_VIKING                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_PARLE_KIDOU                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BOX_CONTROL                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_LOMILOMI                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_IRIMA_BED                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GUZMA_BED                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_KIAVE_BED                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SUREIN_BED                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_MAOS_BED                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_MAIRE_BED                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_HAPU_BED                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_ZAIDANBOSS_BED                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DROP_ITEM                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_KIYURE                        );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_DANSA                         );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_UMI_ENCOUNT                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SABAKU_ENCOUNT                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_KISEKAE_CNT                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_BATTLEROYAL_WIN               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_RESORT_ITEM                   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_RESORT_WAIWAI                 );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_RESORT_POKAPOKA               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_QR_READ_PICTURE               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_QR_READ_POPULAR               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_QR_READ_OUTSIDE               );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FIELDSCAN                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_PLANT                         );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SHADOW                        );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_WEED                          );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GROUND                        );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_KOSOKU                        );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_YASHIGANI                     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_FISHING_BUBBLE                );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_PLAYER_BED                    );

  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_CAPTURE     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_GTSTRADE    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_EGG         );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_POKEMAME    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_FESCOIN     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_FIELDSCAN   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLEFES   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_MANTABP     );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_FISHINGPOKE );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLEROYAL );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_EVOLUTION   );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLETREE  );

  IS_COMPATIBLE_CC(Savedata::Record::RECID_LILIE_BED                     );

    /// 2バイトレコード余り。追加する時はここを書き換えていく事。
    /// ↓↓↓ レコード定義を変更したらrecord.cppのlimit_tblも変更すること！
  IS_COMPATIBLE_CC(Savedata::Record::RECID_SMALL_99                      );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_ALL_MAX                       );

  IS_COMPATIBLE_CC(Savedata::Record::RECID_4BYTE_LAST                    );
  IS_COMPATIBLE_CC(Savedata::Record::RECID_2BYTE_LAST                    );



  /*
    PokeDiaryDefine.h
  */
  IS_COMPATIBLE_CC(PDID_NULL                           );
  IS_COMPATIBLE_CC(PDID_PERSONAL_NUM                   );

  IS_COMPATIBLE_CC(PDID_LIVE_NULL                      );
  IS_COMPATIBLE_CC(PDID_LIVE_NUM                       );


  IS_COMPATIBLE_CC(PDID_COUNT_NULL                     );
  IS_COMPATIBLE_CC(PDID_COUNT_FRIENDLY_SHOP            );
  IS_COMPATIBLE_CC(PDID_COUNT_KISEKAE                  );
  IS_COMPATIBLE_CC(PDID_COUNT_GANBA_RIBBON             );
  IS_COMPATIBLE_CC(PDID_COUNT_NAME_CHANGE              );
  IS_COMPATIBLE_CC(PDID_COUNT_SODATEYA_TAMAGO          );
  IS_COMPATIBLE_CC(PDID_COUNT_ID_PRESENT               );
  IS_COMPATIBLE_CC(PDID_COUNT_CAMERA_OYAJI             );
  IS_COMPATIBLE_CC(PDID_COUNT_FOSSIL                   );
  IS_COMPATIBLE_CC(PDID_COUNT_CHECK_NATSUKI            );
  IS_COMPATIBLE_CC(PDID_COUNT_PLANT_SEED               );
  IS_COMPATIBLE_CC(PDID_COUNT_GET_SEED                 );
  IS_COMPATIBLE_CC(PDID_COUNT_BP_SHOP                  );
  IS_COMPATIBLE_CC(PDID_COUNT_WIN_BH_CHAMP             );
  IS_COMPATIBLE_CC(PDID_COUNT_DENDOU                   );
  IS_COMPATIBLE_CC(PDID_COUNT_GTS_POKEPUT              );
  IS_COMPATIBLE_CC(PDID_COUNT_GTS_POKETRADE            );
  IS_COMPATIBLE_CC(PDID_COUNT_MTRADE                   );
  IS_COMPATIBLE_CC(PDID_COUNT_P2PTRADE                 );
  IS_COMPATIBLE_CC(PDID_COUNT_BSPOT                    );
  IS_COMPATIBLE_CC(PDID_COUNT_JF_ATTLACTION            );
  IS_COMPATIBLE_CC(PDID_COUNT_P2PBATTLE                );
  IS_COMPATIBLE_CC(PDID_COUNT_JF_SHOP                  );
  IS_COMPATIBLE_CC(PDID_COUNT_RIDE                     );
  IS_COMPATIBLE_CC(PDID_COUNT_EFFORT                   );
  IS_COMPATIBLE_CC(PDID_COUNT_PARLE                    );
  IS_COMPATIBLE_CC(PDID_COUNT_CAFE                     );
  IS_COMPATIBLE_CC(PDID_COUNT_BATTLEROYAL              );
  IS_COMPATIBLE_CC(PDID_COUNT_LVUP                     );
  IS_COMPATIBLE_CC(PDID_COUNT_MARASADA                 );
  IS_COMPATIBLE_CC(PDID_COUNT_MONOHIROI                );
  IS_COMPATIBLE_CC(PDID_COUNT_NAKAYOSHI                );
  IS_COMPATIBLE_CC(PDID_COUNT_LOMILOMI                 );
  IS_COMPATIBLE_CC(PDID_COUNT_TRAINING                 );
  IS_COMPATIBLE_CC(PDID_COUNT_WAZAMASHIN               );
  IS_COMPATIBLE_CC(PDID_COUNT_BATTLEBIKING             );
  IS_COMPATIBLE_CC(PDID_COUNT_POKRESORT                );
  IS_COMPATIBLE_CC(PDID_COUNT_QR                       );
  IS_COMPATIBLE_CC(PDID_COUNT_FIELDSCAN                );
  IS_COMPATIBLE_CC(PDID_COUNT_BATTLEMOVIE              );

  IS_COMPATIBLE_CC(PDID_COUNT_NUM                      );

  /*
    monsno_def.h
  */
  IS_COMPATIBLE_CC(MONSNO_NULL                         );
  IS_COMPATIBLE_CC(MONSNO_HUSIGIDANE                   );
  IS_COMPATIBLE_CC(MONSNO_HUSIGISOU                    );
  IS_COMPATIBLE_CC(MONSNO_HUSIGIBANA                   );
  IS_COMPATIBLE_CC(MONSNO_HITOKAGE                     );
  IS_COMPATIBLE_CC(MONSNO_RIZAADO                      );
  IS_COMPATIBLE_CC(MONSNO_RIZAADON                     );
  IS_COMPATIBLE_CC(MONSNO_ZENIGAME                     );
  IS_COMPATIBLE_CC(MONSNO_KAMEERU                      );
  IS_COMPATIBLE_CC(MONSNO_KAMEKKUSU                    );
  IS_COMPATIBLE_CC(MONSNO_KYATAPII                     );
  IS_COMPATIBLE_CC(MONSNO_TORANSERU                    );
  IS_COMPATIBLE_CC(MONSNO_BATAHURII                    );
  IS_COMPATIBLE_CC(MONSNO_BIIDORU                      );
  IS_COMPATIBLE_CC(MONSNO_KOKUUN                       );
  IS_COMPATIBLE_CC(MONSNO_SUPIAA                       );
  IS_COMPATIBLE_CC(MONSNO_POPPO                        );
  IS_COMPATIBLE_CC(MONSNO_PIZYON                       );
  IS_COMPATIBLE_CC(MONSNO_PIZYOTTO                     );
  IS_COMPATIBLE_CC(MONSNO_KORATTA                      );
  IS_COMPATIBLE_CC(MONSNO_RATTA                        );
  IS_COMPATIBLE_CC(MONSNO_ONISUZUME                    );
  IS_COMPATIBLE_CC(MONSNO_ONIDORIRU                    );
  IS_COMPATIBLE_CC(MONSNO_AABO                         );
  IS_COMPATIBLE_CC(MONSNO_AABOKKU                      );
  IS_COMPATIBLE_CC(MONSNO_PIKATYUU                     );
  IS_COMPATIBLE_CC(MONSNO_RAITYUU                      );
  IS_COMPATIBLE_CC(MONSNO_SANDO                        );
  IS_COMPATIBLE_CC(MONSNO_SANDOPAN                     );
  IS_COMPATIBLE_CC(MONSNO_NIDORAN_F                    );
  IS_COMPATIBLE_CC(MONSNO_NIDORIINA                    );
  IS_COMPATIBLE_CC(MONSNO_NIDOKUIN                     );
  IS_COMPATIBLE_CC(MONSNO_NIDORAN_M                    );
  IS_COMPATIBLE_CC(MONSNO_NIDORIINO                    );
  IS_COMPATIBLE_CC(MONSNO_NIDOKINGU                    );
  IS_COMPATIBLE_CC(MONSNO_PIPPI                        );
  IS_COMPATIBLE_CC(MONSNO_PIKUSII                      );
  IS_COMPATIBLE_CC(MONSNO_ROKON                        );
  IS_COMPATIBLE_CC(MONSNO_KYUUKON                      );
  IS_COMPATIBLE_CC(MONSNO_PURIN                        );
  IS_COMPATIBLE_CC(MONSNO_PUKURIN                      );
  IS_COMPATIBLE_CC(MONSNO_ZUBATTO                      );
  IS_COMPATIBLE_CC(MONSNO_GORUBATTO                    );
  IS_COMPATIBLE_CC(MONSNO_NAZONOKUSA                   );
  IS_COMPATIBLE_CC(MONSNO_KUSAIHANA                    );
  IS_COMPATIBLE_CC(MONSNO_RAHURESIA                    );
  IS_COMPATIBLE_CC(MONSNO_PARASU                       );
  IS_COMPATIBLE_CC(MONSNO_PARASEKUTO                   );
  IS_COMPATIBLE_CC(MONSNO_KONPAN                       );
  IS_COMPATIBLE_CC(MONSNO_MORUFON                      );
  IS_COMPATIBLE_CC(MONSNO_DHIGUDA                      );
  IS_COMPATIBLE_CC(MONSNO_DAGUTORIO                    );
  IS_COMPATIBLE_CC(MONSNO_NYAASU                       );
  IS_COMPATIBLE_CC(MONSNO_PERUSIAN                     );
  IS_COMPATIBLE_CC(MONSNO_KODAKKU                      );
  IS_COMPATIBLE_CC(MONSNO_GORUDAKKU                    );
  IS_COMPATIBLE_CC(MONSNO_MANKII                       );
  IS_COMPATIBLE_CC(MONSNO_OKORIZARU                    );
  IS_COMPATIBLE_CC(MONSNO_GAADHI                       );
  IS_COMPATIBLE_CC(MONSNO_UINDHI                       );
  IS_COMPATIBLE_CC(MONSNO_NYOROMO                      );
  IS_COMPATIBLE_CC(MONSNO_NYOROZO                      );
  IS_COMPATIBLE_CC(MONSNO_NYOROBON                     );
  IS_COMPATIBLE_CC(MONSNO_KEESHI                       );
  IS_COMPATIBLE_CC(MONSNO_YUNGERAA                     );
  IS_COMPATIBLE_CC(MONSNO_HUUDHIN                      );
  IS_COMPATIBLE_CC(MONSNO_WANRIKII                     );
  IS_COMPATIBLE_CC(MONSNO_GOORIKII                     );
  IS_COMPATIBLE_CC(MONSNO_KAIRIKII                     );
  IS_COMPATIBLE_CC(MONSNO_MADATUBOMI                   );
  IS_COMPATIBLE_CC(MONSNO_UTUDON                       );
  IS_COMPATIBLE_CC(MONSNO_UTUBOTTO                     );
  IS_COMPATIBLE_CC(MONSNO_MENOKURAGE                   );
  IS_COMPATIBLE_CC(MONSNO_DOKUKURAGE                   );
  IS_COMPATIBLE_CC(MONSNO_ISITUBUTE                    );
  IS_COMPATIBLE_CC(MONSNO_GOROON                       );
  IS_COMPATIBLE_CC(MONSNO_GOROONYA                     );
  IS_COMPATIBLE_CC(MONSNO_PONIITA                      );
  IS_COMPATIBLE_CC(MONSNO_GYAROPPU                     );
  IS_COMPATIBLE_CC(MONSNO_YADON                        );
  IS_COMPATIBLE_CC(MONSNO_YADORAN                      );
  IS_COMPATIBLE_CC(MONSNO_KOIRU                        );
  IS_COMPATIBLE_CC(MONSNO_REAKOIRU                     );
  IS_COMPATIBLE_CC(MONSNO_KAMONEGI                     );
  IS_COMPATIBLE_CC(MONSNO_DOODOO                       );
  IS_COMPATIBLE_CC(MONSNO_DOODORIO                     );
  IS_COMPATIBLE_CC(MONSNO_PAUWAU                       );
  IS_COMPATIBLE_CC(MONSNO_ZYUGON                       );
  IS_COMPATIBLE_CC(MONSNO_BETOBETAA                    );
  IS_COMPATIBLE_CC(MONSNO_BETOBETON                    );
  IS_COMPATIBLE_CC(MONSNO_SHERUDAA                     );
  IS_COMPATIBLE_CC(MONSNO_PARUSHEN                     );
  IS_COMPATIBLE_CC(MONSNO_GOOSU                        );
  IS_COMPATIBLE_CC(MONSNO_GOOSUTO                      );
  IS_COMPATIBLE_CC(MONSNO_GENGAA                       );
  IS_COMPATIBLE_CC(MONSNO_IWAAKU                       );
  IS_COMPATIBLE_CC(MONSNO_SURIIPU                      );
  IS_COMPATIBLE_CC(MONSNO_SURIIPAA                     );
  IS_COMPATIBLE_CC(MONSNO_KURABU                       );
  IS_COMPATIBLE_CC(MONSNO_KINGURAA                     );
  IS_COMPATIBLE_CC(MONSNO_BIRIRIDAMA                   );
  IS_COMPATIBLE_CC(MONSNO_MARUMAIN                     );
  IS_COMPATIBLE_CC(MONSNO_TAMATAMA                     );
  IS_COMPATIBLE_CC(MONSNO_NASSII                       );
  IS_COMPATIBLE_CC(MONSNO_KARAKARA                     );
  IS_COMPATIBLE_CC(MONSNO_GARAGARA                     );
  IS_COMPATIBLE_CC(MONSNO_SAWAMURAA                    );
  IS_COMPATIBLE_CC(MONSNO_EBIWARAA                     );
  IS_COMPATIBLE_CC(MONSNO_BERORINGA                    );
  IS_COMPATIBLE_CC(MONSNO_DOGAASU                      );
  IS_COMPATIBLE_CC(MONSNO_MATADOGASU                   );
  IS_COMPATIBLE_CC(MONSNO_SAIHOON                      );
  IS_COMPATIBLE_CC(MONSNO_SAIDON                       );
  IS_COMPATIBLE_CC(MONSNO_RAKKII                       );
  IS_COMPATIBLE_CC(MONSNO_MONZYARA                     );
  IS_COMPATIBLE_CC(MONSNO_GARUURA                      );
  IS_COMPATIBLE_CC(MONSNO_TATTUU                       );
  IS_COMPATIBLE_CC(MONSNO_SIIDORA                      );
  IS_COMPATIBLE_CC(MONSNO_TOSAKINTO                    );
  IS_COMPATIBLE_CC(MONSNO_AZUMAOU                      );
  IS_COMPATIBLE_CC(MONSNO_HITODEMAN                    );
  IS_COMPATIBLE_CC(MONSNO_SUTAAMII                     );
  IS_COMPATIBLE_CC(MONSNO_BARIYAADO                    );
  IS_COMPATIBLE_CC(MONSNO_SUTORAIKU                    );
  IS_COMPATIBLE_CC(MONSNO_RUUZYURA                     );
  IS_COMPATIBLE_CC(MONSNO_EREBUU                       );
  IS_COMPATIBLE_CC(MONSNO_BUUBAA                       );
  IS_COMPATIBLE_CC(MONSNO_KAIROSU                      );
  IS_COMPATIBLE_CC(MONSNO_KENTAROSU                    );
  IS_COMPATIBLE_CC(MONSNO_KOIKINGU                     );
  IS_COMPATIBLE_CC(MONSNO_GYARADOSU                    );
  IS_COMPATIBLE_CC(MONSNO_RAPURASU                     );
  IS_COMPATIBLE_CC(MONSNO_METAMON                      );
  IS_COMPATIBLE_CC(MONSNO_IIBUI                        );
  IS_COMPATIBLE_CC(MONSNO_SYAWAAZU                     );
  IS_COMPATIBLE_CC(MONSNO_SANDAASU                     );
  IS_COMPATIBLE_CC(MONSNO_BUUSUTAA                     );
  IS_COMPATIBLE_CC(MONSNO_PORIGON                      );
  IS_COMPATIBLE_CC(MONSNO_OMUNAITO                     );
  IS_COMPATIBLE_CC(MONSNO_OMUSUTAA                     );
  IS_COMPATIBLE_CC(MONSNO_KABUTO                       );
  IS_COMPATIBLE_CC(MONSNO_KABUTOPUSU                   );
  IS_COMPATIBLE_CC(MONSNO_PUTERA                       );
  IS_COMPATIBLE_CC(MONSNO_KABIGON                      );
  IS_COMPATIBLE_CC(MONSNO_HURIIZAA                     );
  IS_COMPATIBLE_CC(MONSNO_SANDAA                       );
  IS_COMPATIBLE_CC(MONSNO_FAIYAA                       );
  IS_COMPATIBLE_CC(MONSNO_MINIRYUU                     );
  IS_COMPATIBLE_CC(MONSNO_HAKURYUU                     );
  IS_COMPATIBLE_CC(MONSNO_KAIRYUU                      );
  IS_COMPATIBLE_CC(MONSNO_MYUUTUU                      );
  IS_COMPATIBLE_CC(MONSNO_MYUU                         );
  IS_COMPATIBLE_CC(MONSNO_TIKORIITA                    );
  IS_COMPATIBLE_CC(MONSNO_BEIRIIHU                     );
  IS_COMPATIBLE_CC(MONSNO_MEGANIUMU                    );
  IS_COMPATIBLE_CC(MONSNO_HINOARASI                    );
  IS_COMPATIBLE_CC(MONSNO_MAGUMARASI                   );
  IS_COMPATIBLE_CC(MONSNO_BAKUHUUN                     );
  IS_COMPATIBLE_CC(MONSNO_WANINOKO                     );
  IS_COMPATIBLE_CC(MONSNO_ARIGEITU                     );
  IS_COMPATIBLE_CC(MONSNO_OODAIRU                      );
  IS_COMPATIBLE_CC(MONSNO_OTATI                        );
  IS_COMPATIBLE_CC(MONSNO_OOTATI                       );
  IS_COMPATIBLE_CC(MONSNO_HOOHOO                       );
  IS_COMPATIBLE_CC(MONSNO_YORUNOZUKU                   );
  IS_COMPATIBLE_CC(MONSNO_REDHIBA                      );
  IS_COMPATIBLE_CC(MONSNO_REDHIAN                      );
  IS_COMPATIBLE_CC(MONSNO_ITOMARU                      );
  IS_COMPATIBLE_CC(MONSNO_ARIADOSU                     );
  IS_COMPATIBLE_CC(MONSNO_KUROBATTO                    );
  IS_COMPATIBLE_CC(MONSNO_TYONTII                      );
  IS_COMPATIBLE_CC(MONSNO_RANTAAN                      );
  IS_COMPATIBLE_CC(MONSNO_PITYUU                       );
  IS_COMPATIBLE_CC(MONSNO_PHI                          );
  IS_COMPATIBLE_CC(MONSNO_PUPURIN                      );
  IS_COMPATIBLE_CC(MONSNO_TOGEPII                      );
  IS_COMPATIBLE_CC(MONSNO_TOGETIKKU                    );
  IS_COMPATIBLE_CC(MONSNO_NEITHI                       );
  IS_COMPATIBLE_CC(MONSNO_NEITHIO                      );
  IS_COMPATIBLE_CC(MONSNO_MERIIPU                      );
  IS_COMPATIBLE_CC(MONSNO_MOKOKO                       );
  IS_COMPATIBLE_CC(MONSNO_DENRYUU                      );
  IS_COMPATIBLE_CC(MONSNO_KIREIHANA                    );
  IS_COMPATIBLE_CC(MONSNO_MARIRU                       );
  IS_COMPATIBLE_CC(MONSNO_MARIRURI                     );
  IS_COMPATIBLE_CC(MONSNO_USOKKII                      );
  IS_COMPATIBLE_CC(MONSNO_NYOROTONO                    );
  IS_COMPATIBLE_CC(MONSNO_HANEKKO                      );
  IS_COMPATIBLE_CC(MONSNO_POPOKKO                      );
  IS_COMPATIBLE_CC(MONSNO_WATAKKO                      );
  IS_COMPATIBLE_CC(MONSNO_EIPAMU                       );
  IS_COMPATIBLE_CC(MONSNO_HIMANATTU                    );
  IS_COMPATIBLE_CC(MONSNO_KIMAWARI                     );
  IS_COMPATIBLE_CC(MONSNO_YANYANMA                     );
  IS_COMPATIBLE_CC(MONSNO_UPAA                         );
  IS_COMPATIBLE_CC(MONSNO_NUOO                         );
  IS_COMPATIBLE_CC(MONSNO_EEFI                         );
  IS_COMPATIBLE_CC(MONSNO_BURAKKII                     );
  IS_COMPATIBLE_CC(MONSNO_YAMIKARASU                   );
  IS_COMPATIBLE_CC(MONSNO_YADOKINGU                    );
  IS_COMPATIBLE_CC(MONSNO_MUUMA                        );
  IS_COMPATIBLE_CC(MONSNO_ANNOON                       );
  IS_COMPATIBLE_CC(MONSNO_SOONANSU                     );
  IS_COMPATIBLE_CC(MONSNO_KIRINRIKI                    );
  IS_COMPATIBLE_CC(MONSNO_KUNUGIDAMA                   );
  IS_COMPATIBLE_CC(MONSNO_FORETOSU                     );
  IS_COMPATIBLE_CC(MONSNO_NOKOTTI                      );
  IS_COMPATIBLE_CC(MONSNO_GURAIGAA                     );
  IS_COMPATIBLE_CC(MONSNO_HAGANEERU                    );
  IS_COMPATIBLE_CC(MONSNO_BURUU                        );
  IS_COMPATIBLE_CC(MONSNO_GURANBURU                    );
  IS_COMPATIBLE_CC(MONSNO_HARIISEN                     );
  IS_COMPATIBLE_CC(MONSNO_HASSAMU                      );
  IS_COMPATIBLE_CC(MONSNO_TUBOTUBO                     );
  IS_COMPATIBLE_CC(MONSNO_HERAKUROSU                   );
  IS_COMPATIBLE_CC(MONSNO_NYUURA                       );
  IS_COMPATIBLE_CC(MONSNO_HIMEGUMA                     );
  IS_COMPATIBLE_CC(MONSNO_RINGUMA                      );
  IS_COMPATIBLE_CC(MONSNO_MAGUMAGGU                    );
  IS_COMPATIBLE_CC(MONSNO_MAGUKARUGO                   );
  IS_COMPATIBLE_CC(MONSNO_URIMUU                       );
  IS_COMPATIBLE_CC(MONSNO_INOMUU                       );
  IS_COMPATIBLE_CC(MONSNO_SANIIGO                      );
  IS_COMPATIBLE_CC(MONSNO_TEPPOUO                      );
  IS_COMPATIBLE_CC(MONSNO_OKUTAN                       );
  IS_COMPATIBLE_CC(MONSNO_DERIBAADO                    );
  IS_COMPATIBLE_CC(MONSNO_MANTAIN                      );
  IS_COMPATIBLE_CC(MONSNO_EAAMUDO                      );
  IS_COMPATIBLE_CC(MONSNO_DERUBIRU                     );
  IS_COMPATIBLE_CC(MONSNO_HERUGAA                      );
  IS_COMPATIBLE_CC(MONSNO_KINGUDORA                    );
  IS_COMPATIBLE_CC(MONSNO_GOMAZOU                      );
  IS_COMPATIBLE_CC(MONSNO_DONFAN                       );
  IS_COMPATIBLE_CC(MONSNO_PORIGON2                     );
  IS_COMPATIBLE_CC(MONSNO_ODOSISI                      );
  IS_COMPATIBLE_CC(MONSNO_DOOBURU                      );
  IS_COMPATIBLE_CC(MONSNO_BARUKII                      );
  IS_COMPATIBLE_CC(MONSNO_KAPOERAA                     );
  IS_COMPATIBLE_CC(MONSNO_MUTYUURU                     );
  IS_COMPATIBLE_CC(MONSNO_EREKIDDO                     );
  IS_COMPATIBLE_CC(MONSNO_BUBHI                        );
  IS_COMPATIBLE_CC(MONSNO_MIRUTANKU                    );
  IS_COMPATIBLE_CC(MONSNO_HAPINASU                     );
  IS_COMPATIBLE_CC(MONSNO_RAIKOU                       );
  IS_COMPATIBLE_CC(MONSNO_ENTEI                        );
  IS_COMPATIBLE_CC(MONSNO_SUIKUN                       );
  IS_COMPATIBLE_CC(MONSNO_YOOGIRASU                    );
  IS_COMPATIBLE_CC(MONSNO_SANAGIRASU                   );
  IS_COMPATIBLE_CC(MONSNO_BANGIRASU                    );
  IS_COMPATIBLE_CC(MONSNO_RUGIA                        );
  IS_COMPATIBLE_CC(MONSNO_HOUOU                        );
  IS_COMPATIBLE_CC(MONSNO_SEREBHI                      );
  IS_COMPATIBLE_CC(MONSNO_KIMORI                       );
  IS_COMPATIBLE_CC(MONSNO_ZYUPUTORU                    );
  IS_COMPATIBLE_CC(MONSNO_ZYUKAIN                      );
  IS_COMPATIBLE_CC(MONSNO_ATYAMO                       );
  IS_COMPATIBLE_CC(MONSNO_WAKASYAMO                    );
  IS_COMPATIBLE_CC(MONSNO_BASYAAMO                     );
  IS_COMPATIBLE_CC(MONSNO_MIZUGOROU                    );
  IS_COMPATIBLE_CC(MONSNO_NUMAKUROO                    );
  IS_COMPATIBLE_CC(MONSNO_RAGURAAZI                    );
  IS_COMPATIBLE_CC(MONSNO_POTIENA                      );
  IS_COMPATIBLE_CC(MONSNO_GURAENA                      );
  IS_COMPATIBLE_CC(MONSNO_ZIGUZAGUMA                   );
  IS_COMPATIBLE_CC(MONSNO_MASSUGUMA                    );
  IS_COMPATIBLE_CC(MONSNO_KEMUSSO                      );
  IS_COMPATIBLE_CC(MONSNO_KARASARISU                   );
  IS_COMPATIBLE_CC(MONSNO_AGEHANTO                     );
  IS_COMPATIBLE_CC(MONSNO_MAYURUDO                     );
  IS_COMPATIBLE_CC(MONSNO_DOKUKEIRU                    );
  IS_COMPATIBLE_CC(MONSNO_HASUBOO                      );
  IS_COMPATIBLE_CC(MONSNO_HASUBURERO                   );
  IS_COMPATIBLE_CC(MONSNO_RUNPAPPA                     );
  IS_COMPATIBLE_CC(MONSNO_TANEBOO                      );
  IS_COMPATIBLE_CC(MONSNO_KONOHANA                     );
  IS_COMPATIBLE_CC(MONSNO_DAATENGU                     );
  IS_COMPATIBLE_CC(MONSNO_SUBAME                       );
  IS_COMPATIBLE_CC(MONSNO_OOSUBAME                     );
  IS_COMPATIBLE_CC(MONSNO_KYAMOME                      );
  IS_COMPATIBLE_CC(MONSNO_PERIPPAA                     );
  IS_COMPATIBLE_CC(MONSNO_RARUTOSU                     );
  IS_COMPATIBLE_CC(MONSNO_KIRURIA                      );
  IS_COMPATIBLE_CC(MONSNO_SAANAITO                     );
  IS_COMPATIBLE_CC(MONSNO_AMETAMA                      );
  IS_COMPATIBLE_CC(MONSNO_AMEMOOSU                     );
  IS_COMPATIBLE_CC(MONSNO_KINOKOKO                     );
  IS_COMPATIBLE_CC(MONSNO_KINOGASSA                    );
  IS_COMPATIBLE_CC(MONSNO_NAMAKERO                     );
  IS_COMPATIBLE_CC(MONSNO_YARUKIMONO                   );
  IS_COMPATIBLE_CC(MONSNO_KEKKINGU                     );
  IS_COMPATIBLE_CC(MONSNO_TUTININ                      );
  IS_COMPATIBLE_CC(MONSNO_TEKKANIN                     );
  IS_COMPATIBLE_CC(MONSNO_NUKENIN                      );
  IS_COMPATIBLE_CC(MONSNO_GONYONYO                     );
  IS_COMPATIBLE_CC(MONSNO_DOGOOMU                      );
  IS_COMPATIBLE_CC(MONSNO_BAKUONGU                     );
  IS_COMPATIBLE_CC(MONSNO_MAKUNOSITA                   );
  IS_COMPATIBLE_CC(MONSNO_HARITEYAMA                   );
  IS_COMPATIBLE_CC(MONSNO_RURIRI                       );
  IS_COMPATIBLE_CC(MONSNO_NOZUPASU                     );
  IS_COMPATIBLE_CC(MONSNO_ENEKO                        );
  IS_COMPATIBLE_CC(MONSNO_ENEKORORO                    );
  IS_COMPATIBLE_CC(MONSNO_YAMIRAMI                     );
  IS_COMPATIBLE_CC(MONSNO_KUTIITO                      );
  IS_COMPATIBLE_CC(MONSNO_KOKODORA                     );
  IS_COMPATIBLE_CC(MONSNO_KODORA                       );
  IS_COMPATIBLE_CC(MONSNO_BOSUGODORA                   );
  IS_COMPATIBLE_CC(MONSNO_ASANAN                       );
  IS_COMPATIBLE_CC(MONSNO_TYAAREMU                     );
  IS_COMPATIBLE_CC(MONSNO_RAKURAI                      );
  IS_COMPATIBLE_CC(MONSNO_RAIBORUTO                    );
  IS_COMPATIBLE_CC(MONSNO_PURASURU                     );
  IS_COMPATIBLE_CC(MONSNO_MAINAN                       );
  IS_COMPATIBLE_CC(MONSNO_BARUBIITO                    );
  IS_COMPATIBLE_CC(MONSNO_IRUMIIZE                     );
  IS_COMPATIBLE_CC(MONSNO_ROZERIA                      );
  IS_COMPATIBLE_CC(MONSNO_GOKURIN                      );
  IS_COMPATIBLE_CC(MONSNO_MARUNOOMU                    );
  IS_COMPATIBLE_CC(MONSNO_KIBANIA                      );
  IS_COMPATIBLE_CC(MONSNO_SAMEHADAA                    );
  IS_COMPATIBLE_CC(MONSNO_HOERUKO                      );
  IS_COMPATIBLE_CC(MONSNO_HOERUOO                      );
  IS_COMPATIBLE_CC(MONSNO_DONMERU                      );
  IS_COMPATIBLE_CC(MONSNO_BAKUUDA                      );
  IS_COMPATIBLE_CC(MONSNO_KOOTASU                      );
  IS_COMPATIBLE_CC(MONSNO_BANEBUU                      );
  IS_COMPATIBLE_CC(MONSNO_BUUPIGGU                     );
  IS_COMPATIBLE_CC(MONSNO_PATTIIRU                     );
  IS_COMPATIBLE_CC(MONSNO_NAKKURAA                     );
  IS_COMPATIBLE_CC(MONSNO_BIBURAABA                    );
  IS_COMPATIBLE_CC(MONSNO_HURAIGON                     );
  IS_COMPATIBLE_CC(MONSNO_SABONEA                      );
  IS_COMPATIBLE_CC(MONSNO_NOKUTASU                     );
  IS_COMPATIBLE_CC(MONSNO_TIRUTTO                      );
  IS_COMPATIBLE_CC(MONSNO_TIRUTARISU                   );
  IS_COMPATIBLE_CC(MONSNO_ZANGUUSU                     );
  IS_COMPATIBLE_CC(MONSNO_HABUNEEKU                    );
  IS_COMPATIBLE_CC(MONSNO_RUNATOON                     );
  IS_COMPATIBLE_CC(MONSNO_SORUROKKU                    );
  IS_COMPATIBLE_CC(MONSNO_DOZYOTTI                     );
  IS_COMPATIBLE_CC(MONSNO_NAMAZUN                      );
  IS_COMPATIBLE_CC(MONSNO_HEIGANI                      );
  IS_COMPATIBLE_CC(MONSNO_SIZARIGAA                    );
  IS_COMPATIBLE_CC(MONSNO_YAZIRON                      );
  IS_COMPATIBLE_CC(MONSNO_NENDOORU                     );
  IS_COMPATIBLE_CC(MONSNO_RIRIIRA                      );
  IS_COMPATIBLE_CC(MONSNO_YUREIDORU                    );
  IS_COMPATIBLE_CC(MONSNO_ANOPUSU                      );
  IS_COMPATIBLE_CC(MONSNO_AAMARUDO                     );
  IS_COMPATIBLE_CC(MONSNO_HINBASU                      );
  IS_COMPATIBLE_CC(MONSNO_MIROKAROSU                   );
  IS_COMPATIBLE_CC(MONSNO_POWARUN                      );
  IS_COMPATIBLE_CC(MONSNO_KAKUREON                     );
  IS_COMPATIBLE_CC(MONSNO_KAGEBOUZU                    );
  IS_COMPATIBLE_CC(MONSNO_ZYUPETTA                     );
  IS_COMPATIBLE_CC(MONSNO_YOMAWARU                     );
  IS_COMPATIBLE_CC(MONSNO_SAMAYOORU                    );
  IS_COMPATIBLE_CC(MONSNO_TOROPIUSU                    );
  IS_COMPATIBLE_CC(MONSNO_TIRIIN                       );
  IS_COMPATIBLE_CC(MONSNO_ABUSORU                      );
  IS_COMPATIBLE_CC(MONSNO_SOONANO                      );
  IS_COMPATIBLE_CC(MONSNO_YUKIWARASI                   );
  IS_COMPATIBLE_CC(MONSNO_ONIGOORI                     );
  IS_COMPATIBLE_CC(MONSNO_TAMAZARASI                   );
  IS_COMPATIBLE_CC(MONSNO_TODOGURAA                    );
  IS_COMPATIBLE_CC(MONSNO_TODOZERUGA                   );
  IS_COMPATIBLE_CC(MONSNO_PAARURU                      );
  IS_COMPATIBLE_CC(MONSNO_HANTEERU                     );
  IS_COMPATIBLE_CC(MONSNO_SAKURABISU                   );
  IS_COMPATIBLE_CC(MONSNO_ZIIRANSU                     );
  IS_COMPATIBLE_CC(MONSNO_RABUKASU                     );
  IS_COMPATIBLE_CC(MONSNO_TATUBEI                      );
  IS_COMPATIBLE_CC(MONSNO_KOMORUU                      );
  IS_COMPATIBLE_CC(MONSNO_BOOMANDA                     );
  IS_COMPATIBLE_CC(MONSNO_DANBARU                      );
  IS_COMPATIBLE_CC(MONSNO_METANGU                      );
  IS_COMPATIBLE_CC(MONSNO_METAGUROSU                   );
  IS_COMPATIBLE_CC(MONSNO_REZIROKKU                    );
  IS_COMPATIBLE_CC(MONSNO_REZIAISU                     );
  IS_COMPATIBLE_CC(MONSNO_REZISUTIRU                   );
  IS_COMPATIBLE_CC(MONSNO_RATHIASU                     );
  IS_COMPATIBLE_CC(MONSNO_RATHIOSU                     );
  IS_COMPATIBLE_CC(MONSNO_KAIOOGA                      );
  IS_COMPATIBLE_CC(MONSNO_GURAADON                     );
  IS_COMPATIBLE_CC(MONSNO_REKKUUZA                     );
  IS_COMPATIBLE_CC(MONSNO_ZIRAATI                      );
  IS_COMPATIBLE_CC(MONSNO_DEOKISISU                    );
  IS_COMPATIBLE_CC(MONSNO_NAETORU                      );
  IS_COMPATIBLE_CC(MONSNO_HAYASIGAME                   );
  IS_COMPATIBLE_CC(MONSNO_DODAITOSU                    );
  IS_COMPATIBLE_CC(MONSNO_HIKOZARU                     );
  IS_COMPATIBLE_CC(MONSNO_MOUKAZARU                    );
  IS_COMPATIBLE_CC(MONSNO_GOUKAZARU                    );
  IS_COMPATIBLE_CC(MONSNO_POTTYAMA                     );
  IS_COMPATIBLE_CC(MONSNO_POTTAISI                     );
  IS_COMPATIBLE_CC(MONSNO_ENPERUTO                     );
  IS_COMPATIBLE_CC(MONSNO_MUKKURU                      );
  IS_COMPATIBLE_CC(MONSNO_MUKUBAADO                    );
  IS_COMPATIBLE_CC(MONSNO_MUKUHOOKU                    );
  IS_COMPATIBLE_CC(MONSNO_BIPPA                        );
  IS_COMPATIBLE_CC(MONSNO_BIIDARU                      );
  IS_COMPATIBLE_CC(MONSNO_KOROBOOSI                    );
  IS_COMPATIBLE_CC(MONSNO_KOROTOKKU                    );
  IS_COMPATIBLE_CC(MONSNO_KORINKU                      );
  IS_COMPATIBLE_CC(MONSNO_RUKUSIO                      );
  IS_COMPATIBLE_CC(MONSNO_RENTORAA                     );
  IS_COMPATIBLE_CC(MONSNO_SUBOMII                      );
  IS_COMPATIBLE_CC(MONSNO_ROZUREIDO                    );
  IS_COMPATIBLE_CC(MONSNO_ZUGAIDOSU                    );
  IS_COMPATIBLE_CC(MONSNO_RAMUPARUDO                   );
  IS_COMPATIBLE_CC(MONSNO_TATETOPUSU                   );
  IS_COMPATIBLE_CC(MONSNO_TORIDEPUSU                   );
  IS_COMPATIBLE_CC(MONSNO_MINOMUTTI                    );
  IS_COMPATIBLE_CC(MONSNO_MINOMADAMU                   );
  IS_COMPATIBLE_CC(MONSNO_GAAMEIRU                     );
  IS_COMPATIBLE_CC(MONSNO_MITUHANII                    );
  IS_COMPATIBLE_CC(MONSNO_BIIKUIN                      );
  IS_COMPATIBLE_CC(MONSNO_PATIRISU                     );
  IS_COMPATIBLE_CC(MONSNO_BUIZERU                      );
  IS_COMPATIBLE_CC(MONSNO_HUROOZERU                    );
  IS_COMPATIBLE_CC(MONSNO_THERINBO                     );
  IS_COMPATIBLE_CC(MONSNO_THERIMU                      );
  IS_COMPATIBLE_CC(MONSNO_KARANAKUSI                   );
  IS_COMPATIBLE_CC(MONSNO_TORITODON                    );
  IS_COMPATIBLE_CC(MONSNO_ETEBOOSU                     );
  IS_COMPATIBLE_CC(MONSNO_HUWANTE                      );
  IS_COMPATIBLE_CC(MONSNO_HUWARAIDO                    );
  IS_COMPATIBLE_CC(MONSNO_MIMIRORU                     );
  IS_COMPATIBLE_CC(MONSNO_MIMIROPPU                    );
  IS_COMPATIBLE_CC(MONSNO_MUUMAAZI                     );
  IS_COMPATIBLE_CC(MONSNO_DONKARASU                    );
  IS_COMPATIBLE_CC(MONSNO_NYARUMAA                     );
  IS_COMPATIBLE_CC(MONSNO_BUNYATTO                     );
  IS_COMPATIBLE_CC(MONSNO_RIISYAN                      );
  IS_COMPATIBLE_CC(MONSNO_SUKANPUU                     );
  IS_COMPATIBLE_CC(MONSNO_SUKATANKU                    );
  IS_COMPATIBLE_CC(MONSNO_DOOMIRAA                     );
  IS_COMPATIBLE_CC(MONSNO_DOOTAKUN                     );
  IS_COMPATIBLE_CC(MONSNO_USOHATI                      );
  IS_COMPATIBLE_CC(MONSNO_MANENE                       );
  IS_COMPATIBLE_CC(MONSNO_PINPUKU                      );
  IS_COMPATIBLE_CC(MONSNO_PERAPPU                      );
  IS_COMPATIBLE_CC(MONSNO_MIKARUGE                     );
  IS_COMPATIBLE_CC(MONSNO_HUKAMARU                     );
  IS_COMPATIBLE_CC(MONSNO_GABAITO                      );
  IS_COMPATIBLE_CC(MONSNO_GABURIASU                    );
  IS_COMPATIBLE_CC(MONSNO_GONBE                        );
  IS_COMPATIBLE_CC(MONSNO_RIORU                        );
  IS_COMPATIBLE_CC(MONSNO_RUKARIO                      );
  IS_COMPATIBLE_CC(MONSNO_HIPOPOTASU                   );
  IS_COMPATIBLE_CC(MONSNO_KABARUDON                    );
  IS_COMPATIBLE_CC(MONSNO_SUKORUPI                     );
  IS_COMPATIBLE_CC(MONSNO_DORAPION                     );
  IS_COMPATIBLE_CC(MONSNO_GUREGGURU                    );
  IS_COMPATIBLE_CC(MONSNO_DOKUROGGU                    );
  IS_COMPATIBLE_CC(MONSNO_MASUKIPPA                    );
  IS_COMPATIBLE_CC(MONSNO_KEIKOUO                      );
  IS_COMPATIBLE_CC(MONSNO_NEORANTO                     );
  IS_COMPATIBLE_CC(MONSNO_TAMANTA                      );
  IS_COMPATIBLE_CC(MONSNO_YUKIKABURI                   );
  IS_COMPATIBLE_CC(MONSNO_YUKINOOO                     );
  IS_COMPATIBLE_CC(MONSNO_MANYUURA                     );
  IS_COMPATIBLE_CC(MONSNO_ZIBAKOIRU                    );
  IS_COMPATIBLE_CC(MONSNO_BEROBERUTO                   );
  IS_COMPATIBLE_CC(MONSNO_DOSAIDON                     );
  IS_COMPATIBLE_CC(MONSNO_MOZYANBO                     );
  IS_COMPATIBLE_CC(MONSNO_EREKIBURU                    );
  IS_COMPATIBLE_CC(MONSNO_BUUBAAN                      );
  IS_COMPATIBLE_CC(MONSNO_TOGEKISSU                    );
  IS_COMPATIBLE_CC(MONSNO_MEGAYANMA                    );
  IS_COMPATIBLE_CC(MONSNO_RIIFIA                       );
  IS_COMPATIBLE_CC(MONSNO_GUREISIA                     );
  IS_COMPATIBLE_CC(MONSNO_GURAION                      );
  IS_COMPATIBLE_CC(MONSNO_MANMUU                       );
  IS_COMPATIBLE_CC(MONSNO_PORIGONz                     );
  IS_COMPATIBLE_CC(MONSNO_ERUREIDO                     );
  IS_COMPATIBLE_CC(MONSNO_DAINOOZU                     );
  IS_COMPATIBLE_CC(MONSNO_YONOWAARU                    );
  IS_COMPATIBLE_CC(MONSNO_YUKIMENOKO                   );
  IS_COMPATIBLE_CC(MONSNO_ROTOMU                       );
  IS_COMPATIBLE_CC(MONSNO_YUKUSII                      );
  IS_COMPATIBLE_CC(MONSNO_EMURITTO                     );
  IS_COMPATIBLE_CC(MONSNO_AGUNOMU                      );
  IS_COMPATIBLE_CC(MONSNO_DHIARUGA                     );
  IS_COMPATIBLE_CC(MONSNO_PARUKIA                      );
  IS_COMPATIBLE_CC(MONSNO_HIIDORAN                     );
  IS_COMPATIBLE_CC(MONSNO_REZIGIGASU                   );
  IS_COMPATIBLE_CC(MONSNO_GIRATHINA                    );
  IS_COMPATIBLE_CC(MONSNO_KURESERIA                    );
  IS_COMPATIBLE_CC(MONSNO_FIONE                        );
  IS_COMPATIBLE_CC(MONSNO_MANAFI                       );
  IS_COMPATIBLE_CC(MONSNO_DAAKURAI                     );
  IS_COMPATIBLE_CC(MONSNO_SHEIMI                       );
  IS_COMPATIBLE_CC(MONSNO_ARUSEUSU                     );
  IS_COMPATIBLE_CC(MONSNO_BIKUTHINI                    );
  IS_COMPATIBLE_CC(MONSNO_TUTAAZYA                     );
  IS_COMPATIBLE_CC(MONSNO_ZYANOBII                     );
  IS_COMPATIBLE_CC(MONSNO_ZYAROODA                     );
  IS_COMPATIBLE_CC(MONSNO_POKABU                       );
  IS_COMPATIBLE_CC(MONSNO_TYAOBUU                      );
  IS_COMPATIBLE_CC(MONSNO_ENBUOO                       );
  IS_COMPATIBLE_CC(MONSNO_MIZYUMARU                    );
  IS_COMPATIBLE_CC(MONSNO_HUTATIMARU                   );
  IS_COMPATIBLE_CC(MONSNO_DAIKENKI                     );
  IS_COMPATIBLE_CC(MONSNO_MINEZUMI                     );
  IS_COMPATIBLE_CC(MONSNO_MIRUHOGGU                    );
  IS_COMPATIBLE_CC(MONSNO_YOOTERII                     );
  IS_COMPATIBLE_CC(MONSNO_HAADERIA                     );
  IS_COMPATIBLE_CC(MONSNO_MUURANDO                     );
  IS_COMPATIBLE_CC(MONSNO_TYORONEKO                    );
  IS_COMPATIBLE_CC(MONSNO_REPARUDASU                   );
  IS_COMPATIBLE_CC(MONSNO_YANAPPU                      );
  IS_COMPATIBLE_CC(MONSNO_YANAKKII                     );
  IS_COMPATIBLE_CC(MONSNO_BAOPPU                       );
  IS_COMPATIBLE_CC(MONSNO_BAOKKII                      );
  IS_COMPATIBLE_CC(MONSNO_HIYAPPU                      );
  IS_COMPATIBLE_CC(MONSNO_HIYAKKII                     );
  IS_COMPATIBLE_CC(MONSNO_MUNNA                        );
  IS_COMPATIBLE_CC(MONSNO_MUSYAANA                     );
  IS_COMPATIBLE_CC(MONSNO_MAMEPATO                     );
  IS_COMPATIBLE_CC(MONSNO_HATOOBOO                     );
  IS_COMPATIBLE_CC(MONSNO_KENHOROU                     );
  IS_COMPATIBLE_CC(MONSNO_SIMAMA                       );
  IS_COMPATIBLE_CC(MONSNO_ZEBURAIKA                    );
  IS_COMPATIBLE_CC(MONSNO_DANGORO                      );
  IS_COMPATIBLE_CC(MONSNO_GANTORU                      );
  IS_COMPATIBLE_CC(MONSNO_GIGAIASU                     );
  IS_COMPATIBLE_CC(MONSNO_KOROMORI                     );
  IS_COMPATIBLE_CC(MONSNO_KOKOROMORI                   );
  IS_COMPATIBLE_CC(MONSNO_MOGURYUU                     );
  IS_COMPATIBLE_CC(MONSNO_DORYUUZU                     );
  IS_COMPATIBLE_CC(MONSNO_TABUNNE                      );
  IS_COMPATIBLE_CC(MONSNO_DOKKORAA                     );
  IS_COMPATIBLE_CC(MONSNO_DOTEKKOTU                    );
  IS_COMPATIBLE_CC(MONSNO_ROOBUSIN                     );
  IS_COMPATIBLE_CC(MONSNO_OTAMARO                      );
  IS_COMPATIBLE_CC(MONSNO_GAMAGARU                     );
  IS_COMPATIBLE_CC(MONSNO_GAMAGEROGE                   );
  IS_COMPATIBLE_CC(MONSNO_NAGEKI                       );
  IS_COMPATIBLE_CC(MONSNO_DAGEKI                       );
  IS_COMPATIBLE_CC(MONSNO_KURUMIRU                     );
  IS_COMPATIBLE_CC(MONSNO_KURUMAYU                     );
  IS_COMPATIBLE_CC(MONSNO_HAHAKOMORI                   );
  IS_COMPATIBLE_CC(MONSNO_HUSIDE                       );
  IS_COMPATIBLE_CC(MONSNO_HOIIGA                       );
  IS_COMPATIBLE_CC(MONSNO_PENDORAA                     );
  IS_COMPATIBLE_CC(MONSNO_MONMEN                       );
  IS_COMPATIBLE_CC(MONSNO_ERUHUUN                      );
  IS_COMPATIBLE_CC(MONSNO_TYURINE                      );
  IS_COMPATIBLE_CC(MONSNO_DOREDHIA                     );
  IS_COMPATIBLE_CC(MONSNO_BASURAO                      );
  IS_COMPATIBLE_CC(MONSNO_MEGUROKO                     );
  IS_COMPATIBLE_CC(MONSNO_WARUBIRU                     );
  IS_COMPATIBLE_CC(MONSNO_WARUBIARU                    );
  IS_COMPATIBLE_CC(MONSNO_DARUMAKKA                    );
  IS_COMPATIBLE_CC(MONSNO_HIHIDARUMA                   );
  IS_COMPATIBLE_CC(MONSNO_MARAKATTI                    );
  IS_COMPATIBLE_CC(MONSNO_ISIZUMAI                     );
  IS_COMPATIBLE_CC(MONSNO_IWAPARESU                    );
  IS_COMPATIBLE_CC(MONSNO_ZURUGGU                      );
  IS_COMPATIBLE_CC(MONSNO_ZURUZUKIN                    );
  IS_COMPATIBLE_CC(MONSNO_SINBORAA                     );
  IS_COMPATIBLE_CC(MONSNO_DESUMASU                     );
  IS_COMPATIBLE_CC(MONSNO_DESUKAAN                     );
  IS_COMPATIBLE_CC(MONSNO_PUROTOOGA                    );
  IS_COMPATIBLE_CC(MONSNO_ABAGOORA                     );
  IS_COMPATIBLE_CC(MONSNO_AAKEN                        );
  IS_COMPATIBLE_CC(MONSNO_AAKEOSU                      );
  IS_COMPATIBLE_CC(MONSNO_YABUKURON                    );
  IS_COMPATIBLE_CC(MONSNO_DASUTODASU                   );
  IS_COMPATIBLE_CC(MONSNO_ZOROA                        );
  IS_COMPATIBLE_CC(MONSNO_ZOROAAKU                     );
  IS_COMPATIBLE_CC(MONSNO_TIRAAMHI                     );
  IS_COMPATIBLE_CC(MONSNO_TIRATIINO                    );
  IS_COMPATIBLE_CC(MONSNO_GOTIMU                       );
  IS_COMPATIBLE_CC(MONSNO_GOTIMIRU                     );
  IS_COMPATIBLE_CC(MONSNO_GOTIRUZERU                   );
  IS_COMPATIBLE_CC(MONSNO_YUNIRAN                      );
  IS_COMPATIBLE_CC(MONSNO_DABURAN                      );
  IS_COMPATIBLE_CC(MONSNO_RANKURUSU                    );
  IS_COMPATIBLE_CC(MONSNO_KOARUHII                     );
  IS_COMPATIBLE_CC(MONSNO_SUWANNA                      );
  IS_COMPATIBLE_CC(MONSNO_BANIPUTTI                    );
  IS_COMPATIBLE_CC(MONSNO_BANIRITTI                    );
  IS_COMPATIBLE_CC(MONSNO_BAIBANIRA                    );
  IS_COMPATIBLE_CC(MONSNO_SIKIZIKA                     );
  IS_COMPATIBLE_CC(MONSNO_MEBUKIZIKA                   );
  IS_COMPATIBLE_CC(MONSNO_EMONGA                       );
  IS_COMPATIBLE_CC(MONSNO_KABURUMO                     );
  IS_COMPATIBLE_CC(MONSNO_SYUBARUGO                    );
  IS_COMPATIBLE_CC(MONSNO_TAMAGETAKE                   );
  IS_COMPATIBLE_CC(MONSNO_MOROBARERU                   );
  IS_COMPATIBLE_CC(MONSNO_PURURIRU                     );
  IS_COMPATIBLE_CC(MONSNO_BURUNGERU                    );
  IS_COMPATIBLE_CC(MONSNO_MAMANBOU                     );
  IS_COMPATIBLE_CC(MONSNO_BATYURU                      );
  IS_COMPATIBLE_CC(MONSNO_DENTYURA                     );
  IS_COMPATIBLE_CC(MONSNO_TESSIIDO                     );
  IS_COMPATIBLE_CC(MONSNO_NATTOREI                     );
  IS_COMPATIBLE_CC(MONSNO_GIARU                        );
  IS_COMPATIBLE_CC(MONSNO_GIGIARU                      );
  IS_COMPATIBLE_CC(MONSNO_GIGIGIARU                    );
  IS_COMPATIBLE_CC(MONSNO_SIBISIRASU                   );
  IS_COMPATIBLE_CC(MONSNO_SIBIBIIRU                    );
  IS_COMPATIBLE_CC(MONSNO_SIBIRUDON                    );
  IS_COMPATIBLE_CC(MONSNO_RIGUREE                      );
  IS_COMPATIBLE_CC(MONSNO_OOBEMU                       );
  IS_COMPATIBLE_CC(MONSNO_HITOMOSI                     );
  IS_COMPATIBLE_CC(MONSNO_RANPURAA                     );
  IS_COMPATIBLE_CC(MONSNO_SYANDERA                     );
  IS_COMPATIBLE_CC(MONSNO_KIBAGO                       );
  IS_COMPATIBLE_CC(MONSNO_ONONDO                       );
  IS_COMPATIBLE_CC(MONSNO_ONONOKUSU                    );
  IS_COMPATIBLE_CC(MONSNO_KUMASYUN                     );
  IS_COMPATIBLE_CC(MONSNO_TUNBEAA                      );
  IS_COMPATIBLE_CC(MONSNO_HURIIZIO                     );
  IS_COMPATIBLE_CC(MONSNO_TYOBOMAKI                    );
  IS_COMPATIBLE_CC(MONSNO_AGIRUDAA                     );
  IS_COMPATIBLE_CC(MONSNO_MAGGYO                       );
  IS_COMPATIBLE_CC(MONSNO_KOZYOHUU                     );
  IS_COMPATIBLE_CC(MONSNO_KOZYONDO                     );
  IS_COMPATIBLE_CC(MONSNO_KURIMUGAN                    );
  IS_COMPATIBLE_CC(MONSNO_GOBITTO                      );
  IS_COMPATIBLE_CC(MONSNO_GORUUGU                      );
  IS_COMPATIBLE_CC(MONSNO_KOMATANA                     );
  IS_COMPATIBLE_CC(MONSNO_KIRIKIZAN                    );
  IS_COMPATIBLE_CC(MONSNO_BAHHURON                     );
  IS_COMPATIBLE_CC(MONSNO_WASIBON                      );
  IS_COMPATIBLE_CC(MONSNO_WHOOGURU                     );
  IS_COMPATIBLE_CC(MONSNO_BARUTYAI                     );
  IS_COMPATIBLE_CC(MONSNO_BARUZIINA                    );
  IS_COMPATIBLE_CC(MONSNO_KUITARAN                     );
  IS_COMPATIBLE_CC(MONSNO_AIANTO                       );
  IS_COMPATIBLE_CC(MONSNO_MONOZU                       );
  IS_COMPATIBLE_CC(MONSNO_ZIHEDDO                      );
  IS_COMPATIBLE_CC(MONSNO_SAZANDORA                    );
  IS_COMPATIBLE_CC(MONSNO_MERARUBA                     );
  IS_COMPATIBLE_CC(MONSNO_URUGAMOSU                    );
  IS_COMPATIBLE_CC(MONSNO_KOBARUON                     );
  IS_COMPATIBLE_CC(MONSNO_TERAKION                     );
  IS_COMPATIBLE_CC(MONSNO_BIRIZION                     );
  IS_COMPATIBLE_CC(MONSNO_TORUNEROSU                   );
  IS_COMPATIBLE_CC(MONSNO_BORUTOROSU                   );
  IS_COMPATIBLE_CC(MONSNO_RESIRAMU                     );
  IS_COMPATIBLE_CC(MONSNO_ZEKUROMU                     );
  IS_COMPATIBLE_CC(MONSNO_RANDOROSU                    );
  IS_COMPATIBLE_CC(MONSNO_KYUREMU                      );
  IS_COMPATIBLE_CC(MONSNO_KERUDHIO                     );
  IS_COMPATIBLE_CC(MONSNO_MEROETTA                     );
  IS_COMPATIBLE_CC(MONSNO_GENOSEKUTO                   );
  IS_COMPATIBLE_CC(MONSNO_HARIMARON                    );
  IS_COMPATIBLE_CC(MONSNO_HARIBOOGU                    );
  IS_COMPATIBLE_CC(MONSNO_BURIGARON                    );
  IS_COMPATIBLE_CC(MONSNO_FOKKO                        );
  IS_COMPATIBLE_CC(MONSNO_TEERUNAA                     );
  IS_COMPATIBLE_CC(MONSNO_MAFOKUSII                    );
  IS_COMPATIBLE_CC(MONSNO_KEROMATU                     );
  IS_COMPATIBLE_CC(MONSNO_GEKOGASIRA                   );
  IS_COMPATIBLE_CC(MONSNO_GEKKOUGA                     );
  IS_COMPATIBLE_CC(MONSNO_HORUBII                      );
  IS_COMPATIBLE_CC(MONSNO_HORUUDO                      );
  IS_COMPATIBLE_CC(MONSNO_YAYAKOMA                     );
  IS_COMPATIBLE_CC(MONSNO_HINOYAKOMA                   );
  IS_COMPATIBLE_CC(MONSNO_FAIAROO                      );
  IS_COMPATIBLE_CC(MONSNO_KOHUKIMUSI                   );
  IS_COMPATIBLE_CC(MONSNO_KOHUURAI                     );
  IS_COMPATIBLE_CC(MONSNO_BIBIYON                      );
  IS_COMPATIBLE_CC(MONSNO_SISIKO                       );
  IS_COMPATIBLE_CC(MONSNO_KAENZISI                     );
  IS_COMPATIBLE_CC(MONSNO_HURABEBE                     );
  IS_COMPATIBLE_CC(MONSNO_HURAETTE                     );
  IS_COMPATIBLE_CC(MONSNO_HURAAJESU                    );
  IS_COMPATIBLE_CC(MONSNO_MHEEKURU                     );
  IS_COMPATIBLE_CC(MONSNO_GOOGOOTO                     );
  IS_COMPATIBLE_CC(MONSNO_YANTYAMU                     );
  IS_COMPATIBLE_CC(MONSNO_GORONDA                      );
  IS_COMPATIBLE_CC(MONSNO_TORIMIAN                     );
  IS_COMPATIBLE_CC(MONSNO_NYASUPAA                     );
  IS_COMPATIBLE_CC(MONSNO_NYAONIKUSU                   );
  IS_COMPATIBLE_CC(MONSNO_HITOTUKI                     );
  IS_COMPATIBLE_CC(MONSNO_NIDANGIRU                    );
  IS_COMPATIBLE_CC(MONSNO_GIRUGARUDO                   );
  IS_COMPATIBLE_CC(MONSNO_SYUSYUPU                     );
  IS_COMPATIBLE_CC(MONSNO_HUREHUWAN                    );
  IS_COMPATIBLE_CC(MONSNO_PEROPPAHU                    );
  IS_COMPATIBLE_CC(MONSNO_PERORIIMU                    );
  IS_COMPATIBLE_CC(MONSNO_MAAIIKA                      );
  IS_COMPATIBLE_CC(MONSNO_KARAMANERO                   );
  IS_COMPATIBLE_CC(MONSNO_KAMETETE                     );
  IS_COMPATIBLE_CC(MONSNO_GAMENODESU                   );
  IS_COMPATIBLE_CC(MONSNO_KUZUMOO                      );
  IS_COMPATIBLE_CC(MONSNO_DORAMIDORO                   );
  IS_COMPATIBLE_CC(MONSNO_UDEPPOU                      );
  IS_COMPATIBLE_CC(MONSNO_BUROSUTAA                    );
  IS_COMPATIBLE_CC(MONSNO_ERIKITERU                    );
  IS_COMPATIBLE_CC(MONSNO_EREZAADO                     );
  IS_COMPATIBLE_CC(MONSNO_TIGORASU                     );
  IS_COMPATIBLE_CC(MONSNO_GATIGORASU                   );
  IS_COMPATIBLE_CC(MONSNO_AMARUSU                      );
  IS_COMPATIBLE_CC(MONSNO_AMARURUGA                    );
  IS_COMPATIBLE_CC(MONSNO_NINFIA                       );
  IS_COMPATIBLE_CC(MONSNO_RUTYABURU                    );
  IS_COMPATIBLE_CC(MONSNO_DEDENNE                      );
  IS_COMPATIBLE_CC(MONSNO_MERESII                      );
  IS_COMPATIBLE_CC(MONSNO_NUMERA                       );
  IS_COMPATIBLE_CC(MONSNO_NUMEIRU                      );
  IS_COMPATIBLE_CC(MONSNO_NUMERUGON                    );
  IS_COMPATIBLE_CC(MONSNO_KUREHFI                      );
  IS_COMPATIBLE_CC(MONSNO_BOKUREE                      );
  IS_COMPATIBLE_CC(MONSNO_OOROTTO                      );
  IS_COMPATIBLE_CC(MONSNO_BAKETTYA                     );
  IS_COMPATIBLE_CC(MONSNO_PANPUZIN                     );
  IS_COMPATIBLE_CC(MONSNO_KATIKOORU                    );
  IS_COMPATIBLE_CC(MONSNO_KUREBEESU                    );
  IS_COMPATIBLE_CC(MONSNO_ONBATTO                      );
  IS_COMPATIBLE_CC(MONSNO_ONBAAN                       );
  IS_COMPATIBLE_CC(MONSNO_ZERUNEASU                    );
  IS_COMPATIBLE_CC(MONSNO_IBERUTARU                    );
  IS_COMPATIBLE_CC(MONSNO_ZIGARUDE                     );
  IS_COMPATIBLE_CC(MONSNO_DHIANSII                     );
  IS_COMPATIBLE_CC(MONSNO_HUUPA                        );
  IS_COMPATIBLE_CC(MONSNO_BORUKENION                   );
  IS_COMPATIBLE_CC(MONSNO_HUKUROU1                     );
  IS_COMPATIBLE_CC(MONSNO_HUKUROU2                     );
  IS_COMPATIBLE_CC(MONSNO_HUKUROU3                     );
  IS_COMPATIBLE_CC(MONSNO_TORA1                        );
  IS_COMPATIBLE_CC(MONSNO_TORA2                        );
  IS_COMPATIBLE_CC(MONSNO_TORA3                        );
  IS_COMPATIBLE_CC(MONSNO_ASIKA1                       );
  IS_COMPATIBLE_CC(MONSNO_ASIKA2                       );
  IS_COMPATIBLE_CC(MONSNO_ASIKA3                       );
  IS_COMPATIBLE_CC(MONSNO_KUTIBASI1                    );
  IS_COMPATIBLE_CC(MONSNO_KUTIBASI2                    );
  IS_COMPATIBLE_CC(MONSNO_KUTIBASI3                    );
  IS_COMPATIBLE_CC(MONSNO_MANGUUSU1                    );
  IS_COMPATIBLE_CC(MONSNO_MANGUUSU2                    );
  IS_COMPATIBLE_CC(MONSNO_KUWAGATA1                    );
  IS_COMPATIBLE_CC(MONSNO_KUWAGATA2                    );
  IS_COMPATIBLE_CC(MONSNO_KUWAGATA3                    );
  IS_COMPATIBLE_CC(MONSNO_YASIGANI1                    );
  IS_COMPATIBLE_CC(MONSNO_YASIGANI2                    );
  IS_COMPATIBLE_CC(MONSNO_MITUSUI                      );
  IS_COMPATIBLE_CC(MONSNO_ABU1                         );
  IS_COMPATIBLE_CC(MONSNO_ABU2                         );
  IS_COMPATIBLE_CC(MONSNO_OOKAMI1                      );
  IS_COMPATIBLE_CC(MONSNO_OOKAMI2                      );
  IS_COMPATIBLE_CC(MONSNO_YOWASI                       );
  IS_COMPATIBLE_CC(MONSNO_ONIHITODE1                   );
  IS_COMPATIBLE_CC(MONSNO_ONIHITODE2                   );
  IS_COMPATIBLE_CC(MONSNO_ROBA1                        );
  IS_COMPATIBLE_CC(MONSNO_ROBA2                        );
  IS_COMPATIBLE_CC(MONSNO_MIZUGUMO1                    );
  IS_COMPATIBLE_CC(MONSNO_MIZUGUMO2                    );
  IS_COMPATIBLE_CC(MONSNO_HANAKAMA1                    );
  IS_COMPATIBLE_CC(MONSNO_HANAKAMA2                    );
  IS_COMPATIBLE_CC(MONSNO_RANPUDAKE1                   );
  IS_COMPATIBLE_CC(MONSNO_RANPUDAKE2                   );
  IS_COMPATIBLE_CC(MONSNO_DOKUTOKAGE1                  );
  IS_COMPATIBLE_CC(MONSNO_DOKUTOKAGE2                  );
  IS_COMPATIBLE_CC(MONSNO_RESSAA1                      );
  IS_COMPATIBLE_CC(MONSNO_RESSAA2                      );
  IS_COMPATIBLE_CC(MONSNO_HURUUTU1                     );
  IS_COMPATIBLE_CC(MONSNO_HURUUTU2                     );
  IS_COMPATIBLE_CC(MONSNO_HURUUTU3                     );
  IS_COMPATIBLE_CC(MONSNO_REI                          );
  IS_COMPATIBLE_CC(MONSNO_UUTAN                        );
  IS_COMPATIBLE_CC(MONSNO_HUTTOSARU                    );
  IS_COMPATIBLE_CC(MONSNO_GUSOKUMUSI1                  );
  IS_COMPATIBLE_CC(MONSNO_GUSOKUMUSI2                  );
  IS_COMPATIBLE_CC(MONSNO_SUNABA1                      );
  IS_COMPATIBLE_CC(MONSNO_SUNABA2                      );
  IS_COMPATIBLE_CC(MONSNO_NAMAKO                       );
  IS_COMPATIBLE_CC(MONSNO_GURIPUSU1                    );
  IS_COMPATIBLE_CC(MONSNO_GURIPUSU2                    );
  IS_COMPATIBLE_CC(MONSNO_NAGAREBOSI                   );
  IS_COMPATIBLE_CC(MONSNO_KOARA                        );
  IS_COMPATIBLE_CC(MONSNO_MATAMATA                     );
  IS_COMPATIBLE_CC(MONSNO_NIZITYUU                     );
  IS_COMPATIBLE_CC(MONSNO_HORAA                        );
  IS_COMPATIBLE_CC(MONSNO_KAWAHAGI                     );
  IS_COMPATIBLE_CC(MONSNO_DORAGON                      );
  IS_COMPATIBLE_CC(MONSNO_DARIN                        );
  IS_COMPATIBLE_CC(MONSNO_UROKO1                       );
  IS_COMPATIBLE_CC(MONSNO_UROKO2                       );
  IS_COMPATIBLE_CC(MONSNO_UROKO3                       );
  IS_COMPATIBLE_CC(MONSNO_THIKITORI                    );
  IS_COMPATIBLE_CC(MONSNO_THIKITYOU                    );
  IS_COMPATIBLE_CC(MONSNO_THIKIUSI                     );
  IS_COMPATIBLE_CC(MONSNO_THIKISAKANA                  );
  IS_COMPATIBLE_CC(MONSNO_BEBII                        );
  IS_COMPATIBLE_CC(MONSNO_MAYU                         );
  IS_COMPATIBLE_CC(MONSNO_NIKKOU                       );
  IS_COMPATIBLE_CC(MONSNO_GEKKOU                       );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOr                     );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOy                     );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOb                     );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOg                     );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOp                     );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOi                     );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOo                     );
  IS_COMPATIBLE_CC(MONSNO_PURIZUMU                     );
  IS_COMPATIBLE_CC(MONSNO_MABOROSI16                   );
  IS_COMPATIBLE_CC(MONSNO_MABOROSI17                   );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOs                     );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOh                     );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOd1                    );
  IS_COMPATIBLE_CC(MONSNO_BIISUTOd2                    );
  IS_COMPATIBLE_CC(MONSNO_MABOROSI18                   );
  IS_COMPATIBLE_CC(MONSNO_TAMAGO                       );
  IS_COMPATIBLE_CC(MONSNO_DAMETAMAGO                   );
  IS_COMPATIBLE_CC(MONSNO_END                          );
  IS_COMPATIBLE_CC(MONSNO_MAX                          );

  /*  form  */
  //別フォルム定義
  //フシギバナ
  IS_COMPATIBLE_REDEF(FORMNO_HUSIGIBANA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HUSIGIBANA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_HUSIGIBANA_MAX);

  //リザードン
  IS_COMPATIBLE_REDEF(FORMNO_RIZAADON_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RIZAADON_MEGAX);
  IS_COMPATIBLE_REDEF(FORMNO_RIZAADON_MEGAY);
  IS_COMPATIBLE_REDEF(FORMNO_RIZAADON_MAX);

  //カメックス
  IS_COMPATIBLE_REDEF(FORMNO_KAMEKKUSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_KAMEKKUSU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_KAMEKKUSU_MAX);

  //スピアー
  IS_COMPATIBLE_REDEF(FORMNO_SUPIAA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_SUPIAA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_SUPIAA_MAX);

  //ピジョット
  IS_COMPATIBLE_REDEF(FORMNO_PIZYOTTO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_PIZYOTTO_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_PIZYOTTO_MAX);

  //コラッタ
  IS_COMPATIBLE_REDEF(FORMNO_KORATTA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_KORATTA_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_KORATTA_MAX);

  //ラッタ
  IS_COMPATIBLE_REDEF(FORMNO_RATTA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RATTA_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_RATTA_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_RATTA_MAX);

  //ピカチュウ
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_SATOPIKAMU);
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_SATOPIKAAG);
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_SATOPIKADP);
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_SATOPIKABW);
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_SATOPIKAXY);
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_SATOPIKASM);
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_SATOPIKA20);
  IS_COMPATIBLE_REDEF(FORMNO_PIKATYUU_MAX);

  //ライチュウ
  IS_COMPATIBLE_REDEF(FORMNO_RAITYUU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RAITYUU_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_RAITYUU_MAX);

  //サンド
  IS_COMPATIBLE_REDEF(FORMNO_SANDO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_SANDO_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_SANDO_MAX);

  //サンドパン
  IS_COMPATIBLE_REDEF(FORMNO_SANDOPAN_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_SANDOPAN_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_SANDOPAN_MAX);

  //ロコン
  IS_COMPATIBLE_REDEF(FORMNO_ROKON_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ROKON_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_ROKON_MAX);

  //キュウコン
  IS_COMPATIBLE_REDEF(FORMNO_KYUUKON_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_KYUUKON_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_KYUUKON_MAX);

  //ディグダ
  IS_COMPATIBLE_REDEF(FORMNO_DHIGUDA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_DHIGUDA_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_DHIGUDA_MAX);

  //ダグトリオ
  IS_COMPATIBLE_REDEF(FORMNO_DAGUTORIO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_DAGUTORIO_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_DAGUTORIO_MAX);

  //ニャース
  IS_COMPATIBLE_REDEF(FORMNO_NYAASU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_NYAASU_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_NYAASU_MAX);

  //ペルシアン
  IS_COMPATIBLE_REDEF(FORMNO_PERUSIAN_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_PERUSIAN_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_PERUSIAN_MAX);

  //フーディン
  IS_COMPATIBLE_REDEF(FORMNO_HUUDHIN_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HUUDHIN_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_HUUDHIN_MAX);

  //イシツブテ
  IS_COMPATIBLE_REDEF(FORMNO_ISITUBUTE_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ISITUBUTE_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_ISITUBUTE_MAX);

  //ゴローン
  IS_COMPATIBLE_REDEF(FORMNO_GOROON_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GOROON_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_GOROON_MAX);

  //ゴローニャ
  IS_COMPATIBLE_REDEF(FORMNO_GOROONYA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GOROONYA_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_GOROONYA_MAX);

  //ヤドラン
  IS_COMPATIBLE_REDEF(FORMNO_YADORAN_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_YADORAN_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_YADORAN_MAX);

  //ベトベター
  IS_COMPATIBLE_REDEF(FORMNO_BETOBETAA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BETOBETAA_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_BETOBETAA_MAX);

  //ベトベトン
  IS_COMPATIBLE_REDEF(FORMNO_BETOBETON_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BETOBETON_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_BETOBETON_MAX);

  //ゲンガー
  IS_COMPATIBLE_REDEF(FORMNO_GENGAA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GENGAA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_GENGAA_MAX);

  //ナッシー
  IS_COMPATIBLE_REDEF(FORMNO_NASSII_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_NASSII_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_NASSII_MAX);

  //ガラガラ
  IS_COMPATIBLE_REDEF(FORMNO_GARAGARA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GARAGARA_ALOLA);
  IS_COMPATIBLE_REDEF(FORMNO_GARAGARA_MAX);

  //ガルーラ
  IS_COMPATIBLE_REDEF(FORMNO_GARUURA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GARUURA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_GARUURA_MAX);

  //カイロス
  IS_COMPATIBLE_REDEF(FORMNO_KAIROSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_KAIROSU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_KAIROSU_MAX);

  //ギャラドス
  IS_COMPATIBLE_REDEF(FORMNO_GYARADOSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GYARADOSU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_GYARADOSU_MAX);

  //プテラ
  IS_COMPATIBLE_REDEF(FORMNO_PUTERA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_PUTERA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_PUTERA_MAX);

  //ミュウツー
  IS_COMPATIBLE_REDEF(FORMNO_MYUUTUU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_MYUUTUU_MEGAX);
  IS_COMPATIBLE_REDEF(FORMNO_MYUUTUU_MEGAY);
  IS_COMPATIBLE_REDEF(FORMNO_MYUUTUU_MAX);

  //デンリュウ
  IS_COMPATIBLE_REDEF(FORMNO_DENRYUU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_DENRYUU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_DENRYUU_MAX);

  //アンノーン
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNA);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNB);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNC);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UND);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNE);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNF);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNG);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNH);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNI);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNJ);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNK);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNL);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNM);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNN);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNO);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNP);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNQ);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNR);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNS);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNT);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNU);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNV);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNW);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNX);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNY);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_UNZ);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_EXC);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_QUE);
  IS_COMPATIBLE_REDEF(FORMNO_ANNOON_MAX);

  //ハガネール
  IS_COMPATIBLE_REDEF(FORMNO_HAGANEERU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HAGANEERU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_HAGANEERU_MAX);

  //ハッサム
  IS_COMPATIBLE_REDEF(FORMNO_HASSAMU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HASSAMU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_HASSAMU_MAX);

  //ヘラクロス
  IS_COMPATIBLE_REDEF(FORMNO_HERAKUROSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HERAKUROSU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_HERAKUROSU_MAX);

  //ヘルガー
  IS_COMPATIBLE_REDEF(FORMNO_HERUGAA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HERUGAA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_HERUGAA_MAX);

  //バンギラス
  IS_COMPATIBLE_REDEF(FORMNO_BANGIRASU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BANGIRASU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_BANGIRASU_MAX);

  //ジュカイン
  IS_COMPATIBLE_REDEF(FORMNO_ZYUKAIN_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ZYUKAIN_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_ZYUKAIN_MAX);

  //バシャーモ
  IS_COMPATIBLE_REDEF(FORMNO_BASYAAMO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BASYAAMO_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_BASYAAMO_MAX);

  //ラグラージ
  IS_COMPATIBLE_REDEF(FORMNO_RAGURAAZI_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RAGURAAZI_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_RAGURAAZI_MAX);

  //サーナイト
  IS_COMPATIBLE_REDEF(FORMNO_SAANAITO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_SAANAITO_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_SAANAITO_MAX);

  //ヤミラミ
  IS_COMPATIBLE_REDEF(FORMNO_YAMIRAMI_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_YAMIRAMI_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_YAMIRAMI_MAX);

  //クチート
  IS_COMPATIBLE_REDEF(FORMNO_KUTIITO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_KUTIITO_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_KUTIITO_MAX);

  //ボスゴドラ
  IS_COMPATIBLE_REDEF(FORMNO_BOSUGODORA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BOSUGODORA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_BOSUGODORA_MAX);

  //チャーレム
  IS_COMPATIBLE_REDEF(FORMNO_TYAAREMU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_TYAAREMU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_TYAAREMU_MAX);

  //ライボルト
  IS_COMPATIBLE_REDEF(FORMNO_RAIBORUTO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RAIBORUTO_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_RAIBORUTO_MAX);

  //サメハダー
  IS_COMPATIBLE_REDEF(FORMNO_SAMEHADAA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_SAMEHADAA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_SAMEHADAA_MAX);

  //バクーダ
  IS_COMPATIBLE_REDEF(FORMNO_BAKUUDA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BAKUUDA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_BAKUUDA_MAX);

  //チルタリス
  IS_COMPATIBLE_REDEF(FORMNO_TIRUTARISU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_TIRUTARISU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_TIRUTARISU_MAX);

  //ポワルン
  IS_COMPATIBLE_REDEF(FORMNO_POWARUN_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_POWARUN_SUN);
  IS_COMPATIBLE_REDEF(FORMNO_POWARUN_RAIN);
  IS_COMPATIBLE_REDEF(FORMNO_POWARUN_SNOW);
  IS_COMPATIBLE_REDEF(FORMNO_POWARUN_MAX);

  //ジュペッタ
  IS_COMPATIBLE_REDEF(FORMNO_ZYUPETTA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ZYUPETTA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_ZYUPETTA_MAX);

  //アブソル
  IS_COMPATIBLE_REDEF(FORMNO_ABUSORU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ABUSORU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_ABUSORU_MAX);

  //オニゴーリ
  IS_COMPATIBLE_REDEF(FORMNO_ONIGOORI_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ONIGOORI_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_ONIGOORI_MAX);

  //ボーマンダ
  IS_COMPATIBLE_REDEF(FORMNO_BOOMANDA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BOOMANDA_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_BOOMANDA_MAX);

  //メタグロス
  IS_COMPATIBLE_REDEF(FORMNO_METAGUROSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_METAGUROSU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_METAGUROSU_MAX);

  //ラティアス
  IS_COMPATIBLE_REDEF(FORMNO_RATHIASU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RATHIASU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_RATHIASU_MAX);

  //ラティオス
  IS_COMPATIBLE_REDEF(FORMNO_RATHIOSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RATHIOSU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_RATHIOSU_MAX);

  //カイオーガ
  IS_COMPATIBLE_REDEF(FORMNO_KAIOOGA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_KAIOOGA_ALPHA);
  IS_COMPATIBLE_REDEF(FORMNO_KAIOOGA_MAX);

  //グラードン
  IS_COMPATIBLE_REDEF(FORMNO_GURAADON_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GURAADON_OMEGA);
  IS_COMPATIBLE_REDEF(FORMNO_GURAADON_MAX);

  //レックウザ
  IS_COMPATIBLE_REDEF(FORMNO_REKKUUZA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_REKKUUZA_DELTA);
  IS_COMPATIBLE_REDEF(FORMNO_REKKUUZA_MAX);

  //デオキシス
  IS_COMPATIBLE_REDEF(FORMNO_DEOKISISU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_DEOKISISU_ATTACK);
  IS_COMPATIBLE_REDEF(FORMNO_DEOKISISU_DEFENCE);
  IS_COMPATIBLE_REDEF(FORMNO_DEOKISISU_SPEED);
  IS_COMPATIBLE_REDEF(FORMNO_DEOKISISU_MAX);

  //ミノムッチ
  IS_COMPATIBLE_REDEF(FORMNO_MINOMUTTI_KUSA);
  IS_COMPATIBLE_REDEF(FORMNO_MINOMUTTI_SUNA);
  IS_COMPATIBLE_REDEF(FORMNO_MINOMUTTI_GOMI);
  IS_COMPATIBLE_REDEF(FORMNO_MINOMUTTI_MAX);

  //ミノマダム
  IS_COMPATIBLE_REDEF(FORMNO_MINOMADAMU_KUSA);
  IS_COMPATIBLE_REDEF(FORMNO_MINOMADAMU_JIMEN);
  IS_COMPATIBLE_REDEF(FORMNO_MINOMADAMU_HAGANE);
  IS_COMPATIBLE_REDEF(FORMNO_MINOMADAMU_MAX);

  //チェリム
  IS_COMPATIBLE_REDEF(FORMNO_THERIMU_NEGA);
  IS_COMPATIBLE_REDEF(FORMNO_THERIMU_POSI);
  IS_COMPATIBLE_REDEF(FORMNO_THERIMU_MAX);

  //カラナクシ
  IS_COMPATIBLE_REDEF(FORMNO_KARANAKUSI_WEST);
  IS_COMPATIBLE_REDEF(FORMNO_KARANAKUSI_EAST);
  IS_COMPATIBLE_REDEF(FORMNO_KARANAKUSI_MAX);

  //トリトドン
  IS_COMPATIBLE_REDEF(FORMNO_TORITODON_WEST);
  IS_COMPATIBLE_REDEF(FORMNO_TORITODON_EAST);
  IS_COMPATIBLE_REDEF(FORMNO_TORITODON_MAX);

  //ミミロップ
  IS_COMPATIBLE_REDEF(FORMNO_MIMIROPPU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_MIMIROPPU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_MIMIROPPU_MAX);

  //ガブリアス
  IS_COMPATIBLE_REDEF(FORMNO_GABURIASU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GABURIASU_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_GABURIASU_MAX);

  //ルカリオ
  IS_COMPATIBLE_REDEF(FORMNO_RUKARIO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RUKARIO_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_RUKARIO_MAX);

  //ユキノオー
  IS_COMPATIBLE_REDEF(FORMNO_YUKINOOO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_YUKINOOO_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_YUKINOOO_MAX);

  //エルレイド
  IS_COMPATIBLE_REDEF(FORMNO_ERUREIDO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ERUREIDO_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_ERUREIDO_MAX);

  //ロトム
  IS_COMPATIBLE_REDEF(FORMNO_ROTOMU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ROTOMU_HOT);
  IS_COMPATIBLE_REDEF(FORMNO_ROTOMU_WASH);
  IS_COMPATIBLE_REDEF(FORMNO_ROTOMU_COLD);
  IS_COMPATIBLE_REDEF(FORMNO_ROTOMU_FAN);
  IS_COMPATIBLE_REDEF(FORMNO_ROTOMU_CUT);
  IS_COMPATIBLE_REDEF(FORMNO_ROTOMU_MAX);

  //ギラティナ
  IS_COMPATIBLE_REDEF(FORMNO_GIRATHINA_ANOTHER);
  IS_COMPATIBLE_REDEF(FORMNO_GIRATHINA_ORIGIN);
  IS_COMPATIBLE_REDEF(FORMNO_GIRATHINA_MAX);

  //シェイミ
  IS_COMPATIBLE_REDEF(FORMNO_SHEIMI_LAND);
  IS_COMPATIBLE_REDEF(FORMNO_SHEIMI_SKY);
  IS_COMPATIBLE_REDEF(FORMNO_SHEIMI_MAX);

  //アルセウス
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_BATTLE);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_HIKOU);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_POISON);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_JIMEN);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_IWA);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_MUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_GHOST);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_METAL);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_FIRE);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_WATER);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_KUSA);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_ELECTRIC);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_ESPER);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_KOORI);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_DRAGON);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_AKU);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_FAIRY);
  IS_COMPATIBLE_REDEF(FORMNO_ARUSEUSU_MAX);

  //タブンネ
  IS_COMPATIBLE_REDEF(FORMNO_TABUNNE_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_TABUNNE_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_TABUNNE_MAX);

  //バスラオ
  IS_COMPATIBLE_REDEF(FORMNO_BASURAO_BLACK);
  IS_COMPATIBLE_REDEF(FORMNO_BASURAO_WHITE);
  IS_COMPATIBLE_REDEF(FORMNO_BASURAO_MAX);

  //ヒヒダルマ
  IS_COMPATIBLE_REDEF(FORMNO_HIHIDARUMA_ACTION);
  IS_COMPATIBLE_REDEF(FORMNO_HIHIDARUMA_MEDITATION);
  IS_COMPATIBLE_REDEF(FORMNO_HIHIDARUMA_MAX);

  //シキジカ
  IS_COMPATIBLE_REDEF(FORMNO_SIKIZIKA_SPRING);
  IS_COMPATIBLE_REDEF(FORMNO_SIKIZIKA_SUMMER);
  IS_COMPATIBLE_REDEF(FORMNO_SIKIZIKA_AUTUMN);
  IS_COMPATIBLE_REDEF(FORMNO_SIKIZIKA_WINTER);
  IS_COMPATIBLE_REDEF(FORMNO_SIKIZIKA_MAX);

  //メブキジカ
  IS_COMPATIBLE_REDEF(FORMNO_MEBUKIZIKA_SPRING);
  IS_COMPATIBLE_REDEF(FORMNO_MEBUKIZIKA_SUMMER);
  IS_COMPATIBLE_REDEF(FORMNO_MEBUKIZIKA_AUTUMN);
  IS_COMPATIBLE_REDEF(FORMNO_MEBUKIZIKA_WINTER);
  IS_COMPATIBLE_REDEF(FORMNO_MEBUKIZIKA_MAX);

  //トルネロス
  IS_COMPATIBLE_REDEF(FORMNO_TORUNEROSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_TORUNEROSU_AR);
  IS_COMPATIBLE_REDEF(FORMNO_TORUNEROSU_MAX);

  //ボルトロス
  IS_COMPATIBLE_REDEF(FORMNO_BORUTOROSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BORUTOROSU_AR);
  IS_COMPATIBLE_REDEF(FORMNO_BORUTOROSU_MAX);

  //ランドロス
  IS_COMPATIBLE_REDEF(FORMNO_RANDOROSU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_RANDOROSU_AR);
  IS_COMPATIBLE_REDEF(FORMNO_RANDOROSU_MAX);

  //キュレム
  IS_COMPATIBLE_REDEF(FORMNO_KYUREMU_A);
  IS_COMPATIBLE_REDEF(FORMNO_KYUREMU_WHITE);
  IS_COMPATIBLE_REDEF(FORMNO_KYUREMU_BLACK);
  IS_COMPATIBLE_REDEF(FORMNO_KYUREMU_MAX);

  //ケルディオ
  IS_COMPATIBLE_REDEF(FORMNO_KERUDHIO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_KERUDHIO_SWORD);
  IS_COMPATIBLE_REDEF(FORMNO_KERUDHIO_MAX);

  //メロエッタ
  IS_COMPATIBLE_REDEF(FORMNO_MEROETTA_VOICE);
  IS_COMPATIBLE_REDEF(FORMNO_MEROETTA_STEP);
  IS_COMPATIBLE_REDEF(FORMNO_MEROETTA_MAX);

  //ゲノセクト
  IS_COMPATIBLE_REDEF(FORMNO_GENOSEKUTO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GENOSEKUTO_AQUA);
  IS_COMPATIBLE_REDEF(FORMNO_GENOSEKUTO_INAZUMA);
  IS_COMPATIBLE_REDEF(FORMNO_GENOSEKUTO_BLAZE);
  IS_COMPATIBLE_REDEF(FORMNO_GENOSEKUTO_FREEZE);
  IS_COMPATIBLE_REDEF(FORMNO_GENOSEKUTO_MAX);

  //ゲッコウガ
  IS_COMPATIBLE_REDEF(FORMNO_GEKKOUGA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GEKKOUGA_GEKKOUGA2);
  IS_COMPATIBLE_REDEF(FORMNO_GEKKOUGA_SATOSHI);
  IS_COMPATIBLE_REDEF(FORMNO_GEKKOUGA_MAX);

  //ビビヨン
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA1);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA2);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA3);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA4);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA5);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA6);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA7);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA8);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA9);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA10);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA11);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA12);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA13);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA14);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA15);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA16);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA17);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA18);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA19);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_AREA20);
  IS_COMPATIBLE_REDEF(FORMNO_BIBIYON_MAX);

  //フラベベ
  IS_COMPATIBLE_REDEF(FORMNO_HURABEBE_RED);
  IS_COMPATIBLE_REDEF(FORMNO_HURABEBE_YELLOW);
  IS_COMPATIBLE_REDEF(FORMNO_HURABEBE_ORANGE);
  IS_COMPATIBLE_REDEF(FORMNO_HURABEBE_BLUE);
  IS_COMPATIBLE_REDEF(FORMNO_HURABEBE_WHITE);
  IS_COMPATIBLE_REDEF(FORMNO_HURABEBE_MAX);

  //フラエッテ
  IS_COMPATIBLE_REDEF(FORMNO_HURAETTE_RED);
  IS_COMPATIBLE_REDEF(FORMNO_HURAETTE_YELLOW);
  IS_COMPATIBLE_REDEF(FORMNO_HURAETTE_ORANGE);
  IS_COMPATIBLE_REDEF(FORMNO_HURAETTE_BLUE);
  IS_COMPATIBLE_REDEF(FORMNO_HURAETTE_WHITE);
  IS_COMPATIBLE_REDEF(FORMNO_HURAETTE_HAKAI);
  IS_COMPATIBLE_REDEF(FORMNO_HURAETTE_MAX);

  //フラージェス
  IS_COMPATIBLE_REDEF(FORMNO_HURAAJESU_RED);
  IS_COMPATIBLE_REDEF(FORMNO_HURAAJESU_YELLOW);
  IS_COMPATIBLE_REDEF(FORMNO_HURAAJESU_ORANGE);
  IS_COMPATIBLE_REDEF(FORMNO_HURAAJESU_BLUE);
  IS_COMPATIBLE_REDEF(FORMNO_HURAAJESU_WHITE);
  IS_COMPATIBLE_REDEF(FORMNO_HURAAJESU_MAX);

  //トリミアン
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_HEART);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_STAR);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_DIAMOND);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_YOUNGLADY);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_LADY);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_GENTLEMAN);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_ANTOINETTE);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_RENJISHI);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_SPHINX);
  IS_COMPATIBLE_REDEF(FORMNO_TORIMIAN_MAX);

  //ニャオニクス
  IS_COMPATIBLE_REDEF(FORMNO_NYAONIKUSU_MALE);
  IS_COMPATIBLE_REDEF(FORMNO_NYAONIKUSU_FEMALE);
  IS_COMPATIBLE_REDEF(FORMNO_NYAONIKUSU_MAX);

  //ギルガルド
  IS_COMPATIBLE_REDEF(FORMNO_GIRUGARUDO_TATE);
  IS_COMPATIBLE_REDEF(FORMNO_GIRUGARUDO_HOKO);
  IS_COMPATIBLE_REDEF(FORMNO_GIRUGARUDO_MAX);

  //バケッチャ
  IS_COMPATIBLE_REDEF(FORMNO_BAKETTYA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_BAKETTYA_SMALL);
  IS_COMPATIBLE_REDEF(FORMNO_BAKETTYA_BIG);
  IS_COMPATIBLE_REDEF(FORMNO_BAKETTYA_KING);
  IS_COMPATIBLE_REDEF(FORMNO_BAKETTYA_MAX);

  //パンプジン
  IS_COMPATIBLE_REDEF(FORMNO_PANPUZIN_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_PANPUZIN_SMALL);
  IS_COMPATIBLE_REDEF(FORMNO_PANPUZIN_BIG);
  IS_COMPATIBLE_REDEF(FORMNO_PANPUZIN_KING);
  IS_COMPATIBLE_REDEF(FORMNO_PANPUZIN_MAX);

  //ゼルネアス
  IS_COMPATIBLE_REDEF(FORMNO_ZERUNEASU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ZERUNEASU_RADIATION);
  IS_COMPATIBLE_REDEF(FORMNO_ZERUNEASU_MAX);

  //ジガルデ
  IS_COMPATIBLE_REDEF(FORMNO_ZIGARUDE_50PER);
  IS_COMPATIBLE_REDEF(FORMNO_ZIGARUDE_10PER);
  IS_COMPATIBLE_REDEF(FORMNO_ZIGARUDE_SW10PER);
  IS_COMPATIBLE_REDEF(FORMNO_ZIGARUDE_SW50PER);
  IS_COMPATIBLE_REDEF(FORMNO_ZIGARUDE_100PER);
  IS_COMPATIBLE_REDEF(FORMNO_ZIGARUDE_MAX);

  //ディアンシー
  IS_COMPATIBLE_REDEF(FORMNO_DHIANSII_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_DHIANSII_MEGA);
  IS_COMPATIBLE_REDEF(FORMNO_DHIANSII_MAX);

  //フーパ
  IS_COMPATIBLE_REDEF(FORMNO_HUUPA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HUUPA_EARNEST);
  IS_COMPATIBLE_REDEF(FORMNO_HUUPA_MAX);

  //デカグース
  IS_COMPATIBLE_REDEF(FORMNO_MANGUUSU2_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_MANGUUSU2_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_MANGUUSU2_MAX);

  //クワガノン
  IS_COMPATIBLE_REDEF(FORMNO_KUWAGATA3_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_KUWAGATA3_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_KUWAGATA3_MAX);

  //オドリドリ
  IS_COMPATIBLE_REDEF(FORMNO_MITUSUI_FLAMENCO);
  IS_COMPATIBLE_REDEF(FORMNO_MITUSUI_CHEER);
  IS_COMPATIBLE_REDEF(FORMNO_MITUSUI_HULA);
  IS_COMPATIBLE_REDEF(FORMNO_MITUSUI_NICHIBU);
  IS_COMPATIBLE_REDEF(FORMNO_MITUSUI_MAX);

  //アブリボン
  IS_COMPATIBLE_REDEF(FORMNO_ABU2_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_ABU2_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_ABU2_MAX);

  //ルガルガン
  IS_COMPATIBLE_REDEF(FORMNO_OOKAMI2_MIDDAY);
  IS_COMPATIBLE_REDEF(FORMNO_OOKAMI2_MIDNIGHT);
  IS_COMPATIBLE_REDEF(FORMNO_OOKAMI2_MAX);

  //ヨワシ
  IS_COMPATIBLE_REDEF(FORMNO_YOWASI_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_YOWASI_GYOGUN_L);
  IS_COMPATIBLE_REDEF(FORMNO_YOWASI_MAX);

  //オニシズクモ
  IS_COMPATIBLE_REDEF(FORMNO_MIZUGUMO2_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_MIZUGUMO2_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_MIZUGUMO2_MAX);

  //ラランテス
  IS_COMPATIBLE_REDEF(FORMNO_HANAKAMA2_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HANAKAMA2_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_HANAKAMA2_MAX);

  //エンニュート
  IS_COMPATIBLE_REDEF(FORMNO_DOKUTOKAGE2_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_DOKUTOKAGE2_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_DOKUTOKAGE2_MAX);

  //シルヴァディ
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_BATTLE);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_HIKOU);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_POISON);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_JIMEN);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_IWA);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_MUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_GHOST);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_METAL);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_FIRE);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_WATER);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_KUSA);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_ELECTRIC);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_ESPER);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_KOORI);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_DRAGON);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_AKU);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_FAIRY);
  IS_COMPATIBLE_REDEF(FORMNO_GURIPUSU2_MAX);

  //メテノ
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_M_RED);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_M_ORANGE);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_M_YELLOW);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_M_GREEN);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_M_BLUE);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_M_INDIGO);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_M_PURPLE);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_RED);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_ORANGE);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_YELLOW);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_GREEN);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_BLUE);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_INDIGO);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_PURPLE);
  IS_COMPATIBLE_REDEF(FORMNO_NAGAREBOSI_MAX);

  //ミミッキュ
  IS_COMPATIBLE_REDEF(FORMNO_HORAA_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_HORAA_KUBIORE);
  IS_COMPATIBLE_REDEF(FORMNO_HORAA_NORMAL_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_HORAA_KUBIORE_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_HORAA_MAX);

  //ジャラランガ
  IS_COMPATIBLE_REDEF(FORMNO_UROKO3_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_UROKO3_NUSHI);
  IS_COMPATIBLE_REDEF(FORMNO_UROKO3_MAX);

  //ネクロズマ
  IS_COMPATIBLE_REDEF(FORMNO_PURIZUMU_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_PURIZUMU_FORM_S);
  IS_COMPATIBLE_REDEF(FORMNO_PURIZUMU_FORM_L);
  IS_COMPATIBLE_REDEF(FORMNO_PURIZUMU_FORM_N);
  IS_COMPATIBLE_REDEF(FORMNO_PURIZUMU_MAX);

  //マギアナ
  IS_COMPATIBLE_REDEF(FORMNO_MABOROSI16_PRESENT);
  IS_COMPATIBLE_REDEF(FORMNO_MABOROSI16_PAST);
  IS_COMPATIBLE_REDEF(FORMNO_MABOROSI16_MAX);

  IS_COMPATIBLE_REDEF(FORMNO_TAMAGO_NORMAL);
  IS_COMPATIBLE_REDEF(FORMNO_TAMAGO_MANAFI);
  IS_COMPATIBLE_REDEF(FORMNO_TAMAGO_MAX);

  IS_COMPATIBLE_REDEF(OTHER_FORM_MAX);

  // １種族あたりのフォルム数の最大値
  IS_COMPATIBLE_REDEF(MAX_FORM_NUM);



  /*
    wazano_def.h
  */
  IS_COMPATIBLE_CC(WAZANO_NULL                         );
  IS_COMPATIBLE_CC(WAZANO_HATAKU                       );
  IS_COMPATIBLE_CC(WAZANO_KARATETYOPPU                 );
  IS_COMPATIBLE_CC(WAZANO_OUHUKUBINTA                  );
  IS_COMPATIBLE_CC(WAZANO_RENZOKUPANTI                 );
  IS_COMPATIBLE_CC(WAZANO_MEGATONPANTI                 );
  IS_COMPATIBLE_CC(WAZANO_NEKONIKOBAN                  );
  IS_COMPATIBLE_CC(WAZANO_HONOONOPANTI                 );
  IS_COMPATIBLE_CC(WAZANO_REITOUPANTI                  );
  IS_COMPATIBLE_CC(WAZANO_KAMINARIPANTI                );
  IS_COMPATIBLE_CC(WAZANO_HIKKAKU                      );
  IS_COMPATIBLE_CC(WAZANO_HASAMU                       );
  IS_COMPATIBLE_CC(WAZANO_HASAMIGIROTIN                );
  IS_COMPATIBLE_CC(WAZANO_KAMAITATI                    );
  IS_COMPATIBLE_CC(WAZANO_TURUGINOMAI                  );
  IS_COMPATIBLE_CC(WAZANO_IAIGIRI                      );
  IS_COMPATIBLE_CC(WAZANO_KAZEOKOSI                    );
  IS_COMPATIBLE_CC(WAZANO_TUBASADEUTU                  );
  IS_COMPATIBLE_CC(WAZANO_HUKITOBASI                   );
  IS_COMPATIBLE_CC(WAZANO_SORAWOTOBU                   );
  IS_COMPATIBLE_CC(WAZANO_SIMETUKERU                   );
  IS_COMPATIBLE_CC(WAZANO_TATAKITUKERU                 );
  IS_COMPATIBLE_CC(WAZANO_TURUNOMUTI                   );
  IS_COMPATIBLE_CC(WAZANO_HUMITUKE                     );
  IS_COMPATIBLE_CC(WAZANO_NIDOGERI                     );
  IS_COMPATIBLE_CC(WAZANO_MEGATONKIKKU                 );
  IS_COMPATIBLE_CC(WAZANO_TOBIGERI                     );
  IS_COMPATIBLE_CC(WAZANO_MAWASIGERI                   );
  IS_COMPATIBLE_CC(WAZANO_SUNAKAKE                     );
  IS_COMPATIBLE_CC(WAZANO_ZUTUKI                       );
  IS_COMPATIBLE_CC(WAZANO_TUNODETUKU                   );
  IS_COMPATIBLE_CC(WAZANO_MIDAREDUKI                   );
  IS_COMPATIBLE_CC(WAZANO_TUNODORIRU                   );
  IS_COMPATIBLE_CC(WAZANO_TAIATARI                     );
  IS_COMPATIBLE_CC(WAZANO_NOSIKAKARI                   );
  IS_COMPATIBLE_CC(WAZANO_MAKITUKU                     );
  IS_COMPATIBLE_CC(WAZANO_TOSSIN                       );
  IS_COMPATIBLE_CC(WAZANO_ABARERU                      );
  IS_COMPATIBLE_CC(WAZANO_SUTEMITAKKURU                );
  IS_COMPATIBLE_CC(WAZANO_SIPPOWOHURU                  );
  IS_COMPATIBLE_CC(WAZANO_DOKUBARI                     );
  IS_COMPATIBLE_CC(WAZANO_DABURUNIIDORU                );
  IS_COMPATIBLE_CC(WAZANO_MISAIRUBARI                  );
  IS_COMPATIBLE_CC(WAZANO_NIRAMITUKERU                 );
  IS_COMPATIBLE_CC(WAZANO_KAMITUKU                     );
  IS_COMPATIBLE_CC(WAZANO_NAKIGOE                      );
  IS_COMPATIBLE_CC(WAZANO_HOERU                        );
  IS_COMPATIBLE_CC(WAZANO_UTAU                         );
  IS_COMPATIBLE_CC(WAZANO_TYOUONPA                     );
  IS_COMPATIBLE_CC(WAZANO_SONIKKUBUUMU                 );
  IS_COMPATIBLE_CC(WAZANO_KANASIBARI                   );
  IS_COMPATIBLE_CC(WAZANO_YOUKAIEKI                    );
  IS_COMPATIBLE_CC(WAZANO_HINOKO                       );
  IS_COMPATIBLE_CC(WAZANO_KAENHOUSYA                   );
  IS_COMPATIBLE_CC(WAZANO_SIROIKIRI                    );
  IS_COMPATIBLE_CC(WAZANO_MIZUDEPPOU                   );
  IS_COMPATIBLE_CC(WAZANO_HAIDOROPONPU                 );
  IS_COMPATIBLE_CC(WAZANO_NAMINORI                     );
  IS_COMPATIBLE_CC(WAZANO_REITOUBIIMU                  );
  IS_COMPATIBLE_CC(WAZANO_HUBUKI                       );
  IS_COMPATIBLE_CC(WAZANO_SAIKEKOUSEN                  );
  IS_COMPATIBLE_CC(WAZANO_BABURUKOUSEN                 );
  IS_COMPATIBLE_CC(WAZANO_OORORABIIMU                  );
  IS_COMPATIBLE_CC(WAZANO_HAKAIKOUSEN                  );
  IS_COMPATIBLE_CC(WAZANO_TUTUKU                       );
  IS_COMPATIBLE_CC(WAZANO_DORIRUKUTIBASI               );
  IS_COMPATIBLE_CC(WAZANO_ZIGOKUGURUMA                 );
  IS_COMPATIBLE_CC(WAZANO_KETAGURI                     );
  IS_COMPATIBLE_CC(WAZANO_KAUNTAA                      );
  IS_COMPATIBLE_CC(WAZANO_TIKYUUNAGE                   );
  IS_COMPATIBLE_CC(WAZANO_KAIRIKI                      );
  IS_COMPATIBLE_CC(WAZANO_SUITORU                      );
  IS_COMPATIBLE_CC(WAZANO_MEGADOREIN                   );
  IS_COMPATIBLE_CC(WAZANO_YADORIGINOTANE               );
  IS_COMPATIBLE_CC(WAZANO_SEITYOU                      );
  IS_COMPATIBLE_CC(WAZANO_HAPPAKATTAA                  );
  IS_COMPATIBLE_CC(WAZANO_SOORAABIIMU                  );
  IS_COMPATIBLE_CC(WAZANO_DOKUNOKONA                   );
  IS_COMPATIBLE_CC(WAZANO_SIBIREGONA                   );
  IS_COMPATIBLE_CC(WAZANO_NEMURIGONA                   );
  IS_COMPATIBLE_CC(WAZANO_HANABIRANOMAI                );
  IS_COMPATIBLE_CC(WAZANO_ITOWOHAKU                    );
  IS_COMPATIBLE_CC(WAZANO_RYUUNOIKARI                  );
  IS_COMPATIBLE_CC(WAZANO_HONOONOUZU                   );
  IS_COMPATIBLE_CC(WAZANO_DENKISYOKKU                  );
  IS_COMPATIBLE_CC(WAZANO_10MANBORUTO                  );
  IS_COMPATIBLE_CC(WAZANO_DENZIHA                      );
  IS_COMPATIBLE_CC(WAZANO_KAMINARI                     );
  IS_COMPATIBLE_CC(WAZANO_IWAOTOSI                     );
  IS_COMPATIBLE_CC(WAZANO_ZISIN                        );
  IS_COMPATIBLE_CC(WAZANO_ZIWARE                       );
  IS_COMPATIBLE_CC(WAZANO_ANAWOHORU                    );
  IS_COMPATIBLE_CC(WAZANO_DOKUDOKU                     );
  IS_COMPATIBLE_CC(WAZANO_NENRIKI                      );
  IS_COMPATIBLE_CC(WAZANO_SAIKOKINESISU                );
  IS_COMPATIBLE_CC(WAZANO_SAIMINZYUTU                  );
  IS_COMPATIBLE_CC(WAZANO_YOGANOPOOZU                  );
  IS_COMPATIBLE_CC(WAZANO_KOUSOKUIDOU                  );
  IS_COMPATIBLE_CC(WAZANO_DENKOUSEKKA                  );
  IS_COMPATIBLE_CC(WAZANO_IKARI                        );
  IS_COMPATIBLE_CC(WAZANO_TEREPOOTO                    );
  IS_COMPATIBLE_CC(WAZANO_NAITOHEDDO                   );
  IS_COMPATIBLE_CC(WAZANO_MONOMANE                     );
  IS_COMPATIBLE_CC(WAZANO_IYANAOTO                     );
  IS_COMPATIBLE_CC(WAZANO_KAGEBUNSIN                   );
  IS_COMPATIBLE_CC(WAZANO_ZIKOSAISEI                   );
  IS_COMPATIBLE_CC(WAZANO_KATAKUNARU                   );
  IS_COMPATIBLE_CC(WAZANO_TIISAKUNARU                  );
  IS_COMPATIBLE_CC(WAZANO_ENMAKU                       );
  IS_COMPATIBLE_CC(WAZANO_AYASIIHIKARI                 );
  IS_COMPATIBLE_CC(WAZANO_KARANIKOMORU                 );
  IS_COMPATIBLE_CC(WAZANO_MARUKUNARU                   );
  IS_COMPATIBLE_CC(WAZANO_BARIAA                       );
  IS_COMPATIBLE_CC(WAZANO_HIKARINOKABE                 );
  IS_COMPATIBLE_CC(WAZANO_KUROIKIRI                    );
  IS_COMPATIBLE_CC(WAZANO_RIHUREKUTAA                  );
  IS_COMPATIBLE_CC(WAZANO_KIAIDAME                     );
  IS_COMPATIBLE_CC(WAZANO_GAMAN                        );
  IS_COMPATIBLE_CC(WAZANO_YUBIWOHURU                   );
  IS_COMPATIBLE_CC(WAZANO_OUMUGAESI                    );
  IS_COMPATIBLE_CC(WAZANO_ZIBAKU                       );
  IS_COMPATIBLE_CC(WAZANO_TAMAGOBAKUDAN                );
  IS_COMPATIBLE_CC(WAZANO_SITADENAMERU                 );
  IS_COMPATIBLE_CC(WAZANO_SUMOGGU                      );
  IS_COMPATIBLE_CC(WAZANO_HEDOROKOUGEKI                );
  IS_COMPATIBLE_CC(WAZANO_HONEKONBOU                   );
  IS_COMPATIBLE_CC(WAZANO_DAIMONZI                     );
  IS_COMPATIBLE_CC(WAZANO_TAKINOBORI                   );
  IS_COMPATIBLE_CC(WAZANO_KARADEHASAMU                 );
  IS_COMPATIBLE_CC(WAZANO_SUPIIDOSUTAA                 );
  IS_COMPATIBLE_CC(WAZANO_ROKETTOZUTUKI                );
  IS_COMPATIBLE_CC(WAZANO_TOGEKYANON                   );
  IS_COMPATIBLE_CC(WAZANO_KARAMITUKU                   );
  IS_COMPATIBLE_CC(WAZANO_DOWASURE                     );
  IS_COMPATIBLE_CC(WAZANO_SUPUUNMAGE                   );
  IS_COMPATIBLE_CC(WAZANO_TAMAGOUMI                    );
  IS_COMPATIBLE_CC(WAZANO_TOBIHIZAGERI                 );
  IS_COMPATIBLE_CC(WAZANO_HEBINIRAMI                   );
  IS_COMPATIBLE_CC(WAZANO_YUMEKUI                      );
  IS_COMPATIBLE_CC(WAZANO_DOKUGASU                     );
  IS_COMPATIBLE_CC(WAZANO_TAMANAGE                     );
  IS_COMPATIBLE_CC(WAZANO_KYUUKETU                     );
  IS_COMPATIBLE_CC(WAZANO_AKUMANOKISSU                 );
  IS_COMPATIBLE_CC(WAZANO_GODDOBAADO                   );
  IS_COMPATIBLE_CC(WAZANO_HENSIN                       );
  IS_COMPATIBLE_CC(WAZANO_AWA                          );
  IS_COMPATIBLE_CC(WAZANO_PIYOPIYOPANTI                );
  IS_COMPATIBLE_CC(WAZANO_KINOKONOHOUSI                );
  IS_COMPATIBLE_CC(WAZANO_HURASSYU                     );
  IS_COMPATIBLE_CC(WAZANO_SAIKOWHEEBU                  );
  IS_COMPATIBLE_CC(WAZANO_HANERU                       );
  IS_COMPATIBLE_CC(WAZANO_TOKERU                       );
  IS_COMPATIBLE_CC(WAZANO_KURABUHANMAA                 );
  IS_COMPATIBLE_CC(WAZANO_DAIBAKUHATU                  );
  IS_COMPATIBLE_CC(WAZANO_MIDAREHIKKAKI                );
  IS_COMPATIBLE_CC(WAZANO_HONEBUUMERAN                 );
  IS_COMPATIBLE_CC(WAZANO_NEMURU                       );
  IS_COMPATIBLE_CC(WAZANO_IWANADARE                    );
  IS_COMPATIBLE_CC(WAZANO_HISSATUMAEBA                 );
  IS_COMPATIBLE_CC(WAZANO_KAKUBARU                     );
  IS_COMPATIBLE_CC(WAZANO_TEKUSUTYAA                   );
  IS_COMPATIBLE_CC(WAZANO_TORAIATAKKU                  );
  IS_COMPATIBLE_CC(WAZANO_IKARINOMAEBA                 );
  IS_COMPATIBLE_CC(WAZANO_KIRISAKU                     );
  IS_COMPATIBLE_CC(WAZANO_MIGAWARI                     );
  IS_COMPATIBLE_CC(WAZANO_WARUAGAKI                    );
  IS_COMPATIBLE_CC(WAZANO_SUKETTI                      );
  IS_COMPATIBLE_CC(WAZANO_TORIPURUKIKKU                );
  IS_COMPATIBLE_CC(WAZANO_DOROBOU                      );
  IS_COMPATIBLE_CC(WAZANO_KUMONOSU                     );
  IS_COMPATIBLE_CC(WAZANO_KOKORONOME                   );
  IS_COMPATIBLE_CC(WAZANO_AKUMU                        );
  IS_COMPATIBLE_CC(WAZANO_KAENGURUMA                   );
  IS_COMPATIBLE_CC(WAZANO_IBIKI                        );
  IS_COMPATIBLE_CC(WAZANO_NOROI                        );
  IS_COMPATIBLE_CC(WAZANO_ZITABATA                     );
  IS_COMPATIBLE_CC(WAZANO_TEKUSUTYAA2                  );
  IS_COMPATIBLE_CC(WAZANO_EAROBURASUTO                 );
  IS_COMPATIBLE_CC(WAZANO_WATAHOUSI                    );
  IS_COMPATIBLE_CC(WAZANO_KISIKAISEI                   );
  IS_COMPATIBLE_CC(WAZANO_URAMI                        );
  IS_COMPATIBLE_CC(WAZANO_KONAYUKI                     );
  IS_COMPATIBLE_CC(WAZANO_MAMORU                       );
  IS_COMPATIBLE_CC(WAZANO_MAHHAPANTI                   );
  IS_COMPATIBLE_CC(WAZANO_KOWAIKAO                     );
  IS_COMPATIBLE_CC(WAZANO_DAMASIUTI                    );
  IS_COMPATIBLE_CC(WAZANO_TENSINOKISSU                 );
  IS_COMPATIBLE_CC(WAZANO_HARADAIKO                    );
  IS_COMPATIBLE_CC(WAZANO_HEDOROBAKUDAN                );
  IS_COMPATIBLE_CC(WAZANO_DOROKAKE                     );
  IS_COMPATIBLE_CC(WAZANO_OKUTANHOU                    );
  IS_COMPATIBLE_CC(WAZANO_MAKIBISI                     );
  IS_COMPATIBLE_CC(WAZANO_DENZIHOU                     );
  IS_COMPATIBLE_CC(WAZANO_MIYABURU                     );
  IS_COMPATIBLE_CC(WAZANO_MITIDURE                     );
  IS_COMPATIBLE_CC(WAZANO_HOROBINOUTA                  );
  IS_COMPATIBLE_CC(WAZANO_KOGOERUKAZE                  );
  IS_COMPATIBLE_CC(WAZANO_MIKIRI                       );
  IS_COMPATIBLE_CC(WAZANO_BOONRASSYU                   );
  IS_COMPATIBLE_CC(WAZANO_ROKKUON                      );
  IS_COMPATIBLE_CC(WAZANO_GEKIRIN                      );
  IS_COMPATIBLE_CC(WAZANO_SUNAARASI                    );
  IS_COMPATIBLE_CC(WAZANO_GIGADOREIN                   );
  IS_COMPATIBLE_CC(WAZANO_KORAERU                      );
  IS_COMPATIBLE_CC(WAZANO_AMAERU                       );
  IS_COMPATIBLE_CC(WAZANO_KOROGARU                     );
  IS_COMPATIBLE_CC(WAZANO_MINEUTI                      );
  IS_COMPATIBLE_CC(WAZANO_IBARU                        );
  IS_COMPATIBLE_CC(WAZANO_MIRUKUNOMI                   );
  IS_COMPATIBLE_CC(WAZANO_SUPAAKU                      );
  IS_COMPATIBLE_CC(WAZANO_RENZOKUGIRI                  );
  IS_COMPATIBLE_CC(WAZANO_HAGANENOTUBASA               );
  IS_COMPATIBLE_CC(WAZANO_KUROIMANAZASI                );
  IS_COMPATIBLE_CC(WAZANO_MEROMERO                     );
  IS_COMPATIBLE_CC(WAZANO_NEGOTO                       );
  IS_COMPATIBLE_CC(WAZANO_IYASINOSUZU                  );
  IS_COMPATIBLE_CC(WAZANO_ONGAESI                      );
  IS_COMPATIBLE_CC(WAZANO_PUREZENTO                    );
  IS_COMPATIBLE_CC(WAZANO_YATUATARI                    );
  IS_COMPATIBLE_CC(WAZANO_SINPINOMAMORI                );
  IS_COMPATIBLE_CC(WAZANO_ITAMIWAKE                    );
  IS_COMPATIBLE_CC(WAZANO_SEINARUHONOO                 );
  IS_COMPATIBLE_CC(WAZANO_MAGUNITYUUDO                 );
  IS_COMPATIBLE_CC(WAZANO_BAKURETUPANTI                );
  IS_COMPATIBLE_CC(WAZANO_MEGAHOON                     );
  IS_COMPATIBLE_CC(WAZANO_RYUUNOIBUKI                  );
  IS_COMPATIBLE_CC(WAZANO_BATONTATTI                   );
  IS_COMPATIBLE_CC(WAZANO_ANKOORU                      );
  IS_COMPATIBLE_CC(WAZANO_OIUTI                        );
  IS_COMPATIBLE_CC(WAZANO_KOUSOKUSUPIN                 );
  IS_COMPATIBLE_CC(WAZANO_AMAIKAORI                    );
  IS_COMPATIBLE_CC(WAZANO_AIANTEERU                    );
  IS_COMPATIBLE_CC(WAZANO_METARUKUROO                  );
  IS_COMPATIBLE_CC(WAZANO_ATEMINAGE                    );
  IS_COMPATIBLE_CC(WAZANO_ASANOHIZASI                  );
  IS_COMPATIBLE_CC(WAZANO_KOUGOUSEI                    );
  IS_COMPATIBLE_CC(WAZANO_TUKINOHIKARI                 );
  IS_COMPATIBLE_CC(WAZANO_MEZAMERUPAWAA                );
  IS_COMPATIBLE_CC(WAZANO_KUROSUTYOPPU                 );
  IS_COMPATIBLE_CC(WAZANO_TATUMAKI                     );
  IS_COMPATIBLE_CC(WAZANO_AMAGOI                       );
  IS_COMPATIBLE_CC(WAZANO_NIHONBARE                    );
  IS_COMPATIBLE_CC(WAZANO_KAMIKUDAKU                   );
  IS_COMPATIBLE_CC(WAZANO_MIRAAKOOTO                   );
  IS_COMPATIBLE_CC(WAZANO_ZIKOANZI                     );
  IS_COMPATIBLE_CC(WAZANO_SINSOKU                      );
  IS_COMPATIBLE_CC(WAZANO_GENSINOTIKARA                );
  IS_COMPATIBLE_CC(WAZANO_SYADOOBOORU                  );
  IS_COMPATIBLE_CC(WAZANO_MIRAIYOTI                    );
  IS_COMPATIBLE_CC(WAZANO_IWAKUDAKI                    );
  IS_COMPATIBLE_CC(WAZANO_UZUSIO                       );
  IS_COMPATIBLE_CC(WAZANO_HUKURODATAKI                 );
  IS_COMPATIBLE_CC(WAZANO_NEKODAMASI                   );
  IS_COMPATIBLE_CC(WAZANO_SAWAGU                       );
  IS_COMPATIBLE_CC(WAZANO_TAKUWAERU                    );
  IS_COMPATIBLE_CC(WAZANO_HAKIDASU                     );
  IS_COMPATIBLE_CC(WAZANO_NOMIKOMU                     );
  IS_COMPATIBLE_CC(WAZANO_NEPPUU                       );
  IS_COMPATIBLE_CC(WAZANO_ARARE                        );
  IS_COMPATIBLE_CC(WAZANO_ITYAMON                      );
  IS_COMPATIBLE_CC(WAZANO_ODATERU                      );
  IS_COMPATIBLE_CC(WAZANO_ONIBI                        );
  IS_COMPATIBLE_CC(WAZANO_OKIMIYAGE                    );
  IS_COMPATIBLE_CC(WAZANO_KARAGENKI                    );
  IS_COMPATIBLE_CC(WAZANO_KIAIPANTI                    );
  IS_COMPATIBLE_CC(WAZANO_KITUKE                       );
  IS_COMPATIBLE_CC(WAZANO_KONOYUBITOMARE               );
  IS_COMPATIBLE_CC(WAZANO_SIZENNOTIKARA                );
  IS_COMPATIBLE_CC(WAZANO_ZYUUDEN                      );
  IS_COMPATIBLE_CC(WAZANO_TYOUHATU                     );
  IS_COMPATIBLE_CC(WAZANO_TEDASUKE                     );
  IS_COMPATIBLE_CC(WAZANO_TORIKKU                      );
  IS_COMPATIBLE_CC(WAZANO_NARIKIRI                     );
  IS_COMPATIBLE_CC(WAZANO_NEGAIGOTO                    );
  IS_COMPATIBLE_CC(WAZANO_NEKONOTE                     );
  IS_COMPATIBLE_CC(WAZANO_NEWOHARU                     );
  IS_COMPATIBLE_CC(WAZANO_BAKADIKARA                   );
  IS_COMPATIBLE_CC(WAZANO_MAZIKKUKOOTO                 );
  IS_COMPATIBLE_CC(WAZANO_RISAIKURU                    );
  IS_COMPATIBLE_CC(WAZANO_RIBENZI                      );
  IS_COMPATIBLE_CC(WAZANO_KAWARAWARI                   );
  IS_COMPATIBLE_CC(WAZANO_AKUBI                        );
  IS_COMPATIBLE_CC(WAZANO_HATAKIOTOSU                  );
  IS_COMPATIBLE_CC(WAZANO_GAMUSYARA                    );
  IS_COMPATIBLE_CC(WAZANO_HUNKA                        );
  IS_COMPATIBLE_CC(WAZANO_SUKIRUSUWAPPU                );
  IS_COMPATIBLE_CC(WAZANO_HUUIN                        );
  IS_COMPATIBLE_CC(WAZANO_RIHURESSYU                   );
  IS_COMPATIBLE_CC(WAZANO_ONNEN                        );
  IS_COMPATIBLE_CC(WAZANO_YOKODORI                     );
  IS_COMPATIBLE_CC(WAZANO_HIMITUNOTIKARA               );
  IS_COMPATIBLE_CC(WAZANO_DAIBINGU                     );
  IS_COMPATIBLE_CC(WAZANO_TUPPARI                      );
  IS_COMPATIBLE_CC(WAZANO_HOGOSYOKU                    );
  IS_COMPATIBLE_CC(WAZANO_HOTARUBI                     );
  IS_COMPATIBLE_CC(WAZANO_RASUTAAPAAZI                 );
  IS_COMPATIBLE_CC(WAZANO_MISUTOBOORU                  );
  IS_COMPATIBLE_CC(WAZANO_FEZAADANSU                   );
  IS_COMPATIBLE_CC(WAZANO_HURAHURADANSU                );
  IS_COMPATIBLE_CC(WAZANO_BUREIZUKIKKU                 );
  IS_COMPATIBLE_CC(WAZANO_DOROASOBI                    );
  IS_COMPATIBLE_CC(WAZANO_AISUBOORU                    );
  IS_COMPATIBLE_CC(WAZANO_NIIDORUAAMU                  );
  IS_COMPATIBLE_CC(WAZANO_NAMAKERU                     );
  IS_COMPATIBLE_CC(WAZANO_HAIPAABOISU                  );
  IS_COMPATIBLE_CC(WAZANO_DOKUDOKUNOKIBA               );
  IS_COMPATIBLE_CC(WAZANO_BUREIKUKUROO                 );
  IS_COMPATIBLE_CC(WAZANO_BURASUTOBAAN                 );
  IS_COMPATIBLE_CC(WAZANO_HAIDOROKANON                 );
  IS_COMPATIBLE_CC(WAZANO_KOMETTOPANTI                 );
  IS_COMPATIBLE_CC(WAZANO_ODOROKASU                    );
  IS_COMPATIBLE_CC(WAZANO_WHEZAABOORU                  );
  IS_COMPATIBLE_CC(WAZANO_AROMASERAPII                 );
  IS_COMPATIBLE_CC(WAZANO_USONAKI                      );
  IS_COMPATIBLE_CC(WAZANO_EAKATTAA                     );
  IS_COMPATIBLE_CC(WAZANO_OOBAAHIITO                   );
  IS_COMPATIBLE_CC(WAZANO_KAGIWAKERU                   );
  IS_COMPATIBLE_CC(WAZANO_GANSEKIHUUZI                 );
  IS_COMPATIBLE_CC(WAZANO_GINIRONOKAZE                 );
  IS_COMPATIBLE_CC(WAZANO_KINZOKUON                    );
  IS_COMPATIBLE_CC(WAZANO_KUSABUE                      );
  IS_COMPATIBLE_CC(WAZANO_KUSUGURU                     );
  IS_COMPATIBLE_CC(WAZANO_KOSUMOPAWAA                  );
  IS_COMPATIBLE_CC(WAZANO_SIOHUKI                      );
  IS_COMPATIBLE_CC(WAZANO_SIGUNARUBIIMU                );
  IS_COMPATIBLE_CC(WAZANO_SYADOOPANTI                  );
  IS_COMPATIBLE_CC(WAZANO_ZINTUURIKI                   );
  IS_COMPATIBLE_CC(WAZANO_SUKAIAPPAA                   );
  IS_COMPATIBLE_CC(WAZANO_SUNAZIGOKU                   );
  IS_COMPATIBLE_CC(WAZANO_ZETTAIREIDO                  );
  IS_COMPATIBLE_CC(WAZANO_DAKURYUU                     );
  IS_COMPATIBLE_CC(WAZANO_TANEMASINGAN                 );
  IS_COMPATIBLE_CC(WAZANO_TUBAMEGAESI                  );
  IS_COMPATIBLE_CC(WAZANO_TURARABARI                   );
  IS_COMPATIBLE_CC(WAZANO_TEPPEKI                      );
  IS_COMPATIBLE_CC(WAZANO_TOOSENBOU                    );
  IS_COMPATIBLE_CC(WAZANO_TOOBOE                       );
  IS_COMPATIBLE_CC(WAZANO_DORAGONKUROO                 );
  IS_COMPATIBLE_CC(WAZANO_HAADOPURANTO                 );
  IS_COMPATIBLE_CC(WAZANO_BIRUDOAPPU                   );
  IS_COMPATIBLE_CC(WAZANO_TOBIHANERU                   );
  IS_COMPATIBLE_CC(WAZANO_MADDOSYOTTO                  );
  IS_COMPATIBLE_CC(WAZANO_POIZUNTEERU                  );
  IS_COMPATIBLE_CC(WAZANO_HOSIGARU                     );
  IS_COMPATIBLE_CC(WAZANO_BORUTEKKAA                   );
  IS_COMPATIBLE_CC(WAZANO_MAZIKARURIIHU                );
  IS_COMPATIBLE_CC(WAZANO_MIZUASOBI                    );
  IS_COMPATIBLE_CC(WAZANO_MEISOU                       );
  IS_COMPATIBLE_CC(WAZANO_RIIHUBUREEDO                 );
  IS_COMPATIBLE_CC(WAZANO_RYUUNOMAI                    );
  IS_COMPATIBLE_CC(WAZANO_ROKKUBURASUTO                );
  IS_COMPATIBLE_CC(WAZANO_DENGEKIHA                    );
  IS_COMPATIBLE_CC(WAZANO_MIZUNOHADOU                  );
  IS_COMPATIBLE_CC(WAZANO_HAMETUNONEGAI                );
  IS_COMPATIBLE_CC(WAZANO_SAIKOBUUSUTO                 );
  IS_COMPATIBLE_CC(WAZANO_HANEYASUME                   );
  IS_COMPATIBLE_CC(WAZANO_ZYUURYOKU                    );
  IS_COMPATIBLE_CC(WAZANO_MIRAKURUAI                   );
  IS_COMPATIBLE_CC(WAZANO_MEZAMASIBINTA                );
  IS_COMPATIBLE_CC(WAZANO_AAMUHANMAA                   );
  IS_COMPATIBLE_CC(WAZANO_ZYAIROBOORU                  );
  IS_COMPATIBLE_CC(WAZANO_IYASINONEGAI                 );
  IS_COMPATIBLE_CC(WAZANO_SIOMIZU                      );
  IS_COMPATIBLE_CC(WAZANO_SIZENNOMEGUMI                );
  IS_COMPATIBLE_CC(WAZANO_FEINTO                       );
  IS_COMPATIBLE_CC(WAZANO_TUIBAMU                      );
  IS_COMPATIBLE_CC(WAZANO_OIKAZE                       );
  IS_COMPATIBLE_CC(WAZANO_TUBOWOTUKU                   );
  IS_COMPATIBLE_CC(WAZANO_METARUBAASUTO                );
  IS_COMPATIBLE_CC(WAZANO_TONBOGAERI                   );
  IS_COMPATIBLE_CC(WAZANO_INFAITO                      );
  IS_COMPATIBLE_CC(WAZANO_SIPPEGAESI                   );
  IS_COMPATIBLE_CC(WAZANO_DAMEOSI                      );
  IS_COMPATIBLE_CC(WAZANO_SASIOSAE                     );
  IS_COMPATIBLE_CC(WAZANO_NAGETUKERU                   );
  IS_COMPATIBLE_CC(WAZANO_SAIKOSIHUTO                  );
  IS_COMPATIBLE_CC(WAZANO_KIRIHUDA                     );
  IS_COMPATIBLE_CC(WAZANO_KAIHUKUHUUZI                 );
  IS_COMPATIBLE_CC(WAZANO_SIBORITORU                   );
  IS_COMPATIBLE_CC(WAZANO_PAWAATORIKKU                 );
  IS_COMPATIBLE_CC(WAZANO_IEKI                         );
  IS_COMPATIBLE_CC(WAZANO_OMAZINAI                     );
  IS_COMPATIBLE_CC(WAZANO_SAKIDORI                     );
  IS_COMPATIBLE_CC(WAZANO_MANEKKO                      );
  IS_COMPATIBLE_CC(WAZANO_PAWAASUWAPPU                 );
  IS_COMPATIBLE_CC(WAZANO_GAADOSUWAPPU                 );
  IS_COMPATIBLE_CC(WAZANO_OSIOKI                       );
  IS_COMPATIBLE_CC(WAZANO_TOTTEOKI                     );
  IS_COMPATIBLE_CC(WAZANO_NAYAMINOTANE                 );
  IS_COMPATIBLE_CC(WAZANO_HUIUTI                       );
  IS_COMPATIBLE_CC(WAZANO_DOKUBISI                     );
  IS_COMPATIBLE_CC(WAZANO_HAATOSUWAPPU                 );
  IS_COMPATIBLE_CC(WAZANO_AKUARINGU                    );
  IS_COMPATIBLE_CC(WAZANO_DENZIHUYUU                   );
  IS_COMPATIBLE_CC(WAZANO_HUREADORAIBU                 );
  IS_COMPATIBLE_CC(WAZANO_HAKKEI                       );
  IS_COMPATIBLE_CC(WAZANO_HADOUDAN                     );
  IS_COMPATIBLE_CC(WAZANO_ROKKUKATTO                   );
  IS_COMPATIBLE_CC(WAZANO_DOKUDUKI                     );
  IS_COMPATIBLE_CC(WAZANO_AKUNOHADOU                   );
  IS_COMPATIBLE_CC(WAZANO_TUZIGIRI                     );
  IS_COMPATIBLE_CC(WAZANO_AKUATEERU                    );
  IS_COMPATIBLE_CC(WAZANO_TANEBAKUDAN                  );
  IS_COMPATIBLE_CC(WAZANO_EASURASSYU                   );
  IS_COMPATIBLE_CC(WAZANO_SIZAAKUROSU                  );
  IS_COMPATIBLE_CC(WAZANO_MUSINOSAZAMEKI               );
  IS_COMPATIBLE_CC(WAZANO_RYUUNOHADOU                  );
  IS_COMPATIBLE_CC(WAZANO_DORAGONDAIBU                 );
  IS_COMPATIBLE_CC(WAZANO_PAWAAJEMU                    );
  IS_COMPATIBLE_CC(WAZANO_DOREINPANTI                  );
  IS_COMPATIBLE_CC(WAZANO_SINKUUHA                     );
  IS_COMPATIBLE_CC(WAZANO_KIAIDAMA                     );
  IS_COMPATIBLE_CC(WAZANO_ENAZIIBOORU                  );
  IS_COMPATIBLE_CC(WAZANO_BUREIBUBAADO                 );
  IS_COMPATIBLE_CC(WAZANO_DAITINOTIKARA                );
  IS_COMPATIBLE_CC(WAZANO_SURIKAE                      );
  IS_COMPATIBLE_CC(WAZANO_GIGAINPAKUTO                 );
  IS_COMPATIBLE_CC(WAZANO_WARUDAKUMI                   );
  IS_COMPATIBLE_CC(WAZANO_BARETTOPANTI                 );
  IS_COMPATIBLE_CC(WAZANO_YUKINADARE                   );
  IS_COMPATIBLE_CC(WAZANO_KOORINOTUBUTE                );
  IS_COMPATIBLE_CC(WAZANO_SYADOOKUROO                  );
  IS_COMPATIBLE_CC(WAZANO_KAMINARINOKIBA               );
  IS_COMPATIBLE_CC(WAZANO_KOORINOKIBA                  );
  IS_COMPATIBLE_CC(WAZANO_HONOONOKIBA                  );
  IS_COMPATIBLE_CC(WAZANO_KAGEUTI                      );
  IS_COMPATIBLE_CC(WAZANO_DOROBAKUDAN                  );
  IS_COMPATIBLE_CC(WAZANO_SAIKOKATTAA                  );
  IS_COMPATIBLE_CC(WAZANO_SINENNOZUTUKI                );
  IS_COMPATIBLE_CC(WAZANO_MIRAASYOTTO                  );
  IS_COMPATIBLE_CC(WAZANO_RASUTAAKANON                 );
  IS_COMPATIBLE_CC(WAZANO_ROKKUKURAIMU                 );
  IS_COMPATIBLE_CC(WAZANO_KIRIBARAI                    );
  IS_COMPATIBLE_CC(WAZANO_TORIKKURUUMU                 );
  IS_COMPATIBLE_CC(WAZANO_RYUUSEIGUN                   );
  IS_COMPATIBLE_CC(WAZANO_HOUDEN                       );
  IS_COMPATIBLE_CC(WAZANO_HUNEN                        );
  IS_COMPATIBLE_CC(WAZANO_RIIHUSUTOOMU                 );
  IS_COMPATIBLE_CC(WAZANO_PAWAAWHIPPU                  );
  IS_COMPATIBLE_CC(WAZANO_GANSEKIHOU                   );
  IS_COMPATIBLE_CC(WAZANO_KUROSUPOIZUN                 );
  IS_COMPATIBLE_CC(WAZANO_DASUTOSYUUTO                 );
  IS_COMPATIBLE_CC(WAZANO_AIANHEDDO                    );
  IS_COMPATIBLE_CC(WAZANO_MAGUNETTOBOMU                );
  IS_COMPATIBLE_CC(WAZANO_SUTOONEZZI                   );
  IS_COMPATIBLE_CC(WAZANO_YUUWAKU                      );
  IS_COMPATIBLE_CC(WAZANO_SUTERUSUROKKU                );
  IS_COMPATIBLE_CC(WAZANO_KUSAMUSUBI                   );
  IS_COMPATIBLE_CC(WAZANO_OSYABERI                     );
  IS_COMPATIBLE_CC(WAZANO_SABAKINOTUBUTE               );
  IS_COMPATIBLE_CC(WAZANO_MUSIKUI                      );
  IS_COMPATIBLE_CC(WAZANO_TYAAZIBIIMU                  );
  IS_COMPATIBLE_CC(WAZANO_UDDOHANMAA                   );
  IS_COMPATIBLE_CC(WAZANO_AKUAJETTO                    );
  IS_COMPATIBLE_CC(WAZANO_KOUGEKISIREI                 );
  IS_COMPATIBLE_CC(WAZANO_BOUGYOSIREI                  );
  IS_COMPATIBLE_CC(WAZANO_KAIHUKUSIREI                 );
  IS_COMPATIBLE_CC(WAZANO_MOROHANOZUTUKI               );
  IS_COMPATIBLE_CC(WAZANO_DABURUATAKKU                 );
  IS_COMPATIBLE_CC(WAZANO_TOKINOHOUKOU                 );
  IS_COMPATIBLE_CC(WAZANO_AKUUSETUDAN                  );
  IS_COMPATIBLE_CC(WAZANO_MIKADUKINOMAI                );
  IS_COMPATIBLE_CC(WAZANO_NIGIRITUBUSU                 );
  IS_COMPATIBLE_CC(WAZANO_MAGUMASUTOOMU                );
  IS_COMPATIBLE_CC(WAZANO_DAAKUHOORU                   );
  IS_COMPATIBLE_CC(WAZANO_SIIDOHUREA                   );
  IS_COMPATIBLE_CC(WAZANO_AYASIIKAZE                   );
  IS_COMPATIBLE_CC(WAZANO_SYADOODAIBU                  );
  IS_COMPATIBLE_CC(WAZANO_TUMETOGI                     );
  IS_COMPATIBLE_CC(WAZANO_WAIDOGAADO                   );
  IS_COMPATIBLE_CC(WAZANO_GAADOSHEA                    );
  IS_COMPATIBLE_CC(WAZANO_PAWAASHEA                    );
  IS_COMPATIBLE_CC(WAZANO_WANDAARUUMU                  );
  IS_COMPATIBLE_CC(WAZANO_SAIKOSYOKKU                  );
  IS_COMPATIBLE_CC(WAZANO_BENOMUSYOKKU                 );
  IS_COMPATIBLE_CC(WAZANO_BODHIPAAZI                   );
  IS_COMPATIBLE_CC(WAZANO_IKARINOKONA                  );
  IS_COMPATIBLE_CC(WAZANO_TEREKINESISU                 );
  IS_COMPATIBLE_CC(WAZANO_MAZIKKURUUMU                 );
  IS_COMPATIBLE_CC(WAZANO_UTIOTOSU                     );
  IS_COMPATIBLE_CC(WAZANO_YAMAARASI                    );
  IS_COMPATIBLE_CC(WAZANO_HAZIKERUHONOO                );
  IS_COMPATIBLE_CC(WAZANO_HEDOROWHEEBU                 );
  IS_COMPATIBLE_CC(WAZANO_TYOUNOMAI                    );
  IS_COMPATIBLE_CC(WAZANO_HEBIIBONBAA                  );
  IS_COMPATIBLE_CC(WAZANO_SINKURONOIZU                 );
  IS_COMPATIBLE_CC(WAZANO_EREKIBOORU                   );
  IS_COMPATIBLE_CC(WAZANO_MIZUBITASI                   );
  IS_COMPATIBLE_CC(WAZANO_NITOROTYAAZI                 );
  IS_COMPATIBLE_CC(WAZANO_TOGUROWOMAKU                 );
  IS_COMPATIBLE_CC(WAZANO_ROOKIKKU                     );
  IS_COMPATIBLE_CC(WAZANO_ASIDDOBOMU                   );
  IS_COMPATIBLE_CC(WAZANO_IKASAMA                      );
  IS_COMPATIBLE_CC(WAZANO_SINPURUBIIMU                 );
  IS_COMPATIBLE_CC(WAZANO_NAKAMADUKURI                 );
  IS_COMPATIBLE_CC(WAZANO_OSAKINIDOUZO                 );
  IS_COMPATIBLE_CC(WAZANO_RINSYOU                      );
  IS_COMPATIBLE_CC(WAZANO_EKOOBOISU                    );
  IS_COMPATIBLE_CC(WAZANO_NASIKUZUSI                   );
  IS_COMPATIBLE_CC(WAZANO_KURIASUMOGGU                 );
  IS_COMPATIBLE_CC(WAZANO_ASISUTOPAWAA                 );
  IS_COMPATIBLE_CC(WAZANO_FASUTOGAADO                  );
  IS_COMPATIBLE_CC(WAZANO_SAIDOTHENZI                  );
  IS_COMPATIBLE_CC(WAZANO_NETTOU                       );
  IS_COMPATIBLE_CC(WAZANO_KARAWOYABURU                 );
  IS_COMPATIBLE_CC(WAZANO_IYASINOHADOU                 );
  IS_COMPATIBLE_CC(WAZANO_TATARIME                     );
  IS_COMPATIBLE_CC(WAZANO_HURIIFOORU                   );
  IS_COMPATIBLE_CC(WAZANO_GIATHENZI                    );
  IS_COMPATIBLE_CC(WAZANO_TOMOENAGE                    );
  IS_COMPATIBLE_CC(WAZANO_YAKITUKUSU                   );
  IS_COMPATIBLE_CC(WAZANO_SAKIOKURI                    );
  IS_COMPATIBLE_CC(WAZANO_AKUROBATTO                   );
  IS_COMPATIBLE_CC(WAZANO_MIRAATAIPU                   );
  IS_COMPATIBLE_CC(WAZANO_KATAKIUTI                    );
  IS_COMPATIBLE_CC(WAZANO_INOTIGAKE                    );
  IS_COMPATIBLE_CC(WAZANO_GIHUTOPASU                   );
  IS_COMPATIBLE_CC(WAZANO_RENGOKU                      );
  IS_COMPATIBLE_CC(WAZANO_MIZUNOTIKAI                  );
  IS_COMPATIBLE_CC(WAZANO_HONOONOTIKAI                 );
  IS_COMPATIBLE_CC(WAZANO_KUSANOTIKAI                  );
  IS_COMPATIBLE_CC(WAZANO_BORUTOTHENZI                 );
  IS_COMPATIBLE_CC(WAZANO_MUSINOTEIKOU                 );
  IS_COMPATIBLE_CC(WAZANO_ZINARASI                     );
  IS_COMPATIBLE_CC(WAZANO_KOORINOIBUKI                 );
  IS_COMPATIBLE_CC(WAZANO_DORAGONTEERU                 );
  IS_COMPATIBLE_CC(WAZANO_HURUITATERU                  );
  IS_COMPATIBLE_CC(WAZANO_EREKINETTO                   );
  IS_COMPATIBLE_CC(WAZANO_WAIRUDOBORUTO                );
  IS_COMPATIBLE_CC(WAZANO_DORIRURAINAA                 );
  IS_COMPATIBLE_CC(WAZANO_DABURUTYOPPU                 );
  IS_COMPATIBLE_CC(WAZANO_HAATOSUTANPU                 );
  IS_COMPATIBLE_CC(WAZANO_UDDOHOON                     );
  IS_COMPATIBLE_CC(WAZANO_SEINARUTURUGI                );
  IS_COMPATIBLE_CC(WAZANO_SHERUBUREEDO                 );
  IS_COMPATIBLE_CC(WAZANO_HIITOSUTANPU                 );
  IS_COMPATIBLE_CC(WAZANO_GURASUMIKISAA                );
  IS_COMPATIBLE_CC(WAZANO_HAADOROORAA                  );
  IS_COMPATIBLE_CC(WAZANO_KOTTONGAADO                  );
  IS_COMPATIBLE_CC(WAZANO_NAITOBAASUTO                 );
  IS_COMPATIBLE_CC(WAZANO_SAIKOBUREIKU                 );
  IS_COMPATIBLE_CC(WAZANO_SUIIPUBINTA                  );
  IS_COMPATIBLE_CC(WAZANO_BOUHUU                       );
  IS_COMPATIBLE_CC(WAZANO_AHUROBUREIKU                 );
  IS_COMPATIBLE_CC(WAZANO_GIASOOSAA                    );
  IS_COMPATIBLE_CC(WAZANO_KAENDAN                      );
  IS_COMPATIBLE_CC(WAZANO_TEKUNOBASUTAA                );
  IS_COMPATIBLE_CC(WAZANO_INISIENOUTA                  );
  IS_COMPATIBLE_CC(WAZANO_SINPINOTURUGI                );
  IS_COMPATIBLE_CC(WAZANO_KOGOERUSEKAI                 );
  IS_COMPATIBLE_CC(WAZANO_RAIGEKI                      );
  IS_COMPATIBLE_CC(WAZANO_AOIHONOO                     );
  IS_COMPATIBLE_CC(WAZANO_HONOONOMAI                   );
  IS_COMPATIBLE_CC(WAZANO_HURIIZUBORUTO                );
  IS_COMPATIBLE_CC(WAZANO_KOORUDOHUREA                 );
  IS_COMPATIBLE_CC(WAZANO_BAAKUAUTO                    );
  IS_COMPATIBLE_CC(WAZANO_TURARAOTOSI                  );
  IS_COMPATIBLE_CC(WAZANO_vJENEREETO                   );
  IS_COMPATIBLE_CC(WAZANO_KUROSUHUREIMU                );
  IS_COMPATIBLE_CC(WAZANO_KUROSUSANDAA                 );
  IS_COMPATIBLE_CC(WAZANO_HURAINGUPURESU               );
  IS_COMPATIBLE_CC(WAZANO_TATAMIGAESI                  );
  IS_COMPATIBLE_CC(WAZANO_GEPPU                        );
  IS_COMPATIBLE_CC(WAZANO_TAGAYASU                     );
  IS_COMPATIBLE_CC(WAZANO_NEBANEBANETTO                );
  IS_COMPATIBLE_CC(WAZANO_TODOMEBARI                   );
  IS_COMPATIBLE_CC(WAZANO_GOOSUTODAIBU                 );
  IS_COMPATIBLE_CC(WAZANO_HAROWHIN                     );
  IS_COMPATIBLE_CC(WAZANO_OTAKEBI                      );
  IS_COMPATIBLE_CC(WAZANO_PURAZUMASYAWAA               );
  IS_COMPATIBLE_CC(WAZANO_PARABORATYAAZI               );
  IS_COMPATIBLE_CC(WAZANO_MORINONOROI                  );
  IS_COMPATIBLE_CC(WAZANO_HANAHUBUKI                   );
  IS_COMPATIBLE_CC(WAZANO_HURIIZUDORAI                 );
  IS_COMPATIBLE_CC(WAZANO_TYAAMUBOISU                  );
  IS_COMPATIBLE_CC(WAZANO_SUTEZERIHU                   );
  IS_COMPATIBLE_CC(WAZANO_HIKKURIKAESU                 );
  IS_COMPATIBLE_CC(WAZANO_DOREINKISSU                  );
  IS_COMPATIBLE_CC(WAZANO_TORIKKUGAADO                 );
  IS_COMPATIBLE_CC(WAZANO_HURAWAAGAADO                 );
  IS_COMPATIBLE_CC(WAZANO_GURASUFIIRUDO                );
  IS_COMPATIBLE_CC(WAZANO_MISUTOFIIRUDO                );
  IS_COMPATIBLE_CC(WAZANO_SOUDEN                       );
  IS_COMPATIBLE_CC(WAZANO_ZYARETUKU                    );
  IS_COMPATIBLE_CC(WAZANO_YOUSEINOKAZE                 );
  IS_COMPATIBLE_CC(WAZANO_MUUNFOOSU                    );
  IS_COMPATIBLE_CC(WAZANO_BAKUONPA                     );
  IS_COMPATIBLE_CC(WAZANO_FEARIIROKKU                  );
  IS_COMPATIBLE_CC(WAZANO_KINGUSIIRUDO                 );
  IS_COMPATIBLE_CC(WAZANO_NAKAYOKUSURU                 );
  IS_COMPATIBLE_CC(WAZANO_NAISYOBANASI                 );
  IS_COMPATIBLE_CC(WAZANO_DAIYASUTOOMU                 );
  IS_COMPATIBLE_CC(WAZANO_SUTIIMUBAASUTO               );
  IS_COMPATIBLE_CC(WAZANO_IZIGENHOORU                  );
  IS_COMPATIBLE_CC(WAZANO_MIZUSYURIKEN                 );
  IS_COMPATIBLE_CC(WAZANO_MAZIKARUHUREIMU              );
  IS_COMPATIBLE_CC(WAZANO_NIIDORUGAADO                 );
  IS_COMPATIBLE_CC(WAZANO_AROMAMISUTO                  );
  IS_COMPATIBLE_CC(WAZANO_KAIDENPA                     );
  IS_COMPATIBLE_CC(WAZANO_BENOMUTORAPPU                );
  IS_COMPATIBLE_CC(WAZANO_HUNZIN                       );
  IS_COMPATIBLE_CC(WAZANO_ZIOKONTOROORU                );
  IS_COMPATIBLE_CC(WAZANO_ZIBASOUSA                    );
  IS_COMPATIBLE_CC(WAZANO_HAPPIITAIMU                  );
  IS_COMPATIBLE_CC(WAZANO_EREKIFIIRUDO                 );
  IS_COMPATIBLE_CC(WAZANO_MAZIKARUSYAIN                );
  IS_COMPATIBLE_CC(WAZANO_OIWAI                        );
  IS_COMPATIBLE_CC(WAZANO_TEWOTUNAGU                   );
  IS_COMPATIBLE_CC(WAZANO_TUBURANAHITOMI               );
  IS_COMPATIBLE_CC(WAZANO_HOPPESURISURI                );
  IS_COMPATIBLE_CC(WAZANO_TEKAGEN                      );
  IS_COMPATIBLE_CC(WAZANO_MATOWARITUKU                 );
  IS_COMPATIBLE_CC(WAZANO_GUROUPANTI                   );
  IS_COMPATIBLE_CC(WAZANO_DESUUINGU                    );
  IS_COMPATIBLE_CC(WAZANO_SAUZANAROO                   );
  IS_COMPATIBLE_CC(WAZANO_SAUZANWHEEBU                 );
  IS_COMPATIBLE_CC(WAZANO_GURANDOFOOSU                 );
  IS_COMPATIBLE_CC(WAZANO_HAMETUNOHIKARI               );
  IS_COMPATIBLE_CC(WAZANO_KONGENNOHADOU                );
  IS_COMPATIBLE_CC(WAZANO_DANGAINOTURUGI               );
  IS_COMPATIBLE_CC(WAZANO_GARYOUTENSEI                 );
  IS_COMPATIBLE_CC(WAZANO_IZIGENRASSYU                 );
  IS_COMPATIBLE_CC(WAZANO_URUTORADASSYUATAKKU          );
  IS_COMPATIBLE_CC(WAZANO_NOOMARUZENRYOKU              );
  IS_COMPATIBLE_CC(WAZANO_ZENRYOKUMUSOUGEKIRETUKEN     );
  IS_COMPATIBLE_CC(WAZANO_KAKUTOUZENRYOKU              );
  IS_COMPATIBLE_CC(WAZANO_FAINARUDAIBUATAKKU           );
  IS_COMPATIBLE_CC(WAZANO_HIKOUZENRYOKU                );
  IS_COMPATIBLE_CC(WAZANO_ASIDDOPOIZUNDERIITO          );
  IS_COMPATIBLE_CC(WAZANO_DOKUZENRYOKU                 );
  IS_COMPATIBLE_CC(WAZANO_RAIZINGURANDOOOBAA           );
  IS_COMPATIBLE_CC(WAZANO_ZIMENZENRYOKU                );
  IS_COMPATIBLE_CC(WAZANO_WAARUZUENDOFOORU             );
  IS_COMPATIBLE_CC(WAZANO_IWAZENRYOKU                  );
  IS_COMPATIBLE_CC(WAZANO_ZETTAIHOSYOKUKAITENZAN       );
  IS_COMPATIBLE_CC(WAZANO_MUSIZENRYOKU                 );
  IS_COMPATIBLE_CC(WAZANO_MUGENANYAHENOIZANAI          );
  IS_COMPATIBLE_CC(WAZANO_GOOSUTOZENRYOKU              );
  IS_COMPATIBLE_CC(WAZANO_TYOUZETURASENRENGEKI         );
  IS_COMPATIBLE_CC(WAZANO_HAGANEZENRYOKU               );
  IS_COMPATIBLE_CC(WAZANO_DAINAMIKKUHURUHUREIMU        );
  IS_COMPATIBLE_CC(WAZANO_HONOOZENRYOKU                );
  IS_COMPATIBLE_CC(WAZANO_HAIPAAAKUATORUNEEDO          );
  IS_COMPATIBLE_CC(WAZANO_MIZUZENRYOKU                 );
  IS_COMPATIBLE_CC(WAZANO_BURUUMUSYAINEKUSUTORA        );
  IS_COMPATIBLE_CC(WAZANO_KUSAZENRYOKU                 );
  IS_COMPATIBLE_CC(WAZANO_SUPAAKINGUGIGABORUTO         );
  IS_COMPATIBLE_CC(WAZANO_DENKIZENRYOKU                );
  IS_COMPATIBLE_CC(WAZANO_MAKISIMAMUSAIBUREIKAA        );
  IS_COMPATIBLE_CC(WAZANO_ESUPAAZENRYOKU               );
  IS_COMPATIBLE_CC(WAZANO_REIZINGUZIOHURIIZU           );
  IS_COMPATIBLE_CC(WAZANO_KOORIZENRYOKU                );
  IS_COMPATIBLE_CC(WAZANO_ARUTHIMETTODORAGONBAAN       );
  IS_COMPATIBLE_CC(WAZANO_DORAGONZENRYOKU              );
  IS_COMPATIBLE_CC(WAZANO_BURAKKUHOORUIKURIPUSU        );
  IS_COMPATIBLE_CC(WAZANO_AKUZENRYOKU                  );
  IS_COMPATIBLE_CC(WAZANO_RABURIISUTAAINPAKUTO         );
  IS_COMPATIBLE_CC(WAZANO_FEARIIZENRYOKU               );
  IS_COMPATIBLE_CC(WAZANO_HISSATUNOPIKATYUUTO          );
  IS_COMPATIBLE_CC(WAZANO_SUNAATUME                    );
  IS_COMPATIBLE_CC(WAZANO_DEAIGASIRA                   );
  IS_COMPATIBLE_CC(WAZANO_TOOTIKA                      );
  IS_COMPATIBLE_CC(WAZANO_KAGENUI                      );
  IS_COMPATIBLE_CC(WAZANO_ddRARIATTO                   );
  IS_COMPATIBLE_CC(WAZANO_UTAKATANOARIA                );
  IS_COMPATIBLE_CC(WAZANO_AISUHANMAA                   );
  IS_COMPATIBLE_CC(WAZANO_HURAWAAHIIRU                 );
  IS_COMPATIBLE_CC(WAZANO_10MANBARIKI                  );
  IS_COMPATIBLE_CC(WAZANO_TIKARAWOSUITORU              );
  IS_COMPATIBLE_CC(WAZANO_SOORAABUREEDO                );
  IS_COMPATIBLE_CC(WAZANO_KONOHA                       );
  IS_COMPATIBLE_CC(WAZANO_SUPOTTORAITO                 );
  IS_COMPATIBLE_CC(WAZANO_DOKUNOITO                    );
  IS_COMPATIBLE_CC(WAZANO_TOGISUMASU                   );
  IS_COMPATIBLE_CC(WAZANO_ASISUTOGIA                   );
  IS_COMPATIBLE_CC(WAZANO_ZIGOKUDUKI                   );
  IS_COMPATIBLE_CC(WAZANO_KAHUNDANGO                   );
  IS_COMPATIBLE_CC(WAZANO_ANKAASYOTTO                  );
  IS_COMPATIBLE_CC(WAZANO_SAIKOFIIRUDO                 );
  IS_COMPATIBLE_CC(WAZANO_TOBIKAKARU                   );
  IS_COMPATIBLE_CC(WAZANO_HONOONOMUTI                  );
  IS_COMPATIBLE_CC(WAZANO_TUKEAGARU                    );
  IS_COMPATIBLE_CC(WAZANO_MOETUKIRU                    );
  IS_COMPATIBLE_CC(WAZANO_SUPIIDOSUWAPPU               );
  IS_COMPATIBLE_CC(WAZANO_SUMAATOHOON                  );
  IS_COMPATIBLE_CC(WAZANO_ZYOUKA                       );
  IS_COMPATIBLE_CC(WAZANO_MEZAMERUDANSU                );
  IS_COMPATIBLE_CC(WAZANO_KOAPANISSYAA                 );
  IS_COMPATIBLE_CC(WAZANO_TOROPIKARUKIKKU              );
  IS_COMPATIBLE_CC(WAZANO_SAIHAI                       );
  IS_COMPATIBLE_CC(WAZANO_KUTIBASIKYANON               );
  IS_COMPATIBLE_CC(WAZANO_SUKEIRUNOIZU                 );
  IS_COMPATIBLE_CC(WAZANO_DORAGONHANMAA                );
  IS_COMPATIBLE_CC(WAZANO_BUNMAWASU                    );
  IS_COMPATIBLE_CC(WAZANO_OORORABEERU                  );
  IS_COMPATIBLE_CC(WAZANO_SYADOUAROOZUSUTORAIKU        );
  IS_COMPATIBLE_CC(WAZANO_HAIPAADAAKUKURASSYAA         );
  IS_COMPATIBLE_CC(WAZANO_WADATUMINOSINFONIA           );
  IS_COMPATIBLE_CC(WAZANO_GAADHIANDEAROORA             );
  IS_COMPATIBLE_CC(WAZANO_HAISINSENYOUZENRYOKU         );
  IS_COMPATIBLE_CC(WAZANO_RAITONINGUSAAHURAIDO         );
  IS_COMPATIBLE_CC(WAZANO_HONKIWODASUKOUGEKI           );
  IS_COMPATIBLE_CC(WAZANO_SEBUNEBORUBUUSUTO            );
  IS_COMPATIBLE_CC(WAZANO_ORIZINZUSUUPAANOVHA          );
  IS_COMPATIBLE_CC(WAZANO_TORAPPUSHERU                 );
  IS_COMPATIBLE_CC(WAZANO_HURUURUKANON                 );
  IS_COMPATIBLE_CC(WAZANO_SAIKOFANGU                   );
  IS_COMPATIBLE_CC(WAZANO_ZIDANDA                      );
  IS_COMPATIBLE_CC(WAZANO_SYADOOBOON                   );
  IS_COMPATIBLE_CC(WAZANO_AKUSERUROKKU                 );
  IS_COMPATIBLE_CC(WAZANO_AKUABUREIKU                  );
  IS_COMPATIBLE_CC(WAZANO_PURIZUMUREEZAA               );
  IS_COMPATIBLE_CC(WAZANO_SYADOOSUTIIRU                );
  IS_COMPATIBLE_CC(WAZANO_METEODORAIBU                 );
  IS_COMPATIBLE_CC(WAZANO_SYADOOREI                    );
  IS_COMPATIBLE_CC(WAZANO_NAMIDAME                     );
  IS_COMPATIBLE_CC(WAZANO_BIRIBIRITIKUTIKU             );
  IS_COMPATIBLE_CC(WAZANO_SIZENNOIKARI                 );
  IS_COMPATIBLE_CC(WAZANO_MARUTIATAKKU                 );
  IS_COMPATIBLE_CC(WAZANO_SATOPIKASENYOUZENRYOKU       );
  IS_COMPATIBLE_CC(WAZANO_HANABISENYOU);
  IS_COMPATIBLE_CC(WAZANO_MABOROSIWAZA);
  IS_COMPATIBLE_CC(WAZANO_HIKARIDORAGON);
  IS_COMPATIBLE_CC(WAZANO_SINNEKUROZUMAZETTO);
  IS_COMPATIBLE_CC(WAZANO_ZENRYOKUMETEODORAIBU);
  IS_COMPATIBLE_CC(WAZANO_ZENRYOKUSYADOOREI);
  IS_COMPATIBLE_CC(WAZANO_ZENRYOKUZYARETUKU);
  IS_COMPATIBLE_CC(WAZANO_ZENRYOKUSUTOONEZZI);
  IS_COMPATIBLE_CC(WAZANO_ZENRYOKUSUKEIRUNOIZU);

  IS_COMPATIBLE_CC(WAZANO_MAX                          );


  /*
    tokusei.h
  */
  IS_COMPATIBLE_CC(TOKUSEI_NULL                        );
  IS_COMPATIBLE_CC(TOKUSEI_AKUSYUU                     );
  IS_COMPATIBLE_CC(TOKUSEI_AMEHURASI                   );
  IS_COMPATIBLE_CC(TOKUSEI_KASOKU                      );
  IS_COMPATIBLE_CC(TOKUSEI_KABUTOAAMAA                 );
  IS_COMPATIBLE_CC(TOKUSEI_GANZYOU                     );
  IS_COMPATIBLE_CC(TOKUSEI_SIMERIKE                    );
  IS_COMPATIBLE_CC(TOKUSEI_ZYUUNAN                     );
  IS_COMPATIBLE_CC(TOKUSEI_SUNAGAKURE                  );
  IS_COMPATIBLE_CC(TOKUSEI_SEIDENKI                    );
  IS_COMPATIBLE_CC(TOKUSEI_TIKUDEN                     );
  IS_COMPATIBLE_CC(TOKUSEI_TYOSUI                      );
  IS_COMPATIBLE_CC(TOKUSEI_DONKAN                      );
  IS_COMPATIBLE_CC(TOKUSEI_NOOTENKI                    );
  IS_COMPATIBLE_CC(TOKUSEI_HUKUGAN                     );
  IS_COMPATIBLE_CC(TOKUSEI_HUMIN                       );
  IS_COMPATIBLE_CC(TOKUSEI_HENSYOKU                    );
  IS_COMPATIBLE_CC(TOKUSEI_MENEKI                      );
  IS_COMPATIBLE_CC(TOKUSEI_MORAIBI                     );
  IS_COMPATIBLE_CC(TOKUSEI_RINPUN                      );
  IS_COMPATIBLE_CC(TOKUSEI_MAIPEESU                    );
  IS_COMPATIBLE_CC(TOKUSEI_KYUUBAN                     );
  IS_COMPATIBLE_CC(TOKUSEI_IKAKU                       );
  IS_COMPATIBLE_CC(TOKUSEI_KAGEHUMI                    );
  IS_COMPATIBLE_CC(TOKUSEI_SAMEHADA                    );
  IS_COMPATIBLE_CC(TOKUSEI_HUSIGINAMAMORI              );
  IS_COMPATIBLE_CC(TOKUSEI_HUYUU                       );
  IS_COMPATIBLE_CC(TOKUSEI_HOUSI                       );
  IS_COMPATIBLE_CC(TOKUSEI_SINKURO                     );
  IS_COMPATIBLE_CC(TOKUSEI_KURIABODHI                  );
  IS_COMPATIBLE_CC(TOKUSEI_SIZENKAIHUKU                );
  IS_COMPATIBLE_CC(TOKUSEI_HIRAISIN                    );
  IS_COMPATIBLE_CC(TOKUSEI_TENNOMEGUMI                 );
  IS_COMPATIBLE_CC(TOKUSEI_SUISUI                      );
  IS_COMPATIBLE_CC(TOKUSEI_YOURYOKUSO                  );
  IS_COMPATIBLE_CC(TOKUSEI_HAKKOU                      );
  IS_COMPATIBLE_CC(TOKUSEI_TOREESU                     );
  IS_COMPATIBLE_CC(TOKUSEI_TIKARAMOTI                  );
  IS_COMPATIBLE_CC(TOKUSEI_DOKUNOTOGE                  );
  IS_COMPATIBLE_CC(TOKUSEI_SEISINRYOKU                 );
  IS_COMPATIBLE_CC(TOKUSEI_MAGUMANOYOROI               );
  IS_COMPATIBLE_CC(TOKUSEI_MIZUNOBEERU                 );
  IS_COMPATIBLE_CC(TOKUSEI_ZIRYOKU                     );
  IS_COMPATIBLE_CC(TOKUSEI_BOUON                       );
  IS_COMPATIBLE_CC(TOKUSEI_AMEUKEZARA                  );
  IS_COMPATIBLE_CC(TOKUSEI_SUNAOKOSI                   );
  IS_COMPATIBLE_CC(TOKUSEI_PURESSYAA                   );
  IS_COMPATIBLE_CC(TOKUSEI_ATUISIBOU                   );
  IS_COMPATIBLE_CC(TOKUSEI_HAYAOKI                     );
  IS_COMPATIBLE_CC(TOKUSEI_HONOONOKARADA               );
  IS_COMPATIBLE_CC(TOKUSEI_NIGEASI                     );
  IS_COMPATIBLE_CC(TOKUSEI_SURUDOIME                   );
  IS_COMPATIBLE_CC(TOKUSEI_KAIRIKIBASAMI               );
  IS_COMPATIBLE_CC(TOKUSEI_MONOHIROI                   );
  IS_COMPATIBLE_CC(TOKUSEI_NAMAKE                      );
  IS_COMPATIBLE_CC(TOKUSEI_HARIKIRI                    );
  IS_COMPATIBLE_CC(TOKUSEI_MEROMEROBODHI               );
  IS_COMPATIBLE_CC(TOKUSEI_PURASU                      );
  IS_COMPATIBLE_CC(TOKUSEI_MAINASU                     );
  IS_COMPATIBLE_CC(TOKUSEI_TENKIYA                     );
  IS_COMPATIBLE_CC(TOKUSEI_NENTYAKU                    );
  IS_COMPATIBLE_CC(TOKUSEI_DAPPI                       );
  IS_COMPATIBLE_CC(TOKUSEI_KONZYOU                     );
  IS_COMPATIBLE_CC(TOKUSEI_HUSIGINAUROKO               );
  IS_COMPATIBLE_CC(TOKUSEI_HEDOROEKI                   );
  IS_COMPATIBLE_CC(TOKUSEI_SINRYOKU                    );
  IS_COMPATIBLE_CC(TOKUSEI_MOUKA                       );
  IS_COMPATIBLE_CC(TOKUSEI_GEKIRYUU                    );
  IS_COMPATIBLE_CC(TOKUSEI_MUSINOSIRASE                );
  IS_COMPATIBLE_CC(TOKUSEI_ISIATAMA                    );
  IS_COMPATIBLE_CC(TOKUSEI_HIDERI                      );
  IS_COMPATIBLE_CC(TOKUSEI_ARIZIGOKU                   );
  IS_COMPATIBLE_CC(TOKUSEI_YARUKI                      );
  IS_COMPATIBLE_CC(TOKUSEI_SIROIKEMURI                 );
  IS_COMPATIBLE_CC(TOKUSEI_YOGAPAWAA                   );
  IS_COMPATIBLE_CC(TOKUSEI_SHERUAAMAA                  );
  IS_COMPATIBLE_CC(TOKUSEI_EAROKKU                     );
  IS_COMPATIBLE_CC(TOKUSEI_TIDORIASI                   );
  IS_COMPATIBLE_CC(TOKUSEI_DENKIENZIN                  );
  IS_COMPATIBLE_CC(TOKUSEI_TOUSOUSIN                   );
  IS_COMPATIBLE_CC(TOKUSEI_HUKUTUNOKOKORO              );
  IS_COMPATIBLE_CC(TOKUSEI_YUKIGAKURE                  );
  IS_COMPATIBLE_CC(TOKUSEI_KUISINBOU                   );
  IS_COMPATIBLE_CC(TOKUSEI_IKARINOTUBO                 );
  IS_COMPATIBLE_CC(TOKUSEI_KARUWAZA                    );
  IS_COMPATIBLE_CC(TOKUSEI_TAINETU                     );
  IS_COMPATIBLE_CC(TOKUSEI_TANZYUN                     );
  IS_COMPATIBLE_CC(TOKUSEI_KANSOUHADA                  );
  IS_COMPATIBLE_CC(TOKUSEI_DAUNROODO                   );
  IS_COMPATIBLE_CC(TOKUSEI_TETUNOKOBUSI                );
  IS_COMPATIBLE_CC(TOKUSEI_POIZUNHIIRU                 );
  IS_COMPATIBLE_CC(TOKUSEI_TEKIOURYOKU                 );
  IS_COMPATIBLE_CC(TOKUSEI_SUKIRURINKU                 );
  IS_COMPATIBLE_CC(TOKUSEI_URUOIBODHI                  );
  IS_COMPATIBLE_CC(TOKUSEI_SANPAWAA                    );
  IS_COMPATIBLE_CC(TOKUSEI_HAYAASI                     );
  IS_COMPATIBLE_CC(TOKUSEI_NOOMARUSUKIN                );
  IS_COMPATIBLE_CC(TOKUSEI_SUNAIPAA                    );
  IS_COMPATIBLE_CC(TOKUSEI_MAZIKKUGAADO                );
  IS_COMPATIBLE_CC(TOKUSEI_NOOGAADO                    );
  IS_COMPATIBLE_CC(TOKUSEI_ATODASI                     );
  IS_COMPATIBLE_CC(TOKUSEI_TEKUNISYAN                  );
  IS_COMPATIBLE_CC(TOKUSEI_RIIHUGAADO                  );
  IS_COMPATIBLE_CC(TOKUSEI_BUKIYOU                     );
  IS_COMPATIBLE_CC(TOKUSEI_KATAYABURI                  );
  IS_COMPATIBLE_CC(TOKUSEI_KYOUUN                      );
  IS_COMPATIBLE_CC(TOKUSEI_YUUBAKU                     );
  IS_COMPATIBLE_CC(TOKUSEI_KIKENYOTI                   );
  IS_COMPATIBLE_CC(TOKUSEI_YOTIMU                      );
  IS_COMPATIBLE_CC(TOKUSEI_TENNEN                      );
  IS_COMPATIBLE_CC(TOKUSEI_IROMEGANE                   );
  IS_COMPATIBLE_CC(TOKUSEI_FIRUTAA                     );
  IS_COMPATIBLE_CC(TOKUSEI_SUROOSUTAATO                );
  IS_COMPATIBLE_CC(TOKUSEI_KIMOTTAMA                   );
  IS_COMPATIBLE_CC(TOKUSEI_YOBIMIZU                    );
  IS_COMPATIBLE_CC(TOKUSEI_AISUBODHI                   );
  IS_COMPATIBLE_CC(TOKUSEI_HAADOROKKU                  );
  IS_COMPATIBLE_CC(TOKUSEI_YUKIHURASI                  );
  IS_COMPATIBLE_CC(TOKUSEI_MITUATUME                   );
  IS_COMPATIBLE_CC(TOKUSEI_OMITOOSI                    );
  IS_COMPATIBLE_CC(TOKUSEI_SUTEMI                      );
  IS_COMPATIBLE_CC(TOKUSEI_MARUTITAIPU                 );
  IS_COMPATIBLE_CC(TOKUSEI_HURAWAAGIHUTO               );
  IS_COMPATIBLE_CC(TOKUSEI_NAITOMEA                    );
  IS_COMPATIBLE_CC(TOKUSEI_WARUITEGUSE                 );
  IS_COMPATIBLE_CC(TOKUSEI_TIKARAZUKU                  );
  IS_COMPATIBLE_CC(TOKUSEI_AMANOZYAKU                  );
  IS_COMPATIBLE_CC(TOKUSEI_KINTYOUKAN                  );
  IS_COMPATIBLE_CC(TOKUSEI_MAKENKI                     );
  IS_COMPATIBLE_CC(TOKUSEI_YOWAKI                      );
  IS_COMPATIBLE_CC(TOKUSEI_NOROWAREBODHI               );
  IS_COMPATIBLE_CC(TOKUSEI_IYASINOKOKORO               );
  IS_COMPATIBLE_CC(TOKUSEI_HURENDOGAADO                );
  IS_COMPATIBLE_CC(TOKUSEI_KUDAKERUYOROI               );
  IS_COMPATIBLE_CC(TOKUSEI_HEVHIMETARU                 );
  IS_COMPATIBLE_CC(TOKUSEI_RAITOMETARU                 );
  IS_COMPATIBLE_CC(TOKUSEI_MARUTISUKEIRU               );
  IS_COMPATIBLE_CC(TOKUSEI_DOKUBOUSOU                  );
  IS_COMPATIBLE_CC(TOKUSEI_NETUBOUSOU                  );
  IS_COMPATIBLE_CC(TOKUSEI_SYUUKAKU                    );
  IS_COMPATIBLE_CC(TOKUSEI_TEREPASII                   );
  IS_COMPATIBLE_CC(TOKUSEI_MURAKKE                     );
  IS_COMPATIBLE_CC(TOKUSEI_BOUZIN                      );
  IS_COMPATIBLE_CC(TOKUSEI_DOKUSYU                     );
  IS_COMPATIBLE_CC(TOKUSEI_SAISEIRYOKU                 );
  IS_COMPATIBLE_CC(TOKUSEI_HATOMUNE                    );
  IS_COMPATIBLE_CC(TOKUSEI_SUNAKAKI                    );
  IS_COMPATIBLE_CC(TOKUSEI_MIRAKURUSUKIN               );
  IS_COMPATIBLE_CC(TOKUSEI_ANARAIZU                    );
  IS_COMPATIBLE_CC(TOKUSEI_IRYUUZYON                   );
  IS_COMPATIBLE_CC(TOKUSEI_KAWARIMONO                  );
  IS_COMPATIBLE_CC(TOKUSEI_SURINUKE                    );
  IS_COMPATIBLE_CC(TOKUSEI_MIIRA                       );
  IS_COMPATIBLE_CC(TOKUSEI_ZISINKAZYOU                 );
  IS_COMPATIBLE_CC(TOKUSEI_SEIGINOKOKORO               );
  IS_COMPATIBLE_CC(TOKUSEI_BIBIRI                      );
  IS_COMPATIBLE_CC(TOKUSEI_MAZIKKUMIRAA                );
  IS_COMPATIBLE_CC(TOKUSEI_SOUSYOKU                    );
  IS_COMPATIBLE_CC(TOKUSEI_ITAZURAGOKORO               );
  IS_COMPATIBLE_CC(TOKUSEI_SUNANOTIKARA                );
  IS_COMPATIBLE_CC(TOKUSEI_TETUNOTOGE                  );
  IS_COMPATIBLE_CC(TOKUSEI_DARUMAMOODO                 );
  IS_COMPATIBLE_CC(TOKUSEI_SYOURINOHOSI                );
  IS_COMPATIBLE_CC(TOKUSEI_TAABOBUREIZU                );
  IS_COMPATIBLE_CC(TOKUSEI_TERABORUTEEZI               );
  IS_COMPATIBLE_CC(TOKUSEI_AROMABEERU                  );
  IS_COMPATIBLE_CC(TOKUSEI_HURAWAABEERU                );
  IS_COMPATIBLE_CC(TOKUSEI_HOOBUKURO                   );
  IS_COMPATIBLE_CC(TOKUSEI_HENGENZIZAI                 );
  IS_COMPATIBLE_CC(TOKUSEI_FAAKOOTO                    );
  IS_COMPATIBLE_CC(TOKUSEI_MAZISYAN                    );
  IS_COMPATIBLE_CC(TOKUSEI_BOUDAN                      );
  IS_COMPATIBLE_CC(TOKUSEI_KATIKI                      );
  IS_COMPATIBLE_CC(TOKUSEI_GANZYOUAGO                  );
  IS_COMPATIBLE_CC(TOKUSEI_HURIIZUSUKIN                );
  IS_COMPATIBLE_CC(TOKUSEI_SUIITOBEERU                 );
  IS_COMPATIBLE_CC(TOKUSEI_BATORUSUITTI                );
  IS_COMPATIBLE_CC(TOKUSEI_HAYATENOTUBASA              );
  IS_COMPATIBLE_CC(TOKUSEI_MEGARANTYAA                 );
  IS_COMPATIBLE_CC(TOKUSEI_KUSANOKEGAWA                );
  IS_COMPATIBLE_CC(TOKUSEI_KYOUSEI                     );
  IS_COMPATIBLE_CC(TOKUSEI_KATAITUME                   );
  IS_COMPATIBLE_CC(TOKUSEI_FEARIISUKIN                 );
  IS_COMPATIBLE_CC(TOKUSEI_NUMENUME                    );
  IS_COMPATIBLE_CC(TOKUSEI_SUKAISUKIN                  );
  IS_COMPATIBLE_CC(TOKUSEI_OYAKOAI                     );
  IS_COMPATIBLE_CC(TOKUSEI_DAAKUOORA                   );
  IS_COMPATIBLE_CC(TOKUSEI_FEARIIOORA                  );
  IS_COMPATIBLE_CC(TOKUSEI_OORABUREIKU                 );
  IS_COMPATIBLE_CC(TOKUSEI_HAZIMARINOUMI               );
  IS_COMPATIBLE_CC(TOKUSEI_OWARINODAITI                );
  IS_COMPATIBLE_CC(TOKUSEI_DERUTASUTORIIMU             );
  IS_COMPATIBLE_CC(TOKUSEI_ZIKYUURYOKU                 );
  IS_COMPATIBLE_CC(TOKUSEI_NIGEGOSI                    );
  IS_COMPATIBLE_CC(TOKUSEI_KIKIKAIHI                   );
  IS_COMPATIBLE_CC(TOKUSEI_MIZUGATAME                  );
  IS_COMPATIBLE_CC(TOKUSEI_HITODENASI                  );
  IS_COMPATIBLE_CC(TOKUSEI_RIMITTOSIIRUDO              );
  IS_COMPATIBLE_CC(TOKUSEI_HARIKOMI                    );
  IS_COMPATIBLE_CC(TOKUSEI_SUIHOU                      );
  IS_COMPATIBLE_CC(TOKUSEI_HAGANETUKAI                 );
  IS_COMPATIBLE_CC(TOKUSEI_GYAKUZYOU                   );
  IS_COMPATIBLE_CC(TOKUSEI_YUKIKAKI                    );
  IS_COMPATIBLE_CC(TOKUSEI_ENKAKU                      );
  IS_COMPATIBLE_CC(TOKUSEI_URUOIBOISU                  );
  IS_COMPATIBLE_CC(TOKUSEI_HIIRINGUSIHUTO              );
  IS_COMPATIBLE_CC(TOKUSEI_EREKISUKIN                  );
  IS_COMPATIBLE_CC(TOKUSEI_SAAHUTEERU                  );
  IS_COMPATIBLE_CC(TOKUSEI_GYOGUN                      );
  IS_COMPATIBLE_CC(TOKUSEI_BAKENOKAWA                  );
  IS_COMPATIBLE_CC(TOKUSEI_KIZUNAHENGE                 );
  IS_COMPATIBLE_CC(TOKUSEI_SUWAAMUTHENZI               );
  IS_COMPATIBLE_CC(TOKUSEI_HUSYOKU                     );
  IS_COMPATIBLE_CC(TOKUSEI_ZETTAINEMURI                );
  IS_COMPATIBLE_CC(TOKUSEI_ZYOOUNOIGEN                 );
  IS_COMPATIBLE_CC(TOKUSEI_TOBIDASUNAKAMI              );
  IS_COMPATIBLE_CC(TOKUSEI_ODORIKO                     );
  IS_COMPATIBLE_CC(TOKUSEI_BATTERII                    );
  IS_COMPATIBLE_CC(TOKUSEI_MOHUMOHU                    );
  IS_COMPATIBLE_CC(TOKUSEI_BIBIDDOBODHI                );
  IS_COMPATIBLE_CC(TOKUSEI_SOURUHAATO                  );
  IS_COMPATIBLE_CC(TOKUSEI_KAARIIHEAA                  );
  IS_COMPATIBLE_CC(TOKUSEI_RESIIBAA                    );
  IS_COMPATIBLE_CC(TOKUSEI_KAGAKUNOTIKARA              );
  IS_COMPATIBLE_CC(TOKUSEI_URUTORAFOOSU                );
  IS_COMPATIBLE_CC(TOKUSEI_arSISUTEMU                  );
  IS_COMPATIBLE_CC(TOKUSEI_EREKIMEIKAA                 );
  IS_COMPATIBLE_CC(TOKUSEI_SAIKOMEIKAA                 );
  IS_COMPATIBLE_CC(TOKUSEI_MISUTOMEIKAA                );
  IS_COMPATIBLE_CC(TOKUSEI_GURASUMEIKAA                );
  IS_COMPATIBLE_CC(TOKUSEI_METARUPUROTEKUTO            );
  IS_COMPATIBLE_CC(TOKUSEI_FANTOMUGAADO                );
  IS_COMPATIBLE_CC(TOKUSEI_PURIZUMUAAMAA               );
  IS_COMPATIBLE_CC(TOKUSEI_BUREINPURIZUMU              );
  IS_COMPATIBLE_CC(TOKUSEI_MAX                         );
  IS_COMPATIBLE_CC(TOKUSEI_NUM                         );

  /*
    seikaku.h
  */
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_GANBARIYA    );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_SAMISHIGARI  );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_YUUKAN       );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_IJIPPARI     );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_YANTYA       );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_ZUBUTOI      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_SUNAO        );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_NONKI        );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_WANPAKU      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_NOUTENKI     );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_OKUBYOU      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_SEKKATI      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_MAJIME       );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_YOUKI        );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_MUJYAKI      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_HIKAEME      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_OTTORI       );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_REISEI       );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_TEREYA       );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_UKKARIYA     );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_ODAYAKA      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_OTONASHII    );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_NAMAIKI      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_SINTYOU      );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_KIMAGURE     );
  IS_COMPATIBLE_CC(pml::pokepara::SEIKAKU_NUM          );


  /*
    itemsym.h
  */
  IS_COMPATIBLE_REDEF(ITEM_DUMMY_DATA);
  IS_COMPATIBLE_REDEF(ITEM_MASUTAABOORU);
  IS_COMPATIBLE_REDEF(ITEM_HAIPAABOORU);
  IS_COMPATIBLE_REDEF(ITEM_SUUPAABOORU);
  IS_COMPATIBLE_REDEF(ITEM_MONSUTAABOORU);
  IS_COMPATIBLE_REDEF(ITEM_SAFARIBOORU);
  IS_COMPATIBLE_REDEF(ITEM_NETTOBOORU);
  IS_COMPATIBLE_REDEF(ITEM_DAIBUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_NESUTOBOORU);
  IS_COMPATIBLE_REDEF(ITEM_RIPIITOBOORU);
  IS_COMPATIBLE_REDEF(ITEM_TAIMAABOORU);
  IS_COMPATIBLE_REDEF(ITEM_GOOZYASUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_PUREMIABOORU);
  IS_COMPATIBLE_REDEF(ITEM_DAAKUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_HIIRUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_KUIKKUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_PURESYASUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_KIZUGUSURI);
  IS_COMPATIBLE_REDEF(ITEM_DOKUKESI);
  IS_COMPATIBLE_REDEF(ITEM_YAKEDONAOSI);
  IS_COMPATIBLE_REDEF(ITEM_KOORINAOSI);
  IS_COMPATIBLE_REDEF(ITEM_NEMUKEZAMASI);
  IS_COMPATIBLE_REDEF(ITEM_MAHINAOSI);
  IS_COMPATIBLE_REDEF(ITEM_KAIHUKUNOKUSURI);
  IS_COMPATIBLE_REDEF(ITEM_MANTANNOKUSURI);
  IS_COMPATIBLE_REDEF(ITEM_SUGOIKIZUGUSURI);
  IS_COMPATIBLE_REDEF(ITEM_IIKIZUGUSURI);
  IS_COMPATIBLE_REDEF(ITEM_NANDEMONAOSI);
  IS_COMPATIBLE_REDEF(ITEM_GENKINOKAKERA);
  IS_COMPATIBLE_REDEF(ITEM_GENKINOKATAMARI);
  IS_COMPATIBLE_REDEF(ITEM_OISIIMIZU);
  IS_COMPATIBLE_REDEF(ITEM_SAIKOSOODA);
  IS_COMPATIBLE_REDEF(ITEM_MIKKUSUORE);
  IS_COMPATIBLE_REDEF(ITEM_MOOMOOMIRUKU);
  IS_COMPATIBLE_REDEF(ITEM_TIKARANOKONA);
  IS_COMPATIBLE_REDEF(ITEM_TIKARANONEKKO);
  IS_COMPATIBLE_REDEF(ITEM_BANNOUGONA);
  IS_COMPATIBLE_REDEF(ITEM_HUKKATUSOU);
  IS_COMPATIBLE_REDEF(ITEM_PIIPIIEIDO);
  IS_COMPATIBLE_REDEF(ITEM_PIIPIIRIKABAA);
  IS_COMPATIBLE_REDEF(ITEM_PIIPIIEIDAA);
  IS_COMPATIBLE_REDEF(ITEM_PIIPIIMAKKUSU);
  IS_COMPATIBLE_REDEF(ITEM_HUENSENBEI);
  IS_COMPATIBLE_REDEF(ITEM_KINOMIZYUUSU);
  IS_COMPATIBLE_REDEF(ITEM_SEINARUHAI);
  IS_COMPATIBLE_REDEF(ITEM_MAKKUSUAPPU);
  IS_COMPATIBLE_REDEF(ITEM_TAURIN);
  IS_COMPATIBLE_REDEF(ITEM_BUROMUHEKISIN);
  IS_COMPATIBLE_REDEF(ITEM_INDOMETASIN);
  IS_COMPATIBLE_REDEF(ITEM_RIZOTIUMU);
  IS_COMPATIBLE_REDEF(ITEM_HUSIGINAAME);
  IS_COMPATIBLE_REDEF(ITEM_POINTOAPPU);
  IS_COMPATIBLE_REDEF(ITEM_KITOSAN);
  IS_COMPATIBLE_REDEF(ITEM_POINTOMAKKUSU);
  IS_COMPATIBLE_REDEF(ITEM_MORINOYOUKAN);
  IS_COMPATIBLE_REDEF(ITEM_EFEKUTOGAADO);
  IS_COMPATIBLE_REDEF(ITEM_KURITHIKATTO);
  IS_COMPATIBLE_REDEF(ITEM_PURASUPAWAA);
  IS_COMPATIBLE_REDEF(ITEM_DHIFENDAA);
  IS_COMPATIBLE_REDEF(ITEM_SUPIIDAA);
  IS_COMPATIBLE_REDEF(ITEM_YOKUATAARU);
  IS_COMPATIBLE_REDEF(ITEM_SUPESYARUAPPU);
  IS_COMPATIBLE_REDEF(ITEM_SUPESYARUGAADO);
  IS_COMPATIBLE_REDEF(ITEM_PIPPININGYOU);
  IS_COMPATIBLE_REDEF(ITEM_ENEKONOSIPPO);
  IS_COMPATIBLE_REDEF(ITEM_AOIBIIDORO);
  IS_COMPATIBLE_REDEF(ITEM_KIIROBIIDORO);
  IS_COMPATIBLE_REDEF(ITEM_AKAIBIIDORO);
  IS_COMPATIBLE_REDEF(ITEM_KUROIBIIDORO);
  IS_COMPATIBLE_REDEF(ITEM_SIROIBIIDORO);
  IS_COMPATIBLE_REDEF(ITEM_ASASENOSIO);
  IS_COMPATIBLE_REDEF(ITEM_ASASENOKAIGARA);
  IS_COMPATIBLE_REDEF(ITEM_AKAIKAKERA);
  IS_COMPATIBLE_REDEF(ITEM_AOIKAKERA);
  IS_COMPATIBLE_REDEF(ITEM_KIIROIKAKERA);
  IS_COMPATIBLE_REDEF(ITEM_MIDORINOKAKERA);
  IS_COMPATIBLE_REDEF(ITEM_SIRUBAASUPUREE);
  IS_COMPATIBLE_REDEF(ITEM_GOORUDOSUPUREE);
  IS_COMPATIBLE_REDEF(ITEM_ANANUKENOHIMO);
  IS_COMPATIBLE_REDEF(ITEM_MUSIYOKESUPUREE);
  IS_COMPATIBLE_REDEF(ITEM_TAIYOUNOISI);
  IS_COMPATIBLE_REDEF(ITEM_TUKINOISI);
  IS_COMPATIBLE_REDEF(ITEM_HONOONOISI);
  IS_COMPATIBLE_REDEF(ITEM_KAMINARINOISI);
  IS_COMPATIBLE_REDEF(ITEM_MIZUNOISI);
  IS_COMPATIBLE_REDEF(ITEM_RIIHUNOISI);
  IS_COMPATIBLE_REDEF(ITEM_TIISANAKINOKO);
  IS_COMPATIBLE_REDEF(ITEM_OOKINAKINOKO);
  IS_COMPATIBLE_REDEF(ITEM_SINZYU);
  IS_COMPATIBLE_REDEF(ITEM_OOKINASINZYU);
  IS_COMPATIBLE_REDEF(ITEM_HOSINOSUNA);
  IS_COMPATIBLE_REDEF(ITEM_HOSINOKAKERA);
  IS_COMPATIBLE_REDEF(ITEM_KINNOTAMA);
  IS_COMPATIBLE_REDEF(ITEM_HAATONOUROKO);
  IS_COMPATIBLE_REDEF(ITEM_AMAIMITU);
  IS_COMPATIBLE_REDEF(ITEM_SUKUSUKUKOYASI);
  IS_COMPATIBLE_REDEF(ITEM_ZIMEZIMEKOYASI);
  IS_COMPATIBLE_REDEF(ITEM_NAGANAGAKOYASI);
  IS_COMPATIBLE_REDEF(ITEM_NEBANEBAKOYASI);
  IS_COMPATIBLE_REDEF(ITEM_NEKKONOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_TUMENOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_KAINOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_KOURANOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_HIMITUNOKOHAKU);
  IS_COMPATIBLE_REDEF(ITEM_TATENOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_ZUGAINOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_KITYOUNAHONE);
  IS_COMPATIBLE_REDEF(ITEM_HIKARINOISI);
  IS_COMPATIBLE_REDEF(ITEM_YAMINOISI);
  IS_COMPATIBLE_REDEF(ITEM_MEZAMEISI);
  IS_COMPATIBLE_REDEF(ITEM_MANMARUISI);
  IS_COMPATIBLE_REDEF(ITEM_KANAMEISI);
  IS_COMPATIBLE_REDEF(ITEM_HAKKINDAMA);
  IS_COMPATIBLE_REDEF(ITEM_AKUAKASETTO);
  IS_COMPATIBLE_REDEF(ITEM_INAZUMAKASETTO);
  IS_COMPATIBLE_REDEF(ITEM_BUREIZUKASETTO);
  IS_COMPATIBLE_REDEF(ITEM_HURIIZUKASETTO);
  IS_COMPATIBLE_REDEF(ITEM_HAATOSUIITU);
  IS_COMPATIBLE_REDEF(ITEM_KONGOUDAMA);
  IS_COMPATIBLE_REDEF(ITEM_SIRATAMA);
  IS_COMPATIBLE_REDEF(ITEM_HAZIMETEMEERU);
  IS_COMPATIBLE_REDEF(ITEM_DAISUKIMEERU);
  IS_COMPATIBLE_REDEF(ITEM_OSASOIMEERU);
  IS_COMPATIBLE_REDEF(ITEM_KANSYAMEERU);
  IS_COMPATIBLE_REDEF(ITEM_SITUMONMEERU);
  IS_COMPATIBLE_REDEF(ITEM_OSUSUMEMEERU);
  IS_COMPATIBLE_REDEF(ITEM_OKAESIMEERU);
  IS_COMPATIBLE_REDEF(ITEM_BURIZZIMEERUs);
  IS_COMPATIBLE_REDEF(ITEM_BURIZZIMEERUh);
  IS_COMPATIBLE_REDEF(ITEM_BURIZZIMEERUc);
  IS_COMPATIBLE_REDEF(ITEM_BURIZZIMEERUv);
  IS_COMPATIBLE_REDEF(ITEM_BURIZZIMEERUw);
  IS_COMPATIBLE_REDEF(ITEM_KURABONOMI);
  IS_COMPATIBLE_REDEF(ITEM_KAGONOMI);
  IS_COMPATIBLE_REDEF(ITEM_MOMONNOMI);
  IS_COMPATIBLE_REDEF(ITEM_TIIGONOMI);
  IS_COMPATIBLE_REDEF(ITEM_NANASINOMI);
  IS_COMPATIBLE_REDEF(ITEM_HIMERINOMI);
  IS_COMPATIBLE_REDEF(ITEM_ORENNOMI);
  IS_COMPATIBLE_REDEF(ITEM_KIINOMI);
  IS_COMPATIBLE_REDEF(ITEM_RAMUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_OBONNOMI);
  IS_COMPATIBLE_REDEF(ITEM_FIRANOMI);
  IS_COMPATIBLE_REDEF(ITEM_UINOMI);
  IS_COMPATIBLE_REDEF(ITEM_MAGONOMI);
  IS_COMPATIBLE_REDEF(ITEM_BANZINOMI);
  IS_COMPATIBLE_REDEF(ITEM_IANOMI);
  IS_COMPATIBLE_REDEF(ITEM_ZURINOMI);
  IS_COMPATIBLE_REDEF(ITEM_BURIINOMI);
  IS_COMPATIBLE_REDEF(ITEM_NANANOMI);
  IS_COMPATIBLE_REDEF(ITEM_SESINANOMI);
  IS_COMPATIBLE_REDEF(ITEM_PAIRUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_ZAROKUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_NEKOBUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_TAPORUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_ROMENOMI);
  IS_COMPATIBLE_REDEF(ITEM_UBUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_MATOMANOMI);
  IS_COMPATIBLE_REDEF(ITEM_MOKOSINOMI);
  IS_COMPATIBLE_REDEF(ITEM_GOSUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_RABUTANOMI);
  IS_COMPATIBLE_REDEF(ITEM_NOMERUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_NOWAKINOMI);
  IS_COMPATIBLE_REDEF(ITEM_SIIYANOMI);
  IS_COMPATIBLE_REDEF(ITEM_KAISUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_DORINOMI);
  IS_COMPATIBLE_REDEF(ITEM_BERIBUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_OKKANOMI);
  IS_COMPATIBLE_REDEF(ITEM_ITOKENOMI);
  IS_COMPATIBLE_REDEF(ITEM_SOKUNONOMI);
  IS_COMPATIBLE_REDEF(ITEM_RINDONOMI);
  IS_COMPATIBLE_REDEF(ITEM_YATHENOMI);
  IS_COMPATIBLE_REDEF(ITEM_YOPUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_BIAANOMI);
  IS_COMPATIBLE_REDEF(ITEM_SYUKANOMI);
  IS_COMPATIBLE_REDEF(ITEM_BAKOUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_UTANNOMI);
  IS_COMPATIBLE_REDEF(ITEM_TANGANOMI);
  IS_COMPATIBLE_REDEF(ITEM_YOROGINOMI);
  IS_COMPATIBLE_REDEF(ITEM_KASIBUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_HABANNOMI);
  IS_COMPATIBLE_REDEF(ITEM_NAMONOMI);
  IS_COMPATIBLE_REDEF(ITEM_RIRIBANOMI);
  IS_COMPATIBLE_REDEF(ITEM_HOZUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_TIIRANOMI);
  IS_COMPATIBLE_REDEF(ITEM_RYUGANOMI);
  IS_COMPATIBLE_REDEF(ITEM_KAMURANOMI);
  IS_COMPATIBLE_REDEF(ITEM_YATAPINOMI);
  IS_COMPATIBLE_REDEF(ITEM_ZUANOMI);
  IS_COMPATIBLE_REDEF(ITEM_SANNOMI);
  IS_COMPATIBLE_REDEF(ITEM_SUTAANOMI);
  IS_COMPATIBLE_REDEF(ITEM_NAZONOMI);
  IS_COMPATIBLE_REDEF(ITEM_MIKURUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_IBANNOMI);
  IS_COMPATIBLE_REDEF(ITEM_ZYAPONOMI);
  IS_COMPATIBLE_REDEF(ITEM_RENBUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_HIKARINOKONA);
  IS_COMPATIBLE_REDEF(ITEM_SIROIHAABU);
  IS_COMPATIBLE_REDEF(ITEM_KYOUSEIGIPUSU);
  IS_COMPATIBLE_REDEF(ITEM_GAKUSYUUSOUTI);
  IS_COMPATIBLE_REDEF(ITEM_SENSEINOTUME);
  IS_COMPATIBLE_REDEF(ITEM_YASURAGINOSUZU);
  IS_COMPATIBLE_REDEF(ITEM_MENTARUHAABU);
  IS_COMPATIBLE_REDEF(ITEM_KODAWARIHATIMAKI);
  IS_COMPATIBLE_REDEF(ITEM_OUZYANOSIRUSI);
  IS_COMPATIBLE_REDEF(ITEM_GINNOKONA);
  IS_COMPATIBLE_REDEF(ITEM_OMAMORIKOBAN);
  IS_COMPATIBLE_REDEF(ITEM_KIYOMENOOHUDA);
  IS_COMPATIBLE_REDEF(ITEM_KOKORONOSIZUKU);
  IS_COMPATIBLE_REDEF(ITEM_SINKAINOKIBA);
  IS_COMPATIBLE_REDEF(ITEM_SINKAINOUROKO);
  IS_COMPATIBLE_REDEF(ITEM_KEMURIDAMA);
  IS_COMPATIBLE_REDEF(ITEM_KAWARAZUNOISI);
  IS_COMPATIBLE_REDEF(ITEM_KIAINOHATIMAKI);
  IS_COMPATIBLE_REDEF(ITEM_SIAWASETAMAGO);
  IS_COMPATIBLE_REDEF(ITEM_PINTORENZU);
  IS_COMPATIBLE_REDEF(ITEM_METARUKOOTO);
  IS_COMPATIBLE_REDEF(ITEM_TABENOKOSI);
  IS_COMPATIBLE_REDEF(ITEM_RYUUNOUROKO);
  IS_COMPATIBLE_REDEF(ITEM_DENKIDAMA);
  IS_COMPATIBLE_REDEF(ITEM_YAWARAKAISUNA);
  IS_COMPATIBLE_REDEF(ITEM_KATAIISI);
  IS_COMPATIBLE_REDEF(ITEM_KISEKINOTANE);
  IS_COMPATIBLE_REDEF(ITEM_KUROIMEGANE);
  IS_COMPATIBLE_REDEF(ITEM_KUROOBI);
  IS_COMPATIBLE_REDEF(ITEM_ZISYAKU);
  IS_COMPATIBLE_REDEF(ITEM_SINPINOSIZUKU);
  IS_COMPATIBLE_REDEF(ITEM_SURUDOIKUTIBASI);
  IS_COMPATIBLE_REDEF(ITEM_DOKUBARI);
  IS_COMPATIBLE_REDEF(ITEM_TOKENAIKOORI);
  IS_COMPATIBLE_REDEF(ITEM_NOROINOOHUDA);
  IS_COMPATIBLE_REDEF(ITEM_MAGATTASUPUUN);
  IS_COMPATIBLE_REDEF(ITEM_MOKUTAN);
  IS_COMPATIBLE_REDEF(ITEM_RYUUNOKIBA);
  IS_COMPATIBLE_REDEF(ITEM_SIRUKUNOSUKAAHU);
  IS_COMPATIBLE_REDEF(ITEM_APPUGUREEDO);
  IS_COMPATIBLE_REDEF(ITEM_KAIGARANOSUZU);
  IS_COMPATIBLE_REDEF(ITEM_USIONOOKOU);
  IS_COMPATIBLE_REDEF(ITEM_NONKINOOKOU);
  IS_COMPATIBLE_REDEF(ITEM_RAKKIIPANTI);
  IS_COMPATIBLE_REDEF(ITEM_METARUPAUDAA);
  IS_COMPATIBLE_REDEF(ITEM_HUTOIHONE);
  IS_COMPATIBLE_REDEF(ITEM_NAGANEGI);
  IS_COMPATIBLE_REDEF(ITEM_AKAIBANDANA);
  IS_COMPATIBLE_REDEF(ITEM_AOIBANDANA);
  IS_COMPATIBLE_REDEF(ITEM_PINKUNOBANDANA);
  IS_COMPATIBLE_REDEF(ITEM_MIDORINOBANDANA);
  IS_COMPATIBLE_REDEF(ITEM_KIIRONOBANDANA);
  IS_COMPATIBLE_REDEF(ITEM_KOUKAKURENZU);
  IS_COMPATIBLE_REDEF(ITEM_TIKARANOHATIMAKI);
  IS_COMPATIBLE_REDEF(ITEM_MONOSIRIMEGANE);
  IS_COMPATIBLE_REDEF(ITEM_TATUZINNOOBI);
  IS_COMPATIBLE_REDEF(ITEM_HIKARINONENDO);
  IS_COMPATIBLE_REDEF(ITEM_INOTINOTAMA);
  IS_COMPATIBLE_REDEF(ITEM_PAWAHURUHAABU);
  IS_COMPATIBLE_REDEF(ITEM_DOKUDOKUDAMA);
  IS_COMPATIBLE_REDEF(ITEM_KAENDAMA);
  IS_COMPATIBLE_REDEF(ITEM_SUPIIDOPAUDAA);
  IS_COMPATIBLE_REDEF(ITEM_KIAINOTASUKI);
  IS_COMPATIBLE_REDEF(ITEM_FOOKASURENZU);
  IS_COMPATIBLE_REDEF(ITEM_METORONOOMU);
  IS_COMPATIBLE_REDEF(ITEM_KUROITEKKYUU);
  IS_COMPATIBLE_REDEF(ITEM_KOUKOUNOSIPPO);
  IS_COMPATIBLE_REDEF(ITEM_AKAIITO);
  IS_COMPATIBLE_REDEF(ITEM_KUROIHEDORO);
  IS_COMPATIBLE_REDEF(ITEM_TUMETAIIWA);
  IS_COMPATIBLE_REDEF(ITEM_SARASARAIWA);
  IS_COMPATIBLE_REDEF(ITEM_ATUIIWA);
  IS_COMPATIBLE_REDEF(ITEM_SIMETTAIWA);
  IS_COMPATIBLE_REDEF(ITEM_NEBARINOKAGIDUME);
  IS_COMPATIBLE_REDEF(ITEM_KODAWARISUKAAHU);
  IS_COMPATIBLE_REDEF(ITEM_KUTTUKIBARI);
  IS_COMPATIBLE_REDEF(ITEM_PAWAARISUTO);
  IS_COMPATIBLE_REDEF(ITEM_PAWAABERUTO);
  IS_COMPATIBLE_REDEF(ITEM_PAWAARENZU);
  IS_COMPATIBLE_REDEF(ITEM_PAWAABANDO);
  IS_COMPATIBLE_REDEF(ITEM_PAWAAANKURU);
  IS_COMPATIBLE_REDEF(ITEM_PAWAAUEITO);
  IS_COMPATIBLE_REDEF(ITEM_KIREINANUKEGARA);
  IS_COMPATIBLE_REDEF(ITEM_OOKINANEKKO);
  IS_COMPATIBLE_REDEF(ITEM_KODAWARIMEGANE);
  IS_COMPATIBLE_REDEF(ITEM_HINOTAMAPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_SIZUKUPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_IKAZUTIPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_MIDORINOPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_TURARANOPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_KOBUSINOPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_MOUDOKUPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_DAITINOPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_AOZORAPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_HUSIGINOPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_TAMAMUSIPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_GANSEKIPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_MONONOKEPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_RYUUNOPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_KOWAMOTEPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_KOUTETUPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_AYASIIOKOU);
  IS_COMPATIBLE_REDEF(ITEM_GANSEKIOKOU);
  IS_COMPATIBLE_REDEF(ITEM_MANPUKUOKOU);
  IS_COMPATIBLE_REDEF(ITEM_SAZANAMINOOKOU);
  IS_COMPATIBLE_REDEF(ITEM_OHANANOOKOU);
  IS_COMPATIBLE_REDEF(ITEM_KOUUNNOOKOU);
  IS_COMPATIBLE_REDEF(ITEM_KIYOMENOOKOU);
  IS_COMPATIBLE_REDEF(ITEM_PUROTEKUTAA);
  IS_COMPATIBLE_REDEF(ITEM_EREKIBUUSUTAA);
  IS_COMPATIBLE_REDEF(ITEM_MAGUMABUUSUTAA);
  IS_COMPATIBLE_REDEF(ITEM_AYASIIPATTI);
  IS_COMPATIBLE_REDEF(ITEM_REIKAINONUNO);
  IS_COMPATIBLE_REDEF(ITEM_SURUDOITUME);
  IS_COMPATIBLE_REDEF(ITEM_SURUDOIKIBA);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN01);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN02);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN03);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN04);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN05);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN06);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN07);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN08);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN09);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN10);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN11);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN12);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN13);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN14);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN15);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN16);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN17);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN18);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN19);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN20);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN21);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN22);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN23);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN24);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN25);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN26);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN27);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN28);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN29);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN30);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN31);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN32);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN33);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN34);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN35);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN36);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN37);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN38);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN39);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN40);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN41);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN42);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN43);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN44);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN45);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN46);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN47);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN48);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN49);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN50);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN51);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN52);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN53);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN54);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN55);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN56);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN57);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN58);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN59);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN60);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN61);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN62);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN63);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN64);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN65);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN66);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN67);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN68);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN69);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN70);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN71);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN72);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN73);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN74);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN75);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN76);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN77);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN78);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN79);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN80);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN81);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN82);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN83);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN84);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN85);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN86);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN87);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN88);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN89);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN90);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN91);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN92);
  IS_COMPATIBLE_REDEF(ITEM_HIDENMASIN01);
  IS_COMPATIBLE_REDEF(ITEM_HIDENMASIN02);
  IS_COMPATIBLE_REDEF(ITEM_HIDENMASIN03);
  IS_COMPATIBLE_REDEF(ITEM_HIDENMASIN04);
  IS_COMPATIBLE_REDEF(ITEM_HIDENMASIN05);
  IS_COMPATIBLE_REDEF(ITEM_HIDENMASIN06);
  IS_COMPATIBLE_REDEF(ITEM_TANKENSETTO);
  IS_COMPATIBLE_REDEF(ITEM_TAKARABUKURO);
  IS_COMPATIBLE_REDEF(ITEM_RUURUBUKKU);
  IS_COMPATIBLE_REDEF(ITEM_POKETORE);
  IS_COMPATIBLE_REDEF(ITEM_POINTOKAADO);
  IS_COMPATIBLE_REDEF(ITEM_BOUKENNOOTO);
  IS_COMPATIBLE_REDEF(ITEM_SIIRUIRE);
  IS_COMPATIBLE_REDEF(ITEM_AKUSESARIIIRE);
  IS_COMPATIBLE_REDEF(ITEM_SIIRUBUKURO);
  IS_COMPATIBLE_REDEF(ITEM_TOMODATITETYOU);
  IS_COMPATIBLE_REDEF(ITEM_HATUDENSYOKII);
  IS_COMPATIBLE_REDEF(ITEM_KODAINOOMAMORI);
  IS_COMPATIBLE_REDEF(ITEM_GINGADANNOKAGI);
  IS_COMPATIBLE_REDEF(ITEM_AKAIKUSARI);
  IS_COMPATIBLE_REDEF(ITEM_TAUNMAPPU);
  IS_COMPATIBLE_REDEF(ITEM_BATORUSAATYAA);
  IS_COMPATIBLE_REDEF(ITEM_KOINKEESU);
  IS_COMPATIBLE_REDEF(ITEM_BORONOTURIZAO);
  IS_COMPATIBLE_REDEF(ITEM_IITURIZAO);
  IS_COMPATIBLE_REDEF(ITEM_SUGOITURIZAO);
  IS_COMPATIBLE_REDEF(ITEM_KODAKKUZYOURO);
  IS_COMPATIBLE_REDEF(ITEM_POFINKEESU);
  IS_COMPATIBLE_REDEF(ITEM_ZITENSYA);
  IS_COMPATIBLE_REDEF(ITEM_RUUMUKII);
  IS_COMPATIBLE_REDEF(ITEM_OOKIDONOTEGAMI);
  IS_COMPATIBLE_REDEF(ITEM_MIKADUKINOHANE);
  IS_COMPATIBLE_REDEF(ITEM_MENBAAZUKAADO);
  IS_COMPATIBLE_REDEF(ITEM_TENKAINOHUE);
  IS_COMPATIBLE_REDEF(ITEM_HUNENOTIKETTO);
  IS_COMPATIBLE_REDEF(ITEM_KONTESUTOPASU);
  IS_COMPATIBLE_REDEF(ITEM_KAZANNOOKIISI);
  IS_COMPATIBLE_REDEF(ITEM_OTODOKEMONO);
  IS_COMPATIBLE_REDEF(ITEM_HIKIKAEKEN1);
  IS_COMPATIBLE_REDEF(ITEM_HIKIKAEKEN2);
  IS_COMPATIBLE_REDEF(ITEM_HIKIKAEKEN3);
  IS_COMPATIBLE_REDEF(ITEM_SOUKONOKAGI);
  IS_COMPATIBLE_REDEF(ITEM_HIDENNOKUSURI);
  IS_COMPATIBLE_REDEF(ITEM_BATORUREKOODAA);
  IS_COMPATIBLE_REDEF(ITEM_GURASIDEANOHANA);
  IS_COMPATIBLE_REDEF(ITEM_HIMITUNOKAGI);
  IS_COMPATIBLE_REDEF(ITEM_BONGURIKEESU);
  IS_COMPATIBLE_REDEF(ITEM_ANNOONNOOTO);
  IS_COMPATIBLE_REDEF(ITEM_KINOMIPURANTAA);
  IS_COMPATIBLE_REDEF(ITEM_DAUZINGUMASIN);
  IS_COMPATIBLE_REDEF(ITEM_BURUUKAADO);
  IS_COMPATIBLE_REDEF(ITEM_OISIISIPPO);
  IS_COMPATIBLE_REDEF(ITEM_TOUMEINASUZU);
  IS_COMPATIBLE_REDEF(ITEM_KAADOKII);
  IS_COMPATIBLE_REDEF(ITEM_TIKANOKAGI);
  IS_COMPATIBLE_REDEF(ITEM_ZENIGAMEZYOURO);
  IS_COMPATIBLE_REDEF(ITEM_AKAIUROKO);
  IS_COMPATIBLE_REDEF(ITEM_OTOSIMONO);
  IS_COMPATIBLE_REDEF(ITEM_RINIAPASU);
  IS_COMPATIBLE_REDEF(ITEM_KIKAINOBUHIN);
  IS_COMPATIBLE_REDEF(ITEM_GINIRONOHANE);
  IS_COMPATIBLE_REDEF(ITEM_NIZIIRONOHANE);
  IS_COMPATIBLE_REDEF(ITEM_HUSIGINATAMAGO);
  IS_COMPATIBLE_REDEF(ITEM_AKABONGURI);
  IS_COMPATIBLE_REDEF(ITEM_AOBONGURI);
  IS_COMPATIBLE_REDEF(ITEM_KIBONGURI);
  IS_COMPATIBLE_REDEF(ITEM_MIDOBONGURI);
  IS_COMPATIBLE_REDEF(ITEM_MOMOBONGURI);
  IS_COMPATIBLE_REDEF(ITEM_SIROBONGURI);
  IS_COMPATIBLE_REDEF(ITEM_KUROBONGURI);
  IS_COMPATIBLE_REDEF(ITEM_SUPIIDOBOORU);
  IS_COMPATIBLE_REDEF(ITEM_REBERUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_RUAABOORU);
  IS_COMPATIBLE_REDEF(ITEM_HEBIIBOORU);
  IS_COMPATIBLE_REDEF(ITEM_RABURABUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_HURENDOBOORU);
  IS_COMPATIBLE_REDEF(ITEM_MUUNBOORU);
  IS_COMPATIBLE_REDEF(ITEM_KONPEBOORU);
  IS_COMPATIBLE_REDEF(ITEM_PAAKUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_FOTOARUBAMU);
  IS_COMPATIBLE_REDEF(ITEM_gbPUREIYAA);
  IS_COMPATIBLE_REDEF(ITEM_UMINARINOSUZU);
  IS_COMPATIBLE_REDEF(ITEM_IKARIMANZYUU);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO01);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO02);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO03);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO04);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO05);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO06);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO07);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO08);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO09);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO10);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO11);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO12);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO13);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO14);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO15);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO16);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO17);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO18);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO19);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO20);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO21);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO22);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO23);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO24);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO25);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO26);
  IS_COMPATIBLE_REDEF(ITEM_DEETAKAADO27);
  IS_COMPATIBLE_REDEF(ITEM_MOEGIIRONOTAMA);
  IS_COMPATIBLE_REDEF(ITEM_ROKKUKAPUSERU);
  IS_COMPATIBLE_REDEF(ITEM_BENIIRONOTAMA);
  IS_COMPATIBLE_REDEF(ITEM_AIIRONOTAMA);
  IS_COMPATIBLE_REDEF(ITEM_NAZONOSUISYOU);
  IS_COMPATIBLE_REDEF(ITEM_KIREINAUROKO);
  IS_COMPATIBLE_REDEF(ITEM_SINKANOKISEKI);
  IS_COMPATIBLE_REDEF(ITEM_KARUISI);
  IS_COMPATIBLE_REDEF(ITEM_GOTUGOTUMETTO);
  IS_COMPATIBLE_REDEF(ITEM_HUUSEN);
  IS_COMPATIBLE_REDEF(ITEM_REDDOKAADO);
  IS_COMPATIBLE_REDEF(ITEM_NERAINOMATO);
  IS_COMPATIBLE_REDEF(ITEM_SIMETUKEBANDO);
  IS_COMPATIBLE_REDEF(ITEM_KYUUKON);
  IS_COMPATIBLE_REDEF(ITEM_ZYUUDENTI);
  IS_COMPATIBLE_REDEF(ITEM_DASSYUTUBOTAN);
  IS_COMPATIBLE_REDEF(ITEM_HONOONOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_MIZUNOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_DENKINOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_KUSANOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_KOORINOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_KAKUTOUZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_DOKUNOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_ZIMENNOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_HIKOUNOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_ESUPAAZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_MUSINOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_IWANOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_GOOSUTOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_DORAGONZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_AKUNOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_HAGANENOZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_TAIRYOKUNOHANE);
  IS_COMPATIBLE_REDEF(ITEM_KINRYOKUNOHANE);
  IS_COMPATIBLE_REDEF(ITEM_TEIKOUNOHANE);
  IS_COMPATIBLE_REDEF(ITEM_TIRYOKUNOHANE);
  IS_COMPATIBLE_REDEF(ITEM_SEISINNOHANE);
  IS_COMPATIBLE_REDEF(ITEM_SYUNPATUNOHANE);
  IS_COMPATIBLE_REDEF(ITEM_KIREINAHANE);
  IS_COMPATIBLE_REDEF(ITEM_HUTANOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_HANENOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_RIBATHITIKETTO);
  IS_COMPATIBLE_REDEF(ITEM_DERUDAMA);
  IS_COMPATIBLE_REDEF(ITEM_DORIIMUBOORU);
  IS_COMPATIBLE_REDEF(ITEM_POKEZYARASI);
  IS_COMPATIBLE_REDEF(ITEM_GUZZUKEESU);
  IS_COMPATIBLE_REDEF(ITEM_DORAGONNOHONE);
  IS_COMPATIBLE_REDEF(ITEM_KAORUKINOKO);
  IS_COMPATIBLE_REDEF(ITEM_DEKAIKINNOTAMA);
  IS_COMPATIBLE_REDEF(ITEM_ODANGOSINZYU);
  IS_COMPATIBLE_REDEF(ITEM_SUISEINOKAKERA);
  IS_COMPATIBLE_REDEF(ITEM_KODAINODOUKA);
  IS_COMPATIBLE_REDEF(ITEM_KODAINOGINKA);
  IS_COMPATIBLE_REDEF(ITEM_KODAINOKINKA);
  IS_COMPATIBLE_REDEF(ITEM_KODAINOTUBO);
  IS_COMPATIBLE_REDEF(ITEM_KODAINOUDEWA);
  IS_COMPATIBLE_REDEF(ITEM_KODAINOSEKIZOU);
  IS_COMPATIBLE_REDEF(ITEM_KODAINOOUKAN);
  IS_COMPATIBLE_REDEF(ITEM_HIUNAISU);
  IS_COMPATIBLE_REDEF(ITEM_KURITHIKATTO2);
  IS_COMPATIBLE_REDEF(ITEM_SUPIIDAA2);
  IS_COMPATIBLE_REDEF(ITEM_spAPPU2);
  IS_COMPATIBLE_REDEF(ITEM_spGAADO2);
  IS_COMPATIBLE_REDEF(ITEM_DHIFENDAA2);
  IS_COMPATIBLE_REDEF(ITEM_PURASUPAWAA2);
  IS_COMPATIBLE_REDEF(ITEM_YOKUATAARU2);
  IS_COMPATIBLE_REDEF(ITEM_SUPIIDAA3);
  IS_COMPATIBLE_REDEF(ITEM_spAPPU3);
  IS_COMPATIBLE_REDEF(ITEM_spGAADO3);
  IS_COMPATIBLE_REDEF(ITEM_DHIFENDAA3);
  IS_COMPATIBLE_REDEF(ITEM_PURASUPAWAA3);
  IS_COMPATIBLE_REDEF(ITEM_YOKUATAARU3);
  IS_COMPATIBLE_REDEF(ITEM_SUPIIDAA6);
  IS_COMPATIBLE_REDEF(ITEM_spAPPU6);
  IS_COMPATIBLE_REDEF(ITEM_spGAADO6);
  IS_COMPATIBLE_REDEF(ITEM_DHIFENDAA6);
  IS_COMPATIBLE_REDEF(ITEM_PURASUPAWAA6);
  IS_COMPATIBLE_REDEF(ITEM_YOKUATAARU6);
  IS_COMPATIBLE_REDEF(ITEM_SUKIRUKOORU);
  IS_COMPATIBLE_REDEF(ITEM_AITEMUDOROPPU);
  IS_COMPATIBLE_REDEF(ITEM_AITEMUKOORU);
  IS_COMPATIBLE_REDEF(ITEM_HURATTOKOORU);
  IS_COMPATIBLE_REDEF(ITEM_KURITHIKATTO3);
  IS_COMPATIBLE_REDEF(ITEM_RAITOSUTOON);
  IS_COMPATIBLE_REDEF(ITEM_DAAKUSUTOON);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN93);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN94);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN95);
  IS_COMPATIBLE_REDEF(ITEM_RAIBUKYASUTAA);
  IS_COMPATIBLE_REDEF(ITEM_GODDOSUTOON);
  IS_COMPATIBLE_REDEF(ITEM_HAITATUBUTU1);
  IS_COMPATIBLE_REDEF(ITEM_HAITATUBUTU2);
  IS_COMPATIBLE_REDEF(ITEM_HAITATUBUTU3);
  IS_COMPATIBLE_REDEF(ITEM_RAIBUKYASUTAA_1);
  IS_COMPATIBLE_REDEF(ITEM_MEDARUBOKKUSU);
  IS_COMPATIBLE_REDEF(ITEM_IDENSINOKUSABI);
  IS_COMPATIBLE_REDEF(ITEM_IDENSINOKUSABI_1);
  IS_COMPATIBLE_REDEF(ITEM_KYOKASYOU);
  IS_COMPATIBLE_REDEF(ITEM_MARUIOMAMORI);
  IS_COMPATIBLE_REDEF(ITEM_HIKARUOMAMORI);
  IS_COMPATIBLE_REDEF(ITEM_PURAZUMAKAADO);
  IS_COMPATIBLE_REDEF(ITEM_YOGORETAHANKATI);
  IS_COMPATIBLE_REDEF(ITEM_AKUROMAMASIIN);
  IS_COMPATIBLE_REDEF(ITEM_WASUREMONO);
  IS_COMPATIBLE_REDEF(ITEM_WASUREMONO_1);
  IS_COMPATIBLE_REDEF(ITEM_UTUSIKAGAMI);
  IS_COMPATIBLE_REDEF(ITEM_ZYAKUTENHOKEN);
  IS_COMPATIBLE_REDEF(ITEM_TOTUGEKITYOKKI);
  IS_COMPATIBLE_REDEF(ITEM_HOROKYASUTAA);
  IS_COMPATIBLE_REDEF(ITEM_HAKASENOTEGAMI);
  IS_COMPATIBLE_REDEF(ITEM_ROORAASUKEETO);
  IS_COMPATIBLE_REDEF(ITEM_SEIREIPUREETO);
  IS_COMPATIBLE_REDEF(ITEM_TOKUSEIKAPUSERU);
  IS_COMPATIBLE_REDEF(ITEM_HOIPPUPOPPU);
  IS_COMPATIBLE_REDEF(ITEM_NIOIBUKURO);
  IS_COMPATIBLE_REDEF(ITEM_HIKARIGOKE);
  IS_COMPATIBLE_REDEF(ITEM_YUKIDAMA);
  IS_COMPATIBLE_REDEF(ITEM_BOUZINGOOGURU);
  IS_COMPATIBLE_REDEF(ITEM_POKEMONNOHUE);
  IS_COMPATIBLE_REDEF(ITEM_TAWAWAKOYASI);
  IS_COMPATIBLE_REDEF(ITEM_BIKKURIKOYASI);
  IS_COMPATIBLE_REDEF(ITEM_GUNGUNKOYASI);
  IS_COMPATIBLE_REDEF(ITEM_TONDEMOKOYASI);
  IS_COMPATIBLE_REDEF(ITEM_GENGANAITO);
  IS_COMPATIBLE_REDEF(ITEM_SAANAITONAITO);
  IS_COMPATIBLE_REDEF(ITEM_DENRYUUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_HUSIGIBANAITO);
  IS_COMPATIBLE_REDEF(ITEM_RIZAADONAITOx);
  IS_COMPATIBLE_REDEF(ITEM_KAMEKKUSUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_MYUUTUNAITOx);
  IS_COMPATIBLE_REDEF(ITEM_MYUUTUNAITOy);
  IS_COMPATIBLE_REDEF(ITEM_BASYAAMONAITO);
  IS_COMPATIBLE_REDEF(ITEM_TYAAREMUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_HERUGANAITO);
  IS_COMPATIBLE_REDEF(ITEM_BOSUGODORANAITO);
  IS_COMPATIBLE_REDEF(ITEM_ZYUPETTANAITO);
  IS_COMPATIBLE_REDEF(ITEM_BANGIRASUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_HASSAMUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_KAIROSUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_PUTERANAITO);
  IS_COMPATIBLE_REDEF(ITEM_RUKARIONAITO);
  IS_COMPATIBLE_REDEF(ITEM_YUKINOONAITO);
  IS_COMPATIBLE_REDEF(ITEM_GARUURANAITO);
  IS_COMPATIBLE_REDEF(ITEM_GYARADOSUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_ABUSORUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_RIZAADONAITOy);
  IS_COMPATIBLE_REDEF(ITEM_HUUDHINAITO);
  IS_COMPATIBLE_REDEF(ITEM_HERAKUROSUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_KUTIITONAITO);
  IS_COMPATIBLE_REDEF(ITEM_RAIBORUTONAITO);
  IS_COMPATIBLE_REDEF(ITEM_GABURIASUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_RATHIASUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_RATHIOSUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_ROZERUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_AKKINOMI);
  IS_COMPATIBLE_REDEF(ITEM_TARAPUNOMI);
  IS_COMPATIBLE_REDEF(ITEM_HASUBOOZYOURO);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN96);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN97);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN98);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN99);
  IS_COMPATIBLE_REDEF(ITEM_WAZAMASIN100);
  IS_COMPATIBLE_REDEF(ITEM_HATUDENSYOPASU);
  IS_COMPATIBLE_REDEF(ITEM_MEGARINGU);
  IS_COMPATIBLE_REDEF(ITEM_SUGOSOUNAISI);
  IS_COMPATIBLE_REDEF(ITEM_ARIHURETAISI);
  IS_COMPATIBLE_REDEF(ITEM_BAAGENTIKETTO);
  IS_COMPATIBLE_REDEF(ITEM_EREBEETANOKII);
  IS_COMPATIBLE_REDEF(ITEM_tmvPASU);
  IS_COMPATIBLE_REDEF(ITEM_KAROSUENBUREMU);
  IS_COMPATIBLE_REDEF(ITEM_TANKENKOKOROE);
  IS_COMPATIBLE_REDEF(ITEM_HUSIGINAOKIMONO);
  IS_COMPATIBLE_REDEF(ITEM_RENZUKEESU);
  IS_COMPATIBLE_REDEF(ITEM_KOSUMEPOOTI);
  IS_COMPATIBLE_REDEF(ITEM_ISYOUTORANKU);
  IS_COMPATIBLE_REDEF(ITEM_MIAREGARETTO);
  IS_COMPATIBLE_REDEF(ITEM_SYARASABURE);
  IS_COMPATIBLE_REDEF(ITEM_AGONOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_HIRENOKASEKI);
  IS_COMPATIBLE_REDEF(ITEM_HANSAMUTIKETTO);
  IS_COMPATIBLE_REDEF(ITEM_ZITENSYA_1);
  IS_COMPATIBLE_REDEF(ITEM_HOROKYASUTAA_1);
  IS_COMPATIBLE_REDEF(ITEM_YOUSEIZYUERU);
  IS_COMPATIBLE_REDEF(ITEM_MEGATYAAMU);
  IS_COMPATIBLE_REDEF(ITEM_MEGAGUROOBU);
  IS_COMPATIBLE_REDEF(ITEM_MAHHAZITENSYA);
  IS_COMPATIBLE_REDEF(ITEM_DAATOZITENSYA);
  IS_COMPATIBLE_REDEF(ITEM_HOERUKOZYOURO);
  IS_COMPATIBLE_REDEF(ITEM_DEBONNONIMOTU);
  IS_COMPATIBLE_REDEF(ITEM_HAIBUKURO);
  IS_COMPATIBLE_REDEF(ITEM_TIKANOKAGI_1);
  IS_COMPATIBLE_REDEF(ITEM_POROKKUKITTO);
  IS_COMPATIBLE_REDEF(ITEM_DAIGOHENOTEGAMI);
  IS_COMPATIBLE_REDEF(ITEM_MUGENNOTIKETTO);
  IS_COMPATIBLE_REDEF(ITEM_TANTIKI);
  IS_COMPATIBLE_REDEF(ITEM_GOOGOOGOOGURU);
  IS_COMPATIBLE_REDEF(ITEM_INSEKI);
  IS_COMPATIBLE_REDEF(ITEM_1GOUSITUNOKAGI);
  IS_COMPATIBLE_REDEF(ITEM_2GOUSITUNOKAGI);
  IS_COMPATIBLE_REDEF(ITEM_4GOUSITUNOKAGI);
  IS_COMPATIBLE_REDEF(ITEM_6GOUSITUNOKAGI);
  IS_COMPATIBLE_REDEF(ITEM_SOUKONOKAGI_1);
  IS_COMPATIBLE_REDEF(ITEM_DEBONSUKOOPU);
  IS_COMPATIBLE_REDEF(ITEM_HUNENOTIKETTO_1);
  IS_COMPATIBLE_REDEF(ITEM_HIDENMASIN07);
  IS_COMPATIBLE_REDEF(ITEM_DEBONBONBE);
  IS_COMPATIBLE_REDEF(ITEM_RAIBUSUUTU);
  IS_COMPATIBLE_REDEF(ITEM_RAIBUDORESU);
  IS_COMPATIBLE_REDEF(ITEM_MAGUMASUUTU);
  IS_COMPATIBLE_REDEF(ITEM_AKUASUUTU);
  IS_COMPATIBLE_REDEF(ITEM_PEATIKETTO);
  IS_COMPATIBLE_REDEF(ITEM_MEGABANGURU);
  IS_COMPATIBLE_REDEF(ITEM_MEGANEKKURESU);
  IS_COMPATIBLE_REDEF(ITEM_MEGAGURASU);
  IS_COMPATIBLE_REDEF(ITEM_MEGAANKAA);
  IS_COMPATIBLE_REDEF(ITEM_MEGARABERUPIN);
  IS_COMPATIBLE_REDEF(ITEM_MEGATHIARA);
  IS_COMPATIBLE_REDEF(ITEM_MEGAANKURETTO);
  IS_COMPATIBLE_REDEF(ITEM_INSEKI_1);
  IS_COMPATIBLE_REDEF(ITEM_RAGURAAZINAITO);
  IS_COMPATIBLE_REDEF(ITEM_ZYUKAINNAITO);
  IS_COMPATIBLE_REDEF(ITEM_YAMIRAMINAITO);
  IS_COMPATIBLE_REDEF(ITEM_TIRUTARISUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_ERUREIDONAITO);
  IS_COMPATIBLE_REDEF(ITEM_TABUNNENAITO);
  IS_COMPATIBLE_REDEF(ITEM_METAGUROSUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_SAMEHADANAITO);
  IS_COMPATIBLE_REDEF(ITEM_YADORANNAITO);
  IS_COMPATIBLE_REDEF(ITEM_HAGANEERUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_PIZYOTTONAITO);
  IS_COMPATIBLE_REDEF(ITEM_ONIGOORINAITO);
  IS_COMPATIBLE_REDEF(ITEM_DHIANSINAITO);
  IS_COMPATIBLE_REDEF(ITEM_IMASIMENOTUBO);
  IS_COMPATIBLE_REDEF(ITEM_MEGABURESU);
  IS_COMPATIBLE_REDEF(ITEM_BAKUUDANAITO);
  IS_COMPATIBLE_REDEF(ITEM_MIMIROPPUNAITO);
  IS_COMPATIBLE_REDEF(ITEM_BOOMANDANAITO);
  IS_COMPATIBLE_REDEF(ITEM_SUPIANAITO);
  IS_COMPATIBLE_REDEF(ITEM_INSEKI_2);
  IS_COMPATIBLE_REDEF(ITEM_INSEKI_3);
  IS_COMPATIBLE_REDEF(ITEM_KIISUTOON);
  IS_COMPATIBLE_REDEF(ITEM_INSEKINOKAKERA);
  IS_COMPATIBLE_REDEF(ITEM_MUGENNOHUE);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_HONOOBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_MIZUBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_DENKIBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_KUSABIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_KOORIBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_KAKUTOUBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_DOKUBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_ZIMENBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_HIKOUBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_ESUPAABIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_MUSIBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_IWABIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_GOOSUTOBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_DORAGONBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_AKUBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_HAGANEBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_FEARIIBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_PIKATYUUBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_GINNOOUKAN);
  IS_COMPATIBLE_REDEF(ITEM_KINNOOUKAN);
  IS_COMPATIBLE_REDEF(ITEM_ZENRYOKURINGU);
  IS_COMPATIBLE_REDEF(ITEM_HUKUROU3BIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_TORA3BIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_ASIKA3BIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_THIKIBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_HAISINBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_ARORAIBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_KABIGONBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_IIBUIBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_MYUUBIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_HONOOBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_MIZUBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_DENKIBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_KUSABIIZU);
  IS_COMPATIBLE_REDEF(ITEM_KOORIBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_KAKUTOUBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_DOKUBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_ZIMENBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_HIKOUBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_ESUPAABIIZU);
  IS_COMPATIBLE_REDEF(ITEM_MUSIBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_IWABIIZU);
  IS_COMPATIBLE_REDEF(ITEM_GOOSUTOBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_DORAGONBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_AKUBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_HAGANEBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_FEARIIBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_PIKATYUUBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_HUKUROU3BIIZU);
  IS_COMPATIBLE_REDEF(ITEM_TORA3BIIZU);
  IS_COMPATIBLE_REDEF(ITEM_ASIKA3BIIZU);
  IS_COMPATIBLE_REDEF(ITEM_THIKIBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_HAISINBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_ARORAIBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_KABIGONBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_IIBUIBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_MYUUBIIZU);
  IS_COMPATIBLE_REDEF(ITEM_SATOPIKABIIZU);
  IS_COMPATIBLE_REDEF(ITEM_SATOPIKABIIZU_1);
  IS_COMPATIBLE_REDEF(ITEM_PURIANOHANA);
  IS_COMPATIBLE_REDEF(ITEM_BISUKASUNOHANA);
  IS_COMPATIBLE_REDEF(ITEM_BATOBARADANOHANA);
  IS_COMPATIBLE_REDEF(ITEM_KURAUNNOHANA);
  IS_COMPATIBLE_REDEF(ITEM_ZAIRYOUBUKURO);
  IS_COMPATIBLE_REDEF(ITEM_TURIZAO);
  IS_COMPATIBLE_REDEF(ITEM_HAKASENOHUKUMEN);
  IS_COMPATIBLE_REDEF(ITEM_FESUTIKETTO);
  IS_COMPATIBLE_REDEF(ITEM_KAGAYAKUISI);
  IS_COMPATIBLE_REDEF(ITEM_BIBIRIDAMA);
  IS_COMPATIBLE_REDEF(ITEM_ZIGARUDEKYUUBU);
  IS_COMPATIBLE_REDEF(ITEM_KANZENKYUUBU);
  IS_COMPATIBLE_REDEF(ITEM_KOORINOISI);
  IS_COMPATIBLE_REDEF(ITEM_RAIDOGIA);
  IS_COMPATIBLE_REDEF(ITEM_URUTORABOORU);
  IS_COMPATIBLE_REDEF(ITEM_OOKIIMARASADA);
  IS_COMPATIBLE_REDEF(ITEM_KURENAINOMITU);
  IS_COMPATIBLE_REDEF(ITEM_YAMABUKINOMITU);
  IS_COMPATIBLE_REDEF(ITEM_USUMOMONOMITU);
  IS_COMPATIBLE_REDEF(ITEM_MURASAKINOMITU);
  IS_COMPATIBLE_REDEF(ITEM_TAIYOUNOHUE);
  IS_COMPATIBLE_REDEF(ITEM_TUKINOHUE);
  IS_COMPATIBLE_REDEF(ITEM_KUKUINOTEGAMI);
  IS_COMPATIBLE_REDEF(ITEM_AYASIIKAADO);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII3);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII4);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII5);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII6);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII7);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII8);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII9);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII10);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII11);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII12);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII13);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII14);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII15);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII16);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII17);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII18);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII19);
  IS_COMPATIBLE_REDEF(ITEM_IBENTODAMII20);
  IS_COMPATIBLE_REDEF(ITEM_GURANDOKOOTO);
  IS_COMPATIBLE_REDEF(ITEM_BOUGOPATTO);
  IS_COMPATIBLE_REDEF(ITEM_EREKISIIDO);
  IS_COMPATIBLE_REDEF(ITEM_SAIKOSIIDO);
  IS_COMPATIBLE_REDEF(ITEM_MISUTOSIIDO);
  IS_COMPATIBLE_REDEF(ITEM_GURASUSIIDO);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII7);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII8);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII9);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII10);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII11);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII12);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII13);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII14);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII15);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII16);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII17);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII18);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII19);
  IS_COMPATIBLE_REDEF(ITEM_NOOMARUDAMII20);
  IS_COMPATIBLE_REDEF(ITEM_KAIHUKUDAMII1);
  IS_COMPATIBLE_REDEF(ITEM_KAIHUKUDAMII2);
  IS_COMPATIBLE_REDEF(ITEM_KAIHUKUDAMII3);
  IS_COMPATIBLE_REDEF(ITEM_KAIHUKUDAMII4);
  IS_COMPATIBLE_REDEF(ITEM_KAIHUKUDAMII5);
  IS_COMPATIBLE_REDEF(ITEM_FAITOMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_HURAINGUMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_POIZUNMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_GURAUNDOMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_ROKKUMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_BAGUMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_GOOSUTOMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_SUTIIRUMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_FAIYAAMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_UOOTAAMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_GURASUMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_EREKUTOROMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_SAIKIKKUMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_AISUMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_DORAGONMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_DAAKUMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_FEARIIMEMORI);
  IS_COMPATIBLE_REDEF(ITEM_SORUGAREOz_1);
  IS_COMPATIBLE_REDEF(ITEM_RUNAAARAz_1);
  IS_COMPATIBLE_REDEF(ITEM_NEKUROZUMAz_1);
  IS_COMPATIBLE_REDEF(ITEM_MIMIKKYUz_1);
  IS_COMPATIBLE_REDEF(ITEM_RUGARUGANz_1);
  IS_COMPATIBLE_REDEF(ITEM_ZYARARANGAz_1);
  IS_COMPATIBLE_REDEF(ITEM_SORUGAREOz);
  IS_COMPATIBLE_REDEF(ITEM_RUNAAARAz);
  IS_COMPATIBLE_REDEF(ITEM_NEKUROZUMAz);
  IS_COMPATIBLE_REDEF(ITEM_MIMIKKYUz);
  IS_COMPATIBLE_REDEF(ITEM_RUGARUGANz);
  IS_COMPATIBLE_REDEF(ITEM_ZYARARANGAz);


  IS_COMPATIBLE_REDEF(ITEM_DATA_MAX);



  /*
    JoinFestaNetDefine.h
  */
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::JOIN_FESTA_BEACON_VERSION_SYMBOL                    );

//  static const u8 JOIN_FESTA_BEACON_VERSION = JOIN_FESTA_BEACON_VERSION_SYMBOL;
//  static const u8 PACKET_MAX_SIZE =  164;

  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL               );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::JOIN_FESTA_BEACON_GUEST_USER_MAX                    );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX                                      );


  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP                           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST                         );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_ANOTHER_MAX                   );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MY                            );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX                           );

  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_NORAML                  );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_SHOP                    );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_PERSONAL                );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_FIELD                              );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_SINGLE                      );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_DOUBLE                      );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_MULTI                       );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_ROYAL                       );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_TRADE                              );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_GTS                                );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_MIRACLE_TRADE                      );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_SPOT                        );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_ATTRACTION                         );

  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_NONE                    );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_BASIC_TOP              );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS        );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS_FIELD  );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_SINGLE           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DOUBLE           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_MULTI            );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_ROYAL            );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE                   );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_GTS                     );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_SPOT             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_MIRACLE_TRADE           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_SHOP                 );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_PERSONAL             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_BASIC_BOTTOM           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_BATTLE_TOP             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CANCEL           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CONNECT          );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_JOIN             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DISJOIN          );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_BATTLE_BOTTOM          );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_TRADE_TOP              );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE            );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_CANCEL            );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_CONNECT           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_JOIN              );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_DISJOIN           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_TRADE_BOTTOM           );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_MESSAGE_TOP            );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_MESSAGE_BOTTOM         );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_ATTRACTION_TOP         );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION              );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_ATTRACTION_BOTTOM      );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUESR_TERMINATE_FOR_NIJI      );

  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_NONE                      );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_BATTLE_INVITE             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_BATTLE_RECIEVE            );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TRADE_INVITE              );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TRADE_RECIEVE             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_MESSAGE                   );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_ATTRACTION                );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_2             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_3             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_4             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_5             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_6             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_7             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_8             );
  IS_COMPATIBLE_CC(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TERMINATE_FOR_NIJI        );

  /*
    JoinFestaPawnDefine.h
  */

  //! ポケマメのアイテムID（アイテムではないが、抽選テーブルに入っている為定義切る）
  IS_COMPATIBLE_CC(JoinFestaScript::POKEMAME_ITEM_ID);

  /**
   * @enum PersonalEventType
   *       パーソナルイベント種類
   */
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_NULL                   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECENT                 );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECORD_BOAST           );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECORD_GAME            );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECORD_INTRODUCTION    );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECORD_GUESS           );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP1   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP2   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP3   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP4   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP5   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP6   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP7   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP8   );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_PERSON  );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_QUESTIONNAIRE_GUESS    );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_QUESTIONNAIRE_Q        );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_THANKS                 );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_DRESSUP_BOAST          );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_DRESSUP_Q              );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_DRESSUP_GUESS          );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_SECRET                 );
  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_FES_BOAST              );

  IS_COMPATIBLE_CC(JoinFestaScript::PERSONAL_EVENT_MAX                    );

  /**
   * @enum RecordCategory
   *       レコードのカテゴリ
   */
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_BATTLE                );
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_TRADE                 );
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_GREED                 );
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_POKEMON               );
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_DAILY                 );
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_CIRCLE                );
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_VARIETY               );
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_TOTAL                 );
  IS_COMPATIBLE_CC(JoinFestaScript::RECORD_CATEGORY_MAX                   );

  /**
   * @enum PatternType
   *       定型文種類
   */
  IS_COMPATIBLE_CC(JoinFestaScript::PATTERN_HELLO                         );
  IS_COMPATIBLE_CC(JoinFestaScript::PATTERN_PARTING                       );
  IS_COMPATIBLE_CC(JoinFestaScript::PATTERN_EMOTION                       );
  IS_COMPATIBLE_CC(JoinFestaScript::PATTERN_REGRET                        );
  IS_COMPATIBLE_CC(JoinFestaScript::PATTERN_SECRET                        );

  /**
   * @enum RewardType
   *       ご褒美種類
   */
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_ATTRACTION                     );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_COMMAND_SHOP                   );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_COMMAND_DRESSUP                );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_NAMING                         );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_PREMIUM                        );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_BGM                            );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_THEME1                         );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_THEME2                         );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_THEME3                         );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_SELECT_WORD                    );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_PRESENT_SHOP                   );

  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_MAX                            );

  /**
   * @enum RewardStateType
   *       ご褒美状態
   */
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_STATE_NULL                     );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_STATE_SATISFY_CONDITION        );
  IS_COMPATIBLE_CC(JoinFestaScript::REWARD_STATE_FINISH                   );

  /**
   * @enum CommRouteType
   *       通信経路
   */
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_BEACON                     );
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_SUBSCRIPTION               );
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_GTS                        );
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_MIRACLE_TRADE              );
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_RANDOM_BATTLE              );
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_P2P_TRADE                  );
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_P2P_BATTLE                 );
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_QUICK                      );

  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_MAX                        );
  IS_COMPATIBLE_CC(JoinFestaScript::COMM_ROUTE_NULL                       );

  /**
   * @enum FacilitiesType
   *       施設種類
   */
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_TYPE_LOTTERY               );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_TYPE_AIR_TOY               );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE             );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_TYPE_STALL                 );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP             );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_TYPE_FORTUNE               );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_TYPE_DYEING                );

  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_TYPE_MAX                   );

  /**
   * @enum FacilitiesIndex
   *       施設インデックス（逆時計回りで0オリジン)
   */
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_0                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_1                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_2                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_3                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_4                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_5                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_6                    );

  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_MAX                  );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_INDEX_NULL                 );

  /**
   * @enum FacilitiesColor
   *       施設カラー
   */
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_1                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_2                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_3                    );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_4                    );

  // 染物屋用
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_RED                  );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_GREEN                );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_ORANGE               );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_PURPLE               );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_YELLOW               );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_BLUE                 );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_NAVY                 );
  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_PINK                 );

  IS_COMPATIBLE_CC(JoinFestaScript::FACILITIES_COLOR_MAX                  );

  /**
   * @enum FortuneResultHit
   *       占いの結果：当たり
   */
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_HIT_NULL               );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_HIT_SMALL              );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_HIT_MIDDLE             );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_HIT_BIG                );

  /**
   * @enum FortuneResultAction
   *       占い結果:行動
   */
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_NULL            );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_GTS             );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_MIRACLE         );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_BTLSPOT         );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_FESCIRCLE       );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_ATTRACTION      );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_LOTTERY         );
  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_FUN_HOUSE       );

  IS_COMPATIBLE_CC(JoinFestaScript::FORTUNE_RESULT_ACTION_MAX             );


  /**
   * @enum AttractionId
   *       アトラクションID
   */
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RIDE_KENTAROSU_1           );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RIDE_KAIRIKY_1             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_1      );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_2      );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_3      );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_4      );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_5      );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_6      );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_7      );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_JP         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_EN         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_FR         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_IT         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_GE         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_SP         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_KR         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_CT         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_LANGUAGE_1                 );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_AFFINITY_1                 );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_AFFINITY_SAKASA_1          );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_1        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_1              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_2        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_2              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_3        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_3              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_4        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_4              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_5        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_5              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_6        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_6              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_7        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_7              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_8        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_8              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_9        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_9              );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_10       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_10             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_11       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_11             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_12       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_12             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_13       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_13             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_14       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_14             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_15       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_15             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_16       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_16             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_17       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_17             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_18       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_TYPE_SEARCH_18             );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_DIFFERENCE_1       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_DIFFERENCE_2       );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_1            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_2            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_3            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_4            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_5            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_6            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_7            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_8            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_9            );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_10           );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_11           );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_12           );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_13           );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_14           );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_15           );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_QUESTIONNAIRE_16           );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_DISGUISE_1         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_DISGUISE_2         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_1         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_2         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_3         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_4         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_5         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_6         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_7         );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_8         );

  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_NUM                        );
  IS_COMPATIBLE_CC(JoinFestaScript::ATTRACTION_NULL                       );


  /*
    msg_jf_phrase.h
  */
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_001);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_002);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_003);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_004);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_005);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_006);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_007);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_008);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_009);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_010);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_011);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_012);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_013);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_014);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_015);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_016);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_017);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_018);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_019);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_020);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_021);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_022);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_023);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_024);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_025);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_026);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_027);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_028);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_029);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_030);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_031);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_032);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_033);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_034);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_035);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_036);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_037);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_038);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_039);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_040);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_041);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_042);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_043);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_044);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_045);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_046);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_047);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_048);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_049);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_050);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_051);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_052);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_053);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_054);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_055);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_056);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_057);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_058);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_059);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_060);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_061);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_062);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_063);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_064);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_065);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_066);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_067);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_068);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_069);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_070);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_071);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_072);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_073);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_074);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_075);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_076);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_077);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_078);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_079);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_080);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_081);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_082);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_083);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_084);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_085);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_086);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_087);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_088);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_089);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_090);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_091);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_092);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_093);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_094);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_095);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_096);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_097);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_098);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_099);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_100);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_101);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_102);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_103);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_104);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_105);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_106);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_107);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_108);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_109);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_110);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_111);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_112);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_113);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_114);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_115);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_116);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_117);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_118);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_119);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_120);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_121);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_122);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_123);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_124);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_125);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_126);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_127);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_128);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_129);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_130);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_131);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_132);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_133);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_134);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_135);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_136);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_137);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_138);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_139);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_140);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_141);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_142);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_143);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_144);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_145);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_146);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_147);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_148);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_149);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_150);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_151);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_152);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_153);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_154);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_155);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_156);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_157);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_158);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_159);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_160);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_161);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_162);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_163);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_164);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_165);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_166);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_167);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_168);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_169);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_170);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_171);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_172);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_173);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_174);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_175);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_176);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_177);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_178);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_179);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_180);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_181);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_182);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_183);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_184);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_185);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_186);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_187);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_188);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_189);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_190);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_191);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_192);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_193);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_194);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_195);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_196);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_197);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_198);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_199);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_200);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_201);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_202);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_203);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_204);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_205);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_206);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_207);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_208);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_209);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_210);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_211);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_212);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_213);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_214);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_215);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_216);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_217);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_218);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_219);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_220);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_221);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_222);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_223);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_224);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_225);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_226);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_227);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_228);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_229);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_230);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_231);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_232);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_233);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_234);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_235);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_236);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_237);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_238);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_239);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_240);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_241);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_242);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_243);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_244);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_245);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_246);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_247);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_248);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_249);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_250);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_251);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_252);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_253);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_254);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_255);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_256);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_257);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_258);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_259);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_260);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_261);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_262);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_263);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_264);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_265);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_266);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_267);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_268);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_269);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_270);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_271);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_272);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_273);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_274);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_275);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_276);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_277);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_278);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_279);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_280);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_281);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_282);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_283);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_284);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_285);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_286);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_287);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_288);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_289);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_290);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_291);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_292);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_293);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_294);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_295);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_296);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_297);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_298);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_299);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_300);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_301);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_302);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_303);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_304);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_305);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_306);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_307);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_308);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_309);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_310);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_311);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_312);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_313);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_314);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_315);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_316);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_317);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_318);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_319);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_320);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_321);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_322);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_323);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_324);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_325);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_326);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_327);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_328);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_329);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_330);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_331);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_332);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_333);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_334);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_335);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_336);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_337);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_338);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_339);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_340);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_341);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_342);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_343);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_344);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_345);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_346);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_347);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_348);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_349);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_350);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_351);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_352);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_353);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_354);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_355);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_356);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_357);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_358);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_359);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_360);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_361);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_362);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_363);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_364);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_365);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_366);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_367);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_368);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_369);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_370);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_371);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_372);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_373);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_374);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_375);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_376);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_377);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_378);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_379);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_380);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_381);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_382);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_383);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_384);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_385);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_386);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_387);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_388);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_389);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_390);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_391);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_392);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_393);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_394);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_395);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_396);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_397);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_398);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_399);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_400);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_401);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_402);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_403);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_404);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_405);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_406);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_407);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_408);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_409);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_410);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_411);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_412);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_413);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_414);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_415);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_416);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_417);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_418);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_419);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_420);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_421);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_422);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_423);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_424);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_425);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_426);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_427);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_428);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_429);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_430);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_431);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_432);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_433);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_434);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_435);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_436);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_437);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_438);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_439);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_440);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_441);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_442);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_443);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_444);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_445);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_446);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_447);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_448);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_449);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_450);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_451);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_452);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_453);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_454);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_455);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_456);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_457);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_458);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_459);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_460);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_461);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_462);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_463);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_464);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_465);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_466);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_467);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_468);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_469);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_470);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_471);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_472);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_473);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_474);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_475);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_476);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_477);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_478);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_479);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_480);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_481);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_482);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_483);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_484);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_485);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_486);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_487);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_488);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_489);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_490);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_491);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_492);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_493);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_494);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_495);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_496);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_497);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_498);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_499);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_500);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_501);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_502);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_503);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_504);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_505);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_506);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_507);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_508);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_509);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_510);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_511);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_512);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_513);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_514);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_515);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_516);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_517);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_518);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_519);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_520);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_521);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_522);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_523);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_524);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_m_525);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_001);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_002);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_003);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_004);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_005);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_006);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_007);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_008);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_009);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_010);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_011);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_012);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_013);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_014);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_015);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_016);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_017);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_018);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_019);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_020);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_021);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_022);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_023);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_024);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_025);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_026);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_027);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_028);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_029);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_030);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_031);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_032);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_033);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_034);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_035);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_036);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_037);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_038);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_039);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_040);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_041);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_042);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_043);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_044);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_045);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_046);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_047);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_048);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_049);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_050);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_051);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_052);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_053);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_054);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_055);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_056);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_057);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_058);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_059);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_060);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_061);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_062);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_063);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_064);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_065);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_066);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_067);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_068);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_069);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_070);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_071);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_072);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_073);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_074);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_075);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_076);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_077);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_078);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_079);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_080);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_081);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_082);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_083);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_084);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_085);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_086);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_087);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_088);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_089);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_090);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_091);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_092);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_093);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_094);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_095);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_096);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_097);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_098);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_099);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_100);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_101);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_102);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_103);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_104);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_105);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_106);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_107);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_108);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_109);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_110);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_111);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_112);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_113);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_114);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_115);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_116);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_117);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_118);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_119);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_120);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_121);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_122);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_123);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_124);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_125);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_126);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_127);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_128);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_129);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_130);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_131);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_132);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_133);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_134);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_135);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_136);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_137);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_138);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_139);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_140);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_141);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_142);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_143);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_144);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_145);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_146);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_147);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_148);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_149);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_150);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_151);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_152);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_153);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_154);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_155);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_156);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_157);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_158);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_159);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_160);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_161);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_162);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_163);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_164);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_165);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_166);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_167);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_168);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_169);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_170);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_171);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_172);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_173);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_174);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_175);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_176);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_177);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_178);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_179);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_180);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_181);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_182);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_183);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_184);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_185);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_186);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_187);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_188);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_189);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_190);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_191);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_192);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_193);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_194);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_195);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_196);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_197);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_198);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_199);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_200);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_201);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_202);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_203);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_204);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_205);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_206);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_207);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_208);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_209);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_210);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_211);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_212);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_213);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_214);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_215);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_216);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_217);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_218);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_219);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_220);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_221);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_222);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_223);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_224);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_225);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_226);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_227);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_228);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_229);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_230);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_231);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_232);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_233);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_234);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_235);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_236);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_237);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_238);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_239);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_240);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_241);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_242);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_243);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_244);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_245);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_246);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_247);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_248);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_249);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_250);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_251);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_252);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_253);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_254);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_255);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_256);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_257);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_258);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_259);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_260);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_261);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_262);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_263);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_264);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_265);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_266);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_267);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_268);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_269);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_270);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_271);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_272);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_273);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_274);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_275);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_276);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_277);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_278);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_279);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_280);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_281);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_282);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_283);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_284);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_285);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_286);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_287);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_288);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_289);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_290);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_291);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_292);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_293);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_294);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_295);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_296);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_297);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_298);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_299);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_300);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_301);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_302);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_303);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_304);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_305);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_306);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_307);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_308);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_309);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_310);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_311);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_312);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_313);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_314);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_315);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_316);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_317);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_318);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_319);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_320);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_321);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_322);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_323);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_324);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_325);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_326);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_327);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_328);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_329);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_330);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_331);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_332);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_333);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_334);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_335);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_336);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_337);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_338);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_339);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_340);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_341);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_342);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_343);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_344);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_345);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_346);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_347);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_348);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_349);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_350);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_351);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_352);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_353);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_354);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_355);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_356);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_357);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_358);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_359);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_360);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_361);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_362);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_363);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_364);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_365);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_366);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_367);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_368);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_369);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_370);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_371);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_372);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_373);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_374);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_375);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_376);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_377);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_378);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_379);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_380);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_381);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_382);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_383);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_384);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_385);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_386);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_387);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_388);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_389);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_390);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_391);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_392);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_393);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_394);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_395);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_396);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_397);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_398);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_399);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_400);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_401);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_402);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_403);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_404);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_405);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_406);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_407);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_408);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_409);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_410);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_411);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_412);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_413);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_414);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_415);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_416);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_417);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_418);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_419);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_420);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_421);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_422);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_423);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_424);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_425);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_426);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_427);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_428);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_429);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_430);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_431);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_432);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_433);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_434);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_435);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_436);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_437);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_438);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_439);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_440);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_441);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_442);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_443);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_444);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_445);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_446);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_447);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_448);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_449);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_450);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_451);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_452);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_453);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_454);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_455);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_456);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_457);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_458);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_459);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_460);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_461);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_462);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_463);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_464);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_465);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_466);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_467);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_468);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_469);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_470);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_471);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_472);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_473);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_474);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_475);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_476);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_477);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_478);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_479);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_480);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_481);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_482);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_483);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_484);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_485);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_486);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_487);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_488);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_489);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_490);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_491);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_492);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_493);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_494);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_495);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_496);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_497);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_498);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_499);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_500);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_501);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_502);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_503);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_504);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_505);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_506);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_507);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_508);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_509);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_510);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_511);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_512);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_513);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_514);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_515);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_516);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_517);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_518);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_519);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_520);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_521);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_522);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_523);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_524);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_f_525);
  IS_COMPATIBLE_REDEF(msg_jf_phrase_max);

  /*
    JoinFestaPersonalData.h
  */
  /*  Date  */
  IS_COMPATIBLE_STRUCT_SIZE(NetAppLib::JoinFesta::JoinFestaPersonalData::Date);

  /*  CoreData  */
  IS_COMPATIBLE_STRUCT_SIZE(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_header);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_personalInfo);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_gameStatus1);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_gameStatus2);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_gameStatus3);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_gameStatus4);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_gameStatus5);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_gameStatus6);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_gameStatus7);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_gameStatus8);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_date);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_eventRecordCategory);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_eventQuestionId);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_eventSaveIndex);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_commRoute);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_isScriptListSeleted);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_eventRecordIndex);
  /*  taking the address of a bit field is not allowed  */  
//  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, m_penaltyTime);
//  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData, padding);

  /*  SubData */
  IS_COMPATIBLE_STRUCT_SIZE(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_unionPacket);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_unionPakcetType);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_relation);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_isLock);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_isOnline);
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_isAttractionAddPoint    );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_attractionState         );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_isDummyData             );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_attractionEventId       );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_attractionPokeType1     );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_attractionPokeType2     );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_attractionMonsNo        );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_isPresetData            );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, m_presetDataId            );
  IS_COMPATIBLE_STRUCT_MEMBER(NetAppLib::JoinFesta::JoinFestaPersonalData::SubData, padding                   );

  /*
    MyStatus
  */
  IS_COMPATIBLE_CC(Savedata::MyStatus::SERIALIZE_DATA_SIZE    );
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_ID);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_rom_code);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_sex);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_padding1);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_pssDataStoreId);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_nexUniqueId);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_transferdId);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_firstTransferdId);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_principalId);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_addressId);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_latitude);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_longitude);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_regionCode);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_pokeLanguageId);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_coppacsRestriction);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_coppacsSetting);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_name);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_icon);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_ballThrowType);
  IS_COMPATIBLE_CC(Savedata::MyStatus::CORE_DATA_OffsetOf_padding3);


  /*
    msg_place_name
  */
  IS_COMPATIBLE_REDEF(MAPNAME_NOTHING);
  IS_COMPATIBLE_REDEF(MAPNAME_NOTHING_01);
  IS_COMPATIBLE_REDEF(MAPNAME_NAZONOBASYO);
  IS_COMPATIBLE_REDEF(MAPNAME_NAZONOBASYO_01);
  IS_COMPATIBLE_REDEF(MAPNAME_TOOIBASYO);
  IS_COMPATIBLE_REDEF(MAPNAME_TOOIBASYO_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0601_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_S0601_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_C0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_C0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_C0102_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_C0102_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_C0103_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_C0103_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_T0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102_T0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0102_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0102_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0202_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0202_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0302_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0302_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0502_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0502_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0601_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102D0601_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102H0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102H0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102H0102_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0102H0102_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0701_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0701_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0801_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0801_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0901_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S0901_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S1001_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_S1001_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_I1801_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_I1801_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_T0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_T0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_C0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_C0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_C0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_C0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_C0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201_C0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201I1701_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201I1701_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0101_00);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0202_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0202_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0303_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0303_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201H0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0302_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0302_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0601_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201D0601_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201I0701_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201I0701_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201I1401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0201I1401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0701_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0701_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0801_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0801_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0901_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0901_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S1001_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S1001_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0601_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_S0601_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301T0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301T0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_C0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_C0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0601_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0601_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0701_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0701_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0801_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0801_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0901_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D0901_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D1001_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D1001_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D1101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D1101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301H0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301H0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D1301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301D1301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301PL0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301PL0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0601_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0601_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0701_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_S0701_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_T0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401_T0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0201_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0201_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0301_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0301_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0401_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0401_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0501_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0501_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0601_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0601_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0801_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401D0801_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401BT0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0401BT0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0501I0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0501I0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0701PH0101_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0701PH0101_02);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_C0502_01);
  IS_COMPATIBLE_REDEF(PLACENAME_Z_F0301_C0502_02);

  IS_COMPATIBLE_REDEF(msg_place_name_max);


  /*
    msg_place_name_out
  */
  IS_COMPATIBLE_REDEF(MAPNAME_SUTEKINABASYO);
  IS_COMPATIBLE_REDEF(MAPNAME_TOOIBASYO_OUT);
  IS_COMPATIBLE_REDEF(MAPNAME_EIGA);
  IS_COMPATIBLE_REDEF(MAPNAME_EIGA16);
  IS_COMPATIBLE_REDEF(MAPNAME_EIGA17);
  IS_COMPATIBLE_REDEF(MAPNAME_EIGA18);
  IS_COMPATIBLE_REDEF(MAPNAME_EIGA19);
  IS_COMPATIBLE_REDEF(MAPNAME_EIGA20);
  IS_COMPATIBLE_REDEF(MAPNAME_EIGA21);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONSENTAH);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMOANIME);
  IS_COMPATIBLE_REDEF(MAPNAME_PCTOKYO);
  IS_COMPATIBLE_REDEF(MAPNAME_PCOHSAKA);
  IS_COMPATIBLE_REDEF(MAPNAME_PCHUKUOKA);
  IS_COMPATIBLE_REDEF(MAPNAME_PCNAGOYA);
  IS_COMPATIBLE_REDEF(MAPNAME_PCSAPPORO);
  IS_COMPATIBLE_REDEF(MAPNAME_PCYOKOHAMA);
  IS_COMPATIBLE_REDEF(MAPNAME_PCTOHOKU);
  IS_COMPATIBLE_REDEF(MAPNAME_PCTOKYOBAY);
  IS_COMPATIBLE_REDEF(MAPNAME_PCHIROSHIMA);
  IS_COMPATIBLE_REDEF(MAPNAME_PCKYOTO);
  IS_COMPATIBLE_REDEF(MAPNAME_PCSKYTREETOWN);
  IS_COMPATIBLE_REDEF(MAPNAME_POKESTORE);
  IS_COMPATIBLE_REDEF(MAPNAME_WCS);
  IS_COMPATIBLE_REDEF(MAPNAME_WCS16);
  IS_COMPATIBLE_REDEF(MAPNAME_WCS17);
  IS_COMPATIBLE_REDEF(MAPNAME_WCS18);
  IS_COMPATIBLE_REDEF(MAPNAME_WCS19);
  IS_COMPATIBLE_REDEF(MAPNAME_WCS20);
  IS_COMPATIBLE_REDEF(MAPNAME_WCS21);
  IS_COMPATIBLE_REDEF(MAPNAME_WORLDS);
  IS_COMPATIBLE_REDEF(MAPNAME_WORLDS16);
  IS_COMPATIBLE_REDEF(MAPNAME_WORLDS17);
  IS_COMPATIBLE_REDEF(MAPNAME_WORLDS18);
  IS_COMPATIBLE_REDEF(MAPNAME_WORLDS19);
  IS_COMPATIBLE_REDEF(MAPNAME_WORLDS20);
  IS_COMPATIBLE_REDEF(MAPNAME_WORLDS21);
  IS_COMPATIBLE_REDEF(MAPNAME_VGE);
  IS_COMPATIBLE_REDEF(MAPNAME_VGE16);
  IS_COMPATIBLE_REDEF(MAPNAME_VGE17);
  IS_COMPATIBLE_REDEF(MAPNAME_VGE18);
  IS_COMPATIBLE_REDEF(MAPNAME_VGE19);
  IS_COMPATIBLE_REDEF(MAPNAME_VGE20);
  IS_COMPATIBLE_REDEF(MAPNAME_VGE21);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONIBENTO);
  IS_COMPATIBLE_REDEF(MAPNAME_BATORUTAIKAI);
  IS_COMPATIBLE_REDEF(MAPNAME_GEHMUIBENTO);
  IS_COMPATIBLE_REDEF(MAPNAME_DAISUKIKURABU);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONBANGUMI);
  IS_COMPATIBLE_REDEF(MAPNAME_KONSAHTO);
  IS_COMPATIBLE_REDEF(MAPNAME_ONRAINPUREZENTO);
  IS_COMPATIBLE_REDEF(MAPNAME_PGL);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONIBENTO16);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONIBENTO17);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONIBENTO18);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONIBENTO19);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONIBENTO20);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONIBENTO21);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONFYESUTA);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONFYESUTA16);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONFYESUTA17);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONFYESUTA18);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONFYESUTA19);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONFYESUTA20);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEMONFYESUTA21);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEPAHKU);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEPAHKU16);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEPAHKU17);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEPAHKU18);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEPAHKU19);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEPAHKU20);
  IS_COMPATIBLE_REDEF(MAPNAME_POKEPAHKU21);
  IS_COMPATIBLE_REDEF(MAPNAME_IBENTOKAIZYOU);
  IS_COMPATIBLE_REDEF(MAPNAME_GEEMUFURIIKU);
  IS_COMPATIBLE_REDEF(MAPNAME_SUTAJIAMU);
  IS_COMPATIBLE_REDEF(MAPNAME_VGC);
  IS_COMPATIBLE_REDEF(MAPNAME_VGC16);
  IS_COMPATIBLE_REDEF(MAPNAME_VGC17);
  IS_COMPATIBLE_REDEF(MAPNAME_VGC18);
  IS_COMPATIBLE_REDEF(MAPNAME_VGC19);
  IS_COMPATIBLE_REDEF(MAPNAME_VGC20);
  IS_COMPATIBLE_REDEF(MAPNAME_VGC21);
  IS_COMPATIBLE_REDEF(MAPNAME_VC);
  IS_COMPATIBLE_REDEF(MAPNAME_PMGO);
  IS_COMPATIBLE_REDEF(MAPNAME_PMBANK);
  IS_COMPATIBLE_REDEF(MAPNAME_PMSHOP);
  IS_COMPATIBLE_REDEF(MAPNAME_TAIKENBAN);
  IS_COMPATIBLE_REDEF(MAPNAME_PMTRAINERCLUB);

  IS_COMPATIBLE_REDEF(msg_place_name_out_max);


  /*
    msg_place_name_per
  */
  IS_COMPATIBLE_REDEF(MAPNAME_TOOKUNIIRUHITO);
  IS_COMPATIBLE_REDEF(MAPNAME_SODATEYA);
  IS_COMPATIBLE_REDEF(MAPNAME_TOREZYAHANTA);
  IS_COMPATIBLE_REDEF(MAPNAME_ONSENBAASAN);

  IS_COMPATIBLE_REDEF(msg_place_name_per_max);


  /*
    msg_place_name_spe
  */
  IS_COMPATIBLE_REDEF(MAPNAME_NAIBUTUUSINKOUKAN);
  IS_COMPATIBLE_REDEF(MAPNAME_TUUSINKOUKAN);
  IS_COMPATIBLE_REDEF(MAPNAME_KANTO);
  IS_COMPATIBLE_REDEF(MAPNAME_ZYOTO);
  IS_COMPATIBLE_REDEF(MAPNAME_HOUEN);
  IS_COMPATIBLE_REDEF(MAPNAME_SINOH);
  IS_COMPATIBLE_REDEF(MAPNAME_TOOKUHANARETATOTI);
  IS_COMPATIBLE_REDEF(MAPNAME_HUSEINAKOHDO);
  IS_COMPATIBLE_REDEF(MAPNAME_IISHU);
  IS_COMPATIBLE_REDEF(MAPNAME_KAROSU);
  IS_COMPATIBLE_REDEF(MAPNAME_3DSLINK);
  IS_COMPATIBLE_REDEF(MAPNAME_HOLOHOLO);
  IS_COMPATIBLE_REDEF(MAPNAME_VCKANTOH);
  IS_COMPATIBLE_REDEF(MAPNAME_ORAS);
  IS_COMPATIBLE_REDEF(MAPNAME_AROORA);
  IS_COMPATIBLE_REDEF(MAPNAME_POKERESORT);

  IS_COMPATIBLE_REDEF(msg_place_name_spe_max);




}


} /*  namespace VersionCompatibility  */
} /*  namespace TestForJenkins  */
