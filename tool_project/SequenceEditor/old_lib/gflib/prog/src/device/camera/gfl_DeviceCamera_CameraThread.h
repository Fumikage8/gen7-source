#ifndef __gfl_DeviceCamera_CameraThread__
#define __gfl_DeviceCamera_CameraThread__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_CameraThread.h
 * @brief  �J����1�@�̏���������X���b�h
 * @author obata_toshihoro
 * @date   2010.11.26
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <bugfix_gflib.h>
#include <gfl_Macros.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>

namespace gfl {
  namespace device {
    namespace camera {

      class BufferManager;


      class CameraThread : public gfl::base::Thread 
      {
        GFL_FORBID_COPY_AND_ASSIGN( CameraThread ); // �R�s�[�R���X�g���N�^, ������Z�q���֎~

        public:
        /**
         * @brief �Z�b�g�A�b�v�p�����[�^
         */
        struct SetupParam {
          gfl::heap::HeapBase* pHeap;                   // �g�p����q�[�v
          nn::camera::Port port;                        // �J�����̐ڑ���|�[�g
          nn::camera::CameraSelect select;              // �ǂ̃J�����Ȃ̂�
          nn::camera::Size captureSize;                 // �J�������L���v�`������摜�̉𑜓x
          s16 captureWidth;                             // �𑜓x�̕�
          s16 captureHeight;                            // �𑜓x�̍���
          gfl::device::camera::BufferManager* pBuffers; // �J�����f�[�^�̃o�b�t�@
          nn::os::Event* pY2rStartEvent;                // YUV �f�[�^�̓]��������ʒm����C�x���g
          s16 trimmingWidth;                            // �g���~���O��̉摜�̉���
          s16 trimmingHeight;                           // �g���~���O��̉摜�̏c��
          bool noiseFilterEnable;                       // �m�C�Y�t�B���^�[�̗L��
          bool autoExposureEnable;                      // �����I�o�@�\�̗L��
          s16 autoExposureRect_left;                    // �����I�o�̌v�Z�Ɏg�p�����`�̈� ( ���� x ���W )
          s16 autoExposureRect_top;                     // �����I�o�̌v�Z�Ɏg�p�����`�̈� ( ���� y ���W )
          s16 autoExposureRect_width;                   // �����I�o�̌v�Z�Ɏg�p�����`�̈� ( ���� )
          s16 autoExposureRect_height;                  // �����I�o�̌v�Z�Ɏg�p�����`�̈� ( �c�� )
          nn::camera::WhiteBalance whiteBalance;        // �z���C�g�o�����X�̎w��
          bool autoWhiteBalanceEnable;                  // �I�[�g�z���C�g�o�����X�̗L��
          s16 autoWhiteBalanceRect_left;                // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ( ���� x ���W )
          s16 autoWhiteBalanceRect_top;                 // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ( ���� y ���W )
          s16 autoWhiteBalanceRect_width;               // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ( ���� )
          s16 autoWhiteBalanceRect_height;              // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ( �c�� )
          s8 sharpness;                                 // �N���x
          nn::camera::Flip flip;                        // ���]����
          nn::camera::FrameRate frameRate;              // �t���[�����[�g
          nn::camera::PhotoMode photoMode;              // �B�e���[�h
          nn::camera::Effect effect;                    // �G�t�F�N�g
          nn::camera::Contrast contrast;                // �R���g���X�g
          nn::camera::LensCorrection lensCorrection;    // �����Y�␳�̒��x
          u32*                 errorBitFlag;  // ERROR_BIT_????
        };
        //-------------------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //-------------------------------------------------------------------------------
        CameraThread( const SetupParam& param );

        //-------------------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //-------------------------------------------------------------------------------
        virtual ~CameraThread();


        public:
        //-------------------------------------------------------------------------------
        /**
         * @brief �J�������N������
         */
        //-------------------------------------------------------------------------------
        void Activate();


        //-------------------------------------------------------------------------------
        /**
         * @brief �L���v�`�����J�n����
         */
        //-------------------------------------------------------------------------------
        void StartCapture();

        //-------------------------------------------------------------------------------
        /** 
         * @brief �L���v�`�����~����
         */
        //-------------------------------------------------------------------------------
        void StopCapture( void );

        //-------------------------------------------------------------------------------
        /**
         * @brief �L���v�`�������ǂ����𒲂ׂ�
         */
        //-------------------------------------------------------------------------------
        bool IsBusy( void ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief �I����ݒ肵�܂��B
         * @param[in] exposure  �I�o�ʂ��w�肵�܂��B�w��\�Ȓl�͈̔͂� [-5, +5] �ł��B
         */
        //-------------------------------------------------------------------------------
        void SetExposureEx(s8 exposure);


        private:
        // �X���b�h����
        virtual void Initialize( void ); // ����������
        virtual void Finalize( void );   // �I������
        virtual void Main( void );       // ���C������
        virtual void OnKill( void );     // �X���b�h�j�����̏���


        protected:
        nn::camera::Port m_port;            // �J�����̐ڑ���|�[�g
        nn::camera::CameraSelect m_select;  // �ǂ̃J�����Ȃ̂�
        gfl::device::camera::BufferManager* m_pBuffers; // �L���v�`���f�[�^�̃o�b�t�@
        nn::os::Event m_threadEndEvent;     // �X���b�h�𔲂��邽�߂̃C�x���g
        nn::os::Event m_yuvReceiveEndEvent; // YUV �f�[�^�̎�M�������ʒm�����C�x���g
        nn::os::Event m_bufferErrorEvent;   // �o�b�t�@�G���[���ʒm�����C�x���g
        nn::os::Event m_vsyncEvent;         // �J������ VSync ���ʒm�����C�x���g
        nn::os::Event m_activateEvent;      // �J�������N�����邽�߂̃C�x���g
        nn::os::Event m_captureStartEvent;  // �L���v�`�����J�n���邽�߂̃C�x���g
        nn::os::Event m_captureStopEvent;   // �L���v�`�����~���邽�߂̃C�x���g
        nn::os::Event* m_pY2rStartEvent;    // 1�t���[������YUV�f�[�^�̎�M������ʒm����C�x���g
        s32 m_yuvTransferUnit;              // 1��̓]���T�C�Y
        
#ifdef BUGFIX_BTS6907_20130528  // �J�����L���v�`�����ɓd�����j���[��\�������, �L���v�`�����I�����Ȃ��s��̏C��
        // StopCapture ������, IsBusy �ŃL���v�`���I����҂ꍇ, IsBusy ����� true ��Ԃ���ԂɂȂ邱�Ƃ�����.
        // ���̏ꍇ�͈�莞�ԂŃ^�C���A�E�g��, �L���v�`������~�����Ƃ݂Ȃ�.
        bool m_isCaptureStoppedByTimeout;
#endif

        // StopCapture ������, IsBusy �ŃL���v�`���I����҂ꍇ,
        // �r���� StartCapture �������Ă��܂���, �L���v�`�����I�����������ɑ҂������Ă��܂�.
        // ���̂���, �L���v�`���̊J�n�E��~�����͔r���I�ɍs��.
        nn::os::CriticalSection m_CS_forCaptureSwitch;

        u32* m_errorBitFlag;  // ERROR_BIT_????
        bool m_threadEndFlag;  // SBTS[2574]: �y�Ƃ܂�FQR�R�[�h�ǂݎ���ʂŃJ�����@�\����~���邱�Ƃ�����A���̏�ԂŖ߂낤�Ƃ���ƂƂ܂�z  //sango,2014/06/25,kawada_koji


        private:
        // �J����������������
        void InitCamera( const SetupParam& param );
        void SetCaptureSize( const SetupParam& param );
        void SetNoiseFilter( const SetupParam& param );
        void SetExposure( const SetupParam& param );
        void SetWhiteBalance( const SetupParam& param );
        void SetTrimming( const SetupParam& param );
        void SetSharpness( const SetupParam& param );
        void SetFlip( const SetupParam& param );
        void SetFrameRate( const SetupParam& param );
        void SetPhotoMode( const SetupParam& param );
        void SetEffect( const SetupParam& param );
        void SetContrast( const SetupParam& param );
        void SetLensCorrection( const SetupParam& param );

        // 1�t���[�����̃f�[�^�]���̐ݒ������
        void SetNextFrameReceiving( void );

        // �J������ Vsync ����
        void VsyncFunc( void );

        // �L���v�`���̊J�n��r���I�ɍs��
        void StartCaptureExclusively( void );

        // �L���v�`���̒�~�ƒ�~�҂���r���I�ɍs��
        void StopCaptureAndWaitCaptureStopExclusively( void );


        private:
        // �X���b�h���m�ۂ���X�^�b�N�T�C�Y
        static const u16 STACK_SIZE = 4096;


#if GFL_DEBUG
        // FatalError���N�����Ƃ��̏󋵂�����o���t���O
        public:
        void DebugSetErrorBit(void);
#endif

      };


    } // namespace camera
  } // namespace device
} // namespace gfl
#endif
