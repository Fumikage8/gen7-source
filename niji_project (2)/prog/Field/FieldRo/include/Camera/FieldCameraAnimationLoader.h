/**
* @brief  カメラアニメーションを1セットだけ管理するクラス
* @file   FieldCameraAnimationLoader.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERANIMATIONLOADER_H_INCLUDED__ )
#define __FIELDCAMERANIMATIONLOADER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

// niji
#include "System/include/nijiAllocator.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );


/**
* @brief フィールドカメラを管理するクラス
*/
class CameraAnimationLoader
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( CameraAnimationLoader );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    u32 dummy;
  };

  class LoadDescription
  {
  public:
    s32                           m_ArcNo;
    s32                           m_ArcDataNo;
    gfl2::fs::AsyncFileManager*   m_pFileManager;
    gfl2::heap::HeapBase*         m_pHeap;  
    System::nijiAllocator*        m_pAllocator;
  };

  struct State
  {
    enum Value
    {
      NONE = 0,
      LOADING,
      COMPLETE
    };
  };


  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  CameraAnimationLoader( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~CameraAnimationLoader( void );


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
  * @brief ロード処理
  *
  * @return 無し
  */
  void Load( const LoadDescription& rLoadDesc );

  /**
  * @brief ロード破棄処理
  *
  * @return 無し
  */
  void UnLoad( void );

  /**
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( void );

  /**
  * @brief ロード中か取得
  *
  * @return ロード中ならtrue、ロードしていない、ロード完了ならfalseを返す
  */
  b32 IsLoading( void ) const;
  
  /**
  * @brief アニメーションデータ取得
  *
  * @param  dataIndex データ番号
  *
  * @return アニメーションデータ( NULL なら引数不正 )
  */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetData( u32 dataIndex );


private:

  //
  // 制御
  //
  State::Value                  m_State;
  s32                           m_ArcNo;
  s32                           m_ArcDataNo;

  //
  // 内部オブジェクト
  //
  gfl2::renderingengine::scenegraph::resource::ResourceNode**  m_ppResourceNodeTable;
  u32                                                          m_DataNum;
  void*                                                       m_pBinary;

  //
  // 外部オブジェクト
  //
  gfl2::fs::AsyncFileManager*   m_pFileManager;
  gfl2::heap::HeapBase*         m_pHeap;  
  System::nijiAllocator*        m_pAllocator;

};  // class FieldCameraManager


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERANIMATIONLOADER_H_INCLUDED__