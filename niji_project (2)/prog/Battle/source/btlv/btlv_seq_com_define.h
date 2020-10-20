//======================================================================
/**
 * @file   btlv_seq_com_define.h
 * @author SquenceConverter.exe
 * @data   2017/02/14 15:48
*/
//======================================================================
#ifndef __BTLV_SEQ_COM_DEFINE_H__
#define __BTLV_SEQ_COM_DEFINE_H__

//-------------------------------------------
//Sequence内Define定義
//-------------------------------------------
#define SEQ_DEF_POS_ATK (0)  //攻撃側
#define SEQ_DEF_POS_DEF (1)  //防御側
#define SEQ_DEF_POS_DEFTRG (2)  //防御側中心キャラ
#define SEQ_DEF_POS_GROUP (0)  //no_comment
#define SEQ_DEF_SPPOS_SELF_FIELD (0)  //攻撃側場
#define SEQ_DEF_SPPOS_ENEMY_FIELD (1)  //防御側場
#define SEQ_DEF_SPPOS_ENEMY_CENTER (2)  //防御中心
#define SEQ_DEF_SPPOS_GROUP (0)  //no_comment
#define SEQ_DEF_NODE_ORIGIN (0)  //原点
#define SEQ_DEF_NODE_BUTURI (1)  //接触攻撃
#define SEQ_DEF_NODE_SHOOT (2)  //非接触攻撃
#define SEQ_DEF_NODE_WAIST (3)  //Waist(腰)
#define SEQ_DEF_NODE_HEAD (4)  //頭上
#define SEQ_DEF_NODE_FRONT (5)  //正面
#define SEQ_DEF_NODE_FACE (6)  //頭中心
#define SEQ_DEF_NODE_EYE (7)  //目中心
#define SEQ_DEF_NODE_MOUTH (8)  //口
#define SEQ_DEF_NODE_HORN (9)  //角
#define SEQ_DEF_NODE_TAIL (10)  //尻尾
#define SEQ_DEF_NODE_HAND1 (11)  //手１
#define SEQ_DEF_NODE_HAND2 (12)  //手２
#define SEQ_DEF_NODE_FOOT1 (13)  //足１
#define SEQ_DEF_NODE_FOOT2 (14)  //足２
#define SEQ_DEF_NODE_CENTER (15)  //中心
#define SEQ_DEF_NODE_BASE (16)  //オボン(非表示でも使用可・サイズ補正なし)
#define SEQ_DEF_NODE_INTRO (50)  //導入演出用登場位置
#define SEQ_DEF_NODE_GROUP (0)  //no_comment
#define MOTION_WAIT (0)  //待機
#define MOTION_APPEAL (7)  //アピール
#define MOTION_DAMAGE (16)  //ダメージ
#define MOTION_DOWN (17)  //ダウン
#define MOTION_LAND_A (3)  //登場(飛び出し)
#define MOTION_LAND_B (4)  //登場(落下ループ)
#define MOTION_LAND_C (5)  //登場(着地)
#define MOTION_LAND (6)  //登場
#define MOTION_ATK_BUTSURI1 (8)  //物理１
#define MOTION_ATK_BUTSURI2 (9)  //物理２
#define MOTION_ATK_BUTSURI3 (10)  //物理３
#define MOTION_ATK_BUTSURI4 (11)  //物理４
#define MOTION_ATK_TOKUSHU1 (12)  //特殊１
#define MOTION_ATK_TOKUSHU2 (13)  //特殊２
#define MOTION_ATK_TOKUSHU3 (14)  //特殊３
#define MOTION_ATK_TOKUSHU4 (15)  //特殊４
#define MOTION_ATK_WAIT_B (1)  //待機B
#define MOTION_ATK_WAIT_C (2)  //待機C
#define POKE_MOTION_ATK_BUTSURI (30)  //攻撃：物理
#define POKE_MOTION_ATK_TOKUSHU (31)  //攻撃：特殊
#define POKE_MOTION_ATK_BODYBLOW (32)  //攻撃：体当たり
#define POKE_MOTION_ATK_PUNCH (33)  //攻撃：パンチ
#define POKE_MOTION_ATK_KICK (34)  //攻撃：キック
#define POKE_MOTION_ATK_TAIL (35)  //攻撃：尻尾
#define POKE_MOTION_ATK_BITE (36)  //攻撃：噛む
#define POKE_MOTION_ATK_PECK (37)  //攻撃：つつく
#define POKE_MOTION_ATK_RADIAL (38)  //攻撃：放射
#define POKE_MOTION_ATK_CRY (39)  //攻撃：鳴く
#define POKE_MOTION_ATK_DUST (40)  //攻撃：こな
#define POKE_MOTION_ATK_SHOOT (41)  //攻撃：発射
#define POKE_MOTION_ATK_GUARD (42)  //攻撃：守り
#define SEQ_DEF_MOTION_GROUP (0)  //no_comment
#define SEQ_DEF_ATK_MOT_GROUP (0)  //no_comment
#define POKE_EYE_OPEN (0)  //開き
#define POKE_EYE_HALF (1)  //半目
#define POKE_EYE_CLOSE (2)  //閉じ
#define POKE_EYE_CLOSE2 (3)  //閉じ２
#define POKE_EYE_ANGRY (4)  //怒り
#define POKE_EYE_SMILE (5)  //喜び
#define POKE_EYE_SAD (6)  //悲しみ
#define POKE_EYE_OTHER (7)  //その他
#define POKE_EYE_RESET (31)  //リセット
#define SEQ_DEF_POKE_EYE_GROUP (0)  //no_comment
#define SEQ_DEF_MOVETYPE_LINER (0)  //等速移動
#define SEQ_DEF_MOVETYPE_FAST (1)  //最初速く
#define SEQ_DEF_MOVETYPE_SLOW (2)  //最初遅く
#define SEQ_DEF_MOVETYPE_SIN (3)  //遅い→早い→遅い
#define SEQ_DEF_MOVETYPE_GROUP (0)  //no_comment
#define SEQ_DEF_DRAWTYPE_NORAML (0)  //通常描画
#define SEQ_DEF_DRAWTYPE_NO_EDGE (1)  //エッジ無し
#define SEQ_DEF_DRAWTYPE_SCREEN (2)  //画面貼り付け
#define SEQ_DEF_DRAWTYPE_BEFORE (3)  //前描画
#define SEQ_DEF_DRAWTYPE_AFTER (4)  //後描画
#define SEQ_DEF_DRAWTYPE_ORTHO (5)  //正射影
#define SEQ_DEF_DRAWTYPE_AFTER_ORTHO (6)  //正射影(後)
#define SEQ_DEF_DRAWTYPE_BALL (7)  //ボール専用(ポケ後エッジ前)
#define SEQ_DEF_DRAWTYPE_BG (8)  //背景用
#define SEQ_DEF_DRAWTYPE_AFTER_LYT (9)  //画面貼り付け(レイアウト後)
#define SEQ_DEF_DRAWTYPE_DISTORTION (10)  //歪み
#define SEQ_DEF_DRAWTYPE_SCREEN_BEFORE (11)  //画面貼り付け(前描画)
#define SEQ_DEF_DRAWTYPE_BEFORE_EDGE (12)  //前描画+エッジ
#define SEQ_DEF_DRAWTYPE_BG_AFTER (13)  //背景用(後)
#define SEQ_DEF_DRAWTYPE_GROUP (0)  //no_comment
#define SEQ_DEF_ROTATE_ORDER_DEF (0)  //デフォルト
#define SEQ_DEF_ROTATE_ORDER_XYZ (1)  //XYZ
#define SEQ_DEF_ROTATE_ORDER_XZY (2)  //XZY
#define SEQ_DEF_ROTATE_ORDER_YXZ (3)  //YXZ
#define SEQ_DEF_ROTATE_ORDER_YZX (4)  //YZX
#define SEQ_DEF_ROTATE_ORDER_ZXY (5)  //ZXY
#define SEQ_DEF_ROTATE_ORDER_ZYX (6)  //ZYX
#define SEQ_DEF_ROTATE_ORDER_GROUP (0)  //no_comment
#define SEQ_DEF_EFF_DRAWTYPE_NORAML (0)  //通常描画
#define SEQ_DEF_EFF_DRAWTYPE_SCREEN (1)  //画面貼り付け
#define SEQ_DEF_EFF_DRAWTYPE_BEFORE (2)  //前描画
#define SEQ_DEF_EFF_DRAWTYPE_LOW (3)  //通常描画(低)
#define SEQ_DEF_EFF_DRAWTYPE_HIGHT (4)  //通常描画(高)
#define SEQ_DEF_EFF_DRAWTYPE_GROUP (0)  //no_comment
#define SEQ_DEF_FADE_UPPER (0)  //上画面
#define SEQ_DEF_FADE_LOWER (1)  //下画面
#define SEQ_DEF_FADE_BOTH (2)  //両画面
#define SEQ_DEF_FADE_WHITE (0)  //白フェード
#define SEQ_DEF_FADE_BLACK (1)  //黒フェード
#define SEQ_DEF_AXIS_X (0)  //X軸
#define SEQ_DEF_AXIS_Y (1)  //Y軸
#define SEQ_DEF_AXIS_Z (2)  //Z軸
#define SEQ_DEF_AXIS_X_ROT (3)  //X軸(Objの回転加味)
#define SEQ_DEF_AXIS_Z_ROT (4)  //Z軸(Objの回転加味)
#define SEQ_DEF_AXIS_GROUP (0)  //no_comment
#define SEQ_DEF_TRAINER_SELF1 (0)  //プレイヤー１
#define SEQ_DEF_TRAINER_ENEMY1 (1)  //敵１
#define SEQ_DEF_TRAINER_SELF2 (2)  //プレイヤー２
#define SEQ_DEF_TRAINER_ENEMY2 (3)  //敵２
#define SEQ_DEF_TRAINER_SEQ_TARGET1 (4)  //エフェクト対象1
#define SEQ_DEF_TRAINER_SEQ_TARGET2 (5)  //エフェクト対象2
#define SEQ_DEF_TRAINER_GROUP (0)  //no_comment
#define SEQ_DEF_TR_MOTION_IN (0)  //登場
#define SEQ_DEF_TR_MOTION_BALL (1)  //ボール投げ
#define SEQ_DEF_TR_MOTION_ORDER (2)  //指示
#define SEQ_DEF_TR_MOTION_LOSE (3)  //負け
#define SEQ_DEF_TR_MOTION_WAIT1 (4)  //待機１
#define SEQ_DEF_TR_MOTION_WAIT2 (5)  //待機２
#define SEQ_DEF_TR_MOTION_IN2 (6)  //登場２
#define SEQ_DEF_TR_MOTION_BALL2 (7)  //ボール投げ２
#define SEQ_DEF_TR_MOTION_LOSE2 (8)  //負け２
#define SEQ_DEF_TR_MOTION_BALL3 (9)  //ボール投げ３(_ba021_ball03)
#define SEQ_DEF_TR_MOTION_BALL_M (10)  //マルチ投げ
#define SEQ_DEF_TR_MOTION_IN3 (11)  //登場３
#define SEQ_DEF_TR_MOTION_BALL3_2 (12)  //ボール投げ３(_ba015_ball03)
#define SEQ_DEF_TR_MOTION_LOSE3 (13)  //負け３
#define SEQ_DEF_TR_MOTION_ZWAZA (14)  //ククイ全力モーション
#define SEQ_DEF_TR_MOTION_MULTI_0 (15)  //PC用マルチ前半(PG分岐のためリストに入れない)
#define SEQ_DEF_TR_MOTION_IN4 (16)  //登場４
#define SEQ_DEF_TR_MOTION_BALL4 (17)  //ボール投げ４
#define SEQ_DEF_TR_MOTION_LOSE4 (18)  //負け４
#define SEQ_DEF_TR_MOTION_POSE (99)  //ポーズ(モーション選択分岐)
#define SEQ_DEF_TR_MOTION_BALL_M0 (98)  //マルチ投げ(前半)
#define SEQ_DEF_TR_MOTION_GROUP (0)  //no_comment
#define SEQ_DEF_TR_TEX_MAIN (0)  //メイン
#define SEQ_DEF_TR_TEX_SHADOW (1)  //影
#define SEQ_DEF_TR_TEX_SUB (2)  //サブ(加工)
#define SEQ_DEF_TR_TEX_GROUP (0)  //no_comment
#define SEQ_DEF_TR_ENV_TOUJYO (0)  //登場
#define SEQ_DEF_TR_ENV_LOSE (1)  //敗北
#define SEQ_DEF_TR_ENV_GROUP (0)  //no_comment
#define SEQ_DEF_FOLLOW_PARTICLE (0)  //パーティクル
#define SEQ_DEF_FOLLOW_MODEL (1)  //モデル
#define SEQ_DEF_FOLLOW_GROUP (0)  //no_comment
#define SEQ_DEF_SEPAN_NONE (0)  //パン補正なし
#define SEQ_DEF_SEPAN_ATK (1)  //攻撃側
#define SEQ_DEF_SEPAN_DEF (2)  //防御側
#define SEQ_DEF_SEPAN_GROUP (0)  //no_comment
#define CLUSTER_SPAWN_POINT (0)  //点
#define CLUSTER_SPAWN_SPHERE (1)  //球
#define CLUSTER_SPAWN_CYLINDER1 (2)  //円柱(底面基準)
#define CLUSTER_SPAWN_CYLINDER2 (3)  //円柱(中央基準)
#define CLUSTER_SPAWN_BOX (4)  //箱
#define CLUSTER_SPAWN_GROUP (0)  //no_comment
#define CLUSTER_ORIGIN (0)  //原点
#define CLUSTER_AXIS_X (1)  //X軸
#define CLUSTER_AXIS_Y (2)  //Y軸
#define CLUSTER_AXIS_Z (3)  //Z軸
#define CLUSTER_PLANE_X (4)  //X面
#define CLUSTER_PLANE_Y (5)  //Y面
#define CLUSTER_PLANE_Z (6)  //Z面
#define CLUSTER_POS_GROUP (0)  //no_comment
#define CLUSTER_POS_GROUP_AXIS (0)  //no_comment
#define CLUSTER_POS_GROUP_PLANE (0)  //no_comment
#define CLUSTER_REFRECT_NONE (0)  //処理無し
#define CLUSTER_REFRECT_NORMAL (1)  //反射
#define CLUSTER_REFRECT_STOP (2)  //停止
#define CLUSTER_REFRECT_DEATH (3)  //死亡
#define CLUSTER_REFRECT_GROUP (0)  //no_comment
#define CLUSTER_CHILD_NONE (0)  //処理無し(親)
#define CLUSTER_CHILD_TIME (1)  //時間
#define CLUSTER_CHILD_REFRECT (2)  //反射時
#define CLUSTER_CHILD_DEATH (3)  //死亡時
#define CLUSTER_CHILD_GROUP (0)  //no_comment
#define SEQ_DEF_WAIT_KEY (0)  //キー待ち
#define SEQ_DEF_WAIT_MESSAGE (1)  //メッセージ待ち
#define SEQ_DEF_WAIT_LOAD (2)  //開始時読み込み待ち
#define SEQ_DEF_WAIT_LOAD_POKE (3)  //ポケモン読み込み待ち
#define SEQ_DEF_WAIT_LOAD_BG (4)  //背景読み込み待ち(フィールド技)
#define SEQ_DEF_WAIT_POKE_INTRO (5)  //ポケモン導入演出(着地、レア)
#define SEQ_DEF_WAIT_FADE (10)  //フェード待ち
#define SEQ_DEF_WAIT_DEMOFADE_SYNC (11)  //デモフェード同期
#define SEQ_DEF_WAIT_GROUP (0)  //no_comment
#define SEQ_DEF_WEATHER_NONE (0)  //リセット
#define SEQ_DEF_WEATHER_SHINE (1)  //晴れ
#define SEQ_DEF_WEATHER_RAIN (2)  //雨
#define SEQ_DEF_WEATHER_SNOW (3)  //雪
#define SEQ_DEF_WEATHER_SAND (4)  //砂嵐
#define SEQ_DEF_WEATHER_STORM (5)  //大雨
#define SEQ_DEF_WEATHER_DAY (6)  //大日照り
#define SEQ_DEF_WEATHER_TURBULENCE (7)  //乱気流
#define SEQ_DEF_WEATHER_GROUP (0)  //no_comment
#define SEQ_DEF_FWAZA_TYPE_NONE (0)  //なし(リセット)
#define SEQ_DEF_FWAZA_TYPE_GRASS (1)  //グラスフィールド
#define SEQ_DEF_FWAZA_TYPE_MIST (2)  //ミストフィールド
#define SEQ_DEF_FWAZA_TYPE_ELEC (3)  //エレキフィールド
#define SEQ_DEF_FWAZA_TYPE_PHYCO (4)  //サイコフィールド
#define SEQ_DEF_FWAZA_TYPE_GROUP (0)  //no_comment
#define ROTOM_EFFECT_WILD (0)  //野生開始演出
#define ROTOM_EFFECT_LOOP (1)  //ループ
#define ROTOM_EFFECT_CLOSE (99)  //閉じる
#define ROTOM_EFFECT_GROUP (0)  //no_comment

//-------------------------------------------
//Sequence内グループオプション定義
//-------------------------------------------
#define GRP_OPT_NONE (0)  //オプション無
#define GRP_OPT_SINGLE (10)  //シングル
#define GRP_OPT_DOUBLE (11)  //ダブル
#define GRP_OPT_ROYAL (12)  //ロイヤル
#define GRP_OPT_NOT_SINGLE (13)  //シングル以外
#define GRP_OPT_ATK_NEAR (20)  //攻撃側が手前
#define GRP_OPT_ATK_FAR (21)  //攻撃側が奥
#define GRP_OPT_DEF_NEAR (22)  //防御側が手前
#define GRP_OPT_DEF_FAR (23)  //防御側が奥
#define GRP_OPT_ATK_R_NEAR (30)  //攻撃側が手前(ロイヤル)
#define GRP_OPT_ATK_R_RIGHT (31)  //攻撃側が右(ロイヤル)
#define GRP_OPT_ATK_R_FAR (32)  //攻撃側が奥(ロイヤル)
#define GRP_OPT_ATK_R_LEFT (33)  //攻撃側が左(ロイヤル)
#define GRP_OPT_NOT_COMM (40)  //非通信
#define GRP_OPT_COMM (41)  //通信
#define GRP_OPT_TIME_DAY (50)  //時間帯(朝昼/晴れ)
#define GRP_OPT_TIME_EVENING (51)  //時間帯(夕方/晴れ)
#define GRP_OPT_TIME_NIGHT (52)  //時間帯(夜/晴れ)
#define GRP_OPT_TIME_DAY_CLOUD (53)  //時間帯(朝昼/曇り)
#define GRP_OPT_TIME_EVENING_CLOUD (54)  //時間帯(夕方/曇り)
#define GRP_OPT_TIME_NIGHT_CLOUD (55)  //時間帯(夜/曇り)
#define GRP_OPT_ATK_RARE (60)  //攻撃側が色違い
#define GRP_OPT_ATK_NO_RARE (61)  //攻撃側が色違いではない
#define GRP_OPT_DEF_CAPTURE_DEMO (80)  //捕獲チュートリアル
#define GRP_OPT_DEF_NOT_CAPTURE_DEMO (81)  //捕獲チュートリアル以外
#define GRP_OPT_DEF_WIN (90)  //Winのみ
#define GRP_OPT_DEF_CTR (91)  //CTRのみ


//SKIP使用時のコマンドサイズ配列

static const int btlv_seq_com_define_size_table[] =
{
   0,//LabelPokemon
  28,//PokemonMovePosition
  44,//PokemonMoveRelativePoke
   4,//PokemonMoveReset
   0,//PokemonMoveResetAll
  20,//PokemonScale
  24,//PokemonRotate
  16,//PokemonRotatePoke
  32,//PokemonRotateSpecialPos
   8,//PokemonRotateOrder
   8,//PokemonVisible
   4,//PokemonVisibleOther
   4,//PokemonVisibleAll
   8,//PokemonVisibleShadow
  12,//PokemonMotion
  16,//PokemonMotionResource
  28,//PokemonMotionResourceTiki
   8,//PokemonMotionStopResource
   8,//PokemonAttackMotion
   8,//PokemonSetMotionSpeed
   4,//PokemonIntroMotion
   8,//PokemonEdgeEnable
  36,//PokemonShaderCol
   8,//PokemonShaderFullPower
  76,//PokemonFollowMode
   4,//PokemonSpMoveReset
  28,//PokemonSpMoveShake
   4,//PokemonSetOriginScale
  12,//PokemonSetEnableFloat
   4,//PokemonRareEffect
  20,//PokemonLookAt
  12,//PokemonSetEye
   0,//LabelCamera
  36,//CameraMovePosition
  60,//CameraMoveRelativePoke
  44,//CameraMoveSpecialPos
   4,//CameraReset
  16,//CameraRotateTrg
  16,//CameraRotatePos
   8,//CameraRotatePosPoke
  16,//CameraShake
  44,//CameraAnimetionPoke
  24,//CameraAnimetionPosition
  72,//CameraAnimetionTrainer
  52,//CameraAnimetionDoubleIntro
   4,//CameraAnimetionEnd
   4,//CameraAnimetionRotate
  12,//CameraAnimetionScale
   4,//CameraAnimetionChangeSpeed
   0,//LabelParticle
  20,//ParticleCreate
   0,//ParticleStop
   0,//ParticleDelete
  16,//ParticleMovePosition
  44,//ParticleMoveRelativePoke
  24,//ParticleMoveSpecialPos
  36,//ParticleMoveRelativeModel
  16,//ParticleScale
  16,//ParticleRotate
  16,//ParticleRotatePoke
  16,//ParticleRotateOffset
  72,//ParticleFollowModel
  32,//ParticleFollowPoke
  76,//ParticleFollowTrainer
   4,//ParticleSetAutoRotate
   4,//ParticlCameraAutoRotate
   0,//ParticleSpMoveReset
  20,//ParticleSpMoveShake
   0,//LabelModel
  16,//ModelCreate
   8,//ModelCreateBall
   0,//ModelDelete
  16,//ModelMovePosition
  44,//ModelMoveRelativePoke
  24,//ModelMoveSpecialPos
  16,//ModelScale
  16,//ModelRotate
  16,//ModelRotatePoke
  16,//ModelRotateOffset
   4,//ModelVisible
  72,//ModelFollowModel
  32,//ModelFollowPoke
  84,//ModelFollowTrainer
  44,//ModelAttachTrainer
   4,//ModelSetAutoRotate
   4,//ModelAutoCameraRotate
  52,//ModelSetConstancColor
  20,//ModelLightDir
  12,//ModelSetAnimation
   4,//ModelAnimationStop
   8,//ModelSetAnimationSpeed
  40,//ModelReplaceTexture
  44,//ModelReplaceTextureTrainer
   0,//ModelSpMoveReset
  20,//ModelSpMoveShake
   0,//LabelSound
  28,//SoundPlaySe
   8,//SoundCtrlVolume
   8,//SoundCtrlPan
   8,//SoundCtrlPitch
   8,//SoundCtrlTempo
   4,//SoundPlayVoice
   8,//SoundPlayVoiceId
  24,//SE3D_SetParam
  24,//SE3D_Play
   4,//SE3D_Stop
  16,//SE3D_MovePosition
  24,//SE3D_MoveRelativePoke
  24,//SE3D_MoveSpecialPos
  20,//SE3D_FollowPoke
  20,//SE3D_FollowObj
  16,//SE3D_VoicePlay
  16,//SE3D_VoicePlayId
   4,//SoundBGMVolume
   8,//SoundZukanSE
   0,//LabelDisp
  12,//GaugeDisp
   4,//GaugeDispAll
   4,//GaugeDamage
   4,//BallBarDisp
   8,//HitBackFlg
   0,//LabelEffectSp
  20,//EffSpBackColFlg
  16,//EffSpBackColSet
   8,//EffDispBg
   4,//EffSpFadeIn
  16,//EffSpFadeOut
  12,//EffLightDirection
  20,//EffCharaLightDirection
  12,//EffCharaLightEnv
  12,//EffCharaLightFile
  20,//EffPokeLightDirection
   0,//LabelCluster
  80,//ClusterCreate
  72,//ClusterCreateEffect
  16,//ClusterMoveAbsolute
  44,//ClusterMoveRelativePoke
  24,//ClusterMoveSpecialPos
  16,//ClusterScale
  16,//ClusterRotate
  16,//ClusterRotatePoke
  24,//ClusterSpeedDiffuse
  24,//ClusterSpeedDir
  24,//ClusterAccelGravity
  28,//ClusterStartScale
  24,//ClusterStartRotate
  80,//ClusterAnimeScale
  80,//ClusterAnimeRotate
  20,//ClusterSetRefrect
   4,//ClusterSetSpawnTime
  12,//ClusterSetChild
   0,//LabelTrainer
  20,//TrainerMove
   8,//TrainerMoveReset
  20,//TrainerRotate
   8,//TrainerDisp
   4,//TrainerDispAll
   4,//TrainerDispOther
   8,//TrainerDispShadow
  12,//TrainerChangeMotion
  12,//TrainerChangeMotionResource
  12,//TrainerChangeMotionKisekae
   8,//TrainerChangeMotionSpeed
   8,//TrainerSetMotionFrame
   8,//TrainerEdgeEnable
   0,//LabelSpecial
   8,//MessageDispStd
   8,//MessageDispSet
   0,//MessageHide
   4,//SpecialWaitSequence
   0,//SpecialSyncDemoFade
   4,//SpecialChangeWeather
   8,//SpecialChangePokemon
   8,//SpecialTameVisible
   8,//SpecialMigawriVisible
   8,//SpecialMigawriVisibleZenryoku
  48,//SpecialChainAttakDefine
   4,//SpecialFieldEffectCreate
   8,//SpecialDeleteCamera
   4,//SpecialDeleteParticle
   4,//SpecialDeleteModel
   4,//SpecialDeleteModelAnime
   4,//SpecialDeleteKisekaeAnime
   4,//SpecialFesTrainerSetup
   4,//MessageDispSFes
   0,//LabelRotom
   4,//RotomStartEvent
   8,//RotomStartMessage
   0,//RotomDelete
   0,//TestLable
  20,//TestSpecialCommand
   0,//DummyLabel
};
#endif //__BTLV_SEQ_COM_DEFINE_H__
