//=============================================================================
/**
 * @file   gfl_UI_Device.h
 * @brief  ���̓f�o�C�X�̒��ۃN���X
 * @author obata_toshihiro
 * @date   2010.10.22
 */
//=============================================================================
#ifndef __GFL_UI_DEVICE_H__
#define __GFL_UI_DEVICE_H__
#pragma once

#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "ui/gfl_UI_Types.h"

namespace gfl {
  namespace ui {


    class DeviceImplementer;
    class RepeatTimer;
    class LogPlayer;
    class LogRecorder; 


    class Device 
    {
      GFL_FORBID_COPY_AND_ASSIGN( Device );


      public:

      // ����t���[�����[�g
      enum FrameRate {
        FRAME_RATE_60,
        FRAME_RATE_30,
        FRAME_RATE_NUM,
      };

      //-----------------------------------------------------------------------
      /**
       * @brief �t���[�����[�g���擾����
       * @return ���ݐݒ肳��Ă���t���[�����[�g
       */
      //-----------------------------------------------------------------------
      FrameRate GetFrameRate( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �t���[�����[�g��ݒ肷��
       * @param rate  �V���ɐݒ肷��t���[�����[�g
       */
      //-----------------------------------------------------------------------
      void SetFrameRate( FrameRate rate ); 

      //-----------------------------------------------------------------------
      /**
       * @brief ���͂̍X�V�^�C�~���O�𓯊�����
       */
      //-----------------------------------------------------------------------
      void SynchronizeInputUpdate( void );




      //-----------------------------------------------------------------------
      /**
       * @brief ���O���L�^����o�b�t�@���m�ۂ���
       *
       * @param heap         �g�p����q�[�v
       * @param buffer_size  �o�b�t�@�̃T�C�Y
       */
      //-----------------------------------------------------------------------
      void AllocLogBuffer( gfl::heap::HeapBase* heap, u32 buffer_size );

      //-----------------------------------------------------------------------
      /**
       * @brief ���O���L�^����o�b�t�@���Z�b�g����( �o�b�t�@�̉���͍s���܂��� )
       *
       * @param p_buffer     �o�b�t�@�擪�ւ̃|�C���^
       * @param buffer_size  �o�b�t�@�̃T�C�Y
       */
      //-----------------------------------------------------------------------
      void SetLogBuffer( void* p_buffer, u32 buffer_size ); 

      //-----------------------------------------------------------------------
      /**
       * @brief ���O�̍̎���J�n����
       */
      //-----------------------------------------------------------------------
      void StartLogRec( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ���O�̍̎���I������
       */
      //-----------------------------------------------------------------------
      void StopLogRec( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ���O�̍̎撆���ǂ����𒲂ׂ�
       * @retval true  ���O�̎撆
       * @retval false ���O�̎撆�łȂ�
       */
      //-----------------------------------------------------------------------
      bool IsLogRecording( void ) const; 

      //-----------------------------------------------------------------------
      /**
       * @brief �L�^�������O�̃t���[�������擾����
       */
      //-----------------------------------------------------------------------
      u32 GetRecordedLogFrameCount( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �L�^�������O�ɂ���, �Đ��Ώۂ̃t���[������ݒ肷��
       * @param playFrameCount  �Đ��ΏۂƂ���t���[����( �����Ȃ�Ō�܂ōĐ����� )
       */
      //-----------------------------------------------------------------------
      void SetPlayableLogFrameCount( s32 playFrameCount );

      //-----------------------------------------------------------------------
      /**
       * @brief ���O�̍Đ����J�n����
       */
      //-----------------------------------------------------------------------
      void StartLogPlay( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ���O�̍Đ����I������
       */
      //-----------------------------------------------------------------------
      void StopLogPlay( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ���O�̍Đ������ǂ����𒲂ׂ�
       * @retval true  ���O�Đ���
       * @retval false ���O�Đ����łȂ�
       */
      //-----------------------------------------------------------------------
      bool IsLogPlaying() const; 

      //-----------------------------------------------------------------------
      /**
       * @brief ���O�̃��[�v�Đ����L�����H
       * @retval true   ���[�v�Đ�����
       * @retval false  ���[�v�Đ����Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsLogLoopPlayEnable( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���O�̃��[�v�Đ����L�����ǂ����H�ݒ肷��
       * @param isLoopEnable  true:���[�v�Đ�����, false:���[�v�Đ����Ȃ�
       *
       * @note �f�t�H���g�l�� false �ł�
       */
      //-----------------------------------------------------------------------
      void SetLogLoopPlayEnable( bool isLoopEnable ); 

      //-------------------------------------------------------------------
      /**
       *	@brief ���O�̃o�b�t�@���擾 
       */
      //--------------------------------------------------------------------
      void* GetLogBuffer( void ) const;

      //-------------------------------------------------------------------
      /**
       *	@brief ���O�̍ő�o�b�t�@�T�C�Y���擾 
       */
      //--------------------------------------------------------------------
      u32 GetLogBufferMaxSize( void ) const;

      //--------------------------------------------------------------------
      /**
       * @brief �f�[�^�̋L�^�ʒu���擾����
       *
       * @return �f�[�^�̋L�^�ʒu
       *
       */
      //--------------------------------------------------------------------
      u32 GetLogRecPos( void ) const;

      //--------------------------------------------------------------------
      /**
       * @brief �f�[�^�̋L�^�ʒu��ݒ肷��
       *
       * @param recPos  �f�[�^�̋L�^�ʒu
       *
       * @note recPos �ȍ~�̃f�[�^�͎����܂�
       */
      //--------------------------------------------------------------------
      void SetLogRecPos( u32 recPos );

      //--------------------------------------------------------------------
      /**
       * @brief �f�[�^�̋L�^�ő�ʒu���擾����
       *
       * @return �f�[�^�̋L�^�ő�ʒu
       *
       */
      //--------------------------------------------------------------------
      u32 GetLogRecMaxPos( void ) const;

      // ���s�[�g���o�p�����[�^
      struct RepeatParam 
      {
        u32 start_wait;   // �ŏ��̃��s�[�g����܂ł̑҂�����[frame]
        u32 interval;     // ���s�[�g����̊Ԋu[frame]
      };

      // ���s�[�g�p�����[�^�̃f�t�H���g�l
      static const u32 DEFAULT_REPEAT_START_WAIT = 20;
      static const u32 DEFAULT_REPEAT_INTERVAL   = 5;

      //-----------------------------------------------------------------------
      /**
       * @brief ���s�[�g���͂̌��o�p�����[�^���f�t�H���g�l�Ƀ��Z�b�g����
       */
      //-----------------------------------------------------------------------
      void SetDefaultRepeatParam( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ���s�[�g���͂̌��o�p�����[�^��ݒ肷��
       * @param param �ݒ肷�錟�o�p�����[�^
       */
      //-----------------------------------------------------------------------
      void SetRepeatParam( const RepeatParam& param );

      //-----------------------------------------------------------------------
      /**
       * @brief ���s�[�g���͂̌��o�p�����[�^���擾����
       * @param[out] p_param ���o�p�����[�^�̊i�[��
       */
      //-----------------------------------------------------------------------
      void GetRepeatParam( RepeatParam* p_param ) const;




      //-----------------------------------------------------------------------
      /**
       * @brief �f�o�C�X�X�V����
       */
      //-----------------------------------------------------------------------
      virtual void UpdateDevice( void );


      
      //-----------------------------------------------------------------------
      /**
       * @brief �f�o�C�X���ғ���Ԃɂ��邩�H
       * @retval true   �f�o�C�X�͉ғ����Ă���
       * @retval false  �f�o�C�X�͉ғ����Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsDeviceRunning( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �f�o�C�X�̉ғ���Ԃ�ύX����
       * @param isEnable  true:�f�o�C�X���ғ�����, false:�f�o�C�X���~����
       *
       * @note �f�o�C�X���~�����ꍇ, �ȍ~, �S�Ă̓��͂�����������܂�.
       */
      //-----------------------------------------------------------------------
      void SetDeviceRunningEnable( bool isEnable );



      //-----------------------------------------------------------------------
      /**
       * @brief �f�o�C�X���͒l�ɑ΂���S�Ă̐���� On/Off ��؂�ւ���
       * @param isEnable  true:���͒l�̐����L���ɂ���, false:���͒l�̐���𖳌��ɂ���
       *
       * @note isEnable �� false ���w�肵���ꍇ, 
       *       ( �����Ȃ鑼�̃��\�b�h�̉e�����󂯂Ă��Ȃ� )�n�[�h�E�F�A���͂�Ԃ��܂�
       *
       * @note ���̃��\�b�h���琧�䂪�߂������_����L���ɂȂ�܂�
       */
      //-----------------------------------------------------------------------
      void SetSoftwareControlEnable( bool isEnable );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�o�C�X���͒l�ɑ΂��鐧�䂪�L�����H
       * @retval true   ���͒l�ɑ΂��鐧�䂪�L��
       * @retval false  ���͒l�ɑ΂��鐧�䂪����
       */
      //-----------------------------------------------------------------------
      bool IsSoftwareControlEnable( void ) const;



#if GFL_DEBUG
      //----------------------------------------------------------------------------
      /**
       *	@brief  �f�o�b�O�L�[�@�������ݒ�
       */
      //-----------------------------------------------------------------------------
      void SetDebugKeyDisable( bool is_disable );
      bool IsDebugKeyDisable( void ) const;
#endif // GFL_DEBUG

      protected:

      /**
       * @brief �R���X�g���N�^
       * @param heap         �g�p����q�[�v
       * @param implementer  ���ۂɓ��͂̓ǂݎ����s���C���X�^���X
       */
      Device( gfl::heap::HeapBase* heap, DeviceImplementer* implementer );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~Device(); 

      /**
       * @brief ���݂̌��o�f�[�^�ƑO��̌��o�f�[�^���r��, �����f�[�^�������Ă��邩�ǂ����𒲂ׂ�
       * @retval true   �O��̌��o�f�[�^�ƍ���̌��o�f�[�^������
       * @retval false  �O��̌��o�f�[�^�ƍ���̌��o�f�[�^���قȂ�
       */
      virtual bool IsRepeatingSameInput( void ) const;

      /**
       * @brief �����f�[�^��~�ς���
       * @param buffer               �f�[�^�̊i�[��
       * @param detective_data       ����̌��o�f�[�^
       * @param prev_detective_data  �O��̌��o�f�[�^
       */
      virtual void StoreEffectiveData( void* buffer, const void* detective_data, const void* prev_detective_data ) const = 0;

      /**
       * @brief �Q�Ƃ��ׂ������f�[�^���擾����
       */
      const void* GetRunningEffectiveData( void ) const;



      DeviceImplementer* m_pImplementer;
      RepeatTimer* m_pRepeatTimer;
      LogPlayer* m_pLogPlayer;
      LogRecorder* m_pLogRecorder;

      FrameRate m_frameRate;                              // ����t���[�����[�g
      u32 m_frameCount;                                   // �t���[���J�E���^
      static u8 m_inputUpdateInterval[ FRAME_RATE_NUM ];  // �e�t���[�����[�g�ɑ΂���, ���͏�Ԃ̍X�V�Ԋu

      bool m_isRunning;
      bool m_isSoftwareControlEnable;
      bool m_isLogRecording;        // ���O�̍̎撆���ǂ���
      bool m_isLogPlaying;          // ���O�̍Đ������ǂ���
      bool m_isLogLoopPlayEnable;   // ���O�����[�v�Đ����邩�H

      void* m_nowDetectiveData;     // ���݂̌��o�f�[�^
      void* m_prevDetectiveData;    // �O��̌��o�f�[�^
      void* m_effectiveDataBuffer;  // �����f�[�^�̃o�b�t�@
      void* m_effectiveData;        // �����f�[�^
      void* m_invalidEffectiveData; // �����Ȏ����f�[�^

      void* m_nowRawDetectiveData;     // ���݂̖����H�Ȍ��o�f�[�^
      void* m_prevRawDetectiveData;    // �O��̖����H�Ȍ��o�f�[�^
      void* m_rawEffectiveDataBuffer;  // �����H�Ȏ����f�[�^�̃t���[�����[�g�z���o�b�t�@
      void* m_rawEffectiveData;        // �����H�Ȏ��s�f�[�^

#if GFL_DEBUG
      bool mDebugKeyDisable;  // �f�o�b�O�L�[����
#endif

      private: 
      void UpdateRawEffectiveData( void );
      void CopyNowDetectiveData_ToPrevDetectiveData( void );
      void UpdateNowDetectiveData_ByLogPlayer( void );
      void UpdateNowDetectiveData_ByImplementer( void ); 
      void UpdateEffectiveDataBufferByLastDetectiveData( void );
      bool IsOnEffectiveDataUpdateFrame( void ) const;
      void CopyEffectiveDataBuffer_ToEffectiveData( void );
      void ClearEffectiveDataBuffer( void );

    };

  }  // namespace ui
}  // namespace gfl


#endif // __GFL_UI_DEVICE_H__
