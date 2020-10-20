//==============================================================================
/**
 * @file	JoinFestaFacilitiesModelResourceManager.h
 * @brief	ジョインフェスタ施設モデルリソースマネージャー
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:52
 */
// =============================================================================
#ifndef __JOIN_FESTA_FACILITIES_MODEL_RESOURCE_M_H__
#define __JOIN_FESTA_FACILITIES_MODEL_RESOURCE_M_H__
#pragma once

// ライブラリ
#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"
// JoinFesta
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaFacilitiesModelResourceManager
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaFacilitiesModelResourceManager);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------


public:
  // コンストラクタ
  JoinFestaFacilitiesModelResourceManager();
  // デストラクタ
  virtual ~JoinFestaFacilitiesModelResourceManager();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  /**
   * @brief   読み込み
   * @param   heap      ヒープ
   *
   * @return  trueで読み込み完了
   */
  bool Load( gfl2::heap::HeapBase * heap );

  /**
   * @brief   破棄
   * @return  trueで破棄完了
   */
  bool Terminate();

  /**
   * @brief   施設のモデルリソース取得
   */
  void* GetModel( JoinFestaScript::FacilitiesType type );

  /**
   * @brief   城のモデルリソース取得
   */
  void* GetCastleModel();

  /**
   * @brief   RR団の城のモデルリソース取得
   */
  void* GetRRCastleModel();

  /**
   * @brief   通常時の地面のモデルリソース取得
   */
  void* GetNormalGroundModel();

  /**
   * @brief   RR団の地面のモデルリソース取得
   */
  void* GetRRGroundModel();

  /**
   * @brief   RR団イベント用の空のモデルリソース取得
   */
  void* GetRRSkyModel();

  /**
   * @brief   アトラクション時の地面のモデルリソース取得
   */
  void* GetAttractionGroundModel();

  /**
   * @brief   城のアニメID取得
   * @return  アニメID
   */
  u32 GetCastleAnimeId();

  /**
   * @brief   RR団の城のアニメID取得
   * @return  アニメID
   */
  u32 GetRRCastleAnimeId();

  /**
   * @brief   城のテクスチャパターンアニメID取得
   * @return  アニメID
   */
  u32 GetCastleTextureAnimeId();

  /**
   * @brief   RR団イベント用の空のアニメID取得
   */
  u32 GetRRSkyAnimeId();

  /**
   * @brief   アニメーションデータ取得
   */
  void* GetAnimationData();

  /**
   * @brief   施設のアニメID取得
   * @return  アニメID
   */
  u32 GetAnimeId( JoinFestaScript::FacilitiesType type );

  /**
   * @brief   施設のテクスチャパターンアニメID取得
   * @return  アニメID
   */
  u32 GetTextureAnimeId( JoinFestaScript::FacilitiesType type );

public:

  
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================

private:

  bool                        m_isLoaded;       //! 読み込み完了フラグ
  u8                          m_loadSeq;        //! 読み込みシーケンス
  void*                       m_pResourceBuff;  //! リソースバッファ
  gfl2::fs::BinLinkerAccessor m_BinLinker;      //! 

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JOIN_FESTA_FACILITIES_MODEL_RESOURCE_M_H__
