#ifndef __GFL_DEVICECAMERA_SYSTEM_H__
#define __GFL_DEVICECAMERA_SYSTEM_H__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_System.h
 * @brief  �J�����V�X�e��
 * @author obata_toshihiro
 * @date   2010.11.26
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once




/*
SBTS[2572]: �J�����N�����̓X���[�v���Ă͂Ȃ�Ȃ��B


[�ڂ������]
kujira��sango��project�̂���ł́A
gflib_cpp�̃f�o�C�X�J�����N�����̓X���[�v�����Ă͂Ȃ�Ȃ��B
�����J�����N�����ɃX���[�v�����Ă��܂��ƈȉ��̂悤�ȃt���[�Ɋׂ�A
���C���X���b�hnnMain���i�܂Ȃ��Ȃ��Ă��܂��B


(1) �RDS�{�̂��X���[�v��������

�t�^����ĂRDS�{�̂��X���[�v�����悤�Ƃ���ƁA
�܂��RDS�{�̂��X���[�v��������
project��SleepHandler::SleepQueryCallback���Ăяo���B
���̎��_�ł�nn::applet::REPLY_LATER��Ԃ��Ă���̂�
�A�v���P�[�V�����͂܂��X���[�v�ɂ͓���Ȃ��B

project\prog\src\system\sleep.cpp
AppletQueryReply SleepHandler::SleepQueryCallback( uptr arg )
  return nn::applet::REPLY_LATER;  //�ԓ���ۗ����܂��B(�����Ԃ�����́A���₩��ReplySleepQuery()�ŕԓ����Ă�������)


(2) nn::camera���X���[�v����

�A�v���P�[�V�����̓X���[�v���Ă��Ȃ���
nn::camera�̓X���[�v���J�n����B
���̂��߁Ann::camera�̊֐���ResultIsSleeping��Ԃ��悤�ɂȂ�B
gflib_cpp�̃J������ResultIsSleeping���Ԃ��Ă����
ResultIsSleeping�����������܂ł��̏�Ń��[�v���đ҂���ɂȂ��Ă���B

gflib_cpp\gflib\prog\src\device\camera\gfl_DeviceCamera_System.cpp
      void System::InitializeLibrary( void )
          while( result == nn::camera::CTR::ResultIsSleeping() ) {
            //NTSC���100milli�͖ڈ�
            nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
            result = nn::camera::Initialize();
          }


(3) ���C���X���b�h���i�܂Ȃ�

gflib_cpp�̃J�����̏�����(gfl::device::camera::System��new)��
���C���X���b�h�ōs���Ă���ꍇ�A
�J�����̏�������ResultIsSleeping�����҂����[�v����i�܂Ȃ��Ȃ��Ă��邽��
���C���X���b�h�����̏ꏊ����i�܂Ȃ��Ȃ�B

�{���Ȃ�A�A�v���P�[�V�������X���[�v�ɓ���ɂ�

project\prog\src\system\main.cpp
extern "C" bool GameMainLoop( gamesystem::GameManager* pGameManager )
      if(!sleepFunction(pGameManager, &isStopWifi)){

    project\prog\src\system\sleep.cpp
    void SleepHandler::SleepSystem( void )
      nn::applet::ReplySleepQuery(nn::applet::REPLY_ACCEPT);

���Ă΂�Ȃ���΂Ȃ�Ȃ����A
���̓��C���X���b�h���J�����̏��������s���Ă���ꏊ����
�i�܂Ȃ��Ȃ��Ă��邽��
�����ɂ͓��B�ł����X���[�v�ł��Ȃ��܂܂ł��邱�ƂɂȂ�B


(4) ���A���Ȃ��Ȃ�

�t�^���J���ĂRDS�{�̂��X���[�v���畜�A�����悤�Ƃ��Ă��A
�A�v���P�[�V����������ɃX���[�v���Ă��Ȃ�����
���A���邱�Ƃ��ł��Ȃ��B

�X���[�v����̕��A�́A�����
project\prog\src\system\sleep.cpp
void SleepHandler::SleepSystem( void )
  nn::applet::ReplySleepQuery(nn::applet::REPLY_ACCEPT);
�ɓ��B���Ă��ď��߂ĉ\�Ȃ��̂Ȃ̂ŁA
���C���X���b�h���i�܂�
�����ɓ��B���Ă��Ȃ����͕��A���ł��Ȃ��B

���̂��߁A�t�^�͊J���Ă��邪�A
nn::camera�̊֐���ResultIsSleeping��Ԃ�������Ƃ����󋵂ɂȂ�A
���ς�炸�J�����̏�������ResultIsSleeping�����҂����[�v����
���C���X���b�h�͐i�܂Ȃ��ł���B


�ȏ�ł��B

kujira��sango��project�̂���ł�
�X���[�v�v�����������Ƃ��ɒ����ɃX���[�v�ɂ͈ڍs���Ȃ��̂ŁA
���C���X���b�hnnMain����X���[�v���A��҂悤�Ȏd�g�݂̂�����̂��Ăяo���Ƃ���
���l�̂��Ƃ��N���Ȃ��悤�ɋC��t���܂��傤�I
*/




#include <nn.h>
#include <gfl_Heap.h> // for gfl::heap
#include <gfl_Macros.h> // for GFL_FORBID_COPY_AND_ASSIGN
#include <device/camera/gfl_DeviceCamera_Types.h>


namespace gfl {
  namespace device { 
    namespace camera { 

      class BufferManager;
      class CameraThread;
      class Y2rThread;
      class Controller;


      class System
      {
        GFL_FORBID_COPY_AND_ASSIGN( System ); // �R�s�[�R���X�g���N�^, ������Z�q���֎~


        public:
        /**
         * @brief �J�������Ƃ̐ݒ�
         */
        struct CameraParam {
          bool isValid;                        // �ȍ~�̃p�����[�^���L�����ǂ���
          DeviceID deviceID;                   // �ǂ̃J�����Ȃ̂��H
          CaptureSize captureSize;             // �J�������L���v�`������摜�̉𑜓x
          s32 trimmingWidth;                   // �L���v�`���摜(YUV)�̃g���~���O��̕�
          s32 trimmingHeight;                  // �L���v�`���摜(YUV)�̃g���~���O��̍���
          s32 outputWidth;                     // �o�͉摜(RGB)�̕�
          s32 outputHeight;                    // �o�͉摜(RGB)�̍���
          OutputFormat outputFormat;           // �o�̓s�N�Z����RGB�t�H�[�}�b�g
          BlockAlignment outputBlockAlignment; // �o�͉摜��RGB�f�[�^�̕��я�
          s16 outputAlpha;                     // �o�͉摜(RGB)�̃A���t�@�l (R8G8B8A8 or R5G5B5A1 �̏ꍇ�̂ݗL��)
          bool noiseFilterEnable;              // �m�C�Y�t�B���^�[�̗L��
          bool autoExposureEnable;             // �����I�o�@�\�̗L��
          s16 autoExposureRect_left;           // �����I�o�̌v�Z�Ɏg�p�����`�̈� (���� x ���W)
          s16 autoExposureRect_top;            // �����I�o�̌v�Z�Ɏg�p�����`�̈� (���� y ���W)
          s16 autoExposureRect_width;          // �����I�o�̌v�Z�Ɏg�p�����`�̈� (��)
          s16 autoExposureRect_height;         // �����I�o�̌v�Z�Ɏg�p�����`�̈� (����)
          WhiteBalance whiteBalance;           // �z���C�g�o�����X�̎w��
          bool autoWhiteBalanceEnable;         // �I�[�g�z���C�g�o�����X�̗L��
          s16 autoWhiteBalanceRect_left;       // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� (���� x ���W)
          s16 autoWhiteBalanceRect_top;        // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� (���� y ���W)
          s16 autoWhiteBalanceRect_width;      // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� (��)
          s16 autoWhiteBalanceRect_height;     // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� (����)
          s8 sharpness;                        // �N���x [-4, 5]
          Flip flip;                           // ���]����
          FrameRate frameRate;                 // �t���[�����[�g
          PhotoMode photoMode;                 // �B�e���[�h
          Effect effect;                       // �G�t�F�N�g
          Contrast contrast;                   // �R���g���X�g
          LensCorrection lensCorrection;       // �����Y�␳�̒��x
        };

        /**
         * @brief �Z�b�g�A�b�v�p�����[�^
         */
        struct SetupParam { 
          gfl::heap::HeapBase* pHeap;                              // �V�X�e�����g�p����q�[�v
          gfl::heap::HeapBase* pDeviceHeap;                        // �V�X�e�����g�p����f�o�C�X�q�[�v
          CameraParam          cameraParam[ CAMERA_NUM ];          // �e�J�����̐ݒ�
          u32                  cameraThreadPriority[ CAMERA_NUM ]; // �e�J��������X���b�h�̗D��x
          u32                  y2rThreadPriority;                  // �L���v�`���摜�����X���b�h�̗D��x
        };


        //-------------------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //-------------------------------------------------------------------------------
        System( const SetupParam& param );  // SBTS[2572]: �J�����N�����̓X���[�v���Ă͂Ȃ�Ȃ��B�ڂ��������gflib_cpp/gflib/prog/include/device/camera/gfl_DeviceCamera_System.h�ɁB

        //-------------------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //-------------------------------------------------------------------------------
        virtual ~System(); 


        public:
        //-------------------------------------------------------------------------------
        /**
         * @brief �J�����ւ̃A�N�Z�b�T���擾����
         * @param cameraID  �擾�Ώۂ̃J����
         * @retval NULL  �w�肵���J���������݂��Ȃ��ꍇ
         */
        //-------------------------------------------------------------------------------
        Controller* GetCameraController( CameraID cameraID ) const;



        protected:
        gfl::heap::HeapBase* m_pHeap;       // �V�X�e�����g�p����q�[�v
        gfl::heap::HeapBase* m_pDeviceHeap; // �V�X�e�����g�p����f�o�C�X�q�[�v

        BufferManager* m_buffer[ CAMERA_NUM ];           // �e�J�����̃o�b�t�@
        CameraThread*  m_cameraThread[ CAMERA_NUM ];     // �e�J�����X���b�h
        Y2rThread*     m_y2rThread;                      // YUV ==> RGB �ϊ��X���b�h
        nn::os::Event  m_y2rStartEvent[ CAMERA_NUM ];    // �J�����X���b�h ==> Y2R�X���b�h �ւ̕ϊ��ʒB�C�x���g
        Controller*    m_cameraController[ CAMERA_NUM ]; // �e�J�����ւ̃A�N�Z�b�T

        u32 m_errorBitFlag;  // �G���[�����������Ƃ�0�ȊO�ƂȂ�B�Ώۂ̃r�b�g��1�ɂȂ��Ă���BERROR_BIT_????



#if GFL_DEBUG
        // FatalError���N�����Ƃ��̏󋵂�����o���t���O
        private:
        static bool s_debugFatalErrorOccurFlag;
        public:
        static void DebugSetFatalErrorOccurFlag(bool flag)
        {
          s_debugFatalErrorOccurFlag = flag;
        }
        static bool DebugGetFatalErrorOccurFlag(void)
        {
          return s_debugFatalErrorOccurFlag;
        }
        void DebugSetErrorBitIfFatalErrorOccurFlagIsOn(void);
#endif



        private:
        void InitializeLibrary( void );                        // �J�������C�u����������������
        void InitBuffers( const SetupParam& param );           // �e�J�����̃o�b�t�@�𐶐�����
        void InitY2rThread( const SetupParam& param );         // Y2R�X���b�h�𐶐�����
        void InitCameraThreads( const SetupParam& param );     // �J�����X���b�h�𐶐�����
        void InitCameraControllers( const SetupParam& param ); // �J�����̃A�N�Z�b�T�𐶐�����

        BufferManager* CreateBuffer( const CameraParam& param );
        CameraThread* CreateCameraThread( CameraID cameraID, const CameraParam& param );
        Controller* CreateCameraController( CameraID cameraID, const CameraParam& param );

        void FinalizeLibrary( void );            // �J�������C�u�������I������
        void DeleteBuffers( void );              // �e�J�����̃o�b�t�@��j������
        void KillAndDeleteY2rThread( void );     // Y2R�X���b�h��j������
        void KillAndDeleteCameraThreads( void ); // �J�����X���b�h��j������
        void DeleteCameraController( void );     // �J�����̃A�N�Z�b�T��j������



        // �ȉ��Agfl::device::camera::Controller �p�̃��\�b�h ////////////////////////////

        public:
        //-------------------------------------------------------------------------------
        /**
         * @brief �J�������N������
         */
        //-------------------------------------------------------------------------------
        void ActivateCamera( CameraID cameraID );

        //-------------------------------------------------------------------------------
        /**
         * @brief ���ׂẴJ�������~������
         */
        //-------------------------------------------------------------------------------
        void DeactivateAllCamera( void ); 

        //-------------------------------------------------------------------------------
        /**
         * @brief �L���v�`�����J�n����
         */
        //-------------------------------------------------------------------------------
        void StartCapture( CameraID cameraID );

        //-------------------------------------------------------------------------------
        /**
         * @brief �L���v�`�����~����
         */
        //-------------------------------------------------------------------------------
        void StopCapture( CameraID cameraID );

        //-------------------------------------------------------------------------------
        /**
         * @brief �L���v�`�������ǂ����𒲂ׂ�
         */
        //-------------------------------------------------------------------------------
        bool IsCapturing( CameraID cameraID ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief �o�b�t�@���擾����
         */
        //-------------------------------------------------------------------------------
        const BufferManager* GetBufferManager( CameraID cameraID ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief �I����ݒ肵�܂��B
         * @param[in] exposure  �I�o�ʂ��w�肵�܂��B�w��\�Ȓl�͈̔͂� [-5, +5] �ł��B
         */
        //-------------------------------------------------------------------------------
        void SetExposure(CameraID cameraID, s8 exposure);

        //-------------------------------------------------------------------------------
        /**
         * @brief �V���b�^�[���̍Đ��ƃJ����LED�̈ꎞ�������s���܂��B
         */
        //-------------------------------------------------------------------------------
        void PlayShutterSound( ShutterSoundType type );

      }; 


    } // namespace camera
  } // namespace device
} // namespace gfl
#endif // __GFL_DEVICECAMERA_SYSTEM_H__
