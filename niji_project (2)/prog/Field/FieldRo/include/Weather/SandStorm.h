//======================================================================
/**
 * @file SandStorm.h
 * @date 2015/12/09 20:27:15
 * @author saito_nozomu
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __SAND_STORM_H_INCLUDED__
#define __SAND_STORM_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Field/FieldRo/include/Weather/WeatherBase.h"

GFL_NAMESPACE_BEGIN(Field)

GFL_NAMESPACE_BEGIN(Camera)
  class CameraManager;
GFL_NAMESPACE_END(Camera)

GFL_NAMESPACE_BEGIN(Effect)
class EffectManager;
class EffectSandStorm;
GFL_NAMESPACE_END(Effect)

GFL_NAMESPACE_BEGIN(weather)

class BillBoardFade;

/**
 * @class SandStorm
 * @brief 天候砂嵐を制御するクラス
 */
class SandStorm : public WeatherBase
{
  GFL_FORBID_COPY_AND_ASSIGN(SandStorm);
  public:
    struct InitParam
    {
     Effect::EffectManager *pEffectManager;
     Camera::CameraManager *pCameraManager;
    };
  
  public:
    SandStorm(gfl2::heap::HeapBase* pHeap);
    virtual ~SandStorm();
    void Initialize(InitParam &param);

    /**
     * @brief 天候の変更要請
     * @param[in] type 天候種類
     */
    void ChangeWeatherRequestCore(WEATHER_TYPE type);
    
  private:
    bool SetupFunc(void);
    void StartFunc(void);
    void MainFunc(void);
    bool FadeOutFunc(void);
    
    /**
     * @brief ＳＥ再生
     */
    void StartSE(void);

    /**
     * @brief 天候のフェードアウト
     * @param[in] type 天候種類
     */
    void FadeoutWeatherRequest(WEATHER_TYPE type);

    /**
     * @brief タイプ別フェードアウト終了チェック
     * @param[in] type
     * @return bool ture でフェードアウト終了
     */
    bool IsFinishedFadeOut(WEATHER_TYPE type);
  
  private:
    gfl2::heap::HeapBase* m_pHeap; //! ヒープ
    Effect::EffectManager *m_pEffectManager; //! エフェクトマネージャポインタ
    Camera::CameraManager *m_pCameraManager; //! カメラマネージャへのポインタ

    Effect::EffectSandStorm *m_pSandStormEff;//! 砂嵐エフェクトへのポインタ

    BillBoardFade*      m_pSandStormFade; //!砂嵐ビルボードフェードコントロールクラスへのポインタ
    
};

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)
#endif // __SAND_STORM_H_INCLUDED__