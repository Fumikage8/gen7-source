//======================================================================
/**
 * @file Rain.h
 * @date 2015/10/20 17:15:57
 * @author saito_nozomu
 * @brief 天候　雨
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __RAIN_H_INCLUDED__
#define __RAIN_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Field/FieldRo/include/Weather/WeatherBase.h"

class BaseCollisionScene;

GFL_NAMESPACE_BEGIN(Field)

GFL_NAMESPACE_BEGIN(Camera)
  class CameraManager;
GFL_NAMESPACE_END(Camera)

GFL_NAMESPACE_BEGIN(Effect)
class EffectManager;
class EffectRain;
GFL_NAMESPACE_END(Effect)

GFL_NAMESPACE_BEGIN(weather)

class Thunder;
class BillBoardFade;

#define RAIN_EFF_TBL_MAX (8)
/**
 * @class Rain
 * @brief 天候雨を制御するクラス
 */
class Rain : public WeatherBase
{
  GFL_FORBID_COPY_AND_ASSIGN(Rain);
  public:
    struct InitParam
    {
     Effect::EffectManager *pEffectManager;
     Camera::CameraManager *pCameraManager;
     BaseCollisionScene    *pCollisionSceneContainer;
    };
  
  public:
    Rain(gfl2::heap::HeapBase* pHeap);
    virtual ~Rain();
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
    void StopFunc(void);
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
    bool m_generate; //! 生成フラグ
    int m_generate_margine; //! 生成間隔

    Effect::EffectRain *m_pEffectTable[RAIN_EFF_TBL_MAX]; //!エフェクトテーブル

    Effect::EffectRain *m_pRainEff;//! 雨エフェクト（空エミッター用）へのポインタ
    BaseCollisionScene*         m_pCollisionSceneContainer;                 //! 高さ取得用レイチェックで対象とするシーン

    Thunder*            m_pThunder; //! 雷コントロールクラスへのポインタ
    BillBoardFade*      m_pRainFade; //!雨ビルボードフェードコントロールクラスへのポインタ
    BillBoardFade*      m_pMistFade; //!霧ビルボードフェードコントロールクラスへのポインタ
    
};

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)
#endif // __RAIN_H_INCLUDED__