﻿#pragma once
//=================================================================================================
/**
*  @file   FieldSubobject.h
*  @brief  フィールド用 動作モデルのサブオブジェクト
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../../../../../poke_3d_lib/model/include/gfl2_CharaModel.h"
#include "System/include/nijiAllocator.h"
#include "Field/FieldStatic/include/FieldTransformNode.h"
#include "FieldMoveModel_define.h"

namespace poke_3d { namespace model {
   class CharaModelFactory;
}}

namespace Field {
  class MyRenderingPipeLine;
}

//=================================================================================================
//
// ※注意※  このクラスでは座標/姿勢/拡縮を保持してはいけません。
//           座標/姿勢/拡縮を保持して良いのは描画インスタンス(poke_3d::model::CharaModel)のみです。
//           ただ諸々の処理を簡単/高速に行うために、描画インスタンスと同内容のツリーを形成する必要があります
//
//=================================================================================================
namespace Field{ namespace MoveModel {

class FieldSubobject : public gfl2::renderingengine::scenegraph::NodeTrait<FieldSubobject,Field::FieldTransformNode>
{

  static const u32 DELAY_DELETE_FRAME = 1;

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldSubobject();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldSubobject();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化
  *
  *  @param  pHeap                 ヒープ
  *  @param  pMyRenderingPipeLine  レンダリングパイプライン
  *  @param  pCharaModelFactory    キャラリソースファクトリー
  *  @param  pFieldSubobjectHeader 作成リクエスト
  */
  //-----------------------------------------------------------------------------
  virtual void Setup(gfl2::heap::HeapBase*              pHeap,
                     Field::MyRenderingPipeLine*        pMyRenderingPipeLine,
                     poke_3d::model::CharaModelFactory* pCharaModelFactory,
                     const FieldSubobjectHeader*        pFieldSubobjectHeaderResource);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  解放(リソース)
  */
  //-----------------------------------------------------------------------------
  virtual void Clear(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  更新
  */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  SRT/アニメーション更新(自身を保持している動作モデルの座標決定後に必要な処理を行う)
  */
  //-----------------------------------------------------------------------------
  void Attach(b32 isVisible,s32 animationId,f32 animationFrame,u32 parentCharaId);
  void PostUpdate(b32 isVisible, u32 parentCharaId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化しているかどうか
  *
  *  @return trueは初期化済み、falseは未初期化
  */
  //-----------------------------------------------------------------------------
  b32 IsSetup(void){ return m_isSetup; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  自身を削除できるタイミングかどうか
  *
  *  @return trueなら削除できる、falseなら削除できない
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableDelete(void){ return (m_deleteState>DELAY_DELETE_FRAME) ? true:false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  削除リクエストが発行されているかどうか
  *
  *  @return trueは発行済み、falseは未発行
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableDeleteRequest(void){ return m_isDeleteRequest; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  削除リクエストを発行
  */
  //-----------------------------------------------------------------------------
  void EnableDeleteRequest(void);

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
  *  @brief   リクエストヘッダーを取得
  *
  *  @return  リクエストヘッダー
  */
  //-----------------------------------------------------------------------------
  FieldSubobjectHeader GetHeader(void){ return m_header; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリングを有効
  */
  //-----------------------------------------------------------------------------
  void EnableCulling(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリングを無効
  */
  //-----------------------------------------------------------------------------
  void DisableCulling(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリングを状態を取得
  *
  *  @return カリング状態か取得
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableCulilng(void){ return m_isCulling; }

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  描画パイプラインに自身のキャラモデルを追加
  */
  //-----------------------------------------------------------------------------
  void AddEdgeRenderingTarget(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  描画パイプラインから自身のキャラモデルを削除
  */
  //-----------------------------------------------------------------------------
  void RemoveEdgeRenderingTarget(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  モデルインスタンスノードを親から登録を外す
  */
  //-----------------------------------------------------------------------------
  void RemoveParentNode(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  モデルインスタンスノードを親に再設定する
  */
  //-----------------------------------------------------------------------------
  void ReSetParentNode(void);

public:

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

private:

  //! アロケータ
  System::nijiAllocator        *m_pLocalAllocator;
  Field::MyRenderingPipeLine   *m_pMyRenderingPipeLine;

  //! ステート
  FieldSubobjectHeader          m_header;
  b32                           m_isSetup;
  b32                           m_isDeleteRequest;
  u32                           m_deleteState;
  b32                           m_isCulling;
  b32                           m_isRegistToPipeline;
  gfl2::renderingengine::scenegraph::DagNode* m_pParentNode;

  //! キャラ用描画インスタンス
  poke_3d::model::CharaModel   *m_pCharaDrawInstance;
};


}; //end of namespace MoveModel
}; //end of namespace Field
