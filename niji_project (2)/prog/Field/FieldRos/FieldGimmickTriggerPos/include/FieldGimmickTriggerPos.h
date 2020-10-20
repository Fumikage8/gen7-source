//=================================================================================================
/**
*  @file FieldGimmickTriggerPos.h
*  @brief フィールドギミック 電気試練
*  @author ikuta_junya
*  @date 2016.02.17
*/
//=================================================================================================

#if !defined __FIELD_GIMMICK_TRIGGERPOS_H_INCLUDED__
#define __FIELD_GIMMICK_TRIGGERPOS_H_INCLUDED__
#pragma once

// niji
#include <model/include/gfl2_BaseModel.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include "System/include/nijiAllocator.h"

// field
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"
#include <niji_reference_files/script/FieldPawnTypes.h>

// reference
//#include <niji_reference_files/script/FieldPawnTypes.h>

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @class 電気試練ギミック
 */
class FieldGimmickTriggerPos : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickTriggerPos );

public:

  /**
   *  @brief コンストラクタ
   *  @param pHeap ヒープ
   *  @param pFieldmap フィールドマップ
   */
  FieldGimmickTriggerPos( gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap );

  /**
   *  @brief デストラクタ
   */
  virtual ~FieldGimmickTriggerPos();

  /**
   *  @brief  生成
   */
  virtual void Create(void);
  virtual void UpdateCreate(void);
  virtual bool IsCreated(void);

  /**
 *  @brief  Ready中1度だけ呼び出し
 */
  virtual void Ready(void);

  /**
   *  @biref 破棄更新
   *  @return 破棄が完了したかどうか
   *
   *  破棄が完了するまでマイフレーム呼出がなされる関数
   */
  virtual bool Finalize( void );

  /**
   *  @brief 更新
   *  Fieldmap::Updateにて呼出
   *  UpdateNormalが終了してから呼び出される
   */
  virtual void UpdateLast( void );

  /**
   *  @brief  イベント起動チェック
   */
  virtual bool EventRequestCheck(const EventCheckRequest *cpReq);

  // poke_3d::model::BaseModel* GetModel(s32 index){ return m_pModel; };
  
  /**
   *	@brief  トリガーPOSの有効フラグを設定。
   *  @note   トリガーPOSギミックが適用されていないマップで使用すると、アサート
    *	@note   対象のイベントが保持している、当たり判定情報を必ずクリアします。対象POSが反応する状態で実行しないでください。
   *
   *	@param	event_id イベントID
   *  @param  flag　　 true:有効 false:無効
   */
  void SetActiveFlag(u32 event_id, b32 flag);

private:

  void HitCheck(b32 isEvent, const gfl2::math::Vector3& rPos);

  /**
   *  @brief ギミックの状態
   */
  enum State
  {
    STATE_NONE,       ///< なし(動作していない)
    STATE_INIT,       ///< 初期化中
    STATE_RUNNIG,     ///< 動作中
    STATE_END,        ///< 終了

    STATE_MAX
  };

private:

  s32                                           m_InitializeState;
  s32                                           m_FinalizeState;

  // 外部オブジェクト
  gfl2::heap::HeapBase*                         m_pFieldDeviceHeap;

  struct EVENT_DATA;
  EVENT_DATA*                                   m_pEventDataTable;
  s32                                           m_EventDataTableSize;
};

/**
 *  @class UB用ギミック
 */
class FieldGimmickUb01 : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickUb01 );

public:

  /**
   *  @brief コンストラクタ
   *  @param pHeap ヒープ
   *  @param pFieldmap フィールドマップ
   */
  FieldGimmickUb01( gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap );

  /**
   *  @brief デストラクタ
   */
  virtual ~FieldGimmickUb01();

  /**
   *  @brief  生成
   */
  virtual void Create(void);
  virtual void UpdateCreate(void);
  virtual bool IsCreated(void);

  /**
 *  @brief  Ready中1度だけ呼び出し
 */
  virtual void Ready(void);

  /**
   *  @biref 破棄更新
   *  @return 破棄が完了したかどうか
   *
   *  破棄が完了するまでマイフレーム呼出がなされる関数
   */
  virtual bool Finalize( void );

  /**
   *  @brief 更新
   *  Fieldmap::Updateにて呼出
   *  UpdateNormalが終了してから呼び出される
   */
  virtual void UpdateLast( void );


private:

  /**
   *  @brief ギミックの状態
   */
  enum State
  {
    STATE_NONE,       ///< なし(動作していない)
    STATE_INIT,       ///< 初期化中
    STATE_RUNNIG,     ///< 動作中
    STATE_END,        ///< 終了

    STATE_MAX
  };

private:

  s32                                           m_InitializeState;
  s32                                           m_FinalizeState;

  // 外部オブジェクト
  gfl2::heap::HeapBase*                         m_pFieldDeviceHeap;
  gfl2::fs::AsyncFileManager*                   m_pFileReadManager;

  // 内部オブジェクト
  System::nijiAllocator*                                          m_pAllocator;
};


GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_TRIGGERPOS_H_INCLUDED__