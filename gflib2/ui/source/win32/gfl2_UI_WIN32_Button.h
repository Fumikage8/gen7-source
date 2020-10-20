//=============================================================================
/**
 * @file   gfl2_UI_WIN32_Button.h
 * @brief  3DSのボタン入力を検出するクラス
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifdef  GF_PLATFORM_WIN32
#ifndef __gfl2_UI_WIN32BUTTON_H__
#define __gfl2_UI_WIN32BUTTON_H__

#include <macro/include/gfl2_Macros.h>
#include "../gfl2_UI_DeviceImplementer.h"

namespace gfl2 {
  namespace ui {


    class WIN32_Button : public DeviceImplementer 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( WIN32_Button );

      public:

      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param[in] reader  ボタン入力の読み取りを行うインスタンス
       * @param[in] status  読み取ったボタン入力を取得するバッファ
       */
      WIN32_Button();

      /**
       * @brief デストラクタ
       */
      virtual ~WIN32_Button();

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
      bool m_isStickEmulationEnable;
    };


  } //namespace ui
} //namespace gfl2

#endif //__gfl2_UI_WIN32BUTTON_H__
#endif