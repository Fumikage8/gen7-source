#include <clr/include/RenderingEngine/Renderer/gfl2_DrawManager_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/gfx/gfl2_GFGL_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace renderer { 

InitDescription::InitDescription( u32 nodeSize, u32 tagSize )
{
	this->m_pNative = new gfl2::renderingengine::renderer::DrawManager::InitDescription( nodeSize, tagSize );
}

InitDescription::!InitDescription()
{
	delete this->m_pNative;
}

DrawManager^ DrawManager::Create()
{
  gfl2::renderingengine::renderer::DrawManager* pNative;
  pNative = gfl2::renderingengine::renderer::DrawManager::Create(gfl2::clr::gfx::GFGL::GetGLAllocator());
  DrawManager^ pObject = gcnew DrawManager(pNative);
  return pObject;
}

void DrawManager::AddObject( scenegraph::instance::DrawableNode^ pNode )
{
	m_pNative->AddObject(pNode->GetNative());
}

void DrawManager::RemoveAllObject()
{
	m_pNative->RemoveAllObject();
}

void DrawManager::SetProjectionMatrix( gfl2::clr::math::Matrix^ mat )
{
	// こういう書き方は出来ない
	//gfl2::math::Matrix* pMatrix = mat.GetNative();
	//m_pNative->SetProjectionMatrix(*pMatrix);

	// なので4x4の行列の値をひとつずつ取ってきて設定
	gfl2::math::Matrix				nativeMat;
	for(s32 col = 0; col < 4; ++col)
	{
		for(s32 row = 0; row < 4; ++row)
		{
			float elem = mat->GetElem(col, row);
			nativeMat.SetElem(col, row, elem);
		}
	}
  gfl2::math::Matrix44 nativeMat44 = *(gfl2::math::Matrix44*)&nativeMat;
	this->m_pNative->SetProjectionMatrix( nativeMat44.Transpose() );
}

void DrawManager::SetViewMatrix( gfl2::clr::math::Matrix^ mat )
{
	//gfl2::math::Matrix* pMatrix = mat.GetNative();
	//m_pNative->SetViewMatrix(*pMatrix);
	
	// 4x4の行列の値をひとつずつ取ってきて設定
	gfl2::math::Matrix				nativeMat;
	for(s32 col = 0; col < 4; ++col)
	{
		for(s32 row = 0; row < 4; ++row)
		{
			float elem = mat->GetElem(col, row);
			nativeMat.SetElem(col, row, elem);
		}
	}
  nativeMat = nativeMat.Transpose();
  gfl2::math::Matrix34 nativeMat34 = *(gfl2::math::Matrix34*)&nativeMat;
	this->m_pNative->SetViewMatrix( nativeMat34 );
}

void DrawManager::Draw()
{
	this->m_pNative->Draw();
}

/*
DrawManager::DrawManager(InitDescription desc)
{

}
*/

}}}}
