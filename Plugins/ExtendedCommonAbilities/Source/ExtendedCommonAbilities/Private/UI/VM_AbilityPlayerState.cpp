// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_AbilityPlayerState.h"

#include "AbilityPlayerState.h"
#include "Teams/CommonTeamStatics.h"


void UVM_AbilityPlayerState::SetPlayerState(AAbilityPlayerState* NewPlayerState)
{
	AAbilityPlayerState* OldPlayerState = PlayerState.Get();
	if (OldPlayerState != NewPlayerState)
	{
		if (OldPlayerState)
		{
			OldPlayerState->OnTeamChangedEvent.RemoveAll(this);
		}

		PlayerState = NewPlayerState;

		if (PlayerState.IsValid())
		{
			PlayerState->OnTeamChangedEvent.AddUObject(this, &UVM_AbilityPlayerState::OnTeamChanged);
		}

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetPlayerState);
	}
}

int32 UVM_AbilityPlayerState::GetTeamId() const
{
	return PlayerState.IsValid() ? PlayerState->GetTeamId() : INDEX_NONE;
}

void UVM_AbilityPlayerState::OnTeamChanged(UObject* TeamAgent, int32 NewTeamId, int32 OldTeamId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetTeamId);
}
