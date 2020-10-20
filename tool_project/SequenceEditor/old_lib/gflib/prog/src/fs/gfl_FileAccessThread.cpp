//========================================================================================
/**
 * @brief  ファイルへの読み書きを行うスレッド
 * @file   gfl_FileAccessThread.cpp
 * @author obata_toshihiro
 * @date   2011.04.22
 */
//========================================================================================
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_FileBase.h> 
#include "gfl_FileAccessThread.h"

namespace gfl {
  namespace fs { 


    //------------------------------------------------------------------------------------
    // コンストラクタ
    //------------------------------------------------------------------------------------
    FileAccessThread::FileAccessThread( 
        gfl::heap::HeapBase* heap, u32 stack_size, const OperationSetting& setting ) : 
      gfl::base::Thread( heap, stack_size ),
      m_setting( setting ),
      m_operated_size( 0 ),
      m_file_close_flag( false )
    {
      GFL_ASSERT( 0 < m_setting.one_time_access_size );
    } 

    //------------------------------------------------------------------------------------
    // デストラクタ
    //------------------------------------------------------------------------------------
    FileAccessThread::~FileAccessThread()
    {
    }

    // スレッド開始処理
    void FileAccessThread::Initialize( void )
    {
      // バッファNULLチェック
      if( m_setting.data_buffer == NULL ) {
        GFL_ASSERT(0);  //@check
        this->Kill();
        return;
      }

      // ファイルを開く
      gfl::fs::File* file = m_setting.file;
      if( file->IsOpen() == false ) {
        bool open_result = file->Open( m_setting.file_path, m_setting.open_mode );
        if( !open_result ) {
          this->Kill();
        }
        m_file_close_flag = true;  // 元々閉じていたなら, 終了時に閉じる
      }
      // @note 既にオープン済みのファイルが指定されている場合,
      // setting.open_modeとの整合性が取れていないと失敗する.

      m_operated_size = 0; 
    }

    // スレッドメイン処理
    void FileAccessThread::Main( void )
    {
      gfl::fs::File* file = m_setting.file;
      GFL_ASSERT( file->IsOpen() );  //@check

      switch( m_setting.open_mode ) {
      case OPEN_MODE_READ:  
        {
          u32 read_pos = m_setting.file_offset + m_operated_size;
          void* dest_buffer = gfl::std::AddAddress( m_setting.data_buffer, m_operated_size );
          file->Seek( SEEK_BASE_BEGIN, read_pos );
          m_operated_size += file->Read( dest_buffer, m_setting.one_time_access_size );  
        }
        break;
      case OPEN_MODE_WRITE_NEW:  
      case OPEN_MODE_WRITE_ADD:  
        {
          u32 write_pos = m_setting.file_offset + m_operated_size;
          void* source_buffer = gfl::std::AddAddress( m_setting.data_buffer, m_operated_size );
          file->Seek( SEEK_BASE_BEGIN, write_pos );
          m_operated_size += file->Write( source_buffer, m_setting.one_time_access_size );  
        }
        break;
      default:
        GFL_ASSERT(0);  //@fix
				this->Kill();
        break;  
      }

      if( m_setting.total_data_size <= m_operated_size ) {
				this->Kill();
      }
    }

    // スレッド終了処理
    void FileAccessThread::Finalize( void )
    {
      if( m_file_close_flag ) {
        m_setting.file->Close();
      }

      if( m_setting.callback_func ) {
        m_setting.callback_func( m_operated_size, m_setting.callback_work );
      }
    } 



#if 0		// Uncompが重いので使わない方向で。
		//====================================================================================
		//====================================================================================
		// 圧縮ファイル展開		2011/10/07 by nakahiro
		//====================================================================================
		//====================================================================================

    //------------------------------------------------------------------------------------
    // コンストラクタ
    //------------------------------------------------------------------------------------
    CompFileReadThread::CompFileReadThread( 
        gfl::heap::HeapBase* heap, u32 stack_size, const OperationSetting& setting ) : 
      gfl::base::Thread( heap, stack_size ),
      m_setting( setting ),
      m_operated_size( 0 ),
      m_file_close_flag( false ),
			comp_flag( true ),
			comp_sequence( 0 )
    {
      GFL_ASSERT( 0 < m_setting.one_time_access_size );
    } 

    //------------------------------------------------------------------------------------
    // デストラクタ
    //------------------------------------------------------------------------------------
    CompFileReadThread::~CompFileReadThread()
    {
    }

    // スレッド開始処理
    void CompFileReadThread::Initialize( void )
    {
      // ファイルを開く
      gfl::fs::File* file = m_setting.file;
      if( file->IsOpen() == false ) {
        bool open_result = file->Open( m_setting.file_path, OPEN_MODE_READ );
        if( !open_result ) {
          this->Kill();
        }
        m_file_close_flag = true;  // 元々閉じていたなら, 終了時に閉じる
      }
      // @note 既にオープン済みのファイルが指定されている場合,
      // setting.open_modeとの整合性が取れていないと失敗する.

      m_operated_size = 0; 
    }

    // スレッドメイン処理
    void CompFileReadThread::Main( void )
    {
      gfl::fs::File* file = m_setting.file;
      GFL_ASSERT( file->IsOpen() );

			switch( comp_sequence ){
			case 0:		// 圧縮データ展開領域確保
				tmpBuff = GflHeapAllocMemoryLowAlign( m_setting.heap, m_setting.total_data_size, 4 );
				comp_sequence = 1;
			case 1:		// 圧縮データ読み込み
				{
					u32 read_pos = m_setting.file_offset + m_operated_size;
					void* dest_buffer = gfl::std::AddAddress( tmpBuff, m_operated_size );
					file->Seek( SEEK_BASE_BEGIN, read_pos );
					m_operated_size += file->Read( dest_buffer, m_setting.one_time_access_size );
				}
	      if( m_setting.total_data_size <= m_operated_size ) {
					*m_setting.data_buffer = GflHeapAllocMemoryLowAlign(
																		m_setting.heap,
																		nn::cx::GetUncompressedSize( tmpBuff ),
																		m_setting.data_buffer_align );
					nn::cx::InitUncompContextLZ( &context, *m_setting.data_buffer );
					comp_sequence = 2;
				}
				break;

			case 2:		// 圧縮データ展開
				s32	ret = nn::cx::ReadUncompLZ( &context, tmpBuff, m_setting.total_data_size );
				// 終了
				if( ret == 0 ){
					GflHeapFreeMemory( tmpBuff );
					this->Kill();
				// エラー
				}else if( ret < 0 ){
					GFL_ASSERT(0);
					GflHeapFreeMemory( tmpBuff );
					this->Kill();
				}
				break;
			}
    }

    // スレッド終了処理
    void CompFileReadThread::Finalize( void )
    {
      if( m_file_close_flag ) {
        m_setting.file->Close();
      }

      if( m_setting.callback_func ) {
        m_setting.callback_func( m_operated_size, m_setting.callback_work );
      }
    } 
#endif

  } // namespace fs
} // namespace gfl
