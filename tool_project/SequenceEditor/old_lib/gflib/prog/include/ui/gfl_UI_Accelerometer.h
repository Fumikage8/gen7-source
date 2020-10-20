//=============================================================================
/**
 * @brief  加速度センサのインターフェース
 * @file   gfl_UI_Accelerometer.h
 * @author obata_toshihiro
 * @date   2011.03.24
 */
//=============================================================================
#ifndef __GFL_UI_ACCELEROMETER_H__
#define __GFL_UI_ACCELEROMETER_H__
#pragma once

#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <ui/gfl_UI_Types.h>
#include <ui/gfl_UI_Device.h>

namespace gfl {
  namespace ui {

    class DeviceImplementerForAccelerometer;


    class Accelerometer : public Device
    {
      GFL_FORBID_COPY_AND_ASSIGN( Accelerometer );
      friend class DeviceManager;

      public:


      // 検出データ
      struct DetectiveData 
      {
        // 有効なデータかどうか
        bool is_valid;

        // 計測値
        f32 measured_value_x;
        f32 measured_value_y;
        f32 measured_value_z;

        // 加速度
        f32 acceleration_x;
        f32 acceleration_y;
        f32 acceleration_z;
      }; 


      // 実効データ
      struct EffectiveData 
      {
        // 有効なデータかどうか
        bool is_valid;

        // 計測値
        f32 measured_value_x;
        f32 measured_value_y;
        f32 measured_value_z;

        // 加速度
        f32 acceleration_x;
        f32 acceleration_y;
        f32 acceleration_z;
      };


      static const s16 MAX_MESURED_VALUE; // 測定値の絶対値
      static const s16 MAX_PLAY_RANGE;    // 測定値の「遊び」幅の最大値
      static const s16 MAX_SENSITIVITY;   // 検出感度の最大値


      //-----------------------------------------------------------------------
      /**
       * @brief 有効なデータを持っているかどうかを調べる
       *
       * @retval true   有効なデータを持っている
       * @retval false  有効なデータを持っていない
       */
      //-----------------------------------------------------------------------
      bool IsValid( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 計測値を取得する
       *
       * @param[out] buffer  取得した値の格納先
       *
       * @note 各軸の向き
       * x軸+  十字ボタンの左方向
       * y軸+  タッチパネルの実装面方向
       * z軸+  十字ボタンの上方向
       */
      //-----------------------------------------------------------------------
      void GetMeasuredValue( gfl::math::VEC3* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 加速度を取得する
       *
       * @param[out] buffer  取得した値の格納先
       */
      //-----------------------------------------------------------------------
      void GetAcceleration( gfl::math::VEC3* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値の「遊び」幅を設定する
       *
       * @param range  遊び幅[0, MAX_PLAY_RANGE]
       */
      //-----------------------------------------------------------------------
      void SetPlayRange( s16 range );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値の「遊び」幅を取得する
       *
       * @retval [0, MAX_PLAY_RANGE]
       */
      //-----------------------------------------------------------------------
      s16 GetPlayRange( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 検出感度を設定する
       *
       * @param sensitivity  検出感度[0, MAX_SENSITIVITY]
       */
      //-----------------------------------------------------------------------
      void SetSensitivity( s16 sensitivity );

      //-----------------------------------------------------------------------
      /**
       * @brief 検出感度を取得する
       *
       * @retval [0, MAX_SENSITIVITY]
       */
      //-----------------------------------------------------------------------
      s16 GetSensitivity( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値へのオフセットを有効にする
       */
      //-----------------------------------------------------------------------
      void EnableOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値へのオフセットを無効にする
       */
      //-----------------------------------------------------------------------
      void DisableOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値へのオフセットが有効かどうかを調べる
       *
       * @retval true   オフセットが有効
       * @retval false  オフセットが無効
       */
      //-----------------------------------------------------------------------
      bool IsOffsetEnable( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 現在のオフセット値を取得する
       *
       * @param[out] buffer  取得したオフセット値を格納するバッファ
       */
      //-----------------------------------------------------------------------
      void GetOffset( gfl::math::VEC3* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 測定値へのオフセット値を直接指定する
       *
       * @param offset  測定値に加算するオフセット値
       *
       * @note 測定値に単純に加算されます
       */
      //-----------------------------------------------------------------------
      void SetOffset( const gfl::math::VEC3& offset );


      //-----------------------------------------------------------------------
      /**
       * @brief オフセット値をリセットする( オフセットが無効な値にする )
       */
      //-----------------------------------------------------------------------
      void ResetOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 軸回転を有効にする
       */
      //-----------------------------------------------------------------------
      void EnableAxisRotation( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 軸回転を無効にする
       */
      //-----------------------------------------------------------------------
      void DisableAxisRotation( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 軸回転が有効かどうかを調べる
       *
       * @retval true  軸回転が有効
       * @retval false 軸回転が無効
       */
      //-----------------------------------------------------------------------
      bool IsAxisRotationEnable( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 軸回転に使用している回転行列を取得する
       *
       * @param buffer  取得した行列の格納先
       */
      //-----------------------------------------------------------------------
      void GetAxisRotationMatrix( gfl::math::MTX34* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 軸回転に使用する回転行列を設定する
       *
       * @param matrix  軸回転に使用する回転行列
       */
      //-----------------------------------------------------------------------
      void SetAxisRotationMatrix( const gfl::math::MTX34& matrix );

      //-----------------------------------------------------------------------
      /**
       * @brief 軸回転に使用する回転行列をリセットする( 軸回転しない状態にする )
       */
      //-----------------------------------------------------------------------
      void ResetAxisRotationMatrix( void );



      protected:

      /**
       * @brief コンストラクタ
       *
       * @param heap         使用するヒープ
       * @param implementer  実際に入力の読み取りを行うオブジェクト
       */
      Accelerometer( gfl::heap::HeapBase* heap, DeviceImplementerForAccelerometer* implementer );

      /**
       * @brief デストラクタ
       */
      virtual ~Accelerometer(); 

      /**
       * @brief 実効データを蓄積する
       *
       * @param buffer               データの格納先
       * @param detective_data       今回の検出データ
       * @param prev_detective_data  前回の検出データ
       */
      virtual void StoreEffectiveData( void* buffer, const void* detective_data, const void* prev_detective_data ) const;



      private:
      DeviceImplementerForAccelerometer* m_imple_for_accelerometer;

      const EffectiveData& GetEffectiveData( void ) const;
    }; 


  }  //namespace ui
}  //namespace gfl 


#endif  //__GFL_UI_ACCELEROMETER_H__
