#include <clr/include/gfx/gfl2_ImageData_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace gfx {

ImageData::ImageData()
{
	m_pNative = new gfl2::gfx::ImageData();
}

ImageData::!ImageData()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = NULL;
	}
}

void ImageData::ReadFromFile( System::String^ path )
{
	IntPtr					ptr;
	const char*			path_;

	ptr = Marshal::StringToHGlobalAnsi( path );
	path_ = (const char*)ptr.ToPointer();

	m_pNative->ReadFromFile( path_ );

	Marshal::FreeHGlobal( IntPtr((void*)path_) );
}


}}}