// Copyright Bohdon Sayre, All Rights Reserved.


#include "Targeting/ExtendedTargetingSystemTypes.h"


DEFINE_TARGETING_DATA_STORE(FExtendedTargetingTransformResultsSet)


FExtendedTargetingTransformResultsSet& FExtendedTargetingTransformResultsSet::FindOrAdd(FTargetingRequestHandle Handle)
{
	return UE::TargetingSystem::TTargetingDataStore<FExtendedTargetingTransformResultsSet>::FindOrAdd(Handle);
}

FExtendedTargetingTransformResultsSet* FExtendedTargetingTransformResultsSet::Find(FTargetingRequestHandle Handle)
{
	return UE::TargetingSystem::TTargetingDataStore<FExtendedTargetingTransformResultsSet>::Find(Handle);
}
