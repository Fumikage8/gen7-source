/**
* @brief  フィールドカメラ、フォーカス機能
* @file   FieldCameraAreaFocus.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAAREAFOCUS_H_INCLUDED__ )
#define __FIELDCAMERAAREAFOCUS_H_INCLUDED__
#pragma once

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>
#include <math/include/gfl2_MathAABB.h>

#if PM_DEBUG
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinGroup;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#endif

// field
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"


class DynamicActor;


GFL_NAMESPACE_BEGIN( Field );

class FieldTrainerActor;
class PlacementDataManager;

GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

/**
* @brief フィールドカメラのサポート挙動
*/
class Focus
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( Focus );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
      const Target::TargetBase*         m_pTarget;                // 注視対象
      const PlacementDataManager*       m_pPlacementDataManager;
  };

  // 検索結果
  struct SearchResult
  {
    b32 isHit;                              // falseなら後述のパラメータは不正
    u32 id;                                 // エリアでユニークなID
    gfl2::math::Vector3 position;           // 対象座標
    gfl2::math::Vector3 interestPosition;   // 自機注目座標
    f32 checkLengh;                         // Hit時の長さ設定
  };
 
  // イベントによる停止
  struct EventStopBit
  {
    enum Value
    {
      FOCUS  = 0x01,
      EFFECT = 0x01 << 1,
      BGM    = 0x01 << 2,
      INTERES= 0x01 << 3,
      SE     = 0x01 << 4
    };
  };

    // フォーカスで操作するパラメーター一式
  struct FocusCameraParam
  {
    gfl2::math::Vector3               m_CameraTargetPos;       //< カメラターゲット座標
    gfl2::math::Vector3               m_CameraPos;             //< カメラ座標
    f32                               m_FovY;                   //< 画角
  };

  /**
  * @brief コンストラクタ
  *
  * @param rInitDesc  初期化設定
  *
  * @return 無し
  */ 
  Focus( const InitializeDescription& rInitDesc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~Focus( void );

  /**
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( poke_3d::model::BaseCamera* pCamera, f32 minDistance = 0.0f );

  /**
  * @brief カメラ反映処理
  *
  * @return 無し
  */
  void Execute( poke_3d::model::BaseCamera* pCamera ) const;

  /**
  * @brief 注目すべきか取得する
  *
  * @return true 有効 false 無効
  */
  b32 IsInterest( void ) const;
  
  /**
  * @brief 注目座標を取得する
  *
  * @param  pOut 座標格納先
  *
  * @return true 有効 false 無効( IsInterest関数と連動 )
  */
  b32 GetInterestPosition( gfl2::math::Vector3* pOut ) const;
  
  /**
  * @brief フォーカス判定に使用する視線アクターを取得 
  *
  * @return 視線アクター
  */
  DynamicActor* GetFocusEyeActor( void ){ return m_pPlayerEyeDynamicActor; }

  /**
  * @brief フォーカス判定に使用する視線アクターを取得(継続用に拡大されたもの) 
  *
  * @return 視線アクター
  */
  DynamicActor* GetFocusEyeActorForKeep( void ){ return m_pKeepPlayerEyeDynamicActor; }
  
  /**
  * @brief サポート挙動が有効か取得する(フォーカス戻り中もtrueを返す)
  *
  * @return true 有効 false 無効
  */
  b32 IsValid( void ) const;

  /**
  * @brief イベントによるフォーカス中止フラグを設定
  * @note  1フレームで複数回切り替わる可能性がある
  *
  * @return true 有効 false 無効
  */
  void ClearEventStopBit( void ); 

  /**
  * @brief イベントによるフォーカス中止フラグを設定
  * @note  1フレームで複数回切り替わる可能性がある
  *
  * @return true 有効 false 無効
  */
  void EventStop( u32 bit );
  

  void Reset( void );
  void ResetForRideEvent(void);
#if PM_DEBUG
  void ResistDebugMenu( void );
  void UnResistDebugMenu( void );
  void DebugDrawCameraCheckInfo( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
#endif

private:

  /**
  * @brief エフェクト状態を更新
  *
  * @return 無し
  */
  void UpdateEffect( void );
  
  /**
  * @brief BGM状態を更新
  *
  * @return 無し
  */
  void UpdateBGM( void );
  
  /**
  * @brief 効果音を更新
  *
  * @return 無し
  */
  void UpdateSE( void );

  /**
  * @brief キープ状態更新
  *
  * @return 無し
  */
  void UpdateKeepFrame( void );


  /**
  * @brief フォーカス速度を取得
  *
  * @return フォーカス速度
  */
  f32 GetFocusSpeed( void ) const;

  /**
  * @brief 自機の視線アクターを更新
  *
  * @return 無し
  */
  void UpdatePlayerEyeActor( void );

  /**
  * @brief フォーカス対象トレーナーを取得
  *
  * @param  pOut            今回の結果
  * @param  pResultLostKeep keepFlagによるHITならtrue
  * @param  rPrevResult     前回の結果
  * @param  keepFlag        true = 前回のターゲットが存在した場合、Keepする
  *
  * @return 対象
  */
  void SearchFocusTrainerEx( SearchResult* pOut, b32* pResultLostKeep, const SearchResult& rPrevResult, b32 keepFlag );

#if PM_DEBUG
  void SearchFocusTrainerForDebugCameraCheck( SearchResult* pOut, b32* pResultLostKeep, const SearchResult& rPrevResult, b32 keepFlag );
#endif


  /**
  * @brief 注目座標を所得
  *
  * @param pTarget 対象
  * @param pOut    座標
  *
  * @return 成否
  */
  b32 GetInterestPositionByActor( const FieldTrainerActor* pTarget, gfl2::math::Vector3* pOut ) const;

  /**
  * @brief カメラフレームIN処理、ターゲットを納めるカメラ位置を算出
  *
  * @param rOffsetVector  カメラ向き(TargetToPositon)
  * @param fovY           画角
  * @param pTarget        カメラターゲット座標格納先
  * @param pPosition      カメラ座標格納先
  * @param minDistace     最小カメラ距離
  *
  * @return 無し
  */
  void GetPosEx( const SearchResult& rResult, const gfl2::math::Vector3& rOffsetVector, f32 fovY, gfl2::math::Vector3* pTarget, gfl2::math::Vector3* pPosition, f32 minDistace = 0.0f, f32 maxDistance = FLT_MAX ) const;

  // サーチ結果からカメラ位置を算出
  void GetFixParam( const poke_3d::model::BaseCamera* pCamera, const SearchResult& rResult, const FocusCameraParam& rOffset, FocusCameraParam* pParam, f32 minDistace = 0.0f ) const;

  // FocusCameraParamの線形補間
  void LerpParam( const FocusCameraParam& rSrc, const FocusCameraParam& rTarget, FocusCameraParam* pOut, f32 t ) const;
  
  // 現在のカメラデータをFocusCameraParamに設定
  void SetParam( const poke_3d::model::BaseCamera* pCamera, FocusCameraParam* pOut ) const;

  // FocusCameraParamを0クリア
  void ClearParam( FocusCameraParam* pOut ) const;

  // FocusCameraParamを足した結果を取得
  void AddParam( const FocusCameraParam& rSrc1, const FocusCameraParam& rSrc2, FocusCameraParam* pOut ) const;


  struct State
  {
    enum Value
    {
      STOP = 0,
      MOVE_IN,
      MOVE_OUT
    };
  };

  //
  // 内部制御
  //
  u32                               m_EventStopBit;
  u32                               m_StopBit;                    // 0以外ならフォーカス停止
  u32                               m_BgmStopBit;                 // 0以外ならBGM影響を停止
  b32                               m_IsReady;                    // true 準備完了済み
  u32                               m_KeepFrame;                  // 0になるまでロスト前のターゲットをキープする
  State::Value                      m_State;
  ValueController                   m_FocusValueController;

  f32                               m_HitCheckLength;
  gfl2::math::Vector3               m_InterestPosition;           // 自機が注目すべき座標

  FocusCameraParam                  m_OffsetParam;                // 遷移が発生した瞬間のオフセット値、絶対座標ではなく相対座標で補間するために記憶する
  FocusCameraParam                  m_FixParam;                   // 最終的な座標


  DynamicActor*                     m_pPlayerEyeDynamicActor;     //< 視線ヒットを受けるためのアクター
  DynamicActor*                     m_pKeepPlayerEyeDynamicActor; //< 視線ヒットを受けるためのアクター(以前のHIT用)
  SearchResult                      m_NowSearchResult;

#if PM_DEBUG
  gfl2::debug::DebugWinGroup*       m_pDebugWinGroup;
#endif

  //
  // 外部オブジェクト
  //
  const Target::TargetBase*         m_pTarget;                // 注視対象
  const PlacementDataManager*       m_pPlacementDataManager;

};  // class Focus

GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAAREAFOCUS_H_INCLUDED__


