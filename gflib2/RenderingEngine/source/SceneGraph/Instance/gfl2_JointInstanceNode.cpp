#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerShaderDriver.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  ModelInstanceNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
JointInstanceNode::JointInstanceNode() : NodeTrait<JointInstanceNode, TransformNode>(),
  m_SkinningMatrix(math::Matrix34::GetIdentity()),
  m_pResourceData(NULL),
  m_NeededRendering(false)
{

}

JointInstanceNode::~JointInstanceNode()
{
	
}

void JointInstanceNode::SetJointData( const resource::GfMdlResourceNode::JointData *pData )
{
  m_pResourceData = pData;
	
	m_NeededRendering = pData->m_BitStatus.m_NeededRendering;
  this->SetNonInheritanceScale( pData->m_BitStatus.m_ScaleSegmentCompensate == 0 );

  this->SetBillboardType(pData->m_BitStatus.m_BillboardType);

  // @attention Windows版のバイナリには転置する前の行列が入っている、タイミングを見てCTR版と揃える必要がある
#if defined(GF_PLATFORM_WIN32)
  math::Matrix44 tmp = pData->m_GlobalMatrix.Inverse();
  tmp = tmp.Transpose();
  m_BindPoseMatrix = math::Matrix34(tmp[0], tmp[1], tmp[2]);
#elif defined(GF_PLATFORM_CTR)
  m_BindPoseMatrix = pData->m_GlobalMatrix.Inverse();
#endif
  m_DefaultSRT = math::SRT(pData->m_LocalScale, pData->m_LocalRotation, pData->m_LocalTranslate);
  m_LocalSRT = m_DefaultSRT;

  this->SetTransformFlag(TransformNode::TransformFlag::JointNode);
}


}}}}

