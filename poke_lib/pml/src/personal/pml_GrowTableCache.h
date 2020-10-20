//=============================================================================
/**
 * @brief  成長曲線テーブルのキャッシュ管理
 * @file   pml_GrowTableCache.h
 * @author obata_toshihiro
 * @date   2011.01.11
 */
//=============================================================================
#ifndef __PML_GROWTABLECACHE_H__
#define __PML_GROWTABLECACHE_H__

#include <heap/include/gfl2_Heap.h>
#include <pml/include/pml_Type.h>

namespace pml { 
  namespace personal {

    class GrowTable;


    class GrowTableCache 
    {
      GFL_FORBID_COPY_AND_ASSIGN( GrowTableCache ); 

      public:
      GrowTableCache( gfl2::heap::HeapBase* heap, u32 cache_size );
      virtual ~GrowTableCache();
      bool GetTable( u32 growType, GrowTable* buffer ) const;
      void RegisterTable( const GrowTable& table );

      private: 
      GrowTable** m_cacheData;
      u8 m_maxDataNum;
      u8 m_registerPos;    // 新しいデータを登録する場所
      bool SearchData( s32, u32* ) const;
    };


  } // namespace personal
} // namespace pml 

#endif //__PML_GROWTABLECACHE_H__
