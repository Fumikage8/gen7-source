


//==============================================================================
/**
 * @file   gfl2_DeviceCamera.cpp
 * @date   2013/03/04 Mon.
 * @date   2015/07/30 Thu. 14:56:42
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
#include  "DeviceCamera/include/gfl2_DeviceCamera.h"



//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(gfl2::devicecamera::DeviceCamera)


namespace  gfl2  {
namespace  devicecamera  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DeviceCamera::DeviceCamera(void) :
    mState(STATE_MAX)
  , mpCameraThread(NULL)
  , mpY2RThread(NULL)
  , mSleepCounter(0)
{
  for(u32 num = 0; num < GFL_NELEMS(mCS); ++num)  {
    mCS[num].Initialize();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DeviceCamera::~DeviceCamera() {
  Cleanup();
  for(u32 num = 0; num < GFL_NELEMS(mCS); ++num)  {
    mCS[num].Finalize();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCamera::Setup(const Config& rConfig)  {
  const nn::camera::Port  port  = camSel2Port(rConfig.camType);
  s16                     inputImageSize[2];


  Cleanup();

  {
    mpCameraThread  = GFL_NEW(rConfig.pAppHeap) DeviceCameraThread();

    mpCameraThread->SetCameraType(      rConfig.camType);
    mpCameraThread->SetResolution(      rConfig.resolution);
    mpCameraThread->SetPhotoMode(       rConfig.photoMode);
    mpCameraThread->SetTrimming(        rConfig.trimming);
    mpCameraThread->SetExposure(        rConfig.exposure);
    mpCameraThread->SetWhiteBalance(    rConfig.whiteBalance);
    mpCameraThread->SetSharpness(       rConfig.sharpness);
    mpCameraThread->SetFlip(            rConfig.flip);
    mpCameraThread->SetFrameRate(       rConfig.frameRate);
    mpCameraThread->SetEffect(          rConfig.effect);
    mpCameraThread->SetContrast(        rConfig.contrast);
    mpCameraThread->SetLensCorrection(  rConfig.lensCorrection);
    mpCameraThread->SetNoiseFilter(     rConfig.noiseFilter);
    {
      DeviceCameraThread::Config    threadConf;

      threadConf.pAppHeap           = rConfig.pAppHeap;
      threadConf.pDevHeap           = rConfig.pDevHeap;
      if(rConfig.trimming.isEnable) {
        inputImageSize[0]  = rConfig.trimming.w;
        inputImageSize[1]  = rConfig.trimming.h;
      }else {
        inputImageSize[0]  = rConfig.resolution.w;
        inputImageSize[1]  = rConfig.resolution.h;
      }

      /*  port1  */
      if(0
        || (port == nn::camera::PORT_CAM1)
        || (port == nn::camera::PORT_BOTH)
      )  {
        const u32   frameBytes  = nn::camera::GetFrameBytes(inputImageSize[0], inputImageSize[1]);

        mCaptureBuffer[0].Setup(rConfig.pDevHeap, frameBytes);
        threadConf.pCaptureBuffer[0]  = &mCaptureBuffer[0];
      }
      
      /*  port2  */
      if(0
        || (port == nn::camera::PORT_CAM2)
        || (port == nn::camera::PORT_BOTH)
      )  {
        const u32   frameBytes  = nn::camera::GetFrameBytes(inputImageSize[0], inputImageSize[1]);

        mCaptureBuffer[1].Setup(rConfig.pDevHeap, frameBytes);
        threadConf.pCaptureBuffer[1]  = &mCaptureBuffer[1];
      }

      threadConf.threadPriority_Active    = rConfig.cameraThread_Active;
      threadConf.threadPriority_Inactive  = rConfig.cameraThread_Inactive;
      mpCameraThread->Setup(threadConf);
    }
  }

  /*  Y2R  */
  {
    mpY2RThread  = GFL_NEW(rConfig.pAppHeap) Y2RThread();

    {
      const u32   frameBytes  = Y2RThread::GetOutputFrameBytes(rConfig.outputFormat, rConfig.outImageSize[0], rConfig.outImageSize[1]);

      Y2RThread::Config   threadConf;

      threadConf.pAppHeap             = rConfig.pAppHeap;
      threadConf.pDevHeap             = rConfig.pDevHeap;
      threadConf.pBuffer[0]           = /*  through  */
      threadConf.pBuffer[1]           = NULL;
      threadConf.inputFormat          = Y2RThread::INPUT_FROM_CAMERA;
      threadConf.outputFormat         = rConfig.outputFormat;
      threadConf.rotation             = Y2RThread::ROT_NONE;
      threadConf.blockAlignment       = Y2RThread::ALIGN_FOR_TEXTURE;
      threadConf.imageSize[0]         = inputImageSize[0];            /*  実際の変換は入力画像サイズで行う  */
      threadConf.imageSize[1]         = inputImageSize[1];
      threadConf.alpha                = rConfig.alpha;
      threadConf.pDeviceCameraThread  = mpCameraThread;

      /*  port1  */
      if(0
        || (port == nn::camera::PORT_CAM1)
        || (port == nn::camera::PORT_BOTH)
      )  {
        mY2RBuffer[0].Setup(rConfig.pDevHeap, frameBytes);
        threadConf.pBuffer[0]  = &mY2RBuffer[0];
      }
      
      /*  port2  */
      if(0
        || (port == nn::camera::PORT_CAM2)
        || (port == nn::camera::PORT_BOTH)
      )  {
        mY2RBuffer[1].Setup(rConfig.pDevHeap, frameBytes);
        threadConf.pBuffer[1]  = &mY2RBuffer[1];
      }

      threadConf.threadPriority_Active    = rConfig.y2rThreadPriority_Active;
      threadConf.threadPriority_Inactive  = rConfig.y2rThreadPriority_Inactive;
      mpY2RThread->Setup(threadConf);
    }
  }

}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCamera::Cleanup(void) {
  /*  Y2R  */
  if(mpY2RThread) {
    mpY2RThread->Cleanup();
    GFL_SAFE_DELETE(mpY2RThread);
    
    mY2RBuffer[0].Cleanup();
    mY2RBuffer[1].Cleanup();
  }

  /*  camera  */
  if(mpCameraThread)  {
    mpCameraThread->Cleanup();
    GFL_SAFE_DELETE(mpCameraThread);
    mCaptureBuffer[0].Cleanup();
    mCaptureBuffer[1].Cleanup();
  }

  mSleepCounter = 0;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DeviceCamera::Start(void) {
  mpCameraThread->StartRequest();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DeviceCamera::State   DeviceCamera::Update(void)  {
  if(mpCameraThread && mpY2RThread) {
    if(mpCameraThread->IsCaptured() && mpY2RThread->IsIdle())  {
      mpCameraThread->ResetCaptured();
      mY2RBuffer[0].SwapBuffer();
      mY2RBuffer[1].SwapBuffer();
      mpY2RThread->ConvertRequest(mCaptureBuffer[0].SwapReadBuffer(), mCaptureBuffer[1].SwapReadBuffer());
    }
  }
  return mState;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   DeviceCamera::SleepRequest(void) {
  DeviceCamera*   pThis = GFL_SINGLETON_INSTANCE(DeviceCamera);

  if(pThis)  {
    nn::os::CriticalSection::ScopedLock sl(pThis->mCS[CS_SLEEPCOUNTER_ACCESS]);


    if(!pThis->mSleepCounter)
    {
      if(pThis->mpY2RThread)  {
        GFL_PRINT("sleep req: Y2R\n");
        pThis->mpY2RThread->SleepRequest();
      }
      if(pThis->mpCameraThread) {
        GFL_PRINT("sleep req: CAM\n");
        pThis->mpCameraThread->SleepRequest();
      }
    }
    ++pThis->mSleepCounter;
    GFL_PRINT("sleep cnt: %d\n", pThis->mSleepCounter);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   DeviceCamera::WaitForSleep(void) {
  DeviceCamera*   pThis = GFL_SINGLETON_INSTANCE(DeviceCamera);

  if(pThis)  {
    if(pThis->mpY2RThread)  {
      GFL_PRINT("wait sleep: Y2R\n");
      pThis->mpY2RThread->WaitForSleep();
    }
    if(pThis->mpCameraThread) {
      GFL_PRINT("wait sleep: CAM\n");
      pThis->mpCameraThread->WaitForSleep();
    }
  }
  //GFL_PRINT("slept\n");
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   DeviceCamera::AwakeRequest(void) {
  DeviceCamera*                       pThis = GFL_SINGLETON_INSTANCE(DeviceCamera);

  if(pThis)  {
    nn::os::CriticalSection::ScopedLock sl(pThis->mCS[CS_SLEEPCOUNTER_ACCESS]);

    if(pThis->mSleepCounter)
    {
      --pThis->mSleepCounter;
      GFL_PRINT("sleep cnt: %d\n", pThis->mSleepCounter);
      if(!pThis->mSleepCounter)
      {
        if(pThis->mpCameraThread) {
          GFL_PRINT("awake req: Y2R\n");
          pThis->mpCameraThread->AwakeRequest();
        }
        if(pThis->mpY2RThread)  {
          GFL_PRINT("awake req: CAM\n");
          pThis->mpY2RThread->AwakeRequest();
        }
      }
    }

  }
  //GFL_PRINT("awake\n");
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
const u8* DeviceCamera::GetYUVImage(const CameraType type, u32* pSize)  {
  const u8*   pYUV  = NULL;

  switch(camSel2Port(type))  {
    case  nn::camera::PORT_CAM1:  pYUV  = mCaptureBuffer[0].GetReadBuffer();  if(pSize) {*pSize  = mCaptureBuffer[0].GetSize();} break;
    case  nn::camera::PORT_CAM2:  pYUV  = mCaptureBuffer[1].GetReadBuffer();  if(pSize) {*pSize  = mCaptureBuffer[1].GetSize();} break;
  }

  return pYUV;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
uptr  DeviceCamera::GetTextureAddr(const Y2RThread::Ch ch)  {
  const void*  pImage  = mY2RBuffer[ch].GetReadBuffer();

  return (pImage ? nngxGetPhysicalAddr(reinterpret_cast<uptr>(pImage)) : NULL);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool    DeviceCamera::SetPicturePaneTexture(gfl2::lyt::LytPicture* pPicturePane, const Y2RThread::Ch ch, const u16 imageW, const u16 imageH)  {
  bool  isValid = false;
  u32   w,  h;

  if(this->GetImageSize(w, h))  {
    const uptr  physicalAddr  = this->GetTextureAddr(ch);

    if(physicalAddr)  {
      nw::lyt::Material*    pNWMaterial = pPicturePane->GetMaterial();
      nw::lyt::TextureInfo  texInfo;
      const u16             imageW_     = (imageW) ? imageW : w;
      const u16             imageH_     = (imageH) ? imageH : h;
      const u16             realW_      = nn::math::CeilPwr2(imageW_);
      const u16             realH_      = nn::math::CeilPwr2(imageH_);

      pNWMaterial->GetTexMap(0).GetTextureInfo(&texInfo);

      texInfo.Set(
        texInfo.GetTextureObject(),
        physicalAddr,
        nw::lyt::TexSize(imageW_, imageH_),
        nw::lyt::TexSize(realW_, realH_),
        texInfo.GetFormat(),
        nw::lyt::TEXELROTATION_NONE
      );

      pNWMaterial->SetTexMap(0, &texInfo);
      isValid = true;
    }
  }

  return isValid;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool    DeviceCamera::GetImageSize(u32& rW, u32& rH) const  {
  bool  isValid = false;

  if(mpY2RThread) {
    mpY2RThread->GetImageSize(rW, rH);
    isValid = true;
  }

  return isValid;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  DeviceCamera::SetResolution(    const Resolution      &rRHS)  {if(mpCameraThread) mpCameraThread->SetResolution(rRHS);}
void  DeviceCamera::SetPhotoMode(     const PhotoMode       &rRHS)  {if(mpCameraThread) mpCameraThread->SetPhotoMode(rRHS);}
void  DeviceCamera::SetExposure(      const Exposure        &rRHS)  {if(mpCameraThread) mpCameraThread->SetExposure(rRHS);}
void  DeviceCamera::SetWhiteBalance(  const WhiteBalance    &rRHS)  {if(mpCameraThread) mpCameraThread->SetWhiteBalance(rRHS);}
void  DeviceCamera::SetSharpness(     const Sharpness       &rRHS)  {if(mpCameraThread) mpCameraThread->SetSharpness(rRHS);}
void  DeviceCamera::SetFlip(          const Flip            &rRHS)  {if(mpCameraThread) mpCameraThread->SetFlip(rRHS);}
void  DeviceCamera::SetFrameRate(     const FrameRate       &rRHS)  {if(mpCameraThread) mpCameraThread->SetFrameRate(rRHS);}
void  DeviceCamera::SetEffect(        const Effect          &rRHS)  {if(mpCameraThread) mpCameraThread->SetEffect(rRHS);}
void  DeviceCamera::SetContrast(      const Contrast        &rRHS)  {if(mpCameraThread) mpCameraThread->SetContrast(rRHS);}
void  DeviceCamera::SetLensCorrection(const LensCorrection  &rRHS)  {if(mpCameraThread) mpCameraThread->SetLensCorrection(rRHS);}
void  DeviceCamera::SetNoiseFilter(   const NoiseFilter     &rRHS)  {if(mpCameraThread) mpCameraThread->SetNoiseFilter(rRHS);}
void  DeviceCamera::ApplyContext(void)                              {if(mpCameraThread) mpCameraThread->ApplyContext();}


















}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */
#endif