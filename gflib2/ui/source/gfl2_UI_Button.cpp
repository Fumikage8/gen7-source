//=============================================================================
/**
 * @file   gfl2_UI_Button.cpp
 * @brief  ボタンのアクセッサ
 * @author obata_toshihiro
 * @date   2010.10.22
 */
//=============================================================================
#include <macro/include/gfl2_Macros.h>
#include "ui/include/gfl2_UI_Button.h"
#include "./gfl2_UI_DeviceImplementer.h"
#include "./gfl2_UI_RepeatTimer.h"
#include <debug/include/gfl2_Assert.h>

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap         使用するヒープ
     * @param implementer  実際に入力を読み取るインスタンス
     */
    //-------------------------------------------------------------------------
    Button::Button( gfl2::heap::HeapBase* heap, DeviceImplementer* implementer ) :
      Device( heap, implementer )
    {
      gfl2::std::MemClear( m_buttonInfo, sizeof(m_buttonInfo) );
      this->InitButtonAssignment();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    Button::~Button()
    {
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 指定したボタンが押されているかどうか調べる
     * @param button       調べるボタン ( gfl2::ui::BUTTON_xxxx )
     * @param targetState  チェック対象の入力状態
     * @retval true   押されている
     * @retval false  押されていない
     */
    //-------------------------------------------------------------------------
    bool Button::IsHold( u32 button, InputStateID targetState ) const
    {
      const u32 hold = this->GetHold( targetState );
      return ( hold & button ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したボタンが押されたかどうかを調べる
     * @param button       調べるボタン ( gfl2::ui::BUTTON_xxxx )
     * @param targetState  チェック対象の入力状態
     * @retval true   押された
     * @retval false  押されていない
     */
    //-------------------------------------------------------------------------
    bool Button::IsTrigger( u32 button, InputStateID targetState ) const
    {
      const u32 trigger = this->GetTrigger( targetState );
      return ( trigger & button ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したボタンが離されたかどうかを調べる
     * @param button       調べるボタン ( gfl2::ui::BUTTON_xxxx )
     * @param targetState  チェック対象の入力状態
     * @retval true   離された
     * @retval false  離されていない
     */
    //-------------------------------------------------------------------------
    bool Button::IsRelease( u32 button, InputStateID targetState ) const
    {
      const u32 release = this->GetRelease( targetState );
      return ( release & button ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したボタンが押され続けているかどうかを調べる
     * @param button       調べるボタン ( gfl2::ui::BUTTON_xxxx )
     * @param targetState  チェック対象の入力状態
     * @retval true   押され続けていて, リピートタイミングを検出した
     * @retval false  押され続けていない, またはリピートタイミングでない
     */
    //-------------------------------------------------------------------------
    bool Button::IsRepeat( u32 button, InputStateID targetState ) const
    {
      const u32 repeat = this->GetRepeat( targetState );
      return ( repeat & button ) != 0;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 押されているボタンの情報を取得する
     * @param targetState  チェック対象の入力状態
     * @return 押されている全てのボタン( gfl2::ui::BUTTON_xxxx )の論理和
     */
    //-------------------------------------------------------------------------
    u32 Button::GetHold( InputStateID targetState ) const
    {
      const InputState& state = this->GetEffectiveInputState( targetState );
      return state.hold;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 押されたボタンの情報を取得する
     * @param targetState  チェック対象の入力状態
     * @return 押された全てのボタン( gfl2::ui::BUTTON_xxxx )の論理和
     */
    //-------------------------------------------------------------------------
    u32 Button::GetTrigger( InputStateID targetState ) const
    {
      const InputState& state = this->GetEffectiveInputState( targetState );
      return state.trigger;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 離されたかボタンの情報を取得する
     * @param targetState  チェック対象の入力状態
     * @return 離された全てのボタン( gfl2::ui::BUTTON_xxxx )の論理和
     */
    //-------------------------------------------------------------------------
    u32 Button::GetRelease( InputStateID targetState ) const
    {
      const InputState& state = this->GetEffectiveInputState( targetState );
      return state.release;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リピートを検出したボタンの情報を取得する
     * @param targetState  チェック対象の入力状態
     * @return リピートを検出した全てのボタン( gfl2::ui::BUTTON_xxxx )の論理和
     */
    //-------------------------------------------------------------------------
    u32 Button::GetRepeat( InputStateID targetState ) const
    {
      const InputState& state = this->GetEffectiveInputState( targetState );
      return ( state.trigger | state.repeat );  // リピートはトリガを含む
    }

    /**
     * @brief 実際に参照すべき入力状態を取得する
     * @param targetState  チェック対象の入力状態
     */
    const Button::InputState& Button::GetEffectiveInputState( InputStateID targetState ) const
    {
      const EffectiveData& data = this->GetEffectiveData();

      if( INPUT_STATE_NUM <= targetState ) {
        GFL_ASSERT(0); //@check
        return data.inputState[ INPUT_STATE_ASSIGNED ];
      } 
      else {
        return data.inputState[ targetState ];
      }
    }

    /**
     * @brief 実際に参照すべき実行データを取得する
     */
    const Button::EffectiveData& Button::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 現在の検出データと前回の検出データを比較し, 同じデータが続いているかどうかを調べる
     * @retval true   前回の検出データと今回の検出データが同じ
     * @retval false  前回の検出データと今回の検出データが異なる
     */
    //-------------------------------------------------------------------------
    bool Button::IsRepeatingSameInput( void ) const
    {
      const DetectiveData* now_data  = reinterpret_cast<DetectiveData*>(m_nowDetectiveData);
      const DetectiveData* prev_data = reinterpret_cast<DetectiveData*>(m_prevDetectiveData);

      if( ( now_data->hold != 0 ) && 
          ( now_data->hold == prev_data->hold ) )
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
    void Button::StoreEffectiveData( 
        void* buffer,
        const void* detective_data,
        const void* prev_detective_data ) const
    {
      const DetectiveData* now_data = reinterpret_cast<const DetectiveData*>( detective_data ); 
      const DetectiveData* prev_data = reinterpret_cast<const DetectiveData*>( prev_detective_data ); 

      DetectiveData assigned_now_data, assigned_prev_data;
      this->CalcAssignedDetectiveData( &assigned_now_data, *now_data );
      this->CalcAssignedDetectiveData( &assigned_prev_data, *prev_data );

      EffectiveData* effective_data = reinterpret_cast<EffectiveData*>( buffer ); 
      for( u32 i=0; i<BUTTON_NUM; i++ ) {
        const u32 button_bit = ( 1 << i );
        this->CalcInputState( &(effective_data->inputState[ INPUT_STATE_ASSIGNED ]), assigned_now_data.hold, assigned_prev_data.hold, button_bit );
        this->CalcInputState( &(effective_data->inputState[ INPUT_STATE_ORIGINAL ]), now_data->hold, prev_data->hold, button_bit );
      }
    }

    /**
     * @brief 検出データに対し, ボタンの割り当て情報を適用する
     * @param[out] pDestData  ボタン割り当て情報を適用したデータ
     * @param[in]  srcData    ボタン割り当て情報を適用する元データ
     */
    void Button::CalcAssignedDetectiveData( DetectiveData* pDestData, const DetectiveData& srcData ) const
    {
      pDestData->hold = 0;

      for( u32 i=0; i<BUTTON_NUM; i++ )
      {
        if( srcData.hold & m_buttonInfo[i].assignedButton ) {
          pDestData->hold |= (1<<i);
        }
      }
    }

    /**
     * @brief 前回と最新の入力から, 入力状態を作成する
     * @param[out] buffer     作成したデータの格納先
     * @param[in]  now_hold   今回のホールド状態
     * @param[in]  prev_hold  前回のホールド状態
     * @param[in]  button     解決するボタン
     */
    void Button::CalcInputState( InputState* buffer, u32 now_hold, u32 prev_hold, u32 button ) const
    {
      bool hold    = ( (               now_hold & button ) != 0 );
      bool trigger = ( ( ~prev_hold &  now_hold & button ) != 0 );
      bool release = ( (  prev_hold & ~now_hold & button ) != 0 );
      bool repeat  = m_pRepeatTimer->IsRepeat() && hold;

      if( release )
      {
        if( buffer->trigger & button )
        {
          // 前フレームのトリガを無効化
          buffer->hold    &= ~button;
          buffer->trigger &= ~button;
          buffer->release &= ~button;
          buffer->repeat  &= ~button;
        }
        else
        {
          // リリースされた
          buffer->hold    &= ~button;
          buffer->trigger &= ~button;
          buffer->release |=  button;
          buffer->repeat  &= ~button;
        }
      }
      else if( ( buffer->release & button ) && trigger )
      {
        // 前フレームのリリースを無効化
        buffer->hold    |=  button;
        buffer->trigger &= ~button;
        buffer->release &= ~button;

        if( repeat ) {
          buffer->repeat |= button;
        }
        else {
          buffer->repeat &= ~button;
        }
      }
      else
      {
        buffer->hold    |= ( hold ) ? ( button ) : ( 0 );
        buffer->trigger |= ( trigger ) ? ( button ) : ( 0 );
        buffer->release |= ( release ) ? ( button ) : ( 0 );
        buffer->repeat  |= ( repeat ) ? ( button ) : ( 0 );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタン割り当て情報を追加する
     * @param targetButton  操作対象のボタン
     * @param assignButton  割り当てるボタン
     *
     * @note AddButtonAssignment( gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L ) とした場合,
     *       Lボタンを押すと, Aボタンが押された判定になります.
     */
    //-------------------------------------------------------------------------
    void Button::AddButtonAssignment( ButtonID targetButton, ButtonID assignButton )
    {
      m_buttonInfo[ targetButton ].assignedButton |= (1<<assignButton);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタン割り当て情報を解除する
     * @param targetButton  操作対象のボタン
     * @param removeButton  割り当てを解除するボタン
     *
     * @note RemoveButtonAssignment( gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L ) とした場合,
     *       Lボタンを押しても, Aボタンが押された判定にならなくなります.
     */
    //-------------------------------------------------------------------------
    void Button::RemoveButtonAssignment( ButtonID targetButton, ButtonID removeButton )
    {
      m_buttonInfo[ targetButton ].assignedButton &= ~(1<<removeButton);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタン割り当て情報を初期化する
     */
    //-------------------------------------------------------------------------
    void Button::InitButtonAssignment( void )
    {
      for( u32 i=0; i<BUTTON_NUM; i++ ) {
        m_buttonInfo[i].assignedButton = ( 1 << i );
      }
    }



  } // namespace ui
} // namespace gfl2
