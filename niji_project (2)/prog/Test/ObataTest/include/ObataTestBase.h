//=============================================================================
/**
 * @file    ObataTestBase.h
 * @brief   ポケモンライブラリのテスト
 * @author  obata_toshihiro
 * @date    2015.05.15
 */
//=============================================================================
#if PM_DEBUG
#ifndef OBATATESTBASE_H_INCLUDED
#define OBATATESTBASE_H_INCLUDED
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>
#include <Print/include/WordSet.h>
#include <pml/include/poketype_def.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )


//=============================================================================
/**
 * @class  ObataTestBase
 * @brief  ポケモンライブラリのテストに共通する処理を定義するクラス
 */
//=============================================================================
class ObataTestBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ObataTestBase );

public:
  ObataTestBase( void );
  virtual ~ObataTestBase();


protected:
  void SetupBase( void );
  void CleanupBase( void );

  gfl2::heap::HeapBase* SystemHeap( void ) const;
  gfl2::heap::HeapBase* DeviceHeap( void ) const;
  const gfl2::ui::Button* UiButton( void ) const;

  static const u32 STRBUF_NUM = 3;
  gfl2::str::StrBuf* GetStrBuf( u32 bufIndex ) const;
  void GetMonsName( gfl2::str::StrBuf* destBuffer, const pml::pokepara::PokemonParam* pokeParam ) const;
  void GetMonsName( gfl2::str::StrBuf* destBuffer, MonsNo monsno ) const;
  void GetWazaName( gfl2::str::StrBuf* destBuffer, WazaNo wazano ) const;
  void GetTokuseiName( gfl2::str::StrBuf* destBuffer, TokuseiNo tokuseiNo ) const;
  void GetSeikakuName( gfl2::str::StrBuf* destBuffer, pml::pokepara::Seikaku seikaku ) const;


private:
  void CreateHeap( void );
  void DeleteHeap( void );
  void CreateMessageData( void );
  void DeleteMessageData( void );
  void CreateStrBuf( void );
  void DeleteStrBuf( void );
  void CreateWordset( void );
  void DeleteWordset( void );

  static const u32 PRIVATE_STRBUF_NUM = 2;

  gfl2::heap::HeapBase* m_systemHeap;
  gfl2::heap::HeapBase* m_deviceHeap;
  gfl2::str::MsgData* m_messageData;
  gfl2::str::StrBuf* m_publicStrBuf[ STRBUF_NUM ];
  gfl2::str::StrBuf* m_privateStrBuf[ PRIVATE_STRBUF_NUM ];
  print::WordSet* m_wordset;
};


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )


#endif // OBATATESTBASE_H_INCLUDED
#endif // PM_DEBUG

