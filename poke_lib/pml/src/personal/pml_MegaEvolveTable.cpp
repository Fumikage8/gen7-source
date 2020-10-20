//=============================================================================
/**
 * @brief  ポケモンのメガ進化データのアクセッサ
 * @file   pml_MegaEvolveTable.cpp
 * @author obata_toshihiro
 * @date   2011.03.27
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_std_string.h>

#include <pml/include/pml_Type.h>
#include <pml/include/system/pml_Library.h>
#include <pml/include/personal/pml_PersonalConst.h>
#include <pml/include/personal/pml_MegaEvolveTable.h>
#include "pml_MegaEvolveTableLocal.h"

namespace pml { 
  namespace personal { 


    const char *MegaEvolveTable::s_garcPath = NULL;
    //-------------------------------------------------------------------------
    /**
     * @brief 準備
     * @param heap      作業領域に使用するヒープ
     * @param garc_path 進化テーブルファイルのパス
     */
    //-------------------------------------------------------------------------
    void MegaEvolveTable::Initialize( 
        gfl2::heap::HeapBase* heap, const char* garc_path )
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
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 後片付け
     */
    //-------------------------------------------------------------------------
    void MegaEvolveTable::Finalize( void )
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.fileName = s_garcPath;
      pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );

      s_garcPath = NULL;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap バッファ確保に使用するヒープ
     */
    //-------------------------------------------------------------------------
    MegaEvolveTable::MegaEvolveTable( gfl2::heap::HeapBase* heap ) :
      m_monsno( MONSNO_NULL ),
      m_table( NULL )
    {
      m_table = static_cast<MEGA_EVOLVE_TABLE*>(GflHeapAllocMemory( heap, sizeof(MEGA_EVOLVE_TABLE) ));
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    MegaEvolveTable::~MegaEvolveTable()
    {
      GflHeapFreeMemory( m_table ); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief モンスターNo.を取得する
     */
    //-------------------------------------------------------------------------
    MonsNo MegaEvolveTable::GetMonsNo( void ) const
    {
      return m_monsno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化テーブルを読み込む
     * @param monsno モンスターNo.
     *
     * @param data 関連付けるデータ
     */
    //-------------------------------------------------------------------------
    void MegaEvolveTable::LoadTable( MonsNo monsno )
    {
      if( m_monsno != monsno )
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
        readReq.fileName = s_garcPath;
        readReq.bufSize = sizeof(MEGA_EVOLVE_TABLE);
        readReq.datId = monsno;
        readReq.pBuf = m_table;
        pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );

        m_monsno = monsno;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データをコピーする
     * @param[out] dest コピー先
     */
    //-------------------------------------------------------------------------
    void MegaEvolveTable::CopyTable( MegaEvolveTable* dest ) const
    {
      gfl2::std::MemCopy( m_table, dest->m_table, sizeof(MEGA_EVOLVE_TABLE) );
      dest->m_monsno = m_monsno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化経路の数を取得する
     */
    //-------------------------------------------------------------------------
    u8 MegaEvolveTable::GetEvolutionRouteNum( void ) const
    {
      int num = 0;
      for( int i=0; i<MAX_MEGA_EVOLVE_ROUTE_NUM; i++ )
      {
        if( this->GetEvolutionCondition(i) == MEGA_EVOLVE_COND_NONE ) {
          break;
        }
        num++;
      }
      return num;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化の条件を取得する
     * @param route_index  進化経路のインデックス[0, MAX_MEGA_EVOLVE_ROUTE_NUM]
     */
    //-------------------------------------------------------------------------
    MegaEvolveCond MegaEvolveTable::GetEvolutionCondition( u8 route_index ) const
    { 
      if( route_index >= MAX_MEGA_EVOLVE_ROUTE_NUM )
      {
        GFL_ASSERT( 0 );  //@fix 引数異常の場合は無効値を返す
        return MEGA_EVOLVE_COND_NONE;
      }
      u16 condition = m_table->data[ route_index ].condition;
      return static_cast<MegaEvolveCond>( condition );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 進化条件に関するパラメータを取得する
     *
     * @param[in]  route_index  進化経路のインデックス[0, MAX_MEGA_EVOLVE_ROUTE_NUM]
     *
     * @return 進化条件に関するパラメータ
     */
    //-------------------------------------------------------------------------
    u16 MegaEvolveTable::GetEvolutionParam( u8 route_index ) const
    {
      if( route_index >= this->GetEvolutionRouteNum() )
      {
        GFL_ASSERT( 0 );  //@fix 引数異常を補正する
        route_index = 0;
      }
      return m_table->data[ route_index ].param;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 進化後のフォルムNo.を取得する
     *
     * @param route_index  進化経路のインデックス[0, MAX_MEGA_EVOLVE_ROUTE_NUM]
     */
    //-------------------------------------------------------------------------
    FormNo MegaEvolveTable::GetEvolvedFormNo( u8 route_index ) const
    {
      if( route_index >= this->GetEvolutionRouteNum() )
      {
        GFL_ASSERT( 0 );  //@fix 引数異常を補正する
        route_index = 0;
      }
      u16 monsno = m_table->data[ route_index ].next_formno;
      return static_cast<MonsNo>( monsno );
    }



  } // namespace personal
} // namespace pml 
