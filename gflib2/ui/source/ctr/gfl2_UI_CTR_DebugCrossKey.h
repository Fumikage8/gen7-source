﻿//=============================================================================
/**
 * @file   gfl2_UI_CTR_DebugCrossKey.h
 * @brief  3DSのデバッグパッドの十字キー入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR
#ifndef __gfl2_UI_CTR_DEBUGCROSSKEY_H__
#define __gfl2_UI_CTR_DEBUGCROSSKEY_H__
#include <nn.h>
#include <macro/include/gfl2_Macros.h>
#include "../gfl2_UI_DeviceImplementerForVectorDevice.h"

namespace gfl2 {
  namespace ui {


    class CTR_DebugCrossKey : public DeviceImplementerForVectorDevice 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( CTR_DebugCrossKey );

      public: 

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       *
       * @param p_reader  パッド入力の読み取りを行うインスタンス
       * @param p_status  読み取った入力を取得するバッファ
       */
      //-----------------------------------------------------------------------
      CTR_DebugCrossKey( nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_DebugCrossKey();

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
      nn::hid::DebugPadReader* m_pPadReader;
      nn::hid::DebugPadStatus* m_pPadStatus;
    };


  } //namespace ui
} //namespace gfl2

#endif //__gfl2_UI_CTR_DEBUGCROSSKEY_H__
#endif