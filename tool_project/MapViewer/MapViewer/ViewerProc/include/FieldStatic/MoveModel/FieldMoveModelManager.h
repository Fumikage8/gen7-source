#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModelManager.h
*  @brief  フィールド用 動作モデルマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include <renderingengine/include/SceneGraph/Instance/gfl2_InstanceNode.h>
#include "../../../../../../git_program/poke_3d_lib/model/include/gfl2_CharaModelFactory.h"
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel.h"
#include "ViewerProc/include/FieldRo/FieldRootNode.h"

#include "ViewerProc/include/ReadViewerData.h"

namespace Field
{
  class MyRenderingPipeLine;
}

namespace Field{ namespace MoveModel {

class FieldMoveModelManager
{
public:

  enum CREATE_STATE
  {
    CREATE_SUCCESS,
    CREATE_ERROR,
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveModelManager();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveModelManager();

  //----------------------------------------------------------------------------
  /**
  *  @brief  初期化(ワーク)
  *
  *  @param  pInstanceRootNode    ツリーを形成するためのルートノード
  *  @param  pHeap                ワークの作成に利用されるヒープ
  *
  *  @note   ここで利用されるヒープのアドレスは内部で保持してはいけません。どのアプリか関係なくいつもいきているためゲーム起動時のみ利用可
  */
  //-----------------------------------------------------------------------------
  void InitializeWork(gfl2::renderingengine::scenegraph::instance::InstanceNode* pInstanceRootNode,
                      gfl2::heap::HeapBase*                                      pHeap);

  //----------------------------------------------------------------------------
  /**
  *  @brief  初期化(リソース)
  *
  *  @param  MyRenderingPipeLine  レンダリングパイプライン
  *  @param  pAllocator           アロケータ
  *  @param  pHeap                ヒープ
  *  @param  pFileManager         ファイルマネージャー
  */
  //-----------------------------------------------------------------------------
  void InitializeResource(Field::MyRenderingPipeLine* pMyRenderingPipeLine,
                          gfl2::gfx::IGLAllocator*    pAllocator,
                          gfl2::heap::HeapBase*       pHeap,
                          gfl2::fs::AsyncFileManager* pFileManager );

  //----------------------------------------------------------------------------
  /**
  *  @brief  解放(ワーク)
  */
  //-----------------------------------------------------------------------------
  void TerminateWork(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  解放(リソース)
  */
  //-----------------------------------------------------------------------------
  void TerminateResource(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  前更新(コリジョン以外の座標/姿勢の変更を行う)
  */
  //-----------------------------------------------------------------------------
  virtual void PrevUpdate(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  更新(コリジョンシーンへの座標/姿勢の登録を行う)
  */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  後更新(コリジョンシーンからの結果を適用し、最終的な座標/姿勢の決定/イベントの実行を行う)
  */
  //-----------------------------------------------------------------------------
  virtual void PostUpdate(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  使われていない動作モデルの要素を返す 
  *
  *  @return 動作モデルの要素(FIELD_MOVE_MODEL_MAXの場合はエラー)
  */
  //-----------------------------------------------------------------------------
  u32 GetFieldMoveModelIndexFromFreeSpace(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベントID(識別用のユニークなID)から動作モデルの要素を返す 
  *
  *  @param  eventId               イベントID
  *  @return 動作モデルの要素(FIELD_MOVE_MODEL_MAXの場合はエラー)
  */
  //-----------------------------------------------------------------------------
  u32 GetFieldMoveModelIndexFromEventId(u32 eventId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルのワーク作成
  *
  *  @param  fieldMoveModelId       動作モデルID
  *  @param  pFieldMoveModelHeader  動作モデルリクエスト
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  CREATE_STATE CreateWork(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルのリソース作成
  *
  *  @param  fieldMoveModelId       動作モデルID
  *  @param  pFieldMoveModelHeader  動作モデルリクエスト
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  CREATE_STATE CreateResource(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルの削除
  *
  *  @param  fieldMoveModelId  動作モデルID
  */
  //-----------------------------------------------------------------------------
  void Delete(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIエディタに自身のインスタンスを登録
  *
  *  @param  エディタに表示したい動作モデルID
  */
  //-----------------------------------------------------------------------------
  void CreateRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIエディタから自身のインスタンスを削除
  *
  *  @param  エディタから削除したい動作モデルID
  */
  //-----------------------------------------------------------------------------
  void DeleteRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルの取得
  *
  *  @param  fieldMoveModelId  動作モデルID
  *  @return 動作モデル
  */
  //-----------------------------------------------------------------------------
  FieldMoveModel* GetFieldMoveModel(FIELD_MOVE_MODEL_ID fieldMoveModelId){ return &m_fieldMoveModel[ static_cast<u32>(fieldMoveModelId) ]; }
  FieldMoveModel* GetFieldMoveModel(u32                 fieldMoveModelId){ return &m_fieldMoveModel[                  fieldMoveModelId  ]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  有効な全動作モデルをイベント状態(アクションコマンド、動作コードを共に動かさない)にする
  */
  //-----------------------------------------------------------------------------
  void OnEventStateAll(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  有効な全動作モデルをイベント状態(アクションコマンド、動作コードを共に動かさない)から解除する
  */
  //-----------------------------------------------------------------------------
  void OffEventStateAll(void);

  //=====================================================================================================================
  //! @brief poke_3d::model::CharaModelManagerのラッパー部分
  //=====================================================================================================================
#if 0
  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースの非同期ロード開始
  *
  *  @param  characterId       キャラクターID
  */
  //-----------------------------------------------------------------------------
  void LoadAsync(u32 characterId){ m_charaModelFactory.LoadModelAsync(characterId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  着せ替えパーツリソースの非同期ロード開始
  *
  *  @param  dressUpParam      着せ替えパラメータ
  */
  //-----------------------------------------------------------------------------
  void LoadDressUpPartsAsync(const poke_3d::model::DressUpParam& dressUpParam){ m_charaModelFactory.LoadDressUpPartsAsync(dressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースが非同期ロード中かどうか
  *
  *  @param  characterId       キャラクターID
  *  @return trueは成功、falseは失敗
  */
  //-----------------------------------------------------------------------------
  bool IsLoaded(u32 characterId){ return m_charaModelFactory.IsModelLoaded(characterId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定着せ替えパーツリソースが非同期ロード中かどうか
  *
  *  @param  dressUpParam      着せ替えパラメータ
  *  @return trueは成功、falseは失敗
  */
  //-----------------------------------------------------------------------------
  bool IsDressUpPartsLoaded(const poke_3d::model::DressUpParam& dressUpParam){ return m_charaModelFactory.IsDressUpPartsLoaded(dressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースの破棄
  *
  *  @param  characterId       キャラクターID
  */
  //-----------------------------------------------------------------------------
  void Unload(u32 characterId){ m_charaModelFactory.UnloadModel(characterId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定着せ替えパーツリソースの破棄
  *
  *  @param  dressUpParam      着せ替えパラメータ
  */
  //-----------------------------------------------------------------------------
  void UnloadDressUpParts(const poke_3d::model::DressUpParam& dressUpParam){ m_charaModelFactory.UnloadDressUpParts(dressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  キャラクターリソースの全破棄
  */
  //-----------------------------------------------------------------------------
  void UnloadAll(void){ m_charaModelFactory.UnloadModelAll(); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  着せ替えパーツリソースの全破棄
  */
  //-----------------------------------------------------------------------------
  void UnloadDressUpPartsAll(void){ m_charaModelFactory.UnloadDressUpPartsAll(); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースノードの作成
  *
  *  @param  characterId       キャラクターID
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  bool SetupModel(u32 characterId){ return m_charaModelFactory.SetupModel( m_pAllocator , characterId ); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定着せ替えパーツリソースノードの作成
  *
  *  @param  pDressUpParam     着せ替えパラメータ
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  bool SetupDressUpParts(poke_3d::model::DressUpParam* pDressUpParam){ return m_charaModelFactory.SetupDressUpParts( m_pAllocator , *pDressUpParam ); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief 非常駐アニメーションパックの非同期読み込み開始
  *
  *  @param dynamicAnimationId 読み込む非常駐アニメーションID
  */
  //-----------------------------------------------------------------------------
  void LoadDynamicAnimationAsync( u32 dynamicAnimationId ){ m_charaModelFactory.LoadDynamicAnimationAsync(dynamicAnimationId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief 非常駐アニメーションパックの非同期読み込みの完了確認
  *
  *  @param dynamicAnimationId 確認する非常駐アニメーションID
  *  @return 読み込み完了していたらtrue, まだならfalse
  */
  //-----------------------------------------------------------------------------
  bool IsDynamicAnimationLoaded( u32 dynamicAnimationId ){ return m_charaModelFactory.IsDynamicAnimationLoaded(dynamicAnimationId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief 非常駐アニメーションパックのセットアップ
  *
  *  @param dynamicAnimationId セットアップする非常駐アニメーションID
  */
  //-----------------------------------------------------------------------------
  void SetupDynamicAnimation( u32 dynamicAnimationId ){ m_charaModelFactory.SetupDynamicAnimation(m_pAllocator, m_pHeap, dynamicAnimationId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief 非常駐アニメーションパックのアンロード
  *
  *  @param dynamicAnimationId アンロードする非常駐アニメーションID
  */
  //-----------------------------------------------------------------------------
  void UnloadDynamicAnimation( u32 dynamicAnimationId ){ m_charaModelFactory.UnloadDynamicAnimation(dynamicAnimationId); }
#endif //@saito_del
private:
  
  //=====================================================================================================================
  //! @brief 内部
  //=====================================================================================================================

  // シーングラフ用
  gfl2::renderingengine::scenegraph::instance::InstanceNode* m_pInstanceRootNode;
  FieldMoveModel                    m_fieldMoveModelRoot;
  FieldMoveModel                    m_fieldMoveModel[FIELD_MOVE_MODEL_MAX];
  Field::MyRenderingPipeLine       *m_pMyRenderingPipeLine;

  // キャラクター描画用インスタンスのファクトリー用
  gfl2::gfx::IGLAllocator          *m_pAllocator;
  gfl2::heap::HeapBase             *m_pHeap;
  gfl2::fs::AsyncFileManager       *m_pFileManager;
  ///poke_3d::model::CharaModelFactory m_charaModelFactory;

	GLResourceAllocator m_ResourceAllocator;

};

}; //end of namespace MoveModel
}; //end of namespace Field
