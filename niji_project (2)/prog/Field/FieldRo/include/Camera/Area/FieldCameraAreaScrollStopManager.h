/**
* @brief  フィールドカメラ、スクロールストップ管理
* @file   FieldCameraScrollStopManager.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERASCROLLSTOPMANAGER_H_INCLUDED__ )
#define __FIELDCAMERASCROLLSTOPMANAGER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Vector3.h>


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

class ScrollStopDataAccessor;

/**
* @brief フィールドカメラのスクロールストップデータを管理し、スクロールストップ処理を実行する機能
*/
class ScrollStopManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ScrollStopManager );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    void* m_pData;
    gfl2::heap::HeapBase* m_pHeap;
  };

  /**
  * @brief コンストラクタ
  *
  * @param rInitDesc  初期化設定
  *
  * @return 無し
  */ 
  ScrollStopManager( const InitializeDescription& rInitDesc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ScrollStopManager( void );

  /**
  * @brief スクロールストップ処理を実行する
  *
  * @param  rSrc  加工前の座標
  * @param  pDist 加工結果の格納先
  *
  * @return 実際に加工されていたらtrue
  */
  b32 Execute( const gfl2::math::Vector3& rSrc, gfl2::math::Vector3* pDist ) const;
  
  /**
  * @brief INデータの無効化フラグを設定します
  *
  * @param  data_index データのindex値
  * @param  flag       設定するフラグ
  *
  * @return なし
  */
  void SetInDataInvalidFlag( u32 data_index, b32 flag );
   
private:

  /**
  * @brief スクロールストップ処理を実行する
  *
  * @param  type  データタイプ
  * @param  rSrc  加工前の座標
  * @param  pDist 加工結果の格納先
  *
  * @return 実際に加工されていたらtrue
  */
  b32 ExecuteImpl( s32 type, const gfl2::math::Vector3& rSrc, gfl2::math::Vector3* pDist ) const;

  //
  // 内部制御
  //
  ScrollStopDataAccessor*             m_pScrollStopDataAccessor;
  b32*                                m_pInDataInvalidFlagTable;
  //
  // 外部オブジェクト
  //
};  // class ScrollStopManager


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERASCROLLSTOPMANAGER_H_INCLUDED__