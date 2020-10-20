//=============================================================================
/**
 * @brief  加速度センサのインターフェース
 * @file   gfl2_UI_Accelerometer.cpp
 * @author obata_toshihiro
 * @date   2011.03.24
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR
#include <nn.h>
#endif
#include "ui/include/gfl2_UI_Accelerometer.h"
#include <debug/include/gfl2_Assert.h>
#include "./gfl2_UI_DeviceImplementerForAccelerometer.h"
#include <util/include/gfl2_std_string.h>

namespace gfl2 {
  namespace ui {


#if defined(GF_PLATFORM_CTR)
	      // 測定値の絶対値
    const s16 Accelerometer::MAX_MESURED_VALUE = nn::hid::MAX_OF_ACCELEROMETER_VALUE;

    // 測定値の「遊び」幅の最大値
    const s16 Accelerometer::MAX_PLAY_RANGE = nn::hid::MAX_OF_ACCELEROMETER_PLAY;

    // 検出感度の最大値
    const s16 Accelerometer::MAX_SENSITIVITY = nn::hid::MAX_OF_ACCELEROMETER_SENSITIVITY;
#elif defined(GF_PLATFORM_WIN32)
	  	    // 測定値の絶対値
    const s16 Accelerometer::MAX_MESURED_VALUE = 0;

    // 測定値の「遊び」幅の最大値
    const s16 Accelerometer::MAX_PLAY_RANGE = 0;

    // 検出感度の最大値
    const s16 Accelerometer::MAX_SENSITIVITY = 0;
#endif


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param heap         使用するヒープ
     * @param implementer  実際に入力の読み取りを行うオブジェクト
     */
    //-------------------------------------------------------------------------
    Accelerometer::Accelerometer( 
        gfl2::heap::HeapBase* heap, DeviceImplementerForAccelerometer* implementer ) :
      Device( heap, implementer ),
      m_imple_for_accelerometer( implementer )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    Accelerometer::~Accelerometer()
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 有効なデータを持っているかどうかを調べる
     * @retval true   有効なデータを持っている
     * @retval false  有効なデータを持っていない
     */
    //-------------------------------------------------------------------------
    bool Accelerometer::IsValid( void ) const
    {
      const EffectiveData* data = reinterpret_cast<EffectiveData*>( m_effectiveData );
      return data->is_valid;
    } 

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void Accelerometer::GetMeasuredValue( gfl2::math::VEC3* buffer ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      buffer->x = data.measured_value_x;
      buffer->y = data.measured_value_y;
      buffer->z = data.measured_value_z;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 加速度を取得する
     *
     * @param[out] buffer  取得した値の格納先
     */
    //-------------------------------------------------------------------------
    void Accelerometer::GetAcceleration( gfl2::math::VEC3* buffer ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      buffer->x = data.acceleration_x;
      buffer->y = data.acceleration_y;
      buffer->z = data.acceleration_z;
    }

    /**
     * @brief 実際に参照すべき実行データを取得する
     */
    const Accelerometer::EffectiveData& Accelerometer::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値の「遊び」幅を設定する
     *
     * @param range  遊び幅[0, MAX_PLAY_RANGE]
     */
    //-------------------------------------------------------------------------
    void Accelerometer::SetPlayRange( s16 range )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->SetPlayRange( range );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値の「遊び」幅を取得する
     *
     * @retval [0, MAX_PLAY_RANGE]
     */
    //-------------------------------------------------------------------------
    s16 Accelerometer::GetPlayRange( void ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      s16 play_range = 0;
      if( m_imple_for_accelerometer ) {
        play_range = m_imple_for_accelerometer->GetPlayRange();
      }
      return play_range;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 検出感度を設定する
     *
     * @param sensitivity  検出感度[0, MAX_SENSITIVITY]
     */
    //-------------------------------------------------------------------------
    void Accelerometer::SetSensitivity( s16 sensitivity )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->SetSensitivity( sensitivity );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 検出感度を取得する
     *
     * @retval [0, MAX_SENSITIVITY]
     */
    //-------------------------------------------------------------------------
    s16 Accelerometer::GetSensitivity( void ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      s16 sensitivity = 0;
      if( m_imple_for_accelerometer ) {
        sensitivity = m_imple_for_accelerometer->GetSensitivity();
      }
      return sensitivity;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値へのオフセットを有効にする
     */
    //-------------------------------------------------------------------------
    void Accelerometer::EnableOffset( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->EnableOffset();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値へのオフセットを無効にする
     */
    //-------------------------------------------------------------------------
    void Accelerometer::DisableOffset( void )
    { 
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->DisableOffset();
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値へのオフセットが有効かどうかを調べる
     *
     * @retval true   オフセットが有効
     * @retval false  オフセットが無効
     */
    //-------------------------------------------------------------------------
    bool Accelerometer::IsOffsetEnable( void ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        return m_imple_for_accelerometer->IsOffsetEnable();
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 現在のオフセット値を取得する
     *
     * @param[out] buffer  取得したオフセット値を格納するバッファ
     */
    //-------------------------------------------------------------------------
    void Accelerometer::GetOffset( gfl2::math::VEC3* buffer ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->GetOffset( buffer );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値へのオフセット値を直接指定する
     *
     * @param offset  測定値に加算するオフセット値
     *
     * @note 測定値に単純に加算されます
     */
    //-------------------------------------------------------------------------
    void Accelerometer::SetOffset( const gfl2::math::VEC3& offset )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->SetOffset( offset );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief オフセット値をリセットする( オフセットが無効な値にする )
     */
    //-------------------------------------------------------------------------
    void Accelerometer::ResetOffset( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->ResetOffset();
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 軸回転を有効にする
     */
    //-------------------------------------------------------------------------
    void Accelerometer::EnableAxisRotation( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->EnableAxisRotation();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 軸回転を無効にする
     */
    //-------------------------------------------------------------------------
    void Accelerometer::DisableAxisRotation( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->DisableAxisRotation();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 軸回転が有効かどうかを調べる
     *
     * @retval true  軸回転が有効
     * @retval false 軸回転が無効
     */
    //-------------------------------------------------------------------------
    bool Accelerometer::IsAxisRotationEnable( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        return m_imple_for_accelerometer->IsAxisRotationEnable();
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 軸回転に使用している回転行列を取得する
     *
     * @param buffer  取得した行列の格納先
     */
    //-------------------------------------------------------------------------
    void Accelerometer::GetAxisRotationMatrix( gfl2::math::MTX34* buffer ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->GetAxisRotationMatrix( buffer );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 軸回転に使用する回転行列を設定する
     *
     * @param matrix  軸回転に使用する回転行列
     */
    //-------------------------------------------------------------------------
    void Accelerometer::SetAxisRotationMatrix( const gfl2::math::MTX34& matrix )
    { 
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->SetAxisRotationMatrix( matrix );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 軸回転に使用する回転行列をリセットする( 軸回転しない状態にする )
     */
    //-------------------------------------------------------------------------
    void Accelerometer::ResetAxisRotationMatrix( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->ResetAxisRotationMatrix();
      }
    }



    //-------------------------------------------------------------------------
    /**
     * @brief 実効データを蓄積する
     *
     * @param buffer               データの格納先
     * @param detective_data       今回の検出データ
     * @param prev_detective_data  前回の検出データ
     */
    //-------------------------------------------------------------------------
    void Accelerometer::StoreEffectiveData(
        void* buffer, 
        const void* detective_data,
        const void* prev_detective_data ) const
    {
      GFL_UNUSED( prev_detective_data );
      GFL_ASSERT( sizeof(EffectiveData) == sizeof(DetectiveData) ); //@check
      const DetectiveData* now_data = reinterpret_cast<const DetectiveData*>( detective_data ); 
      EffectiveData* effective_data = reinterpret_cast<EffectiveData*>( buffer );
      gfl2::std::MemCopy( now_data, effective_data, sizeof(EffectiveData) );
    }


  }  //namespace ui
}  //namespace gfl2
