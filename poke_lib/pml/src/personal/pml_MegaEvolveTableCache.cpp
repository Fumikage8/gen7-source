//=============================================================================
/**
 * @brief  メガ進化データのキャッシュ管理
 * @file   pml_MegaEvolveTableCache.cpp
 * @author obata_toshihiro
 * @date   2012.03.27
 */
//=============================================================================
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_MegaEvolveTable.h>
#include "pml_MegaEvolveTableCache.h"


namespace pml { 
  namespace personal {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap       キャッシュの確保に使用するヒープ
     * @param cache_size キャッシュするデータの数
     */
    //-------------------------------------------------------------------------
    MegaEvolveTableCache::MegaEvolveTableCache( gfl2::heap::HeapBase* heap, u32 cache_size ) :
      m_cacheData( NULL ),
      m_maxDataNum( cache_size ),
      m_registerPos( 0 )
    {
      m_cacheData = GFL_NEW( heap ) MegaEvolveTable*[ cache_size ];
      for( u32 i=0; i<cache_size; i++ ) {
        m_cacheData[i] = GFL_NEW( heap ) MegaEvolveTable( heap );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    MegaEvolveTableCache::~MegaEvolveTableCache()
    {
      for( u32 i=0; i<m_maxDataNum; i++ ) {
        GFL_DELETE m_cacheData[i];
      }
      GFL_DELETE[] m_cacheData;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief キャッシュからデータを取得する
     * @param[in]  monsno 検索キー( モンスターNo. )
     * @param[out] buffer ヒットしたデータの格納先
     * @return キャッシュにヒットしたか？
     */
    //-------------------------------------------------------------------------
    bool MegaEvolveTableCache::GetTable( MonsNo monsno, MegaEvolveTable* buffer ) const
    { 
      u32 index = 0;
      bool hit = this->SearchData( monsno, &index );
      if( hit ) {
        m_cacheData[ index ]->CopyTable( buffer );
      }
      return hit;
    }

    /**
     * @brief キャッシュデータを検索する
     * @param[in]  monsno 検索キー( モンスターNo. )
     * @param[out] index  ヒットしたデータのインデックス
     * @retval true:ヒットした, false:ヒットしなかった
     */
    bool MegaEvolveTableCache::SearchData( MonsNo monsno, u32* index ) const
    {
      for( u32 i=0; i<m_maxDataNum; i++ )
      {
        if( m_cacheData[i]->GetMonsNo() == monsno )
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
    void MegaEvolveTableCache::RegisterTable( const MegaEvolveTable& data )
    {
      MonsNo monsno = data.GetMonsNo();
      u32 index = 0;
      bool hit = this->SearchData( monsno, &index ); 
      if( !hit )
      {
        // 登録
        data.CopyTable( m_cacheData[ m_registerPos ] );

        // 次に登録する場所を進める
        m_registerPos = ( m_registerPos + 1 ) % m_maxDataNum;
      }
    }



  } // namespace personal
} // namespace pml 
