#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModel.h
*  @brief  フィールドギミック カイリキ岩
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

GFL_NAMESPACE_BEGIN( Field )

namespace Effect
{
  class EffectKairikyRockSmoke;
  class EffectKairikyRockDown;
  class EffectKairikyRockSmoke2;
  class EffectKairikyRockDown2;
};

class FieldGimmickKairikyRock : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickKairikyRock );

public:

  // コンストラクタ
  FieldGimmickKairikyRock( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap, u32 zoneId, PlacementDataManager* pPlacementDataManager );
  // デストラクタ
  virtual ~FieldGimmickKairikyRock();

  /**
   *  @brief  生成
   */
  virtual void Create( void );

  /**
   *  @biref  破棄更新
   *  @return 破棄が完了したかどうか
   *
   *  破棄が完了するまでマイフレーム呼出がなされる関数
   */
  virtual bool Finalize( void );

  /**
   *  @brief  イベント起動チェック
   */
  virtual bool EventRequestCheck( const EventCheckRequest *cpReq );

  /**
   *  @brief  イベントの中身
   */
  bool ExecuteKairiky( void );

  /**
   *  @brief  岩がおせる場所かどうか取得
   */
  b32 IsKairikyEventLocation( void ){ return (b32)m_isKairikyEventLocation; }

private:

  b32 EnablePush(u32 zoneId,u32 index,u32 x,u32 z);

private:

  u32                    m_deleteState;
  gfl2::heap::HeapBase*  m_pHeap;
  PlacementDataManager*  m_pPlacementDataManager;
  u32                    m_zoneId;
  b8                     m_enableKairikyEvent;
  b8                     m_isKairikyEventLocation;
  b8                     m_isKeepPushing;               //!< 押し続けている状態かどうか（専用の岩の時のみ効果がある）
  u8                     m_padding;
  u32                    m_eventState;
  u32                    m_eventCnt;
  gfl2::math::Vector3    m_eventOffsetPos;
  gfl2::math::Quaternion m_eventStartQua;
  gfl2::math::Quaternion m_eventTargetQuaForPlayer;
  gfl2::math::Quaternion m_eventTargetQuaForRock;
  FieldMoveModelActor*   m_eventRockActor;
  u32                    m_eventInterpFrame;

  Field::Effect::EffectKairikyRockSmoke* m_pEffectKairikyRockSmoke;
  Field::Effect::EffectKairikyRockDown*  m_pEffectKairikyRockDown;
  Field::Effect::EffectKairikyRockSmoke2* m_pEffectKairikyRockSmokeL;
  Field::Effect::EffectKairikyRockDown2*  m_pEffectKairikyRockDownL;
  
};

GFL_NAMESPACE_END( Field )
