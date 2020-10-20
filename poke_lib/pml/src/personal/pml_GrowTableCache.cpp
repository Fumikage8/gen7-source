//=============================================================================
/**
 * @brief  成長曲線テーブルのキャッシュ管理
 * @file   pml_GrowTableCache.cpp
 * @author obata_toshihiro
 * @date   2011.01.11
 */
//=============================================================================
#include <heap/include/gfl2_Heap.h>

#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_GrowTable.h>
#include "pml_GrowTableCache.h"

namespace pml { 
  namespace personal { 


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap       キャッシュの確保に使用するヒープ
     * @param cache_size キャッシュするテーブルの数
     */
    //-------------------------------------------------------------------------
    GrowTableCache::GrowTableCache( gfl2::heap::HeapBase* heap, u32 cache_size ) :
      m_cacheData( NULL ),
      m_maxDataNum( cache_size ),
      m_registerPos( 0 )
    {
      m_cacheData = GFL_NEW( heap ) GrowTable*[ cache_size ];
      for( u32 i=0; i<cache_size; i++ ) {
        m_cacheData[i] = GFL_NEW( heap ) GrowTable();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    GrowTableCache::~GrowTableCache()
    {
      for( u32 i=0; i<m_maxDataNum; i++ ) {
        GFL_DELETE m_cacheData[i];
      }
      GFL_DELETE[] m_cacheData;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief キャッシュからデータを取得する
     * @param[in]  grow_type  成長曲線タイプ
     * @param[out] buffer     取得したデータの格納先
     * @return キャッシュにヒットしたか？
     */
    //-------------------------------------------------------------------------
    bool GrowTableCache::GetTable( u32 grow_type, GrowTable* buffer ) const
    { 
      u32 index = 0; 
      bool hit = SearchData( grow_type, &index ); 
      if( hit ) {
        m_cacheData[ index ]->Copy( buffer );
      }
      return hit;
    }

    /**
     * @brief 指定した条件に該当するデータを検索し, インデックスを取得する
     * @param[in]  grow_type  成長曲線タイプ
     * @param[out] index      該当データのインデックスの格納先
     * @retval true  キャッシュにヒットした場合
     * @retval false キャッシュにヒットしなかった場合
     */
    bool GrowTableCache::SearchData( s32 grow_type, u32* index ) const
    {
      for( int i=0; i<m_maxDataNum; i++ )
      {
        if( m_cacheData[i]->GetGrowType() == grow_type )
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
     * @param table  登録するデータ
     */
    //-------------------------------------------------------------------------
    void GrowTableCache::RegisterTable( const GrowTable& table )
    {
      u32 index = 0; 
      u32 grow_type = table.GetGrowType();
      if( SearchData( grow_type, &index ) == false )
      {
        // 登録
        table.Copy( m_cacheData[ m_registerPos ] );

        // 次に登録する場所を進める
        m_registerPos = ( m_registerPos + 1 ) % m_maxDataNum;
      }
    }


  } // namespace personal
} // namespace pml 
