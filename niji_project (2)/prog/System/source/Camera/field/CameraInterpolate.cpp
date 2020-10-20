
//=============================================================================
/**
 * @file   CameraInterpolate.cpp
 * @brief  補間関数・クラス
 * @author 池田 哲平
 * @date   2012.05.31
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include "CameraInterpolate.h"

namespace System { namespace Camera { namespace field
{
  //-----------------------------------------------------------------------------
  // 補間比率値を取得する(返す値は0～1)
  //-----------------------------------------------------------------------------
  float GetInterpolateRatioValue(const float fValue, const float fZeroBorder, const float fOneBorder)
  {
#if PM_DEBUG
    if( fZeroBorder > fOneBorder ) GFL_PRINT("壱境界(%.2f)より零境界(%.2f)の方が値が大きいです \n", fOneBorder, fZeroBorder);
#endif

    if     ( fValue <= fZeroBorder ) return 0.0f;
    else if( fValue >= fOneBorder  ) return 1.0f;
    else                             return GetInterpolateRatioValueNoBorderClip(fValue, fZeroBorder, fOneBorder);
  }

  const float INTERPOLATE_VEL_MAX_INFINITY = -1.0f;

  //-----------------------------------------------------------------------------
  // ばね補間による補間移動値を取得する
  //-----------------------------------------------------------------------------
  float GetSpringInterpolateMove(const float fSpringCoe, const float fVelMax, const float fDiffMove, const float fDiffTime)
  {
    const float fDiffMoveAbs = gfl2::math::FAbs(fDiffMove);

    float fMove = fSpringCoe*fDiffMove*fDiffTime;
    float fMoveAbs = gfl2::math::FAbs(fMove);

    // 追い越しチェック
    if( fMoveAbs > fDiffMoveAbs )
    {
      if( fMove >= 0.0f ) fMove =  fDiffMoveAbs;
      else                fMove = -fDiffMoveAbs;
      fMoveAbs = fDiffMoveAbs;
    }

    // 最大速度リミット
    if( fVelMax > 0.0f )
    {
      const float fMoveMax = fVelMax*fDiffTime;
      if( fMoveAbs > fMoveMax )
      {
        if( fMove >= 0.0f ) fMove =  fMoveMax;
        else                fMove = -fMoveMax;
      }
    }

    return fMove;
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CSpringInterpolate

  //-----------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------
  CSpringInterpolate::CSpringInterpolate() :
  m_bIsRadiInterp(false) ,
  m_fEpsilon(NEARLY_EQUAL_EPSILON_MOVE) ,
  m_fValue(0.0f) 
  {
    SetParam(1.0f);
  }
  
  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  float CSpringInterpolate::Update(const float fHopeValue, const float fDiffTime)
  {
    // 希望値と近似の時は処理を行わない
    const float fDiffValue = GetDiffValue(fHopeValue);
    if( CUtil::IsNearlyEqualZero(fDiffValue, m_fEpsilon) == false )
    {
      m_fValue += GetSpringInterpolateMove(m_fSpringCoe, m_fVelMax, fDiffValue, fDiffTime);
      if( m_bIsRadiInterp ) m_fValue = CUtil::ConvertFromMinusPiToPlusPi(m_fValue); // 値が大きくなると精度が落ちるので
    }

    return m_fValue;
  }
  
  //-----------------------------------------------------------------------------
  // fValue1 - fValue2 を取得する
  //-----------------------------------------------------------------------------
  float CSpringInterpolate::GetDiffValue(const float fValue1, const float fValue2) const
  {
    if( m_bIsRadiInterp ) return CUtil::AddRadian(fValue1, -fValue2);
    else                  return fValue1 - fValue2;
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CSpringInterpolate3D

  //-----------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------
  CSpringInterpolate3D::CSpringInterpolate3D() :
  m_bIsRadiInterp(false) ,
  m_fEpsilon(NEARLY_EQUAL_EPSILON_MOVE) ,
  m_inValue(System::Camera::internal::VEC3_ZERO)
  {
    SetParam(1.0f);
  }

  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  const gfl2::math::VEC3& CSpringInterpolate3D::Update(const gfl2::math::VEC3& inHopeValue, const float fDiffTime)
  {
    gfl2::math::VEC3 inDiff; GetDiffValue(&inDiff, inHopeValue, m_inValue);
    const float fDistSqu = gfl2::math::VEC3SquareLen(&inDiff);
    if( fDistSqu > m_fEpsilon*m_fEpsilon )
    {
      const float fDist = gfl2::math::FSqrt(fDistSqu);
      const float fMove = GetSpringInterpolateMove(m_fSpringCoe, m_fVelMax, fDist, fDiffTime);
      m_inValue += (fMove/fDist)*inDiff;
      if( m_bIsRadiInterp == true )
      {
        m_inValue.x = CUtil::ConvertFromMinusPiToPlusPi(m_inValue.x);
        m_inValue.y = CUtil::ConvertFromMinusPiToPlusPi(m_inValue.y);
        m_inValue.z = CUtil::ConvertFromMinusPiToPlusPi(m_inValue.z);
      }
    }

    return m_inValue;
  }

  //-----------------------------------------------------------------------------
  // inValue1 - inValue2 を取得する
  //-----------------------------------------------------------------------------
  void CSpringInterpolate3D::GetDiffValue(gfl2::math::VEC3* pDiffValue, const gfl2::math::VEC3& inValue1, const gfl2::math::VEC3& inValue2) const
  {
    if( m_bIsRadiInterp == true )
    {
      pDiffValue->x = CUtil::AddRadian(inValue1.x, -inValue2.x);
      pDiffValue->y = CUtil::AddRadian(inValue1.y, -inValue2.y);
      pDiffValue->z = CUtil::AddRadian(inValue1.z, -inValue2.z);
    }
    else
      (*pDiffValue) = inValue1 - inValue2;
  }

  //-----------------------------------------------------------------------------
  // 希望値に到達したか確認する
  //-----------------------------------------------------------------------------
  bool CSpringInterpolate3D::IsReachHopeValueCommon(const gfl2::math::VEC3& inHopeValue, const float fEpsilonSqu) const
  {
    gfl2::math::VEC3 inDiff; GetDiffValue(&inDiff, inHopeValue, m_inValue);
    return (gfl2::math::VEC3SquareLen(&inDiff) <= fEpsilonSqu);
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CSpringInterpolateMat33

  //-----------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------
  CSpringInterpolateMat33::CSpringInterpolateMat33() :
  m_inValue(System::Camera::internal::MTX33_IDENTITY)
  {
    SetParam(1.0f);
  }

  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  const gfl2::math::MTX33& CSpringInterpolateMat33::Update(const gfl2::math::MTX33& inHopeValue, const float fDiffTime)
  {
    if( IsReachHopeValue(inHopeValue) == false )
    {
      float fRatio = m_fSpringCoe*fDiffTime; CUtil::MaxClip(&fRatio, 1.0f);
      gfl2::math::MTX33Slerp(&m_inValue, m_inValue, inHopeValue, fRatio); // inHopeValueが正規化されているので、多分計算誤差による再正規化を行わなくて問題ない
    }

    return m_inValue;
  }

  //-----------------------------------------------------------------------------
  // 希望値に到達したか確認する
  //-----------------------------------------------------------------------------
  bool CSpringInterpolateMat33::IsReachHopeValue(const gfl2::math::MTX33& inHopeValue) const
  {
    // ２軸の角度差が近似であれば同値と判断してよい
    gfl2::math::VEC3 inCur, inHope;
    gfl2::math::MTX33GetAxisZ(&inCur,  m_inValue); // 弾けそうな軸からチェック
    gfl2::math::MTX33GetAxisZ(&inHope, inHopeValue);
    if( CUtil::GetCos(inCur, inHope) >= NEARLY_EQUAL_ONE_EPSILON_COS )
    {
      gfl2::math::MTX33GetAxisY(&inCur,  m_inValue);
      gfl2::math::MTX33GetAxisY(&inHope, inHopeValue);
      if( CUtil::GetCos(inCur, inHope) >= NEARLY_EQUAL_ONE_EPSILON_COS ) return true;
    }

    return false;
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CDoubleSpringInterpolate

static const float REACH_MARGIN = 3.0f;

  //-----------------------------------------------------------------------------
  // リセット
  //-----------------------------------------------------------------------------
  float CDoubleSpringInterpolate::Reset(const float fResetValue)
  {
    m_inHopeInterp.Reset(fResetValue);
    m_inCurInterp .Reset(fResetValue);
    return m_inCurInterp.GetValue();
  }

  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  float CDoubleSpringInterpolate::Update(const float fHopeValue, const float fDiffTime)
  {
    // まず希望値を補間してから現在値を補間。これによりブレーキ効果だけでなくアクセル効果も表現することができる。
    m_inHopeInterp.Update(fHopeValue, fDiffTime);
    m_inCurInterp .Update(m_inHopeInterp.GetValue(), fDiffTime);
    return m_inCurInterp.GetValue();
  }
  
  //-----------------------------------------------------------------------------
  // 外部更新
  //-----------------------------------------------------------------------------
  void CDoubleSpringInterpolate::OutUpdate(const float fValue)
  {
    const float fDiff = m_inCurInterp.GetDiffValue(fValue);
    m_inHopeInterp.Reset(m_inHopeInterp.GetDiffValue(m_inHopeInterp.GetValue(), -fDiff));
    m_inCurInterp .Reset(fValue);
  }

  //-----------------------------------------------------------------------------
  // 制御パラメータを設定する
  //-----------------------------------------------------------------------------
  void CDoubleSpringInterpolate::SetParam(const float fSpringCoe, const float fVelMax)
  {
    m_inHopeInterp.SetParam(fSpringCoe     , fVelMax);
    m_inCurInterp .SetParam(fSpringCoe*0.5f, INTERPOLATE_VEL_MAX_INFINITY);
  }

  //-----------------------------------------------------------------------------
  // 制御パラメータを設定する(拡張版)
  //-----------------------------------------------------------------------------
  void CDoubleSpringInterpolate::SetParamExtend(const float fHopeSpringCoe, const float fCurSpringCoe, const float fHopeVelMax, const float fCurVelMax)
  {
    m_inHopeInterp.SetParam(fHopeSpringCoe, fHopeVelMax);
    m_inCurInterp .SetParam(fCurSpringCoe , fCurVelMax);
  }

  //-----------------------------------------------------------------------------
  // 入力希望値に到達したか確認する
  //-----------------------------------------------------------------------------
  bool CDoubleSpringInterpolate::IsReachHopeValue(const float fHopeValue) const { return IsReachHopeValue(fHopeValue, REACH_MARGIN*m_inCurInterp.GetEpsilon()); }
  bool CDoubleSpringInterpolate::IsReachHopeValue(const float fHopeValue, const float fReachEpsilon) const
  {
    return CUtil::IsNearlyEqualZero(m_inCurInterp .GetDiffValue(fHopeValue), fReachEpsilon) &&
           CUtil::IsNearlyEqualZero(m_inHopeInterp.GetDiffValue(fHopeValue), fReachEpsilon);
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CDoubleSpringInterpolate3D

  //-----------------------------------------------------------------------------
  // リセット
  //-----------------------------------------------------------------------------
  const gfl2::math::VEC3& CDoubleSpringInterpolate3D::Reset(const gfl2::math::VEC3& inResetValue)
  {
    m_inHopeInterp.Reset(inResetValue);
    m_inCurInterp .Reset(inResetValue);
    return m_inCurInterp.GetValue();
  }

  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  const gfl2::math::VEC3& CDoubleSpringInterpolate3D::Update(const gfl2::math::VEC3& inHopeValue, const float fDiffTime)
  {
    m_inHopeInterp.Update(inHopeValue, fDiffTime);
    m_inCurInterp .Update(m_inHopeInterp.GetValue(), fDiffTime);
    return m_inCurInterp.GetValue();
  }

  //-----------------------------------------------------------------------------
  // 外部更新
  //-----------------------------------------------------------------------------
  void CDoubleSpringInterpolate3D::OutUpdate(const gfl2::math::VEC3& inValue)
  {
    gfl2::math::VEC3 inDiff;      m_inCurInterp .GetDiffValue(&inDiff, inValue, m_inCurInterp.GetValue());
    gfl2::math::VEC3 inHopeValue; m_inHopeInterp.GetDiffValue(&inHopeValue, m_inHopeInterp.GetValue(), -inDiff);
    m_inHopeInterp.Reset(inHopeValue);
    m_inCurInterp .Reset(inValue);
  }

  //-----------------------------------------------------------------------------
  // 制御パラメータを設定する
  //-----------------------------------------------------------------------------
  void CDoubleSpringInterpolate3D::SetParam(const float fSpringCoe, const float fVelMax)
  {
    m_inHopeInterp.SetParam(fSpringCoe     , fVelMax);
    m_inCurInterp .SetParam(fSpringCoe*0.5f, INTERPOLATE_VEL_MAX_INFINITY);
  }

  //-----------------------------------------------------------------------------
  // 制御パラメータを設定する(拡張版)
  //-----------------------------------------------------------------------------
  void CDoubleSpringInterpolate3D::SetParamExtend(const float fHopeSpringCoe, const float fCurSpringCoe, const float fHopeVelMax, const float fCurVelMax)
  {
    m_inHopeInterp.SetParam(fHopeSpringCoe, fHopeVelMax);
    m_inCurInterp .SetParam(fCurSpringCoe , fCurVelMax);
  }

  //-----------------------------------------------------------------------------
  // 希望値に到達したか判定する
  //-----------------------------------------------------------------------------
  bool CDoubleSpringInterpolate3D::IsReachHopeValue(const gfl2::math::VEC3& inHopeValue) const { return IsReachHopeValue(inHopeValue, REACH_MARGIN*m_inCurInterp.GetEpsilon()); }
  bool CDoubleSpringInterpolate3D::IsReachHopeValue(const gfl2::math::VEC3& inHopeValue, const float fEpsilon) const
  {
    const float fEpsilonSqu = fEpsilon*fEpsilon;
    return (m_inCurInterp.IsReachHopeValueCommon(inHopeValue, fEpsilonSqu) && m_inHopeInterp.IsReachHopeValueCommon(inHopeValue, fEpsilonSqu));
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CTimeRatioInterpolate
  
  //-----------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------
  CTimeRatioInterpolate::CTimeRatioInterpolate() :
  m_fOneRatioTime(1.0f) ,
  m_fRatio(0.0f) ,
  m_fTimer(0.0f)
  {
    return ;
  }
  
  //-----------------------------------------------------------------------------
  // リセット
  //-----------------------------------------------------------------------------
  float CTimeRatioInterpolate::Reset(const float fResetRatio)
  {
    m_fRatio = fResetRatio;
    m_fTimer = m_fOneRatioTime*fResetRatio;
    return m_fRatio;
  }
  
  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  float CTimeRatioInterpolate::Update(const float fDiffTime, const bool bIsRatioUp)
  {
    if( bIsRatioUp == true )
    {
      m_fTimer += fDiffTime;
      if( m_fTimer >= m_fOneRatioTime )
      {
        m_fRatio = 1.0f;
        m_fTimer = m_fOneRatioTime;
      }
      else
        m_fRatio = m_fTimer/m_fOneRatioTime;
    }
    else
    {
      m_fTimer -= fDiffTime;
      if( m_fTimer <= 0.0f )
      {
        m_fRatio = 0.0f;
        m_fTimer = 0.0f;
      }
      else
        m_fRatio = m_fTimer/m_fOneRatioTime;
    }

    return m_fRatio;
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CTimeRatioInterpolateEx

  //-----------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------
  CTimeRatioInterpolateEx::CTimeRatioInterpolateEx() :
  m_fZeroRatioTime(0.0f) ,
  m_fMaxTime(m_fOneRatioTime)
  {
    return ;
  }

  //-----------------------------------------------------------------------------
  // リセット
  //-----------------------------------------------------------------------------
  float CTimeRatioInterpolateEx::Reset(const float fResetRatio)
  {
    m_fRatio = fResetRatio;
    if     ( m_fRatio <= 0.0f ) m_fTimer = 0.0f;
    else if( m_fRatio >= 1.0f ) m_fTimer = m_fMaxTime;
    else                        m_fTimer = GetRatioInterpolateValue(m_fZeroRatioTime, m_fOneRatioTime, m_fRatio);

    return m_fRatio;
  }

  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  float CTimeRatioInterpolateEx::Update(const float fDiffTime, const bool bIsRatioUp)
  {
    if( bIsRatioUp == true )
    {
      m_fTimer += fDiffTime;
      if( m_fTimer >= m_fMaxTime )
      {
        m_fRatio = 1.0f;
        m_fTimer = m_fMaxTime;
      }
      else
        m_fRatio = GetInterpolateRatioValue(m_fTimer, m_fZeroRatioTime, m_fOneRatioTime);
    }
    else
    {
      m_fTimer -= fDiffTime;
      if( m_fTimer <= 0.0f )
      {
        m_fRatio = 0.0f;
        m_fTimer = 0.0f;
      }
      else
        m_fRatio = GetInterpolateRatioValue(m_fTimer, m_fZeroRatioTime, m_fOneRatioTime);
    }

    return m_fRatio;
  }

  //-----------------------------------------------------------------------------
  // 制御パラメータを設定する
  //-----------------------------------------------------------------------------
  void CTimeRatioInterpolateEx::SetParam(const float fZeroRatioTime, const float fOneRatioTime, const float fMaxTime)
  {
    m_fZeroRatioTime = fZeroRatioTime;
    m_fOneRatioTime  = fOneRatioTime;
    m_fMaxTime       = fMaxTime;

    if     ( m_fRatio <= 0.0f ) CUtil::MaxClip(&m_fTimer, m_fZeroRatioTime);
    else if( m_fRatio >= 1.0f ) CUtil::MaxMinClip(&m_fTimer, m_fMaxTime, m_fOneRatioTime);
    else                        m_fTimer = GetRatioInterpolateValue(m_fZeroRatioTime, m_fOneRatioTime, m_fRatio);
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CLinearWaveInterpolate

  //-----------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------
  CLinearWaveInterpolate::CLinearWaveInterpolate() :
  m_fStepCoe(1.0f) ,
  m_fValue  (0.0f) ,
  m_bIsAdd  (true)
  {
    return ;
  }

  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  float CLinearWaveInterpolate::Update(const float fDiffTime)
  {
    if( m_bIsAdd == true )
    {
      m_fValue += m_fStepCoe*fDiffTime;

      if( m_fValue >= 1.0f )
      {
        // fDiffTimeがとんでもない値でも破綻しないようwhile保険処理
        while( m_fValue > 2.0f ) m_fValue -= 1.0f;

        m_fValue = 2.0f - m_fValue;
        m_bIsAdd = false;
      }
    }
    else
    {
      m_fValue -= m_fStepCoe*fDiffTime;

      if( m_fValue <= 0.0f )
      {
        while( m_fValue < -1.0f ) m_fValue += 1.0f;

        m_fValue = -m_fValue;
        m_bIsAdd = true;
      }
    }

    return m_fValue;
  }

} // namespace field
} } // namespace System namespace Camera

/*  EOF  */





