//=============================================================================
/**
 * @file gfl_UI_LogPlayer.cpp
 * @brief  ���O�f�[�^�̍Đ��Ǘ�
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
     * @brief �R���X�g���N�^
     *
     * @param p_recorder  �Đ����郍�O
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
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    LogPlayer::~LogPlayer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�f�[�^���擾����
     *
     * @param p_buffer  ���o�����f�[�^�̊i�[��
     */
    //-------------------------------------------------------------------------
    void LogPlayer::GetLogData( void* p_buffer ) const
    {
      const u32 data_index = this->GetCurrentLogDataIndex();
      m_pRecorder->GetLogData( data_index, p_buffer );
    }

    /**
     * @brief ���ݎQ�Ƃ��ׂ����O�f�[�^�̃C���f�b�N�X���擾����
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
     * @brief ���O�̍Đ��ӏ���ύX����
     */
    //-------------------------------------------------------------------------
    void LogPlayer::SetPlayPos( int pos )
    {
      ICHI_PRINT("LogPlayer::SetPlayPos %d \n", pos );
      m_playDataIdx = pos;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̍Đ��I���ʒu��ݒ肷��
     * @param endPos  �Đ��ŏI�ʒu
     */
    //-------------------------------------------------------------------------
    void LogPlayer::SetPlayEndPos( int endPos )
    {
      ICHI_PRINT("LogPlayer::SetPlayEndPos %d \n", endPos );
      m_maxPlayDataIdx = endPos;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̍Đ��ӏ���i�߂�
     */
    //-------------------------------------------------------------------------
    void LogPlayer::IncPlayPos( void )
    {
      m_playDataIdx++;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �Đ����I���������ǂ����𒲂ׂ�
     * @retval true   �Đ����I������
     * @retval false  �Đ����I�����Ă��Ȃ�
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
     *	@brief  ���O�̃o�b�t�@���擾
     */
    //--------------------------------------------------------------------
    void* LogPlayer::GetLogBuffer( void ) const
    {
      return m_pRecorder->GetLogBuffer();
    }

  } //namespace ui 
} //namespace gfl 
