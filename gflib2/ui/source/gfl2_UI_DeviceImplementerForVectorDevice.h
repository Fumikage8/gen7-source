//=============================================================================
/**
 * @file   gfl2_UI_DeviceImplementerForVectorDevice.h
 * @brief  ベクター型デバイスの入力を検出するクラスのインターフェース
 * @author obata_toshihiro
 * @date   2011.04.04
 */
//=============================================================================
#ifndef __gfl2_UI_DEVICE_IMPLEMENTER_FOR_VECTORDEVICE_H__
#define __gfl2_UI_DEVICE_IMPLEMENTER_FOR_VECTORDEVICE_H__
#include "ui/include/gfl2_UI_VectorDevice.h"
#include "./gfl2_UI_DeviceImplementer.h"

namespace gfl2 {
  namespace ui {


    class DeviceImplementerForVectorDevice : public DeviceImplementer
    { 
      GFL_FORBID_COPY_AND_ASSIGN( DeviceImplementerForVectorDevice );

      public:
      //-----------------------------------------------------------------------
      /**
       * @brief デフォルトコンストラクタ
       */
      //-----------------------------------------------------------------------
      DeviceImplementerForVectorDevice( void );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~DeviceImplementerForVectorDevice();

      //-----------------------------------------------------------------------
      /**
       * @brief 入力データのサイズを取得する
       */
      //-----------------------------------------------------------------------
      virtual u32 GetDetectiveDataSize( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 実効データのサイズを取得する
       */
      //-----------------------------------------------------------------------
      virtual u32 GetEffectiveDataSize( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプモードを取得する
       */
      //-----------------------------------------------------------------------
      virtual VectorDevice::StickClampMode GetStickClampMode( void ) const = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプモードを設定する
       * @param clamp_mode 設定するクランプモード
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampMode( VectorDevice::StickClampMode clamp_mode ) = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプパラメータを取得する
       *
       * @param[out] min  最小クランプ値の格納先
       * @param[out] max  最大クランプ値の格納先
       */
      //-----------------------------------------------------------------------
      virtual void GetStickClampParam( s16* min, s16* max ) const = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプパラメータを設定する
       *
       * @param[in] min  最小クランプ値
       * @param[in] max  最大クランプ値
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampParam( s16 min, s16 max ) = 0;
    };


  } //namespace ui
} //namespace gfl2

#endif //__gfl2_UI_DEVICE_IMPLEMENTER_FOR_VECTORDEVICE_H__
