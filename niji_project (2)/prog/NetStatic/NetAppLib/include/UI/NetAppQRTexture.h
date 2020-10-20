#if !defined(NETAPPQRTEXTURE_H_INCLUDED)
#define NETAPPQRTEXTURE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppQRTexture.h
 * @date   2015/10/16 Fri. 15:30:54
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
#include  <Layout/include/gfl2_Layout.h>



// =============================================================================
/**/
//==============================================================================
namespace gfl2  {
namespace qr    {
  class QRRGBImageInfo;
}
}


namespace NetAppLib {
  namespace System  {class  ApplicationWorkBase;}
namespace UI        {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/16 Fri. 15:33:52
  */
//==============================================================================
class NetAppQRTexture
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppQRTexture);
public:
  NetAppQRTexture(System::ApplicationWorkBase* pWorkBase);
  virtual ~NetAppQRTexture();

  void  Build(const gfl2::qr::QRRGBImageInfo* pQRImageInfo);
  void  Release(void);
  void  AssignTextureToPicturePane(gfl2::lyt::LytPicture* pPicture);

  bool  IsBuilt(void) const {return (m_pTextureBuffer != NULL);}

protected:
  System::ApplicationWorkBase*  m_pWorkBase;
  u8*                           m_pTextureBuffer;
  gfl2::lyt::LytResTextureInfo  m_TextureInfo;
};


} /*  namespace UI        */
} /*  namespace NetAppLIb */
#endif  /*  #if !defined(NETAPPQRTEXTURE_H_INCLUDED)  */
