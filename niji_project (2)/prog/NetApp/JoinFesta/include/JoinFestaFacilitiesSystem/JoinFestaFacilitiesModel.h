//==============================================================================
/**
 * @file	JoinFestaFacilitiesModel.h
 * @brief	ジョインフェスタ施設モデル
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:52
 */
// =============================================================================
#ifndef __JOIN_FESTA_FACILITIES_MODEL_H__
#define __JOIN_FESTA_FACILITIES_MODEL_H__
#pragma once

// ライブラリ
#include <macro/include/gfl2_macros.h>
#include <math/include/gfl2_Vector3.h>
#include <heap/include/gfl2_Heap.h>

#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>

// niji
#include "System/include/nijiAllocator.h"

// JoinFesta
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaFacilitiesModel
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaFacilitiesModel);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  //! アニメID無効値
  static const u32 ANIME_ID_NULL = 1000;


public:
  // コンストラクタ
  JoinFestaFacilitiesModel( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual ~JoinFestaFacilitiesModel();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
public:

  /**
   * @brief 作成
   * @param modelBuff    モデルリソースバッファ
   * @param animationBuff アニメーションバッファ
   * @param animeId      ループアニメID
   * @param textureAnimeId テクスチャパターンアニメID
   */
  void Create( void* modelBuff, void* animationBuff,
               const u32 animeId = ANIME_ID_NULL,
               const u32 textureAnimeId = ANIME_ID_NULL );

  /**
   * @brief 作成
   *
   * @param modelBuff    モデルリソースバッファ
   * @param resourceBuff アニメーションバッファ
   * @param position     位置
   * @param rotationRadY 回転Ｙ（ラジアン）
   * @param animeId      ループアニメID
   * @param textureAnimeId テクスチャパターンアニメID
   */
  void Create( void* modelBuff, void* animationBuff,
               const gfl2::math::Vector3& position,
               const f32 rotationRadY,
               const u32 animeId = ANIME_ID_NULL,
               const u32 textureAnimeId = ANIME_ID_NULL );

  /**
   * @brief アニメ更新
   */
  void UpdateAnime();

  /**
   * @brief アニメ変更
   */
  void ChangeAnime( u32 animeId );

  /**
   * @brief テクスチャ変更
   */
  void ChangeTexture( u32 textureId );

  /**
   * @brief 破棄
   * @return trueで破棄完了
   */
  bool Delete();


  //------------------------------------------------------------------
  /**
    *  @brief    表示設定
    *  @note     trueで表示
    */
  //------------------------------------------------------------------
  void SetVisible( bool isVisible )
  {
    if( m_pModel )
    {
      m_pModel->SetVisible( isVisible );
    }
  }

  //------------------------------------------------------------------
  /**
    *  @brief    WorldMatrixの取得
    *  @note     BaseModel::GetWorldMatrix()がconstでない
    */
  //------------------------------------------------------------------
  const gfl2::math::Matrix34* GetWorldMatrix(void)
  {
    return (m_pModel ? (&m_pModel->GetWorldMatrix()) : NULL);
  }


  
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================

private:

  gfl2::heap::HeapBase*     m_pHeap;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pModelResourceRootNode; 
  poke_3d::model::BaseModel* m_pModel; 
  System::nijiAllocator* m_pAllocator; 
  gfl2::animation::AnimationPackList m_AnimPackList;
  u32                         m_textureAnimeId; //! テクスチャパターンアニメID

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JOIN_FESTA_FACILITIES_MODEL_H__
