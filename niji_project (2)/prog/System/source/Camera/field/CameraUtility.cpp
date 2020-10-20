
//=============================================================================
/**
 * @file   CameraUtility.cpp
 * @brief  ユーティリティマクロ・クラス
 * @author 池田 哲平
 * @date   2012.05.29
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#include "system/include/GflUse.h"
#include "CameraUtility.h"
#include "CameraInterpolate.h"
#include "CameraDebugPrimitive.h"

namespace System { namespace Camera { namespace field
{
  const gfl2::math::VEC4 VEC4_COLOR_WHITE      (1.0f, 1.0f, 1.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_RED        (1.0f, 0.0f, 0.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_GREEN      (0.0f, 1.0f, 0.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_BLUE       (0.0f, 0.0f, 1.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_YELLOW     (1.0f, 1.0f, 0.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_THIN_WHITE (0.5f, 0.5f, 0.5f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_THIN_RED   (1.0f, 0.5f, 0.5f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_THIN_GREEN (0.5f, 1.0f, 0.5f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_THIN_BLUE  (0.5f, 0.5f, 1.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_THIN_YELLOW(1.0f, 1.0f, 0.5f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_ORANGE     (1.0f, 0.5f, 0.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_SKY        (0.0f, 1.0f, 1.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_THIN_SKY   (0.5f, 1.0f, 1.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_PURPLE     (1.0f, 0.0f, 1.0f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_PEACH      (1.0f, 0.5f, 0.5f, 1.0f);
  const gfl2::math::VEC4 VEC4_COLOR_BROWN      (153.0f/255.0f, 76.0f/255.0f, 0.0f, 1.0f);

  //-----------------------------------------------------------------------------
  // 乱数を取得する(0 <= ret < 1)
  //-----------------------------------------------------------------------------
  float CUtil::GetRandom()
  {
    const u32 MAX = 65535;
    const u32 nRand = System::GflUse::GetPublicRand(MAX);
    return static_cast<float>(nRand)/static_cast<float>(MAX);
  }

  //-----------------------------------------------------------------------------
  // ブール型の乱数を取得する
  //-----------------------------------------------------------------------------
  bool CUtil::GetRandomBool()
  {
    const u32 BORDER = 0xffffffff/2;  // @fix cov_ctr[10040]: kujira(sango)から移植したときに/2を付け忘れていたのを修正。
    return (System::GflUse::GetPublicRand() > BORDER);
  }

  //-----------------------------------------------------------------------------
  // 入力ラジアン値を-πから+πの間に変換する
  //-----------------------------------------------------------------------------
  float CUtil::ConvertFromMinusPiToPlusPi(const float fRadi)
  {
    const float Pi   = System::Camera::internal::F_PI;
    const float Pix2 = System::Camera::internal::F_PIx2;
    if( fRadi >= -Pi && fRadi <= Pi ) return fRadi;
    else
    {
      const float fAngle = fRadi;
      float fOffset;
      *(u32*)&fOffset = (*(u32*)&Pi | (*(u32*)&fAngle & 0x80000000));
      return fAngle - Pix2 * (float)(int)((fAngle + fOffset) / Pix2);
    }
  }

  //-----------------------------------------------------------------------------
  // ローカル空間レベルの位置を取得する
  //-----------------------------------------------------------------------------
  void CUtil::GetLocalPosition(gfl2::math::VEC3* pDestPosi, const gfl2::math::VEC3& inSrcPosi, const gfl2::math::MTX34& inMat)
  {
    gfl2::math::VEC3 inDestPosiWorld; // &dest == &src 時の対処
    gfl2::math::MTX34GetTranslate(&inDestPosiWorld, inMat);
    gfl2::math::VEC3Sub(&inDestPosiWorld, &inSrcPosi, &inDestPosiWorld);

    gfl2::math::MTX33 inMatInv(inMat); gfl2::math::MTX33InverseOrtho(&inMatInv, &inMatInv);
    gfl2::math::VEC3Transform(pDestPosi, &inMatInv, &inDestPosiWorld);
  }

  //-----------------------------------------------------------------------------
  // 正規化ベクトルの取得可能チェックを行い、取得可能時は取得する
  //-----------------------------------------------------------------------------
  bool CUtil::CheckAndGetNormalizeVector(gfl2::math::VEC3* pNormVec, float* pfLength, const gfl2::math::VEC3& inSrcVec)
  {
    const float fLenSqu = gfl2::math::VEC3SquareLen(&inSrcVec);
    if( fLenSqu > NEARLY_EQUAL_EPSILON_LENGTH*NEARLY_EQUAL_EPSILON_LENGTH )
    {
      const float fLen = gfl2::math::FSqrt(fLenSqu);
      if( pNormVec ) (*pNormVec) = (1.0f/fLen)*inSrcVec;
      if( pfLength ) (*pfLength) = fLen;
      return true;
    }
    else
      return false;
  }

  //-----------------------------------------------------------------------------
  // 入力方向から出力方向へ到達する回転角度を取得する
  //-----------------------------------------------------------------------------
  float CUtil::GetReachRadian(const gfl2::math::VEC3& inSrcDir, const gfl2::math::VEC3& inDestDir, const gfl2::math::VEC3& inRotAxis)
  {
    float fRadi = GetACosLimitClip(GetCos(inSrcDir, inDestDir));
    gfl2::math::VEC3 inJudgeAxis; gfl2::math::VEC3Cross(&inJudgeAxis, &inSrcDir, &inDestDir);
    if( IsInverseVector(inJudgeAxis, inRotAxis) ) fRadi *= -1.0f;

    return fRadi;
  }

  //-----------------------------------------------------------------------------
  // 入力方向から出力方向へ到達する回転角度を取得する(回転軸算出版)
  //-----------------------------------------------------------------------------
  float CUtil::GetReachRadian(gfl2::math::VEC3* pRotAxis, const gfl2::math::VEC3& inSrcDir, const gfl2::math::VEC3& inDestDir)
  {
#if 1

    float fRadi = 0.0f;
    gfl2::math::VEC3 inRotAxis(System::Camera::internal::VEC3_UNIT_Z);

    const float fCos = GetCos(inSrcDir, inDestDir);
    if     ( fCos >=  NEARLY_EQUAL_ONE_EPSILON_COS ) ; // 0度
    else if( fCos <= -NEARLY_EQUAL_ONE_EPSILON_COS )   // 180度
    {
      gfl2::math::VEC3 inOptDir;
      if( gfl2::math::FAbs(inSrcDir.z) > 0.9f ) inOptDir = System::Camera::internal::VEC3_UNIT_X;
      else                                     inOptDir = System::Camera::internal::VEC3_UNIT_Z;

      gfl2::math::VEC3Cross(&inRotAxis, &inSrcDir, &inOptDir);
      fRadi = System::Camera::internal::F_PI;
    }
    else
    {
      gfl2::math::VEC3Cross(&inRotAxis, &inSrcDir, &inDestDir);
      fRadi = GetACosLimitClip(fCos); // 外積で回転軸の向きが動的に変わるので、回転方向(+ or -)は常に一方向
    }

    if( pRotAxis ) (*pRotAxis) = inRotAxis;
    return fRadi;

#else

    gfl2::math::VEC3 inRotAxis; gfl2::math::VEC3Cross(&inRotAxis, &inSrcDir, &inDestDir);

    // 0,180度時例外処理
    if( IsNearlyEqualZeroXYZ(inRotAxis, NEARLY_EQUAL_EPSILON_DIR) == true )
    {
      gfl2::math::VEC3 inOptDir;
      if( gfl2::math::FAbs(inSrcDir.z) > 0.9f ) inOptDir = System::Camera::internal::VEC3_UNIT_X;
      else                                     inOptDir = System::Camera::internal::VEC3_UNIT_Z;

      gfl2::math::VEC3Cross(&inRotAxis, &inSrcDir, &inOptDir);
    }

    if( pRotAxis ) (*pRotAxis) = inRotAxis;
    return GetReachRadian(inSrcDir, inDestDir, inRotAxis);

#endif
  }

  //-----------------------------------------------------------------------------
  // 入力方向から出力方向へ到達する回転マトリクスを取得する
  //-----------------------------------------------------------------------------
  void CUtil::GetReachMatrix(gfl2::math::MTX33* pMat, const gfl2::math::VEC3& inSrcDir, const gfl2::math::VEC3& inDestDir)
  {
    gfl2::math::VEC3 inAxis;
    const float fRadi = GetReachRadian(&inAxis, inSrcDir, inDestDir);

    if( fRadi == 0.0f ) gfl2::math::MTX33Identity(pMat);
    else
    {
      gfl2::math::VEC3Normalize(&inAxis, &inAxis);
      gfl2::math::MTX33RotAxisRad(pMat, &inAxis, fRadi);
    }
  }

  //-----------------------------------------------------------------------------
  // 最大・最少位置を取得する
  //-----------------------------------------------------------------------------
  void CUtil::GetMaxMinPosition(gfl2::math::VEC3* pMax, gfl2::math::VEC3* pMin, const gfl2::math::VEC3 ainPosi[], const int nPosiNum)
  {
    GFL_ASSERT(nPosiNum > 0);

    (*pMax) = (*pMin) = ainPosi[0];
    for( int i = 1; i < nPosiNum; i++)
    {
      const gfl2::math::VEC3& inPosi = ainPosi[i];
      if     ( pMax->x < inPosi.x ) pMax->x = inPosi.x;
      else if( pMin->x > inPosi.x ) pMin->x = inPosi.x;

      if     ( pMax->y < inPosi.y ) pMax->y = inPosi.y;
      else if( pMin->y > inPosi.y ) pMin->y = inPosi.y;

      if     ( pMax->z < inPosi.z ) pMax->z = inPosi.z;
      else if( pMin->z > inPosi.z ) pMin->z = inPosi.z;
    }
  }

  //-----------------------------------------------------------------------------
  // 上昇ラインを描く比率配列を設定する
  //-----------------------------------------------------------------------------
  void CUtil::SetUpLineRatioArray(float afArray[], const int nArrayNum)
  {
    if( nArrayNum == 1 )
      afArray[0] = 1.0f;
    else
    {
      const float fEnd = static_cast<float>(nArrayNum - 1);
      for( int i = 0; i < nArrayNum; i++ )
        afArray[i] = static_cast<float>(i)/fEnd;
    }
  }

  //-----------------------------------------------------------------------------
  // 遊び(０範囲)を加えた入力値を取得する
  //-----------------------------------------------------------------------------
  float CUtil::GetAddMarginInputValue(const float fSrc, const float fMargin)
  {
    const float fDest = GetInterpolateRatioValue(gfl2::math::FAbs(fSrc), fMargin, 1.0f);
    return (fSrc >= 0.0f) ? fDest : -fDest;
  }

  //-----------------------------------------------------------------------------
  // XとYを取り換えるマトリクスを取得する
  //-----------------------------------------------------------------------------
  void CUtil::GetExchangeXYMatrix(gfl2::math::MTX44* pMat)
  {
    gfl2::math::MTX44Identity(pMat);
    pMat->m[0][0] = pMat->m[1][1] = 0.0f;
    pMat->m[1][0] = -1.0f;
    pMat->m[0][1] =  1.0f;
  }

  //-----------------------------------------------------------------------------
  // よー・ぴっち・ばんくをツール用からゲーム用へ変換する
  //-----------------------------------------------------------------------------
  void CUtil::ConvertFromToolToGameYPB(float* pfYawRadi, float* pfPitchRadi, float* pfBankRadi)
  {
    (*pfYawRadi)    = CUtil::AddRadian(-(*pfYawRadi), -System::Camera::internal::F_PI);
    (*pfPitchRadi) *= -1.0f;
    // (*pfBankRadi)   = (*pfBankRadi);
  }

  //-----------------------------------------------------------------------------
  // よー・ぴっち・ばんくをゲーム用からツール用へ変換する
  //-----------------------------------------------------------------------------
  void CUtil::ConvertFromGameToToolYPB(float* pfYawRadi, float* pfPitchRadi, float* pfBankRadi)
  {
    (*pfYawRadi)    = -CUtil::AddRadian(*pfYawRadi, System::Camera::internal::F_PI);
    (*pfPitchRadi) *= -1.0f;
  }

  //-----------------------------------------------------------------------------
  // ワールド位置をスクリーン位置に変換する
  //-----------------------------------------------------------------------------
  void CUtil::ConvertFromWorldToScreenPosition(gfl2::math::VEC2* pScreenPosi, const gfl2::math::VEC3& inWorldPosi, const gfl2::math::MTX44& inViewProjMat, const float fScreenWidth, const float fScreenHeight)
  {
// GFBTS[2597] 3D座標を2D座標に変換する際、カメラの後にワールド位置が存在すると、変換後位置が前後してしまう問題を修正
#if 1  // #ifdef BUGFIX_GFBTS2597_20130722  // nijiにて修正されたコードを通るように#if 1にした。  2016/05/17 kawada_koji@gamefreak.co.jp
    // カメラの後ろにワールド位置があってもうまく動くよう対処
    gfl2::math::VEC4 inProjPosi; nw::math::VEC3Transform(&inProjPosi, &inViewProjMat, &inWorldPosi);
    float fW = gfl2::math::FAbs(inProjPosi.w); CUtil::MinClip(&fW, UTIL_METER_TO_UNIT(0.001f)); // w = -view space z
    inProjPosi.x /= fW;
    inProjPosi.y /= fW;
#else
    gfl2::math::VEC3 inProjPosi; System::Camera::internal::VEC3TransformCoord(&inProjPosi, &inViewProjMat, &inWorldPosi);  //nn::math::VEC3TransformCoord(&inProjPosi, &inViewProjMat, &inWorldPosi);
#endif
    pScreenPosi->x = (       UTIL_MINUS_ONE_ONE_TO_ZERO_ONE(inProjPosi.x))*fScreenWidth;
    pScreenPosi->y = (1.0f - UTIL_MINUS_ONE_ONE_TO_ZERO_ONE(inProjPosi.y))*fScreenHeight;
  }

  //---------------------------------------------------------------------------------------------------------------------
  // CSphere

  //-----------------------------------------------------------------------------
  // 複数の点を内包する球を設定する
  //-----------------------------------------------------------------------------
  void CUtil::CSphere::Set(const gfl2::math::VEC3 ainPosi[], const int nPosiNum)
  {
    gfl2::math::VEC3 inMax, inMin; GetMaxMinPosition(&inMax, &inMin, ainPosi, nPosiNum);
    m_inPosi = GetCenterValue(inMax, inMin);

    float fRadiusSqu = gfl2::math::VEC3SquareDist(&m_inPosi, &ainPosi[0]);
    for( int i = 1; i < nPosiNum; i++ ) MinClip(&fRadiusSqu, gfl2::math::VEC3SquareDist(&m_inPosi, &ainPosi[i]));

    m_fRadius = gfl2::math::FSqrt(fRadiusSqu);
  }

  //-----------------------------------------------------------------------------
  // 球との衝突判定を行う
  //-----------------------------------------------------------------------------
  bool CUtil::CSphere::IsHitSphere(const CSphere& inSphere) const
  {
    const float fDistSqu = gfl2::math::VEC3SquareDist(&m_inPosi, &inSphere.m_inPosi);
    const float fMergeRadius = m_fRadius + inSphere.m_fRadius;
    return (fDistSqu <= fMergeRadius*fMergeRadius);
  }

  //-----------------------------------------------------------------------------
  // デバッグ描画
  //-----------------------------------------------------------------------------
  void CUtil::CSphere::DrawDebug(const gfl2::math::VEC4& inColor) const
  {
#if PM_DEBUG
    CDebugPrim::DrawSphere(m_inPosi, m_fRadius, inColor);
#endif
  }

} // namespace field
} } // namespace System namespace Camera

/*  EOF  */





