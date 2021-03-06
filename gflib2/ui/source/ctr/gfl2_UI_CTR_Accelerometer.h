﻿//=============================================================================
/**
 * @file   gfl2_UI_CTR_Accelerometer.h
 * @brief  3DSの加速度センサの入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.03.25
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR



#ifndef __gfl2_UI_CTR_ACCELEROMETER_H__
#define __gfl2_UI_CTR_ACCELEROMETER_H__
#include <nn.h>
#include "macro/include/gfl2_Macros.h"
#include <math/include/gfl2_math.h>
#include "../gfl2_UI_DeviceImplementerForAccelerometer.h"

namespace gfl2 {
  namespace ui {


    class CTR_Accelerometer : public DeviceImplementerForAccelerometer
    {
      GFL_FORBID_COPY_AND_ASSIGN( CTR_Accelerometer );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       *
       * @param reader  ボタン入力の読み取りを行うインスタンス
       * @param status  読み取ったボタン入力を取得するバッファ
       */
      //-----------------------------------------------------------------------
      CTR_Accelerometer( nn::hid::AccelerometerReader* reader, nn::hid::AccelerometerStatus* status );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_Accelerometer( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 入力データを取得する
       *
       * @param[out] buffer  取得したデータの格納先 
       */
      //-----------------------------------------------------------------------
      virtual void GetDetectiveData( void* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値の「遊び」幅を設定する
       */
      //-----------------------------------------------------------------------
      virtual void SetPlayRange( s16 value );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値の「遊び」幅を取得する
       */
      //-----------------------------------------------------------------------
      virtual s16 GetPlayRange( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 検出感度を設定する
       */
      //-----------------------------------------------------------------------
      virtual void SetSensitivity( s16 value );

      //-----------------------------------------------------------------------
      /**
       * @brief 検出感度を取得する
       */
      //-----------------------------------------------------------------------
      virtual s16 GetSensitivity( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値へのオフセットを有効にする
       */
      //-----------------------------------------------------------------------
      virtual void EnableOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値へのオフセットを無効にする
       */
      //-----------------------------------------------------------------------
      virtual void DisableOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値へのオフセットが有効かどうかを調べる
       *
       * @retval true   オフセットが有効
       * @retval false  オフセットが無効
       */
      //-----------------------------------------------------------------------
      virtual bool IsOffsetEnable( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 現在のオフセット値を取得する
       *
       * @param[out] buffer  取得したオフセット値を格納するバッファ
       */
      //-----------------------------------------------------------------------
      virtual void GetOffset( gfl2::math::VEC3* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値へのオフセット値を直接指定する
       *
       * @param offset  測定値に加算するオフセット値
       *
       * @note 測定値に単純に加算されます
       */
      //-----------------------------------------------------------------------
      virtual void SetOffset( const gfl2::math::VEC3& offset );

      //-----------------------------------------------------------------------
      /**
       * @brief オフセット値をリセットする( オフセットが無効な値にする )
       */
      //-----------------------------------------------------------------------
      virtual void ResetOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値に対する軸回転を有効にする
       */
      //-----------------------------------------------------------------------
      virtual void EnableAxisRotation( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値に対する軸回転を無効にする
       */
      //-----------------------------------------------------------------------
      virtual void DisableAxisRotation( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値に対する軸回転が有効かどうかを調べる
       *
       * @retval true  軸回転が有効
       * @retval false 軸回転が無効
       */
      //-----------------------------------------------------------------------
      virtual bool IsAxisRotationEnable( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値に対する軸回転として適用している回転行列を取得する
       *
       * @param buffer  取得した行列の格納先
       */
      //-----------------------------------------------------------------------
      virtual void GetAxisRotationMatrix( gfl2::math::MTX34* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値に対する軸回転として適用する回転行列を設定する
       *
       * @param matrix  軸回転に使用する回転行列
       */
      //-----------------------------------------------------------------------
      virtual void SetAxisRotationMatrix( const gfl2::math::MTX34& matrix );

      //-----------------------------------------------------------------------
      /**
       * @brief 軸回転に使用する回転行列をリセットする( 軸回転しない状態にする )
       */
      //-----------------------------------------------------------------------
      virtual void ResetAxisRotationMatrix( void );


      private:
      nn::hid::AccelerometerReader* m_pReader;
      nn::hid::AccelerometerStatus* m_pStatus;
    };


  } //namespace ui
} //namespace gfl2

#endif  // __gfl2_UI_CTR_ACCELEROMETER_H__
#endif