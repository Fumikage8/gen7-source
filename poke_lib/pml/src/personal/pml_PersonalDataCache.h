//=============================================================================
/**
 * @brief  パーソナルデータのキャッシュ管理
 * @file   pml_PersonalDataCache.h
 * @author obata_toshihiro
 * @date   2011.01.12
 */
//=============================================================================
#ifndef __PML_PERSONALDATACACHE_H__
#define __PML_PERSONALDATACACHE_H__
#include <heap/include/gfl2_Heap.h>
#include <pml/include/pml_Type.h> 

namespace pml { 
  namespace personal {

    class PersonalData;


    class PersonalDataCache 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( PersonalDataCache ); 

      public:
      PersonalDataCache( gfl2::heap::HeapBase* heap, u8 cache_size );
      virtual ~PersonalDataCache();
      bool GetData( MonsNo monsno, FormNo formno, PersonalData* buffer ) const;
      void RegisterData( const PersonalData& data ); 


      private:
      PersonalData** m_cacheData; // キャッシュデータ配列
      u8 m_maxDataNum;            // キャッシュデータの最大数
      u8 m_registerPos;           // 次にデータを登録する場所
      bool Search( MonsNo, FormNo, u32* ) const;
    };


  } // namespace personal
} // namespace pml 

#endif //__PML_PERSONALDATACACHE_H__
