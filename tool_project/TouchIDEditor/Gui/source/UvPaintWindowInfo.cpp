#include "../include/UvPaintWindowInfo.h"
#include "../include/GLUtil.h"
#include <string>
#include <vector>

namespace Gui { 

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  UvPaintWindowInfo::UvPaintWindowInfo( QMainWindow *pParent, gfl::cmdl::Model *pModel, gfl::cmdl::Image *pImageData, bool load_id ) :
    m_pParent(pParent), m_pModel(pModel), m_pImageData(pImageData), m_LoadId(load_id)
  {
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  gfl::cmdl::Material::TextureCoordinator* UvPaintWindowInfo::GetTextureCoordinator()
  {
    if(m_LoadId){
      return 0;
    }

    gfl::cmdl::Material* pMaterial = m_pModel->GetMaterial();

    for( int i = 0; i < m_pModel->GetMaterialCount(); ++i )
    {
      gfl::cmdl::Material::TextureCoordinator *pTextureCoordinator = pMaterial[i].GetTextureCoordinator();
      gfl::cmdl::Material::TextureMapper *pTextureMapper = pMaterial[i].GetTextureMapper();

      for ( int i2 = 0; i2 < pMaterial[i].GetTextureCoordinatorCount(); ++i2 )
      {
        if ( m_pImageData->GetFileName() == pTextureMapper[i2].GetFileName() )
        {
          return &pTextureCoordinator[i2];
        }
      }
    }

    return NULL;
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  gfl::cmdl::Material::TextureMapper*	UvPaintWindowInfo::GetTextureMapper()
  {
    if(m_LoadId)
    {
      return 0;
    }

    gfl::cmdl::Material* pMaterial = m_pModel->GetMaterial();

    for( int i = 0; i < m_pModel->GetMaterialCount(); ++i )
    {
      gfl::cmdl::Material::TextureCoordinator *pTextureCoordinator = pMaterial[i].GetTextureCoordinator();
      gfl::cmdl::Material::TextureMapper *pTextureMapper = pMaterial[i].GetTextureMapper();

      for ( int i2 = 0; i2 < pMaterial[i].GetTextureCoordinatorCount(); ++i2 )
      {
        if ( m_pImageData->GetFileName() == pTextureMapper[i2].GetFileName() )
        {
          return &pTextureMapper[i2];
        }
      }
    }
    return NULL;
  }

}