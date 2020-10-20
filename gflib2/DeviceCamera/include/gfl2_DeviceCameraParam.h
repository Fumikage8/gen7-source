
#if !defined(GFL2_DEVICECAMERAPARAM_H_INCLUDED)
#define GFL2_DEVICECAMERAPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   gfl2_DeviceCameraParam.h
 * @date   2013/03/12 Tue.
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


namespace  gfl2  {
namespace  devicecamera  {
// =============================================================================
/**/
//==============================================================================
/** 状態  */
enum  State  {
  STATE_OK,
  STATE_ERROR,
};

/** カメラ指定  */
enum  CameraType  {
  CAM_NONE      = nn::camera::SELECT_NONE,      /**<  なし      */
  CAM_IN        = nn::camera::SELECT_IN1,       /**<  内        */
  CAM_OUT_L     = nn::camera::SELECT_OUT2,      /**<  外L       */
  CAM_OUT_R     = nn::camera::SELECT_OUT1,      /**<  外R       */
  CAM_IN_OUT_L  = nn::camera::SELECT_IN1_OUT2,  /**<  内&外L    */
  CAM_IN_OUT_R  = nn::camera::SELECT_IN1_OUT1,  /**<  内&外R    */
  CAM_OUT_LR    = nn::camera::SELECT_OUT1_OUT2, /**<  外LR      */
  CAM_ALL       = nn::camera::SELECT_ALL,       /**<  全        */
};


/** ホワイトバランス  */
enum  WhiteBalanceType  {
  WB_AUTO               = nn::camera::WHITE_BALANCE_AUTO,
  WB_3200K              = nn::camera::WHITE_BALANCE_3200K,
  WB_4150K              = nn::camera::WHITE_BALANCE_4150K,
  WB_5200K              = nn::camera::WHITE_BALANCE_5200K,
  WB_6000K              = nn::camera::WHITE_BALANCE_6000K,
  WB_7000K              = nn::camera::WHITE_BALANCE_7000K,
  WB_NORMAL             = nn::camera::WHITE_BALANCE_NORMAL,                   /**<  WB_AUTOと等価             */
  WB_TUNGSTEN           = nn::camera::WHITE_BALANCE_TUNGSTEN,                 /**<  タングステン光(白熱電球)  */
  WB_WHITE_FLUORESCENT  = nn::camera::WHITE_BALANCE_WHITE_FLUORESCENT_LIGHT,  /**<  白色蛍光灯                */
  WB_DAYLIGHT           = nn::camera::WHITE_BALANCE_DAYLIGHT,                 /**<  太陽光                    */
  WB_CLOUDY             = nn::camera::WHITE_BALANCE_CLOUDY,                   /**<  くもり                    */
  WB_HORIZON            = nn::camera::WHITE_BALANCE_HORIZON,                  /**<  夕焼け                    */
  WB_SHADE              = nn::camera::WHITE_BALANCE_SHADE,                    /**<  日陰                      */
};

/** フレームレート  */
enum  FrameRateType {
  FPS_15           = nn::camera::FRAME_RATE_15,         /**<  15fps固定   */
  FPS_15_TO_5      = nn::camera::FRAME_RATE_15_TO_5,    /**<  明るさに応じて15fpsから5fpsの間で自動的に変化   */
  FPS_15_TO_2      = nn::camera::FRAME_RATE_15_TO_2,    /**<  明るさに応じて15fpsから2fpsの間で自動的に変化   */
  FPS_10           = nn::camera::FRAME_RATE_10,         /**<  10fps固定   */
  FPS_8_5          = nn::camera::FRAME_RATE_8_5,        /**<  8.5fps固定    */
  FPS_5            = nn::camera::FRAME_RATE_5,          /**<  5fps固定    */
  FPS_20           = nn::camera::FRAME_RATE_20,         /**<  20fps固定   */
  FPS_20_TO_5      = nn::camera::FRAME_RATE_20_TO_5,    /**<  明るさに応じて20fpsから5fpsの間で自動的に変化   */
  FPS_30           = nn::camera::FRAME_RATE_30,         /**<  30fps固定   */
  FPS_30_TO_5      = nn::camera::FRAME_RATE_30_TO_5,    /**<  明るさに応じて30fpsから5fpsの間で自動的に変化   */
  FPS_15_TO_10     = nn::camera::FRAME_RATE_15_TO_10,   /**<  明るさに応じて15fpsから10fpsの間で自動的に変化    */
  FPS_20_TO_10     = nn::camera::FRAME_RATE_20_TO_10,   /**<  明るさに応じて20fpsから10fpsの間で自動的に変化    */
  FPS_30_TO_10     = nn::camera::FRAME_RATE_30_TO_10,   /**<  明るさに応じて30fpsから10fpsの間で自動的に変化    */
};

/** 撮影モード  */
enum  PhotoModeType {
  MODE_NORMAL     = nn::camera::PHOTO_MODE_NORMAL,      /**<  補正なし            */
  MODE_PORTRAIT   = nn::camera::PHOTO_MODE_PORTRAIT,    /**<  人物                */
  MODE_LANDSCAPE  = nn::camera::PHOTO_MODE_LANDSCAPE,   /**<  風景                */
  MODE_NIGHTVIEW  = nn::camera::PHOTO_MODE_NIGHTVIEW,   /**<  暗視                */
  MODE_LETTER     = nn::camera::PHOTO_MODE_LETTER,      /**<  文字撮影 (QRコード) */
};

/** フリップ  */
enum  FlipType  {
  FLIP_NONE         = nn::camera::FLIP_NONE,            /**<  反転処理を行わない  */
  FLIP_HORIZONTAL   = nn::camera::FLIP_HORIZONTAL,      /**<  左右反転を行う      */
  FLIP_VERTICAL     = nn::camera::FLIP_VERTICAL,       /**<  上下反転を行う      */
  FLIP_REVERSE      = nn::camera::FLIP_REVERSE,        /**<  180度回転を行う     */
};


/** エフェクト  */
enum  EffectType  {
  EFFECT_NONE     = nn::camera::EFFECT_NONE,        /**<  エフェクトなし      */
  EFFECT_MONO     = nn::camera::EFFECT_MONO,        /**<  モノクロ調      */
  EFFECT_SEPIA    = nn::camera::EFFECT_SEPIA,       /**<  セピア調      */
  EFFECT_NEGATIVE = nn::camera::EFFECT_NEGATIVE,    /**<  ネガポジ反転      */
  EFFECT_NEGAFILM = nn::camera::EFFECT_NEGAFILM,    /**<  フィルム調のネガポジ反転(NEGATIVEに対して、UとVの順番が入れ替わっている)      */
  EFFECT_SEPIA01  = nn::camera::EFFECT_SEPIA01,     /**<  セピア調      */
};

/** コントラスト  */
enum  ContrastType  {
  CONTRAST_01           = nn::camera::CONTRAST_PATTERN_01,  /**<  コントラストのパターンNo.1    */
  CONTRAST_02           = nn::camera::CONTRAST_PATTERN_02,  /**<  コントラストのパターンNo.2    */
  CONTRAST_03           = nn::camera::CONTRAST_PATTERN_03,  /**<  コントラストのパターンNo.3    */
  CONTRAST_04           = nn::camera::CONTRAST_PATTERN_04,  /**<  コントラストのパターンNo.4    */
  CONTRAST_05           = nn::camera::CONTRAST_PATTERN_05,  /**<  コントラストのパターンNo.5    */
  CONTRAST_06           = nn::camera::CONTRAST_PATTERN_06,  /**<  コントラストのパターンNo.6    */
  CONTRAST_07           = nn::camera::CONTRAST_PATTERN_07,  /**<  コントラストのパターンNo.7    */
  CONTRAST_08           = nn::camera::CONTRAST_PATTERN_08,  /**<  コントラストのパターンNo.8    */
  CONTRAST_09           = nn::camera::CONTRAST_PATTERN_09,  /**<  コントラストのパターンNo.9    */
  CONTRAST_10           = nn::camera::CONTRAST_PATTERN_10,  /**<  コントラストのパターンNo.10   */
  CONTRAST_11           = nn::camera::CONTRAST_PATTERN_11,  /**<  コントラストのパターンNo.11   */
  CONTRAST_LOW          = nn::camera::CONTRAST_LOW,         /**<  デフォルトよりもコントラスト比が低くなる設定    */
  CONTRAST_NORMAL       = nn::camera::CONTRAST_NORMAL,      /**<  デフォルトの設定    */
  CONTRAST_HIGH         = nn::camera::CONTRAST_HIGH,        /**<  デフォルトよりもコントラスト比が高くなる設定    */
};

/** レンズコレクション  */
enum  LensCorrectionType  {
  LC_OFF     = nn::camera::LENS_CORRECTION_OFF,       /**<  レンズ補正をOFFにする */
  LC_ON_70   = nn::camera::LENS_CORRECTION_ON_70,     /**<  画像の中心と周辺の明るさの比率が70になるように周辺の明るさを調整する設定  */
  LC_ON_90   = nn::camera::LENS_CORRECTION_ON_90,     /**<  画像の中心と周辺の明るさの比率が90になるように周辺の明るさを調整する設定  */
  LC_DARK    = nn::camera::LENS_CORRECTION_DARK,      /**<  デフォルト設定よりも画像の周辺が暗くなる設定  */
  LC_NORMAL  = nn::camera::LENS_CORRECTION_NORMAL,    /**<  デフォルト設定  */
  LC_BRIGHT  = nn::camera::LENS_CORRECTION_BRIGHT,    /**<  デフォルト設定よりも画像の周辺が明るくなる設定  */
};


/**
  解像度
  @note
    出力サイズとクロップ矩形を設定できます。
    カメラモジュールは最大640*480で撮影でき、そこからどれだけの領域を出力するのかを設定します。
    1) クロップ矩形を最大とし、出力サイズを256*192とした場合、640*480で撮影された画像が256*192に縮小されて出力されます。
    2) クロップ矩形及び出力サイズをともに256*192とした場合、640*480の画像から256*192を切り出すことになります。
    1)、2)共に出力サイズは256*192ですが、2)は1)からズームしたような見た目となります。
*/
struct  Resolution  {
  static const s16  maxW = 640;
  static const s16  maxH = 480;

  s16   w, h;       /**<  出力画像サイズ  */
  s16   crop[4];    /**<  撮影画像640*480中、出力画像として出力する領域（left, top, right, bottom）※w,hよりも大きい領域であること  */

  Resolution(void) : w(640), h(480) {
    crop[0] = 0;    crop[1] = 0;
    crop[2] = w-1;  crop[3] = h-1;
  }

  /**
    出力サイズを設定し、クロップ矩形を出力サイズに合わせる
  */
  void  Set(const s16 w_, const s16 h_) {
    w = w_;         h = h_;
    crop[0] = 0;    crop[1] = 0;
    crop[2] = w-1;  crop[3] = h-1;
  }

  /** アスペクト比を固定して出力サイズ幅を設定  */
  void  SetByW(const s16 w_)  {
    this->Set(w_, w_ * maxH / maxW);
  }

  /** アスペクト比を固定して出力サイズ高を設定  */
  void  SetByH(const s16 h_)  {
    this->Set(h_ * maxW / maxH, h_);
  }

  /** クロップ矩形を中央に設定  */
  void  SetCropCenter(const s16 w_, const s16 h_) {
    crop[0] = (maxW - w_) / 2;
    crop[1] = (maxH - h_) / 2;
    crop[2] = crop[0] + w_ - 1;
    crop[3] = crop[1] + h_ - 1;
  }
};


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
struct  PhotoMode {
  PhotoModeType   type;
  
  PhotoMode(void) : type(MODE_NORMAL)  {}
};


/**
  トリミング設定
  @note
  設定できる値には以下の制限があります。
    ・トリミング範囲は (xStart, yStart) から (xEnd-1, yEnd-1) までとなる。
    ・xEnd - xStart, 及び yEnd - yStart の値が偶数になる必要がある。
    ・xStart および yStart も偶数である必要がある。
*/
struct  Trimming  {
  bool    isEnable;
  s16     w, h;

  Trimming(void) : isEnable(false), w(640), h(480) {}
};


/** 自動露光  */
struct  Exposure  {
  bool  isEnable;
  s8    value;         /**<  [-5, +5]  */
  u8    rectUnit[4];   /**<  自動露出の計算基準となる領域（x,y,w,h : x40, y30[px]単位）※16x16[unit](640x480[px])の領域内であること  */
  Exposure(void) : isEnable(true), value(0) {
    rectUnit[0] = 0;    rectUnit[1] = 0;
    rectUnit[2] = 16;   rectUnit[3] = 16;
  }
};


/** ホワイトバランス  */
struct  WhiteBalance  {
  WhiteBalanceType  type;
  u8                rectUnit[4];   /**<  自動WBの計算基準となる領域（x,y,w,h : x40, y30[px]単位）※16x16[unit](640x480[px])の領域内であること  */
  
  WhiteBalance(void) : type(WB_AUTO) {
    rectUnit[0] = 0;    rectUnit[1] = 0;
    rectUnit[2] = 16;   rectUnit[3] = 16;
  }
};


/** シャープネス  */
struct  Sharpness {
  s8    value;          /**<  [-4, +5]  */
  
  Sharpness(void) : value(0) {}
};


/** 反転  */
struct  Flip  {
  FlipType  type;
  
  Flip(void) : type(FLIP_NONE)  {}
};


/** フレームレート  */
struct  FrameRate {
  FrameRateType   type;

  FrameRate(void) : type(FPS_15)  {}
};


/** エフェクト  */
struct  Effect  {
  EffectType      type;
  
  Effect(void) : type(EFFECT_NONE) {}
};


/** コントラスト  */
struct  Contrast  {
  ContrastType    type;
  
  Contrast(void) : type(CONTRAST_NORMAL)  {}
};


/** レンズコレクション  */
struct  LensCorrection  {
  LensCorrectionType  type;
  
  LensCorrection(void)  : type(LC_OFF)  {}
};


/** ノイズフィルタ  */
struct  NoiseFilter {
  bool    isEnable;
  
  NoiseFilter(void) : isEnable(true)  {}
};


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
inline nn::camera::Port  camSel2Port(const nn::camera::CameraSelect camSel)  {
  nn::camera::Port  port  = nn::camera::PORT_NONE;

  switch(camSel)  {
    case  nn::camera::SELECT_OUT1:             /*  外側カメラ(R)を指定                 */
    case  nn::camera::SELECT_IN1:              /*  内側カメラを指定                    */
    case  nn::camera::SELECT_IN1_OUT1:         /*  内側カメラと外側カメラ(R)を指定     */
      port  = nn::camera::PORT_CAM1;
      break;

    case  nn::camera::SELECT_OUT2:             /*  外側カメラ(L)を指定                 */
      port  = nn::camera::PORT_CAM2;
      break;

    case  nn::camera::SELECT_OUT1_OUT2:        /*  外側カメラ(R)と外側カメラ(L)を指定  */
    case  nn::camera::SELECT_IN1_OUT2:         /*  内側カメラと外側カメラ(L)を指定     */
    case  nn::camera::SELECT_ALL:              /*  全てのカメラを指定                  */
      port  = nn::camera::PORT_BOTH;
      break;

    case  nn::camera::SELECT_NONE:             /*  カメラ指定なし                      */
    default:
      break;
  }

  return port;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
inline nn::camera::Port   camSel2Port(const CameraType camType)   {return camSel2Port((nn::camera::CameraSelect)camType);}


}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */
#endif  /*  #if !defined(GFL2_DEVICECAMERAPARAM_H_INCLUDED)  */



