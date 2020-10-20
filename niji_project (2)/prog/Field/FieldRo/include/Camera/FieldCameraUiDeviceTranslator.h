/**
* @brief  入力情報をカメラを考慮した形に変換する機能
* @file   FieldCameraUiDeviceTranslator.h
* @author ikuta_junya
* @data   2015.11.20
*/

#if !defined( __FIELDCAMERAUIDEVICETRANSLATOR_H_INCLUDED__ )
#define __FIELDCAMERAUIDEVICETRANSLATOR_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
// #include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Matrix34.h>

#include <util/include/gfl2_List.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// field


GFL_NAMESPACE_BEGIN( Field );

class Fieldmap;

GFL_NAMESPACE_BEGIN( Camera );

class CameraManager;
class Stack;

/**
* @brief 入力情報をカメラを考慮した形に変換する機能
*/
class UiDeviceTranslator
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( UiDeviceTranslator );

public:

  struct Mode
  {
    enum Value
    {
      NORMAL = 0,
      DIRECT        // 即時反映
    };
  };


  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    const Camera::CameraManager*  m_pCameraManager;
    // const Fieldmap*               m_pFieldmap;        // for GetPlayer
    Fieldmap*                     m_pFieldmap;        // for GetPlayer
    gfl2::heap::HeapBase*         m_pSystemHeap;
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  UiDeviceTranslator( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~UiDeviceTranslator( void );
  
  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );

  /**
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( void );

  /**
  * @brief フィールドの準備が完了した際に実行される関数
  *
  * @return 無し
  */
  void Ready( void );
  
  /**
  * @brief 型式を変更
  *
  * @param  mode  型式
  *
  * @return 無し
  */
  void SetMode( Mode::Value mode );


  /**
  * @brief X入力量を取得
  *
  * @return 入力量( -1.0f ～ 1.0f )
  */
  f32 GetX( void ) const;

  /**
  * @brief Y入力量を取得
  *
  * @return 入力量( -1.0f ～ 1.0f )
  */
  f32 GetY( void ) const;
  

private:

  /**
  * @brief 入力変換に使うカメラ角を更新するかチェック
  *
  * @param  rOriStick スティック入力値(無加工)
  *
  * @return true 更新する false 更新しない
  */
  b32 CameraAngleCheckUpdate( const gfl2::math::Vector2& rOriStick );

  //
  // 制御
  //
  Mode::Value             m_Mode;
  f32                     m_ViewRotationY;
  f32                     m_X;
  f32                     m_Y;
  b32                     m_ReadyFlag;      // 準備完了ならtrue


  //
  // 内部オブジェクト
  //
  gfl2::util::List<gfl2::math::Vector2>*     m_pStickLog;

  //
  // 外部オブジェクト
  //
  const Camera::CameraManager*  m_pCameraManager;
  // const Fieldmap*                     m_pFieldmap;        // for GetPlayer
  Fieldmap*                     m_pFieldmap;        // for GetPlayer

  //
  // デバッグ用
  //

};  // class UiDeviceTranslator


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAUIDEVICETRANSLATOR_H_INCLUDED__