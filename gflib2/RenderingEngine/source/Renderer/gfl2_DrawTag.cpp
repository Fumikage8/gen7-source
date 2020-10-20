#include <RenderingEngine/include/renderer/gfl2_DrawTag.h>

namespace gfl2 { namespace renderingengine { namespace renderer { 

DrawTag::DrawTag():
  m_IsVisible(true),
	m_DrawLayerNo( 0 ),
	m_LightSetNo( -1 ),
	m_pWorldAABB( NULL ),
  m_RenderKey(0),
	m_pDrawListener( NULL )
{

}

DrawTag::~DrawTag()
{

}

}}}

