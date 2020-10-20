//======================================================================
/**
 * @file EventGimmickEncount.h
 * @date 2015/09/02 16:06:40
 * @author saita_kazuki
 * @brief ギミックエンカウントイベント
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EVENT_GIMMICK_ENCOUNT_H_INCLUDED__
#define __EVENT_GIMMICK_ENCOUNT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_MathAABB.h>
#include <math/include/gfl2_Easing.h>

#include "GameSys/include/GameEvent.h"
#include "Field/FieldStatic/include/FieldLocation.h"

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDefine.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"
#include "Field/FieldRo/include/Encount/FieldEncountPokeParam.h"
#include <niji_conv_header/field/FieldEffectTypes.h>

namespace Field {
  class IFieldGimmickEncountActorBase;
  class EventWork;
  class Fieldmap;
  class FieldRenderingPipeLineManager;
  class CaptureRenderingPipeLine;
  namespace Encount {
    class Encount;
  }
  namespace GimmickEncount {
    class LocalWork;
  }
  namespace TrialModel {
    class FieldTrialModel;
    class TrialModelFactory;
  }
  namespace Effect {
    class IEffectBase;
    class EffectDummyPolygon;
    class EffectGimmickEncount;
    class EffectManager;
  }
  namespace MoveModel{
    class FieldMoveModel;
  }
  namespace StaticModel {
    class StaticModelManager;
  }
}
namespace System {
  class nijiAllocator;
}
namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {
  class TextureResourceNode;
}}}}

GFL_NAMESPACE_BEGIN( Field )

/**
 * @class EventGimmickEncount
 * @brief ギミックエンカウントイベント
 */
class EventGimmickEncount : public  GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventGimmickEncount); // コピーコンストラクタ＋代入禁止

public:

  struct Description
  {
    IFieldGimmickEncountActorBase*  pActor;
    Encount::Encount*               pEncount;
    EventWork*                      pEventWork;
  };

  /**
   * @brief   コンストラクタ
   */
  EventGimmickEncount( gfl2::heap::HeapBase* pHeap);

  /**
   * @brief   デストラクタ
   */
  ~EventGimmickEncount();

  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  virtual bool BootChk(GameSys::GameManager* p_gman);

  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  virtual void InitFunc(GameSys::GameManager* p_gman);

  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  virtual void EndFunc(GameSys::GameManager* p_gman);

  /**
   * @brief   セットアップ
   * @param   desc このイベントに必要な外部情報
   */
  void Setup( const Description& desc);

private:

  void InitFunc_Standby( GimmickEncount::LocalWork* pLocalWork, GameSys::GameManager* p_gman);
  void InitFunc_Chase( GimmickEncount::LocalWork* pLocalWork, GameSys::GameManager* p_gman);
  void InitFunc_Patrol( GimmickEncount::LocalWork* pLocalWork, GameSys::GameManager* p_gman);
  void InitFunc_Escape( GimmickEncount::LocalWork* pLocalWork, GameSys::GameManager* p_gman);

  void initializeEventData( GameSys::GameManager* p_gman);

  void initializePokeData( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* pHeap);

  void initializeTypesData( GameSys::GameManager* p_gman);

  void preTerminateSymbol( GameSys::GameManager* p_gman);
  void terminateSymbol( GameSys::GameManager* p_gman);

  void initializeUpdater( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pTemporaryHeap);
  bool updateUpdater();
  bool updateUpdaterBattleCall();
  void terminateUpdater();

  void initializeIcon( Effect::EffectManager* pEffectManager, MoveModel::FieldMoveModel* pMoveModel);
  bool updateIcon();
  void terminateIcon( Effect::EffectManager* pEffectManager);

  u32 GetDropItemType( u32 modelID, Field::GimmickEncount::GimmickType type) const ;
  u32 GetDropItemType_StaticModel( u32 modelID) const ;
  u32 GetDropItemType_MoveModel( u32 modelID) const ;

  void RefrectRecode( u32 modelID, Field::GimmickEncount::GimmickType type);
  void RefrectRecode_StaticModel( u32 modelID);
  void RefrectRecode_MoveModel( u32 modelID);

private:

  ///< ポケモンの移動演出種類
  enum PokeMoveType
  {
    POKE_MOVE_TYPE_NONE,    ///< なし
    POKE_MOVE_TYPE_FALL,    ///< 落ちてくる
    POKE_MOVE_TYPE_COME,    ///< 向かってくる
  };

private:

  //======================================================================
  // ギミックエンカウントシーケンス更新インターファース
  //======================================================================

  /**
  * @class IEventGimmickEncountUpdate
  * @brief ギミックエンカウントシーケンス更新インターファース
  */
  class IEventGimmickEncountUpdate
  {
    GFL_FORBID_COPY_AND_ASSIGN( IEventGimmickEncountUpdate); //コピーコンストラクタ＋代入禁止

  public:

    /**
    * @brief セットアップ情報
    */
    struct Description
    {
      Description()
        : p_gman( NULL)
        , pEvent( NULL)
        , pHeap( NULL)
        , pTemporaryHeap( NULL)
      {}

      GameSys::GameManager* p_gman;
      EventGimmickEncount*  pEvent;
      gfl2::heap::HeapBase* pHeap;
      gfl2::heap::HeapBase* pTemporaryHeap;
    };

  public:

    IEventGimmickEncountUpdate();
    virtual ~IEventGimmickEncountUpdate();

    /**
    * @brief セットアップ
    * @param desc セットアップ情報
    */
    virtual void Setup( const Description& desc) = 0;

    /**
    * @brief 更新
    * @retval true 完了
    * @retval false 実行中
    */
    virtual bool Update() = 0;

    /**
    * @brief バトル呼び出しまでの更新
    * @retval true 完了
    * @retval false 実行中
    */
    virtual bool UpdateBattleCall() = 0;

  protected:

    Description       m_desc;
    u32               m_seq;
  };

  //======================================================================
  // ギミックエンカウント通常シーケンス更新
  //======================================================================

  /**
  * @class EventGimmickEncountNormalUpdate
  * @brief ギミックエンカウント通常シーケンス更新
  */
  class EventGimmickEncountNormalUpdate : public IEventGimmickEncountUpdate
  {
    GFL_FORBID_COPY_AND_ASSIGN( EventGimmickEncountNormalUpdate); //コピーコンストラクタ＋代入禁止

  public:

    EventGimmickEncountNormalUpdate();
    virtual ~EventGimmickEncountNormalUpdate();

    /**
    * @brief セットアップ
    * @param desc セットアップ情報
    */
    virtual void Setup( const Description& desc);

    /**
    * @brief 更新
    * @retval true 完了
    * @retval false 実行中
    */
    virtual bool Update();

    /**
    * @brief バトル呼び出しまでの更新
    * @retval true 完了
    * @retval false 実行中
    */
    virtual bool UpdateBattleCall();
  };

  //======================================================================
  // ギミックエンカウント飛び出し演出ありシーケンス更新
  //======================================================================

  /**
  * @class EventGimmickEncountEffectUpdate
  * @brief ギミックエンカウント飛び出し演出ありシーケンス更新
  */
  class EventGimmickEncountJumpMoveEffectUpdate : public IEventGimmickEncountUpdate
  {
    GFL_FORBID_COPY_AND_ASSIGN( EventGimmickEncountJumpMoveEffectUpdate); //コピーコンストラクタ＋代入禁止

  public:

    EventGimmickEncountJumpMoveEffectUpdate();
    virtual ~EventGimmickEncountJumpMoveEffectUpdate();

    /**
    * @brief セットアップ
    * @param desc セットアップ情報
    */
    virtual void Setup( const Description& desc);

    /**
    * @brief 更新
    * @retval true 完了
    * @retval false 実行中
    */
    virtual bool Update();

    /**
    * @brief バトル呼び出しまでの更新
    * @retval true 完了
    * @retval false 実行中
    */
    virtual bool UpdateBattleCall();

  private:

    void initializeAllocator( gfl2::heap::HeapBase* pHeap);
    void terminateAllocator();

    void initializePokeModelSystem();
    bool initializeWaitPokeModelSystem();
    void updatePokeModelSystem();
    void terminatePokeModelSystem();
    bool terminateWaitPokeModelSystem();

    void initializeCapturePolygon();
    void preTerminateCapturePolygon();
    void terminateCapturePolygon();

    void initializePokeModel();
    bool initializeWaitPokeModel();
    void updatePokeModel();
    void preTerminatePokeModel();
    void terminatePokeModel();

    void initializeTexture();
    bool initializeWaitTexture();
    void terminateTexture();

    void initializeColorShaderPath();
    void terminateColorShaderPath();

    void initializeEffect( const gfl2::math::Vector3& pos);
    void terminateEffect();

    gfl2::math::Vector3 CalculateFirstPosition( EventGimmickEncount::PokeMoveType type);
    gfl2::math::Vector3 CalculateGoalPosition( EventGimmickEncount::PokeMoveType type, f32 heightOffset);
    gfl2::math::Vector3 CalculateMoveVector( EventGimmickEncount::PokeMoveType type);
    f32 CalculateGoalLength( EventGimmickEncount::PokeMoveType type, f32 offsetWidth, f32 offsetHeight);
    f32 CalculateScale();
    gfl2::math::Quaternion CalculateRotationCore( const gfl2::math::Vector3& pos, const gfl2::math::Vector3& target);
    gfl2::math::Quaternion CalculateRotation( const gfl2::math::Vector3& pos);

  private:

    Effect::EffectDummyPolygon*                                       m_pCapturePolygon;          ///< キャプチャ差し替え用板ポリゴンモデル
    TrialModel::FieldTrialModel*                                      m_pPokeModel;               ///< ポケモデル
    gfl2::math::Vector3                                               m_jumpMoveVec;              ///< 飛び出し移動ベクトル
    gfl2::math::Vector3                                               m_startPosition;            ///< 開始位置
    gfl2::math::Vector3                                               m_goalPosition;             ///< 目標位置
    f32                                                               m_goalLength;               ///< 目標位置に到着したとする長さ
    s32                                                               m_pokeWaistJointIndex;      ///< ポケモデル腰ジョイント番号
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_pTextureResourceNode;     ///< テクスチャリソースノード  };
    Effect::IEffectBase*                                              m_pEffect;                  ///< 飛び出し演出エフェクト
    System::nijiAllocator*                                            m_pAllocator;               ///< アロケーター


    Fieldmap*                                                         m_pFieldmap;
    FieldRenderingPipeLineManager*                                    m_pPipeLineManager;
    CaptureRenderingPipeLine*                                         m_pPipeLine;
    TrialModel::TrialModelFactory*                                    m_pPokeModelFactory;
    Effect::EffectManager*                                            m_pEffectManager;
    StaticModel::StaticModelManager*                                  m_pStaticModelManager;

    f32                                                               m_time;
    f32                                                               m_timeAdd;
    f32                                                               m_radian;
    f32                                                               m_radianAdd;

    f32                                                               m_pokeBaseScale;
  };

private:

  Description   m_desc;           ///< 外部情報
  Location      m_playerLocation; ///< プレイヤーロケーション
  u32           m_presetID;       ///< プリセットID
  u32           m_encountDataID;  ///< エンカウントテーブル
  u32           m_dropItemID;     ///< ドロップアイテムID
  u32           m_scriptID;       ///< スクリプトID
  u32           m_wildBtlResult;  ///< 野生バトル結果
  bool          m_isJumpEvent;    ///< 飛び出し演出をするか
  u32           m_jumpSE;         ///< 飛び出し時SE
  u32           m_jumpMoveSE;     ///< 飛び出し移動SE
  u32           m_flagwork;       ///< フラグワーク
  u32           m_eventID;        ///< イベントID
  PokeMoveType  m_pokeMoveType;   ///< ポケモンの移動演出種類
  u32           m_jumpEffectType; ///< 飛び出し演出エフェクトタイプ

  const Encount::EncountPokeParam*  m_pEncPokeParam;    ///< 抽選されたエンカウントするポケモン情報
  const GimmickEncount::PokeData* m_pPokeData;        ///< ギミックエンカウントポケモンデータ
  gfl2::math::Vector3             m_symbolPos;        ///< シンボル位置
  IEventGimmickEncountUpdate*     m_pUpdater;         ///< シーケンス更新モジュール

  Effect::IEffectBase*            m_pExclamationIcon; ///< 「！」アイコン
  MoveModel::FieldMoveModel*      m_pMoveModelForIcon;///< アイコンを紐付ける動作モデル
  gfl2::math::Easing::EaseFunc    m_easingFunc;       ///< イージング関数指定

  u32                             m_battleCallOption; ///< バトルコールオプションに追加するオプション
  bool                            m_isSymbolEncount;  ///< シンボルエンカウントか

  gfl2::math::Vector3             m_dropItemPos;      ///< ドロップアイテム位置
  u32                             m_dropItemType;     ///< ドロップアイテムの見た目出しわけに使用

  Field::Effect::Type             m_exclamationEffID; ///< 「！」or「？」のエフェクトID
};

GFL_NAMESPACE_END( Field )

#endif // __EVENT_GIMMICK_ENCOUNT_H_INCLUDED__
