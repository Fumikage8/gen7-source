
//=============================================================================
/**
 * @file   CameraDebugPrimitive.h
 * @brief  デバッグプリミティブ描画サポートクラスヘッダ
 * @author 池田 哲平
 * @date   2012.05.30
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.27
 */
//=============================================================================

#if !defined( __CAMERA_DEBUG_PRIMITIVE_H__ )
#define __CAMERA_DEBUG_PRIMITIVE_H__
#pragma once

#include <math/include/gfl2_math.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include "System/source/Camera/internal/CameraMisc.h"
#include "CameraUtility.h"

namespace poke_3d { namespace model
{
class BaseCamera;
}
}

namespace System { namespace Camera
{
class InFrameCamera;
}
}

namespace System { namespace Camera { namespace field
{
  //-----------------------------------------------------------------------------
  /**
   * @brief デバッグプリミティブの描画をサポートするクラス
   * @brief 基本ラインでグリッドや矢印などデバッグ時に重宝するものを簡単に描画する。
   */
  //-----------------------------------------------------------------------------
  class CDebugPrim
  {
  public:
    /// 頂点クラス
    class CVertex
    {
    public:
      gfl2::math::VEC3 m_inPosi;
      gfl2::math::VEC4 m_inColor;

    public:
      CVertex() : m_inPosi(System::Camera::internal::VEC3_ZERO), m_inColor(VEC4_COLOR_WHITE) { return ; }
      CVertex(const gfl2::math::VEC3& inPosi, const gfl2::math::VEC4& inColor) : m_inPosi(inPosi), m_inColor(inColor) { return ; }
    };

#if PM_DEBUG

  private:
    static gfl2::gfx::DrawUtil::VertexFormat m_ainVertexBuf[6]; ///< 頂点バッファ
    //static gfl2::math::VEC3 m_inCamPosi; ///< カメラ位置  // [niji注] nijiでは使っていないのでコメントアウト。
    //static gfl2::math::VEC3 m_inCamDir;  ///< カメラの向き  // [niji注] nijiでは使っていないのでコメントアウト。

  public:
    static bool Begin(const poke_3d::model::BaseCamera& inCamera, const gfl2::gfx::CtrDisplayNo displayNo = gfl2::gfx::CtrDisplayNo::LEFT, const bool bIsExchangeXY = true); ///< 描画開始
    static bool Begin(const System::Camera::InFrameCamera& inCamera, const gfl2::gfx::CtrDisplayNo displayNo = gfl2::gfx::CtrDisplayNo::LEFT, const bool bIsExchangeXY = true); ///< 描画開始
    static void End(); ///< 描画終了

    static bool BeginProjSpace(const gfl2::gfx::CtrDisplayNo displayNo = gfl2::gfx::CtrDisplayNo::LEFT); ///< 描画開始(プロジェクション空間版)
    static void EndProjSpace() { End(); } ///< 描画終了(プロジェクション空間版)

    //-----------------------------------------------------------------------------
    /**
     * @brief 線を描画する
     *
     * @param inSrcPosi  始点
     * @param inDestPosi 終点
     * @param inColor    色
     */
    //-----------------------------------------------------------------------------
    static inline void DrawLine(const gfl2::math::VEC3& inSrcPosi, const gfl2::math::VEC3& inDestPosi, const gfl2::math::VEC4& inColor) { DrawLine(CVertex(inSrcPosi, inColor), CVertex(inDestPosi, inColor)); }
    static void DrawLine(const CVertex& inSrc, const CVertex& inDest);

    //-----------------------------------------------------------------------------
    /**
     * @brief 複数の繋がった線を描画する
     *
     * @param ainPosi  各線の位置配列
     * @param nPosiNum 位置配列数
     * @param inColor  色
     */
    //-----------------------------------------------------------------------------
    static void DrawLineList(const gfl2::math::VEC3 ainPosi[], const int nPosiNum, const gfl2::math::VEC4& inColor);
    static void DrawLineList(const CVertex ainVertex[], const int nVertexNum);

    //-----------------------------------------------------------------------------
    /**
     * @brief NULLを描画する
     *
     * @param inPosi  位置
     * @param fSize   大きさ
     * @param inColor 色
     */
    //-----------------------------------------------------------------------------
    static void DrawNull(const gfl2::math::VEC3& inPosi, const float fSize, const gfl2::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 三角形ポリゴンを描画する
     *
     * @param inPosi1 頂点位置１
     * @param inPosi2 頂点位置２
     * @param inPosi3 頂点位置３
     * @param inColor 色
     */
    //-----------------------------------------------------------------------------
    static void DrawTrianglePolygon(const gfl2::math::VEC3& inPosi1, const gfl2::math::VEC3& inPosi2, const gfl2::math::VEC3& inPosi3, const gfl2::math::VEC4& inColor);
    static void DrawTrianglePolygonScale(const gfl2::math::VEC3& inPosi1, const gfl2::math::VEC3& inPosi2, const gfl2::math::VEC3& inPosi3, const gfl2::math::VEC4& inColor, const float fScale);

    //-----------------------------------------------------------------------------
    /**
     * @brief 四角形ポリゴンを描画する
     *
     * @param ainPosi 頂点位置配列(ストリップで描画できるようにアラインされていること)
     * @param inColor 色
     */
    //-----------------------------------------------------------------------------
    static void DrawQuadranglePolygon(const gfl2::math::VEC3 ainPosi[], const gfl2::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 四角形ポリゴンを描画する(プロジェクション空間版)
     *
     * @param ainPosi 頂点位置配列(-1～1。ストリップで描画できるようにアラインされていること)
     * @param inColor 色
     * @param fZ      深度値(0～-1。精度は遠くになるほど急激に低くなるので、初期値[-0.05f]でもかなり手前)
     */
    //-----------------------------------------------------------------------------
    static void DrawQuadranglePolygonProjSpace(const gfl2::math::VEC2 ainPosi[], const gfl2::math::VEC4& inColor, const float fZ = -0.05f);

#if 0
    // [niji注] nijiでは使っていないのでコメントアウト。
    //-----------------------------------------------------------------------------
    /**
     * @brief 格子を描画する
     *
     * @param nDivideX 横軸分割数
     * @param nDivideZ 奥軸分割数
     * @param fLengthX 横軸長
     * @param fLengthZ 奥軸長
     * @param inPosi   中心位置
     * @param inColor  色
     */
    //-----------------------------------------------------------------------------
    static void DrawGrid(const int nDivideX, const int nDivideZ, const float fLengthX, const float fLengthZ, const gfl::math::VEC3& inPosi, const gfl::math::VEC4& inColor)
    {
      gfl::math::MTX34 mat; gfl::math::MTX34Translate(&mat, &inPosi);
      DrawGrid(nDivideX, nDivideZ, fLengthX, fLengthZ, mat, inColor);
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 格子を描画する(回転可版)
     *
     * @param nDivideX   横軸分割数
     * @param nDivideZ   奥軸分割数
     * @param fLengthX   横軸長
     * @param fLengthZ   奥軸長
     * @param inWorldMat ワールド空間変換行列
     * @param inColor    色
     */
    //-----------------------------------------------------------------------------
    static void DrawGrid(const int nDivideX, const int nDivideZ, const float fLengthX, const float fLengthZ, const gfl::math::MTX34& inWorldMat, const gfl::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 矢印を描画する
     *
     * @param inPosi   位置
     * @param inDirMat ワールド方向変換行列
     * @param fLength  長さ
     * @param inColor  色
     */
    //-----------------------------------------------------------------------------
    static void DrawArrow(const gfl::math::VEC3& inPosi, const gfl::math::MTX33& inDirMat, const float fLength, const gfl::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 矢印を描画する
     *
     * @param inPosi  位置
     * @param inVec   向き(正規化は不要)
     * @param fLength 長さ
     * @param inColor 色
     */
    //-----------------------------------------------------------------------------
    static void DrawArrow(const gfl::math::VEC3& inPosi, const gfl::math::VEC3& inVec, const float fLength, const gfl::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 矢印を描画する(終点入力版)
     *
     * @param inSrcPosi  始点
     * @param inDestPosi 終点
     * @param inColor    色
     */
    //-----------------------------------------------------------------------------
    static void DrawArrow(const gfl::math::VEC3& inSrcPosi, const gfl::math::VEC3& inDestPosi, const gfl::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 空間軸を描画する
     *
     * @param inAxisX X軸方向
     * @param inAxisY Y軸方向
     * @param inAxisZ Z軸方向
     * @param inPosi  中心位置
     * @param fLength 軸の長さ
     */
    //----------------------------------------------------------------------------- 
    static void DrawSpaceAxis(const gfl::math::VEC3& inAxisX, const gfl::math::VEC3& inAxisY, const gfl::math::VEC3& inAxisZ, const gfl::math::VEC3& inPosi, const float fLength);

    //-----------------------------------------------------------------------------
    /**
     * @brief マトリクス33から空間軸を描画する
     *
     * @param inMat   入力マトリクス
     * @param inPosi  平行移動位置
     * @param fLength 軸の長さ
     */
    //----------------------------------------------------------------------------- 
    static void DrawSpaceAxisByMTX33(const gfl::math::MTX33& inMat, const gfl::math::VEC3& inPosi, const float fLength)
    {
      gfl::math::VEC3 inAxisX; gfl::math::MTX33GetAxisX(&inAxisX, inMat);
      gfl::math::VEC3 inAxisY; gfl::math::MTX33GetAxisY(&inAxisY, inMat);
      gfl::math::VEC3 inAxisZ; gfl::math::MTX33GetAxisZ(&inAxisZ, inMat);
      DrawSpaceAxis(inAxisX, inAxisY, inAxisZ, inPosi, fLength);
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief マトリクス34から空間軸を描画する
     *
     * @param inMat   入力マトリクス
     * @param fLength 軸の長さ
     */
    //----------------------------------------------------------------------------- 
    static void DrawSpaceAxisByMTX34(const gfl::math::MTX34& inMat34, const float fLength)
    {
      const gfl::math::MTX33 inMat33(inMat34);
      gfl::math::VEC3 inPosi; gfl::math::MTX34GetTranslate(&inPosi, inMat34);
      DrawSpaceAxisByMTX33(inMat33, inPosi, fLength);
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 平面を描画する
     *
     * @param inPosi    位置
     * @param inNormVec 法線ベクタ
     * @param fSize     大きさ
     * @param inColor   色
     * @param fUpOfst   上オフセット
     */
    //-----------------------------------------------------------------------------
    static void DrawPlane(const gfl::math::VEC3& inPosi, const gfl::math::VEC3& inNormVec, const float fSize, const gfl::math::VEC4& inColor, const float fUpOfst = 0.0f);
#endif

    //-----------------------------------------------------------------------------
    /**
     * @brief 円を描画する(単位行列の時はXY平面上に円が描かれる)
     *
     * @param inWorldMat ワールド空間変換マトリクス
     * @param fRadius    半径
     * @param inColor    色
     */
    //-----------------------------------------------------------------------------
    static void DrawCircle(const gfl2::math::MTX34& inWorldMat, const float fRadius, const gfl2::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 球を描画する
     *
     * @param inPosi  中心位置
     * @param fRadius 半径
     * @param inColor 色
     */
    //-----------------------------------------------------------------------------
    static void DrawSphere(const gfl2::math::VEC3& inPosi, const float fRadius, const gfl2::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 箱を描画する
     *
     * @param inWorldMat ワールド空間変換マトリクス
     * @param inLength   長さ
     * @param inColor    色
     */
    //-----------------------------------------------------------------------------
    static void DrawBox(const gfl2::math::MTX34& inWorldMat, const gfl2::math::VEC3& inLength, const gfl2::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief 箱を描画する(たくさん描きすぎて描画バッファがオーバーフローしてしまう時などに使用するシンプル版)
     *
     * @param inWorldMat ワールド空間変換マトリクス
     * @param inLength   長さ
     * @param inColor    色
     */
    //-----------------------------------------------------------------------------
    static void DrawBoxSimple(const gfl2::math::MTX34& inWorldMat, const gfl2::math::VEC3& inLength, const gfl2::math::VEC4& inColor);

    //-----------------------------------------------------------------------------
    /**
     * @brief AABBを描画する
     *
     * @param inAABB    AABB
     * @param inColor   色
     * @param bIsSimple シンプル描画フラグ
     */
    //-----------------------------------------------------------------------------
    static void DrawAABB(const gfl2::math::AABB& inAABB, const gfl2::math::VEC4& inColor, const bool bIsSimple = false);

    //-----------------------------------------------------------------------------
    /**
     * @brief AABBを描画する(サイズが０の時はNULLを描画する)
     *
     * @param inAABB    AABB
     * @param inColor   色
     * @param bIsSimple シンプル描画フラグ
     */
    //-----------------------------------------------------------------------------
    static void DrawAABBCheckSize(const gfl2::math::AABB& inAABB, const gfl2::math::VEC4& inColor, const bool bIsSimple = false);

    //-----------------------------------------------------------------------------
    /**
     * @brief エルミート曲線を描画する
     *
     * @param inSrcPosi     始点位置
     * @param inSrcTangent  始点の接線ベクタ
     * @param inDestPosi    終点位置
     * @param inDestTangent 終点の接線ベクタ
     * @param nDivideNum    分割数
     * @param inColor       色
     */
    //-----------------------------------------------------------------------------
    //static void DrawHermiteCurve(const gfl::math::VEC3& inSrcPosi, const gfl::math::VEC3& inSrcTangent, const gfl::math::VEC3& inDestPosi, const gfl::math::VEC3& inDestTangent, const int nDivideNum, const gfl::math::VEC4& inColor);  // [niji注] nijiでは使っていないのでコメントアウト。

    //-----------------------------------------------------------------------------
    /**
     * @brief 枠を描画する(プロジェクション空間版)
     *
     * @param inPosiMax    最大位置(-1～1)
     * @param inPosiMin    最少位置(-1～1)
     * @param fWeightX     縦線の太さ(0～2)
     * @param inColor      色
     * @param fAspectRatio アスペクト比率
     * @param fZ      　　 深度値(0～-1)
     */
    //-----------------------------------------------------------------------------
    static void DrawFrameProjSpace(const gfl2::math::VEC2& inPosiMax, const gfl2::math::VEC2& inPosiMin, const float fWeightX,
      const gfl2::math::VEC4& inColor, const float fAspectRatio = 400.0f/240.0f, const float fZ = -0.05f);

    /// 構図アシストラインを描画する
    static void DrawCameraCompositionAssistLine();

    //-----------------------------------------------------------------------------
    /**
     * @brief 暗転フィルタを描画する
     *
     * @param fRatio 暗転率(0～1)
     */
    //-----------------------------------------------------------------------------
    static void DrawDarkFilter(const float fRatio);

  private:
    static void  BeginCommonFirst(const gfl2::gfx::CtrDisplayNo displayNo);
    static void  BeginCommon();
    /*
    // [niji注] nijiではWin32版でOpenGLの機能を使ったライン描画しかしていないのでコメントアウト。
    static void  GetLineSideOffsetDirection(gfl::math::VEC3* pDir, const gfl::math::VEC3& inPosi, const gfl::math::VEC3& inLineVec);
    static float GetLineScale(const gfl::math::VEC3& inPosi);
    */

    static gfl2::math::Vector4 ConvertNwVec3ToGfVec4(const gfl2::math::VEC3& nwVec3);
    static gfl2::math::VEC3    ConvertGfVec4ToNwVec3(const gfl2::math::Vector4& gfVec4);

#else

  public:
    static bool IsNeedInit() { return false; }
    static void Init() { ; }
    static void Release() { ; }
    static bool Begin(const poke_3d::model::BaseCamera& inCamera, const gfl2::gfx::CtrDisplayNo displayNo = gfl2::gfx::CtrDisplayNo::LEFT, const bool bIsExchangeXY = true) { return false; }
    static void End() { ; }
    static bool BeginProjSpace(const gfl2::gfx::CtrDisplayNo displayNo = gfl2::gfx::CtrDisplayNo::LEFT) { return false; }
    static void EndProjSpace() { ; }

    static void DrawLine(const gfl2::math::VEC3& inSrcPosi, const gfl2::math::VEC3& inDestPosi, const gfl2::math::VEC4& inColor) { ; }
    static void DrawLine(const CVertex& inSrc, const CVertex& inDest) { ; }
    static void DrawLineList(const gfl2::math::VEC3 ainPosi[], const int nPosiNum, const gfl2::math::VEC4& inColor) { ; }
    static void DrawLineList(const CVertex ainVertex[], const int nVertexNum) { ; }
    static void DrawNull(const gfl2::math::VEC3& inPosi, const float fSize, const gfl2::math::VEC4& inColor) { ; }
    static void DrawTrianglePolygon(const gfl2::math::VEC3& inPosi1, const gfl2::math::VEC3& inPosi2, const gfl2::math::VEC3& inPosi3, const gfl2::math::VEC4& inColor) { ; }
    static void DrawQuadranglePolygon(const gfl2::math::VEC3 ainPosi[], const gfl2::math::VEC4& inColor) { ; }
    static void DrawQuadranglePolygonProjSpace(const gfl2::math::VEC2 ainPosi[], const gfl2::math::VEC4& inColor, const float fZ = -0.05f) { ; }
/*
    // [niji注] nijiでは使っていないのでコメントアウト。
    static void DrawGrid(const int nDivideX, const int nDivideZ, const float fLengthX, const float fLengthZ, const gfl::math::VEC3& inPosi, const gfl::math::VEC4& inColor) { ; }
    static void DrawGrid(const int nDivideX, const int nDivideZ, const float fLengthX, const float fLengthZ, const gfl::math::MTX34& inWorldMat, const gfl::math::VEC4& inColor) { ; }
    static void DrawArrow(const gfl::math::VEC3& inPosi, const gfl::math::MTX33& inDirMat, const float fLength, const gfl::math::VEC4& inColor) { ; }
    static void DrawArrow(const gfl::math::VEC3& inPosi, const gfl::math::VEC3& inVec, const float fLength, const gfl::math::VEC4& inColor) { ; }
    static void DrawArrow(const gfl::math::VEC3& inSrcPosi, const gfl::math::VEC3& inDestPosi, const gfl::math::VEC4& inColor) { ; }
    static void DrawSpaceAxis(const gfl::math::VEC3& inAxisX, const gfl::math::VEC3& inAxisY, const gfl::math::VEC3& inAxisZ, const gfl::math::VEC3& inPosi, const float fLength) { ; }
    static void DrawSpaceAxisByMTX33(const gfl::math::MTX33& inMat, const gfl::math::VEC3& inPosi, const float fLength) { ; }
    static void DrawSpaceAxisByMTX34(const gfl::math::MTX34& inMat34, const float fLength) { ; }
    static void DrawPlane(const gfl::math::VEC3& inPosi, const gfl::math::VEC3& inNormVec, const float fSize, const gfl::math::VEC4& inColor, const float fUpOfst = 0.0f) { ; }
*/
    static void DrawCircle(const gfl2::math::MTX34& inWorldMat, const float fRadius, const gfl2::math::VEC4& inColor) { ; }
    static void DrawSphere(const gfl2::math::VEC3& inPosi, const float fRadius, const gfl2::math::VEC4& inColor) { ; }
    static void DrawBox(const gfl2::math::MTX34& inWorldMat, const gfl2::math::VEC3& inLength, const gfl2::math::VEC4& inColor) { ; }
    static void DrawBoxSimple(const gfl2::math::MTX34& inWorldMat, const gfl2::math::VEC3& inLength, const gfl2::math::VEC4& inColor) { ; }
    static void DrawAABB(const gfl2::math::AABB& inAABB, const gfl2::math::VEC4& inColor, const bool bIsSimple = false) { ; }
    static void DrawAABBCheckSize(const gfl2::math::AABB& inAABB, const gfl2::math::VEC4& inColor, const bool bIsSimple = false) { ; }
    //static void DrawHermiteCurve(const gfl::math::VEC3& inSrcPosi, const gfl::math::VEC3& inSrcTangent, const gfl::math::VEC3& inDestPosi, const gfl::math::VEC3& inDestTangent, const int nDivideNum, const gfl::math::VEC4& inColor) { ; }  // [niji注] nijiでは使っていないのでコメントアウト。
    static void DrawFrameProjSpace(const gfl2::math::VEC2& inPosiMax, const gfl2::math::VEC2& inPosiMin, const float fWeightX, const gfl2::math::VEC4& inColor, const float fAspectRatio = 400.0f/240.0f, const float fZ = -0.05f) { ; }
    static void DrawCameraCompositionAssistLine() { ; }
    //static void DrawDarkFilter(const float fRatio) { ; }  // [niji注] nijiでは使っていないのでコメントアウト。

#endif
  };

} // namespace field
} } // namespace System namespace Camera

#endif // __CAMERA_DEBUG_PRIMITIVE_H__

/*  EOF  */



#if 0
    // 線三角形を描画する
    static void drawLineTriangle(const float fSize,const tiMatrix& inWorldMat,const tiVector& inColor);

    // 同線楕球
    static void drawLineEllipticalSphereFixVertexNum(const tiVector& inRadius,const tiMatrix& inWorldMat,const tiVector& inColor);

    // 同線円筒(fBottomDist:inPosiから円筒の底までの距離)
    static void drawLineCylinderFixVertexNum(const tiVector& inPosi,const tiVector& inUpVector,const float fBottomDist,const float fRadius,const float fLength,const tiVector& inColor);

    // 同線楕円筒
    static void drawLineEllipticalCylinderFixVertexNum(const tiMatrix& inWorldMat,const float fBottomDist,const float fRadiusX,const float fRadiusZ,const float fLength,const tiVector& inColor);
#endif





