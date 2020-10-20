
//=============================================================================
/**
 * @file   ModelInFrame.cpp
 * @brief  枠の中にモデルを描画制御クラス
 * @author 池田 哲平
 * @date   2013.01.08
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#include <model/include/gfl2_BaseCamera.h>
#include "PokeTool/include/PokeModel.h"
#include "PokeTool/source/PokeModelLocal.h"
#include "field/CameraSpace.h"
#include "internal/CameraMisc.h"

#include "System/include/Camera/ModelInFrame.h"
#include "field/CameraDebugPrimitive.h" 

// #define MODEL_IN_FRAME_DRAW_DEBUG

namespace System { namespace Camera
{
  //-----------------------------------------------------------------------------
  // SRTを設定する
  //-----------------------------------------------------------------------------
  void CModelInFrame::SetSRT(PokeTool::PokeModel* pModel, const gfl2::math::VEC3& inPosi, const float fYaw, const float __fFrameRatioX__, const float __fFrameRatioY__, const poke_3d::model::BaseCamera& inCamera, const float fMulScale,
    gfl2::math::MTX34* pCameraMat, gfl2::math::MTX34* pCameraMatInv, float* pfScreenLenY)
  {
    float fFrameRatioX = __fFrameRatioX__, fFrameRatioY = __fFrameRatioY__, fAABBYaw = 0.0f;
    {
      const PokeTool::PokeSettingData* pSettingData = pModel->GetSettingData();
      if( pSettingData )
      {
        fAABBYaw = GFL_MATH_DEG_TO_RAD(pSettingData->camRotate);

        const float fScale = pSettingData->camSize; // 各モデル毎の枠スケールを反映
        fFrameRatioX *= fScale;
        fFrameRatioY *= fScale;
      }
    }

    // 引数で送られてきたスケールを反映
    fFrameRatioX *= fMulScale;
    fFrameRatioY *= fMulScale;

    // 右手系のビュー空間は視線方向がZ-となっているので。
    // 低負荷で反転させることはできるだろうが、気づき辛いミスをする可能性とそこまで速度を求められていないであろうと判断して確実に算出できる方法を選択する
    // gfl::math::MTX34 inViewMat; inCamera.GetViewMatrix(&inViewMat);

    gfl2::math::MTX34 inCameraMat, inCameraMatInv;
    gfl2::math::VEC3  inCameraPosi, inLookAtPosi, inCameraUpVec;
    gfl2::math::Vector3  vector3_inCameraPosi, vector3_inLookAtPosi, vector3_inCameraUpVec;
    vector3_inCameraPosi  = inCamera.GetPosition();
    vector3_inLookAtPosi  = inCamera.GetTargetPosition();
    vector3_inCameraUpVec = inCamera.GetUpVector();
    inCameraPosi  = gfl2::math::ConvertGfVec3ToNwVec3(vector3_inCameraPosi );
    inLookAtPosi  = gfl2::math::ConvertGfVec3ToNwVec3(vector3_inLookAtPosi );
    inCameraUpVec = gfl2::math::ConvertGfVec3ToNwVec3(vector3_inCameraUpVec);
    field::CSpace::CreateCameraMatrix(&inCameraMat, inCameraPosi, inLookAtPosi, inCameraUpVec);
    gfl2::math::MTX34InverseOrtho(&inCameraMatInv, &inCameraMat);

    float fScreenLenHalfX, fScreenLenHalfY;
    gfl2::math::VEC3 inPosiView; gfl2::math::VEC3Transform(&inPosiView, &inCameraMatInv, &inPosi);
    field::CUtil::GetScreenLengthHalf3D(&fScreenLenHalfX, &fScreenLenHalfY, inPosiView.z, inCamera.GetFovy(), inCamera.GetAspectRatio());
    const float fFrameLenHalfX = fScreenLenHalfX*fFrameRatioX;
    const float fFrameLenHalfY = fScreenLenHalfY*fFrameRatioY;

    pModel->SetScale(1.0f, 1.0f, 1.0f);
    pModel->SetRotation(0.0f, fYaw, 0.0f);
    pModel->SetOffset(gfl2::math::Vector3::GetZero());

    gfl2::math::AABB inAABB;
    if( GetBoundingAABB(&inAABB, pModel, fAABBYaw) == true )
    {
      // まず境界箱が戦闘用とかわいがり用で同じものを使用していて、つまり
      // 戦闘で浮いているものとかわいがりで降りているもので同じものが使用される。
      // そのため上下オフセットは不具合の元となるので処理に加えない。
      // スケーリングのみで対処する。
#if 0
      inAABB.min.y = 0.0f; // この設定でスケールが小さくなりすぎる時がある
      inAABB.Transform(inCameraMatInv);

      gfl::math::VEC3 inAABBLen; inAABB.GetLength(&inAABBLen);
      float fScale = fFrameLenX/inAABBLen.x;
      field::CUtil::MaxClip(&fScale, fFrameLenY/inAABBLen.y);
#else
      // CModelViewerInFrameと同じスケールになるよう変更
      float fScale = GetScaleX(inAABB, fFrameLenHalfX);
      field::CUtil::MaxClip(&fScale, GetScaleY(NULL, inAABB, fFrameLenHalfY));
#endif
      pModel->SetScale(fScale, fScale, fScale);
    }

    gfl2::math::Vector3 vector3_inPosi = gfl2::math::ConvertNwVec3ToGfVec3(inPosi);
    pModel->SetPosition(vector3_inPosi);

    if( pCameraMat )
    {
      (*pCameraMat)    = inCameraMat;
      (*pCameraMatInv) = inCameraMatInv;
      (*pfScreenLenY)  = fScreenLenHalfY*2.0f;
    }

#ifdef MODEL_IN_FRAME_DRAW_DEBUG
    if( GetBoundingAABB(&m_inDebugAABB, pModel, fAABBYaw) == false ) m_inDebugAABB.Set(gfl2::math::Vector4::GetZero());
    m_inDebugAABB.Offset(gfl2::math::Vector4(vector3_inPosi));
#endif
  }

  //-----------------------------------------------------------------------------
  // SRTを設定する(頭頂部合わせる版)
  //-----------------------------------------------------------------------------
  void CModelInFrame::SetSRTAdjustTop(PokeTool::PokeModel* pModel, const gfl2::math::Vector3& inPosi, const float fYaw, const gfl2::math::Vector3& inTopPosiOfst,
    const EScreen eScreen, const int nFrameLenX, const int nFrameLenY, const int nAdjustPosiY, const poke_3d::model::BaseCamera& inCamera, const float fMulScale)
  {
    float fScreenWidth, fScreenHeight; GetScreenWidthHeight(&fScreenWidth, &fScreenHeight, eScreen);

    // ScaleとYawを設定
    gfl2::math::MTX34 inCameraMat, inCameraMatInv;
    float fScreenHeightView;
    gfl2::math::VEC3 vec3_inPosi = gfl2::math::ConvertGfVec3ToNwVec3(inPosi);
    SetSRT(pModel, vec3_inPosi, fYaw, nFrameLenX/fScreenWidth, nFrameLenY/fScreenHeight, inCamera, fMulScale, &inCameraMat, &inCameraMatInv, &fScreenHeightView);


    // カメラマトリクスをかけると、inPosi + inTopPosiOfst位置にくるような座標を求める
    gfl2::math::Vector3 inTopPosiView = inPosi + inTopPosiOfst*pModel->GetScale().x;
    gfl2::math::VEC3 vec3_inTopPosiView = gfl2::math::ConvertGfVec3ToNwVec3(inTopPosiView);
    gfl2::math::VEC3Transform(&vec3_inTopPosiView, &inCameraMatInv, &vec3_inTopPosiView);

    const float fAdjustPosiViewY = fScreenHeightView*(0.5f - (static_cast<float>(nAdjustPosiY)/fScreenHeight));
    const float fPosiOfstY       = fAdjustPosiViewY - vec3_inTopPosiView.y;

    gfl2::math::VEC3 inAxisY; gfl2::math::MTX34GetAxisY(&inAxisY, inCameraMat);
    const gfl2::math::VEC3 inOfst = inAxisY*fPosiOfstY;
    const gfl2::math::VEC3 inDestPosi = vec3_inPosi + inOfst;
    gfl2::math::Vector3 vector3_inDestPosi = gfl2::math::ConvertNwVec3ToGfVec3(inDestPosi);
    pModel->SetPosition(vector3_inDestPosi);

#ifdef MODEL_IN_FRAME_DRAW_DEBUG
    gfl2::math::Vector4 vector4_inOfst(inOfst.x, inOfst.y, inOfst.z);
    m_inDebugAABB.Offset(inOfst);
#endif
  }

  //-----------------------------------------------------------------------------
  // 境界AABBを取得する
  //-----------------------------------------------------------------------------
  bool CModelInFrame::GetBoundingAABB(gfl2::math::AABB* pAABB, PokeTool::PokeModel* pModel, const float fYaw, const bool bIsTouchGround)
  {
    /*
      bool GetMotionCollision(gfl::math::AABB* box); 現在のアニメーションのコリジョンを取ります。
      bool GetMotionCollision(gfl::math::AABB* box, POKEMODEL_ANIME type); アニメーションの定義を指定して取得しますが、バトルのアニメセットが読まれているか、
                                                                           可愛がりのアニメセットが呼ばれているかで、設定する値が変わります。
      bool GetMotionCollisionDirect(gfl::math::AABB* box , int type = -1); モーションの管理番号での指定です。waitは必ず0番にあるので、これを使うのが確実かと思います。
    */
    gfl2::math::AABB inTemp;
    if( pModel && pModel->GetMotionCollisionDirect(&inTemp, 0) == true ) 
    {
      *pAABB = inTemp;
      gfl2::math::MTX33 inRotMat; field::CSpace::GetYawRotMatrix(&inRotMat, fYaw);

      // 最初の方法が正しくはあるが、回転すると隙間が目立ちすぎるので、少しはみでることを許容して小さい境界AABBを作成する
#if 0
      gfl::math::MTX34 inRotMat34; gfl::math::MTX34SetMTX33VEC3(&inRotMat34, inRotMat, gfl::math::VEC3_ZERO);
      pAABB->Transform(inRotMat34);
#else
      gfl2::math::VEC3 ainPosi[] = { gfl2::math::VEC3(pAABB->GetMin().x, 0.0f, 0.0f), gfl2::math::VEC3(pAABB->GetMax().x, 0.0f, 0.0f), gfl2::math::VEC3(0.0f, 0.0f, pAABB->GetMin().z), gfl2::math::VEC3(0.0f, 0.0f, pAABB->GetMax().z) };
      for( int i = 0; i < GET_ARRAY_NUMBER(ainPosi); i++ ) gfl2::math::VEC3Transform(&ainPosi[i], &inRotMat, &ainPosi[i]);

      gfl2::math::VEC3 inMin(ainPosi[0]), inMax(ainPosi[0]);
      for( int i = 1; i < GET_ARRAY_NUMBER(ainPosi); i++ )
      {
        if     ( inMin.x > ainPosi[i].x ) inMin.x = ainPosi[i].x;
        else if( inMax.x < ainPosi[i].x ) inMax.x = ainPosi[i].x;

        if     ( inMin.z > ainPosi[i].z ) inMin.z = ainPosi[i].z;
        else if( inMax.z < ainPosi[i].z ) inMax.z = ainPosi[i].z;
      }

      gfl2::math::Vector4 aabbMin = pAABB->GetMin();
      gfl2::math::Vector4 aabbMax = pAABB->GetMax();

      aabbMin.x = inMin.x;
      aabbMin.z = inMin.z;
      
      aabbMax.x = inMax.x;
      aabbMax.z = inMax.z;

      pAABB->SetMin(aabbMin);
      pAABB->SetMax(aabbMax);
#endif

      if( bIsTouchGround == true )
      {
        aabbMax.y -= aabbMin.y;
        aabbMin.y  = 0.0f;
      
        pAABB->SetMin(aabbMin);
        pAABB->SetMax(aabbMax);
      }

      return (System::Camera::internal::IsAABBVolumeZero(*pAABB) == false);
    }

    return false;
  }

  //-----------------------------------------------------------------------------
  // X軸スケール値を取得する
  //-----------------------------------------------------------------------------
  float CModelInFrame::GetScaleX(const gfl2::math::AABB& inAABB, const float fFrameLenHalf)
  {
    const float fMaxAbs  = gfl2::math::FAbs(inAABB.GetMax().x);
    const float fMinAbs  = gfl2::math::FAbs(inAABB.GetMin().x);
    const float fLenHalf = (fMaxAbs > fMinAbs) ? fMaxAbs : fMinAbs;
    if( fLenHalf > 0.0f ) return fFrameLenHalf/fLenHalf;
    else                  return 1.0f;
  }

  //-----------------------------------------------------------------------------
  // Y軸スケール値を取得する
  //-----------------------------------------------------------------------------
  float CModelInFrame::GetScaleY(float* pfLenHalf, const gfl2::math::AABB& inAABB, const float fFrameLenHalf)
  {
    const float fLenHalf = (inAABB.GetMax().y - inAABB.GetMin().y)*0.5f;
    if( pfLenHalf) (*pfLenHalf) = fLenHalf;

    if( fLenHalf > 0.0f ) return fFrameLenHalf/fLenHalf;
    else                  return 1.0f;
  }

#if PM_DEBUG

  gfl2::math::AABB CModelInFrame::m_inDebugAABB(gfl2::math::Vector4::GetZero());

  //-----------------------------------------------------------------------------
  // デバッグ描画
  //-----------------------------------------------------------------------------
  void CModelInFrame::DrawDebug()
  {
#ifdef MODEL_IN_FRAME_DRAW_DEBUG
    field::CDebugPrim::DrawAABB(m_inDebugAABB, field::VEC4_COLOR_RED, true);
#endif
  }

#endif

} // namespace Camera
} // namespace System

/*  EOF  */





