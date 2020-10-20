#if !defined(GFL2_DEVICECAMERA_H_INCLUDED)
#define GFL2_DEVICECAMERA_H_INCLUDED
#pragma once
#if defined(GF_PLATFORM_CTR)
//==============================================================================
/**
 * @file   gfl2_DeviceCamera.h
 * @date   2015/07/30 Thu. 13:05:50
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
#include  <base/include/gfl2_Singleton.h>
#include  <layout/include/gfl2_Layout.h>

#include  "DeviceCamera/include/gfl2_DeviceCameraParam.h"
#include  "DeviceCamera/include/gfl2_DeviceCameraThread.h"
#include  "DeviceCamera/include/gfl2_Y2RThread.h"
#include  "DeviceCamera/include/gfl2_CaptureBuffer.h"


namespace  gfl2  {
namespace  devicecamera  {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @brief  デバイスカメラ制御
  *  @detail Y2Rを使わずCameraのみを扱うことは通常発生しないので、Y2R制御までをサポートする
  *  @date   2015/07/30 Thu. 13:06:35
  */
//==============================================================================
class DeviceCamera : public gfl2::base::Singleton<DeviceCamera>  {
  GFL_FORBID_COPY_AND_ASSIGN( DeviceCamera );

public:

  //------------------------------------------------------------------
  /**
    *  @brief   設定情報
    *  @note    最低限、
    *             pAppHeap
    *             pDevHeap
    *             camType
    *             outImageSize
    *           の設定が必要
    */
  //------------------------------------------------------------------
  struct  Config  {
    gfl2::heap::HeapBase*   pAppHeap;    /**<  システムメモリであること  */
    gfl2::heap::HeapBase*   pDevHeap;    /**<  デバイスメモリであること  */

    /*
      スレッド優先

      ←高優先（小さい値）                                                                            低優先（大きい値）→
      cameraThread_Active < y2rThreadPriority_Active < メインスレッド < cameraThread_Inactive <= y2rThreadPriority_Inactive

      となるように設定する
    */
    s32                     cameraThread_Active;            /**<  撮影スレッド優先：active時    */
    s32                     cameraThread_Inactive;          /**<  撮影スレッド優先：非active時  */
    s32                     y2rThreadPriority_Active;       /**<  変換スレッド優先：active時    */
    s32                     y2rThreadPriority_Inactive;     /**<  変換スレッド優先：非active時  */

    CameraType              camType;                        /**<  カメラ種別  */
    Resolution              resolution;                     /**<  解像度      */

    /**
      撮影モード
      @note
        撮影モードはガンマやゲインなどの調整をしていますが、他にもシャープネス、露出、ホワイトバランスの設定も上書きしています。 
        以下が設定される値になります。 
                                    NORMAL      PORTRAIT    LANDSCAPE   NIGHTVIEW   LETTER
          Sharpness                 0           -2          +1          -1          +2
          Exposure                  0           0           0           +2          +2
          WhiteBalance              NORMAL      NORMAL      DAYLIGHT    NORMAL      NORMAL
          Contrast                  NORMAL      LOW         NORMAL      NORMAL      HIGH
          ゲイン (APIでは設定不可)  通常        通常        通常        最大        通常
          AutoExposureWindow        外:全体     中心        外:全体     外:全体     外:全体
                                    内:中心     中心        内:中心     内:中心     内:中心 
      @note
        上の表における AutoExposureWindowの設定値は、nn::camera::CTR::SetAutoExposureWindow 関数において以下の指定をすることに相当します。
                    測光エリアの全体        測光エリアの中心
          startX    0                       80
          startY    0                       60
          width     640                     480
          height    480                     360
    */
    PhotoMode       photoMode;

    /**
      トリミング設定
      @note
      設定できる値には以下の制限があります。
        ・トリミング範囲は (xStart, yStart) から (xEnd-1, yEnd-1) までとなる。
        ・xEnd - xStart, 及び yEnd - yStart の値が偶数になる必要がある。
        ・xStart および yStart も偶数である必要がある。
    */
    Trimming        trimming;
    Exposure        exposure;         /**<  自動露光            */
    WhiteBalance    whiteBalance;     /**<  ホワイトバランス    */
    Sharpness       sharpness;        /**<  シャープネス        */
    Flip            flip;             /**<  反転                */
    FrameRate       frameRate;        /**<  フレームレート      */
    Effect          effect;           /**<  エフェクト          */
    Contrast        contrast;         /**<  コントラスト        */
    LensCorrection  lensCorrection;   /**<  レンズコレクション  */
    NoiseFilter     noiseFilter;      /**<  ノイズフィルタ      */

    /**
      出力画像（バッファ）のサイズ w, h
      @note
        出力画像を格納するバッファのサイズを指定します。
        strideには対応させていないので、幅は入力画像に合わせるべきです。
    */
    u16                       outImageSize[2];
    Y2RThread::OutputFormat   outputFormat;
    u8                        alpha;

    Config(void)
      : pAppHeap(NULL)
      , pDevHeap(NULL)
      , cameraThread_Active(DeviceCameraThread::DEFAULT_THREAD_PRIORITY_ACTIVE)
      , cameraThread_Inactive(DeviceCameraThread::DEFAULT_THREAD_PRIORITY_INACTIVE)
      , y2rThreadPriority_Active(Y2RThread::DEFAULT_THREAD_PRIORITY_ACTIVE)
      , y2rThreadPriority_Inactive(Y2RThread::DEFAULT_THREAD_PRIORITY_INACTIVE)
      , camType(CAM_NONE)
      , resolution()                     /**<  解像度      */
      , photoMode()
      , trimming()
      , exposure()         /**<  自動露光            */
      , whiteBalance()     /**<  ホワイトバランス    */
      , sharpness()        /**<  シャープネス        */
      , flip()             /**<  反転                */
      , frameRate()        /**<  フレームレート      */
      , effect()           /**<  エフェクト          */
      , contrast()         /**<  コントラスト        */
      , lensCorrection()   /**<  レンズコレクション  */
      , noiseFilter()      /**<  ノイズフィルタ      */
      , outputFormat(Y2RThread::OUTPUT_RGB_24)
      , alpha(0xff)
    {
      outImageSize[0] = 0;
      outImageSize[1] = 0;
    }
  };

  enum  State {
    STATE_MAX
  };

  DeviceCamera(void);
  virtual ~DeviceCamera();

  //------------------------------------------------------------------
  /**
    *  @brief    使用準備
    *  @note     撮影は開始しない
    */
  //------------------------------------------------------------------
  void    Setup(const Config& rConfig);

  //------------------------------------------------------------------
  /**
    *  @brief    使用終了
    *  @note     Threadの待ち合わせなどを行うので、しばらくブロックされる可能性がある
    *            暗転時などに行うほうが良い
    */
  //------------------------------------------------------------------
  void    Cleanup(void);

  //------------------------------------------------------------------
  /**
    *  @brief    撮影開始
    */
  //------------------------------------------------------------------
  void    Start(void);

  //------------------------------------------------------------------
  /**
    *  @brief    状態の更新
    */
  //------------------------------------------------------------------
  State   Update(void);


  //------------------------------------------------------------------
  /**
    *  @brief    エラー状態であるか
    */
  //------------------------------------------------------------------
  bool    IsError(void) const   {return (mpCameraThread ? mpCameraThread->IsError() : false);}




  //------------------------------------------------------------------
  /**
    *  @brief    YUV画像の取得
    *  @note     取得したデータはUpdate()でSwapされ無効となる
    */
  //------------------------------------------------------------------
  const u8* GetYUVImage(const CameraType type, u32* pSize = NULL);

  //------------------------------------------------------------------
  /**
    *  @brief    Y2R変換後のTextureデータの取得
    *  @note     取得したデータはUpdate()でSwapされ無効となる
    */
  //------------------------------------------------------------------
  uptr      GetTextureAddr(const Y2RThread::Ch ch);

  //------------------------------------------------------------------
  /**
    *  @brief    Y2R変換後のTextureデータをPicturePaneに適用する
    *  @note     PicturePaneは、Y2R出力フォーマットにあわせてダミーテクスチャを貼っておく
    */
  //------------------------------------------------------------------
  bool      SetPicturePaneTexture(gfl2::lyt::LytPicture* pPicturePane, const Y2RThread::Ch ch, const u16 imageW = 0, const u16 imageH = 0);

  //------------------------------------------------------------------
  /**
    *  @brief    Y2R出力画像サイズの取得
    */
  //------------------------------------------------------------------
  bool      GetImageSize(u32& rW, u32& rH) const;




  /*
    context設定
    Setup後に有効
    
    CameraTypeとTrimmingは撮影中には設定できないので省略
  */
//  void  SetCameraType(    const CameraType      &rCamType);         /**<  カメラ種別  */
//  void  SetTrimming(      const Trimming        &rTrimming);        
  void  SetResolution(    const Resolution      &rRHS);  /**<  解像度              */
  void  SetPhotoMode(     const PhotoMode       &rRHS);
  void  SetExposure(      const Exposure        &rRHS);  /**<  自動露光            */
  void  SetWhiteBalance(  const WhiteBalance    &rRHS);  /**<  ホワイトバランス    */
  void  SetSharpness(     const Sharpness       &rRHS);  /**<  シャープネス        */
  void  SetFlip(          const Flip            &rRHS);  /**<  反転                */
  void  SetFrameRate(     const FrameRate       &rRHS);  /**<  フレームレート      */
  void  SetEffect(        const Effect          &rRHS);  /**<  エフェクト          */
  void  SetContrast(      const Contrast        &rRHS);  /**<  コントラスト        */
  void  SetLensCorrection(const LensCorrection  &rRHS);  /**<  レンズコレクション  */
  void  SetNoiseFilter(   const NoiseFilter     &rRHS);  /**<  ノイズフィルタ      */

  void  ApplyContext(void);


  /*
    debug
  */
  void  ForceFatal(void)
  {
#if GFL_DEBUG
    if(mpCameraThread)
    {
      mpCameraThread->ForceFatal();
    }
#endif
  }


  /*
    蓋閉じ対応用
  */
  //------------------------------------------------------------------
  /**
    *  @brief    スリープ要求
    *  @note     蓋閉じに対応するため、蓋閉じ検知後、メインスレッド停止前にコールする必要がある
    *  @note     要求を投げるだけなので、SleepRequest()～WaitForSleep()間で他の処理を行うことが可能（セーブの終了待ちなど）
    *  @note     未初期化時は何も行わない
    */
  //------------------------------------------------------------------
  static void   SleepRequest(void);

  //------------------------------------------------------------------
  /**
    *  @brief    スリープ移行待ち
    *  @note     DeviceCamera/Y2Rが停止しSleep状態に移行するまでブロックする
    *  @note     蓋閉じに対応するため、蓋閉じ検知後、メインスレッド停止前にコールする必要がある
    *  @note     未初期化時は何も行わない
    */
  //------------------------------------------------------------------
  static void   WaitForSleep(void);

  //------------------------------------------------------------------
  /**
    *  @brief    起床要求
    *  @note     蓋開けを検知し、LCDを再起動した後に本関数でカメラを再起動する
    *  @note     未初期化時は何も行わない
    */
  //------------------------------------------------------------------
  static void   AwakeRequest(void);


protected:
private:
  enum  CSID  {
    CS_SLEEPCOUNTER_ACCESS,
    
    CS_MAX
  };

  nn::os::CriticalSection     mCS[CS_MAX];

  State                       mState;
  DeviceCameraThread*         mpCameraThread;
  CaptureBuffer               mCaptureBuffer[2];
  Y2RThread*                  mpY2RThread;
  Y2RBuffer                   mY2RBuffer[Y2RThread::CH_MAX];
  u32                         mSleepCounter;

};


/*  ********************************************************************
    global
  *******************************************************************/


/*  ********************************************************************
    prototype
  *******************************************************************/


}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */
SINGLETON_INSTANCE_DECLARATION( gfl2::devicecamera::DeviceCamera );
#endif  /*  #if defined(GF_PLATFORM_CTR)                */
#endif  /*  #if !defined(GFL2_DEVICECAMERA_H_INCLUDED)  */



