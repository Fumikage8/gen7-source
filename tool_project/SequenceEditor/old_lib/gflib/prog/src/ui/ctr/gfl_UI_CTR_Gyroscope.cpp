//=============================================================================
/**
 * @file   gfl_UI_CTR_Gyroscope.cpp
 * @brief  3DSのジャイロセンサ入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.03.31
 */
//=============================================================================
#ifdef GFL_UI_USE_GYROSCOPE

#include "gfl_Math.h"
#include "ui/gfl_UI_Gyroscope.h"
#include "./gfl_UI_CTR_Gyroscope.h"
#include "../gfl_UI_DeviceImplementerForGyroscope.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param reader  入力の取得に使用するインスタンス
     * @param status  取得した入力を格納するバッファ
     */
    //-------------------------------------------------------------------------
    CTR_Gyroscope::CTR_Gyroscope( 
        nn::hid::GyroscopeReader* reader,
        nn::hid::GyroscopeStatus* status ) :
      DeviceImplementerForGyroscope(),
      m_pReader( reader ),
      m_pStatus( status ),
      m_angleMagnification( 1.0f, 1.0f, 1.0f ),
      m_dirMagnification( 1.0f )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CTR_Gyroscope::~CTR_Gyroscope()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データを取得する
     *
     * @param buffer  取得したデータの格納先 
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::GetDetectiveData( void* buffer ) const
    {
      bool valid = m_pReader->ReadLatest( m_pStatus );
      Gyroscope::DetectiveData* data = reinterpret_cast<Gyroscope::DetectiveData*>( buffer );
      data->is_valid = valid;
      data->speed = m_pStatus->speed;
      data->angle = m_pStatus->angle;
      data->direction_x = m_pStatus->direction.x;
      data->direction_y = m_pStatus->direction.y;
      data->direction_z = m_pStatus->direction.z;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::Main( void )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度を計算する際に掛ける倍率を取得する
     *
     * @retval[x] ピッチ方向の角速度計算の倍率
     * @retval[y] ヨー方向の角速度計算の倍率
     * @retval[z] ロール方向の角速度計算の倍率
     */
    //-------------------------------------------------------------------------
    const gfl::math::VEC3& CTR_Gyroscope::GetAngleMagnification( void ) const
    {
      return m_angleMagnification;
    }

    //--------------------------------------------------------------------------
    /**
     * @brief 
     */
    //--------------------------------------------------------------------------
    void CTR_Gyroscope::SetDirection( const gfl::math::MTX33& mtx )
    {
      m_pReader->SetDirection( mtx );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度を計算する際に掛ける倍率を設定する
     *
     * @retval[x] ピッチ方向の角速度計算の倍率
     * @retval[y] ヨー方向の角速度計算の倍率
     * @retval[z] ロール方向の角速度計算の倍率
     *
     * @note デフォルト値は(1.0, 1.0, 1.0)
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetAngleMagnification( const gfl::math::VEC3& value )
    {
      m_angleMagnification = value;
      m_pReader->SetAngleMagnification( value.x, value.y, value.z );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 姿勢ベクトルの計算に使用する角速度に適用する倍率を取得する
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetDirectionMagnification( void ) const
    {
      return m_dirMagnification;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 姿勢ベクトルの計算に使用する角速度に適用する倍率を設定する
     *
     * @note デフォルト値は1.0
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetDirectionMagnification( f32 value )
    {
      m_dirMagnification = value;
      m_pReader->SetDirectionMagnification( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正が有効かどうかを調べる
     *
     * @retval true   ゼロ点ドリフトの補正が有効
     * @retval false  ゼロ点ドリフトの補正が無効
     *
     * @note デフォルトは有効
     */
    //-------------------------------------------------------------------------
    bool CTR_Gyroscope::IsZeroDriftEnable( void ) const
    {
      return m_pReader->IsEnableZeroDrift();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正を有効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::EnableZeroDrift( void )
    {
      m_pReader->EnableZeroDrift();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正を無効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::DisableZeroDrift( void )
    {
      m_pReader->DisableZeroDrift();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正モードを取得する
     */
    //-------------------------------------------------------------------------
    Gyroscope::ZeroDriftMode CTR_Gyroscope::GetZeroDriftMode( void ) const
    {
      nn::hid::ZeroDriftMode mode;
      m_pReader->GetZeroDriftMode( mode );
      switch( mode ) {
      case nn::hid::GYROSCOPE_ZERODRIFT_LOOSE:    return Gyroscope::ZERO_DRIFT_MODE_LOOSE;
      case nn::hid::GYROSCOPE_ZERODRIFT_STANDARD: return Gyroscope::ZERO_DRIFT_MODE_STANDARD;
      case nn::hid::GYROSCOPE_ZERODRIFT_TIGHT:    return Gyroscope::ZERO_DRIFT_MODE_TIGHT;
      default:
                                                  GFL_ASSERT(0); //@check
                                                  break;
      }
      return Gyroscope::ZERO_DRIFT_MODE_STANDARD;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正モードを設定する
     *
     * @note デフォルトは ZERO_DRIFT_MODE_STANDARD
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetZeroDriftMode( Gyroscope::ZeroDriftMode next_mode )
    {
      nn::hid::ZeroDriftMode mode = nn::hid::GYROSCOPE_ZERODRIFT_STANDARD;
      switch( next_mode ) {
      case Gyroscope::ZERO_DRIFT_MODE_LOOSE:    mode = nn::hid::GYROSCOPE_ZERODRIFT_LOOSE;  break;
      case Gyroscope::ZERO_DRIFT_MODE_STANDARD: mode = nn::hid::GYROSCOPE_ZERODRIFT_STANDARD;  break;
      case Gyroscope::ZERO_DRIFT_MODE_TIGHT:    mode = nn::hid::GYROSCOPE_ZERODRIFT_TIGHT;  break;
      default:  
                                                GFL_ASSERT(0); //@check
                                                break;
      }
      m_pReader->SetZeroDriftMode( mode );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフト補正処理の働き具合を取得する
     *
     * @retval 負数  補正処理が無効の場合
     * @retval 正数  0.0は補正が行われなかった( 動きを検知した )ことを示す
     *               1.0に近づくほど安定している( 動きが少ない )ことを示す
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetZeroDriftEffect( void ) const
    {
      return m_pReader->GetZeroDriftEffect();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正が有効かどうかを調べる
     *
     * @retval true   ゼロ点の遊び補正が有効
     * @retval false  ゼロ点の遊び補正が無効
     *
     * @note デフォルトは無効
     */
    //-------------------------------------------------------------------------
    bool CTR_Gyroscope::IsZeroPlayParamEnable( void ) const
    {
      return m_pReader->IsEnableZeroPlay();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正を有効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::EnableZeroPlayParam( void )
    {
      m_pReader->EnableZeroPlay();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正を無効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::DisableZeroPlayParam( void )
    {
      m_pReader->DisableZeroPlay();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正の閾値を取得する
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetZeroPlayParam( void ) const
    {
      f32 value = 0.0f;
      m_pReader->GetZeroPlayParam( value );
      return value;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正の閾値を設定する
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetZeroPlayParam( f32 value )
    {
      m_pReader->SetZeroPlayParam( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正処理の働き具合を取得する
     *
     * @retval 負数  補正処理が無効の場合
     * @retval 正数  角速度が設定値に近くなるほど0.0に近づく
     *               0.0の場合は補正が行われなかったことを示す
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetZeroPlayEffect( void ) const
    {
      return m_pReader->GetZeroPlayEffect();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度の軸回転が有効かどうかを調べる
     *
     * @retval true   角速度への回転処理が有効
     * @retval false  角速度への回転処理が無効
     *
     * @note デフォルトは無効
     */
    //-------------------------------------------------------------------------
    bool CTR_Gyroscope::IsAxisRotationEnable( void ) const
    {
      return m_pReader->IsEnableAxisRotation();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度の軸回転を有効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::EnableAxisRotation( void )
    {
      m_pReader->EnableAxisRotation();
    }

    //-------------------------------------------------------------------------
    /*
     * @brief 角速度の軸回転を無効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::DisableAxisRotation( void )
    {
      m_pReader->DisableAxisRotation();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度の軸回転行列を取得する
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::GetAxisRotationMatrix( gfl::math::MTX34* buffer ) const
    {
      m_pReader->GetAxisRotationMatrix( buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度の軸回転行列を設定する
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetAxisRotationMatrix( const gfl::math::MTX34& matrix )
    {
      m_pReader->SetAxisRotationMatrix( matrix );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度の軸回転行列を初期化( 回転しない状態に )す;
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::ResetAxisRotationMatrix( void )
    {
      m_pReader->ResetAxisRotationMatrix();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 各軸が現在の姿勢ベクトルになるような軸回転行列を設定する
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetAxisRotationMatrixFromCurrentDirection( void )
    {
      gfl::math::MTX34 matrix;
      matrix.SetupIdentity();
      matrix._00 = m_pStatus->direction.x.x;
      matrix._10 = m_pStatus->direction.x.y;
      matrix._20 = m_pStatus->direction.x.z;
      matrix._01 = m_pStatus->direction.y.x;
      matrix._11 = m_pStatus->direction.y.y;
      matrix._21 = m_pStatus->direction.y.z;
      matrix._02 = m_pStatus->direction.z.x;
      matrix._12 = m_pStatus->direction.z.y;
      matrix._22 = m_pStatus->direction.z.z;
      this->SetAxisRotationMatrix( matrix ); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正が有効かどうかを調べる
     *
     * @retval true   加速度による三次元姿勢の補正が有効
     * @retval false  加速度による三次元姿勢の補正が無効
     *
     * @note デフォルトは有効
     */
    //-------------------------------------------------------------------------
    bool CTR_Gyroscope::IsRevisionByAccelerationEnable( void ) const
    {
      return m_pReader->IsEnableAccRevise();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正を有効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::EnableRevisionByAcceleration( void )
    {
      m_pReader->EnableAccRevise();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正を無効にする
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::DisableRevisionByAcceleration( void )
    {
      m_pReader->DisableAccRevise();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の重みを取得する
     *
     * @note デフォルト値は0.03
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetRevisionPowerByAcceleration( void ) const
    {
      f32 power = 0.0f;
      f32 range = 0.0f;
      m_pReader->GetAccReviseParam( power, range );
      return power;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の重みを設定する
     *
     * @note 値が大きいほど急激な補正が掛かります
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetRevisionPowerByAcceleration( f32 value )
    {
      f32 range = this->GetRevisionRangeByAcceleration();
      m_pReader->SetAccReviseParam( value, range );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の有効範囲を取得する
     *
     * @note デフォルト値は0.4
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetRevisionRangeByAcceleration( void ) const
    {
      f32 power = 0.0f;
      f32 range = 0.0f;
      m_pReader->GetAccReviseParam( power, range );
      return range;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の有効範囲を設定する
     *
     * @note
     * 例えば, 0.4 を指定した場合は 
     * 0.6G ～ 1.4G の範囲ならば補正計算に加速度を使用します.
     *
     * @note
     * 補正のパラメータはすべての方向に一様に適用されます.
     * 
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetRevisionRangeByAcceleration( f32 value )
    { 
      f32 power = this->GetRevisionPowerByAcceleration();
      m_pReader->SetAccReviseParam( power, value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の重みと有効範囲を初期化する
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::ResetRevisionParamByAcceleration( void )
    {
      m_pReader->ResetAccReviseParam();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正処理の働き具合を取得する
     *
     * @retval 0      補正が無効の場合
     * @retval 0以上  1.0に近づくほど, 
     *                三次元姿勢の方向を加速度の方向に近づくように補正したことを示す
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetRevisionEffectByAcceleration( void ) const
    {
      return m_pReader->GetAccReviseEffect();
    }


  } //namespace ui
} //namespace gfl

#endif  // GFL_UI_USE_GYROSCOPE
