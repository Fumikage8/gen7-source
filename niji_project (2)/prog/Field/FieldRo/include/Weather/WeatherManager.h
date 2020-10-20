//======================================================================
/**
 * @file WeatherManager.h
 * @date 2015/10/20 16:32:49
 * @author saito_nozomu
 * @brief 天候管理クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __WEATHER_MANAGER_H_INCLUDED__
#define __WEATHER_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "Field/FieldRo/include/Weather/WeatherTypeDef.h"

class BaseCollisionScene;

GFL_NAMESPACE_BEGIN(Field)

GFL_NAMESPACE_BEGIN(Camera)
  class CameraManager;
GFL_NAMESPACE_END(Camera)

GFL_NAMESPACE_BEGIN( Effect )
  class EffectManager;
GFL_NAMESPACE_END(Effect)

  GFL_NAMESPACE_BEGIN(weather)
  class WeatherBase;
  class Rain;
  class Snow;
  class SandStorm;

  /**
   * @class MyClass
   * @brief クラスの説明
   */
  class WeatherManager
  {
      GFL_FORBID_COPY_AND_ASSIGN(WeatherManager);

  public:
    WeatherManager(){
      m_heap = NULL;
      m_pEffectManager = NULL;
      m_pCameraManager = NULL;
      m_pCollisionSceneContainer = NULL;
      m_pWeather = NULL;
      m_pFadeoutWeather = NULL;
      m_pRain = NULL;
      m_pSnow = NULL;
      m_pSandStorm = NULL;
      m_seq = SEQ_IDLE;
      m_ReserveWeatherType = WEATHER_NONE;
    }
    ~WeatherManager(){}
    struct InitParam
    {
      gfl2::heap::HeapBase *pHeap;
      Effect::EffectManager *pEffectManager;
      Camera::CameraManager *pCameraManager;
      BaseCollisionScene    *pCollisionSceneContainer; 
    };
    
    void Initialize(InitParam &param);

    void Terminate(void);

    /**
     * @fn
     * 管理クラスの稼動開始
     * @brief 管理クラスの稼動開始
     * @detail 稼動させることで、Update関数の中身が実行される
     */
    void Start(WEATHER_TYPE type);

    /**
     * @fn
     * 管理クラスの稼動終了
     * @brief 管理クラスの稼動終了
     * @detail 稼動を終了させる。Update関数の中身が実行されなくなる
     */
    void End(void);
 
    /**
     * @brief アップデート処理
     * @detail 毎シンクコールする
     */
    void Update(void);

    /**
     * @brief 稼動している天候があるかチェック
     * @return tureであるfalseでない
     */
    bool CheckExecute(void);

    /**
     * @brief 天候変更
     * @param type 天候種類
     */
    void Change(WEATHER_TYPE type);

    /**
     * @brief 現在の天候取得
     * @return WEATHER_TYPE 天候種類
     */
    WEATHER_TYPE GetNowWeather(void);

    /**
     * @brief 天候実行
     * @param type 天候種類
     */
    void Execute(WEATHER_TYPE type);

    /**
     * @brief 天候が変化中か？
     * @return tureで変更中
     */
    bool IsChanging(void);

  private:

    /**
     * @brief 天候タイプから天候カテゴリを取得
     * @param type 天候タイプ
     * @return WEATHER_CATEGORY 天候カテゴリ
     */
    WEATHER_CATEGORY GetWeatherCategory(WEATHER_TYPE type);

    enum SEQ
    {
      SEQ_IDLE,
      SEQ_SETUP_WAIT,
      SEQ_EXECUTE,
    };

    gfl2::heap::HeapBase *m_heap; //! ヒープ

    Effect::EffectManager *m_pEffectManager; //!エフェクトマネージャへのポインタ
    Camera::CameraManager *m_pCameraManager; //!カメラマネージャへのポインタ
    BaseCollisionScene    *m_pCollisionSceneContainer; //! コリジョンシーンへのポインタ
    
    WeatherBase *m_pWeather; //! 稼動させる天候へのポインタ
    WeatherBase *m_pFadeoutWeather; //! 終了させる天候へのポインタ
    Rain * m_pRain; //!雨系統モジュールへのポインタ
    Snow * m_pSnow; //!雪系統モジュールへのポインタ
    SandStorm * m_pSandStorm; //!砂嵐系統モジュールへのポインタ
    
    SEQ m_seq; //! シーケンスコントローラ

    WEATHER_TYPE m_ReserveWeatherType; //!予約してある天候
  };

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)
#endif // __WEATHER_MANAGER_H_INCLUDED__