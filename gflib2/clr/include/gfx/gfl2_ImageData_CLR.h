#ifndef GFLIB2_GRX_CLR_IMAGEDATA_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_IMAGEDATA_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace clr { namespace gfx {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class ImageData{
public:

	ImageData();
	~ImageData(){this->!ImageData();}
	!ImageData();

	void ReadFromFile( System::String^ path );

	gfl2::gfx::ImageData* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::ImageData			*m_pNative;
};


}}}

#endif