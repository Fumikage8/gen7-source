//=============================================================================================
/**
 * @file  TurtleSalmonSave.cpp
 * @brief TurtleSalmonのセーブインターフェイス用クラス
 */
//=============================================================================================
#include "util/include/gfl2_std_string.h"
#include "Savedata/include/TurtleSalmonSave.h"


GFL_NAMESPACE_BEGIN(Savedata)

//--------------------------------------------------------------
/// コンストラクタ
//--------------------------------------------------------------
TurtleSalmonSave::TurtleSalmonSave()
{
  gfl2::std::MemClear( &mData, sizeof(mData) );
}

//--------------------------------------------------------------
/// デストラクタ
//--------------------------------------------------------------
TurtleSalmonSave::~TurtleSalmonSave()
{
}

GFL_NAMESPACE_END(Savedata)

