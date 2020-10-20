//==============================================================================
/**
 * @file        PawnObject.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/02/27(金) 15:59:38
 */
//==============================================================================

#include "Pawn/include/PawnObject.h"
#include <math/include/gfl2_math_random.h>
#include <fs/include/gfl2_Fs.h>
#include "System/include/GflUse.h"

GFL_NAMESPACE_BEGIN( PawnScript )

//------------------------------------------------------------------------------
/**
 * @func        Random
 * @brief       Random関数.
 *
 * @param[in,out] AMX*            未使用.
 * @param[in]     const cell* val .
 *
 * @return      cell  
 */
//------------------------------------------------------------------------------
cell PawnObject::Random( AMX *, const cell *val )
{
  return System::GflUse::GetPublicRand( val[1] );
}

//------------------------------------------------------------------------------
/**
 * @func        Load
 * @brief       読み込み関数.
 *
 * @param[in] const void*       buffer  バッファ.
 * @param[in] const size_t      size    サイズ.
 * @param[in] const c8 * const  amxname 名.
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void PawnObject::Load( const void* buffer, const size_t size, const c8 * const amxname )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] = {
    GFL_PAWN_FUNC( Random )
    GFL_PAWN_FUNC_END
  };
  Pawn::Load( buffer, size, amxname );
  RegisterPawnFunction( s_FuncTable );
}

GFL_NAMESPACE_END( PawnScript )
