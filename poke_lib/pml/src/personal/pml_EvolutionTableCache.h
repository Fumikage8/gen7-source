//=============================================================================
/**
 * @brief  進化テーブルのキャッシュ管理
 * @file   pml_EvolutionTableCache.h
 * @author obata_toshihiro
 * @date   2011.01.11
 */
//=============================================================================
#ifndef __PMLEVOLUTIONTABLECACHE_H__
#define __PMLEVOLUTIONTABLECACHE_H__

#include <heap/include/gfl2_Heap.h>

#include <pml/include/pml_Type.h>

namespace pml { 
  namespace personal {

    class EvolutionTable;


    class EvolutionTableCache 
    {
      GFL_FORBID_COPY_AND_ASSIGN( EvolutionTableCache ); 

      public:
      EvolutionTableCache( gfl2::heap::HeapBase* heap, u32 cache_size );
      virtual ~EvolutionTableCache();
      bool GetTable( MonsNo monsno, FormNo formno, EvolutionTable* buffer ) const;
      void RegisterTable( const EvolutionTable& table );

      private: 
      EvolutionTable** m_cacheData;
      u32 m_maxDataNum;
      u32 m_registerPos;  // 新しいデータを登録する場所

      bool SearchData( MonsNo, FormNo, u32* ) const;
    };


  } // namespace personal
} // namespace pml 

#endif //__PMLEVOLUTIONTABLECACHE_H__
