//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dMultiPackAnimConnector.cpp
 *	@brief  3Dアニメーションファイルを1つにまとめたPackAnimファイルを扱う
 *	@author	Koji Kawada
 *	@date		2012.08.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dResAnim.h>

#include <grp/g3d/gfl_G3dPackAnimConnector.h>
#include <grp/g3d/gfl_G3dMultiPackAnimConnector.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	1つのModelと複数のPackAnimを結び付けるクラス
//=====================================
ModelMultiPackAnimConnector::ModelMultiPackAnimConnector(void)
  : m_pack_anim_list_head(NULL),

    m_anim_no(ANIM_NO_NONE),

    m_model(NULL),
    m_step_frame(0.0f),
    m_loop(false)
{
  // 何もしない
}

ModelMultiPackAnimConnector::~ModelMultiPackAnimConnector()
{
  UnsetAllPackAnim();

  // コンストラクタと同じ初期化
  {
    m_pack_anim_list_head = NULL;

    m_anim_no = ANIM_NO_NONE;

    m_model = NULL;
    m_step_frame = 0.0f;
    m_loop = false;
  }
}

void ModelMultiPackAnimConnector::SetModel(Model* model)
{
  UnsetAllPackAnim();

  m_model = model;
}

void ModelMultiPackAnimConnector::SetPackAnim(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    s32 slot_index,
    PackAnim* pack_anim
)
{
  UnsetPackAnim(slot_index);

  if( m_model )
  {
    PackAnimElem* curr_elem = NULL;
    
    //////////////////////////////////////////
    // PackAnimConnectorを作成する
    //////////////////////////////////////////
    {
      curr_elem = GFL_NEW(heap_allocator->GetHeapBase()) PackAnimElem;
      curr_elem->pack_anim_connector = NULL;
      curr_elem->prev = NULL;
      curr_elem->next = NULL;

      ModelPackAnimConnector* pac = GFL_NEW(heap_allocator->GetHeapBase()) ModelPackAnimConnector;
      { 
        pac->SetModel(m_model);
        pac->SetPackAnim(
            heap_allocator,
            device_allocator,
            slot_index,
            pack_anim
        );
        pac->ChangeAnimNo(m_anim_no);
        pac->SetStepFrame(m_step_frame);
        pac->SetLoop(m_loop);
      }

      curr_elem->pack_anim_connector = pac;
    }
 
    //////////////////////////////////////////
    // 既に他のスロットにPackAnimが存在する場合、フレーム番号をここで作成したPackAnimConnectorにも適用する
    //////////////////////////////////////////
    if( m_pack_anim_list_head )
    {
      f32 max_frame = 0.0f;  // アニメーションがないものは0.0fを返してくるので、全スロットをチェックして最大のフレームを採用することにする。
      PackAnimElem* elem = m_pack_anim_list_head;
      while(elem)
      {
        f32 frame = elem->pack_anim_connector->GetFrame();
        if( max_frame < frame ) max_frame = frame; 
        elem = elem->next;
      }
      curr_elem->pack_anim_connector->SetFrame(max_frame);
    }
 
    //////////////////////////////////////////
    // リストに追加する
    //////////////////////////////////////////
    {
      // 末尾を得る
      PackAnimElem* tail = NULL;
      PackAnimElem* elem = m_pack_anim_list_head;
      while(elem)
      {
        tail = elem;
        elem = elem->next;
      }
 
      if( tail )
      {
        // 末尾に追加
        tail->next = curr_elem;
        curr_elem->prev = tail;
      }
      else
      {
        // リストに初追加
        m_pack_anim_list_head = curr_elem;
      }
    }
  }
  else
  {
    GFL_ASSERT(m_model);  //@check
  }
}

void ModelMultiPackAnimConnector::UnsetPackAnim(s32 slot_index)
{
  PackAnimElem* elem = getPackAnimElem(slot_index);
  if( elem )
  {
    GFL_DELETE elem->pack_anim_connector;

    if( elem->prev ) elem->prev->next = elem->next;
    else m_pack_anim_list_head = elem->next;  // 先頭だったので、先頭変更

    if( elem->next ) elem->next->prev = elem->prev;

    GFL_DELETE elem;
  }

  // 全てのPackAnimの設定を破棄したとき
  if( !m_pack_anim_list_head ) m_anim_no = ANIM_NO_NONE;
}

void ModelMultiPackAnimConnector::UnsetAllPackAnim(void)
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    s32 curr_slot_index = elem->pack_anim_connector->GetSlotIndex();
    elem = elem->next;  // 破棄前に次のPackAnimElemのポインタを得ておく
    UnsetPackAnim(curr_slot_index);
  }
}

void ModelMultiPackAnimConnector::ChangeAnimNo(s32 anim_no)
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    elem->pack_anim_connector->ChangeAnimNo(anim_no);
    elem = elem->next;
  }
}

void ModelMultiPackAnimConnector::SetStepFrame(f32 step_frame)
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    elem->pack_anim_connector->SetStepFrame(step_frame);
    elem = elem->next;
  }
}

void ModelMultiPackAnimConnector::SetLoop(b32 loop)
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    elem->pack_anim_connector->SetLoop(loop);
    elem = elem->next;
  }
}

ModelMultiPackAnimConnector::PackAnimElem* ModelMultiPackAnimConnector::getPackAnimElem(s32 slot_index) const
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    s32 curr_slot_index = elem->pack_anim_connector->GetSlotIndex();
    if( slot_index == curr_slot_index ) return elem;
    elem = elem->next;
  }
  return elem;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

