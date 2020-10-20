//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		StringMonsName.cpp
 *	@brief  ポケモン種族名ソート
 *	@author	Toru=Nagihashi
 *	@date		2012.09.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// module
#include "AppLib/include/Sort/StringMonsName.h"
// resource
#include "arc_index/sort_string.gaix"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)

//-----------------------------------------------------------------------
// 生成、破棄
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
StringMonsName::StringMonsName()
  : StringBinary()
{
}
/**
 * @brief デストラクタ
 */
StringMonsName::~StringMonsName()
{
}

//-----------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------
/**
 * @brief 同期読み込み
 *
 * @param heap  ヒープ
 */
void StringMonsName::Load( gfl2::heap::HeapBase * heap )
{
  this->StringBinary::LoadSync( heap,GARC_sort_string_monsname_initial_index_DAT,GARC_sort_string_monsname_sort_table_DAT, GARC_sort_string_monsname_initial_to_sort_DAT );
}

/**
 * @brief 非同期読み込み設定
 *
 * @param heap  ヒープ
 * @param work_heap 作業用ヒープ
 *
 * @note  LoadMain()で読み込んでください
 */
void StringMonsName::LoadSetup( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * work_heap )
{
  this->StringBinary::LoadSetup(
    heap, work_heap, GARC_sort_string_monsname_initial_index_DAT, GARC_sort_string_monsname_sort_table_DAT, GARC_sort_string_monsname_initial_to_sort_DAT );
}

GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)
