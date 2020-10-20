//=============================================================================
/**
 * @brief  進化テーブルのローカル定義
 * @file   pml_EvolutionTableLocal.h
 * @author obata_toshihiro
 * @date   2010.01.26
 */
//=============================================================================
#ifndef __PML_EVOLUTIONTABLELOCAL_H__
#define __PML_EVOLUTIONTABLELOCAL_H__

namespace pml {
  namespace personal {


    //-------------------------------------------------------
    /**
     * @brief 1つの進化についてのデータ
     */
    //-------------------------------------------------------
    struct EvolveData 
    {
      u16	condition;       // 進化条件
      u16	param;           // 進化条件についてのパラメータ
      u16	next_monsno;     // 進化後のモンスターNo.
      u8  next_formno;     // 進化後のフォルムNo.
      u8  enable_level;    // 進化可能なレベル
    };

    // 進化経路の最大数
    static const u8 MaxEvolutionRouteNum = 8;

    // 進化後のフォルムNo.について、進化前のフォルム番号を引き継ぐことを表す値
    static const u8 NEXT_FORMNO_INHERIT = 0xff;

    //-------------------------------------------------------
    /**
     * @brief ポケモン1体分の進化データ
     */
    //-------------------------------------------------------
    struct EvolveTable 
    {
      EvolveData	data[ MaxEvolutionRouteNum ];
    };


  } // namespace personal 
} // namespace pml

#endif //__PML_EVOLUTIONTABLELOCAL_H__
