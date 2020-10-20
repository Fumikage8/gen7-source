//======================================================================
/**
 * @file FieldDebugSodateya.h
 * @date 2015/12/19 19:41:35
 * @author onoue_masayuki
 * @brief 育て屋デバッグメニュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDDEBUGSODATEYA_H_INCLUDED__
#define __FIELDDEBUGSODATEYA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#if PM_DEBUG

namespace gfl2
{
  namespace str
  {
    class MsgData;
  }
  namespace debug
  {
    class DebugWinGroup;
    class DebugWinItem;
  }
}

namespace debug
{
  namespace PokeMake
  {
    struct APP_PARAM;
  }
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)


class DebugSodateya
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugSodateya);
public:

  static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item );
  static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item );

  DebugSodateya();

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup* root );

  gfl2::heap::HeapBase* GetHeap( void ){ return this->pHeap; }

private:

  enum SodateyaMenuID{
    SODATEYA_MENU_BREED,
    SODATEYA_MENU_EGG_WARM,
    SODATEYA_MENU_EGG_SET,
    SODATEYA_MENU_EGG_DEL,
    SODATEYA_MENU_EGG_ADOPT,
    SODATEYA_MENU_POKE_LEAVE,
    SODATEYA_MENU_POKE_TAKEBACK,
    SODATEYA_MENU_MEMBER_DEL,
    SODATEYA_MENU_PP_CHECK,
  };

  gfl2::heap::HeapBase*         pHeap;
  gfl2::str::MsgData*     pMsgData;    
  
  //! メッセージデータ取得
  inline gfl2::str::MsgData* GetMessageData( void ) { return pMsgData; }

  //育てる
  static void DebugWinFunc_SodateyaBreed100( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaBreed10000( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaBreed1milion( void* userWork, gfl2::debug::DebugWinItem* item );
  ///タマゴ系
  static void DebugWinFunc_SodateyaEggWarm( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaEggSet( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaEggDel( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaEggAdopt( void* userWork, gfl2::debug::DebugWinItem* item );
  //ポケモン系
  static void DebugWinFunc_SodateyaPokeLeave( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaPokeTakeBack( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaPokeTakeBack2( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaPartyMemberDel( void* userWork, gfl2::debug::DebugWinItem* item );
  //PokeParaチェック系
  static void DebugWinFunc_SodateyaPokePara1Check( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaPokePara2Check( void* userWork, gfl2::debug::DebugWinItem* item );
  static void DebugWinFunc_SodateyaPokeParaEggCheck( void* userWork, gfl2::debug::DebugWinItem* item );

  static void DebugWinFunc_Sodateya( void* userWork, gfl2::debug::DebugWinItem* item, SodateyaMenuID menu, int param );
  static void PPCheck( debug::PokeMake::APP_PARAM* p_param, GameSys::GameManager* p_gman );

};



GFL_NAMESPACE_END(Debug)
  GFL_NAMESPACE_END(Field)

#endif //PM_DEBUG
#endif // __FIELDDEBUGSODATEYA_H_INCLUDED__