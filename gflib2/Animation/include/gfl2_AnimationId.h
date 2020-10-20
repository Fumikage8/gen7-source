#ifndef GFLIB2_ANIMATION_ID_H_INCLUDED
#define GFLIB2_ANIMATION_ID_H_INCLUDED

#include <types/include/gfl2_Typedef.h>

namespace gfl2 { namespace animation {

/**
 * @brief パック済みアニメーションID
 */
struct PackedAnimationId
{
  u16 animationId;        // アニメーション番号
  u16 packId;             // パック番号

  PackedAnimationId(u32 x)
  {
    *this = *(PackedAnimationId*)&x;
  }
};

/**
 * @brief パック済み非常駐アニメーションID
 */
struct PackedDynamicAnimationId
{
  u16 charaId;          // キャラ番号
  u16 packId;           // パック番号

  PackedDynamicAnimationId(u32 x)
  {
    *this = *(PackedDynamicAnimationId*)&x;
  }
};


}}

#endif // #ifndef GFLIB2_ANIMATION_ID_H_INCLUDED
