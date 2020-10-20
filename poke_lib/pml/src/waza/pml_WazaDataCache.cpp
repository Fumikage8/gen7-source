//=============================================================================
/**
 * @brief  技データのキャッシュ管理
 * @file   pml_WazaDataCache.cpp
 * @author obata_toshihiro
 * @date   2011.01.11
 */
//=============================================================================
#include <pml/include/pml_Type.h>
#include <pml/include/waza/pml_WazaData.h>
#include "pml_WazaDataCache.h"

namespace pml { 
  namespace wazadata { 


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap       キャッシュの確保に使用するヒープ
     * @param cache_size キャッシュするデータの数
     */
    //-------------------------------------------------------------------------
    WazaDataCache::WazaDataCache( gfl2::heap::HeapBase* heap, u8 cache_size ) :
      m_cacheData( NULL ),
      m_maxDataNum( cache_size ),
      m_registerPos( 0 )
    {
      m_cacheData = GFL_NEW( heap ) WazaData*[ cache_size ];
      for( u32 i=0; i<cache_size; i++ ) {
        m_cacheData[i] = GFL_NEW( heap ) WazaData( heap );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    WazaDataCache::~WazaDataCache()
    {
      for( u32 i=0; i<m_maxDataNum; i++ ) {
        GFL_DELETE m_cacheData[i];
      }
      GFL_DELETE[] m_cacheData;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief キャッシュからデータを取得する
     * @param[in]  wazano 検索キー( 技No. )
     * @param[out] buffer ヒットしたデータの格納先
     * @return キャッシュにヒットしたか？
     */
    //-------------------------------------------------------------------------
    bool WazaDataCache::GetData( WazaNo wazano, WazaData* buffer ) const
    { 
      u32 index = 0;
      bool hit = this->Search( wazano, &index );
      if( hit ) {
        m_cacheData[ index ]->CopyData( buffer );
      }
      return hit;
    }

    /**
     * @brief キャッシュデータを検索する
     * @param[in]  wazano 検索キー( 技No. )
     * @param[out] index  ヒットしたデータのインデックス
     * @retval true:ヒットした, false:ヒットしなかった
     */
    bool WazaDataCache::Search( WazaNo wazano, u32* index ) const
    {
      for( u32 i=0; i<m_maxDataNum; i++ )
      {
        if( m_cacheData[i]->GetWazaNo() == wazano ) 
        {
          *index = i;
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したデータをキャッシュに登録する
     * @param data   登録するデータ
     */
    //-------------------------------------------------------------------------
    void WazaDataCache::RegisterData( const WazaData& data )
    {
      WazaNo wazano = data.GetWazaNo();
      u32 index = 0;
      bool hit = this->Search( wazano, &index ); 
      if( !hit )
      {
        // 登録
        data.CopyData( m_cacheData[ m_registerPos ] );

        // 次に登録する場所を進める
        m_registerPos = ( m_registerPos + 1 ) % m_maxDataNum;
      }
    }


  } // namespace wazadata
} // namespace pml 
