﻿//=============================================================================
/**
 * @file   gfl2_UI_CTR_Button.h
 * @brief  3DSのボタン入力を検出するクラス
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR
#ifndef __gfl2_UI_CTRBUTTON_H__
#define __gfl2_UI_CTRBUTTON_H__
#include <nn.h>
#include <macro/include/gfl2_Macros.h>
#include "../gfl2_UI_DeviceImplementer.h"

namespace gfl2 {
  namespace ui {


    class CTR_Button : public DeviceImplementer 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( CTR_Button );

      public:

      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param[in] reader  ボタン入力の読み取りを行うインスタンス
       * @param[in] status  読み取ったボタン入力を取得するバッファ
       */
      CTR_Button( nn::hid::PadReader* reader, nn::hid::PadStatus* status );

      /**
       * @brief デストラクタ
       */
      virtual ~CTR_Button();

      //-----------------------------------------------------------------------
      // 基本動作
      //-----------------------------------------------------------------------
      /**
       * @brief メイン処理
       */
      virtual void Main( void ); 

      /**
       * @brief 入力データを取得する
       * @param[out] buffer  取得したデータの格納先 
       */
      virtual void GetDetectiveData( void* buffer ) const;

      /**
       * @brief 入力データのサイズを取得する
       */
      virtual u32 GetDetectiveDataSize( void ) const;

      /**
       * @brief 実効データのサイズを取得する
       */
      virtual u32 GetEffectiveDataSize( void ) const;

      //-----------------------------------------------------------------------
      // スティックによるエミュレーション
      //----------------------------------------------------------------------- 
      /**
       * @brief アナログスティックによるエミュレーションを有効にする
       */
      void SetStickEmulationEnable( void );

      /**
       * @brief アナログスティックによるエミュレーションを無効にする
       */
      void SetStickEmulationDisable( void );


      protected:
      nn::hid::PadReader* m_pPadReader;
      nn::hid::PadStatus* m_pPadStatus;
      bool m_isStickEmulationEnable;
    };


  } //namespace ui
} //namespace gfl2

#endif //__gfl2_UI_CTRBUTTON_H__
#endif