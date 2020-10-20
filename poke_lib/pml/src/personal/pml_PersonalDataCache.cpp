//=============================================================================
/**
 * @brief  パーソナルデータのキャッシュ管理
 * @file   pml_PersonalDataCache.cpp
 * @author obata_toshihiro
 * @date   2011.01.11
 */
//=============================================================================
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_PersonalData.h>
#include "pml_PersonalDataCache.h"

namespace pml { 
  namespace personal { 


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap       キャッシュの確保に使用するヒープ
     * @param cache_size キャッシュするデータの数
     */
    //-------------------------------------------------------------------------
    PersonalDataCache::PersonalDataCache( gfl2::heap::HeapBase* heap, u8 cache_size ) :
      m_cacheData( NULL ),
      m_maxDataNum( cache_size ),
      m_registerPos( 0 )
    {
      m_cacheData = GFL_NEW( heap ) PersonalData*[ cache_size ];
      for( u32 i=0; i<cache_size; i++ ) {
        m_cacheData[i] = GFL_NEW( heap ) PersonalData( heap );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    PersonalDataCache::~PersonalDataCache()
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
    bool PersonalDataCache::GetData( 
        MonsNo monsno,
        FormNo formno,
        PersonalData* buffer ) const
    { 
      u32 index = 0;
      bool hit = this->Search( monsno, formno, &index );
      if( hit ) {
        m_cacheData[ index ]->CopyData( buffer );
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
    bool PersonalDataCache::Search( 
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
    void PersonalDataCache::RegisterData( const PersonalData& data )
    {
      MonsNo monsno = data.GetMonsNo();
      FormNo formno = data.GetFormNo();
      u32 index = 0;
      bool hit = this->Search( monsno, formno, &index ); 
      if( !hit )
      {
        // 登録
        data.CopyData( m_cacheData[ m_registerPos ] );

        // 次に登録する場所を進める
        m_registerPos = ( m_registerPos + 1 ) % m_maxDataNum;
      }
    }


  } // namespace personal
} // namespace pml 
