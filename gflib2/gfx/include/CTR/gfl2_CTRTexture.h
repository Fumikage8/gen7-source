#ifndef GFLIB2_GRX_CTR_CTRTEXTURE_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRTEXTURE_H_INCLUDED

#include <gfx/include/gfl2_texture.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRGL;

class CTRTexture : public Texture
{
public:

	friend class CTRGL;

	CTRTexture( const TextureDesc &rDesc );
	virtual ~CTRTexture();

	//! @brief テクスチャを更新する
	virtual void Update( const void* pData,	u32 depth = 0 );

  uptr GetVirtualAddr() const
  {
    return m_VirtualAddr;
  }

  uptr GetPhysicalAddr() const
  {
    return m_PhysicalAddr;
  }

  PicaDataTextureFormat GetFormat() const
  {
    return m_Format;
  }

	virtual void* Lock();
	virtual void UnLock();

private:

  uptr m_VirtualAddr;
  uptr m_PhysicalAddr;
  PicaDataTextureFormat m_Format;
};

}}}

#endif
