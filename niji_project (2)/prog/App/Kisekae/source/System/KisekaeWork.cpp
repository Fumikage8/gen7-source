// ============================================================================
/*
 * @file KisekaeWork.h
 * @brief きせかえアプリのワーククラスです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/System/KisekaeWork.h"
#include "App/Kisekae/source/Tool/CharaModelViewer.h"

//  nijiのインクルード
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "System/include/ArcIdGetter.h"

#include "System/include/DressUp.h"

//  nijiのインクルードIsColorDressup
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "model/include/gfl2_CharaModelFactory.h"

//  きせかえデータベース
#include "KisekaeData/include/KisekaeDataBase.h"

//  メッセージのインクルード
#include <arc_index/message.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

  //  no
  u16 ItemDataBase::GetNo()
  {
    return  (u16)m_pGoodsData->no;
  }

  //  カテゴリー
  Savedata::Fashion::Category ItemDataBase::GetCategory()
  {
    return  m_saveCategory;
  }

  //  アイテム名のtextid
  u32 ItemDataBase::GetItemTxtID()
  {
    if( m_pGoodsData == NULL )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return  m_pGoodsData->goods_txt_id;
  }

  //  色情報名のtextid
  u32 ItemDataBase::GetColorTxtID()
  {
    if( m_pGoodsData == NULL )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return  m_pGoodsData->color_txt_id;
  }

  //  購入する
  void ItemDataBase::SetBuy()
  {
    m_pFashionSave->Add( m_saveCategory, m_pGoodsData->item_id );
    m_bBuy  = true;
    //  色染め用のアイテムを購入した時にすでに別の色染め用のをもっているかチェック
    //  もっていたらそれはバグなのでアサートでとめる
    if( ItemList::IsMultipleHaveByDebug( m_saveCategory, m_sex, m_pGoodsData->item_id, m_pDressupModelResourceManagerCoreDebugChk ) == true )
    {
      GFL_ASSERT( 0 );
    }
  }

  //  newフラグがあるか
  bool ItemDataBase::IsNew()
  {
    return  m_pFashionSave->IsNew( m_saveCategory, m_pGoodsData->item_id );
  }

  //  newフラグをクリア
  void ItemDataBase::ClearNew()
  {
    m_pFashionSave->ClearNewFlag( m_saveCategory, m_pGoodsData->item_id );
  }

  //  金額
  u32 ItemDataBase::GetMoney()
  {
    if( m_pGoodsData == NULL )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return  m_pGoodsData->sell_num;
  }

  //  パーツid
  s16 ItemDataBase::GetItemId()
  {
    return  m_partsId;
  }

  //  アイテムアイコンのテクスチャーid
  u32 ItemDataBase::GetItemIconTexId()
  {
    if( m_pGoodsData == NULL )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return  m_pGoodsData->tex_id;
  }

  //  基本アイテムリスト
  ItemList::ItemList( const poke_3d::model::DressUpParam::Sex sex )
  {
    _Clear();

    m_sex = sex;
  }

  ItemList::~ItemList()
  {
  }

  //  着せ替えアイテム名のmstxtのgarcId
  gfl2::fs::ArcFile::ARCDATID ItemList::GetItemTextMsTxtArcDatId()
  {
    return  GARC_message_kisekae_item_name_DAT;
  }

  //  色情報名のmstxtのgarcId
  gfl2::fs::ArcFile::ARCDATID ItemList::GetColorTextMsTxtArcDatId()
  {
    return  GARC_message_kisekae_color_DAT;
  }

  void ItemList::SetActiveCategory( const Savedata::Fashion::Category category )
  {
    if( GFL_NELEMS( m_aCategoryData ) <= category )
    {
      GFL_ASSERT( 0 );
      return;
    }

    m_oldActiveCategory = m_activeCategory;
    m_activeCategory  = (u32)category;
  }

  ItemDataBase* ItemList::GetData( const u32 listIdx )
  {
    return GetData( m_activeCategory, listIdx );
  }

  ItemDataBase* ItemList::GetData( const u32 categoryIdx, const u32 listIdx )
  {
    if( GFL_NELEMS( m_aCategoryData ) <= categoryIdx )
    {
      return  NULL;
    }
    else
    if( m_aCategoryData[ categoryIdx ].num <= listIdx )
    {
      return  NULL;
    }

    return m_aCategoryData[ categoryIdx ].pData[ listIdx ];
  }

  u32 ItemList::GetDataMax()
  {
    return GetDataMax( m_activeCategory );
  }

  u32 ItemList::GetDataMax( const u32 categoryIdx )
  {
    if( GFL_NELEMS( m_aCategoryData ) <= categoryIdx )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return m_aCategoryData[ categoryIdx ].num;
  }

  //  リストのカーソル位置取得
  u32 ItemList::GetListScroll()
  {
    if( GFL_NELEMS( m_aCategoryData ) <= m_activeCategory )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return m_aCategoryData[ m_activeCategory ].listScroll;

  }

  f32 ItemList::GetListPos()
  {
    if( GFL_NELEMS( m_aCategoryData ) <= m_activeCategory )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return m_aCategoryData[ m_activeCategory ].listPos;
  }

  //  リストのカーソル位置設定
  void ItemList::SetListScroll( const u32 listScroll )
  {
    if( GFL_NELEMS( m_aCategoryData ) <= m_activeCategory )
    {
      GFL_ASSERT( 0 );
      return;
    }

    m_aCategoryData[ m_activeCategory ].listScroll = listScroll;
  }

  void ItemList::SetListPos( const f32 listPos )
  {
    if( GFL_NELEMS( m_aCategoryData ) <= m_activeCategory )
    {
      GFL_ASSERT( 0 );
      return;
    }

    m_aCategoryData[ m_activeCategory ].listPos  = listPos;
  }

  bool ItemList::IsColorDressup( const u32 colorId )
  {
    //  色染め用のカラーなら色染め対象としてリスト項目データ取得
    {
      static const u32 aColorId[]  =
      {
       DRESSUP_COLOR_ID_CLOTH_WH,
       DRESSUP_COLOR_ID_CLOTH_RD1,
       DRESSUP_COLOR_ID_CLOTH_RD2,
       DRESSUP_COLOR_ID_CLOTH_RD3,
       DRESSUP_COLOR_ID_CLOTH_YE1,
       DRESSUP_COLOR_ID_CLOTH_YE2,
       DRESSUP_COLOR_ID_CLOTH_YE3,
       DRESSUP_COLOR_ID_CLOTH_GR1,
       DRESSUP_COLOR_ID_CLOTH_GR2,
       DRESSUP_COLOR_ID_CLOTH_GR3,
       DRESSUP_COLOR_ID_CLOTH_BL1,
       DRESSUP_COLOR_ID_CLOTH_BL2,
       DRESSUP_COLOR_ID_CLOTH_BL3,
       DRESSUP_COLOR_ID_CLOTH_OR1,
       DRESSUP_COLOR_ID_CLOTH_OR2,
       DRESSUP_COLOR_ID_CLOTH_OR3,
       DRESSUP_COLOR_ID_CLOTH_NB1,
       DRESSUP_COLOR_ID_CLOTH_NB2,
       DRESSUP_COLOR_ID_CLOTH_NB3,
       DRESSUP_COLOR_ID_CLOTH_PP1,
       DRESSUP_COLOR_ID_CLOTH_PP2,
       DRESSUP_COLOR_ID_CLOTH_PP3,
       DRESSUP_COLOR_ID_CLOTH_PN1,
       DRESSUP_COLOR_ID_CLOTH_PN2,
       DRESSUP_COLOR_ID_CLOTH_PN3,
       DRESSUP_COLOR_ID_CLOTH_LG,
      };

      u32 loopNum = GFL_NELEMS( aColorId );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( aColorId[ i ] == colorId )
        {
          return  true;
        }
      }
    }

    return false;
  }

  //  きせかえアイテムが重複所持しているかチェック(デバッグ用)
  bool ItemList::IsMultipleHaveByDebug( const Savedata::Fashion::Category saveCategory, poke_3d::model::DressUpParam::Sex sex, const s32 chkItemId, poke_3d::model::DressUpModelResourceManagerCore* pDressupResManger )
  {
  //  デバッグ版のみチェックする(処理が重いので)
  #if PM_DEBUG
    if( pDressupResManger == NULL )
    {
      return  false;
    }

    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    poke_3d::model::DressUpParam::ItemCategory  dressUpItemCategory = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( saveCategory );
    //  色染め用のカラーidをもっているか場合はこのアイテムは色染め用と判断
    const table_data_GoodsData* pSearchItemData = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( sex, saveCategory, chkItemId );
    // @fix NMCat[732] 色染めフラグがないので次へ
    if( pSearchItemData == NULL )
    {
      return  false;
    }
    else
    if( pSearchItemData->color_change == 0 )
    {
      return  false;
    }

    u32 haveCount = 0;
    //  パーツidを取得する
    u32 nowPartsId  = pDressupResManger->GetDressUpPartsIndex( dressUpItemCategory, chkItemId );

    Savedata::Fashion*  pFashionSave  = pGameManager->GetGameData()->GetFashion();
    //  １カテゴリーの全アイテムを見ているのでループ数は多い
    u32 loopNum = pFashionSave->GetCagoryItmeDataNum( saveCategory );
    for( u32 itemId = 0; itemId < loopNum; ++itemId )
    {
      //  アイテムを保有しているかチェック
      if( pFashionSave->IsOwned( saveCategory, itemId ) == false )
      {
        continue;
      }

      //  パーツidを取得して同じパーツidか見る
      u32 chkPartsId  = pDressupResManger->GetDressUpPartsIndex( dressUpItemCategory, itemId );
      //  パーツidが同じであれば、パーツが同じで色違いということになる
      if( chkPartsId == nowPartsId )
      {
        const table_data_GoodsData* pChkItemData = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( sex, saveCategory, itemId );
        if( pChkItemData != NULL )
        {
          // @fix NMCat[732] 色染めフラグがあるので色染め用と判断
          if( pChkItemData->color_change == 1 )
          {
            ++haveCount;
          }
        }
      }
    }

    //  所持数が１より多い場合は多重所持になっている
    return  ( 1 < haveCount );
  #else

    return  false;
  #endif

  }

  //  ショップリスト
  ShopItemList::ShopItemList( const app::util::Kisekae::KisekaeShopID shopId, const poke_3d::model::DressUpParam::Sex sex, poke_3d::model::DressUpModelResourceManagerCore* pDressupResManger )
  : ItemList( sex )
  {
    //  ショップリストデータ作成
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    Savedata::Fashion*  pFashionSave  = pGameManager->GetGameData()->GetFashion();

    u32 listNum = app::util::Kisekae::KisekaeDataBase::GetShopsDataNum( shopId );

    u32 freeDataIdx  = 0;
    ItemData* pData = NULL;
    CATEGORY_DATA*  pCurrentCategoryData  = NULL;
    for( u32 i = 0; i < listNum; ++i )
    {
      GFL_ASSERT( freeDataIdx < GFL_NELEMS( m_aData ) );

      pData = NULL;

      app::util::Kisekae::KisekaeDataBase::SHOP_DATA  shopData  = app::util::Kisekae::KisekaeDataBase::GetShopData( shopId, i );

      //  データがないのでスキップ
      if( shopData.pGoodsData == NULL )
      {
        continue;
      }
      else
      //  性別が異なるとアウト
      if( m_sex != shopData.pGoodsData->sex )
      {
        continue;
      }

      //  バッファアドレスを参照する
      pData = &m_aData[ freeDataIdx ];

      pData->m_saveCategory = (Savedata::Fashion::Category)shopData.category;
      pData->m_pFashionSave = pFashionSave;
      pData->m_bBuy         = pFashionSave->IsOwned( pData->m_saveCategory, shopData.pGoodsData->item_id );
      pData->m_pGoodsData   = shopData.pGoodsData;
      pData->m_sex          = m_sex;
      pData->m_partsId      = pData->m_pGoodsData->item_id;
      //  デバッグチェックのためにリソースアドレスを設定（扱うには注意）
      pData->m_pDressupModelResourceManagerCoreDebugChk = pDressupResManger;

      //  きせかえアイテムの場合、すでに持っている場合は購入済みにする
      if( pData->m_bBuy == false )
      {
        //  すでにもっているかチェックしてもっていたら購入済みにする
        //  ※色違いで同じパーツは重複してセーブしていないのが前提なので多重になることはない
        if( _IsHaveColorDressup( shopData.category, *pData, pDressupResManger ) == true )
        {
          //  すでに色染めを持っているので重複して入手は不可能なので購入済みに
          pData->m_bBuy = true;
        }
      }

      //  リストに追加
      {
        pCurrentCategoryData  = &m_aCategoryData[ shopData.category ];
        pCurrentCategoryData->pData[ pCurrentCategoryData->num ]  = pData;
        ++pCurrentCategoryData->num;
        ++freeDataIdx;
      }
    }
  }

  //  色染め用の着せ替えを持っているか
  bool ShopItemList::_IsHaveColorDressup( const Savedata::Fashion::Category category, const ItemData& rItemData, poke_3d::model::DressUpModelResourceManagerCore* pDressupResManger )
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    poke_3d::model::DressUpParam::ItemCategory  dressUpItemCategory = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( category );
    //  色染め用のカラーかどうか
    if( rItemData.m_pGoodsData->color_change == 0 )
    {
      return  false;
    }

    //  パーツidを取得する
    u32 nowPartsId  = pDressupResManger->GetDressUpPartsIndex( dressUpItemCategory, rItemData.m_pGoodsData->item_id );

    Savedata::Fashion*  pFashionSave  = pGameManager->GetGameData()->GetFashion();
    //  １カテゴリーの全アイテムを見ているのでループ数は多い
    u32 loopNum = pFashionSave->GetCagoryItmeDataNum( category );
    for( u32 itemId = 0; itemId < loopNum; ++itemId )
    {
      //  アイテムを保有しているかチェック
      if( pFashionSave->IsOwned( category, itemId ) == false )
      {
        continue;
      }
      
      //  きせかえidと同じidならチェックは不要
      if( itemId == (u32)rItemData.m_pGoodsData->item_id )
      {
        continue;
      }

      //  パーツidを取得して同じパーツidか見る
      u32 chkPartsId  = pDressupResManger->GetDressUpPartsIndex( dressUpItemCategory, itemId );
      //  パーツidが同じであれば、パーツが同じで色違いということになる
      if( chkPartsId == nowPartsId )
      {
        //  ※色違いで同じパーツは重複してセーブしていないのが前提
        //  チェックしたアイテムが色染めなのかどうかチェック
        //  同じパーツでも色染めとそうでない色があるので
        const table_data_GoodsData* pSearchItemData = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( m_sex, category, itemId );
        if( pSearchItemData == NULL )
        {
        }
        else
          // @fix NMCat[732] 色染めフラグがあるので色染め用と判断
        if( pSearchItemData->color_change == 1 )
        {
          return  true;
        }
      }
    }

    return  false;
  }

  //  所持リスト
  HaveItemList::HaveItemList( const poke_3d::model::DressUpParam::Sex sex )
  : ItemList( sex )
  {
    //  所持リストデータ作成
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    //  デフォルト着せ替えでつけはずしできないデータを取得
    {
      if( m_sex == poke_3d::model::DressUpParam::SEX_MALE )
      {
        ::System::DressUp::GetDressUpParamDefaultIndispensableHero( &m_defaultIndispensableDressUpParam );
      }
      else
      if( m_sex == poke_3d::model::DressUpParam::SEX_FEMALE )
      {
        ::System::DressUp::GetDressUpParamDefaultIndispensableHeroine( &m_defaultIndispensableDressUpParam );
      }
      else
      {
        GFL_ASSERT( 0 );
      }
    }

    Savedata::Fashion*  pFashionSave  = pGameManager->GetGameData()->GetFashion();

    u32 loopNum = GFL_NELEMS( m_aCategoryData );

    ItemData* pData = NULL;
    u32 freeDataIdx = 0;
    for( u32 categoryIdx = 0; categoryIdx < loopNum; ++categoryIdx )
    {
      GFL_ASSERT( freeDataIdx < GFL_NELEMS( m_aData ) );

      Savedata::Fashion::Category saveDressupCategory = (Savedata::Fashion::Category)categoryIdx;

      //  セーブのカテゴリーからきせかえパラメータのカテゴリーに変換
      poke_3d::model::DressUpParam::ItemCategory  dressUpParamCategory  = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( saveDressupCategory );

      u32 listMax = pFashionSave->GetCagoryItmeDataNum( saveDressupCategory );
      for( u32 item_id = 0; item_id < listMax; ++item_id )
      {
        //  デフォルトきせかえではずすことができないのはリストに加えない
        if( m_defaultIndispensableDressUpParam.itemParams[ dressUpParamCategory ] == (s32)item_id )
        {
          continue;
        }

        if( pFashionSave->IsOwned( categoryIdx, item_id ) == 0 )
        {
          continue;
        }

        const table_data_GoodsData* pGoodsData  = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( m_sex, saveDressupCategory, item_id );

        //  着せ替えデータベースにない
        if( pGoodsData == NULL )
        {
          continue;
        }

        //  性別が異なるとアウト
        if( m_sex != pGoodsData->sex )
        {
          continue;
        }

        pData = &m_aData[ freeDataIdx ];

        pData->m_saveCategory = saveDressupCategory;
        pData->m_pFashionSave = pFashionSave;
        pData->m_sex          = m_sex;
        pData->m_pGoodsData   = pGoodsData;

        pData->m_partsId  = pData->m_pGoodsData->item_id;

        m_aCategoryData[ categoryIdx ].pData[ m_aCategoryData[ categoryIdx ].num ]  = pData;

        //  リスト数を増やす
        ++m_aCategoryData[ categoryIdx ].num;
        ++freeDataIdx;
      }
    }
  }

  //  所持リストのアイテムデータ
      
  //  既読にする
  bool HaveItemList::ItemData::Alrady()
  {
    Savedata::Fashion* pFashion = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFashion();
    pFashion->ClearNewFlag( m_saveCategory, m_pGoodsData->item_id );

    return  true;
  }

  //  パーツid
  s16 HaveItemList::ItemData::GetItemId()
  {
    //  色そめを持っているのであればそちらのidを優先する
    return  ItemDataBase::GetItemId();
  }

  //  色リスト
  ColorItemList::ColorItemList( const poke_3d::model::DressUpParam::Sex sex, const u32 shopId, const s32 colorId, const bool bUseKinomi, poke_3d::model::DressUpModelResourceManagerCore* pDressUpResource )
  : ItemList( sex )
  {
    _Clear();

    //  指定した色で染めるのでデフォルトの色は含めない

    GFL_ASSERT( pDressUpResource );

    //  リストデータ作成
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    CREATE_DATA aCreateArray[]  =
    {
      {
        colorId,
        0,
      },
    };

    u32 createDataMax = GFL_NELEMS( aCreateArray );

    //  消費コイン取得
    {
      u32 shopDataListMax   = app::util::Kisekae::KisekaeDataBase::GetColorShopDataNum( shopId );
      const table_data_DyeingData* pShopData  = NULL;
      for( u32 shopDataListIdx = 0; shopDataListIdx < shopDataListMax; ++shopDataListIdx)
      {
        pShopData  = app::util::Kisekae::KisekaeDataBase::GetColorShopData( shopId, shopDataListIdx );
        if( pShopData == NULL )
        {
          continue;
        }

        //  作成するデータにコイン値の設定をする
        {
          for( u32 createDataIdx = 0; createDataIdx < createDataMax; ++createDataIdx )
          {
            if( aCreateArray[ createDataIdx ].colorId == pShopData->color_id )
            {
              if( _IsSelectColor( pShopData, bUseKinomi ) == false )
              {
                break;
              }

              if( bUseKinomi == true )
              {
                aCreateArray[ createDataIdx ].useCoin = pShopData->kinomi_coin;
              }
              else
              {
                aCreateArray[ createDataIdx ].useCoin = pShopData->coin;
              }

              break;
            }
          }
        }
      }
    }

    //  データ作成
    {
      _CreateData( sex, aCreateArray, createDataMax, bUseKinomi, pDressUpResource );
    }

    Refresh();
  }

  //  ショップの全色指定したデータリスト作成
  ColorItemList::ColorItemList( 
      const poke_3d::model::DressUpParam::Sex sex,
      const u32 shopId,
      const bool bUseKinomi,
      poke_3d::model::DressUpModelResourceManagerCore* pDressUpResource )
      : ItemList( sex )
  {
    _Clear();

    GFL_ASSERT( pDressUpResource );

    CREATE_DATA aCreateDataArray[ 32 ];
    gfl2::std::MemClear( aCreateDataArray, sizeof( aCreateDataArray ));

    u32 colorIdArrayNum = 0;

    //  カラーリストデータを作成
    {
      u32 shopDataListMax = app::util::Kisekae::KisekaeDataBase::GetColorShopDataNum( shopId );
      const table_data_DyeingData* pShopData  = NULL;
      for( u32 shopDataListIdx = 0; shopDataListIdx < shopDataListMax; ++shopDataListIdx)
      {
        pShopData  = app::util::Kisekae::KisekaeDataBase::GetColorShopData( shopId, shopDataListIdx );
        if( pShopData == NULL )
        {
          continue;
        }

        if( _IsSelectColor( pShopData, bUseKinomi ) == false )
        {
          continue;
        }

        aCreateDataArray[ colorIdArrayNum ].colorId  = pShopData->color_id;

        if( bUseKinomi == true )
        {
          aCreateDataArray[ colorIdArrayNum ].useCoin  = pShopData->kinomi_coin;
        }
        else
        {
          aCreateDataArray[ colorIdArrayNum ].useCoin  = pShopData->coin;
        }

        ++colorIdArrayNum;
      }
    }

    //  リストデータ作成
    {
      _CreateData( sex, aCreateDataArray, colorIdArrayNum, bUseKinomi, pDressUpResource );
    }

    Refresh();
  }

  void ColorItemList::Refresh()
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    Savedata::Fashion*  pFashionSave  = pGameManager->GetGameData()->GetFashion();

    u32 categoryLoopNum = GFL_NELEMS( m_aOrgCategoryData );

    CATEGORY_DATA*  pOrgCategoryData      = NULL;
    CATEGORY_DATA*  pSettingCategoryData  = NULL;
    ColorItemList::ItemData*  pItemData   = NULL;
    for( u32 categoryIdx = 0; categoryIdx < categoryLoopNum; ++categoryIdx )
    {
      pOrgCategoryData      = &m_aOrgCategoryData[ categoryIdx ];
      pSettingCategoryData  = &m_aCategoryData[ categoryIdx ];

      pSettingCategoryData->num = 0;
      for( u32 itemIdx = 0; itemIdx < pOrgCategoryData->num; ++itemIdx )
      {
        //  すでに色染めしているのか
        pItemData = (ColorItemList::ItemData*)pOrgCategoryData->pData[ itemIdx ];

        //  すでに色染めしている
        if( pFashionSave->IsOwned( categoryIdx, pItemData->m_pGoodsData->item_id ) != 0 )
        {
          continue;
        }

        pSettingCategoryData->pData[ pSettingCategoryData->num ]  = pOrgCategoryData->pData[ itemIdx ];
        ++pSettingCategoryData->num;
      }
    }
  }

  //  データ生成
  void ColorItemList::_CreateData(
    const poke_3d::model::DressUpParam::Sex sex,
    const CREATE_DATA* pCrateDataArray,
    const u32  createDataArrayNum,
    const bool bUseKinomi,
    poke_3d::model::DressUpModelResourceManagerCore* pDressUpResource )
  {
    if( pCrateDataArray == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }
    else
    if( createDataArrayNum <= 0 )
    {
      GFL_ASSERT( 0 );
      return;
    }

    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    Savedata::Fashion*  pFashionSave  = pGameManager->GetGameData()->GetFashion();

    u32 loopNum = GFL_NELEMS( m_aCategoryData );
    for( u32 categoryIdx = 0; categoryIdx < loopNum; ++categoryIdx )
    {
      u32 listMax = pFashionSave->GetCagoryItmeDataNum( (Savedata::Fashion::Category)categoryIdx );
      for( u32 item_id = 0; item_id < listMax; ++item_id )
      {
        //  色染め含めて必ず１種類しかもっていない前提
        if( pFashionSave->IsOwned( categoryIdx, item_id ) == 0 )
        {
          continue;
        }

        {
          //  所持しているきせかえから色きせかえデータを取得
          const table_data_GoodsData* pGoodsData   = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( m_sex, (Savedata::Fashion::Category)categoryIdx, item_id );
          if( pGoodsData == NULL )
          {
            continue;
          }

          // @fix NMCat[732] 色染めフラグがないので次へ
          if( pGoodsData->color_change == 0 )
          {
            continue;
          }

          //  ショップの全色判定
          for( u32 colorIdArrayIdx = 0; colorIdArrayIdx < createDataArrayNum; ++colorIdArrayIdx )
          {
            //  カラーidからdressupParmaのカラー用アイテムidを取得
            u32 itemId  = _GetColorItemId( pDressUpResource, pCrateDataArray[ colorIdArrayIdx ].colorId, (Savedata::Fashion::Category)categoryIdx, pGoodsData );
            if( itemId == ERROR_CODE_ITEM_ID )
            {
              continue;
            }

            //  きせかえデータベースから色染め用のitemidを検索
            const table_data_GoodsData* pColorGoodsData  = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( m_sex, (Savedata::Fashion::Category)categoryIdx, itemId );
            if( pColorGoodsData == NULL )
            {
              continue;
            }

            // @fix NMCat[732] 色染めフラグがないので次へ
            if( pColorGoodsData->color_change == 0 )
            {
              continue;
            }

            //  カテゴリーリストに追加
            _Add( (Savedata::Fashion::Category)categoryIdx, item_id, pColorGoodsData, pCrateDataArray[ colorIdArrayIdx ], pDressUpResource );
          }
        }
      }
    }
  }

  //  色選択対象か
  bool ColorItemList::_IsSelectColor( const table_data_DyeingData* pShopData, const bool bUseKinomi )
  {
    if( pShopData == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    //  コイン値がないのは選択できない
    if( bUseKinomi == true )
    {
      if( pShopData->kinomi_coin <= 0 )
      {
        return  false;
      }
    }
    else
    {
      if( pShopData->coin <= 0 )
      {
        return  false;
      }
    }

    return  true;
  } 

  void ColorItemList::_Add( 
  const Savedata::Fashion::Category saveCategoryIdx, const u32 colorSaveDataNo,
  const table_data_GoodsData* pColorGoodsData, const CREATE_DATA& createData, poke_3d::model::DressUpModelResourceManagerCore* pDressupResManger )
  {
    GFL_ASSERT( m_freeDataListIdx < GFL_NELEMS( m_aData ) );
    GFL_ASSERT( pDressupResManger );

    if( m_sex != pColorGoodsData->sex )
    {
      return;
    }

    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    Savedata::Fashion*  pFashionSave  = pGameManager->GetGameData()->GetFashion();

    ItemData* pData = &m_aData[ m_freeDataListIdx ];

    pData->m_pGoodsData       = pColorGoodsData;
    pData->m_saveCategory     = saveCategoryIdx;
    pData->m_pFashionSave     = pFashionSave;
    pData->m_sex              = m_sex;
    pData->m_colorSaveDataNo  = colorSaveDataNo;
    pData->m_partsId          = pData->m_pGoodsData->item_id;
    pData->m_pDressUpResource = pDressupResManger;
    pData->m_pItemList        = this;

    pData->m_money    = createData.useCoin;
    pData->m_colorId  = createData.colorId;

    m_aOrgCategoryData[ saveCategoryIdx ].pData[ m_aOrgCategoryData[ saveCategoryIdx ].num ]  = pData;

    //  リスト数を増やす
    ++m_aOrgCategoryData[ saveCategoryIdx ].num;
    ++m_freeDataListIdx;
  }

  //  カラーid/きせかえsaveid/きせかえデータからきせかえアイテムidを取得
  s32 ColorItemList::_GetColorItemId( poke_3d::model::DressUpModelResourceManagerCore* pDressUpResource, const u32 colorId, const Savedata::Fashion::Category saveCategory, const table_data_GoodsData* pGoodsData )
  {    
    if( pGoodsData->sex != m_sex )
    {
      return  ERROR_CODE_ITEM_ID;
    }

    // @fix NMCat[732] 色染めフラグがないので次へ
    if( pGoodsData->color_change == 0 )
    {
      return  ERROR_CODE_ITEM_ID;
    }

    poke_3d::model::DressUpParam::ItemCategory  dressUpItemCategory = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( saveCategory );
    u32 partsIdx  = pDressUpResource->GetDressUpPartsIndex( dressUpItemCategory, pGoodsData->item_id );

    //  所持しているきせかえアイテムが指定した色そめデータを持っているか
    if( pDressUpResource->IsDressUpPartsColorEnabled( dressUpItemCategory, partsIdx, colorId ) == false )
    {
      return  ERROR_CODE_ITEM_ID;
    }

    return  pDressUpResource->GetDressUpItemIndex( dressUpItemCategory, partsIdx, colorId );
  }

  //  色リストのアイテムデータ

  ColorItemList::ItemData::ItemData() :
  ItemDataBase()
  {
    _Clear();
  }

  //  金額
  u32 ColorItemList::ItemData::GetMoney()
  {
    return  m_money;
  }

  //  色染めする
  void ColorItemList::ItemData::SettingColor()
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GFL_ASSERT( pGameManager );

    Savedata::Fashion*  pFashionSave  = pGameManager->GetGameData()->GetFashion();

    u32 addItemId = GetItemId();

    u32 bNew  = 0;
    //  色染め用のアイテムidを所持する
    //  色染めにかかわるアイテムidをすべて消して今色染めしているアイテムidのみを所持する
    {
      poke_3d::model::DressUpParam::ItemCategory  dressUpItemCategory = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( GetCategory() );
      u32 partsIdx  = m_pDressUpResource->GetDressUpPartsIndex( dressUpItemCategory, addItemId );

      //  もともとの色を含めて色染めをすべて所持からはずす
      //  色染めも一つのアイテムになっているので
      {
        // @fix GFNMCat[2114] 色種類指定が３２だったのが３７になっていた
        //                    マジックナンバーだったので色定義数に直した
        for( u32 i = 0; i < DRESSUP_COLOR_ID_CLOTH_COUNT;  ++i )
        {
          // @fix GFNMCat[2114] 色判定を自前でやっていたが、すでに関数が用意しているのでこれを使用
          if( m_pDressUpResource->IsDressUpPartsColorEnabled( dressUpItemCategory, partsIdx, i ) == false )
          {
            //  対応したカラービットでないのでスキップ
            continue;
          }

          u32 itemId  = m_pDressUpResource->GetDressUpItemIndex( dressUpItemCategory, partsIdx, i );
          const table_data_GoodsData* pColorGoodsData  = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( m_sex, GetCategory(), itemId );
          if( pColorGoodsData == NULL )
          {
            //  色染め用でないのでスキップ
            continue;
          }

          // @fix NMCat[732] 色染めフラグがないので次へ
          if( pColorGoodsData->color_change == 0 )
          {
            continue;
          }

          //  newを設定している場合は設定しなおす
          if( bNew == 0 )
          {
            bNew  = pFashionSave->IsNew( GetCategory(), itemId );
          }

          pFashionSave->Del( GetCategory(), (s32)itemId );
        }
      }
    }

    //  最後に残った色染めを所持する(もともとの色染めしていないのも所持からはずす)
    pFashionSave->Add( GetCategory(), (s32)addItemId );

    if( bNew == 0 )
    {
      //  色染めのアイテムがnewになっていないのではずす
      pFashionSave->ClearNewFlag( GetCategory(), (s32)GetItemId() );
    }
  }

  //  色染め色がデフォルトか
  bool ColorItemList::ItemData::IsColorDefault()
  {
    static const s32 aDefaultColorId[] =
    {
      DRESSUP_COLOR_ID_CLOTH_WH,
      DRESSUP_COLOR_ID_CLOTH_LG
    };

    u32 loopNum = GFL_NELEMS( aDefaultColorId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( aDefaultColorId[ i ] == m_colorId )
      {
        return  true;
      }
    }

    return  false;
  }

  Work::Work( APP_PARAM* pAppParam ) :
    ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE )
  {
    _Clear();

    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;

    app::util::Heap*  pUseAppHeap = ApplicationWorkBase::GetAppHeap();

    //  作業用の文字列
    m_pTempStrBufArray[ 0 ] = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 512, pUseAppHeap->GetDeviceHeap() );
    m_pTempStrBufArray[ 1 ] = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 512, pUseAppHeap->GetDeviceHeap() );

    m_pCommonGrpIcon  = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) app::tool::AppCommonGrpIconData();
    m_pCommonGrpIcon->Initialize( pUseAppHeap );
  }


  Work::~Work()
  {
    m_pCommonGrpIcon->Terminate();
    GFL_SAFE_DELETE( m_pCommonGrpIcon );

    {
      u32 loopNum = GFL_NELEMS( m_pTempStrBufArray );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_pTempStrBufArray[ i ] );
      }
    }

    GFL_SAFE_DELETE( m_pAcitveItemList );
  }

  //  このクラスが使えるか
  bool Work::IsReady()
  {
    return  m_pCommonGrpIcon->IsReady();
  }

  //  リスト作成
  void Work::CreateItemList()
  {
    if( m_pAcitveItemList != NULL )
    {
      //  一度しか作らない
      GFL_ASSERT( 0 );
      return;
    }

    app::util::Heap*  pUseAppHeap = ApplicationWorkBase::GetAppHeap();

    //  性別取得
    Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
    poke_3d::model::DressUpParam::Sex sex = (poke_3d::model::DressUpParam::Sex)pMyStatus->GetSex();

    if( m_pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_SHOP )
    {
      //  ショップ
      m_pAcitveItemList = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) ShopItemList( m_pAppParam->typeData.shop.shopId, sex, m_pCharaModelViewr->GetDressUpModelResourceManagerCore( sex ) );
    }
    else
    if( m_pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_DRESS_UP || m_pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_FINDER_STUDIO )
    {
      //  きせかえ
      m_pAcitveItemList = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) HaveItemList( sex );
    }
    else
    if( m_pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_COLOR_SHOP )
    {
      //  いろ染め
      if( m_pAppParam->typeData.colorShop.colorId == App::Kisekae::ALL_COLOR_SELECT )
      {
        //  ショップの色すべてを見てリスト作成
        m_pAcitveItemList = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) ColorItemList( 
        sex,
        m_pAppParam->typeData.colorShop.shopId,
        m_pAppParam->typeData.colorShop.bUseKinomiDiscount,
        m_pCharaModelViewr->GetDressUpModelResourceManagerCore( sex ) );
      }
      else
      {
        //  特定の色を指定して、ショップにある色でリスト作成
        m_pAcitveItemList = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) ColorItemList( 
        sex,
        m_pAppParam->typeData.colorShop.shopId,
        m_pAppParam->typeData.colorShop.colorId,
        m_pAppParam->typeData.colorShop.bUseKinomiDiscount,
        m_pCharaModelViewr->GetDressUpModelResourceManagerCore( sex ) );
      }
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  void Work::SetViewSystemAccessor( ViewSystemAccessor* pAccessor )
  {
    GFL_ASSERT( pAccessor );

    m_pViewSystemAccessor = pAccessor;
  }

  //  モデルビューア参照の設定
  void Work::SetModelViewer( app::tool::CharaModelViewer* pAddr )
  {
    GFL_ASSERT( pAddr );
    m_pCharaModelViewr  = pAddr;
  }

  //  着せ替えデータ出力
  void Work::OutputDressupParam( poke_3d::model::DressUpParam* pOutputParam, const poke_3d::model::DressUpParam& rNewParam, poke_3d::model::DressUpParam rOldParam )
  {
    if( pOutputParam == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }
    else
    if( m_pCharaModelViewr == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    // @fix NMCat[1234] きせかえできない場合は変更前のきせかえデータに変える
    //                  こうしないとカテゴリー切り替え前のきせかえ表示が残るから
    if( m_pCharaModelViewr->CheckDressUpCombination( rNewParam ) == false )
    {
      gfl2::std::MemCopy( &rOldParam, pOutputParam, sizeof( rOldParam ) );
    }
  }

  gfl2::str::StrBuf*  Work::GetTmpStrBuf( const u32 idx )
  {
    if( GFL_NELEMS( m_pTempStrBufArray ) <= idx )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    return  m_pTempStrBufArray[ idx ];
  }

  //  アイコンのarcid
  u32 Work::GetIconArcId()
  {
    return ARCID_SHOP_KISEKAE_ICON_TEX_DATA;
  }

  //  ドレスアップのリソースコア取得（性別指定済み）
  poke_3d::model::DressUpModelResourceManagerCore*  Work::GetDressUpModelResManagerCore()
  {
    GFL_ASSERT( m_pCharaModelViewr != NULL );

    Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
    poke_3d::model::DressUpParam::Sex sex = (poke_3d::model::DressUpParam::Sex)pMyStatus->GetSex();

    return m_pCharaModelViewr->GetDressUpModelResourceManagerCore( sex );
  }

  //  下画面のボタンidをカテゴリーidに変換
  Savedata::Fashion::Category Work::ConvCategoryIdFromLowerItemSelectViewButtonId( App::Kisekae::View::LowerItemSelectView::ButtonId buttonId )
  {
    const App::Kisekae::View::LowerItemSelectView::ButtonId aCategoryBtnId[]  =
    {
      App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_00,
      App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_01,
      App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_02,
      App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_03,
      App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_04,
      App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_05,
      App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_06,
      App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_07,
    };

    //  カテゴリーボタンを押しているかチェック
    {
      u32 loopNum = GFL_NELEMS( aCategoryBtnId );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( aCategoryBtnId[ i ] == buttonId )
        {
          return  (Savedata::Fashion::Category)i;
        }
      }
    }

    return Savedata::Fashion::CATEGORY_MAX;
  }

  //  きせかえパラメータチェックデータ出力
  void Work::OutputChkDressUpParam( poke_3d::model::DressUpParam* pOutput, const poke_3d::model::DressUpParam& rParam, App::Kisekae::System::ItemDataBase* pItemDataBase )
  {
    *pOutput  = rParam;

    poke_3d::model::DressUpParam::ItemCategory  dressUpParamCategory  = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( (Savedata::Fashion::Category)pItemDataBase->GetCategory() );
    pOutput->itemParams[ dressUpParamCategory ]  = (s16)pItemDataBase->GetItemId();
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
