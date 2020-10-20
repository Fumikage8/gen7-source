#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <gfx/include/gfl2_CommonGL.h>

namespace gfl2 { namespace gfx {

CommonGL::CommonGL() :
	m_pActiveDisplay( NULL ),
	m_IsSceneDrawing( false )
{

}

CommonGL::~CommonGL()
{

}

//  アクティブなディスプレイを変更する
b32 CommonGL::SetActiveDisplay_( DisplayDeviceHandle hDisplay )
{
	//  ポインタを更新
	if( m_pActiveDisplay != hDisplay )
	{
		//  シーンのレンダリング中は切り替えられない
		if( this->m_IsSceneDrawing )
			return false;
	}

	//  ポインタを更新
	m_pActiveDisplay = hDisplay;

	return true;
}

//  アクティブなディスプレイを取得する
DisplayDeviceHandle CommonGL::GetActiveDisplay_(  )
{
	return m_pActiveDisplay;
}

}}
