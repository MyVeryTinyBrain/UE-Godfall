// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTable.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BlackboardKeyType_StateTable.generated.h"

class UBlackboardComponent;

/**
 * 
 */
UCLASS(EditInlineNew, meta = (DisplayName = "StateTable"))
class GODFALL_API UBlackboardKeyType_StateTable : public UBlackboardKeyType
{
	GENERATED_UCLASS_BODY()

public:
	//UBlackboardKeyType_StateTable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	typedef FStateTable FDataType;
	static const FDataType InvalidValue;

	static FStateTable GetValue(const UBlackboardKeyType_StateTable* KeyOb, const uint8* RawData);
	static bool SetValue(UBlackboardKeyType_StateTable* KeyOb, uint8* RawData, const FStateTable& Value);

	virtual EBlackboardCompare::Type CompareValues(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock,
		const UBlackboardKeyType* OtherKeyOb, const uint8* OtherMemoryBlock) const override;

protected:
	virtual FString DescribeValue(const UBlackboardComponent& OwnerComp, const uint8* RawData) const override;
	virtual bool TestTextOperation(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock, ETextKeyOperation::Type Op, const FString& OtherString) const override;
	virtual void Clear(UBlackboardComponent& OwnerComp, uint8* MemoryBlock) override;
	virtual bool IsEmpty(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock) const override;
	virtual void CopyValues(UBlackboardComponent& OwnerComp, uint8* MemoryBlock, const UBlackboardKeyType* SourceKeyOb, const uint8* SourceBlock) override;
	
private:
	UPROPERTY(VisibleAnywhere, DisplayName = "StateTable")
	FStateTable mStateTable;
};
