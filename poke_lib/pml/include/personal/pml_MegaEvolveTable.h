//=============================================================================
/**
 * @brief  ポケモンのメガ進化データのアクセッサ
 * @file   pml_MegaEvolveTable.h
 * @author obata_toshihiro
 * @date   2011.03.27
 */
//=============================================================================
#ifndef __PML_MEGAEVOLVETABLE_H__
#define __PML_MEGAEVOLVETABLE_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_PersonalConst.h> 


namespace pml { 
  namespace personal { 

    struct MEGA_EVOLVE_TABLE;


    //---------------------------------------------------------------------
    /**
     * @class MegaEvolveTable
     * @brief ポケモン　メガ進化テーブル
     */
    //---------------------------------------------------------------------
    class MegaEvolveTable
    {
      GFL_FORBID_COPY_AND_ASSIGN( MegaEvolveTable ); 

      public: 
      static void Initialize( gfl2::heap::HeapBase* heap, const char* garc_path );
      static void Finalize( void );
      private:
      static const char *s_garcPath;

      public: 
      MegaEvolveTable( gfl2::heap::HeapBase* heap );
      virtual ~MegaEvolveTable();
      void LoadTable( MonsNo monsno );
      void CopyTable( MegaEvolveTable* dest ) const;
      MonsNo GetMonsNo( void ) const;
      u8 GetEvolutionRouteNum( void ) const;                        // 進化経路の数
      MegaEvolveCond GetEvolutionCondition( u8 route_index ) const; // 進化の条件
      u16 GetEvolutionParam( u8 route_index ) const;                // 進化条件に関するパラメータ
      FormNo GetEvolvedFormNo( u8 route_index ) const;              // 進化後のフォルムNo.

      private:
      MonsNo m_monsno;
      MEGA_EVOLVE_TABLE* m_table;
    };


  } // namespace personal
} // namespace pml 

#endif //__PML_MEGAEVOLVETABLE_H__
