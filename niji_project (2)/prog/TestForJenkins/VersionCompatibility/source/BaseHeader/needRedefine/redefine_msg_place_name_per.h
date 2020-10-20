#if !defined(REDEFINE_MSG_PLACE_NAME_PER_H_INCLUDED)
#define REDEFINE_MSG_PLACE_NAME_PER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   redefine_msg_place_name_per.h
 * @date   2016/11/09 Wed. 15:41:42
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

#include  "./message/msg_place_name_per.h"

#include  "./redefineMacro.h"


TO_ENUM(MAPNAME_TOOKUNIIRUHITO);
TO_ENUM(MAPNAME_SODATEYA);
TO_ENUM(MAPNAME_TOREZYAHANTA);
TO_ENUM(MAPNAME_ONSENBAASAN);

TO_ENUM(msg_place_name_per_max);


#undef  MAPNAME_TOOKUNIIRUHITO
#undef  MAPNAME_SODATEYA
#undef  MAPNAME_TOREZYAHANTA
#undef  MAPNAME_ONSENBAASAN

#undef  msg_place_name_per_max



/*  break include-guard  */
#undef  __MSG_PLACE_NAME_PER_H__


#endif  /*  #if !defined(REDEFINE_MSG_PLACE_NAME_PER_H_INCLUDED)  */
