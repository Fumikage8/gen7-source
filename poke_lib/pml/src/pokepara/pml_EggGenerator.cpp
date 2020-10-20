//=============================================================================
/**
 * @brief  ポケモンのタマゴ生成
 * @file   pml_EggGenerator.cpp
 * @author obata
 * @date   2012.04.12
 */
//=============================================================================
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include "pml/include/pml_Item.h"
#include "pml/include/system/pml_Library.h"
#include "pml/include/personal/pml_PersonalSystem.h"
#include "pml/include/pokepara/pml_PokemonParamSpec.h"
#include "pml/include/pokepara/pml_PokemonParam.h"
#include "pml/include/pokepara/pml_PokemonCoreParam.h"
#include "pml/include/pokepara/pml_EggGenerator.h"
#include "pml/include/pokepara/pml_EggWazaData.h"
#include "pml/include/personal/pml_PersonalUtil.h"
#include "./pml_PokemonParamCalcTool.h"
#include <niji_conv_header/poke_lib/monsno_def.h>

namespace pml {
  namespace pokepara {


    //-------------------------------------------------------------------------
    /**
     * @brief 準備
     * @param heap 作業領域として使用するヒープ
     */
    //-------------------------------------------------------------------------
    void EggGenerator::Initialize( gfl2::heap::HeapBase* heap )
    {

      gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
      arcReq.fileName = pml::Library::m_setting->garcPath_KowazaTable;
      arcReq.heapForFile = heap;
      arcReq.heapForReq = heap->GetLowerHandle();
      /*
        2016/08/25  Yu Muto
        GF_DEVELOP_HIO_RESOURCE!=0の場合は、
        全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
      */
#if GF_DEVELOP_HIO_RESOURCE
#else
      arcReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
      pml::Library::m_pmlFileManager->SyncArcFileOpen(arcReq);

    }

    //-------------------------------------------------------------------------
    /**
     * @brief 後片付け
     */
    //-------------------------------------------------------------------------
    void EggGenerator::Finalize( void )
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.fileName = pml::Library::m_setting->garcPath_KowazaTable;
      pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );
    }



    // 両親ポケモン
    struct Parents
    {
      const CoreParam* father;
      const CoreParam* mother;
    };


    // タマゴのパラメータ
    class EggParam 
    {
    public:
      InitialSpec spec;
      u32 wazaCount;
      WazaNo wazano[ MAX_WAZA_NUM ];
      u32 ballID;

      bool isTalentDerivedFromFather[ POWER_NUM ];
      bool isTalentDerivedFromMother[ POWER_NUM ];
      u32 derivedTalentCount;      // 継承することが決定した個体値の数
      u32 totalDeriveTalentCount;  // 両親から継承する個体値の数

      EggParam( void ) :
        spec(),
        wazaCount(0),
        ballID(0),
        derivedTalentCount(0),
        totalDeriveTalentCount(0)
      {
        for( u32 i=0; i<MAX_WAZA_NUM; i++ ) {
          wazano[i] = WAZANO_NULL;
        }
        for( u32 i=0; i<POWER_NUM; i++ ) {
          isTalentDerivedFromFather[i] = false;
          isTalentDerivedFromMother[i] = false;
        }
      }
    };


    //---------------------------------------------------------------------
    /**
     * @brief タマゴを作成する
     * @param heap      タマゴ生成に使用するヒープ
     * @param poke1     親ポケモン1
     * @param poke2     親ポケモン2
     * @param setting   設定
     */
    //---------------------------------------------------------------------
    PokemonParam* EggGenerator::CreateEgg( 
        gfl2::heap::HeapBase* heap,
        const CoreParam* poke1, 
        const CoreParam* poke2, 
        const Setting& setting )
    {
      CoreParam* egg_core = CreateEgg_CoreParam( heap->GetLowerHandle(), poke1, poke2, setting ); 
      PokemonParam* egg = GFL_NEW( heap ) PokemonParam( heap, *egg_core ); 
      GFL_SAFE_DELETE( egg_core );
      return egg;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief タマゴを作成する
     * @param heap      タマゴ生成に使用するヒープ
     * @param poke1     親ポケモン1
     * @param poke2     親ポケモン2
     * @param setting   設定
     */
    //-------------------------------------------------------------------------
    CoreParam* EggGenerator::CreateEgg_CoreParam( 
        gfl2::heap::HeapBase* heap,
        const CoreParam* poke1, 
        const CoreParam* poke2, 
        const Setting& setting )
    {

      bool poke1_fast = poke1->StartFastMode();
      bool poke2_fast = poke2->StartFastMode();

      // 父母を決定
      Parents parents;
      DecideParents( &parents, poke1, poke2 );

      // タマゴのパラメータを決定
      EggParam egg_param;
      DecideEggParams( heap, &egg_param, parents, setting ); 

      poke1->EndFastMode( poke1_fast );
      poke2->EndFastMode( poke2_fast );

      // タマゴを作成
      CoreParam* egg = GFL_NEW( heap ) CoreParam( heap, egg_param.spec ); 
      SetupEggParam( heap, egg, egg_param, parents, setting );
      return egg;
    }

    /**
     * @brief 父母の役割分担を決定する
     * @param[out] parents 決定した役割の格納先
     * @param[in]  poke1   親ポケモン1
     * @param[in]  poke2   親ポケモン2
     */
    void EggGenerator::DecideParents( 
        Parents* parents,
        const CoreParam* poke1,
        const CoreParam* poke2 )
    {
      parents->father = poke1;
      parents->mother = poke2;
      CorrectParents_SEX( parents );
      CorrectParents_METAMON( parents );
    }

    /**
     * @brief 父母の組み合わせを訂正する ( 性別 )
     * @param[in,out] parents 両親ポケモン
     */
    void EggGenerator::CorrectParents_SEX( Parents* parents )
    {
      Sex father_sex;
      Sex mother_sex;

      // 父親チェック
      father_sex = parents->father->GetSex();
      mother_sex = parents->mother->GetSex();
      if( father_sex == SEX_FEMALE ) {
        ReverseParents( parents );
      }

      // 母親チェック
      father_sex = parents->father->GetSex();
      mother_sex = parents->mother->GetSex();
      if( mother_sex == SEX_MALE ) {
        ReverseParents( parents );
      }

#if PML_DEBUG //製品版では発生しないはず
      if( ( ( father_sex == SEX_MALE ) && ( mother_sex == SEX_MALE ) ) ||
          ( ( father_sex == SEX_FEMALE ) && ( mother_sex == SEX_FEMALE ) ) ) 
      {
        GFL_ASSERT_MSG( 0, "WARNING!! 父ポケモン・母ポケモンが同性です\n" );  //@check プログラム検証用
      }
#endif
    }

    /**
     * @brief 父母の組み合わせを訂正する ( メタモン )
     * @param[in,out] parents 両親ポケモン
     *
     * メタモン × ♂ ==> 父:♂       母:メタモン
     * メタモン × ♀ ==> 父:メタモン 母:♀
     * メタモン × ？ ==> 父:？       母:メタモン
     */
    void EggGenerator::CorrectParents_METAMON( Parents* parents )
    {
      MonsNo father_monsno;
      Sex mother_sex;

      father_monsno = parents->father->GetMonsNo();
      mother_sex = parents->mother->GetSex();

      if( ( father_monsno == MONSNO_METAMON ) && 
          ( mother_sex == SEX_UNKNOWN ) ) 
      {
        ReverseParents( parents );
      }
    }

    /**
     * @brief 父母を入れ替える
     * @param[in,out] parents 両親ポケモン
     */
    void EggGenerator::ReverseParents( Parents* parents )
    {
      const CoreParam* temp = parents->father;
      parents->father = parents->mother;
      parents->mother = temp;
    }

    /**
     * @brief タマゴのパラメータを決定する
     * @param[in]  heap      作業用に使用するヒープ
     * @param[out] egg_param 決定したパラメータの格納先
     * @param[in]  parents   両親ポケモン
     * @param[in]  setting   設定
     */
    void EggGenerator::DecideEggParams( 
        gfl2::heap::HeapBase* heap,
        EggParam* egg_param,
        const Parents& parents,
        const Setting& setting )
    {
      // ID・レベル
      egg_param->spec.id = setting.IDNo; 
      egg_param->spec.level = 1;

      // モンスターナンバー
      DecideEggParam_MonsNo_BASIC( egg_param, parents ); 
      DecideEggParam_MonsNo_NIDORAN( egg_param, parents, setting.pRandom );
      DecideEggParam_MonsNo_BARUBIITO( egg_param, parents, setting.pRandom );
      DecideEggParam_MonsNo_FIONE( egg_param, parents );
      DecideEggParam_MonsNo_BABY( egg_param, parents ); 

      // フォルム
      DecideEggParam_Form_BASIC( egg_param, parents );
      DecideEggParam_Form_MOTHER( egg_param, parents ); 
      DecideEggParam_Form_REGION( egg_param, parents, setting );
      DecideEggParam_Form_PERSONAL_REGION( heap, egg_param, parents );

      // 性別
      DecideEggParam_Sex( egg_param, egg_param->spec.monsno, egg_param->spec.formno, setting.pRandom );

      // 性格
      DecideEggParam_Seikaku_BASIC( egg_param, parents, setting.pRandom );
      DecideEggParam_Seikaku_KAWARAZUNOISI( egg_param, parents, setting.pRandom ); 

      // 特性
      DecideEggParam_TokuseiIndex( egg_param, parents, setting.pRandom ); 

      // なつき度 ( 孵化カウント )
      DecideEggParam_BirthCount( egg_param );

      // 個体値
      DecideDerivedTalentPower_AKAIITO( egg_param, parents );
      DecideDerivedTalentPower_ITEM( egg_param, parents, setting.pRandom );
      DecideDerivedTalentPower_RANDOM( egg_param, parents, setting.pRandom );
      DecideEggParam_TalentPower_RANDOM( egg_param, parents, setting.pRandom );
      DecideEggParam_TalentPower_DERIVE( egg_param, parents ); 

      // 個性乱数
      DecideEggParam_PersonalRnd( egg_param, parents, setting.pRandom );

      // レア乱数
      DecideEggParam_RareRnd( egg_param, parents, setting.haveItem_HIKARUOMAMORI, setting.pRandom ); 

      // わざ
      DecideEggParam_Waza_DEFAULT( egg_param );
      DecideEggParam_Waza_PARENTS( egg_param, parents );
      DecideEggParam_Waza_EGG( heap, egg_param, parents );
      //DecideEggParam_Waza_MACHINE( egg_param, parents ); //kujiraでは技マシンは何回でも使えるので継承しないことになった 130315
      DecideEggParam_Waza_PITYUU( egg_param, parents );

      // ボールID
      DecideEggParam_BallID( egg_param, parents, setting.pRandom );
    }

    /**
     * @brief タマゴのモンスターナンバーを決定する( 基本ルール )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 基本ルール
     * タマゴは母の種ポケモンになる。
     * ただし、母がメタモンの場合、父の種ポケモンになる。
     */
    void EggGenerator::DecideEggParam_MonsNo_BASIC( EggParam* egg, const Parents& parents )
    {
      const CoreParam* base_poke = GetBasePokemon_forMonsNo( parents );
      MonsNo base_monsno = base_poke->GetMonsNo();
      egg->spec.monsno = personal::GetSeedMonsNo( base_monsno );
    }

    /**
     * @brief 子ポケモンのモンスターNo.を決定するためのベースとなる親ポケモンを取得する
     * @param parents 両親ポケモン
     */
    const CoreParam* EggGenerator::GetBasePokemon_forMonsNo( const Parents& parents )
    {
      MonsNo mother_monsno = parents.mother->GetMonsNo();

      if( mother_monsno == MONSNO_METAMON ) 
      {
        // 母親がメタモン ==> 父親
        return parents.father;
      }
      else 
      {
        // 通常 ==> 母親
        return parents.mother;
      }
    }

    /**
     * @brief タマゴのモンスターナンバーを決定する( 例外ケース：ニドラン )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * ニドラン♀
     * ニドラン♂
     * ニドリーノ
     * ニドキング　からは, ニドラン♂ ( 50% ) or ニドラン♀ ( 50% ) が産まれる
     *
     * ニドリーナ
     * ニドクイン　はタマゴグループが『無生殖』なので, タマゴは発見されない。
     */
    void EggGenerator::DecideEggParam_MonsNo_NIDORAN( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      const CoreParam* base_poke = GetBasePokemon_forMonsNo( parents );
      MonsNo base_monsno = base_poke->GetMonsNo();

      if( ( base_monsno == MONSNO_NIDORAN_F ) ||
          ( base_monsno == MONSNO_NIDORAN_M ) ||
          ( base_monsno == MONSNO_NIDORIINO ) ||
          ( base_monsno == MONSNO_NIDOKINGU ) ) 
      {
        bool male = ( pRandom->Next(2) == 0 );
        egg->spec.monsno = ( male ) ?  ( MONSNO_NIDORAN_M ) : ( MONSNO_NIDORAN_F );
      }
    }

    /**
     * @brief タマゴのモンスターナンバーを決定する ( 例外ケース：バルビート, イルミーゼ )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * バルビート
     * イルミーゼ からは, バルビート ( 50% ) or イルミーゼ ( 50% ) が産まれる。
     */
    void EggGenerator::DecideEggParam_MonsNo_BARUBIITO( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      const CoreParam* base_poke = GetBasePokemon_forMonsNo( parents );
      MonsNo base_monsno = base_poke->GetMonsNo();

      if( ( base_monsno == MONSNO_BARUBIITO ) ||
          ( base_monsno == MONSNO_IRUMIIZE ) ) 
      {
        bool male = ( pRandom->Next(2) == 0 ); 
        egg->spec.monsno = ( male ) ? ( MONSNO_BARUBIITO ) : ( MONSNO_IRUMIIZE );
      }
    }

    /**
     * @brief タマゴのモンスターナンバーを決定する ( 例外ケース：フィオネ )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * マナフィの子はフィオネ。
     */
    void EggGenerator::DecideEggParam_MonsNo_FIONE( EggParam* egg, const Parents& parents )
    {
      const CoreParam* base_poke = GetBasePokemon_forMonsNo( parents );
      MonsNo base_monsno = base_poke->GetMonsNo(); 
      if( base_monsno == MONSNO_MANAFI ) {
        egg->spec.monsno = MONSNO_FIONE;
      }
    }

    /**
     * @brief タマゴのモンスターナンバーを決定する ( 例外ケース：べビィポケモン )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * べビィポケモンは、親が特定の道具を持っている場合にのみ生まれる。
     */
    void EggGenerator::DecideEggParam_MonsNo_BABY( EggParam* egg, const Parents& parents )
    {
      u32 father_item;
      u32 mother_item;

      const u32 TABLE_SIZE = 12;
      const u32 BABY_TABLE[ TABLE_SIZE ][3] = // 例外テーブル
      {
        // べビィポケモン  // 必須アイテム      // 子ポケモン
        {MONSNO_RURIRI,    ITEM_USIONOOKOU,     MONSNO_MARIRU},
        {MONSNO_SOONANO,   ITEM_NONKINOOKOU,    MONSNO_SOONANSU},
        {MONSNO_MANENE,    ITEM_AYASIIOKOU,     MONSNO_BARIYAADO},
        {MONSNO_USOHATI,   ITEM_GANSEKIOKOU,    MONSNO_USOKKII},
        {MONSNO_GONBE,     ITEM_MANPUKUOKOU,    MONSNO_KABIGON},
        {MONSNO_TAMANTA,   ITEM_SAZANAMINOOKOU, MONSNO_MANTAIN},
        {MONSNO_SUBOMII,   ITEM_OHANANOOKOU,    MONSNO_ROZERIA},
        {MONSNO_PINPUKU,   ITEM_KOUUNNOOKOU,    MONSNO_RAKKII},
        {MONSNO_RIISYAN,   ITEM_KIYOMENOOKOU,   MONSNO_TIRIIN},
      };

      father_item = parents.father->GetItem();
      mother_item = parents.mother->GetItem();

      // べビィポケモンの条件を満たさない場合は, 通常の子ポケモンになる
      for( u32 i=0; i<TABLE_SIZE; i++ )
      {
        if( ( egg->spec.monsno == static_cast<MonsNo>( BABY_TABLE[i][0] ) ) &&
            ( father_item != BABY_TABLE[i][1] ) &&
            ( mother_item != BABY_TABLE[i][1] ) ) 
        {
          egg->spec.monsno = static_cast<MonsNo>( BABY_TABLE[i][2] );
          break;
        }
      }
    }

    /**
     * @brief タマゴのフォルムを決定する ( 基本ルール )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 基本はフォルムNo.0
     */
    void EggGenerator::DecideEggParam_Form_BASIC( EggParam* egg, const Parents& parents )
    {
      egg->spec.formno = 0;
    }

    /**
     * @brief タマゴのフォルムを決定する ( 例外ケース：子がコフキムシ==リアル地域ポケモン )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * data_BaseMonsNoTblに登録されたポケモンがフォルム継承元親の場合,
     * 生まれてくる子は継承元の姿を継承する BTS1936
     * このテーブルに属さないものはフォルム0で生まれてくる(コフキムシは特別ルール)
     *
     * 基本は母からの継承だが、メタモンが母の場合のみ父を継承元とする
     *
     * 例：
     * 母親が『カラナクシ』or『トリトドン』なら, 母のフォルムを継承
     * 母親が『ミノムッチ』or『ミノマダム』なら, 母のフォルムを継承
     * 母親が『メタモン』and 父親が『バスラオ』なら, 父であるバスラオのフォルムを継承
     *
     * momiji追加案件：たそがれルガルガン及び、たそがれイワンコの子供はたそがれフォルムを引き継ぐ
     * ルガルガンのフォルムは0～2 0=昼の姿　1=夜の姿　2=たそがれの姿
     * イワンコのフォルムは0=通常（昼夜区別なし）1=たそがれの姿
     * イワンコが親の場合は親のフォルムを引き継ぐことで問題なし（たそがれイワンコからはたそがれイワンコが生まれ、通常イワンコからは通常イワンコが生まれる）
     * ルガルガンが親の場合は、たそがれフォルムのときのみ、たそがれイワンコが生まれるように特殊処理が必要（たそがれルガルガンのフォルムは２、たそがれイワンコのフォルムは1）
     */
    void EggGenerator::DecideEggParam_Form_MOTHER( EggParam* egg, const Parents& parents )
    {
      //下記のポケモンがフォルム継承親の場合、子は継承元のフォルムを引き継ぐ
      static const u16 data_BaseMonsNoTbl[] = {
        MONSNO_MINOMUTTI,   //ミノムッチ
        MONSNO_MINOMADAMU,  //ミノマダム
        MONSNO_KARANAKUSI,  //カラナクシ
        MONSNO_TORITODON,   //トリトドン
        MONSNO_BASURAO,     //バスラオ

        //以下kujira新ルール(kujiraに四季がないので、シカは母フォルムを引き継ぐことになった)
        MONSNO_SIKIZIKA,    //シキジカ
        MONSNO_MEBUKIZIKA,  //メブキジカ
        
        //以下kujira新ポケ
        MONSNO_HURABEBE,    //フラベベ
        MONSNO_HURAETTE,    //フラエッテ
        MONSNO_HURAAJESU,   //フラージェス
        MONSNO_BAKETTYA,    //バケッチャ
        MONSNO_PANPUZIN,    //パンプジン
        
        //以下niji新ポケ
        MONSNO_NAGAREBOSI,    // メテノ @fix GFNMCat[393] メテノがフォルムを引き継がない問題を修正
        MONSNO_MITUSUI,       // オドリドリ @fix GFNMCat[394] オドリドリがフォルムを引き継がない問題を修正
        //momiji追加
        MONSNO_OOKAMI1,       // イワンコ フォルム0＝通常（昼夜）　フォルム１=たそがれ　　20170523　社外要望
      };

      const CoreParam* base_poke = GetBasePokemon_forForm( parents );
      MonsNo base_monsno = base_poke->GetMonsNo();

      //20170523 社外要望　たそがれルガルガンの子供はたそがれフォルムを引き継ぐ
      FormNo form_no = base_poke->GetFormNo();
      if ((base_monsno == MONSNO_OOKAMI2) && (form_no == FORMNO_OOKAMI2_TWILIGHT)) //たそがれルガルガンのときのみ処理を行う
      {
        egg->spec.formno = FORMNO_OOKAMI1_TWILIGHT; //イワンコをたそがれフォルムにする
        return;
      }

      for( int i = 0;i < GFL_NELEMS(data_BaseMonsNoTbl);i++){
        if( data_BaseMonsNoTbl[i] == base_monsno ){
          egg->spec.formno = base_poke->GetFormNo();
          break;
        }
      }
    }

    /**
     * @brief タマゴのフォルムを決定する ( 例外ケース：コフキムシ )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     * @param[in]  setting 生成設定データ
     *
     * 生まれる子がコフキムシの場合は国・地域によりきめられたフォルムを指定する
     * 対象となる親は コフキムシ、コフーライ、ビビヨン
     */
    void EggGenerator::DecideEggParam_Form_REGION( EggParam* egg, const Parents& parents, const Setting& setting )
    {
      const CoreParam* base_poke;
      MonsNo base_monsno;
      MonsNo seed_monsno;

      base_poke   = GetBasePokemon_forForm( parents );
      base_monsno = base_poke->GetMonsNo();
      seed_monsno = personal::GetSeedMonsNo( base_monsno );
      if (seed_monsno == MONSNO_KOHUKIMUSI )
      {
        egg->spec.formno = setting.realRegionFormNo;
      }
    }

    /**
     * @brief タマゴのフォルムを決定する ( 地方種 )
     * @param[in]  heap    作業領域に使用するヒープ
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     * @param[in]  setting 生成設定データ
     *
     * 地方種IDをもっている場合は、地方種のフォルム番号を設定する
     * しかし、かわらずの石を親が持っている場合は、設定を行わない
     */
    void EggGenerator::DecideEggParam_Form_PERSONAL_REGION( gfl2::heap::HeapBase* heap, EggParam* egg, const Parents& parents )
    {
      // 地方種が存在しないバージョンかチェック
      if( pml::Library::m_setting->personalRegionID == personal::REGION_NORMAL )
      {
        return;
      }

      bool mother_have_KAWARAZUNOISI = ( parents.mother->GetItem() == ITEM_KAWARAZUNOISI );
      bool father_have_KAWARAZUNOISI = ( parents.father->GetItem() == ITEM_KAWARAZUNOISI );

      MonsNo mother_seed_monsno = personal::GetSeedMonsNo(parents.mother->GetMonsNo());
      MonsNo father_seed_monsno = personal::GetSeedMonsNo(parents.father->GetMonsNo());

      if( mother_have_KAWARAZUNOISI )
      {
        // 母親が変わらずの石を持っている場合でかつ、地方種を持たない場合、子は既存種
        // @fix NMCat[4098] メタモンが母親で、かわらずの石を使うと、既存種をかわらずの石で初発見してしまう可能性がある問題を修正
        if( parents.mother->GetMonsNo() != MONSNO_METAMON )
        {
          if( !IsCurrentRegionFormNo(heap,parents.mother) )
          {
            return;
          }
        }
      } 
      else if( father_have_KAWARAZUNOISI )
      {
        // @fix NMCat[4098] 母親の既存種をまだ見たこのない状態で、種族の父親がかわらずの石を持っていると、母親の既存種をかわらずの石で初発見してしまう問題を修正
        // ニドランやイルミーゼのように、生まれてくる子が、母のシードと一致しない場合は、この判定方法では対応しきれません。
        if( (mother_seed_monsno == father_seed_monsno) || (parents.mother->GetMonsNo() == MONSNO_METAMON) )
        {
          // 父親が変わらずの石を持っている場合でかつ、地方種を持たない場合、子は既存種
          if( !IsCurrentRegionFormNo(heap,parents.father) )
          {
            return;
          }
        }
      }

      // 地方種を持っている場合は地方種を設定する
      u32 formNum;
      pml::FormNo* formArray = GFL_NEW_LOW_ARRAY( heap ) pml::FormNo[MAX_FORM_NUM];

      pml::personal::Util::GetRegionFormNo(
        formArray, 
        &formNum, 
        egg->spec.monsno, 
        static_cast<pml::personal::RegionID>( pml::Library::m_setting->personalRegionID ));

      if( 0 < formNum )
      {
        egg->spec.formno = formArray[0];
      }
      GFL_SAFE_DELETE_ARRAY( formArray );
    }

    /**
     * @brief 子ポケモンのフォルム決定のベースとなる親ポケモンを取得する
     * @param parents 両親ポケモン
     */
    const CoreParam* EggGenerator::GetBasePokemon_forForm( const Parents& parents )
    {
      MonsNo mother_monsno = parents.mother->GetMonsNo();

      if( mother_monsno == MONSNO_METAMON ) 
      {
        // 母親がメタモン ==> 父親
        return parents.father;
      }
      else 
      {
        // 通常 ==> 母親
        return parents.mother;
      }
    }

    /**
     * @brief タマゴの性別を決定する
     * @param[out] egg     タマゴパラメータ
     * @param[in]  monsno  タマゴのモンスターNo.
     * @param[in]  formno  タマゴのフォルムNo.
     *
     * パーソナルの性別分布に従い、ランダムに決定する
     */
    void EggGenerator::DecideEggParam_Sex( EggParam* egg, MonsNo monsno, FormNo formno, gfl2::math::Random* pRandom )
    {
      personal::LoadPersonalData( monsno, formno );
      u32 sex_vector = personal::GetPersonalParam( personal::PARAM_ID_sex );

      if( sex_vector == personal::SEX_VECTOR_ONLY_MALE )
      {
        // ♂のみ
        egg->spec.sex = SEX_MALE;
      }
      else if( sex_vector == personal::SEX_VECTOR_ONLY_FEMALE )
      {
        // ♀のみ
        egg->spec.sex = SEX_FEMALE;
      }
      else if( sex_vector == personal::SEX_VECTOR_UNKNOWN )
      {
        // 不明
        egg->spec.sex = SEX_UNKNOWN;
      }
      else
      {
        // ♂♀両方が存在
        u32 range = personal::SEX_VECTOR_RANDOM_MAX - personal::SEX_VECTOR_RANDOM_MIN;
        u32 rnd = pRandom->Next( range );
        rnd += personal::SEX_VECTOR_RANDOM_MIN;  // [SEX_VECTOR_RND_MIN, SEX_VECTOR_MAX]
        egg->spec.sex = ( rnd < sex_vector ) ?  SEX_FEMALE : SEX_MALE;
      }
    }

    /**
     * @brief タマゴの性格を決定する ( 基本ルール )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 基本はランダムに決定する
     */
    void EggGenerator::DecideEggParam_Seikaku_BASIC( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      egg->spec.seikaku = pRandom->Next( SEIKAKU_NUM );
    }

    /**
     * @brief タマゴの性格を決定する ( 例外ケース：かわらずのいし )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 『かわらずのいし』をもっている親の性格を100%継承する
     * 両親ともに持っている場合は50%ずつの確率で一方の性格を継承する
     */
    void EggGenerator::DecideEggParam_Seikaku_KAWARAZUNOISI( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      bool father_have_KAWARAZUNOISI = ( parents.father->GetItem() == ITEM_KAWARAZUNOISI );
      bool mother_have_KAWARAZUNOISI = ( parents.mother->GetItem() == ITEM_KAWARAZUNOISI );
      Seikaku father_seikaku = parents.father->GetSeikaku();
      Seikaku mother_seikaku = parents.mother->GetSeikaku();

      if( father_have_KAWARAZUNOISI && mother_have_KAWARAZUNOISI ) 
      { 
        // 両親とも所持
        if( pRandom->Next(2) == 0 )  {
          egg->spec.seikaku = father_seikaku;
        }
        else {
          egg->spec.seikaku = mother_seikaku;
        }
      }
      else if( father_have_KAWARAZUNOISI ) 
      { 
        // 父親のみ所持
        egg->spec.seikaku = father_seikaku;
      }
      else if( mother_have_KAWARAZUNOISI ) 
      { 
        // 母親のみ持所
        egg->spec.seikaku = mother_seikaku;
      }
    }
   
    /**
     * @brief タマゴが何番目の特性かを決定する
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * ※kujiraでの新仕様
     *
     * 継承元の決まり方：
     * 　母がメタモンの場合 ==> 父から継承
     * 　それ以外           ==> 母から継承
     *
     * 継承元の親の特性によるタマゴの特性の決まり方：
     * 　継承元が特性1    ==> 特性1( 80% )  特性2( 20% )
     * 　継承元が特性2    ==> 特性1( 20% )  特性2( 80% )
     * 　継承元が特性3    ==> 特性1( 20% )  特性2( 20% )  特性3( 60% )
     * 　継承元が不正特性 ==> 特性1( 50% )  特性2( 50% )
     *
     * ※syachi世代までの仕様
     *  継承元は母で固定。特性の決まり方は変わらない(不正特性判定は無かった 補足1)。
     *
     *  加えて、メタモン特別ルールが存在し
     *  syachi世代では両親のどちらかがメタモンの場合、タマゴの特性は 
     *  特性1( 50% ) or 特性2( 50% ) になっていた。
     *  
     *  補足1：
     *   syachi世代では継承元である母のTokuseiIndexのみを判定しており、
     *   母の特性Indexはtokusei_3_flagとpersonal_rndの個体値判定bitの組み合わせで決まるため
     *  「母が不正な特性を持っていたか？」どうかは判定していなかった。
     *
     */
    void EggGenerator::DecideEggParam_TokuseiIndex( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      u8 tokusei_index = 0;
      u32 rnd = pRandom->Next(100);
      MonsNo mother_monsno = parents.mother->GetMonsNo();

      if( mother_monsno == MONSNO_METAMON ){
        //母がメタモンの場合のみ、父から特性を継承
        tokusei_index = parents.father->GetTokuseiIndexStrict();
      }else{
        //母から特性を継承
        tokusei_index = parents.mother->GetTokuseiIndexStrict();
      }

      switch( tokusei_index ) 
      {
        // 継承元が特性1
      case 0:
        if( rnd < 80 ) { egg->spec.tokusei_index = 0; } // 80% ==> 特性1
        else           { egg->spec.tokusei_index = 1; } // 20% ==> 特性2
        break;

        // 継承元が特性2
      case 1: 
        if( rnd < 20 ) { egg->spec.tokusei_index = 0; } // 20% ==> 特性1
        else           { egg->spec.tokusei_index = 1; } // 80% ==> 特性2
        break;

        // 継承元が特性3
      case 2: 
        if( rnd < 20 )      { egg->spec.tokusei_index = 0; } // 20% ==> 特性1
        else if( rnd < 40 ) { egg->spec.tokusei_index = 1; } // 20% ==> 特性2
        else                { egg->spec.tokusei_index = 2; } // 60% ==> 特性3
        break;
        // 継承元の特性が不正
      default:
        if( rnd < 50 ) { egg->spec.tokusei_index = 0; } // 50% ==> 特性1
        else           { egg->spec.tokusei_index = 1; } // 50% ==> 特性2
        break;
      }
    }

    /**
     * @brief タマゴのなつき度 ( 孵化カウント ) を決定する
     * @param[in,out] egg  タマゴパラメータ
     *
     * タマゴの間は, なつき度の値を孵化カウンタとして利用する
     */
    void EggGenerator::DecideEggParam_BirthCount( EggParam* egg )
    {
      personal::LoadPersonalData( egg->spec.monsno, egg->spec.formno );
      u32 birth_count = personal::GetPersonalParam( personal::PARAM_ID_egg_birth );
      egg->spec.familiarity = birth_count;
    }

    /**
     * @brief 両親から継承する個体値を決定する( あかいいと )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 親が「あかいいと」を持っている場合
     * 両親から継承する個体値の数が5に増える
     */
    void EggGenerator::DecideDerivedTalentPower_AKAIITO( EggParam* egg, const Parents& parents )
    {
      u32 father_item = parents.father->GetItem();
      u32 mother_item = parents.mother->GetItem();

      if( ( father_item == ITEM_AKAIITO ) ||
          ( mother_item == ITEM_AKAIITO ) )
      {
        egg->totalDeriveTalentCount = 5;
      }
      else
      {
        // 通常は3つ継承する
        egg->totalDeriveTalentCount = 3;
      }
    }

    /**
     * @brief 両親から継承する個体値を決定する( パワー系アイテム所持 )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 親がパワーxxxx系のアイテムを所持している場合, 
     * アイテムに対応する個体値を親から計１つ継承する
     * 両親が所持している場合, 父が有効( 50% ) 母が有効( 50% )
     */
    void EggGenerator::DecideDerivedTalentPower_ITEM( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      // 各能力に対応する道具
      const u32 POWER_ITEMS[ POWER_NUM ] = 
      {
        ITEM_PAWAAUEITO,
        ITEM_PAWAARISUTO,
        ITEM_PAWAABERUTO,
        ITEM_PAWAARENZU,
        ITEM_PAWAABANDO,
        ITEM_PAWAAANKURU,
      };

      // 父親の所持アイテムをチェック
      bool father_have = false;
      u32 father_item = parents.father->GetItem();
      u32 derive_power_index_from_father = 0;

      for( u32 i=0; i<POWER_NUM; i++ )
      {
        if( father_item == POWER_ITEMS[i] ) {
          father_have = true;
          derive_power_index_from_father = i;
          break;
        }
      }

      // 母親の所持アイテムをチェック
      bool mother_have = false;
      u32 mother_item = parents.mother->GetItem();
      u32 devive_power_index_from_mother = 0;

      for( u32 i=0; i<POWER_NUM; i++ )
      {
        if( mother_item == POWER_ITEMS[i] ) {
          mother_have = true;
          devive_power_index_from_mother = i;
          break;
        }
      }

      if( father_have && mother_have ) 
      { 
        // 両親とも所持
        if( pRandom->Next(2) == 0 ) { 
          egg->isTalentDerivedFromFather[ derive_power_index_from_father ] = true;
          egg->derivedTalentCount++;
        }
        else {
          egg->isTalentDerivedFromMother[ devive_power_index_from_mother ] = true;
          egg->derivedTalentCount++;
        }
      }
      else if( father_have ) 
      { 
        // 父親のみ所持
        egg->isTalentDerivedFromFather[ derive_power_index_from_father ] = true;
        egg->derivedTalentCount++;
      }
      else if( mother_have ) 
      { 
        // 母親のみ所持
        egg->isTalentDerivedFromMother[ devive_power_index_from_mother ] = true;
        egg->derivedTalentCount++;
      }
    }

    /**
     * @brief タマゴが両親から継承する個体値を決定する( 基本ルール )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 両親の個体値ををランダムに継承する
     */
    void EggGenerator::DecideDerivedTalentPower_RANDOM( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      while( egg->derivedTalentCount < egg->totalDeriveTalentCount )
      {
        u32 power_index = pRandom->Next( POWER_NUM );

        if( egg->isTalentDerivedFromFather[ power_index ] ||
            egg->isTalentDerivedFromMother[ power_index ] )
        {
          continue; // 選択済み
        }

        if( pRandom->Next(2) == 0 ) 
        {
          // 父から継承
          egg->isTalentDerivedFromFather[ power_index ] = true;
          egg->derivedTalentCount++;
        }
        else 
        {
          // 母から継承
          egg->isTalentDerivedFromMother[ power_index ] = true;
          egg->derivedTalentCount++;
        }
      }
    }

    /**
     * @brief タマゴの個体値をランダムに決定する
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     */
    void EggGenerator::DecideEggParam_TalentPower_RANDOM( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      for( u32 i=0; i<POWER_NUM; i++ )
      {
        egg->spec.talentPower[i] = pRandom->Next( MAX_TALENT_POWER + 1 );
      } 
    }

    /**
     * @brief タマゴの個体値を両親から継承する
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 計３つの個体値を両親から継承する
     */
    void EggGenerator::DecideEggParam_TalentPower_DERIVE( EggParam* egg, const Parents& parents )
    {
      for( u32 i=0; i<POWER_NUM; i++ )
      {
        const CoreParam* derive = NULL;

        if( egg->isTalentDerivedFromFather[i] ) { 
          derive = parents.father;
        }
        else if( egg->isTalentDerivedFromMother[i] ) {
          derive = parents.mother;
        }

        if( derive ) {
          egg->spec.talentPower[i] = derive->GetNativeTalentPower( static_cast<PowerID>(i) );
        }
      } 
    }

    /**
     * @brief タマゴの個性乱数を決定する ( 基本ルール )
     * @param[out] egg     タマゴパラメータ
     * @param[in]  parents 両親ポケモン
     *
     * 基本はランダムに決定する
     */
    void EggGenerator::DecideEggParam_PersonalRnd( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      egg->spec.personal_rnd = pRandom->Next();
    }

    /**
     * @brief タマゴの個性乱数を決定する ( レア抽選 )
     * @param[out] egg                 タマゴパラメータ
     * @param[in]  parents             両親ポケモン
     * @param[in]  have_HIKARUOMAMORI 『ひかるおまもり』を持っているか？
     *
     * 言語コード(BW世代での国コードに相当)が異なる場合, 6回抽選する
     * 『ひかるおまもり』を持っている場合, +2回抽選する
     */
    void EggGenerator::DecideEggParam_RareRnd( 
        EggParam* egg, const Parents& parents, bool have_HIKARUOMAMORI, gfl2::math::Random* pRandom )
    { 
      enum {
        RARE_CHANCE_LANG_ID       = 6, // 言語コード(BW世代までの国コードに相当)による抽選回数
        RARE_CHANGE_HIKARUOMAMORI = 2, //「ひかるおまもり」による抽選回数
      };

      u32 try_count = 0; 

      // 親同士の言語コード(BW世代での国コード相当)が異なるなら, 6回抽選する
      u32 father_lang = parents.father->GetLangId();
      u32 mother_lang = parents.mother->GetLangId();
      if( father_lang != mother_lang ) { 
        try_count += RARE_CHANCE_LANG_ID;
      } 

      //「ひかるおまもり」を持っている場合は抽選回数+2
      if( have_HIKARUOMAMORI ) { 
        try_count += RARE_CHANGE_HIKARUOMAMORI; 
      } 

      for( u32 i=0; i<try_count; i++ )
      {
        const u32 rand = pRandom->Next();
        egg->spec.rare_rnd = rand;
        if( pokepara::CalcTool::IsRareColor( static_cast<u32>(egg->spec.id), rand ) ) {
          break;
        }
      } 
    }

    /**
     * @brief タマゴの技を決定する( デフォルト技 )
     * @param[out] egg  タマゴパラメータ
     */
    void EggGenerator::DecideEggParam_Waza_DEFAULT( EggParam* egg )
    {
      personal::LoadWazaOboeData( egg->spec.monsno, egg->spec.formno );

      u32 waza_count =  personal::GetWazaOboeNum();
      for( u32 i=0; i<waza_count; i++ )
      {
        u32 wazaLv = personal::GetWazaOboeLevel( i );
        WazaNo wazano = personal::GetWazaOboeWazaNo( i );
        if( egg->spec.level < wazaLv ) { 
          break; 
        }
        PushWaza_to_EggParam( egg, wazano );
      }
    }

    /**
     * @brief 技を押し出し式に追加する
     * @param[out] egg    追加先バッファ
     * @param[in]  wazano 追加する技No.
     */
    void EggGenerator::PushWaza_to_EggParam( EggParam* egg, WazaNo wazano )
    {
      // 技番号が不正
      if( wazano == WAZANO_NULL ) { 
        return; 
      }

      // すでに覚えている
      for( u32 i=0; i<egg->wazaCount; i++ ) {
        if( egg->wazano[i] == wazano ) { return; }
      }

      // 押し出し式に追加
      if( egg->wazaCount < MAX_WAZA_NUM ) 
      {
        egg->wazano[ egg->wazaCount ] = wazano;
        egg->wazaCount++;
      }
      else 
      {
        for( u32 i=0; i<MAX_WAZA_NUM-1; i++ ) {
          egg->wazano[i] = egg->wazano[i+1];
        }
        egg->wazano[ MAX_WAZA_NUM - 1 ] = wazano;
      }
    }
    
    /**
     * @brief 子技継承をチェックして押し出し式に追加する処理のコア
     * @param[out] egg         追加先バッファ
     * @param[in]  parent      継承元の親のCoreParam
     * @param[in]  eggWazaData タマゴの中身のポケモンの子技データ
     */
    void EggGenerator::PushEggWaza_to_EggParam( EggParam* egg, const CoreParam* parent, const EggWazaData& eggWazaData )
    {
      /*
       * 継承元の親がメタモンだったら子技継承しない
       *
       * 本来メタモンは「へんしん」以外憶えないが、DP世代のメタモンを経由した技継承関連バグで
       * 過去作から連れてきたメタモンが「へんしん」以外の技を憶えている可能性があるので、これを封じ込める
       */
      if( parent->GetMonsNo() == MONSNO_METAMON ){
        return;
      }

      const u32 eggWazaNum = eggWazaData.GetEggWazaNum();

      for( u32 i=0; i<MAX_WAZA_NUM; i++ )
      {
        const WazaNo wazano = parent->GetWazaNo( i ); 
        for( u32 eggWazaIndex=0; eggWazaIndex<eggWazaNum; eggWazaIndex++ )
        {
          if( wazano == eggWazaData.GetEggWazaNo( eggWazaIndex ) ) 
          {
            PushWaza_to_EggParam( egg, wazano );
            break;
          }
        }
      }
    }

    /**
     * @brief タマゴ技を覚えさせる
     *
     * タマゴから孵るポケモンのみが覚えられる技のうち, 父/母ポケモンが覚えている技を習得
     *
     * syachi世代までは父からの継承のみだったが、kujiraで母からも継承するように仕様変更。
     * 母からの継承の方が強い。
     *
     * BTS2030：母からの子技継承が実装されていないバグに対処
     * BTS2040：メタモンから子技継承してしまうバグに対処
     */
    void EggGenerator::DecideEggParam_Waza_EGG( 
        gfl2::heap::HeapBase* heap, EggParam* egg, const Parents& parents )
    {
      // 子技データ読み込み
      gfl2::heap::HeapBase* lower_heap = heap->GetLowerHandle();
      EggWazaData eggWazaData( lower_heap );
      eggWazaData.Load( pml::Library::m_setting->garcPath_KowazaTable, egg->spec.monsno, egg->spec.formno );

      //母継承の方が優先されるので、先ずは父からの子技継承チェック
      PushEggWaza_to_EggParam( egg, parents.father, eggWazaData );
      
      //父の後に母継承をチェック。父継承は全て押し出されることもある
      PushEggWaza_to_EggParam( egg, parents.mother, eggWazaData );
    }

    /**
     * @brief 両親の技を受け継ぐ
     * @param[out] egg      タマゴパラメータ
     * @param[in]  parents  両親ポケモン
     *
     * 自分もレベルを上げることで覚える技のうち, 
     * 両親ともに覚えている技を習得
     */
    void EggGenerator::DecideEggParam_Waza_PARENTS( EggParam* egg, const Parents& parents )
    {
      personal::LoadWazaOboeData( egg->spec.monsno, egg->spec.formno ); 
      u32 wazaoboe_count = personal::GetWazaOboeNum();

      // 子が覚える全ての技をチェック
      for( u32 i=0; i<wazaoboe_count; i++ )
      {
        WazaNo wazano = personal::GetWazaOboeWazaNo( i );

        // 父が覚えているかどうかチェック
        bool father_have = false;
        for( u32 j=0; j<MAX_WAZA_NUM; j++ )
        {
          WazaNo father_wazano = parents.father->GetWazaNo( j ); 
          if( father_wazano == wazano ) {
            father_have = true;
            break;
          }
        }

        // 母が覚えているかどうかチェック
        bool mother_have = false;
        for( u32 j=0; j<MAX_WAZA_NUM; j++ )
        {
          WazaNo mother_wazano = parents.mother->GetWazaNo( j );
          if( mother_wazano == wazano ) {
            mother_have = true;
            break;
          }
        }

        // 両親とも覚えていたら継承する
        if( father_have && mother_have ) {
          PushWaza_to_EggParam( egg, wazano );
        }
      }
    }

    /**
     * @brief 父ポケモンのマシン技を受け継ぐ
     * @param[out] egg      タマゴパラメータ
     * @param[in]  parents  両親ポケモン
     *
     * 自分も覚えることの出来るマシン技のうち, 
     * 父ポケモンが覚えている技を習得
     */
    void EggGenerator::DecideEggParam_Waza_MACHINE( EggParam* egg, const Parents& parents )
    {
      for( u32 i=0; i<MAX_WAZA_NUM; i++ )
      {
        WazaNo father_wazano = parents.father->GetWazaNo( i );

        // 対応する技マシンがあり, タマゴポケに使用できるなら習得
        for( u32 itemno=ITEM_WAZAMASIN01; itemno<=ITEM_DATA_MAX; itemno++ ) // @attention 遅い場合ループ回数を減らす
        {
          WazaNo item_wazano = static_cast<WazaNo>( item::ITEM_GetWazaNo( itemno ) );
          if( ( father_wazano == item_wazano ) &&
              ( IsWazaMachineAbleToUse( egg->spec.monsno, egg->spec.formno, itemno ) ) ) 
          {
            PushWaza_to_EggParam( egg, father_wazano );
            break;
          }
        }
      }
    }

    /**
     * @brief 技マシンが使用できるかどうかを判定する
     * @param monsno  技マシン使用対象ポケモンのモンスターNo.
     * @param formno  モンスターのフォームNo.
     * @param itemno  使用する技マシンのアイテムNo.
     * @retval true  技マシンを使用できる
     * @retval false 技マシンを使用できない
     */
    bool EggGenerator::IsWazaMachineAbleToUse( MonsNo monsno, FormNo formno, u16 itemno )
    {
      //u16 seed_monsno;

      if( MONSNO_END < monsno ) {
        //GFL_PRINT( "WARNING!! モンスターNo.が不正です( monsno=%d )\n", monsno );
        GFL_ASSERT(0);  //@check 引数異常はfalseを返す
        return false;
      }

      if( item::ITEM_CheckWazaMachine( itemno ) == false ) {
        //GFL_PRINT( "WARNING!! アイテム( %d )は技マシンではありません\n", itemno );
        //GFL_ASSERT(0);
        return false;
      } 

      personal::LoadPersonalData( monsno, formno );

      // 技マシン番号( 0 origin )を取得
      u32 machine_no = item::ITEM_GetWazaMashineNo( itemno );

      // 該当する技マシンフラグ・検査ビット位置を取得
      u32 machine_flag = 0;
      u32 bit_pos = 0;
      if( machine_no < 32*1 ) 
      {
        machine_flag = personal::GetPersonalParam( personal::PARAM_ID_machine1 );
        bit_pos = machine_no;
      }
      else if( machine_no < 32*2 ) 
      {
        machine_flag = personal::GetPersonalParam( personal::PARAM_ID_machine2 );
        bit_pos = machine_no - 32;
      }
      else if( machine_no < 32*3 ) 
      {
        machine_flag = personal::GetPersonalParam( personal::PARAM_ID_machine3 );
        bit_pos = machine_no - 32*2;
      }
      else 
      {
        machine_flag = personal::GetPersonalParam( personal::PARAM_ID_machine4 );
        bit_pos = machine_no - 32*3;
      }

      return ( machine_flag & ( 1 << bit_pos ) ) != 0 ?true:false;
    }

    /**
     * @brief 特殊タマゴ作成処理 ( ピチュー )
     * @param[out] egg      タマゴパラメータ
     * @param[in]  parents  両親ポケモン
     *
     * 親がアイテム『でんきだま』を持っていたら,
     * 技『ボルテッカー』を習得
     */
    void EggGenerator::DecideEggParam_Waza_PITYUU( EggParam* egg, const Parents& parents )
    {
      if( egg->spec.monsno != MONSNO_PITYUU ) 
      { 
        return; 
      }

      if( ( parents.father->GetItem() == ITEM_DENKIDAMA ) ||
          ( parents.mother->GetItem() == ITEM_DENKIDAMA ) ) 
      {
        PushWaza_to_EggParam( egg, WAZANO_BORUTEKKAA );
      }
    }

    /**
     * @brief タマゴのボールIDを決定する
     * @param[out] egg      タマゴパラメータ
     * @param[in]  parents  両親ポケモン
     *
     * 母がメタモンの場合は父のボールを引き継ぐ
     * 母と父が同じポケモンの場合は、ランダムで母もしくは父のボールを引き継ぐ
     * 母と父が違うポケモンの場合は、母のボールを引き継ぐ
     * ただし、「マスターボール」と「プレシャスボール」の場合は「モンスターボール」にする。
     */
    void EggGenerator::DecideEggParam_BallID( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom )
    {
      const u32 mother_monsno = parents.mother->GetMonsNo();
      const u32 father_monsno = parents.father->GetMonsNo();
      const u32 mother_ball = parents.mother->GetMemories( MEMORIES_CAPTURED_BALL );
      const u32 father_ball = parents.father->GetMemories( MEMORIES_CAPTURED_BALL );

      if( mother_monsno == MONSNO_METAMON )
      {
        egg->ballID = father_ball;
      }
      else
      {
        if( mother_monsno == father_monsno )
        {
          u32 rnd = pRandom->Next( 100 ) + 1; // 1から100の乱数
          egg->ballID = 50 < rnd ? father_ball : mother_ball;
        }else{
          egg->ballID = mother_ball;
        }
      }

      if( ( egg->ballID == item::BALLID_MASUTAABOORU ) ||
          ( egg->ballID == item::BALLID_PURESYASUBOORU ) ) 
      {
        egg->ballID = item::BALLID_MONSUTAABOORU;
      }
    }

    /**
     * @brief タマゴのパラメータをセットアップする
     * @param[in]  heap      作業領域に使用するヒープ
     * @param[out] egg       セットアップ対象のパラメータ
     * @param[in]  egg_param タマゴパラメータ
     * @param[in]  parents   両親ポケモン
     * @param[in]  setting   設定
     */
    void EggGenerator::SetupEggParam( 
        gfl2::heap::HeapBase* heap,
        CoreParam* egg, 
        const EggParam& egg_param,
        const Parents& parents,
        const Setting& setting )
    {
      bool fastModeFlag = egg->StartFastMode(); 

      // 親の名前
      egg->SetParentName( setting.parentName );

      // ニックネームを "タマゴ" にする
      gfl2::str::StrBuf nickname( MONS_NAME_LENGTH + EOM_LENGTH, heap );
      personal::GetMonsName( &nickname, MONSNO_TAMAGO );
      egg->SetNickName( &nickname );

      // タマゴ技・継承技をセット
      for( u32 i=0; i<egg_param.wazaCount; i++ )
      {
        egg->SetWaza( i, egg_param.wazano[i] );
        egg->SetTamagoWazaNo( i, egg_param.wazano[i] );
      }

      // ボールID
      egg->SetMemories( MEMORIES_CAPTURED_BALL, egg_param.ballID );

      // タマゴにする
      egg->SetEggFlag();

      egg->EndFastMode( fastModeFlag );
    }

    /**
     * @brief 現在のROMに対応した地方種IDを持っているか
     * @param[in]   heap          ヒープ
     * @param[in]   coreParam     ポケモンデータ
     * @retval 持っている場合にtrue
     */
    bool EggGenerator::IsCurrentRegionFormNo( gfl2::heap::HeapBase* heap, const CoreParam* coreParam )
    {
      u32 formNum;
      pml::FormNo* formArray = GFL_NEW_LOW_ARRAY( heap ) pml::FormNo[MAX_FORM_NUM];

      pml::personal::Util::GetRegionFormNo(
        formArray, 
        &formNum, 
        coreParam->GetMonsNo(), 
        static_cast<pml::personal::RegionID>( pml::Library::m_setting->personalRegionID ));

      bool isHaveRegionFormNo = ( 0 < formNum );
      bool isCurrentRegionFormNo = false;
      if( isHaveRegionFormNo )
      {
        // 地方種フォルムNoと現在のフィルムナンバーが一致しているかチェック
        isCurrentRegionFormNo =(formArray[0] == coreParam->GetFormNo());
      }else{
        // 地方種フォルムを持たない場合は常にfalse
        isCurrentRegionFormNo = false;
      }

      GFL_SAFE_DELETE_ARRAY( formArray );
      return isCurrentRegionFormNo;

    }

  } //namespace pokepara
} //namespace pml
