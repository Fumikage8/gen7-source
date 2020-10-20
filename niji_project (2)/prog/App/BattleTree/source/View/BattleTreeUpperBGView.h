// ============================================================================
/*
 * @file UpperBGView.h
 * @brief バトルツリーアプリの上画面を表示するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_UPPER_BG_VIEW_H_INCLUDE )
#define BATTLE_TREE_UPPER_BG_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

#include "App/BattleTree/source/System/BattleTreeWork.h"

//  nijiのインクルード
#include "AppLib/include/Tool/PokeIconTexPool.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(View)

  class UpperBGView :
    public NetAppLib::System::NetApplicationViewBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( UpperBGView );
  public:
    enum PokeInfoPos
    {
      POKE_INFO_POS_LEFT  = 0,
      POKE_INFO_POS_RIGHT,
      POKE_INFO_POS_MAX,
    };

    UpperBGView( App::BattleTree::System::BattleTreeWork* pWork );
    virtual~UpperBGView();

    bool  InitFunc();
    bool  EndFunc();

    //  インフォテキストを指定
    void SetInfoText( const gfl2::str::STRCODE* pText );
    //  ポケモン表示設定
    void SetPokeInfo( const PokeTool::SimpleParam& rParam, const PokeInfoPos infoPos );

    // UIView
    virtual void Update(void);
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    enum LayoutWorkID
    {
      LAYOUT_WORK_ID_UPPER,
    };

    //  ポケモンインフォオブジェクト
    class PokeInfoObject
    {
    public:
//      PokeInfoObject(  gfl2::lyt::LytWk* pLayoutWork, app::tool::PokeIconToolEx* pIconToolEx, const u32 iconId, const PokeInfoPos posInfo );
      PokeInfoObject( gfl2::heap::HeapBase* pHeap, gfl2::lyt::LytWk* pLayoutWork, App::Tool::PokeIconTexPool* pPokeIconPool, const u32 iconId, const PokeInfoPos posInfo );
      virtual ~PokeInfoObject();

      void SetInfo( const PokeTool::SimpleParam& rParam, app::util::G2DUtil* pG2DUtil );
      void Update();

    private:
      void _Clear()
      {
        m_bReplace          = false;
        m_pNameTextBox      = NULL;
        m_pIconPic          = NULL;
        m_iconId            = 0;
        m_replaceCount      = 0;

        m_pPokeIconTexData  = NULL;
        m_pPokeIconTexPool  = NULL;
      }

      gfl2::lyt::LytTextBox*  m_pNameTextBox;
      gfl2::lyt::LytPicture*  m_pIconPic;

      bool  m_bReplace;
      s32   m_replaceCount;
      u32   m_iconId;
      PokeTool::SimpleParam                         m_simplePokeParam;
      App::Tool::PokeIconTexData*                   m_pPokeIconTexData;
      App::Tool::PokeIconTexPool*                   m_pPokeIconTexPool;

      gfl2::heap::HeapBase*                         m_pTempHeap;
    };

    void _Clear()
    {
      m_pWork                   = NULL;
      m_pInfoTextBox            = NULL;
      m_pPokeIconTexPool        = NULL;

      gfl2::std::MemClear( m_pPokeInfoObject, sizeof( m_pPokeInfoObject ) );
    }

    PokeInfoObject*                               m_pPokeInfoObject[ POKE_INFO_POS_MAX ];
    gfl2::lyt::LytTextBox*                        m_pInfoTextBox;

    App::BattleTree::System::BattleTreeWork*      m_pWork;
    App::Tool::PokeIconTexPool*                   m_pPokeIconTexPool;
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREE_UPPER_BG_VIEW_H_INCLUDE
