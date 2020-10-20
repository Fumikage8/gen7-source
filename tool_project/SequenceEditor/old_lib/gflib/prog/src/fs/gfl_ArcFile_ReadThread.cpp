//=============================================================================
/**
 * @brief アーカイブ読み込みスレッド
 * @file gfl_ArcFile_ReadThread.cpp
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#include <base/gfl_Thread.h>
#include "gfl_ArcFile_ReadThread.h"

namespace gfl {
  namespace fs {


    // スレッドのスタックサイズ
    const u32 ArcFileReadThread::STACK_SIZE = 4096;


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap_for_thread スレッドの動作に使用するヒープ
     * @param desc            アーカイブ読み込みの詳細情報
     */
    //-------------------------------------------------------------------------
    ArcFileReadThread::ArcFileReadThread( 
        gfl::heap::HeapBase* heap_for_thread,
        const Description& desc ) :
      base::Thread( heap_for_thread, STACK_SIZE ),
      m_desc( desc )
    {
      GFL_ASSERT_MSG( m_desc.dest_buf, "読み込んだデータの格納先を指定してください\n" );  //@check
      GFL_ASSERT_MSG( m_desc.arcfile, "読み込み対象のアーカイブを指定してください\n" );  //@check
      GFL_ASSERT_MSG( m_desc.heap_for_buf, "バッファ確保のためのヒープを指定してください\n" );  //@check
      GFL_ASSERT_MSG( m_desc.heap_for_arc, "アーカイブファイルの高速化テーブルのためのヒープを指定してください\n" ); //@check
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ArcFileReadThread::~ArcFileReadThread()
    {
    }



    /**
     * @brief メイン処理
     * @note gfl::base::Thread をオーバーライド
     */
    void ArcFileReadThread::Main( void )
    {
      ArcFile* arcfile = m_desc.arcfile;

      // ファイルを開く
      if( arcfile->IsOpen() == false ) {
        arcfile->Open();
      }

      // ファイルオープンを待つ
      nn::os::LightEvent* open_event = arcfile->GetOpenEvent();
      if( !open_event->IsSignaled() ) {
        open_event->Wait();
      }

      // HIO 読み込みでなければ, 高速モードへ移行
      if( arcfile->IsHioFile() == false )
      {
        if( arcfile->IsFastMode() == false ) {
          arcfile->SetupForFastMode( m_desc.heap_for_arc );
        }

        // 高速モードへの移行を待つ
        nn::os::LightEvent* fast_event = arcfile->GetFastSetupEvent();
        if( !fast_event->IsSignaled() ) {
          fast_event->Wait();
        }
      }

      // 読み込み
      void* data = NULL;
      u32 data_size = 0; 
      if( m_desc.compressed ) 
      {
        // 圧縮データ
        gfl::heap::HeapBase* heap_for_uncomp = ( m_desc.heap_for_uncomp == NULL ) ? m_desc.heap_for_buf : m_desc.heap_for_uncomp;
        data = arcfile->LoadCompressedDataAlloc( m_desc.datID, heap_for_uncomp, m_desc.heap_for_buf, m_desc.buf_alignment, &data_size );
      }
      else 
      {
        // 非圧縮データ
        data_size = arcfile->GetDataSize( m_desc.datID );
        if( 0 < data_size ) {
          data = GflHeapAllocMemoryAlign( m_desc.heap_for_buf, data_size, m_desc.buf_alignment );
          arcfile->LoadData( m_desc.datID, data );
        }
      }

      // 読み込んだデータを返す
      *( m_desc.dest_buf ) = data;
      if( m_desc.dest_size ) {
        *( m_desc.dest_size ) = data_size;
      }

      // 自身のスレッドをKILL
      this->Kill();
    }



  }  // namespace fs
}  // namespace gfl
