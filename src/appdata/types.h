#pragma once

#include "enums.h"
#include "types-helper.h"

///////////////////////////////
/// All Forward Declarations
///////////////////////////////

class CostSkillCardManager;
class PlayerSkillCardManager;
class BattleEntityStat;
class BattleEntityStatProcessor;
class BattleEntity;
class BattleSummary;
class Battle;
class CharacterGroup;
class PlayerGroup;
class ShieldEffect;
class ShieldInfo;
class Character;
class DamageResult;
class HeroAction;
class NewSkillAction;
class NewNormalAttackAction;
class Screen;

///////////////////////////////
/// Class Definitions
///////////////////////////////

class CostSkillCardManager
{
    UNITY_CLASS_DECL("BlueArchive.dll", "CostSkillCardManager")

    UNITY_FIELD(float, MaxCost, 0x38)
    UNITY_FIELD(float, CurCost, 0x3c)
    UNITY_FIELD(float, FeverChargeScale, 0x40)
    UNITY_FIELD(float, CostPerFrame, 0x44)
    UNITY_FIELD(bool, IsShuffle, 0x48)
    UNITY_FIELD(int64_t, RegenStartDelayFrame, 0x60)
    UNITY_FIELD(bool, IsMainPlayerSkillCard, 0x68)
    UNITY_FIELD(float, RegenCostBefore, 0x6c)
};

class PlayerSkillCardManager : public CostSkillCardManager
{
    UNITY_CLASS_DECL("BlueArchive.dll", "PlayerSkillCardManager")

    UNITY_METHOD(void, ProcessSkillCard, PlayerSkillCardManager*)
};

class BattleEntityStat
{
    UNITY_CLASS_DECL("BlueArchive.dll", "BattleEntityStat")

    UNITY_METHOD(int64_t, get_Item, BattleEntityStat*, StatType_Enum)
};

class BattleEntityStatProcessor
{
    UNITY_CLASS_DECL("BlueArchive.dll", "BattleEntityStatProcessor")

    UNITY_FIELD(int64_t, BattlePower, 0x10)
    UNITY_FIELD(BattleEntityStat*, DefaultStat, 0x18)
    UNITY_FIELD(BattleEntityStat*, InitialStat, 0x20)
    UNITY_FIELD(BattleEntityStat*, CurrentStat, 0x28)
    UNITY_FIELD(BattleEntity*, owner, 0x48)
    UNITY_FIELD(int64_t, lastMaxHP, 0x60)

    UNITY_METHOD(void, Apply, BattleEntityStatProcessor*, StatType_Enum)
};

class BattleEntity
{
    UNITY_CLASS_DECL("BlueArchive.dll", "BattleEntity")

    UNITY_FIELD(void*, Damaged, 0x28)  // EventHandler<BattleEntityDamagedEventArgs>
    UNITY_FIELD(TacticEntityType_Enum, TacticEntityType, 0x38)
    UNITY_FIELD(UnityResolve::UnityType::String*, Name, 0x58)
    UNITY_FIELD(int64_t, HitPoint, 0x78)
    UNITY_FIELD(int64_t, HitPointBefore, 0x80)
    UNITY_FIELD(int64_t, MaxHPCapGauge, 0x88)
    UNITY_FIELD(int64_t, SummonedTime, 0x90)
    UNITY_FIELD(ArmorType_Enum, ArmorType, 0x98)
    UNITY_FIELD(BattleEntityStatProcessor*, statProcessor, 0xA0)

    // UNITY_METHOD(void, OnDamaged, BattleEntity*, void*)
    UNITY_METHOD(bool, CanBeTargeted, BattleEntity*, BattleEntity*, SkillSlot_Enum)
    UNITY_METHOD(int64_t, AddHitPoint, BattleEntity*, int64_t)
    UNITY_METHOD(void, ApplyStat, BattleEntity*, StatType_Enum)
    UNITY_METHOD(BattleEntityStat*, get_CurrentStat, BattleEntity*)
};

class BattleSummary
{
    UNITY_CLASS_DECL("BlueArchive.dll", "BattleSummary")

    UNITY_FIELD(int64_t, HashKey, 0x10)
    UNITY_FIELD(bool, IsBossBattle, 0x18)
    UNITY_FIELD(BattleTypes_Enum, BattleType, 0x1C)
    UNITY_FIELD(int64_t, StageId, 0x20)
    UNITY_FIELD(int64_t, GroundId, 0x28)
    UNITY_FIELD(GroupTag_Enum, Winner, 0x30)
    UNITY_FIELD(BattleEndType_Enum, EndType, 0x34)
    UNITY_FIELD(int, EndFrame, 0x38)
    UNITY_FIELD(double, UnitType, 0x40)
    UNITY_FIELD(double, ResultValue, 0x48)
    UNITY_FIELD(int, ContinueCount, 0x78)
    UNITY_FIELD(float, ElapsedRealtime, 0x7C)
    UNITY_FIELD(int, writeFrame, 0x80)
    UNITY_FIELD(int64_t, EventContentId, 0x88)
    UNITY_FIELD(int64_t, FixedEchelonId, 0x90)
    UNITY_FIELD(bool, IsAbort, 0x98)
    UNITY_FIELD(bool, IsDefeatBattle, 0x99)
    UNITY_FIELD(Battle*, battle, 0xA0)

    UNITY_METHOD(void, InitBattleTypeSummary, BattleSummary*, Battle*)
    UNITY_METHOD(void, Finalize, BattleSummary*, Battle*, GroupTag_Enum, BattleEndType_Enum, int, float)
};

struct TimeSpan
{
    int64_t _ticks;
};

class LogicGameTime
{
    UNITY_CLASS_DECL("BlueArchive.dll", "LogicGameTime")

    UNITY_FIELD(float, UnitySecondPerFrame, 0x10)
    UNITY_FIELD(TimeSpan, SecondPerFrame, 0x18)
    UNITY_FIELD(int32_t, FramePerSecond, 0x20)
    UNITY_FIELD(TimeSpan, TotalGameTime, 0x28)
    UNITY_FIELD(int32_t, CurrentFrame, 0x30)
    UNITY_FIELD(bool, Paused, 0x34)
    UNITY_FIELD(int32_t, TimeoutFrame, 0x38)
    UNITY_FIELD(bool, Timeout, 0x3C)

    UNITY_METHOD(void, Pause, LogicGameTime*)
    UNITY_METHOD(void, Resume, LogicGameTime*)
};

class LogicEffect
{
    UNITY_CLASS_DECL("BlueArchive.dll", "LogicEffect")

    // UNITY_FIELD(SkillSpecification*, SkillSpecification, 0x10)
    // UNITY_FIELD(LogicEffectHitSpecification*, LogicEffectHitSpecification, 0x18)
    // UNITY_FIELD(BattleEntity*, Invoker, 0x20)
    // UNITY_FIELD(BattleEntity*, Target, 0x28)
    // UNITY_FIELD(BattleEntity*, OriginalTarget, 0x30)
    // UNITY_FIELD(Func_1_Boolean_*, ExpirationCheck, 0x38)
    // UNITY_FIELD(Entity*, ExpirationCheckOwner, 0x40)
    // UNITY_FIELD(String*, SkillEntityName, 0x48)
    // UNITY_FIELD(int64_t, SpawnRate, 0x50)
    // UNITY_FIELD(String*, LogicEffectGroupId, 0x58)
    // UNITY_FIELD(LogicEffectCategory__Enum, Category, 0x60)
    // UNITY_FIELD(String*, templateId, 0x68)
    // UNITY_FIELD(Hash64, TemplateIdHash, 0x70)
    // UNITY_FIELD(int32_t, Channel, 0x78)
    // UNITY_FIELD(BasisPoint, ApplyRate, 0x80)
    // UNITY_FIELD(uint32_t, CommonVisualIdHash, 0x88)
    // UNITY_FIELD(Vector2, HitPosition, 0x8C)
    // UNITY_FIELD(Vector2, BulletPosition, 0x94)
    // UNITY_FIELD(Vector2, BulletDirection, 0x9C)
    // UNITY_FIELD(Entity*, BulletEntity, 0xA8)
    // UNITY_FIELD(ResolvePriority__Enum, priority, 0xB0)
    // UNITY_FIELD(int32_t, Priority, 0xB4)
    // UNITY_FIELD(int32_t, ResolveIndex, 0xB8)
    // UNITY_FIELD(int32_t, DotIndex, 0xBC)
    // UNITY_FIELD(int32_t, ExtraCostUsed, 0xC0)
    // UNITY_FIELD(bool, ForceFloaterHide, 0xC4)
};

// Obfuscated class
class LogicEffectProcessor
{
    UNITY_CLASS_DECL_FROM_FIELD_NAME("BlueArchive.dll", "CrowdControlGaugeEffect", "logicEffectProcessor")

    UNITY_FIELD(UnityResolve::UnityType::List<LogicEffect>*, logicEffects, 0x48)
};

class Battle
{
    UNITY_CLASS_DECL("BlueArchive.dll", "Battle")

    UNITY_FIELD(BattleLogicState_Enum, state, 0x188)
    UNITY_FIELD(LogicGameTime*, GameTime, 0x190)
    UNITY_FIELD(int64_t, StartTickRealTime, 0x198)
    UNITY_FIELD(int, MaxDurationFrame, 0x1A0)
    UNITY_FIELD(LogicEffectProcessor*, LogicEffectProcessor_, 0x1D8)
    UNITY_FIELD(BattleSummary*, BattleSummary_, 0x1F0)
    UNITY_FIELD(double, UnitType, 0x200)
    UNITY_FIELD(double, ResultValue, 0x208)
    UNITY_FIELD(PlayerGroup*, playerGroup, 0x220)
    UNITY_FIELD(int, TotalEnemyCount, 0x2F8)
    UNITY_FIELD(int, RemainEnemyCount, 0x2FC)

    UNITY_METHOD(void, Update, Battle*)
    UNITY_METHOD(void, Begin, Battle*)
    UNITY_METHOD_BETWEEN(void, Finalize, "Resume", "Push", Battle*, BattleEndType_Enum)
};

class CharacterGroup
{
    UNITY_FIELD(GroupTag_Enum, GroupTag, 0x18)
};

class PlayerGroup : public CharacterGroup
{
    UNITY_FIELD(int, EchelonNumber, 0x140)
    UNITY_FIELD(PlayerSkillCardManager*, PlayerSkillCardManager_, 0x150)
};

class ShieldEffect
{
    UNITY_FIELD(int64_t, BaseAmount, 0xC8)
    UNITY_FIELD(StatType_Enum, TargetStatType, 0xD0)
    UNITY_FIELD(StatType_Enum, CasterStatType, 0xE0)
    UNITY_FIELD(int32_t, DurationFrame, 0xF0)
    UNITY_FIELD(bool, IsDispellable, 0xF4)
};

class ShieldInfo
{
    UNITY_FIELD(int64_t, CurrentHP_k, 0x10)
    UNITY_FIELD(int64_t, MaxHP_k, 0x18)
    UNITY_FIELD(ShieldEffect, ShieldEffect_k, 0x28)
};

// Obfuscated class
class Character : public BattleEntity
{
    UNITY_CLASS_DECL_FROM_FIELD_NAME("BlueArchive.dll", "CharacterMovementComponent", "<Character>k__BackingField")

    UNITY_FIELD(bool, IsSearchAndMoveActivated, 0x118)
    UNITY_FIELD(int32_t, lastTargetFindFrame, 0x11C)
    UNITY_FIELD(int64_t, ServerId, 0x1C0)
    UNITY_FIELD(int64_t, OwnerAccountId, 0x1C8)
    UNITY_FIELD(int64_t, CharacterId, 0x1D0)
    UNITY_FIELD(int64_t, CharacterSkillListGroupId, 0x1D8)
    UNITY_FIELD(int64_t, PersonalityId, 0x1E0)
    UNITY_FIELD(int64_t, AIPatternId, 0x1E8)
    UNITY_FIELD(bool, CheckCanUseAutoPublicSkill, 0x1F0)
    UNITY_FIELD(FormationLine_Enum, Line, 0x1F4)
    UNITY_FIELD(int32_t, LineIndex, 0x1F8)
    UNITY_FIELD(bool, IsAttacked, 0x1FC)
    UNITY_FIELD(bool, canUseObstacleOfStandMotionDefault, 0x1FD)
    UNITY_FIELD(bool, canUseObstacleOfKneelMotionDefault, 0x1FE)
    UNITY_FIELD(bool, IsNPC, 0x1FF)
    UNITY_FIELD(bool, IsAirUnit, 0x200)
    UNITY_FIELD(bool, IsDefenseCharacter, 0x201)
    UNITY_FIELD(int64_t, AirUnitHeight, 0x208)
    UNITY_FIELD(BulletType_Enum, BulletType, 0x210)
    UNITY_FIELD(int64_t, RandomEffectRaidus, 0x218)
    UNITY_FIELD(EngageType_Enum, EngageType, 0x220)
    UNITY_FIELD(EntityMaterialType_Enum, MaterialType, 0x224)
    UNITY_FIELD(School_Enum, School, 0x228)
    UNITY_FIELD(int64_t, BulletCount, 0x238)
    UNITY_FIELD(int32_t, FavorRank, 0x240)
    UNITY_FIELD(int32_t, FormCount, 0x244)
    UNITY_FIELD(MovingAreaOptions_Enum, MovingAreaOption, 0x258)
    UNITY_FIELD(Battle*, battleCache, 0x280)
    UNITY_FIELD(CharacterGroup*, CharacterGroup_, 0x288)
    UNITY_FIELD(bool, IsDefaultMobile, 0x298)
    UNITY_FIELD(bool, CanSurvive, 0x299)
    UNITY_FIELD(bool, CheckTSSBlocked, 0x29A)
    UNITY_FIELD(GroundNodeType_Enum, PassableNodeType, 0x29C)
    UNITY_FIELD(bool, CanAttackWhileMove, 0x2A0)
    UNITY_FIELD(bool, IsJumpable, 0x2A1)
    UNITY_FIELD(bool, IsOnJump, 0x2A2)
    UNITY_FIELD(bool, IsOnMoveEndRootMotion, 0x2A3)
    UNITY_FIELD(bool, IsWeaponMounted, 0x2A4)
    UNITY_FIELD(WeaponType_Enum, WeaponType, 0x2A8)
    UNITY_FIELD(int32_t, JumpMotionFrame, 0x2D0)
    UNITY_FIELD(int32_t, MoveStartFrame, 0x2D4)
    UNITY_FIELD(int32_t, MoveEndFrame, 0x2D8)
    UNITY_FIELD(int32_t, DeadFrame, 0x2DC)
    UNITY_FIELD(int32_t, AppearFrame, 0x2E0)
    UNITY_FIELD(bool, IsAppearFinished, 0x2E4)
    UNITY_FIELD(bool, IsInteractionTSAFinished, 0x2F0)
    UNITY_FIELD(bool, IsForceIdle, 0x300)
    UNITY_FIELD(bool, IsPrivateWeapon, 0x338)
    UNITY_FIELD(bool, IsRootMotionInLastMove, 0x398)
    UNITY_FIELD(bool, IsAttackEnterSkipByLastSkill, 0x399)
    UNITY_FIELD(bool, RequireReleaseFormConversion, 0x3B0)
    UNITY_FIELD(ShieldInfo*, CurrentShield, 0x3B8)
    UNITY_FIELD(bool, CanBattleItemMove, 0x3D0)
    UNITY_FIELD(int64_t, AIPhase, 0x428)
    UNITY_FIELD(int64_t, AIPhaseToChange, 0x430)
    UNITY_FIELD(int64_t, currentActiveGauge, 0x440)
    UNITY_FIELD(int32_t, CurrentNormalAttackCount, 0x448)

    UNITY_METHOD(void, Update, Character*, Battle*)
    UNITY_METHOD(void, InitAmmo, Character*)
    UNITY_METHOD(void, ReloadAmmo, Character*)
    UNITY_METHOD(void*, ApplyDamage, Character*, BattleEntity*, DamageResult)
};

class DamageResult
{
    UNITY_FIELD(int64_t, AttackPower, 0x00)
    UNITY_FIELD(int64_t, Damage, 0x08)
    UNITY_FIELD(int32_t, Stability, 0x10)
    UNITY_FIELD(int64_t, CriticalMultiplier, 0x18)
    UNITY_FIELD(FontType_Enum, HitResultType, 0x20)
    UNITY_FIELD(bool, IgnoreShield, 0x24)
    UNITY_FIELD(bool, DoNotKillTarget, 0x25)
};

class HeroAction
{
    UNITY_CLASS_DECL("BlueArchive.dll", "HeroAction")

    UNITY_FIELD(BehaviorType_Enum, BehaviorType, 0x30)
    UNITY_FIELD(BattleEntity*, Executer, 0x38)
    UNITY_FIELD(bool, IsLocking, 0x40)
    UNITY_FIELD(int32_t, elapsed, 0x44)
    UNITY_FIELD(int32_t, duration, 0x48)
    UNITY_FIELD(ActionState_Enum, actionState, 0x4C)
};

class NewSkillAction : public HeroAction
{
    UNITY_CLASS_DECL("BlueArchive.dll", "NewSkillAction")

    // UNITY_FIELD(SkillSpecification*, SkillSpecification, 0x50)
    // UNITY_FIELD(TargetCandidateRule, PrimaryCandidateRule, 0x58)
    // UNITY_FIELD(TargetSortRule, PrimarySortRule, 0xF0)
    UNITY_FIELD(SkillApplyType_Enum, PrimarySkillApplyType, 0x110)
    UNITY_FIELD(int64_t, Range, 0x120)
    UNITY_FIELD(int64_t, Angle, 0x128)
    UNITY_FIELD(int64_t, MinRange, 0x130)
    UNITY_FIELD(SpawnDirectionTypes_Enum, ExecuterDirectionType, 0x138)
    UNITY_FIELD(UnityResolve::UnityType::Vector2, ExecuterDirectionWorldPosition, 0x13C)
    UNITY_FIELD(SpawnDirectionTypes_Enum, CurrentInvokerDirectionType, 0x144)
    UNITY_FIELD(UnityResolve::UnityType::Vector2, CurrentInvokerDirectionWorldPosition, 0x148)
    // UNITY_FIELD(SkillEntityDAO*, MainEntityData, 0x150)
    UNITY_FIELD(bool, IsAttackEnterSkipByLastSkill, 0x160)
    UNITY_FIELD(UnityResolve::UnityType::Vector2, RootMotionStartPosition, 0x164)
    UNITY_FIELD(UnityResolve::UnityType::Vector2, RootMotionEndPosition, 0x16C)
    UNITY_FIELD(bool, ignoreCrashByTSSObstacleCheck, 0x174)
};

class NewNormalAttackAction : public NewSkillAction
{
    UNITY_CLASS_DECL("BlueArchive.dll", "NewNormalAttackAction")

    UNITY_FIELD(Character*, ownerCharacter, 0x178)
    UNITY_FIELD(float, currentPhaseEndIn, 0x180)
    UNITY_FIELD(float, initCurPhaseEndIn, 0x184)
    // UNITY_FIELD(GroundNode*, lastEnteredAttackPosition, 0x188)
    UNITY_FIELD(int32_t, attackEnterDuration, 0x190)
    UNITY_FIELD(int32_t, attackStartDuration, 0x194)
    UNITY_FIELD(int32_t, attackBurstOverDelayDuration, 0x198)
    UNITY_FIELD(int32_t, attackEndDuration, 0x19C)
    UNITY_FIELD(int32_t, attackReadyStartDuration, 0x1A0)
    UNITY_FIELD(int32_t, attackReadyEndDuration, 0x1A4)
    UNITY_FIELD(int32_t, attackReloadDuration, 0x1A8)
    UNITY_FIELD(int64_t, remainBurstRoundCount, 0x1B0)
    UNITY_FIELD(int64_t, lastBurstRoundCount, 0x1B8)
    UNITY_FIELD(int32_t, shotCountInOneAttackingAction, 0x1C0)
    UNITY_FIELD(int32_t, currentShotCountInOneAttackingAction, 0x1C4)
    UNITY_FIELD(float, shotOccuredIn, 0x1C8)
    UNITY_FIELD(bool, isShotOverInOneAttackingAction, 0x1CC)
    UNITY_FIELD(int32_t, attackIngDuration, 0x1D0)
    // UNITY_FIELD(NormalAttackPhaseDAO*, phaseData, 0x1E0)
    UNITY_FIELD(bool, overrideEntityDirection, 0x1F8)
    UNITY_FIELD(SpawnDirectionTypes_Enum, exclusiveIngInvokerDirectionOverride, 0x1FC)

    UNITY_METHOD(void, Update, NewNormalAttackAction*, Battle*)
};


class Screen 
{
    UNITY_CLASS_DECL("UnityEngine.CoreModule.dll", "Screen")

    
        //System.Void UnityEngine.Screen::SetResolution(System.Int32, System.Int32, System.Boolean)
    //UNITY_FIELD()

        UNITY_METHOD(void, SetResolution, int,int,bool)
};
