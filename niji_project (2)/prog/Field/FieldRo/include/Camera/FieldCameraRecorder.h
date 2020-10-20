/**
* @brief  フィールドカメラ、レコード機能
* @file   FieldCameraRecorder.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERARECORDERER_H_INCLUDED__ )
#define __FIELDCAMERARECORDERER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>


// 前方宣言


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

/**
* @brief フィールドカメラ、1制御単位クラス
*/
class CameraRecorder
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( CameraRecorder );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
  };

  struct RecType
  {
    enum Value
    {
      FOR_MAYA = 0,
      MAX
    };
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  CameraRecorder( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~CameraRecorder( void );
  
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
  * @brief 記録対象を設定
  *
  * @param pTarget 記録対象
  *
  * @return 無し
  */
  void SetTarget( const poke_3d::model::BaseCamera* pTarget );
  void ClearTarget( void );

  /**
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( f32 bank = 0.0f );

  /**
  * @brief 記録開始
  *
  * @param  type        記録型式
  * @param  pFileName   保存ファイル名(拡張子含まず)
  *
  * @return 無し
  */
  void Start( RecType::Value type, const c8* pFileName );

  /**
  * @brief 記録停止
  *
  * @return 無し
  */
  void End( void );

  /**
  * @brief 記録中か取得
  *
  * @return true 記録中、 false 記録していない
  */
  b32 IsRecording( void ) const;


private:


  //
  // 制御
  //
  u32                         m_CurrentFrame;                                 // 現在のフレーム
  b32                         m_IsRecording;                                  // 記録中ならtrue
  RecType::Value              m_RecodingType;                                 // 記録型式
  
  //
  // 内部オブジェクト
  //
  class Impl;
  Impl*                       m_pImpl;

  //
  // 外部オブジェクト
  //
  const poke_3d::model::BaseCamera* m_pTargetCamera;                                  // 記録対象
  gfl2::heap::HeapBase*       m_pSystemHeap;

};  // class FieldCameraRecorder


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERARECORDERER_H_INCLUDED__