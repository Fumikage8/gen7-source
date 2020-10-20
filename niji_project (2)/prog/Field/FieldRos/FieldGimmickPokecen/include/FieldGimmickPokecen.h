//==============================================================================
/**
 * @file	FieldGimmickPokecen.h
 * @brief	フィールドギミック：ポケモンセンター
 * @author	ohhira_syo
 * @date	2015/11/12 18:56:00
 */
// =============================================================================

#if !defined(__FIELD_GIMMICK_POKECEN_H__)
#define __FIELD_GIMMICK_POKECEN_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "AppLib/include/Util/app_util_heap.h"

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <model/include/gfl2_BaseModel.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include "System/include/nijiAllocator.h"

#include "AppLib/include/Tool/PokeCapAccesser.h"

//誕生日用
#include <niji_conv_header/field/pokecen/pokecen_conv.h>

//ResortIslandDraw::TextureResourceNodeGetter
//#include "App/Resort/source/ResortIslandDraw.h"

// FieldGimmick
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"


namespace gfl2{
  namespace renderingengine {
    namespace scenegraph {
      namespace resource{
        class ResourceNode;
      }
    }
  }
}



GFL_NAMESPACE_BEGIN( Field )

/// ポケモンセンター用ギミック
class FieldGimmickPokecen : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickPokecen );

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
public:
  //ResourtIslandDrawからそのまま持ってきたもの（そのまま参照すると3DS版で落ちるためコピペ）
  //! @brief  テクスチャリソースノードゲッター
  //! @note   gfl2::renderingengine::scenegraph::Traverser::VisitorのImplementに渡すクラスとしての役割と
  //!         テクスチャリソースノードを取得するクラスとしての役割を持つ。
  class TextureResourceNodeGetter
  {
    GFL_FORBID_COPY_AND_ASSIGN(TextureResourceNodeGetter);

  public:
    //! @brief  gfl2::renderingengine::scenegraph::Traverser::VisitorのImplementに必要なメンバ関数
    void Visit(gfl2::renderingengine::scenegraph::DagNode* dagNode);
  
  public:
    //! @brief  dagNode内に存在するテクスチャリソースノードを取得する(最初の1個を見付けた時点で探すのを止める)
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode);

  public:
    TextureResourceNodeGetter(void);
    virtual ~TextureResourceNodeGetter();

  private:
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* getTextureResourceNode(void);

  private:
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_textureResourceNode;
  };

public:
  // コンストラクタ
  FieldGimmickPokecen( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap );
  // デストラクタ
  virtual ~FieldGimmickPokecen();

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================

  /**
   *  @brief  生成
   */
  virtual void Create( void );
  virtual void UpdateCreate( void );
  virtual bool IsCreated( void );
  /**
   *  @biref  破棄更新
   *  @return 破棄が完了したかどうか
   *
   *  破棄が完了するまでマイフレーム呼出がなされる関数
   */
  virtual bool Finalize( void );

  /**
   *  @brief  破棄
   */
  virtual void Delete( void );

  /**
   *  @brief  Ready中1度だけ呼び出し
   */
  virtual void Ready( void );

  /**
   *  @brief  他のReadyが終わった後に1度だけ呼び出し
   */
  virtual void ReadyAfter( void );
  
  /**
   *  @brief  更新
   *  Fieldmap::Updateにて呼出
   *  キャラクタ前更新よりも手前で呼び出される
   */
  virtual void UpdateFirst( void );
  /**
   *  @brief  更新
   *  Fieldmap::Updateにて呼出
   *  UpdateNormalが終了してから呼び出される
   */
  virtual void UpdateLast( void );
  /**
   *  @brief  更新
   *  Fieldmap::UpdateNormalにて呼び出される
   */
  virtual void CalculateAfter( void );

  /**
   *  @brief  特殊ステップチェック
   */
  virtual bool MoveRequestCheck( const EventCheckRequest *cpReq );

  /**
   *  @brief  イベント起動チェック
   */
  virtual bool EventRequestCheck( const EventCheckRequest *cpReq );


  //=スクリプト用インターフェース=

  //#回復装置：基本

  /**
   *  @brief 回復装置：開始
   */
  void MachineInit();

  /**
   *  @brief 回復装置：ボールの表示のオンオフ
   */
  void MachineBallSetVisible(int index, bool visible);

  /**
   *  @brief 回復装置：終了
   */
  void MachineFin();

  //#誕生日演出

  /**
   *  @brief アニメーション開始
   */
  void BirthdayAnimStart(u32 model_id, bool is_on = true);

  /**
   *  @brief アニメーション終了チェック
   */
  bool BirthdayAnimIsEnd(u32 model_id);


  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================

private:
  //汎用
  gfl2::heap::HeapBase*                         m_pFieldDeviceHeap;
  gfl2::fs::AsyncFileManager*                   m_pFileReadManager;

  //#モンスターボール用
  void*                                                           m_pResourceBuff;
  gfl2::fs::BinLinkerAccessor                                     m_BinLinker;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pResource[6];
  poke_3d::model::BaseModel*                                      m_pModel[6];
  System::nijiAllocator*                                          m_pAllocator;

  //#キャプチャ画像用
  AppLib::Tool::PokeCapAccesser*                             m_pPokeCapAccesser;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pCapResourceNode[6];
  void*                                                      m_pCapturePoke[6];
  TextureResourceNodeGetter                                  m_TexGetter;
  app::util::Heap                                            m_ResourceManagerHeap;
  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* m_pMaterialNodeRef[6];

  //#誕生日用
  void Ready_Birthday();
  bool                                                       m_bAppRecover;
  s8                                                         m_AnimReq[POKECEN_MDLID_NUM];
  void*                                                      m_pResourceBuff_Birthday;
  gfl2::fs::BinLinkerAccessor                                m_BinLinker_Birthday;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pModelResource_Birthday[POKECEN_MDLID_NUM];
//  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMotionResource_Birthday[POKECEN_MDLID_NUM];
  poke_3d::model::BaseModel*                                 m_pModel_Birthday[POKECEN_MDLID_NUM];
  gfl2::animation::AnimationPackList                         m_AnimPackList[POKECEN_MDLID_NUM];
};

GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_POKECEN_H__
