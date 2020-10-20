/**
* @brief  フィールドカメラを管理するクラス
* @file   FieldCameraManager.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAMANGERER_H_INCLUDED__ )
#define __FIELDCAMERAMANGERER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <Collision/include/gfl2_CollisionModel.h>

// field
#include "ViewerProc/include/FieldRo/FieldRootNode.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );


GFL_NAMESPACE_BEGIN( Area );
class AreaDataManager;
GFL_NAMESPACE_END( Area );


GFL_NAMESPACE_BEGIN( Target );
class TargetBase;
GFL_NAMESPACE_END( Target );

class CameraUnit;


// CameraUnit定義
struct UnitNo
{
  enum Value
  {
    FIELD = 0,    // フィールド
    EVENT,        // スクリプト/ポケファインダー用   
    // SCRIPT,     // スクリプト用
    DEBUG,
    MAX
  };
};



/**
* @brief フィールドカメラを管理するクラス
*/
class CameraManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( CameraManager );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::heap::HeapBase*         m_pDeviceHeap;
    gfl2::gfx::IGLAllocator*      m_pDeviceAllocator;
    Target::TargetBase*           m_pTarget;
    gfl2::fs::BinLinkerAccessor*  m_pCameraAreaDataAccessor;   // カメラエリア関連リソースデータ
    //gfl2::fs::AsyncFileManager*   m_pFileManager;              // ファイル読み込みマネージャー
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  CameraManager( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~CameraManager( void );
  


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
  * @brief カメラターゲット変更処理
  *
  * @param  pTarger ターゲット (寿命の管理は行いません)
  *
  * @return 無し
  */
  void ChangeCameraTarget( Target::TargetBase* pTarger );

  /**
  * @brief 開発要SetUp
  *
  * @return 無し
  */
  void SetUpTest( void );

  /**
  * @brief 自機操作、カリングなどに使用するカメラを取得する
  *
  * @return カメラ
  */
  const CameraUnit* GetMainGamePlayCamera( void ) const;
  
  /**
  * @brief 描画に使用するカメラを取得する
  * @note  基本的にGamePlayCameraと同じだが、デバッグView時などは違うカメラが返る事がある
  *
  * @return カメラ
  */
  const CameraUnit* GetMainViewCamera( void ) const;

  /**
  * @brief 自機操作、カリングなどに使用するカメラを取得する
  *
  * @return カメラ
  */
  CameraUnit* GetMainGamePlayCamera( void );

  /**
  * @brief 描画に使用するカメラを取得する
  * @note  基本的にGamePlayCameraと同じだが、デバッグView時などは違うカメラが返る事がある
  *
  * @return カメラ
  */
  CameraUnit* GetMainViewCamera( void );

  /**
  * @brief カメラを取得する
  *
  * @param  unitNo  ユニット番号 ( 今はUnitNo )
  *
  * @return カメラ
  */
  const CameraUnit* GetCameraByUnitNo( u32 unitNo ) const;

  /**
  * @brief カメラを取得する
  *
  * @param  unitNo  ユニット番号 ( 今はUnitNo )
  *
  * @return カメラ
  */
  CameraUnit* GetCameraByUnitNo( u32 unitNo );

  /**
  * @brief 使用するカメラを設定する( play / view 両方)
  *
  * @param  unitNo  ユニット番号 ( 今はUnitNo )
  * @param  prevSleep trueなら現在使用しているカメラをSleepして変更
  *
  * @return 成否
  */
  b32 ChangeMainCameraByUnitNo( u32 unitNo, b32 prevSleep = true );

  /**
  * @brief 描画で使用するカメラを上書き設定する
  *
  * @param  unitNo  ユニット番号 ( 今はUnitNo )
  * @param  prevSleep trueなら現在使用しているカメラをSleepして変更( Mainで使用されている場合はSleepしない )
  *
  * @return 成否
  */
  b32 OverrideViewCameraByUnitNo( u32 unitNo, b32 prevSleep = true );

  /**
  * @brief 描画で使用するカメラを上書き設定を解除
  *
  * @param  prevSleep trueなら現在使用しているカメラをSleepして変更( Mainで使用されている場合はSleepしない )
  *
  * @return 成否
  */
  b32 ClearOverrideViewCamera( b32 prevSleep = true );

  /**
  * @brief 描画で使用するカメラを上書き設定しているか？
  * @note  結果的に同じカメラでも、Overrideしている扱いになる
  *
  * @param  pUnitNo 設定しているUnitNo格納先  NULL指定可、戻り値trueのときのみ有効
  *
  * @return true している   false していない
  */
  b32 IsOverrideViewCamera( u32* pUnitNo = NULL ) const;

  /**
  * @brief カメラの更新フラグを設定
  *
  * @param  unitNo  ユニット番号 ( 今はUnitNo )
  * @param  isActive  trueなら更新
  *
  * @return 成否
  */
  b32 SetActiveFlagByUnitNo( u32 unitNo, b32 isActive );

  /**
  * @brief カメラシステムに登録されているターゲットを取得
  *
  * @return ターゲット
  */
  const Target::TargetBase* GetTarget( void ) const { return m_pTarget; }


private:

  //
  // 制御
  //
  s32                       m_MainCameraUnitNo; // 自機操作、カリングなどに使用するカメラ
  s32                       m_ViewCameraUnitNo; // 描画に使用するカメラの番号( -1 のとき、Mainカメラを使用 )
 
  //
  // 内部オブジェクト
  //
  CameraUnit*               m_pCameraUnitTable[ UnitNo::MAX ];
  gfl2::heap::HeapBase*     m_pCameraUnitInstanceHeap;            // CameraUnitインスタンス生成に使用するヒープ
  Area::AreaDataManager*    m_pCameraAreaDataManager;
  gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pCameraAreaClollisionModelResource;

  //
  // 外部オブジェクト
  //
  Target::TargetBase* m_pTarget;

};  // class FieldCameraManager


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAMANGERER_H_INCLUDED__