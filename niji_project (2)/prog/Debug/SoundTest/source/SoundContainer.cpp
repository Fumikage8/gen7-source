//======================================================================
/**
 * @file SoundContainer.cpp
 * @date 2015/10/16 15:15:18
 * @author saito_nozomu
 * @brief サウンドコンテナ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <fs/include/gfl2_Fs.h>
#include <fs/include/gfl2_FsPcUtil.h>
#include <util/include/gfl2_std_string.h>
#include "Debug/SoundTest/include/SoundContainer.h"

#include <niji_conv_header/poke_lib/monsno_def.h>
#include "PokeTool/include/PokeToolGraNo.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

#define POKE_NUM (1000)


/**
 * @brief モンスターナンバー（図鑑ナンバー）→開発ナンバー（グラフィックナンバー）
 * @param monsno  モンスターナンバー図鑑ナンバー
 * @return 開発ナンバー（グラフィックナンバー）
 */
 u16 MonsNo2GraNo( u16 monsno )
{
	return PokeTool::MonsNo2GraNo(monsno);
}

/**
 * @brief 開発ナンバー→モンスターナンバー（図鑑ナンバー）
 * @param 開発ナンバー（グラフィックナンバー）
 * @return （図鑑ナンバー）
 */
u16 GraNo2MonsNo( u16 grano )
{
  // 少しだけ高速化
  if(grano > MONSNO_END / 2)
  {
    // ある程度大きい値を指定したということは、開発Noはある程度最後のほう
    // なので後ろから検索
    for(int i = MONSNO_END; i > 0 ; --i)
    {
      if(grano == PokeTool::MonsNo2GraNo(i)){return i;}
    }
  }
  else
  {
    for(int i = 0; i < MONSNO_END; ++i)
    {
      if(grano == PokeTool::MonsNo2GraNo(i)){return i;}
    }
  }

  NOZOMU_PRINT( "%d:存在しない開発Noです\n",grano );
  return 0;
}

/**
 * @brief 初期化関数
 * @param heap  ヒープ
 */
void SoundContainer::Initialize(gfl2::heap::HeapBase* heap)
{
	m_list = GFL_NEW_ARRAY(heap) List[CATEGORY_MAX];
	m_list_pv = GFL_NEW_ARRAY(heap) List[POKE_NUM];
	m_list_pv_zukan = GFL_NEW_ARRAY(heap) List[POKE_NUM];
}

/**
 * @brief デストラクタ
 */
SoundContainer::~SoundContainer()
{
	for(int i = 0; i < POKE_NUM; ++i)
	{
		for(List::Iterator it = m_list_pv[i].begin(); it != m_list_pv[i].end();)
		{
		  SoundInfo* info = &*it;
		  it = m_list_pv[i].erase(it);
			GFL_DELETE info;
		}
		m_list_pv[i].Clear();
	}
	for(int i = 0; i < POKE_NUM; ++i)
	{
		for(List::Iterator it = m_list_pv_zukan[i].begin(); it != m_list_pv_zukan[i].end();)
		{
		  SoundInfo* info = &*it;
		  it = m_list_pv_zukan[i].erase(it);
			GFL_DELETE info;
		}
		m_list_pv_zukan[i].Clear();
	}
	for(int i = 0; i < CATEGORY_MAX; ++i)
	{
		for(List::Iterator it = m_list[i].begin(); it != m_list[i].end();)
		{
		  SoundInfo* info = &*it;
		  it = m_list[i].erase(it);
			GFL_DELETE info;
		}
		m_list[i].Clear();
	}

	GFL_DELETE_ARRAY(m_list);
	GFL_DELETE_ARRAY(m_list_pv);
	GFL_DELETE_ARRAY(m_list_pv_zukan);
}

#if 0
void SoundContainer::ReLoad(gfl2::heap::HeapBase* heap, const wchar_t* path_sound, const wchar_t* path_voice)
{
	for(int i = 0; i < POKE_NUM; ++i)
	{
		for(List::Iterator it = m_list_pv[i].begin(); it != m_list_pv[i].end();)
		{
		  SoundInfo* info = &*it;
		  it = m_list_pv[i].erase(it);
			GFL_DELETE info;
		}
		m_list_pv[i].Clear();
	}
	for(int i = 0; i < POKE_NUM; ++i)
	{
		for(List::Iterator it = m_list_pv_zukan[i].begin(); it != m_list_pv_zukan[i].end();)
		{
		  SoundInfo* info = &*it;
		  it = m_list_pv_zukan[i].erase(it);
			GFL_DELETE info;
		}
		m_list_pv_zukan[i].Clear();
	}
	for(int i = 0; i < CATEGORY_MAX; ++i)
	{
		for(List::Iterator it = m_list[i].begin(); it != m_list[i].end();)
		{
		  SoundInfo* info = &*it;
		  it = m_list[i].erase(it);
			GFL_DELETE info;
		}
		m_list[i].Clear();
	}
	this->Load(heap, path_sound, path_voice);
}
#endif

/**
 * @brief リストテキストデータロード関数
 * @param heap ヒープ
 * @param path_sound　音リストテキストパス
 * @param path_voice　鳴き声リストテキストパス
 */
void SoundContainer::Load(gfl2::heap::HeapBase* heap, const char* path_sound, const char* path_voice)
{
	this->_Load(heap, path_sound);
	this->_Load(heap, path_voice);
}

/**
 * @brief リストテキストデータロード関数
 * @param heap ヒープ
 * @param path　テキストパス
 */
void SoundContainer::_Load(gfl2::heap::HeapBase* heap, const char* path)
{
#if defined(GF_PLATFORM_CTR)
  nn::fs::FileReader file( path );
  u32 fileSize = file.GetSize();
  void *pData = GflHeapAllocMemoryLow(heap, fileSize);
  
  file.Read(pData, fileSize);
#else
  size_t size;
  gfl2::fs::PcUtil::GetFileSize(&size, path);
  void *pData = GflHeapAllocMemoryLow(heap, size);
  gfl2::std::MemClear( pData, size );
  gfl2::fs::Result res = gfl2::fs::PcUtil::ReadFile(NULL, pData, size, path);
#endif
  char* token, *next_token;
  char *dat = (char*)pData;
#if defined(GF_PLATFORM_WIN32)
  token = strtok_s(dat, ",", &next_token);
	if(token != NULL)
	{
		while(token != NULL)
		{
			// ここでinfo.idをセット
			//GFL_PRINT("%s\n", token);
      
			long tmp_id;
			sscanf_s(token, "%ld", &tmp_id);

			int msg_index;
			token = strtok_s(NULL, ",", &next_token);
			if(token == NULL){/*GFL_ASSERT(0);*/ break;}
			sscanf_s(token, "%d", &msg_index);
			
			int category;
			token = strtok_s(NULL, ",", &next_token);
			if(token == NULL){/*GFL_ASSERT(0);*/ break;}
			sscanf_s(token, "%d", &category);
			
			long parent;
			token = strtok_s(NULL, ",", &next_token);
			if(token == NULL){/*GFL_ASSERT(0);*/ break;}
			sscanf_s(token, "%ld", &parent);
			
			SoundInfo* info = GFL_NEW(heap) SoundInfo();
			info->id = tmp_id;
			info->msg_index = msg_index;
			info->parent = parent;
			info->category = (Category)category;
			
			// タイプがPVのものは、種類別にリストに登録
			if(category == CATEGORY_PV)
			{
				if(parent == -1){GFL_ASSERT(0);}
				GFL_ASSERT(parent < POKE_NUM);
				m_list_pv[parent].push_back(info);
				if(parent > m_max_poke_number){m_max_poke_number = parent;}
				info->csid = parent;
			}
			else
			{
				if(category >= CATEGORY_MAX || category < 0){GFL_PRINT("category=%d\n",category);GFL_ASSERT(0);}
				else
				{
					m_list[category].push_back(info);
					info->csid = m_list[category].size();
          GFL_PRINT("push_back:%d\n", info->id);
				}
			}
			
			token = strtok_s(NULL, ",", &next_token);
			
		}
	}
	else
	{
		GFL_ASSERT(0);
	}
#else
  token = strtok(dat, ",");
	if(token != NULL)
	{
		while(token != NULL)
		{
			// ここでinfo.idをセット
			//GFL_PRINT("%s\n", token);
      
			long tmp_id;
			sscanf(token, "%ld", &tmp_id);

			int msg_index;
			token = strtok(NULL, ",");
			if(token == NULL){/*GFL_ASSERT(0);*/ break;}
			sscanf(token, "%d", &msg_index);
			
			int category;
			token = strtok(NULL, ",");
			if(token == NULL){/*GFL_ASSERT(0);*/ break;}
			sscanf(token, "%d", &category);
			
			long parent;
			token = strtok(NULL, ",");
			if(token == NULL){/*GFL_ASSERT(0);*/ break;}
			sscanf(token, "%ld", &parent);
			
			SoundInfo* info = GFL_NEW(heap) SoundInfo();
			info->id = tmp_id;
			info->msg_index = msg_index;
			info->parent = parent;
			info->category = (Category)category;
			
			// タイプがPVのものは、種類別にリストに登録
			if(category == CATEGORY_PV)
			{
				if(parent == -1){GFL_ASSERT(0);}
				GFL_ASSERT(parent < POKE_NUM);
				m_list_pv[parent].push_back(info);
				if(parent > m_max_poke_number){m_max_poke_number = parent;}
				info->csid = parent;
			}
			else
			{
				if(category >= CATEGORY_MAX || category < 0){GFL_PRINT("category=%d\n",category);GFL_ASSERT(0);}
				else
				{
					m_list[category].push_back(info);
					info->csid = m_list[category].size();
          GFL_PRINT("push_back:%d\n", info->id);
				}
			}
			
			token = strtok(NULL, ",");
			
		}
	}
	else
	{
		GFL_ASSERT(0);
	}
#endif


  GflHeapFreeMemory(pData);

  // 鳴き声図鑑順登録
	for(int i = 0; i < POKE_NUM; ++i)
	{
		for(Iterator it = m_list_pv[i].begin(); it != m_list_pv[i].end(); ++it)
		{
			SoundInfo* info = GFL_NEW(heap) SoundInfo();
			info->id = it->id;
			info->msg_index = it->msg_index;
			info->parent = it->parent;
			info->category = it->category;
			info->csid = it->csid;
			m_list_pv_zukan[GraNo2MonsNo(i)].push_back(info);
		}
	}

}


GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif //PM_DEBUG
