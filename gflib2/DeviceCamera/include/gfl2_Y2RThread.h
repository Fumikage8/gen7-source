#if !defined(GFL2_Y2RTHREAD_H_INCLUDED)
#define GFL2_Y2RTHREAD_H_INCLUDED
#pragma once
#if defined(GF_PLATFORM_CTR)
//==============================================================================
/**
 * @file   gfl2_Y2RThread.h
 * @date   2015/07/30 Thu. 14:43:28 過去プロジェクトより移植
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


namespace  gfl2  {
namespace  devicecamera  {
class DeviceCameraThread;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  Y2RThread
  *  @date   2015/07/30 Thu. 14:52:52
  */
//==============================================================================
class Y2RThread {
  GFL_FORBID_COPY_AND_ASSIGN(Y2RThread);
public:
  /*
    スレッド優先デフォルト
  */
  static const s32  DEFAULT_THREAD_PRIORITY_ACTIVE      = nn::os::DEFAULT_THREAD_PRIORITY - 2;    /**<  コンバート受付/処理中のスレッド優先度               */
  static const s32  DEFAULT_THREAD_PRIORITY_INACTIVE    = nn::os::DEFAULT_THREAD_PRIORITY + 3;    /**<  初期化/終了処理などを行っているときのスレッド優先度 */


  /**
    コンバートチャンネル
    使用するチャンネルの数、どちらのチャンネルに何を入れるのかは使用側に委ねる
  */
  enum  Ch  {
    CH_1,
    CH_2,
    
    CH_MAX
  };

  enum  InputFormat  {
    INPUT_YUV422_INDIV_8   = nn::y2r::INPUT_YUV422_INDIV_8,
    INPUT_YUV420_INDIV_8   = nn::y2r::INPUT_YUV420_INDIV_8,
    INPUT_YUV422_INDIV_16  = nn::y2r::INPUT_YUV422_INDIV_16,
    INPUT_YUV420_INDIV_16  = nn::y2r::INPUT_YUV420_INDIV_16,
    INPUT_YUV422_BATCH     = nn::y2r::INPUT_YUV422_BATCH,

    INPUT_FROM_CAMERA     = INPUT_YUV422_BATCH
  };

  enum  OutputFormat  {
    OUTPUT_RGB_32       = nn::y2r::OUTPUT_RGB_32,
    OUTPUT_RGB_24       = nn::y2r::OUTPUT_RGB_24,
    OUTPUT_RGB_16_5551  = nn::y2r::OUTPUT_RGB_16_555,
    OUTPUT_RGB_16_565   = nn::y2r::OUTPUT_RGB_16_565,
  };

  enum  Rotation  {
    ROT_NONE            = nn::y2r::ROTATION_NONE,
    ROT_DEGREE_90       = nn::y2r::ROTATION_CLOCKWISE_90,
    ROT_DEGREE_180      = nn::y2r::ROTATION_CLOCKWISE_180,
    ROT_DEGREE_270      = nn::y2r::ROTATION_CLOCKWISE_270,
  };


  enum  BlockAlignment  {
    ALIGN_BLOCK   = nn::y2r::BLOCK_8_BY_8,    /**<  8x8ブロックフォーマット  */
    ALIGN_LINE    = nn::y2r::BLOCK_LINE,      /**<  水平ラインフォーマット   */

    ALIGN_FOR_TEXTURE = ALIGN_BLOCK           /**<  テクスチャ用             */
  };

  //------------------------------------------------------------------
  /**
    *  @brief    setupパラメータ
    */
  //------------------------------------------------------------------
  struct Config {
    gfl2::heap::HeapBase*         pAppHeap;
    gfl2::heap::HeapBase*         pDevHeap;
    Y2RBuffer*                    pBuffer[CH_MAX];
    s32                           threadPriority_Active;
    s32                           threadPriority_Inactive;
    InputFormat                   inputFormat;
    OutputFormat                  outputFormat;
    Rotation                      rotation;
    BlockAlignment                blockAlignment;
    u16                           imageSize[2];           /**<  w : 1024以下の8の倍数, h  */
    u8                            alpha;
    const DeviceCameraThread*     pDeviceCameraThread;    /**<  カメラ画像を入力する場合は必須  */

    Config(void) :
        pAppHeap(NULL)
      , pDevHeap(NULL)
      , threadPriority_Active(DEFAULT_THREAD_PRIORITY_ACTIVE)
      , threadPriority_Inactive(DEFAULT_THREAD_PRIORITY_INACTIVE)
      , inputFormat(INPUT_FROM_CAMERA)
      , outputFormat(OUTPUT_RGB_24)
      , rotation(ROT_NONE)
      , blockAlignment(ALIGN_FOR_TEXTURE)
      , alpha(0xff)
      , pDeviceCameraThread(NULL)
    {
      for(u32 num = 0; num < GFL_NELEMS(pBuffer); ++num)  {
        pBuffer[num]  = NULL;
      }
      imageSize[0]  = 640;
      imageSize[1]  = 480;
    }
  };


  Y2RThread(void);
  virtual ~Y2RThread();

  void  Setup(const Config& rConfig);
  void  Cleanup(void);
  void  ConvertRequest(const u8* pImage_Ch1, const u8* pImage_Ch2 = NULL);

  inline void  SleepRequest(void)    {mLightEvent[LEVENT_Y2RTHREAD_SLEEP_REQ].Signal();}
  inline void  AwakeRequest(void)    {mLightEvent[LEVENT_Y2RTHREAD_AWAKE_REQ].Signal();}
  inline void  WaitForSleep(void)    {mLightEvent[LEVENT_Y2RTHREAD_SLEPT].Wait();}

  inline bool  IsIdle(void) const    {return (mState == STATE_IDLE);}
  inline bool  IsConverted(void)     {return mLightEvent[LEVENT_Y2RTHREAD_CONVERTED].TryWait();}
  inline bool  IsError(void) const   {return mIsFatalError;}

  void  GetImageSize(u32& rW, u32& rH);

  static u32   GetOutputFrameBytes(const OutputFormat format, const u32 w, const u32 h);

protected:
private:
  struct Context  {
    u32   inputImageBytes;
    u32   outputImageBytes;
    u16   inputTransferUnit;
    u16   outputTransferUnit;

    Context(void) :
        inputImageBytes(0)
      , outputImageBytes(0)
      , inputTransferUnit(0)
      , outputTransferUnit(0)
    {}
  };

  struct  Environment {
    const u8*   pNextSrc;
    const u8*   pCurrentSrc;
    Y2RBuffer*  pBuffer;
    
    Environment(void) :
       pNextSrc(NULL)
      ,pCurrentSrc(NULL)
      ,pBuffer(NULL)
    {}
  };

  enum  EventID {
    /*  撮影関連  */
    EVENT_Y2R_TRANS_END,

    EVENT_NUM
  };

  enum  LightEventID  {
    LEVENT_Y2RTHREAD_START_REQ,
    LEVENT_Y2RTHREAD_SLEEP_REQ,   /**<  Y2Rスレッドスリープ要求  */
    LEVENT_Y2RTHREAD_SLEPT,       /**<  Y2Rスレッドスリープ通知  */
    LEVENT_Y2RTHREAD_AWAKE_REQ,   /**<  Y2Rスレッド起床          */
    LEVENT_Y2RTHREAD_KILL_REQ,    /**<  Y2Rスレッドkill要求      */
    LEVENT_Y2RTHREAD_CONVERTED,   /**<  変換完了                 */

    LEVENT_NUM
  };

  enum  CSID  {
    CS_UPDATE_SRC,
    
    CS_MAX
  };


  /**   */
  enum  State {
    STATE_STANDBY,        /**<  初期状態    */
    STATE_SETUP,          /**<  設定中      */
    STATE_IDLE,           /**<  待機状態    */
    STATE_CONVERT,        /**<  変換中      */
    STATE_ERROR,          /**<  エラー      */
    STATE_CLEANUP,        /**<  終了中      */
  };



  ThreadStack*                mpThreadStack;
  nn::os::Thread              mThread;
  s32                         mThreadPriority_Active;
  s32                         mThreadPriority_Inactive;
  nn::os::CriticalSection     mCS[CS_MAX];
  nn::os::Event               mEvent[EVENT_NUM];
  nn::os::LightEvent          mLightEvent[LEVENT_NUM];
  nn::y2r::PackageParameter   mPackagePrm;
  const DeviceCameraThread*   mpDeviceCameraThread;
  State                       mState;                     /**<  内部状態の簡易チェックのために値をセットしていますが、排他は行っていないので同期判定などには使えません  */
  Environment                 mEnv[CH_MAX];
  Context                     mContext;
  bool                        mIsFatalError;              /**<    */

  static void   ThreadMain(Y2RThread* pThis);
  void          ThreadProc_Setup(void);
  void          ThreadProc_Idle(void);
  void          ThreadProc_Convert(void);
  void          ThreadProc_Error(void);
  void          ThreadProc_Cleanup(void);

  void          QuerySleep(void);
  bool          QueryKill(void)               {return mLightEvent[LEVENT_Y2RTHREAD_KILL_REQ].TryWait();}
  bool          ResultHandler(const nn::Result nnResult);
  bool          IsAvailable(void)             {return (!mIsFatalError && !mLightEvent[LEVENT_Y2RTHREAD_KILL_REQ].TryWait());}
  inline void   KillRequest(void)             {mLightEvent[LEVENT_Y2RTHREAD_KILL_REQ].Signal();}
};


}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */
#endif
#endif  /*  #if !defined(GFL2_Y2RTHREAD_H_INCLUDED)  */





