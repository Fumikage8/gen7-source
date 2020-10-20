//=============================================================================
/**
 * @brief  ポケモンパラメータ 生成システム
 * @file   pml_PokemonParamFactory.h
 * @author obata_toshihiro
 * @date   2010.12.28
 */
//=============================================================================
#ifndef __PML_POKEMONPARAM_FACTORY_H__
#define __PML_POKEMONPARAM_FACTORY_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <pml/include/pml_Type.h>
#include <pml/include/pml_Personal.h>

namespace pml {
  /**
   * @namespace pml::pokepara
   */
  namespace pokepara { 

    class PokemonParam; 
    struct CoreData;
    struct CalcData;
    struct InitialSpec;


    //-----------------------------------------------------------------------
    /**
     * @class Factory
     * @brief ポケモン生成クラス
     */
    //-----------------------------------------------------------------------
    class Factory 
    {
      GFL_FORBID_COPY_AND_ASSIGN( Factory ); 

      public: 

      //-----------------------------------------------------------------------
      // コアデータ
      //-----------------------------------------------------------------------
      static CoreData* CreateCoreData( gfl2::heap::HeapBase* heap );
      static CoreData* CreateCoreData( gfl2::heap::HeapBase* heap, MonsNo monsno, u16 level, u64 id );
      static CoreData* CreateCoreData( gfl2::heap::HeapBase* heap, const InitialSpec& spec );
      static void DeleteCoreData( CoreData* core_data ); 

      static void SetupCoreData( CoreData* coreData, const InitialSpec& spec );

      //-----------------------------------------------------------------------
      // 算出データ
      //----------------------------------------------------------------------- 
      static CalcData* CreateCalcData( gfl2::heap::HeapBase* heap, u32 personal_rnd );
      static void DeleteCalcData( CalcData* calc_data );

      //-----------------------------------------------------------------------
      // 特殊なポケモンパラメータの生成
      //-----------------------------------------------------------------------
      /**
       * @brief 「ヌケニン」を作成する
       * @param heap    ヌケニン作成に使用するヒープ
       * @param tutinin ヌケニンのベースとなる「ツチニン」or「テッカニン」
       * @return 作成したヌケニン
       */
      static PokemonParam* CreateNukenin( gfl2::heap::HeapBase* heap, PokemonParam* tutinin );


      private:

      static void FixInitSpec( InitialSpec*, const InitialSpec& );
      static Sex GetSexAtRandomInPersonalDistribution( MonsNo, FormNo );
      static CoreData* AllocCoreData( gfl2::heap::HeapBase* heap );
      static void ClearCoreData( CoreData* );
      static void InitCoreData( CoreData*, const InitialSpec& );
    };



  } // namespace pokepara
} // namespace pml



#endif  // __PML_POKEMONPARAM_FACTORY_H__
