/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_CameraThread.cpp
 * @brief  �J����1�@�̏���������X���b�h
 * @author obata_toshihoro
 * @date   2010.11.26
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <bugfix_gflib.h>
#include <gfl_Base.h>
#include <gfl_Heap.h> 
#include <gfl_Debug.h>
#include "gfl_DeviceCamera_BufferManager.h"
#include "gfl_DeviceCamera_CameraThread.h"

namespace gfl {
  namespace device {
    namespace camera { 


      //---------------------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //---------------------------------------------------------------------------------
      CameraThread::CameraThread( const SetupParam& param ) :
        Thread( param.pHeap, STACK_SIZE ),
        m_port( param.port ),
        m_select( param.select ),
        m_pBuffers( param.pBuffers ),
        m_threadEndEvent(),
        m_yuvReceiveEndEvent(),
        m_bufferErrorEvent(),
        m_vsyncEvent(),
        m_activateEvent(),
        m_captureStartEvent(),
        m_captureStopEvent(),
        m_pY2rStartEvent( param.pY2rStartEvent ),
        m_yuvTransferUnit( 0 ),
#ifdef BUGFIX_BTS6907_20130528  // �J�����L���v�`�����ɓd�����j���[��\�������, �L���v�`�����I�����Ȃ��s��̏C��
        m_isCaptureStoppedByTimeout( false ),
#endif
        m_CS_forCaptureSwitch(),
        m_errorBitFlag(param.errorBitFlag),
        m_threadEndFlag(false)
      { 
        m_threadEndEvent.Initialize( false );
        m_activateEvent.Initialize( false );
        m_captureStartEvent.Initialize( false );
        m_captureStopEvent.Initialize( false ); 
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::camera::GetBufferErrorInterruptEvent( &m_bufferErrorEvent, m_port ); 
          nn::camera::GetVsyncInterruptEvent( &m_vsyncEvent, m_port ); 
        }
        else
        {
          m_bufferErrorEvent.Initialize( false );
          m_vsyncEvent.Initialize( false );
          m_yuvReceiveEndEvent.Initialize( false );
        }
        m_CS_forCaptureSwitch.Initialize();

        this->InitCamera( param );
      }


      /**
       * @brief �J����������������
       */
      void CameraThread::InitCamera( const SetupParam& param )
      { 
        this->SetCaptureSize( param );
        this->SetNoiseFilter( param );
        this->SetTrimming( param ); 
        this->SetFlip( param );
        this->SetFrameRate( param );
        this->SetEffect( param );
        this->SetLensCorrection( param );

        // �B�e���[�h
        if( param.photoMode == nn::camera::PHOTO_MODE_NORMAL )
        {
          this->SetSharpness( param );
          this->SetExposure( param );
          this->SetWhiteBalance( param );
          this->SetContrast( param );
        }
        else
        {
          // �B�e���[�h��ݒ肷���, sharpness, exposure, whitebalance, contrast ���㏑�������
          this->SetPhotoMode( param );
        }

        // �摜�f�[�^�̓]���T�C�Y
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          m_yuvTransferUnit = nn::camera::GetMaxBytes( param.trimmingWidth, param.trimmingHeight );  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          nn::camera::SetTransferBytes( m_port, m_yuvTransferUnit, param.trimmingWidth, param.trimmingHeight ); 
        }
        else
        {
          m_yuvTransferUnit = 0;
        }
      }


      /**
       * @brief �L���v�`������摜�̃T�C�Y��ݒ肷��
       */
      void CameraThread::SetCaptureSize( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetSize( m_select, param.captureSize ); 
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETSIZE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetSize( m_select, param.captureSize ); 
            }
#endif
          }
        }
      }

      /**
       * @brief �m�C�Y�t�B���^��ݒ肷��
       */
      void CameraThread::SetNoiseFilter( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetNoiseFilter( m_select, param.noiseFilterEnable );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETNOISEFILTER_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetNoiseFilter( m_select, param.noiseFilterEnable );
            }
#endif
          }
        }
      }

      /**
       * @brief �����I����ݒ肷��
       */
      void CameraThread::SetExposure( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetAutoExposure( m_select, param.autoExposureEnable );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT_STOP(0); //@check  // ���̃G���[�Ή��������Ă����̂Œǉ��B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETAUTOEXPOSURE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetAutoExposure( m_select, param.autoExposureEnable );
            }
#endif
            
            if( param.autoExposureEnable )
            {
              result = nn::camera::SetAutoExposureWindow( 
                  m_select, 
                  param.autoExposureRect_left,
                  param.autoExposureRect_top,
                  param.autoExposureRect_width,
                  param.autoExposureRect_height );
           
              if( result == nn::camera::ResultFatalError() ) {
                //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                *m_errorBitFlag |= ERROR_BIT_CAMERA_SETAUTOEXPOSUREWINDOW_FATAL_ERROR;
              }
              if(*m_errorBitFlag==ERROR_BIT_NONE)
              {
#ifdef BUGFIX_BTS6963_20130530
                while( result == nn::camera::CTR::ResultIsSleeping() ) {
                  //NTSC���100milli�͖ڈ�
                  nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
                  result = nn::camera::SetAutoExposureWindow( 
                    m_select, 
                    param.autoExposureRect_left,
                    param.autoExposureRect_top,
                    param.autoExposureRect_width,
                    param.autoExposureRect_height );
                }
#endif
              }
            }
          }
        }
      }

      /**
       * @brief �z���C�g�o�����X��ݒ肷��
       */
      void CameraThread::SetWhiteBalance( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetWhiteBalance( m_select, param.whiteBalance );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETWHITEBALANCE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetWhiteBalance( m_select, param.whiteBalance );
            }
#endif

            if( param.whiteBalance == nn::camera::WHITE_BALANCE_NORMAL ) 
            {
#if 0
              // �f�t�H���g�l�� true
              // true �ŏ㏑�����悤�Ƃ���Ɠ����Ŏ~�܂�
              result = nn::camera::SetAutoWhiteBalance( m_select, param.autoWhiteBalanceEnable );
           
              if( result == nn::camera::ResultFatalError() ) {
                GFL_ASSERT(0);
              }
#endif
              result = nn::camera::SetAutoWhiteBalance( m_select, false );
              if( result == nn::camera::ResultFatalError() ) {
                //GFL_ASSERT_STOP(0); //@check  // ���̃G���[�Ή��������Ă����̂Œǉ��B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                *m_errorBitFlag |= ERROR_BIT_CAMERA_SETAUTOWHITEBALANCE_FATAL_ERROR;
              }
              if(*m_errorBitFlag==ERROR_BIT_NONE)
              {
#ifdef BUGFIX_BTS6963_20130530
                while( result == nn::camera::CTR::ResultIsSleeping() ) {
                  //NTSC���100milli�͖ڈ�
                  nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
                  result = nn::camera::SetAutoWhiteBalance( m_select, false );
                }
#endif
           
                result = nn::camera::SetAutoWhiteBalanceWindow( 
                    m_select,
                    param.autoWhiteBalanceRect_left,
                    param.autoWhiteBalanceRect_top,
                    param.autoWhiteBalanceRect_width,
                    param.autoWhiteBalanceRect_height );
               
                if( result == nn::camera::ResultFatalError() ) {
                  //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                  *m_errorBitFlag |= ERROR_BIT_CAMERA_SETAUTOWHITEBALANCEWINDOW_FATAL_ERROR;
                }
                if(*m_errorBitFlag==ERROR_BIT_NONE)
                {
#ifdef BUGFIX_BTS6963_20130530
                  while( result == nn::camera::CTR::ResultIsSleeping() ) {
                    //NTSC���100milli�͖ڈ�
                    nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
                    result = nn::camera::SetAutoWhiteBalanceWindow( 
                        m_select,
                        param.autoWhiteBalanceRect_left,
                        param.autoWhiteBalanceRect_top,
                        param.autoWhiteBalanceRect_width,
                        param.autoWhiteBalanceRect_height );
                  }
#endif
                }
              }
            }
          }
        }
      }

      /**
       * @brief �g���~���O��ݒ肷��
       */
      void CameraThread::SetTrimming( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::camera::SetTrimming( m_port, true );
         
          nn::camera::SetTrimmingParamsCenter(
              m_port,
              static_cast< s16 >( param.trimmingWidth ),
              static_cast< s16 >( param.trimmingHeight ),
              param.captureWidth,
              param.captureHeight );
        }
      }

      /**
       * @brief �N���x��ݒ肷��
       */
      void CameraThread::SetSharpness( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          GFL_ASSERT( -4 <= param.sharpness ); //@check
          GFL_ASSERT( param.sharpness <= 5 ); //@check
          nn::Result result = nn::camera::SetSharpness( m_select, param.sharpness );
         
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETSHARPNESS_FATAL_ERROR;
          }
          
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetSharpness( m_select, param.sharpness );
            }
#endif
          }
        }
      }

      /**
       * @brief ���]������ݒ肷��
       */
      void CameraThread::SetFlip( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::FlipImage( m_select, param.flip );
         
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_FLIPIMAGE_FATAL_ERROR;
          }
          
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::FlipImage( m_select, param.flip );
            }
#endif
          }
        }
      }

      /**
       * @brief �t���[�����[�g��ݒ肷��
       */
      void CameraThread::SetFrameRate( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetFrameRate( m_select, param.frameRate );
         
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETFRAMERATE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetFrameRate( m_select, param.frameRate );
            }
#endif
          }
        }
      }

      /**
       * @brief �B�e���[�h��ݒ肷��
       */
      void CameraThread::SetPhotoMode( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetPhotoMode( m_select, param.photoMode );
         
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETPHOTOMODE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetPhotoMode( m_select, param.photoMode );
            }
#endif
          }
        }
      }

      /**
       * @brief �G�t�F�N�g��ݒ肷��
       */
      void CameraThread::SetEffect( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetEffect( m_select, param.effect );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETEFFECT_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetEffect( m_select, param.effect );
            }
#endif
          }
        }
      }

      /**
       * @brief �R���g���X�g��ݒ肷��
       */
      void CameraThread::SetContrast( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetContrast( m_select, param.contrast );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETCONTRAST_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetContrast( m_select, param.contrast );
            }
#endif
          }
        }
      }

      /**
       * @brief �����Y�␳�̒��x��ݒ肷��
       */
      void CameraThread::SetLensCorrection( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetLensCorrection( m_select, param.lensCorrection );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ������GFL_ASSERT_STOP�̂͂��Ȃ̂�GFL_ASSERT�̂܂ܖY����Ă����B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETLENSCORRECTION_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetLensCorrection( m_select, param.lensCorrection );
            }
#endif
          }
        }
      }




      //---------------------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //---------------------------------------------------------------------------------
      CameraThread::~CameraThread() 
      {
        m_threadEndEvent.Finalize();
        m_yuvReceiveEndEvent.Finalize();
        m_bufferErrorEvent.Finalize();
        m_vsyncEvent.Finalize();
        m_activateEvent.Finalize();
        m_captureStartEvent.Finalize();
        m_captureStopEvent.Finalize();

        m_CS_forCaptureSwitch.Finalize();
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief �J�������N������
       */
      //---------------------------------------------------------------------------------
      //
      void CameraThread::Activate()
      {
        m_activateEvent.Signal();
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�����J�n����
       */
      //---------------------------------------------------------------------------------
      void CameraThread::StartCapture()
      {
        m_captureStartEvent.Signal();
      }

      //---------------------------------------------------------------------------------
      /** 
       * @brief �L���v�`�����~����
       */
      //---------------------------------------------------------------------------------
      void CameraThread::StopCapture( void )
      {
        m_captureStopEvent.Signal();
      } 

      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�������ǂ����𒲂ׂ�
       */
      //---------------------------------------------------------------------------------
      bool CameraThread::IsBusy( void ) const
      {
#ifdef BUGFIX_BTS6907_20130528  // �J�����L���v�`�����ɓd�����j���[��\�������, �L���v�`�����I�����Ȃ��s��̏C��
        // nn::camera::IsBusy() ��, �L���v�`�����I�����Ă��Ă���� true ��Ԃ���ԂɂȂ邱�Ƃ�����܂��B
        // ���̏ꍇ, �^�C���A�E�g�����Ȃ�L���v�`�����I�����Ă���Ƃ݂Ȃ��܂��B
        if( m_isCaptureStoppedByTimeout ) {
          return false;
        }
#endif
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          return nn::camera::IsBusy( m_port );
        }
        else
        {
          return false;
        }
      }

      //-------------------------------------------------------------------------------
      /**
       * @brief �I����ݒ肵�܂��B
       * @param[in] exposure  �I�o�ʂ��w�肵�܂��B�w��\�Ȓl�͈̔͂� [-5, +5] �ł��B
       */
      //-------------------------------------------------------------------------------
      void CameraThread::SetExposureEx(s8 exposure)
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetExposure( m_select, exposure );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT_STOP(0); //@check  // ���̊֐��͐V�K�ǉ��֐��B�Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETEXPOSURE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetExposure( m_select, exposure );
            }
#endif
          }
        }
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �X���b�h����������
       */
      //---------------------------------------------------------------------------------
      void CameraThread::Initialize( void )
      {
        this->SetNextFrameReceiving(); // YUV �f�[�^��M�J�n
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �X���b�h�I������
       */
      //---------------------------------------------------------------------------------
      void CameraThread::Finalize( void )
      {
      } 


      //---------------------------------------------------------------------------------
      /**
       * @brief �X���b�h�j�����̏���
       */
      //---------------------------------------------------------------------------------
      void CameraThread::OnKill( void )
      {
        m_threadEndEvent.Signal();
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �X���b�h���C������
       */
      //---------------------------------------------------------------------------------
      void CameraThread::Main( void )
      {
        if( m_threadEndFlag )
        {
          nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( 50 ) );  // SBTS[2574]: �y�Ƃ܂�FQR�R�[�h�ǂݎ���ʂŃJ�����@�\����~���邱�Ƃ�����A���̏�ԂŖ߂낤�Ƃ���ƂƂ܂�z  //sango,2014/06/25,kawada_koji
        }
        else
        {
          s32 event_idx = -1;

          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            nn::os::WaitObject * p_events[7] =
            {
              &m_threadEndEvent,      // �X���b�h�I��
              &m_yuvReceiveEndEvent,  // 1�t���[�����̓]������
              &m_bufferErrorEvent,    // �o�b�t�@�G���[
              &m_vsyncEvent,          // �J���� V �u�����N
              &m_captureStopEvent,    // �L���v�`���̒�~
              &m_activateEvent,       // �J�����̋N��
              &m_captureStartEvent,   // �L���v�`���J�n
            };
         
            event_idx = nn::os::WaitObject::WaitAny( p_events, 7 );
          }
          else
          {
            // SBTS[2574]: �y�Ƃ܂�FQR�R�[�h�ǂݎ���ʂŃJ�����@�\����~���邱�Ƃ�����A���̏�ԂŖ߂낤�Ƃ���ƂƂ܂�z  //sango,2014/06/26,kawada_koji
            // �G���[���N���Ă���Ƃ��ɁAm_yuvReceiveEndEvent���V�O�i����Ԃ̂܂܂ɂȂ邱�Ƃ����蓾�����Ȃ̂ŁAm_yuvReceiveEndEvent�͔��肩��O���B

            nn::os::WaitObject * p_events[6] =
            {
              &m_threadEndEvent,      // �X���b�h�I��
              &m_bufferErrorEvent,    // �o�b�t�@�G���[
              &m_vsyncEvent,          // �J���� V �u�����N
              &m_captureStopEvent,    // �L���v�`���̒�~
              &m_activateEvent,       // �J�����̋N��
              &m_captureStartEvent,   // �L���v�`���J�n
            };
         
            event_idx = nn::os::WaitObject::WaitAny( p_events, 6 );
            if(event_idx >= 1) event_idx += 1;
          }
         
          if( event_idx == 0 ) // �X���b�h�I��
          {
            this->StopCaptureAndWaitCaptureStopExclusively();
            m_threadEndFlag = true;
          }
          else if( event_idx == 1 ) // 1�t���[�����̓]������
          {
            m_pY2rStartEvent->Signal(); // �]��������ʒm
            this->SetNextFrameReceiving();
          }
          else if( event_idx == 2 ) // �o�b�t�@�G���[
          {
            this->SetNextFrameReceiving();
            this->StartCaptureExclusively();
          }
          else if( event_idx == 3 ) // �J���� V �u�����N
          {
            this->VsyncFunc();
          }
          else if( event_idx == 4 ) // �L���v�`���̒�~
          {
            this->StopCaptureAndWaitCaptureStopExclusively();
          }
          else if( event_idx == 5 ) // �J�����̋N��
          {
            if(*m_errorBitFlag==ERROR_BIT_NONE)
            {
              nn::Result result = nn::camera::Activate( m_select );
              if( result == nn::camera::ResultFatalError() ) {
                //GFL_ASSERT_STOP(0);  // �Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                *m_errorBitFlag |= ERROR_BIT_CAMERA_ACTIVATE_FATAL_ERROR;
              }
              if(*m_errorBitFlag==ERROR_BIT_NONE)
              {
#ifdef BUGFIX_BTS6963_20130530
                while( result == nn::camera::CTR::ResultIsSleeping() ) {
                  //NTSC���100milli�͖ڈ�
                  nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
                  result = nn::camera::Activate( m_select );
                }
#endif
              }
            }
          }
          else if( event_idx == 6 ) // �L���v�`���J�n
          {
            this->SetNextFrameReceiving();
            this->StartCaptureExclusively();
          }
          else
          {
            GFL_PRINT( "Illegal event number.\n" );
            GFL_ASSERT(0); //@check
          }
        }
      }


      /** 
       * @brief 1�t���[�����̃f�[�^�]������
       */
      void CameraThread::SetNextFrameReceiving( void )
      { 
        // �]����� YUV �o�b�t�@��؂�ւ���
        m_pBuffers->SwitchReceivingYuvBuffer();

        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // ���̃t���[���̓]���ݒ�
          nn::camera::SetReceiving(
              &m_yuvReceiveEndEvent,
              m_pBuffers->GetYuvBuffer_Receiving(),
              m_port,
              m_pBuffers->GetYuvBufferSize(),
              m_yuvTransferUnit );
        }
      }


      /** 
       * @brief �L���v�`���̊J�n��r���I�ɍs��
       */
      void CameraThread::StartCaptureExclusively( void )
      {
        // �L���v�`���̊J�n�E��~�͔r���I�ɍs��
        nn::os::CriticalSection::ScopedLock sl( m_CS_forCaptureSwitch ); 
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::camera::StartCapture( m_port );
        }
        
#ifdef BUGFIX_BTS6907_20130528  // �J�����L���v�`�����ɓd�����j���[��\�������, �L���v�`�����I�����Ȃ��s��̏C��
        m_isCaptureStoppedByTimeout = false;
#endif
      }


      /**
       * @brief �L���v�`���̒�~�ƒ�~�҂���r���I�ɍs��
       */
      void CameraThread::StopCaptureAndWaitCaptureStopExclusively( void )
      {
        // �L���v�`���̊J�n�E��~�͔r���I�ɍs��
        nn::os::CriticalSection::ScopedLock sl( m_CS_forCaptureSwitch ); 

        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::camera::StopCapture( m_port );

#ifdef BUGFIX_BTS6907_20130528  // �J�����L���v�`�����ɓd�����j���[��\�������, �L���v�`�����I�����Ȃ��s��̏C��
        
          // �� nn::camera::IsBusy() �̃��t�@�����X���
          // �unn::camera::CTR::StopCapture �֐��̌Ăяo���̑O�ɁAnn::camera::CTR::Activate( SELECT_NONE ) �ɂ���ăJ�������~�����ꍇ�A
          // �@nn::camera::CTR::Activate �֐��̌Ăяo���̃^�C�~���O�ɂ���ẮA �{�֐������ true ��Ԃ���ԂɂȂ�\��������܂��B�v
          // �A�v���b�g�̋N����W���Ȃǂɂ��, �J�����������I�ɒ�~�����ꍇ, ��L�̃P�[�X�ɊY�����܂��B
          // �ʏ�, nn::camera::StopCapture() �̌�� nn::camera::IsBusy() �� false ��Ԃ��͍̂ő�ł� 1�J�����t���[���Ȃ̂�, 
          // �ł��������t���[�����[�g�ł��� 5fps ��z�肵, 200ms ��Ƀ^�C���A�E�g�����܂�.
          s32 waitCount = 0;
          while( nn::camera::IsBusy( m_port ) )
          {
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( 1 ) );
            
            ++waitCount;
            if( 200 < waitCount ) {
              m_isCaptureStoppedByTimeout = true;
              break;
            }
          }
#else
          while( nn::camera::IsBusy( m_port ) ) 
          {
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( 1 ) );
          }
#endif
        }
        else
        {
          // �G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
#ifdef BUGFIX_BTS6907_20130528  // �J�����L���v�`�����ɓd�����j���[��\�������, �L���v�`�����I�����Ȃ��s��̏C��
          m_isCaptureStoppedByTimeout = true;
#else
          // ������ł͉������Ȃ�
#endif
        }
      }


      /**
       * @brief �J����Vsync����
       */
      void CameraThread::VsyncFunc( void )
      {
      }




#if GFL_DEBUG
      // FatalError���N�����Ƃ��̏󋵂�����o���t���O
      void CameraThread::DebugSetErrorBit(void)
      {
        *m_errorBitFlag |= ERROR_BIT_DEBUG_FATAL_ERROR_OCCUR;
      }
#endif



    } // namespace camera
  } // namespace device
} // namespace gfl 
