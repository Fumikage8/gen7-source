#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRDisplayList.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace ctr {

// コンストラクタ
CTRDisplayList::CTRDisplayList() :
  m_pBeginAddr(NULL),
  m_pEndAddr(NULL),
  m_JumpSize(0)
{
}

// デストラクタ
CTRDisplayList::~CTRDisplayList()
{
	
}

// 開始
void CTRDisplayList::Begin(u32* pBeginAddr)
{
  m_pBeginAddr = pBeginAddr;
}

// 終了
void CTRDisplayList::End(u32* pEndAddr, u32 jumpSize)
{
  m_pEndAddr = pEndAddr;
  m_JumpSize = jumpSize;
}

}}}
