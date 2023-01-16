// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <EngineMinimal.h>
#include <NavigationSystem.h>
#include <NiagaraSystem.h>
#include <NiagaraFunctionLibrary.h>
#include "GodfallTags.h"
#include "GodfallEnums.h"
#include "GodfallPresets.h"

#define EditAnywhere
#define VisibleAnywhere
#define BlueprintReadOnly
#define BlueprintReadWrite

// 블루프린트에서 호출 가능한 함수로 정의합니다.
#define BlueprintCallable

#define BlueprintPure
#define BlueprintThreadSafe

// 블루프린트에서 수신 가능한 이벤트로 정의합니다.
// 해당 함수는 구현부를 정의하면 안 됩니다.
// 또한 해당 함수의 이름 + _Implementation 이라는 가상함수를 선언합니다.
// 이 가상함수에 코드로 이벤트 내용을 구현할 수 있습니다.
#define BlueprintNativeEvent

#define DisplayName
#define Category
#define Transient

#define Abstract
#define BlueprintType
#define Blueprintable

// 메타 속성을 적용할 수 있습니다.
#define Meta

// Private 접근 제한자를 노출시킬수 있습니다.
#define AllowPrivateAccess

// 사용자가 마우스를 끌 때 숫자가 경계를 벗어나는 것을 막지만 여전히 범위를 벗어나는 숫자를 입력할 수 있습니다.
#define ClampMin

// 사용자가 마우스를 끌 때 숫자가 경계를 벗어나는 것을 막지만 여전히 범위를 벗어나는 숫자를 입력할 수 있습니다.
#define ClampMax

// 설정하면 사용자가 지정된 수보다 낮은 속성을 끌 수 없게 됩니다. 
// 그러나 사용 는 입력하여 이 값보다 낮은 숫자를 설정할 수 있습니다. 대부분의 경우 UIMin및 ClampMin를 모두 사용하는 것이 좋습니다.
#define UIMin

// 설정하면 사용자가 지정된 수보다 낮은 속성을 끌 수 없게 됩니다. 
// 그러나 사용 는 입력하여 이 값보다 낮은 숫자를 설정할 수 있습니다. 대부분의 경우 UIMax및 ClampMax를 모두 사용하는 것이 좋습니다.
#define UIMax

// 다른 속성에 따라 속성을 읽기 전용에서 쓰기 가능으로 변경하는 데 사용할 수 있습니다.
#define EditCondition

// 기본적으로 EditCondition조건이 false로 평가되면 속성을 읽기 전용으로 변경합니다.
#define EditConditionHides

DECLARE_LOG_CATEGORY_EXTERN(Godfall, Log, All)
#define GODFALL_CALLINFO								(FString(__FUNCTION__) + FString(TEXT(": ")) + FString::FromInt(__LINE__))
#define GODFALL_UE_LOG(Verbosity, Str)					UE_LOG(Godfall, Verbosity, TEXT("%s %s"), *GODFALL_CALLINFO, Str)
#define GODFALL_UE_LOG_FORMAT(Verbosity, Format, ...)	UE_LOG(Godfall, Verbosity, TEXT("%s %s"), *GODFALL_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define LOG(Str)										GODFALL_UE_LOG(Log, Str)
#define LOGF(Format, ...)								GODFALL_UE_LOG_FORMAT(Log, Format, ##__VA_ARGS__)
#define LOGW(Str)										GODFALL_UE_LOG(Warning, Str)
#define LOGFW(Format, ...)								GODFALL_UE_LOG_FORMAT(Warning, Format, ##__VA_ARGS__)
#define LOGE(Str)										GODFALL_UE_LOG(Error, Str)
#define LOGFE(Format, ...)								GODFALL_UE_LOG_FORMAT(Error, Format, ##__VA_ARGS__)


#define Null (0)
#define Nullable (0)
#define CantBeNull (0)