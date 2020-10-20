
//=============================================================================
/**
 * @file   ModelViewerInFrame.cpp
 * @brief  枠の中におけるモデルビューア制御クラス
 * @author 池田 哲平
 * @date   2012.10.12
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include "PokeTool/include/PokeModel.h"
#include "PokeTool/source/PokeModelLocal.h"
#include "field/CameraUtility.h"
#include "field/CameraSpace.h"
#include "field/CameraInterpolate.h"
#include "internal/CameraMisc.h"
#include "internal/MathPlane.h"

#include "System/include/Camera/ModelViewerInFrame.h"
#include "System/include/Camera/InFrameCamera.h"
//#include "system/camera/CameraULCD.h"  // [niji注] nijiの立体視はどうする？
#include "field/CameraDebugPrimitive.h" 

namespace System { namespace Camera
{
  const float CModelViewerInFrame::VIEW_ANGLE = GFL_MATH_DEG_TO_RAD(30.0f);
  const float CModelViewerInFrame::SCREEN_DISTANCE_DEFAULT = UTIL_METER_TO_UNIT_BATTLE(4.0f); // 計算誤差や不具合の可能性を考慮してカメラを適度に離す

  //-----------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------
  CModelViewerInFrame::CModelViewerInFrame() :
  m_eScreen(SCREEN_UPPER) ,
  m_eTurnDir(TURN_DIRECTION_LEFT) ,
  m_eDrawPosi(DRAW_POSITION_BOTTOM) ,
  m_inBaseFramePosiLT(internal::VEC2_ZERO) ,
  m_inBaseFramePosiRB(internal::VEC2_ZERO) ,
  m_fFrameScale(1.0f) ,
  m_fFramePosiOfstRatioY(0.0f) ,
  m_pModel(NULL) ,
  m_fModelYaw(0.0f) ,
  m_fModelYawOfstR(0.0f) ,
  m_fCameraYaw(0.0f) ,
  m_fCameraPitch(0.0f) ,
  m_bIsULCD(true) ,
  m_fDepthRange(0.3f) ,
  m_fDepthLevel(SCREEN_DISTANCE_DEFAULT) ,
  m_fLimitParallax(0.0f/*CCameraULCD::LIMIT_PARALLAX_MAX*/) ,  // [niji注] nijiの立体視はどうする？
  m_bIsEnableDepthOffset(true) ,
  m_bIsTouchGround(false) ,
  m_fCenterOfstY(0.0f) ,
  m_eCenterOfstGetPhase(CENTER_OFFSET_GET_PHASE_END),
  m_userPosOffset(gfl2::math::VEC3(0.0f,0.0f,0.0f)),
  m_userScale(gfl2::math::VEC3(1.0f,1.0f,1.0f)),
  m_modelVisible(true)
  {
    ResetModelInitSRT();
  }

  //-----------------------------------------------------------------------------
  // モデルの初期化時のSRTをリセットします
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::ResetModelInitSRT()
  {
    m_inModelInitScale    = internal::VEC3_UNIT_SCALE;
    m_inModelInitRot      = internal::VEC3_ZERO;
    m_inModelInitPosi     = internal::VEC3_ZERO;
    m_inModelInitPosiOfst = internal::VEC3_ZERO;
  }

  //-----------------------------------------------------------------------------
  // 初期化
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::Init(const EScreen eScreen, const int nLeft, const int nTop, const int nRight, const int nBottom, const ETurnDirection eTurnDir)
  {
    m_eScreen  = eScreen;
    m_eTurnDir = eTurnDir;
    SetFrame(nLeft, nTop, nRight, nBottom);
    ResetModel();
  }

  //-----------------------------------------------------------------------------
  // 枠を(再)設定します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::SetFrame(const int nLeft, const int nTop, const int nRight, const int nBottom)
  {
    GFL_ASSERT_MSG( (nLeft < nRight && nTop < nBottom), "枠の設定が不正です。左(%d) < 右(%d) かつ 上(%d) < 下(%d) でないといけません。\n", nLeft, nRight, nTop, nBottom);
#if PM_DEBUG
    int width   = gfl2::math::Abs( nLeft - nRight );
    int height  = gfl2::math::Abs( nTop - nBottom );

    if( 2 < gfl2::math::Abs( width - height ) )
    {
      //  ぴったりの時は成功
      //  ぴったりじゃなくても指定した縦横の差が小さい場合は成功
      GFL_ASSERT_MSG( 0, "枠の横縦の値が正方形になっていません。 左(%d), 右(%d), 上(%d). 下(%d), 横(%d), 縦(%d) \n", nLeft, nRight, nTop, nBottom, width, height );
    }
#endif

    m_inBaseFramePosiLT.x = static_cast<float>(nLeft);
    m_inBaseFramePosiLT.y = static_cast<float>(nTop);
    m_inBaseFramePosiRB.x = static_cast<float>(nRight);
    m_inBaseFramePosiRB.y = static_cast<float>(nBottom);
  }

  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::Update(InFrameCamera* pCamera, SUpdateData* pData)
  {
    if( pCamera )
    {
      float fAspectRatio, fGroundHeight;
      gfl2::math::VEC2 inFramePosiMax, inFramePosiMin, inFrameLenHalf;
      // フレーム（枠）情報を取得
      GetFrameData(&fAspectRatio, &inFramePosiMax, &inFramePosiMin, &inFrameLenHalf, &fGroundHeight);

      // ポケモンモデルの中心座標を取得するには、1度Calculateを通る必要があるため、処理を分岐
      if( m_pModel )
      {
        switch( m_eCenterOfstGetPhase )
        {
        case CENTER_OFFSET_GET_PHASE_1:
          m_pModel->SetVisible(false); // 最初のフレームは正しい骨の位置を取れないので、不可視としてしのぐ
          m_eCenterOfstGetPhase = CENTER_OFFSET_GET_PHASE_2;
          break;

        case CENTER_OFFSET_GET_PHASE_2: {
          gfl2::math::Vector3 inNodePosi;
          if( m_pModel->GetEffectNodePosition(PokeTool::MODEL_NODE_CENTER, 0, &inNodePosi) == true )
          {
            gfl2::math::Vector3 inRootPosi = m_pModel->GetMergePosition();
            m_fCenterOfstY = (inNodePosi.y - inRootPosi.y)/GetScale();
          }

          m_pModel->SetVisible(m_modelVisible);
          m_eCenterOfstGetPhase = CENTER_OFFSET_GET_PHASE_END;
          break; }
        
        case CENTER_OFFSET_GET_PHASE_END:  // 最初の数フレームの位置設定が終わった後は、Visible設定を行わなければならない。(Visibleの制御はインフレームが行うので)
          m_pModel->SetVisible(m_modelVisible);
          break;
        }

        m_pModel->SetScale(1.0f, 1.0f, 1.0f);
        {
          // 左向きと右向きでモデルの大きさが変わってしまっては問題なので、SetRotate()のみ左右を反映させてその他は常にm_fModelYawを使用する
          float fYaw = m_fModelYaw;
          if     ( m_eTurnDir == TURN_DIRECTION_LEFT  ) { if( fYaw > 0.0f ) fYaw *= -1.0f; }
          else if( m_eTurnDir == TURN_DIRECTION_RIGHT )
          {
            if( fYaw < 0.0f ) fYaw *= -1.0f;
            fYaw += m_fModelYawOfstR;
          }
          m_pModel->SetRotation(0.0f, fYaw, 0.0f);
        }
        m_pModel->SetOffset(gfl2::math::Vector3::GetZero());

        gfl2::math::AABB inAABB;
        if( GetBoundingAABB(&inAABB, m_pModel, m_fModelYaw, m_bIsTouchGround) == true ) // スケール無しの基準となるAABBを取得する
        {
          float fLenHalfY;
          const float fScaleX = GetScaleX(inAABB, inFrameLenHalf.x);
          const float fScaleY = GetScaleY(&fLenHalfY, inAABB, inFrameLenHalf.y);

          float fScale = 1.0f, fPosiOfstYAdjust = 0.0f;
          if( fScaleX < fScaleY )
          {
            fScale = fScaleX;
            if( m_eDrawPosi == DRAW_POSITION_BOTTOM ) fPosiOfstYAdjust = inFrameLenHalf.y - fLenHalfY*fScaleX; // 枠の下端にAABBを沿わせるための調整値  // あると赤枠が灰枠内で下付けになる、ないと赤枠が灰枠内で中央付けになる
                // 赤枠はポケモンのボックス、灰枠は何だろう？、黒枠はスクリーンかな？  // 赤枠は灰枠内にあり、灰枠は黒枠内にある。
          }
          else
            fScale = fScaleY;

          m_pModel->SetScale(fScale, fScale, fScale);

          gfl2::math::VEC3 inPosiOfst;
          inPosiOfst.x = 0.0f; // xはそのまま
          inPosiOfst.y = -(inAABB.GetMax().y + inAABB.GetMin().y)*0.5f; // yは中心が原点に来るようオフセット
          inPosiOfst.z = (m_bIsEnableDepthOffset == true) ? -inAABB.GetMax().z : 0.0f; // zはスケーリングしても前面とXY平面が常に重なるようオフセット
          gfl2::math::VEC3Scale(&inPosiOfst, &inPosiOfst, fScale); // 回転 → 平行移動 → スケールの順に変換するので、平行移動にもスケールがかかる
          inPosiOfst.y -= fPosiOfstYAdjust;
          m_pModel->SetPosition(gfl2::math::ConvertNwVec3ToGfVec3(inPosiOfst));

          gfl2::math::VEC3 userScaleMul = fScale*m_userScale;
          m_pModel->SetScale(gfl2::math::ConvertNwVec3ToGfVec3(userScaleMul));  // 他の計算に影響を与えたくないので、最後にスケールを掛ける。
        }
        else
          m_pModel->SetPosition(0.0f, -inFrameLenHalf.y, 0.0f);
      }

      // カメラ・注視点位置取得
      gfl2::math::VEC3 inCameraPosi, inLookAtPosi;
      GetCameraAndLookAtPosition(&inCameraPosi, &inLookAtPosi);

      // 影更新
      UpdateShadow(fGroundHeight, inCameraPosi);

      // pCamera->UpdateCameraMatrix();

      // ビュー行列生成
      gfl2::math::MTX34 inViewMat;
      gfl2::math::MTX34LookAt(&inViewMat, &inCameraPosi, &internal::VEC3_UNIT_Y, &inLookAtPosi);

      // 射影行列生成(枠の位置によって見た目が変化しないように、中央で撮ってからプロジェクション空間レベルで枠内へとスライドする)
      gfl2::math::MTX44 inProjMat;
      gfl2::math::VEC2  inFramePosiCenter;
      {
        const float NEAR_VAL = UTIL_METER_TO_UNIT_BATTLE( 0.3f);
        const float FAR_VAL  = UTIL_METER_TO_UNIT_BATTLE(20.0f); // ファーが近いほど立体感がでる
        // 中心にとるプロジェクション行列
        gfl2::math::MTX44PerspectiveRad(&inProjMat, VIEW_ANGLE, fAspectRatio, NEAR_VAL, FAR_VAL);

        // フレームの中心まで移動させる。
        gfl2::math::VEC2Add(&inFramePosiCenter, &inFramePosiMax, &inFramePosiMin);
        gfl2::math::VEC2Scale(&inFramePosiCenter, &inFramePosiCenter, 0.5f);

        gfl2::math::VEC3 temp(inFramePosiCenter.x, inFramePosiCenter.y, 0.0f);
        gfl2::math::MTX44 inTranslateMat; gfl2::math::MTX44Translate(&inTranslateMat, &temp);
        gfl2::math::MTX44Mult(&inProjMat, &inTranslateMat, &inProjMat); // 単純に平行移動成分に加算するだけではうまくいかないっぽい

        // 下画面の時はPIVOT_UPSIDE_TO_TOPとしてもうまく動いてくれないので、こちらで対処する
        /*if( m_eScreen == SCREEN_LOWER )  // [niji注] この処理があると、Win32版では縦横ひっくり返り、CTR版では画面からいなくなってしまったので、コメントアウトした。
        {
          gfl2::math::MTX44 mat; field::CUtil::GetExchangeXYMatrix(&mat);
          gfl2::math::MTX44Mult(&inProjMat, &mat, &inProjMat);
        }*/
      }

      //pCamera->SetCameraAndTargetPosition(inCameraPosi, inLookAtPosi);
      //pCamera->SetUpVector(gfl::math::VEC3_UNIT_Y);
      //pCamera->SetFovy(VIEW_ANGLE);
      //pCamera->SetAspect(fAspectRatio);

      gfl2::math::Matrix34 matrix34_inViewMat = gfl2::math::ConvertNwMtx34ToGfMtx34(inViewMat);
      gfl2::math::Matrix44 matrix44_inProjMat = gfl2::math::ConvertNwMtx44ToGfMtx44(inProjMat);

      pCamera->SetViewMatrix(matrix34_inViewMat);
      pCamera->SetProjectionMatrix(matrix44_inProjMat);

      // 立体視は上画面だけ
      if( m_eScreen == SCREEN_UPPER )
      {
        // スクリーンまでの距離はデフォルト値だとカメラが回転したときにうまくいかないケースが生じるかもしれないが、それでもレアケースであろうと推測してとりあえずそのまま
        /*
        // [niji注] nijiの立体視はどうする？
        if( m_bIsULCD ) CCameraULCD::Set(pCamera, m_fDepthRange, m_fDepthLevel, m_fLimitParallax);
        else            CCameraULCD::Set(pCamera, 0.0f);
        */

        //pCamera->UpdateCameraMatrices(false);
      }

      if( pData )
      {
        pData->m_inViewMat    = matrix34_inViewMat;
        pData->m_fViewAngle   = VIEW_ANGLE;
        pData->m_fAspectRatio = fAspectRatio;
        pData->m_inCenterPosi = gfl2::math::ConvertNwVec2ToGfVec2(inFramePosiCenter);
      }
    }
  }

  //-----------------------------------------------------------------------------
  // スケールを取得します
  //-----------------------------------------------------------------------------
  float CModelViewerInFrame::GetScale() const { return m_pModel->GetScale().x; }

  //-----------------------------------------------------------------------------
  // 枠に関するデータを取得します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::GetFrameData(float* pfAspectRatio, gfl2::math::VEC2* pFramePosiMax, gfl2::math::VEC2* pFramePosiMin, gfl2::math::VEC2* pFrameLenHalf, float* pfGroundHeight)
  {
    float fScreenWidth, fScreenHeight; GetScreenWidthHeight(&fScreenWidth, &fScreenHeight, m_eScreen);
    (*pfAspectRatio) = fScreenWidth/fScreenHeight; // アスペクト比率

    gfl2::math::VEC2 inFramePosiLT, inFramePosiRB;
    GetFrameScreenPosition(&inFramePosiLT, &inFramePosiRB);

    // 枠位置最大(プロジェクション空間)
    pFramePosiMax->x = inFramePosiRB.x;
    pFramePosiMax->y = inFramePosiLT.y;

    // 枠位置最少(プロジェクション空間)
    pFramePosiMin->x = inFramePosiLT.x;
    pFramePosiMin->y = inFramePosiRB.y;

    gfl2::math::VEC2* apPosi[] = { pFramePosiMax, pFramePosiMin };
    for( int i = 0; i < GET_ARRAY_NUMBER(apPosi); i++ )
    {
      apPosi[i]->x =  UTIL_ZERO_ONE_TO_MINUS_ONE_ONE(apPosi[i]->x/fScreenWidth);
      apPosi[i]->y = -UTIL_ZERO_ONE_TO_MINUS_ONE_ONE(apPosi[i]->y/fScreenHeight);
    }
    // GFL_PRINT("max[%.3f %.3f] min[%.3f %.3f] \n", m_inFramePosiMax.x, m_inFramePosiMax.y, m_inFramePosiMin.x, m_inFramePosiMin.y);

    // 枠の長さの半分(ビュー空間)
    float fLenHalfX, fLenHalfY;
    field::CUtil::GetScreenLengthHalf3D(&fLenHalfX, &fLenHalfY, SCREEN_DISTANCE_DEFAULT, VIEW_ANGLE, *pfAspectRatio);
    gfl2::math::VEC2Sub(pFrameLenHalf, pFramePosiMax, pFramePosiMin);
    gfl2::math::VEC2Scale(pFrameLenHalf, pFrameLenHalf, 0.5f);
    pFrameLenHalf->x *= fLenHalfX;
    pFrameLenHalf->y *= fLenHalfY;

    // 地面の高さ
    if( pfGroundHeight )
    {
      (*pfGroundHeight) = -( ( ((inFramePosiRB.y - inFramePosiLT.y)/fScreenHeight)*fLenHalfY ) +
                             ( ((m_inBaseFramePosiRB.y - inFramePosiRB.y)/fScreenHeight)*fLenHalfY*2.0f ) );
    }
  }

  //-----------------------------------------------------------------------------
  // 枠のスクリーン位置を取得します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::GetFrameScreenPosition(gfl2::math::VEC2* pFramePosiLT, gfl2::math::VEC2* pFramePosiRB)
  {
    // X
    {
      const float fCenter  = field::GetCenterValue(m_inBaseFramePosiLT.x, m_inBaseFramePosiRB.x);
      const float fLenHalf = (m_inBaseFramePosiRB.x - fCenter)*m_fFrameScale;
      pFramePosiLT->x = fCenter - fLenHalf;
      pFramePosiRB->x = fCenter + fLenHalf;
    }

    // Y
    {
      if( m_eDrawPosi == DRAW_POSITION_BOTTOM )  // こっちだと灰枠が黒枠内で下付けになる、else側だと灰枠が黒枠内で中央付けになる
      {
        const float fBaseLen = m_inBaseFramePosiRB.y - m_inBaseFramePosiLT.y;
        pFramePosiRB->y = m_inBaseFramePosiRB.y;
        if( m_bIsTouchGround == false ) pFramePosiRB->y -= fBaseLen*m_fFramePosiOfstRatioY;
        pFramePosiLT->y = pFramePosiRB->y - fBaseLen*m_fFrameScale;
      }
      else /* DRAW_POSITION_CENTER */
      {
        const float fCenter  = field::GetCenterValue(m_inBaseFramePosiLT.y, m_inBaseFramePosiRB.y);
        const float fLenHalf = (m_inBaseFramePosiRB.y - fCenter)*m_fFrameScale;
        pFramePosiLT->y = fCenter - fLenHalf;
        pFramePosiRB->y = fCenter + fLenHalf;
      }
    }
  }

  //-----------------------------------------------------------------------------
  // ワールド空間基準の境界AABBを取得します
  //-----------------------------------------------------------------------------
  bool CModelViewerInFrame::GetBoundingAABBWorld(gfl2::math::AABB* pAABB)
  {
    if( GetBoundingAABB(pAABB, m_pModel, m_fModelYaw, m_bIsTouchGround) == true )
    {
      gfl2::math::Vector3 inPosi; inPosi = m_pModel->GetMergePosition();
      *pAABB = pAABB->Offset(gfl2::math::Vector4(inPosi));
      return true;
    }
    else
      return false;
  }

  //-----------------------------------------------------------------------------
  // 影を更新します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::UpdateShadow(const float fGroundHeight, const gfl2::math::VEC3& inCameraPosi)
  {
    if( m_pModel && m_pModel->GetShadowModel() != NULL )
    {
      /////////////////////////////////////////////////////////////////////
      // nijiではインフレームで影は表示しないことになった
      m_pModel->GetShadowModel()->SetVisible(false);
      return;
      /////////////////////////////////////////////////////////////////////


      /*
      @fix GFCHECK[8230]  ワーニング対処
      nijiではインフレームで影は表示しないので、以下の処理は不要となり手前でreturnしているので、この部分はコメントアウトしておく。

      //m_pModel->SetShadowHeight(fGroundHeight);  // @attention nijiは影なし  todo 影がちゃんと出るようになったら何かしら対応する

      const float YAW_DEG   = 166.0f; // 光源の横角度(-180～180)
      const float PITCH_DEG = 17.0f;  // 光源の縦角度(0～90)

      float fYaw = GFL_MATH_DEG_TO_RAD(YAW_DEG); if( m_eTurnDir == TURN_DIRECTION_RIGHT ) fYaw *= -1.0f;
      const float fPitch = GFL_MATH_DEG_TO_RAD(PITCH_DEG);
      gfl2::math::VEC3 inDir; field::CSpace::GetDirectionByYawPitch(&inDir, fYaw, fPitch);
      //m_pModel->SetShadowDir(inDir);  // @attention nijiは影なし  todo 影がちゃんと出るようになったら何かしら対応する
      // GFL_PRINT("%.3f %.3f %.3f \n", inDir.x, inDir.y, inDir.z);

      // 地面にフラットだと映りがよくないので、少しだけ上下回転を加える
      const float PITCH_OFST = GFL_MATH_DEG_TO_RAD(-2.0f);
      gfl2::math::MTX33 inYawMat; field::CSpace::GetYawRotMatrix(&inYawMat, fYaw);
      gfl2::math::VEC3  inAxis;   gfl2::math::MTX33GetAxisX(&inAxis, inYawMat);
      gfl2::math::MTX33 inRotMat; gfl2::math::MTX33RotAxisRad(&inRotMat, &inAxis, PITCH_OFST);
      gfl2::math::MTX34 inRotMat34; gfl2::math::MTX34SetMTX33VEC3(&inRotMat34, inRotMat, internal::VEC3_ZERO);
      //m_pModel->SetShadowMatrix(inRotMat34);  // @attention nijiは影なし  todo 影がちゃんと出るようになったら何かしら対応する

      // 射影面の下にカメラがあるときは描画しない
      gfl2::math::VEC3 inNormDir; gfl2::math::MTX33GetAxisY(&inNormDir, inRotMat);
      const internal::Plane inPlane(inNormDir, gfl2::math::VEC3(0.0f, fGroundHeight, 0.0f));
      m_pModel->GetShadowModel()->SetVisible(inPlane.GetDistance(inCameraPosi) > 0.0f);     
      */
    }
  }

  //-----------------------------------------------------------------------------
  // カメラ・注視点位置を取得します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::GetCameraAndLookAtPosition(gfl2::math::VEC3* pCameraPosi, gfl2::math::VEC3* pLookAtPosi)
  {
    float fDistToLookAt = SCREEN_DISTANCE_DEFAULT;
    (*pLookAtPosi) = internal::VEC3_ZERO;

    gfl2::math::Vector3 inCenter(gfl2::math::Vector3::GetZero());
#if 0
    gfl::math::AABB_VEC3 inAABB;
    if( GetBoundingAABBWorld(&inAABB) ) inAABB.GetCenter(&inCenter);
#else
    // ルートポジションのy z　を注視する。
    if( m_pModel )
    {
      inCenter = m_pModel->GetMergePosition(); // 立体視による飛び出しの見辛さを考慮にいれなければ、ルート基準でカメラを回転させるにこしたことはない
      //if( m_eDrawPosi == DRAW_POSITION_CENTER ) pLookAtPosi->y = inCenter.y + m_fCenterOfstY*GetScale();  // あると赤枠が灰枠内で上に上がる、ないと赤枠が灰枠内で中央付けになる  // センターに出したいので、なしにした。
      *pLookAtPosi = -m_userPosOffset;  // 他の計算に影響を与えたくないので、注視点で位置オフセットする。
    }
#endif

    pLookAtPosi->z  = inCenter.z;
    fDistToLookAt  -= inCenter.z;

    gfl2::math::VEC3 inDir; field::CSpace::GetDirectionByYawPitch(&inDir, m_fCameraYaw, m_fCameraPitch);
    (*pCameraPosi) = (*pLookAtPosi) - inDir*fDistToLookAt;
  }

  //-----------------------------------------------------------------------------
  // モデルを設定して初期化します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::SetAndInitModel(PokeTool::PokeModel* pModel)
  {
    m_pModel = pModel;
    m_fCenterOfstY = 0.0f;
    m_eCenterOfstGetPhase = CENTER_OFFSET_GET_PHASE_END;

    const PokeTool::PokeSettingData* pSettingData = NULL;
    if( pModel )
    {
      pSettingData = pModel->GetSettingData();

      m_inModelInitScale    = gfl2::math::ConvertGfVec3ToNwVec3(pModel->GetScale());
      m_inModelInitRot      = gfl2::math::ConvertGfVec3ToNwVec3(pModel->GetRotation());
      m_inModelInitPosi     = gfl2::math::ConvertGfVec3ToNwVec3(pModel->GetPosition());
      m_inModelInitPosiOfst = gfl2::math::ConvertGfVec3ToNwVec3(pModel->GetOffset());
      /*if( m_eDrawPosi == DRAW_POSITION_CENTER )*/ m_eCenterOfstGetPhase = CENTER_OFFSET_GET_PHASE_1;  // [niji注] ifをコメントアウト。最初のフレームは変な位置に出るので、不可視としてしのぐため。

#if PM_DEBUG
      if( pModel->IsEnable() == false )
        GFL_PRINT("ぽけもでるの初期化が終了する前にぽけもんいんふれーむの初期化が呼ばれました。修正よろしくお願いします。\n");
#endif
    }
    else
      ResetModelInitSRT();

    ResetCameraYawPitch();

    if( pSettingData )
    {
      m_fModelYaw = GFL_MATH_DEG_TO_RAD(pSettingData->camRotate);
      m_fModelYawOfstR = GFL_MATH_DEG_TO_RAD(pSettingData->camRotateRightOfs);

      m_fFrameScale = pSettingData->camSize;
      m_fFramePosiOfstRatioY = pSettingData->camYOffset;

      SetCameraPitch(GFL_MATH_DEG_TO_RAD(pSettingData->camXRotate));
      // GFL_PRINT("モデルよー:%.2f 枠スケ:%.3f 枠オフセＹ:%.3f \n", pSettingData->camRotate, pSettingData->camSize, pSettingData->camYOffset);
    }
    else
    {
      m_fModelYaw = GFL_MATH_DEG_TO_RAD(-20.0f);
      m_fModelYawOfstR = 0.0f;

      m_fFrameScale = 1.0f;
      m_fFramePosiOfstRatioY = 0.0f;
    }
  }

  //-----------------------------------------------------------------------------
  // モデルの初期化時のSRTに戻します。
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::ReturnModelInitSRT()
  {
    if( m_pModel )
    {
      m_pModel->SetScale   (gfl2::math::ConvertNwVec3ToGfVec3(m_inModelInitScale));
      m_pModel->SetRotation(gfl2::math::ConvertNwVec3ToGfVec3(m_inModelInitRot));
      m_pModel->SetPosition(gfl2::math::ConvertNwVec3ToGfVec3(m_inModelInitPosi));
      m_pModel->SetOffset  (gfl2::math::ConvertNwVec3ToGfVec3(m_inModelInitPosiOfst));
    }
  }

  //-----------------------------------------------------------------------------
  // デバッグ描画
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::DrawDebug(gfl2::gfx::CtrDisplayNo displayNo, const InFrameCamera* pCamera)
  {
#if PM_DEBUG

    if( m_nDispInfo > 0 && pCamera )
    {
      const gfl2::gfx::CtrDisplayNo subDisplayNo = (m_eScreen == SCREEN_UPPER) ? gfl2::gfx::CtrDisplayNo::DOWN : gfl2::gfx::CtrDisplayNo::LEFT;
      const float DARK_RATIO = 0.6f;

      if( m_pModel )
      {
        DrawAABB(displayNo, *pCamera);

        /*
        違う画面の描画があるのでコメントアウト。テキスト描画があるのでコメントアウト。
        gfl2::math::AABB inAABB;
        const bool bIsGetAABB = GetBoundingAABBWorld(&inAABB);
        if( field::CDebugPrim::BeginProjSpace(subDisplayNo) == true )  // 違う画面の描画はnijiではできない
        {
          const float fScale = GetScale();

          field::CDebugPrim::DrawDarkFilter(DARK_RATIO);

          const int x = 10;
          const int offset_y = 15;
          int y = 10;
          if( bIsGetAABB == true && fScale > 0.0f )
          {
            gfl2::math::Vector4 vector4_inLen; vector4_inLen = inAABB.GetDifference();
            gfl2::math::VEC3 inLen(vector4_inLen.x, vector4_inLen.y, vector4_inLen.z); gfl2::math::VEC3Scale(&inLen, &inLen, 1.0f/fScale);
            //gfl::grp::util::DrawUtil::DrawText(x, y, L"基準境界箱[%.1f , %.1f , %.1f]", inLen.x, inLen.y, inLen.z); y += offset_y;
          }
          else
          {
            //gfl::grp::util::DrawUtil::DrawText(x, y, L"基準境界箱なし"); y += offset_y;
          }

          //gfl::grp::util::DrawUtil::DrawText(x, y, L"モデルスケール[%.3f] 枠スケール[%.2f]", fScale, m_fFrameScale); y += offset_y;
          //gfl::grp::util::DrawUtil::DrawText(x, y, L"モデルよー[%.1f]", GFL_MATH_RAD_TO_DEG(m_fModelYaw)); y += offset_y;

          field::CDebugPrim::EndProjSpace();
        }
        */
      }
      else
      {
        /*
        違う画面の描画があるのでコメントアウト。
        // 画面がパシャパシャとうるさいので、暗転のみ常に行う
        if( field::CDebugPrim::BeginProjSpace(subDisplayNo) == true )  // 違う画面の描画はnijiではできない
        {
          field::CDebugPrim::DrawDarkFilter(DARK_RATIO);

          field::CDebugPrim::EndProjSpace();
        }
        */
      }

      // 枠描画
      DrawFrame(displayNo);
    }

#endif
  }

  //-----------------------------------------------------------------------------
  // 立体視パラメータを設定する(CCameraULCD::Setのパラメータ参照)
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::SetULCDParam(const float fDepthRange, const float fDepthLevel, const float fLimitParallax)
  {
    m_fDepthRange    = fDepthRange   ;
    m_fDepthLevel    = fDepthLevel   ;
    m_fLimitParallax = fLimitParallax;
  }

  //-----------------------------------------------------------------------------
  // 立体視パラメータを取得する
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::GetULCDParam(float* fDepthRange, float* fDepthLevel, float* fLimitParallax) const
  {
    *fDepthRange    = m_fDepthRange   ;
    *fDepthLevel    = m_fDepthLevel   ;
    *fLimitParallax = m_fLimitParallax;
  }

  //-----------------------------------------------------------------------------
  // カメラよー・ぴっちをリセットします
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::ResetCameraYawPitch()
  {
    m_fCameraYaw   = System::Camera::internal::F_PI;
    m_fCameraPitch = 0.0f;
  }

  const float CModelViewerInFrame::CAMERA_PITCH_MAX = GFL_MATH_DEG_TO_RAD(85.0f); // 上回転リミット
  const float CModelViewerInFrame::CAMERA_PITCH_MIN = -CAMERA_PITCH_MAX;          // 下回転リミット

  //-----------------------------------------------------------------------------
  // カメラよー・ぴっちを設定・加減算します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::SetCameraYaw  (const float fSetRadi) { m_fCameraYaw = field::CUtil::ConvertFromMinusPiToPlusPi( fSetRadi ); }
  void CModelViewerInFrame::AddCameraYaw  (const float fAddRadi) { m_fCameraYaw = field::CUtil::AddRadian(m_fCameraYaw, fAddRadi); }
  void CModelViewerInFrame::SetCameraPitch(const float fSetRadi)
  {
    m_fCameraPitch = fSetRadi;
    field::CUtil::MaxMinClip(&m_fCameraPitch, CAMERA_PITCH_MAX, CAMERA_PITCH_MIN);
  }
  void CModelViewerInFrame::AddCameraPitch(const float fAddRadi)
  {
    // リミットに近づくにつれ軽くブレーキをかける
    float fMulRatio = 1.0f;
    if( fAddRadi*m_fCameraPitch > 0.0f ) fMulRatio = field::GetRatioZeroOneInterpolateValue(1.0f, 0.1f, field::GetInterpolateRatioValue(gfl2::math::FAbs(m_fCameraPitch), CAMERA_PITCH_MAX - GFL_MATH_DEG_TO_RAD(15.0f), CAMERA_PITCH_MAX));

    m_fCameraPitch += fAddRadi*fMulRatio;
    field::CUtil::MaxMinClip(&m_fCameraPitch, CAMERA_PITCH_MAX, CAMERA_PITCH_MIN);
  }

  //-----------------------------------------------------------------------------
  // 内部でモデルのVisibleを変更するので、モデルを表示するのかしないのかはこの関数で設定して下さい。
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::SetModelVisible(const bool visible)
  {
    m_modelVisible = visible;

    // 即時反映できる状態なら、即時反映する。
    if( m_pModel )
    {
      if( m_eCenterOfstGetPhase == CENTER_OFFSET_GET_PHASE_END )
      {
        // 即時反映できる状態なら、即時反映する。
        m_pModel->SetVisible(m_modelVisible);
      }
      else
      {
        // 非表示なら状態を気にしなくていいので、必ず即時反映する。
        if(m_modelVisible == false)
        {
          m_pModel->SetVisible(m_modelVisible);
        }
      }
    }
  }
  
  //-----------------------------------------------------------------------------
  // モデルを画面上に表示中か否か(ユーザがm_modelVisibleにfalseを設定しているときfalse、モデルがないときfalse、モデルを非表示にしているときfalse)
  //-----------------------------------------------------------------------------
  bool CModelViewerInFrame::IsModelOnDisplay(void) const
  {
    bool ret = false;
    if(m_modelVisible)
    {
      if(m_pModel != NULL)
      {
        if(m_pModel->IsVisible())
        {
          ret = true;
        }
      }
    }
    return ret;
  }


#if PM_DEBUG

  //-----------------------------------------------------------------------------
  // ディスプレイタイプ配列を取得します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::GetDisplayTypeArray(gfl2::gfx::CtrDisplayNo aDisplayNo[], int* pnNum)
  {
    if( m_eScreen == SCREEN_UPPER )
    {
      aDisplayNo[0] = gfl2::gfx::CtrDisplayNo::LEFT;
      aDisplayNo[1] = gfl2::gfx::CtrDisplayNo::RIGHT;
      (*pnNum) = 2;
    }
    else
    {
      aDisplayNo[0] = gfl2::gfx::CtrDisplayNo::DOWN;
      (*pnNum) = 1;
    }
  }
  //-----------------------------------------------------------------------------
  // 画面番号が同じか
  //-----------------------------------------------------------------------------
  bool CModelViewerInFrame::IsDisplayNoTheSame(gfl2::gfx::CtrDisplayNo displayNo)
  {
    gfl2::gfx::CtrDisplayNo aDisplayNo[2];
    int nNum;
    GetDisplayTypeArray(aDisplayNo, &nNum);
    for( int i = 0; i < nNum; i++ )
    {
      if(aDisplayNo[i] == displayNo)
      {
        return true;
      }
    }
    return false;
  }

  //-----------------------------------------------------------------------------
  // 枠を描画します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::DrawFrame(gfl2::gfx::CtrDisplayNo displayNo, const bool bIsDrawAdjustFrame)
  {
    if(!IsDisplayNoTheSame(displayNo)) return;

    float fAspectRatio1;
    gfl2::math::VEC2 inPosiMax1, inPosiMin1, inLenHalf1;
    const float fBackupScale = m_fFrameScale;
    const float fBackupFramePosiOfstRatioY = m_fFramePosiOfstRatioY;
    m_fFrameScale = 1.0f;
    m_fFramePosiOfstRatioY = 0.0f;
    GetFrameData(&fAspectRatio1, &inPosiMax1, &inPosiMin1, &inLenHalf1);

    float fAspectRatio2 = 1.0f;
    gfl2::math::VEC2 inPosiMax2(internal::VEC2_ZERO), inPosiMin2(internal::VEC2_ZERO), inLenHalf2(internal::VEC2_ZERO);
    m_fFrameScale = fBackupScale;
    m_fFramePosiOfstRatioY = fBackupFramePosiOfstRatioY;
    if( bIsDrawAdjustFrame ) GetFrameData(&fAspectRatio2, &inPosiMax2, &inPosiMin2, &inLenHalf2);

    {
      if( field::CDebugPrim::BeginProjSpace(displayNo) == true )
      {
        field::CDebugPrim::DrawFrameProjSpace(inPosiMax1, inPosiMin1, 0.05f, gfl2::math::VEC4(0.0f, 0.0f, 0.0f, 0.45f), fAspectRatio1, -1.0f); // 正しい枠を描画
        if( bIsDrawAdjustFrame ) field::CDebugPrim::DrawFrameProjSpace(inPosiMax2, inPosiMin2, 0.015f, gfl2::math::VEC4(0.5f, 0.5f, 0.5f, 1.0f), fAspectRatio2, -1.0f); // モデル毎の調整を加えた枠を描画

        field::CDebugPrim::EndProjSpace();
      }
    }
  }

  //-----------------------------------------------------------------------------
  // AABBを描画します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::DrawAABB(gfl2::gfx::CtrDisplayNo displayNo, const InFrameCamera& inCamera)
  {
    if(!IsDisplayNoTheSame(displayNo)) return;
    
    gfl2::math::AABB inAABB;
    if( GetBoundingAABBWorld(&inAABB) == true )
    {
      {
        if( field::CDebugPrim::Begin(inCamera, displayNo, false) == true )
        {
          field::CDebugPrim::DrawAABB(inAABB, field::VEC4_COLOR_RED, true);
          field::CDebugPrim::End();
        }
      }
    }
  }

  int CModelViewerInFrame::m_nDispInfo = 0;

#endif

  //-----------------------------------------------------------------------------
  // ワールド位置を取得します
  //-----------------------------------------------------------------------------
  void CModelViewerInFrame::GetWorldPosition(gfl2::math::Vector3* pWorldPosi, const float __fProjPosiX__, const float __fProjPosiY__, const float fViewPosiZ, const SUpdateData& inData)
  {
    const float fProjPosiX = __fProjPosiX__ - inData.m_inCenterPosi.x;
    const float fProjPosiY = __fProjPosiY__ - inData.m_inCenterPosi.y;

    float fLenHalfX, fLenHalfY; field::CUtil::GetScreenLengthHalf3D(&fLenHalfX, &fLenHalfY, fViewPosiZ, inData.m_fViewAngle, inData.m_fAspectRatio);
    const gfl2::math::VEC3 inViewPosi(fLenHalfX*fProjPosiX, fLenHalfY*fProjPosiY, -fViewPosiZ);

    gfl2::math::MTX34 mtx34_inData_inViewMat = gfl2::math::ConvertGfMtx34ToNwMtx34(inData.m_inViewMat);
    gfl2::math::MTX34 inCameraMat; gfl2::math::MTX34InverseOrtho(&inCameraMat, &mtx34_inData_inViewMat);
    gfl2::math::VEC3 vec3_WorldPosi;
    gfl2::math::VEC3Transform(&vec3_WorldPosi, &inCameraMat, &inViewPosi);
    *pWorldPosi = gfl2::math::ConvertNwVec3ToGfVec3(vec3_WorldPosi);
  }

} // namespace Camera
} // namespace System

/*  EOF  */



#if 0

  //-----------------------------------------------------------------------------
  /**
   * @brief (呼び出し側が)入力を受け取ることができるか判定します。
   *        デバッグで調整や確認を行っているときに、こちらが入力を奪いたいときがありますので、
   *        戻り値がfalseを返しているときは、呼び出し側は入力を取らないようにお願いします。
   *
   * @param pButton ボタンの入力情報クラス
   *
   * @retval true  入力を受け取ってよい
   * @retval false 入力を受け取ってはいけない
   */
  //-----------------------------------------------------------------------------
  bool CModelViewerInFrame::IsEnableReceiveInput(const gfl::ui::Button* pButton)
  {
#if PM_DEBUG

    if( gfl::debug::DebugWin_IsOpen() == true ) return false;

    if( pButton )
    {
      if( pButton->IsHold(gfl::ui::BUTTON_R) && pButton->IsHold(gfl::ui::BUTTON_X) ) return false;
    }

#endif

    return true;
  }

#endif





