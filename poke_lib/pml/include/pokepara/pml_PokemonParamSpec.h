//=============================================================================
/**
 * @brief  ポケモンパラメータの生成パラメータ
 * @file   pml_PokemonParamSpec.h
 * @author obata_toshihiro
 * @date   2011.01.20
 */
//=============================================================================
#ifndef __PML_POKEMONPARAMSPEC_H__
#define __PML_POKEMONPARAMSPEC_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <niji_conv_header/poke_lib/monsno_def.h>

#include "pml/include/pml_Type.h"
#include "pml/include/pml_Const.h"
#include "pml/include/pokepara/pml_PokemonParamType.h"


namespace pml {
  ///@namespace pml::pokepara
  namespace pokepara {

    
    enum {
      INIT_SPEC_SEX_BOTH            = 0xff,                ///< ♂♀どちらでも可
      INIT_SPEC_FORM_RANDOM         = 0xff,                ///< フォルムをランダムにする
      INIT_SPEC_TOKUSEI_1_OR_2      = 0xff,                ///< 特性1,特性2のどちらかにする
      INIT_SPEC_TOKUSEI_1_OR_2_OR_3 = 0xfe,                ///< 特性1,特性2,特性3のどちらかにする
      INIT_SPEC_TALENT_RANDOM       = 0xffff,              ///< 個体値を乱数で決定する
      INIT_SPEC_TALENT_V_NUM_NONE   = 0x0,                 ///< 個体値のランダムV指定なしの時の値
      INIT_SPEC_SEIKAKU_RANDOM      = 0xffff,              ///< 性格を乱数で決定する
      INIT_SPEC_FAMILIARITY_DEFAULT = 0xffff,              ///< なつき度を種族ごとの初期値にする
      INIT_SPEC_RARE_TRY_COUNT_DEFAULT = 0x01,             ///< レアになるまでレア乱数を引き直す回数
    };
    static const u64 INIT_SPEC_ID_RANDOM           = 0xffffffffffffffff;  ///< IDNo.を乱数で決定する
    static const u64 INIT_SPEC_PERSONALRND_RANDOM  = 0xffffffffffffffff;  ///< 個性乱数を乱数で決定する
    static const u64 INIT_SPEC_RARE_FALSE          = 0x00000001ffffffff;  ///< レアにしない
    static const u64 INIT_SPEC_RARE_TRUE           = 0x00000002ffffffff;  ///< レアにする
    static const u64 INIT_SPEC_RARE_RANDOM         = 0x00000003ffffffff;  ///< どちらでも可

    /**
     * @brief ポケパラ初期パラメータ
     *
     * この構造体にポケモンのパラメータを設定し
     * pml::pokepara::Factory に渡すことで, ポケモンパラメータを生成します.
     *
     * InitialSpec インスタンス生成直後は
     * 全てのパラメータがランダムで決定されるような指定になっています.
     * 固定したいパラメータのみを変更してください.
     */
    struct InitialSpec 
    {
      u64 personal_rnd;             ///< 個性乱数
      u64 rare_rnd;                 ///< レア乱数
      u64 id;                       ///< IDNo.  
      MonsNo monsno;                ///< モンスターNo.
      FormNo formno;                ///< フォルムNo.
      u16 level;                    ///< レベル
      u16 sex;                      ///< 性別
      u16 seikaku;                  ///< 性格
      u8  tokusei_index;            ///< 特性
      u8  rare_try_count;           ///< rare_rndがINIT_SPEC_RARE_RANDOMの時、レアになるまで乱数を引き直す回数
      u16 talentPower[ POWER_NUM ]; ///< 個体値
      u32 familiarity;              ///< なつき度
      u8  talentVNum;               ///< 個体値をV=31にする個数の指定 0以上ならtalentPower[x]の個別指定は無視して、talentVNum分をランダムでVにする

      InitialSpec( void ) :
        personal_rnd( INIT_SPEC_PERSONALRND_RANDOM ),
        rare_rnd( INIT_SPEC_RARE_RANDOM ),
        id( INIT_SPEC_ID_RANDOM ),
        monsno( MONSNO_TAMAGO ),
        formno( 0 ),
        level( MIN_POKE_LEVEL ),
        sex( INIT_SPEC_SEX_BOTH ),
        seikaku( INIT_SPEC_SEIKAKU_RANDOM ),
        tokusei_index(  INIT_SPEC_TOKUSEI_1_OR_2 ),
        rare_try_count( INIT_SPEC_RARE_TRY_COUNT_DEFAULT ),
        familiarity( INIT_SPEC_FAMILIARITY_DEFAULT ),
        talentVNum( INIT_SPEC_TALENT_V_NUM_NONE )
      {
        for( u32 i=0; i<POWER_NUM; i++ ) {
          talentPower[i] = INIT_SPEC_TALENT_RANDOM;
        }
      }
    };



  } // namespace pokepara
} // namespace pml


#endif //__PML_POKEMONPARAMSPEC_H__
