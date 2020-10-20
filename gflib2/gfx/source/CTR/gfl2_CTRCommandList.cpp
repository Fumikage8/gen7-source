#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRCommandList.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRCommandList::CTRCommandList()
{
}

CTRCommandList::~CTRCommandList()
{
}

void CTRCommandList::DebugDumpConsole()
{
  NN_LOG("------- CommandDump Begin Main\n");

  u32 requestCount = this->GetUsedRequestCount();
  u32* pTopAddr = this->GetTopBufferAddr();
  u32* pEndAddr = this->GetCurrentBufferAddr();

  NN_LOG("pCurrent %p\n", pEndAddr);
  NN_LOG("pCurrentP %p\n", nngxGetPhysicalAddr((uptr)pEndAddr));

  NN_LOG("----- request count %u\n\n", requestCount);
  u32* pReqAddr = this->GetTopRequestAddr();
  for (u32 i = 0; i < requestCount; ++i)
  {
    for (u32 j = 0; j < 7; ++j)
    {
      NN_LOG("0x%08X\n", *pReqAddr);
      ++pReqAddr;
    }

    NN_LOG("\n");
  }
  NN_LOG("----- request end\n");

  while (pTopAddr != pEndAddr)
  {
    NN_LOG("0x%08X\n", *pTopAddr);

    ++pTopAddr;
  }
  NN_LOG("------- CommandDump End Main\n");
}

void CTRCommandList::Add3DCommandNoCacheFlush(const void* pBuffer, u32 bufferSize)
{
  if ( bufferSize == 0 )
  {//空の描画パスから実行されることもある。
    return;
  }

  GFL_ASSERT_MSG( (bufferSize % 16) == 0, "CommandBufferSize is not 16Byte Align Size." );

  nngxAdd3DCommandNoCacheFlushRaw(&m_CommandList, pBuffer, bufferSize);
}

}}}
