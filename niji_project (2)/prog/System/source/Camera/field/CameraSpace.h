
//=============================================================================
/**
 * @file   CameraSpace.h
 * @brief  空間制御クラスヘッダ
 * @author 池田 哲平
 * @date   2012.05.29
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#if !defined( __CAMERA_SPACE_H__ )
#define __CAMERA_SPACE_H__
#pragma once

#include <math/include/gfl2_math.h>
#include "CameraUtility.h"

namespace System { namespace Camera { namespace field
{
  /**
   * @brief よー・ぴっち・ばんく割り当てインデックス列挙型
   */
  enum EAssignIndexYPB
  {
    ASSIGN_INDEX_YAW = 0 ,
    ASSIGN_INDEX_PITCH   ,
    ASSIGN_INDEX_BANK    ,
  };

  /**
   * @brief 解決軸順番列挙型
   * @brief 各X(ぴっち),Y(よー),Z(ばんく)軸回転をどの順番で解決していくかを定義する列挙型
   */
  enum ESolveAxisOrder
  {
    SOLVE_AXIS_ORDER_YPB = 0 , ///< よー・ぴっち・ばんくの順で解決
    SOLVE_AXIS_ORDER_PYB     , ///< ぴっち・よー・ばんくの順で解決
  };

  /**
   * @brief 空間制御クラス
   * @brief 主にマトリクスの生成や回転をサポートする
   */
  class CSpace
  {
  public:
    //-----------------------------------------------------------------------------
    /**
     * @brief マトリクス33を生成する
     *
     * @param pMat       出力マトリクス
     * @param inFrontDir 前方向(正規化必須)
     * @param inUpVec    上ベクタ(未正規化でもよい)
     */
    //-----------------------------------------------------------------------------
    static void CreateMTX33ByFrontUpDirection(gfl2::math::MTX33* pMat, const gfl2::math::VEC3& inFrontDir, const gfl2::math::VEC3& inUpVec);

    //-----------------------------------------------------------------------------
    /**
     * @brief マトリクス33を生成する(UP方向基準)
     *
     * @param pMat       出力マトリクス
     * @param inUpDir    上方向(正規化必須)
     * @param inFrontVec 前ベクタ(未正規化でもよい)
     */
    //-----------------------------------------------------------------------------
    static void CreateMTX33ByUpFrontDirection(gfl2::math::MTX33* pMat, const gfl2::math::VEC3& inUpDir, const gfl2::math::VEC3& inFrontVec);

    //-----------------------------------------------------------------------------
    /**
     * @brief カメラマトリクスを生成する
     *
     * @param pMat         出力カメラマトリクス
     * @param inPosi       カメラ位置
     * @param inLookAtPosi 注視点位置
     * @param inUpVec      上ベクタ(未正規化でもよい)    
     */
    //-----------------------------------------------------------------------------
    static void CreateCameraMatrix(gfl2::math::MTX34* pMat, const gfl2::math::VEC3& inPosi, const gfl2::math::VEC3& inLookAtPosi, const gfl2::math::VEC3& inUpVec);

    //---------------------------------------------------------------------------------------------------------------------
    // よー・ぴっち・ばんく → マトリクス

    //-----------------------------------------------------------------------------
    /**
     * @brief よーから方向XZを取得する
     *
     * @param pDirXZ 出力方向XZ
     * @param fYaw   よー(ラジアン)
     */
    //-----------------------------------------------------------------------------
    static void GetDirectionXZByYaw(gfl2::math::VEC3* pDirXZ, const float fYaw);

    //-----------------------------------------------------------------------------
    /**
     * @brief よー・ぴっちから方向を取得する
     *
     * @param pDir   出力方向
     * @param fYaw   よー(ラジアン)
     * @param fPitch ぴっち(ラジアン)
     */
    //-----------------------------------------------------------------------------
    static void GetDirectionByYawPitch(gfl2::math::VEC3* pDir, const float fYaw, const float fPitch);

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力マトリクスに対してよー回転を行ったマトリクスを取得する
     *
     * @param pDestMat       出力マトリクス
     * @param fYaw           よー(ラジアン)
     * @param bIsIdentityMat inSrcMatが単位行列か
     * @param inSrcMat       入力マトリクス
     */
    //-----------------------------------------------------------------------------
    static inline void GetYawRotMatrix(gfl2::math::MTX33* pDestMat, const float fYaw, const bool bIsIdentityMat = true, const gfl2::math::MTX33& inSrcMat = System::Camera::internal::MTX33_IDENTITY)
    { GetYawPitchBankRotMatrix(pDestMat, gfl2::math::VEC3(fYaw, 0.0f, 0.0f), bIsIdentityMat, inSrcMat); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力マトリクスに対してぴっち回転を行ったマトリクスを取得する
     *
     * @param pDestMat       出力マトリクス
     * @param fPitch         ぴっち(ラジアン)
     * @param bIsIdentityMat inSrcMatが単位行列か
     * @param inSrcMat       入力マトリクス
     */
    //-----------------------------------------------------------------------------
    static inline void GetPitchRotMatrix(gfl2::math::MTX33* pDestMat, const float fPitch, const bool bIsIdentityMat = true, const gfl2::math::MTX33& inSrcMat = System::Camera::internal::MTX33_IDENTITY)
    { GetYawPitchBankRotMatrix(pDestMat, gfl2::math::VEC3(0.0f, fPitch, 0.0f), bIsIdentityMat, inSrcMat); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力マトリクスに対してばんく回転を行ったマトリクスを取得する
     *
     * @param pDestMat       出力マトリクス
     * @param fBank          ばんく(ラジアン)
     * @param bIsIdentityMat inSrcMatが単位行列か
     * @param inSrcMat       入力マトリクス
     */
    //-----------------------------------------------------------------------------
    static inline void GetBankRotMatrix(gfl2::math::MTX33* pDestMat, const float fBank, const bool bIsIdentityMat = true, const gfl2::math::MTX33& inSrcMat = System::Camera::internal::MTX33_IDENTITY)
    { GetYawPitchBankRotMatrix(pDestMat, gfl2::math::VEC3(0.0f, 0.0f, fBank), bIsIdentityMat, inSrcMat); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力マトリクスに対してよー・ぴっち・ばんく回転を行ったマトリクスを取得する
     *
     * @param pDestMat       出力マトリクス
     * @param inYPB          よー・ぴっち・ばんく(ラジアン)
     * @param bIsIdentityMat inSrcMatが単位行列か
     * @param inSrcMat       入力マトリクス
     */
    //-----------------------------------------------------------------------------
    static inline void GetYawPitchBankRotMatrix(gfl2::math::MTX33* pDestMat, const gfl2::math::VEC3& inYPB, const bool bIsIdentityMat = true, const gfl2::math::MTX33& inSrcMat = System::Camera::internal::MTX33_IDENTITY)
    { return GetYawPitchBankRotMatrixExtend(pDestMat, SOLVE_AXIS_ORDER_YPB, inYPB, bIsIdentityMat, inSrcMat); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力マトリクスに対して指定した解決軸順番でよー・ぴっち・ばんく回転を行ったマトリクスを取得する
     *
     * @param pDestMat        出力マトリクス
     * @param eSolveAxisOrder 解決軸順番
     * @param inYPB           よー・ぴっち・ばんく(ラジアン)
     * @param bIsIdentityMat  inSrcMatが単位行列か
     * @param inSrcMat        入力マトリクス
     */
    //-----------------------------------------------------------------------------
    static void GetYawPitchBankRotMatrixExtend(gfl2::math::MTX33* pDestMat, const ESolveAxisOrder eSolveAxisOrder, const gfl2::math::VEC3& inYPB, bool bIsIdentityMat = true, const gfl2::math::MTX33& inSrcMat = System::Camera::internal::MTX33_IDENTITY);

    //---------------------------------------------------------------------------------------------------------------------
    // マトリクス → よー・ぴっち・ばんく

    ///< 方向から一意のよー・ぴっちを取得できるかチェックする
    static bool IsYawPitchGetDirection(const gfl2::math::VEC3& inDir);

    //-----------------------------------------------------------------------------
    /**
     * @brief 方向からよーを取得する
     *
     * @param inDir 方向
     *
     * @return よー
     */
    //-----------------------------------------------------------------------------
    static float GetYawByDirection(const gfl2::math::VEC3& inDir);

    //-----------------------------------------------------------------------------
    /**
     * @brief 方向からぴっちを取得する
     *
     * @param inDir 方向
     *
     * @return ぴっち
     */
    //-----------------------------------------------------------------------------
    static float GetPitchByDirection(const gfl2::math::VEC3& inDir) { return CUtil::GetASinLimitClip(-inDir.y); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 方向からよー・ぴっちを取得する
     *
     * @param pfYaw   よー
     * @param pfPitch ぴっち
     * @param inDir   方向
     * 
     * @retval true  一意の値を取得できた
     * @retval false 一意の値を取得できなかった
     */
    //-----------------------------------------------------------------------------
    static bool GetYawPitchByDirection(float* pfYaw, float* pfPitch, const gfl2::math::VEC3& inDir);

    //-----------------------------------------------------------------------------
    /**
     * @brief マトリクスからばんくを取得する
     *
     * @param inMat マトリクス
     * 
     * @return ばんく
     */
    //-----------------------------------------------------------------------------
    static float GetBankByMatrix(const gfl2::math::MTX33& inMat);

    //-----------------------------------------------------------------------------
    /**
     * @brief マトリクスからよー・ぴっち・ばんくを取得する
     *
     * @param pYPB  出力よー・ぴっち・ばんく(ラジアン)
     * @param inMat 入力マトリクス
     */
    //-----------------------------------------------------------------------------
    static void GetYawPitchBankByMatrix(gfl2::math::VEC3* pYPB, const gfl2::math::MTX33& inMat);

    //-----------------------------------------------------------------------------
    /**
     * @brief マトリクスからよー・ぴっち・ばんくを取得するの拡張版
     *
     * @param pYPB            出力よー・ぴっち・ばんく(ラジアン)
     * @param eSolveAxisOrder 解決軸順番
     * @param inMat           入力マトリクス
     */
    //-----------------------------------------------------------------------------
    static void GetYawPitchBankByMatrixExtend(gfl2::math::VEC3* pYPB, const ESolveAxisOrder eSolveAxisOrder, const gfl2::math::MTX33& inMat);
  };

} // namespace field
} } // namespace System namespace Camera

#endif // __CAMERA_SPACE_H__

/*  EOF  */





