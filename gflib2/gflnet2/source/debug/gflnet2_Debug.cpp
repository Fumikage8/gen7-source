#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_Debug.cpp
 * @brief	 通信基底クラス
 * @author k.ohno
 * @date	 2011.8.25
 */
//=============================================================================

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <gflnet2/include/debug/gflnet2_Debug.h>

#if GFL_DEBUG
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)

//------------------------------------------------------------------
/**
 * @brief	  ダンプ
 * @param   pData  データ
 * @param   pMsg   メッセージ
 * @return  成功したらtrue
 */
//------------------------------------------------------------------

void Debug::Dump(const u8* pData,const u8* pMsg)
{
  //const u8* x = pData;
  //static const int by=10;
  
  //OHNO_PRINT("%s\n",pMsg);
  //for(int b=0;b<10;b++){
    //OHNO_PRINT(" %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n",
    //           x[b*by],x[b*by+1],x[b*by+2],x[b*by+3],x[b*by+4],
    //           x[b*by+5],x[b*by+6],x[b*by+7],x[b*by+8],x[b*by+9]
    //           );
  //}
}


GFL_NAMESPACE_END(gflnet2)


#endif//GFL_DEBUG
#endif
