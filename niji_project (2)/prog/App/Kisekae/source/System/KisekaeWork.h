// ============================================================================
/*
 * @file KisekaeWork.h
 * @brief きせかえアプリのワーククラスです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_WORK_H_INCLUDE )
#define KISEKAE_WORK_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

//  nijiのインクルード
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "Savedata/include/FashionSave.h"

//  アプリのパラメータ
#include "App/Kisekae/include/KisekaeAppParam.h"

//  viewのインクルード
#include "App/Kisekae/source/View/KisekaeLowerItemSelectView.h"

//  リソースのインクルード
#include <arc_def_index/arc_def.h>

//  きせかえデータベース
#include "KisekaeData/include/KisekaeDataBase.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)

  // 着せ替えモデルリソースマネージャーコア
  class DressUpModelResourceManagerCore;

GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class CharaModelViewer;
  class AppCommonGrpIconData;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
  class ViewSystemAccessor;

  //  アイテムデータ
  class ItemDataBase
  {
  public:
    friend class ShopItemList;
    friend class HaveItemList;
    friend class ColorItemList;

    ItemDataBase()
    {
      _Clear();
    }

    //  no
    u16 GetNo();

    //  カテゴリー
    Savedata::Fashion::Category GetCategory();

    //  アイテム名のtextid
    u32 GetItemTxtID();

    //  色情報名のtextid
    u32 GetColorTxtID();
      
    //  色染め
    virtual void SettingColor() {}

    //  色染め色がデフォルトか
    virtual bool IsColorDefault() { return false; }

    //  既読にする
    virtual bool Alrady() { return false; }

    //  金額
    virtual u32 GetMoney();

    //  きせかえのitemid
    virtual s16 GetItemId();

    //  アイテムアイコンのテクスチャーid
    u32 GetItemIconTexId();

    //  購入する
    void SetBuy();

    //  購入したか
    bool IsBuy() { return m_bBuy; }

    //  newフラグがあるか
    virtual bool IsNew();

    //  newフラグをクリア
    void ClearNew();

  private:
    void _Clear()
    {
      m_saveCategory  = Savedata::Fashion::CATEGORY_BAG;
      m_pFashionSave  = NULL;
      m_pGoodsData    = NULL;
      m_sex           = poke_3d::model::DressUpParam::SEX_MALE;
      m_partsId       = -1;
      m_pDressupModelResourceManagerCoreDebugChk  = NULL;
      m_bBuy  = false;
    }

    const table_data_GoodsData* m_pGoodsData;

    Savedata::Fashion*  m_pFashionSave;

    poke_3d::model::DressUpParam::Sex m_sex;

    Savedata::Fashion::Category m_saveCategory;

    s16 m_partsId;

    bool  m_bBuy;

    /*
      デバッグチェックするために必要なので、それ以外では使ってはいけない
    */
    poke_3d::model::DressUpModelResourceManagerCore*  m_pDressupModelResourceManagerCoreDebugChk;
  };

  //  基本アイテムリスト
  //  各カテゴリーリストをまとまっている
  class ItemList
  {
  public:
    //  着せ替えアイテム名のmstxtのgarcId
    static gfl2::fs::ArcFile::ARCDATID GetItemTextMsTxtArcDatId();

    //  色情報名のmstxtのgarcId
    static gfl2::fs::ArcFile::ARCDATID GetColorTextMsTxtArcDatId();

    //  まず着せ替え相手の性別を指定
    ItemList( const poke_3d::model::DressUpParam::Sex sex );
    virtual ~ItemList();

    //  アクティブになっているカテゴリー値を指定
    void SetActiveCategory( const Savedata::Fashion::Category category );
    Savedata::Fashion::Category GetActiveCategory() const { return (Savedata::Fashion::Category)m_activeCategory; }

    //  アクティブカテゴリーが切り替わる一つ前のカテゴリー値を取得
    Savedata::Fashion::Category GetOldCategory() const { return (Savedata::Fashion::Category)m_oldActiveCategory; }

    //  データ取得
    ItemDataBase* GetData( const u32 listIdx );

    //  カテゴリー値とアイテムリストidxを指定
    ItemDataBase* GetData( const u32 categoryIdx, const u32 listIdx );

    //  データ数取得
    u32 GetDataMax();

    //  カテゴリー値を指定
    u32 GetDataMax( const u32 categoryIdx );

    //  リストのカーソル位置取得
    u32 GetListScroll();
    f32 GetListPos();

    //  リストのカーソル位置設定
    void SetListScroll( const u32 listScroll );
    void SetListPos( const f32 listPos );

    //  リスト再構築
    virtual void Refresh()  {};

    //  着せ替えデフォルトパラメータではずせないのを取得
    virtual const poke_3d::model::DressUpParam* GetDefaultIndispensableDressUpParam() { return NULL; }

    //  色染め用かどうか
    static bool IsColorDressup( const u32 colorId );

    //  きせかえアイテムが重複所持しているかチェック(デバッグ用)
    static bool IsMultipleHaveByDebug( const Savedata::Fashion::Category saveCategory, poke_3d::model::DressUpParam::Sex sex, const s32 chkItemId, poke_3d::model::DressUpModelResourceManagerCore* pDressupResManger );

  protected:
    struct CATEGORY_DATA
    {
      ItemDataBase*  pData[ Savedata::Fashion::FASHION_ARRAY_NUM ];

      u32   listScroll;
      f32   listPos;
      u32   num;
    };

    //  セーブするリスト最大数を確保
    CATEGORY_DATA  m_aCategoryData[ Savedata::Fashion::CATEGORY_MAX ];

    poke_3d::model::DressUpParam::Sex m_sex;
  private:
    void _Clear()
    {
      gfl2::std::MemClear( m_aCategoryData, sizeof( m_aCategoryData ) );
      m_oldActiveCategory = m_activeCategory  = 0;

      m_sex = poke_3d::model::DressUpParam::SEX_MALE;
    }

    u32 m_activeCategory;
    u32 m_oldActiveCategory;
  };

  //  ショップリスト
  class ShopItemList : public ItemList
  {
  public:
    ShopItemList( const app::util::Kisekae::KisekaeShopID shopId, const poke_3d::model::DressUpParam::Sex sex, poke_3d::model::DressUpModelResourceManagerCore* pDressupResManger );

  private:
    class ItemData : public ItemDataBase
    {
    public:      
      //  newフラグがあるか
      virtual bool IsNew() { return false; }
    };

    //  色染め用の着せ替えを持っているか
    bool _IsHaveColorDressup( const Savedata::Fashion::Category category, const ItemData& rItemData, poke_3d::model::DressUpModelResourceManagerCore* pDressupResManger );

    //  カテゴリ数 * 最大アイテム項目数
    ItemData  m_aData[ Savedata::Fashion::FASHION_ARRAY_NUM * Savedata::Fashion::CATEGORY_MAX ];
  };

  //  所持リスト
  class HaveItemList : public ItemList
  {
  public:
    HaveItemList( const poke_3d::model::DressUpParam::Sex sex );

    //  着せ替えデフォルトパラメータではずせないのを取得
    virtual const poke_3d::model::DressUpParam* GetDefaultIndispensableDressUpParam() { return &m_defaultIndispensableDressUpParam; }

  private:
    class ItemData : public ItemDataBase
    {
    public:      
      //  既読にする
      virtual bool Alrady();

      //  パーツid
      virtual s16 GetItemId();
    };

    //  カテゴリ数 * 最大アイテム項目数
    ItemData  m_aData[ Savedata::Fashion::FASHION_ARRAY_NUM * Savedata::Fashion::CATEGORY_MAX ];

    //  きせかえデフォルトではずせない
    poke_3d::model::DressUpParam  m_defaultIndispensableDressUpParam;
  };

  //  色リスト
  class ColorItemList: public ItemList
  {
  public:
    //  指定した色のみのデータリスト作成
    ColorItemList( 
      const poke_3d::model::DressUpParam::Sex sex,
      const u32 shopId,
      const s32 colorId,
      const bool bUseKinomi,
      poke_3d::model::DressUpModelResourceManagerCore* pDressUpResource );

    //  ショップの全色指定したデータリスト作成
    ColorItemList( 
      const poke_3d::model::DressUpParam::Sex sex,
      const u32 shopId,
      const bool bUseKinomi,
      poke_3d::model::DressUpModelResourceManagerCore* pDressUpResource );

    //  リスト再構築
    virtual void Refresh();

  private:
    enum
    {
      ERROR_CODE_ITEM_ID  = 0xffffffff,
    };

    struct CREATE_DATA
    {
      s32 colorId;
      u32 useCoin;
    };

    //  データ生成
    void _CreateData(
      const poke_3d::model::DressUpParam::Sex sex,
      const CREATE_DATA* pCrateDataArray,
      const u32  createDataArrayNum,
      const bool bUseKinomi,
      poke_3d::model::DressUpModelResourceManagerCore* pDressUpResource );

    //  色選択対象か
    bool _IsSelectColor( const table_data_DyeingData* pShopData, const bool bUseKinomi );

    class ItemData : public ItemDataBase
    {
    public:
      ItemData();

      //  newフラグがあるか
      virtual bool IsNew() { return false; }

      //  金額
      virtual u32 GetMoney();

      //  色染め
      virtual void SettingColor();

      //  色染め色がデフォルトか
      virtual bool IsColorDefault();

      u32       m_money;
      u32       m_colorSaveDataNo;
      s32       m_colorId;
      ItemList* m_pItemList;

      poke_3d::model::DressUpModelResourceManagerCore* m_pDressUpResource;

    private:
      void _Clear()
      {
        m_money = 0;
        m_colorSaveDataNo = 0;
        m_colorId = 0;
        m_pItemList = NULL;
        m_pDressUpResource  = NULL;
      }

    };

    void _Clear()
    {
      m_freeDataListIdx   = 0;
      gfl2::std::MemClear( m_aOrgCategoryData, sizeof( m_aOrgCategoryData ) );
    }

    //  選択リストに追加
    void _Add( 
    const Savedata::Fashion::Category saveCategoryIdx, const u32 colorSaveDataNo, 
    const table_data_GoodsData* pColorGoodsData, const CREATE_DATA& createData, poke_3d::model::DressUpModelResourceManagerCore* pDressupResManger );

    //  性別/カラーid/きせかえsaveid/きせかえデータからきせかえアイテムidを取得
    s32 _GetColorItemId( 
    poke_3d::model::DressUpModelResourceManagerCore* pDressUpResource,
    const u32 colorId,
    const Savedata::Fashion::Category saveCategory,
    const table_data_GoodsData* pGoodsData );

    //  カテゴリ数 * 最大アイテム項目数
    ItemData  m_aData[ Savedata::Fashion::FASHION_ARRAY_NUM * Savedata::Fashion::CATEGORY_MAX ];

    //  派生先で保管するアイテムリスト(リストが増減するので、増減前の元のデータを保持)
    CATEGORY_DATA  m_aOrgCategoryData[ Savedata::Fashion::CATEGORY_MAX ];

    u32 m_freeDataListIdx;
  };

  class Work : public NetAppLib::System::ApplicationWorkBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( Work );

    //  system/heapのメモリサイズ指定
    static const int  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
    static const int  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 30;   // 10MB

  public:
    Work( APP_PARAM* pAppParam );
    virtual~Work();

    //  このクラスが使えるか
    bool IsReady();

    //  リスト作成
    void CreateItemList();

    APP_PARAM*  GetAppParam() { return m_pAppParam; }

    //  共通アイコングラフィックデータ
    app::tool::AppCommonGrpIconData*  GetCommonGrpIconData() const { return m_pCommonGrpIcon; }

    //  アイテムリスト取得
    ItemList* GetItemList() const { return m_pAcitveItemList; }

    //  Viewシステムの参照の設定・取得
    void SetViewSystemAccessor( ViewSystemAccessor* pAccessor );
    ViewSystemAccessor* GetViewSystemAccessor() { return m_pViewSystemAccessor; }

    //  モデルビューア参照の設定・取得
    void SetModelViewer( app::tool::CharaModelViewer* pAddr );
    app::tool::CharaModelViewer* GetCharaModelViewer() const { return m_pCharaModelViewr; }

    //  着せ替えデータ出力
    void OutputDressupParam( poke_3d::model::DressUpParam* pOutputParam, const poke_3d::model::DressUpParam& rNewParam, poke_3d::model::DressUpParam rOldParam );

    //  作業用の文字列取得
    gfl2::str::StrBuf*  GetTmpStrBuf( const u32 idx );

    //  アイコンのarcid
    u32 GetIconArcId();

    //  アイコンのサイズ
    u32 GetIconSizeWidth() const { return 40; }
    u32 GetIconSizeHeight() const { return 40; }

    //  アイコンファイルが圧縮しているか
    bool IsIconFileComp() const { return true; }

    //  下画面のボタンidをカテゴリーidに変換
    Savedata::Fashion::Category ConvCategoryIdFromLowerItemSelectViewButtonId( App::Kisekae::View::LowerItemSelectView::ButtonId buttonId );

    //  モデル回転のデフォルトベクトル
    const gfl2::math::Vector3& GetModelRotDefault() const { return m_modelRotVec; }

    //  ドレスアップのリソースコア取得（MyStatsuの性別のデータを返す）
    poke_3d::model::DressUpModelResourceManagerCore*  GetDressUpModelResManagerCore();

    //  きせかえをしたかのフラグを立てる・フラグを取得
    void OnDressup() { m_bDressup = true; }
    bool IsDressup() const { return m_bDressup; }

    //  きせかえパラメータチェックデータ出力
    void OutputChkDressUpParam( poke_3d::model::DressUpParam* pOutput, const poke_3d::model::DressUpParam& rParam, App::Kisekae::System::ItemDataBase* pItemDataBase );

  private:
    void _Clear()
    {
      m_pAppParam           = NULL;
      m_pViewSystemAccessor = NULL;
      m_pAcitveItemList     = NULL;
      m_pCharaModelViewr    = NULL;
      m_pCommonGrpIcon      = NULL;
      m_bDressup    = false;

      m_modelRotVec = gfl2::math::Vector3( 0.0f, -0.01f, 0.0f );

      gfl2::std::MemClear( m_pTempStrBufArray, sizeof( m_pTempStrBufArray ) );
    }

    APP_PARAM*          m_pAppParam;
    ViewSystemAccessor* m_pViewSystemAccessor;
    app::tool::CharaModelViewer*      m_pCharaModelViewr;
    app::tool::AppCommonGrpIconData*  m_pCommonGrpIcon;

    ItemList*             m_pAcitveItemList;

    bool                m_bDressup;
    gfl2::str::StrBuf*  m_pTempStrBufArray[ 2 ];

    gfl2::math::Vector3 m_modelRotVec;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // NETAPPSKELETONWORK_H_INCLUDE
