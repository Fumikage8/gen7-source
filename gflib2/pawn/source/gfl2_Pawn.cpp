//==============================================================================
/**
 * @file    gfl2_Pawn.cpp
 * @brief   要約説明
  
    このファイルの詳細説明
 * 
 * @author    N.Takeda
 * @date    2014/10/29(水) 11:13:47
 */
//==============================================================================

#pragma once

#include "pawn/include/gfl2_Pawn.h"
#include "util/include/gfl2_std_string.h"

namespace gfl2 {
namespace pawn {


//------------------------------------------------------------------------------
/**
 * @func    PawnNew
 * @brief   PawnNew関数.
 * @author    N.Takeda
 * @date    2014/10/29(水) 11:21:21
 *
 * @param[in] const s32 iSize   サイズ.
 * @param[in] const s32 iCategory カテゴリー.
 *
 * @return    u8* 
 */
//------------------------------------------------------------------------------
u8  *Pawn::PawnNew( const s32 iSize, const s32 iCategory )
{
#ifdef USE_GFLHEAP
  return GFL_NEW_ARRAY( m_pHeap ) u8 [ iSize ];
#else
  return new u8 [ iSize ];
#endif
}


//------------------------------------------------------------------------------
/**
 * @func    PawnDel
 * @brief   PawnDel関数.
 * @author    N.Takeda
 * @date    2014/10/29(水) 11:21:20
 *
 * @param[in,out] u8* pPtr  ポインタ.
 *
 * @return    なし (none)
 */
//------------------------------------------------------------------------------
void  Pawn::PawnDel( u8 *pPtr )
{
#ifdef USE_GFLHEAP
  GFL_DELETE_ARRAY( pPtr );
#else
  delete [] pPtr;
#endif
}


//------------------------------------------------------------------------------
/**
 * @func    PawnMemCpy
 * @brief   PawnMemCpy関数.
 * @author    N.Takeda
 * @date    2014/10/29(水) 11:21:17
 *
 * @param[in,out] void*   pD    .
 * @param[in]   const void* pS    .
 * @param[in]   const s32 iLength 長さ.
 *
 * @return    なし (none)
 */
//------------------------------------------------------------------------------
void  Pawn::PawnMemCpy( void *pD, const void *pS, const s32 iLength )
{
  gfl2::std::MemCopy( pS, pD, iLength );
}


//------------------------------------------------------------------------------
/**
 * @func    PawnMemFill
 * @brief   PawnMemFill関数.
 * @author    N.Takeda
 * @date    2014/10/29(水) 11:21:16
 *
 * @param[in,out] void*   pPtr  ポインタ.
 * @param[in]   const u8  _uVal .
 * @param[in]   const s32 iLength 長さ.
 *
 * @return    なし (none)
 */
//------------------------------------------------------------------------------
void  Pawn::PawnMemFill( void *pPtr, const u8 _uVal, const s32 iLength )
{
  gfl2::std::MemFill( pPtr, _uVal, iLength );
}

}
}
