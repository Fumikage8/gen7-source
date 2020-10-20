//=============================================================================
/**
 * @brief  進化テーブルのキャッシュ管理
 * @file   pml_EvolutionTableCache.cpp
 * @author obata_toshihiro
 * @date   2011.01.11
 */
//=============================================================================
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_EvolutionTable.h>
#include "pml_EvolutionTableCache.h"


namespace pml { 
  namespace personal {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap       キャッシュの確保に使用するヒープ
     * @param cache_size キャッシュするデータの数
     */
    //-------------------------------------------------------------------------
    EvolutionTableCache::EvolutionTableCache( gfl2::heap::HeapBase* heap, u32 cache_size ) :
      m_cacheData( NULL ),
      m_maxDataNum( cache_size ),
      m_registerPos( 0 )
    {
      m_cacheData = GFL_NEW( heap ) EvolutionTable*[ cache_size ];
      for( u32 i=0; i<cache_size; i++ ) {
        m_cacheData[i] = GFL_NEW( heap ) EvolutionTable( heap );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    EvolutionTableCache::~EvolutionTableCache()
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
     * @param[in]  formno 検索キー( フォルムNo. )
     * @param[out] buffer ヒットしたデータの格納先
     * @return キャッシュにヒットしたか？
     */
    //-------------------------------------------------------------------------
    bool EvolutionTableCache::GetTable( MonsNo monsno, FormNo formno, EvolutionTable* buffer ) const
    { 
      u32 index = 0;
      bool hit = this->SearchData( monsno, formno, &index );
      if( hit ) {
        m_cacheData[ index ]->CopyTable( buffer );
      }
      return hit;
    }

    /**
     * @brief キャッシュデータを検索する
     * @param[in]  monsno 検索キー( モンスターNo. )
     * @param[in]  formno 検索キー( フォルムNo. )
     * @param[out] index  ヒットしたデータのインデックス
     * @retval true:ヒットした, false:ヒットしなかった
     */
    bool EvolutionTableCache::SearchData( MonsNo monsno, FormNo formno, u32* index ) const
    {
      for( u32 i=0; i<m_maxDataNum; i++ )
      {
        if( ( m_cacheData[i]->GetMonsNo() == monsno ) &&
            ( m_cacheData[i]->GetFormNo() == formno ) )
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
    void EvolutionTableCache::RegisterTable( const EvolutionTable& data )
    {
      MonsNo monsno = data.GetMonsNo();
      FormNo formno = data.GetFormNo();
      u32 index = 0;
      bool hit = this->SearchData( monsno, formno, &index ); 
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
