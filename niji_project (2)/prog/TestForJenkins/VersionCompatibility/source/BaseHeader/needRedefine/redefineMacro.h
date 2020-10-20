#if !defined(REDEFINEMACRO_H_INCLUDED)
#define REDEFINEMACRO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   redefineMacro.h
 * @date   2016/10/28 Fri. 19:41:05
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
#if !defined TO_ENUM
  #define TO_ENUM(_macro)     namespace TestForJenkins{ namespace VersionCompatibility{ namespace NijiID{enum{enum_##_macro = _macro}; }}}
#endif


#endif  /*  #if !defined(REDEFINEMACRO_H_INCLUDED)  */
