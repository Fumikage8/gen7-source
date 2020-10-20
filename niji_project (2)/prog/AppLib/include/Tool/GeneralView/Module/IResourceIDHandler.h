#if !defined(IRESOURCEIDHANDLER_H_INCLUDED)
#define IRESOURCEIDHANDLER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   IResourceIDHandler.h
 * @date   2015/09/30 Wed. 16:22:19
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>


namespace app   {
namespace tool  {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @brief  リソースID取得ハンドラ
  *  @date   2015/09/30 Wed. 16:27:15
  */
//==============================================================================
class IResourceIDHandler
{
public:
  virtual ~IResourceIDHandler(){}
  virtual void  IResourceIDHandler_NotifyResourceID(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID, const bool isCompressed) = 0;
};


} /*  namespace tool  */
} /*  namespace app   */
#endif  /*  #if !defined(IRESOURCEIDHANDLER_H_INCLUDED)  */
