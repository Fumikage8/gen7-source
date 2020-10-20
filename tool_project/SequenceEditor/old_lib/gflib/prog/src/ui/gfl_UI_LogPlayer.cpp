//=============================================================================
/**
 * @file gfl_UI_LogPlayer.cpp
 * @brief  ログデータの再生管理
 * @author obata_toshihiro
 * @date 2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "gfl_Macros.h"
#include "./gfl_UI_LogRecorder.h"
#include "./gfl_UI_LogPlayer.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param p_recorder  再生するログ
     */
    //-------------------------------------------------------------------------
    LogPlayer::LogPlayer( const LogRecorder* p_recorder ) :
      m_pRecorder( p_recorder ),
      m_playDataIdx( 0 ),
      m_maxPlayDataIdx( -1 )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    LogPlayer::~LogPlayer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログデータを取得する
     *
     * @param p_buffer  取り出したデータの格納先
     */
    //-------------------------------------------------------------------------
    void LogPlayer::GetLogData( void* p_buffer ) const
    {
      const u32 data_index = this->GetCurrentLogDataIndex();
      m_pRecorder->GetLogData( data_index, p_buffer );
    }

    /**
     * @brief 現在参照すべきログデータのインデックスを取得する
     */
    u32 LogPlayer::GetCurrentLogDataIndex( void ) const
    {
      if( this->IsPlayEnd() ) {
        const u32 log_data_num = m_pRecorder->GetLogDataNum();
        return ( 0 < log_data_num ) ? ( log_data_num - 1 ) : ( 0 );
      }
      return m_playDataIdx;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの再生箇所を変更する
     */
    //-------------------------------------------------------------------------
    void LogPlayer::SetPlayPos( int pos )
    {
      ICHI_PRINT("LogPlayer::SetPlayPos %d \n", pos );
      m_playDataIdx = pos;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの再生終了位置を設定する
     * @param endPos  再生最終位置
     */
    //-------------------------------------------------------------------------
    void LogPlayer::SetPlayEndPos( int endPos )
    {
      ICHI_PRINT("LogPlayer::SetPlayEndPos %d \n", endPos );
      m_maxPlayDataIdx = endPos;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの再生箇所を進める
     */
    //-------------------------------------------------------------------------
    void LogPlayer::IncPlayPos( void )
    {
      m_playDataIdx++;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 再生が終了したかどうかを調べる
     * @retval true   再生が終了した
     * @retval false  再生が終了していない
     */
    //-------------------------------------------------------------------------
    bool LogPlayer::IsPlayEnd( void ) const
    {
      if( ( 0 <= m_maxPlayDataIdx ) && ( m_maxPlayDataIdx < m_playDataIdx ) ) {
        ICHI_PRINT("LogPlayer::IsPlayEnd a %d %d \n", m_maxPlayDataIdx, m_playDataIdx );
        return true;
      }

      const u32 log_data_num = m_pRecorder->GetLogDataNum();
      if( log_data_num <= m_playDataIdx ) {
        ICHI_PRINT("LogPlayer::IsPlayEnd b %d %d \n", log_data_num, m_playDataIdx );
        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------
    /**
     *	@brief  ログのバッファを取得
     */
    //--------------------------------------------------------------------
    void* LogPlayer::GetLogBuffer( void ) const
    {
      return m_pRecorder->GetLogBuffer();
    }

  } //namespace ui 
} //namespace gfl 
