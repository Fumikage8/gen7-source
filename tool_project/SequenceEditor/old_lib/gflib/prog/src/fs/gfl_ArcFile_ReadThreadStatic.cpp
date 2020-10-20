//=============================================================================
/**
 * @brief アーカイブ読み込みスレッド
 * @file gfl_ArcFile_ReadThread.cpp
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#include <base/gfl_Thread.h>
#include "gfl_ArcFile_ReadThreadStatic.h"

namespace gfl {
  namespace fs {


    // スレッドのスタックサイズ
    const u32 ArcFileReadThreadStatic::STACK_SIZE = 4096;


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap_for_thread スレッドの動作に使用するヒープ
     * @param desc            アーカイブ読み込みの詳細情報
     */
    //-------------------------------------------------------------------------
    ArcFileReadThreadStatic::ArcFileReadThreadStatic( 
        gfl::heap::HeapBase* heap_for_thread,
        const Description& desc ) :
      base::Thread( heap_for_thread, STACK_SIZE ),
      m_desc( desc )
    {
      GFL_ASSERT_MSG( m_desc.dest_buf, "読み込んだデータの格納先を指定してください\n" );  //@check
      GFL_ASSERT_MSG( m_desc.arcfile, "読み込み対象のアーカイブを指定してください\n" );  //@check
      GFL_ASSERT_MSG( m_desc.heap_for_arc, "アーカイブファイルの高速化テーブルのためのヒープを指定してください\n" ); //@check
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ArcFileReadThreadStatic::~ArcFileReadThreadStatic()
    {
    }



    /**
     * @brief メイン処理
     * @note gfl::base::Thread をオーバーライド
     */
    void ArcFileReadThreadStatic::Main( void )
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

      // 高速モードへ移行
      if( arcfile->IsFastMode() == false ) {
        arcfile->SetupForFastMode( m_desc.heap_for_arc );
      }

      // 高速モードへの移行を待つ
      nn::os::LightEvent* fast_event = arcfile->GetFastSetupEvent();
      if( !fast_event->IsSignaled() ) {
        fast_event->Wait();
      }

      // 読み込み
			size_t readSize = 0;
      if( m_desc.compressed ) 
      {
        arcfile->LoadCompressedData
					(m_desc.datID, m_desc.heap_for_uncomp, m_desc.dest_buf, m_desc.dest_size, &readSize);
      }
      else 
      {
        // 非圧縮データ
        readSize = arcfile->GetDataSize( m_desc.datID );
        if( readSize <= m_desc.dest_size ) {
          arcfile->LoadData( m_desc.datID, m_desc.dest_buf );
        }
      }
      if( m_desc.read_size != NULL ){
        *m_desc.read_size = readSize;
      }

      // 自身のスレッドをKILL
      this->Kill();
    }



  }  // namespace fs
}  // namespace gfl
