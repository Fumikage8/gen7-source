//============================================================================================
/**
 * @file    GTSPokeNameSearch.cpp
 * @brief   文字列をポケモンNO変換する（みつからなかったら-1)
 * @author  Akito Mori / 移植 endo_akira
 * @date    12.08.17 / 15.09.03
 *
 * @note 日本語・英語は頭文字から対象を絞り込んでから探索するが、韓国語は\n
 * @note 子音指定ができないため全探索する
 
 */
//============================================================================================
#include "NetApp/GTS/source/GTSPokeNameSearch.h"
#include "NetApp/GTS/source/GTSZukanSortTable.hpp"
#include "PokeTool/include/PokeToolPersonalSort.h"
#include <pml/include/personal/pml_PersonalSystem.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "System/include/PokemonVersion.h"
#include "NetStatic/NetAppLib/include/Util/NetAppLatinumStringTool.h"
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


//=============================================================================================
/**
 * @brief	コンストラクタ
 *
 * @param   none		
 */
//=============================================================================================
GtsPokeNameSearch::GtsPokeNameSearch( app::util::Heap *heap, Savedata::ZukanData *pZukanData )
{
  GFL_PRINT("heap=%08x, pZukanData=%08x\n", heap, pZukanData );
  
  m_heap = heap;

  gfl2::heap::HeapBase* pDeviceHeap = heap->GetDeviceHeap();
  
  // 文字列取得
  m_search_str = GFL_NEW_LOW( pDeviceHeap ) gfl2::str::StrBuf( SEARCH_TOPWORD_TABLE_NUM_JP*3, pDeviceHeap );
  m_kata_str   = GFL_NEW_LOW( pDeviceHeap ) gfl2::str::StrBuf( SEARCH_TOPWORD_TABLE_NUM_JP*3, pDeviceHeap );
  m_pZukanData = pZukanData;

}

//=============================================================================================
/**
 * @brief	デストラクタ
 *
 * @param   none		
 */
//=============================================================================================
GtsPokeNameSearch::~GtsPokeNameSearch(void)
{
  GFL_DELETE m_kata_str;
  GFL_DELETE m_search_str;
}

//=============================================================================================
/**
 * @brief	頭文字の濁点・半濁点を外す（「パ・バ」だったら「ハ」にする）
 *
 * @param   array		
 */
//=============================================================================================
void GtsPokeNameSearch::DeleteDakutenHandakuten( gfl2::str::STRCODE *array )
{
// msg_gts_02A_047        System    ガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポァィゥェォッ
// msg_gts_02A_048        System    カキクケコサシスセソタチツテトハヒフヘホハヒフヘホアイウエオツ

  static const gfl2::str::STRCODE DAKUTEN_HANDAKUTEN_TABLE[]          = L"ガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポァィゥェォッ";
  static const gfl2::str::STRCODE DAKUTEN_HANDAKUTEN_REPLACE_TABLE[]  = L"カキクケコサシスセソタチツテトハヒフヘホハヒフヘホアイウエオツ";

  // 濁点・半濁点があるか比較
  for(int i=0;i<TRANS_TOPWORD_TABLE_NUM_JP;i++){
    if(DAKUTEN_HANDAKUTEN_TABLE[i]==array[0]){
      array[0] = DAKUTEN_HANDAKUTEN_REPLACE_TABLE[i];  // あったら書き換え
      GFL_PRINT("%dがヒット %dに変換\n", DAKUTEN_HANDAKUTEN_TABLE[i], DAKUTEN_HANDAKUTEN_REPLACE_TABLE[i]);
    }
  }
}

//=============================================================================================
/**
 * @brief	ひらがな入力されている文字をカタカナに変換する
 *
 * @param   array		
 */
//=============================================================================================
void GtsPokeNameSearch::ChangeHiraToKatakana( gfl2::str::StrBuf *src, gfl2::str::StrBuf *des )
{
  static const int CODE_NUM = 80;
  static const gfl2::str::STRCODE hira[] = L"あいうえおかきくけこさしすせそたちつてと"
                                          L"なにぬねのはひふへほまみむめもやゆよらりるれろわをん"
                                          L"がぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽ"
                                          L"ぁぃぅぇぉっゃゅょ";
  static const gfl2::str::STRCODE kata[] = L"アイウエオカキクケコサシスセソタチツテト"
                                          L"ナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン"
                                          L"ガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポ"
                                          L"ァィゥェォッャュョ";
  gfl2::str::STRCODE buf[SEARCH_TOPWORD_TABLE_NUM_JP*3];
  const gfl2::str::STRCODE *p_code = src->GetPtr();
  int length = src->GetLength();

  int n;
  // ひらがなを1文字ずつ比較
  for(int i=0;i<length;i++){
    for(n=0;n<CODE_NUM;n++){
      if(hira[n]==p_code[i]){
        buf[i] = kata[n];  // ヒットしたら書き換え
        GFL_PRINT("hira[%d] hit\n", n);
        break;
      }
    }
    if(n==CODE_NUM){ buf[i] = p_code[i]; }
  }
  buf[length] = 0;
  
  // 変換した結果を格納する
  des->SetStr( buf );
  
}

//=============================================================================================
/**
 * @brief	入力されているポケモン名の頭文字を取得して検索キーに変換できるかチェック
 *
 * @param   name  入力したポケモン名
 *
 * @retval  int -1:該当なし 0-49: 検索用のキーNO
 */
//=============================================================================================
int GtsPokeNameSearch::GetString2SearchKey( gfl2::str::StrBuf *name )
{
  int result = -1;
  int i;
  // 入力されていなければ無し
  if(name->GetLength()==0){
    return result;  // -1
  }

  // ひらがな→カタカナ変換
  ChangeHiraToKatakana( name, m_kata_str );
  
#if PM_DEBUG  
  
  const gfl2::str::STRCODE *sc1 = name->GetPtr();
  const gfl2::str::STRCODE *sc2 = m_kata_str->GetPtr();
  GFL_PRINT("src=%04x,%04x,%04x,%04x,%04x,%04x\n", sc1[0],sc1[1],sc1[2],sc1[3],sc1[4],sc1[5]);
  GFL_PRINT("des=%04x,%04x,%04x,%04x,%04x,%04x\n", sc2[0],sc2[1],sc2[2],sc2[3],sc2[4],sc2[5]);
#endif
  // 入力ポケモン名の頭文字取得
  const gfl2::str::STRCODE *p_src = m_kata_str->GetPtr();
  gfl2::str::STRCODE word         = p_src[0];

  // 頭文字が濁点・半濁点の場合に変換する
  DeleteDakutenHandakuten( &word );

  // msg_gts_02A_049        System    アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワ

  static const gfl2::str::STRCODE SEARCH_KEY_CONVERT_TABLE[] = L"アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワ";

  // ア～ワまで比較
  for( i = 0 ; i < GFL_NELEMS( SEARCH_KEY_CONVERT_TABLE ); i++ )
  {
    if( SEARCH_KEY_CONVERT_TABLE[i] == word )
    {
      result = i;
      break;
    }
  }
  GFL_PRINT("%d回 頭文字を比較\n",i);

  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		直接入力のポケモン名が存在するか返す
 * @return	1-795: ポケモンNO,  0:なし
 *
 */
//--------------------------------------------------------------------------------------------
int GtsPokeNameSearch::CheckString2PokeNo( gfl2::str::StrBuf *string, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork )
{
  int result = 0;

  gfl2::heap::HeapBase* pDeviceHeap = m_heap->GetDeviceHeap();

  // 比較用ポケモン名バッファ
  gfl2::str::StrBuf *name = GFL_NEW_LOW( pDeviceHeap ) gfl2::str::StrBuf( pml::MONS_NAME_BUFFER_SIZE, pDeviceHeap );
  
  // ポケモンパーソナルデータソートクラス初期化
  PokeTool::PersonalSort* p_sort = GFL_NEW( pDeviceHeap ) PokeTool::PersonalSort( System::GetLang(), m_pZukanData, pDeviceHeap, PokeTool::PersonalSort::LOAD_TYPE_FULL );

  // 韓国語は子音から絞り込めないためポケモン名を全探索する
  if(System::GetLang()==POKEMON_LANG_KOREA)
  {
    for(int i=MONSNO_HUSIGIDANE;i<MONSNO_TAMAGO;i++){
      pml::personal::GetMonsName( name, static_cast<MonsNo>(i) );
      if(string->Compare( *name )){
        result = static_cast<int>(i);
        GFL_PRINT( "番号%dを発見\n", i );
        break;
      }
    }

  // 日本語はポケモンソートを使用して絞り込みを行ってから探索
  }
  else
  if(System::GetLang()==POKEMON_LANG_JAPAN)
  {
    int key    = GetString2SearchKey( string ); // 検索用のキー取得
    if(key<0){
      GFL_PRINT( "頭文字が探索できる文字でない\n");
    }else{
      // 直接入力は全図鑑から無条件で検索できる
      p_sort->SetConditionZukan( PokeTool::PokemonSort::CONDITION_ZUKAN_NONE );
      p_sort->OrderByMonsName( NetApp::GTS::ZUKAN_SORT_KEY_TABLE[key] );
      GFL_PRINT("ポケモン総数 = %d", p_sort->GetSortedDataMax());
      for( int i=0; i<p_sort->GetSortedDataMax(); ++i )
      {
        // 該当するポケモン名を取得し、比較する
        MonsNo monsno = p_sort->GetSortedMonsNo( i );
        pml::personal::GetMonsName( name, monsno );
        if(m_kata_str->Compare( *name )){
          result = static_cast<int>(monsno);
          GFL_PRINT( "番号%dを発見\n", monsno );
          break;
        }
      }
    }
  // 英語・欧州言語は一林さんのアルファベット文字列比較ルーチンを使用して全探索する
  }
  else
  if( System::GetLang() == POKEMON_LANG_CHINA || System::GetLang() == POKEMON_LANG_TAIWAN )
  {
    ChangeHiraToKatakana( string, m_kata_str );

    result = getMonsNo( m_kata_str, pGTSWork->GetJapanMonsNameMessageUtility() );
    
    // @fix GFNMCat[2146] : 英語は対処しないようになった
    /*
    if( result == 0 )
    {
      result = getMonsNo( string, pGTSWork->GetEnglishMonsNameMessageUtility() );
    }
    */
  }
  else
  {
    for(int i=MONSNO_HUSIGIDANE;i<MONSNO_TAMAGO;i++){
      pml::personal::GetMonsName( name, static_cast<MonsNo>(i) );
      if(NetAppLib::Util::NetAppLatinumStringTool::CompareBufferNoCaseNoAccent( string, name )){
        result = static_cast<int>(i);
        GFL_PRINT( "番号%dを発見\n", i );
        break;
      }
    }
  }
  GFL_DELETE p_sort;
  GFL_DELETE name;

  return result;
}


int GtsPokeNameSearch::getMonsNo( gfl2::str::StrBuf *pMonsName, NetAppLib::Message::MessageUtility* pMessageUtility )
{
  int result = 0;

  if( pMessageUtility )
  {
    for( u32 monsNo = 1; monsNo <= MONSNO_END; ++monsNo )
    {
      gfl2::str::StrBuf& monsName = pMessageUtility->GetString( GARC_message_monsname_DAT, monsNo );

      if( NetAppLib::Util::NetAppLatinumStringTool::CompareBufferNoCaseNoAccent( pMonsName, &monsName ) )
      {
        result = static_cast<int>( monsNo );

        break;
      }
    }
  }

  return result;
}


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
