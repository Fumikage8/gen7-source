//=============================================================================
/**
 * @file   gfl2_UI_VectorDevice.cpp
 * @brief  方向入力デバイスのアクセッサ
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#include <macro/include/gfl2_Macros.h>
#include "ui/include/gfl2_UI_VectorDevice.h"
#include "./gfl2_UI_RepeatTimer.h"
#include "./gfl2_UI_DeviceImplementerForVectorDevice.h"


namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param heap         使用するヒープ
     * @param implementer  実際に入力の読み取りを行うインスタンス
     */
    //-------------------------------------------------------------------------
    VectorDevice::VectorDevice( 
        gfl2::heap::HeapBase* heap, DeviceImplementerForVectorDevice* implementer ) :
      Device( heap, implementer ),
      m_pImplementer( implementer )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    VectorDevice::~VectorDevice()
    {
    }




    //--------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを取得する
     */
    //--------------------------------------------------------------------------
    VectorDevice::StickClampMode VectorDevice::GetStickClampMode( void ) const
    {
      return m_pImplementer->GetStickClampMode();
    }

    //--------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを設定する
     * @param clamp_mode 設定するクランプモード
     */
    //--------------------------------------------------------------------------
    void VectorDevice::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
      m_pImplementer->SetStickClampMode( clamp_mode );
    }

    //--------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを取得する
     *
     * @param[out] min  最小クランプ値の格納先
     * @param[out] max  最大クランプ値の格納先
     */
    //--------------------------------------------------------------------------
    void VectorDevice::GetStickClampParam( s16* min, s16* max ) const
    {
      m_pImplementer->GetStickClampParam( min, max );
    }

    //--------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを設定する
     *
     * @param[in] min  最小クランプ値
     * @param[in] max  最大クランプ値
     */
    //--------------------------------------------------------------------------
    void VectorDevice::SetStickClampParam( s16 min, s16 max )
    {
      m_pImplementer->SetStickClampParam( min, max );
    }





    //-------------------------------------------------------------------------
    /**
     * @brief 入力された方向を取得する
     *
     * @return 入力方向のx成分[-1, 1]
     */
    //-------------------------------------------------------------------------
    f32 VectorDevice::GetX( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.x;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力された方向を取得する
     *
     * @return 入力方向のy成分[-1, 1]
     */
    //-------------------------------------------------------------------------
    f32 VectorDevice::GetY( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.y;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 入力された方向を取得する
     *
     * @param dest_x  取得したx成分[-1, 1]の格納先
     * @param dest_y  取得したy成分[-1, 1]の格納先
     */
    //-------------------------------------------------------------------------
    void VectorDevice::GetVector( f32* dest_x, f32* dest_y ) const
    {
      *dest_x = this->GetX();
      *dest_y = this->GetY();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 入力された方向を取得する
     *
     * @param dest_vec  取得した入力方向ベクトルの格納先
     */
    //-------------------------------------------------------------------------
    void VectorDevice::GetVector( gfl2::math::VEC2* dest_vec ) const
    {
      dest_vec->x = this->GetX();
      dest_vec->y = this->GetY();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力された方向を取得する
     *
     * @param dest_vec  取得した入力方向ベクトルの格納先
     */
    //-------------------------------------------------------------------------
    void VectorDevice::GetVector( gfl2::math::VEC3* dest_vec ) const
    {
      dest_vec->x = this->GetX();
      dest_vec->y = this->GetY();
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 指定した方向が入力されているかどうかを調べる
     *
     * @param dir  判定する向き( gfl2::ui::DIR_xxxx )
     *
     * @retval true   入力されている
     * @retval false  入力されていない
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsHold( u32 dir ) const
    {
      return ( this->GetHold() & dir ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 指定した方向が入力されたかどうかを調べる
     *
     * @param dir  判定する向き( gfl2::ui::DIR_xxxx )
     *
     * @retval true   入力された
     * @retval false  入力されていない
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsTrigger( u32 dir ) const
    {
      return ( this->GetTrigger() & dir ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 指定した方向の入力が無くなったかどうかを調べる
     *
     * @param dir  判定する向き( gfl2::ui::DIR_xxxx )
     *
     * @retval true   入力が無くなった
     * @retval false  入力が無くなっていない
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsRelease( u32 dir ) const
    {
      return ( this->GetRelease() & dir ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 指定した方向のリピートを検出したかどうかを調べる
     *
     * @param dir  判定する向き( gfl2::ui::DIR_xxxx )
     *
     * @retval true   リピートを検出した
     * @retval false  入リピートを検出していない
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsRepeat( u32 dir ) const
    {
      return ( this->GetRepeat() & dir ) != 0;
    } 




    //-------------------------------------------------------------------------
    /**
     * @brief 押されている方向の情報を取得する
     * @return 押されている全ての方向( gfl2::ui::DIR_xxxx )の論理和
     */
    //-------------------------------------------------------------------------
    u32 VectorDevice::GetHold( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.hold;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 押された方向の情報を取得する
     * @return 押された全ての方向( gfl2::ui::DIR_xxxx )の論理和
     */
    //-------------------------------------------------------------------------
    u32 VectorDevice::GetTrigger( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.trigger;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 離されたか方向の情報を取得する
     * @return 離された全ての方向( gfl2::ui::DIR_xxxx )の論理和
     */
    //-------------------------------------------------------------------------
    u32 VectorDevice::GetRelease( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.release;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リピートを検出した方向の情報を取得する
     * @return リピートを検出した全ての方向( gfl2::ui::DIR_xxxx )の論理和
     */
    //-------------------------------------------------------------------------
    u32 VectorDevice::GetRepeat( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return ( data.trigger | data.repeat );  // リピートはトリガを含む
    }

    /**
     * @brief 実際に参照すべき実行データを取得する
     */
    const VectorDevice::EffectiveData& VectorDevice::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 最新の検出データから入力方向のx成分[-1, 1]を取得する
     */
    //-------------------------------------------------------------------------
    f32 VectorDevice::GetLatestX( void ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      return now_detect->x;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 最新の検出データから入力方向のy成分[-1, 1] を取得する
     */
    //-------------------------------------------------------------------------
    f32 VectorDevice::GetLatestY( void ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      return now_detect->y;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 最新の検出データについて, 指定した方向が入力されているかどうかを調べる
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsHoldLatest( u32 dir ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      u32 now_hold = this->GetHoldState( now_detect->x, now_detect->y );
      return ( now_hold & dir ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 最新の検出データについて, 指定した方向が入力されたかどうかを調べる
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsTriggerLatest( u32 dir ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      const DetectiveData* prev_detect = reinterpret_cast<DetectiveData*>( m_prevDetectiveData ); 
      u32 now_hold = this->GetHoldState( now_detect->x, now_detect->y );
      u32 prev_hold = this->GetHoldState( prev_detect->x, prev_detect->y );
      return ( ( prev_hold & dir ) == 0 ) && ( ( now_hold & dir ) != 0 );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 最新の検出データについて, 指定した方向の入力が無くなったかどうかを調べる
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsReleaseLatest( u32 dir ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      const DetectiveData* prev_detect = reinterpret_cast<DetectiveData*>( m_prevDetectiveData ); 
      u32 now_hold = this->GetHoldState( now_detect->x, now_detect->y );
      u32 prev_hold = this->GetHoldState( prev_detect->x, prev_detect->y );
      return ( ( prev_hold & dir ) != 0 ) && ( ( now_hold & dir ) == 0 );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 最新の検出データについて, 指定した方向のリピートを検出したかどうかを調べる
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsRepeatLatest( u32 dir ) const
    {
      if( !m_pRepeatTimer->IsRepeat() ) {
        return false;
      }
      return this->IsHoldLatest( dir );
    } 




    //-------------------------------------------------------------------------
    /**
     * @brief 現在の検出データと前回の検出データを比較し, 同じデータが続いているかどうかを調べる
     * @retval true   前回の検出データと今回の検出データが同じ
     * @retval false  前回の検出データと今回の検出データが異なる
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsRepeatingSameInput( void ) const
    {
      const DetectiveData* now_data  = reinterpret_cast<DetectiveData*>(m_nowDetectiveData);
      const DetectiveData* prev_data = reinterpret_cast<DetectiveData*>(m_prevDetectiveData);

      if( ( ( now_data->x != 0 ) || ( now_data->y != 0 ) ) &&
          ( now_data->x == prev_data->x ) &&
          ( now_data->y == prev_data->y ) )
      {
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データを蓄積する
     * @param[out] buffer               データの格納先
     * @param[in]  detective_data       今回の検出データ
     * @param[in]  prev_detective_data  前回の検出データ
     */
    //-------------------------------------------------------------------------
    void VectorDevice::StoreEffectiveData( 
        void* buffer,
        const void* detective_data,
        const void* prev_detective_data ) const
    {
      const DetectiveData* now_data = reinterpret_cast<const DetectiveData*>( detective_data ); 
      const DetectiveData* prev_data = reinterpret_cast<const DetectiveData*>( prev_detective_data ); 

      EffectiveData* buffer_data = reinterpret_cast<EffectiveData*>( buffer );
      buffer_data->x = now_data->x;
      buffer_data->y = now_data->y;

      const u32 now_hold = this->GetHoldState( now_data->x, now_data->y );
      const u32 prev_hold = this->GetHoldState( prev_data->x, prev_data->y );
      this->MergeDetectData( buffer_data, now_hold, prev_hold, DIR_UP );
      this->MergeDetectData( buffer_data, now_hold, prev_hold, DIR_DOWN );
      this->MergeDetectData( buffer_data, now_hold, prev_hold, DIR_LEFT );
      this->MergeDetectData( buffer_data, now_hold, prev_hold, DIR_RIGHT );
    }

    /**
     * @brief 入力データから, 入力されている方向の情報を取得する
     * @param x x方向入力
     * @param y y方向入力
     * @return 指定した入力で「押されている」方向の論理和
     */
    u32 VectorDevice::GetHoldState( f32 x, f32 y ) const
    {
      u32 hold = 0;
      hold |= ( 0 < y ) ? DIR_UP : 0;
      hold |= ( y < 0 ) ? DIR_DOWN : 0;
      hold |= ( x < 0 ) ? DIR_LEFT : 0;
      hold |= ( 0 < x ) ? DIR_RIGHT : 0;
      return hold;
    }

    /**
     * @brief 検出データをマージし, 実効データを生成する
     * @param[out] buffer     生成した実効データの格納先
     * @param[in]  now_hold   今回のホールド状態
     * @param[in]  prev_hold  前回のホールド状態
     * @param[in]  dir        解決対象の方向
     */
    void VectorDevice::MergeDetectData( EffectiveData* buffer, u32 now_hold, u32 prev_hold, Direction dir ) const
    {
      bool hold    = ( (               now_hold & dir ) != 0 );
      bool release = ( (  prev_hold & ~now_hold & dir ) != 0 );
      bool trigger = ( ( ~prev_hold &  now_hold & dir ) != 0 );
      bool repeat  = m_pRepeatTimer->IsRepeat() && hold;

      if( release )
      {
        if( buffer->trigger & dir )
        {
          // 前フレームのトリガを無効化
          buffer->hold    &= ~dir;
          buffer->trigger &= ~dir;
          buffer->release &= ~dir;
          buffer->repeat  &= ~dir;
        }
        else
        {
          // リリースされた
          buffer->hold    &= ~dir;
          buffer->trigger &= ~dir;
          buffer->release |=  dir;
          buffer->repeat  &= ~dir;
        }
      }
      else if( ( buffer->release & dir ) && trigger )
      {
        // 前フレームのリリースを無効化
        buffer->hold    |=  dir;
        buffer->trigger &= ~dir;
        buffer->release &= ~dir;

        if( repeat ) {
          buffer->repeat |= dir;
        }
        else {
          buffer->repeat &= ~dir;
        }
      }
      else
      {
        buffer->hold    |= ( hold ) ? ( dir ) : ( 0 );
        buffer->trigger |= ( trigger ) ? ( dir ) : ( 0 );
        buffer->release |= ( release ) ? ( dir ) : ( 0 );
        buffer->repeat  |= ( repeat ) ? ( dir ) : ( 0 );
      }
    }


  } //namespace ui
} //namespace gfl2
