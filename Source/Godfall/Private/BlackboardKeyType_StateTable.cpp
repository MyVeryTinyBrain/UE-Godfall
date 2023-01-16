// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackboardKeyType_StateTable.h"

const UBlackboardKeyType_StateTable::FDataType UBlackboardKeyType_StateTable::InvalidValue = FStateTable(false);

UBlackboardKeyType_StateTable::UBlackboardKeyType_StateTable(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ValueSize = 0;
	bCreateKeyInstance = true;

	SupportedOp = EBlackboardKeyOperation::Text;
}

FStateTable UBlackboardKeyType_StateTable::GetValue(const UBlackboardKeyType_StateTable* KeyOb, const uint8* RawData)
{
	return KeyOb->mStateTable;
}

bool UBlackboardKeyType_StateTable::SetValue(UBlackboardKeyType_StateTable* KeyOb, uint8* RawData, const FStateTable& Value)
{
	const bool bChanged = !(KeyOb->mStateTable == Value);
	KeyOb->mStateTable = Value;
	return bChanged;
}

EBlackboardCompare::Type UBlackboardKeyType_StateTable::CompareValues(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock,
	const UBlackboardKeyType* OtherKeyOb, const uint8* OtherMemoryBlock) const
{
	const FStateTable& MyValue = GetValue(this, MemoryBlock);
	const FStateTable& OtherValue = GetValue((UBlackboardKeyType_StateTable*)OtherKeyOb, OtherMemoryBlock);
	
	return (MyValue == OtherValue) ? EBlackboardCompare::Equal : EBlackboardCompare::NotEqual;
}

FString UBlackboardKeyType_StateTable::DescribeValue(const UBlackboardComponent& OwnerComp, const uint8* RawData) const
{
	return mStateTable.ToString();
}

bool UBlackboardKeyType_StateTable::TestTextOperation(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock, ETextKeyOperation::Type Op, const FString& OtherString) const
{
	bool succeeded;
	bool available = mStateTable.GetAvailable(*OtherString, succeeded);

	if (!succeeded) return false;

	switch (Op)
	{
		case ETextKeyOperation::Equal:			return (available);
		case ETextKeyOperation::NotEqual:		return (!available);
		case ETextKeyOperation::Contain:		return (succeeded);
		case ETextKeyOperation::NotContain:		return (!succeeded);
		default: break;
	}

	return false;
}

void UBlackboardKeyType_StateTable::Clear(UBlackboardComponent& OwnerComp, uint8* MemoryBlock)
{
	mStateTable.Clear();
}

bool UBlackboardKeyType_StateTable::IsEmpty(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock) const
{
	return mStateTable.IsEmpty();
}

void UBlackboardKeyType_StateTable::CopyValues(UBlackboardComponent& OwnerComp, uint8* MemoryBlock, const UBlackboardKeyType* SourceKeyOb, const uint8* SourceBlock)
{
	mStateTable = ((UBlackboardKeyType_StateTable*)SourceKeyOb)->mStateTable;
}
