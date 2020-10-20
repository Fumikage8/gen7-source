/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_System.cpp
 * @brief  カメラシステム
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
       * @brief カメラデバイスの情報
       *
       * DeviceID から
       * nn::camera::CameraSelect, nn::camera::Port は一意に決まる
       */
      struct DeviceInfo { 
        nn::camera::CameraSelect select; // カメラの組み合わせ
        nn::camera::Port         port;   // カメラの接続先ポート
      };

      const DeviceInfo g_DeviceInfo[ DEVICE_ID_NUM ] = 
      {
        { nn::camera::SELECT_IN1,  nn::camera::PORT_CAM1 }, // DEVICE_ID_IN
        { nn::camera::SELECT_OUT2, nn::camera::PORT_CAM2 }, // DEVICE_ID_OUT_L
        { nn::camera::SELECT_OUT1, nn::camera::PORT_CAM1 }, // DEVICE_ID_OUT_R
      };


      /**
       * @brief 解像度の幅を取得する
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
       * @brief 解像度の高さを取得する
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
      // FatalErrorが起きたときの状況をつくり出すフラグ
      bool System::s_debugFatalErrorOccurFlag  = false;
#endif




      //---------------------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       *
       * SBTS[2572]: カメラ起動中はスリープしてはならない。詳しい解説はgflib_cpp/gflib/prog/include/device/camera/gfl_DeviceCamera_System.hに。
       */
      //---------------------------------------------------------------------------------
      System::System( const SetupParam& param ) :
        m_pHeap( param.pHeap ),
        m_pDeviceHeap( param.pDeviceHeap ),
        m_errorBitFlag(ERROR_BIT_NONE)
      {
        this->InitializeLibrary();

        // Y2R -> RGB 変換依頼イベントを初期化
        for( int i=0; i<CAMERA_NUM; i++ )
        {
          m_y2rStartEvent[i].Initialize( false ); 
        }

        this->InitBuffers( param );
        this->InitY2rThread( param );
        this->InitCameraThreads( param );
        this->InitCameraControllers( param );


        // Y2Rスレッド起動
        if(m_y2rThread) m_y2rThread->Start( param.y2rThreadPriority );

        // カメラスレッド起動
        for( int i=0; i<CAMERA_NUM; i++ )
        {
          if( m_cameraThread[i] ) {
            m_cameraThread[i]->Start( param.cameraThreadPriority[i] );
          }
        }


#if 0
        // カメラが被写体を撮影するタイミングを同期させます。
        // CTR ではカメラは独立に動作する構成になっているため、
        // フレームレートの設定を同じにしていたとしても、カメラが被写体を撮影するタイミングが異なります。
        // これが大きいと、左右の被写体の動きがズレます。
        // そこで、SynchronizeVsyncTiming 関数によって、被写体を撮影するタイミングをできる同じタイミングにします。
        // この関数を使用すると、タイミングのズレを数 100 μsec 程度に抑えることができます(Releaseビルド時)。
        // この関数はカメラの起動中に呼び出す必要があります。
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
       * @brief カメラライブラリを初期化する
       */
      void System::InitializeLibrary( void )
      {
        nn::Result result;

#if GFL_DEBUG 
        // FatalErrorが起きたときの状況をつくり出すフラグ
        if( s_debugFatalErrorOccurFlag )
        {
          m_errorBitFlag |= ERROR_BIT_CAMERA_INITIALIZE_FATAL_ERROR;
        }
        else
        {
#endif

        result = nn::camera::Initialize();
        if( result == nn::camera::ResultFatalError() ) {
          //GFL_ASSERT_STOP(0);  // ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          m_errorBitFlag |= ERROR_BIT_CAMERA_INITIALIZE_FATAL_ERROR;
        }

#if GFL_DEBUG
        }  // if( s_debugFatalErrorOccurFlag )
#endif

        if(m_errorBitFlag==ERROR_BIT_NONE)
        {
#ifdef BUGFIX_BTS6963_20130530
          while( result == nn::camera::CTR::ResultIsSleeping() ) {
            //NTSCより100milliは目安
            nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
            result = nn::camera::Initialize();
          }
#endif
        }

        bool y2rResult = nn::y2r::Initialize();
        s32  y2rCount = 0;
        while( !y2rResult )
        {
          // CTR-SDKの関数リファレンスより1ミリ秒の待ちを挟んで再度呼ぶ
          nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(1));
          y2rResult = nn::y2r::Initialize();
          ++y2rCount;
          if(y2rCount >= 1000) break;  // 1秒待ってダメだったらあきらめる
        }
        if(!y2rResult)
        {
          m_errorBitFlag |= ERROR_BIT_Y2R_INITIALIZE_ERROR;
        }
      }


      //====================================
      /**
       * @brief 各カメラのバッファを初期化する
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
       * @breif カメラのバッファを生成する
       */
      BufferManager* System::CreateBuffer( const CameraParam& param )
      {
        GFL_ASSERT( param.isValid );  //@check

        gfl::device::camera::BufferManager::SetupParam buf_param;
        buf_param.pHeap         = m_pHeap;
        buf_param.yuvBufferNum  = 3;
        if(m_errorBitFlag==ERROR_BIT_NONE)
        {
          buf_param.yuvBufferSize = nn::camera::GetFrameBytes( param.trimmingWidth, param.trimmingHeight );  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          buf_param.rgbBufferSize = nn::y2r::GetOutputImageSize( param.outputWidth, param.outputHeight, static_cast<nn::y2r::OutputFormat>(param.outputFormat) );  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
        }
        else
        {
          buf_param.yuvBufferSize = param.trimmingWidth * param.trimmingHeight * 4;  // @todo CTRでない場合要注意！ 注意喚起のためこのtodoは残しておきます。  // CTR CTRプログラミングマニュアル システム編 2014-01-21 Ver.7.0  カメラのキャプチャ画像は YUV 一括のフォーマットでのみ取得することができますので、ほとんどの場合、キャプチャ画像の変換時には INPUT_YUV422_BATCH を指定することになります。
          buf_param.rgbBufferSize = param.outputWidth * param.outputHeight * OUTPUT_BIT_PER_PIXEL[param.outputFormat]/8;
        }

        return GFL_NEW(m_pHeap) gfl::device::camera::BufferManager( buf_param );
      }


      //====================================
      /**
       * @brief Y2Rスレッドを初期化する
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
            thread_param.convertParam[i].outputOffset         = nn::y2r::GetOutputImageSize(  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
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
       * @brief カメラスレッドを初期化する
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
       * @brief カメラスレッドを生成する
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

        // 内側カメラは左右反転の指定が逆になる
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
       * @brief カメラのアクセッサを初期化する
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
       * @brief カメラのアクセッサを生成する
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
       * @brief デストラクタ
       */
      //---------------------------------------------------------------------------------
      System::~System()
      {
        this->DeleteCameraController();

        // スレッド停止・破棄
        this->KillAndDeleteY2rThread();
        this->KillAndDeleteCameraThreads();

        // Y2R -> RGB 変換依頼イベントを破棄
        for( int i=0; i < CAMERA_NUM; i++ )
        {
          m_y2rStartEvent[i].Finalize();
        }

        this->DeleteBuffers(); 
        this->FinalizeLibrary();
      }


      /**
       * @brief カメラのアクセッサを破棄する
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
       * @brief Y2Rスレッドを破棄する
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
       * @brief カメラスレッドを破棄する
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
       * @brief 各カメラのバッファを破棄する
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
       * @brief カメラライブラリを終了する
       */
      void System::FinalizeLibrary( void )
      {
        nn::camera::Finalize();  // Initializeできていなくても呼んで大丈夫だった。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
        nn::y2r::Finalize();  // Initializeできていなくても呼んで大丈夫だった。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief カメラへのアクセッサを取得する
       * @param cameraID  取得対象のカメラ
       * @retval NULL  指定したカメラが存在しない場合
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
       * @brief カメラを起動する
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
       * @brief すべてのカメラを停止させる
       */
      //---------------------------------------------------------------------------------
      void System::DeactivateAllCamera( void )
      {
        // すべてのカメラのキャプチャを停止させる
        for( int i=0; i<CAMERA_NUM; i++ ) {
          if( m_cameraThread[i] ) {
            m_cameraThread[i]->StopCapture();
          }
        }

        // キャプチャの停止完了を待つ
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

        // 全カメラを停止させる
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
       * @brief キャプチャを開始する
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
       * @brief キャプチャを停止する
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
       * @brief キャプチャ中かどうかを調べる
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
       * @brief バッファを取得する
       */
      //---------------------------------------------------------------------------------
      const BufferManager* System::GetBufferManager( CameraID cameraID ) const
      {
        return m_buffer[ cameraID ];
      }


      //-------------------------------------------------------------------------------
      /**
       * @brief 露光を設定します。
       * @param[in] exposure  露出量を指定します。指定可能な値の範囲は [-5, +5] です。
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
       * @brief シャッター音の再生とカメラLEDの一時消灯を行います。
       */
      //-------------------------------------------------------------------------------
      void System::PlayShutterSound( ShutterSoundType type )
      {
        //if(m_errorBitFlag==ERROR_BIT_NONE)  // PlayShutterSoundはエラー中でも呼べるようなので、ifはコメントアウトしておく。
        {
          nn::Result result = nn::camera::PlayShutterSound( static_cast<nn::camera::ShutterSoundType>(type) );
          if( result == nn::camera::ResultFatalError() ) {
            m_errorBitFlag |= ERROR_BIT_CAMERA_PLAYSHUTTERSOUND_FATAL_ERROR;
          }
        }
      }


#if GFL_DEBUG
      // FatalErrorが起きたときの状況をつくり出すフラグ
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
                break;  // どこかで立てたらもう立てなくていいのでループを抜ける。
              }
            }
          }
        }
      }
#endif


    } // namespace camera
  } // namespace device
} // namespace gfl
