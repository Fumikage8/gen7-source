#ifndef GFLIB2_GRX_COMMONGL_H_INCLUDED
#define GFLIB2_GRX_COMMONGL_H_INCLUDED

#include <stdio.h>
#include <gfx/include/gfl2_types.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <gfx/include/gfl2_Color.h>

namespace gfl2 { namespace gfx { 

class CommonGL : public GLMemory
{
public:
	virtual ~CommonGL();

protected:

	CommonGL();

	//仮想関数は禁止

	b32 SetActiveDisplay_( DisplayDeviceHandle hDisplay );
	DisplayDeviceHandle GetActiveDisplay_();

	void SetDrawFlag_( b32 f ){ m_IsSceneDrawing = f; }
	b32 IsSceneDrawing_(){return m_IsSceneDrawing;}

	DisplayDeviceHandle		m_pActiveDisplay;
	b32										m_IsSceneDrawing;
};

}}

#endif
