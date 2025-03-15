#pragma once
#include "NativeGameplayTags.h"

namespace SMTags
{
	namespace Ability
	{
		namespace Activation
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hold);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Slash);
		}

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Immune);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(CatchBuff);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakIndicator);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_ElectricGuitar);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Piano);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Bass);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Skill);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Skill_ElectricGuitar);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Skill_Piano);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Skill_Bass);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak_ElectricGuitar);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak_Piano);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak_Bass);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Displayable);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Slot_Add);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Slot_Remove);
	}

	namespace AttributeSet
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxHP);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HP);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MoveSpeed);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxStamina);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stamina);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxSkillGauge);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SkillGauge);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Heal);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);
	}

	namespace Character
	{
		namespace Cooldown
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hold);
		}

		namespace State
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(State);
			
			namespace Common
			{
				namespace Blocking
				{
					UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hold);
					UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
					UE_DECLARE_GAMEPLAY_TAG_EXTERN(Skill);
					UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak);
					UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sampling);
				}

				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Skill);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Uncontrollable);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Invincible);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(MoveSpeedBuff);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(UnlimitStamina);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(UnlimitSkillGauge);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(PowerUp);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hold);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Held);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stun);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Neutralized);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Exhausted);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreaked);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonHoldable);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Immune);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Jump);
			}

			namespace ElectricGuitar
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(SlowBullet);
			}

			namespace Piano
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(ImpactArrow);
			}

			namespace Bass
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Charge);
			}
		}
	}

	namespace Data
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Duration);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown);
	}

	namespace Item
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttributeChanger);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(UseType_Hold);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(UseType_Overlap);
	}

	namespace Event
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event);
		namespace AnimNotify
		{
			namespace Common
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hold);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakEnd);
			}

			namespace ElectricGuitar
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(SlowBulletShoot);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakFlash);
			}

			namespace Piano
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(PianoNoiseBreakShoot);
			}

			namespace Bass
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChargeFail);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakWeaponTrailActivate);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakWeaponTrailDeactivate);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakSlash);
			}

			namespace Attack // 베이스쪽으로 옮겨야함
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttackStart);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(CanInput);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(CanProceedNextAction);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeftSlashNext);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(RightSlashNext);
			}
		}

		namespace Character
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(BuzzerBeaterEnd);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(CaughtExitEnd);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stun);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Neutralize);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Exhausted);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreaked);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Healed);
		}
	}

	namespace GameplayCue
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ApplyHeal);

		namespace Type
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Executed);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(OnActive);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Removed);
		}

		namespace Common
		{
			namespace Item
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Invincible);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(PowerUp);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(MoveSpeedUp);
			}

			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Neutralize);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Immune);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hold);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Held);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HoldSuccess);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HoldHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stun);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitFlash);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sampling);
		}

		namespace Piano
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Archery);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcheryCharge1);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcheryCharge2);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcheryProjectileCharge1);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcheryProjectileCharge2);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcheryHitCharge1);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcheryHitCharge2);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcheryWallHitCharge1);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArcheryWallHitCharge2);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ImpactArrow);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ImpactArrowReady);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ImpactArrowExplosion);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ImpactArrowHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakBurst);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakBurstHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HoldWeapon);
		}

		namespace ElectricGuitar
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shoot);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShootProjectile);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShootHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShootWallHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SlowBullet);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SlowBulletCharge);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SlowBulletProjectile);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SlowBulletHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SlowBulletDebuff);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakFlash);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakBurst);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakBurstHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HoldWeapon);
		}

		namespace Bass
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Slash);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SlashHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Charge);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChargeCrush);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChargeHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreak);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakBurst);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakBurstHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakWeaponTrail);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NoiseBreakSlash);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HoldWeapon);
		}

		namespace SpecialAction
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Smash);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SmashSuccessWithCharacter);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SmashSplashHit);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SmashSuccessWithHealItem);
		}

		namespace JumpPad
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(UseJumpPad);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Land);
		}
	}
}
