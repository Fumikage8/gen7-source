//======================================================================
/**
* @file FieldCameraAreaSubControllerBase.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリアの子操作インターフェイス
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREASUBCONTROLLERERBASE_H_INCLUDED__
#define __FIELDCAMERAAREASUBCONTROLLERERBASE_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>


// Field
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

class AreaDataManager;

GFL_NAMESPACE_BEGIN( SubController );

/**
* @brief フィールドカメラエリアの子操作インターフェイス
*/
class SubControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( SubControllerBase );

public:

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  SubControllerBase( void ) {
#if PM_DEBUG
    m_DebugControlFlag = false;
#endif
  }

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~SubControllerBase( void ) {}

  /**
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Update( poke_3d::model::BaseCamera* pCamera ) = 0;

  /**
  * @brief 子操作のタイプを取得
  *
  * @return 子操作のタイプ定義
  */
  virtual Type::Value GetType( void ) const = 0;

#if PM_DEBUG

  /**
  * @brief　補間中か取得する
  *
  * @return true 補間中 false 補間中じゃない
  */
  virtual b32 IsIpo( void ) const { return false; };

  /**
  * @brief　デバッグ操作の影響があるか取得する
  *
  * @return true ある false ない
  */
  virtual b32 IsDebugEffect( void ) const { return false; };

  /**
  * @brief　カメラ情報を出力する必要があるか取得する
  *
  * @return true ある false ない
  */
  virtual b32 IsPutsInfoFile( void ) const { return false; };

  /**
  * @brief デバッグ処理(Updateで呼ばれる)
  *
  * @param  pCamera           操作するカメラの実体
  * @param  pAreaDataManager  エリア設定データ
  *
  * @return 無し
  */
  virtual void UpdateDebugDraw( poke_3d::model::BaseCamera* pCamera, const Area::AreaDataManager* pAreaDataManager ){};

  /**
  * @brief カメラ情報を出力
  *
  * @param  pCamera           操作するカメラの実体
  * @param  pAreaDataManager  エリア設定データ
  *
  * @return 無し
  */
  virtual void PutsInfoFile( poke_3d::model::BaseCamera* pCamera, const Area::AreaDataManager* pAreaDataManager ){};


  void DebugSetOrijinalData( const Area::CAMERA_AREA_SETTING_DATA& rData )
  {
    m_DebugOrijinalData = rData;
  }

  void SetDebugControlFlag( b32 flag ) { m_DebugControlFlag = flag; }
  b32 GetDebugControlFlag( void ) const { return m_DebugControlFlag; }

  Area::CAMERA_AREA_SETTING_DATA m_DebugOrijinalData;

#endif // #if PM_DEBUG

private:

#if PM_DEBUG
  b32 m_DebugControlFlag;
#endif // #if PM_DEBUG

};

GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERAAREASUBCONTROLLERERBASE_H_INCLUDED__