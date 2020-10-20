//=============================================================================
/**
 * @brief  ジャイロセンサのインターフェース
 * @file   gfl2_UI_Gyroscope.cpp
 * @author obata_toshihiro
 * @date   2011.03.24
 */
//=============================================================================
#ifdef GFL2_UI_USE_GYROSCOPE

#include <heap/include/gfl2_Heap.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>
#include <ui/include/gfl2_UI_Types.h>
#include "ui/include/gfl2_UI_Device.h"
#include "ui/include/gfl2_UI_Gyroscope.h"
#include "./gfl2_UI_DeviceImplementerForGyroscope.h"


namespace gfl2
{
  namespace ui
  {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param heap         使用するヒープ
     * @param implementer  実際に入力の読み取りを行うオブジェクト
     */
    //-------------------------------------------------------------------------

    Gyroscope::Gyroscope( gfl2::heap::HeapBase* heap, DeviceImplementerForGyroscope* implementer ) :
    Device( heap, implementer ),
    m_pImplementer( implementer )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------

    Gyroscope::~Gyroscope( )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 有効なデータを持っているかどうかを調べる
     * @retval true   有効なデータを持っている
     * @retval false  有効なデータを持っていない
     */
    //-------------------------------------------------------------------------

    bool Gyroscope::IsValid( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.is_valid;
    }

    /**
     * @brief 実際に参照すべき実行データを取得する
     */
    const Gyroscope::EffectiveData& Gyroscope::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 角速度を取得する
     *
     * @retval[x] ピッチ方向の値
     * @retval[y] ヨー方向の値
     * @retval[z] ロール方向の値
     *
     * @note 各成分の値は, 360dpsを1.0とする値です.
     */
    //-------------------------------------------------------------------------

    const gfl2::math::VEC3& Gyroscope::GetSpeed( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.speed;
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

    const gfl2::math::VEC3& Gyroscope::GetAngleMagnification( void ) const
    {
      return m_pImplementer->GetAngleMagnification( );
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

    void Gyroscope::SetAngleMagnification( const gfl2::math::VEC3& value )
    {
      m_pImplementer->SetAngleMagnification( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度に対する軸回転が有効かどうかを調べる
     *
     * @retval true   角速度への回転処理が有効
     * @retval false  角速度への回転処理が無効
     *
     * @note デフォルトは無効
     */
    //-------------------------------------------------------------------------

    bool Gyroscope::IsAxisRotationEnable( void ) const
    {
      return m_pImplementer->IsAxisRotationEnable( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度に対する軸回転を有効にする
     */
    //-------------------------------------------------------------------------

    void Gyroscope::EnableAxisRotation( void )
    {
      m_pImplementer->EnableAxisRotation( );
    }

    //-------------------------------------------------------------------------
    /*
     * @brief 角速度に対する軸回転を無効にする
     */
    //-------------------------------------------------------------------------

    void Gyroscope::DisableAxisRotation( void )
    {
      m_pImplementer->DisableAxisRotation( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度に対する軸回転行列を取得する
     * @param[out] 回転行列の格納先
     */
    //-------------------------------------------------------------------------

    void Gyroscope::GetAxisRotationMatrix( gfl2::math::MTX34* buffer ) const
    {
      m_pImplementer->GetAxisRotationMatrix( buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度に対する軸回転行列を設定する
     * @param matrix 設定する回転行列
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetAxisRotationMatrix( const gfl2::math::MTX34& matrix )
    {
      m_pImplementer->SetAxisRotationMatrix( matrix );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 角速度に対する軸回転行列を初期化( 回転しない状態に )する
     */
    //-------------------------------------------------------------------------

    void Gyroscope::ResetAxisRotationMatrix( void )
    {
      m_pImplementer->ResetAxisRotationMatrix( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 各軸が現在の姿勢ベクトルになるような軸回転行列を設定する
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetAxisRotationMatrixFromCurrentDirection( void )
    {
      m_pImplementer->SetAxisRotationMatrixFromCurrentDirection( );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 回転角を取得する
     *
     * @retval[x] ピッチ方向の値
     * @retval[y] ヨー方向の値
     * @retval[z] ロール方向の値
     *
     * @note 各成分の値は, 360度を1.0とする値です.
     */
    //-------------------------------------------------------------------------

    const gfl2::math::VEC3& Gyroscope::GetAngle( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.angle;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 姿勢ベクトル( x軸 )を取得する
     */
    //-------------------------------------------------------------------------

    const gfl2::math::VEC3& Gyroscope::GetDirectionOfX( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.direction_x;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 姿勢ベクトル( y軸 )を取得する
     */
    //-------------------------------------------------------------------------

    const gfl2::math::VEC3& Gyroscope::GetDirectionOfY( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.direction_y;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 姿勢ベクトル( z軸 )を取得する
     */
    //-------------------------------------------------------------------------

    const gfl2::math::VEC3& Gyroscope::GetDirectionOfZ( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.direction_z;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief 姿勢ベクトルを設定する
     */
    //-----------------------------------------------------------------------

    void Gyroscope::SetDirection( const gfl2::math::MTX33& mtx )
    {
      m_pImplementer->SetDirection( mtx );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 姿勢ベクトルの計算に使用する角速度に適用する倍率を取得する
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetDirectionMagnification( void ) const
    {
      return m_pImplementer->GetDirectionMagnification( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 姿勢ベクトルの計算に使用する角速度に適用する倍率を設定する
     *
     * @note デフォルト値は1.0
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetDirectionMagnification( f32 value )
    {
      m_pImplementer->SetDirectionMagnification( value );
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

    bool Gyroscope::IsZeroDriftEnable( void ) const
    {
      return m_pImplementer->IsZeroDriftEnable( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正を有効にする
     */
    //-------------------------------------------------------------------------

    void Gyroscope::EnableZeroDrift( void )
    {
      m_pImplementer->EnableZeroDrift( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正を無効にする
     */
    //-------------------------------------------------------------------------

    void Gyroscope::DisableZeroDrift( void )
    {
      m_pImplementer->DisableZeroDrift( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正モードを取得する
     */
    //-------------------------------------------------------------------------

    Gyroscope::ZeroDriftMode Gyroscope::GetZeroDriftMode( void ) const
    {
      return m_pImplementer->GetZeroDriftMode( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点ドリフトの補正モードを設定する
     *
     * @note デフォルトは ZERO_DRIFT_MODE_STANDARD
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetZeroDriftMode( Gyroscope::ZeroDriftMode next_mode )
    {
      m_pImplementer->SetZeroDriftMode( next_mode );
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

    f32 Gyroscope::GetZeroDriftEffect( void ) const
    {
      return m_pImplementer->GetZeroDriftEffect( );
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

    bool Gyroscope::IsZeroPlayParamEnable( void ) const
    {
      return m_pImplementer->IsZeroPlayParamEnable( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正を有効にする
     */
    //-------------------------------------------------------------------------

    void Gyroscope::EnableZeroPlayParam( void )
    {
      m_pImplementer->EnableZeroPlayParam( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正を無効にする
     */
    //-------------------------------------------------------------------------

    void Gyroscope::DisableZeroPlayParam( void )
    {
      m_pImplementer->DisableZeroPlayParam( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正の閾値を取得する
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetZeroPlayParam( void ) const
    {
      return m_pImplementer->GetZeroPlayParam( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゼロ点の遊び補正の閾値を設定する
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetZeroPlayParam( f32 value )
    {
      m_pImplementer->SetZeroPlayParam( value );
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

    f32 Gyroscope::GetZeroPlayEffect( void ) const
    {
      return m_pImplementer->GetZeroPlayEffect( );
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

    bool Gyroscope::IsRevisionByAccelerationEnable( void ) const
    {
      return m_pImplementer->IsRevisionByAccelerationEnable( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正を有効にする
     */
    //-------------------------------------------------------------------------

    void Gyroscope::EnableRevisionByAcceleration( void )
    {
      m_pImplementer->EnableRevisionByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正を無効にする
     */
    //-------------------------------------------------------------------------

    void Gyroscope::DisableRevisionByAcceleration( void )
    {
      m_pImplementer->DisableRevisionByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の重みを取得する
     *
     * @note デフォルト値は0.03
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetRevisionPowerByAcceleration( void ) const
    {
      return m_pImplementer->GetRevisionPowerByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の重みを設定する
     *
     * @note 値が大きいほど急激な補正が掛かります
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetRevisionPowerByAcceleration( f32 value )
    {
      m_pImplementer->SetRevisionPowerByAcceleration( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の有効範囲を取得する
     *
     * @note デフォルト値は0.4
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetRevisionRangeByAcceleration( void ) const
    {
      return m_pImplementer->GetRevisionRangeByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の有効範囲を設定する
     *
     * @note
     * 例えば, 0.4 を指定した場合は 
     * 0.6G 〜 1.4G の範囲ならば補正計算に加速度を使用します.
     *
     * @note
     * 補正のパラメータはすべての方向に一様に適用されます.
     * 
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetRevisionRangeByAcceleration( f32 value )
    {
      m_pImplementer->SetRevisionRangeByAcceleration( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度による三次元姿勢の補正の重みと有効範囲を初期化する
     */
    //-------------------------------------------------------------------------

    void Gyroscope::ResetRevisionParamByAcceleration( void )
    {
      m_pImplementer->ResetRevisionParamByAcceleration( );
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

    f32 Gyroscope::GetRevisionEffectByAcceleration( void ) const
    {
      return m_pImplementer->GetRevisionEffectByAcceleration( );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 実効データを蓄積する
     * @param[out] buffer               データの格納先
     * @param[in]  detective_data       今回の検出データ
     * @param[in]  prev_detective_data  前回の検出データ
     */
    //-------------------------------------------------------------------------

    void Gyroscope::StoreEffectiveData(
                                        void* buffer,
                                        const void* detective_data,
                                        const void* prev_detective_data ) const
    {
      GFL_UNUSED( prev_detective_data );
      GFL_ASSERT( sizeof (EffectiveData ) == sizeof (DetectiveData ) ); //@check
      const DetectiveData* now_data = reinterpret_cast < const DetectiveData* >( detective_data );
      EffectiveData* effective_data = reinterpret_cast < EffectiveData* >( buffer );
      gfl2::std::MemCopy( now_data, effective_data, sizeof (EffectiveData ) );
    }




  } //namespace ui
} //namespace gfl2 

#endif  // GFL2_UI_USE_GYROSCOPE
