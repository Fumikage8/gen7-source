//=============================================================================
/**
 * @file   gfl2_UI_Button.h
 * @brief  ボタンのアクセッサ
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __gfl2_UI_BUTTON_H__
#define __gfl2_UI_BUTTON_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <macro/include/gfl2_Macros.h>
#include <ui/include/gfl2_UI_Types.h>
#include "ui/include/gfl2_UI_Device.h"

namespace gfl2 {
  namespace ui {

    class DeviceImplementer;


    class Button : public Device 
    {
      GFL_FORBID_COPY_AND_ASSIGN( Button );
      friend class DeviceManager;


      public: 

      // 検出データ
      struct DetectiveData 
      {
        u32 hold;  // 押されているボタン
      }; 

      // 入力状態
      struct InputState
      {
        u32 hold;     //「押されている」ボタンの論理和
        u32 trigger;  //「押された」ボタンの論理和
        u32 release;  //「離された」ボタンの論理和
        u32 repeat;   //「押され続けている」ボタンの論理和
      };

      // 入力状態の識別子
      enum InputStateID {
        INPUT_STATE_ASSIGNED,  // ボタンの割り当てが適用された状態
        INPUT_STATE_ORIGINAL,  // オリジナルの状態
        INPUT_STATE_NUM,
      };

      // 実効データ
      struct EffectiveData 
      {
        InputState inputState[ INPUT_STATE_NUM ];
      };

      // ボタン毎の情報
      struct ButtonInfo
      {
        u32 assignedButton;
      };

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したボタンが押されているかどうか調べる
       * @param button       調べるボタン ( gfl2::ui::BUTTON_xxxx )
       * @param targetState  チェック対象の入力状態
       * @retval true   押されている
       * @retval false  押されていない
       */
      //-----------------------------------------------------------------------
      bool IsHold( u32 button, InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したボタンが押されたかどうかを調べる
       * @param button       調べるボタン ( gfl2::ui::BUTTON_xxxx )
       * @param targetState  チェック対象の入力状態
       * @retval true   押された
       * @retval false  押されていない
       */
      //-----------------------------------------------------------------------
      bool IsTrigger( u32 button, InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したボタンが離されたかどうかを調べる
       * @param button       調べるボタン ( gfl2::ui::BUTTON_xxxx )
       * @param targetState  チェック対象の入力状態
       * @retval true   離された
       * @retval false  離されていない
       */
      //-----------------------------------------------------------------------
      bool IsRelease( u32 button, InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したボタンが押され続けているかどうかを調べる
       * @param button       調べるボタン ( gfl2::ui::BUTTON_xxxx )
       * @param targetState  チェック対象の入力状態
       * @retval true   押され続けていて, リピートタイミングを検出した
       * @retval false  押され続けていない, またはリピートタイミングでない
       */
      //-----------------------------------------------------------------------
      bool IsRepeat( u32 button, InputStateID targetState=INPUT_STATE_ASSIGNED ) const; 

      //-----------------------------------------------------------------------
      /**
       * @brief 押されているボタンの情報を取得する
       * @param targetState  チェック対象の入力状態
       * @return 押されている全てのボタン( gfl2::ui::BUTTON_xxxx )の論理和
       */
      //-----------------------------------------------------------------------
      u32 GetHold( InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 押されたボタンの情報を取得する
       * @param targetState  チェック対象の入力状態
       * @return 押された全てのボタン( gfl2::ui::BUTTON_xxxx )の論理和
       */
      //-----------------------------------------------------------------------
      u32 GetTrigger( InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 離されたかボタンの情報を取得する
       * @param targetState  チェック対象の入力状態
       * @return 離された全てのボタン( gfl2::ui::BUTTON_xxxx )の論理和
       */
      //-----------------------------------------------------------------------
      u32 GetRelease( InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief リピートを検出したボタンの情報を取得する
       * @param targetState  チェック対象の入力状態
       * @return リピートを検出した全てのボタン( gfl2::ui::BUTTON_xxxx )の論理和
       */
      //-----------------------------------------------------------------------
      u32 GetRepeat( InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ボタン割り当て情報を追加する
       * @param targetButton  操作対象のボタン
       * @param assignButton  割り当てるボタン
       *
       * @note AddButtonAssignment( gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L ) とした場合,
       *       Lボタンを押すと, LボタンとAボタンが押された判定になります.
       */
      //-----------------------------------------------------------------------
      void AddButtonAssignment( ButtonID targetButton, ButtonID assignButton );

      //-----------------------------------------------------------------------
      /**
       * @brief ボタン割り当て情報を解除する
       * @param targetButton  操作対象のボタン
       * @param removeButton  割り当てを解除するボタン
       *
       * @note RemoveButtonAssignment( gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L ) とした場合,
       *       Lボタンを押しても, Aボタンが押された判定にならなくなります.
       */
      //-----------------------------------------------------------------------
      void RemoveButtonAssignment( ButtonID targetButton, ButtonID removeButton );

      //-----------------------------------------------------------------------
      /**
       * @brief ボタン割り当て情報を初期化する
       */
      //-----------------------------------------------------------------------
      void InitButtonAssignment( void );



      protected:

      /**
       * @brief コンストラクタ
       * @param heap         使用するヒープ
       * @param implementer  実際に入力を読み取るインスタンス
       */
      Button( gfl2::heap::HeapBase* heap, DeviceImplementer* implementer );

      /**
       * @brief デストラクタ
       */
      virtual ~Button(); 

      /**
       * @brief 現在の検出データと前回の検出データを比較し, 同じデータが続いているかどうかを調べる
       * @retval true   前回の検出データと今回の検出データが同じ
       * @retval false  前回の検出データと今回の検出データが異なる
       */
      virtual bool IsRepeatingSameInput( void ) const; 

      /**
       * @brief 実効データを蓄積する
       * @param[out] buffer               データの格納先
       * @param[in]  detective_data       今回の検出データ
       * @param[in]  prev_detective_data  前回の検出データ
       */
      virtual void StoreEffectiveData( void* buffer, const void* detective_data, const void* prev_detective_data ) const;

      /**
       * @brief 前回と最新の入力から, 入力状態を作成する
       * @param[out] buffer     作成したデータの格納先
       * @param[in]  now_hold   今回のホールド状態
       * @param[in]  prev_hold  前回のホールド状態
       * @param[in]  button     解決するボタン
       */
      void CalcInputState( InputState* buffer, u32 now_hold, u32 prev_hold, u32 button ) const;


      private:
      ButtonInfo m_buttonInfo[ BUTTON_NUM ];

      private:
      void CalcAssignedDetectiveData( DetectiveData*, const DetectiveData& ) const;
      const InputState& GetEffectiveInputState( InputStateID ) const;
      const EffectiveData& GetEffectiveData( void ) const;
    };


  }  //namespace ui
}  //namespace gfl2

#endif // __gfl2_UI_BUTTON_H__
