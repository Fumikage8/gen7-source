#if !defined(GFL2_DEVICECAMERATHREAD_H_INCLUDED)
#define GFL2_DEVICECAMERATHREAD_H_INCLUDED
#pragma once
#if defined(GF_PLATFORM_CTR)
//==============================================================================
/**
 * @file   gfl2_DeviceCameraThread.h
 * @date   2013/03/12 Tue.
 * @date   2015/07/30 Thu. 14:49:57
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <nn.h>
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <debug/include/gfl2_Assert.h>

#include  "DeviceCamera/include/gfl2_CaptureBuffer.h"
#include  "DeviceCamera/include/gfl2_DeviceCameraParam.h"


namespace  gfl2  {
namespace  devicecamera  {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @note  （撮影サイズなどの）Context切り替え(nn::camera::CTR::Context)には対応していない
  *  @date  2015/07/30 Thu. 14:50:45
  */
//==============================================================================
class DeviceCameraThread  {
  GFL_FORBID_COPY_AND_ASSIGN(DeviceCameraThread);
public:
  /*
    スレッド優先デフォルト
  */
  static const s32  DEFAULT_THREAD_PRIORITY_ACTIVE      = nn::os::DEFAULT_THREAD_PRIORITY - 3;    /**<  キャプチャ中のスレッド優先度                        */
  static const s32  DEFAULT_THREAD_PRIORITY_INACTIVE    = nn::os::DEFAULT_THREAD_PRIORITY + 3;    /**<  初期化/終了処理などを行っているときのスレッド優先度 */


  //------------------------------------------------------------------
  /**
    *  @brief    setupパラメータ
    */
  //------------------------------------------------------------------
  struct  Config  {
    gfl2::heap::HeapBase*     pAppHeap;
    gfl2::heap::HeapBase*     pDevHeap;
    CaptureBuffer*            pCaptureBuffer[2];            /**<  各ポート用バッファ  */
    s32                       threadPriority_Active;        /**<  キャプチャ中のスレッド優先度：撮影画像の更新速度を最大とするため、通常はメインスレッドより高く（値を小さく）する                    */
    s32                       threadPriority_Inactive;      /**<  初期化/終了処理などを行っているときのスレッド優先度：通常はメインスレッドを停止させないようメインスレッドより低く（値を大きく）する */

    Config(void)
      : pAppHeap(NULL)
      , pDevHeap(NULL)
      , threadPriority_Active(DEFAULT_THREAD_PRIORITY_ACTIVE)
      , threadPriority_Inactive(DEFAULT_THREAD_PRIORITY_INACTIVE)
    {
      pCaptureBuffer[0] =
      pCaptureBuffer[1] = NULL;
    }
  };

  DeviceCameraThread(void);
  virtual ~DeviceCameraThread();

  //------------------------------------------------------------------
  /**
    *  @brief    使用準備
    *  @note     内部で自動的にCleanup()がコールされる
    *  @note     Setupをコールすると起動待ち状態（Sleepとは異なる）となるので、StartRequestで撮影を開始する
    */
  //------------------------------------------------------------------
  void            Setup(const Config& rConfig);

  //------------------------------------------------------------------
  /**
    *  @brief    使用終了
    *  @detail   撮影を停止し、撮影スレッドをKillする
    *  @note     Setup()内部でも自動的にコールされる
    */
  //------------------------------------------------------------------
  void            Cleanup(void);
  inline bool     IsError(void) const       {return mIsFatalError;}

  inline bool     IsCaptured(void)          {return mLightEvent[LEVENT_CAMTHREAD_CAPTURED].TryWait();}
  inline void     ResetCaptured(void)       {mLightEvent[LEVENT_CAMTHREAD_CAPTURED].ClearSignal();}

  inline void     StartRequest(void)        {mLightEvent[LEVENT_CAMTHREAD_START_REQ].Signal();}   /**<  撮影開始  */
  inline void     SleepRequest(void)        {mLightEvent[LEVENT_CAMTHREAD_SLEEP_REQ].Signal();}   /**<  撮影中断  */
  inline void     AwakeRequest(void)        {mLightEvent[LEVENT_CAMTHREAD_AWAKE_REQ].Signal();}   /**<  撮影再開  */
  inline void     WaitForSleep(void)        {mEvent[EVENT_CAM_PORT1_BUFERROR].Signal(); mLightEvent[LEVENT_CAMTHREAD_SLEPT].Wait();}

  bool            GetY2RCoef(nn::y2r::StandardCoefficient* pDst) const;


  /*
    各種設定
    ※CameraTypeとTrimmingは撮影中には変更できない
  */
  void    SetCameraType(      const CameraType&     rRHS)   {mContext.Lock();   mContext.camType      = rRHS;   mContext.Unlock();}
  void    SetTrimming(        const Trimming&       rRHS)   {mContext.Lock();   mContext.trimming     = rRHS;   mContext.Unlock();}

  void    SetResolution(      const Resolution&     rRHS)   {mContext.Lock();   mContext.resolution   = rRHS;   mContext.Unlock();}
  void    SetPhotoMode(       const PhotoMode&      rRHS)   {mContext.Lock();   mContext.photoMode    = rRHS;   mContext.Unlock();}
  void    SetExposure(        const Exposure&       rRHS)   {mContext.Lock();   mContext.exposure     = rRHS;   mContext.Unlock();}
  void    SetWhiteBalance(    const WhiteBalance&   rRHS)   {mContext.Lock();   mContext.wb           = rRHS;   mContext.Unlock();}
  void    SetSharpness(       const Sharpness&      rRHS)   {mContext.Lock();   mContext.sharpness    = rRHS;   mContext.Unlock();}
  void    SetFlip(            const Flip&           rRHS)   {mContext.Lock();   mContext.flip         = rRHS;   mContext.Unlock();}
  void    SetFrameRate(       const FrameRate&      rRHS)   {mContext.Lock();   mContext.fps          = rRHS;   mContext.Unlock();}
  void    SetEffect(          const Effect&         rRHS)   {mContext.Lock();   mContext.effect       = rRHS;   mContext.Unlock();}
  void    SetContrast(        const Contrast&       rRHS)   {mContext.Lock();   mContext.contrast     = rRHS;   mContext.Unlock();}
  void    SetLensCorrection(  const LensCorrection& rRHS)   {mContext.Lock();   mContext.lc           = rRHS;   mContext.Unlock();}
  void    SetNoiseFilter(     const NoiseFilter&    rRHS)   {mContext.Lock();   mContext.noiseFilter  = rRHS;   mContext.Unlock();}

  void    ApplyContext(void);

  void    ForceFatal(void)
  {
#if GFL_DEBUG
    m_IsForceFatal  = true;
#endif
  }

protected:
private:
  /** カメラパラメータ  */
  template<class Prm>
  struct  CameraPrm {
    bool  isDirty;
    Prm   prm;

    CameraPrm(void) : isDirty(true), prm()    {}
    const CameraPrm<Prm>&  operator =(const Prm& rRHS) {prm  = rRHS;   isDirty = true;  return *this;}
  };

  /*  カメラ設定  */
  struct  Context {
    nn::os::CriticalSection   criticalSection;
    CameraPrm<CameraType>     camType;
    CameraPrm<Resolution>     resolution;
    CameraPrm<PhotoMode>      photoMode;
    CameraPrm<Trimming>       trimming;
    CameraPrm<Exposure>       exposure;
    CameraPrm<WhiteBalance>   wb;
    CameraPrm<Sharpness>      sharpness;
    CameraPrm<Flip>           flip;
    CameraPrm<FrameRate>      fps;
    CameraPrm<Effect>         effect;
    CameraPrm<Contrast>       contrast;
    CameraPrm<LensCorrection> lc;
    CameraPrm<NoiseFilter>    noiseFilter;


    Context(void)
      : criticalSection(nn::WithInitialize())
      , camType()
      , resolution()
      , photoMode()
      , trimming()
      , exposure()
      , wb()
      , sharpness()
      , flip()
      , fps()
      , effect()
      , contrast()
      , lc()
      , noiseFilter()
    {
    }

    void  SetAllDirty(const bool isDirty)
    {
      Lock();
      {
        camType.isDirty     =
        resolution.isDirty  =
        photoMode.isDirty   =
        trimming.isDirty    =
        exposure.isDirty    =
        wb.isDirty          =
        sharpness.isDirty   =
        flip.isDirty        =
        fps.isDirty         =
        effect.isDirty      =
        contrast.isDirty    =
        lc.isDirty          =
        noiseFilter.isDirty = isDirty;
      }
      Unlock();
    }

    void  Lock(void)    {criticalSection.Enter();}
    void  Unlock(void)  {criticalSection.Leave();}
  };
  

  enum  EventID {
    /*  撮影関連  */
    EVENT_CAM_PORT1_RECV,
    EVENT_CAM_PORT1_BUFERROR,
    EVENT_CAM_PORT1_VSYNC,
    EVENT_CAM_PORT2_RECV,
    EVENT_CAM_PORT2_BUFERROR,
    EVENT_CAM_PORT2_VSYNC,

    EVENT_NUM
  };

  enum  LightEventID  {
    LEVENT_CAMTHREAD_START_REQ,
    LEVENT_CAMTHREAD_SLEEP_REQ,           /**<  カメラスレッドスリープ要求  */
    LEVENT_CAMTHREAD_SLEPT,               /**<  カメラスレッドスリープ通知  */
    LEVENT_CAMTHREAD_AWAKE_REQ,           /**<  カメラスレッド起床          */
    LEVENT_CAMTHREAD_KILL_REQ,            /**<  カメラスレッドkill要求      */
    LEVENT_CAMTHREAD_APPLY_CONTEXT_REQ,   /**<  applyContextInternal要求    */
    LEVENT_CAMTHREAD_CAPTURED,            /**<  撮影画像がある              */
    LEVENT_NUM
  };


  /**   */
  enum  State {
    STATE_STANDBY,        /**<  初期状態    */
    STATE_INITIALIZE,     /**<  初期化中    */
    STATE_IDLE,           /**<  待機状態    */
    STATE_SETUP,          /**<  起動中      */
    STATE_CAPTURE,        /**<  撮影中      */
    STATE_ERROR,          /**<  エラー      */
    STATE_CLEANUP,        /**<  停止中      */
    STATE_FINALIZE,       /**<  終了中      */
  };

  Config                        mConfig;
  ThreadStack*                  mpThreadStack;
  nn::os::Thread                mThread;
  nn::os::Event                 mEvent[EVENT_NUM];
  nn::os::LightEvent            mLightEvent[LEVENT_NUM];
  State                         mState;
  Context                       mContext;
  s16                           mOutImageSize[2];
  nn::y2r::StandardCoefficient  mY2RCoef;

  bool                          mIsY2RCoefAvailable;
  bool                          mIsFatalError;          /**<    */
#if GFL_DEBUG
  bool                          m_IsForceFatal;
#endif

  static void   ThreadMain(DeviceCameraThread* pThis);
  void          ThreadProc_Initialize(void);
  void          ThreadProc_Idle(void);
  void          ThreadProc_Setup(void);
  void          ThreadProc_Capture(void);
  void          ThreadProc_Error(void);
  void          ThreadProc_Cleanup(void);
  void          ThreadProc_Finalize(void);

  void          QuerySleep(void);
  bool          QueryKill(void)               {return mLightEvent[LEVENT_CAMTHREAD_KILL_REQ].TryWait();}
  bool          ResultHandler(const nn::Result nnResult);
  bool          IsAvailable(void)             {return (!mIsFatalError && !mLightEvent[LEVENT_CAMTHREAD_KILL_REQ].TryWait());}
  inline void   KillRequest(void)             {mLightEvent[LEVENT_CAMTHREAD_KILL_REQ].Signal();}

  void  StartCapture(void);
  void  SetReceiving(const nn::camera::Port port);
  void  OnReceive(const nn::camera::Port port);
  void  StopCapture(void);

  void  ApplyContextInternal(void);
};


}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */
#endif
#endif  /*  #if !defined(GFL2_DEVICECAMERATHREAD_H_INCLUDED)  */



