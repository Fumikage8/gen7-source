#ifndef GFLIB2_GRX_WINDX11_WINDX11STATEOBJECT_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11STATEOBJECT_H_INCLUDED

#include <gfx/include/gfl2_StateObject.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11GL;

class WinDX11BlendStateObject : public BlendStateObject
{
	friend class WinDX11GL;
public:
	virtual void UpdateState();
	virtual ~WinDX11BlendStateObject();

protected:
	
	WinDX11BlendStateObject();

	AutoComPtr<ID3D11BlendState>				m_pBlendState;

};

class WinDX11RasterizerStateObject : public RasterizerStateObject
{
	friend class WinDX11GL;
public:
	virtual void UpdateState();
	virtual ~WinDX11RasterizerStateObject();

protected:
	
	WinDX11RasterizerStateObject();

	AutoComPtr<ID3D11RasterizerState>	  m_pRasterizerState;

};

class WinDX11DepthStencilStateObject : public DepthStencilStateObject
{
	friend class WinDX11GL;
public:
	virtual void UpdateState();
	virtual ~WinDX11DepthStencilStateObject();

protected:
	
	WinDX11DepthStencilStateObject();

	AutoComPtr<ID3D11DepthStencilState>  m_pDepthStencilState;

};

class WinDX11SamplerStateObject : public SamplerStateObject
{
	friend class WinDX11GL;
public:
	virtual void UpdateState();
	virtual ~WinDX11SamplerStateObject();

protected:
	
	WinDX11SamplerStateObject();

	AutoComPtr<ID3D11SamplerState>       m_pTextureSampler;

};


}}}

#endif