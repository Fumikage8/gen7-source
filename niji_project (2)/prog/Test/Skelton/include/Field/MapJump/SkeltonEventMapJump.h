#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
 @file event_mapjump.h
 @brief 擬似マップジャンプのイベント
 @date 2015.01.23
 @author saita_kazuki
 */
//*************************************************************************************************

#ifndef __FLD_EVENTMAPJUMP__
#define __FLD_EVENTMAPJUMP__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "GameSys/include/GameEvent.h"
#include "Test/Skelton/include/Field/MapJump/SkeltonJumpDataManager.h"

namespace GameSys {
class GameManager;
}
namespace gfl2 {
namespace collision {
class CollisionModel;
}
}
GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)
class SkeltonFieldTransform;
class SkeltonModel;

//=================================================================================================
// 擬似マップジャンプイベント
//=================================================================================================
class SkeltonEventMapJump : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(SkeltonEventMapJump);

public:

  // 設定用
  struct Setting
  {
    gfl2::math::Vector4  jumpPos;    // 移動後位置
    s32                 fadeFrame;  // フェードフレーム数
    f32                 angle;      // ジャンプ後向き
    gfl2::collision::CollisionModel* pCollisionModel;
    SkeltonModel*       pPlayer;
  };

public:
  static void CallMapJumpEvent( GameSys::GameManager * gmgr , const JUMP_DATA* pJumpData, SkeltonModel* pPlayer, const SkeltonFieldTransform& transform, gfl2::collision::CollisionModel* pCollisionModel);
public:
  SkeltonEventMapJump(gfl2::heap::HeapBase * pHeap );
  ~SkeltonEventMapJump();

  virtual bool BootChk( GameSys::GameManager * gmgr );
  virtual void InitFunc( GameSys::GameManager * gmgr );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager * gmgr );
  virtual void EndFunc( GameSys::GameManager * gmgr );

public:
  void Setup( const Setting& setting);

private:
  u32                 m_seq;        // シーケンス
  Setting             m_setting;    // 設定データ

  SkeltonModel*       pPlayer;

  f32 GetHeight( const gfl2::math::Vector4& pos);
};

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // __FLD_EVENTMAPJUMP__
#endif // GF_PLATFORM_WIN32

