//=============================================================================
/**
 * @brief  ポケモン技覚えデータのアクセッサ
 * @file   pml_WazaOboeData.cpp
 * @author obata_toshihiro
 * @date   2010.12.20
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include <pml/include/system/pml_Library.h>

#include <pml/include/personal/pml_PersonalData.h>
#include <pml/include/personal/pml_WazaOboeData.h>
#include "pml_WazaOboeLocal.h"

namespace pml { 
  namespace personal {

    PersonalData* WazaOboeData::s_PersonalData = NULL;
    const char* WazaOboeData::s_garcPath = NULL;
    //-------------------------------------------------------------------------
    /**
     * @brief 初期化処理
     * @param heap 作業領域として使用するヒープ
     * @param garc_path 技覚えデータファイルのパス
     */
    //-------------------------------------------------------------------------
    void WazaOboeData::Initialize( gfl2::heap::HeapBase* heap, const char* garc_path )
    {
      if( s_garcPath == NULL )
      {
        s_garcPath = garc_path;
        gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
        arcReq.fileName = s_garcPath;
        arcReq.heapForFile = heap;
        arcReq.heapForReq = heap->GetLowerHandle();
        /*
          2016/08/25  Yu Muto
          GF_DEVELOP_HIO_RESOURCE!=0の場合は、
          全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
        */
#if GF_DEVELOP_HIO_RESOURCE
#else
        arcReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
        pml::Library::m_pmlFileManager->SyncArcFileOpen(arcReq);
      }

      if( s_PersonalData == NULL ) {
        s_PersonalData = GFL_NEW( heap ) PersonalData( heap );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 終了処理
     */
    //-------------------------------------------------------------------------
    void WazaOboeData::Finalize( void )
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.fileName = s_garcPath;
      pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );

      s_garcPath = NULL;
      GFL_SAFE_DELETE( s_PersonalData );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap データの確保に使用するヒープ
     */
    //-------------------------------------------------------------------------
    WazaOboeData::WazaOboeData( gfl2::heap::HeapBase* heap ) : 
      m_monsno( MONSNO_NULL ),
      m_formno( 0 ),
      m_data( NULL ),
      m_validCodeNum( 0 )
    {
      m_data = static_cast<WazaOboeTable*>(GflHeapAllocMemory( heap, sizeof(WazaOboeTable) ));
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    WazaOboeData::~WazaOboeData( void )
    {
      GflHeapFreeMemory( m_data );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief モンスターNo.を取得する
     */
    //-------------------------------------------------------------------------
    MonsNo WazaOboeData::GetMonsNo( void ) const
    {
      return m_monsno;
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief フォルムNo.を取得する
     */
    //-------------------------------------------------------------------------
    FormNo WazaOboeData::GetFormNo( void ) const
    {
      return m_formno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技覚えデータを読み込む
     * @param monsno  対象のモンスターNo.
     * @param formno  対象のフォルムNo.
     */
    //-------------------------------------------------------------------------
    void WazaOboeData::LoadData( MonsNo monsno, FormNo formno )
    {
      if( ( m_monsno != monsno ) || ( m_formno != formno ) )
      {
        u32 dataID = s_PersonalData->GetDataID( monsno, formno );

        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
        readReq.fileName = s_garcPath;
        readReq.bufSize = sizeof(WazaOboeTable);
        readReq.datId = dataID;
        readReq.pBuf = m_data;
        pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );

        m_validCodeNum = this->CountValidWazaOboeCode();
        m_monsno = monsno;
        m_formno = formno;
      }
    }
    
    /* ------------------------------------------------------------------------- */
    /**
     * @brief 技覚えデータの非同期読み込み
     *
     * @param monsno モンスターNO
     * @param formno フォルムNO
     * @param p_temp_heap スレッド用レンポラリヒープ
     *
     * @return true 非同期読み込み開始, false 非同期読み込み開始失敗
     */
    /* ------------------------------------------------------------------------- */
    bool WazaOboeData::StartAsyncRead( MonsNo monsno, FormNo formno, gfl2::heap::HeapBase * p_temp_heap )
    {
      if( ( m_monsno != monsno ) || ( m_formno != formno ) )
      {
        u32 dataID = s_PersonalData->GetDataID( monsno, formno );

        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
        readReq.fileName = s_garcPath;
        readReq.bufSize = sizeof(WazaOboeTable);
        readReq.datId = dataID;
        readReq.pBuf = m_data;
        readReq.heapForReq = p_temp_heap;

        pml::Library::m_pmlFileManager->AddArcFileLoadDataBufReq( readReq );

        m_monsno = monsno;
        m_formno = formno;
        m_validCodeNum = 0;
        return true;
      }

      return false;
    }

    /* ------------------------------------------------------------------------- */
    /**
     * @brief 技覚えデータの非同期読み込み完了待ち＆完了処理
     *
     * @return true 完了, false 未完了
     */
    /* ------------------------------------------------------------------------- */
    bool WazaOboeData::WaitAsyncReadFinish( void )
    {
      if (pml::Library::m_pmlFileManager->IsArcFileLoadDataBufFinished(m_data) )
      {
        if (m_validCodeNum == 0)
        {
          m_validCodeNum = this->CountValidWazaOboeCode();
        }
        return true;
      }
      return false;
    }

    /**
     * @brief 指定したテーブルに含まれる, 有効な技覚えコードの数を数える
     */
    u8 WazaOboeData::CountValidWazaOboeCode( void ) const
    {
      u8 valid_code_num = 0; 
      for( int i=0; i<MAX_WAZAOBOE_CODE_NUM; i++ )
      {
        if( ( m_data->codes[i].wazano == WAZAOBOE_END_CODE ) &&
            ( m_data->codes[i].level == WAZAOBOE_END_CODE ) )
        {
          break; // 終端コードを発見
        }
        valid_code_num++;
      }
      return valid_code_num;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データをコピーする
     * @param[out] buffer コピー先
     */
    //-------------------------------------------------------------------------
    void WazaOboeData::CopyData( WazaOboeData* dest ) const
    {
      gfl2::std::MemCopy( m_data, dest->m_data, sizeof(WazaOboeTable) );
      dest->m_monsno = this->m_monsno;
      dest->m_formno = this->m_formno;
      dest->m_validCodeNum = this->m_validCodeNum;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技覚えテーブルの要素数を取得する
     */
    //-------------------------------------------------------------------------
    u8 WazaOboeData::GetValidCodeNum( void ) const
    {
      return m_validCodeNum;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 覚え技の種類を取得する
     * @param data_index 習得技のインデックス
     */
    //-------------------------------------------------------------------------
    WazaOboeData::OboeWazaKind WazaOboeData::GetOboeWazaKind( u16 data_index ) const
    {
      if( m_validCodeNum <= data_index ) {
        GFL_ASSERT(0);  //@check 引数異常を補正する
        return OBOEWAZA_KIND_LEVEL;
      }

      u16 level = this->GetLevel( data_index );
      if( level == 0 ) return OBOEWAZA_KIND_EVOLVE;
      if( level == 1 ) return OBOEWAZA_KIND_BASE;
      return OBOEWAZA_KIND_LEVEL;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技を習得するレベルを取得する
     * @param data_index 習得技のインデックス
     *
     * @retval 0      対象の技が「進化習得技」の場合
     * @retval 1      対象の技が「基本習得技」の場合
     * @retval 2以上  対象の技が「レベル習得技」の場合
     */
    //-------------------------------------------------------------------------
    u16 WazaOboeData::GetLevel( u16 data_index ) const
    {
      if( m_validCodeNum <= data_index ) { // 引数が不正値
        GFL_ASSERT(0);  //@check 引数異常を補正する
        data_index = 0;
      }
      return m_data->codes[ data_index ].level;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技IDを取得する
     * @param data_index 習得技のインデックス
     */
    //-------------------------------------------------------------------------
    WazaNo WazaOboeData::GetWazaNo( u16 data_index ) const
    {
      if( m_validCodeNum <= data_index ) { // 引数が不正値
        GFL_ASSERT(0);  //@check 引数異常を補正する
        data_index = 0;
      }
      u16 wazano = m_data->codes[ data_index ].wazano;
      return static_cast<WazaNo>( wazano );
    }


  } // namespace personal
} // namespace pml
