#ifndef __POKEPARY_SAVE_H__
#define __POKEPARY_SAVE_H__
//=============================================================================================
/**
 * @file  PokePartySave.h
 * @date  2012.04.25
 * @brief pml::PokePartyのセーブインターフェイス用クラス
 * @author  tamada
 */
//=============================================================================================
#pragma once
//#include <gflib.h>
#include "pml/include/pml_PokeParty.h"
#include "Savedata/include/SaveDataInterface.h"
#include "debug/include/gfl2_Assert.h"


GFL_NAMESPACE_BEGIN(Savedata)

class PokePartySave : public SaveDataInterface
{
  public:
    PokePartySave();
    ~PokePartySave();

  public:
    virtual void SetData( void * data ) { GFL_ASSERT(0); }; ///<呼び出されることを想定していない
    virtual void * GetData( void ) { return &mData; };
   virtual size_t GetDataSize( void ) { return sizeof(mData); };
    virtual void Clear( gfl2::heap::HeapBase * heap )
    {
        pml::PokeParty * tmp = GFL_NEW(heap) pml::PokeParty( heap );
        tmp->Clear();
        this->Serialize( tmp );
        GFL_DELETE tmp;
    };

    //--------------------------------------------------------------
    /**
     * @brief PokeParty→セーブデータへの変換処理
     * @param pParty
     */
    //--------------------------------------------------------------
    void Serialize( const pml::PokeParty * pParty );

    //--------------------------------------------------------------
    /**
     * @brief セーブデータ→PokePartyへの変換処理
     * @param pParty
     */
    //--------------------------------------------------------------
    void Deserialize( pml::PokeParty * pParty );

  private:
    pml::PokeParty::SERIALIZE_DATA mData;
};  // PokePartySave

GFL_NAMESPACE_END(Savedata)

#endif //__POKEPARY_SAVE_H__
