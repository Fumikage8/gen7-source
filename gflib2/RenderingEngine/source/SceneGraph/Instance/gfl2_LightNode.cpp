#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  LightNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
LightNode::LightNode() : NodeTrait<LightNode, CullingObjectNode>(),
	m_LightType( LightType::Directional ),
	m_Position(0.0f, 0.0f, 0.0f),
	m_Direction(0.0f, 0.0f, 1.0f),
	m_Color(1.0f, 1.0f, 1.0f, 1.0f),
	m_ConeAngle( 0.0f ),
	m_DecayRate( 0.0f ),
	m_Intensity( 1.0f ),
  m_Priority( 0 ),
  m_NodeNameHash( 0 )
{
	
}

LightNode::~LightNode()
{
	
}

math::Vector3 LightNode::GetWorldDirection() const
{
  // このままだとワールド行列の位置成分まで反映されてしまう、位置成分を削除してから計算する
  math::Matrix34 dirMatrix = m_WorldMatrix;
  dirMatrix[0][3] = 0.0f;
  dirMatrix[1][3] = 0.0f;
  dirMatrix[2][3] = 0.0f;

  return dirMatrix * m_Direction;
}

math::Matrix34 LightNode::GetMatrix( void ) const
{
  math::Vector3 worldDir = this->GetWorldDirection();
	math::Matrix34 lookAt = math::Matrix34::GetLookAt( math::Vector3::GetZero(), worldDir, math::Vector3(0.0f,1.0f,0.0f) );
  math::Vector3 pos = m_WorldMatrix.GetElemPosition();
	lookAt[0][3] = pos.x;
	lookAt[1][3] = pos.y;
	lookAt[2][3] = pos.z;
	return lookAt;
}


}}}}

