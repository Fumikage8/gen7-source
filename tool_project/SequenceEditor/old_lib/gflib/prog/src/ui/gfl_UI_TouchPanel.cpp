//=============================================================================
/**
 * @file gfl_UI_TouchPanel.cpp
 * @brief タッチパネルのアクセッサ
 * @author toshihiro_obata
 * @date 2010.10.22
 */
//=============================================================================
#include <nn.h>
#include "gfl_Macros.h"
#include "gfl_Math.h"
#include "ui/gfl_UI_TouchPanel.h"
#include "./gfl_UI_DeviceImplementer.h"
#include "./gfl_UI_RepeatTimer.h"

namespace gfl {
  namespace ui {


    // ダブルクリック判定のデフォルト設定
    const TouchPanel::DoubleClickCond TouchPanel::DEFAULT_DCLICK_COND = 
    {
      DCLICK_RANGE_RECT,
      DEFAULT_DCLICK_RADIUS,
      DEFAULT_DCLICK_FRAME,
    };




    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap         使用するヒープ
     * @param implementer  実際にタッチパネル入力の読み取りを行うインスタンス
     */
    //-------------------------------------------------------------------------
    TouchPanel::TouchPanel( 
        gfl::heap::HeapBase* heap, DeviceImplementer* implementer ) :
      Device( heap, implementer ),
      m_dclick_cond( DEFAULT_DCLICK_COND ),
      m_dclick_state(),
      m_update_flag(true)
    {
      m_dclick_state.detected = false;
      m_dclick_state.first = false;
      m_dclick_state.first_x = 0;
      m_dclick_state.first_y = 0;
      m_dclick_state.count = 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    TouchPanel::~TouchPanel()
    {
    }




    //-------------------------------------------------------------------------
    /**
     * @brief タッチされているx座標を取得する
     * @retval 現在タッチされているx座標( タッチされていない場合は 0 )
     */
    //-------------------------------------------------------------------------
    u16 TouchPanel::GetX( void ) const 
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.last_x;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチされているy座標を取得する
     * @retval 現在タッチされているy座標( タッチされていない場合は 0 )
     */
    //-------------------------------------------------------------------------
    u16 TouchPanel::GetY( void ) const 
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.last_y;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチされている座標を取得する
     * @param[out] out_x  取得したx座標の格納先
     * @param[out] out_y  取得したy座標の格納先
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetXY( u16* out_x, u16* out_y ) const
    {
      *out_x = this->GetX();
      *out_y = this->GetY();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチされている座標を取得する
     * @param[out] vec  取得した座標の格納先
     *
     * @note vec->z は変更しません。
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetXY( gfl::math::VEC3* vec ) const
    {
      vec->x = this->GetX();
      vec->y = this->GetY();
    } 



    //-------------------------------------------------------------------------
    /**
     * @brief 前回タッチされたx座標を取得する
     * @retval 現在タッチされているx座標( タッチされていない場合は 0 )
     */
    //-------------------------------------------------------------------------
    u16 TouchPanel::GetPrevX( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.prev_x;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 前回タッチされたy座標を取得する
     * @retval 現在タッチされているy座標( タッチされていない場合は 0 )
     */
    //-------------------------------------------------------------------------
    u16 TouchPanel::GetPrevY( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.prev_y;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 前回タッチされた座標を取得する
     * @param[out] out_x  取得したx座標の格納先
     * @param[out] out_y  取得したy座標の格納先
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetPrevXY( u16* out_x, u16* out_y ) const
    {
      *out_x = this->GetPrevX();
      *out_y = this->GetPrevY();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 前回タッチされた座標を取得する
     * @param[out] vec  取得した座標の格納先
     *
     * @note vec->z は変更しません。
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetPrevXY( gfl::math::VEC3* vec ) const
    {
      vec->x = this->GetPrevX();
      vec->y = this->GetPrevY();
    }




    //-------------------------------------------------------------------------
    /**
     * @brief タッチ座標のx変化量を取得する
     * @return 前回からのx座標の変化量
     *
     * @note
     * タッチされていない間の入力は座標(0,0)として扱われています.
     * よって, タッチされた瞬間は(0,0)からの変化量(=現在値)が返ります.
     */
    //-------------------------------------------------------------------------
    int TouchPanel::GetMoveX( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.last_x - data.prev_x;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチ座標のy変化量を取得する
     * @return 前回からのy座標の変化量
     *
     * @note
     * タッチされていない間の入力は座標(0,0)として扱われています.
     * よって, タッチされた瞬間は(0,0)からの変化量(=現在値)が返ります.
     */
    //-------------------------------------------------------------------------
    int TouchPanel::GetMoveY( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.last_y - data.prev_y;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチ座標の変化量を取得する
     * @param[out] out_x  x方向の変化量の格納先
     * @param[out] out_y  y方向の変化量の格納先
     *
     * @note
     * タッチされていない間の入力は座標(0,0)として扱われています.
     * よって, タッチされた瞬間は(0,0)からの変化量(=現在値)が返ります.
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetMoveXY( int* out_x, int* out_y ) const
    {
      *out_x = this->GetMoveX();
      *out_y = this->GetMoveY();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチ座標の変化量を取得する
     * @param vec  取得した変化量の格納先
     *
     * @note
     * タッチされていない間の入力は座標(0,0)として扱われています.
     * よって, タッチされた瞬間は(0,0)からの変化量(=現在値)が返ります.
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetMoveXY( gfl::math::VEC3* vec ) const
    {
      vec->x = this->GetMoveX();
      vec->y = this->GetMoveY();
    }






    //-------------------------------------------------------------------------
    /**
     * @brief タッチされているかどうかを調べる
     * @retval true   タッチされている
     * @retval false  タッチされていない
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsTouch( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.hold;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチされたかどうかを調べる
     * @retval true   タッチされた
     * @retval false  タッチされてない
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsTouchTrigger( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.trigger;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチが離されたかどうかを調べる
     * @retval true   タッチが離された
     * @retval false  タッチが離されていない
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsTouchRelease( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.release;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タッチのリピート入力を検出したかどうかを調べる
     * @retval true   リピート入力を検出した
     * @retval false  リピート入力を検出していない
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsTouchRepeat( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return ( data.repeat || data.trigger );  // リピートはトリガを含む
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ダブルクリックを検出したかどうかを調べる
     * @retval true   検出した
     * @retval false  検出していない
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsDoubleClick( void ) const
    {
      return m_dclick_state.detected;
    }

    /**
     * @brief 実際に参照すべき実行データを取得する
     */
    const TouchPanel::EffectiveData& TouchPanel::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }





    //-------------------------------------------------------------------------
    /**
     * @brief タッチパネル固有の更新処理
     */
    //-------------------------------------------------------------------------
    void TouchPanel::UpdateTouchPanel( void )
    {
      if( this->GetFrameRate() == FRAME_RATE_30 )
      {
        m_update_flag ^= 1;
      }
      else
      {
        m_update_flag = 1;
      }
        
      if( m_update_flag )
      {
        this->UpdateDClickState();
      }
    }

    /**
     * @brief ダブルクリックの状態を更新する
     */
    void TouchPanel::UpdateDClickState( void )
    {
      m_dclick_state.detected = false;

      // 残り時間をデクリメント
      if( m_dclick_state.first ) {
        --m_dclick_state.count;
        if( m_dclick_state.count <= 0 ) {
          m_dclick_state.first = false;
        }
      }

      if( this->IsTouchTrigger() ) 
      {
        u16 touch_x = 0;
        u16 touch_y = 0;
        this->GetXY( &touch_x, &touch_y );

        if( m_dclick_state.first ) {  // 2クリック目
          if( this->CheckDClickRange( touch_x, touch_y ) ) {
            m_dclick_state.detected = true;
            m_dclick_state.first = false;
          }
        }
        else { // 1クリック目を検出
          m_dclick_state.first = true;
          m_dclick_state.count = m_dclick_cond.frame;
          m_dclick_state.first_x = touch_x; 
          m_dclick_state.first_y = touch_y;
        }
      }
    }

    /**
     * @brief ダブルクリックの範囲チェック
     * @param x  チェックするx座標
     * @param y  チェックするy座標
     * @retval true   範囲内
     * @retval false  範囲外
     */
    bool TouchPanel::CheckDClickRange( u16 x, u16 y ) const
    {
      s16 diff_x = x - m_dclick_state.first_x;
      s16 diff_y = y - m_dclick_state.first_y;

      if( m_dclick_cond.range == DCLICK_RANGE_RECT )
      {
        return ( 
            ( gfl::math::Abs(diff_x) <= m_dclick_cond.radius ) &&
            ( gfl::math::Abs(diff_y) <= m_dclick_cond.radius ) );
      }
      else
      {
        u32 dist = ( diff_x * diff_x ) + ( diff_y + diff_y );
        u32 min = m_dclick_cond.radius * m_dclick_cond.radius;
        return ( dist <= min );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ダブルクリックとみなすタッチ間のフレーム数を設定する
     * @param cond 設定する条件
     */
    //-------------------------------------------------------------------------
    void TouchPanel::SetDoubleClickCond( const DoubleClickCond& cond )
    {
      m_dclick_cond = cond;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ダブルクリックとみなすタッチ間のフレーム数を取得する
     */
    //-------------------------------------------------------------------------
    const TouchPanel::DoubleClickCond& TouchPanel::GetDoubleClickCond( void ) const
    {
      return m_dclick_cond;
    }





    //-------------------------------------------------------------------------
    /**
     * @brief 現在の入力と前回の入力を比較し, 同じデータが続いているかどうかを調べる
     * @retval true   前回と今回の入力は同じ
     * @retval false  前回と今回の入力は異なる
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsRepeatingSameInput( void ) const
    {
      const DetectiveData* now_data  = reinterpret_cast<DetectiveData*>(m_nowDetectiveData);
      const DetectiveData* prev_data = reinterpret_cast<DetectiveData*>(m_prevDetectiveData);
      return ( now_data->touch && prev_data->touch );  // タッチが続いていればリピートとする
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データを蓄積する
     * @param buffer               データの格納先
     * @param detective_data       今回の検出データ
     * @param prev_detective_data  前回の検出データ
     */
    //-------------------------------------------------------------------------
    void TouchPanel::StoreEffectiveData( 
        void* buffer, const void* detective_data, const void* prev_detective_data ) const
    {
      const DetectiveData* now_data = reinterpret_cast<const DetectiveData*>( detective_data ); 
      const DetectiveData* prev_data = reinterpret_cast<const DetectiveData*>( prev_detective_data ); 
      EffectiveData* buffer_data = reinterpret_cast<EffectiveData*>( buffer );

      if( now_data->touch ) {
        buffer_data->last_x = now_data->x;
        buffer_data->last_y = now_data->y;
      } 

      if( prev_data->touch ) {
        buffer_data->prev_x = prev_data->x;
        buffer_data->prev_y = prev_data->y;
      }

      bool release = (   prev_data->touch  & (~now_data->touch) );
      bool trigger = ( (~prev_data->touch) &   now_data->touch );

      if( buffer_data->trigger && release )
      {
        // 前フレームの trigger を無効化
        buffer_data->hold    = false;
        buffer_data->trigger = false;
        buffer_data->release = false;
        buffer_data->repeat  = false;
      }
      else if( buffer_data->release && trigger )
      {
        // 前フレームの release を無効化
        buffer_data->hold    = true;
        buffer_data->trigger = false;
        buffer_data->release = false;
        buffer_data->repeat  = m_pRepeatTimer->IsRepeat();
      }
      else
      {
        buffer_data->hold     = now_data->touch;
        buffer_data->trigger |= ( (~prev_data->touch) & now_data->touch );
        buffer_data->release |= ( prev_data->touch & (~now_data->touch) );
        buffer_data->repeat  |= m_pRepeatTimer->IsRepeat() ? now_data->touch : 0;
      }
    }



  } // namespace ui
} // namespace gfl
