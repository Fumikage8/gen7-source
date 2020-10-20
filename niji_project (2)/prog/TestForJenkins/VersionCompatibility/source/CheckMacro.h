#if !defined(CHECKMACRO_H_INCLUDED)
#define CHECKMACRO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   CheckMacro.h
 * @date   2016/10/28 Fri. 19:44:50
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#define _SHOWVAL(val) #val
#define SHOWVAL(val)  #val " = " _SHOWVAL(val)


/**  不一致でエラー  */
#define IS_COMPATIBLE_CC(_symbol)                     do{STATIC_ASSERT((_symbol) == (NijiID::_symbol));}while(0)        /**<  コンパイラシンボル用チェック      */
#define IS_COMPATIBLE_PP(_macro)                      do{STATIC_ASSERT((_macro)  == (NIJIID__##_macro));}while(0)       /**<  プリプロセッサシンボル用チェック  */
#define IS_COMPATIBLE_REDEF(_macro)                   do{STATIC_ASSERT((_macro)  == (NijiID::enum_##_macro));}while(0)

#define IS_COMPATIBLE_STRUCT_SIZE(_struct)            do{STATIC_ASSERT(sizeof(_struct) == sizeof(NijiID::_struct));}while(0)
#define IS_COMPATIBLE_STRUCT_MEMBER(_struct, _member) do{STATIC_ASSERT(offsetof(_struct, _member) == offsetof(NijiID::_struct, _member));}while(0)


#endif  /*  #if !defined(CHECKMACRO_H_INCLUDED)  */
