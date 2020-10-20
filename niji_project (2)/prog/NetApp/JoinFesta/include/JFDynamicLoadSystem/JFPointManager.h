//==============================================================================
/**
 * @file	JFPointManager.h
 * @brief	ジョインフェスタモデル設置ポイントマネージャ
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
 */
// =============================================================================

#ifndef __JF_POINT_MANAGER_H__
#define __JF_POINT_MANAGER_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFPoint.h"

// パーソナルデータ通知リスナー
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

// 前方宣言
struct  JoinFestaDataTable;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JFPointManager : public NetAppLib::JoinFesta::JoinFestaPersonalListener
{
  GFL_FORBID_COPY_AND_ASSIGN(JFPointManager);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------


public:
  // コンストラクタ
  JFPointManager( gfl2::heap::HeapBase* pHeap, bool isRoom, bool isTutorial, const JoinFestaDataTable* pDataTable );
  // デストラクタ
  virtual ~JFPointManager();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   周囲のポイントでモデルが非表示のポイントを取得
   *
   * @param   [in]pos 調査中心座標
   * @retval  見つかった場合：モデル表示していないJFPointのポインタ
   * @retval  見つからない場合:NULL
   */
  //------------------------------------------------------------------
  JFPoint* GetModelNonDispOfNearPoint( const gfl2::math::Vector3 pos ) const;

  //------------------------------------------------------------------
  /**
   * @brief   モデル表示しているポイントの中で一番遠いポイントを取得
   *
   * @param   [in]pos 調査中心座標
   * @retval  見つかった場合：JFPointのポインタ
   * @retval  見つからない場合:NULL
   */
  //------------------------------------------------------------------
  JFPoint* GetModelDispInFarPoint( const gfl2::math::Vector3 pos ) const;

  //------------------------------------------------------------------
  /**
   * @brief   モデル表示しているポイントが表示範囲内かチェック
   *
   * @param   [in]pos 調査中心座標
   * @retval  ture:全て範囲内
   * @retval  false:範囲内じゃない
   */
  //------------------------------------------------------------------
  bool CheckModelDispPointToRange( const gfl2::math::Vector3 pos ) const;

  //------------------------------------------------------------------
  /**
   * @brief   モデル表示済みポイントリストに登録
   *
   * @param   [in]jfPoint   モデル表示したJFPoint
   */
  //------------------------------------------------------------------
  void RegistJFModelViewList( JFPoint* jfPoint );

  //------------------------------------------------------------------
  /**
   * @brief   モデル表示済みポイントリストを解除
   *
   * @param   [in]jfPoint   解除したいJFPoint
   */
  //------------------------------------------------------------------
  void UnregistJFModelViewList( JFPoint* jfPoint );

  //------------------------------------------------------------------
  /**
   * @brief   モデル表示済みポイント数を取得
   * @return  モデル表示済み数
   */
  //------------------------------------------------------------------
  u32 GetModelViewCount();

  //------------------------------------------------------------------
  /**
   * @brief   ポイントを取得
   * @param   [in]index ポイントインデックス値
   * @return  JFPoint
   */
  //------------------------------------------------------------------
  JFPoint* GetJFPoint( const u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ポイントを取得
   * @param   [in]targetFriendKey 対象のフレンドキー
   * @return  JFPoint（無い場合はNULL）
   */
  //------------------------------------------------------------------
  JFPoint* GetJFPoint( JoinFestaFriendKey targetFriendKey );

public:
  /*****************************************************************/
  /* JoinFestaPersonalListener                                     */
  /*****************************************************************/
  //------------------------------------------------------------------
  /**
   * @brief ジョインフェスタパーソナルデータ追加イベント通知
   *
   * @param[in]  person  追加されたパーソナル情報
   */
  //------------------------------------------------------------------
  virtual void OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

#if PM_DEBUG
  // 位置をDump
  void Dump();
  void calc( int i, gfl2::math::Vector3 pos );
#endif

private:
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief パーソナルデータのセットアップ
   */
  //------------------------------------------------------------------
  void SetupPersonalData();

  //------------------------------------------------------------------
  /**
   * @brief アトラクション専用パーソナルデータのセットアップ
   */
  //------------------------------------------------------------------
  void SetupPersonalDataForAttraction( gfl2::heap::HeapBase* pHeap );

//------------------------------------------------------------------
  /**
   * @brief チュートリアル専用パーソナルデータのセットアップ
   */
  //------------------------------------------------------------------
  void SetupPersonalDataForTutorial();

  //------------------------------------------------------------------
  /**
   * @brief 城内専用パーソナルデータのセットアップ
   */
  //------------------------------------------------------------------
  void SetupPersonalDataForRoom();


private:
  gfl2::util::FixedSizeContainer<JFPoint*, JoinFestaDefine::JFPOINT_MAX>   m_JFPointList;              ///< 配置ポイントリスト
  gfl2::util::List<JFPoint*>*   m_JFModelViewOnPointList;   ///< モデル表示しているポイントのポインタリスト

  bool m_isRoom;      //!< 城外か城内か

  const JoinFestaDataTable* m_pDataTable;

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JF_POINT_MANAGER_H__
