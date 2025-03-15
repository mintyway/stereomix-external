// Copyright Studio Surround. All Rights Reserved.

#pragma once

UENUM(BlueprintType)
enum class ESMShaderStencil : uint8
{
	NonOutline = 0,
	BlackOutline = 1,
	GreenOutline = 2,
	RedOutline = 3,
	Hit = 254,
	Max UMETA(Hidden)
};
