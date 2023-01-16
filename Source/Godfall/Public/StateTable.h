#pragma once

#include "Godfall.h"
#include "StateTable.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FStateTableElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Available = true;

	bool operator==(const FStateTableElement& rhs) const
	{
		return (Name == rhs.Name) && (Available == rhs.Available);
	}
};

USTRUCT(BlueprintType, Blueprintable)
struct FStateTable
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStateTableElement> States;

	UPROPERTY()
	bool Invalid = false;

	FStateTable() {}

	FStateTable(bool Valid)
	{
		this->Invalid = !Valid;
	}

	bool AddState(const FName& Name, bool Available = true)
	{
		if (ContainsStates(Name))
		{
			return false;
		}

		FStateTableElement element;
		element.Available = Available;
		element.Name = Name;
		States.Add(element);
		return true;
	}

	void AddStates(const TArray<FName>& Names)
	{
		for (auto& name : Names)
		{
			AddState(name);
		}
	}

	bool RemoveState(const FName& Name)
	{
		bool containsName;
		int32 index = GetIndex(Name, containsName);
		
		if (containsName)
		{
			States.RemoveAt(index);
			return true;
		}

		return false;
	}

	bool ContainsStates(const FName& Name) const
	{
		for (const auto& element : States)
		{
			if (element.Name == Name)
			{
				return true;
			}
		}

		return false;
	}

	int32 GetIndex(const FName& Name, bool& containsName) const
	{
		for (int32 i = 0; i < States.Num(); ++i)
		{
			const auto& element = States[i];
			if (element.Name == Name)
			{
				containsName = true;
				return i;
			}
		}

		containsName = false;
		return -1;
	}

	bool GetAvailable(const FName& Name, bool& containsName) const
	{
		for (const auto& element : States)
		{
			if (element.Name == Name)
			{
				containsName = true;
				return element.Available;
			}
		}

		containsName = false;
		return false;
	}

	void SetAvailable(const FName& Name, bool available, bool& containsName)
	{
		for (auto& element : States)
		{
			if (element.Name == Name)
			{
				containsName = true;
				element.Available = available;
			}
		}

		containsName = false;
	}

	bool TrySetAvailable(const FName& Name, bool available, bool& containsName)
	{
		for (auto& element : States)
		{
			if (element.Name == Name)
			{
				containsName = true;

				if (element.Available != available)
				{
					element.Available = available;
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		containsName = false;
		return false;
	}

	FString ToString() const
	{
		FString describe;

		for (int32 i = 0; i < States.Num(); ++i)
		{
			const auto& element = States[i];

			describe.Append(FString::Printf(TEXT("%d: [%s, %s]"), i, *element.Name.ToString(), element.Available ? TEXT("true") : TEXT("false")));
		}

		return describe;
	}

	bool IsEmpty() const
	{
		return States.IsEmpty();
	}

	void Clear()
	{
		States.Empty();
	}

	bool operator==(const FStateTable& rhs) const
	{
		return (Invalid == rhs.Invalid) && (States == rhs.States);
	}
};
