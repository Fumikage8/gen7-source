//=============================================================================
/**
 * @file   pml_WazaOboeLocal.h
 * @brief  ポケモン技覚えデータ
 * @author obata_toshihiro
 * @date   2010.12.28
 */
//=============================================================================
#ifndef __PML_WAZAOBOELOCAL_H__
#define __PML_WAZAOBOELOCAL_H__

namespace pml { 
  namespace personal {



    // 進化習得技とは…　　進化した時点で覚える技のこと
    // 基本習得技とは…　　レベル1で覚える技のこと
    // レベル習得技とは…　それぞれ設定されたレベルになった時に覚える技のこと
    



    // 技覚えテーブルの最大要素数
    // 進化習得技4個 + 基本習得技20個 ＋ レベル習得技30個 ＋ 終端コード
    static const u8 MAX_WAZAOBOE_CODE_NUM = 55;

    // 技覚え終端コード
    // 技覚えテーブルの末尾を表す値
    static const u16 WAZAOBOE_END_CODE = 0xffff; 


    /**
     * @brief 技覚えコード
     */
    struct WazaOboeCode 
    { 
      u16 wazano;  // 覚える技No.
      u16 level;   // 覚えるレベル( 0なら進化習得技, 1なら基本習得技 )
    }; 


    /**
     * @brief 技覚えテーブル
     *
     * ポケモンが習得可能な技が、
     *   1.進化習得技
     *   2.基本習得技
     *   3.レベル習得技
     * の順番に格納されている。
     *
     * 末尾には終端コード( WAZAOBOE_END_CODE )が格納されている。
     */
    struct WazaOboeTable 
    {
      WazaOboeCode codes[ MAX_WAZAOBOE_CODE_NUM ];
    };


  } // namespace personal
} // namespace pml 

#endif //__PML_WAZAOBOELOCAL_H__
