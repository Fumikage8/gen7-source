//=============================================================================
/**
 * @file   gfl_UI_VectorDevice.h
 * @brief  方向入力デバイスのアクセッサ
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __GFL_UI_VECTORDEVICE_H__
#define __GFL_UI_VECTORDEVICE_H__
#pragma once

#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "math/gfl_MathType.h"
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Device.h"


namespace gfl {
  namespace ui {

    class DeviceImplementerForVectorDevice;


    class VectorDevice : public Device 
    {
      GFL_FORBID_COPY_AND_ASSIGN( VectorDevice );
      friend class DeviceManager;

      public:


      // 検出データ
      struct DetectiveData 
      {
        f32 x;  // x 方向入力
        f32 y;  // y 方向入力
      };

      // 実効データ
      struct EffectiveData 
      {
        f32 x;        // x方向入力
        f32 y;        // y方向入力
        u32 hold;     // 現時点で「押されている」と判定される方向の論理和
        u32 trigger;  // 現時点で「押された」と判定される方向の論理和
        u32 release;  // 現時点で「離された」と判定される方向の論理和
        u32 repeat;   // 現時点で「押され続けている」と判定される方向の論理和
      };



      //-----------------------------------------------------------------------
      /**
       * @brief 入力された方向を取得する
       *
       * @return 入力方向のx成分[-1, 1]
       */
      //-----------------------------------------------------------------------
      f32 GetX( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 入力された方向を取得する
       *
       * @return 入力方向のy成分[-1, 1]
       */
      //-----------------------------------------------------------------------
      f32 GetY( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 入力された方向を取得する
       *
       * @param dest_x  取得したx成分[-1, 1]の格納先
       * @param dest_y  取得したy成分[-1, 1]の格納先
       */
      //-----------------------------------------------------------------------
      void GetVector( f32* dest_x, f32* dest_y ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 入力された方向を取得する
       *
       * @param dest_vec  取得した入力方向ベクトルの格納先
       */
      //-----------------------------------------------------------------------
      void GetVector( gfl::math::VEC2* dest_vec ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 入力された方向を取得する
       *
       * @param dest_vec  取得した入力方向ベクトルの格納先
       */
      //-----------------------------------------------------------------------
      void GetVector( gfl::math::VEC3* dest_vec ) const; 




      //-----------------------------------------------------------------------
      /**
       * @brief 指定した方向が入力されているかどうかを調べる
       *
       * @param dir  判定する向き( gfl::ui::DIR_xxxx )
       *
       * @retval true   入力されている
       * @retval false  入力されていない
       */
      //-----------------------------------------------------------------------
      bool IsHold( u32 dir ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 指定した方向が入力されたかどうかを調べる
       *
       * @param dir  判定する向き( gfl::ui::DIR_xxxx )
       *
       * @retval true   入力された
       * @retval false  入力されていない
       */
      //-----------------------------------------------------------------------
      bool IsTrigger( u32 dir ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 指定した方向の入力が無くなったかどうかを調べる
       *
       * @param dir  判定する向き( gfl::ui::DIR_xxxx )
       *
       * @retval true   入力が無くなった
       * @retval false  入力が無くなっていない
       */
      //-----------------------------------------------------------------------
      bool IsRelease( u32 dir ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 指定した方向のリピートを検出したかどうかを調べる
       *
       * @param dir  判定する向き( gfl::ui::DIR_xxxx )
       *
       * @retval true   リピートを検出した
       * @retval false  入リピートを検出していない
       */
      //-----------------------------------------------------------------------
      bool IsRepeat( u32 dir ) const;



      //-----------------------------------------------------------------------
      /**
       * @brief 押されている方向の情報を取得する
       * @return 押されている全ての方向( gfl::ui::DIR_xxxx )の論理和
       */
      //-----------------------------------------------------------------------
      u32 GetHold( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 押された方向の情報を取得する
       * @return 押された全ての方向( gfl::ui::DIR_xxxx )の論理和
       */
      //-----------------------------------------------------------------------
      u32 GetTrigger( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 離されたか方向の情報を取得する
       * @return 離された全ての方向( gfl::ui::DIR_xxxx )の論理和
       */
      //-----------------------------------------------------------------------
      u32 GetRelease( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief リピートを検出した方向の情報を取得する
       * @return リピートを検出した全ての方向( gfl::ui::DIR_xxxx )の論理和
       */
      //-----------------------------------------------------------------------
      u32 GetRepeat( void ) const;



      // 最新の検出情報を取得する
      // これらのメソッドでは, 動作フレームレートの設定に拘らず
      // 最後の更新で検出した値を返します.
      f32 GetLatestX( void ) const;
      f32 GetLatestY( void ) const;
      bool IsHoldLatest( u32 dir ) const;
      bool IsTriggerLatest( u32 dir ) const;
      bool IsReleaseLatest( u32 dir ) const;
      bool IsRepeatLatest( u32 dir ) const;



      // アナログスティックのクランプモード
      enum StickClampMode {
        STICK_CLAMP_MODE_CIRCLE,   // 円形クランプ
        STICK_CLAMP_MODE_CROSS,    // 十字形クランプ
        STICK_CLAMP_MODE_MINIMUM,  // 最小クランプ
      };

      //-------------------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプモードを取得する
       */
      //-------------------------------------------------------------------------------
      StickClampMode GetStickClampMode( void ) const;

      //-------------------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプモードを設定する
       * @param clamp_mode 設定するクランプモード
       */
      //-------------------------------------------------------------------------------
      void SetStickClampMode( StickClampMode clamp_mode );


      // モード別のクランプパラメータ上限値・下限値
      static const s16 MAX_STICK_CLAMP_PARAM_ON_CIRCLE_MODE;
      static const s16 MIN_STICK_CLAMP_PARAM_ON_CIRCLE_MODE;
      static const s16 MAX_STICK_CLAMP_PARAM_ON_CROSS_MODE;
      static const s16 MIN_STICK_CLAMP_PARAM_ON_CROSS_MODE;
      static const s16 MAX_STICK_CLAMP_PARAM_ON_MINIMUM_MODE;
      static const s16 MIN_STICK_CLAMP_PARAM_ON_MINIMUM_MODE;

      //-------------------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプパラメータを取得する
       *
       * @param[out] min  最小クランプ値の格納先
       * @param[out] max  最大クランプ値の格納先
       */
      //-------------------------------------------------------------------------------
      void GetStickClampParam( s16* min, s16* max ) const;

      //-------------------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプパラメータを設定する
       *
       * @param[in] min  最小クランプ値
       * @param[in] max  最大クランプ値
       */
      //-------------------------------------------------------------------------------
      void SetStickClampParam( s16 min, s16 max );





      protected:

      /**
       * @brief コンストラクタ
       *
       * @param heap         使用するヒープ
       * @param implementer  実際に入力の読み取りを行うインスタンス
       */
      VectorDevice( gfl::heap::HeapBase* heap, DeviceImplementerForVectorDevice* implementer );

      /**
       * @brief デストラクタ
       */
      virtual ~VectorDevice(); 

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
      virtual void StoreEffectiveData( 
          void* buffer,
          const void* detective_data,
          const void* prev_detective_data ) const;



      private:
      DeviceImplementerForVectorDevice* m_pImplementer;

      u32 GetHoldState( f32, f32 ) const;
      void MergeDetectData( EffectiveData*, u32, u32, Direction ) const;
      const EffectiveData& GetEffectiveData( void ) const;

    };


  } // namespace ui
} // namespace gfl

#endif // __GFL_UI_VECTORDEVICE_H__
