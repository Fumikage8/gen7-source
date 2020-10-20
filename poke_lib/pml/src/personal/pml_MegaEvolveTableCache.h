//=============================================================================
/**
 * @brief  メガ進化データのキャッシュ管理
 * @file   pml_MegaEvolveTableCache.h
 * @author obata_toshihiro
 * @date   2011.01.11
 */
//=============================================================================
#ifndef __PMLMEGAEVOLVETABLECACHE_H__
#define __PMLMEGAEVOLVETABLECACHE_H__

#include <heap/include/gfl2_Heap.h>

#include <pml/include/pml_Type.h>

namespace pml { 
  namespace personal {

    class MegaEvolveTable;


    class MegaEvolveTableCache 
    {
      GFL_FORBID_COPY_AND_ASSIGN( MegaEvolveTableCache ); 

      public:
      MegaEvolveTableCache( gfl2::heap::HeapBase* heap, u32 cache_size );
      virtual ~MegaEvolveTableCache();
      bool GetTable( MonsNo monsno, MegaEvolveTable* buffer ) const;
      void RegisterTable( const MegaEvolveTable& table );

      private: 
      MegaEvolveTable** m_cacheData;
      u32 m_maxDataNum;
      u32 m_registerPos;  // 新しいデータを登録する場所

      bool SearchData( MonsNo, u32* ) const;
    };


  } // namespace personal
} // namespace pml 

#endif //__PMLMEGAEVOLVETABLECACHE_H__
