//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		StringTokusei.cpp
 *	@brief  特性名ソート
 *	@author	Toru=Nagihashi
 *	@date		2012.09.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// module
#include "AppLib/include/Sort/StringTokusei.h"
// resource
#include "arc_index/sort_string.gaix"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)

//-----------------------------------------------------------------------
// 定数
//-----------------------------------------------------------------------
const TokuseiNo StringTokusei::SECRET_TOKUSEI_TABLE[ SECRET_TOKUSEI_TABLE_MAX ] =
{
	TOKUSEI_OYAKOAI,						// おやこあい
	TOKUSEI_OWARINODAITI,				// おわりのだいち
	TOKUSEI_HAZIMARINOUMI,			// はじまりのうみ
	TOKUSEI_DERUTASUTORIIMU,		// デルタストリーム
	TOKUSEI_SUKAISUKIN,					// スカイスキン
  TOKUSEI_SOURUHAATO,         // ソウルハート(add niji)
  // momiji追加分
  TOKUSEI_BUREINPURIZUMU,     // ブレインフォース
  TOKUSEI_NULL
};

//-----------------------------------------------------------------------
// 生成、破棄
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
StringTokusei::StringTokusei()
  : StringBinary()
{
}
/**
 * @brief デストラクタ
 */
StringTokusei::~StringTokusei()
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
void StringTokusei::Load( gfl2::heap::HeapBase * heap )
{
  this->StringBinary::LoadSync( heap,GARC_sort_string_tokusei_initial_index_DAT,GARC_sort_string_tokusei_sort_table_DAT, GARC_sort_string_tokusei_initial_to_sort_DAT );
}

/**
 * @brief 非同期読み込み設定
 *
 * @param heap  ヒープ
 * @param work_heap 作業用ヒープ
 *
 * @note  LoadMain()で読み込んでください
 */
void StringTokusei::LoadSetup( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * work_heap )
{
  this->StringBinary::LoadSetup(
    heap, work_heap, GARC_sort_string_tokusei_initial_index_DAT, GARC_sort_string_tokusei_sort_table_DAT, GARC_sort_string_tokusei_initial_to_sort_DAT );
}

/**
 * @brief 隠す特性かどうかチェック
 *
 * @param tokusei  特性
 *
 * @return  trueならば隠す特性  falseならば隠さない特性
 */
bool StringTokusei::IsSecretTokusei( TokuseiNo tokusei ) const
{
  for( int i = 0; i < SECRET_TOKUSEI_TABLE_MAX; ++i )
  {
    if( SECRET_TOKUSEI_TABLE[i] == tokusei )
    {
      return true;
    }
  }
  
  return false;
}

GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)
