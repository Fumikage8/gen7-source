//=============================================================================
/**
 * @brief  ポケモンの成長曲線テーブルのアクセッサ
 * @file   pml_GrowTable.cpp
 * @author obata_toshihiro
 * @date   2011.01.07
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_std_string.h>

#include <pml/include/pml_Const.h>
#include <pml/include/system/pml_Library.h>
#include <pml/include/personal/pml_PersonalConst.h>
#include <pml/include/personal/pml_GrowTable.h>

namespace pml { 
  namespace personal { 


    const char *GrowTable::s_garcPath = NULL;
    //-------------------------------------------------------------------------
    /**
     * @brief 準備
     * @param heap      作業領域に使用するヒープ
     * @param garc_path 成長曲線テーブルファイルのパス
     */
    //-------------------------------------------------------------------------
    void GrowTable::Initialize( 
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
        //m_arcfile->SetupForFastMode( heap );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 後片付け
     */
    //-------------------------------------------------------------------------
    void GrowTable::Finalize( void )
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.fileName = s_garcPath;
      pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );

      s_garcPath = NULL;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    GrowTable::GrowTable( void ) :
      m_growType( -1 )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    GrowTable::~GrowTable()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 成長曲線テーブルを読み込む
     * @param grow_type 成長タイプ
     */
    //-------------------------------------------------------------------------
    void GrowTable::Load( s32 grow_type )
    {
      if( m_growType != grow_type ) 
      {
        const size_t bufSize = TABLE_ELEMS * sizeof(u32);

        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
        readReq.fileName = s_garcPath;
        readReq.bufSize = bufSize;
        readReq.datId = grow_type;
        readReq.pBuf = m_data;
        pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );

        m_growType = grow_type;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief テーブルをコピーする
     */
    //-------------------------------------------------------------------------
    void GrowTable::Copy( GrowTable* dest ) const
    {
      gfl2::std::MemCopy( m_data, dest->m_data, DATASIZE );
      dest->m_growType = m_growType;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 成長タイプを取得する
     */
    //-------------------------------------------------------------------------
    s32 GrowTable::GetGrowType( void ) const
    {
      return m_growType;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したレベルになるための最小経験値を取得する
     * @param level ポケモンのレベル
     */
    //-------------------------------------------------------------------------
    u32 GrowTable::GetMinExp( u16 level ) const
    {
      if( pml::MAX_POKE_LEVEL < level ) {
        GFL_ASSERT(0);  //@check 引数異常を補正する
        level = pml::MAX_POKE_LEVEL;
      } 
      return m_data[ level ];
    } 


  } // namespace personal
} // namespace pml 
