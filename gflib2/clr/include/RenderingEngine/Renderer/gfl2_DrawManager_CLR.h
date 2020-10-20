#ifndef GFLIB2_RENDERINGENGINE_CLR_RENDERER_DRAWMANAGER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_RENDERER_DRAWMANAGER_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace renderer {

public ref class InitDescription
{
public:
	InitDescription( u32 nodeSize, u32 tagSize );
	~InitDescription()
	{
		this->!InitDescription();
	}
	!InitDescription();

	gfl2::renderingengine::renderer::DrawManager::InitDescription* GetNative()
	{
		return m_pNative;
	}

private:
	gfl2::renderingengine::renderer::DrawManager::InitDescription* m_pNative;
};

public ref class DrawManager
{
public:
	static DrawManager^ Create();
	void AddObject(scenegraph::instance::DrawableNode^ pNode);
	void RemoveAllObject();

	gfl2::renderingengine::renderer::DrawManager* GetNative() { return m_pNative; }
		
	void SetProjectionMatrix( gfl2::clr::math::Matrix^ mat );

	void SetViewMatrix( gfl2::clr::math::Matrix^ mat );

	void Draw();

protected:
	DrawManager( gfl2::renderingengine::renderer::DrawManager* native)
	{
		m_pNative = native;
	}
	//DrawManager(InitDescription^ desc);

private:
	gfl2::renderingengine::renderer::DrawManager* m_pNative;
};

}}}}

#endif