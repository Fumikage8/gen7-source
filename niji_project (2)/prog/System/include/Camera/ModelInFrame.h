
//=============================================================================
/**
 * @file   ModelInFrame.h
 * @brief  枠の中にモデルを描画制御クラスヘッダ
 * @author 池田 哲平
 * @date   2013.01.07
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#if !defined( __MODEL_IN_FRAME_H__ )
#define __MODEL_IN_FRAME_H__
#pragma once

#include <math/include/gfl2_math.h>
#include <math/include/gfl2_MathAABB.h>

namespace poke_3d { namespace model
{
  class BaseCamera;
}
}

namespace PokeTool
{
  class PokeModel;
}

namespace System { namespace Camera
{
  //-----------------------------------------------------------------------------
  /**
   * @brief 枠の中にモデルを描画制御クラス
   * @brief 枠(スクリーン座標)の中にモデルを描画するよう制御する。
   * @brief モデルとカメラの位置を変えずにスケーリングだけで調整する。
   * @brief モデルとカメラのスケール・回転・位置を自由に変えてよくて、見映えのよいアングルで映したいとき(=UIとして使用するとき)は、
   * @brief CModelViewerInFrameクラスを使用すること。
   */
  //-----------------------------------------------------------------------------
  class CModelInFrame
  {
  public:
    /// 画面列挙型
    enum EScreen
    {
      SCREEN_UPPER , ///< 上画面
      SCREEN_LOWER , ///< 下画面
    };

  public:
    //-----------------------------------------------------------------------------
    /**
     * @brief 指定した位置に(概ね)指定した枠の大きさでモデルが描画されるようSRT(Scale + Rotation + Translation)を設定します。
     *        内部でいろいろとフィルタリングをかけていますので、pModel->Get～で値を取得して計算を行うと不具合が生じる可能性があります。
     *        必ず自分で値を保持しておいて、それを使用して計算を行うようにしてください。
     *
     * @param pModel     SRTの設定対象となるモデル
     * @param inPosi     モデル位置(ワールド空間)
     * @param fYaw       モデルＹ軸回転角度(ラジアン)
     * @param eScreen    描画するスクリーン
     * @param nFrameLenX 枠の長さＸ(スクリーン空間)
     * @param nFrameLenY 枠の長さＹ(スクリーン空間)
     * @param inCamera   カメラ
     * @param fMulScale  乗算スケール値
     */
    //-----------------------------------------------------------------------------
    static inline void SetSRT(PokeTool::PokeModel* pModel, const gfl2::math::Vector3& inPosi, const float fYaw, const EScreen eScreen, const int nFrameLenX, const int nFrameLenY, const poke_3d::model::BaseCamera& inCamera, const float fMulScale = 1.0f)
    {
      float fWidth, fHeight; GetScreenWidthHeight(&fWidth, &fHeight, eScreen);
      gfl2::math::VEC3 vec3_inPosi = gfl2::math::ConvertGfVec3ToNwVec3(inPosi);
      SetSRT(pModel, vec3_inPosi, fYaw, nFrameLenX/fWidth, nFrameLenY/fHeight, inCamera, fMulScale);
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 指定した位置に(概ね)指定した枠の大きさで、かつ指定したＹ座標位置に頭頂部がくる条件でモデルが描画されるようSRT(Scale + Rotation + Translation)を設定します。
     *        注意点はSetSRT()と同じです。
     *
     * @param pModel        SRTの設定対象となるモデル
     * @param inPosi        モデル位置(ワールド空間)
     * @param fYaw          モデルＹ軸回転角度(ラジアン)
     * @param inTopPosiOfst 頭頂位置オフセット(ワールド空間)
     * @param eScreen       描画するスクリーン
     * @param nFrameLenX    枠の長さＸ(スクリーン空間)
     * @param nFrameLenY    枠の長さＹ(スクリーン空間)
     * @param nAdjustPosiY  頭頂部を合わせるＹ座標位置(スクリーン空間)
     * @param inCamera      カメラ
     * @param fMulScale     乗算スケール値
     */
    //-----------------------------------------------------------------------------
    static void SetSRTAdjustTop(PokeTool::PokeModel* pModel, const gfl2::math::Vector3& inPosi, const float fYaw, const gfl2::math::Vector3& inTopPosiOfst,
      const EScreen eScreen, const int nFrameLenX, const int nFrameLenY, const int nAdjustPosiY, const poke_3d::model::BaseCamera& inCamera, const float fMulScale = 1.0f);

  private:
    static void SetSRT(PokeTool::PokeModel* pModel, const gfl2::math::VEC3& inPosi, const float fYaw, const float fFrameRatioX, const float fFrameRatioY, const poke_3d::model::BaseCamera& inCamera, const float fMulScale,
      gfl2::math::MTX34* pCameraMat = NULL, gfl2::math::MTX34* pCameraMatInv = NULL, float* pfScreenLenY = NULL);

  protected:
    static inline void GetScreenWidthHeight(float* pfWidth, float* pfHeight, const EScreen eScreen)
    {
      (*pfWidth)  = (eScreen == SCREEN_UPPER) ? 400.0f : 320.0f;
      (*pfHeight) = 240.0f;
    }
    static bool  GetBoundingAABB(gfl2::math::AABB* pAABB, PokeTool::PokeModel* pModel, const float fYaw, const bool bIsTouchGround = false);
    static float GetScaleX(const gfl2::math::AABB& inAABB, const float fFrameLenHalf);
    static float GetScaleY(float* pfLenHalf, const gfl2::math::AABB& inAABB, const float fFrameLenHalf);

#if PM_DEBUG
  private:
    static gfl2::math::AABB m_inDebugAABB;
  public:
    static void DrawDebug(); ///< デバッグ描画
#endif
  };

} // namespace Camera
} // namespace System

#endif // __MODEL_IN_FRAME_H__

/*  EOF  */





