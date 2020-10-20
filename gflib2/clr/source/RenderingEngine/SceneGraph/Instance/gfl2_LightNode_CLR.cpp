#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_LightNode_CLR.h>
#include <clr/include/util/gfl2_String_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

LightNode::LightNode(gfl2::renderingengine::scenegraph::instance::LightNode* pNative) :
m_pNative(pNative)
{
}

LightNode::!LightNode()
{
	// このノードを持つDrawEnvNodeで削除の処理を行うのでネイティブポインタのdeleteはしない
}

void LightNode::Release()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = NULL;
  }
}

void LightNode::SetNodeName(System::String^ name)
{
  std::string name_ = gfl2::clr::util::String::GetNativeString(name);
  m_pNative->SetNodeName(name_.c_str());
}

void LightNode::SetNodeNameHash(u32 hash)
{
	m_pNative->SetNodeNameHash(hash);
}

void LightNode::SetLightType(gfl2::clr::renderingengine::LightType::Enum type)
{
	m_pNative->SetLightType(static_cast<gfl2::renderingengine::LightType::Enum>(type));
}

void LightNode::SetColor(clr::math::Vector^ color)
{
	m_pNative->SetColor( *( color->GetNative() ) );
}

void LightNode::SetIntensity( f32 intensity )
{
	m_pNative->SetIntensity(intensity);
}

void LightNode::SetPosition( clr::math::Vector^ pos )
{
	m_pNative->SetPosition( pos->GetNativeVector3() );
}

void LightNode::SetDirection( clr::math::Vector^ dir )
{
  m_pNative->SetDirection( dir->GetNativeVector3()  );
}

void LightNode::SetDecayRate( f32 decayRate )
{
	m_pNative->SetDecayRate(decayRate);
}

void LightNode::SetConeAngle( f32 coneAngle )
{
	m_pNative->SetConeAngle(coneAngle);
}

void LightNode::SetPriority( s32 priority )
{
	m_pNative->SetPriority(priority);
}

u32 LightNode::GetNodeNameHash()
{
	return m_pNative->GetNodeNameHash();
}

gfl2::clr::renderingengine::LightType::Enum LightNode::GetLightType()
{
  return static_cast<gfl2::clr::renderingengine::LightType::Enum>((u32)m_pNative->GetLightType());
}

clr::math::Vector^ LightNode::GetColor()
{
	clr::math::Vector^ color = gcnew clr::math::Vector( m_pNative->GetColor() );
	return color;
}

f32 LightNode::GetIntensity()
{
	return m_pNative->GetIntensity();
}

clr::math::Vector^ LightNode::GetPosition()
{
	clr::math::Vector^ position = gcnew clr::math::Vector( m_pNative->GetPosition() );
	return position;
}

clr::math::Vector^ LightNode::GetDirection()
{
	clr::math::Vector^ direction = gcnew clr::math::Vector( m_pNative->GetDirection() );
	return direction;
}

f32 LightNode::GetDecayRate()
{
	return m_pNative->GetDecayRate();
}

f32 LightNode::GetConeAngle()
{
	return m_pNative->GetConeAngle();
}

s32 LightNode::GetPriority()
{
	return m_pNative->GetPriority();
}
		
clr::math::Matrix^ LightNode::GetMatrix()
{
	gfl2::clr::math::Matrix^ matrix = gcnew gfl2::clr::math::Matrix(m_pNative->GetMatrix());
	return matrix;
}

}}}}}