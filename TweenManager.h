// Inan Evin, Copyright 2021

#pragma once

#include "CoreMinimal.h"
#include "Core/TweenCommon.h"

class CTweenManager;

struct FTweenPackFloat
{
	float* m_var = nullptr;
	float m_fromOrAmount = 0.0f;
	float m_toOrSpeed = 0.0f;
	float m_baseValue = 0.0f;
};

struct FTweenPackFVector
{
	FVector* m_var = nullptr;
	FVector m_fromOrAmount = FVector::ZeroVector;
	FVector m_toOrSpeed = FVector::ZeroVector;
	FVector m_baseValue = FVector::ZeroVector;
};


struct FTweenPackFRotator
{
	FRotator* m_var = nullptr;
	FRotator m_fromOrAmount = FRotator::ZeroRotator;
	FRotator m_toOrSpeed = FRotator::ZeroRotator;
	FRotator m_baseValue = FRotator::ZeroRotator;
};


class CTween
{
public:
	CTween(ETweenID id, FTimerManager* timerManager)
	{
		m_id = id;
		m_timerManager = timerManager;
	}

	~CTween()
	{
		Cleanup();
	}

	FORCEINLINE CTween& SetRate(float rate)
	{
		m_rate = rate;
		return *this;
	}

	FORCEINLINE CTween& SetDelay(float delay)
	{
		m_delay = delay;
		return *this;
	}

	FORCEINLINE CTween& SetEase(ETweenEase ease)
	{
		m_ease = ease;
		return *this;
	}

	FORCEINLINE CTween& SetMultiplier(float multiplier)
	{
		m_multiplier = multiplier;
		return *this;
	}

	FORCEINLINE CTween& SetRemoveWhenFinished(bool removeWhenFinished)
	{
		m_removeWhenFinished = removeWhenFinished;
		return *this;
	}

	FORCEINLINE TweenCompleted& OnComplete()
	{
		return m_onTweenCompleted;
	}

	FORCEINLINE TweenCompleted& OnUpdate()
	{
		return m_onTweenUpdated;
	}

	FORCEINLINE CTween& Interpolate(float* var, float from, float to, float duration)
	{
		return EnsureVariableTypeAndDelegate<float>(ETweenVariableType::TypeFloat, ETweenType::TypeInterpolation, var, from, to, duration);
	}

	FORCEINLINE CTween& Interpolate(FVector* var, FVector from, FVector to, float duration)
	{
		return EnsureVariableTypeAndDelegate<FVector>(ETweenVariableType::TypeFVector, ETweenType::TypeInterpolation, var, from, to, duration);
	}

	FORCEINLINE CTween& Interpolate(FRotator* var, FRotator from, FRotator to, float duration)
	{
		return EnsureVariableTypeAndDelegate<FRotator>(ETweenVariableType::TypeFRotator, ETweenType::TypeInterpolation, var, from, to, duration);
	}

	FORCEINLINE CTween& Shake(float* var, float amount, float speed, float duration)
	{
		return EnsureVariableTypeAndDelegate<float>(ETweenVariableType::TypeFloat, ETweenType::TypeShake, var, amount, speed, duration, *var);
	}

	FORCEINLINE CTween& Shake(FVector* var, FVector amount, FVector speed, float duration)
	{
		return EnsureVariableTypeAndDelegate<FVector>(ETweenVariableType::TypeFVector, ETweenType::TypeShake, var, amount, speed, duration, *var);
	}

	FORCEINLINE CTween& Shake(FRotator* var, FRotator amount, FRotator speed, float duration)
	{
		return EnsureVariableTypeAndDelegate<FRotator>(ETweenVariableType::TypeFRotator, ETweenType::TypeShake, var, amount, speed, duration, *var);
	}

	void Stop();
	bool IsActive() const;

private:
	
	template <typename T>
	CTween& EnsureVariableTypeAndDelegate(ETweenVariableType type, ETweenType tweenType, T* var, T fromOrAmount, T toOrSpeed, float duration, T baseValue = T())
	{
		// If not bound, bind according to the tween type.
		if (!m_delegate.IsBound())
		{
			m_type = tweenType;
			m_variableType = type;

			switch (m_variableType)
			{
			case ETweenVariableType::TypeFloat:
				m_floatPack = new FTweenPackFloat();
				break;
			case ETweenVariableType::TypeFVector:
				m_vectorPack = new FTweenPackFVector();
				break;
			case ETweenVariableType::TypeFRotator:
				m_rotatorPack = new FTweenPackFRotator();
				break;
			}

			if (tweenType == ETweenType::TypeInterpolation)
				m_delegate.BindRaw(this, &CTween::OnStepInterpolate);
			else if (tweenType == ETweenType::TypeShake)
				m_delegate.BindRaw(this, &CTween::OnStepShake);
		}
		else
		{
			if (m_variableType != type)
			{
			#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				UE_LOG(LogTemp, Warning, TEXT("Can't start the tween because the previous variable type is not the same as requested type. %s"), *UEnum::GetValueAsString(type));
			#endif
				return *this;
			}
			else
			{
				// If variable type is the same, we can use this tweener.
				// But if the tween types are different, unbind & rebind the delegate.
				if (m_type != tweenType)
				{
					m_delegate.Unbind();

					if (tweenType == ETweenType::TypeInterpolation)
						m_delegate.BindRaw(this, &CTween::OnStepInterpolate);
					else if (tweenType == ETweenType::TypeShake)
						m_delegate.BindRaw(this, &CTween::OnStepShake);

					m_type = tweenType;
				}
			}
		}

		AssignVariablePack(var, fromOrAmount, toOrSpeed, baseValue);
		m_elapsedTime = 0.0f;
		m_duration = duration;

		if (m_timerManager->TimerExists(m_handle))
			m_timerManager->ClearTimer(m_handle);

		m_timerManager->SetTimer(m_handle, m_delegate, m_rate, true, m_delay);
		return *this;
	}

	FORCEINLINE void AssignVariablePack(float* var, float fromOrAmount, float toOrSpeed, float base) const
	{
		m_floatPack->m_var = var;
		m_floatPack->m_fromOrAmount = fromOrAmount;
		m_floatPack->m_toOrSpeed = toOrSpeed;
		m_floatPack->m_baseValue = base;
	}

	FORCEINLINE void AssignVariablePack(FVector* var, FVector fromOrAmount, FVector toOrSpeed, FVector base ) const
	{
		m_vectorPack->m_var = var;
		m_vectorPack->m_fromOrAmount = fromOrAmount;
		m_vectorPack->m_toOrSpeed = toOrSpeed;
		m_vectorPack->m_baseValue = base;
	}

	FORCEINLINE void AssignVariablePack(FRotator* var, FRotator fromOrAmount, FRotator toOrSpeed, FRotator base) const
	{
		m_rotatorPack->m_var = var;
		m_rotatorPack->m_fromOrAmount = fromOrAmount;
		m_rotatorPack->m_toOrSpeed = toOrSpeed;
		m_rotatorPack->m_baseValue = base;
	}

	void OnStepInterpolate();
	void OnStepShake();

	template <typename T>
	void StepInterpolate(T* var, T from, T to)
	{
		auto t = m_elapsedTime / m_duration;

		if (var != nullptr)
		{
			if (m_ease == ETweenEase::Linear)
				*var = FMath::Lerp(from, to, t);
			else if (m_ease == ETweenEase::EaseIn)
				*var = FMath::InterpEaseIn(from, to, t, 90.0f);
			else if (m_ease == ETweenEase::EaseOut)
				*var = FMath::InterpEaseOut(from, to, t, 90.0f);
			else if (m_ease == ETweenEase::SineIn)
				*var = FMath::InterpSinIn(from, to, t);
			else if (m_ease == ETweenEase::SineOut)
				*var = FMath::InterpSinIn(from, to, t);
		}

		if (m_onTweenUpdated.IsBound())
			m_onTweenUpdated.Execute();

		m_elapsedTime += m_rate;
		
		// TODO: Find a better equality check for below.
		if (m_elapsedTime >= m_duration)
		{
			if (var != nullptr)
				*var = to;

			if (m_onTweenUpdated.IsBound())
				m_onTweenUpdated.Execute();
			
			Stop();
		}
	}

	template <typename T>
	void StepShake(T* val, T amount, T speed, T* base)
	{
	}

	template <>
	void StepShake<float>(float* var, float amount, float speed, float* base)
	{
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		const float delta = 0.0f;
		float target = *base + FMath::Sin(m_elapsedTime * speed) * amount * delta * 100;
		*base = FMath::Lerp(*base, 0.0f, s_shakeBaseResetSpeed * delta);
		if (m_onTweenUpdated.IsBound())
			m_onTweenUpdated.Execute();

		if (m_multiplier > 0.0f)
		{
			const float multiplier = FMath::Lerp(m_multiplier, 0.0f, m_elapsedTime / m_duration);
			target *= multiplier;
		}

		*var = FMath::Lerp(*var, target, s_shakeInterpolateSpeed * delta);
		//*var = FMath::FInterpTo(*var, target, delta, SHAKE_LERP_SPEED);
		ElapseShakeData();
	}

	template <>
	void StepShake<FVector>(FVector* var, FVector amount, FVector speed, FVector* base)
	{
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		const float delta = 0.0f;

		FVector target = *base + FVector(
			FMath::Sin(m_elapsedTime * speed.X) * amount.X,
			FMath::Sin(m_elapsedTime * speed.Y) * amount.Y,
			FMath::Sin(m_elapsedTime * speed.Z) * amount.Z
		);
		*base = FMath::Lerp(*base, FVector::ZeroVector, s_shakeBaseResetSpeed * delta);

		if (m_onTweenUpdated.IsBound())
			m_onTweenCompleted.Execute();


		if (m_multiplier > 0.0f)
		{
			const float multiplier = FMath::Lerp(m_multiplier, 0.0f, m_elapsedTime / m_duration);
			target *= multiplier;
		}
		*var = FMath::Lerp(*var, target, s_shakeInterpolateSpeed * delta);
	//	*var = FMath::VInterpTo(*var, target, delta, SHAKE_LERP_SPEED);
		ElapseShakeData();
	}

	template <>
	void StepShake<FRotator>(FRotator* var, FRotator amount, FRotator speed, FRotator* base)
	{
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		// TODO: PROVIDE YOUR OWN WAY OF GETTING THE DELTA TIME HERE
		const float delta = 0.0f;

		FRotator target = *base + FRotator(
			FMath::Sin(m_elapsedTime * speed.Pitch) * amount.Pitch,
			FMath::Sin(m_elapsedTime * speed.Yaw) * amount.Yaw,
			FMath::Sin(m_elapsedTime * speed.Roll) * amount.Roll
		) * delta * 100;
		
		*base = FMath::Lerp(*base, FRotator::ZeroRotator, s_shakeBaseResetSpeed * delta);

		if (m_onTweenUpdated.IsBound())
			m_onTweenCompleted.Execute();


		if (m_multiplier > 0.0f)
		{
			const float multiplier = FMath::Lerp(m_multiplier, 0.0f, m_elapsedTime / m_duration);
			target *= multiplier;
		}

		*var = FMath::Lerp(*var, target, s_shakeInterpolateSpeed * delta);
	//	*var = FMath::RInterpTo(*var, target, delta, SHAKE_LERP_SPEED);
		ElapseShakeData();
	}

	void ElapseShakeData()
	{
		m_elapsedTime += m_rate;

		if (m_duration > 0.0f && m_elapsedTime >= m_duration)
			Stop();
	}

	void Cleanup();

private:
	
	friend class CTweenManager;

	TweenUpdated m_onTweenUpdated;
	TweenCompleted m_onTweenCompleted;
	FTimerManager* m_timerManager = nullptr;
	FTimerDelegate m_delegate;
	FTweenPackFloat* m_floatPack = nullptr;
	FTweenPackFVector* m_vectorPack = nullptr;
	FTweenPackFRotator* m_rotatorPack = nullptr;
	FTimerHandle m_handle;
	ETweenEase m_ease = ETweenEase::Linear;
	ETweenType m_type = ETweenType::TypeInterpolation;
	ETweenVariableType m_variableType = ETweenVariableType::TypeFloat;
	ETweenID m_id = ETweenID::NONE;
	float m_delay = -1.0f;
	float m_rate = 0.02f;
	float m_duration = 0.0f;
	float m_elapsedTime = 0.0f;
	float m_multiplier = -1.0f;
	bool m_removeWhenFinished = false;
};

class CTweenManager
{
public:
	
	static CTween* Get(ETweenID id);
	static void StopTween(ETweenID id);

	
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	static void DebugInformation();

#endif

private:
	
	friend class CTween;
	static void RemoveTween(ETweenID id);
	static void PrintTweenMap();
	static void Clear();


private:
	static TMap<ETweenID, CTween*> s_tweenMap;
};
