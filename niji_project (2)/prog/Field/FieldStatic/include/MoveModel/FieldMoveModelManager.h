#ifndef __FIELDMOVEMODELMANAGER_H__
#define __FIELDMOVEMODELMANAGER_H__

#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModelManager.h
*  @brief  フィールド用 動作モデルマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <util/include/gfl2_GLHeapAllocator.h>
#include "../../../../../../poke_3d_lib/model/include/gfl2_CharaModelFactory.h"
#include "../../include/MoveModel/FieldMoveModel.h"
#include "../../include/MoveModel/FieldMoveModelPlayer.h"
#include "../../include/MoveModel/FieldSubobject.h"
#include "Field/FieldRo/include/FieldRootNode.h"
#include <System/include/DressUp.h>

namespace Field
{
  class MyRenderingPipeLine;
}
namespace Savedata
{
  class FieldMoveModelSave;
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

  enum DYNAMIC_RES_LOAD_INDEX
  {
     DYNAMIC_RES_LOAD_INDEX_RIDE = 0,
     DYNAMIC_RES_LOAD_INDEX_SUBOBJECT,
     DYNAMIC_RES_LOAD_INDEX_EVENT,
     DYNAMIC_RES_LOAD_INDEX_MAX,
  };

  enum TRAFFIC_CULLING_PRIORITY
  {
    TRAFFIC_CULLING_PRIORITY_MIDDLE,
    TRAFFIC_CULLING_PRIORITY_HIGH,
  };

  static const s32 DEFAULT_DISP_MAX_CNT = 8;  // 最大表示数(プレイヤーとライドポケモンは除く)

  // 挙動設定( ※momiji追加 )
  struct SETTING
  {
    f32 cullingBoxSizeHalf;             // 通常カリングBOXのサイズ
    f32 cullingBoxSizeHalfForTrainer;   // トレーナー用カリングBOXのサイズ
    f32 cullingDispLength;              // 距離
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief コンストラクタ
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
  *  @brief  初期化(マネージャーのワーク)
  *
  *  @param  pInstanceRootNode    ツリーを形成するためのルートノード
  *  @param  pHeap                ワークの作成に利用されるヒープ
  *  @param  pAllocator           ワークで作成に利用されるアロケータ
  *
  *  @note   ここで利用されるヒープ/アロケータのアドレスは内部で保持してはいけません。どのアプリか関係なくいつもいきているためゲーム起動時のみ利用可
  */
  //-----------------------------------------------------------------------------
  void InitializeManagerWork(gfl2::renderingengine::scenegraph::instance::InstanceNode* pInstanceRootNode,
                             gfl2::heap::HeapBase*                                      pHeap,
                             gfl2::gfx::IGLAllocator*                                   pAllocator);

  //----------------------------------------------------------------------------
  /**
  *  @brief  初期化(マネージャーのリソース)
  *
  *  @param  MyRenderingPipeLine  レンダリングパイプライン
  *  @param  pAllocator           アロケータ
  *  @param  pHeap                ヒープ
  *  @param  pFileManager         ファイルマネージャー
  *  @param  dressUpParam         着せ替えパラメーター
  *  @param  isExecuteJoinFesta   ジョインフェスタが起動中かどうか
  *  @param  isExecuteBattleInst  バトル施設が起動中かどうか
  *  @param  isExecuteBattleFes   バトルフェスが起動中かどうか
  *  @param  isNpcHeapLargeZone   メモリ使用量の大きい動作モデルを配置した世界どうか（例：アクジキングの世界）
  */
  //-----------------------------------------------------------------------------
  void InitializeManagerResource(Field::MyRenderingPipeLine*    pMyRenderingPipeLine,
                                  gfl2::gfx::IGLAllocator*      pAllocator,
                                  gfl2::heap::HeapBase*         pHeap,
                                  gfl2::fs::AsyncFileManager*   pFileManager,
                                  poke_3d::model::DressUpParam  dressUpParam,
                                  b32                           isExecuteJoinFesta,
                                  b32                           isExecuteBattleInst,
                                  b32                           isExecuteBattleFes,
                                  b32                           isNpcHeapLargeZone );

  //----------------------------------------------------------------------------
  /**
  *  @brief  解放(マネージャーのワーク)
  */
  //-----------------------------------------------------------------------------
  void TerminateManagerWork(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  解放(マネージャーのリソース)
  */
  //-----------------------------------------------------------------------------
  void TerminateManagerResource(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化(管理している全ての動作モデルのワーク)
  *
  *  @param  fieldMoveModelId       動作モデルID
  *  @param  pFieldMoveModelHeader  動作モデルリクエスト
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  CREATE_STATE InitializeMoveModelWork(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeaderWork* pFieldMoveModelHeaderWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化(管理している全ての動作モデルのリソース)
  *
  *  @param  fieldMoveModelId       動作モデルID
  *  @param  pFieldMoveModelHeader  動作モデルリクエスト
  *  @param  pDressUpModelResourceManager    着せ替えリソースマネージャ（通常はNULLでOK。ジョインフェスタのみ渡す。)
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  CREATE_STATE InitializeMoveModelResource(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeaderResource* pFieldMoveModelHeaderResource, poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager = NULL);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化(管理している全てのサブオブジェクト)
  *
  *  @param  fieldSubobjectId       サブオブジェクトID
  *  @param  pFieldSubobjectHeader  サブオブジェクトリクエスト
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  CREATE_STATE InitializeSubobject(FIELD_SUB_OBJECT_ID fieldSubobjectId,const FieldSubobjectHeader* pFieldSubobjectHeader);

  //----------------------------------------------------------------------------
  /**
  *  @brief  解放(管理している全ての動作モデルのワーク)
  */
  //-----------------------------------------------------------------------------
  void TerminateMoveModelWork(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  解放(管理している全ての動作モデルのリソース)
  */
  //-----------------------------------------------------------------------------
  void TerminateMoveModelResource(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  解放(管理している全てのサブオブジェクト)
  */
  //-----------------------------------------------------------------------------
  void TerminateSubobject(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定動作モデルの削除(リソースのみ)
  *
  *  @param  fieldMoveModelId  動作モデルID
  */
  //-----------------------------------------------------------------------------
  void TerminateMoveModelResource(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定動作モデルの削除(リソース/ワーク共に)
  *
  *  @param  fieldMoveModelId  動作モデルID
  */
  //-----------------------------------------------------------------------------
  void TerminateMoveModelWorkResource(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定サブオブジェクトの削除
  *
  *  @param  fieldSubobjectId  サブオブジェクトID
  */
  //-----------------------------------------------------------------------------
  void TerminateSubobject(FIELD_SUB_OBJECT_ID fieldSubobjectId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化(着せ替えマネージャー)
  *
  *  @param  pFileManager  ファイルマネージャー
  */
  //-----------------------------------------------------------------------------
  void InitializeDressUpResourceManager(gfl2::fs::AsyncFileManager* pFileManager);

  //----------------------------------------------------------------------------
  /**
  *  @brief  解放(着せ替えマネージャー)
  */
  //-----------------------------------------------------------------------------
  void TerminateDressUpResourceManager(void);

public:
  //-----------------------------------------------------------------------------
  /**
  *  @brief  前更新(コリジョン以外の座標/姿勢の変更を行う)
  */
  //-----------------------------------------------------------------------------
  virtual void PrevUpdate(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  後更新(座標の決定後に必要な処理を行う)
  */
  //-----------------------------------------------------------------------------
  virtual void PostUpdate();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリング
  *
  *  @param  ビュープロジェクション行列
  *  @param  カメラ座標
  */
  //-----------------------------------------------------------------------------
  virtual void UpdateCulling(gfl2::math::Matrix44 viewProj, gfl2::math::Vector3 cameraPos, b32 isFieldCamera, s32 zoneId );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクト更新
  */
  //-----------------------------------------------------------------------------
  void UpdateSubobject(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  最大表示数(プレイヤーとライドポケモンは除く)を変更する
  *
  *  @param  最大表示数(プレイヤーとライドポケモンは除く)
  */
  //-----------------------------------------------------------------------------
  void SetDispMaxCnt(u32 dispMaxCnt){ m_dispMaxCnt = dispMaxCnt; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  往来のカリング優先順位を変更する
  */
  //-----------------------------------------------------------------------------
  void SetTrafficCullingPriority(TRAFFIC_CULLING_PRIORITY priority){ m_trafficCullingPriority = (u8)priority; }

private:
  //-----------------------------------------------------------------------------
  /**
  *  @brief  カメラビューから一番近い動作モデルのインデックスを返す
  *
  *  @param  カメラ座標
  *  @param  動作モデル配列
  *  @param  対象とする描画状況
  *  @param  往来を無視するかどうか
  */
  //-----------------------------------------------------------------------------
  s32 GetNearestMoveModelFromCamera(gfl2::math::Vector3 cameraPos,FieldMoveModel** ppFieldMoveModelSrc,b32 isTargetVisible,b32 isIgnoreTraffic);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  有効な動作モデル(視線トレーナー)のインデックスを返す
  *
  *  @param  動作モデル配列
  *  @param  動作モデル配列の描画動作状況
  */
  //-----------------------------------------------------------------------------
  s32 GetEnableTrainerFromPlayer(FieldMoveModel** ppFieldMoveModelSrc,b32 isVisibleActive);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリングする距離を取得する
  */
  //-----------------------------------------------------------------------------
  f32 GetCullingDispLength( b32 isFieldCamera, s32 zoneId ) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリング判定用BOXサイズを取得する
  */
  //-----------------------------------------------------------------------------
  f32 GetCullingBoxSizeHalfForTrainer(b32 isFieldCamera, s32 zoneId) const;

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  地面にキャラを引っ付ける
  */
  //-----------------------------------------------------------------------------
  virtual void AdjustGround(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルの座標/姿勢をダイナミックアクターの座標/姿勢に適用する
  */
  //-----------------------------------------------------------------------------
  virtual void ApplyMoveModelToActor(FieldMoveModelCollisionType       colType);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルの座標/姿勢をスタティックアクターの座標/姿勢に適用する
  */
  //-----------------------------------------------------------------------------
  virtual void ApplyMoveModelToActor(FieldMoveModelStaticCollisionType colType);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ダイナミックアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
  */
  //-----------------------------------------------------------------------------
  virtual void ApplyActorToMoveModel(FieldMoveModelCollisionType colType);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  全てのコリジョンアクターが衝突を解決しているかどうか
  */
  //-----------------------------------------------------------------------------
  virtual void CheckCollisionSolved(void);

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
  *  @brief  使われていないイベント用動作モデルの要素を返す 
  *
  *  @return 動作モデルの要素(FIELD_MOVE_MODEL_MAXの場合はエラー)
  */
  //-----------------------------------------------------------------------------
  u32 GetFieldMoveEventModelIndexFromFreeSpace(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  使われていないサブオブジェクトの要素を返す 
  *
  *  @return 動作モデルの要素(FIELD_SUB_OBJECT_MAXの場合はエラー)
  */
  //-----------------------------------------------------------------------------
  u32 GetFieldSubobjectIndexFromFreeSpace(void);

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
  *  @brief  動作モデルの取得
  *
  *  @param  fieldMoveModelId  動作モデルID
  *  @return 動作モデル
  */
  //-----------------------------------------------------------------------------
  FieldMoveModel* GetFieldMoveModel(FIELD_MOVE_MODEL_ID fieldMoveModelId)
  {
    if( fieldMoveModelId < FIELD_MOVE_MODEL_MAX )
    {
      return m_pFieldMoveModel[ static_cast<u32>(fieldMoveModelId) ]; 
    }
    else
    {
      GFL_ASSERT_MSG( 0, "GetFieldMoveModel Id Overflow\n" );
      return NULL;
    }
  }
  FieldMoveModel* GetFieldMoveModel(u32                 fieldMoveModelId)
  {
    if( fieldMoveModelId < FIELD_MOVE_MODEL_MAX )
    {
      return m_pFieldMoveModel[                  fieldMoveModelId  ]; 
    }
    else
    {
      GFL_ASSERT_MSG( 0, "GetFieldMoveModel Id Overflow\n" );
      return NULL;
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクトの取得
  *
  *  @param  fieldSubobjectId  サブオブジェクトID
  *  @return サブオブジェクト
  */
  //-----------------------------------------------------------------------------
  FieldSubobject* GetFieldSubobject(FIELD_SUB_OBJECT_ID fieldSubobjectId)
  {
    if( fieldSubobjectId < FIELD_SUB_OBJECT_MAX )
    {
      return m_pFieldSubobject[ static_cast<u32>(fieldSubobjectId) ]; 
    }
    else
    {
      GFL_ASSERT_MSG( 0, "GetFieldSubobject Id Overflow\n" );
      return NULL;
    }
  }
  FieldSubobject* GetFieldSubobject(u32                 fieldSubobjectId)
  {
    if( fieldSubobjectId < FIELD_SUB_OBJECT_MAX )
    {
      return m_pFieldSubobject[                  fieldSubobjectId  ]; 
    }
    else
    {
      GFL_ASSERT_MSG( 0, "GetFieldSubobject Id Overflow\n" );
      return NULL;
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルにサブオブジェクトをセットする
  *
  *  @param  pMoveModel               サブオブジェクトをセットしたい動作モデル
  *  @param  setIndex                 動作モデルの何番目のサブオブジェクトとするか
  *  @param  header                   動作モデルにセットしたいサブオブジェクトのヘッダー
  *  @param  isManual                 専用ワークを使うかどうか
  *  @return trueならセット成功、falseならセット失敗
  */
  //-----------------------------------------------------------------------------
  b32 SetSubobjectToMoveModel(FieldMoveModel* pMoveModel,u32 setIndex,Field::MoveModel::FieldSubobjectHeader* pHeader,b32 isManual);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  有効な全動作モデルをイベント状態(アクションコマンド、動作コードを共に動かさない)にする
  */
  //-----------------------------------------------------------------------------
  void AddEventRequestAll(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  有効な全動作モデルをイベント状態(アクションコマンド、動作コードを共に動かさない)から解除する
  */
  //-----------------------------------------------------------------------------
  void PullEventRequestAll(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルにイベントリクエストを適用
  */
  //-----------------------------------------------------------------------------
  void ApplyEventRequest(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  プレイヤーのモーションIDをライド状況にあったモーションIDに変換する
  *
  *  @param  プレイヤーのモーションID
  *  @param  ライドポケモンのキャラID
  *  @param  指定ライドポケモンに合ったモーションID
  *  @param  指定ライドポケモンに合ったモーションIDの再生終了フレーム
  */
  //-----------------------------------------------------------------------------
  void ConvertPlayerMotionIdToRideMotionId(u32 playerMotionId,u32 rideCharaId,u32* pOutRideMotionId,s32* pOutRideMotionEndFrame);

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
  *  @brief  動作モデルの状況をセーブデータに書き込む
  *
  *  @param  セーブデータ
  */
  //-----------------------------------------------------------------------------
  void SerializeSaveData( Savedata::FieldMoveModelSave* pFieldMoveModelSave );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルの状況をセーブデータから受け取る
  *
  *  @param  セーブデータ
  */
  //-----------------------------------------------------------------------------
  void DeserializeSaveData( Savedata::FieldMoveModelSave* pFieldMoveModelSave );

  //=====================================================================================================================
  //! @brief poke_3d::model::CharaModelManagerのラッパー部分
  //=====================================================================================================================

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースの設定
  *
  *  @param  characterId       キャラクターID
  */
  //-----------------------------------------------------------------------------
  void Set(u32 characterId, void * pData){ m_charaModelFactory.SetModel(characterId, pData); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースの非同期ロード開始
  *
  *  @param  characterId       キャラクターID
  */
  //-----------------------------------------------------------------------------
  void LoadStaticAsync(u32 characterId){ m_charaModelFactory.LoadModelAsync(characterId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースの非同期ロード開始
  *
  *  @param pLoadHeap 読み込みヒープ
  *  @param  characterId       キャラクターID
  */
  //-----------------------------------------------------------------------------
  void LoadStaticAsync(gfl2::heap::HeapBase * pLoadHeap, u32 characterId){ m_charaModelFactory.LoadModelAsync(pLoadHeap,characterId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースの非同期ロード開始  動的確保版
  *
  *  @param  dynamic_load_index ダイナミック読み込みインデックス
  *  @param  characterId       キャラクターID
  */
  //-----------------------------------------------------------------------------
  void LoadDynamicAsync(DYNAMIC_RES_LOAD_INDEX dynamic_load_index, u32 characterId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  着せ替えパーツリソースの非同期ロード開始
  *
  *  @param  dressUpParam      着せ替えパラメータ
  */
  //-----------------------------------------------------------------------------
  void LoadDressUpPartsAsync(const poke_3d::model::DressUpParam& dressUpParam)
  {
    if( !m_dressUpModelResourceManager.IsDressUpPartsLoaded(dressUpParam) )
    {
      m_dressUpModelResourceManager.LoadDressUpPartsAsync(dressUpParam);
    }
  }

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
  bool IsDressUpPartsLoaded(const poke_3d::model::DressUpParam& dressUpParam){ return m_dressUpModelResourceManager.IsDressUpPartsLoaded(dressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースの破棄
  *
  *  @param  characterId       キャラクターID
  */
  //-----------------------------------------------------------------------------
  void Unload(u32 characterId)
  {
    for( s32 i=0; i<DYNAMIC_RES_LOAD_INDEX_MAX ; i++ )
    {
      if( m_pLocalAllocatorForIgnoreID[i] >= 0 && m_pLocalAllocatorForIgnoreID[i] == (s32)characterId )
      {
        m_pLocalAllocatorForIgnoreID[i] = -1;
        return;
      }
    }
    m_charaModelFactory.UnloadModel(characterId);
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定着せ替えパーツリソースの破棄
  *
  *  @param  dressUpParam      着せ替えパラメータ
  */
  //-----------------------------------------------------------------------------
  void UnloadDressUpParts(const poke_3d::model::DressUpParam& dressUpParam){ m_dressUpModelResourceManager.UnloadDressUpParts(dressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  * @brief 指定着せ替えパーツリソースを開放できるか確認
  *
  * @param dressUpParam 着せ替えパラメータ
  *
  * @return true:解放できる false:解放できない
  */
  //-----------------------------------------------------------------------------
  b32 CanUnloadDressUpParts(const poke_3d::model::DressUpParam& dressUpParam){ return m_dressUpModelResourceManager.CanUnloadDressUpParts(dressUpParam); }

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
  void UnloadDressUpPartsAll(void){ m_dressUpModelResourceManager.UnloadDressUpPartsAll(); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定キャラクターリソースノードの作成
  *
  *  @param  characterId       キャラクターID
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  bool SetupModel(u32 characterId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定着せ替えパーツリソースノードの作成
  *
  *  @param  pDressUpParam     着せ替えパラメータ
  *  @return 成否
  */
  //-----------------------------------------------------------------------------
  bool SetupDressUpParts(const poke_3d::model::DressUpParam* pDressUpParam)
  {
    if( m_dressUpModelResourceManager.IsSetupDressUpParts(*pDressUpParam) )
    {
      return true;
    }
    return m_dressUpModelResourceManager.SetupDressUpParts( m_pLocalAllocatorForDressModelResource , *pDressUpParam );
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief 非常駐アニメーションパックの非同期読み込み開始
  *
  *  @param pLoadHeap 読み込みヒープ
  *  @param dynamicAnimationId 読み込む非常駐アニメーションID
  */
  //-----------------------------------------------------------------------------
  void LoadDynamicAnimationAsync( gfl2::heap::HeapBase* pLoadHeap,u32 dynamicAnimationId ){ m_charaModelFactory.LoadDynamicAnimationAsync(pLoadHeap,dynamicAnimationId); }

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
  *  @param pGfxAllocator Gfxライブラリ用のアロケータ
  *  @param pLoadHeap 読み込みヒープ
  *  @param dynamicAnimationId セットアップする非常駐アニメーションID
  */
  //-----------------------------------------------------------------------------
  void SetupDynamicAnimation( gfl2::gfx::IGLAllocator* pGfxAllocator,gfl2::heap::HeapBase* pLoadHeap,u32 dynamicAnimationId ){ m_charaModelFactory.SetupDynamicAnimation(pGfxAllocator, pLoadHeap, dynamicAnimationId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief 非常駐アニメーションパックのアンロード
  *
  *  @param dynamicAnimationId アンロードする非常駐アニメーションID
  */
  //-----------------------------------------------------------------------------
  void UnloadDynamicAnimation( u32 dynamicAnimationId ){ m_charaModelFactory.UnloadDynamicAnimation(dynamicAnimationId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief CHARA.garcのアプリケーションデータを取得
  *
  *  @param  characterId キャラクターID
  *  @return バイナリデータ
  */
  //-----------------------------------------------------------------------------
  const ApplicationDataForCommon* GetApplicationData( u32 characterId ){ return (const ApplicationDataForCommon*)m_charaModelFactory.GetApplicationData( characterId ); }

  //-----------------------------------------------------------------------------
  /**
  * @brief 女主人公のデフォルトの着せ替え組み合わせ取得
  *
  * @param pOutDressUpParam 書き込み先の着せ替えパラメータ
  */
  //-----------------------------------------------------------------------------
  void GetDressUpParamDefaultHeroine( poke_3d::model::DressUpParam* pOutDressUpParam ){ System::DressUp::GetDressUpParamDefaultHeroine(pOutDressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  * @brief 男主人公のデフォルトの着せ替え組み合わせ取得
  *
  * @param pOutDressUpParam 書き込み先の着せ替えパラメータ
  */
  //-----------------------------------------------------------------------------
  void GetDressUpParamDefaultHero( poke_3d::model::DressUpParam* pOutDressUpParam ){ System::DressUp::GetDressUpParamDefaultHero(pOutDressUpParam); }

  //---------------------------------------------------------------------------------
  // ■ 着せ替え非常駐アニメーション

  /**
   * @brief 着せ替え非常駐アニメーションパックの同期読み込み
   *
   * @param pLoadHeap 読み込みヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationSync(gfl2::heap::HeapBase* pLoadHeap,const poke_3d::model::DressUpParam& dressUpParam, u32 dynamicAnimationId)
  {
    m_dressUpModelResourceManager.LoadDressUpDynamicAnimationSync( pLoadHeap, dressUpParam, dynamicAnimationId ); 
    m_charaModelFactory.LoadDynamicAnimationSync( pLoadHeap, dynamicAnimationId );
  }

  /**
   * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
   *
   * @param pLoadHeap 読み込みヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationAsync(gfl2::heap::HeapBase* pLoadHeap,const poke_3d::model::DressUpParam& dressUpParam, u32 dynamicAnimationId)
  {
    m_dressUpModelResourceManager.LoadDressUpDynamicAnimationAsync( pLoadHeap, dressUpParam, dynamicAnimationId );
    m_charaModelFactory.LoadDynamicAnimationAsync( pLoadHeap, dynamicAnimationId );
  }

  /**
   * @brief 着せ替え非常駐アニメーションパックの非同期読み込みの完了確認
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 確認する非常駐アニメーションID
   *
   * @return 読み込み完了していたらtrue, まだならfalse
   */
  bool IsDressUpDynamicAnimationLoaded(const poke_3d::model::DressUpParam& dressUpParam, u32 dynamicAnimationId)
  {
    return m_dressUpModelResourceManager.IsDressUpDynamicAnimationLoaded( dressUpParam, dynamicAnimationId ) && m_charaModelFactory.IsDynamicAnimationLoaded( dynamicAnimationId ); 
  }

  /**
   * @brief 着せ替え読み込んだ非常駐アニメーションパックを開放する
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 開放する非常駐アニメーションID
   */
  void UnloadDressUpDynamicAnimation(const poke_3d::model::DressUpParam& dressUpParam, u32 dynamicAnimationId)
  {
    m_dressUpModelResourceManager.UnloadDressUpDynamicAnimation( dressUpParam, dynamicAnimationId); 
    m_charaModelFactory.UnloadDynamicAnimation( dynamicAnimationId );
  }

  /**
   * @brief 着せ替え非常駐アニメーションパックのセットアップ
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pLoadHeap 読み込みヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId セットアップする非常駐アニメーションID
   */
  void SetupDressUpDynamicAnimation(gfl2::gfx::IGLAllocator* pGfxAllocator,gfl2::heap::HeapBase* pLoadHeap,const poke_3d::model::DressUpParam& dressUpParam, u32 dynamicAnimationId)
  {
    m_dressUpModelResourceManager.SetupDressUpDynamicAnimation( pGfxAllocator, pLoadHeap, dressUpParam, dynamicAnimationId); 
    m_charaModelFactory.SetupDynamicAnimation( pGfxAllocator, pLoadHeap, dynamicAnimationId );
  }

  /**
  * @brief カリングを行う対象か取得します
  *
  * @param index チェック対象となる動作モデルのデータindexです
  */
  b32 IsCullingTarget(s32 index) const;


  FieldMoveModel* GetInterestFureaiPokemon( void );

#if PM_DEBUG
  //-----------------------------------------------------------------------------
  /**
  * @brief 着せ替えパラメータのランダム取得
  *
  * @param pOutDressUpParam 受け取る着せ替えパラメータ
  */
  //-----------------------------------------------------------------------------
  void GetDressUpParamRand(poke_3d::model::DressUpParam* pOutDressUpParam);
  /* ------------------------------------------------------------------------- */
  /**
  * @brief  イベント終了時の座標チェック機能を利用するかどうか取得
  *
  * @return trueは利用する、falseなら利用しない
  */
  /* ------------------------------------------------------------------------- */
  static b32 CheckEnablePositionForEventScript( void );

  /**
  * @brief  モデル種類数を取得します
  *
  * @return モデルの種類数
  */
  s32 GetCountOfModelTypes( void ); //const;

  /**
  * @brief  存在する動作モデルの数を取得します
  *
  * @return 動作モデル数
  */
  s32 GetCountOfSetupCompleteModel( void ) const;

  /**
  * @brief  存在する配置データの動作モデルの数を取得します
  *
  * @return 動作モデル数
  */
  s32 GetCountOfSetupCompletePlacementDataModel( void ) const;
  
  /**
  * @brief  配置データの動作モデルの最大数を取得します
  *
  * @return 動作モデル数
  */
  s32 GetCountOfPlacementDataModelMax( void ) const;

  /**
  * @brief  描画中の動作モデルの数を取得します
  *
  * @return 動作モデル数
  */
  s32 GetCountOfDrawingModel( void ) const;
  
  /**
  * @brief  動作モデルの描画制限数を取得します
  *
  * @return 描画制限数
  */
  s32 GetCountOfDispMax( void ) const { return m_dispMaxCnt; }

  /**
  * @brief  描画する動作モデルの数を取得します
  *
  * @return 描画する動作モデルの数
  */
  s32 GetCountOfDrawableModel( void ) const;
  
  /**
  * @brief  カリングターゲットで描画する動作モデルの数を取得します
  *
  * @return 描画する動作モデルの数
  */
  s32 GetCountOfDrawableCullingTragetModel( void ) const;

  /**
  * @brief  カリングターゲットで描画中の動作モデルの数を取得します
  *
  * @return 動作モデル数
  */
  s32 GetCountOfDrawingCullingTargetModel( void ) const;
 
  //----------------------------------------------------------------------------
  /**
  *  @brief  設定データへのポインタを取得
  *  @note   デバッグメニューで操作するため、それ以外では使用禁止
  */
  //-----------------------------------------------------------------------------
  static SETTING* GetSettingData( void ) { return &m_Setting; }

  /**
  * @brief  スクリプト操作があった動作モデルの座標を取得
  */
  void OutPutsEndPositionForEventScript(u16 zoneId, u16 areaId);

  /**
  * @brief  バミリアサートチェック
  */
  void CheckSafetyPositionForEventScript(void);

  /**
  * @brief  アクション終了アサートチェック
  */
  void CheckActionEndForEventScript(void);

#endif
  
  /**
  * @brief  バミリアサートチェック、クリア
  */
  void ClearCheckSafetyPositionForEventScript(void);

private:
  
  //=====================================================================================================================
  //! @brief 内部
  //=====================================================================================================================

  // シーングラフ用
  gfl2::renderingengine::scenegraph::instance::InstanceNode* m_pInstanceRootNode;
  FieldMoveModel                   *m_pFieldMoveModelRoot;
  FieldMoveModel                   *m_pFieldMoveModel[FIELD_MOVE_MODEL_MAX];
  FieldSubobject                   *m_pFieldSubobject[FIELD_SUB_OBJECT_MAX];
  Field::MyRenderingPipeLine       *m_pMyRenderingPipeLine;

  // カリング用
  u32                               m_dispMaxCnt;
  b8                                m_isUseCulling;
  u8                                m_trafficCullingPriority;

  // イベント管理用
  u8                                m_charaEventRequestCnt;
  u8                                m_isPadding;

  // キャラクター描画用インスタンスのファクトリー
  gfl2::gfx::IGLAllocator          *m_pAllocator;
  gfl2::heap::HeapBase             *m_pParentHeap;
  gfl2::heap::HeapBase             *m_pLocalHeapForDressModelResource;
  gfl2::heap::HeapBase             *m_pLocalHeapForModelResource[DYNAMIC_RES_LOAD_INDEX_MAX];
  gfl2::heap::HeapBase             *m_pLocalHeapForMoveModel[FIELD_MOVE_MODEL_MAX];
  gfl2::heap::HeapBase             *m_pLocalHeapForSubobject[FIELD_SUB_OBJECT_MAX];
  System::nijiAllocator            *m_pLocalAllocatorForDressModelResource;
  System::nijiAllocator            *m_pLocalAllocatorForModelResource  [DYNAMIC_RES_LOAD_INDEX_MAX];
  s32                               m_pLocalAllocatorForModelResourceID[DYNAMIC_RES_LOAD_INDEX_MAX];
  s32                               m_pLocalAllocatorForIgnoreID       [DYNAMIC_RES_LOAD_INDEX_MAX];

  poke_3d::model::CharaModelFactory m_charaModelFactory;
  poke_3d::model::DressUpModelResourceManager m_dressUpModelResourceManager;
  b32                                         m_isSetupDressUpModelResourceManager;

  // その他
  struct RideMotionConvertData
  {
    s32 playerMotionId;
    s32 rideMotionId      [FieldMoveModelPlayer::RIDE_POKEMON_MAX_CNT];
    s32 rideMotionEndFrame[FieldMoveModelPlayer::RIDE_POKEMON_MAX_CNT];
  };
  static const u32                  RIDE_MOTION_CONVER_DATA_CNT = 17;


  static SETTING m_Setting;             // 挙動設定、デバッグメニューで操作します ※momiji追加
};

}; //end of namespace MoveModel
}; //end of namespace Field

#endif //__FIELDMOVEMODELMANAGER_H__
