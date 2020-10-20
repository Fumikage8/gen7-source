//=============================================================================
/**
 * @brief  ポケモンの進化テーブルのアクセッサ
 * @file   pml_EvolutionTable.h
 * @author obata_toshihiro
 * @date   2011.01.27
 */
//=============================================================================
#ifndef __PML_EVOLUTIONTABLE_H__
#define __PML_EVOLUTIONTABLE_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_PersonalConst.h> 


namespace pml { 
  namespace personal { 

    struct EvolveTable;


    //---------------------------------------------------------------------
    /**
     * @class EvolutionTable
     * @brief ポケモン進化テーブル
     */
    //---------------------------------------------------------------------
    class EvolutionTable
    {
      GFL_FORBID_COPY_AND_ASSIGN( EvolutionTable ); 

      public: 
      static void Initialize( gfl2::heap::HeapBase* heap, const char* garc_path );
      static void Finalize( void );
      private:
      static const char* m_garcPath;

      public: 
      EvolutionTable( gfl2::heap::HeapBase* heap );
      virtual ~EvolutionTable();
      void LoadTable( MonsNo monsno, FormNo formno );
      void CopyTable( EvolutionTable* dest ) const;
      MonsNo GetMonsNo( void ) const;
      FormNo GetFormNo( void ) const;
      u8 GetEvolutionRouteNum( void ) const;                     // 進化経路の数
      EvolveCond GetEvolutionCondition( u8 route_index ) const;  // 進化の条件
      u16 GetEvolutionParam( u8 route_index ) const;             // 進化条件に関するパラメータ
      MonsNo GetEvolvedMonsNo( u8 route_index ) const;           // 進化後のモンスターNo.
      FormNo GetEvolvedFormNo( u8 route_index ) const;           // 進化後のフォルムNo.
      bool IsEvolvedFormNoSpecified( u8 route_index ) const;     // 進化後のフォルムNo.が指定されているか？
      u8 GetEvolveEnableLevel( u8 route_index ) const;           // 進化可能なレベルを取得する

      private:
      MonsNo m_monsno;
      FormNo m_formno;
      EvolveTable* m_table;
    };


  } // namespace personal
} // namespace pml 

#endif //__PML_EVOLUTIONTABLE_H__
