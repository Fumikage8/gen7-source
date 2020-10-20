/**
* @brief  フィールドカメラの操作、イベントスクリプト用
* @file   FieldCameraControllerTypeEventScript.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERASHAKECONTROLLER_H_INCLUDED__ )
#define __FIELDCAMERASHAKECONTROLLER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

class ShakeController
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ShakeController );

public:

  ShakeController();
  virtual ~ShakeController();

  // システム公開関数
  void Set( poke_3d::model::BaseCamera* pCamera );
  void Clear( void );
  void Update( void );

  // 使用者公開関数
  void Begin( gfl2::renderingengine::scenegraph::resource::ResourceNode* pAnime, b32 loopFlag, f32 stepFrame, f32 scale );
  void End( u32 fadeFrame = 0 );
  b32 IsShake( void ) const;

private:
  void EndImpl( void );


  // 内部変数
  b32                         m_IsShake;
  f32                         m_Scale;
  
  // 内部オブジェクト
  Area::ValueController       m_EndFadeValue;

  // 外部オブジェクト
  poke_3d::model::BaseCamera* m_pCamera;
};


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERASHAKECONTROLLER_H_INCLUDED__

  