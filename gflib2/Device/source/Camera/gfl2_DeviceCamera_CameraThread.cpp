#if GF_PLATFORM_CTR
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl2_DeviceCamera_CameraThread.cpp
 * @brief  カメラ1機の処理をするスレッド
 * @author obata_toshihoro
 * @date   2010.11.26
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <thread/include/gfl2_Thread.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "gfl2_DeviceCamera_BufferManager.h"
#include "gfl2_DeviceCamera_CameraThread.h"
#include <Device/include/Camera/gfl2_DeviceCamera_Types.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(device)
GFL_NAMESPACE_BEGIN(camera)


      //---------------------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //---------------------------------------------------------------------------------
      CameraThread::CameraThread( const SetupParam& param ) :
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
        m_isCaptureStoppedByTimeout( false ),
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

        m_pThread = GFL_NEW( param.pHeap ) gfl2::thread::Thread( this, param.pHeap, true, STACK_SIZE );
      }


      /**
       * @brief カメラを初期化する
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

        // 撮影モード
        if( param.photoMode == nn::camera::PHOTO_MODE_NORMAL )
        {
          this->SetSharpness( param );
          this->SetExposure( param );
          this->SetWhiteBalance( param );
          this->SetContrast( param );
        }
        else
        {
          // 撮影モードを設定すると, sharpness, exposure, whitebalance, contrast が上書きされる
          this->SetPhotoMode( param );
        }

        // 画像データの転送サイズ
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          m_yuvTransferUnit = nn::camera::GetMaxBytes( param.trimmingWidth, param.trimmingHeight );  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          nn::camera::SetTransferBytes( m_port, m_yuvTransferUnit, param.trimmingWidth, param.trimmingHeight ); 
        }
        else
        {
          m_yuvTransferUnit = 0;
        }
      }


      /**
       * @brief キャプチャする画像のサイズを設定する
       */
      void CameraThread::SetCaptureSize( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetSize( m_select, param.captureSize ); 
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETSIZE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetSize( m_select, param.captureSize ); 
            }
          }
        }
      }

      /**
       * @brief ノイズフィルタを設定する
       */
      void CameraThread::SetNoiseFilter( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetNoiseFilter( m_select, param.noiseFilterEnable );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETNOISEFILTER_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetNoiseFilter( m_select, param.noiseFilterEnable );
            }
          }
        }
      }

      /**
       * @brief 自動露光を設定する
       */
      void CameraThread::SetExposure( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetAutoExposure( m_select, param.autoExposureEnable );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT_STOP(0); //@check  // このエラー対応が抜けていたので追加。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETAUTOEXPOSURE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetAutoExposure( m_select, param.autoExposureEnable );
            }
            
            if( param.autoExposureEnable )
            {
              result = nn::camera::SetAutoExposureWindow( 
                  m_select, 
                  param.autoExposureRect_left,
                  param.autoExposureRect_top,
                  param.autoExposureRect_width,
                  param.autoExposureRect_height );
           
              if( result == nn::camera::ResultFatalError() ) {
                //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                *m_errorBitFlag |= ERROR_BIT_CAMERA_SETAUTOEXPOSUREWINDOW_FATAL_ERROR;
              }
              if(*m_errorBitFlag==ERROR_BIT_NONE)
              {
                while( result == nn::camera::CTR::ResultIsSleeping() ) {
                  //NTSCより100milliは目安
                  nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
                  result = nn::camera::SetAutoExposureWindow( 
                    m_select, 
                    param.autoExposureRect_left,
                    param.autoExposureRect_top,
                    param.autoExposureRect_width,
                    param.autoExposureRect_height );
                }
              }
            }
          }
        }
      }

      /**
       * @brief ホワイトバランスを設定する
       */
      void CameraThread::SetWhiteBalance( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetWhiteBalance( m_select, param.whiteBalance );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETWHITEBALANCE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetWhiteBalance( m_select, param.whiteBalance );
            }

            if( param.whiteBalance == nn::camera::WHITE_BALANCE_NORMAL ) 
            {
#if 0
              // デフォルト値は true
              // true で上書きしようとすると内部で止まる
              result = nn::camera::SetAutoWhiteBalance( m_select, param.autoWhiteBalanceEnable );
           
              if( result == nn::camera::ResultFatalError() ) {
                GFL_ASSERT(0);
              }
#endif
              result = nn::camera::SetAutoWhiteBalance( m_select, false );
              if( result == nn::camera::ResultFatalError() ) {
                //GFL_ASSERT_STOP(0); //@check  // このエラー対応が抜けていたので追加。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                *m_errorBitFlag |= ERROR_BIT_CAMERA_SETAUTOWHITEBALANCE_FATAL_ERROR;
              }
              if(*m_errorBitFlag==ERROR_BIT_NONE)
              {
                while( result == nn::camera::CTR::ResultIsSleeping() ) {
                  //NTSCより100milliは目安
                  nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
                  result = nn::camera::SetAutoWhiteBalance( m_select, false );
                }
           
                result = nn::camera::SetAutoWhiteBalanceWindow( 
                    m_select,
                    param.autoWhiteBalanceRect_left,
                    param.autoWhiteBalanceRect_top,
                    param.autoWhiteBalanceRect_width,
                    param.autoWhiteBalanceRect_height );
               
                if( result == nn::camera::ResultFatalError() ) {
                  //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                  *m_errorBitFlag |= ERROR_BIT_CAMERA_SETAUTOWHITEBALANCEWINDOW_FATAL_ERROR;
                }
                if(*m_errorBitFlag==ERROR_BIT_NONE)
                {
                  while( result == nn::camera::CTR::ResultIsSleeping() ) {
                    //NTSCより100milliは目安
                    nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
                    result = nn::camera::SetAutoWhiteBalanceWindow( 
                        m_select,
                        param.autoWhiteBalanceRect_left,
                        param.autoWhiteBalanceRect_top,
                        param.autoWhiteBalanceRect_width,
                        param.autoWhiteBalanceRect_height );
                  }
                }
              }
            }
          }
        }
      }

      /**
       * @brief トリミングを設定する
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
       * @brief 鮮明度を設定する
       */
      void CameraThread::SetSharpness( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          GFL_ASSERT( -4 <= param.sharpness ); //@check
          GFL_ASSERT( param.sharpness <= 5 ); //@check
          nn::Result result = nn::camera::SetSharpness( m_select, param.sharpness );
         
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETSHARPNESS_FATAL_ERROR;
          }
          
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetSharpness( m_select, param.sharpness );
            }
          }
        }
      }

      /**
       * @brief 反転処理を設定する
       */
      void CameraThread::SetFlip( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::FlipImage( m_select, param.flip );
         
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_FLIPIMAGE_FATAL_ERROR;
          }
          
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::FlipImage( m_select, param.flip );
            }
          }
        }
      }

      /**
       * @brief フレームレートを設定する
       */
      void CameraThread::SetFrameRate( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetFrameRate( m_select, param.frameRate );
         
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETFRAMERATE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetFrameRate( m_select, param.frameRate );
            }
          }
        }
      }

      /**
       * @brief 撮影モードを設定する
       */
      void CameraThread::SetPhotoMode( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetPhotoMode( m_select, param.photoMode );
         
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETPHOTOMODE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetPhotoMode( m_select, param.photoMode );
            }
          }
        }
      }

      /**
       * @brief エフェクトを設定する
       */
      void CameraThread::SetEffect( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetEffect( m_select, param.effect );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETEFFECT_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetEffect( m_select, param.effect );
            }
          }
        }
      }

      /**
       * @brief コントラストを設定する
       */
      void CameraThread::SetContrast( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetContrast( m_select, param.contrast );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETCONTRAST_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetContrast( m_select, param.contrast );
            }
          }
        }
      }

      /**
       * @brief レンズ補正の程度を設定する
       */
      void CameraThread::SetLensCorrection( const SetupParam& param )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetLensCorrection( m_select, param.lensCorrection );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT(0); //@check  // ここもGFL_ASSERT_STOPのはずなのにGFL_ASSERTのまま忘れられていた。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETLENSCORRECTION_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetLensCorrection( m_select, param.lensCorrection );
            }
          }
        }
      }




      //---------------------------------------------------------------------------------
      /**
       * @brief デストラクタ
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

        GFL_SAFE_DELETE( m_pThread );
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief スレッド開始
       */
      //---------------------------------------------------------------------------------
      void CameraThread::Start( int no )
      {
        m_pThread->Start( no );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief プライオリティの設定
       */
      //---------------------------------------------------------------------------------
      void CameraThread::SetPriority( int no )
      {
        m_pThread->SetPriority( no );
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief カメラを起動する
       */
      //---------------------------------------------------------------------------------
      //
      void CameraThread::Activate()
      {
        m_activateEvent.Signal();
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャを開始する
       */
      //---------------------------------------------------------------------------------
      void CameraThread::StartCapture()
      {
        m_captureStartEvent.Signal();
      }

      //---------------------------------------------------------------------------------
      /** 
       * @brief キャプチャを停止する
       */
      //---------------------------------------------------------------------------------
      void CameraThread::StopCapture( void )
      {
        m_captureStopEvent.Signal();
      } 

      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャ中かどうかを調べる
       */
      //---------------------------------------------------------------------------------
      bool CameraThread::IsBusy( void ) const
      {
        // nn::camera::IsBusy() は, キャプチャが終了していても常に true を返す状態になることがあります。
        // その場合, タイムアウトしたならキャプチャが終了しているとみなします。
        if( m_isCaptureStoppedByTimeout ) {
          return false;
        }
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
       * @brief 露光を設定します。
       * @param[in] exposure  露出量を指定します。指定可能な値の範囲は [-5, +5] です。
       */
      //-------------------------------------------------------------------------------
      void CameraThread::SetExposureEx(s8 exposure)
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::SetExposure( m_select, exposure );
          if( result == nn::camera::ResultFatalError() ) {
            //GFL_ASSERT_STOP(0); //@check  // この関数は新規追加関数。ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_SETEXPOSURE_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::SetExposure( m_select, exposure );
            }
          }
        }
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief スレッド初期化処理
       */
      //---------------------------------------------------------------------------------
      void CameraThread::Initialize( void )
      {
        this->SetNextFrameReceiving(); // YUV データ受信開始
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief スレッド終了処理
       */
      //---------------------------------------------------------------------------------
      void CameraThread::Finalize( void )
      {
      } 


      //---------------------------------------------------------------------------------
      /**
       * @brief スレッド破棄時の処理
       */
      //---------------------------------------------------------------------------------
      void CameraThread::Kill( void )
      {
        m_pThread->Kill();
        m_threadEndEvent.Signal();
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief スレッドメイン処理
       */
      //---------------------------------------------------------------------------------
      void CameraThread::Function( void )
      {
        Initialize();

        if( m_threadEndFlag )
        {
          nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( 50 ) );  // SBTS[2574]: 【とまり：QRコード読み取り画面でカメラ機能が停止することがあり、その状態で戻ろうとするととまる】  //sango,2014/06/25,kawada_koji
        }
        else
        {
          s32 event_idx = -1;

          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            nn::os::WaitObject * p_events[7] =
            {
              &m_threadEndEvent,      // スレッド終了
              &m_yuvReceiveEndEvent,  // 1フレーム分の転送完了
              &m_bufferErrorEvent,    // バッファエラー
              &m_vsyncEvent,          // カメラ V ブランク
              &m_captureStopEvent,    // キャプチャの停止
              &m_activateEvent,       // カメラの起動
              &m_captureStartEvent,   // キャプチャ開始
            };
         
            event_idx = nn::os::WaitObject::WaitAny( p_events, 7 );
          }
          else
          {
            // SBTS[2574]: 【とまり：QRコード読み取り画面でカメラ機能が停止することがあり、その状態で戻ろうとするととまる】  //sango,2014/06/26,kawada_koji
            // エラーが起きているときに、m_yuvReceiveEndEventがシグナル状態のままになることがあり得そうなので、m_yuvReceiveEndEventは判定から外す。

            nn::os::WaitObject * p_events[6] =
            {
              &m_threadEndEvent,      // スレッド終了
              &m_bufferErrorEvent,    // バッファエラー
              &m_vsyncEvent,          // カメラ V ブランク
              &m_captureStopEvent,    // キャプチャの停止
              &m_activateEvent,       // カメラの起動
              &m_captureStartEvent,   // キャプチャ開始
            };
         
            event_idx = nn::os::WaitObject::WaitAny( p_events, 6 );
            if(event_idx >= 1) event_idx += 1;
          }
         
          if( event_idx == 0 ) // スレッド終了
          {
            this->StopCaptureAndWaitCaptureStopExclusively();
            m_threadEndFlag = true;
          }
          else if( event_idx == 1 ) // 1フレーム分の転送完了
          {
            m_pY2rStartEvent->Signal(); // 転送完了を通知
            this->SetNextFrameReceiving();
          }
          else if( event_idx == 2 ) // バッファエラー
          {
            this->SetNextFrameReceiving();
            this->StartCaptureExclusively();
          }
          else if( event_idx == 3 ) // カメラ V ブランク
          {
            this->VsyncFunc();
          }
          else if( event_idx == 4 ) // キャプチャの停止
          {
            this->StopCaptureAndWaitCaptureStopExclusively();
          }
          else if( event_idx == 5 ) // カメラの起動
          {
            if(*m_errorBitFlag==ERROR_BIT_NONE)
            {
              nn::Result result = nn::camera::Activate( m_select );
              if( result == nn::camera::ResultFatalError() ) {
                //GFL_ASSERT_STOP(0);  // ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                *m_errorBitFlag |= ERROR_BIT_CAMERA_ACTIVATE_FATAL_ERROR;
              }
              if(*m_errorBitFlag==ERROR_BIT_NONE)
              {
                while( result == nn::camera::CTR::ResultIsSleeping() ) {
                  //NTSCより100milliは目安
                  nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
                  result = nn::camera::Activate( m_select );
                }
              }
            }
          }
          else if( event_idx == 6 ) // キャプチャ開始
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
       * @brief 1フレーム分のデータ転送処理
       */
      void CameraThread::SetNextFrameReceiving( void )
      { 
        // 転送先の YUV バッファを切り替える
        m_pBuffers->SwitchReceivingYuvBuffer();

        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // 次のフレームの転送設定
          nn::camera::SetReceiving(
              &m_yuvReceiveEndEvent,
              m_pBuffers->GetYuvBuffer_Receiving(),
              m_port,
              m_pBuffers->GetYuvBufferSize(),
              m_yuvTransferUnit );
        }
      }


      /** 
       * @brief キャプチャの開始を排他的に行う
       */
      void CameraThread::StartCaptureExclusively( void )
      {
        // キャプチャの開始・停止は排他的に行う
        nn::os::CriticalSection::ScopedLock sl( m_CS_forCaptureSwitch ); 
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::camera::StartCapture( m_port );
        }
        
        m_isCaptureStoppedByTimeout = false;
      }


      /**
       * @brief キャプチャの停止と停止待ちを排他的に行う
       */
      void CameraThread::StopCaptureAndWaitCaptureStopExclusively( void )
      {
        // キャプチャの開始・停止は排他的に行う
        nn::os::CriticalSection::ScopedLock sl( m_CS_forCaptureSwitch ); 

        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::camera::StopCapture( m_port );

        
          // ※ nn::camera::IsBusy() のリファレンスより
          // 「nn::camera::CTR::StopCapture 関数の呼び出しの前に、nn::camera::CTR::Activate( SELECT_NONE ) によってカメラを停止した場合、
          // 　nn::camera::CTR::Activate 関数の呼び出しのタイミングによっては、 本関数が常に true を返す状態になる可能性があります。」
          // アプレットの起動や蓋閉じなどにより, カメラが強制的に停止した場合, 上記のケースに該当します。
          // 通常, nn::camera::StopCapture() の後で nn::camera::IsBusy() が false を返すのは最大でも 1カメラフレームなので, 
          // 最も小さいフレームレートである 5fps を想定し, 200ms 後にタイムアウトさせます.
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
        }
        else
        {
          // エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          m_isCaptureStoppedByTimeout = true;
        }
      }


      /**
       * @brief カメラVsync処理
       */
      void CameraThread::VsyncFunc( void )
      {
      }




#if GFL_DEBUG
      // FatalErrorが起きたときの状況をつくり出すフラグ
      void CameraThread::DebugSetErrorBit(void)
      {
        *m_errorBitFlag |= ERROR_BIT_DEBUG_FATAL_ERROR_OCCUR;
      }
#endif



GFL_NAMESPACE_END(camera)
GFL_NAMESPACE_END(device)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
