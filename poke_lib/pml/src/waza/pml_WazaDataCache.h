//=============================================================================
/**
 * @brief  技データのキャッシュ管理
 * @file   pml_WazaDataCache.h
 * @author obata_toshihiro
 * @date   2011.01.12
 */
//=============================================================================
#ifndef __PML_WAZADATACACHE_H__
#define __PML_WAZADATACACHE_H__
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <pml/include/pml_Type.h> 

namespace pml { 
  namespace wazadata {

    class WazaData;


    class WazaDataCache 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( WazaDataCache ); 

      public:
      WazaDataCache( gfl2::heap::HeapBase* heap, u8 cache_size );
      virtual ~WazaDataCache();
      bool GetData( WazaNo wazano, WazaData* buffer ) const;
      void RegisterData( const WazaData& data ); 

      private:
      WazaData** m_cacheData;
      u8 m_maxDataNum;
      u8 m_registerPos;  // 次にデータを登録する場所
      bool Search( WazaNo, u32* ) const;
    };


  } // namespace wazadata
} // namespace pml 

#endif //__PML_WAZADATACACHE_H__
