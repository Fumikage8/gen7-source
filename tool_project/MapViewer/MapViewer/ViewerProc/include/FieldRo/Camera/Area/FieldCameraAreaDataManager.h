/**
* @brief  フィールドカメラ、カメラエリアデータ管理
* @file   FieldCameraAreaDataManager.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__ )
#define __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Easing.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );


struct AreaNo
{
  enum Value
  {
    DEFAULT = 0xFFFF,       // デフォルト設定データ 
    INVALID = 0xFFFE,       // 無効設定データ 
  };
};


struct BIN_DATA;

struct DataType
{
  enum Value
  {
    FOLLOW = 0,               // 通常追従型
    FOLLOW_TRANS,             // 2つのパラメータ間を遷移する追従型
    HOLD,
    INHERIT,
    PATH,
    MAX,
    INVALID = 255,
  };
};


struct POINT_DATA
{
  gfl2::math::Vector3 offset;
  gfl2::math::Vector3 rotation;
  f32                 fovY;
  f32                 distance;
};


// @todo 仮
struct DATA
{
  DataType::Value               dataType;
  u32                           interpolationFrame;
  gfl2::math::Easing::EaseFunc  easeFunc;

  POINT_DATA          pointA;
  POINT_DATA          pointB;
};

/**
* @brief フィールドカメラの操作、カメラエリア用
*/
class AreaDataManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( AreaDataManager );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    void* m_pData;
    // const gfl2::fs::BinLinkerAccessor* m_BinData;
  };

  /**
  * @brief コンストラクタ
  *
  * @param rInitDesc  初期化設定
  *
  * @return 無し
  */ 
  AreaDataManager( const InitializeDescription& rInitDesc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~AreaDataManager( void );
  
  /**
  * @brief エリアのデフォルト設定を取得
  *
  * @return 成否(falseはありえない)
  */
  b32 GetDefaultData( DATA* pOut ) const;


  // @todo 仮
  b32 GetData( DATA* pOut, s32 areaId, s32 prevAreaId = 1 ) const;


  s32 GetPriority( s32 areaId ) const;

  void Lerp( POINT_DATA* pOut, const POINT_DATA& r1, const POINT_DATA& r2, f32 t ) const;


private:

  //
  // 内部制御
  //

  POINT_DATA                        m_DefaultData;


  //
  // 外部オブジェクト
  //
  BIN_DATA*                         m_pBinData;

};  // class AreaDataManager


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__

  