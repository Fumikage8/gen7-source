/**
* @brief  フィールドシステム(実体)
* @file   FieldSystem.h
* @author ikuta_junya
* @data   2015.02.07
*/

#if !defined( __FIELDSYSTEM_H_INCLUDED__ )
#define __FIELDSYSTEM_H_INCLUDED__ 
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// 前方宣言
GFL_NAMESPACE_BEGIN( fld );
GFL_NAMESPACE_BEGIN( mmodel );
class MoveModelManager;
GFL_NAMESPACE_END( mmodel );
GFL_NAMESPACE_END( fld );

GFL_NAMESPACE_BEGIN( Field );

// 前方宣言
GFL_NAMESPACE_BEGIN( Camera );
class FieldCamera;
GFL_NAMESPACE_END( Camera );

/**
* @brief フィールドシステム(実体)
*/
class FieldSystem
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldSystem );


public:

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  FieldSystem( void )
  :m_pFieldCamera(NULL)
  ,m_pMoveModelManager(NULL)
  {};

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~FieldSystem( void ){};


  /**
  * @brief 現在使用されているカメラを取得する
  *
  * @return 現在使用されているカメラ
  */
  Camera::FieldCamera* GetActiveCamera( void )
  {
    return m_pFieldCamera;
  }

  /**
  * @brief 自機に追従しているカメラを取得する
  *
  * @return 機に追従しているカメラ
  */
  Camera::FieldCamera* GetPlayerCamera( void )
  {
    return m_pFieldCamera;
  }

  /**
  * @brief 動作モデルのマネージャーを取得
  *
  * @return 動作モデルのマネージャー
  */
  fld::mmodel::MoveModelManager* GetMoveModelManager( void )
  {
    return m_pMoveModelManager;
  }

  /**
  * @brief 動作モデルのマネージャーを設定
  *
  * @param 動作モデルのマネージャー
  */
  void SetMoveModelManager( fld::mmodel::MoveModelManager* pMoveModelManager )
  {
    m_pMoveModelManager = pMoveModelManager;
  }

  Camera::FieldCamera*            m_pFieldCamera; 
  fld::mmodel::MoveModelManager*  m_pMoveModelManager;

};  // class FieldSystem

GFL_NAMESPACE_END( Field );


#endif // __FIELDSYSTEM_H_INCLUDED__