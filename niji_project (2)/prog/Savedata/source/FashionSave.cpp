
//============================================================================================
/**
 * @file		FashionSave.cpp
 * @brief		きせかえセーブデータ
 * @author	uchida_yuto
 * @date		15.10.09
 *
 * namespace	savedata::
 */
//============================================================================================
#include "Savedata/include/FashionSave.h"

#include "Savedata/include/MyStatus.h"
#include "Savedata/include/GameTime.h"

#include "System/include/DressUp.h"

#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN(Savedata)

  /************************************************************************/
  /*  ライブラリのアイテムカテゴリーとセーブのカテゴリの対応表            */
  /************************************************************************/
  const s32 Fashion::CATEGORY_CONV_TABLE[ Fashion::CATEGORY_MAX + 1 ][ 2 ] =
  {
    { Fashion::CATEGORY_HAT,     poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT           },                // 帽子
    { Fashion::CATEGORY_ACCHAIR, poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR       },                //  かみかざり
    { Fashion::CATEGORY_ACCEYE,  poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE        },                //  めがね
    { Fashion::CATEGORY_TOPS,    poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS          },                // トップス
    { Fashion::CATEGORY_BOTTOMS, poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS       },                // ボトムス
    { Fashion::CATEGORY_LEGS,    poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS          },                // レッグ
    { Fashion::CATEGORY_SHOES,   poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES         },                // シューズ
    { Fashion::CATEGORY_BAG,     poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG           },                // バッグ
    { -1, -1 }
  };

  //  SavedateのカテゴリーからDressUpParamのカテゴリーに変換
  poke_3d::model::DressUpParam::ItemCategory Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( const Category category )
  {
    //static const poke_3d::model::DressUpParam::ItemCategory aConvTable[ CATEGORY_MAX ] =

    //  検索して見つかったら返す
    for( u32 i = 0; CATEGORY_CONV_TABLE[ i ][ 0 ] != -1; ++i )
    {
      if( CATEGORY_CONV_TABLE[ i ][ 0 ] == category )
      {
        return  (poke_3d::model::DressUpParam::ItemCategory)CATEGORY_CONV_TABLE[ i ][ 1 ];
      }
    }

    GFL_ASSERT( 0 );
    return  poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT;
  }

  //  DressUpParamのカテゴリーからSavedateのカテゴリーに変換
  Fashion::Category Fashion::ConvSavedataCategoryFromDressUpParamItemCategory( const poke_3d::model::DressUpParam::ItemCategory category )
  {
    //  検索して見つかったら返す
    for( u32 i = 0; CATEGORY_CONV_TABLE[ i ][ 1 ] != -1; ++i )
    {
      if( CATEGORY_CONV_TABLE[ i ][ 1 ] == category )
      {
        return  (Fashion::Category)CATEGORY_CONV_TABLE[ i ][ 0 ];
      }
    }

    //  ここにくるとエラー
    GFL_ASSERT( 0 );
    return  Fashion::CATEGORY_ACCEYE;
  }

  //  きせかえパラメータが変更しているか
  bool Fashion::IsChangeDressUpParam( const poke_3d::model::DressUpParam& rChkParam, const poke_3d::model::DressUpParam& rOrgParam )
  {
    for( u32 i = 0; i < Savedata::Fashion::CATEGORY_MAX; ++i )
    {
      poke_3d::model::DressUpParam::ItemCategory  category  = Savedata::Fashion::ConvDressUpParamItemCategoryFromtSavedataCategory( (Savedata::Fashion::Category)i );
      if( rOrgParam.itemParams[ category ] != rChkParam.itemParams[ category ] )
      {
        return  true;
      }
    }

    return  false;
  }

  // con
  Fashion::Fashion()
  {
    // 着せ替えアイテムの種類の最大数をチェック
  
    this->Clear(NULL);
    STATIC_ASSERT( sizeof( FashionData ) == 6664 );
  }

  //des
  Fashion::~Fashion()
  {
  }

  //-----------------------------------------------------------------------------
  /**
  * @brief セーブデータのクリア
  * @param heap　クリアに使用するテンポラリヒープ
  */
  //-----------------------------------------------------------------------------
  void Fashion::Clear( gfl2::heap::HeapBase * heap )
  {
    gfl2::std::MemClear(&m_data, sizeof(FashionData));
  }

  void Fashion::InitializeDefaultFashionItem(bool is_female)
  {
    // 初期の服を追加
    poke_3d::model::DressUpParam defaultDressUpParamMale;
    poke_3d::model::DressUpParam defaultDressUpParamFemale;
    if( is_female == false )
    {
      defaultDressUpParamMale.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ]   = poke_3d::model::DressUpParam::SEX_MALE;
      defaultDressUpParamMale.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS ]  = DRESSUP_ITEM_ID_P1_TOPS_TSHIRT_PTSHIRT01;
      
    }
    else
    {
      defaultDressUpParamFemale.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ]   = poke_3d::model::DressUpParam::SEX_FEMALE;
      defaultDressUpParamFemale.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS ]  = DRESSUP_ITEM_ID_P2_TOPS_UTANK_PUTANK03;
    }

    Savedata::MyStatus* pStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

    //  デフォルト所持のきせかえ一覧
    poke_3d::model::DressUpParam  aFirstHaveDressUpParam[]  =
    {
      pStatus->GetDressup(),
      defaultDressUpParamMale,
      defaultDressUpParamFemale,
    };

    u32 haveDataMax = GFL_NELEMS( aFirstHaveDressUpParam );

    //  デフォルトパラメータから初期所持リストを追加
    {
      for( u32 i = 0; i < CATEGORY_MAX; ++i )
      {
        poke_3d::model::DressUpParam::ItemCategory dressUpItemCategory  = ConvDressUpParamItemCategoryFromtSavedataCategory( (Category)i );

        for( u32 have_data_idx = 0; have_data_idx < haveDataMax; ++have_data_idx )
        {
          poke_3d::model::DressUpParam* pDressUpParam = &aFirstHaveDressUpParam[ have_data_idx ];
          if( pDressUpParam->bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ] == -1 )
          {
            continue;
          }

          if( is_female == false )
          {
            //  性別が異なるなら設定をスキップ
            if( pDressUpParam->bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ] == poke_3d::model::DressUpParam::SEX_FEMALE )
            {
              continue;
            }
          }
          else
          {
            //  性別が異なるなら設定をスキップ
            if( pDressUpParam->bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ] == poke_3d::model::DressUpParam::SEX_MALE )
            {
              continue;
            }
          }

          if( pDressUpParam->itemParams[ dressUpItemCategory ] != -1 )
          {
            Add( i, pDressUpParam->itemParams[ dressUpItemCategory ] );
          }
        }
      }
    }
  }

  bool Fashion::IsOwned(u32 type, int id)
  {
    ItemData* pItemData = _GetItmeDataSearchId( type, id );
    if( pItemData != NULL )
    {
      return pItemData->bHave;
    }

    return  false;
  }

  bool Fashion::IsNew(u32 type, int id)
  {
    ItemData* pItemData = _GetItmeDataSearchId( type, id );
    if( pItemData != NULL )
    {
      return pItemData->bNew;
    }

    return  false;
  }

  bool Fashion::Add( u32 type, int id)
  {
    if( CATEGORY_MAX <= type )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    ItemData* pItemData = _GetItmeDataSearchId( type, id );
    if( pItemData == NULL )
    {
      //  type, idから指定したデータが存在しない
      GFL_ASSERT( 0 );
      return  false;
    }

    pItemData->bHave = 1;
    //  新しく追加したのでNew
    pItemData->bNew  = 1;

    return  true;
  }

  //! 衣類の追加を削除
  void Fashion::Del(u32 type, int id )
  {
    if( CATEGORY_MAX <= type )
    {
      GFL_ASSERT( 0 );
      return;
    }

    //  すでに追加されているかチェック
    ItemData* pItemData = _GetItmeDataSearchId( type, id );
    {
      if( pItemData == NULL )
      {
        return;
      }
    }

    pItemData->bHave = 0;
    pItemData->bNew  = 0;
  }

  void Fashion::ClearNewFlag( u32 type, int id)
  {
    ItemData* pItemData = _GetItmeDataSearchId( type, id );
    if( pItemData != NULL )
    {
      pItemData->bNew = false;
    }
  }

  //! 所持着せ替えアイテムの総数を取得
  u32 Fashion::GetHaveItemTotalNum()
  {
    u32 totalNum = 0;
    for( u32 categoryType=0; categoryType<CATEGORY_MAX; categoryType++ )
    {

      ItemData* pItemData = GetCagoryItmeData( (Category)categoryType );
      const u32 dataNum = GetCagoryItmeDataNum( (Category)categoryType );

      // そのカテゴリのアイテム数分
      for( u32 itemIndex=0; itemIndex<dataNum; itemIndex++ )
      {
        // @fix GFNMCat[3530]素足は除く
        if( (Category)categoryType == CATEGORY_LEGS && itemIndex == 0 )
        {
          continue;
        }

        if( pItemData[ itemIndex ].bHave == true )
        {
          totalNum++;
        }
      }
    }

    return totalNum;
  }


  //  カテゴリーの配列アドレスを取得
  Fashion::ItemData* Fashion::GetCagoryItmeData( const Category type )
  {
    CategoryItemData* pCategoryItemData = _GetCategoryItmeData( type );
    if( pCategoryItemData != NULL )
    {
      return pCategoryItemData->pItemData;
    }

    GFL_ASSERT( 0 );
    return NULL;
  }

  //  カテゴリーの配列の個数取得
  u32 Fashion::GetCagoryItmeDataNum( const Category type )
  {
    CategoryItemData* pCategoryItemData = _GetCategoryItmeData( type );
    if( pCategoryItemData != NULL )
    {
      return pCategoryItemData->dataNum;
    }

    GFL_ASSERT( 0 );
    return 0;
  }

  Fashion::CategoryItemData* Fashion::_GetCategoryItmeData( u32 type )
  {
    static CategoryItemData saItemData[ CATEGORY_MAX ] =
    {
      { CATEGORY_TOPS,    m_data.aTopsData,     GFL_NELEMS( m_data.aTopsData ) },
      { CATEGORY_BOTTOMS, m_data.aBottomsData,  GFL_NELEMS( m_data.aBottomsData ) },
      { CATEGORY_LEGS,    m_data.aLegsData,     GFL_NELEMS( m_data.aLegsData ) },
      { CATEGORY_SHOES,   m_data.aShoesData,    GFL_NELEMS( m_data.aShoesData ) },
      { CATEGORY_BAG,     m_data.aBagData,      GFL_NELEMS( m_data.aBagData ) },
      { CATEGORY_HAT,     m_data.aHatData,      GFL_NELEMS( m_data.aHatData ) },
      { CATEGORY_ACCEYE,  m_data.aAcceyeData,   GFL_NELEMS( m_data.aAcceyeData ) },
      { CATEGORY_ACCHAIR, m_data.aAcchairData,  GFL_NELEMS( m_data.aAcchairData ) },
    };

    u32 loopNum = GFL_NELEMS( saItemData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( saItemData[ i ].category == (Category)type )
      {
        return &saItemData[ type ];
      }
    }

    GFL_ASSERT( 0 );
    return  NULL;
  }

  Fashion::ItemData* Fashion::_GetItmeDataSearchId( u32 type, int id )
  {
    if( CATEGORY_MAX <= type )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    ItemData* pItemData = GetCagoryItmeData( (Category)type );
    s32 dataNum = (s32)GetCagoryItmeDataNum( (Category)type );
    if( dataNum <= id )
    {
      return  NULL;
    }

    return  &pItemData[ id ];
  }

//-----------------------------------------------------------------------------

GFL_NAMESPACE_END(savedata)


