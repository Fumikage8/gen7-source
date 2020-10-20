//======================================================================
/**
 * @file	Object_Chara.h
 * @brief	オブジェクト キャラモデル
 * @author	ariizumi
 * @data	15/03/16
 */
//======================================================================
#if !defined( __OBJECT_CHARA_H__ )
#define __OBJECT_CHARA_H__
#pragma once


#include <fs/include/gfl2_Fs.h>
#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

#include <DemoLib/Object/include/Object_GfMdl.h>

GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( model )
    class CharaModel;
    class CharaModelFactory;
    class DressUpModelResourceManager;
    struct DressUpParam;
    class DressUpModel;
  GFL_NAMESPACE_END( mdoel )
GFL_NAMESPACE_END( poke_3d )


GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

enum
{
  DRESSUP_ANI_NONE = -1,
  DRESSUP_ANI_MALE_BASE = 0,
  DRESSUP_ANI_FEMALE_BASE,
  DRESSUP_ANI_MALE_HAIR_MEDIUM,
  DRESSUP_ANI_FEMALE_HAIR_MEDIUM,
  DRESSUP_ANI_FEMALE_HAIR_LONG,
  DRESSUP_ANI_FEMALE_HAIR_SHORT,
  DRESSUP_ANI_FEMALE_HAIR_BOB,
  DRESSUP_ANI_FEMALE_HAIR_MAKI,
  DRESSUP_ANI_FEMALE_HAIR_MATOME,
  DRESSUP_ANI_FEMALE_HAIR_TWIN,
  DRESSUP_ANI_FEMALE_HAT_DHAT,
  DRESSUP_ANI_FEMALE_BOTTOMS_FLARE,
  DRESSUP_ANI_FEMALE_TOPS_DCUT,  
  DRESSUP_ANI_MALE_FACE,
  DRESSUP_ANI_FEMALE_FACE,
  DRESSUP_ANI_FEMALE_HAT_RESORT,
  //momiji追加
  DRESSUP_ANI_FEMALE_HAIR_MMJ_SPONY,
  DRESSUP_ANI_FEMALE_HAT_MMJ_DHAT,
  MAX_DRESSUP_ANI_TYPE,
};

enum
{
  DRESSUP_ANISLOT_BASE = 0,
  DRESSUP_ANISLOT_HAIR,
  DRESSUP_ANISLOT_HAT,
  DRESSUP_ANISLOT_TOP,
  DRESSUP_ANISLOT_BOTTOM,
  DRESSUP_ANISLOT_FACE,
  MAX_DRESSUP_ANISLOT,
};

class ObjectRenderPipeLine;

class Object_Chara :public Object_GfMdl
{
public:
  static const u32 INVALID_MODEL_ID = 0xFFFFFFFF;
  struct LOAD_PARAM
  {
    u32 charaId;
    //値を入れると着せ替えモデル
    poke_3d::model::DressUpParam *dressParam;
    //
    u32 attachModelId;
    char attachJoint[32];

    LOAD_PARAM()
      :charaId(INVALID_MODEL_ID)
      ,dressParam(NULL)
      ,attachModelId(INVALID_MODEL_ID)
    {
    }
  };
  //コンストラクタ
  Object_Chara(ObjectRenderPipeLine *pipe,poke_3d::model::CharaModelFactory *charaFactory,poke_3d::model::DressUpModelResourceManager *dressUpResourceManager);
  //デストラクタ
  virtual ~Object_Chara();

  //モデル生成
  //※ キャラに関してはFactoryがリソースを握っているので、同じキャラを出す場合２回のLoad/Setupは不要。
  //   その場合Object側でLoadをしないでCreateNoLoadしてください。
  virtual void StartLoad(const LOAD_PARAM &param);
  virtual bool IsFinishLoad(void);
  virtual void Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator);
  virtual void CreateNoLoad(const LOAD_PARAM &param,gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator);

  virtual void Delete(void);
  void FreeDressUpAnimationNodes(void);
  void ClearDressUpAnimation(void);

  //ObjectRenderPipeLine経由で追加する
  virtual void AddRenderPath(int opt = 0);
  virtual void RemoveRenderPath(void);
  virtual bool IsAddRenderPath(void){return mIsAddRenderPath;}
  //継承
  virtual void SetVisible(const bool flg);

  virtual void ChangeAnimation( u32 animeIdx );
  //再生中アニメIndex(リソース渡し再生時は前のアニメIndexになります)
  u32 GetAnimationIndex(void){return mAnimeIdx;}
  //リソース渡し再生(全力技・デモ等外部でモーションを持つ場合)
  //戻す際は普通にChangeAnimationしてください。
  virtual void ChangeAnimationResource( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,int aniSlot=0 );
  
  virtual void ChangeAnimationResourceDressUp( void *buf, gfl2::gfx::IGLAllocator *allocator  );
  void ChangeAnimationResourceDressUp_Battle( void *buf, gfl2::gfx::IGLAllocator *allocator  );
  void ChangeAnimationResourceDressUp_Field( void *buf, gfl2::gfx::IGLAllocator *allocator  );
  
  void SetReserveAnimation( int type ){mReserveAnime = type;}
  void SetAutoResetAniation( bool flg ){mIsAutoResetAnime = flg;}

  virtual void SetAnimationSpeed(f32 speed,int aniSlot=0 );
  //継承

  bool IsMale(void);

  bool IsMale_Field(void);
  bool IsMale_Battle(void);

  bool IsFemale(void);

  bool IsFemale_Field(void);
  bool IsFemale_Battle(void);

  bool IsBattleModel(void) { return(mIsBattleModel); }
  bool IsFieldModel(void) { return(!mIsBattleModel); }

  void SetIsBattleModel(void) { mIsBattleModel=true; }
  void SetIsFieldModel(void) { mIsBattleModel=false; }

  int DressUpBattle_HairType(void);
  int DressUpBattle_TopType(void);
  int DressUpBattle_BottomType(void);
  int DressUpBattle_HatType(void);

  int DressUpField_HairType(void);
  int DressUpField_TopType(void);
  int DressUpField_BottomType(void);
  int DressUpField_HatType(void);

  void SetConstantColor( int contantColorIndex , const gfl2::math::Vector4 &col);
  poke_3d::model::DressUpModel* GetDressUpModel(void){return mDressModel;}
  poke_3d::model::CharaModel* GetCharaModel(void){return mCharaModel;}
  poke_3d::model::CharaModel* GetAttachModel(void){return mAttachModel;}


  void SetDressUpResouceManager(poke_3d::model::DressUpModelResourceManager *mng){mDressUpModelResourceManager = mng;}
protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);
  //こっちはUpdateCoreの後に呼ばれる。座標反映後の更新はこちらで。
  virtual void UpdateAlwaysAfterCore(int step=1);

  poke_3d::model::CharaModelFactory *mCharaFactory;
  poke_3d::model::DressUpModelResourceManager *mDressUpModelResourceManager;

  u32 mCharaId;
  poke_3d::model::DressUpParam *mDressParam;

  bool mIsLoadData;

  poke_3d::model::DressUpModel *mDressModel;
  poke_3d::model::CharaModel   *mCharaModel;

  u32 mAttachModelId;
  char mAttachJoint[32];
  poke_3d::model::CharaModel   *mAttachModel;

  int mReserveAnime;
  bool mIsAutoResetAnime;
  bool mIsBattleModel;

  u32 mAnimeIdx;

  gfl2::renderingengine::scenegraph::resource::ResourceNode *anmResNodeDressUp[MAX_DRESSUP_ANISLOT];
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_CHARA_H__