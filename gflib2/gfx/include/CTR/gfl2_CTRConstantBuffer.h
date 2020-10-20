#ifndef GFLIB2_GRX_CTR_CTRCONSTANTBUFFER_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRCONSTANTBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRGL;

class CTRConstantBuffer : public ConstantBuffer
{
public:
  static const s32 MAX_ELEMENT_COUNT = 16;
public:
	friend class CTRGL;

	CTRConstantBuffer(const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer);
	virtual ~CTRConstantBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

  u32 GetElementCount() const
  {
    return m_ElementCount;
  }

  ConstantType GetType(u32 index) const
  {
    return static_cast<ConstantType>(m_TypeList[index]);
  }

  const void* GetBuffer(u32 index) const
  {
    u32 offset = m_OffsetList[index];
    const u8* pBuffer = static_cast<const u8*>(m_pBuffer);
    return pBuffer + offset;
  }

private:

  u32           m_ElementCount;
	u32						m_Size;
  void*         m_pBuffer;
  u32           m_OffsetList[MAX_ELEMENT_COUNT];
  u8            m_TypeList[MAX_ELEMENT_COUNT];
};

}}}

#endif
