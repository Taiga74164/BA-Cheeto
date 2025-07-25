﻿#pragma once

enum class TacticEntityType_Enum : int32_t
{
    None = 0x00000000,
    Student = 0x00000001,
    Minion = 0x00000002,
    Elite = 0x00000004,
    Champion = 0x00000008,
    Boss = 0x00000010,
    Obstacle = 0x00000020,
    Servant = 0x00000040,
    Vehicle = 0x00000080,
    Summoned = 0x00000100,
    Hallucination = 0x00000200,
    DestructibleProjectile = 0x00000400,
};

enum class SkillSlot_Enum : int32_t
{
    None = 0x00000000,
    NormalAttack01 = 0x00000001,
    NormalAttack02 = 0x00000002,
    NormalAttack03 = 0x00000003,
    NormalAttack04 = 0x00000004,
    NormalAttack05 = 0x00000005,
    NormalAttack06 = 0x00000006,
    NormalAttack07 = 0x00000007,
    NormalAttack08 = 0x00000008,
    NormalAttack09 = 0x00000009,
    NormalAttack10 = 0x0000000a,
    ExSkill01 = 0x0000000b,
    ExSkill02 = 0x0000000c,
    ExSkill03 = 0x0000000d,
    ExSkill04 = 0x0000000e,
    ExSkill05 = 0x0000000f,
    ExSkill06 = 0x00000010,
    ExSkill07 = 0x00000011,
    ExSkill08 = 0x00000012,
    ExSkill09 = 0x00000013,
    ExSkill10 = 0x00000014,
    Passive01 = 0x00000015,
    Passive02 = 0x00000016,
    Passive03 = 0x00000017,
    Passive04 = 0x00000018,
    Passive05 = 0x00000019,
    Passive06 = 0x0000001a,
    Passive07 = 0x0000001b,
    Passive08 = 0x0000001c,
    Passive09 = 0x0000001d,
    Passive10 = 0x0000001e,
    ExtraPassive01 = 0x0000001f,
    ExtraPassive02 = 0x00000020,
    ExtraPassive03 = 0x00000021,
    ExtraPassive04 = 0x00000022,
    ExtraPassive05 = 0x00000023,
    ExtraPassive06 = 0x00000024,
    ExtraPassive07 = 0x00000025,
    ExtraPassive08 = 0x00000026,
    ExtraPassive09 = 0x00000027,
    ExtraPassive10 = 0x00000028,
    Support01 = 0x00000029,
    Support02 = 0x0000002a,
    Support03 = 0x0000002b,
    Support04 = 0x0000002c,
    Support05 = 0x0000002d,
    Support06 = 0x0000002e,
    Support07 = 0x0000002f,
    Support08 = 0x00000030,
    Support09 = 0x00000031,
    Support10 = 0x00000032,
    EnterBattleGround = 0x00000033,
    LeaderSkill01 = 0x00000034,
    LeaderSkill02 = 0x00000035,
    LeaderSkill03 = 0x00000036,
    LeaderSkill04 = 0x00000037,
    LeaderSkill05 = 0x00000038,
    LeaderSkill06 = 0x00000039,
    LeaderSkill07 = 0x0000003a,
    LeaderSkill08 = 0x0000003b,
    LeaderSkill09 = 0x0000003c,
    LeaderSkill10 = 0x0000003d,
    Equipment01 = 0x0000003e,
    Equipment02 = 0x0000003f,
    Equipment03 = 0x00000040,
    Equipment04 = 0x00000041,
    Equipment05 = 0x00000042,
    Equipment06 = 0x00000043,
    Equipment07 = 0x00000044,
    Equipment08 = 0x00000045,
    Equipment09 = 0x00000046,
    Equipment10 = 0x00000047,
    PublicSkill01 = 0x00000048,
    PublicSkill02 = 0x00000049,
    PublicSkill03 = 0x0000004a,
    PublicSkill04 = 0x0000004b,
    PublicSkill05 = 0x0000004c,
    PublicSkill06 = 0x0000004d,
    PublicSkill07 = 0x0000004e,
    PublicSkill08 = 0x0000004f,
    PublicSkill09 = 0x00000050,
    PublicSkill10 = 0x00000051,
    GroupBuff01 = 0x00000052,
    HexaBuff01 = 0x00000053,
    EventBuff01 = 0x00000054,
    EventBuff02 = 0x00000055,
    EventBuff03 = 0x00000056,
    MoveAttack01 = 0x00000057,
    MetamorphNormalAttack = 0x00000058,
    GroundPassive01 = 0x00000059,
    GroundPassive02 = 0x0000005a,
    GroundPassive03 = 0x0000005b,
    GroundPassive04 = 0x0000005c,
    GroundPassive05 = 0x0000005d,
    GroundPassive06 = 0x0000005e,
    GroundPassive07 = 0x0000005f,
    GroundPassive08 = 0x00000060,
    GroundPassive09 = 0x00000061,
    GroundPassive10 = 0x00000062,
    HiddenPassive01 = 0x00000063,
    HiddenPassive02 = 0x00000064,
    HiddenPassive03 = 0x00000065,
    HiddenPassive04 = 0x00000066,
    HiddenPassive05 = 0x00000067,
    HiddenPassive06 = 0x00000068,
    HiddenPassive07 = 0x00000069,
    HiddenPassive08 = 0x0000006a,
    HiddenPassive09 = 0x0000006b,
    HiddenPassive10 = 0x0000006c,
    Count = 0x0000006d,
};

enum class BattleLogicState_Enum : int32_t
{
    None = 0x00000000,
    Preparing = 0x00000001,
    InProgress = 0x00000002,
    Finished = 0x00000003,
    Paused = 0x00000004,
};

enum class BattleEndType_Enum : int32_t
{
    None = 0x00000000,
    AllNearlyDead = 0x00000001,
    TimeOut = 0x00000002,
    EscortFailed = 0x00000003,
    Clear = 0x00000004,
};

enum class BattleTypes_Enum : int32_t
{
    None = 0x00000000,
    Adventure = 0x00000001,
    ScenarioMode = 0x00000002,
    WeekDungeonChaserA = 0x00000004,
    WeekDungeonBlood = 0x00000008,
    WeekDungeonChaserB = 0x00000010,
    WeekDungeonChaserC = 0x00000020,
    WeekDungeonFindGift = 0x00000040,
    EventContent = 0x00000080,
    TutorialAdventure = 0x00000100,
    Profiling = 0x00000200,
    SingleRaid = 0x00000800,
    MultiRaid = 0x00001000,
    PracticeRaid = 0x00002000,
    EliminateRaid = 0x00004000,
    MultiFloorRaid = 0x00008000,
    MinigameDefense = 0x00100000,
    Arena = 0x00200000,
    TimeAttack = 0x00800000,
    SchoolDungeonA = 0x02000000,
    SchoolDungeonB = 0x04000000,
    SchoolDungeonC = 0x08000000,
    WorldRaid = 0x10000000,
    Conquest = 0x20000000,
    FieldStory = 0x40000000,
    FieldContent = 0x7fffffff,
    PvE = -301988865,
    WeekDungeon = 0x0000007c,
    SchoolDungeon = 0x0e000000,
    Raid = 0x00007800,
    PvP = 0x00200000,
    All = -1,
};

enum class GroupTag_Enum : int32_t
{
    None = 0x00000000,
    Group01 = 0x00000001,
    Group02 = 0x00000002,
    Group03 = 0x00000004,
    Group04 = 0x00000008,
    Group05 = 0x00000010,
    Group06 = 0x00000020,
    Group07 = 0x00000040,
    Group08 = 0x00000080,
    Group09 = 0x00000100,
    Group10 = 0x00000200,
    Group11 = 0x00000400,
    Group12 = 0x00000800,
    Group13 = 0x00001000,
    Group14 = 0x00002000,
    Group15 = 0x00004000,
    Group16 = 0x00008000,
};

enum class ArmorType_Enum : int32_t
{
    LightArmor = 0x00000000,
    HeavyArmor = 0x00000001,
    Unarmed = 0x00000002,
    Structure = 0x00000003,
    Normal = 0x00000004,
    ElasticArmor = 0x00000005,
};

enum class StatType_Enum : int32_t
{
    None = 0x00000000,
    MaxHP = 0x00000001,
    AttackPower = 0x00000002,
    DefensePower = 0x00000003,
    HealPower = 0x00000004,
    AccuracyPoint = 0x00000005,
    AccuracyRate = 0x00000006,
    DodgePoint = 0x00000007,
    DodgeRate = 0x00000008,
    CriticalPoint = 0x00000009,
    CriticalChanceRate = 0x0000000a,
    CriticalResistChanceRate = 0x0000000b,
    CriticalDamageRate = 0x0000000c,
    MoveSpeed = 0x0000000d,
    SightRange = 0x0000000e,
    ActiveGauge = 0x0000000f,
    StabilityPoint = 0x00000010,
    StabilityRate = 0x00000011,
    ReloadTime = 0x00000012,
    MaxBulletCount = 0x00000013,
    IgnoreDelayCount = 0x00000014,
    WeaponRange = 0x00000015,
    BlockRate = 0x00000016,
    BodyRadius = 0x00000017,
    ActionCount = 0x00000018,
    StrategyMobility = 0x00000019,
    StrategySightRange = 0x0000001a,
    StreetBattleAdaptation = 0x0000001b,
    OutdoorBattleAdaptation = 0x0000001c,
    IndoorBattleAdaptation = 0x0000001d,
    HealEffectivenessRate = 0x0000001e,
    CriticalChanceResistPoint = 0x0000001f,
    CriticalDamageResistRate = 0x00000020,
    LifeRecoverOnHit = 0x00000021,
    NormalAttackSpeed = 0x00000022,
    AmmoCost = 0x00000023,
    GroggyGauge = 0x00000024,
    GroggyTime = 0x00000025,
    DamageRatio = 0x00000026,
    DamagedRatio = 0x00000027,
    OppressionPower = 0x00000028,
    OppressionResist = 0x00000029,
    RegenCost = 0x0000002a,
    InitialWeaponRangeRate = 0x0000002b,
    DefensePenetration = 0x0000002c,
    DefensePenetrationResisit = 0x0000002d,
    ExtendBuffDuration = 0x0000002e,
    ExtendDebuffDuration = 0x0000002f,
    ExtendCrowdControlDuration = 0x00000030,
    EnhanceExplosionRate = 0x00000031,
    EnhancePierceRate = 0x00000032,
    EnhanceMysticRate = 0x00000033,
    EnhanceLightArmorRate = 0x00000034,
    EnhanceHeavyArmorRate = 0x00000035,
    EnhanceUnarmedRate = 0x00000036,
    EnhanceSiegeRate = 0x00000037,
    EnhanceNormalRate = 0x00000038,
    EnhanceStructureRate = 0x00000039,
    EnhanceNormalArmorRate = 0x0000003a,
    DamageRatio2Increase = 0x0000003b,
    DamageRatio2Decrease = 0x0000003c,
    DamagedRatio2Increase = 0x0000003d,
    DamagedRatio2Decrease = 0x0000003e,
    EnhanceSonicRate = 0x0000003f,
    EnhanceElasticArmorRate = 0x00000040,
    ExDamagedRatioIncrease = 0x00000041,
    ExDamagedRatioDecrease = 0x00000042,
    EnhanceExDamageRate = 0x00000043,
    ReduceExDamagedRate = 0x00000044,
    HealRate = 0x00000045,
    HealLightArmorRate = 0x00000046,
    HealHeavyArmorRate = 0x00000047,
    HealUnarmedRate = 0x00000048,
    HealElasticArmorRate = 0x00000049,
    HealNormalArmorRate = 0x0000004a,
    HealedExplosionRate = 0x0000004b,
    HealedPierceRate = 0x0000004c,
    HealedMysticRate = 0x0000004d,
    HealedSonicRate = 0x0000004e,
    HealedNormalRate = 0x0000004f,
    Max = 0x00000050,
};

enum class FontType_Enum : int32_t
{
    None = 0x00000000,
    Damage = 0x00000001,
    Block = 0x00000002,
    Heal = 0x00000003,
    Miss = 0x00000004,
    Critical = 0x00000005,
    Skill = 0x00000006,
    Immune = 0x00000007,
    DamageResist = 0x00000008,
    DamageWeak = 0x00000009,
    CriticalResist = 0x0000000a,
    CriticalWeak = 0x0000000b,
    Effective = 0x0000000c,
    CriticalEffective = 0x0000000d,
};

enum class FormationLine_Enum : int32_t
{
    Students = 0x00000000, TSS = 0x00000001,
};

enum class BulletType_Enum : int32_t
{
    Normal = 0x00000000,
    Pierce = 0x00000001,
    Explosion = 0x00000002,
    Siege = 0x00000003,
    Mystic = 0x00000004,
    None = 0x00000005,
    Sonic = 0x00000006,
};

enum class EngageType_Enum : int32_t
{
    SearchAndMove = 0x00000000, HoldPosition = 0x00000001,
};

enum class EntityMaterialType_Enum : int32_t
{
    Wood = 0x00000000,
    Stone = 0x00000001,
    Flesh = 0x00000002,
    Metal = 0x00000003,
};

enum class School_Enum : int32_t
{
    None = 0x00000000,
    Hyakkiyako = 0x00000001,
    RedWinter = 0x00000002,
    Trinity = 0x00000003,
    Gehenna = 0x00000004,
    Abydos = 0x00000005,
    Millennium = 0x00000006,
    Arius = 0x00000007,
    Shanhaijing = 0x00000008,
    Valkyrie = 0x00000009,
    WildHunt = 0x0000000a,
    SRT = 0x0000000b,
    SCHALE = 0x0000000c,
    ETC = 0x0000000d,
    Tokiwadai = 0x0000000e,
    Sakugawa = 0x0000000f,
};

enum class MovingAreaOptions_Enum : int32_t
{
    None = 0x00000000, FixedAim = 0x00000001, CheckSpawnPositionOutOfMovingArea = 0x00000002,
};

enum class GroundNodeType_Enum : int32_t
{
    None = 0x00000000,
    WalkAble = 0x00000001,
    JumpAble = 0x00000002,
    TSSOnly = 0x00000003,
    NotWalkAble = 0x7fffffff,
};

enum class WeaponType_Enum : int32_t
{
    None = 0x00000000,
    SG = 0x00000001,
    SMG = 0x00000002,
    AR = 0x00000003,
    GL = 0x00000004,
    HG = 0x00000005,
    RL = 0x00000006,
    SR = 0x00000007,
    DSMG = 0x00000008,
    RG = 0x00000009,
    DSG = 0x0000000a,
    Vulcan = 0x0000000b,
    Missile = 0x0000000c,
    Cannon = 0x0000000d,
    Taser = 0x0000000e,
    MG = 0x0000000f,
    Binah = 0x00000010,
    MT = 0x00000011,
    Relic = 0x00000012,
    FT = 0x00000013,
    Akemi = 0x00000014,
};

enum class BehaviorType_Enum : int32_t
{
    None = 0x00000000,
    NormalAttack01 = 0x00000001,
    NormalAttack02 = 0x00000002,
    NormalAttack03 = 0x00000003,
    NormalAttack04 = 0x00000004,
    NormalAttack05 = 0x00000005,
    NormalAttack06 = 0x00000006,
    NormalAttack07 = 0x00000007,
    NormalAttack08 = 0x00000008,
    NormalAttack09 = 0x00000009,
    NormalAttack10 = 0x0000000a,
    UseExSkill01 = 0x0000000b,
    UseExSkill02 = 0x0000000c,
    UseExSkill03 = 0x0000000d,
    UseExSkill04 = 0x0000000e,
    UseExSkill05 = 0x0000000f,
    UseExSkill06 = 0x00000010,
    UseExSkill07 = 0x00000011,
    UseExSkill08 = 0x00000012,
    UseExSkill09 = 0x00000013,
    UseExSkill10 = 0x00000014,
    UsePublicSkill01 = 0x00000015,
    UsePublicSkill02 = 0x00000016,
    UsePublicSkill03 = 0x00000017,
    UsePublicSkill04 = 0x00000018,
    UsePublicSkill05 = 0x00000019,
    UsePublicSkill06 = 0x0000001a,
    UsePublicSkill07 = 0x0000001b,
    UsePublicSkill08 = 0x0000001c,
    UsePublicSkill09 = 0x0000001d,
    UsePublicSkill10 = 0x0000001e,
    Dead = 0x00000032,
    Dying = 0x00000033,
    Retreat = 0x00000034,
    EnterGround = 0x00000035,
    TSSInteract = 0x00000036,
    Idle = 0x00000037,
    Stunned = 0x00000064,
    Hit = 0x00000065,
    Knockback = 0x00000066,
    Panic = 0x00000067,
    Paralysis = 0x00000068,
    Emp = 0x00000069,
    Purify = 0x0000006a,
    Groggy = 0x0000006b,
    GroggyDead = 0x0000006c,
    Frozen = 0x0000006d,
    Move = 0x000000c8,
    MoveToFormationBeacon = 0x000000c9,
    MoveLeft = 0x000000ca,
    MoveRight = 0x000000cb,
    MoveAttack = 0x000000cc,
    ReleaseFormConversion = 0x000000cd,
    Walk = 0x0000012c,
    Stop = 0x0000012d,
    Seek = 0x0000012e,
    Flee = 0x0000012f,
    Evade = 0x00000130,
    Wander = 0x00000131,
    SeekPosition = 0x00000132,
    Feared = 0x00000133,
    Airborn = 0x00000134,
    Charmed = 0x00000135,
    Pulling = 0x00000136,
    Stasis = 0x00000137,
    Following = 0x00000138,
    MetamorphNormalAttack01 = 0x00000139,
};

enum class ActionState_Enum : int32_t
{
    Default = 0x00000000,
    Phase01 = 0x00000001,
    Phase02 = 0x00000002,
    Phase03 = 0x00000003,
    Phase04 = 0x00000004,
    Phase05 = 0x00000005,
    Phase06 = 0x00000006,
    Phase07 = 0x00000007,
    Phase08 = 0x00000008,
    Phase09 = 0x00000009,
    None = 0x0000000a,
};

enum class SkillApplyType_Enum : int32_t
{
    None = 0x00000000,
    Direct = 0x00000002,
    Hitscan = 0x00000004,
    AlwaysBlocked = 0x00000005,
};

enum class SpawnDirectionTypes_Enum : int32_t
{
    None = 0x00000000,
    Invoker = 0x00000001,
    Input = 0x00000002,
    ToTarget = 0x00000003,
    AllyToEnemy = 0x00000004,
    EnemyToAlly = 0x00000005,
    AliveAllyCenter = 0x00000006,
    AliveEnemyCenter = 0x00000007,
    WorldPosition = 0x00000008,
    CasterToTarget = 0x00000009,
    TargetToCaster = 0x0000000a,
};
