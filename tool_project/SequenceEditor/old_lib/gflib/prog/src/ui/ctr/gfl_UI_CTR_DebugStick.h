//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugStick.h
 * @brief  3DSのデバッグパッドのアナログスティック入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifndef __GFL_UI_CTR_DEBUGSTICK_H__
#define __GFL_UI_CTR_DEBUGSTICK_H__
#include "gfl_Macros.h"
#include "../gfl_UI_DeviceImplementerForVectorDevice.h"

namespace gfl {
  namespace ui {


    class CTR_DebugStick : public DeviceImplementerForVectorDevice
    {
      GFL_FORBID_COPY_AND_ASSIGN( CTR_DebugStick );

      public:

      // どのスティックか？
      enum StickID {
        STICK_ID_LEFT,   // 左スティック
        STICK_ID_RIGHT,  // 右スティック
      };

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       *
       * @param stick_id  どのスティックか
       * @param p_reader  入力の読み取りを実行するインスタンス
       * @param p_status  読み取った入力を取得するバッファ
       */
      //-----------------------------------------------------------------------
      CTR_DebugStick( 
          StickID stick_id, 
          nn::hid::DebugPadReader* p_reader,
          nn::hid::DebugPadStatus* p_status );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_DebugStick();

      //-----------------------------------------------------------------------
      /**
       * @brief メイン処理
       */
      //-----------------------------------------------------------------------
      virtual void Main( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 入力データを取得する
       *
       * @param p_buffer  取得したデータの格納先 
       */
      //-----------------------------------------------------------------------
      virtual void GetDetectiveData( void* p_buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプモードを取得する
       */
      //-----------------------------------------------------------------------
      virtual VectorDevice::StickClampMode GetStickClampMode( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプモードを設定する
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampMode( VectorDevice::StickClampMode clamp_mode );

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプパラメータを取得する
       *
       * @param[out] min  最小クランプ値の格納先
       * @param[out] max  最大クランプ値の格納先
       */
      //-----------------------------------------------------------------------
      virtual void GetStickClampParam( s16* min, s16* max ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプパラメータを設定する
       *
       * @param[in] min  最小クランプ値
       * @param[in] max  最大クランプ値
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampParam( s16 min, s16 max );


      protected:
      StickID m_stickId;
      nn::hid::DebugPadReader* m_pPadReader;
      nn::hid::DebugPadStatus* m_pPadStatus;
    };


  } //namespace ui
} //namespace gfl

#endif //__GFL_UI_CTR_DEBUGSTICK_H__
