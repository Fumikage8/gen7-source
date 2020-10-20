//=============================================================================
/**
 * @file   btl_FormChange.h
 * @date   2015/11/05 15:47:19
 * @author obata_toshihiro
 * @brief  簡単な説明
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_FORMCHANGE_H_INCLUDED
#define BTL_FORMCHANGE_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara );
class PokemonParam;
GFL_NAMESPACE_END( pokepara );
GFL_NAMESPACE_END( pml );


GFL_NAMESPACE_BEGIN( btl )

  class BTL_POKEPARAM;


  /**
   * @class FormChange
   * @brief フォルムチェンジに関する仕様を定義するクラス
   */
  class FormChange
  {
    GFL_FORBID_COPY_AND_ASSIGN( FormChange );

    // 実体化を禁止
    FormChange( void ){}
    ~FormChange(){}


  public:

    //-------------------------------------------------------------------------
    /**
     * @brief ヨワシの「むれたすがた」フォルムになる条件を満たしているか？
     * @param yowasi  ヨワシのパラメータ
     * @retval true   条件を満たしている
     * @retval false  条件を満たしていない
     */
    //-------------------------------------------------------------------------
    static bool YOWASI_IsGyogunFormEnable( const pml::pokepara::PokemonParam& yowasi );
    static bool YOWASI_IsGyogunFormEnable( const BTL_POKEPARAM& yowasi );

    //-------------------------------------------------------------------------
    /**
     * @brief メテノのパラメータに応じたフォルムを取得する
     * @param meteno  メテノのパラメータ
     */
    //-------------------------------------------------------------------------
    static FormNo METENO_CalcFormNo( const pml::pokepara::PokemonParam& meteno );
    static FormNo METENO_CalcFormNo( const BTL_POKEPARAM& meteno );





  private:

    static bool YOWASI_IsGyogunFormEnable( u32 level, u32 maxHp, u32 hp );
    static FormNo METENO_CalcFormNo( u32 maxHp, u32 currentHp, FormNo currentForm );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_FORMCHANGE_H_INCLUDED