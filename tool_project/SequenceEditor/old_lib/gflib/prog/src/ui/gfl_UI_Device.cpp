//=============================================================================
/**
 * @file gfl_UI_Device.cpp
 * @brief ���̓f�o�C�X�̒��ۃN���X
 * @author obata_toshihiro
 * @date 2010.10.22
 */
//=============================================================================
#include <nn.h>
#include "gfl_Std.h"
#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Device.h"
#include "./gfl_UI_DeviceImplementer.h"
#include "./gfl_UI_RepeatTimer.h"
#include "./gfl_UI_LogPlayer.h"
#include "./gfl_UI_LogRecorder.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap         �g�p����q�[�v
     * @param implementer  ���ۂɓ��͂̓ǂݎ����s���C���X�^���X
     */
    //-------------------------------------------------------------------------
    Device::Device( gfl::heap::HeapBase* heap, DeviceImplementer* implementer ) :
      m_pImplementer( implementer ),
      m_pRepeatTimer( NULL ),
      m_pLogPlayer( NULL ),
      m_pLogRecorder( NULL ),
      m_frameRate( FRAME_RATE_60 ),
      m_frameCount( 0 ),
      m_isRunning( false ),
      m_isSoftwareControlEnable( true ),
      m_isLogRecording( false ),
      m_isLogPlaying( false ),
      m_isLogLoopPlayEnable( false ),
      m_nowDetectiveData( NULL ),
      m_prevDetectiveData( NULL ),
      m_effectiveDataBuffer( NULL ),
      m_effectiveData( NULL ),
      m_invalidEffectiveData( NULL ),
      m_nowRawDetectiveData( NULL ),
      m_prevRawDetectiveData( NULL ),
      m_rawEffectiveDataBuffer( NULL ),
      m_rawEffectiveData( NULL )
    {
      m_pRepeatTimer = GFL_NEW( heap ) RepeatTimer( DEFAULT_REPEAT_START_WAIT, DEFAULT_REPEAT_INTERVAL );
      m_pLogRecorder = GFL_NEW( heap ) LogRecorder( implementer );
      m_pLogPlayer = GFL_NEW( heap ) LogPlayer( m_pLogRecorder );
      m_nowDetectiveData = GflHeapAllocMemory( heap, implementer->GetDetectiveDataSize() );
      m_prevDetectiveData = GflHeapAllocMemory( heap, implementer->GetDetectiveDataSize() );
      m_effectiveDataBuffer = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
      m_effectiveData = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
      m_invalidEffectiveData = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
      m_nowRawDetectiveData = GflHeapAllocMemory( heap, implementer->GetDetectiveDataSize() );
      m_prevRawDetectiveData = GflHeapAllocMemory( heap, implementer->GetDetectiveDataSize() );
      m_rawEffectiveDataBuffer = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
      m_rawEffectiveData = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    Device::~Device()
    {
      GFL_DELETE m_pImplementer;
      GFL_DELETE m_pRepeatTimer;
      GFL_DELETE m_pLogPlayer;
      GFL_DELETE m_pLogRecorder;
      GflHeapFreeMemory( m_nowDetectiveData );
      GflHeapFreeMemory( m_prevDetectiveData );
      GflHeapFreeMemory( m_effectiveDataBuffer );
      GflHeapFreeMemory( m_effectiveData );
      GflHeapFreeMemory( m_invalidEffectiveData );
      GflHeapFreeMemory( m_nowRawDetectiveData );
      GflHeapFreeMemory( m_prevRawDetectiveData );
      GflHeapFreeMemory( m_rawEffectiveDataBuffer );
      GflHeapFreeMemory( m_rawEffectiveData );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �t���[�����[�g���擾����
     * @return ���ݐݒ肳��Ă���t���[�����[�g
     */
    //-------------------------------------------------------------------------
    Device::FrameRate Device::GetFrameRate( void ) const
    {
      return m_frameRate;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t���[�����[�g��ݒ肷��
     * @param rate  �V���ɐݒ肷��t���[�����[�g
     */
    //-------------------------------------------------------------------------
    void Device::SetFrameRate( Device::FrameRate rate )
    {
      m_frameRate = rate;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ���͂̍X�V�^�C�~���O�𓯊�����
     */
    //-------------------------------------------------------------------------
    void Device::SynchronizeInputUpdate( void )
    {
      m_frameCount = 0;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ���O���L�^����o�b�t�@���m�ۂ���
     *
     * @param heap         �g�p����q�[�v
     * @param buffer_size  �o�b�t�@�̃T�C�Y
     */
    //-------------------------------------------------------------------------
    void Device::AllocLogBuffer( gfl::heap::HeapBase* heap, u32 buffer_size )
    {
      m_pLogRecorder->AllocBuffer( heap, buffer_size );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O���L�^����o�b�t�@���Z�b�g����( �o�b�t�@�̉���͍s���܂��� )
     *
     * @param p_buffer     �o�b�t�@�擪�ւ̃|�C���^
     * @param buffer_size  �o�b�t�@�̃T�C�Y
     */
    //-------------------------------------------------------------------------
    void Device::SetLogBuffer( void* p_buffer, u32 buffer_size )
    {
      m_pLogRecorder->SetBuffer( p_buffer, buffer_size );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̍̎���J�n����
     */
    //-------------------------------------------------------------------------
    void Device::StartLogRec( void )
    {
      this->StopLogPlay(); // ���O�Đ����~�߂�
      m_pLogRecorder->ClearLog();
      m_isLogRecording = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̍̎���I������
     */
    //-------------------------------------------------------------------------
    void Device::StopLogRec( void )
    {
      m_isLogRecording = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̍̎撆���ǂ����𒲂ׂ�
     * @retval true  ���O�̎撆
     * @retval false ���O�̎撆�łȂ�
     */
    //-------------------------------------------------------------------------
    bool Device::IsLogRecording( void ) const
    {
      return m_isLogRecording;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �L�^�������O�̃t���[�������擾����
     */
    //-------------------------------------------------------------------------
    u32 Device::GetRecordedLogFrameCount( void ) const
    {
      return m_pLogRecorder->GetLogDataNum();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �L�^�������O�ɂ���, �Đ��Ώۂ̃t���[������ݒ肷��
     * @param playFrameCount  �Đ��ΏۂƂ���t���[����
     */
    //-------------------------------------------------------------------------
    void Device::SetPlayableLogFrameCount( s32 playFrameCount )
    {
      const s32 end_pos = playFrameCount - 1;
      m_pLogPlayer->SetPlayEndPos( end_pos );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̍Đ����J�n����
     */
    //-------------------------------------------------------------------------
    void Device::StartLogPlay( void )
    {
      ICHI_PRINT("Device::StartLogPlay\n");
      this->StopLogRec(); // ���O�̎���~�߂�
      m_pLogPlayer->SetPlayPosHead();
      m_isLogPlaying = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̍Đ����I������
     */
    //-------------------------------------------------------------------------
    void Device::StopLogPlay( void )
    {
      ICHI_PRINT("Device::StopLogPlay\n");
      m_isLogPlaying = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̍Đ������ǂ����𒲂ׂ�
     * @retval true  ���O�Đ���
     * @retval false ���O�Đ����łȂ�
     */
    //-------------------------------------------------------------------------
    bool Device::IsLogPlaying( void ) const
    {
      return m_isLogPlaying;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̃��[�v�Đ����L�����H
     * @retval true   ���[�v�Đ�����
     * @retval false  ���[�v�Đ����Ȃ�
     */
    //-------------------------------------------------------------------------
    bool Device::IsLogLoopPlayEnable( void ) const
    {
      return m_isLogLoopPlayEnable;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���O�̃��[�v�Đ����L�����ǂ����H�ݒ肷��
     * @param isLoopEnable  true:���[�v�Đ�����, false:���[�v�Đ����Ȃ�
     *
     * @note �f�t�H���g�l�� false �ł�
     */
    //-------------------------------------------------------------------------
    void Device::SetLogLoopPlayEnable( bool isLoopEnable )
    {
      m_isLogLoopPlayEnable = isLoopEnable;
    }

    //-------------------------------------------------------------------
    /**
     *	@brief ���O�̃o�b�t�@���擾 
     */
    //--------------------------------------------------------------------
    void* Device::GetLogBuffer( void ) const
    {
      return m_pLogPlayer->GetLogBuffer();
    }

    //-------------------------------------------------------------------
    /**
     *	@brief ���O�̍ő�o�b�t�@�T�C�Y���擾 
     */
    //--------------------------------------------------------------------
    u32 Device::GetLogBufferMaxSize( void ) const
    {
      return ( m_pImplementer->GetDetectiveDataSize() * m_pLogRecorder->GetRecordableLogDataNum() );
    }

    //--------------------------------------------------------------------
    /**
     * @brief �f�[�^�̋L�^�ʒu���擾����
     *
     * @return �f�[�^�̋L�^�ʒu
     *
     */
    //--------------------------------------------------------------------
    u32 Device::GetLogRecPos( void ) const
    {
      return m_pLogRecorder->GetLogDataNum();
    }
    
    //--------------------------------------------------------------------
    /**
     * @brief �f�[�^�̋L�^�ʒu��ݒ肷��
     *
     * @param recPos  �f�[�^�̋L�^�ʒu
     *
     * @note recPos �ȍ~�̃f�[�^�͎����܂�
     */
    //--------------------------------------------------------------------
    void Device::SetLogRecPos( u32 recPos )
    {
      m_pLogRecorder->SetLogRecPos( recPos );
    }

    //--------------------------------------------------------------------
    /**
     * @brief �f�[�^�̋L�^�ő�ʒu���擾����
     *
     * @return �f�[�^�̋L�^�ő�ʒu
     *
     */
    //--------------------------------------------------------------------
    u32 Device::GetLogRecMaxPos( void ) const
    {
      return m_pLogRecorder->GetRecordableLogDataNum();
    }


    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g���͂̌��o�p�����[�^���f�t�H���g�l�Ƀ��Z�b�g����
     */
    //-------------------------------------------------------------------------
    void Device::SetDefaultRepeatParam( void )
    {
      RepeatParam default_param = { DEFAULT_REPEAT_START_WAIT, DEFAULT_REPEAT_INTERVAL };
      this->SetRepeatParam( default_param );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g���͂̌��o�p�����[�^��ݒ肷��
     * @param param �ݒ肷�錟�o�p�����[�^
     */
    //-------------------------------------------------------------------------
    void Device::SetRepeatParam( const RepeatParam& param )
    {
      m_pRepeatTimer->SetRepeatParam( param );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g���͂̌��o�p�����[�^���擾����
     * @param[out] p_param ���o�p�����[�^�̊i�[��
     */
    //-------------------------------------------------------------------------
    void Device::GetRepeatParam( RepeatParam* p_param ) const
    {
      m_pRepeatTimer->GetRepeatParam( p_param );
    } 

#if GFL_DEBUG
    //----------------------------------------------------------------------------
    /**
     *	@brief  �f�o�b�O�L�[�@�������ݒ�
     */
    //-----------------------------------------------------------------------------
    void Device::SetDebugKeyDisable( bool is_disable )
    {
      mDebugKeyDisable = is_disable;
    }
    bool Device::IsDebugKeyDisable( void ) const
    {
      return mDebugKeyDisable;
    }
#endif // GFL_DEBUG



    //-------------------------------------------------------------------------
    /**
     * @brief �f�o�C�X�X�V����
     */
    //-------------------------------------------------------------------------
    void Device::UpdateDevice( void )
    {
      m_frameCount++;
      m_pImplementer->Main(); 

      this->UpdateRawEffectiveData(); 


      this->CopyNowDetectiveData_ToPrevDetectiveData();

      if( this->IsLogPlaying() ) 
      {
        this->UpdateNowDetectiveData_ByLogPlayer();
        m_pLogPlayer->IncPlayPos();
        if( m_pLogPlayer->IsPlayEnd() ) {
          if( this->IsLogLoopPlayEnable() ) {
            this->StartLogPlay();
          }
          else {
            this->StopLogPlay();
          }
        }
      }
      else 
      {
        this->UpdateNowDetectiveData_ByImplementer();
      }

      if( this->IsLogRecording() ) 
      {
        if( m_pLogRecorder->CanRec() ) {
          m_pLogRecorder->RecCurrentInput();
        }
        else {
          this->StopLogRec();
        }
      }

      m_pRepeatTimer->Update(); 
      if( this->IsRepeatingSameInput() == false ) {
        m_pRepeatTimer->Reset();
      }

      this->UpdateEffectiveDataBufferByLastDetectiveData();
      if( this->IsOnEffectiveDataUpdateFrame() ) {
        this->CopyEffectiveDataBuffer_ToEffectiveData();
        this->ClearEffectiveDataBuffer();
      }
    }

    /**
     * @brief ���H����Ă��Ȃ��f�o�C�X���̓f�[�^���X�V����
     */
    void Device::UpdateRawEffectiveData( void )
    {
      const u32 detective_datasize = m_pImplementer->GetDetectiveDataSize();
      gfl::std::MemCopy( m_nowRawDetectiveData, m_prevRawDetectiveData, detective_datasize );
      m_pImplementer->GetDetectiveData( m_nowRawDetectiveData );
      this->StoreEffectiveData( m_rawEffectiveDataBuffer, m_nowRawDetectiveData, m_prevRawDetectiveData );
      if( this->IsOnEffectiveDataUpdateFrame() ) {
        const u32 effective_datasize = m_pImplementer->GetEffectiveDataSize();
        gfl::std::MemCopy( m_rawEffectiveDataBuffer, m_rawEffectiveData, effective_datasize );
        gfl::std::MemClear( m_rawEffectiveDataBuffer, effective_datasize );
      }
    }

    /**
     * @brief ���݂̌��o�f�[�^��, �O��̌��o�f�[�^�ɃR�s�[����
     */
    void Device::CopyNowDetectiveData_ToPrevDetectiveData( void )
    {
      u32 data_size = m_pImplementer->GetDetectiveDataSize();
      gfl::std::MemCopy( m_nowDetectiveData, m_prevDetectiveData, data_size );
    }

    /**
     * @brief ���O�̍Đ��ɂ��, ���݂̌��o�f�[�^���X�V����
     */
    void Device::UpdateNowDetectiveData_ByLogPlayer( void )
    {
      m_pLogPlayer->GetLogData( m_nowDetectiveData );
    }

    /**
     * @brief �f�o�C�X�ɂ��, ���݂̌��o�f�[�^���X�V����
     */
    void Device::UpdateNowDetectiveData_ByImplementer( void )
    {
      m_pImplementer->GetDetectiveData( m_nowDetectiveData );
    }

    /**
     * @brief ���݂̌��o�f�[�^�ƑO��̌��o�f�[�^���r��, �����f�[�^�������Ă��邩�ǂ����𒲂ׂ�
     */
    bool Device::IsRepeatingSameInput( void ) const
    {
      u32 data_size = m_pImplementer->GetDetectiveDataSize(); 
      return (gfl::std::MemComp( m_nowDetectiveData, m_prevDetectiveData, data_size) == 0); // �r�b�g�P�ʂœ����Ȃ�, ���s�[�g�Ƃ���
    } 

    /**
     * @brief �ŐV�̌��o�f�[�^�ɂ��, �����l�o�b�t�@���X�V����
     */
    void Device::UpdateEffectiveDataBufferByLastDetectiveData( void )
    {
      this->StoreEffectiveData( m_effectiveDataBuffer, m_nowDetectiveData, m_prevDetectiveData );
    }

    // �e�t���[�����[�g�ɑ΂���, ���͏�ԍX�V�Ԋu
    u8 Device::m_inputUpdateInterval[ Device::FRAME_RATE_NUM ] = 
    {
      1,  // FRAME_RATE_60
      2,  // FRAME_RATE_30
    }; 

    /**
     * @brief ���݂̃t���[���������f�[�^���X�V����^�C�~���O���ǂ����𒲂ׂ�
     */
    bool Device::IsOnEffectiveDataUpdateFrame( void ) const
    {
      u8 interval = m_inputUpdateInterval[ m_frameRate ];
      return ( m_frameCount % interval == 0 );
    }

    /**
     * @brief �����l�o�b�t�@�������f�[�^�ɃR�s�[����
     */
    void Device::CopyEffectiveDataBuffer_ToEffectiveData( void )
    {
      u32 data_size = m_pImplementer->GetEffectiveDataSize();
      gfl::std::MemCopy( m_effectiveDataBuffer, m_effectiveData, data_size );
    }

    /**
     * @brief �����l�o�b�t�@���N���A����
     */
    void Device::ClearEffectiveDataBuffer( void )
    {
      u32 data_size = m_pImplementer->GetEffectiveDataSize();
      gfl::std::MemClear( m_effectiveDataBuffer, data_size );
    }



    //-------------------------------------------------------------------------
    /**
     * @brief �f�o�C�X���ғ���Ԃɂ��邩�H
     * @retval true   �f�o�C�X�͉ғ����Ă���
     * @retval false  �f�o�C�X�͉ғ����Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool Device::IsDeviceRunning( void ) const
    {
      return m_isRunning;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�o�C�X�̉ғ���Ԃ�ύX����
     * @param isEnable  true:�f�o�C�X���ғ�����, false:�f�o�C�X�̋������~����
     */
    //-------------------------------------------------------------------------
    void Device::SetDeviceRunningEnable( bool isEnable )
    {
      m_isRunning = isEnable;
    }



    //-----------------------------------------------------------------------
    /**
     * @brief �f�o�C�X���͂ɑ΂���S�Ă̐���� On/Off ��؂�ւ���
     * @param isEnable  true:���͒l�̐����L���ɂ���, false:���͒l�̐���𖳌��ɂ���
     *
     * @note isEnable �� false ���w�肵���ꍇ, 
     *       ( �����Ȃ鑼�̃��\�b�h�̉e�����󂯂Ă��Ȃ� )�n�[�h�E�F�A���͂�Ԃ��܂�
     *
     * @note ���̃��\�b�h���琧�䂪�߂������_����L���ɂȂ�܂�
     */
    //-----------------------------------------------------------------------
    void Device::SetSoftwareControlEnable( bool isEnable )
    {
      m_isSoftwareControlEnable = isEnable;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �f�o�C�X���͂ɑ΂��鐧�䂪�L�����H
     * @retval true   ���͒l�ɑ΂��鐧�䂪�L��
     * @retval false  ���͒l�ɑ΂��鐧�䂪����
     */
    //-----------------------------------------------------------------------
    bool Device::IsSoftwareControlEnable( void ) const
    {
      return m_isSoftwareControlEnable;
    }





    /**
     * @brief �Q�Ƃ��ׂ������f�[�^���擾����
     */
    const void* Device::GetRunningEffectiveData( void ) const
    {
#if GFL_DEBUG
      if( this->IsDebugKeyDisable() ) {
        return m_invalidEffectiveData;
      }
#endif 
      if( this->IsDeviceRunning() == false ) {
        return m_invalidEffectiveData;
      }

      if( this->IsSoftwareControlEnable() == false ) {
        return m_rawEffectiveData;
      }

      return m_effectiveData;
    }




  } //namespace ui
} //namespace gfl
