// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonVideoPlayer.h"
#include "SMVideoPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMediaPlayerEventReceiveEvent);

/**
 * Video Player Blueprint Implementation
 */
UCLASS(DisplayName = "StereoMix Video Player")
class STEREOMIX_API USMVideoPlayer : public UCommonVideoPlayer
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable, DisplayName = "Play", Category = "Video Player")
	void BP_SetVideo(UMediaSource* NewVideo) { SetVideo(NewVideo); }

	UFUNCTION(BlueprintCallable, DisplayName = "Seek", Category = "Video Player")
	void BP_Seek(float PlaybackTime) { Seek(PlaybackTime); }

	UFUNCTION(BlueprintCallable, DisplayName = "Close", Category = "Video Player")
	void BP_Close() { Close(); }

	UFUNCTION(BlueprintCallable, DisplayName = "Set Playback Rate", Category = "Video Player")
	void BP_SetPlaybackRate(float PlaybackRate) { SetPlaybackRate(PlaybackRate); }

	UFUNCTION(BlueprintCallable, DisplayName = "Set Looping", Category = "Video Player")
	void BP_SetLooping(bool bShouldLoopPlayback) { SetLooping(bShouldLoopPlayback); }
	
	UFUNCTION(BlueprintCallable, DisplayName = "Set Is Muted", Category = "Video Player")
	void BP_SetIsMuted(bool bInIsMuted) { SetIsMuted(bInIsMuted); }

	UFUNCTION(BlueprintCallable, DisplayName = "Set Should Match Size", Category = "Video Player")
	void BP_SetShouldMatchSize(bool bInMatchSize) { SetShouldMatchSize(bInMatchSize); }
	
	UFUNCTION(BlueprintCallable, DisplayName = "Play", Category = "Video Player")
	void BP_Play() { Play();}

	UFUNCTION(BlueprintCallable, DisplayName = "Reverse", Category = "Video Player")
	void BP_Reverse() { Reverse(); }

	UFUNCTION(BlueprintCallable, DisplayName = "Pause", Category = "Video Player")
	void BP_Pause() { Pause(); }

	UFUNCTION(BlueprintCallable, DisplayName = "Play From Start", Category = "Video Player")
	void BP_PlayFromStart() { PlayFromStart(); }

	UFUNCTION(BlueprintPure, DisplayName = "Get Video Duration", Category = "Video Player")
	float BP_GetVideoDuration() const { return GetVideoDuration(); }

	UFUNCTION(BlueprintPure, DisplayName = "Get Playback Time", Category = "Video Player")
	float BP_GetPlaybackTime() const { return GetPlaybackTime(); }

	UFUNCTION(BlueprintPure, DisplayName = "Get Playback Rate", Category = "Video Player")
	float BP_GetPlaybackRate() const { return GetPlaybackRate(); }

	UFUNCTION(BlueprintPure, DisplayName = "Is Looping", Category = "Video Player")
	bool BP_IsLooping() const { return IsLooping(); }

	UFUNCTION(BlueprintPure, DisplayName = "Is Paused", Category = "Video Player")
	bool BP_IsPaused() const { return IsPaused(); }

	UFUNCTION(BlueprintPure, DisplayName = "Is Playing", Category = "Video Player")
	bool BP_IsPlaying() const { return IsPlaying(); }

	UFUNCTION(BlueprintPure, DisplayName = "Is Muted", Category = "Video Player")
	bool BP_IsMuted() const { return IsMuted(); }

	UPROPERTY(BlueprintAssignable, DisplayName = "On Media Opened", Category = "Video Player")
	FMediaPlayerEventReceiveEvent OnMediaOpened;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Playback End", Category = "Video Player")
	FMediaPlayerEventReceiveEvent OnPlaybackEnd;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Playback Resumed", Category = "Video Player")
	FMediaPlayerEventReceiveEvent OnPlaybackResumed;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Playback Paused", Category = "Video Player")
	FMediaPlayerEventReceiveEvent OnPlaybackPaused;

	virtual void HandleMediaPlayerEvent(EMediaEvent EventType) override;
};
