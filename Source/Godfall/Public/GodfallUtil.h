#pragma once

#include <EngineMinimal.h>
#include "GodfallUtil.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FParabolaCoefficient2
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// y = C * (X - A)^2 + B
	double A = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// y = C * (X - A)^2 + B
	double B = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// y = C * (X - A)^2 + B
	double C = 0;

	bool IsValid() const { return !(A == B && B == C && C == 0.0); }
};

USTRUCT(BlueprintType, Blueprintable)
struct FParabolaCoefficient3
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FParabolaCoefficient2 Coefficient2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector P0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector P1; // CrossPoint

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector P2;

	/*
	XYDistance = XY 평면에서의 거리
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double P0XYDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double P1XYDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double P2XYDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double CrossPointHeight;

	/* z = f(t') 일때 t'은 델타 타임을 누적한 값입니다.
	t는 0 ~ 1 사이일때 시작점과 끝점을 포함한 사이에 존재합니다. (0일때 시작점, 1일때 끝점)
	이때 t' = t + DeltaTime 입니다.
	결국 이 값은 DeltaTime에 곱해주어 거리에 따라 일정한 속도를 가지도록 하는 값입니다.
	따라서 t' = t + DeltaTime * DeltaTimeMultiplier 가 됩니다. */
	double CalDeltaTimeMultiplier(double movePerSec) const { return  movePerSec / P2XYDistance; }

	bool IsValid() const { return Coefficient2.IsValid(); }

	bool UpdateStartPoint(const FVector& p);
};

namespace GodfallUtil
{
	namespace Vector3D
	{
		bool CalcParabolaCoefficient3(const FVector& p0, double crossPointHeight, const FVector& p2, FParabolaCoefficient3& out);

		FVector Parabola(const FParabolaCoefficient3& coefficient, double t);
	}

	namespace Vector2D
	{
		bool CalcParabolaCoefficient2(const FVector2D& p0, const FVector2D& p1, const FVector2D& p2, FParabolaCoefficient2& out);

		FVector2D Parabola(const FParabolaCoefficient2& coefficient, double x);

		// 원점에서 점으로 향하는 무한한 선분과 사각형의 교점을 구합니다.
		// 사각형의 넓이가 0 또는 음수일 경우에는 영벡터를 반환합니다.
		// 점이 영벡터에 존재할 경우에는 영벡터를 반환합니다.
		FVector2D EndPointInRect(float halfWidth, float halfHeight, const FVector2D& point);
	}

	namespace Sort
	{
		template<typename T>
		void Mix(T& container, uint32 num) 
		{
			for (uint32 i = 0; i < num; ++i) 
			{
				uint32 randIndex = FMath::Rand() % num;
				std::swap(container[i], container[randIndex]);
			}
		}
	}

	struct FConeShape
	{
		FConeShape(const FVector& pivot, const FVector& direction, float radius, float halfAngle) :
			Pivot(pivot), DamageDirection(direction.SquaredLength() < FLT_EPSILON ? FVector::ForwardVector : direction), Radius(radius), HalfAngle(halfAngle)
		{
		}

		FVector Pivot;
		FVector DamageDirection;
		float Radius;
		float HalfAngle;

		bool Contains(const FVector& point) const
		{
			float dist = FVector::Distance(Pivot, point);
			if (dist > Radius) return false;

			float dot = FVector::DotProduct(DamageDirection, (point - Pivot).GetSafeNormal());
			float limitCos = FMath::Cos(FMath::DegreesToRadians(HalfAngle));
			if (dot < limitCos) return false;

			return true;
		}
	};

	namespace String
	{
		bool CompareNames(const TArray<FName>& A, const TArray<FName>& B, FName* equalName = nullptr);
		bool CompareStrings(const TArray<FString>& A, const TArray<FString>& B, FString* equalString = nullptr);
	}
}

