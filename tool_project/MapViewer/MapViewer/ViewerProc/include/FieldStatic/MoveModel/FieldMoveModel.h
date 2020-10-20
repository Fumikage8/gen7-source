#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModel.h
*  @brief  フィールド用 動作モデル
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <model/include/gfl2_CharaModel.h>
#include "ViewerProc/include/FieldStatic/Field3DObjectNode.h"
#include "FieldMoveModel_define.h"
#include "../MoveCode/FieldMoveCode_define.h"
///#include "../ActionCommand/FieldActionCommand_define.h"
#include "../Collision/DynamicActor.h"
#include "../Collision/StaticActor.h"

#include "ViewerProc/include/ReadViewerData.h"

class BaseCollisionScene;

namespace poke_3d { namespace model {
   class CharaModelFactory;
}}

//=================================================================================================
//
// ※注意※  このクラスでは座標/姿勢/拡縮を保持してはいけません。
//           座標/姿勢/拡縮を保持して良いのは描画インスタンス(poke_3d::model::CharaModel)のみです。
//           ただ諸々の処理を簡単/高速に行うために、描画インスタンスと同内容のツリーを形成する必要があります
//
//=================================================================================================
namespace Field{ namespace MoveModel {

class FieldMoveCodeBase;

class FieldMoveModel : public gfl2::renderingengine::scenegraph::NodeTrait<FieldMoveModel,Field::IField3DObjectNode>
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveModel();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveModel();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化(ワーク)
  *
  *  @param  pFieldMoveModelHeader  作成リクエスト
  */
  //-----------------------------------------------------------------------------
  virtual void SetupWork(const FieldMoveModelHeader * pFieldMoveModelHeader);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化(リソース)
  *
  *  @param  pAllocator             アロケータ
  *  @param  pHeap                  ヒープ
  *  @param  pCharaModelFactory     キャラリソースファクトリー
  *  @param  pFieldMoveModelHeader  作成リクエスト
  */
  //-----------------------------------------------------------------------------
  virtual void SetupResource(gfl2::gfx::IGLAllocator*           pAllocator,
                             gfl2::heap::HeapBase*              pHeap,
                             ///poke_3d::model::CharaModelFactory* pCharaModelFactory,
                             const FieldMoveModelHeader*        pFieldMoveModelHeader,
														 GLResourceAllocator* pResourceAllocator);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  解放(ワーク)
  */
  //-----------------------------------------------------------------------------
  void ClearWork(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  解放(リソース)
  */
  //-----------------------------------------------------------------------------
  void ClearResource(void);

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
  *  @brief  初期化しているかどうか
  *
  *  @return trueは初期化済み、falseは未初期化
  */
  //-----------------------------------------------------------------------------
  b32 IsSetup         (void){ if( m_isSetupResource && m_isSetupWork ){return true;} return false; }
  b32 IsSetupWork     (void){ return m_isSetupWork; }
  b32 IsSetupResource (void){ return m_isSetupResource; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デフォルト待機アニメーションの設定
  *
  *  @param  defaultIdleAnimationId  デフォルト待機アニメーション
  */
  //-----------------------------------------------------------------------------
  void SetDefaultIdleAnimationId(u32 defaultIdleAnimationId){ m_defaultIdleAnimationId = defaultIdleAnimationId; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デフォルト待機アニメーションの取得
  *
  *  @return デフォルト待機アニメーション
  */
  //-----------------------------------------------------------------------------
  u32 GetDefaultIdleAnimationId(void){ return m_defaultIdleAnimationId; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  Originジョイントによる移動ベクトルを変換するスケール/クォータニオンを設定(毎フレーム呼ぶ必要があります)
  *          この関数を呼ばなかった場合、スケールには1が使われ、クォータニオンにはCharaModelインスタンスのクォータニオンが使われます
  *
  *  @param  スケール
  *  @param  クォータニオン
  */
  //-----------------------------------------------------------------------------
  void SetCustomValueForOrigin(f32 scale,gfl2::math::Quaternion qua){ m_customScaleForOrigin = scale; m_customQuaternionForOrigin = qua; m_isUseCustomValueForOrigin=true; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  当たる側アクターの取得
  *
  *  @param  index    取得したいアクター
  *  @return アクター
  */
  //-----------------------------------------------------------------------------
  DynamicActor* GetDynamicActor(u32 index){ return m_pDynamicActor[index]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  当たられる側アクターの取得
  *
  *  @param  index    取得したいアクター
  *  @return アクター
  */
  //-----------------------------------------------------------------------------
  StaticActor*  GetStaticActor(u32 index){ return m_pStaticActor[index]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コリジョンシーンコンテナの設定
  *
  *  @param  pCollisionSceneContainer  コリジョンシーン
  *  @note   高さ取得用レイチェックで対象とするシーンです
  */
  //-----------------------------------------------------------------------------
  void SetCollisionSceneContainer(BaseCollisionScene* pCollisionSceneContainer){ m_pCollisionSceneContainer = pCollisionSceneContainer; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  描画インスタンスの取得
  *
  *  @return 描画インスタンス
  */
  //-----------------------------------------------------------------------------
  const poke_3d::model::CharaModel* GetCharaDrawInstance(void) const { return m_pCharaDrawInstance; }
        poke_3d::model::CharaModel* GetCharaDrawInstance(void)       { return m_pCharaDrawInstance; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作ビット  ON
  *
  *  @param  fieldMoveModelStateBit  ビット
  */
  //-----------------------------------------------------------------------------
  void OnStateBit(FieldMoveModelStateBit fieldMoveModelStateBit);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作ビット  OFF
  *
  *  @param  fieldMoveModelStateBit  ビット
  */
  //-----------------------------------------------------------------------------
  void OffStateBit(FieldMoveModelStateBit fieldMoveModelStateBit);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作ビット  チェック
  *
  *  @param  fieldMoveModelStateBit  ビット
  *  @return trueはbitOn、falseはbitOff
  */
  //-----------------------------------------------------------------------------
  bool CheckStateBit(FieldMoveModelStateBit fieldMoveModelStateBit) const;  

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの設定
  *
  *  @param  actionCommand  アクションコマンド
  *  @param  dirY           回転Y
  */
  //-----------------------------------------------------------------------------
  ///void SetActionCommand(FieldActionCommandId actionCommand,f32 dirY=0.0f);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの強制停止
  */
  //-----------------------------------------------------------------------------
  void ClearActionCommand(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ワークからリソースの状態を元に戻す
  */
  //-----------------------------------------------------------------------------
  void ReturnResourceStateFromWork(void);

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  イベントID(識別用のユニークな値)を取得 
  *
  *  @return イベントID(識別用のユニークな値) 
  */ 
  //----------------------------------------------------------------------------- 
  u32 GetEventId(void){ return m_header.eventId; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  地面に吸着させるかどうかを設定
  *
  *  @param  地面に吸着させるかどうか
  */ 
  //----------------------------------------------------------------------------- 
  void SetForceLanding(b32 isForceLanding){ m_isForceLanding = isForceLanding; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  地面に吸着させるかどうかを取得
  *
  *  @return 地面に吸着させるかどうか
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsForceLanding(void){ return m_isForceLanding; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  地面に立っているかどうかを取得
  *
  *  @return 地面に立っているかどうか
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsOnGround(void){ return m_isOnGround; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  立っている地面の高さを取得
  *
  *  @return 地面の高さ
  */ 
  //----------------------------------------------------------------------------- 
  f32 GetGroundHeight(void){ return m_groundHeight; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  立っている地面の属性を取得
  *
  *  @return 地面の属性
  */ 
  //----------------------------------------------------------------------------- 
  u32 GetGroundAttribute(void){ return m_groundAttribute; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  1歩分進んだフレームかどうか
  *
  *  @return trueなら1歩分進み、falseなら進まなかったフレーム
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsOneStepFrame(void){ return m_isOneStepFrame; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  旋回したフレームかどうか
  *
  *  @return trueなら旋回、falseなら旋回しなかったフレーム
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsTurnFrame(void){ return m_isTurnFrame; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIエディタに自身のインスタンスを登録
  *
  *  @param  pDispName  エディタに表示したい名前
  */
  //-----------------------------------------------------------------------------
  virtual void CreateRttiEditorPage(c16* pDispName);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIエディタから自身のインスタンスを削除
  */
  //-----------------------------------------------------------------------------
  virtual void DeleteRttiEditorPage(void);

  //================================================================================================================
  // ■ 親のインターフェースの実装
  //================================================================================================================

  //-----------------------------------------------------------------------------
  /**
  *  @brief 表示ON・OFFを設定
  *
  *  @param isVisible  表示ON・OFF
  */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(bool isVisible);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  表示ON・OFFを取得
  *
  *  @return 表示ON・OFF
  */
  //-----------------------------------------------------------------------------
  virtual bool GetVisible(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief SRTを設定
  *
  *  @param srt 設定したいSRT
  */
  //-----------------------------------------------------------------------------
  virtual void SetLocalSRT(const gfl2::math::SRT & srt);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  SRTを取得
  *
  *  @return SRT
  */
  //-----------------------------------------------------------------------------
  virtual const gfl2::math::SRT & GetLocalSRT(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief モデルインスタンス数を取得
  *
  *  @return モデルインスタンス数
  */
  //-----------------------------------------------------------------------------
  virtual u32 GetModelInstanceNum(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief モデルインスタンスを取得
  *
  *  @return モデルインスタンス
  */
  //-----------------------------------------------------------------------------
  virtual const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * GetModelInstance(int index) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief モデルインスタンスの遅延解放は自由に実装
  *
  *  @note:
  *   削除リクエストを受けたモデルインスタンスを解放
  *   -実行タイミング  フィールドプロセスの先頭で実行します。
  *   -制限            PipeLineの描画後にモデルインスタンスの遅延解放リクエストを出さないこと。
  */
  //-----------------------------------------------------------------------------
  virtual void ExecuteTerminateRequest(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  安全に破棄できる常態かチェック
  *
  *  @return 安全に解放できるかどうか
  */
  //-----------------------------------------------------------------------------
  virtual bool IsCanTerminate(void);


	bool CreateModel(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, poke_3d::model::CharaModel* pCharaModel, GLResourceAllocator *pResourceAllocator);

private:

  //! ステート
  FieldMoveModelHeader        m_header;
  b32                         m_isSetupWork;
  b32                         m_isSetupResource;
  u32                         m_stateBit;

  //! キャラ用描画インスタンス
  poke_3d::model::CharaModel* m_pCharaDrawInstance;

  //! アニメーション
  u32                         m_defaultIdleAnimationId;
  u32                         m_animationId;
  b32                         m_isUseCustomValueForOrigin;
  f32                         m_customScaleForOrigin;
  gfl2::math::Quaternion      m_customQuaternionForOrigin;
  gfl2::math::Vector4          m_lastEnableOriginMoveVector;

  //! ムーブコード
  const FieldMoveCodeBase    *m_pMoveCode;
  FieldMoveCodeWork           m_moveCodeWork;

  //! アクションコマンド
  ///ActionCommandWork           m_actionCommandWork;

  //! コリジョン
  b32                         m_isForceLanding;                           //< 地面に吸着させるかどうか
  b32                         m_isOnGround;                               //< 地面に立っているかどうか
  u32                         m_groundAttribute;                          //< 立っている地面の属性
  f32                         m_groundHeight;                             //< 立っている地面の高さ
  DynamicActor*               m_pDynamicActor[COLLISION_TYPE_MAX];        //< 壁等から押し返しを受けるためのアクター
  StaticActor*                m_pStaticActor [COLLISION_STATIC_TYPE_MAX]; //< 壁になる為のアクター
  BaseCollisionScene*         m_pCollisionSceneContainer;                 //< 高さ取得用レイチェックで対象とするシーン

  //! 別アプリからの復帰用
  gfl2::math::Vector3         m_positionForReturn;
  gfl2::math::Quaternion      m_quaternionForReturn;

  //! 旋回・一歩分進んだか調査用
  b32                         m_isTurnFrame;
  b32                         m_isOneStepFrame;
  f32                         m_oneStepCnt;
  u32                         m_oneStepMaxCnt;

	gfl2::animation::AnimationPackList* m_pAnimationPackList;
	gfl2::renderingengine::scenegraph::resource::ResourceNode *m_pModelResourceNode;
	gfl2::renderingengine::scenegraph::resource::ResourceNode *m_pMotionConfigResourceNode;
	static const int BIN_DATA_NUM = 3;
	c8 *m_pBinData[BIN_DATA_NUM];
	GLResourceAllocator *m_pResourceAllocator;

};


}; //end of namespace MoveModel
}; //end of namespace Field
