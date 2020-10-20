//======================================================================
/**
* @file FieldCameraAreaSubControllerTypeNecrozma.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリアの子操作( ネクロズママップ用 )
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREASUBCONTROLLERERTYPENECROZMA_H_INCLUDED__
#define __FIELDCAMERAAREASUBCONTROLLERERTYPENECROZMA_H_INCLUDED__
#pragma once

// gfl2
#include <math/include/gfl2_SpringSimulation.h>

// Field
#include "FieldCameraAreaSubControllerBase.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

class Support;
class Focus;

GFL_NAMESPACE_BEGIN( SubController );

/**
* @brief フィールドカメラエリアの子操作( 現在位置から2つのパラメータが補間する追従型 )
*/
class SubControllerTypeNecrozma : public SubControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( SubControllerTypeNecrozma );

public:

  struct DATA
  {
    MAIN_PARAM                    paramA;         // A地点(0.0)のパラメータ
    MAIN_PARAM                    paramB;         // B地点(1.0)のパラメータ
    gfl2::math::Easing::EaseFunc  paramEaseFunc;  // パラメータ補間時に使用するイージング関数



  };

  struct POINT_DATA
  {
    MAIN_PARAM          param;
    f32                 height;
  };


  // 指定の高さ以下において、螺旋中心点からの角度の進行度をもとめ、エントランスと螺旋のパラメーターを補間する
  struct ENTRANCE_CONNECTION_DATA
  {
    MAIN_PARAM          param;              // エントランス部分の想定パラメーター

    f32                 effectiveHeight;    // この高さ以下なら有効
    f32                 startDirection;     // 開始角度
    f32                 endDirection;       // 終了角度
  };

  struct SETTING
  {
    POINT_DATA start;
    POINT_DATA end;
    gfl2::math::Easing::EaseFunc  easingFunc;
    s32                           interpolationFrame;
    
    ENTRANCE_CONNECTION_DATA      entrance;
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  SubControllerTypeNecrozma( const Target::TargetBase* pTarget, const Area::Interpolation* pIpo );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~SubControllerTypeNecrozma( void );

  /**
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Update( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief 子操作のタイプを取得
  *
  * @return 子操作のタイプ定義
  */
  virtual Type::Value GetType( void ) const
  {
    return Type::FOLLOW_IPO;
  }

  /**
  * @brief データを取得
  *
  * @return なし
  */
  void GetData( DATA* pOut ) const
  {
    *pOut = m_Data;
  }

  /**
  * @brief　他のカメラからネクロズマカメラへの遷移フレームを取得します。
  *
  * @return 遷移フレーム
  */
  static u32 GetInterpolationFrame(void) { return m_Setting.interpolationFrame; }

#if PM_DEBUG

  /**
  * @brief　デバッグ操作の影響があるか取得する
  *
  * @return true ある false ない
  */
  virtual b32 IsDebugEffect( void ) const { return m_IsDebugEffect; }
  
    /**
  * @brief　カメラ情報を出力する必要があるか取得する
  *
  * @return true ある false ない
  */
  virtual b32 IsPutsInfoFile( void ) const { return m_IsPutsInfoFile; };

  /**
  * @brief　設定パラメータへポインタを取得(デバッグメニュー用)
  *
  * @return ポインタ
  */
  static SETTING* GetSettingPtr( void ) { return &m_Setting; }

#endif // #if PM_DEBUG

private:

  /**
  * @brief　高さから適用する設定を取得します
  *
  * @param  height　高さ
  * @param  pOut 結果を格納するバッファです
  *
  * @return 無し
  */
  void GetDataByHeight(f32 height, MAIN_PARAM* pOut);

  static SETTING m_Setting;           // 設定パラメーター

  DATA m_Data;

#if PM_DEBUG
  b32  m_IsDebugEffect;
  b32  m_IsPrevDebugUpdate;
  b32  m_IsPutsInfoFile;
#endif

  // 外部オブジェクト
  const Target::TargetBase* m_pTarget;                            // 注視対象
  const f32*                            m_pParamRatio;
  const Area::Interpolation*            m_pIpo;
};

GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERAAREASUBCONTROLLERERTYPENECROZMA_H_INCLUDED__
