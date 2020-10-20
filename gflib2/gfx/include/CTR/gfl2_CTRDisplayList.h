#ifndef GFLIB2_GRX_CTR_CTRDISPLAYLIST_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRDISPLAYLIST_H_INCLUDED

#include <gfx/include/gfl2_DisplayList.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRDisplayList : public DisplayList
{
public:

  // コンストラクタ
	CTRDisplayList();

  // デストラクタ
	virtual ~CTRDisplayList();

  // 開始
  void Begin(u32* pBeginAddr);

  // 終了
  void End(u32* pEndAddr, u32 jumpSize);

  u32* GetBeginAddr()
  {
    return m_pBeginAddr;
  }
  u32 GetBufferSize()
  {
    return (m_JumpSize == 0)
      ? (uptr)m_pEndAddr - (uptr)m_pBeginAddr // ジャンプしないなら全コマンド分
      : m_JumpSize;                           // ジャンプするなら最初のジャンプまでのサイズ
  }

private:

  u32* m_pBeginAddr;
  u32* m_pEndAddr;
  u32 m_JumpSize;
};

}}}

#endif
