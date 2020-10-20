
//=============================================================================
/**
 * @file   CameraSpace.cpp
 * @brief  空間制御クラス
 * @author 池田 哲平
 * @date   2012.05.29
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#include "CameraSpace.h"
#include "CameraUtility.h"

namespace System { namespace Camera { namespace field
{
  //-----------------------------------------------------------------------------
  // マトリクス33を生成する
  //-----------------------------------------------------------------------------
  void CSpace::CreateMTX33ByFrontUpDirection(gfl2::math::MTX33* pMat, const gfl2::math::VEC3& inFrontDir, const gfl2::math::VEC3& inUpVec)
  {
    gfl2::math::VEC3 inAxisX; gfl2::math::VEC3Cross(&inAxisX, &inUpVec,    &inFrontDir); gfl2::math::VEC3Normalize(&inAxisX, &inAxisX);
    gfl2::math::VEC3 inAxisY; gfl2::math::VEC3Cross(&inAxisY, &inFrontDir, &inAxisX);
    gfl2::math::MTX33SetAxis(pMat, inAxisX, inAxisY, inFrontDir);
  }

  //-----------------------------------------------------------------------------
  // マトリクス33を生成する
  //-----------------------------------------------------------------------------
  void CSpace::CreateMTX33ByUpFrontDirection(gfl2::math::MTX33* pMat, const gfl2::math::VEC3& inUpDir, const gfl2::math::VEC3& inFrontVec)
  {
    gfl2::math::VEC3 inAxisX; gfl2::math::VEC3Cross(&inAxisX, &inUpDir, &inFrontVec); gfl2::math::VEC3Normalize(&inAxisX, &inAxisX);
    gfl2::math::VEC3 inAxisZ; gfl2::math::VEC3Cross(&inAxisZ, &inAxisX, &inUpDir);
    gfl2::math::MTX33SetAxis(pMat, inAxisX, inUpDir, inAxisZ);
  }

  //-----------------------------------------------------------------------------
  // カメラマトリクスを生成する
  //-----------------------------------------------------------------------------
  void CSpace::CreateCameraMatrix(gfl2::math::MTX34* pMat, const gfl2::math::VEC3& inPosi, const gfl2::math::VEC3& inLookAtPosi, const gfl2::math::VEC3& inUpVec)
  {
    gfl2::math::VEC3 inDir;
    gfl2::math::VEC3Sub(&inDir, &inLookAtPosi, &inPosi);
    gfl2::math::VEC3Normalize(&inDir, &inDir);
    gfl2::math::MTX33 inRotMat; CreateMTX33ByFrontUpDirection(&inRotMat, inDir, inUpVec);
    gfl2::math::MTX34SetMTX33VEC3(pMat, inRotMat, inPosi);
  }

  //---------------------------------------------------------------------------------------------------------------------
  // よー・ぴっち・ばんく → マトリクス

  //-----------------------------------------------------------------------------
  // よーから方向XZを取得する
  //-----------------------------------------------------------------------------
  void CSpace::GetDirectionXZByYaw(gfl2::math::VEC3* pDirXZ, const float fYaw)
  {
    float fSin, fCos; gfl2::math::SinCosRad(&fSin, &fCos, fYaw);
    pDirXZ->x = fSin;
    pDirXZ->y = 0.0f;
    pDirXZ->z = fCos;
  }

  //-----------------------------------------------------------------------------
  // よー・ぴっちから方向を取得する
  //-----------------------------------------------------------------------------
  void CSpace::GetDirectionByYawPitch(gfl2::math::VEC3* pDir, const float fYaw, const float fPitch)
  {
    float fSin, fCos; gfl2::math::SinCosRad(&fSin, &fCos, fPitch);
    pDir->x =  0.0f;
    pDir->y = -fSin;
    pDir->z =  fCos;

    gfl2::math::MTX33 inRotMat; gfl2::math::MTX33RotYRad(&inRotMat, fYaw);
    gfl2::math::VEC3Transform(pDir, &inRotMat, pDir);
  }

  //-----------------------------------------------------------------------------
  // よー・ぴっち・ばんく回転を行ったマトリクスを取得する
  //-----------------------------------------------------------------------------
  void CSpace::GetYawPitchBankRotMatrixExtend(gfl2::math::MTX33* pDestMat, const ESolveAxisOrder eSolveAxisOrder, const gfl2::math::VEC3& inYPB, bool bIsIdentityMat, const gfl2::math::MTX33& inSrcMat)
  {
    int anSolveAxisIndex[3] = { ASSIGN_INDEX_YAW, ASSIGN_INDEX_PITCH, ASSIGN_INDEX_BANK };
    switch( eSolveAxisOrder )
    {
    case SOLVE_AXIS_ORDER_YPB:
      break;

    case SOLVE_AXIS_ORDER_PYB:
      anSolveAxisIndex[0] = ASSIGN_INDEX_PITCH;
      anSolveAxisIndex[1] = ASSIGN_INDEX_YAW;
      break;

    default:
      GFL_ASSERT_ALWAYS;
      break;
    }

    (*pDestMat) = inSrcMat;
    for( int i = 0; i < 3; i++ )
    {
      const int nSolveAxisIndex = anSolveAxisIndex[i];
      const float fRotRadi = inYPB[nSolveAxisIndex];
      if( fRotRadi != 0.0f )
      {
        if( bIsIdentityMat )
        {
          if     ( nSolveAxisIndex == ASSIGN_INDEX_YAW   ) gfl2::math::MTX33RotYRad(pDestMat, fRotRadi);
          else if( nSolveAxisIndex == ASSIGN_INDEX_PITCH ) gfl2::math::MTX33RotXRad(pDestMat, fRotRadi);
          else                                             gfl2::math::MTX33RotZRad(pDestMat, fRotRadi);
          bIsIdentityMat = false;
        }
        else
        {
          gfl2::math::MTX33 inTempMat(*pDestMat);
          if     ( nSolveAxisIndex == ASSIGN_INDEX_YAW   ) gfl2::math::MTX33MultLimitRotYRad(pDestMat, &inTempMat, fRotRadi);
          else if( nSolveAxisIndex == ASSIGN_INDEX_PITCH ) gfl2::math::MTX33MultLimitRotXRad(pDestMat, &inTempMat, fRotRadi);
          else                                             gfl2::math::MTX33MultLimitRotZRad(pDestMat, &inTempMat, fRotRadi);
        }
      }
    }
  }

  //---------------------------------------------------------------------------------------------------------------------
  // マトリクス → よー・ぴっち・ばんく

#define UP_DIR_EPSILON (0.99999f)

  //-----------------------------------------------------------------------------
  // 方向から一意のよー・ぴっちを取得できるかチェックする
  //-----------------------------------------------------------------------------
  bool CSpace::IsYawPitchGetDirection(const gfl2::math::VEC3& inDir) { return (gfl2::math::FAbs(inDir.y) < UP_DIR_EPSILON); }

  //-----------------------------------------------------------------------------
  // 方向からよーを取得する
  //-----------------------------------------------------------------------------
  float CSpace::GetYawByDirection(const gfl2::math::VEC3& inDir)
  { return (IsYawPitchGetDirection(inDir) == true) ? gfl2::math::Atan2Rad(inDir.x, inDir.z) : 0.0f; }

  //-----------------------------------------------------------------------------
  // 方向からよー・ぴっちを取得する
  //-----------------------------------------------------------------------------
  bool CSpace::GetYawPitchByDirection(float* pfYaw, float* pfPitch, const gfl2::math::VEC3& inDir)
  {
    const bool bIsGetDir = IsYawPitchGetDirection(inDir);
    if( bIsGetDir ) (*pfYaw) = gfl2::math::Atan2Rad(inDir.x, inDir.z);
    else            (*pfYaw) = 0.0f;
    (*pfPitch) = gfl2::math::AsinRad(-inDir.y);
    return bIsGetDir;
  }

  //-----------------------------------------------------------------------------
  // マトリクスからばんくを取得する
  //-----------------------------------------------------------------------------
  float CSpace::GetBankByMatrix(const gfl2::math::MTX33& inMat)
  {
    if( gfl2::math::FAbs(inMat.m[1][2]) >= UP_DIR_EPSILON ) return gfl2::math::Atan2Rad(-inMat.m[0][1], inMat.m[0][0]);
    else                                                   return gfl2::math::Atan2Rad( inMat.m[1][0], inMat.m[1][1]);
  }

  //-----------------------------------------------------------------------------
  // マトリクスからよー・ぴっち・ばんくを取得する
  //-----------------------------------------------------------------------------
  void CSpace::GetYawPitchBankByMatrix(gfl2::math::VEC3* pYPB, const gfl2::math::MTX33& inMat)
  {
    if( gfl2::math::FAbs(inMat.m[1][2]) >= UP_DIR_EPSILON )
    {
      (*pYPB)[ASSIGN_INDEX_BANK]  = gfl2::math::Atan2Rad(-inMat.m[0][1], inMat.m[0][0]);
      (*pYPB)[ASSIGN_INDEX_PITCH] = gfl2::math::AsinRad (-inMat.m[1][2]);
      (*pYPB)[ASSIGN_INDEX_YAW]   = 0.0f;
    }
    else
    {
      (*pYPB)[ASSIGN_INDEX_BANK]  = gfl2::math::Atan2Rad( inMat.m[1][0], inMat.m[1][1]);
      (*pYPB)[ASSIGN_INDEX_PITCH] = gfl2::math::AsinRad (-inMat.m[1][2]);
      (*pYPB)[ASSIGN_INDEX_YAW]   = gfl2::math::Atan2Rad( inMat.m[0][2], inMat.m[2][2]);
    }
  }

  //-----------------------------------------------------------------------------
  // マトリクスからよー・ぴっち・ばんくを取得するの拡張版
  //-----------------------------------------------------------------------------
  void CSpace::GetYawPitchBankByMatrixExtend(gfl2::math::VEC3* pYPB, const ESolveAxisOrder eSolveAxisOrder, const gfl2::math::MTX33& inMat)
  {
    switch( eSolveAxisOrder )
    {
    case SOLVE_AXIS_ORDER_YPB:
      GetYawPitchBankByMatrix(pYPB, inMat);
      break;

    case SOLVE_AXIS_ORDER_PYB: {
      gfl2::math::MTX33 inFilterMat;
      gfl2::math::VEC3  inAxisX; gfl2::math::MTX33GetAxisX(&inAxisX, inMat);
      gfl2::math::VEC3  inAxisY; gfl2::math::MTX33GetAxisY(&inAxisY, inMat);
      gfl2::math::VEC3  inAxisZ; gfl2::math::MTX33GetAxisZ(&inAxisZ, inMat);
      gfl2::math::MTX33SetAxis(&inFilterMat, -inAxisY, inAxisX, inAxisZ);

      gfl2::math::MTX33 inLocalMat;
      gfl2::math::MTX33SetAxis(&inLocalMat, -System::Camera::internal::VEC3_UNIT_Y, System::Camera::internal::VEC3_UNIT_X, System::Camera::internal::VEC3_UNIT_Z);
      gfl2::math::MTX33InverseOrtho(&inLocalMat, &inLocalMat);

      gfl2::math::MTX33 inTempMat(inFilterMat);
      gfl2::math::MTX33MultLimit(&inFilterMat, &inLocalMat, &inTempMat);
      gfl2::math::VEC3 inTemp; GetYawPitchBankByMatrix(&inTemp, inFilterMat);
      (*pYPB)[ASSIGN_INDEX_YAW]   = -inTemp[1];
      (*pYPB)[ASSIGN_INDEX_PITCH] =  inTemp[0];
      (*pYPB)[ASSIGN_INDEX_BANK]  =  inTemp[2];
      break; }
      /*
    inFilterMat.makeFromAxis(-getUpVectorByCorrespondAxis(inMat,eCorrespondAxis),
      getSideVectorByCorrespondAxis(inMat,eCorrespondAxis),getForwardVectorByCorrespondAxis(inMat,eCorrespondAxis));
    inFilterMat.mul33(tiMatrix().makeFromAxis(-getCorrespondAxisY(eCorrespondAxis),
      getCorrespondAxisX(eCorrespondAxis),getCorrespondAxisZ(eCorrespondAxis)).transpose33PosiZero());
      */

    default:
      GFL_ASSERT_ALWAYS;
      (*pYPB) = System::Camera::internal::VEC3_ZERO;
      break;
    }
  }

} // namespace field
} } // namespace System namespace Camera

/*  EOF  */





