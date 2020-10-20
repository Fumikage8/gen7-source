
//=============================================================================
/**
 * @file   CameraDebugPrimitive.cpp
 * @brief  デバッグプリミティブ描画サポートクラス
 * @author 池田 哲平
 * @date   2012.05.30
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.27
 */
//=============================================================================

#if PM_DEBUG

#include <System/include/GflUse.h>
#include <model/include/gfl2_BaseCamera.h>
#include <System/include/Camera/InFrameCamera.h>

#include "CameraDebugPrimitive.h"
#include "CameraInterpolate.h"
//#include "field/rail/FieldRailHermite.h"  // [niji注] nijiでは使っていないのでコメントアウト。

namespace System { namespace Camera { namespace field
{
  gfl2::gfx::DrawUtil::VertexFormat CDebugPrim::m_ainVertexBuf[6];
  //gfl2::math::VEC3                  CDebugPrim::m_inCamPosi(System::Camera::internal::VEC3_ZERO);  // [niji注] nijiでは使っていないのでコメントアウト。
  //gfl2::math::VEC3                  CDebugPrim::m_inCamDir (System::Camera::internal::VEC3_UNIT_Z);  // [niji注] nijiでは使っていないのでコメントアウト。

#if 0
  //-----------------------------------------------------------------------------
  // 初期化
  //-----------------------------------------------------------------------------
  void CDebugPrim::Init()
  {
    GFL_ASSERT(m_ainVertexBuf == NULL);
    m_ainVertexBuf = gfl::grp::util::DrawUtil::CreateVertexArray(4);
  }

  //-----------------------------------------------------------------------------
  // 解放処理
  //-----------------------------------------------------------------------------
  void CDebugPrim::Release()
  {
    if( m_ainVertexBuf )
    {
      gfl::grp::util::DrawUtil::DeleteVertexArray(m_ainVertexBuf);
      m_ainVertexBuf = NULL;
    }
  }
#endif

  //-----------------------------------------------------------------------------
  // 描画開始
  //-----------------------------------------------------------------------------
  bool CDebugPrim::Begin(const poke_3d::model::BaseCamera& inCamera, const gfl2::gfx::CtrDisplayNo displayNo, const bool __bIsExchangeXY__)
  {
    {
      BeginCommonFirst(displayNo);
    
      /*
      // [niji注] nijiでは使っていないのでコメントアウト。
      gfl2::math::VEC3 inLookAtPosi;
      {
        m_inCamPosi = gfl2::math::ConvertGfVec3ToNwVec3( inCamera.GetPosition() );
        inLookAtPosi = gfl2::math::ConvertGfVec3ToNwVec3( inCamera.GetTargetPosition() );
      }
      m_inCamDir = inLookAtPosi - m_inCamPosi; gfl2::math::VEC3Normalize(&m_inCamDir, &m_inCamDir);
      */
 
      bool  bIsExchangeXY = false;
      //float fTextScale = 0.55f;  // [niji注] テキストの設定はテキストを使うときに行うことにしたのでコメントアウト。
      if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
      {
        bIsExchangeXY = __bIsExchangeXY__;
        //fTextScale = 0.5f;  // [niji注] テキストの設定はテキストを使うときに行うことにしたのでコメントアウト。
      }

      gfl2::math::Matrix34 matrix34_inViewMat;
      gfl2::math::Matrix44 matrix44_inProjMat;
      //gfl2::math::MTX34 inViewMat;
      gfl2::math::MTX44 inProjMat;
      matrix34_inViewMat = inCamera.GetViewMatrix();
      matrix44_inProjMat = inCamera.GetProjectionMatrix();
      inProjMat = gfl2::math::ConvertGfMtx44ToNwMtx44(matrix44_inProjMat);
      if( bIsExchangeXY == true )
      {
        gfl2::math::MTX44 mat; CUtil::GetExchangeXYMatrix(&mat);
        MTX44Mult(&inProjMat, &mat, &inProjMat);
      }
      matrix44_inProjMat = gfl2::math::ConvertNwMtx44ToGfMtx44(inProjMat);
      gfl2::gfx::DrawUtil::SetViewMatrix(matrix34_inViewMat);
      gfl2::gfx::DrawUtil::SetProjectionMatrix(matrix44_inProjMat);

      //gfl::grp::util::DrawUtil::SetTextScale(fTextScale, fTextScale);  // [niji注] テキストの設定はテキストを使うときに行うことにしたのでコメントアウト。 
      //gfl::grp::util::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);  // [niji注] テキストの設定はテキストを使うときに行うことにしたのでコメントアウト。 

      BeginCommon();
      return true;
    }
  }

  bool CDebugPrim::Begin(const System::Camera::InFrameCamera& inCamera, const gfl2::gfx::CtrDisplayNo displayNo, const bool __bIsExchangeXY__)
  {
    {
      BeginCommonFirst(displayNo);
     
      bool  bIsExchangeXY = false;
      //float fTextScale = 0.55f;  // [niji注] テキストの設定はテキストを使うときに行うことにしたのでコメントアウト。 
      if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
      {
        bIsExchangeXY = __bIsExchangeXY__;
        //fTextScale = 0.5f;  // [niji注] テキストの設定はテキストを使うときに行うことにしたのでコメントアウト。 
      }

      gfl2::math::Matrix34 matrix34_inViewMat;
      gfl2::math::Matrix44 matrix44_inProjMat;
      //gfl2::math::MTX34 inViewMat;
      gfl2::math::MTX44 inProjMat;
      matrix34_inViewMat = inCamera.GetViewMatrix();
      matrix44_inProjMat = inCamera.GetProjectionMatrix();
      inProjMat = gfl2::math::ConvertGfMtx44ToNwMtx44(matrix44_inProjMat);
      if( bIsExchangeXY == true )
      {
        gfl2::math::MTX44 mat; CUtil::GetExchangeXYMatrix(&mat);
        MTX44Mult(&inProjMat, &mat, &inProjMat);
      }
      matrix44_inProjMat = gfl2::math::ConvertNwMtx44ToGfMtx44(inProjMat);
      gfl2::gfx::DrawUtil::SetViewMatrix(matrix34_inViewMat);
      gfl2::gfx::DrawUtil::SetProjectionMatrix(matrix44_inProjMat);

      //gfl::grp::util::DrawUtil::SetTextScale(fTextScale, fTextScale);  // [niji注] テキストの設定はテキストを使うときに行うことにしたのでコメントアウト。 
      //gfl::grp::util::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);  // [niji注] テキストの設定はテキストを使うときに行うことにしたのでコメントアウト。 

      BeginCommon();
      return true;
    }
  }

  //-----------------------------------------------------------------------------
  // 描画終了
  //-----------------------------------------------------------------------------
  void CDebugPrim::End()
  {
    // 簡易描画終了(簡易描画を終えたら必ず呼ぶ)
    gfl2::gfx::DrawUtil::EndRender();
  
    gfl2::gfx::GFGL::EndScene();
  }

  //-----------------------------------------------------------------------------
  // 開始共通処理
  //-----------------------------------------------------------------------------
  void  CDebugPrim::BeginCommonFirst(const gfl2::gfx::CtrDisplayNo displayNo)
  {
    gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[displayNo];
    
    gfl2::gfx::GFGL::SetActiveDisplay(handle);
  
    gfl2::gfx::Surface* pBackBuffer  = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
  
    gfl2::gfx::GFGL::SetRenderTarget( pBackBuffer );
    gfl2::gfx::GFGL::SetDepthStencil( pDepthBuffer );
  
    gfl2::gfx::GFGL::BeginScene();

    {
      u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth( handle );
      u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );
   
      // 簡易描画のシザリング設定(描画前に行うこと)
      gfl2::gfx::DrawUtil::SetScissor(0, 0, displayWidth, displayHeight);  // シザリング領域設定
   
      // カリングの設定(描画前に行うこと)
      gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);  // 両面描画
   
      // ワイヤーフレームON/OFFの設定(描画前に行うこと)
      gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);  // 通常描画
   
      gfl2::gfx::GFGL::SetViewport(0, 0, displayWidth, displayHeight);
 
      // 簡易描画開始(簡易描画を始める前に必ず呼ぶ)
      gfl2::gfx::DrawUtil::BeginRender();
    }
  }

  //-----------------------------------------------------------------------------
  // 開始共通処理
  //-----------------------------------------------------------------------------
  void CDebugPrim::BeginCommon()
  {
    gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::ConvertNwMtx44ToGfMtx44(System::Camera::internal::MTX44_IDENTITY));
	  gfl2::gfx::DrawUtil::SetMaterialColor(gfl2::math::Vector4(1.0f,1.0f,1.0f,1.0f));
	  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
	  gfl2::gfx::DrawUtil::SetTexture(NULL);
  }

  //-----------------------------------------------------------------------------
  // 変換関数
  //-----------------------------------------------------------------------------
  gfl2::math::Vector4 CDebugPrim::ConvertNwVec3ToGfVec4(const gfl2::math::VEC3& nwVec3)
  {
    gfl2::math::Vector4 gfVec4( nwVec3.x, nwVec3.y, nwVec3.z, 0.0f );
    return gfVec4;
  }
  gfl2::math::VEC3    CDebugPrim::ConvertGfVec4ToNwVec3(const gfl2::math::Vector4& gfVec4)
  {
    gfl2::math::VEC3 nwVec3( gfVec4.x, gfVec4.y, gfVec4.z );
    return nwVec3;
  }

  //-----------------------------------------------------------------------------
  // 描画開始(プロジェクション空間版)
  //-----------------------------------------------------------------------------
  bool CDebugPrim::BeginProjSpace(const gfl2::gfx::CtrDisplayNo displayNo)
  {
    {
      BeginCommonFirst(displayNo);

      // このように分けなくても、ビュー・プロジェクション行列の逆行列をかければ理論上同義なのだが、
      // 計算による値の壊れが大きく、表示される絵がガタついて話にならないので、別にプロジェクション空間版を用意した。
      gfl2::math::MTX44 mat = System::Camera::internal::MTX44_IDENTITY; //CUtil::GetExchangeXYMatrix(&mat);  // [niji注] nijiでは縦横入れ替える必要がなかったのでコメントアウトした。
      gfl2::math::Matrix34 matrix34_inViewMat = gfl2::math::ConvertNwMtx34ToGfMtx34(System::Camera::internal::MTX34_IDENTITY);
      gfl2::math::Matrix44 matrix44_inProjMat = gfl2::math::ConvertNwMtx44ToGfMtx44(mat);
      gfl2::gfx::DrawUtil::SetViewMatrix(matrix34_inViewMat);
      gfl2::gfx::DrawUtil::SetProjectionMatrix(matrix44_inProjMat);

      BeginCommon();
      return true;
    }
  }

  //-----------------------------------------------------------------------------
  // 線を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawLine(const CVertex& inSrc, const CVertex& inDest)
  {
    m_ainVertexBuf[0].pos   = ConvertNwVec3ToGfVec4(inSrc. m_inPosi);
    m_ainVertexBuf[0].color = gfl2::math::ConvertNwVec4ToGfVec4(inSrc. m_inColor);
    m_ainVertexBuf[1].pos   = ConvertNwVec3ToGfVec4(inDest.m_inPosi);
    m_ainVertexBuf[1].color = gfl2::math::ConvertNwVec4ToGfVec4(inDest.m_inColor);

    gfl2::gfx::DrawUtil::DrawLines(m_ainVertexBuf, 2, false);
  }

  /*
  // [niji注] nijiではWin32版でOpenGLの機能を使ったライン描画しかしていないのでコメントアウト。
  void CDebugPrim::GetLineSideOffsetDirection(gfl::math::VEC3* pDir, const gfl::math::VEC3& inPosi, const gfl::math::VEC3& inLineVec)
  {
    const gfl::math::VEC3 inTempVec = inPosi - m_inCamPosi;
    gfl::math::VEC3Cross(pDir, &inTempVec, &inLineVec);
    if( CUtil::CheckAndGetNormalizeVector(pDir, NULL, *pDir) == false ) (*pDir) = gfl::math::VEC3_ZERO;
  }

  float CDebugPrim::GetLineScale(const gfl::math::VEC3& inPosi)
  {
    float fDepth = CUtil::GetAxisLength(inPosi - m_inCamPosi, m_inCamDir); CUtil::ZeroClip(&fDepth);
    return 0.01f + fDepth*0.0015f;
  }
  */

  //-----------------------------------------------------------------------------
  // 複数の繋がった線を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawLineList(const gfl2::math::VEC3 ainPosi[], const int nPosiNum, const gfl2::math::VEC4& inColor)
  {
    // 頂点節約のためにトライアングルストリップで一度に書こうとするとラインが歪んでしまうので一本一本書く
    for( int i = 0; i < nPosiNum-1; i++ )
      DrawLine(ainPosi[i], ainPosi[i+1], inColor);
  }

  void CDebugPrim::DrawLineList(const CVertex ainVertex[], const int nVertexNum)
  {
    for( int i = 0; i < nVertexNum-1; i++ )
      DrawLine(ainVertex[i], ainVertex[i+1]);
  }

  //-----------------------------------------------------------------------------
  // NULLを描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawNull(const gfl2::math::VEC3& inPosi, const float fSize, const gfl2::math::VEC4& inColor)
  {
    const float fHalfSize = 0.5f*fSize;
    DrawLine(gfl2::math::VEC3(inPosi.x+fHalfSize, inPosi.y, inPosi.z), gfl2::math::VEC3(inPosi.x-fHalfSize, inPosi.y, inPosi.z), inColor);
    DrawLine(gfl2::math::VEC3(inPosi.x, inPosi.y+fHalfSize, inPosi.z), gfl2::math::VEC3(inPosi.x, inPosi.y-fHalfSize, inPosi.z), inColor);
    DrawLine(gfl2::math::VEC3(inPosi.x, inPosi.y, inPosi.z+fHalfSize), gfl2::math::VEC3(inPosi.x, inPosi.y, inPosi.z-fHalfSize), inColor);
  }

  //-----------------------------------------------------------------------------
  // 三角形ポリゴンを描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawTrianglePolygon(const gfl2::math::VEC3& inPosi1, const gfl2::math::VEC3& inPosi2, const gfl2::math::VEC3& inPosi3, const gfl2::math::VEC4& inColor)
  {
    m_ainVertexBuf[0].pos = ConvertNwVec3ToGfVec4(inPosi1);
    m_ainVertexBuf[1].pos = ConvertNwVec3ToGfVec4(inPosi2);
    m_ainVertexBuf[2].pos = ConvertNwVec3ToGfVec4(inPosi3);
    m_ainVertexBuf[0].color = m_ainVertexBuf[1].color = m_ainVertexBuf[2].color = gfl2::math::ConvertNwVec4ToGfVec4(inColor);
    gfl2::gfx::DrawUtil::DrawTriangles(m_ainVertexBuf, 3, false);
  }

  void CDebugPrim::DrawTrianglePolygonScale(const gfl2::math::VEC3& inPosi1, const gfl2::math::VEC3& inPosi2, const gfl2::math::VEC3& inPosi3, const gfl2::math::VEC4& inColor, const float fScale)
  {
#if 0
    gfl::math::AABB_VEC3 inAABB; inAABB.Create(inPosi1, inPosi2, inPosi3);
    gfl::math::VEC3 inCenter; inAABB.GetCenter(&inCenter);
#else
    const gfl2::math::VEC3 inCenter((inPosi1 + inPosi2 + inPosi3)/3.0f); // 重心の方がうまく均等にスケーリングしてくれる
#endif
    DrawTrianglePolygon(GetRatioInterpolateValue(inCenter, inPosi1, fScale), GetRatioInterpolateValue(inCenter, inPosi2, fScale), GetRatioInterpolateValue(inCenter, inPosi3, fScale), inColor);
  }

  //-----------------------------------------------------------------------------
  // 四角形ポリゴンを描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawQuadranglePolygon(const gfl2::math::VEC3 ainPosi[], const gfl2::math::VEC4& inColor)
  {
    const int TRIANGLE_STRIP_2_TRIANGLES[] =
    {
      0, 1, 2,
      1, 3, 2,
    };

    for( int i = 0; i < 6; i++ )
    {
      m_ainVertexBuf[i].pos   = ConvertNwVec3ToGfVec4(ainPosi[TRIANGLE_STRIP_2_TRIANGLES[i]]);
      m_ainVertexBuf[i].color = gfl2::math::ConvertNwVec4ToGfVec4(inColor);
    }
    gfl2::gfx::DrawUtil::DrawTriangles(m_ainVertexBuf, 6, false);
  }

  //-----------------------------------------------------------------------------
  // 四角形ポリゴンを描画する(プロジェクション空間版)
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawQuadranglePolygonProjSpace(const gfl2::math::VEC2 ainPosi[], const gfl2::math::VEC4& inColor, const float fZ)
  {
    const int TRIANGLE_STRIP_2_TRIANGLES[] =
    {
      0, 1, 2,
      1, 3, 2,
    };

    for( int i = 0; i < 6; i++ )
    {
      const gfl2::math::VEC2* p = &(ainPosi[TRIANGLE_STRIP_2_TRIANGLES[i]]);
      m_ainVertexBuf[i].pos   = gfl2::math::Vector4(p->x, p->y, fZ, 0.0f);
      m_ainVertexBuf[i].color = gfl2::math::ConvertNwVec4ToGfVec4(inColor);
    }
    gfl2::gfx::DrawUtil::DrawTriangles(m_ainVertexBuf, 6, false);
  }

/*
  // [niji注] nijiでは使っていないのでコメントアウト。
  //-----------------------------------------------------------------------------
  // 格子を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawGrid(const int nDivideX, const int nDivideZ, const float fLengthX, const float fLengthZ, const gfl::math::MTX34& inWorldMat, const gfl::math::VEC4& inColor)
  {
    const gfl::math::VEC3 inScale(fLengthX, 1.0f, fLengthZ);
    gfl::math::VEC3 inPosi; gfl::math::MTX34GetTranslate(&inPosi, inWorldMat);
    gfl::math::MTX33 inScaleMat; gfl::math::MTX33Scale(&inScaleMat, &inScale);
    gfl::math::MTX33 inSRMat, inTempMat(inWorldMat); gfl::math::MTX33MultLimit(&inSRMat, &inTempMat, &inScaleMat);
    gfl::math::MTX34 inTransMat; gfl::math::MTX34SetMTX33VEC3(&inTransMat, inSRMat, inPosi);

    const float fOfstX = 1.0f/nDivideX;
    for( int i = 0; i <= nDivideX; i++ )
    {
        const float fX = -0.5f + i*fOfstX;
        gfl::math::VEC3 inSrcPosi (fX, 0.0f, -0.5f); gfl::math::VEC3Transform(&inSrcPosi,  &inTransMat, &inSrcPosi);
        gfl::math::VEC3 inDestPosi(fX, 0.0f,  0.5f); gfl::math::VEC3Transform(&inDestPosi, &inTransMat, &inDestPosi);
        DrawLine(inSrcPosi, inDestPosi, inColor);
    }

    const float fOfstZ = 1.0f/nDivideZ;
    for( int i = 0; i <= nDivideZ; i++ )
    {
        const float fZ = -0.5f + i*fOfstZ;
        gfl::math::VEC3 inSrcPosi (-0.5f, 0.0f, fZ); gfl::math::VEC3Transform(&inSrcPosi,  &inTransMat, &inSrcPosi);
        gfl::math::VEC3 inDestPosi( 0.5f, 0.0f, fZ); gfl::math::VEC3Transform(&inDestPosi, &inTransMat, &inDestPosi);
        DrawLine(inSrcPosi, inDestPosi, inColor);
    }
  }

  //-----------------------------------------------------------------------------
  // 矢印を描画する１
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawArrow(const gfl::math::VEC3& inPosi, const gfl::math::MTX33& inDirMat, const float fLength, const gfl::math::VEC4& inColor)
  {
    // 単位矢印を希望矢印へと変換するマトリクスを生成する
    const gfl::math::VEC3 inScale(fLength, fLength, fLength);
    gfl::math::MTX33 inSRMat; gfl::math::MTX33Scale(&inSRMat, &inScale);
    gfl::math::MTX33 inTempMat(inSRMat); gfl::math::MTX33MultLimit(&inSRMat, &inDirMat, &inTempMat);
    gfl::math::MTX34 inTransMat; gfl::math::MTX34SetMTX33VEC3(&inTransMat, inSRMat, inPosi);

    const float HEAD_LEN = 0.2f; // 矢尻長
    const gfl::math::VEC3 ainPosi[] = {
      gfl::math::VEC3(     0.0f,      0.0f, 0.0f),
      gfl::math::VEC3(     0.0f,      0.0f, 1.0f),
      gfl::math::VEC3( HEAD_LEN,      0.0f, 1.0f-HEAD_LEN),
      gfl::math::VEC3(-HEAD_LEN,      0.0f, 1.0f-HEAD_LEN),
      gfl::math::VEC3(     0.0f,      0.0f, 1.0f),
      gfl::math::VEC3(     0.0f,  HEAD_LEN, 1.0f-HEAD_LEN),
      gfl::math::VEC3(     0.0f, -HEAD_LEN, 1.0f-HEAD_LEN),
      gfl::math::VEC3(     0.0f,      0.0f, 1.0f)
    };

    gfl::math::VEC3 inSrcPosi; gfl::math::VEC3Transform(&inSrcPosi, &inTransMat, &ainPosi[0]);
    for( int i = 0; i < GET_ARRAY_NUMBER(ainPosi)-1; i++ )
    {
      gfl::math::VEC3 inDestPosi; gfl::math::VEC3Transform(&inDestPosi, &inTransMat, &ainPosi[i+1]);
      DrawLine(inSrcPosi, inDestPosi, inColor);
      inSrcPosi = inDestPosi;
    }
  }

  //-----------------------------------------------------------------------------
  // 矢印を描画する２
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawArrow(const gfl::math::VEC3& inPosi, const gfl::math::VEC3& inVec, const float fLength, const gfl::math::VEC4& inColor)
  {
    gfl::math::VEC3 inDir;
    if( CUtil::CheckAndGetNormalizeVector(&inDir, NULL, inVec) == true )
    {
      const float fRatio = GetInterpolateRatioValue(gfl::math::FAbs(inDir.y), 0.5f, 1.0f);
      gfl::math::VEC3 inUpDir = gfl::math::VEC3_UNIT_Y;
      if     ( fRatio >= 1.0f ) inUpDir = gfl::math::VEC3_UNIT_X;
      else if( fRatio >  0.0f )
      {
        inUpDir = gfl::math::VEC3(fRatio, 1.0f-fRatio, 0.0f);
        gfl::math::VEC3Normalize(&inUpDir, &inUpDir);
      }

      gfl::math::MTX33 inDirMat;
      CSpace::CreateMTX33ByFrontUpDirection(&inDirMat, inDir, inUpDir);
      DrawArrow(inPosi, inDirMat, fLength, inColor);
    }
  }

  //-----------------------------------------------------------------------------
  // 矢印を描画する３
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawArrow(const gfl::math::VEC3& inSrcPosi, const gfl::math::VEC3& inDestPosi, const gfl::math::VEC4& inColor)
  {
    float fLength;
    gfl::math::VEC3 inDir;
    if( CUtil::CheckAndGetNormalizeVector(&inDir, &fLength, inDestPosi-inSrcPosi) == true )
      DrawArrow(inSrcPosi, inDir, fLength, inColor);
  }

  //-----------------------------------------------------------------------------
  // 空間軸を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawSpaceAxis(const gfl::math::VEC3& inAxisX, const gfl::math::VEC3& inAxisY, const gfl::math::VEC3& inAxisZ, const gfl::math::VEC3& inPosi, const float fLength)
  {
    DrawArrow(inPosi, inAxisX, fLength, VEC4_COLOR_RED);
    DrawArrow(inPosi, inAxisY, fLength, VEC4_COLOR_GREEN);
    DrawArrow(inPosi, inAxisZ, fLength, VEC4_COLOR_BLUE);
  }

  //-----------------------------------------------------------------------------
  // 平面を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawPlane(const gfl::math::VEC3& __inPosi__, const gfl::math::VEC3& inNormVec, const float fSize, const gfl::math::VEC4& inColor, const float fUpOfst)
  {
    gfl::math::VEC3 inNormDir;
    if( CUtil::CheckAndGetNormalizeVector(&inNormDir, NULL, inNormVec) == true )
    {
      const gfl::math::VEC3 inCenterPosi = __inPosi__ + inNormDir*fUpOfst;
      gfl::math::VEC3 ainPosi[] = {
        gfl::math::VEC3(-0.5f, 0.0f, -0.5f),
        gfl::math::VEC3(-0.5f, 0.0f,  0.5f),
        gfl::math::VEC3( 0.5f, 0.0f,  0.5f),
        gfl::math::VEC3(-0.5f, 0.0f, -0.5f),
        gfl::math::VEC3( 0.5f, 0.0f, -0.5f),
        gfl::math::VEC3( 0.5f, 0.0f,  0.5f),
      };

      gfl::math::MTX33 inScaleMat; gfl::math::MTX33Scale(&inScaleMat, fSize);
      gfl::math::MTX33 inRotMat;   CUtil::GetReachMatrix(&inRotMat, gfl::math::VEC3_UNIT_Y, inNormDir);
      gfl::math::MTX33 inSRMat;    gfl::math::MTX33MultLimit(&inSRMat, &inRotMat, &inScaleMat);
      gfl::math::MTX34 inWorldMat; gfl::math::MTX34SetMTX33VEC3(&inWorldMat, inSRMat, inCenterPosi);
      for( int i = 0; i < GET_ARRAY_NUMBER(ainPosi); i++ ) gfl::math::VEC3Transform(&ainPosi[i], &inWorldMat, &ainPosi[i]);

      DrawLineList(ainPosi, GET_ARRAY_NUMBER(ainPosi), inColor);
      DrawArrow(inCenterPosi, inNormDir, fSize, inColor);
    }
  }
*/

  //-----------------------------------------------------------------------------
  // 円を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawCircle(const gfl2::math::MTX34& inWorldMat, const float fRadius, const gfl2::math::VEC4& inColor)
  {
    const int VERTEX_NUM = 10;
    gfl2::math::VEC3 ainPosi[VERTEX_NUM];

    const float OFST_RADI = System::Camera::internal::F_PIx2/static_cast<float>(VERTEX_NUM-1);
    gfl2::math::MTX33 inTemp(inWorldMat);
    gfl2::math::MTX33 inSRMat; gfl2::math::MTX33MultLimitScale(&inSRMat, &inTemp, fRadius);
    gfl2::math::VEC3  inCenterPosi; gfl2::math::MTX34GetTranslate(&inCenterPosi, inWorldMat);

    for( int i = 0; i < VERTEX_NUM-1; i++ )
    {
      gfl2::math::MTX33 inMat; gfl2::math::MTX33MultLimitRotZRad(&inMat, &inSRMat, i*OFST_RADI);
      gfl2::math::MTX33GetAxisX(&ainPosi[i], inMat);
      ainPosi[i] += inCenterPosi;
    }

    ainPosi[VERTEX_NUM-1] = ainPosi[0];
    DrawLineList(ainPosi, VERTEX_NUM, inColor);
  }

  const int   LINE_CIRCLE_NUM_SPHERE = 7;
  const float OFST_RADI_SPHERE = System::Camera::internal::F_PI/LINE_CIRCLE_NUM_SPHERE;

  //-----------------------------------------------------------------------------
  // 球を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawSphere(const gfl2::math::VEC3& inPosi, const float fRadius, const gfl2::math::VEC4& inColor)
  {
    for( int i = 0; i < LINE_CIRCLE_NUM_SPHERE; i++ )
    {
      gfl2::math::MTX33 inRotMat;   gfl2::math::MTX33RotYRad(&inRotMat, i*OFST_RADI_SPHERE);
      gfl2::math::MTX34 inWorldMat; gfl2::math::MTX34SetMTX33VEC3(&inWorldMat, inRotMat, inPosi);
      DrawCircle(inWorldMat, fRadius, inColor);
    }
  }

  static void GetTransformMatrixForDrawBox(gfl2::math::MTX34* pMat, const gfl2::math::MTX34& inWorldMat, const gfl2::math::VEC3& inLength)
  {
    gfl2::math::MTX33 inRotMat(inWorldMat);
    gfl2::math::MTX33 inSRMat; gfl2::math::MTX33MultLimitScale(&inSRMat, &inRotMat, &inLength);
    gfl2::math::VEC3  inPosi;  gfl2::math::MTX34GetTranslate(&inPosi, inWorldMat);
    gfl2::math::MTX34SetMTX33VEC3(pMat, inSRMat, inPosi);
  }

  //-----------------------------------------------------------------------------
  // 箱を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawBox(const gfl2::math::MTX34& inWorldMat, const gfl2::math::VEC3& inLength, const gfl2::math::VEC4& inColor)
  {
    gfl2::math::MTX34 inTransMat; GetTransformMatrixForDrawBox(&inTransMat, inWorldMat, inLength);

    // 上下
    gfl2::math::VEC3 ainTopBottom[] = { gfl2::math::VEC3(-0.5f, 0.5f, -0.5f) , gfl2::math::VEC3(0.5f, 0.5f, -0.5f) , gfl2::math::VEC3(0.5f, 0.5f, 0.5f) , gfl2::math::VEC3(-0.5f, 0.5f, 0.5f) , gfl2::math::VEC3(-0.5f, 0.5f, -0.5f) ,
      gfl2::math::VEC3(-0.5f, -0.5f, -0.5f) , gfl2::math::VEC3(0.5f, -0.5f, -0.5f) , gfl2::math::VEC3(0.5f, -0.5f, 0.5f) , gfl2::math::VEC3(-0.5f, -0.5f, 0.5f) , gfl2::math::VEC3(-0.5f, -0.5f, -0.5f) };
    for( int i = 0; i < GET_ARRAY_NUMBER(ainTopBottom); i++ ) gfl2::math::VEC3Transform(&ainTopBottom[i], &inTransMat, &ainTopBottom[i]);
    DrawLineList(ainTopBottom, GET_ARRAY_NUMBER(ainTopBottom), inColor);

    // 横描画
    gfl2::math::VEC3 ainSide[] = { gfl2::math::VEC3(-0.5f, 0.5f, -0.5f) , gfl2::math::VEC3(0.5f, -0.5f, -0.5f) , gfl2::math::VEC3(0.5f, 0.5f, -0.5f) , gfl2::math::VEC3(0.5f, -0.5f, 0.5f) ,
      gfl2::math::VEC3(0.5f, 0.5f, 0.5f) , gfl2::math::VEC3(-0.5f, -0.5f, 0.5f) , gfl2::math::VEC3(-0.5f, 0.5f, 0.5f) , gfl2::math::VEC3(-0.5f, -0.5f, -0.5f) };
    for( int i = 0; i < GET_ARRAY_NUMBER(ainSide); i++ ) gfl2::math::VEC3Transform(&ainSide[i], &inTransMat, &ainSide[i]);
    DrawLineList(ainSide,GET_ARRAY_NUMBER(ainSide),inColor);
  }

  void CDebugPrim::DrawBoxSimple(const gfl2::math::MTX34& inWorldMat, const gfl2::math::VEC3& inLength, const gfl2::math::VEC4& inColor)
  {
    gfl2::math::MTX34 inTransMat; GetTransformMatrixForDrawBox(&inTransMat, inWorldMat, inLength);

    // 前後面
    {
      gfl2::math::VEC3 ainPosi[] = { gfl2::math::VEC3(-0.5f, 0.5f, 0.5f), gfl2::math::VEC3(0.5f, 0.5f, 0.5f), gfl2::math::VEC3(0.5f, -0.5f, 0.5f), gfl2::math::VEC3(-0.5f, -0.5f, 0.5f), gfl2::math::VEC3(-0.5f, 0.5f, 0.5f) };
      gfl2::math::VEC3 ainFrontPosi[GET_ARRAY_NUMBER(ainPosi)], ainBackPosi[GET_ARRAY_NUMBER(ainPosi)];
      for( int i = 0; i < GET_ARRAY_NUMBER(ainPosi); i++ )
      {
        gfl2::math::VEC3Transform(&ainFrontPosi[i], &inTransMat, &ainPosi[i]);
        ainPosi[i].z *= -1.0f;
        gfl2::math::VEC3Transform(&ainBackPosi[i], &inTransMat, &ainPosi[i]);
      }

      DrawLineList(ainFrontPosi, GET_ARRAY_NUMBER(ainFrontPosi), inColor);
      DrawLineList(ainBackPosi,  GET_ARRAY_NUMBER(ainBackPosi),  inColor);
    }

    // 残り
    {
      gfl2::math::VEC3 ainPosi[] = { gfl2::math::VEC3(-0.5f, 0.5f, 0.5f), gfl2::math::VEC3(-0.5f, 0.5f, -0.5f) };
      for( int i = 0; i < GET_ARRAY_NUMBER(ainPosi); i++ ) gfl2::math::VEC3Transform(&ainPosi[i], &inTransMat, &ainPosi[i]);
      DrawLineList(ainPosi, GET_ARRAY_NUMBER(ainPosi), inColor);
    }
    {
      gfl2::math::VEC3 ainPosi[] = { gfl2::math::VEC3(0.5f, 0.5f, 0.5f), gfl2::math::VEC3(0.5f, 0.5f, -0.5f) };
      for( int i = 0; i < GET_ARRAY_NUMBER(ainPosi); i++ ) gfl2::math::VEC3Transform(&ainPosi[i], &inTransMat, &ainPosi[i]);
      DrawLineList(ainPosi, GET_ARRAY_NUMBER(ainPosi), inColor);
    }
    {
      gfl2::math::VEC3 ainPosi[] = { gfl2::math::VEC3(0.5f, -0.5f, 0.5f), gfl2::math::VEC3(0.5f, -0.5f, -0.5f) };
      for( int i = 0; i < GET_ARRAY_NUMBER(ainPosi); i++ ) gfl2::math::VEC3Transform(&ainPosi[i], &inTransMat, &ainPosi[i]);
      DrawLineList(ainPosi, GET_ARRAY_NUMBER(ainPosi), inColor);
    }
    {
      gfl2::math::VEC3 ainPosi[] = { gfl2::math::VEC3(-0.5f, -0.5f, 0.5f), gfl2::math::VEC3(-0.5f, -0.5f, -0.5f) };
      for( int i = 0; i < GET_ARRAY_NUMBER(ainPosi); i++ ) gfl2::math::VEC3Transform(&ainPosi[i], &inTransMat, &ainPosi[i]);
      DrawLineList(ainPosi, GET_ARRAY_NUMBER(ainPosi), inColor);
    }
  }

  //-----------------------------------------------------------------------------
  // AABBを描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawAABB(const gfl2::math::AABB& inAABB, const gfl2::math::VEC4& inColor, const bool bIsSimple)
  {
    gfl2::math::Vector4  vector4_inPosi;   vector4_inPosi = inAABB.GetCenter();
    gfl2::math::VEC3  inPosi = ConvertGfVec4ToNwVec3(vector4_inPosi);
    gfl2::math::MTX34 inMat;    gfl2::math::MTX34SetMTX33VEC3(&inMat, System::Camera::internal::MTX33_IDENTITY, inPosi);
    gfl2::math::Vector4  vector4_inLength; vector4_inLength = inAABB.GetDifference();
    gfl2::math::VEC3  inLength = ConvertGfVec4ToNwVec3(vector4_inLength);
    if( bIsSimple ) DrawBoxSimple(inMat, inLength, inColor);
    else            DrawBox(inMat, inLength, inColor);
  }

  void CDebugPrim::DrawAABBCheckSize(const gfl2::math::AABB& inAABB, const gfl2::math::VEC4& inColor, const bool bIsSimple)
  {
    gfl2::math::Vector4 vector4_inLength; vector4_inLength = inAABB.GetDifference();
    gfl2::math::VEC3 inLength = ConvertGfVec4ToNwVec3(vector4_inLength);
    if( CUtil::IsNearlyEqualZeroXYZ(inLength, NEARLY_EQUAL_EPSILON_MOVE) == true )
    {
      gfl2::math::Vector4 vector4_inPosi; vector4_inPosi = inAABB.GetCenter();
      gfl2::math::VEC3 inPosi = ConvertGfVec4ToNwVec3(vector4_inPosi);
      DrawNull(inPosi, UTIL_METER_TO_UNIT(3.0f), inColor); // 確実に視認できるであろうサイズを設定
    }
    else
      DrawAABB(inAABB, inColor, bIsSimple);
  }

  /*
  // [niji注] nijiでは使っていないのでコメントアウト。
  //-----------------------------------------------------------------------------
  // エルミート曲線を描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawHermiteCurve(const gfl::math::VEC3& inSrcPosi, const gfl::math::VEC3& inSrcTangent, const gfl::math::VEC3& inDestPosi, const gfl::math::VEC3& inDestTangent, const int nDivideNum, const gfl::math::VEC4& inColor)
  {
    Hermite inHermite;
    inHermite.AddControlPoints(0,          *reinterpret_cast<const gfl::core::Vector3* >(&inSrcPosi),  *reinterpret_cast<const gfl::core::Vector3* >(&inSrcTangent));
    inHermite.AddControlPoints(nDivideNum, *reinterpret_cast<const gfl::core::Vector3* >(&inDestPosi), *reinterpret_cast<const gfl::core::Vector3* >(&inDestTangent));

    gfl::math::VEC3 inLineSrcPosi(inSrcPosi);
    for( int i = 1; i <= nDivideNum; i++ )
    {
      gfl::math::VEC3 inLineDestPosi;
      inHermite.CalcPositionByTwoPoints(i, reinterpret_cast<gfl::core::Vector3* >(&inLineDestPosi));
      DrawLine(inLineSrcPosi, inLineDestPosi, inColor);
      inLineSrcPosi = inLineDestPosi;
    }
  }
  */

  //-----------------------------------------------------------------------------
  // 枠を描画する(プロジェクション空間版)
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawFrameProjSpace(const gfl2::math::VEC2& __inPosiMax__, const gfl2::math::VEC2& __inPosiMin__, const float fWeightX, const gfl2::math::VEC4& inColor, const float fAspectRatio, const float fZ)
  {
    const gfl2::math::VEC2 inWeightVec(fWeightX, fWeightX*fAspectRatio);
    const gfl2::math::VEC2 inPosiMax(__inPosiMax__ + inWeightVec), inPosiMin(__inPosiMin__ - inWeightVec);
    gfl2::math::VEC2 ainPosi[] = { inPosiMax, inPosiMax, inPosiMin, inPosiMin, inPosiMax };
    ainPosi[1].y = inPosiMin.y;
    ainPosi[3].y = inPosiMax.y;
    gfl2::math::VEC2 ainWeight[] = { System::Camera::internal::VEC2_ZERO, System::Camera::internal::VEC2_ZERO, System::Camera::internal::VEC2_ZERO, System::Camera::internal::VEC2_ZERO };
    ainWeight[0].x = -inWeightVec.x;
    ainWeight[1].y =  inWeightVec.y;
    ainWeight[2].x =  inWeightVec.x;
    ainWeight[3].y = -inWeightVec.y;
    for( int i = 0; i < 4; i++ )
    {
      const gfl2::math::VEC2 inSrcPosi(ainPosi[i]), inDestPosi(ainPosi[i+1]);
      gfl2::math::VEC2 ainVertexPosi[] = { inSrcPosi, inSrcPosi, inDestPosi, inDestPosi };
      ainVertexPosi[1] += ainWeight[i];
      ainVertexPosi[3] += ainWeight[i];
      field::CDebugPrim::DrawQuadranglePolygonProjSpace(ainVertexPosi, inColor, fZ);
    }
  }

  //-----------------------------------------------------------------------------
  // 構図アシストラインを描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawCameraCompositionAssistLine()
  {
    const gfl2::math::VEC4 DIV_COLOR(VEC4_COLOR_WHITE);
    const gfl2::math::VEC4 MASK_COLOR(0.0f, 0.0f, 0.0f, 0.6f);
    const float DIV_OFST = 2.0f/3.0f;
    gfl2::math::VEC2 ainPosi[4];
    float fX, fY;

    // 横分割線
    ainPosi[0].x = ainPosi[1].x =  1.0f;
    ainPosi[2].x = ainPosi[3].x = -1.0f;
    fY = -1.0f;
    for( int i = 0; i < 2; i++ )
    {
      const float WIDTH_HALF = 0.006f;
      fY += DIV_OFST;
      ainPosi[0].y = ainPosi[2].y = fY + WIDTH_HALF;
      ainPosi[1].y = ainPosi[3].y = fY - WIDTH_HALF;
      CDebugPrim::DrawQuadranglePolygonProjSpace(ainPosi, DIV_COLOR);
    }

    // 縦分割線
    ainPosi[0].y = ainPosi[1].y =  1.0f;
    ainPosi[2].y = ainPosi[3].y = -1.0f;
    fX = -1.0f;
    for( int i = 0; i < 2; i++ )
    {
      const float WIDTH_HALF = 0.005f;
      fX += DIV_OFST;
      ainPosi[0].x = ainPosi[2].x = fX + WIDTH_HALF;
      ainPosi[1].x = ainPosi[3].x = fX - WIDTH_HALF;
      CDebugPrim::DrawQuadranglePolygonProjSpace(ainPosi, DIV_COLOR);
    }

    // 横マスク(上だけでよいらしい)
    ainPosi[0].x = ainPosi[1].x =  1.0f;
    ainPosi[2].x = ainPosi[3].x = -1.0f;
    {
      const float WIDTH = 0.06f;
      ainPosi[0].y = ainPosi[2].y = 1.0f - WIDTH;
      ainPosi[1].y = ainPosi[3].y = 1.0f;
      CDebugPrim::DrawQuadranglePolygonProjSpace(ainPosi, MASK_COLOR);
    }

    // 縦マスク
    ainPosi[0].y = ainPosi[1].y =  1.0f;
    ainPosi[2].y = ainPosi[3].y = -1.0f;
    for( int i = 0; i < 2; i++ )
    {
      const float WIDTH = 0.04f;
      fX = (i == 0) ? -1.0f : 1.0f - WIDTH;
      ainPosi[0].x = ainPosi[2].x = fX;
      ainPosi[1].x = ainPosi[3].x = fX + WIDTH;
      CDebugPrim::DrawQuadranglePolygonProjSpace(ainPosi, MASK_COLOR);
    }
  }

  //-----------------------------------------------------------------------------
  // 暗転フィルタを描画する
  //-----------------------------------------------------------------------------
  void CDebugPrim::DrawDarkFilter(const float fRatio) { gfl2::gfx::DrawUtil::FillScreen(gfl2::math::Vector4(0.0f, 0.0f, 0.0f, fRatio)); }

} // namespace field
} } // namespace System namespace Camera

#endif // PM_DEBUG

/*  EOF  */



#if 0

//-----------------------------------------------------------------------------
// 線三角形を描画する
//-----------------------------------------------------------------------------
void tiUtilDebug::drawLineTriangle(const float fSize,const tiMatrix& inWorldMat,const tiVector& inColor)
{
  tiVector ainPosi[4] = { tiVector(0.0f,0.5f,0.0f,1.0f) , tiVector(-0.5f,-0.5f,0.0f,1.0f) , tiVector(0.5f,-0.5f,0.0f,1.0f) };
  const tiMatrix inTransMat = tiMatrix().makeScale(fSize)*inWorldMat;
  for(int i = 0;i < 3;i++)
  {
    ainPosi[i].y += 0.5f/3.0f;
    ainPosi[i]   *= inTransMat;
  }

  ainPosi[3] = ainPosi[0];
  drawLineList(ainPosi,4,inColor);
}

//-----------------------------------------------------------------------------
// 同線楕球
//-----------------------------------------------------------------------------
void tiUtilDebug::drawLineEllipticalSphereFixVertexNum(const tiVector& inRadius,const tiMatrix& inWorldMat,const tiVector& inColor)
{
  const tiMatrix inScaleWorldMat = tiMatrix().makeScale(inRadius)*inWorldMat;
  for(int i = 0;i < TOTAL_LINE_CIRCLE_SPHERE;i++)
  {
    const tiMatrix inTransMat = tiMatrix().makeRotateY(i*OFST_RADI_SPHERE)*inScaleWorldMat;
    drawLineCircleFixVertexNum(1.0f,inTransMat,inColor);
  }
}

//-----------------------------------------------------------------------------
// 同線円筒
//-----------------------------------------------------------------------------
void tiUtilDebug::drawLineCylinderFixVertexNum(const tiVector& inPosi,const tiVector& inUpVector,const float fBottomDist,const float fRadius,const float fLength,const tiVector& inColor)
{
  tiMatrix inWorldMat = tiVector::UNIT_Y.getReachRotMatrix(inUpVector.getNormalize3()); inWorldMat.setTransVector3(inPosi);
  drawLineEllipticalCylinderFixVertexNum(inWorldMat,fBottomDist,fRadius,fRadius,fLength,inColor);
}

//-----------------------------------------------------------------------------
// 同線楕円筒
//-----------------------------------------------------------------------------
void tiUtilDebug::drawLineEllipticalCylinderFixVertexNum(const tiMatrix& inWorldMat,const float fBottomDist,const float fRadiusX,const float fRadiusZ,const float fLength,const tiVector& inColor)
{
  const tiMatrix inRotMat90      = tiMatrix().makeRotateX(TIL_PI_2);
  const tiVector inUpDir         = inWorldMat.getUpVector();
  const tiVector inBottomPosi    = inWorldMat.getTransVector() - fBottomDist*inUpDir;
  const tiMatrix inScaleWorldMat = tiMatrix(tiMatrix().makeScale(fRadiusX,1.0f,fRadiusZ)*inWorldMat).setTransVector3(inBottomPosi);

  //drawArrow(inBottomPosi,inUpDir,fLength,inColor);

  const int TOTAL_LINE_CIRCLE = 10;
  const float fYOfst = fLength/(TOTAL_LINE_CIRCLE-1);
  for(int i = 0;i < TOTAL_LINE_CIRCLE;i++)
  {
    const tiMatrix inTransMat = tiMatrix(inRotMat90).setTransVector3(tiVector(0.0f,fYOfst*i,0.0f))*inScaleWorldMat;
    drawLineCircleFixVertexNum(1.0f,inTransMat,inColor);
  }
}

#endif





