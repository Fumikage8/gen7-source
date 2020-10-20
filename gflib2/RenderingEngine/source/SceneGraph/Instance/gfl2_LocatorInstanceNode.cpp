//=============================================================================
/**
 * @file    gfl2_LocatorInstanceNode.cpp
 * @brief   LocatorInstanceNodeクラス（ロケーターインスタンスノード。TransformNodeの１種。エフェクトの発生源などに用いられる。）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.4.19    ishiguro  LocatorInstanceNode::Util::DetachFromParentNode関数追加
 *
 */
//=============================================================================
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerShaderDriver.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//JointInstanceNode用ビジターを作成
class JointInstanceNodeNodeVisitor
{
public:
	JointInstanceNodeNodeVisitor():m_pLocatorInstanceNodeTop( NULL ){}
	virtual ~JointInstanceNodeNodeVisitor() {}

	inline void Visit( JointInstanceNode* pNode )
	{
		if ( m_pLocatorInstanceNodeTop->GetChildCount() )
		{//子供がいるうちは、まず子供を移動させていく。
			for( u32 childNo = 0; childNo < m_pLocatorInstanceNodeTop->GetChildCount(); ++childNo )
			{
				DagNode							*pDagNode = m_pLocatorInstanceNodeTop->GetChild( childNo );
				LocatorInstanceNode	*pLocatorInstanceNode = pDagNode->SafeCast<LocatorInstanceNode>();

				if ( std::strcmp( pLocatorInstanceNode->GetParentName(), pNode->GetNodeName() ) == 0 )
				{
					pNode->AddChild( pLocatorInstanceNode );//この時点でm_pLocatorInstanceNodeTop->GetChildCount()が１つ減る。
					--childNo;
				}
			}
		}
		else
		{//自分(m_pLocatorInstanceNodeTop自身)を移動
			if ( std::strcmp( m_pLocatorInstanceNodeTop->GetParentName(), pNode->GetNodeName() ) == 0 )
			{
				pNode->AddChild( m_pLocatorInstanceNodeTop );
			}
		}
	}

	LocatorInstanceNode					*m_pLocatorInstanceNodeTop;
};

void LocatorInstanceNode::Util::AttachToJointInstanceNode( JointInstanceNode *pJointNodeTop, LocatorInstanceNode *pLocatorNodeTop )
{
	scenegraph::Traverser::Visitor<JointInstanceNode, JointInstanceNodeNodeVisitor>				visitor;
	visitor.m_pLocatorInstanceNodeTop = pLocatorNodeTop;

	scenegraph::Traverser::Traverse<JointInstanceNode, JointInstanceNodeNodeVisitor>( pJointNodeTop, &visitor );//子供移動
	scenegraph::Traverser::Traverse<JointInstanceNode, JointInstanceNodeNodeVisitor>( pJointNodeTop, &visitor );//自分を移動
}

/**
* @brief ロケーターを親から外す関数
* @param pLocatorNodeTop      対象ロケーター
* @attention 指定ロケーターが子供を持っていた場合は、アサートで止まります。
*/
void LocatorInstanceNode::Util::DetachFromParentNode( LocatorInstanceNode *pLocatorNodeTop )
{
  if ( pLocatorNodeTop == NULL )
  {
    GFL_ASSERT_MSG( 0, "NULLポインターが渡ってきました。誤ったロケーターを渡していませんか？" );
    return;//Finalビルドではリターン
  }

  if ( pLocatorNodeTop->GetChildCount() )
  {
    GFL_ASSERT_MSG( 0, "外そうとしたロケーターが子を持っています。子の扱いを確認してください。" );
    return;//Finalビルドではリターン
  }
  
  DagNode       *pParentNode = pLocatorNodeTop->GetParent();

  if ( pParentNode == NULL )
  {
    GFL_ASSERT_MSG( 0, "すでに親を持たない状態です。誤ったロケーターを渡していませんか？" );
    return;//Finalビルドではリターン
  }
  
  pParentNode->RemoveChild( pLocatorNodeTop );
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  ModelInstanceNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
LocatorInstanceNode::LocatorInstanceNode() : 
  NodeTrait<LocatorInstanceNode, TransformNode>(),
  m_LocatorResourceNode(NULL)
{
}

LocatorInstanceNode::~LocatorInstanceNode()
{
	
}

void LocatorInstanceNode::SetResourceNode( const resource::LocatorResourceNode *pResourceNode )
{
	GFL_ASSERT(pResourceNode);
	SetNodeName( pResourceNode->GetNodeName() );
	std::memcpy( m_ParentName, pResourceNode->GetParentName(), sizeof(m_ParentName) );


  math::Matrix44 tmp = *(math::Matrix44*)&pResourceNode->GetMatrix();
  ///tmp = tmp.Transpose(); // 2015.7.08-PETE Matrix is already TRANSPOSE_MTX!
  math::Vector3 scale = tmp.GetElemScaling();
  tmp.Normalize();
	math::Matrix34 matrix = *(math::Matrix34*)&tmp;

  this->SetLocalScale(scale);
  math::Quaternion rot;
  rot.MatrixToQuaternion(matrix);
  this->SetLocalRotation(rot);
  this->SetLocalTranslate(matrix.GetElemPosition());

	m_LocatorResourceNode = pResourceNode;
}


}}}}

