/**
* @brief  アクションボタンの表示を管理するクラス
* @file   ActionButtonManager.h
*/

#if !defined( __ACTIONBUTTONMANGERER_H_INCLUDED__ )
#define __ACTIONBUTTONMANGERER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <Fs/include/gfl2_FsAsyncFileManager.h>
#include <math/include/gfl2_math.h>

// 前方宣言
class BaseActor;
GFL_NAMESPACE_BEGIN( Field );
class IFieldActorBase;

GFL_NAMESPACE_BEGIN( Effect );
class EffectManager;
class IEffectBase;
GFL_NAMESPACE_END( Effect );

GFL_NAMESPACE_BEGIN( MoveModel );
class FieldMoveModelManager;
GFL_NAMESPACE_END( MoveModel );

GFL_NAMESPACE_BEGIN( ActionButton );

struct ActionButtonData
{
  u8 mEyeValid;
  u8 mAllRange;
  u16 mType;
  int mEyePosX;
  int mEyePosY;
  int mEyePosZ;
  int mIconPosX;
  int mIconPosY;
  int mIconPosZ;
  int mWait;
};

/**
* @brief 管理クラス
*/
class ActionButtonManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ActionButtonManager );

  enum STATE
  {
    IDLE,   ///<エフェクト非表示 アイドル状態
    READY,  ///<エフェクト表示準備中
    DISP    ///<エフェクト表示中
  };

  ActionButtonData *m_pActionButtonDatas;
  size_t m_DataSize;
  int m_DataNum;
  u32 m_eventID;

  Effect::IEffectBase * m_pEffect;
  STATE m_State;
  u32 m_WaitCount;
  u32 m_NowWaitCount;
  b32 m_isFocusTrainer;
  b32 m_isInterestNotClear;
  b32 m_isInterestRequest;
  b32 m_isFureaiPokemon;

public:
  enum ICON_FRAME
  {
    NONE,     ///<表示なし
    NORMAL,   ///<通常
    EMPHASIS,  ///<強調
  };

public:

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  ActionButtonManager( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  ~ActionButtonManager( void );
  


  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( Effect::EffectManager *pEffectManager );

  /**
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( BaseActor *pActor, MoveModel::FieldMoveModelManager* pFieldCharaModelManager, Effect::EffectManager *pEffectManager, b32 isInterestNotClear );

  /**
  * @brief 指定フォーカスＩＤのデータは全方位許容型かを返す
  *
  * @return trueで全方位許容
  */
  bool IsAllRangeFocus(int focus_id);

  /** 
   *  @brief BaseActorから情報を取得する。
   */
  bool GetActionButtonParam( BaseActor *pActor, u32 * p_target_event_id, u32* p_wait_count, ICON_FRAME* p_icon_frame, gfl2::math::Vector3* p_icon_pos, gfl2::math::Vector3* p_attention_pos, b32* p_attention ) const;
  bool GetActionButtonParam( IFieldActorBase *pBaseActor, u32 * p_target_event_id, u32* p_wait_count, ICON_FRAME* p_icon_frame, gfl2::math::Vector3* p_icon_pos, gfl2::math::Vector3* p_attention_pos, b32* p_attention ) const;

private:

  /**
  * @brief 内部ステートのリセット
  *
  * @return 無し
  */
  void ResetState( Effect::EffectManager *pEffectManager );

  /**
  * @brief 対象動作モデルのアイコンオフセット取得
  *
  * @return Vector3
  */
  gfl2::math::Vector3 GetMoveModelIconOffset(MoveModel::FieldMoveModelManager* pFieldCharaModelManager, const u32 characterId);

};  // class ActionButtonManager


GFL_NAMESPACE_END( ActionButton );
GFL_NAMESPACE_END( Field );

#endif // __ACTIONBUTTONMANGERER_H_INCLUDED__