#ifndef __GFL_G3DPARTICLE_H__
#define __GFL_G3DPARTICLE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl_G3dParticle.h
 *  @brief  3Dパーティクル
 *  @author Koji Kawada
 *  @date   2011.05.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/*
キーワード
    ## パーティクルを構成する全てのものに対して同じ値を設定します ##
        パーティクルはトップのnw::gfx::TransformNode、
        その下に複数付くことができるnw::gfx::ParticleModel、nw::gfx::ParticleEmitterからなる。
        これら全てのものに対して同じ値を設定するので、
        デザイナーさんリソースの段階ではバラバラの値だったとしても
        この注意書きのある関数を使用すると同じ値になってしまう。
*/


// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/ut.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dResource.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *     クラス宣言
 */
//-----------------------------------------------------------------------------
//-------------------------------------
/// 3Dパーティクルクラス
//=====================================
class Particle : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Particle);


  // 下記クラスからはprivate関数にもアクセスしてよい
  friend class SceneCalculateGroup;
  friend class Scene;




public:
  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  /// パーティクルの設定内容
  //=====================================
  struct Description
  {
    // nw::gfx::TransformNode
    //コールバックなしs32 max_callbacks;         // 管理できるコールバックの最大数を設定します。
    s32 max_children;          // 子の最大数を設定します。
    //生成時しかメモリ確保しないb32 is_fixed_size_memory;  // 生成時以外にもメモリを確保するかどうかのフラグを設定します。true を指定すると、生成時のみ固定サイズのメモリ確保を行います。

    // モデル  // モデル個々の設定はできません。全てのモデルがこの設定になります。
    s32 model_max_children;

    // エミッター  // エミッター個々の設定はできません。全てのエミッターがこの設定になります。
    s32 emitter_max_children;

    Description(void)
      : max_children(16),
        model_max_children(16),
        emitter_max_children(16)
    {
      // 何もしない
    }
  };
  static void InitializeDescriptionForDefault(
      Description*  description
  );
/*
  関数で設定するのはもうやめるカメラのようなタイプのあるものだけ関数設定にする他は直接代入で
  static void InitializeDescription(
      Description*  description,
      s32 max_children,
      s32 model_max_children,
      s32 emitter_max_children
  );*/

  //---------------------------------------------------------------------------
  /**
   *           定数宣言
   */
  //---------------------------------------------------------------------------
/*
  準備中
  // 計算されるかどうか
  enum CalculateState
  {
    CALCULATE_STATE_NO,  // 計算されない
    CALCULATE_STATE_OK   // 計算される
      // 一部計算される状態などが今後追加される予定
  };
*/
  // 描画されるかどうか
  enum DrawState
  {
    DRAW_STATE_NO,  // 描画されない
    DRAW_STATE_OK   // 描画される
  };

public:
  Particle(void);
  virtual ~Particle();

  // resourceに含まれる全てのModel、全てのEmitterを使用してパーティクルを生成する
  void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Resource*               resource,
    const Description*      description = NULL
  );

/*
  // resourceに含まれるname_in_resource_arrayのModel, Emitterを使用してパーティクルを生成する
  // 例
  // static const char* NAME_ARRAY[4] =
  // {
  //   "FireModel",
  //   "FireEmitter",
  //   "SmokeModel",
  //   "SmokeEmitter",
  // };
  void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Resource*               resource,
    u32                     name_num,
    const char**            name_in_resource_array,
    const Description*      description = NULL
  );

  // resourceに含まれるmodel_index_in_resource_array, emitter_index_in_resource_arrayを使用してパーティクルを生成する
  // 例
  // static const s32 MODEL_INDEX_ARRAY[2] =
  // {
  //   0,
  //   1,
  // }
  // static const s32 EMITTER_INDEX_ARRAY[2] =
  // {
  //   0,
  //   1,
  // }
  void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Resource*               resource,
    u32                     model_num,
    const s32*              model_index_in_resource_array,
    u32                     emitter_num,
    const s32*              emitter_index_in_resource_array,
    const Description*      description = NULL
  );
*/

  virtual void Destroy(void);


public:
  // @note GetNwTransformNodeは今だけの関数なので、使用しないで下さい。→コメントアウトして使えなくした。
  //-----------------------------------------------------------------------------
  /**
   *  @brief         このパーティクルのトップノードを取得
   * 
   *  @retval        nw::gfx::TransformNode*    このパーティクルのトップノード
   */
  //-----------------------------------------------------------------------------
  //nw::gfx::TransformNode* GetNwTransformNode(void) const { return SceneNode::GetNwTransformNode(); }


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         平行移動
   */
  //-----------------------------------------------------------------------------
  void SetTranslate(f32 x, f32 y, f32 z);
  void SetTranslate(const gfl::math::VEC3& t);
  void GetTranslate(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetTranslate(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         回転
   *                 単位はラジアン。
   */
  //-----------------------------------------------------------------------------
  void SetRotateXYZ(f32 x, f32 y, f32 z);
  void SetRotateXYZ(const gfl::math::VEC3& t);
  template <typename TMatrix> void SetRotateMatrix(const TMatrix& m)
  {
    nw::gfx::TransformNode* transform_node = GetNwTransformNode();
    transform_node->Transform().SetRotateMatrix(m);

    {
      nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
      constraint_calculated_transform.SetRotateMatrix(m);
    }
  }
  void SetRotateMatrix(
      f32 m00,
      f32 m01,
      f32 m02,
      f32 m10,
      f32 m11,
      f32 m12,
      f32 m20,
      f32 m21,
      f32 m22
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         スケール
   */
  //-----------------------------------------------------------------------------
  void SetScale(f32 x, f32 y, f32 z);
  void SetScale(const gfl::math::VEC3& t);
  void GetScale(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetScale(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画順を設定する
   *                 プログラマだけが設定できるモデルのレイヤーIDをパーティクルモデルに設定する
   *                 最初に描画 0 <= u8 <= 127 最後に描画
   *                 最優先される描画順 ModelLayerId > MaterialTranslucencyKind > その他 最劣後される描画順
   *
   *                 **** ModelとParticleにあり全部を一列に並べた統一番号です。デフォルト0000000です。 ****
   *
   *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
   *
   *  @param[in]    model_layer_id    モデルのレイヤーID
   */
  //-----------------------------------------------------------------------------
  void SetModelLayerId(u8 model_layer_id);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画順を取得する
   *                 プログラマだけが設定できるモデルのレイヤーIDをパーティクルから取得する
   *                 最初に描画 0 <= u8 <= 127 最後に描画
   *                 最優先される描画順 ModelLayerId > MaterialTranslucencyKind > その他 最劣後される描画順
   *
   *                 **** ModelとParticleにあり全部を一列に並べた統一番号です。デフォルト0000000です。 ****
   *
   *  ## パーティクルを構成する全てのものの中で先頭のものの値を取得します(先頭と先頭以外で異なる値が設定されているとき困ったことになるかもしれません) ##
   *
   *  @retval       u8    モデルのレイヤーID
   */
  //-----------------------------------------------------------------------------
  u8 GetModelLayerId(void) const;

public:
#if 0
  //準備中
  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算フラグ
   *                 trueのとき計算される(@note 全部？一部？)
   *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは計算されない
   */
  //-----------------------------------------------------------------------------
  void SetCalculateFlag(b32 flag);
  b32  GetCalculateFlag(void) const;
#endif
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画フラグ
   *                 trueのとき描画される
   *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは描画されない
   */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(b32 visible);
  virtual b32  IsVisible(void) const;

#if 0
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンツリーフラグ
   *                 trueのときSceneにAddされていたらシーンツリーに入れられる、falseのときSceneにAddされていてもシーンツリーから外される
   *                 これがtrueであることと、ユーザがSceneにAddしたことは、同じではありません。
   */
  //-----------------------------------------------------------------------------
  void SetTreeFlag(b32 flag);
  b32  GetTreeFlag(void) const;
#endif

#if 0
  //準備中
  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算されるかどうか
   */
  //-----------------------------------------------------------------------------
  CalculateState GetCalculateState(void) const;
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画されるかどうか
   */
  //-----------------------------------------------------------------------------
  DrawState GetDrawState(void) const;


  //-----------------------------------------------------------------------------
  /**
   *  @brief   パーティクルモデルの個数を得る
   *
   *  @retval    パーティクルモデルの個数
   */
  //-----------------------------------------------------------------------------
  s32 GetModelNum(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief   パーティクルモデルを得る
   *
   *  @param[in] index  インデックス
   *  @retval    パーティクルモデル
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ParticleModel* GetModel(s32 index) const;

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief  更新フレームを設定する
   *
   *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
   * 
   *  0.0fで停止、1.0fで等倍、2.0fで2倍速
   *  パーティクルは逆再生はできないので、-1.0fなどの負の値は設定できません。
   * 
   *  @param[in]     step_frame      更新フレーム
   */
  //-----------------------------------------------------------------------------
  void SetStepFrame(const f32 step_frame);
#if 0
  //再帰は止めた。
  // friendクラスからアクセスしてはいけないprivateメンバ関数
private:
  void SetStepFrameRecursively(
      nw::gfx::SceneNode*   nw_scene_node,
      const f32             step_frame
#if GFL_DEBUG
      , const s32           recursive_count 
#endif
  );
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief  フレームを設定する
   *
   *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
   * 
   *  @param[in]     frame      フレーム
   */
  //-----------------------------------------------------------------------------
  void SetFrame(const f32 frame);

  //-----------------------------------------------------------------------------
  /**
   *  @brief  フレームを取得する
   * 
   *  @retval      フレーム
   */
  //-----------------------------------------------------------------------------
  f32 GetFrame( void ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief  リセットを行い、初期状態に戻す
   *          GPU処理中はコールできません。
   *          次のどちらかを満たせばGPU処理中ではありません。
   *          (1) シーン登録を解除している。(コマンドバッファがダブルのときはシーン登録を解除してから1フレーム以上経過している)
   *          (2) 終了状態である。(コマンドバッファがダブルのときは終了状態になってから1フレーム以上経過している)
   */
  //-----------------------------------------------------------------------------
  void Reset(void);
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  エミッターのみのリセット
   */
  //-----------------------------------------------------------------------------
  void ResetEmitter(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   放出量を設定する
   *
   *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
   *
   *  @param[in]     ratio  放出量(0.0fで全く放出されない)
   */
  //-----------------------------------------------------------------------------
  void SetEmitterEmissionRatio(f32 ratio);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   放出量を設定する
   *           nameという名前が付いた全てのエミッターに放出量を設定する
   *           nameという名前が付いたエミッターがない場合は何もしない
   *
   *  @param[in]     name   エミッターの名前
   *  @param[in]     ratio  放出量(0.0fで全く放出されない)
   */
  //-----------------------------------------------------------------------------
  void SetEmitterEmissionRatio(const char* name, f32 ratio);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   放出量を取得する
   *           最初に見付けたnameという名前が付いたエミッターの放出量を返す
   *           nameという名前が付いたエミッターがない場合は0.0fを返す
   *
   *  @param[in]     name   エミッターの名前
   *
   *  @retval        f32    放出量(0.0fで全く放出されない)
   */
  //-----------------------------------------------------------------------------
  f32 GetEmitterEmissionRatio(const char* name) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief   現在のパーティクルの個数を取得する
   *           パーティクルを構成する全てのもので現在存在するものの個数の和を返します
   *
   *  @retval  s32  現在のパーティクルの個数
   */
  //-----------------------------------------------------------------------------
  s32  GetParticleCount(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief   終了状態かどうかを取得する
   *           パーティクルを構成する全てのものが終了状態であればtrueを返します
   *           1つでも終了状態でなければfalseを返します
   *
   *  @retval  b32  終了状態のときtrue
   */
  //-----------------------------------------------------------------------------
  b32 IsDone(void) const; 

  //-----------------------------------------------------------------------------
  /**
   *  @brief   無限放出を設定する
   *           エミッターからの放出に終わりがあるか否かの設定です。1個1個の粒子の寿命の設定ではありません。
   * 
   *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
   *
   *  @param[in]  infinity  無限放出にするときtrue
   */
  //-----------------------------------------------------------------------------
  void SetEmitterEmissionSpanInfinity(b32 infinity);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   無限放出かどうかを取得する
   *           パーティクルを構成する全てのものが無限放出でなければfalseを返します
   *           1つでも無限放出であればtrueを返します
   *
   *  @retval  b32  無限放出のときtrue
   */
  //-----------------------------------------------------------------------------
  b32 IsEmitterEmissionSpanInfinity(void) const;

  // friendクラスからのみアクセスしてもよいprivateメンバ関数
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief   セットアップ
   *
   *  @param[in] particle_context パーティクルコンテキスト
   */
  //-----------------------------------------------------------------------------
  void Setup(nw::gfx::ParticleContext* particle_context);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   指定数だけフレームを回す
   *
   *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
   *
   *  @param[in] times            フレームを回す回数
   *  @param[in] particle_context パーティクルコンテキスト
   */
  //-----------------------------------------------------------------------------
  void AddTimes(u32 times, nw::gfx::ParticleContext* particle_context);

private:
  //b32             m_tree_flag;  // trueのときSceneにAddされていたらシーンツリーに入れられる、falseのときSceneにAddされていてもシーンツリーから外される
  
  b32 m_is_setup;                                                 // セットアップが完了していたらtrue
  //nw::ut::MoveArray<nw::gfx::SceneNode*>* m_scene_node_array;     // 生成したシーンノードを管理しておく配列
  nw::gfx::SceneNodeArray*                m_scene_node_array;     // 生成したシーンノードを管理しておく配列

  //nw::ut::MoveArray<nw::gfx::ParticleModel*>*   m_model_array;    // 生成したパーティクルモデルを管理しておく配列
  nw::gfx::ParticleModelArray*                  m_model_array;    // 生成したパーティクルモデルを管理しておく配列
      // m_scene_node_arrayと同じものを指す配列なので、生成/破棄はm_scene_node_arrayに任せる。
  //nw::ut::MoveArray<nw::gfx::ParticleEmitter*>* m_emitter_array;  // 生成したパーティクルエミッターを管理しておく配列
  nw::gfx::ParticleEmitterArray*                m_emitter_array;  // 生成したパーティクルエミッターを管理しておく配列
      // m_scene_node_arrayと同じものを指す配列なので、生成/破棄はm_scene_node_arrayに任せる。
  
#if GFL_DEBUG    
  public:
    //----------------------------------------------------------------
    /**
     * @brief 安全にモデルが解放できたかを確認する機能の有効無効設定
     * @param fEnable
     */
    //----------------------------------------------------------------
    static void SafeDeleteCheckEnable( b32 fEnable )
    {
      s_SafeDeleteCheckEnable        = fEnable;
      s_SafeDeleteCheckEnableUserSet = fEnable;
      if(s_SafeDeleteCheckForce) s_SafeDeleteCheckEnable = true;
    }
    static b32 IsSafeDeleteCheckEnable(){ return s_SafeDeleteCheckEnable; }

    static void SafeDeleteCheckForce(b32 fForce)
    {
      s_SafeDeleteCheckForce = fForce;
      if(s_SafeDeleteCheckForce)
      {
        s_SafeDeleteCheckEnable = true;
      }
      else
      {
        s_SafeDeleteCheckEnable = s_SafeDeleteCheckEnableUserSet;
      }
    }
    static b32 IsSafeDeleteCheckForce(void) { return s_SafeDeleteCheckForce; }

    static void SafeDeleteCheckAssert(b32 fAssert){ s_SafeDeleteCheckAssert = fAssert; }
    static b32  IsSafeDeleteCheckAssert(void) { return s_SafeDeleteCheckAssert; }

    void SetDrawFrameCount( u32 cnt ){ m_DrawFrameCount = cnt; }
    u32 GetDrawFrameCount(){ return m_DrawFrameCount; }
    
    void SetModelName( const char* pName );
    
  private:
    static b32                  s_SafeDeleteCheckEnable;
    static b32                  s_SafeDeleteCheckEnableUserSet;  // Force終了後にユーザ設定値に戻せるように記憶しておく。
    static b32                  s_SafeDeleteCheckForce;          // trueのときs_SafeDeleteCheckEnableをtrueで固定する。
    static b32                  s_SafeDeleteCheckAssert;         // trueのときASSERTで止める。falseのときPRINTで済ませる。

    u32                         m_DrawFrameCount;
    char                        m_ModelName[32];
#endif  
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // #ifndef __GFL_G3DPARTICLE_H__
