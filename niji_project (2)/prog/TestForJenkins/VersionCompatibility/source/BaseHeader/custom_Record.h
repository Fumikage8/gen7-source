#if !defined(CUSTOM_RECORD_H_INCLUDED)
#define CUSTOM_RECORD_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   custom_Record.h
 * @date   2016/11/09 Wed. 11:42:49
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================


namespace Savedata  {
// =============================================================================
/**/
//==============================================================================

/*
  VersionCompatibility用に再定義
*/
class Record
{
public:
#include "./needRedefine/savedata/RecordDefine.h"
};



} /*  namespace Savedata  */
#endif  /*  #if !defined(CUSTOM_RECORD_H_INCLUDED)  */
