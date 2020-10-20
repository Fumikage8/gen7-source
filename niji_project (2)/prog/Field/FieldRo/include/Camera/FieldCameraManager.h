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
#include <math/include/gfl2_SpringSimulation.h>

// niji
#include "System/include/nijiAllocator.h"

// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/Camera/FieldCameraTypes.h"

#if PM_DEBUG
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinGroup;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );
#endif

class BaseCollisionScene;
class DynamicActor;

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );

class Fieldmap;
class ZoneDataLoader;
class PlacementDataManager;

GFL_NAMESPACE_BEGIN( Camera );

class CameraAnimationLoader;

GFL_NAMESPACE_BEGIN( Controller );
class ControllerTypeArea;
class ControllerBase;
GFL_NAMESPACE_END( Controller );

GFL_NAMESPACE_BEGIN( Area );
class AreaDataManager;
class AreaCollisionManager;
class Focus;
class Support;
class ScrollStopManager;
GFL_NAMESPACE_END( Area );


GFL_NAMESPACE_BEGIN( Target );
class TargetBase;
GFL_NAMESPACE_END( Target );

class CameraUnit;
class CameraSystemAnimationResource;


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
    System::nijiAllocator*        m_pNijiDeviceAllocator;
    gfl2::gfx::IGLAllocator*      m_pDeviceAllocator;
    Target::TargetBase*           m_pTarget;
    void*                         m_pCameraZoneDefaultData;
    void*                         m_pCameraSupportZoneDefaultData;
    void*                         m_pSystemAnimationData;
    gfl2::fs::BinLinkerAccessor*  m_pCameraAreaDataAccessor;   // カメラエリア関連リソースデータ
    const Fieldmap*               m_pFieldMap;                  // for 現在ゾーン
    ZoneDataLoader*               m_pZoneData; 
    const PlacementDataManager*   m_pPlacementDataManager;
    BaseCollisionScene*           m_pCameraEyeCollsionScene;
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
  * @brief カメラコリジョンの更新処理
  *
  * @return 無し
  */
  void UpdateCameraEyeCollision( void );

  /**
  * @brief フィールド操作開始時に実行する処理
  *
  * @return 無し
  */
  void Ready( void );

  /**
  * @brief カメラターゲット変更処理
  *
  * @param  pTarger ターゲット (寿命の管理は行いません)
  *
  * @return 無し
  */
  void ChangeCameraTarget( Target::TargetBase* pTarger );

  /**
  * @brief 通常マップ用のカメラセットアップ
  * @note  カメラエリア用コントローラーをフィールドカメラに設定する
  *
  * @return 無し
  */
  void SetUpForNormalMap( void );

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
  * @brief 自機操作、カリングなどに使用するカメラのUnitNoを取得
  *
  * @return UnitNo
  */
  u32 GetUnitNoForMainGamePlayCamera( void ) const;

  /**
  * @brief 描画に使用するカメラのUnitNoを取得
  * @note  基本的にGamePlayCameraと同じだが、デバッグView時などは違うカメラが返る事がある
  *
  * @return UnitNo
  */
  u32 GetUnitNoForGetMainViewCamera( void ) const;

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
  Target::TargetBase* GetTarget( void ) { return m_pTarget; }
  
  /**
  * @brief カメラサポート挙動の制御アクションの実行
  *
  * @param  value     アクション種別
  *
  * @return 成否
  */
  b32 SetSupportAction( SupportAction::Value value );

  /**
  * @brief 通常フィールドコントローラーを差し替える
  *        差し替え後の復帰処理は現状では提供していません
  *
  * @param  pControllerBase 差し替え後のコントローラー
  *
  * @return 無し
  */
  void ChangeFieldController( Controller::ControllerBase* pControllerBase );

  /**
  * @brief フィールドカメラフォーカスモジュールを取得
  *
  * @return フォーカスモジュール( 初期化前はNULLの可能性があります )
  */
  const Area::Focus* GetFocus( void ) const;
  Area::Focus* GetFocus( void );

  /**
  * @brief フィールドカメラサポートモジュールを取得
  *        JFなど通常フィールドカメラを使わない場所では存在しない事があります
  *
  * @return サポートモジュール( NULLの可能性があります )
  */
  const Area::Support* GetSupport( void ) const;

  /**
  * @brief システムアニメーション(常駐)のリソースを取得
  *
  * @return システムアニメーション(常駐)のリソース管理
  */
  CameraSystemAnimationResource* GetSystemAnimationResource( void ){ return m_pSystemAnimationResource; };

  // イベント中にロードするアニメーションの管理
  CameraAnimationLoader* GetEventCameraAnimationLoader(void){ return m_pEventCameraAnimationLoader; }
 
  /**
  * @brief スクリプトイベントインスタンス生成用のヒープを取得
  *
  * @note イベント時の断片化要因を減らすため、Filed常駐ローカルヒープ化した。
  *
  * @return ヒープ
  */
  gfl2::heap::HeapBase* GetHeapForEvCamera(void){ return m_pInstanceHeapForEvCamera; }

  /**
  * @brief スクロールストップ管理を取得
  */
  Area::ScrollStopManager* GetScrollStopManager() { return m_pScrollStopManager; }

  /**
  * @brief カメラエリア用コントローラー取得
  *
  * @return　カメラエリア用コントローラ(JFではNULL)
  */
  Controller::ControllerTypeArea* GetAreaController( void ){ return m_pAreaController; }

  /**
 * @brief カメラシェイクリクエスト(水平方向)
*
* @param  mass          質量
* @param  k             バネ定数
* @param  firstVelocity 初速度
* @param  damping       減衰率
*
* @return リクエスト受理の成否
*/
  b32 GlobalShakeHorizonRequest(f32 mass, f32 k, f32 firstVelocity, f32 damping){ m_GlobalHorizonSpring.Request(mass, k, firstVelocity, damping); return true; }

  /**
* @brief カメラシェイクリクエスト(垂直方向)
*
* @param  mass          質量
* @param  k             バネ定数
* @param  firstVelocity 初速度
* @param  damping       減衰率
*
* @return リクエスト受理の成否
*/
  b32 GlobalShakeVerticalRequest(f32 mass, f32 k, f32 firstVelocity, f32 damping){ m_GlobalVerticalSpring.Request(mass, k, firstVelocity, damping); return true; }

#if PM_DEBUG
  /**
  * @brief カメラエリアのコリジョンデータ管理クラス取得
  *
  * @return　カメラエリアのコリジョンデータ管理クラス 
  */
  Area::AreaCollisionManager* GetAreaCollisionManager( void ) { return m_pCameraAreaCollisionManager; };

  /**
  * @brief 現在HitしているカメラエリアのアトリビュートNoを取得
  *
  * @return　アトリビュートNo( 無効な場合は-1 )
  */
  s32 GetHitAttr( void ) const;


  //
  // デバッグ表示物登録処理
  //
  void DebugRenderResistCameraTarget( b32 drawActiveOnlyFlag );
  void DebugRenderResistCameraTargetPosition( b32 drawActiveOnlyFlag );
#endif  //#if PM_DEBUG

private:

#if PM_DEBUG
  // デバッグメニューへの登録・解除
  void ResistDebugMenu( void );
  void UnResistDebugMenu( void );
#endif

  //
  // 制御
  //
  s32                       m_MainCameraUnitNo; // 自機操作、カリングなどに使用するカメラ
  s32                       m_ViewCameraUnitNo; // 描画に使用するカメラの番号( -1 のとき、Mainカメラを使用 )
 
  //
  // 内部オブジェクト
  //
  CameraUnit*                     m_pCameraUnitTable[ UnitNo::MAX ];
  gfl2::heap::HeapBase*           m_pCameraUnitInstanceHeap;            // CameraUnitインスタンス生成に使用するヒープ
  Area::AreaDataManager*          m_pCameraAreaDataManager;
  Area::AreaCollisionManager*     m_pCameraAreaCollisionManager;
  Controller::ControllerTypeArea* m_pAreaController;              // 通常フィールドカメラ
  CameraSystemAnimationResource*  m_pSystemAnimationResource;
  Area::ScrollStopManager*        m_pScrollStopManager;
  CameraAnimationLoader*          m_pEventCameraAnimationLoader;  // イベント中にロードするアニメーションの管理
  gfl2::heap::HeapBase*           m_pInstanceHeapForEvCamera;     // EventCameraインスタンス生成に使用するヒープ。 @note イベント時の断片化要因を減らすため、Filed常駐ローカルヒープ化した。

#if PM_DEBUG
  gfl2::debug::DebugWinGroup*       m_pDebugWinGroup;
#endif

  gfl2::math::SpringSimulation    m_GlobalHorizonSpring;                // 横バネ
  gfl2::math::SpringSimulation    m_GlobalVerticalSpring;               // 縦バネ

  //
  // 外部オブジェクト
  //
  Target::TargetBase*           m_pTarget;
  const Fieldmap*               m_pFieldMap;                  // for 現在ゾーン
  const PlacementDataManager*   m_pPlacementDataManager;
  BaseCollisionScene*           m_pCameraEyeCollsionScene;
  DynamicActor*                 m_pDynamicActorForCameraEye;

};  // class FieldCameraManager


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAMANGERER_H_INCLUDED__