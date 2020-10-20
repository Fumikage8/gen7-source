//=============================================================================
/**
 * @file   gfl_UI_LogRecorder.cpp
 * @brief  ログデータ管理
 * @author obata_toshihiro
 * @date   2010.10.21
 */ 
//=============================================================================
#include <nn.h>
#include "gfl_Std.h"
#include "gfl_Debug.h"
#include "gfl_Macros.h"
#include "./gfl_UI_DeviceImplementer.h"
#include "./gfl_UI_LogRecorder.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param p_target  ログをとるデバイス
     */
    //-------------------------------------------------------------------------
    LogRecorder::LogRecorder( const DeviceImplementer* p_target ) :
      m_pRecTarget( p_target ),
      m_pLogBuffer( NULL ),
      m_recDataIdx( 0 ),
      m_maxDataNum( 0 ),
      m_bufAllocFlag( false )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    LogRecorder::~LogRecorder()
    {
      this->FreeAllocatedBuffer();
    }

    /**
     * @brief 確保したバッファを解放する
     */
    void LogRecorder::FreeAllocatedBuffer()
    {
      if( m_bufAllocFlag ) {
        GflHeapFreeMemory( m_pLogBuffer );
        m_pLogBuffer = NULL;
        m_maxDataNum = 0;
        m_bufAllocFlag = false;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログバッファを確保する
     *
     * @param heap         使用するヒープ
     * @param buffer_size  バッファのサイズ
     */
    //-------------------------------------------------------------------------
    void LogRecorder::AllocBuffer( gfl::heap::HeapBase* heap, u32 buffer_size )
    {
      void* buffer = GflHeapAllocMemory( heap, buffer_size );
      this->SetBuffer( buffer, buffer_size );

      m_bufAllocFlag = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログバッファをセットする ( 解放はしません )
     *
     * @param p_buffer     バッファ先頭へのポインタ
     * @param buffer_size  バッファのサイズ
     */
    //-------------------------------------------------------------------------
    void LogRecorder::SetBuffer( void* p_buffer, u32 buffer_size )
    {
      this->FreeAllocatedBuffer(); 

      m_pLogBuffer = p_buffer; 
      m_maxDataNum = buffer_size / m_pRecTarget->GetDetectiveDataSize();
      this->ClearLog();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 現在の入力を記録する
     */
    //-------------------------------------------------------------------------
    void LogRecorder::RecCurrentInput( void )
    {
      if( ( m_pLogBuffer == NULL ) ||
          ( m_maxDataNum <= m_recDataIdx ) ) {
        GFL_ASSERT(0);  //@fix
        return;
      }

      u32 data_size = m_pRecTarget->GetDetectiveDataSize();
      void* p_data = reinterpret_cast<void*>((int)m_pLogBuffer + data_size * m_recDataIdx);
      m_pRecTarget->GetDetectiveData( p_data );
      m_recDataIdx++;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 記録可能かどうかを調べる
     */
    //-------------------------------------------------------------------------
    bool LogRecorder::CanRec( void ) const
    {
      if( ( m_pLogBuffer == NULL ) || 
          ( m_maxDataNum <= m_recDataIdx ) ) {
        return false;
      }
      else {
        return true;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 記録したデータをクリアする
     */
    //-------------------------------------------------------------------------
    void LogRecorder::ClearLog( void )
    {
      ICHI_PRINT("LogRecorder::ClearLog m_recDataIdx %d \n", m_recDataIdx );
      m_recDataIdx = 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief データの記録位置を設定する
     *
     * @param recPos  データの記録位置
     *
     * @note recPos 以降のデータは失われます
     */
    //-------------------------------------------------------------------------
    void LogRecorder::SetLogRecPos( u32 recPos )
    {
      if( recPos <= m_maxDataNum ) {
        m_recDataIdx = recPos;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 記録データを取得する
     *
     * @param index     何番目のデータを取り出すか
     * @param p_buffer  取り出したデータの格納先
     */
    //-------------------------------------------------------------------------
    void LogRecorder::GetLogData( u32 index, void* p_buffer ) const
    {
      u32 data_size = m_pRecTarget->GetDetectiveDataSize();
      uptr buffer_head = reinterpret_cast<uptr>( m_pLogBuffer );
      void* p_data = reinterpret_cast<void*>( buffer_head + data_size * index );
      gfl::std::MemCopy( p_data, p_buffer, data_size );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 記録データの数を取得する
     */
    //-------------------------------------------------------------------------
    u32 LogRecorder::GetLogDataNum( void ) const
    {
      return m_recDataIdx;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief 記録データの最大数を取得する
     */
    //-----------------------------------------------------------------------
    u32 LogRecorder::GetRecordableLogDataNum( void ) const
    {
      return m_maxDataNum;
    }

    //--------------------------------------------------------------------
    /**
     *	@brief  ログバッファを取得する
     */
    //--------------------------------------------------------------------
    void* LogRecorder::GetLogBuffer( void ) const
    {
      return m_pLogBuffer;
    }

  } //namespace ui 
} //namespace gfl 
