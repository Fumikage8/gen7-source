#ifndef GFLIB2_GRX_SURFACE_H_INCLUDED
#define GFLIB2_GRX_SURFACE_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>

namespace gfl2 { namespace gfx {

template <class Implement_>
class GFGLBase;

class Surface : public GLMemory
{
public:

	template <class Implement_>
	friend class GFGLBase;

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief サーフェイスの詳細情報
	struct SurfaceDesc
	{
		u32						usage;				//!< 使用用途フラグ
		Pool					pool;					//!< メモリ配置
		u32						width;				//!< 幅
		u32						height;				//!< 高さ
		u32						depth;				//!< 深度
		Format				format;				//!< ピクセルフォーマット
		MultiSample		multiSample;	//!< マルチサンプリング
#if defined(GF_PLATFORM_CTR)
		u32						bufferWidth;		//!< 幅
		u32						bufferHeight;	//!< 高さ
#endif
	};

	virtual ~Surface();

	const SurfaceDesc& GetSurfaceDesc() const { return m_SurfaceDesc; }

protected:
	Surface( const SurfaceDesc &rDesc );
	SurfaceDesc					m_SurfaceDesc;
};

}}

#endif
