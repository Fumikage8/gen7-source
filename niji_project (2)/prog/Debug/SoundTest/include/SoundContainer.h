//======================================================================
/**
 * @file SoundContainer.h
 * @date 2015/10/16 15:06:10
 * @author saito_nozomu
 * @brief サウンドコンテナ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
#if !defined __SOUND_CONTAINER_H_INCLUDED__
#define __SOUND_CONTAINER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <nw/ut.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

extern u16 MonsNo2GraNo( u16 monsno );
extern u16 GraNo2MonsNo( u16 grano );

/**
 * @enum Category
 * カテゴリ定義
 */
enum Category
{
	CATEGORY_BGM,
	CATEGORY_ME,
	CATEGORY_SE,
	CATEGORY_PV,
	CATEGORY_PIKA,
	
	CATEGORY_MAX,
};

/**
 * @struct SoundInfo
 * サウンド情報
 */
struct SoundInfo
{
	long id;
	int csid;
	int msg_index;
	long parent;
	Category category;
	
	nw::ut::LinkListNode node;
};

/**
 * @class SoundContainer
 * @brief サウンドコンテナクラス
 */
class SoundContainer
{
  GFL_FORBID_COPY_AND_ASSIGN( SoundContainer );
public:
	typedef nw::ut::LinkList<SoundInfo, offsetof(SoundInfo,node)> List;
  typedef List::iterator Iterator;
	
public:
  /**
   * @brief コンストラクタ
   */
  SoundContainer(){}
  
  /**
   * @brief デストラクタ
   */
	~SoundContainer();

  /**
   * @brief 初期化関数
   * @param heap  ヒープ
   */
	void Initialize(gfl2::heap::HeapBase* heap);
	
public:
  /**
   * @brief リストテキストデータロード関数
   * @param heap ヒープ
   * @param path_sound　音リストテキストパス
   * @param path_voice　鳴き声リストテキストパス
   */
  void Load(gfl2::heap::HeapBase* heap, const char* path_sound, const char* path_voice);
	//void ReLoad(gfl2::heap::HeapBase* heap, const wchar_t* path_sound, const wchar_t* path_voice);
  /**
   * @brief ポケモン鳴き声開始イテレーター
   * @param poke_number 対象ナンバー
   * @param zukan　図鑑ナンバー順リストを使用するか？trueで使用
   * @return Iterator 該当するイテレーター
   */
	Iterator PVBegin(int poke_number, bool zukan){return zukan ? m_list_pv_zukan[poke_number].begin() : m_list_pv[poke_number].begin();}

  /**
   * @brief ポケモン鳴き声終了イテレーター
   * @param poke_number 対象ナンバー
   * @param zukan　図鑑ナンバー順リストを使用するか？trueで使用
   * @return Iterator 該当するイテレーター
   */
  Iterator PVEnd(int poke_number, bool zukan){return zukan ? m_list_pv_zukan[poke_number].end() : m_list_pv[poke_number].end();}
	//const SoundInfo& PVFront(int poke_number, bool zukan){return  zukan ? m_list_pv_zukan[poke_number].front() : m_list_pv[poke_number].front();}
	//const SoundInfo& PVBack(int poke_number, bool zukan){return zukan ? m_list_pv_zukan[poke_number].back() : m_list_pv[poke_number].back();}
	
  /**
   * @brief ポケモン鳴き声数を取得する
   * @param poke_number 対象ナンバー
   * @param zukan　図鑑ナンバー順リストを使用するか？trueで使用
   * @return int ポケモンが保有する鳴き声の数
   */
  int PVSize(int poke_number, bool zukan){return zukan ? m_list_pv_zukan[poke_number].size() : m_list_pv[poke_number].size();}
	
  /**
   * @brief 開始イテレーター
   * @param type   対象カテゴリ
   * @return Iterator 該当するイテレーター
   */
	Iterator Begin(int type){return m_list[type].begin();}

  /**
   * @brief 終了イテレーター
   * @param type   対象カテゴリ
   * @return Iterator 該当するイテレーター
   */
	Iterator End(int type){return m_list[type].end();}
	
  /**
   * @brief 対象リストの項目数を取得する
   * @param type 対象カテゴリ
   * @return int ポケモンが保有する鳴き声の数
   */
  int Size(int type){return m_list[type].size();}
	
	//const SoundInfo& Front(int type){return m_list[type].front();}
	//const SoundInfo& Back(int type){return m_list[type].back();}
	
  /**
   * @brief ポケモン数を取得する
   * @return int ポケモン数
   */
	int GetMaxPokeNumber(){return m_max_poke_number;}
	
private:
  /**
   * @brief リストテキストデータロード関数
   * @param heap ヒープ
   * @param path　テキストパス
   */
  void _Load(gfl2::heap::HeapBase* heap, const char* path);
	
private:
	List* m_list;             //! サウンドリスト
	List* m_list_pv;          //! 鳴き声リスト開発番号順
	List* m_list_pv_zukan;    //! 鳴き声リスト図鑑順
	int m_max_poke_number;    //! ポケモン数
	
};

  

GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif // __SOUND_CONTAINER_H_INCLUDED__
#endif //PM_DEBUG