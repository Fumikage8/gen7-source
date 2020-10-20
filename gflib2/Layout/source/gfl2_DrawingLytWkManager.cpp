//======================================================================
/**
 * @file gfl2_DrawingLytWkManager.cpp
 * @date 2015/08/03 15:47:18
 * @author kawada_koji
 * @brief 描画中レイアウトワークのマネージャ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <thread/include/gfl2_ThreadStatic.h>

#include <Layout/include/gfl2_Layout.h>
#include <Layout/include/gfl2_DrawingLytWkManager.h>




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)


DrawingLytWkManager::DrawingLytWkManager(gfl2::heap::HeapBase* heap, u32 lytWkNumMax)
  : m_lytWkNumMax(lytWkNumMax),
    m_currListIndex(0)
{
  GFL_ASSERT(heap!=NULL);     // 開発中に気付かせるためのASSERT
  GFL_ASSERT(lytWkNumMax>0);  // 開発中に気付かせるためのASSERT
  
  for(u32 listIndex=0; listIndex<LIST_NUM; ++listIndex)
  {
    m_lytWkNum[listIndex]  = 0;
    m_lytWkList[listIndex] = GFL_NEW_ARRAY(heap) LytWk*[m_lytWkNumMax];
    for(u32 elemIndex=0; elemIndex<m_lytWkNumMax; ++elemIndex)
    {
      m_lytWkList[listIndex][elemIndex] = NULL;
    }
  }
}
DrawingLytWkManager::~DrawingLytWkManager()
{
  for(u32 listIndex=0; listIndex<LIST_NUM; ++listIndex)
  {
    if(m_lytWkNum[listIndex] == 0)
    {
      GFL_SAFE_DELETE_ARRAY(m_lytWkList[listIndex]);
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。描画中のレイアウトが残っている。下記の通りケア済み。
      gfl2::thread::ThreadStatic::CurrentSleep(30);  // milli second  // ちょっと待つことでGPUを進めて、GPUハングの可能性を減らす。
      GFL_SAFE_DELETE_ARRAY(m_lytWkList[listIndex]);
    }
  }
}

void DrawingLytWkManager::SwapList(void)
{
  m_currListIndex = (m_currListIndex +1) % LIST_NUM;  // LIST_NUMは2固定。それ以外は想定していない。
  
  for(u32 elemIndex=0; elemIndex<m_lytWkNum[m_currListIndex]; ++elemIndex)
  {
    m_lytWkList[m_currListIndex][elemIndex]->DecreaseDrawingCount();
    m_lytWkList[m_currListIndex][elemIndex] = NULL;
  }
  m_lytWkNum[m_currListIndex] = 0;
}

void DrawingLytWkManager::AddLytWk(LytWk* lytWk)
{
  if(m_lytWkNum[m_currListIndex] < m_lytWkNumMax)
  {
    m_lytWkList[m_currListIndex][ m_lytWkNum[m_currListIndex] ] = lytWk;
    ++m_lytWkNum[m_currListIndex];
    lytWk->IncreaseDrawingCount();
  }
  else
  {
    GFL_ASSERT(0);  // 最大管理数を越えた。気付いてくれればケアなしでいい。
  }
}

void DrawingLytWkManager::RemoveLytWkForce(LytWk* lytWk)
{
  for(u32 listIndex=0; listIndex<LIST_NUM; ++listIndex)
  {
    u32 i = 0;
    while(i < m_lytWkNum[listIndex])
    {
      if(m_lytWkList[listIndex][i] == lytWk)
      {
        GFL_ASSERT_MSG(0, "描画中のレイアウトを強制的にリストから登録解除する LytWkのアドレス0x%p\n", lytWk);  // ASSERTを抜けたら登録解除しに行く。

        // ここに到達するということはm_lytWkNum[listIndex]は1以上。
        u32 j = i;
        while(j < m_lytWkNum[listIndex] -1)
        {
          m_lytWkList[listIndex][j] = m_lytWkList[listIndex][j] +1;
          ++j;
        }
        m_lytWkList[listIndex][j] = NULL;
        m_lytWkNum[listIndex] = m_lytWkNum[listIndex] -1;

        // iはインクリメントしなくていい。
      }
      else
      {
        ++i;
      }
    }
  }
}


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

