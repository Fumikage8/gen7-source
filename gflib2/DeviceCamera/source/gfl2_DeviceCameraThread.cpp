//==============================================================================
/**
 * @file   gfl2_DeviceCameraThread.cpp
 * @date   2013/03/12 Tue.
 * @date   2015/07/30 Thu. 15:01:09 旧プロジェクトより移植
 * @author muto_yu
 * @brief  
 * @attention 2016/06現在ステレオ撮影には対応していません。対応させるには
 *              - 両CaptureImageの時刻同期（起動時のV同期は取っていますが、両眼の画像が揃っているかの担保等が不十分）
 *              - ロールの調整
 *              - 焦点の調整
 *            などの実装が必要です
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================
#if defined(GF_PLATFORM_CTR)


// =============================================================================
/**/
//==============================================================================
#include  "DeviceCamera/include/gfl2_DeviceCameraThread.h"


namespace  gfl2  {
namespace  devicecamera  {
// =============================================================================
/**/
//==============================================================================
//  #define CAM_LOG                 GFL_PRINT
  #define CAM_LOG(format, ...)


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DeviceCameraThread::DeviceCameraThread(void) :
   mConfig()
  ,mpThreadStack(NULL)
  ,mThread()
  ,mState(STATE_STANDBY)
  ,mContext()
  ,mY2RCoef()
  ,mIsY2RCoefAvailable(false)
  ,mIsFatalError(false)
#if GFL_DEBUG
  ,m_IsForceFatal(false)
#endif
{
  for(u32 num = 0; num < GFL_NELEMS(mEvent); ++num)  {
    mEvent[num].Initialize(false);
  }
  for(u32 num = 0; num < GFL_NELEMS(mLightEvent); ++num)  {
    mLightEvent[num].Initialize(true);
  }

  mOutImageSize[0]  = 640;
  mOutImageSize[1]  = 480;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DeviceCameraThread::~DeviceCameraThread() {
  Cleanup();

  for(u32 num = 0; num < GFL_NELEMS(mEvent); ++num)  {
    mEvent[num].Finalize();
  }
  for(u32 num = 0; num < GFL_NELEMS(mLightEvent); ++num)  {
    mLightEvent[num].Finalize();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCameraThread::Setup(const Config& rConfig)  {
  Cleanup();

  mConfig = rConfig;
  mpThreadStack = GFL_NEW(rConfig.pAppHeap) ThreadStack(rConfig.pAppHeap, 4096);
  mThread.Start(
     this->ThreadMain
    ,this
    ,*mpThreadStack
    ,mConfig.threadPriority_Inactive
  );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCameraThread::Cleanup(void) {
  this->KillRequest();
  this->AwakeRequest();
  mEvent[EVENT_CAM_PORT1_BUFERROR].Signal();    /*  waitAny脱出用  */
  if(mThread.IsAlive()) {
    mThread.Join();
    mThread.Finalize();
  }
  GFL_SAFE_DELETE(mpThreadStack);


//  for(u32 num = 0; num < GFL_NELEMS(mEvent); ++num)  {
//    if(mEvent[num].IsValid()) mEvent[num].ClearSignal();
//  }
  for(u32 num = 0; num < GFL_NELEMS(mLightEvent); ++num)  {
    mLightEvent[num].ClearSignal();
  }

  mIsY2RCoefAvailable = false;
  mIsFatalError       = false;
}


//------------------------------------------------------------------
/**
  *  @brief      カメラスレッド本体
  */
//------------------------------------------------------------------
void    DeviceCameraThread::ThreadMain(DeviceCameraThread* pThis)  {
  pThis->ThreadProc_Initialize();                               /*  ライブラリ初期化          */
  while(!pThis->QueryKill())  {
    if(pThis->IsAvailable())  pThis->ThreadProc_Idle();         /*  一旦ここで停止            */
    if(pThis->IsAvailable())  pThis->ThreadProc_Setup();        /*  カメラ設定/起動           */
    if(pThis->IsAvailable())  pThis->ThreadProc_Capture();      /*  撮影                      */
    if(pThis->mIsFatalError)  pThis->ThreadProc_Error();        /*  エラー時はここでトラップ  */
    pThis->ThreadProc_Cleanup();                                /*  カメラ停止                */
  }
  pThis->ThreadProc_Finalize();                                 /*  ライブラリ終了            */
  pThis->mState  = STATE_STANDBY;
}


//------------------------------------------------------------------
/**
  *  @brief    カメラスレッド処理：ライブラリ初期化
  */
//------------------------------------------------------------------
void  DeviceCameraThread::ThreadProc_Initialize(void)  {
  enum  Step  {
    STEP_INITIALIZE,
    STEP_Y2RCOEF,

    STEP_NUM
  };
  CAM_LOG("CAM::Init\n");
  u32  step  = STEP_INITIALIZE;

  mState = STATE_INITIALIZE;
  while(this->IsAvailable() && (step < STEP_NUM)) {
    nn::Result    nnResult;

    this->QuerySleep();                /*  要求があればここでスリープ  */
    if(!this->IsAvailable())   break;

    switch(step)  {
      case  STEP_INITIALIZE:  {
        CAM_LOG("CAM::Init::Initialize\n");
        nnResult  = nn::camera::Initialize();
      }break;

      /*  Y2R係数  */
      case  STEP_Y2RCOEF: {
        CAM_LOG("CAM::Init::STEP_Y2RCOEF\n");
        nnResult  = nn::camera::GetSuitableY2rStandardCoefficient(&mY2RCoef);
      }break;
    }

    if(this->ResultHandler(nnResult)) {
      ++step;
    }
  }
  CAM_LOG("CAM::Init::done\n");
}


//------------------------------------------------------------------
/**
  *  @brief    カメラスレッド処理：待機中
  */
//------------------------------------------------------------------
void  DeviceCameraThread::ThreadProc_Idle(void)  {
  mIsY2RCoefAvailable = true;
  CAM_LOG("CAM::Idle\n");
  this->mState = STATE_IDLE;
  /*  sleep状態に入れるようにEventを直接待たない  */
  while(!mLightEvent[LEVENT_CAMTHREAD_START_REQ].TryWait()) {
    this->QuerySleep();
    if(!this->IsAvailable())   break;

    nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
  }
  mLightEvent[LEVENT_CAMTHREAD_START_REQ].ClearSignal();
}


//------------------------------------------------------------------
/**
  *  @brief     カメラスレッド処理：初期化/起動
  */
//------------------------------------------------------------------
void    DeviceCameraThread::ThreadProc_Setup(void)  {

  mContext.SetAllDirty(true);
  ApplyContextInternal();

  if(this->IsAvailable()) {
    nn::camera::SetTransferBytes(
      camSel2Port(mContext.camType.prm),
      nn::camera::GetMaxBytes(mOutImageSize[0], mOutImageSize[1]),
      mOutImageSize[0],
      mOutImageSize[1]
    );

    nn::camera::GetBufferErrorInterruptEvent(&mEvent[EVENT_CAM_PORT1_BUFERROR], nn::camera::PORT_CAM1);
    nn::camera::GetBufferErrorInterruptEvent(&mEvent[EVENT_CAM_PORT2_BUFERROR], nn::camera::PORT_CAM2);
    nn::camera::GetVsyncInterruptEvent(&mEvent[EVENT_CAM_PORT1_VSYNC], nn::camera::PORT_CAM1);
    nn::camera::GetVsyncInterruptEvent(&mEvent[EVENT_CAM_PORT2_VSYNC], nn::camera::PORT_CAM2);


    mEvent[EVENT_CAM_PORT1_RECV].ClearSignal();
    mEvent[EVENT_CAM_PORT2_RECV].ClearSignal();
    mEvent[EVENT_CAM_PORT1_BUFERROR].ClearSignal();
    mEvent[EVENT_CAM_PORT2_BUFERROR].ClearSignal();
    mEvent[EVENT_CAM_PORT1_VSYNC].ClearSignal();
    mEvent[EVENT_CAM_PORT2_VSYNC].ClearSignal();

    mThread.ChangePriority(mConfig.threadPriority_Active);
  }
}


/*-----------------------------------------------------------------*//**
  カメラスレッド処理：撮影
*//*------------------------------------------------------------------*/
void  DeviceCameraThread::ThreadProc_Capture(void)  {
  enum  CameraEvent {
    CAMEV_PORT1_RECV,       // カメラからの転送が完了
    CAMEV_PORT1_BUFERROR,   // バッファエラー、誤動作による再起動
    CAMEV_PORT1_VSYNC,      // V ブランク割り込み
    CAMEV_PORT2_RECV,       // カメラからの転送が完了
    CAMEV_PORT2_BUFERROR,   // バッファエラー、誤動作による再起動
    CAMEV_PORT2_VSYNC,      // V ブランク割り込み

    CAMEV_MAX
  };


  CAM_LOG("CAM::Capture\n");
  mState  = STATE_CAPTURE;
  mEvent[EVENT_CAM_PORT1_BUFERROR].Signal();    /*  起動  */
  while(this->IsAvailable()) {
    nn::os::WaitObject*   pEvent[CAMEV_MAX] = {
      &mEvent[EVENT_CAM_PORT1_RECV],
      &mEvent[EVENT_CAM_PORT1_BUFERROR],
      &mEvent[EVENT_CAM_PORT1_VSYNC],
      &mEvent[EVENT_CAM_PORT2_RECV],
      &mEvent[EVENT_CAM_PORT2_BUFERROR],
      &mEvent[EVENT_CAM_PORT2_VSYNC],
    };

    this->QuerySleep();
    if(!this->IsAvailable())  break;
    s32 camEventID = nn::os::WaitObject::WaitAny(pEvent, CAMEV_MAX);

    switch(camEventID)  {
      case  CAMEV_PORT1_RECV:
      case  CAMEV_PORT2_RECV: {       // カメラからの転送が完了
        this->OnReceive((camEventID == CAMEV_PORT1_RECV) ? nn::camera::PORT_CAM1 : nn::camera::PORT_CAM2);
      }break;

      case  CAMEV_PORT1_BUFERROR:
      case  CAMEV_PORT2_BUFERROR: {   // バッファエラーや誤動作による再起動
        mEvent[EVENT_CAM_PORT1_BUFERROR].ClearSignal();
        mEvent[EVENT_CAM_PORT2_BUFERROR].ClearSignal();
        this->StartCapture();   /*  再開  */
      }break;

      case  CAMEV_PORT1_VSYNC:
      case  CAMEV_PORT2_VSYNC:  {      // V ブランク割り込み
        if(mLightEvent[LEVENT_CAMTHREAD_APPLY_CONTEXT_REQ].TryWait())
        {
          ApplyContextInternal();
          mLightEvent[LEVENT_CAMTHREAD_APPLY_CONTEXT_REQ].ClearSignal();
        }
      }break;
    }
  }
  this->StopCapture();
}


//------------------------------------------------------------------
/**
  *  @brief    カメラスレッド処理：エラー
  */
//------------------------------------------------------------------
/*-----------------------------------------------------------------*//**
  
*//*------------------------------------------------------------------*/
void  DeviceCameraThread::ThreadProc_Error(void)  {
  /*  kill要求が来るまで待つ  */
  CAM_LOG("CAM::Error\n");
  mState  = STATE_ERROR;
  while(!mLightEvent[LEVENT_CAMTHREAD_KILL_REQ].TryWait())  {
    this->QuerySleep();
    nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(10));
  }
}


//------------------------------------------------------------------
/**
  *  @brief    カメラスレッド処理：停止処理
  */
//------------------------------------------------------------------
void  DeviceCameraThread::ThreadProc_Cleanup(void)  {
  enum  Step  {
    STEP_INACTIVATION,
    
    STEP_NUM
  };

  u32  step  = STEP_INACTIVATION;

  this->StopCapture();
  CAM_LOG("CAM::Cleanup\n");
  mState  = STATE_CLEANUP;
  while(this->IsAvailable() && (step < STEP_NUM)) {
    nn::Result    nnResult;

    this->QuerySleep();

    switch(step)  {
      case  STEP_INACTIVATION:  {
        nnResult = nn::camera::Activate(nn::camera::SELECT_NONE);
      }break;
    }

    if(this->ResultHandler(nnResult)) ++step;
  }

  mThread.ChangePriority(mConfig.threadPriority_Inactive);
}


//------------------------------------------------------------------
/**
  *  @brief    Y2R変換係数を取得する
  *  @note     内部で起動時に取得したものを格納しますが、蓋閉じ時にはAPIが失敗するため、falseが返る間はリトライする必要があります
  *  @retval   true: 成功
  *  @retval   false: 失敗
  */
//------------------------------------------------------------------
bool  DeviceCameraThread::GetY2RCoef(nn::y2r::StandardCoefficient* pDst) const  {
  GFL_ASSERT_STOP(pDst);
  
  bool  ret = false;
  if(mIsY2RCoefAvailable) {
    *pDst = mY2RCoef;
    ret   = true;
  }

  return ret;
}


//------------------------------------------------------------------
/**
  *  @param    
  */
//------------------------------------------------------------------
void  DeviceCameraThread::ApplyContext(void)
{
  mLightEvent[LEVENT_CAMTHREAD_APPLY_CONTEXT_REQ].Signal();
}


//------------------------------------------------------------------
/**
  *  @brief    カメラスレッド処理：終了処理
  */
//------------------------------------------------------------------
void  DeviceCameraThread::ThreadProc_Finalize(void) {
  CAM_LOG("CAM::Finalize\n");
  mState  = STATE_FINALIZE;
  nn::camera::Finalize();
}



//------------------------------------------------------------------
/**
  *  @brief    sleepリクエスト監視と停止
  */
//------------------------------------------------------------------
void  DeviceCameraThread::QuerySleep(void)  {
  if(mLightEvent[LEVENT_CAMTHREAD_SLEEP_REQ].TryWait())  {
    CAM_LOG("CAM::Sleep\n");
    mLightEvent[LEVENT_CAMTHREAD_SLEEP_REQ].ClearSignal();    /*  要求解除  */
    if(mState == STATE_CAPTURE) {
      this->StopCapture();
    }
    /*  スレッドスリープ  */
    {
      mLightEvent[LEVENT_CAMTHREAD_SLEPT].Signal();           /*  スリープ通知      */
      mLightEvent[LEVENT_CAMTHREAD_AWAKE_REQ].Wait();         /*  起床要求待ち      */
      mLightEvent[LEVENT_CAMTHREAD_AWAKE_REQ].ClearSignal();
      mLightEvent[LEVENT_CAMTHREAD_SLEPT].ClearSignal();
    }
    /*  スリープ前のイベントをクリア  */
    mEvent[EVENT_CAM_PORT1_VSYNC].ClearSignal();
    mEvent[EVENT_CAM_PORT2_VSYNC].ClearSignal();

    mEvent[EVENT_CAM_PORT1_BUFERROR].Signal();    /*  カメラ起動  */
  }
}


//------------------------------------------------------------------
/**
  *  @brief    Resultハンドラ
  *  @retval   true    成功
  *  @retval   false   失敗
  */
//------------------------------------------------------------------
bool  DeviceCameraThread::ResultHandler(const nn::Result nnResult)  {

#if GFL_DEBUG
  if(m_IsForceFatal)
  {
    CAM_LOG("DeviceCameraThread::ResultHandler: forceFatal\n");
    *(const_cast<nn::Result*>(&nnResult))  = nn::camera::CTR::ResultFatalError();
  }
#endif


  if(nnResult.IsSuccess())  {
    return true;

  }else if(nnResult == nn::camera::CTR::ResultIsSleeping())  {
    CAM_LOG("蓋閉じ中であるため、呼び出そうとした関数は使用できないことを示します。\n");
    /*  リトライ  */

  }else if(nnResult == nn::camera::CTR::ResultFatalError())  {
    CAM_LOG("カメラの再起動処理に失敗したことを示します。カメラが物理的に故障している可能性があります。\n");
    mIsFatalError = true;

  }else if(nnResult == nn::camera::CTR::ResultAlreadyInitialized())  {
    CAM_LOG("カメラライブラリが初期化済みであることを示します。\n");
    return true;    /*  とりあえず成功としておく  */

  }else if(nnResult == nn::camera::CTR::ResultUsingOtherProcess())  {
    CAM_LOG("他のプロセスがカメラを使用中であるため、自分は使えないことを示します。\n");
    mIsFatalError = true;

  }else {
    CAM_LOG("未知のエラー\n");

  }

  return false;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCameraThread::StartCapture(void)  {
  const nn::camera::Port  port  = camSel2Port(mContext.camType.prm);

  this->StopCapture();

  // データ転送処理
  this->SetReceiving(port);

  // キャプチャ開始
  nn::camera::StartCapture(port);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCameraThread::SetReceiving(const nn::camera::Port port)  {
  const u32   cameraMaxBytes  = nn::camera::GetMaxBytes(mOutImageSize[0], mOutImageSize[1]);

  if(0
    || (port == nn::camera::PORT_CAM1)
    || (port == nn::camera::PORT_BOTH)
  ) {
    CaptureBuffer*  pBuffer = mConfig.pCaptureBuffer[0];

    if(pBuffer) {
      void* pWriteBuffer  = pBuffer->GetWriteBuffer();

      if(pWriteBuffer)  {
        nn::camera::SetReceiving(
            &mEvent[EVENT_CAM_PORT1_RECV],
            pWriteBuffer,
            nn::camera::PORT_CAM1,
            pBuffer->GetSize(),
            cameraMaxBytes
        );
      }else {
        CAM_LOG("write buffer 1 error\n");
      }
    }
  }


  if(0
    || (port == nn::camera::PORT_CAM2)
    || (port == nn::camera::PORT_BOTH)
  ) {
    CaptureBuffer*  pBuffer = mConfig.pCaptureBuffer[1];

    if(pBuffer) {
      void* pWriteBuffer  = pBuffer->GetWriteBuffer();

      if(pWriteBuffer)  {
        nn::camera::SetReceiving(
            &mEvent[EVENT_CAM_PORT2_RECV],
            pWriteBuffer,
            nn::camera::PORT_CAM2,
            pBuffer->GetSize(),
            cameraMaxBytes
        );
      }else {
        CAM_LOG("write buffer 2 error\n");
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCameraThread::OnReceive(const nn::camera::Port port)  {
  /*  ThreadProc_CaptureからPORT_CAM1/PORT_CAM2でコールされる  */
  if(0
    || (port == nn::camera::PORT_CAM1)
    || (port == nn::camera::PORT_BOTH)
  ) {
    mConfig.pCaptureBuffer[0]->SwapWriteBuffer();
  }


  if(0
    || (port == nn::camera::PORT_CAM2)
    || (port == nn::camera::PORT_BOTH)
  ) {
    mConfig.pCaptureBuffer[1]->SwapWriteBuffer();

  }

  this->SetReceiving(port);   /*  次のフレームを要求  */
  mLightEvent[LEVENT_CAMTHREAD_CAPTURED].Signal();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCameraThread::StopCapture(void) {
  nn::camera::StopCapture(nn::camera::PORT_BOTH);

  // 実際にキャプチャが停止するのはフレームエンドであり、
  // StopCapture の呼び出し直後とは限らないので、終了を待ちます。
  // もし、別のスレッドから StartCapture が実行される場合、
  // 永遠に IsBusy で待ち続けてしまいますので、排他制御が必要となります。
  s32 cnt = 0;
  while(nn::camera::IsBusy(nn::camera::PORT_BOTH)) {
      nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(1));

      // IsBusy のポーリング中にふたが閉じられたとき、
      // カメラが停止されるタイミングによっては、
      // ふたが開くまで IsBusy がずっと true になるので、
      // 無限ループを防ぐためにタイムアウトします。
      // 通常、IsBusy が true になるのは最大でも 1 フレームですので、
      // タイムアウト時間は 1 フレームの時間にしています。
      if (++cnt > 66) {
          CAM_LOG("Busy timeout\n");
          break;
      }
  }
  nn::camera::ClearBuffer(nn::camera::PORT_BOTH);
  ResetCaptured();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCameraThread::ApplyContextInternal(void)
{
  nn::os::CriticalSection::ScopedLock scopedLock(mContext.criticalSection);

  enum  Step  {
    STEP_RESOLUTION,
    STEP_PHOTOMODE,
    STEP_TRIMMING,
    STEP_EXPOSURE,
    STEP_WB,
    STEP_SHARPNESS,
    STEP_FLIP,
    STEP_FPS,
    STEP_EFFECT,
    STEP_CONTRAST,
    STEP_LC,
    STEP_NOISEFILTER,
    STEP_ACTIVATION,
    STEP_VSYNC_TIMING,
    STEP_BRIGHTNESS_SYNC,

    STEP_NUM
  };
  u32                             step      = STEP_RESOLUTION;
  const nn::camera::CameraSelect  camSel    = static_cast<nn::camera::CameraSelect>(mContext.camType.prm);
  const nn::camera::Port          port      = camSel2Port(camSel);
  const s16                       rectUnitH = 40;                           /**<  ウインドウ矩形定義時の設定単位[px]  */
  const s16                       rectUnitV = 30;
  const bool                      isAll     = mContext.camType.isDirty;     /*  使用するカメラが変わったら全て再設定  */
  const bool                      isStereo  = (camSel == CAM_OUT_LR);


  CAM_LOG("CAM::Setup\n");
  mState = STATE_SETUP;
  while(this->IsAvailable() && (step < STEP_NUM)) {
    nn::Result    nnResult;

    this->QuerySleep();                  /*  要求があればここでスリープ  */
    if(!this->IsAvailable())  break;     /*  要求があれば終了            */

    switch(step)  {
      /*  解像度  */
      case  STEP_RESOLUTION:  {
        if(isAll || mContext.resolution.isDirty) {
          nnResult  = nn::camera::SetDetailSize(
                        camSel,
                        mContext.resolution.prm.w, mContext.resolution.prm.h,
                        mContext.resolution.prm.crop[0],
                        mContext.resolution.prm.crop[1],
                        mContext.resolution.prm.crop[2],
                        mContext.resolution.prm.crop[3]
                      );
          mOutImageSize[0]  = mContext.resolution.prm.w;
          mOutImageSize[1]  = mContext.resolution.prm.h;
        }else {
          ++step;
          continue;
        }
      }break;

      /*  撮影モード  */
      case  STEP_PHOTOMODE: {
        if(isAll || mContext.photoMode.isDirty) {
          nnResult  = nn::camera::SetPhotoMode(camSel, (nn::camera::PhotoMode)mContext.photoMode.prm.type);
        }else {
          ++step;
          continue;
        }
      }break;

      /*  トリミング  */
      case  STEP_TRIMMING:  {
        if(isAll || mContext.trimming.isDirty) {
          if(mContext.trimming.prm.isEnable)  {
            nn::camera::SetTrimmingParamsCenter(
              port,
              mContext.trimming.prm.w,    mContext.trimming.prm.h,
              mContext.resolution.prm.w,  mContext.resolution.prm.h
            );
            mOutImageSize[0]  = mContext.trimming.prm.w;
            mOutImageSize[1]  = mContext.trimming.prm.h;
          }
          nn::camera::SetTrimming(port, mContext.trimming.prm.isEnable);
        }
        ++step;
        continue;   /*  失敗が返らない  */
      }

      /*  自動露光  */
      case  STEP_EXPOSURE:  {
        if(isAll || mContext.exposure.isDirty) {
          const bool  isEnable  = mContext.exposure.prm.isEnable;
          bool        isSuccess = true;

          /*  有効にするときのみパラメータ設定  */
          if(isEnable)  {
            nnResult  = nn::camera::SetAutoExposureWindow(
                          camSel,
                          mContext.exposure.prm.rectUnit[0] * rectUnitH,
                          mContext.exposure.prm.rectUnit[1] * rectUnitV,
                          mContext.exposure.prm.rectUnit[2] * rectUnitH,
                          mContext.exposure.prm.rectUnit[3] * rectUnitV
                        );

            isSuccess = nnResult.IsSuccess();
          }

          /*  失敗していなければスイッチ設定  */
          if(isSuccess) {
            nnResult  = nn::camera::SetAutoExposure(camSel, isEnable);
            isSuccess = nnResult.IsSuccess();
          }
        }else {
          ++step;
          continue;
        }
      }break;

      /*  ホワイトバランス  */
      case  STEP_WB:  {
        if(isAll || mContext.wb.isDirty) {
          const bool  isAuto    = (mContext.wb.prm.type == WB_AUTO);
          bool        isSuccess = true;

          /*  auto  */
          nnResult  = nn::camera::SetAutoWhiteBalance(camSel, isAuto);
          isSuccess = nnResult.IsSuccess();

          /*  type  */
          if(isSuccess && !isAuto)  {
            nnResult  = nn::camera::SetWhiteBalance(camSel, (nn::camera::WhiteBalance)mContext.wb.prm.type);
            isSuccess = nnResult.IsSuccess();
          }

          /*  autoWin  */
          if(isSuccess && !isAuto)  {
            nnResult  = nn::camera::SetAutoWhiteBalanceWindow(
                          camSel,
                          mContext.wb.prm.rectUnit[0] * rectUnitH,
                          mContext.wb.prm.rectUnit[1] * rectUnitV,
                          mContext.wb.prm.rectUnit[2] * rectUnitH,
                          mContext.wb.prm.rectUnit[3] * rectUnitV
                        );
          }
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_SHARPNESS: {
        if(isAll || mContext.sharpness.isDirty) {
          nnResult  = nn::camera::SetSharpness(camSel, mContext.sharpness.prm.value);
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_FLIP:  {
        if(isAll || mContext.flip.isDirty) {
          nnResult  = nn::camera::FlipImage(camSel, static_cast<nn::camera::Flip>(mContext.flip.prm.type));
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_FPS: {
        if(isAll || mContext.fps.isDirty) {
          nnResult  = nn::camera::SetFrameRate(camSel, (nn::camera::FrameRate)mContext.fps.prm.type);
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_EFFECT:  {
        if(isAll || mContext.effect.isDirty) {
          nnResult  = nn::camera::SetEffect(camSel, (nn::camera::Effect)mContext.effect.prm.type);
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_CONTRAST:  {
        if(isAll || mContext.contrast.isDirty) {
          nnResult  = nn::camera::SetContrast(camSel, (nn::camera::Contrast)mContext.contrast.prm.type);
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_LC:  {
        if(isAll || mContext.lc.isDirty) {
          nnResult  = nn::camera::SetLensCorrection(camSel, (nn::camera::LensCorrection)mContext.lc.prm.type);
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_NOISEFILTER: {
        if(isAll || mContext.noiseFilter.isDirty) {
          nnResult  = nn::camera::SetNoiseFilter(camSel, mContext.noiseFilter.prm.isEnable);
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_ACTIVATION:  {
        /*
          (注意)
          カメラを起動した直後の4フレームは取得した画像が極端に暗い場合があります。 
          また、カメラが起動した後に自動露出が安定するまでのフレームは上記の4フレームと合わせて、 
          屋内で14フレーム、屋外で30フレーム程度となります。
        */
        nnResult  = nn::camera::Activate(camSel);
      }break;

      case  STEP_VSYNC_TIMING:  {
        if(isStereo)  {
          nnResult  = nn::camera::SynchronizeVsyncTiming((nn::camera::CTR::CameraSelect)CAM_OUT_L, (nn::camera::CTR::CameraSelect)CAM_OUT_R);
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_BRIGHTNESS_SYNC: {
        const bool  isAutoWB        = (mContext.wb.prm.type == WB_AUTO);
        const bool  isNotLandscape  = (mContext.photoMode.prm.type != MODE_LANDSCAPE);
        const bool  isNotContrast10 = (mContext.contrast.prm.type != CONTRAST_10);

        if(1
          &&  isAutoWB
          &&  isNotLandscape
          &&  isNotContrast10
        )  {
          nnResult  = nn::camera::SetBrightnessSynchronization(isStereo);
        }
      }break;

      default:
        GFL_ASSERT(false);
        ++step;
        continue;
    }
    
    if(this->ResultHandler(nnResult)) ++step;
  }


  if(step >= STEP_NUM)
  {
    mContext.SetAllDirty(false);
  }

}



}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */
#endif