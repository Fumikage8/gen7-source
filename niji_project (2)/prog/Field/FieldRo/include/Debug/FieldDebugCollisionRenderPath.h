//======================================================================
/**
 * @file FieldDebugCollisionRenderPath.h
 * @date 2015/07/13 15:57:58
 * @author ikawa_hiroki
 * @brief コリジョン表示用レンダリングパス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __FIELDDEBUGCOLLISIONRENDERPATH_H_INCLUDED__
#define __FIELDDEBUGCOLLISIONRENDERPATH_H_INCLUDED__
#pragma once

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>
#include "Field/FieldRo/include/Collision/BaseShape.h"
#include "System/include/nijiAllocator.h"
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderManager.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Traffic)
class DebugInfo;
GFL_NAMESPACE_END(Traffic)
GFL_NAMESPACE_END(Field)

GFL_NAMESPACE_BEGIN( gfl2 )

class DebugCollisionRenderManager;

/**
  * @class DebugCollisionRenderPath
  * @brief デバッグコリジョン表示用レンダリングパス
  */
class DebugCollisionRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
  GFL_FORBID_COPY_AND_ASSIGN( DebugCollisionRenderPath );

public:
  /**
    * @struct InitDescription
    * @brief  初期化関連構造体
    */
  struct InitDescription
  {
    u32 maxTriangleCount;               //! 描画可能な最大トライアングル数
    f32 borderProjectionCulling;        //! 描画エリアでのカリングの閾値
    f32 borderDepthCulling;        //! 描画エリアでのカリングの閾値
    u32 borderSurfaceDivide;            //! ポリゴン分割するサイズの閾値
    InitDescription() :
      maxTriangleCount(1000),
      borderProjectionCulling( 1.2f ),
      borderDepthCulling( 1.0f ),
      borderSurfaceDivide( 1000 )
    {
    }
  };

  /**
    * @brief コンストラクタ
    * @return なし
    * @detail
    */
  DebugCollisionRenderPath();

  /**
    * @brief デストラクタ
    * @return なし
    * @detail
    */
  virtual ~DebugCollisionRenderPath();

  /**
    * @brief 初期化関数
    * @param[in] InitDescription 初期化用構造体
    * @return なし
    * @detail
    */
  void Initialize( const InitDescription& desc, System::nijiAllocator* pAllocator );

  /**
    * @brief 終了関数
    * @return なし
    * @detail
    */
  void Terminate( void );

  virtual const renderingengine::renderer::IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfx::GLFixedSizeContainer<renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  void SetCurrentTerrain(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pCurrentTerrain)
  {
    m_pCurrentTerrain = pCurrentTerrain;
  }


protected:
  
  void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext, b32 depthTestEnable );

  InitDescription m_InitDescription;                                              // 初期化設定
  gfl2::gfx::DrawUtil::VertexFormat* m_pVertexFormatList;                         // 頂点フォーマットリスト
  u32 m_VertexFormatLimit;                                                        // 頂点フォーマットリスト確保数

  const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* m_pCurrentTerrain; // 現在乗っている地形

  Field::Debug::DebugCollisionRenderManager*  m_pCollisionRenderManager;  // 地形コリジョンモデル表示関連マネージャー

  math::Matrix44 m_ViewProject;                 // 画面座標変換用マトリクス
  gfl2::math::Matrix44 m_CullingMatrixTable[4]; // カリング用行列
  
  f32   m_BorderCulling;                // カリングする閾値
  f32   m_BorderDepthCulling;           // カリングする閾値(デプス方向)
  u32   m_BordarDivide;


  u32   m_DrawPrimitiveMax;             // 描画数最大

  // 描画関数で更新される変数
  u32   m_TotalDrawPrimitiveCount;      // 描画数カウンタ          
  b32   m_IsDrawOver;                   // 描画枠後で表示スキップしたらtrue

  Field::Traffic::DebugInfo *m_pTrafficInfo;
#if defined(GF_PLATFORM_WIN32)
  gfl2::gfx::DepthStencilStateObject   *m_DepthStencilStateObject;     // 深度/ステンシル・ステート・オブジェクト
#endif
private:

  /**
  * @brief カリング判定式
  * @param[in] pVertex 頂点情報
  * @param[in] size    チェック頂点数
  * @detail 
  */
  bool CheckCulling( math::Vector3* pVertex, u32 size );

  /**
  * @brief 三角形の間引きチェック込みで頂点リストに頂点を登録
  * @param[in] position  位置
  * @param[in] color  色指定
  * @param[out] vertexList  頂点リスト
  * @param[out] vertexCount 頂点数カウンター
  * @return 頂点数
  * @detail 
  */
  u32 RegistTriangleWithCullingCheck( math::Vector3 position[3], math::Vector4 color, gfx::DrawUtil::VertexFormat* vertexList, u32 &vertexCount );

  /**
  * @brief シリンダーの描画
  * @param[in] position  位置
  * @param[in] quaternion クォータニオン
  * @param[in] pShape CylinderShapeのポインタ
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawCylinderShape( math::Vector3 position, math::Quaternion quaternion, CylinderShape* pShape, math::Vector4 color );

  /**
  * @brief 球の描画
  * @param[in] position  位置
  * @param[in] quaternion クォータニオン
  * @param[in] pShape CylinderShapeのポインタ
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawSphereShape( math::Vector3 position, math::Quaternion quaternion, SphereShape* pShape, math::Vector4 color );

  /**
  * @brief ボックスの描画
  * @param[in] position  位置
  * @param[in] quaternion クォータニオン
  * @param[in] pShape BoxShapeのポインタ
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawBoxShape( math::Vector3 position, math::Quaternion quaternion, BoxShape* pShape, math::Vector4 color );

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
  u32 DrawObbShape( math::Vector3 position, math::Quaternion quaternion, ObbShape* pShape, math::Vector4 color );

  /**
  * @brief Triangleの描画
  * @param[in] position  位置
  * @param[in] quaternion クォータニオン
  * @param[in] pShape BoxShapeのポインタ
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawTriangleShape( math::Vector3 position, math::Quaternion quaternion, TriangleShape* pShape, math::Vector4 color );

  /**
  * @brief コリジョンモデルの描画
  * @param[in] position  位置
  * @param[in] quaternion クォータニオン
  * @param[in] pModel CollisionModelのポインタ
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawCollisionModel( math::Vector3 position, math::Quaternion quaternion, collision::CollisionModel* pModel, math::Vector4 color, const gfl2::math::Vector4* pAttrColorTable = NULL, u32 attrColorTableSize = 0 );
  u32 DrawCollisionModel( math::Vector3 position, math::Quaternion quaternion, collision::CollisionModel* pModel, math::Vector4 color, f32 clampY, s32 hitAttr = -1 );

  /**
  * @brief コリジョンモデルの描画
  * @param[in] pModel CollisionModelのポインタ
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawCollisionModel( collision::CollisionModel* pModel, math::Vector4 color );

  /**
  * @brief メッシュの描画
  * @param[in] position  位置
  * @param[in] quaternion クォータニオン
  * @param[in] pShape MeshShapeのポインタ
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawMeshShape( math::Vector3 position, math::Quaternion quaternion, MeshShape* pShape, math::Vector4 color );

  /**
  * @brief 直線の描画
  * @param[in] position  位置
  * @param[in] quaternion クォータニオン
  * @param[in] pShape LineShapeのポインタ
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawLineShape( math::Vector3 position, math::Quaternion quaternion, LineShape* pShape, math::Vector4 color );

  /**
  * @brief コリジョンプリミティブ(円)の描画
  * @param[in] position  位置
  * @param[in] radius   半径
  * @param[in] color  色指定
  * @return 頂点数
  * @detail 
  */
  u32 DrawCircle( math::Vector3 position, f32 radius, math::Vector4 color );

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
  u32 DrawLineArea( math::Vector3 startpos, math::Vector3 endpos, f32 width, f32 height, math::Vector4 color );

};

GFL_NAMESPACE_END( gfl2 )

#endif // __FIELDDEBUGCOLLISIONRENDERPATH_H_INCLUDED__

#endif // PM_DEBUG
