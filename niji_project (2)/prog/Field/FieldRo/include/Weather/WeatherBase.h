//======================================================================
/**
 * @file WeatherBase.h
 * @date 2015/10/20 16:33:54
 * @author saito_nozomu
 * @brief 天候クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __WEATHER_BASE_H_INCLUDED__
#define __WEATHER_BASE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Field/FieldRo/include/Weather/WeatherTypeDef.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

/**
 * @def
 * ＳＥの鳴り止むまでのフレーム
 */
#define SE_FADE_FRAME (15)

  /**
   * @class MyClass
   * @brief クラスの説明
   */
  class WeatherBase
  {
  public:
    WeatherBase();
    virtual ~WeatherBase();

    /**
     * @fn
     * ここに関数の説明を書く
     * @brief 要約説明
     * @param[in] var1 var1の説明
     * @param[out] var2 var2の説明
     * @param[in,out] var3 var3の説明
     * @return 戻り値の説明
     * @sa 参照すべき関数を書けばリンクが貼れる
     * @detail 詳細な説明
     */
    void Update(void);

    void SetupRequest(WEATHER_TYPE type);

    bool IsFinishedSetup(void){return m_SetupFinish;}

    void StartRequest(void);

    void StopRequest(void);

    bool IsStoped(void){return m_Stop;}

    bool IsFinishedFadeout(void){return m_Fadeout;}

    /**
     * @brief 天候の変更要請
     * @param[in] type 天候種類
     */
    void ChangeWeatherRequest(WEATHER_TYPE type);

    /**
     * @brief 天候の変更要請
     * @param[in] type 天候種類
     */
    virtual void ChangeWeatherRequestCore(WEATHER_TYPE type){}

    /**
     * @brief 天候タイプ取得
     * @return[out] WEATHER_TYPE 天候種類
     */
    WEATHER_TYPE GetType(void){return m_Type;}

  private:
    virtual bool SetupFunc(void){return true;}

    virtual void StartFunc(void){}
    
    virtual void MainFunc(void){}

    virtual void StopFunc(void){}

    virtual bool FadeOutFunc(void){return true;}

    virtual void StartSE(void){}

  protected:
    enum SEQ
    {
      SEQ_IDLE,
      SEQ_SETUP,
      SEQ_MAIN,
      SEQ_FADEOUT,
    };

    //! シーケンサ
    SEQ m_Seq;
    bool m_SetupFinish;
    bool m_Stop;
    bool m_Fadeout;
    int m_FadeOutSeq; //!フェードアウトシーケンサ

    WEATHER_TYPE m_Type;
    bool m_StartedSE;
  };

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)
#endif // __WEATHER_BASE_H_INCLUDED__