//==============================================================================
/**
 * @file   redefines.h
 * @date   2016/10/28 Fri. 17:14:09
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

#define TO_ENUM(_macro)     namespace TestForJenkins{ namespace VersionCompatibility{ namespace NijiID{enum{enum_##_macro = _macro}; }}}
  #include  "./needRedefine/redefine_battle_dressup.h"
  #include  "./needRedefine/redefine_field_dressup.h"
  #include  "./needRedefine/redefine_msg_jf_phrase.h"
#undef  TO_ENUM



