
//=============================================================================
/**
 * @file   CameraInterpolate.h
 * @brief  補間関数・クラスヘッダ
 * @author 池田 哲平
 * @date   2012.05.31
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#if !defined( __CAMERA_INTERPOLATE_H__ )
#define __CAMERA_INTERPOLATE_H__

#include <math/include/gfl2_math.h>
#include "CameraUtility.h"

namespace System { namespace Camera { namespace field
{
  //-----------------------------------------------------------------------------
  /**
   * @brief 比率補間値を取得する
   *
   * @param src    始点
   * @param dest　 終点
   * @param fRatio 比率
   *
   * @return 補間値
   */
  //-----------------------------------------------------------------------------
  template<typename T> T GetRatioInterpolateValue(const T& src, const T& dest, const float fRatio)
  // { return (1.0f - fRatio)*src + fRatio*dest; }
  { return src + fRatio*(dest - src); }

  //-----------------------------------------------------------------------------
  /**
   * @brief 比率補間値を取得する(0 <= fRatio <= 1の範囲にクリップする)
   *
   * @param src    始点
   * @param dest　 終点
   * @param fRatio 比率
   *
   * @return 補間値
   */
  //-----------------------------------------------------------------------------
  template<typename T> T GetRatioZeroOneInterpolateValue(const T& src, const T& dest, const float fRatio)
  {
    if     ( fRatio <= 0.0f ) return src;
    else if( fRatio >= 1.0f ) return dest;
    else                      return GetRatioInterpolateValue(src, dest, fRatio);
  }

  /// 比率補間ラジアンを取得する(精度を下げないために補間ラジアンに対しても-π～+π変換を行う)
  inline float GetRatioInterpolateRadian(const float src, const float dest, const float fRatio)
  { return CUtil::AddRadian(src, fRatio*CUtil::AddRadian(dest, -src)); }

  /// 比率補間ラジアンを取得する(0 <= fRatio <= 1の範囲にクリップする)
  inline float GetRatioZeroOneInterpolateRadian(const float src, const float dest, const float fRatio)
  {
    if     ( fRatio <= 0.0f ) return CUtil::ConvertFromMinusPiToPlusPi(src);
    else if( fRatio >= 1.0f ) return CUtil::ConvertFromMinusPiToPlusPi(dest);
    else                      return GetRatioInterpolateRadian(src, dest, fRatio);
  }

  //-----------------------------------------------------------------------------
  /**
   * @brief 比率補間値を取得する
   *
   * @param base   基点
   * @param range  範囲
   * @param fRatio 比率
   *
   * @return 補間値
   */
  //-----------------------------------------------------------------------------
  template<typename T> T GetRatioInterpolateValueByBaseAndRange(const T& base, const T& range, const float fRatio)
  { return base + (fRatio - 0.5f)*range; }

  //-----------------------------------------------------------------------------
  /**
   * @brief 中央値を取得する
   *
   * @param value1 値１
   * @param value2 値２
   *
   * @return 中央値
   */
  //-----------------------------------------------------------------------------
  template<typename T> T GetCenterValue(const T& value1, const T& value2) { return 0.5f*(value1 + value2); }

  //-----------------------------------------------------------------------------
  /**
   * @brief 補間比率値を取得する(返す値は0～1)
   *
   * @param fValue      判定値
   * @param fZeroBorder 比率０境界
   * @param fOneBorder  比率１境界
   *
   * @return 補間比率値
   */
  //-----------------------------------------------------------------------------
  extern float GetInterpolateRatioValue(const float fValue, const float fZeroBorder, const float fOneBorder);

  //-----------------------------------------------------------------------------
  /**
   * @brief 補間比率値を取得する(返す値は0～1のクリップなし)
   *
   * @param fValue      判定値
   * @param fZeroBorder 比率０境界
   * @param fOneBorder  比率１境界
   *
   * @return 補間比率値
   */
  //-----------------------------------------------------------------------------
  inline float GetInterpolateRatioValueNoBorderClip(const float fValue, const float fZeroValue, const float fOneValue)
  { return (fValue - fZeroValue) / (fOneValue - fZeroValue); }

  extern const float INTERPOLATE_VEL_MAX_INFINITY;
  //-----------------------------------------------------------------------------
  /**
   * @brief ばね補間による補間移動値を取得する
   *
   * @param fSpringCoe ばね係数
   * @param fVelMax    最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
   * @param fDiffValue 出力値 - 入力値
   * @param fDiffTime  経過時間
   *
   * @return 補間移動値
   */
  //-----------------------------------------------------------------------------
  extern float GetSpringInterpolateMove(const float fSpringCoe, const float fVelMax, const float fDiffValue, const float fDiffTime);

  //-----------------------------------------------------------------------------
  /**
   * @brief ばね補間による補間移動値を取得する
   *
   * @param fSpringCoe    ばね係数
   * @param fVelMax       最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
   * @param fSrcValue     入力値
   * @param fDestValue    出力値
   * @param fDiffTime     経過時間
   * @param bIsRadiInterp ラジアン(回転)補間かどうか
   *
   * @return 補間移動値
   */
  //-----------------------------------------------------------------------------
  inline float GetSpringInterpolateMove(const float fSpringCoe, const float fVelMax, const float fSrcValue, const float fDestValue, const float fDiffTime, const bool bIsRadiInterp = false)
  {
    float fDiffValue = 0.0f;
    if( bIsRadiInterp ) fDiffValue = CUtil::AddRadian(fDestValue, -fSrcValue);
    else                fDiffValue = fDestValue - fSrcValue;
    return GetSpringInterpolateMove(fSpringCoe, fVelMax, fDiffValue, fDiffTime);
  }

  //-----------------------------------------------------------------------------
  /**
   * @brief ばね補間による補間値を取得する
   *
   * @param fSpringCoe    ばね係数
   * @param fVelMax       最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
   * @param fSrcValue     入力値
   * @param fDestValue    出力値
   * @param fDiffTime     経過時間
   * @param bIsRadiInterp ラジアン(回転)補間かどうか
   *
   * @return 補間値
   */
  //-----------------------------------------------------------------------------
  inline float GetSpringInterpolateValue(const float fSpringCoe, const float fVelMax, const float fSrcValue, const float fDestValue, const float fDiffTime, const bool bIsRadiInterp = false)
  {
    if( bIsRadiInterp == true )
    {
      const float fDiffValue = CUtil::AddRadian(fDestValue, -fSrcValue);
      return CUtil::AddRadian(fSrcValue, GetSpringInterpolateMove(fSpringCoe, fVelMax, fDiffValue, fDiffTime));
    }
    else
      return fSrcValue + GetSpringInterpolateMove(fSpringCoe, fVelMax, fDestValue - fSrcValue, fDiffTime);
  }

  //-----------------------------------------------------------------------------
  /**
   * @brief ばね補間制御を行うクラス
   */
  //-----------------------------------------------------------------------------
  class CSpringInterpolate
  {
  private:
    float m_fSpringCoe;    ///< ばね係数
    float m_fVelMax;       ///< 最大速度
    bool  m_bIsRadiInterp; ///< ラジアン(回転)補間かどうか
    float m_fEpsilon;      ///< 希望値到達判定イプシロン

    float m_fValue; ///< 補間値

  public:
    CSpringInterpolate(); ///< コンストラクタ

    inline float Reset(const float fResetValue) { m_fValue = fResetValue; return m_fValue; } ///< リセット
    inline float Begin(const float fBeginValue) { return Reset(fBeginValue); } ///< 開始
    float Update(const float fHopeValue, const float fDiffTime); ///< フレーム処理(戻り値は補間値)

    inline float GetValue()   const { return m_fValue;   } ///< 補間値を取得する
    inline float GetEpsilon() const { return m_fEpsilon; } ///< イプシロンを取得する

    //-----------------------------------------------------------------------------
    /**
     * @brief 制御パラメータを設定する
     *
     * @param fSpringCoe ばね係数
     * @param fVelMax    最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
     */
    //-----------------------------------------------------------------------------
    inline void SetParam(const float fSpringCoe, const float fVelMax = INTERPOLATE_VEL_MAX_INFINITY) { m_fSpringCoe = fSpringCoe, m_fVelMax = fVelMax; }
    inline void SetRadianInterpolateFlag(const bool bIsRadiInterp) { m_bIsRadiInterp = bIsRadiInterp; } ///< ラジアン(回転)補間フラグを設定する
    inline void SetEpsilon(const float fEpsilon) { m_fEpsilon = fEpsilon; } ///< イプシロンを設定する

  private:
    float GetDiffValue(const float fValue1, const float fValue2) const ; ///< fValue1 - fValue2を取得する
    float GetDiffValue(const float fValue) const { return GetDiffValue(fValue, m_fValue); } ///< fValue - m_fValue
  
  public:
    friend class CDoubleSpringInterpolate;
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief ばね補間制御を行うクラス(３次元版)
   */
  //-----------------------------------------------------------------------------
  class CSpringInterpolate3D
  {
  private:
    float m_fSpringCoe;    ///< ばね係数
    float m_fVelMax;       ///< 最大速度
    bool  m_bIsRadiInterp; ///< ラジアン(回転)補間かどうか
    float m_fEpsilon;      ///< 希望値到達判定イプシロン

    gfl2::math::VEC3 m_inValue; ///< 補間値

  public:
    CSpringInterpolate3D(); ///< コンストラクタ

    inline const gfl2::math::VEC3& Reset(const gfl2::math::VEC3& inResetValue) { m_inValue = inResetValue; return m_inValue; } ///< リセット
    inline const gfl2::math::VEC3& Begin(const gfl2::math::VEC3& inBeginValue) { return Reset(inBeginValue); } ///< 開始
    const gfl2::math::VEC3& Update(const gfl2::math::VEC3& inHopeValue, const float fDiffTime); ///< フレーム処理(戻り値は補間値)

    inline const gfl2::math::VEC3& GetValue()   const { return m_inValue;  } ///< 補間値を取得する
    inline float                  GetEpsilon() const { return m_fEpsilon; } ///< イプシロンを取得する

    //-----------------------------------------------------------------------------
    /**
     * @brief 制御パラメータを設定する
     *
     * @param fSpringCoe ばね係数
     * @param fVelMax    最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
     */
    //-----------------------------------------------------------------------------
    inline void SetParam(const float fSpringCoe, const float fVelMax = INTERPOLATE_VEL_MAX_INFINITY) { m_fSpringCoe = fSpringCoe, m_fVelMax = fVelMax; }
    inline void SetRadianInterpolateFlag(const bool bIsRadiInterp) { m_bIsRadiInterp = bIsRadiInterp; } ///< ラジアン(回転)補間フラグを設定する
    inline void SetEpsilon(const float fEpsilon) { m_fEpsilon = fEpsilon; } ///< イプシロンを設定する

  private:
    void GetDiffValue(gfl2::math::VEC3* pDiffValue, const gfl2::math::VEC3& inValue1, const gfl2::math::VEC3& inValue2) const ; ///< inValue1 - inValue2を取得する
    bool IsReachHopeValueCommon(const gfl2::math::VEC3& inHopeValue, const float fEpsilonSqu) const ; ///< 希望値に到達したか確認する

  public:
    friend class CDoubleSpringInterpolate3D;
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief ばね補間制御を行うクラス(正規直交行列3x3版)
   */
  //-----------------------------------------------------------------------------
  class CSpringInterpolateMat33
  {
  private:
    float m_fSpringCoe; ///< ばね係数
    gfl2::math::MTX33 m_inValue; ///< 補間値

  public:
    CSpringInterpolateMat33(); ///< コンストラクタ

    inline const gfl2::math::MTX33& Reset(const gfl2::math::MTX33& inResetValue) { gfl2::math::MTX33Copy(&m_inValue, &inResetValue); return m_inValue; } ///< リセット
    inline const gfl2::math::MTX33& Begin(const gfl2::math::MTX33& inBeginValue) { return Reset(inBeginValue); } ///< 開始
    const gfl2::math::MTX33& Update(const gfl2::math::MTX33& inHopeValue, const float fDiffTime); ///< フレーム処理(戻り値は補間値)

    inline const gfl2::math::MTX33& GetValue() const { return m_inValue;  } ///< 補間値を取得する
    inline void SetParam(const float fSpringCoe) { m_fSpringCoe = fSpringCoe; } ///< 制御パラメータを設定する

  private:
    bool IsReachHopeValue(const gfl2::math::MTX33& inHopeValue) const ; ///< 希望値に到達したか確認する
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief ２重ばね補間制御を行うクラス
   * @brief ２重ばねにすると補間速度の減速だけでなく加速も表現できる
   */
  //-----------------------------------------------------------------------------
  class CDoubleSpringInterpolate
  {
  private:
    CSpringInterpolate m_inHopeInterp; ///< 希望値の補間
    CSpringInterpolate m_inCurInterp;  ///< 現在値の補間

  public:
    CDoubleSpringInterpolate() { SetParam(2.0f); } ///< コンストラクタ

    float Reset(const float fResetValue); ///< リセット
    inline float Begin(const float fBeginValue) { return Reset(fBeginValue); } ///< 開始
    float Update(const float fHopeValue, const float fDiffTime); ///< フレーム処理(戻り値は補間値)
    
    void OutUpdate(const float fValue); ///< 外部更新

    inline float GetValue()     const { return m_inCurInterp .GetValue(); } ///< 補間値を取得する
    inline float GetHopeValue() const { return m_inHopeInterp.GetValue(); } ///< 希望補間値を取得する

    //-----------------------------------------------------------------------------
    /**
     * @brief 制御パラメータを設定する
     *
     * @param fSpringCoe ばね係数
     * @param fVelMax    最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
     */
    //-----------------------------------------------------------------------------
    void SetParam(const float fSpringCoe, const float fVelMax = INTERPOLATE_VEL_MAX_INFINITY);

    //-----------------------------------------------------------------------------
    /**
     * @brief 制御パラメータを設定する(拡張版)
     *
     * @param fHopeSpringCoe 希望値ばね係数
     * @param fCurSpringCoe  現在値ばね係数
     * @param fHopeVelMax    希望値最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
     * @param fCurVelMax     現在値最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
     */
    //-----------------------------------------------------------------------------
    void SetParamExtend(const float fHopeSpringCoe, const float fCurSpringCoe, const float fHopeVelMax = INTERPOLATE_VEL_MAX_INFINITY, const float fCurVelMax = INTERPOLATE_VEL_MAX_INFINITY);

    /// ラジアン(回転)補間フラグを設定する
    inline void SetRadianInterpolateFlag(const bool bIsRadiInterp) { m_inHopeInterp.SetRadianInterpolateFlag(bIsRadiInterp); m_inCurInterp.SetRadianInterpolateFlag(bIsRadiInterp); }
    inline void SetEpsilon(const float fEpsilon) { m_inHopeInterp.SetEpsilon(fEpsilon); m_inCurInterp.SetEpsilon(fEpsilon); } ///< イプシロンを設定する

    bool IsReachHopeValue(const float fHopeValue) const ; ///< 入力希望値に到達したか確認する
    bool IsReachHopeValue(const float fHopeValue, const float fReachEpsilon) const ; ///< 入力希望値に到達したか確認する

    void ResetHope(const float fResetValue) { m_inHopeInterp.Reset(fResetValue); } ///< 希望補間値をリセットする
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief ２重ばね補間を行うクラス(３次元版)
   * @brief ２重ばねにすると補間速度の減速だけでなく加速も表現できる
   */
  //-----------------------------------------------------------------------------
  class CDoubleSpringInterpolate3D
  {
  private:
    CSpringInterpolate3D m_inHopeInterp; ///< 希望値の補間
    CSpringInterpolate3D m_inCurInterp;  ///< 現在値の補間

  public:
    CDoubleSpringInterpolate3D() { SetParam(2.0f); } ///< コンストラクタ

    const gfl2::math::VEC3& Reset(const gfl2::math::VEC3& inResetValue); ///< リセット
    inline const gfl2::math::VEC3& Begin(const gfl2::math::VEC3& inBeginValue) { return Reset(inBeginValue); } ///< 開始
    const gfl2::math::VEC3& Update(const gfl2::math::VEC3& inHopeValue, const float fDiffTime); ///< フレーム処理(戻り値は補間値)
    
    void OutUpdate(const gfl2::math::VEC3& inValue); ///< 外部更新

    inline const gfl2::math::VEC3& GetValue()     const { return m_inCurInterp .GetValue(); } ///< 補間値を取得する
    inline const gfl2::math::VEC3& GetHopeValue() const { return m_inHopeInterp.GetValue(); } ///< 希望補間値を取得する

    //-----------------------------------------------------------------------------
    /**
     * @brief 制御パラメータを設定する
     *
     * @param fSpringCoe ばね係数
     * @param fVelMax    最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
     */
    //-----------------------------------------------------------------------------
    void SetParam(const float fSpringCoe, const float fVelMax = INTERPOLATE_VEL_MAX_INFINITY);

    //-----------------------------------------------------------------------------
    /**
     * @brief 制御パラメータを設定する(拡張版)
     *
     * @param fHopeSpringCoe 希望値のばね係数
     * @param fCurSpringCoe  現在値のばね係数
     * @param fHopeVelMax    希望値の最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
     * @param fCurVelMax     現在値最大速度(INTERPOLATE_VEL_MAX_INFINITYを設定するとリミットなし)
     */
    //-----------------------------------------------------------------------------
    void SetParamExtend(const float fHopeSpringCoe, const float fCurSpringCoe, const float fHopeVelMax = INTERPOLATE_VEL_MAX_INFINITY, const float fCurVelMax = INTERPOLATE_VEL_MAX_INFINITY);

    /// ラジアン(回転)補間フラグを設定する
    inline void SetRadianInterpolateFlag(const bool bIsRadiInterp) { m_inHopeInterp.SetRadianInterpolateFlag(bIsRadiInterp); m_inCurInterp.SetRadianInterpolateFlag(bIsRadiInterp); }
    inline void SetEpsilon(const float fEpsilon) { m_inHopeInterp.SetEpsilon(fEpsilon); m_inCurInterp.SetEpsilon(fEpsilon); } ///< イプシロンを設定する
    
    bool IsReachHopeValue(const gfl2::math::VEC3& inHopeValue) const ; ///< 入力希望値に到達したか確認する
    bool IsReachHopeValue(const gfl2::math::VEC3& inHopeValue, const float fEpsilon) const ; ///< 入力希望値に到達したか確認する
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief 時間による比率の補間を行うクラス
   */
  //-----------------------------------------------------------------------------
  class CTimeRatioInterpolate
  {
  protected:
    float m_fOneRatioTime; ///< 比率が１になる時間

    float m_fRatio; ///< 比率
    float m_fTimer; ///< タイマ

  public:
    CTimeRatioInterpolate(); ///< コンストラクタ

    float Reset(const float fResetRatio = 0.0f); ///< リセット
    inline float Begin(const float fBeginRatio = 0.0f) { return Reset(fBeginRatio); } ///< 開始

    //-----------------------------------------------------------------------------
    /**
     * @brief フレーム処理
     *
     * @param fDiffTime  経過時間
     * @param bIsRatioUp true:比率上昇 false:比率下降
     *
  　 * @return 比率
    */
    //-----------------------------------------------------------------------------
    float Update(const float fDiffTime, const bool bIsRatioUp = true);

    /// 比率が１になる時間を設定する
    void SetParam(const float fOneRatioTime)
    {
      if( fOneRatioTime > 0.0f ) { m_fOneRatioTime = fOneRatioTime; Reset(m_fRatio); }
      else                       { GFL_ASSERT_ALWAYS; }
    }
    void SetParamMinClip(const float fOneRatioTime) { m_fOneRatioTime = fOneRatioTime; CUtil::MinClip(&m_fOneRatioTime, 0.01f); Reset(m_fRatio); }

    float GetRatio()    const { return m_fRatio; } ///< 比率を取得する
    float GetCosRatio() const { return CUtil::ConvertLinearToCosCurveRatioValidRange(m_fRatio); } ///< 比率をコサインカーブに変換して取得する

    bool IsRatioOne()  const { return (m_fRatio >= 1.0f); } ///< 比率が１か判定する
    bool IsRatioZero() const { return (m_fRatio <= 0.0f); } ///< 比率が０か判定する
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief 時間による比率の補間を行うクラスの拡張版
   */
  //-----------------------------------------------------------------------------
  class CTimeRatioInterpolateEx : public CTimeRatioInterpolate
  {
  private:
    float m_fZeroRatioTime; ///< 比率が０になる時間
    float m_fMaxTime; ///< 時間の最大

  public:
    CTimeRatioInterpolateEx(); ///< コンストラクタ
    float Reset(const float fResetRatio = 0.0f); ///< リセット
    inline float Begin(const float fBeginRatio = 0.0f) { return Reset(fBeginRatio); } ///< 開始
    float Update(const float fDiffTime, const bool bIsRatioUp = true); ///< フレーム処理
    void SetParam(const float fZeroRatioTime, const float fOneRatioTime, const float fMaxTime); ///< 制御パラメータを設定する
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief 線形の波補間を行うクラス
   */
  //-----------------------------------------------------------------------------
  class CLinearWaveInterpolate
  {
  private:
    float m_fStepCoe; ///< ステップ係数

    float m_fValue; ///< 補間値(０～１)
    bool  m_bIsAdd; ///< 補間値増加中フラグ

  public:
    CLinearWaveInterpolate(); ///< コンストラクタ

    float Reset(const float fResetValue = 0.0f, const bool bIsAdd = true) { m_fValue = fResetValue; m_bIsAdd = bIsAdd; return m_fValue; } ///< リセット
    float Begin(const float fBeginValue = 0.0f, const bool bIsAdd = true) { return Reset(fBeginValue, bIsAdd); } ///< 開始
    float Update(const float fDiffTime); ///< フレーム処理

    /// ステップ係数を設定する
    void SetParam(const float fStepCoe) { m_fStepCoe = fStepCoe; }

    float GetValue() const { return m_fValue; } ///< 補間値を取得する
  };

} // namespace field
} } // namespace System namespace Camera

#endif // __CAMERA_INTERPOLATE_H__

/*  EOF  */





