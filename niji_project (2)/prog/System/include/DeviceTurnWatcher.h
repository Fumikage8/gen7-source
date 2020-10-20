//=============================================================================
/**
 * @file    DeviceTurnWatcher.h
 * @brief   3DSが逆さまになっているかどうかの判定
 * @author  tamada
 * @date    2012.05.26
 */
//=============================================================================
#if !defined( __DEVICETURNWATCHER_H__ )
#define __DEVICETURNWATCHER_H__ 

#pragma once

// gflib2
#include <heap/include/gfl2_Heap.h>
#include <ui/include/gfl2_UI.h>


GFL_NAMESPACE_BEGIN(System)

  //======================================================================
  /**
   * @brief 逆さ判定用クラス
   */
  //======================================================================
  class DeviceTurnWatcher {

    public:
      DeviceTurnWatcher( gfl2::heap::HeapBase * heap, gfl2::ui::DeviceManager * device_manager );

      ~DeviceTurnWatcher();

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      /// 計測レート
      enum COUNT_RATE{
        COUNT_RATE_60F = 1,     ///< 計測レート：60フレーム
        COUNT_RATE_30F = 2,     ///< 計測レート：30フレーム
      };

      /// 計測モード
      enum COUNT_MODE{
        COUNT_MODE_NORMAL = 0,  ///< 計測モード：通常（3秒）
        COUNT_MODE_SHORT = 1,   ///< 計測モード：短縮（1秒）
      };

      //------------------------------------------------------------------------
      /**
       * @brief 初期化処理
       * @param[in] rate    計測レート
       * @param[in] mode    計測モード
       */
      //------------------------------------------------------------------------
      void Initialize( COUNT_RATE rate = COUNT_RATE_30F, COUNT_MODE mode = COUNT_MODE_NORMAL );

      //------------------------------------------------------------------------
      /**
       * @brief 判定処理
       * @note  判定期間中、毎フレーム呼び出してください
       */
      //------------------------------------------------------------------------
      void Update();

      //------------------------------------------------------------------------
      /**
       * @brief 結果取得
       * @retval  true  ひっくり返った
       * @retval  false 通常の向きのままだった
       */
      //------------------------------------------------------------------------
      bool GetResult() { return m_is_turned; }

    private:
      gfl2::ui::DeviceManager * m_device_manager;
      bool m_is_turned;
      u32 m_count;
      COUNT_RATE m_rate;
      COUNT_MODE m_mode;
      u32 m_count_limit;

  };


GFL_NAMESPACE_END(System)

#endif  // __DEVICETURNWATCHER_H__ 
