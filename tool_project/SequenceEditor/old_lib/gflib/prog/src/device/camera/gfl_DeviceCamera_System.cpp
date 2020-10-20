/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_System.cpp
 * @brief  �J�����V�X�e��
 * @author obata_toshihiro
 * @date   2010.11.22
 */ 
/////////////////////////////////////////////////////////////////////////////////////////
#include <nn.h>
#include <nn/y2r.h>
#include <gfl_Debug.h>
#include <gfl_Heap.h>
#include <device/camera/gfl_DeviceCamera_Types.h>
#include <device/camera/gfl_DeviceCamera_Controller.h>
#include <device/camera/gfl_DeviceCamera_System.h>
#include "gfl_DeviceCamera_Y2rThread.h"
#include "gfl_DeviceCamera_CameraThread.h"
#include "gfl_DeviceCamera_BufferManager.h"


namespace gfl {
  namespace device { 
    namespace camera { 


      /**
       * @brief �J�����f�o�C�X�̏��
       *
       * DeviceID ����
       * nn::camera::CameraSelect, nn::camera::Port �͈�ӂɌ��܂�
       */
      struct DeviceInfo { 
        nn::camera::CameraSelect select; // �J�����̑g�ݍ��킹
        nn::camera::Port         port;   // �J�����̐ڑ���|�[�g
      };

      const DeviceInfo g_DeviceInfo[ DEVICE_ID_NUM ] = 
      {
        { nn::camera::SELECT_IN1,  nn::camera::PORT_CAM1 }, // DEVICE_ID_IN
        { nn::camera::SELECT_OUT2, nn::camera::PORT_CAM2 }, // DEVICE_ID_OUT_L
        { nn::camera::SELECT_OUT1, nn::camera::PORT_CAM1 }, // DEVICE_ID_OUT_R
      };


      /**
       * @brief �𑜓x�̕����擾����
       */
      s16 GetCaptureWidth( CaptureSize size )
      {
        switch( size ) {
        case CAPTURE_SIZE_VGA:            return 640;
        case CAPTURE_SIZE_QVGA:           return 320;
        case CAPTURE_SIZE_QQVGA:          return 160;
        case CAPTURE_SIZE_CIF:            return 352;
        case CAPTURE_SIZE_QCIF:           return 176;
        case CAPTURE_SIZE_DS_LCD:         return 256;
        case CAPTURE_SIZE_DS_LCDx4:       return 512;
        case CAPTURE_SIZE_CTR_TOP_LCD:    return 400;
                                          //case CAPTURE_SIZE_CTR_BOTTOM_LCD: return 320; // alias
        }
        GFL_ASSERT(0);  //@check
        return 0;
      }


      /**
       * @brief �𑜓x�̍������擾����
       */
      s16 GetCaptureHeight( CaptureSize size )
      {
        switch( size ) {
        case CAPTURE_SIZE_VGA:            return 480;
        case CAPTURE_SIZE_QVGA:           return 240;
        case CAPTURE_SIZE_QQVGA:          return 120;
        case CAPTURE_SIZE_CIF:            return 288;
        case CAPTURE_SIZE_QCIF:           return 144;
        case CAPTURE_SIZE_DS_LCD:         return 192;
        case CAPTURE_SIZE_DS_LCDx4:       return 384;
        case CAPTURE_SIZE_CTR_TOP_LCD:    return 240;
                                          //case CAPTURE_SIZE_CTR_BOTTOM_LCD: return 240; // alias
        }
        GFL_ASSERT(0);  //@check
        return 0;
      }



#if GFL_DEBUG 
      // FatalError���N�����Ƃ��̏󋵂�����o���t���O
      bool System::s_debugFatalErrorOccurFlag  = false;
#endif




      //---------------------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       *
       * SBTS[2572]: �J�����N�����̓X���[�v���Ă͂Ȃ�Ȃ��B�ڂ��������gflib_cpp/gflib/prog/include/device/camera/gfl_DeviceCamera_System.h�ɁB
       */
      //---------------------------------------------------------------------------------
      System::System( const SetupParam& param ) :
        m_pHeap( param.pHeap ),
        m_pDeviceHeap( param.pDeviceHeap ),
        m_errorBitFlag(ERROR_BIT_NONE)
      {
        this->InitializeLibrary();

        // Y2R -> RGB �ϊ��˗��C�x���g��������
        for( int i=0; i<CAMERA_NUM; i++ )
        {
          m_y2rStartEvent[i].Initialize( false ); 
        }

        this->InitBuffers( param );
        this->InitY2rThread( param );
        this->InitCameraThreads( param );
        this->InitCameraControllers( param );


        // Y2R�X���b�h�N��
        if(m_y2rThread) m_y2rThread->Start( param.y2rThreadPriority );

        // �J�����X���b�h�N��
        for( int i=0; i<CAMERA_NUM; i++ )
        {
          if( m_cameraThread[i] ) {
            m_cameraThread[i]->Start( param.cameraThreadPriority[i] );
          }
        }


#if 0
        // �J��������ʑ̂��B�e����^�C�~���O�𓯊������܂��B
        // CTR �ł̓J�����͓Ɨ��ɓ��삷��\���ɂȂ��Ă��邽�߁A
        // �t���[�����[�g�̐ݒ�𓯂��ɂ��Ă����Ƃ��Ă��A�J��������ʑ̂��B�e����^�C�~���O���قȂ�܂��B
        // ���ꂪ�傫���ƁA���E�̔�ʑ̂̓������Y���܂��B
        // �����ŁASynchronizeVsyncTiming �֐��ɂ���āA��ʑ̂��B�e����^�C�~���O���ł��铯���^�C�~���O�ɂ��܂��B
        // ���̊֐����g�p����ƁA�^�C�~���O�̃Y���� 100 ��sec ���x�ɗ}���邱�Ƃ��ł��܂�(Release�r���h��)�B
        // ���̊֐��̓J�����̋N�����ɌĂяo���K�v������܂��B
        {
          nn::Result result = nn::camera::SynchronizeVsyncTiming( nn::camera::SELECT_OUT1, nn::camera::SELECT_OUT2 );
          if( result == nn::camera::ResultFatalError() ) {
            GFL_ASSERT(0);
          }
        }
#endif
      }


      //====================================
      /**
       * @brief �J�������C�u����������������
       */
      void System::InitializeLibrary( void )
      {
        nn::Result result;

#if GFL_DEBUG 
        // FatalError���N�����Ƃ��̏󋵂�����o���t���O
        if( s_debugFatalErrorOccurFlag )
        {
          m_errorBitFlag |= ERROR_BIT_CAMERA_INITIALIZE_FATAL_ERROR;
        }
        else
        {
#endif

        result = nn::camera::Initialize();
        if( result == nn::camera::ResultFatalError() ) {
          //GFL_ASSERT_STOP(0);  // �Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          m_errorBitFlag |= ERROR_BIT_CAMERA_INITIALIZE_FATAL_ERROR;
        }

#if GFL_DEBUG
        }  // if( s_debugFatalErrorOccurFlag )
#endif

        if(m_errorBitFlag==ERROR_BIT_NONE)
        {
#ifdef BUGFIX_BTS6963_20130530
          while( result == nn::camera::CTR::ResultIsSleeping() ) {
            //NTSC���100milli�͖ڈ�
            nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
            result = nn::camera::Initialize();
          }
#endif
        }

        bool y2rResult = nn::y2r::Initialize();
        s32  y2rCount = 0;
        while( !y2rResult )
        {
          // CTR-SDK�̊֐����t�@�����X���1�~���b�̑҂�������ōēx�Ă�
          nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(1));
          y2rResult = nn::y2r::Initialize();
          ++y2rCount;
          if(y2rCount >= 1000) break;  // 1�b�҂��ă_���������炠����߂�
        }
        if(!y2rResult)
        {
          m_errorBitFlag |= ERROR_BIT_Y2R_INITIALIZE_ERROR;
        }
      }


      //====================================
      /**
       * @brief �e�J�����̃o�b�t�@������������
       */
      void System::InitBuffers( const SetupParam& param )
      {
        for( int i=0; i<CAMERA_NUM; i++ ) 
        {
          const CameraParam& camera_param = param.cameraParam[i];

          if( camera_param.isValid )
          {
            m_buffer[i] = this->CreateBuffer( camera_param );
          }
          else 
          {
            m_buffer[i] = NULL;
          }
        }
      }

      /**
       * @breif �J�����̃o�b�t�@�𐶐�����
       */
      BufferManager* System::CreateBuffer( const CameraParam& param )
      {
        GFL_ASSERT( param.isValid );  //@check

        gfl::device::camera::BufferManager::SetupParam buf_param;
        buf_param.pHeap         = m_pHeap;
        buf_param.yuvBufferNum  = 3;
        if(m_errorBitFlag==ERROR_BIT_NONE)
        {
          buf_param.yuvBufferSize = nn::camera::GetFrameBytes( param.trimmingWidth, param.trimmingHeight );  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          buf_param.rgbBufferSize = nn::y2r::GetOutputImageSize( param.outputWidth, param.outputHeight, static_cast<nn::y2r::OutputFormat>(param.outputFormat) );  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
        }
        else
        {
          buf_param.yuvBufferSize = param.trimmingWidth * param.trimmingHeight * 4;  // @todo CTR�łȂ��ꍇ�v���ӁI ���ӊ��N�̂��߂���todo�͎c���Ă����܂��B  // CTR CTR�v���O���~���O�}�j���A�� �V�X�e���� 2014-01-21 Ver.7.0  �J�����̃L���v�`���摜�� YUV �ꊇ�̃t�H�[�}�b�g�ł̂ݎ擾���邱�Ƃ��ł��܂��̂ŁA�قƂ�ǂ̏ꍇ�A�L���v�`���摜�̕ϊ����ɂ� INPUT_YUV422_BATCH ���w�肷�邱�ƂɂȂ�܂��B
          buf_param.rgbBufferSize = param.outputWidth * param.outputHeight * OUTPUT_BIT_PER_PIXEL[param.outputFormat]/8;
        }

        return GFL_NEW(m_pHeap) gfl::device::camera::BufferManager( buf_param );
      }


      //====================================
      /**
       * @brief Y2R�X���b�h������������
       */
      void System::InitY2rThread( const SetupParam& param )
      {
        if(m_errorBitFlag==ERROR_BIT_NONE)
        {
          Y2rThread::SetupParam thread_param;
         
          for( int i=0; i<CAMERA_NUM; i++ )
          {
            const CameraParam& camera_param = param.cameraParam[i];
         
            thread_param.convertParam[i].pConvertStartEvent   = &m_y2rStartEvent[i];
            thread_param.convertParam[i].pBuffers             = m_buffer[i];
            thread_param.convertParam[i].trimmingWidth        = camera_param.trimmingWidth;
            thread_param.convertParam[i].trimmingHeight       = camera_param.trimmingHeight;
            thread_param.convertParam[i].outputWidth          = camera_param.outputWidth;
            thread_param.convertParam[i].outputHeight         = camera_param.outputHeight;
            thread_param.convertParam[i].outputFormat         = static_cast<nn::y2r::OutputFormat>(camera_param.outputFormat);
            thread_param.convertParam[i].outputBlockAlignment = static_cast<nn::y2r::BlockAlignment>(camera_param.outputBlockAlignment);
            thread_param.convertParam[i].outputAlpha          = camera_param.outputAlpha;
            thread_param.convertParam[i].outputOffset         = nn::y2r::GetOutputImageSize(  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                camera_param.outputWidth,
                ( camera_param.outputHeight - camera_param.trimmingHeight ) / 2,
                nn::y2r::OUTPUT_RGB_24 );
          }
          thread_param.pHeap = param.pHeap;
          thread_param.errorBitFlag = &m_errorBitFlag;
         
          m_y2rThread = GFL_NEW(param.pHeap) Y2rThread( thread_param );
        }
        else
        {
          m_y2rThread = NULL;
        }
      }


      //====================================
      /**
       * @brief �J�����X���b�h������������
       */
      void System::InitCameraThreads( const SetupParam& param )
      {
        for( s32 i = 0; i < CAMERA_NUM; i++ )
        {
          const CameraParam& camera_param = param.cameraParam[i];

          if( camera_param.isValid )
          {
            m_cameraThread[i] = this->CreateCameraThread( static_cast<CameraID>(i), camera_param );
          }
          else
          {
            m_cameraThread[i] = NULL;
          }
        } 
      }

      /**
       * @brief �J�����X���b�h�𐶐�����
       */
      CameraThread* System::CreateCameraThread( CameraID cameraID, const CameraParam& param )
      {
        GFL_ASSERT( param.isValid );  //@check

        CameraThread::SetupParam thread_param;

        thread_param.pHeap                       = m_pHeap;
        thread_param.port                        = g_DeviceInfo[ param.deviceID ].port;
        thread_param.select                      = g_DeviceInfo[ param.deviceID ].select;
        thread_param.captureSize                 = static_cast<nn::camera::Size>( param.captureSize );
        thread_param.captureWidth                = GetCaptureWidth( param.captureSize );
        thread_param.captureHeight               = GetCaptureHeight( param.captureSize );
        thread_param.pBuffers                    = m_buffer[ cameraID ];
        thread_param.pY2rStartEvent              = &m_y2rStartEvent[ cameraID ];
        thread_param.trimmingWidth               = param.trimmingWidth;
        thread_param.trimmingHeight              = param.trimmingHeight;
        thread_param.noiseFilterEnable           = param.noiseFilterEnable;
        thread_param.autoExposureEnable          = param.autoExposureEnable;
        thread_param.autoExposureRect_left       = param.autoExposureRect_left;
        thread_param.autoExposureRect_top        = param.autoExposureRect_top;
        thread_param.autoExposureRect_width      = param.autoExposureRect_width;
        thread_param.autoExposureRect_height     = param.autoExposureRect_height;
        thread_param.whiteBalance                = static_cast<nn::camera::WhiteBalance>(param.whiteBalance);
        thread_param.autoWhiteBalanceRect_left   = param.autoWhiteBalanceRect_left;
        thread_param.autoWhiteBalanceRect_top    = param.autoWhiteBalanceRect_top;
        thread_param.autoWhiteBalanceRect_width  = param.autoWhiteBalanceRect_width;
        thread_param.autoWhiteBalanceRect_height = param.autoWhiteBalanceRect_height;
        thread_param.sharpness                   = param.sharpness;
        thread_param.flip                        = static_cast<nn::camera::Flip>(param.flip);
        thread_param.frameRate                   = static_cast<nn::camera::FrameRate>(param.frameRate);
        thread_param.photoMode                   = static_cast<nn::camera::PhotoMode>(param.photoMode);
        thread_param.effect                      = static_cast<nn::camera::Effect>(param.effect);
        thread_param.contrast                    = static_cast<nn::camera::Contrast>(param.contrast);
        thread_param.lensCorrection              = static_cast<nn::camera::LensCorrection>(param.lensCorrection);
        thread_param.errorBitFlag                = &m_errorBitFlag;

        // �����J�����͍��E���]�̎w�肪�t�ɂȂ�
        if( param.deviceID == DEVICE_ID_IN )
        {
          if( param.flip == FLIP_NONE ) {
            thread_param.flip = nn::camera::FLIP_HORIZONTAL;
          }
          else if( param.flip == FLIP_HORIZONTAL ) {
            thread_param.flip = nn::camera::FLIP_NONE;
          }
        }

        if(m_errorBitFlag==ERROR_BIT_NONE)
        {
          return GFL_NEW(m_pHeap) CameraThread( thread_param );
        }
        else
        {
          return NULL;
        }
      }


      //====================================
      /**
       * @brief �J�����̃A�N�Z�b�T������������
       */
      void System::InitCameraControllers( const SetupParam& param )
      {
        for( int i=0; i<CAMERA_NUM; i++ )
        {
          const CameraParam& camera_param = param.cameraParam[i];

          if( camera_param.isValid )
          {
            m_cameraController[i] = this->CreateCameraController( static_cast<CameraID>(i), camera_param );
          }
          else
          {
            m_cameraController[i] = NULL;
          }
        }
      }

      /**
       * @brief �J�����̃A�N�Z�b�T�𐶐�����
       */
      Controller* System::CreateCameraController( CameraID cameraID, const CameraParam& param )
      {
        if( !param.isValid ) {
          GFL_ASSERT(0);  //@check
          return NULL;
        }

        Controller::SetupParam controller_param;

        controller_param.cameraID      = cameraID;
        controller_param.pCameraSystem = this;

        return GFL_NEW(m_pHeap) Controller( controller_param );
      }





      //---------------------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //---------------------------------------------------------------------------------
      System::~System()
      {
        this->DeleteCameraController();

        // �X���b�h��~�E�j��
        this->KillAndDeleteY2rThread();
        this->KillAndDeleteCameraThreads();

        // Y2R -> RGB �ϊ��˗��C�x���g��j��
        for( int i=0; i < CAMERA_NUM; i++ )
        {
          m_y2rStartEvent[i].Finalize();
        }

        this->DeleteBuffers(); 
        this->FinalizeLibrary();
      }


      /**
       * @brief �J�����̃A�N�Z�b�T��j������
       */
      void System::DeleteCameraController( void )
      {
        for( int i=0; i<CAMERA_NUM; i++ )
        {
          if( m_cameraController[i] ) {
            GFL_DELETE m_cameraController[i];
          }
        }
      }


      /**
       * @brief Y2R�X���b�h��j������
       */
      void System::KillAndDeleteY2rThread( void )
      {
        if( m_y2rThread ) {
          m_y2rThread->Kill();
          while( m_y2rThread->IsAlive() ) {
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
          }
          GFL_DELETE m_y2rThread;
          m_y2rThread = NULL;
        }
      }


      /**
       * @brief �J�����X���b�h��j������
       */
      void System::KillAndDeleteCameraThreads( void )
      {
        for( int i = 0; i < CAMERA_NUM; i++ )
        {
          if( m_cameraThread[i] )
          {
            m_cameraThread[i]->Kill();
            while( m_cameraThread[i]->IsAlive() ) {
              nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
            }
            GFL_DELETE m_cameraThread[i];
            m_cameraThread[i] = NULL;
          }
        }
      }


      /**
       * @brief �e�J�����̃o�b�t�@��j������
       */
      void System::DeleteBuffers( void )
      {
        for( int i=0; i<CAMERA_NUM; i++ )
        {
          if( m_buffer[i] ) {
            GFL_DELETE m_buffer[i];
          }
        }
      }


      /**
       * @brief �J�������C�u�������I������
       */
      void System::FinalizeLibrary( void )
      {
        nn::camera::Finalize();  // Initialize�ł��Ă��Ȃ��Ă��Ă�ő��v�������B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
        nn::y2r::Finalize();  // Initialize�ł��Ă��Ȃ��Ă��Ă�ő��v�������B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief �J�����ւ̃A�N�Z�b�T���擾����
       * @param cameraID  �擾�Ώۂ̃J����
       * @retval NULL  �w�肵���J���������݂��Ȃ��ꍇ
       */
      //---------------------------------------------------------------------------------
      Controller* System::GetCameraController( CameraID cameraID ) const
      {
        if( CAMERA_NUM <= cameraID ) {
          GFL_ASSERT(0);  //@fix
          return NULL;
        }
        return m_cameraController[ cameraID ];
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �J�������N������
       */
      //---------------------------------------------------------------------------------
      void System::ActivateCamera( CameraID cameraID )
      {
        if( cameraID < CAMERA_NUM )
        {
          if( m_cameraThread[ cameraID ] )
          {
            m_cameraThread[ cameraID ]->Activate();
          }
        }
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief ���ׂẴJ�������~������
       */
      //---------------------------------------------------------------------------------
      void System::DeactivateAllCamera( void )
      {
        // ���ׂẴJ�����̃L���v�`�����~������
        for( int i=0; i<CAMERA_NUM; i++ ) {
          if( m_cameraThread[i] ) {
            m_cameraThread[i]->StopCapture();
          }
        }

        // �L���v�`���̒�~������҂�
        bool finish = false;
        while( finish == false )
        {
          finish = true;
          for( int i=0; i<CAMERA_NUM; i++ ) {
            if( m_cameraThread[i] && m_cameraThread[i]->IsBusy() ) {
              finish = false;
              break;
            }
          }

          if( finish == false ) {
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
          }
        }

        // �S�J�������~������
        if(m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::Activate( nn::camera::SELECT_NONE );
          if( result == nn::camera::ResultFatalError() ) {
            m_errorBitFlag |= ERROR_BIT_CAMERA_ACTIVATE_FATAL_ERROR;
          }
        }
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�����J�n����
       */
      //---------------------------------------------------------------------------------
      void System::StartCapture( CameraID cameraID )
      {
        if( cameraID < CAMERA_NUM )
        {
          if( m_cameraThread[ cameraID ] )
          {
            m_cameraThread[ cameraID ]->StartCapture();
          }
        }
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�����~����
       */
      //---------------------------------------------------------------------------------
      void System::StopCapture( CameraID cameraID )
      {
        if( cameraID < CAMERA_NUM )
        {
          if( m_cameraThread[ cameraID ] )
          {
            m_cameraThread[ cameraID ]->StopCapture();
          }
        }
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�������ǂ����𒲂ׂ�
       */
      //---------------------------------------------------------------------------------
      bool System::IsCapturing( CameraID cameraID ) const
      {
        if( cameraID < CAMERA_NUM )
        {
          if( m_cameraThread[ cameraID ] )
          {
            return m_cameraThread[ cameraID ]->IsBusy();
          }
        }
        return false;
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �o�b�t�@���擾����
       */
      //---------------------------------------------------------------------------------
      const BufferManager* System::GetBufferManager( CameraID cameraID ) const
      {
        return m_buffer[ cameraID ];
      }


      //-------------------------------------------------------------------------------
      /**
       * @brief �I����ݒ肵�܂��B
       * @param[in] exposure  �I�o�ʂ��w�肵�܂��B�w��\�Ȓl�͈̔͂� [-5, +5] �ł��B
       */
      //-------------------------------------------------------------------------------
      void System::SetExposure(CameraID cameraID, s8 exposure)
      {
        if( cameraID < CAMERA_NUM )
        {
          if( m_cameraThread[ cameraID ] )
          {
            m_cameraThread[ cameraID ]->SetExposureEx(exposure);
          }
        }
      }


      //-------------------------------------------------------------------------------
      /**
       * @brief �V���b�^�[���̍Đ��ƃJ����LED�̈ꎞ�������s���܂��B
       */
      //-------------------------------------------------------------------------------
      void System::PlayShutterSound( ShutterSoundType type )
      {
        //if(m_errorBitFlag==ERROR_BIT_NONE)  // PlayShutterSound�̓G���[���ł��Ăׂ�悤�Ȃ̂ŁAif�̓R�����g�A�E�g���Ă����B
        {
          nn::Result result = nn::camera::PlayShutterSound( static_cast<nn::camera::ShutterSoundType>(type) );
          if( result == nn::camera::ResultFatalError() ) {
            m_errorBitFlag |= ERROR_BIT_CAMERA_PLAYSHUTTERSOUND_FATAL_ERROR;
          }
        }
      }


#if GFL_DEBUG
      // FatalError���N�����Ƃ��̏󋵂�����o���t���O
      void System::DebugSetErrorBitIfFatalErrorOccurFlagIsOn(void)
      {
        if( s_debugFatalErrorOccurFlag )
        {
          if(m_errorBitFlag==ERROR_BIT_NONE)
          {
            for( int i=0; i<CAMERA_NUM; i++ )
            {
              if( m_cameraThread[i] )
              {
                m_cameraThread[i]->DebugSetErrorBit();
                break;  // �ǂ����ŗ��Ă���������ĂȂ��Ă����̂Ń��[�v�𔲂���B
              }
            }
          }
        }
      }
#endif


    } // namespace camera
  } // namespace device
} // namespace gfl
