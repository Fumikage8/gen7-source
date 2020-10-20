//=============================================================================================
/**
 * @file  PokePartySave.cpp
 * @date  2012.04.25
 * @brief pml::PokePartyのセーブインターフェイス用クラス
 * @author  tamada
 */
//=============================================================================================
#include "util/include/gfl2_std_string.h"
#include "Savedata/include/PokePartySave.h"


GFL_NAMESPACE_BEGIN(Savedata)

//--------------------------------------------------------------
/// コンストラクタ
//--------------------------------------------------------------
PokePartySave::PokePartySave()
{
  gfl2::std::MemClear( &mData, sizeof(mData) );
}

//--------------------------------------------------------------
/// デストラクタ
//--------------------------------------------------------------
PokePartySave::~PokePartySave()
{
}

//--------------------------------------------------------------
/**
 * @brief PokeParty→セーブデータへの変換処理
 * @param pParty
 */
//--------------------------------------------------------------
void PokePartySave::Serialize( const pml::PokeParty * pParty )
{
  pParty->SerializeFull( &mData );
}

//--------------------------------------------------------------
/**
 * @brief セーブデータ→PokePartyへの変換処理
 * @param pParty
 */
//--------------------------------------------------------------
void PokePartySave::Deserialize( pml::PokeParty * pParty )
{
  pParty->DeserializeFull( &mData );
}

GFL_NAMESPACE_END(Savedata)

