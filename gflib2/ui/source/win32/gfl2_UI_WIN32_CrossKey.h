//=============================================================================
/**
 * @file   gfl2_UI_WIN32_CrossKey.h
 * @brief  3DSの十字キー入力を検出するクラス
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifdef  GF_PLATFORM_WIN32
#ifndef __gfl2_UI_WIN32CROSSKEY_H__
#define __gfl2_UI_WIN32CROSSKEY_H__

#include <macro/include/gfl2_Macros.h>
#include "../gfl2_UI_DeviceImplementerForVectorDevice.h"

namespace gfl2 {
  namespace ui {


    class WIN32_CrossKey : public DeviceImplementerForVectorDevice 
    {
      GFL_FORBID_COPY_AND_ASSIGN( WIN32_CrossKey );

      public: 

      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param[in] reader  ボタン入力の読み取りを行うインスタンス
       * @param[in] status  読み取ったボタン入力を取得するバッファ
       */
      WIN32_CrossKey( void );

      /**
       * @brief デストラクタ
       */
      virtual ~WIN32_CrossKey(); 

      //-----------------------------------------------------------------------
      // 基本動作
      //-----------------------------------------------------------------------
      /**
       * @brief メイン処理
       */
      virtual void Main( void ); 

      /**
       * @brief 入力データを取得する
       * @param buffer  取得したデータの格納先 
       */
      virtual void GetDetectiveData( void* buffer ) const;

      /**
       * @brief アナログスティックのクランプモードを取得する
       */
      virtual VectorDevice::StickClampMode GetStickClampMode( void ) const;

      /**
       * @brief アナログスティックのクランプモードを設定する
       */
      virtual void SetStickClampMode( VectorDevice::StickClampMode clamp_mode );

      /**
       * @brief アナログスティックのクランプパラメータを取得する
       * @param[out] min  最小クランプ値の格納先
       * @param[out] max  最大クランプ値の格納先
       */
      virtual void GetStickClampParam( s16* min, s16* max ) const;

      /**
       * @brief アナログスティックのクランプパラメータを設定する
       * @param[in] min  最小クランプ値
       * @param[in] max  最大クランプ値
       */
      virtual void SetStickClampParam( s16 min, s16 max );

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

#endif //__gfl2_UI_WIN32CROSSKEY_H__
#endif