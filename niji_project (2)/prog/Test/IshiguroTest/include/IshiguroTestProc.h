#if PM_DEBUG
//
// CTR - Ro Module : IshiguroTestProc
//
#pragma  once

// niji
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(IshiguroTest)

class IshiguroTestProc : public GameSys::GameProc
{

public:

	IshiguroTestProc( void );
	virtual ~IshiguroTestProc( void );

	virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
	virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
	virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
	virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo n);
	virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
	virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
	
  
private:

	gfl2::heap::HeapBase* m_Heap;

	gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_pBinModelRootNode;
	gfl2::renderingengine::renderer::DefaultRenderingPipeLine												*m_pRenderingPipeLine;
	//gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_pModelInstanceNode;
	Field::KusaModel::FieldKusaModel																								*m_pFieldKusaModel;

	gfl2::renderingengine::scenegraph::instance::DrawEnvNode													*m_pDrawEnvNode;
	gfl2::renderingengine::scenegraph::instance::LightSetNode												*m_pLightSetNode;
	gfl2::renderingengine::scenegraph::instance::LightNode														*m_pDirectionalLightNode;
	gfl2::renderingengine::scenegraph::instance::LightNode														*m_pAmbientLightNode;
	s32																																							m_Cnt;
	gfl2::math::Vector3																															m_TestPos;

};

GFL_NAMESPACE_END(IshiguroTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
