//=============================================================================
/**
 * @brief  メガ進化データのローカル定義
 * @file   pml_MegaEvolveTableLocal.h
 * @author obata_toshihiro
 * @date   2010.01.26
 */
//=============================================================================
#ifndef __PML_MEGAEVOLVETABLELOCAL_H__
#define __PML_MEGAEVOLVETABLELOCAL_H__

namespace pml {
  namespace personal {


    //-------------------------------------------------------
    /**
     * @brief 1つの進化についてのデータ
     */
    //-------------------------------------------------------
    struct MEGA_EVOLVE_DATA 
    {
      u16 next_formno;     // 進化後のフォルムNo.
      u16	condition;       // 進化条件
      u16	param;           // 進化条件についてのパラメータ
      u16	padding;
    };

    // 進化経路の最大数
    static const u8 MAX_MEGA_EVOLVE_ROUTE_NUM = 2;

    //-------------------------------------------------------
    /**
     * @brief ポケモン1体分の進化データ
     */
    //-------------------------------------------------------
    struct MEGA_EVOLVE_TABLE 
    {
      MEGA_EVOLVE_DATA data[ MAX_MEGA_EVOLVE_ROUTE_NUM ];
    };


  } // namespace personal 
} // namespace pml

#endif //__PML_MEGAEVOLVETABLELOCAL_H__
