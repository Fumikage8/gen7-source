//==============================================================================
/**
 * @file   gfl2_Y2RThread.cpp
 * @date   2013/03/15 Fri.
 * @date   2015/07/30 Thu. 15:32:38 旧プロジェクトより移植
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================
#if defined(GF_PLATFORM_CTR)


// =============================================================================
/**/
//==============================================================================
#include  "DeviceCamera/include/gfl2_DeviceCameraThread.h"
#include  "DeviceCamera/include/gfl2_Y2RThread.h"



// =============================================================================
/**/
//==============================================================================
namespace  gfl2  {
namespace  devicecamera  {
//  #define Y2R_LOG                 GFL_PRINT
  #define Y2R_LOG(format, ...)


u32   GetInputBpp(const Y2RThread::InputFormat format);
u32   GetOutputBpp(const Y2RThread::OutputFormat format);




//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
Y2RThread::Y2RThread(void)  :
   mpThreadStack(NULL)
  ,mThread()
  ,mThreadPriority_Active(DEFAULT_THREAD_PRIORITY_ACTIVE)
  ,mThreadPriority_Inactive(DEFAULT_THREAD_PRIORITY_INACTIVE)
  ,mPackagePrm()
  ,mpDeviceCameraThread(NULL)
  ,mState(STATE_STANDBY)
  ,mContext()
  ,mIsFatalError(false)
{
  for(u32 num = 0; num < GFL_NELEMS(mCS); ++num)  {
    mCS[num].Initialize();
  }
  for(u32 num = 0; num < GFL_NELEMS(mEvent); ++num)  {
    mEvent[num].Initialize(false);
  }
  for(u32 num = 0; num < GFL_NELEMS(mLightEvent); ++num)  {
    mLightEvent[num].Initialize(true);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
Y2RThread::~Y2RThread() {
  Cleanup();
  for(u32 num = 0; num < GFL_NELEMS(mCS); ++num)  {
    mCS[num].Finalize();
  }
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
void  Y2RThread::Setup(const Config& rConfig) {
  GFL_ASSERT_STOP(mState == STATE_STANDBY);
  GFL_ASSERT_STOP(rConfig.pAppHeap);
  GFL_ASSERT_STOP(rConfig.pDevHeap);
  GFL_ASSERT_STOP(rConfig.pBuffer[0] || rConfig.pBuffer[1]);
  GFL_ASSERT_STOP(rConfig.imageSize[0] && (rConfig.imageSize[0] <= 1024) && (!(rConfig.imageSize[0] % 8)));
  GFL_ASSERT_STOP(rConfig.imageSize[1]);
  GFL_ASSERT_STOP(rConfig.threadPriority_Active <= rConfig.threadPriority_Inactive);

  Cleanup();

  mThreadPriority_Active    = rConfig.threadPriority_Active;
  mThreadPriority_Inactive  = rConfig.threadPriority_Inactive;

  mIsFatalError = !nn::y2r::Initialize();
  if(!mIsFatalError)  {
    /*  y2r設定  */
    mpDeviceCameraThread    = rConfig.pDeviceCameraThread;

    mPackagePrm.inputFormat              = (nn::y2r::InputFormat)rConfig.inputFormat;
    mPackagePrm.outputFormat             = (nn::y2r::OutputFormat)rConfig.outputFormat;
    mPackagePrm.rotation                 = (nn::y2r::Rotation)rConfig.rotation;
    mPackagePrm.blockAlignment           = (nn::y2r::BlockAlignment)rConfig.blockAlignment;
    mPackagePrm.inputLineWidth           = rConfig.imageSize[0];
    mPackagePrm.inputLines               = rConfig.imageSize[1];
//    mPackagePrm.standardCoefficientNo    = ;      /* 失敗することがあるのでスレッド内で取得する  */
    mPackagePrm.alpha                    = rConfig.alpha;

    /*  context  */
    {
      /*  input  */
      const u32   Bpp       = GetInputBpp(rConfig.inputFormat);
      const u32   lineBytes = mPackagePrm.inputLineWidth * Bpp;

      mContext.inputImageBytes    = lineBytes * mPackagePrm.inputLines;
      mContext.inputTransferUnit  = lineBytes;
    }
    {
      /*  output  */
      mContext.outputImageBytes   = nn::y2r::GetOutputImageSize(mPackagePrm.inputLineWidth, mPackagePrm.inputLines, mPackagePrm.outputFormat);
      mContext.outputTransferUnit = nn::y2r::GetOutputBlockSize(mPackagePrm.inputLineWidth, mPackagePrm.outputFormat);
    }

    /*  env  */
    {
      for(u32 ch = 0; ch < GFL_NELEMS(mEnv); ++ch)  {
        mEnv[ch].pBuffer  = rConfig.pBuffer[ch];
      }
    }

    mpThreadStack = GFL_NEW(rConfig.pAppHeap) ThreadStack(rConfig.pAppHeap, 4096);
    mThread.Start(
       this->ThreadMain
      ,this
      ,*mpThreadStack
      ,mThreadPriority_Inactive
    );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Y2RThread::Cleanup(void)  {
  this->KillRequest();
  this->AwakeRequest();
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

  nn::y2r::Finalize();

  mIsFatalError         = false;
  mpDeviceCameraThread  = NULL;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Y2RThread::ConvertRequest(const u8* pImage_Ch1, const u8* pImage_Ch2) {
  nn::os::CriticalSection::ScopedLock sl(mCS[CS_UPDATE_SRC]);

  mEnv[0].pNextSrc = pImage_Ch1;
  mEnv[1].pNextSrc = pImage_Ch2;

  mLightEvent[LEVENT_Y2RTHREAD_START_REQ].Signal();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Y2RThread::GetImageSize(u32& rW, u32& rH) {
  rW  = mPackagePrm.inputLineWidth;
  rH  = mPackagePrm.inputLines;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   Y2RThread::ThreadMain(Y2RThread* pThis)  {
  pThis->ThreadProc_Setup();        /*  設定           */
  while(!pThis->QueryKill())  {
    if(pThis->IsAvailable())  pThis->ThreadProc_Idle();         /*  一旦ここで停止            */
    if(pThis->IsAvailable())  pThis->ThreadProc_Convert();      /*  変換                      */
    if(pThis->mIsFatalError)  pThis->ThreadProc_Error();        /*  エラー時はここでトラップ  */
  }
  pThis->mThread.ChangePriority(pThis->mThreadPriority_Inactive);
  pThis->ThreadProc_Cleanup();                                  /*  変換停止                */
  pThis->mState  = STATE_STANDBY;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   Y2RThread::ThreadProc_Setup(void)  {
  enum  Step  {
    STEP_Y2RCOEF,
    STEP_SETUP,
    
    STEP_NUM
  };
  u32  step  = STEP_Y2RCOEF;

  mState  = STATE_SETUP;
  Y2R_LOG("Y2R::Setup\n");

  while(this->IsAvailable() && (step < STEP_NUM)) {
    this->QuerySleep();
    if(!this->IsAvailable())  break;

    switch(step)  {
      case  STEP_Y2RCOEF: {
        /*    */
        if(mpDeviceCameraThread)  {
          if(mpDeviceCameraThread->GetY2RCoef((nn::y2r::StandardCoefficient*)&mPackagePrm.standardCoefficientNo))  {
            ++step;
            continue;
          }else {
            nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(10));
          }
        }else {
          ++step;
          continue;
        }
      }break;

      case  STEP_SETUP: {
        nn::y2r::SetPackageParameter(mPackagePrm);
        nn::y2r::SetTransferEndInterrupt(true);
        nn::y2r::GetTransferEndEvent(&mEvent[EVENT_Y2R_TRANS_END]);

        ++step;
      }break;
    }
  }

  mThread.ChangePriority(mThreadPriority_Active);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   Y2RThread::ThreadProc_Idle(void) {
  mState  = STATE_IDLE;
  Y2R_LOG("Y2R::Idle\n");
  
  while(this->IsAvailable() && !mLightEvent[LEVENT_Y2RTHREAD_START_REQ].TryWait())  {
    this->QuerySleep();
    if(!this->IsAvailable())  break;
    
    nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(1));
  }
  mLightEvent[LEVENT_Y2RTHREAD_START_REQ].ClearSignal();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   Y2RThread::ThreadProc_Convert(void)  {
  bool  isConverted = true;

  Y2R_LOG("Y2R::Conv\n");
  mState  = STATE_CONVERT;
  
  /*  入力イメージ取得  */
  {
    nn::os::CriticalSection::ScopedLock sl(mCS[CS_UPDATE_SRC]);

    for(u32 num = 0; num < GFL_NELEMS(mEnv); ++num) {
      mEnv[num].pCurrentSrc  = mEnv[num].pNextSrc;
    }
    mLightEvent[LEVENT_Y2RTHREAD_CONVERTED].ClearSignal();
  }

  /*  変換処理  */
  for(u32 ch = 0; ch < GFL_NELEMS(mEnv); ++ch) {
    bool  isChConverted = false;

//    this->QuerySleep();   /*  ここではスレッドを止めない  */
    if(!this->IsAvailable())  break;

    Environment&  rEnv  = mEnv[ch];

    if(!(rEnv.pCurrentSrc && rEnv.pBuffer->GetWriteBuffer())) continue;   /*  未使用Chは無視  */

    // 連続で何度も不具合が発生することはきわめて稀ですが、
    // 発生確率は 0 ではないので、念のため、ループを抜ける処理を入れています。
    // このとき、この画像の変換をあきらめて、次の画像の変換に移っています。
    for(u32 retry = 0; retry < 2; ++retry)  {
      // Y2R の出力画像の転送設定をします。
      nn::y2r::CTR::SetReceiving(
        rEnv.pBuffer->GetWriteBuffer(),
        mContext.outputImageBytes,
        mContext.outputTransferUnit
      );

      // Y2R の入力画像の転送設定をします。
      // 転送単位は 1 ライン分のデータとなります。
      nn::y2r::CTR::SetSendingYuv(
        rEnv.pCurrentSrc,
        mContext.inputImageBytes,
        mContext.inputTransferUnit
      );

      // Y2R変換を開始します。
      nn::y2r::CTR::StartConversion();

      // 1つの画像の変換が終わる前に次の変換のためのデータ転送を行なうと
      // 転送が中断されてしまうので、Y2R 変換の終了を待ちます。
      // なお、Y2R がハングアップする不具合の対策のため、タイムアウトを入れています。
      // 512*384 画像を RGB 24bit で転送する場合、変換にかかる時間はおよそ 10msec ですので、
      // その 2 倍の 20msec をタイムアウト時間に設定しています。 
      // 変換にかかる時間はカメラ画像の大きさと、出力画像のフォーマットによって異なります。
      if (mEvent[EVENT_Y2R_TRANS_END].Wait(nn::fnd::TimeSpan::FromMilliSeconds(20)))  {
        // 変換に成功
        isChConverted = true;
        break;    /*  retryループから抜ける  */
      } else  {
        // 変換に失敗
        NN_LOG("Y2R may have hung up (EVENT_Y2R_TRANS_END timeout).\n");

        // 変換を強制終了する
        nn::y2r::StopConversion();
        // リトライする
      }
    } /*  retry  */
    
    
    /*  使用chのConvertに失敗したら、撮影イメージずれを防ぐため今回のConvertをあきらめる  */
    if(!isChConverted)  {
      isConverted = false;
      break;
    }
  }

  if(isConverted)  {
    mLightEvent[LEVENT_Y2RTHREAD_CONVERTED].Signal();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32   Y2RThread::GetOutputFrameBytes(const OutputFormat format, const u32 w, const u32 h) {
  return  nn::y2r::GetOutputImageSize(w, h, (nn::y2r::OutputFormat)format);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   Y2RThread::ThreadProc_Error(void)  {
  /*  kill要求が来るまで待つ  */
  Y2R_LOG("Y2R::Error\n");
  mState  = STATE_ERROR;
  while(!mLightEvent[LEVENT_Y2RTHREAD_KILL_REQ].TryWait())  {
    this->QuerySleep();
    nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(10));
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   Y2RThread::ThreadProc_Cleanup(void)  {
  Y2R_LOG("Y2R::Cleanup\n");
  mState  = STATE_CLEANUP;
  nn::y2r::StopConversion();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   Y2RThread::QuerySleep(void)  {
  if(mLightEvent[LEVENT_Y2RTHREAD_SLEEP_REQ].TryWait())  {
    Y2R_LOG("Y2R::Sleep\n");
    mLightEvent[LEVENT_Y2RTHREAD_SLEEP_REQ].ClearSignal();    /*  要求解除  */
    
    nn::y2r::StopConversion();
    mLightEvent[LEVENT_Y2RTHREAD_CONVERTED].ClearSignal();
    /*  スレッドスリープ  */
    {
      mLightEvent[LEVENT_Y2RTHREAD_SLEPT].Signal();
      mLightEvent[LEVENT_Y2RTHREAD_AWAKE_REQ].Wait();
      mLightEvent[LEVENT_Y2RTHREAD_AWAKE_REQ].ClearSignal();
      mLightEvent[LEVENT_Y2RTHREAD_SLEPT].ClearSignal();
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool   Y2RThread::ResultHandler(const nn::Result nnResult)  {
  if(nnResult.IsSuccess())  {
    return true;

  }else if(nnResult == nn::y2r::CTR::ResultIsSleeping())  {
    Y2R_LOG("システムがスリープ中であるためY2Rを使用できないことを示します。\n");
    /*  リトライ  */

  }else {
    Y2R_LOG("未知のエラー\n");

  }

  return false;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32   GetInputBpp(const Y2RThread::InputFormat format)  {
  u32   Bpp = 2;

  switch(format)  {
    case  Y2RThread::INPUT_YUV422_INDIV_8:
    case  Y2RThread::INPUT_YUV420_INDIV_8:   Bpp = 3;  break;

    case  Y2RThread::INPUT_YUV422_INDIV_16:
    case  Y2RThread::INPUT_YUV420_INDIV_16:  Bpp = 6;  break;

    case  Y2RThread::INPUT_FROM_CAMERA:      Bpp = 2;  break;

    default:
      GFL_ASSERT_STOP(false);
      break;
  }

  return Bpp;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32   GetOutputBpp(const Y2RThread::OutputFormat format) {
  return nn::y2r::GetOutputFormatBytes((nn::y2r::OutputFormat)format);
}





}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */


#endif