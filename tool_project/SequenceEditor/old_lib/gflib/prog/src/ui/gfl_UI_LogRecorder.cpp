//=============================================================================
/**
 * @file   gfl_UI_LogRecorder.cpp
 * @brief  ���O�f�[�^�Ǘ�
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
     * @brief �R���X�g���N�^
     *
     * @param p_target  ���O���Ƃ�f�o�C�X
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
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    LogRecorder::~LogRecorder()
    {
      this->FreeAllocatedBuffer();
    }

    /**
     * @brief �m�ۂ����o�b�t�@���������
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
     * @brief ���O�o�b�t�@���m�ۂ���
     *
     * @param heap         �g�p����q�[�v
     * @param buffer_size  �o�b�t�@�̃T�C�Y
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
     * @brief ���O�o�b�t�@���Z�b�g���� ( ����͂��܂��� )
     *
     * @param p_buffer     �o�b�t�@�擪�ւ̃|�C���^
     * @param buffer_size  �o�b�t�@�̃T�C�Y
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
     * @brief ���݂̓��͂��L�^����
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
     * @brief �L�^�\���ǂ����𒲂ׂ�
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
     * @brief �L�^�����f�[�^���N���A����
     */
    //-------------------------------------------------------------------------
    void LogRecorder::ClearLog( void )
    {
      ICHI_PRINT("LogRecorder::ClearLog m_recDataIdx %d \n", m_recDataIdx );
      m_recDataIdx = 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �f�[�^�̋L�^�ʒu��ݒ肷��
     *
     * @param recPos  �f�[�^�̋L�^�ʒu
     *
     * @note recPos �ȍ~�̃f�[�^�͎����܂�
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
     * @brief �L�^�f�[�^���擾����
     *
     * @param index     ���Ԗڂ̃f�[�^�����o����
     * @param p_buffer  ���o�����f�[�^�̊i�[��
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
     * @brief �L�^�f�[�^�̐����擾����
     */
    //-------------------------------------------------------------------------
    u32 LogRecorder::GetLogDataNum( void ) const
    {
      return m_recDataIdx;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �L�^�f�[�^�̍ő吔���擾����
     */
    //-----------------------------------------------------------------------
    u32 LogRecorder::GetRecordableLogDataNum( void ) const
    {
      return m_maxDataNum;
    }

    //--------------------------------------------------------------------
    /**
     *	@brief  ���O�o�b�t�@���擾����
     */
    //--------------------------------------------------------------------
    void* LogRecorder::GetLogBuffer( void ) const
    {
      return m_pLogBuffer;
    }

  } //namespace ui 
} //namespace gfl 
