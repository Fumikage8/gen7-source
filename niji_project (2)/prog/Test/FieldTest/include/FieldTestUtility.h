//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file    FieldTestUtility.h
 *  @brief  フィールドモック環境　一旦汎用的なクラス、や関数置き場
 *  @author  
 *  @date    2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_TEST_UTILITY_H__ )
#define __FIELD_TEST_UTILITY_H__
#pragma once

#include <stdio.h>
#include <stdarg.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include <heap/include/gfl2_Heap.h>

#include <gfx/include/gfl2_GLMemory.h>

// for NodeCountVisitor
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/*
*          @class Utility
*          @brief 汎用関数郡
*/
//-----------------------------------------------------------------------------
class Utility
{
public:
  //----------------------------------------------------------------------------
  /**
  *  @brief  ファイル読み込み
  *  @param  pFilePath   ファイルパス
  *  @param  alignment   アライメント
  */
  //-----------------------------------------------------------------------------
  static void* ReadData( const c16* pFilePath, u32 alignment = 128 );

  //----------------------------------------------------------------------------
  /**
  *  @brief  ファイル読み込み
  *  @param  pHeap       ヒープ
  *  @param  pFilePath   ファイルパス
  *  @param  alignment   アライメント
  */
  //-----------------------------------------------------------------------------
  static void* ReadData( gfl2::heap::HeapBase *pHeap, const c16* pFilePath, u32 alignmen = 128 );


  static void Sprintf_S( char * buffer, size_t sizeOfBuffer, const char * format, ... )
  {
#if defined(GF_PLATFORM_CTR)
    va_list vlist;
    va_start(vlist, format);
    vsprintf( buffer, format, vlist );
    va_end(vlist);

#elif defined(GF_PLATFORM_WIN32)
    va_list vlist;
    va_start(vlist, format);
    vsprintf_s( buffer, sizeOfBuffer, format, vlist);
    va_end(vlist);
#endif
  }

  static size_t Strnlen( const char * str, size_t strnum )
  {
#if defined(GF_PLATFORM_CTR)
    return strlen( str );
#elif defined(GF_PLATFORM_WIN32)
    return strnlen( str, strnum );
#endif
  }

  static void Strcpy_s( char * dest, const char * src, size_t num )
  {
#if defined(GF_PLATFORM_CTR)
    strcpy( dest, src );
#elif defined(GF_PLATFORM_WIN32)
    strcpy_s( dest, num, src );
#endif
  }

};


// NodeCountVisitorでカウントする対象となるノード名
// 対象ノードを追加したい場合はここにノード名を追加するだけでOKです
// @note 各ノードのハッシュ値に使用している文字列に合わせる必要があります、ハッシュに使用している文字列はgfl2::util::FnvHashのコンストラクタにprint文を仕込むと簡単に取得する事ができます
// @todo 隠蔽したい
static const c8* const sc_NodeCountVisitTargetNameTable[] = 
{
  "Node::DagNode",
  "Node::DagNode::InstanceNode",
  "Node::DagNode::InstanceNode::TransformNode",
  "Node::DagNode::InstanceNode::TransformNode::CullingObjectNode",
  "Node::DagNode::InstanceNode::TransformNode::CullingObjectNode::LightNode",
  "Node::DagNode::InstanceNode::TransformNode::CullingObjectNode::DrawableNode",
  "Node::DagNode::InstanceNode::TransformNode::CullingObjectNode::DrawEnvNode",
  "Node::DagNode::InstanceNode::TransformNode::CullingObjectNode::DrawableNode::ModelInstanceNode",
  "Node::DagNode::InstanceNode::TransformNode::JointInstanceNode",
  "Node::DagNode::InstanceNode::TransformNode::LightSetNode",
  "Node::DagNode::InstanceNode::TransformNode::CameraNode",
  "Node::DagNode::InstanceNode::TransformNode::LocatorInstanceNode",
  "Node::DagNode::InstanceNode::TransformNode::ViewerLocatorInstanceNode",
  "Node::DagNode::InstanceNode::MaterialInstanceNode",
  "Node::DagNode::InstanceNode::MotionConfigInstanceNode",
  "Node::DagNode::InstanceNode::MotionInstanceNode",

  "Node::DagNode::ResourceNode",
  "Node::DagNode::ResourceNode::GfMdlResourceNode",
  "Node::DagNode::ResourceNode::DrawEnvResourceNode",
  "Node::DagNode::ResourceNode::MaterialResourceNode",
  "Node::DagNode::ResourceNode::TextureResourceNode",
  "Node::DagNode::ResourceNode::ShaderResourceNode",
  "Node::DagNode::ResourceNode::MeshResourceNode",
  "Node::DagNode::ResourceNode::LocatorResourceNode",
  "Node::DagNode::ResourceNode::MotionConfigResourceNode",
  "Node::DagNode::ResourceNode::LightSetResourceNode",
  "Node::DagNode::ResourceNode::CameraResourceNode",
  "Node::DagNode::ResourceNode::AnimationResourceNode",
  "Node::DagNode::ResourceNode::BlendShapeAnimationResourceNode",
  "Node::DagNode::ResourceNode::CollisionMeshResourceNode",
  "Node::DagNode::ResourceNode::GfMotResourceNode",

  "Node::DagNode::FieldResourceNode",
};


// Node数をカウントするvisitor
class NodeCountVisitor
{
public:

  struct NODE_COUNT_DATA
  {
    const c8* pNodeName;
    gfl2::util::FnvHash hash;
    u32 count;                  // 実ノード数
    u32 countOfInheritance;     // 継承数
  };

  NodeCountVisitor();
  virtual ~NodeCountVisitor();

  void Visit( gfl2::renderingengine::scenegraph::DagNode* pNode );


  b32 GetNodeCountData( u32 index, NODE_COUNT_DATA* pOut ) const;
  u32 GetNodeCountDataNum( void ) const { return GFL_NELEMS( m_NodeCountTable ); }
  u32 GetNodeCount( void ) const { return m_NodeCount; }
  u32 GetUnknownNodeCount( void ) const { return m_UnknownNodeCount; }
  u32 GetUnknownNodeCountOfInheritance( void ) const { return m_UnknownNodeCountOfInheritance; }


private:
  u32                m_NodeCount;
  u32                m_UnknownNodeCount;                  // 不明型の総数(実ノード数)
  u32                m_UnknownNodeCountOfInheritance;     // 不明型の総数(継承数)
  NODE_COUNT_DATA    m_NodeCountTable[ GFL_NELEMS( sc_NodeCountVisitTargetNameTable ) ];  // 各Node別総数
};


// 訪問数をカウントするvisitor
class VisitCountVisitor
{
public:
  
  VisitCountVisitor() : m_VisitCount( 0 ) {}
  virtual ~VisitCountVisitor() {}

  void Visit( gfl2::renderingengine::scenegraph::DagNode* pNode )
  {
    m_VisitCount++;
  }

  u32 GetVisitCount( void ) const { return m_VisitCount; }

private:
  u32                m_VisitCount;
};



GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )


#endif  // __FIELD_TEST_UTILITY_H__

