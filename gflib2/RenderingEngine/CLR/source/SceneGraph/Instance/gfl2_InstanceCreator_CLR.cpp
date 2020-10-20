#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/CLR/include/SceneGraph/Instance/gfl2_InstanceCreator_CLR.h>

namespace gfl2 { namespace renderingengine { namespace clr { namespace scenegraph { namespace instance {

ModelInstanceNode^ InstanceCreator::CreateModelInstanceNode(resource::ResourceNode^ pSrc)
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode		*pNative;
	pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(pSrc->GetNative());
	gfl2::renderingengine::scenegraph::instance::DrawableNode		*pDrawableNode = pNative->SafeCast<gfl2::renderingengine::scenegraph::instance::DrawableNode>();

	ModelInstanceNode^ pObject = gcnew ModelInstanceNode(pDrawableNode);
	return pObject;
}

}}}}}