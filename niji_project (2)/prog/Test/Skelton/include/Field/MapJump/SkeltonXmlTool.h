#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
 @file xml_tool.h
 @brief xmlパーサーの便利関数など
 @date 2015.01.22
 @author saita_kazuki
 */
//*************************************************************************************************

#ifndef __SKELTON_MAPJUMP_XMLTOOL_H__
#define __SKELTON_MAPJUMP_XMLTOOL_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <xml/include/rapidxml.hpp> // no need to link

#include <math/include/gfl2_Vector4.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

// 下の例のようなxmlのアトリビュートからVectorの値を取る場合はこちら
// <position x="-600.0" y="300.0" z="200.0" />>
static b32 GetVectorValue_Attr( const rapidxml::xml_node<>* pVectorNode, gfl2::math::Vector4* pOut )
{
  pOut->x = static_cast<f32>( atof( pVectorNode->first_attribute("x")->value() ) );
  pOut->y = static_cast<f32>( atof( pVectorNode->first_attribute("y")->value() ) );
  pOut->z = static_cast<f32>( atof( pVectorNode->first_attribute("z")->value() ) );
  return true;
}

// 下の例のようなxmlのノードからVectorの値を取る場合はこちら
// <position>
//   <x>100</x>
//   .
//   .
// </position>
static b32 GetVectorValue_Node( const rapidxml::xml_node<>* pVectorNode, gfl2::math::Vector4* pOut )
{

  pOut->x = static_cast<f32>( atof( pVectorNode->first_node( "x")->value() ) );
  pOut->y = static_cast<f32>( atof( pVectorNode->first_node( "y")->value() ) );
  pOut->z = static_cast<f32>( atof( pVectorNode->first_node( "z")->value() ) );
  return true;
}

// 下の例のようなxmlのノードからVectorの値を取る場合はこちら
// <position>
//   <x>100</x>
//   .
//   .
// </position>
static b32 GetVectorValueXYZW_Node( const rapidxml::xml_node<>* pVectorNode, gfl2::math::Vector4* pOut )
{

  pOut->x = static_cast<f32>( atof( pVectorNode->first_node( "x")->value() ) );
  pOut->y = static_cast<f32>( atof( pVectorNode->first_node( "y")->value() ) );
  pOut->z = static_cast<f32>( atof( pVectorNode->first_node( "z")->value() ) );
  pOut->w = static_cast<f32>( atof( pVectorNode->first_node( "w")->value() ) );
  return true;
}

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)


#endif // __SKELTON_MAPJUMP_XMLTOOL_H__
#endif // GF_PLATFORM_WIN32