#if !defined(IRESOURCEPROVIDER_H_INCLUDED)
#define IRESOURCEPROVIDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   IResourceProvider.h
 * @date   2015/09/30 Wed. 16:26:21
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
  *  @brief  arcID/arcdatIDに対応するデータバイナリを提供するI/F
  */
//==============================================================================
class IResourceProvider
{
public:
  virtual ~IResourceProvider(){}
  virtual void*  IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID) = 0;
};




} /*  namespace tool  */
} /*  namespace app   */
#endif  /*  #if !defined(IRESOURCEPROVIDER_H_INCLUDED)  */
