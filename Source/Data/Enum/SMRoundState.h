// Copyright Studio Surround. All Rights Reserved.

#pragma once

UENUM(BlueprintType, DisplayName = "StereoMix Round State")
enum class ESMRoundState : uint8
{
	None,				// 초기 상태
	Waiting,			// 플레이어들을 기다리는 상태
	LevelSequence,		// 레벨 시퀀스를 재생하는 상태
	PreRound,			// 라운드 시작 전 상태
	InRound,			// 라운드 진행 중 상태
	PostRound,			// 라운드 종료 후 상태
	Result,				// 결과 화면 표시 상태
	// TravelToNextMap,	// 다음 맵으로 이동하는 상태
	Max UMETA(Hidden)
};
