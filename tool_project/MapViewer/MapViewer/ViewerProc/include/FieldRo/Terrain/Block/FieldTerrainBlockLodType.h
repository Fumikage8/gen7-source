/**
* @brief  �t�B�[���h�n�`�u���b�N��LOD���ʎq��`
* @file   FieldTerrainBlockLodType.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINLODTYPE_H_INCLUDED__ )
#define __FIELDTERRAINLODTYPE_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );
GFL_NAMESPACE_BEGIN( Block );

/** @brief  �n�`�u���b�N��LOD���ʎq */
struct LodType
{
  enum Value
  {
    HIGH = 0,
    MIDDLE,
    LOW,
    MAX
  };
};

GFL_NAMESPACE_END( Block );
GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINLODTYPE_H_INCLUDED__