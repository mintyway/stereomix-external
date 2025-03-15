// Copyright Studio Surround. All Rights Reserved.


#include "SMVideoPlayer.h"

#include "IMediaEventSink.h"

void USMVideoPlayer::HandleMediaPlayerEvent(EMediaEvent EventType)
{
	Super::HandleMediaPlayerEvent(EventType);

	switch (EventType)
	{
		case EMediaEvent::MediaOpened:
			OnMediaOpened.Broadcast();
			break;
		case EMediaEvent::PlaybackEndReached:
			OnPlaybackEnd.Broadcast();
			break;
		case EMediaEvent::PlaybackResumed:
			OnPlaybackResumed.Broadcast();
			break;
		case EMediaEvent::PlaybackSuspended:
			OnPlaybackPaused.Broadcast();
			break;
		default:
			break;
	}
}
