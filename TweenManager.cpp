// Inan Evin, Copyright 2021


#include "TweenManager.h"

TMap<ETweenID, CTween*> CTweenManager::s_tweenMap;

void CTween::Stop()
{
	m_timerManager->ClearTimer(m_handle);

	if (m_onTweenCompleted.IsBound())
		m_onTweenCompleted.Execute();

	if (m_removeWhenFinished)
		CTweenManager::RemoveTween(m_id);
}

bool CTween::IsActive() const
{
	return m_timerManager->IsTimerActive(m_handle);
}


void CTween::OnStepInterpolate()
{
	switch (m_variableType)
	{
	case ETweenVariableType::TypeFloat:
		StepInterpolate<float>(m_floatPack->m_var, m_floatPack->m_fromOrAmount, m_floatPack->m_toOrSpeed);
		break;
	case ETweenVariableType::TypeFVector:
		StepInterpolate<FVector>(m_vectorPack->m_var, m_vectorPack->m_fromOrAmount, m_vectorPack->m_toOrSpeed);
		break;
	case ETweenVariableType::TypeFRotator:
		StepInterpolate<FRotator>(m_rotatorPack->m_var, m_rotatorPack->m_fromOrAmount, m_rotatorPack->m_toOrSpeed);
		break;
	}
}

void CTween::OnStepShake()
{
	switch (m_variableType)
	{
	case ETweenVariableType::TypeFloat:
		StepShake<float>(m_floatPack->m_var, m_floatPack->m_fromOrAmount, m_floatPack->m_toOrSpeed, &m_floatPack->m_baseValue);
		break;
	case ETweenVariableType::TypeFVector:
		StepShake<FVector>(m_vectorPack->m_var, m_vectorPack->m_fromOrAmount, m_vectorPack->m_toOrSpeed, &m_vectorPack->m_baseValue);
		break;
	case ETweenVariableType::TypeFRotator:
		StepShake<FRotator>(m_rotatorPack->m_var, m_rotatorPack->m_fromOrAmount, m_rotatorPack->m_toOrSpeed, &m_rotatorPack->m_baseValue);
		break;
	}
}

void CTween::Cleanup()
{
	switch (m_variableType)
	{
	case ETweenVariableType::TypeFloat:
		if (m_floatPack != nullptr)
		{
			delete m_floatPack;
			m_floatPack = nullptr;
		}
		break;
	case ETweenVariableType::TypeFRotator:
		if (m_rotatorPack != nullptr)
		{
			delete m_rotatorPack;
			m_rotatorPack = nullptr;
		}
		break;
	case ETweenVariableType::TypeFVector:
		if (m_vectorPack != nullptr)
		{
			delete m_vectorPack;
			m_vectorPack = nullptr;
		}
		break;
	}
}


CTween* CTweenManager::Get(ETweenID id)
{
	if (!s_tweenMap.Contains(id))
	{
		auto* tw = new CTween(id, &UGameInstanceMain::Get()->GetTimerManager());
		s_tweenMap.Add(id, tw);
		return s_tweenMap[id];
	}
	else
		return s_tweenMap[id];
}

void CTweenManager::RemoveTween(ETweenID id)
{
	if (s_tweenMap.Contains(id))
		s_tweenMap.Remove(id);
	else
	{
	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Tween to remove is not in tween map. %s"), *UEnum::GetValueAsString(id));
	#endif
	}
}

void CTweenManager::PrintTweenMap()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	for (auto& t : s_tweenMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("Print Map - Tween ID: %d, String: %s"), t.Value->m_id, *UEnum::GetValueAsString(t.Value->m_id));
	}
#endif
}

void CTweenManager::Clear()
{
	for (auto& t : s_tweenMap)
	{
		delete t.Value;
	}

	s_tweenMap.Empty();
}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

void CTweenManager::DebugInformation()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	int32 index = 0;
	for(auto& t: s_tweenMap)
	{
		FString tween  = "Tween: " + UEnum::GetValueAsString(t.Value->m_id) + " Is Active: " + (t.Value->IsActive() ? "Yes" : "No");
		GEngine->AddOnScreenDebugMessage(200, 0, FColor::Green, tween);
		index++;
	}
#endif
}

#endif
void CTweenManager::StopTween(ETweenID id)
{
	if (s_tweenMap.Contains(id))
		s_tweenMap[id]->Stop();
	else
	{
	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Tween to stop is not in tween map. %s"), *UEnum::GetValueAsString(id));
	#endif
	}
	
}
