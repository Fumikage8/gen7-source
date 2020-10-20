//===========================================================================
/**
 * @file    gfl2_SceneGraphManager.cpp
 * @brief   gfl2 SceneGraph制御クラス
 * @author  ishiguro_masateru@gamefreak.co.jp
 * @date    2014
 */
//===========================================================================
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>
#include <gfx/include/gfl2_GFGL.h>

#if GFL_DEBUG
#include <algorithm>
#include <debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_EffectNodeLocatorInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ViewerLocatorInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ArrayResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_BlendShapeAnimationResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CameraResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LightSetResourceNode.h>
#endif

namespace gfl2 { namespace renderingengine { namespace scenegraph {

SceneGraphManager					*SceneGraphManager::s_Gp = NULL;

SceneGraphManager::SceneGraphManager( const InitDescription &rDesc ) : gfx::GLMemory(), gfx::IGLGLCallBack(),
  m_NodeMax(0),
  m_NodeCount(0),
  m_RootNode(NULL),
  m_ResourceRootNode(NULL),
  m_InstanceRootNode(NULL),
  m_TransformRootNode(NULL),
  m_DrawingNodeListDBCnt(0),
  m_DrawingNodeListMax(0)
{
	for( u32 i = 0; i < 2; ++i )
  {
    m_DrawingNodeListDB[i].m_NodeNum = 0;
    m_DrawingNodeListDB[i].m_pDrawableNodeList = NULL;
  }
}

SceneGraphManager::~SceneGraphManager()
{
  // SceneGraphが持っているRootノードを生成した順序と逆の順序で削除
  GFL_SAFE_DELETE(m_TransformRootNode);
  GFL_SAFE_DELETE(m_InstanceRootNode);
  GFL_SAFE_DELETE(m_ResourceRootNode);
  GFL_SAFE_DELETE(m_RootNode);

	for( u32 i = 0; i < 2; ++i )
	{
		this->GetInstanceAllocator()->SystemMemoryFree( m_DrawingNodeListDB[i].m_pDrawableNodeList );
	}

#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE
  this->GetInstanceAllocator()->SystemMemoryFree(m_pDebugNodeFlagList);
#endif
}

void SceneGraphManager::SetUp( const InitDescription &rDesc )
{
  m_NodeMax = rDesc.m_NodeStackSize;
  m_NodeCount = 0;

#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE
  m_DebugNodeContainer.CreateBuffer(m_NodeMax, this->GetInstanceAllocator() );
  m_pDebugNodeFlagList = (u8*)this->GetInstanceAllocator()->SystemMemoryMalloc(m_NodeMax);
#endif

  gfx::GLMemory::SetInstanceAllocator( this->GetInstanceAllocator() );
  m_RootNode = new renderingengine::scenegraph::DagNode();
  m_ResourceRootNode = new scenegraph::resource::ResourceNode();
  m_InstanceRootNode = new scenegraph::instance::InstanceNode();
  m_TransformRootNode = new scenegraph::instance::TransformNode();
  gfx::GLMemory::ClearInstanceAllocator();

  m_RootNode->AddChild( m_ResourceRootNode );
  m_RootNode->AddChild( m_InstanceRootNode );
  m_RootNode->AddChild( m_TransformRootNode );

  m_AABBTreeTop.SetParent( NULL );
  m_AABBTreeTop.SetProperties( rDesc.m_AABBDepth, 0 );
  m_AABBTreeTop.SetAreaAABB( rDesc.m_AABBTreeSize );
  m_AABBTreeTop.SetDrawableVolumeAABB( rDesc.m_AABBTreeSize );
  m_AABBTreeTop.CreateChildBox( this->GetInstanceAllocator(), rDesc.m_AABBDepth );

  m_DrawingNodeListDBCnt = 0;
  m_DrawingNodeListMax = rDesc.m_NodeStackSize / 2;

  for( u32 i = 0; i < 2; ++i )
  {
    m_DrawingNodeListDB[i].m_NodeNum = 0;
    m_DrawingNodeListDB[i].m_pDrawableNodeList = reinterpret_cast<scenegraph::instance::ModelInstanceNode**>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(scenegraph::instance::ModelInstanceNode*) * m_DrawingNodeListMax ) );

    for( u32 j = 0; j < m_DrawingNodeListMax; ++j )
    {
      m_DrawingNodeListDB[i].m_pDrawableNodeList[j] = NULL;
    }
  }
}

void SceneGraphManager::Initialize( gfx::IGLAllocator* pAllocator, InitDescription desc )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator );
	if ( s_Gp == NULL )
	{
		s_Gp = new SceneGraphManager( desc );
    s_Gp->SetUp( desc );
		gfx::GFGL::SetGLGLCallBack( s_Gp );
	}
  gfx::GLMemory::ClearInstanceAllocator();
}

void SceneGraphManager::Terminate()
{
	if ( s_Gp )
	{
		delete s_Gp;
		s_Gp = NULL;
	}
}

//TransformNode用ビジターを作成
class TransformNodeVisitor
{
public:
  TransformNodeVisitor() : 
    m_UpdateCount( 0 ),
    m_UpdateSkipCount( 0 )
  {
  }

	virtual ~TransformNodeVisitor() {}

	void Visit( scenegraph::instance::TransformNode* pNode )
	{
		scenegraph::instance::TransformNode* pParent = pNode->GetParent() ? pNode->GetParent()->SafeCast<scenegraph::instance::TransformNode>() : NULL;
    const math::SRT& localSRT = pNode->GetLocalSRT();

    gfl2::math::Matrix34 local;

    // 親が居ない場合
    if (pParent == NULL)
    {
      if( pNode->IsLocalTransformDirty() )
      {
        localSRT.ToMatrix(local);
        pNode->SetWorldMatrix(local);
        pNode->SetLocalTransformDirty( false );
        pNode->SetWorldTransformDirty( true );
        m_UpdateCount++;
      }
      else
      {
        pNode->SetLocalTransformDirty( false );
        pNode->SetWorldTransformDirty( false );
        m_UpdateSkipCount++;
      }
      return;
    }

    // 行列更新
    if( pParent->IsWorldTransformDirty() || pNode->IsLocalTransformDirty() )
    {
      const gfl2::math::Vector3& parentScale = pParent->GetLocalScale();
      if (pNode->IsNonInheritanceScale())
      {
        // 親のスケールを伝播させない
        if ((parentScale.x == 1.0f) && (parentScale.y == 1.0f) && (parentScale.z == 1.0f))
        {
          // 親のスケールが全て１なので通常ので大丈夫
          localSRT.ToMatrix(local);
        }
        else
        {
          localSRT.ToMatrixNonInheritanceScale(local, parentScale);
        }
      }
      else
      {
        // 親のスケールを伝播させる
        localSRT.ToMatrix(local);
      }
    
      pNode->SetLocalTransformDirty( false );
      pNode->SetWorldTransformDirty( true );

      // ワールド行列更新
      math::Matrix34* pNodeWorldPtr = const_cast<math::Matrix34*>(pNode->GetWorldMatrixPtr());
      math::Matrix34::MultFast(pNodeWorldPtr, pParent->GetWorldMatrix(), local);

      // スキニング行列更新
      if (pNode->IsJointNode())
      {
        scenegraph::instance::JointInstanceNode* pJointNode = static_cast<scenegraph::instance::JointInstanceNode*>(pNode);
        
        math::Matrix34* pSkinningMatrixPtr = const_cast<math::Matrix34*>(pJointNode->GetSkinningMatrixPtr());
        math::Matrix34::MultFast(pSkinningMatrixPtr, *pNodeWorldPtr, pJointNode->GetBindPoseMatrix());
      }

      m_UpdateCount++;
    }
    else
    {
      pNode->SetLocalTransformDirty( false );
      pNode->SetWorldTransformDirty( false );
      m_UpdateSkipCount++;
    }
  }

  u32 m_UpdateCount;
  u32 m_UpdateSkipCount;
};
class TransformNodeVisitorFast
{
public:
  TransformNodeVisitorFast() : m_UpdateCount( 0 ), m_UpdateSkipCount( 0 ) {}
  virtual ~TransformNodeVisitorFast() {}

  void Visit( scenegraph::instance::TransformNode* pNode )
  {
    scenegraph::instance::TransformNode* pParent = static_cast<scenegraph::instance::TransformNode*>(pNode->GetParent());
    const math::SRT& localSRT = pNode->GetLocalSRT();

    gfl2::math::Matrix34 local;
    
    // 親が居ない場合
    if (pParent == NULL)
    {
      if( pNode->IsLocalTransformDirty() )
      {
        localSRT.ToMatrix(local);
        pNode->SetWorldMatrix(local);
        pNode->SetLocalTransformDirty( false );
        pNode->SetWorldTransformDirty( true );
        m_UpdateCount++;
      }
      else
      {
        pNode->SetLocalTransformDirty( false );
        pNode->SetWorldTransformDirty( false );
        m_UpdateSkipCount++;
      }
      return;
    }

    // 行列更新
    if( pParent->IsWorldTransformDirty() || pNode->IsLocalTransformDirty() )
    {
      const gfl2::math::Vector3& parentScale = pParent->GetLocalScale();
      if (pNode->IsNonInheritanceScale())
      {
        // 親のスケールを伝播させない
        if ((parentScale.x == 1.0f) && (parentScale.y == 1.0f) && (parentScale.z == 1.0f))
        {
          // 親のスケールが全て１なので通常ので大丈夫
          localSRT.ToMatrix(local);
        }
        else
        {
          localSRT.ToMatrixNonInheritanceScale(local, parentScale);
        }
      }
      else
      {
        // 親のスケールを伝播させる
        localSRT.ToMatrix(local);
      }

      pNode->SetLocalTransformDirty( false );
      pNode->SetWorldTransformDirty( true );

      // ワールド行列更新
      math::Matrix34* pNodeWorldPtr = const_cast<math::Matrix34*>(pNode->GetWorldMatrixPtr());
      math::Matrix34::MultFast(pNodeWorldPtr, pParent->GetWorldMatrix(), local);

      // スキニング行列更新
      if (pNode->IsJointNode())
      {
        scenegraph::instance::JointInstanceNode* pJointNode = static_cast<scenegraph::instance::JointInstanceNode*>(pNode);
        
        math::Matrix34* pSkinningMatrixPtr = const_cast<math::Matrix34*>(pJointNode->GetSkinningMatrixPtr());
        math::Matrix34::MultFast(pSkinningMatrixPtr, *pNodeWorldPtr, pJointNode->GetBindPoseMatrix());
      }

      m_UpdateCount++;
    }
    else
    {
      pNode->SetLocalTransformDirty( false );
      pNode->SetWorldTransformDirty( false );
      m_UpdateSkipCount++;
    }
  }

  u32 m_UpdateCount;
  u32 m_UpdateSkipCount;
};

class TransformNodeVisitorApplyBillboard
{
public:
  TransformNodeVisitorApplyBillboard() : m_UpdateCount( 0 ), m_UpdateSkipCount( 0 ) {}
  virtual ~TransformNodeVisitorApplyBillboard() {}

  void SetViewMatrix(math::Matrix44 *m) { viewMat = m; }
  
  math::Matrix44 *viewMat;

  void Visit( scenegraph::instance::TransformNode* pNode )
  {
		scenegraph::instance::TransformNode* pParent = static_cast<scenegraph::instance::TransformNode*>(pNode->GetParent());

    if( !pNode->IsBillboard() ) 
    {
      m_UpdateSkipCount++;
      return;
    }

		if( pNode->IsJointNode() == false )
		{
			return;
		}
 
    gfl2::math::Matrix44	bb;
		math::Matrix34				resultMtx;

		math::Matrix34*				pNodeWorldPtr = const_cast<math::Matrix34*>(pNode->GetWorldMatrixPtr());
		math::Vector3					worldPos;
		math::Vector3					aim;
		math::Vector3					cameraUp;

		worldPos.x = pNodeWorldPtr->GetElem(0, 3);
		worldPos.y = pNodeWorldPtr->GetElem(1, 3);
		worldPos.z = pNodeWorldPtr->GetElem(2, 3);

		bb = viewMat->Inverse();
		
		aim.x = bb[0][3];
		aim.y = bb[1][3];
		aim.z = bb[2][3];

		cameraUp.x = bb[0][1];
		cameraUp.y = bb[1][1];
		cameraUp.z = bb[2][1];
    
    if(pNode->IsBillboardY())
    {    
			math::Matrix34				worldMat = pNodeWorldPtr->Transpose();
			
			{//モデル(メッシュ)ローカルＹ軸ビルボード。親のトランスフォームは伝播させる。
				f32								scaleX = worldMat[0].Length3();
				f32								scaleZ = worldMat[2].Length3();
				math::Vector3			yVec( worldMat[1].x, worldMat[1].y, worldMat[1].z );
				math::Vector3			zVec = (aim - worldPos).Normalize();
				math::Vector3			xVec = yVec.Cross( zVec ).Normalize();

				zVec = xVec.Cross( yVec ).Normalize();
				zVec = zVec;

				xVec *= scaleX;
				zVec *= scaleZ;

				for( s32 i = 0; i < 3; ++i )
					worldMat[i].Set( xVec[i], yVec[i], zVec[i], worldPos[i] );

				*pNodeWorldPtr = worldMat;
			}
    }
    else if(pNode->IsBillboardXY())
    {
			
			gfl2::math::Matrix34	bb34;

			bb = math::Matrix44::GetLookAt( aim, worldPos, cameraUp );
			bb = bb.Transpose();
			std::memcpy( &bb34, &bb, sizeof(bb34) );       

			{//位置のみ伝版。親の回転等は伝播させない。その上で、向けた後にローカルアニメを適応。
				math::SRT localSRT = pNode->GetLocalSRT();

				gfl2::math::Matrix34 local;

				localSRT.translate.Set( 0, 0, 0 );
				localSRT.ToMatrix(local);

				if ( pParent )
				{
					gfl2::math::Matrix34				parentMat = pParent->GetWorldMatrix().Transpose();
					gfl2::math::Vector3					parentScale;

					parentScale.x = parentMat[0].Length3();
					parentScale.y = parentMat[1].Length3();
					parentScale.z = parentMat[2].Length3();

					gfl2::math::Matrix34				parentScaleMat = gfl2::math::Matrix34::GetScaling( parentScale );

					bb34 = bb34 * parentScaleMat;

					if ( pNode->IsNonInheritanceScale() )
					{
						// 親のスケールを伝播させない
						if ((parentScale.x == 1.0f) && (parentScale.y == 1.0f) && (parentScale.z == 1.0f))
						{
							// 親のスケールが全て１なので通常ので大丈夫
							localSRT.ToMatrix(local);
						}
						else
						{
							localSRT.ToMatrixNonInheritanceScale(local, parentScale);
						}
					}
					else
					{
						// 親のスケールを伝播させる
						localSRT.ToMatrix(local);
					}
				}
				else
				{
					localSRT.ToMatrix(local);
				}

				math::Matrix34::MultFast( pNodeWorldPtr, bb34, local );

				pNodeWorldPtr->SetElem(0,3,worldPos.x);
				pNodeWorldPtr->SetElem(1,3,worldPos.y);
				pNodeWorldPtr->SetElem(2,3,worldPos.z);
			}
    }

		{
			scenegraph::instance::JointInstanceNode* pJointNode = static_cast<scenegraph::instance::JointInstanceNode*>(pNode);            
			math::Matrix34* pSkinningMatrixPtr = const_cast<math::Matrix34*>(pJointNode->GetSkinningMatrixPtr());
			math::Matrix34::MultFast(pSkinningMatrixPtr, *pNodeWorldPtr, pJointNode->GetBindPoseMatrix());
		}
    
    m_UpdateCount++;  
	}
    
  u32 m_UpdateCount;
  u32 m_UpdateSkipCount;
};


void SceneGraphManager::TraverseModel()
{
	scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitor>				visitor;
	scenegraph::SceneGraphManager::Traverse<scenegraph::instance::TransformNode, TransformNodeVisitor>( &visitor );
}

void SceneGraphManager::TraverseModelFast()
{
	scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitorFast>				visitor;

	DagNode		*pChild = s_Gp->m_TransformRootNode->GetChild();

	if( pChild )
		Traverser::TraverseSiblingFast<scenegraph::instance::TransformNode, TransformNodeVisitorFast>( pChild, &visitor, Traverser::DEFAULT_DEPTH-1 );
}

void SceneGraphManager::TraverseModelFastFromChild( gfl2::renderingengine::scenegraph::DagNode *pNode )
{
	scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitorFast>				visitor;

	DagNode		*pChild = pNode->GetChild();

	if( pChild )
		Traverser::TraverseSiblingFast<scenegraph::instance::TransformNode, TransformNodeVisitorFast>( pChild, &visitor, Traverser::DEFAULT_DEPTH-1 );
}

void SceneGraphManager::TraverseModelFast( gfl2::renderingengine::scenegraph::DagNode *pNode )
{
  if (pNode == NULL)
  {
    return;
  }

	scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitorFast>				visitor;
  
  scenegraph::instance::TransformNode* pNodeTransformNode = pNode->SafeCast<scenegraph::instance::TransformNode>();
  if (pNodeTransformNode == NULL)
  {
    return;
  }
  visitor.Visit(pNodeTransformNode);

	DagNode		*pChild = pNode->GetChild();

	if( pChild )
		Traverser::TraverseSiblingFast<scenegraph::instance::TransformNode, TransformNodeVisitorFast>( pChild, &visitor, Traverser::DEFAULT_DEPTH-1 );
}

void SceneGraphManager::TraverseApplyBillboard(math::Matrix44 *viewMtx)
{
  scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitorApplyBillboard>				visitor;

  visitor.SetViewMatrix(viewMtx);

  DagNode		*pChild = s_Gp->m_TransformRootNode->GetChild();

  if( pChild )
    Traverser::TraverseSiblingFast<scenegraph::instance::TransformNode, TransformNodeVisitorApplyBillboard>( pChild, &visitor, Traverser::DEFAULT_DEPTH-1 );
}

void SceneGraphManager::TraverseApplyBillboard(math::Matrix34 *viewMtx)
{
	gfl2::math::Matrix44			viewMtx44( *viewMtx );

	SceneGraphManager::TraverseApplyBillboard( &viewMtx44 );
}

void SceneGraphManager::AddChild( scenegraph::Node* pNode )
{
	if ( s_Gp == NULL )
		return;

	if ( pNode->IsKindOf<DagNode>() )
	{
		DagNode  *pDag = pNode->SafeCast<DagNode>();

		if ( pDag->IsKindOf<resource::ResourceNode>() )
		{
			s_Gp->m_ResourceRootNode->AddChild( pDag );
		}
		else if ( pDag->IsKindOf<instance::InstanceNode>() )
		{
			if ( pDag->IsKindOf<instance::TransformNode>() )
			{
				s_Gp->m_TransformRootNode->AddChild( pDag );
			}
			else
			{
				s_Gp->m_InstanceRootNode->AddChild( pDag );
			}
		}
	}

	instance::CullingObjectNode  *pCullingObjectNode = pNode->SafeCast<instance::CullingObjectNode>();
	if ( pCullingObjectNode )
	{
		SceneGraphManager::HangOffTheTree( pCullingObjectNode );
		SceneGraphManager::HangOnTheTree( pCullingObjectNode );
	}
}

void SceneGraphManager::RemoveChild( scenegraph::Node* pNode )
{
	if ( s_Gp == NULL )			return;

	if ( pNode->IsKindOf<DagNode>() )
	{
		DagNode  *pDag = pNode->SafeCast<DagNode>();

		if ( pDag->IsKindOf<resource::ResourceNode>() )
		{
			s_Gp->m_ResourceRootNode->RemoveChild( pDag );
		}
		else if ( pDag->IsKindOf<instance::InstanceNode>() )
		{
			if ( pDag->IsKindOf<instance::TransformNode>() )
			{
				s_Gp->m_TransformRootNode->RemoveChild( pDag );
			}
			else
			{
				s_Gp->m_InstanceRootNode->RemoveChild( pDag );
			}
		}
	}

	instance::CullingObjectNode  *pCullingObjectNode = pNode->SafeCast<instance::CullingObjectNode>();
	if ( pCullingObjectNode )
		SceneGraphManager::HangOffTheTree( pCullingObjectNode );
}

void SceneGraphManager::UpdateAABBTree()
{
	for( u32 depth = 0; depth <= s_Gp->m_AABBTreeTop.m_Depth; ++depth )
		s_Gp->m_AABBTreeTop.UpdateVolumeAABB( depth );//depth = 0が最下層。最下層から上層に向かって更新。
}

void SceneGraphManager::HangOnTheTree( scenegraph::instance::CullingObjectNode* pNode )
{
	if ( s_Gp == NULL )		return;

	if ( pNode->GetState() & scenegraph::instance::CullingObjectNode::State::IgnoreAABBTree )
		return;

	if ( pNode->GetAABBTree() )
		return;//すでにぶら下がっている！

#ifdef _DEBUG
	{
		math::AABB			targetAABB = pNode->GetWorldAABB();
		math::Vector4		center = targetAABB.GetCenter();
		if( s_Gp->m_AABBTreeTop.GetAreaAABB().IsContain( center ) == false )
		{
#if 0 // @note 毎フレーム出力されてしまうので無効化しました ikuta
			::OutputDebugStringA( pNode->GetNodeName() );
			::OutputDebugStringA( "はAABBTree範囲よりはみ出ています。" );
			::OutputDebugStringA( "\n" );
#endif
		}
	}
#endif

	s_Gp->m_AABBTreeTop.AddObject( pNode );
}
void SceneGraphManager::HangOffTheTree( scenegraph::instance::CullingObjectNode* pNode )
{
	if ( s_Gp == NULL )		return;

	AABBTree				*pAABBTree = pNode->GetAABBTree();
	if ( pAABBTree )
	{
		pAABBTree->RemoveObject( pNode );
	}
}

void SceneGraphManager::AddDrawingNode( scenegraph::instance::ModelInstanceNode* pDrawableNode )
{
	if ( s_Gp == NULL )		return;

	DrawingNodeList			&rDrawingNodeList = s_Gp->m_DrawingNodeListDB[ s_Gp->m_DrawingNodeListDBCnt ];

	if ( rDrawingNodeList.m_NodeNum >= s_Gp->m_DrawingNodeListMax )
		return;

	GFL_ASSERT ( rDrawingNodeList.m_pDrawableNodeList[ rDrawingNodeList.m_NodeNum ] == NULL );

	pDrawableNode->ReferenceCntInc();
	rDrawingNodeList.m_pDrawableNodeList[ rDrawingNodeList.m_NodeNum ] = pDrawableNode;
	++rDrawingNodeList.m_NodeNum;
}

b32 SceneGraphManager::IsDrawingNode( scenegraph::instance::ModelInstanceNode* pDrawableNode )
{
	if ( s_Gp == NULL )		return false;

	DrawingNodeList			&rDrawingNodeList = s_Gp->m_DrawingNodeListDB[ s_Gp->m_DrawingNodeListDBCnt ];

	for( u32 i = 0; i < rDrawingNodeList.m_NodeNum; ++i )
	{
		if ( rDrawingNodeList.m_pDrawableNodeList[i] == pDrawableNode )
			return true;
	}
	return false;
}

void SceneGraphManager::DrawEnd()
{
	m_DrawingNodeListDBCnt ^= 1;//積み先変更

	DrawingNodeList			&rDrawingNodeList = s_Gp->m_DrawingNodeListDB[ s_Gp->m_DrawingNodeListDBCnt ];

	for( u32 i = 0; i < rDrawingNodeList.m_NodeNum; ++i )
	{//積まれていたものは、描画が終わっている。
		GFL_ASSERT ( rDrawingNodeList.m_pDrawableNodeList[i] );
		rDrawingNodeList.m_pDrawableNodeList[i]->ReferenceCntDec();
		rDrawingNodeList.m_pDrawableNodeList[i] = NULL;
	}

	rDrawingNodeList.m_NodeNum = 0;
}

#if GFL_DEBUG

// デバッグダンプ
class DebugDump
{
public:
	DebugDump()
  {
  }
	virtual ~DebugDump()
  {
  }

  enum ClassType
  {
    INSTANCENODE,
    CAMERANODE,
    DRAWENVNODE,
    EFFECTNODELOCATORINSTANCENODE,
    JOINTINSTANCENODE,
    LIGHTNODE,
    LIGHTSETNODE,
    MATERIALINSTANCENODE,
    MODELINSTANCENODE,
    MOTIONCONFIGINSTANCENODE,
    MOTIONINSTANCENODE,
    VIEWERLOCATORINSTANCENODE,
    LOCATORINSTANCENODE,
    DRAWABLENODE,
    CULLINGOBJECTNODE,
    TRANSFORMNODE,

    RESOURCENODE,
    ANIMATIONRESOURCENODE,
    ARRAYRESOURCENODE,
    BLENDSHAPEANIMATIONRESOURCENODE,
    CAMERARESOURCENODE,
    COLLISIONMESHRESOURCENODE,
    DRAWENVRESOURCENODE,
    GFMDLRESOURCENODE,
    GFMOTRESOURCENODE,
    LIGHTSETRESOURCENODE,
    LOCATORRESOURCENODE,
    MATERIALRESOURCENODE,
    MESHRESOURCENODE,
    MOTIONCONFIGRESOURCENODE,
    SHADERRESOURCENODE,
    TEXTURERESOURCENODE,

    DAGNODE,

    CLASS_TYPE_COUNT
  };

  struct DebugNodeInfo
  {
    const char* pNodeName;
    const char* pClassName;
    u32 classSize;
    u32 classType;

    DebugNodeInfo(const char* pNodeName, const char* pClassName, u32 classSize, u32 classType)
    {
      this->pNodeName = pNodeName;
      this->pClassName = pClassName;
      this->classSize = classSize;
      this->classType = classType;
    }
  };
  #define DEBUG_NODE_INFO(nodeType, classType) DebugNodeInfo("", #nodeType, sizeof(nodeType), classType)
  #define DEBUG_NODE_INFO_NAME(pNode, nodeType, classType) DebugNodeInfo(pNode->GetNodeName(), #nodeType, sizeof(nodeType), classType)
  #define DEBUG_NODE_INFO_NAME2(pNode, nodeType, classType) DebugNodeInfo(pNode->GetName(), #nodeType, sizeof(nodeType), classType)

  static DebugNodeInfo GetDebugNodeInfo( scenegraph::DagNode* pNode )
  {
    using namespace scenegraph::instance;
    using namespace scenegraph::resource;

    if (pNode->IsKindOf<InstanceNode>())
    {
      if (pNode->IsKindOf<CameraNode>())
      {
        CameraNode* pNameNode = static_cast<CameraNode*>(pNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, CameraNode, CAMERANODE);
      }
      if (pNode->IsKindOf<DrawEnvNode>())
      {
        return DEBUG_NODE_INFO(DrawEnvNode, DRAWABLENODE);
      }
      if (pNode->IsKindOf<EffectNodeLocatorInstanceNode>())
      {
        return DEBUG_NODE_INFO(EffectNodeLocatorInstanceNode, EFFECTNODELOCATORINSTANCENODE);
      }
      if (pNode->IsKindOf<JointInstanceNode>())
      {
        JointInstanceNode* pNameNode = static_cast<JointInstanceNode*>(pNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, JointInstanceNode, JOINTINSTANCENODE);
      }
      if (pNode->IsKindOf<LightNode>())
      {
        LightNode* pNameNode = static_cast<LightNode*>(pNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, LightNode, LIGHTNODE);
      }
      if (pNode->IsKindOf<LightSetNode>())
      {
        return DEBUG_NODE_INFO(LightSetNode, LIGHTSETNODE);
      }
      if (pNode->IsKindOf<MaterialInstanceNode>())
      {
        MaterialInstanceNode* pNameNode = static_cast<MaterialInstanceNode*>(pNode);
        return DEBUG_NODE_INFO_NAME2(pNameNode, MaterialInstanceNode, MATERIALINSTANCENODE);
      }
      if (pNode->IsKindOf<ModelInstanceNode>())
      {
        ModelInstanceNode* pNameNode = static_cast<ModelInstanceNode*>(pNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, ModelInstanceNode, MODELINSTANCENODE);
      }
      if (pNode->IsKindOf<MotionConfigInstanceNode>())
      {
        return DEBUG_NODE_INFO(MotionConfigInstanceNode, MOTIONCONFIGINSTANCENODE);
      }
      if (pNode->IsKindOf<MotionInstanceNode>())
      {
        return DEBUG_NODE_INFO(MotionInstanceNode, MOTIONINSTANCENODE);
      }
      if (pNode->IsKindOf<ViewerLocatorInstanceNode>())
      {
        return DEBUG_NODE_INFO(ViewerLocatorInstanceNode, VIEWERLOCATORINSTANCENODE);
      }
      if (pNode->IsKindOf<LocatorInstanceNode>())
      {
        LightNode* pNameNode = static_cast<LightNode*>(pNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, LocatorInstanceNode, LOCATORINSTANCENODE);
      }

      if (pNode->IsKindOf<DrawableNode>())
      {
        return DEBUG_NODE_INFO(DrawableNode, DRAWABLENODE);
      }
      if (pNode->IsKindOf<CullingObjectNode>())
      {
        return DEBUG_NODE_INFO(CullingObjectNode, CULLINGOBJECTNODE);
      }

      if (pNode->IsKindOf<TransformNode>())
      {
        return DEBUG_NODE_INFO(TransformNode, TRANSFORMNODE);
      }
      return DEBUG_NODE_INFO(InstanceNode, INSTANCENODE);
    }

    if (pNode->IsKindOf<ResourceNode>())
    {
      ResourceNode* pResNode = static_cast<ResourceNode*>(pNode);

      if (pResNode->IsKindOf<AnimationResourceNode>())
      {
        return DEBUG_NODE_INFO(AnimationResourceNode, ANIMATIONRESOURCENODE);
      }
      if (pResNode->IsKindOf<ArrayResourceNode>())
      {
        return DEBUG_NODE_INFO(ArrayResourceNode, ARRAYRESOURCENODE);
      }
      if (pResNode->IsKindOf<BlendShapeAnimationResourceNode>())
      {
        BlendShapeAnimationResourceNode* pNameNode = static_cast<BlendShapeAnimationResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, BlendShapeAnimationResourceNode, BLENDSHAPEANIMATIONRESOURCENODE);
      }
      if (pResNode->IsKindOf<CameraResourceNode>())
      {
        CameraResourceNode* pNameNode = static_cast<CameraResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, CameraResourceNode, CAMERARESOURCENODE);
      }
      if (pResNode->IsKindOf<CollisionMeshResourceNode>())
      {
        return DEBUG_NODE_INFO(CollisionMeshResourceNode, COLLISIONMESHRESOURCENODE);
      }
      if (pResNode->IsKindOf<DrawEnvResourceNode>())
      {
        DrawEnvResourceNode* pNameNode = static_cast<DrawEnvResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, DrawEnvResourceNode, DRAWENVRESOURCENODE);
      }
      if (pResNode->IsKindOf<GfMdlResourceNode>())
      {
        GfMdlResourceNode* pNameNode = static_cast<GfMdlResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, GfMdlResourceNode, GFMDLRESOURCENODE);
      }
      if (pResNode->IsKindOf<GfMotResourceNode>())
      {
        return DEBUG_NODE_INFO(GfMotResourceNode, GFMOTRESOURCENODE);
      }
      if (pResNode->IsKindOf<LightSetResourceNode>())
      {
        LightSetResourceNode* pNameNode = static_cast<LightSetResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, LightSetResourceNode, LIGHTSETRESOURCENODE);
      }
      if (pResNode->IsKindOf<LocatorResourceNode>())
      {
        LocatorResourceNode* pNameNode = static_cast<LocatorResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, LocatorResourceNode, LOCATORRESOURCENODE);
      }
      if (pResNode->IsKindOf<MaterialResourceNode>())
      {
        MaterialResourceNode* pNameNode = static_cast<MaterialResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME2(pNameNode, MaterialResourceNode, MATERIALRESOURCENODE);
      }
      if (pResNode->IsKindOf<MeshResourceNode>())
      {
        MeshResourceNode* pNameNode = static_cast<MeshResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, MeshResourceNode, MESHRESOURCENODE);
      }
      if (pResNode->IsKindOf<MotionConfigResourceNode>())
      {
        MotionConfigResourceNode* pNameNode = static_cast<MotionConfigResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, MotionConfigResourceNode, MOTIONCONFIGRESOURCENODE);
      }
      if (pResNode->IsKindOf<ShaderResourceNode>())
      {
        ShaderResourceNode* pNameNode = static_cast<ShaderResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, ShaderResourceNode, SHADERRESOURCENODE);
      }
      if (pResNode->IsKindOf<TextureResourceNode>())
      {
        TextureResourceNode* pNameNode = static_cast<TextureResourceNode*>(pResNode);
        return DEBUG_NODE_INFO_NAME(pNameNode, TextureResourceNode, TEXTURERESOURCENODE);
      }

      return DEBUG_NODE_INFO(ResourceNode, RESOURCENODE);
    }

    return DEBUG_NODE_INFO(DagNode, DAGNODE);
  }

	void Visit(scenegraph::DagNode* pNode, u32 depth, u32 childIndex, u32 childCount, u32 depthBit)
	{
    if (pNode == NULL)
    {
      return;
    }

    DebugNodeInfo info = GetDebugNodeInfo(pNode);

    if (depth > 0)
    {
      for (u32 i = 0; i < depth - 1; ++i)
      {
        if (depthBit & (1 << i))
        {
          GFL_PRINT("│");
        }
        else
        {
          GFL_PRINT("  ");
        }
      }
      if (childIndex + 1 == childCount)
      {
        GFL_PRINT("└");
      }
      else
      {
        GFL_PRINT("├");
      }
    }

    m_ClassInfoList[info.classType].pClassName = info.pClassName;
    m_ClassInfoList[info.classType].classSize = info.classSize;
    m_ClassInfoList[info.classType].totalCount += 1;
    m_ClassInfoList[info.classType].totalSize += info.classSize;
    GFL_PRINT("(%s) %s\n", info.pClassName, info.pNodeName);

#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE
    // 自分をフラグから外す
    u8* pNodeFlagList = SceneGraphManager::s_Gp->m_pDebugNodeFlagList;
    for (u32 i = 0; i < SceneGraphManager::GetNodeCount(); ++i)
    {
      if (SceneGraphManager::s_Gp->m_DebugNodeContainer[i] == pNode)
      {
        pNodeFlagList[i] = 0;
        break;
      }
    }
#endif
  }

  void Traverse(scenegraph::DagNode* pNode, u32 depth, u32 childIndex, u32 childCount, u32 depthBit)
  {
    this->Visit(pNode, depth, childIndex, childCount, depthBit);

    u32 nextDepth = depth + 1;
    u32 nowChildCount = pNode->GetChildCount();

    for (u32 i = 0; i < nowChildCount; ++i)
    {
      u32 nowDepthBit = depthBit;
      if (i + 1 == nowChildCount)
      {
        nowDepthBit &= ~(1 << depth);
      }

      this->Traverse(pNode->GetChild(i), nextDepth, i, nowChildCount, nowDepthBit);
    }
  }

  void Dump(scenegraph::DagNode* pRootNode)
  {
    GFL_PRINT("----- SceneGraphManager Dump begin -------------------------------------\n");

    // トータル情報初期化
    for (u32 i = 0; i < CLASS_TYPE_COUNT; ++i)
    {
      m_ClassInfoList[i].pClassName = "";
      m_ClassInfoList[i].classSize = 0;
      m_ClassInfoList[i].totalCount = 0;
      m_ClassInfoList[i].totalSize = 0;
    }

#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE
    u8* pNodeFlagList = SceneGraphManager::s_Gp->m_pDebugNodeFlagList;
    for (u32 i = 0; i < SceneGraphManager::GetNodeCount(); ++i)
    {
      pNodeFlagList[i] = 1;
    }
#endif

    // ルートノードからトラバース
    this->Traverse(pRootNode, 0, 0, 0, 0xFFFFFFFF);

    u32 chainedCount = 0;
    for (u32 i = 0; i < CLASS_TYPE_COUNT; ++i)
    {
      chainedCount += m_ClassInfoList[i].totalCount;
    }
    u32 unchainedCount = SceneGraphManager::GetNodeCount() - chainedCount;

#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE

    GFL_PRINT("\n");
    for (u32 i = 0; i < SceneGraphManager::GetNodeCount(); ++i)
    {
      if (pNodeFlagList[i])
      {
        this->Visit(SceneGraphManager::s_Gp->m_DebugNodeContainer[i]->SafeCast<DagNode>(), 0, 0, 0, 0xFFFFFFFF);
      }
    }
#endif

    // 使用メモリサイズでソート
    ::std::sort(m_ClassInfoList, m_ClassInfoList + CLASS_TYPE_COUNT, DebugClassInfoCompare());

    GFL_PRINT("\n\n");

    // トータルを合計
    u32 totalSize = 0;
    for (u32 i = 0; i < CLASS_TYPE_COUNT; ++i)
    {
      if (m_ClassInfoList[i].totalCount == 0)
      {
        break;
      }
      totalSize += m_ClassInfoList[i].totalSize;
    }

    u32 totalCount = unchainedCount + chainedCount;
    GFL_PRINT("totalSize(0x%08X, %.2fKB) chainedCount(%u)+unchainCount(%u)=totalCount(%u)\n", totalSize, totalSize / 1024.0f, chainedCount, unchainedCount, totalCount);

    GFL_PRINT("\n");

    // トータル情報をダンプ
    for (u32 i = 0; i < CLASS_TYPE_COUNT; ++i)
    {
      if (m_ClassInfoList[i].totalCount == 0)
      {
        break;
      }
      if (m_ClassInfoList[i].totalSize == 0)
      {
        break;
      }
      GFL_PRINT("%-32s : totalSize(0x%08X, %6.2fKB, %5.2f%%) totalCount(%4u, %5.2f%%) classSize(%u)\n", 
        m_ClassInfoList[i].pClassName,
        m_ClassInfoList[i].totalSize, m_ClassInfoList[i].totalSize / 1024.0f, 100.0f * m_ClassInfoList[i].totalSize/totalSize,
        m_ClassInfoList[i].totalCount, 100.0f * m_ClassInfoList[i].totalCount/totalCount,
        m_ClassInfoList[i].classSize);
    }

    GFL_PRINT("----- SceneGraphManager Dump end   -------------------------------------\n");
  }

private:

  struct DebugClassInfo
  {
    const char* pClassName;
    u32 classSize;
    u32 totalCount;
    u32 totalSize;
  };
  struct DebugClassInfoCompare
  {
    bool operator() (const DebugClassInfo& a, const DebugClassInfo& b)
    {
      return a.totalSize > b.totalSize;
    }
  };

  DebugClassInfo m_ClassInfoList[CLASS_TYPE_COUNT];
};

/**
 * @brief シーングラフをコンソールにダンプ
 */
void SceneGraphManager::Dump( void )
{
  scenegraph::DagNode* pRootNode = scenegraph::SceneGraphManager::GetRootNode();

  DebugDump dd;
  dd.Dump(pRootNode);
}

#endif

}}}

