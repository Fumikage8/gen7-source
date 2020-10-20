#include <clr/include/gfx/gfl2_StateObject_CLR.h>

namespace gfl2 { namespace clr { namespace gfx {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
BlendStateObject::!BlendStateObject()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = NULL;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void BlendStateObject::SetBlendOp( BlendOp::Enum color, BlendOp::Enum alpha )
{
	gfl2::gfx::BlendOp		color_ = static_cast<gfl2::gfx::BlendOp>((u32)color);
	gfl2::gfx::BlendOp		alpha_ = static_cast<gfl2::gfx::BlendOp>((u32)alpha);

	m_pNative->SetBlendOp( color_, alpha_ );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void BlendStateObject::SetBlendOp( BlendOp::Enum op )
{
	gfl2::gfx::BlendOp		op_ = static_cast<gfl2::gfx::BlendOp>((u32)op);

	m_pNative->SetBlendOp( op_ );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void BlendStateObject::SetBlendFunc( BlendFunc::Enum sfc, BlendFunc::Enum dfc, BlendFunc::Enum sfa, BlendFunc::Enum dfa )
{
	gfl2::gfx::BlendFunc		sfc_ = static_cast<gfl2::gfx::BlendFunc>((u32)sfc);
	gfl2::gfx::BlendFunc		dfc_ = static_cast<gfl2::gfx::BlendFunc>((u32)dfc);
	gfl2::gfx::BlendFunc		sfa_ = static_cast<gfl2::gfx::BlendFunc>((u32)sfa);
	gfl2::gfx::BlendFunc		dfa_ = static_cast<gfl2::gfx::BlendFunc>((u32)dfa);

	m_pNative->SetBlendFunc( sfc_, dfc_, sfa_, dfa_ );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void BlendStateObject::SetBlendFunc( BlendFunc::Enum src, BlendFunc::Enum dest)
{
	gfl2::gfx::BlendFunc		src_ = static_cast<gfl2::gfx::BlendFunc>((u32)src);
	gfl2::gfx::BlendFunc		dest_ = static_cast<gfl2::gfx::BlendFunc>((u32)dest);

	m_pNative->SetBlendFunc( src_, dest_ );
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
RasterizerStateObject::!RasterizerStateObject()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = NULL;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void RasterizerStateObject::SetCullMode(CullMode::Enum mode)
{
	gfl2::gfx::CullMode mode_ = static_cast<gfl2::gfx::CullMode>( (u32)mode );
	m_pNative->SetCullMode(mode_);
}


//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
CullMode::Enum RasterizerStateObject::GetCullMode()
{
	gfl2::gfx::CullMode		mode;
	mode = m_pNative->GetCullMode();
	int		value = mode.Get();

	return CullMode(value);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void RasterizerStateObject::SetFillMode(const FillMode::Enum mode)
{
	gfl2::gfx::FillMode mode_ = static_cast<gfl2::gfx::FillMode>( (u32)mode );
	m_pNative->SetFillMode(mode_);
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
DepthStencilStateObject::!DepthStencilStateObject()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = NULL;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DepthStencilStateObject::SetDepthTestEnable(bool enable)
{
	m_pNative->SetDepthTestEnable(enable);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DepthStencilStateObject::SetDepthFunc(const CompareFunc::Enum func)
{
	gfl2::gfx::CompareFunc func_ = static_cast<gfl2::gfx::CompareFunc>( (u32)func );
	m_pNative->SetDepthFunc(func_);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
CompareFunc::Enum DepthStencilStateObject::GetDepthFunc()
{
	gfl2::gfx::CompareFunc		func;
	func = m_pNative->GetDepthFunc();
	int		value = func.Get();

	return CompareFunc(value);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DepthStencilStateObject::SetStencilFunc( const PolygonFace::Enum face, const CompareFunc::Enum func, const u8 ref, const u8 mask )
{
	gfl2::gfx::PolygonFace face_ = static_cast<gfl2::gfx::PolygonFace>( (u32)face );
	gfl2::gfx::CompareFunc func_ = static_cast<gfl2::gfx::CompareFunc>( (u32)func );

	m_pNative->SetStencilFunc( face_, func_, ref, mask );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DepthStencilStateObject::SetStencilWriteMask( const u8 bits )
{
	m_pNative->SetStencilWriteMask( bits );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DepthStencilStateObject::SetStencilOp( const PolygonFace::Enum face, const StencilOp::Enum fail, const StencilOp::Enum depthFail, const StencilOp::Enum depthPass )
{
	gfl2::gfx::PolygonFace face_ = static_cast<gfl2::gfx::PolygonFace>( (u32)face );
	gfl2::gfx::StencilOp fail_ = static_cast<gfl2::gfx::StencilOp>( (u32)fail );
	gfl2::gfx::StencilOp depthFail_ = static_cast<gfl2::gfx::StencilOp>( (u32)depthFail );
	gfl2::gfx::StencilOp depthPass_ = static_cast<gfl2::gfx::StencilOp>( (u32)depthPass );

	this->m_pNative->SetStencilOp( face_, fail_, depthFail_, depthPass_ );
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
SamplerStateObject::!SamplerStateObject()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = NULL;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void SamplerStateObject::SetSamplerAddress( const TextureAddress::Enum  wrapS, const TextureAddress::Enum  wrapT, const TextureAddress::Enum  wrapR, const CompareFunc::Enum sampleCmp )
{
	gfl2::gfx::TextureAddress wrapS_ = static_cast<gfl2::gfx::TextureAddress>( (u32)wrapS );
	gfl2::gfx::TextureAddress wrapT_ = static_cast<gfl2::gfx::TextureAddress>( (u32)wrapT );
	gfl2::gfx::TextureAddress wrapR_ = static_cast<gfl2::gfx::TextureAddress>( (u32)wrapR );
	gfl2::gfx::CompareFunc sampleCmp_ = static_cast<gfl2::gfx::CompareFunc>( (u32)sampleCmp );

	m_pNative->SetSamplerAddress( wrapS_, wrapT_, wrapR_, sampleCmp_ );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void SamplerStateObject::SetSamplerBorderColor( Color^ color )
{
	m_pNative->SetSamplerBorderColor( *(color->GetNative()) );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void SamplerStateObject::SetSamplerControl( const u16 minLod, const u16 maxLod, const u8  maxAniso )
{
	m_pNative->SetSamplerControl( minLod, maxLod, maxAniso );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void SamplerStateObject::SetSamplerFilter( const TextureFilter::Enum magFilter,  const TextureFilter::Enum minFilter,  const TextureFilter::Enum mipFilter,  const float mipmapLodBias )
{
	gfl2::gfx::TextureFilter magFilter_ = static_cast<gfl2::gfx::TextureFilter>( (u32)magFilter );
	gfl2::gfx::TextureFilter minFilter_ = static_cast<gfl2::gfx::TextureFilter>( (u32)minFilter );
	gfl2::gfx::TextureFilter mipFilter_ = static_cast<gfl2::gfx::TextureFilter>( (u32)mipFilter );

	m_pNative->SetSamplerFilter( magFilter_,  minFilter_, mipFilter_, mipmapLodBias );
}


}}}