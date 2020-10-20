//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EnvSoundManager.cpp
 *	@brief  環境音
 *	@author	tomoya takahashi
 *	@date		2015.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __ENV_SOUND_MANAGER_H__ )
#define __ENV_SOUND_MANAGER_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_List.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

//
#include "Field/FieldRo/include/PlacementData/accessor/FieldEnvSoundAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldEnvSoundCollisionAccessor.h"

// Sound
#include "niji_conv_header/sound/SoundMiddleID.h"

GFL_NAMESPACE_BEGIN(gfl2)


GFL_NAMESPACE_BEGIN(math)
class Vector3;
class Quaternion;
GFL_NAMESPACE_END(math)

GFL_NAMESPACE_END(gfl2)

namespace gfl2{ namespace util {
  class DrawUtilText;
}}



GFL_NAMESPACE_BEGIN(Field)

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class EnvSoundElem;

#if PM_DEBUG
class EnvSoundElemDebugInfo;
#endif

/**
 *  @class EnvSoundManager
 *  @brief  環境音マネージャー
 */
class EnvSoundManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( EnvSoundManager );

public:
#if PM_DEBUG
  static f32 PAN_CONTROL_START_COS;
  static f32 PAN_CONTROL_END_COS;

  static f32 PAN_CONTROL_POWER;

#else
  static const f32 PAN_CONTROL_START_COS;
  static const f32 PAN_CONTROL_END_COS;

  static const f32 PAN_CONTROL_POWER;

#endif

  enum
  {
    SE_ALL = 0xFFFF, ///< 全ての環境音データのストップ、再生のチェック
  };
public:

  // コンストラクタ・デストラクタ
  EnvSoundManager();
  ~EnvSoundManager();

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase* pHeap, const void * cpBuff );

  /**
   *  @brief  破棄
   */
  void Terminate();


  /**
   *  @brief  更新
   */
  void Update( const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate_player, const gfl2::math::Quaternion& rotate_camera, bool playerForm, FieldEnvSoundAccessor::EnvSoundTimeZone timezone );


  /**
   *  @brief  環境音の再生/停止
   *          初期値は再生
   */
  void SetEnableElem( bool enable_flag, u32 se_id = SE_ALL );


#if PM_DEBUG
  /**
   *  @brief  環境音コリジョンをデバッグ表示へ登録
   *  @param  color         表示色
   *  @param  onlyPlayFlag  再生中のコリジョンのみ表示
   *  @param  onlyActiveFlag true:アクティブなコリジョンのみ表示する false：全て表示する
   */
  void DebugRegistRenderCollision( gfl2::math::Vector4 color, b32 onlyPlayFlag = true, b32 onlyActiveFlag = true );


  /**
   *  @brief  環境音の情報を画面に表示
   */
  void DebugDrawEnvSoundManagerInfo(gfl2::util::DrawUtilText* pDrawUtilText, const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
#endif  //#if PM_DEBUG

private:
  static const s32 ENV_SOUND_MANAGER_NUM = 30;  ///< マネージャーが管理する音源の最大数。
  static const s32 ENV_SOUND_MANAGER_PLAY_LIST_MAX = 3; ///< 同時再生できる音源の数
  static const s32 ENV_SOUND_SE_STOP_FADE_TIME = 4; ///< SEをとめる際のフェード時間

private:

  /**
   *  @brief  今のフレーム再生すべき音源を更新する。
   *  
   *  @param  pos                     プレイヤー位置
   *  @param  rotate_player           プレイヤー方向
   *  @param  rotate_camera           カメラ方向
   *  @param  playerForm              現状のプレイヤーフォルム
   */
  void updateElem( const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate_player, const gfl2::math::Quaternion& rotate_camera, bool playerForm, FieldEnvSoundAccessor::EnvSoundTimeZone timezone );


  /**
   *  @brief  音源情報とのヒット判定を行なう。
   */
  bool checkElem( const EnvSoundElem* cpElem, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate_player, const gfl2::math::Quaternion& rotate_camera, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y );

  /**
   *  @brief  個別ヒット判定「Cylinder」
   */
  bool checkElemCylinder( const FieldEnvSoundCollisionAccessor::Cylinder& colData, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y );
  /**
   *  @brief  個別ヒット判定「Line」
   */
  bool checkElemLine( const FieldEnvSoundCollisionAccessor::Line& colData, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y );
  /**
   *  @brief  個別ヒット判定「Sphere」
   */
  bool checkElemSphere( const FieldEnvSoundCollisionAccessor::Sphere& colData, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y );
  /**
   *  @brief  個別ヒット判定「PipeLine」
   */
  bool checkElemPipeLine( const FieldEnvSoundCollisionAccessor::PipeLine& colData, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y );

  /**
   *  @brief  音源情報を音源情報をPlayリストに追加・更新する。
   */
  void addPlayList( EnvSoundElem* pElem, bool playerForm, f32 distance, f32 distance_max, f32 dir_cos, f32 dir_cross_y, FieldEnvSoundAccessor::EnvSoundTimeZone timezone );

  /**
   *  @brief  更新処理
   */
  void updatePlayList( EnvSoundElem* pElem );

  /**
   *  @brief  音源情報をPlayリストから削除する。
   */
  gfl2::util::List<EnvSoundElem*>::Iterator removePlayList( gfl2::util::List<EnvSoundElem*>::Iterator pElem );

  /** 
   *  @brief  音源の音を一時停止する。
   */
  void stopElem( EnvSoundElem* pElem );

  /** 
   *  @brief  音源の再生を開始する。
   */
  void playElem( EnvSoundElem* pElem );


private:
  FieldEnvSoundAccessor* m_pAccessor;     ///< 環境音データアクセサー
  
  gfl2::util::List<EnvSoundElem*>* m_pSoundElemList;      ///< 環境音源リスト
  gfl2::util::List<EnvSoundElem*>* m_pPlaySoundElemList;  ///< 再生中環境音源リスト

#if PM_DEBUG
  static EnvSoundElemDebugInfo * m_pDebugInfo;
  static bool m_DebugInfoPlayerForm;
  static FieldEnvSoundAccessor::EnvSoundTimeZone m_DebugInfoTimezone;
#endif
};


GFL_NAMESPACE_END(Field)

#endif	// __ENV_SOUND_MANAGER_H__

