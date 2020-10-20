//======================================================================
/**
 * @file FieldTrialModel.h
 * @date 2015/09/08 12:52:42
 * @author miyachi_soichi
 * @brief フィールドで使用するバトルモデル：試練用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_TRIALMODEL_H_INCLUDED__
#define __FIELD_TRIALMODEL_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// field
#include "Field/FieldStatic/include/Field3DObjectNode.h"

GFL_NAMESPACE_BEGIN(PokeTool)
class PokeModel;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(Field)
class MyRenderingPipeLine;

GFL_NAMESPACE_BEGIN(TrialModel)
struct ShadowData;

/**
 * @class TrialModel
 * @brief クラスの説明
 */
class FieldTrialModel : public gfl2::renderingengine::scenegraph::NodeTrait<FieldTrialModel,Field::IField3DObjectNode>
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldTrialModel);

public:
  enum ModelType
  {
    MODEL_TYPE_TRIAL,   //!< 試練用
    MODEL_TYPE_FINDER,  //!< ファインダー用
  };

public:
  static gfl2::util::FnvHash GetTypeHash()
  {
    return gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::FieldNode::Field3DObjectNode::FieldTrialModel") ) );
  }

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldTrialModel( PokeTool::PokeModel *pModel, Field::MyRenderingPipeLine *pPipe );

  /**
   *  @brief  デストラクタ
   */
  ~FieldTrialModel( void ){}

  /**
   *  @brief  更新
   *  @note   アニメーション更新前に行う
   */
  virtual void UpdateBeforeAnimation( void ){}

  /**
   *  @brief  更新
   *  @note   アニメーション更新後に行う
   */
  virtual void UpdateAfterAnimation( void ){}

  /**
   *  @brief  更新
   */
  virtual void UpdateAfterTraverse( void ){}

  /**
   *  @brief  モデル作成
   */
  void CreateModel( void );

  /**
   *  @brief  モデル破棄
   */
  void DeleteModel( void );

  /**
   *  @brief  モデル取得
   */
  PokeTool::PokeModel *GetPokeModel( void ){ return m_pModel; }

  /**
   *  @brief  ロード終了検知
   */
  bool IsReady( void );
  /**
   *  @brief  作成検知
   */
  bool IsEnable( void );

  /**
   *  @brief  表示設定取得
   */
  bool IsDisp( void ) const;

  /**
   *  @brief  表示設定
   */
  void SetDisp( bool flag );

  /**
   *  @brief  load
   */
  void LoadMotion( gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase *pHeap, void *pMotPack );

  /**
   *  @brief  Unload
   */
  void UnLoadMotion( void );

  /**
   *  @brief  モーションの変更
   */
  void ChangeAnimation( u32 idx );

  /**
   *  @brief  モーション待ち
   */
  bool IsAnimationLastFrame( void );

  /**
   *  @brief  座標の取得
   */
  gfl2::math::Vector3 GetPosition( void );

  /**
   *  @brief  影設定
   */
  void SetShadowVisible( bool flag ){ m_bShadowVisible = flag; }
  bool GetShadowVisible( void ) const { return m_bShadowVisible; }
  void SetShadowData( ShadowData *pData ){ m_pShadowData = pData; }
  u32 GetShadowType( void );
  f32 GetShadowScale( void );
  gfl2::math::Vector3 GetShadowOffset( void );

  // ライティング用設定
  void InitLight( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *pDrawEnvNode, u32 offset ){ m_pDrawEnvNode = pDrawEnvNode; m_nOffsetIndex = offset; }
  void SetEnableAmbientColor( bool enable );
  bool IsEnableAmbientColor( void );
  void SetAmbientColor( const gfl2::math::Vector4 &color );

  // AdjustHeight
  void SetFieldAdjustHeight( void );

  // 3DObject派生用：非推奨
  virtual void SetVisible( bool isVisible ){ SetDisp( isVisible ); }
  virtual bool GetVisible( void ) const{ return IsDisp(); }
  virtual void SetLocalSRT( const gfl2::math::SRT &srt );
  virtual const gfl2::math::SRT &GetLocalSRT( void ) const;
  virtual u32 GetModelInstanceNum( void ) const;
  virtual const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *GetModelInstance( int index = 0 ) const;
  virtual void ExecuteTerminateRequest( void );
  virtual bool IsCanTerminate( void );

protected:
  PokeTool::PokeModel                 *m_pModel;
  Field::MyRenderingPipeLine          *m_pRenderingPipeLine;
  void                                *m_pMotPack;
  ShadowData                          *m_pShadowData;
  gfl2::animation::AnimationPackList  m_AnimationPackList;
  bool                                m_bShadowVisible;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode  *m_pDrawEnvNode;
  u32                                                       m_nOffsetIndex;
#if PM_DEBUG
public:
  //! デバッグ用：影サイズ調整
  void DebugFunc_SetShadowScale( f32 scale );
  //! デバッグ用：影オフセット調整
  void DebugFunc_SetShadowOffset( f32 x, f32 z );
#endif
};

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_TRIALMODEL_H_INCLUDED__
