//=============================================================================
/**
 * @brief  技覚えデータのキャッシュ管理
 * @file   pml_WazaOboeDataCache.h
 * @author obata_toshihiro
 * @date   2011.01.17
 */
//=============================================================================
#ifndef __PML_WAZAOBOEDATACACHE_H__
#define __PML_WAZAOBOEDATACACHE_H__
#include <heap/include/gfl2_Heap.h>
#include <pml/include/pml_Type.h>


namespace pml { 
  namespace personal {


    class WazaOboeDataCache 
    {
      GFL_FORBID_COPY_AND_ASSIGN( WazaOboeDataCache ); 

      public:
      WazaOboeDataCache( gfl2::heap::HeapBase* heap, u32 cache_size );
      virtual ~WazaOboeDataCache();
      bool GetData( MonsNo monsno, FormNo formno, WazaOboeData* buffer ) const;
      void RegisterData( const WazaOboeData& data );

      private: 
      WazaOboeData** m_cacheData;
      u32 m_maxDataNum;
      u32 m_registerPos;  // 次にデータを登録する場所

      bool SearchData( MonsNo, FormNo, u32* ) const;
    };


  } // namespace personal
} // namespace pml 

#endif //__PML_WAZAOBOEDATACACHE_H__
