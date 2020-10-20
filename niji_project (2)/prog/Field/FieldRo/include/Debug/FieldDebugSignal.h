//======================================================================
/**
 * @file FieldDebugSignal.h
 * @date 2015/07/27 18:13:27
 * @author miyachi_soichi
 * @brief MotionSignalデバッグ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DEBUG_SIGNAL_H_INCLUDED__
#define __FIELD_DEBUG_SIGNAL_H_INCLUDED__
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

GFL_NAMESPACE_BEGIN(Field)
// 前方宣言
class Fieldmap;
GFL_NAMESPACE_BEGIN(Camera)
GFL_NAMESPACE_BEGIN(Target)
class TargetBase;
GFL_NAMESPACE_END(Target)
GFL_NAMESPACE_BEGIN(Controller)
class ControllerTypeDebugTrainer;
GFL_NAMESPACE_END(Controller)
class CameraManager;
GFL_NAMESPACE_END(Camera)

GFL_NAMESPACE_BEGIN(Debug)

#if PM_DEBUG

/**
 * @class DebugSignal
 * @brief MotionSignalデバッグ
 */
class DebugSignal
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugSignal);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugSignal( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugSignal();

  /**
   *  @brief  初期化
   *  @param  pHeap   ヒープ
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData *pMsgData );

  /**
   *  @brief  破棄
   */
  void Terminate();

  /**
   *  @brief  デバッグメニュー作成
   *  @param  root    メニュー親
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  /**
   *  @brief  カメラ対象の変更
   *  @param  pModel    カメラ注視対象：NULLの場合Playerにもどす
   */
  void ChangeTarget( Field::MoveModel::FieldMoveModel *pModel );

  /**
   *  @brief  選択キャラクタの変更
   *  @param  add   移動値
   */
  void ChangeCharacter( s32 add );

  /**
   *  @brief  キャラクタリストを再作成する
   */
  void ReloadChara( void );

  /**
   *  @brief  シグナルの登録
   */
  void RegistData( void );

  /**
   *  @brief  シグナルの削除
   */
  void ClearData( void );

  /**
   *  @brief  表示データを設定データに合わせる
   */
  void RefreshData( void );

  /**
   *  @brief  シグナルを全リセット(リソース側に合わせる)
   */
  void ResetAllData( void );

  /**
   *  @brief  シグナル配列番号の変更
   *  @param  add   移動値
   */
  void ChangeSignalDataIdx( s32 add );

  //! キャラの取得
  inline Field::IFieldActorBase *GetActor( u32 idx ){ return m_pActor[ idx ]; }
  //! 選択キャラ番号の取得
  inline s32 GetCnt( void ){ return m_nCnt; }
  //! キャラ番号最大値を取得
  inline s32 GetMax( void ){ return m_nMax; }
  //! シグナル番号の取得
  inline s32 GetDataIdx( void ){ return m_nDataIdx; }
  //! モーション番号の取得
  inline u32 GetAnimeID( void ){ return m_nAnimeID; }
  //! イベント種別の取得
  inline u32 GetType( void ){ return m_nTypeID; }
  //! 起動フレームの取得
  inline s32 GetFrame( void ){ return m_nFrame; }
  //! 起動パラメータの取得
  inline s32 GetValue( void ){ return m_nValue; }
  //! ヒープ取得
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }
  //! メッセージデータ取得
  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

  //! モーション番号の設定
  inline void SetAnimeID( u32 id ){ m_nAnimeID = id; }
  //! イベント種別の設定
  inline void SetType( u32 type ){ m_nTypeID = type; }
  //! 起動フレームの設定
  inline void SetFrame( s32 frame ){ m_nFrame = frame; }
  //! 起動パラメータの設定
  inline void SetValue( s32 value ){ m_nValue = value; }

private:
  gfl2::heap::HeapBase *m_pHeap;
  Field::Fieldmap *m_pFieldmap;
  Field::PlacementDataManager *m_pPlacementMgr;
  Field::IFieldActorBase *m_pActor[ 16 ];
  Field::Camera::CameraManager *m_pCamMgr;
  Field::Camera::Target::TargetBase *m_pTarget;
  Field::Camera::Controller::ControllerTypeDebugTrainer *m_pCtrler;
  gfl2::debug::DebugWinGroup *m_pGroup;
  gfl2::str::MsgData* m_pMsgData;

  s32 m_nCnt;
  s32 m_nMax;

  s32 m_nDataIdx;
  u32 m_nAnimeID;
  u32 m_nTypeID;
  s32 m_nValue;
  s32 m_nFrame;
};

#endif // PM_DEBUG

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_DEBUG_SIGNAL_H_INCLUDED__
