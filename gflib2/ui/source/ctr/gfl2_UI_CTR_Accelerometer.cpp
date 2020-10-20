//=============================================================================
/**
 * @file   gfl2_UI_CTR_Accelerometer.cpp
 * @brief  3DSの加速度センサの入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.03.25
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR
#include "./gfl2_UI_CTR_Accelerometer.h"
#include "ui/include/gfl2_UI_Accelerometer.h"
namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param reader  ボタン入力の読み取りを行うインスタンス
     * @param status  読み取ったボタン入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    CTR_Accelerometer::CTR_Accelerometer( 
        nn::hid::AccelerometerReader* reader, nn::hid::AccelerometerStatus* status ) :
      DeviceImplementerForAccelerometer(),
      m_pReader( reader ),
      m_pStatus( status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CTR_Accelerometer::~CTR_Accelerometer( void )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データを取得する
     *
     * @param[out] buffer  取得したデータの格納先 
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::GetDetectiveData( void* buffer ) const
    {
      bool valid = m_pReader->ReadLatest( m_pStatus );
      gfl2::ui::Accelerometer::DetectiveData* data = reinterpret_cast<gfl2::ui::Accelerometer::DetectiveData*>( buffer );
      data->is_valid = valid;
      data->measured_value_x = static_cast<f32>( m_pStatus->x );
      data->measured_value_y = static_cast<f32>( m_pStatus->y );
      data->measured_value_z = static_cast<f32>( m_pStatus->z );

      nn::hid::AccelerationFloat accel;
      m_pReader->ConvertToAcceleration( &accel, 1, m_pStatus );
      data->acceleration_x = accel.x;
      data->acceleration_y = accel.y;
      data->acceleration_z = accel.z;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値の「遊び」幅を設定する
     */
    //-------------------------------------------------------------------------
    s16 CTR_Accelerometer::GetPlayRange( void ) const
    {
      s16 play = 0;
      s16 sensitivity = 0;
      m_pReader->GetSensitivity( &play, &sensitivity );
      return play;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値の「遊び」幅を取得する
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::SetPlayRange( s16 value )
    {
      s16 sensitivity = this->GetSensitivity();
      s16 play = value;
      play = ( play < 0 ) ? 0 : play;
      play = ( Accelerometer::MAX_PLAY_RANGE < play ) ? Accelerometer::MAX_PLAY_RANGE : play;
      m_pReader->SetSensitivity( play, sensitivity );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 検出感度を設定する
     */
    //-------------------------------------------------------------------------
    s16 CTR_Accelerometer::GetSensitivity( void ) const
    {
      s16 play = 0;
      s16 sensitivity = 0;
      m_pReader->GetSensitivity( &play, &sensitivity );
      return sensitivity;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 検出感度を設定する
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::SetSensitivity( s16 value )
    {
      s16 play = this->GetPlayRange();
      s16 sensitivity = value;
      sensitivity = ( sensitivity < 0 ) ? 0 : sensitivity;
      sensitivity = ( Accelerometer::MAX_SENSITIVITY < sensitivity ) ? Accelerometer::MAX_SENSITIVITY : sensitivity;
      m_pReader->SetSensitivity( play, sensitivity );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値へのオフセットを有効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::EnableOffset( void )
    {
      m_pReader->EnableOffset();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値へのオフセットを無効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::DisableOffset( void )
    {
      m_pReader->DisableOffset();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 測定値へのオフセットが有効かどうかを調べる
     *
     * @retval true   オフセットが有効
     * @retval false  オフセットが無効
     */
    //-------------------------------------------------------------------------
    bool CTR_Accelerometer::IsOffsetEnable( void ) const
    {
      return m_pReader->IsEnableOffset();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 現在のオフセット値を取得する
     *
     * @param[out] buffer  取得したオフセット値を格納するバッファ
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::GetOffset( gfl2::math::VEC3* buffer ) const
    {
      s16 offset_x, offset_y, offset_z;
      m_pReader->GetOffset( &offset_x, &offset_y, &offset_z );

      buffer->x = static_cast<f32>( offset_x );
      buffer->y = static_cast<f32>( offset_y );
      buffer->z = static_cast<f32>( offset_z );
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
    void CTR_Accelerometer::SetOffset( const gfl2::math::VEC3& offset )
    {
      m_pReader->SetOffset( 
          static_cast<s16>( offset.x ), 
          static_cast<s16>( offset.y ), 
          static_cast<s16>( offset.z ) );
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief 指定した測定値のときに水平になるようなオフセット値を設定する
     *
     * @param level_value  水平とみなす測定値
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::ResetOffset( void )
    {
      m_pReader->ResetOffset();
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief 測定値に対する軸回転を有効にする
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::EnableAxisRotation( void )
    {
      m_pReader->EnableAxisRotation();
    } 

    //----------------------------------------------------------------------------------
    /**
     * @brief 測定値に対する軸回転を無効にする
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::DisableAxisRotation( void )
    {
      m_pReader->DisableAxisRotation();
    } 

    //----------------------------------------------------------------------------------
    /**
     * @brief 測定値に対する軸回転が有効かどうかを調べる
     *
     * @retval true  軸回転が有効
     * @retval false 軸回転が無効
     */
    //----------------------------------------------------------------------------------
    bool CTR_Accelerometer::IsAxisRotationEnable( void )
    {
      return m_pReader->IsEnableAxisRotation();
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief 測定値に対する軸回転として適用している回転行列を取得する
     *
     * @param buffer  取得した行列の格納先
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::GetAxisRotationMatrix( gfl2::math::MTX34* buffer ) const
    {
      m_pReader->GetAxisRotationMatrix( buffer );
    } 

    //----------------------------------------------------------------------------------
    /**
     * @brief 測定値に対する軸回転として適用する回転行列を設定する
     *
     * @param matrix  軸回転に使用する回転行列
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::SetAxisRotationMatrix( const gfl2::math::MTX34& matrix )
    { 
      m_pReader->SetAxisRotationMatrix( matrix );
    } 

    //----------------------------------------------------------------------------------
    /**
     * @brief 軸回転に使用する回転行列をリセットする( 軸回転しない状態にする )
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::ResetAxisRotationMatrix( void )
    {
      m_pReader->ResetAxisRotationMatrix();
    }


  } //namespace ui
} //namespace gfl2
#endif