// Inan Evin, Copyright 2021

#pragma once
#include <array>

#include "UObject/NameTypes.h"

UENUM()
enum ETweenEase
{
	Linear,
	Shake,
	EaseIn,
	EaseOut,
	SineIn,
	SineOut
};

UENUM()
enum class ETweenVariableType : uint8
{
	TypeFloat,
	TypeFVector,
	TypeFRotator
};

enum ETweenType : uint8
{
	TypeInterpolation,
	TypeShake
};


UENUM()
enum class ETweenID : uint8
{
	TWEENIDEXAMPLE_1 = 0,
	TWEENIDEXAMPLE_2 = 1
	TWEENIDEXAMPLE_3 = 2,
	NONE = 100
};

DECLARE_DELEGATE(TweenUpdated)
DECLARE_DELEGATE(TweenCompleted)

extern float s_shakeInterpolateSpeed;
extern float s_shakeBaseResetSpeed;