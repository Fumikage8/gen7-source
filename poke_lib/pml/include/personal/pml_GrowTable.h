//=============================================================================
/**
 * @brief  ポケモンの成長曲線テーブル
 * @file   pml_GrowTable.h
 * @author obata_toshihiro
 * @date   2011.01.07
 */
//=============================================================================
#ifndef __PML_GROWTABLE_H__
#define __PML_GROWTABLE_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_PersonalConst.h> 


namespace pml { 
  namespace personal { 


    //---------------------------------------------------------------------
    /**
     * @class GrowTable
     * @brief ポケモン成長曲線テーブル
     */
    //---------------------------------------------------------------------
    class GrowTable 
    {
      GFL_FORBID_COPY_AND_ASSIGN( GrowTable ); 

      public: 
      static void Initialize( gfl2::heap::HeapBase* heap, const char* garc_path );
      static void Finalize( void );
      private:
      static const char *s_garcPath;

      public: 
      static const u8 TABLE_ELEMS = 101;   // テーブルの要素数 
      static const u32 DATASIZE = sizeof(u32) * TABLE_ELEMS; 
      GrowTable( void );
      virtual ~GrowTable();
      void Load( s32 grow_type );
      void Copy( GrowTable* dest ) const;
      s32 GetGrowType( void ) const;
      u32 GetMinExp( u16 level ) const;

      private:
      s32 m_growType;
      u32 m_data[ TABLE_ELEMS ];
    };


  } // namespace personal
} // namespace pml 

#endif //__PML_GROWTABLE_H__
