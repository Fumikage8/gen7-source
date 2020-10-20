//======================================================================
/**
 * @file FieldDebugCollisionRenderPath.cpp
 * @date 2015/07/13 16:53:53
 * @author ikawa_hiroki
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <GameSys/include/GameManager.h>
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <heap/include/gfl2_HeapMacros.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <Renderingengine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_UtilCollisionRenderPath.h>

#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"

#include "math/include/gfl2_math_control.h"

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/Collision/BaseShape.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldEnvSoundCollisionAccessor.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"

#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaCollisionManager.h"

#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderManager.h"
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderPath.h"
#include "Field/FieldRo/include/Debug/FieldDebugInfo.h"

#include "Field/FieldDebug/include/FieldDebugTrafficInfo.h"

#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"

#define FIELD_DEBUG_COL_RENDER_DUMP 0


static const u32 DIV_CIRCLE = 8; // 円、球のポリゴン分割数


GFL_NAMESPACE_BEGIN( gfl2 )

DebugCollisionRenderPath::DebugCollisionRenderPath() :
  IRenderPath(),
  m_pVertexFormatList(NULL),
  m_pCurrentTerrain(NULL)
  ,m_DrawPrimitiveMax( 0 )
  ,m_TotalDrawPrimitiveCount( 0 )       
  ,m_IsDrawOver( false )
#if defined(GF_PLATFORM_WIN32)
  ,m_DepthStencilStateObject( NULL )
#endif
{
  m_pTrafficInfo = NULL;
#if defined(GF_PLATFORM_CTR)
  m_DrawPrimitiveMax = gfl2::gfx::DrawUtil::GetPrimitiveMaxExtension() * 0.875;   // @note デバッガ接続時は35000、ROM起動時は3500
#endif
#if defined(GF_PLATFORM_WIN32)
  m_DrawPrimitiveMax = 99999; // 制限無し
#endif
}

DebugCollisionRenderPath::~DebugCollisionRenderPath()
{
  Terminate();
}

void DebugCollisionRenderPath::Initialize( const InitDescription& desc, System::nijiAllocator* pAllocator  )
{
  // ヒープ領域取得
  gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

  // 地形コリジョン表示マネージャー
  m_pCollisionRenderManager = GFL_NEW( pDebugHeap ) Field::Debug::DebugCollisionRenderManager();

  // 初期化設定保存
  m_InitDescription = desc;

  // 頂点フォーマットリスト作成
  m_VertexFormatLimit = desc.maxTriangleCount * 3;
  m_pVertexFormatList = GLNewArray( pAllocator ) gfl2::gfx::DrawUtil::VertexFormat[m_VertexFormatLimit];

  m_BorderCulling = desc.borderProjectionCulling;
  m_BorderDepthCulling = desc.borderDepthCulling;
  m_BordarDivide = desc.borderSurfaceDivide;

  Field::Traffic::DebugInfo::Desc traffic_desc;
  traffic_desc.pHeap = pDebugHeap;
  traffic_desc.pTrafficManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetTrafficManager();
  if( traffic_desc.pTrafficManager )
  {
    m_pTrafficInfo = GFL_NEW( pDebugHeap ) Field::Traffic::DebugInfo(traffic_desc);
  }

#if defined(GF_PLATFORM_WIN32)
  m_DepthStencilStateObject = gfl2::gfx::GFGL::CreateDepthStencilStateObject(pAllocator);
  m_DepthStencilStateObject->SetDepthTestEnable(false);
  m_DepthStencilStateObject->SetDepthWriteEnable(true);
  m_DepthStencilStateObject->SetStencilOp( gfl2::gfx::PolygonFace::CW, gfl2::gfx::StencilOp::Keep, gfl2::gfx::StencilOp::Keep, gfl2::gfx::StencilOp::Replace);
  m_DepthStencilStateObject->SetStencilFunc( gfl2::gfx::PolygonFace::CW, gfl2::gfx::CompareFunc::Always, 0x00, 0xFF );
  m_DepthStencilStateObject->SetStencilTestEnable(true);
  m_DepthStencilStateObject->SetStencilWriteMask(0xFF);
  m_DepthStencilStateObject->UpdateState();
#endif
}

void  DebugCollisionRenderPath::Terminate( void )
{
#if defined(GF_PLATFORM_WIN32)
  GFL_SAFE_DELETE( m_DepthStencilStateObject );
#endif

  GFL_SAFE_DELETE( m_pTrafficInfo );

  if (m_pVertexFormatList != NULL)
  {
    GFL_SAFE_DELETE( m_pCollisionRenderManager );

    GFL_DELETE_ARRAY m_pVertexFormatList;
  }
}

void DebugCollisionRenderPath::SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
	gfl2::gfx::GFGL::SetRenderTarget( pRenderTarget );
	gfl2::gfx::GFGL::SetDepthStencil( pDepthStencil );
}

void DebugCollisionRenderPath::ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  (void)pRenderTarget;
  (void)pDepthStencil;

   // レンダーターゲットのクリアはしない
}


/**
* @brief カリング判定式
* @param[in] pVertex 頂点情報
* @param[in] size    チェック頂点数
* @detail 
*/
bool DebugCollisionRenderPath::CheckCulling( math::Vector3* pVertex, u32 size )
{
  // @note 三角形毎にAABBを作り判定しているため、無駄が多い。 @note 描画頂点数制限が厳しく、メインロジックを変更せずにカリング不具合を直す必要があった
  if( size == 0 )
  {
    return false;
  }

  // AABBを作成
  gfl2::math::AABB aabb;
  aabb.SetMinMax( *pVertex, *pVertex );
  pVertex++;

  for( u32 i = 1; i < size; i++, pVertex++ )
  {
    aabb = aabb.Merge( *pVertex );
  }

  if( gfl2::math::AABB::Util::IsIntersectFrustum( &m_CullingMatrixTable[0], aabb ) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
* @brief 三角形の間引きチェック込みで頂点リストに頂点を登録
* @param[in] position  位置
* @param[in] color  色指定
* @param[out] vertexList  頂点リスト
* @param[out] vertexCount 頂点数カウンター
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::RegistTriangleWithCullingCheck( math::Vector3 position[3], math::Vector4 color, gfx::DrawUtil::VertexFormat* vertexList, u32 &vertexCount )
{
  // カリングチェック
  if( !CheckCulling( position, 3 ) )
  {
    return vertexCount;
  }

  // 頂点数上限チェック (1コール)
  if( vertexCount >= m_VertexFormatLimit - 3 )
  {
    m_IsDrawOver = true;
    return vertexCount;
  }
   
  // 頂点数上限チェック (1フレーム)
  if( m_TotalDrawPrimitiveCount + 3 >= m_DrawPrimitiveMax )
  {
    m_IsDrawOver = true;
    return vertexCount;
  }
    
  for(u32 i=0; i<3; i++)
  {
    gfl2::gfx::DrawUtil::VertexFormat& vertex = vertexList[vertexCount++];
    vertex.pos = position[i];
    vertex.color = color;
    m_TotalDrawPrimitiveCount++;
  }

  return vertexCount;

}

/**
* @brief シリンダーの描画
* @param[in] position  位置
* @param[in] quaternion クォータニオン
* @param[in] pShape CylinderShapeのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawCylinderShape( math::Vector3 position, math::Quaternion quaternion, CylinderShape* pShape, math::Vector4 color )
{
  f32 r = pShape->m_r;
  f32 height = pShape->m_height;

  if( r != 0.0f )
  {
    math::Matrix34 mat;
    quaternion.QuaternionToMatrix( mat );

    gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;    // 頂点リスト
    u32 vertexCount = 0;                    // 頂点数

    math::Vector3 vh( 0.0f, height, 0.0f ); // 高さベクトル

    for (u32 i = 0; i < DIV_CIRCLE; ++i)
    {
      // 中心と外周の2頂点
      math::Vector3 v0 = mat.TransformCoord( pShape->m_pos );
      math::Vector3 v1 = mat.TransformCoord( math::Vector3( math::FCos(2.0f * PI * static_cast<f32>(i) / DIV_CIRCLE) * r, 0.0f, math::FSin(2.0f * PI * static_cast<f32>(i) / DIV_CIRCLE) * r) + pShape->m_pos );
      math::Vector3 v2 = mat.TransformCoord( math::Vector3( math::FCos(2.0f * PI * static_cast<f32>(i+1) / DIV_CIRCLE) * r, 0.0f, math::FSin(2.0f * PI * static_cast<f32>(i+1) / DIV_CIRCLE) * r) + pShape->m_pos );

      // 天面
      {
        math::Vector3 pos[3] =
        {
          v0 + vh + position,
          v1 + vh + position,
          v2 + vh + position,
        };
        RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
      }
      // 側面
      if(height != 0.0f)
      {
        // 側面1
        {
          math::Vector3 pos[3] =
          {
            v1 + vh * 0.99f + position,   // 少し透き間をあけて形状を把握しやすく
            v1 + position,
            v2 + vh * 0.99f + position,   // 少し透き間をあけて形状を把握しやすく
          };
          RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
        }
        // 側面2
        {
          math::Vector3 pos[3] =
          {
            v1 + position,
            v2 + position,
            v2 + vh * 0.99f + position,   // 少し透き間をあけて形状を把握しやすく
          };
          RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
        }
      }
    }
    // 頂点リスト描画
    gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);

    return vertexCount;
  }
  return 0;
}


/**
* @brief 球の描画
* @param[in] position  位置
* @param[in] quaternion クォータニオン
* @param[in] pShape SphereShapeのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawSphereShape( math::Vector3 position, math::Quaternion quaternion, SphereShape* pShape, math::Vector4 color )
{
  f32 r = pShape->m_r;

  if( r != 0.0f )
  {
    math::Matrix34 mat;
    quaternion.QuaternionToMatrix( mat );

    gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;   // 頂点リスト
    u32 vertexCount = 0;                      // 頂点数

    math::Vector3 v0 = mat.TransformCoord( pShape->m_pos );   // 中心ベクトル
    math::Vector3 vh = mat.TransformCoord( math::Vector3( 0.0f, r, 0.0f )) ; // 高さベクトル

    for (u32 i = 0; i < DIV_CIRCLE; ++i)
    {
      // 外周の2頂点
      math::Vector3 v1 = mat.TransformCoord( math::Vector3( math::FCos(2.0f * PI * static_cast<f32>(i) / DIV_CIRCLE) * r, 0.0f, math::FSin(2.0f * PI * static_cast<f32>(i) / DIV_CIRCLE) * r) + pShape->m_pos );
      math::Vector3 v2 = mat.TransformCoord( math::Vector3( math::FCos(2.0f * PI * static_cast<f32>(i+1) / DIV_CIRCLE) * r, 0.0f, math::FSin(2.0f * PI * static_cast<f32>(i+1) / DIV_CIRCLE) * r) + pShape->m_pos );

      for( u32 j = 0; j < DIV_CIRCLE/2; ++j )
      {
        f32 ratioVertical0 = math::FCos( 2.0 * PI * static_cast<f32>(j) / DIV_CIRCLE );
        f32 ratioVertical1 = math::FCos( 2.0 * PI * static_cast<f32>(j+1) / DIV_CIRCLE );
        f32 ratioHorizontal0 = math::FSin( 2.0 * PI * static_cast<f32>(j) / DIV_CIRCLE );
        f32 ratioHorizontal1 = math::FSin( 2.0 * PI * static_cast<f32>(j+1) / DIV_CIRCLE );

        if( j != 0 )  // 上極では描かない
        {
          math::Vector3 pos[3] =
          {
            v1 * ratioHorizontal0 + vh * ratioVertical0 + position,
            v2 * ratioHorizontal0 + vh * ratioVertical0 + position,
            v1 * ratioHorizontal1 + vh * ratioVertical1 + position,
          };
          RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
        }
        if( j != ( DIV_CIRCLE/2 ) - 1 )  // 下極では描かない
        {
          math::Vector3 pos[3] =
          {
            v2 * ratioHorizontal0 + vh * ratioVertical0 + position,
            v2 * ratioHorizontal1 + vh * ratioVertical1 + position,
            v1 * ratioHorizontal1 + vh * ratioVertical1 + position,
          };
          RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
        }
      }
    }
    // 頂点リスト描画
    gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);

    return vertexCount;
  }
  return 0;
}

/**
* @brief ボックスの描画
* @param[in] position  位置
* @param[in] quaternion クォータニオン
* @param[in] pShape BoxShapeのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawBoxShape( math::Vector3 position, math::Quaternion quaternion, BoxShape* pShape, math::Vector4 color )
{
  if(( pShape->m_e0 != 0.0f ) && ( pShape->m_e1 != 0.0f ) && ( pShape->m_height != 0.0f ))
  {
    math::Matrix34 mat;
    quaternion.QuaternionToMatrix( mat );   // 回転マトリクス生成

    gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;   // 頂点リスト
    u32 vertexCount = 0;                      // 頂点数

    math::Vector3 height( 0.0f, pShape->m_height, 0.0f ); // 高さベクトル
    math::Vector3 vh = mat.TransformCoord( height );
    math::Vector3 vw = mat.TransformCoord( pShape->m_u0 * pShape->m_e0 ); // 幅方向ベクトル
    math::Vector3 vd = mat.TransformCoord( pShape->m_u1 * pShape->m_e1 ); // 奥方向ベクトル
    math::Vector3 pos = mat.TransformCoord( pShape->m_pos );

    math::Vector3 vChink_h = vh * 0.02f;    // 輪郭わかるように隙間をあける
    math::Vector3 vChink_w =  mat.TransformCoord( pShape->m_u0 );

#if 0
    u32 div_h = 1 + vh.Length() / (m_BordarDivide / 2);
    u32 div_w = 1 + vw.Length() / (m_BordarDivide / 2);
    u32 div_d = 1 + vd.Length() / (m_BordarDivide / 2);
#endif
    u32 div_h = 1;
    u32 div_w = 1;
    u32 div_d = 1;

    for(u32 i=0; i<div_d; i++)
    {
      for(u32 j=0; j<div_w; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 天面1
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_d))) + vh + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_d))) + vh + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_d))) + vh + pos + position,
          };
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 天面2
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_d))) + vh + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_d))) + vh + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_d))) + vh + pos + position,
          };
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    for(u32 i=0; i<div_h; i++)
    {
      for(u32 j=0; j<div_w; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 前面1
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) - vd + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) - vd + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) - vd + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[0] += vChink_w;
            vert[2] += vChink_w;
          }
          else if( j == div_w - 1)
          {
            vert[1] -= vChink_w;
          }
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 前面2
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) - vd + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) - vd + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) - vd + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[2] += vChink_w;
          }
          else if( j == div_w - 1)
          {
            vert[0] -= vChink_w;
            vert[1] -= vChink_w;
          }
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    for(u32 i=0; i<div_h; i++)
    {
      for(u32 j=0; j<div_w; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 背面1
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[1] += vChink_w;
          }
          else if( j == div_w - 1)
          {
            vert[0] -= vChink_w;
            vert[2] -= vChink_w;
          }
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 背面2
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[0] += vChink_w;
            vert[1] += vChink_w;
          }
          else if( j == div_w - 1)
          {
            vert[2] -= vChink_w;
          }
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    for(u32 i=0; i<div_h; i++)
    {
      for(u32 j=0; j<div_d; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 側面1
        {
          math::Vector3 vert[3] =
          {
            -vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            -vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            -vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 側面2
        {
          math::Vector3 vert[3] =
          {
            -vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
            -vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
            -vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    for(u32 i=0; i<div_h; i++)
    {
      for(u32 j=0; j<div_d; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 側面3
        {
          math::Vector3 vert[3] =
          {
             vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
             vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
             vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 側面4
        {
          math::Vector3 vert[3] =
          {
             vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i+1) / static_cast<f32>(div_h))) + pos + position,
             vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
             vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * ((static_cast<f32>(i) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    // @todo 底面は？


    // 頂点リスト描画
    gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);
    return vertexCount;
  }
  return 0;
}

/**
* @brief OBBボックスの描画
* @DrawBoxShapeを元に、Z軸回転にも対応
* @param[in] position  位置
* @param[in] quaternion クォータニオン
* @param[in] pShape BoxShapeのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawObbShape( math::Vector3 position, math::Quaternion quaternion, ObbShape* pShape, math::Vector4 color )
{
  if(( pShape->m_e0 != 0.0f ) && ( pShape->m_e1 != 0.0f ) && ( pShape->m_e2 != 0.0f ))
  {
    math::Matrix34 mat;
    quaternion.QuaternionToMatrix( mat );   // 回転マトリクス生成

    gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;   // 頂点リスト
    u32 vertexCount = 0;                      // 頂点数

    math::Vector3 vh = mat.TransformCoord( pShape->m_u1 * pShape->m_e1 ); // 縦方向ベクトル
    math::Vector3 vw = mat.TransformCoord( pShape->m_u0 * pShape->m_e0 ); // 幅方向ベクトル
    math::Vector3 vd = mat.TransformCoord( pShape->m_u2 * pShape->m_e2 ); // 奥方向ベクトル
    math::Vector3 pos = mat.TransformCoord( pShape->m_pos );

    math::Vector3 vChink_h = vh * 0.02f;    // 輪郭わかるように隙間をあける
    math::Vector3 vChink_w =  mat.TransformCoord( pShape->m_u0 );

#if 0
    u32 div_h = 1 + vh.Length() / (m_BordarDivide / 2);
    u32 div_w = 1 + vw.Length() / (m_BordarDivide / 2);
    u32 div_d = 1 + vd.Length() / (m_BordarDivide / 2);
#endif
    u32 div_h = 1;
    u32 div_w = 1;
    u32 div_d = 1;

    for(u32 i=0; i<div_d; i++)
    {
      for(u32 j=0; j<div_w; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 天面1
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_d))) + vh + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_d))) + vh + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_d))) + vh + pos + position,
          };
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 天面2
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_d))) + vh + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_d))) + vh + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_d))) + vh + pos + position,
          };
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    for(u32 i=0; i<div_h; i++)
    {
      for(u32 j=0; j<div_w; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 前面1
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) - vd + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) - vd + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) - vd + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[0] += vChink_w;
            vert[2] += vChink_w;
          }
          else if( j == div_w - 1)
          {
            vert[1] -= vChink_w;
          }
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 前面2
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) - vd + vh * ( -1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) - vd + vh * ( -1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) - vd + vh * ( -1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[2] += vChink_w;
          }
          else if( j == div_w - 1)
          {
            vert[0] -= vChink_w;
            vert[1] -= vChink_w;
          }
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    for(u32 i=0; i<div_h; i++)
    {
      for(u32 j=0; j<div_w; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 背面1
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[1] += vChink_w;
          }
          else if( j == div_w - 1)
          {
            vert[0] -= vChink_w;
            vert[2] -= vChink_w;
          }
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 背面2
        {
          math::Vector3 vert[3] =
          {
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_w))) + vd + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
            vw * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_w))) + vd + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[0] += vChink_w;
            vert[1] += vChink_w;
          }
          else if( j == div_w - 1)
          {
            vert[2] -= vChink_w;
          }
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    for(u32 i=0; i<div_h; i++)
    {
      for(u32 j=0; j<div_d; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 側面1
        {
          math::Vector3 vert[3] =
          {
            -vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            -vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            -vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 側面2
        {
          math::Vector3 vert[3] =
          {
            -vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
            -vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
            -vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    for(u32 i=0; i<div_h; i++)
    {
      for(u32 j=0; j<div_d; j++)
      {
        math::Vector4 col = (i%2 == j%2) ? color : color * 0.5f;
        // 側面3
        {
          math::Vector3 vert[3] =
          {
             vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
             vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
             vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 側面4
        {
          math::Vector3 vert[3] =
          {
             vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>((i+1)*2) / static_cast<f32>(div_h))) + pos + position,
             vw + vd * (-1.0f + (static_cast<f32>((j+1)*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
             vw + vd * (-1.0f + (static_cast<f32>(j*2) / static_cast<f32>(div_d))) + vh * (-1.0f + (static_cast<f32>(i*2) / static_cast<f32>(div_h))) + pos + position,
          };
          // 輪郭にあたる部分に透き間を追加
          if( i == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }

    // @todo 底面は？


    // 頂点リスト描画
    gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);
    return vertexCount;
  }
  return 0;
}

/**
* @brief Triangleの描画
* @param[in] position  位置
* @param[in] quaternion クォータニオン
* @param[in] pShape BoxShapeのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawTriangleShape( math::Vector3 position, math::Quaternion quaternion, TriangleShape* pShape, math::Vector4 color )
{
  math::Vector3 vert[3] =
  {
    pShape->m_pos0 + position,
    pShape->m_pos1 + position,
    pShape->m_pos2 + position,
  };

  gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;
  u32 vertexCount = 0;
  for(u32 i=0; i<3; i++)
  {
    gfl2::gfx::DrawUtil::VertexFormat& vertex = vertexList[vertexCount++];
    vertex.pos = vert[i];
    vertex.color = color;
  }

  // 頂点リスト描画
  gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);
  return vertexCount;
}

/**
* @brief コリジョンモデルの描画
* @param[in] position  位置
* @param[in] quaternion クォータニオン
* @param[in] pShape MeshShapeのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawCollisionModel( math::Vector3 position, math::Quaternion quaternion, collision::CollisionModel* pModel, math::Vector4 color, const gfl2::math::Vector4* pAttrColorTable, u32 attrColorTableSize )
{
  if( pModel != NULL)
  {
    math::Matrix34 mat;
    quaternion.QuaternionToMatrix( mat );

    gfl2::renderingengine::scenegraph::resource::CollisionMeshResourceNode* pNode = pModel->GetCollisionMeshNode();
    u32 triangleCount = pNode->GetCollisionTriangleCount();
//    DebugCollisionRenderPath::InitDescription* desc = &m_InitDescription;                // 初期化設定
    gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;   // 頂点リスト
    u32 vertexCount = 0;                      // 頂点数

    // ノードを巡回して頂点リストを作成する
    for (u32 i = 0; i < triangleCount; ++i)
    {
      const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle = pNode->GetCollisionTriangle(i);
      math::Vector4 colorWork( color );

      // カラーテーブルを参照
      if( pAttrColorTable )
      {
        if( pTriangle->m_GroundAttribute >= 0 && pTriangle->m_GroundAttribute < attrColorTableSize )
        {
          colorWork = pAttrColorTable[ pTriangle->m_GroundAttribute ];
        }
      }

      // 非表示
      if( colorWork.w == 0.0f )
      {
        continue;
      }

      // ポリゴン間に透き間をあけて形状を把握しやすくするため、ポリゴンの中心を取得
      math::Vector3 center = math::Vector3( pTriangle->m_Positions[0].x, pTriangle->m_Positions[0].y, pTriangle->m_Positions[0].z ) + 
        math::Vector3( pTriangle->m_Positions[1].x, pTriangle->m_Positions[1].y, pTriangle->m_Positions[1].z ) +
        math::Vector3( pTriangle->m_Positions[2].x, pTriangle->m_Positions[2].y, pTriangle->m_Positions[2].z ) + 
        position;
      center /= 3.0f;
      math::Vector3 pos[3];
      for( u32 j=0; j<3; j++)
      {
        pos[j] = mat.TransformCoord( math::Vector3( pTriangle->m_Positions[j].x, pTriangle->m_Positions[j].y, pTriangle->m_Positions[j].z ) + position);
        pos[j] += (center - pos[j]) * 0.01f;
      }

      RegistTriangleWithCullingCheck( pos, colorWork, vertexList, vertexCount );
    }

    // 頂点リスト描画
    if( vertexCount > 0 )
    {
      gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);
    }

    return vertexCount;
  }
  return 0;
}

u32 DebugCollisionRenderPath::DrawCollisionModel( math::Vector3 position, math::Quaternion quaternion, collision::CollisionModel* pModel, math::Vector4 color, f32 clampY, s32 hitAttr )
{
  math::Vector4 hitColor = math::Vector4( 1.0f, 0.0f, 0.0f, 1.0f );
  math::Vector4 colorTable[] =
  {
    math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f ),
    math::Vector4( 0.0f, 1.0f, 0.0f, 1.0f ),
    math::Vector4( 0.0f, 0.0f, 1.0f, 1.0f ),
    math::Vector4( 1.0f, 1.0f, 0.0f, 1.0f ),
    math::Vector4( 1.0f, 0.0f, 1.0f, 1.0f ),
    math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ),
    math::Vector4( 0.0f, 1.0f, 1.0f, 1.0f ),
    math::Vector4( 0.0f, 0.0f, 1.0f, 1.0f ),
    math::Vector4( 0.5f, 0.0f, 0.0f, 1.0f ),
    math::Vector4( 0.0f, 0.5f, 0.0f, 1.0f ),
    math::Vector4( 0.0f, 0.0f, 0.5f, 1.0f ),
    math::Vector4( 0.5f, 0.5f, 0.0f, 1.0f ),
    math::Vector4( 0.5f, 0.0f, 0.5f, 1.0f ),
    math::Vector4( 0.5f, 0.5f, 0.5f, 1.0f ),
    math::Vector4( 0.0f, 0.5f, 0.5f, 1.0f ),
    math::Vector4( 0.0f, 0.0f, 0.5f, 1.0f ),
  };


  if( pModel != NULL)
  {
    math::Matrix34 mat;
    quaternion.QuaternionToMatrix( mat );

    gfl2::renderingengine::scenegraph::resource::CollisionMeshResourceNode* pNode = pModel->GetCollisionMeshNode();
    u32 triangleCount = pNode->GetCollisionTriangleCount();
//    DebugCollisionRenderPath::InitDescription* desc = &m_InitDescription;                // 初期化設定
    gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;   // 頂点リスト
    u32 vertexCount = 0;                      // 頂点数

    // ノードを巡回して頂点リストを作成する
    for (u32 i = 0; i < triangleCount; ++i)
    {
      const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle = pNode->GetCollisionTriangle(i);

      if( pTriangle->m_GroundAttribute == -1 )
      {
        continue;
      }

      // ポリゴン間に透き間をあけて形状を把握しやすくするため、ポリゴンの中心を取得
      math::Vector3 center = math::Vector3( pTriangle->m_Positions[0].x, pTriangle->m_Positions[0].y, pTriangle->m_Positions[0].z ) + 
        math::Vector3( pTriangle->m_Positions[1].x, pTriangle->m_Positions[1].y, pTriangle->m_Positions[1].z ) +
        math::Vector3( pTriangle->m_Positions[2].x, pTriangle->m_Positions[2].y, pTriangle->m_Positions[2].z ) + 
        position;
      center /= 3.0f;
      math::Vector3 pos[3];
      for( u32 j=0; j<3; j++)
      {
        pos[j] = mat.TransformCoord( math::Vector3( pTriangle->m_Positions[j].x, pTriangle->m_Positions[j].y, pTriangle->m_Positions[j].z ) + position);
        pos[j] += (center - pos[j]) * 0.01f;
      }

      // 表示色
      math::Vector4 color;
      if( pTriangle->m_GroundAttribute == hitAttr )
      {
        color = hitColor;
      }
      else
      {
        color = colorTable[ pTriangle->m_GroundAttribute % GFL_NELEMS( colorTable ) ];
      }

      RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
    }
    // 頂点リスト描画
    gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);

    return vertexCount;
  }
  return 0;
}


/**
* @brief コリジョンモデルの描画
* @param[in] pModel CollisionModelのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawCollisionModel( collision::CollisionModel* pModel, math::Vector4 color )
{
  return DrawCollisionModel( math::Vector3( 0.0f, 0.0f, 0.0f ), math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f), pModel, color );
}  

/**
* @brief メッシュの描画
* @param[in] position  位置
* @param[in] pCollisionModel コリジョンモデルのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawMeshShape( math::Vector3 position, math::Quaternion quaternion, MeshShape* pShape, math::Vector4 color )
{
  collision::CollisionModel* pModel = pShape->m_pCollisionModel;

  return DrawCollisionModel( position, quaternion, pModel, color );
};


/**
* @brief 直線の描画
* @param[in] position  位置
* @param[in] quaternion クォータニオン
* @param[in] pShape MeshShapeのポインタ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawLineShape( math::Vector3 position, math::Quaternion quaternion, LineShape* pShape, math::Vector4 color )
{
  if( pShape->m_height != 0.0f )
  {
    math::Matrix34 mat;
    quaternion.QuaternionToMatrix( mat );   // 回転マトリクス生成

    gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;   // 頂点リスト
    u32 vertexCount = 0;                      // 頂点数

    math::Vector3 height( 0.0f, pShape->m_height, 0.0f ); // 高さベクトル
    math::Vector3 vh = mat.TransformCoord( height );
    math::Vector3 vpos0 = mat.TransformCoord( pShape->m_pos0 );
    math::Vector3 vpos1 = mat.TransformCoord( pShape->m_pos1 );

    {
      math::Vector3 pos[3] =
      {
        vpos0 + vh + position,
        vpos1 + vh + position,
        vpos0 + position,
      };
      RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
    }
    {
      math::Vector3 pos[3] =
      {
        vpos1 + vh + position,
        vpos1 + position,
        vpos0 + position,
      };
      RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
    }
    // 頂点リスト描画
    gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);

    return vertexCount;
  }
  return 0;
}


/**
* @brief コリジョンプリミティブ(円)の描画
* @param[in] position  位置
* @param[in] radius   半径
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawCircle( math::Vector3 position, f32 radius, math::Vector4 color )
{
  if( radius != 0.0f )
  {
    gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;   // 頂点リスト
    u32 vertexCount = 0;                      // 頂点数

    for (u32 i = 0; i < DIV_CIRCLE; ++i)
    {
      // 中心と外周の2頂点
      math::Vector3 v0 = position;
      math::Vector3 v1 = math::Vector3( math::FCos(2.0f * PI * static_cast<f32>(i) / DIV_CIRCLE) * radius, 0.0f, math::FSin(2.0f * PI * static_cast<f32>(i) / DIV_CIRCLE) * radius ) + position;
      math::Vector3 v2 = math::Vector3( math::FCos(2.0f * PI * static_cast<f32>(i+1) / DIV_CIRCLE) * radius, 0.0f, math::FSin(2.0f * PI * static_cast<f32>(i+1) / DIV_CIRCLE) * radius ) + position;
      math::Vector3 pos[3] =
      {
        v0, v1, v2,
      };
      RegistTriangleWithCullingCheck( pos, color, vertexList, vertexCount );
    }
    // 頂点リスト描画
    gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);

    return vertexCount;
  }
  return 0;
}


/**
* @brief 直線周辺範囲の描画
* @param[in] startpos  始点位置
* @param[in] endpos  終点位置
* @param[in] width   幅
* @param[in] height  高さ
* @param[in] color  色指定
* @return 頂点数
* @detail 
*/
u32 DebugCollisionRenderPath::DrawLineArea( math::Vector3 startpos, math::Vector3 endpos, f32 width, f32 height, math::Vector4 color )
{
  GameSys::GameManager* pgm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::Fieldmap* pfm = pgm->GetFieldmap();

  /**
  LineShape Line;
  math::Vector4 dummycolor(1.0f, 0.0f, 0.0f);
  Line.m_height = height;
  math::Vector3 pos0(startpos.x, startpos.y, startpos.z);
  math::Vector3 pos1(endpos.x, endpos.y, endpos.z);
  Line.m_pos0 = pos0;
  Line.m_pos1 = pos1;
  DrawLineShape( math::Vector3(0.0f, 0.0f, 0.0f), gfl2::math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &Line, dummycolor );
  **/

  // 直線のベクトル
  math::Vector3 vLine((endpos.x - startpos.x), (endpos.y - startpos.y), (endpos.z - startpos.z));

  math::Vector3 vNormal = vLine.Normalize();  // 直線方向単位ベクトル
  math::Vector3 vBase( 0.0f, 0.0f, 1.0f );    // z+方向を基準とする

  math::Vector3 vCross = vBase.Cross(vNormal);    // 外積から回転の向き取得
  f32 rad = math::AcosRad( vBase.Dot(vNormal) );  // 内積から回転角取得

  math::Quaternion quat(vCross, rad);

  math::Matrix34 mat;
  quat.QuaternionToMatrix( mat );

  math::Vector3 vh = mat.TransformCoord( math::Vector3( 0.0f, height, 0.0f ) );
  math::Vector3 vw = mat.TransformCoord( math::Vector3( width, 0.0f, 0.0f ) ); // 幅方向ベクトル
  math::Vector3 vd = mat.TransformCoord( math::Vector3( 0.0f, 0.0f, vLine.Length() ) ); // 奥方向ベクトル
  math::Vector3 pos = startpos;

  f32 h_Player = pfm->GetPlayerPosition().y;
  math::Vector3 vb = mat.TransformCoord( math::Vector3( 0.0f, h_Player, 0.0f ) );

  math::Vector3 vChink_h = vh * 0.005f;  // 輪郭がわかるように隙間をあけるためのベクトル
//  math::Vector3 vChink_h = vh.Normalize();  // 輪郭がわかるように隙間をあけるためのベクトル

  gfl2::gfx::DrawUtil::VertexFormat* vertexList = m_pVertexFormatList;   // 頂点リスト
  u32 vertexCount = 0;                      // 頂点数

#if 0
  s32 div_h = 1 + vh.Length() / m_BordarDivide;
  s32 div_w = 1 + vw.Length() / m_BordarDivide;
  s32 div_d = 1 + vd.Length() / m_BordarDivide;
#endif
  u32 div_h = 1;
  u32 div_w = 1;
  u32 div_d = 1;
  for(s32 side=0; side<2; side++) // ラインの両端
  {
    for(s32 i=0; i<div_d; i++)   // ライン水平方向
    {
      for(s32 j=0; j<div_w; j++)  // ライン垂直方向
      {
        math::Vector4 col = (i%2 == (j+side)%2) ? color : color * 0.5f;
        // 天面1
        {
          math::Vector3 vert[3] =
         {
            vw * (static_cast<f32>(((side)?1:-1) * (j+1)) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + vh + pos,
            vw * (static_cast<f32>(((side)?1:-1) * j) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + vh + pos,
            vw * (static_cast<f32>(((side)?1:-1) * (j+1)) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + vh + pos,
          };
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 天面2
        {
          math::Vector3 vert[3] =
          {
            vw * (static_cast<f32>(((side)?1:-1) * j) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + vh + pos,
            vw * (static_cast<f32>(((side)?1:-1) * j) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + vh + pos,
            vw * (static_cast<f32>(((side)?1:-1) * (j+1)) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + vh + pos,
          };
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 底面1
       {
          math::Vector3 vert[3] =
          {
            vw * (static_cast<f32>(((side)?1:-1) * (j+1)) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + pos,
            vw * (static_cast<f32>(((side)?1:-1) * j) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + pos,
            vw * (static_cast<f32>(((side)?1:-1) * (j+1)) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + pos,
          };
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
       }
        // 底面2
        {
          math::Vector3 vert[3] =
          {
            vw * (static_cast<f32>(((side)?1:-1) * j) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + pos,
            vw * (static_cast<f32>(((side)?1:-1) * j) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + pos,
            vw * (static_cast<f32>(((side)?1:-1) * (j+1)) / static_cast<f32>(div_w)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + pos,
          };
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }
  }

  for(s32 side=0; side<2; side++) // ラインの両端
  {
    for(s32 i=0; i<div_d; i++)   // ライン水平方向
    {
      for(s32 j=0; j<div_h; j++)  // 垂直方向
      {
        math::Vector4 col = (i%2 == (j+side)%2) ? color : color * 0.5f;
        // 側面1
        {
          math::Vector3 vert[3] =
          {
            vw * (static_cast<f32>((side)?1:-1)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + vh * (static_cast<f32>(j+1) / static_cast<f32>(div_h)) + pos,
            vw * (static_cast<f32>((side)?1:-1)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + vh * (static_cast<f32>(j+1) / static_cast<f32>(div_h)) + pos,
            vw * (static_cast<f32>((side)?1:-1)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + vh * (static_cast<f32>(j) / static_cast<f32>(div_h)) + pos,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[2] += vChink_h;
          }
          else if( j == div_h - 1 )
          {
            vert[0] -= vChink_h;
            vert[1] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
        // 側面2
        {
          math::Vector3 vert[3] =
          {
            vw * (static_cast<f32>((side)?1:-1)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + vh * (static_cast<f32>(j+1) / static_cast<f32>(div_h)) + pos,
            vw * (static_cast<f32>((side)?1:-1)) + vd * (static_cast<f32>(i) / static_cast<f32>(div_d)) + vh * (static_cast<f32>(j) / static_cast<f32>(div_h)) + pos,
            vw * (static_cast<f32>((side)?1:-1)) + vd * (static_cast<f32>(i+1) / static_cast<f32>(div_d)) + vh * (static_cast<f32>(j) / static_cast<f32>(div_h)) + pos,
          };
          // 輪郭にあたる部分に透き間を追加
          if( j == 0 )
          {
            vert[1] += vChink_h;
            vert[2] += vChink_h;
          }
          else if( j == div_h - 1 )
          {
            vert[0] -= vChink_h;
          }
          RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
        }
      }
    }
  }

    // 両端丸め
  for (s32 k = 0; k < DIV_CIRCLE; ++k)
  {
    // 中心と外周の2頂点
    math::Vector3 vOrg = pos + vd * ( k / ( DIV_CIRCLE / 2 ) );

    math::Vector3 vR1 = mat.TransformCoord( math::Vector3( math::FCos(2.0f * PI * static_cast<f32>(k) / DIV_CIRCLE) * width, 0.0f, - math::FSin(2.0f * PI * static_cast<f32>(k) / DIV_CIRCLE) * width ) );
    math::Vector3 vR2 = mat.TransformCoord( math::Vector3( math::FCos(2.0f * PI * static_cast<f32>(k+1) / DIV_CIRCLE) * width, 0.0f, - math::FSin(2.0f * PI * static_cast<f32>(k+1) / DIV_CIRCLE) * width ) );

    for(s32 j=0; j<div_w; j++)  // ライン垂直方向
    {
      math::Vector4 col = (k%2 == j%2) ? color : color * 0.5f;
      // 天面1
      {
        math::Vector3 vert[3] =
        {
          vR1 * (static_cast<f32>(j+1) / static_cast<f32>(div_w)) + vh + vOrg,
          vR2 * (static_cast<f32>(j+1) / static_cast<f32>(div_w)) + vh + vOrg,
          vR1 * (static_cast<f32>(j) / static_cast<f32>(div_w)) + vh + vOrg,
        };
        RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
      }
      // 天面2
      if(j != 0)
      {
        math::Vector3 vert[3] =
        {
          vR2 * (static_cast<f32>(j+1) / static_cast<f32>(div_w)) + vh + vOrg,
          vR2 * (static_cast<f32>(j) / static_cast<f32>(div_w)) + vh + vOrg,
          vR1 * (static_cast<f32>(j) / static_cast<f32>(div_w)) + vh + vOrg,
        };
        RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
      }
      // 底面1
      {
        math::Vector3 vert[3] =
        {
          vR1 * (static_cast<f32>(j+1) / static_cast<f32>(div_w)) + vOrg,
          vR2 * (static_cast<f32>(j+1) / static_cast<f32>(div_w)) + vOrg,
          vR1 * (static_cast<f32>(j) / static_cast<f32>(div_w)) + vOrg,
        };
        RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
      }
      // 底面2
      if(j != 0)
      {
        math::Vector3 vert[3] =
        {
          vR2 * (static_cast<f32>(j+1) / static_cast<f32>(div_w)) + vOrg,
          vR2 * (static_cast<f32>(j) / static_cast<f32>(div_w)) + vOrg,
          vR1 * (static_cast<f32>(j) / static_cast<f32>(div_w)) + vOrg,
        };
        RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
      }
    }
    // 側面
    for(s32 j=0; j<div_h; j++)
    {
      math::Vector4 col = (k%2 == j%2) ? color : color * 0.5f;
      {
        math::Vector3 vert[3] =
        {
          vR1 + vh * (static_cast<f32>(j+1) / static_cast<f32>(div_h)) + vOrg,
          vR2 + vh * (static_cast<f32>(j+1) / static_cast<f32>(div_h)) + vOrg,
          vR1 + vh * (static_cast<f32>(j) / static_cast<f32>(div_h)) + vOrg,
        };
        // 輪郭にあたる部分に透き間を追加
        if( j == 0 )
        {
          vert[2] += vChink_h;
        }
        else if( j == div_h - 1 )
        {
          vert[0] -= vChink_h;
          vert[1] -= vChink_h;
        }
        RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
      }
      {
        math::Vector3 vert[3] =
        {
          vR2 + vh * (static_cast<f32>(j+1) / static_cast<f32>(div_h)) + vOrg,
          vR2 + vh * (static_cast<f32>(j) / static_cast<f32>(div_h)) + vOrg,
          vR1 + vh * (static_cast<f32>(j) / static_cast<f32>(div_h)) + vOrg,
        };
        // 輪郭にあたる部分に透き間を追加
        if( j == 0 )
        {
          vert[1] += vChink_h;
          vert[2] += vChink_h;
        }
        else if( j == div_h - 1 )
        {
          vert[0] -= vChink_h;
        }
        RegistTriangleWithCullingCheck( vert, col, vertexList, vertexCount );
      }
    }
  }
  gfx::DrawUtil::DrawTriangles( m_pVertexFormatList, vertexCount, false);

  return vertexCount;
}



void DebugCollisionRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  // 描画数チェックをリセット
  m_TotalDrawPrimitiveCount = 0;       
  m_IsDrawOver = false;


  // レンダーターゲットの設定
  SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );


  // 描画開始
  gfx::GFGL::BeginScene();
//  gfx::DrawUtil::ClearScissor();

	if ( gfx::GFGL::GetRenderTarget() )
	{

		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
		{
			gfx::GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			gfx::GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		}
		else
		{
			const gfx::Surface::SurfaceDesc			&rDesc = gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
			gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}

		//GFGL::ClearRenderTarget( rDrawContext.m_pRenderTarget, Color( 0.4f, 0.4f, 0.4f) );
		//GFGL::ClearDepthStencil( rDrawContext.m_pDepthStencil, 1.0f, 0xFF );
	}

  // カリングチェック用マトリクス
  m_ViewProject = rDrawContext.m_Projection;
  m_ViewProject *= rDrawContext.m_View;
  gfl2::math::AABB::Util::CalcFrustumPlanes( m_CullingMatrixTable, rDrawContext.m_Projection * rDrawContext.m_View );

  // 深度テストありの描画物を描画
  gfx::DrawUtil::BeginRenderExtension();
  Execute( rDrawContext, true );
  if( m_pTrafficInfo )
  {
    m_pTrafficInfo->Draw();
  }
  gfx::DrawUtil::EndRenderExtension();

  // 深度テストなしの描画物を描画
  gfx::DrawUtil::BeginRenderExtension();
  Execute( rDrawContext, false );
  gfx::DrawUtil::EndRenderExtension();
  // 簡易描画のデプステストを無しに設定する
  gfx::DrawUtil::SetDepthTestEnable(false);

#if defined(GF_PLATFORM_WIN32)
  gfl2::gfx::GFGL::SetDepthStencilState( m_DepthStencilStateObject );
#endif


  // レンダーアクターリストクリア
  m_pCollisionRenderManager->ClearRenderCollisionActorInfo();
  
  // レンダーモデルリストクリア
  m_pCollisionRenderManager->ClearRenderCollisionModelInfo();
  
  // 環境音コリジョンデータリストクリア
  m_pCollisionRenderManager->ClearRenderEnvSoundCollisionInfo();
  
  // レンダープリミティブリストクリア
  m_pCollisionRenderManager->ClearRenderCollisionPrimitiveInfo();

#if FIELD_DEBUG_COL_RENDER_DUMP
  GFL_PRINT("debug vertex num : %d\n", nVertex );
  GFL_PRINT("debug object num : %d\n", drawObjectNum );
#endif

  gfx::GFGL::EndScene();

  // 描画オーバーを通知
  GameSys::GameManager* pgm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::Fieldmap* pfm = pgm->GetFieldmap();
  pfm->GetFieldDebugInfoWin()->SetDebugDrawOver( m_IsDrawOver );

//  ClearRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );
}


void DebugCollisionRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext, b32 depthTestEnable )
{
  // シザー、カリング、フィルの設定
  gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  if( depthTestEnable && FIELD_DEBUG_SWITCH(  Field::Debug::DebugTypes::Switch::COLLISION_OPACITY ) )
  {
    // 深度テストON
    gfx::DrawUtil::SetDepthTestEnable( true );
    gfx::DrawUtil::SetMaterialAlpha( 0.6f );
  }
  else
  {
    // 深度テストOFF
    gfx::DrawUtil::SetDepthTestEnable(false);
    gfx::DrawUtil::SetMaterialAlpha( 0.6f );
  }

  // プロジェクション行列設定
  gfx::DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
  // ビュー行列設定
  gfx::DrawUtil::SetViewMatrix(rDrawContext.m_View);
  // モデル行列設定
  gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());
  
  u32 nVertex = 0;

  // コリジョン表示マネージャーから登録されているアクターを取得して表示
  for(u32 i=0; i<m_pCollisionRenderManager->GetRenderCollisionActorInfoNum(); i++)
  {
    Field::Debug::DebugCollisionRenderManager::RenderCollisionActorInfo* pRenderActor = m_pCollisionRenderManager->GetRenderCollisionActorInfo( i );
    BaseActor* pActor = pRenderActor->pActor;
    
    // @note 一律で深度テスト有り扱い
    if( depthTestEnable == false )
    {
      continue;
    }
    
    if(pActor != NULL && pActor->IsActive())
    {
      BaseActor::SHAPE shapeType = pActor->GetShapeType();

      // 形状ごとに描画処理
      switch ( shapeType )
      {
      case BaseActor::SHAPE_CYLINDER:
        {
          CylinderShape* pShape = static_cast<CylinderShape*>(pActor->GetShape());
          if( pShape )
          {
            nVertex += DrawCylinderShape( pActor->GetPosition(), pActor->GetQuaternion(), pShape, pRenderActor->color );
          }
        }
        break;
      case BaseActor::SHAPE_SPHERE:
        {
          SphereShape* pShape = static_cast<SphereShape*>(pActor->GetShape());
          if( pShape )
          {
            nVertex += DrawSphereShape( pActor->GetPosition(), pActor->GetQuaternion(), pShape, pRenderActor->color );
          }
        }
        break;
      case BaseActor::SHAPE_BOX:
        {
          BoxShape* pShape = static_cast<BoxShape*>(pActor->GetShape());
          if( pShape )
          {
            nVertex += DrawBoxShape( pActor->GetPosition(), pActor->GetQuaternion(), pShape, pRenderActor->color );
          }
        }
        break;
      case BaseActor::SHAPE_OBB:
        {
          ObbShape* pShape = static_cast<ObbShape*>(pActor->GetShape());
          if( pShape )
          {
            nVertex += DrawObbShape( pActor->GetPosition(), pActor->GetQuaternion(), pShape, pRenderActor->color );
          }
        }
        break;
      case BaseActor::SHAPE_TRIANGLE:
        {
          TriangleShape* pShape = static_cast<TriangleShape*>(pActor->GetShape());
          if( pShape )
          {
            nVertex += DrawTriangleShape( pActor->GetPosition(), pActor->GetQuaternion(), pShape, pRenderActor->color );
          }
        }
        break;
      case BaseActor::SHAPE_MESH:
        {
          MeshShape* pShape = static_cast<MeshShape*>(pActor->GetShape());
          if( pShape )
          {
            nVertex += DrawCollisionModel( pActor->GetPosition() + pRenderActor->pos, math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f), pShape->m_pCollisionModel, pRenderActor->color, pRenderActor->pColorTable, pRenderActor->colorTableSize );
          }
        }
        break;
      case BaseActor::SHAPE_LINE:
        {
          LineShape* pShape = static_cast<LineShape*>(pActor->GetShape());
          if( pShape )
          {
            nVertex += DrawLineShape( pActor->GetPosition(), pActor->GetQuaternion(), pShape, pRenderActor->color );
          }
        }
        break;

      default:
        break;
      }
    }
  }
 
  // コリジョン表示マネージャーから登録されているコリジョンモデルを取得して表示
  for(u32 i=0; i<m_pCollisionRenderManager->GetRenderCollisionModelInfoNum(); i++)
  {
    // @note 一律で深度テスト有り扱い
    if( depthTestEnable == false )
    {
      continue;
    }
    
    Field::Debug::DebugCollisionRenderManager::RenderCollisionModelInfo* pRenderModel = m_pCollisionRenderManager->GetRenderCollisionModelInfo( i );
    if( pRenderModel->isHightClamp )
    {
      nVertex += DrawCollisionModel( math::Vector3( 0.0f, 0.0f, 0.0f ), math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f), pRenderModel->pModel, pRenderModel->color, m_pCollisionRenderManager->GetPlayerPosY(), pRenderModel->hitAttr );
    }
    else
    {
      nVertex += DrawCollisionModel(  math::Vector3( 0.0f, 0.0f, 0.0f ), math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f), pRenderModel->pModel, pRenderModel->color, pRenderModel->pAttrColorTable, pRenderModel->attrColorTableSize );
    }
  }
 
  // コリジョン表示マネージャーから登録されているコリジョンプリミティブを取得して表示
  for(u32 i=0; i<m_pCollisionRenderManager->GetRenderCollisionPrimitiveInfoNum(); i++)
  {
    Field::Debug::DebugCollisionRenderManager::RenderCollisionPrimitiveInfo* pRenderPrim = m_pCollisionRenderManager->GetRenderCollisionPrimitiveInfo( i );
    
    if( pRenderPrim->isDepthTestEnable != depthTestEnable )
    {
      continue;
    }

    switch(pRenderPrim->type)
    {
    case Field::Debug::DebugCollisionRenderManager::COLLISION_PRIMITIVE_CYLINDER :
      {
        CylinderShape* pShape = static_cast<CylinderShape*>(pRenderPrim->pShape);
        if( pShape )
        {
          nVertex += DrawCylinderShape( pRenderPrim->position, gfl2::math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f), pShape, pRenderPrim->color );
        }
      }
      break;
    }
  }


  // コリジョン表示マネージャーから登録されている環境音コリジョンデータを取得して表示
  GameSys::GameManager* pgm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::Fieldmap* pfm = pgm->GetFieldmap();
  gfl2::math::Vector3 cam_target, cam_pos;
  pfm->GetCameraManager()->GetMainViewCamera()->GetCameraAndTargetPosition(&cam_target, &cam_pos); 
  f32 view_y = cam_pos.y - 60.0f;//Y方向にコリジョンが大きすぎるためカメラを越えないように制御
  for(u32 i=0; i<m_pCollisionRenderManager->GetRenderEnvSoundCollisionInfoNum(); i++)
  {
    // @note 一律で深度テスト有り扱い
    if( depthTestEnable == false )
    {
      continue;
    }
    
    Field::Debug::DebugCollisionRenderManager::RenderEnvSoundCollisionInfo* pEnvSound = m_pCollisionRenderManager->GetRenderEnvSoundCollisionInfo( i );
    if( pEnvSound->cpData != NULL )
    {
      Field::FieldEnvSoundCollisionAccessor accessor;
      accessor.Initialize( pEnvSound->cpData );

      for( u32 j=0; j<accessor.GetDataMax(); j++ )
      {
        Field::FieldEnvSoundCollisionAccessor::CollisionData data;
        accessor.GetData(j, &data);
        switch( data.type )
        {
        case Field::FieldEnvSoundCollisionAccessor::CYLINDER : 
          {
            math::Vector3 position(data.cylinder.center[0], data.cylinder.center[1], data.cylinder.center[2]);

            CylinderShape Cylinder;
            Cylinder.m_height = gfl2::math::Clamp( data.cylinder.height + position.y, position.y, view_y ) - position.y;
            Cylinder.m_r = data.cylinder.radius;
            Cylinder.m_pos = math::Vector3(0, 0, 0);
            nVertex += DrawCylinderShape( position, gfl2::math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &Cylinder, pEnvSound->color );
          }
          break;

        case Field::FieldEnvSoundCollisionAccessor::SPHERE : 
          {
            math::Vector3 position(data.sphere.center[0], data.sphere.center[1], data.sphere.center[2]);
            CylinderShape Cylinder;
            Cylinder.m_height = gfl2::math::Clamp( data.sphere.radius + position.y, position.y, view_y ) - position.y;
            Cylinder.m_r = data.sphere.radius;
            Cylinder.m_pos = math::Vector3(0, 0, 0);
            nVertex += DrawCylinderShape( position, gfl2::math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &Cylinder, pEnvSound->color );

          }
          break;

        case Field::FieldEnvSoundCollisionAccessor::LINE : 
          {
            f32 max_y = gfl2::math::Max( data.line.startpos[1], data.line.endpos[1] );

            nVertex += DrawLineArea( math::Vector3( data.line.startpos[0], data.line.startpos[1], data.line.startpos[2] ),
                          math::Vector3( data.line.endpos[0], data.line.endpos[1], data.line.endpos[2] ),
                          data.line.width, gfl2::math::Clamp( data.line.height + max_y, max_y, view_y ) - max_y, pEnvSound->color );
          }
          break;

        case Field::FieldEnvSoundCollisionAccessor::PIPE_LINE : 
          {
            f32 max_y = gfl2::math::Max( data.pipe_line.startpos[1], data.pipe_line.endpos[1] );

            nVertex += DrawLineArea( math::Vector3( data.pipe_line.startpos[0], data.pipe_line.startpos[1], data.pipe_line.startpos[2] ),
                          math::Vector3( data.pipe_line.endpos[0], data.pipe_line.endpos[1], data.pipe_line.endpos[2] ),
                          data.pipe_line.width, gfl2::math::Clamp( data.pipe_line.width + max_y, max_y, view_y ) - max_y, pEnvSound->color );
          }
          break;

        case Field::FieldEnvSoundCollisionAccessor::SNAKE : 
          {
          }
          break;

        default :
          break;
        }
      }
    }
  }
}

GFL_NAMESPACE_END( gfl2 );

#endif // PM_DEBUG
