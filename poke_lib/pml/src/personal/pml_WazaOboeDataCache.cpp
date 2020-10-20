//=============================================================================
/**
 * @brief  技覚えデータのキャッシュ管理
 * @file   pml_WazaOboeDataCache.cpp
 * @author obata_toshihiro
 * @date   2011.01.17
 */
//=============================================================================
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_WazaOboeData.h>
#include "pml_WazaOboeDataCache.h"


namespace pml { 
  namespace personal {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap       キャッシュの確保に使用するヒープ
     * @param cache_size キャッシュするデータの数
     */
    //-------------------------------------------------------------------------
    WazaOboeDataCache::WazaOboeDataCache( gfl2::heap::HeapBase* heap, u32 cache_size ) :
      m_cacheData( NULL ),
      m_maxDataNum( cache_size ),
      m_registerPos( 0 )
    {
      m_cacheData = GFL_NEW( heap ) WazaOboeData*[ cache_size ];
      for( u32 i=0; i<cache_size; i++ ) {
        m_cacheData[i] = GFL_NEW( heap ) WazaOboeData( heap );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    WazaOboeDataCache::~WazaOboeDataCache()
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
    bool WazaOboeDataCache::GetData(
        MonsNo monsno,
        FormNo formno,
        WazaOboeData* buffer ) const
    { 
      u32 index = 0; 
      bool hit = SearchData( monsno, formno, &index );
      if( hit ) {
        m_cacheData[ index ]->CopyData( buffer );
      }
      return hit;
    }

    /**
     * @brief 指定した条件に該当するデータを検索し, インデックスを取得する
     * @param[in]  monsno 検索キー( モンスターNo. )
     * @param[in]  formno 検索キー( フォルムNo. )
     * @param[out] index  ヒットしたデータのインデックス
     * @retval true:ヒットした, false:ヒットしなかった
     */
    bool WazaOboeDataCache::SearchData( 
        MonsNo monsno,
        FormNo formno,
        u32* index ) const
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
    void WazaOboeDataCache::RegisterData( const WazaOboeData& data )
    {
      MonsNo monsno = data.GetMonsNo();
      FormNo formno = data.GetFormNo();
      u32 index = 0; 
      if( SearchData( monsno, formno, &index ) == false )
      {
        // 登録
        data.CopyData( m_cacheData[ m_registerPos ] );

        // 次に登録する場所を進める
        m_registerPos = ( m_registerPos + 1 ) % m_maxDataNum;
      }
    }



  } // namespace personal
} // namespace pml 
