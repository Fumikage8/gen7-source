//=============================================================================
/**
 * @brief  ポケモンの進化テーブルのアクセッサ
 * @file   pml_EvolutionTable.cpp
 * @author obata_toshihiro
 * @date   2011.01.27
 */
//=============================================================================
#include <heap/include/gfl2_Heap.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_Assert.h>

#include <pml/include/system/pml_Library.h>
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_PersonalConst.h>
#include <pml/include/personal/pml_EvolutionTable.h>
#include "pml_EvolutionTableLocal.h"

namespace pml { 
  namespace personal { 


    const char* EvolutionTable::m_garcPath = NULL;
    //-------------------------------------------------------------------------
    /**
     * @brief 準備
     * @param heap      作業領域に使用するヒープ
     * @param garc_path 進化テーブルファイルのパス
     */
    //-------------------------------------------------------------------------
    void EvolutionTable::Initialize( 
        gfl2::heap::HeapBase* heap, const char* garc_path )
    {
      if( m_garcPath == NULL )
      {
        m_garcPath = garc_path;

        gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
        arcReq.fileName = m_garcPath;
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
    void EvolutionTable::Finalize( void )
    {
      if( m_garcPath )
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.fileName = m_garcPath;
        pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );
        m_garcPath = NULL;
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap バッファ確保に使用するヒープ
     */
    //-------------------------------------------------------------------------
    EvolutionTable::EvolutionTable( gfl2::heap::HeapBase* heap ) :
      m_monsno( MONSNO_NULL ),
      m_formno( 0 ),
      m_table( NULL )
    {
      m_table = static_cast<EvolveTable*>(GflHeapAllocMemory( heap, sizeof(EvolveTable) ));
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    EvolutionTable::~EvolutionTable()
    {
      GflHeapFreeMemory( m_table ); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief モンスターNo.を取得する
     */
    //-------------------------------------------------------------------------
    MonsNo EvolutionTable::GetMonsNo( void ) const
    {
      return m_monsno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief フォルムNo.を取得する
     */
    //-------------------------------------------------------------------------
    FormNo EvolutionTable::GetFormNo( void ) const
    {
      return m_formno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化テーブルを読み込む
     * @param monsno モンスターNo.
     * @param formno フォルムNo.
     */
    //-------------------------------------------------------------------------
    void EvolutionTable::LoadTable( MonsNo monsno, FormNo formno )
    {
      if( ( m_monsno != monsno ) ||
          ( m_formno != formno ) )
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
        readReq.fileName = m_garcPath;
        readReq.bufSize = sizeof(EvolveTable);
        readReq.datId = PersonalData::GetDataID( monsno, formno );
        readReq.pBuf = m_table;
        pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );

        m_monsno = monsno;
        m_formno = formno;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データをコピーする
     * @param[out] dest コピー先
     */
    //-------------------------------------------------------------------------
    void EvolutionTable::CopyTable( EvolutionTable* dest ) const
    {
      gfl2::std::MemCopy( m_table, dest->m_table, sizeof(EvolveTable) );
      dest->m_monsno = m_monsno;
      dest->m_formno = m_formno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化経路の数を取得する
     */
    //-------------------------------------------------------------------------
    u8 EvolutionTable::GetEvolutionRouteNum( void ) const
    {
      int num = 0;
      for( int i=0; i<MaxEvolutionRouteNum; i++ )
      {
        if( this->GetEvolutionCondition(i) == EVOLVE_COND_NONE ) {
          break;
        }
        num++;
      }
      return num;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化の条件を取得する
     * @param route_index  進化経路のインデックス[0, MaxEvolutionRouteNum]
     */
    //-------------------------------------------------------------------------
    EvolveCond EvolutionTable::GetEvolutionCondition( u8 route_index ) const
    { 
      if( route_index >= MaxEvolutionRouteNum )
      {
        GFL_ASSERT( 0 );  //@fix 引数異常の場合は無効値を返す
        return EVOLVE_COND_NONE;
      }
      u16 condition = m_table->data[ route_index ].condition;
      return static_cast<EvolveCond>( condition );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 進化条件に関するパラメータを取得する
     *
     * @param route_index  進化経路のインデックス[0, MaxEvolutionRouteNum]
     */
    //-------------------------------------------------------------------------
    u16 EvolutionTable::GetEvolutionParam( u8 route_index ) const
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
     * @brief 進化後のモンスターNo.を取得する
     *
     * @param route_index  進化経路のインデックス[0, MaxEvolutionRouteNum]
     */
    //-------------------------------------------------------------------------
    MonsNo EvolutionTable::GetEvolvedMonsNo( u8 route_index ) const
    {
      if( route_index >= this->GetEvolutionRouteNum() )
      {
        GFL_ASSERT( 0 );  //@fix 引数異常を補正する
        route_index = 0;
      }
      u16 monsno = m_table->data[ route_index ].next_monsno;
      return static_cast<MonsNo>( monsno );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化後のフォルムNo.を取得する
     *
     * @param route_index  進化経路のインデックス[0, MaxEvolutionRouteNum]
     */
    //-------------------------------------------------------------------------
    FormNo EvolutionTable::GetEvolvedFormNo( u8 route_index ) const
    {
      if( route_index >= this->GetEvolutionRouteNum() )
      {
        GFL_ASSERT( 0 );  //@fix 引数異常を補正する
        route_index = 0;
      }
      u16 formno = m_table->data[ route_index ].next_formno;
      return static_cast<FormNo>( formno );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化後のフォルムNo.が指定されているか？
     *
     * @param route_index  進化経路のインデックス[0, MaxEvolutionRouteNum]
     *
     * @retval true    指定されている( EvolutionTable::GetEvolvedFormNo で取得したフォルムNo.にしてください )
     * @retval false   指定されていない( 進化前のフォルムNo.を引きついてください )
     */
    //-------------------------------------------------------------------------
    bool EvolutionTable::IsEvolvedFormNoSpecified( u8 route_index ) const
    {
      FormNo formno = this->GetEvolvedFormNo( route_index );
      return ( formno != NEXT_FORMNO_INHERIT );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化可能なレベルを取得する
     *
     * @param route_index  進化経路のインデックス[0, MaxEvolutionRouteNum]
     *
     * @retval [ 0, MAX_POKE_LEVEL ]
     */
    //-------------------------------------------------------------------------
    u8 EvolutionTable::GetEvolveEnableLevel( u8 route_index ) const
    {
      if( route_index >= this->GetEvolutionRouteNum() )
      {
        GFL_ASSERT( 0 );  //@fix 引数異常を補正する
        route_index = 0;
      }
      return m_table->data[ route_index ].enable_level;
    }


  } // namespace personal
} // namespace pml 
