#include "GodfallUtil.h"

bool FParabolaCoefficient3::UpdateStartPoint(const FVector& p)
{
	FParabolaCoefficient3 newValue;
	if (GodfallUtil::Vector3D::CalcParabolaCoefficient3(p, CrossPointHeight, P2, newValue))
	{
		*this = newValue;
		return true;
	}

	return false;
}

bool GodfallUtil::Vector3D::CalcParabolaCoefficient3(const FVector& p0, double crossPointHeight, const FVector& p2, FParabolaCoefficient3& out)
{
	out.P0 = p0;
	out.P1 = (p0 + p2) * 0.5f + FVector(0.0f, 0.0f, crossPointHeight);
	out.P2 = p2;

	out.P0XYDistance = 0.0;
	out.P1XYDistance = FVector2D::Distance(FVector2D(out.P0.X, out.P0.Y), FVector2D(out.P1.X, out.P1.Y));
	out.P2XYDistance = out.P1XYDistance + FVector2D::Distance(FVector2D(out.P1.X, out.P1.Y), FVector2D(out.P2.X, out.P2.Y));

	FVector2D p2d0, p2d1, p2d2;
	p2d0 = FVector2D(out.P0XYDistance / out.P2XYDistance, out.P0.Z);
	p2d1 = FVector2D(out.P1XYDistance / out.P2XYDistance, out.P1.Z);
	p2d2 = FVector2D(out.P2XYDistance / out.P2XYDistance, out.P2.Z);
	if (!Vector2D::CalcParabolaCoefficient2(p2d0, p2d1, p2d2, out.Coefficient2))
	{
		return false;
	}

	out.CrossPointHeight = crossPointHeight;

	return true;
}

FVector GodfallUtil::Vector3D::Parabola(const FParabolaCoefficient3& coefficient, double t)
{
	double x = FMath::Lerp(coefficient.P0.X, coefficient.P2.X, t);
	double y = FMath::Lerp(coefficient.P0.Y, coefficient.P2.Y, t);
	double z = Vector2D::Parabola(coefficient.Coefficient2, t).Y;
	return FVector(x, y, z);
}

bool GodfallUtil::Vector2D::CalcParabolaCoefficient2(const FVector2D& p0, const FVector2D& p1, const FVector2D& p2, FParabolaCoefficient2& out)
{
	const double& a = p1.X;
	const double& b = p1.Y;
	const double& c = p2.X;
	const double& d = p2.Y;
	const double& f = p0.X;
	const double& g = p0.Y;

	double A0 = g * (a * a - c * c) + d * (f * f - a * a) + b * (c * c - f * f);
	double A1 = 2 * ((g - d) * a + (b - g) * c + (d - b) * f);
	if (A1 == 0) return false;
	double A = A0 / A1;

	double C0 = b - d;
	double C1 = a * a - c * c + 2 * A * (c - a);
	if (C1 == 0) return false;
	double C = C0 / C1;

	double B = d - C * (c - A) * (c - A);

	out.A = A;
	out.B = B;
	out.C = C;
	return true;
}

FVector2D GodfallUtil::Vector2D::Parabola(const FParabolaCoefficient2& coefficient, double x)
{
	const double& A = coefficient.A;
	const double& B = coefficient.B;
	const double& C = coefficient.C;
	double y = C * (x - A) * (x - A) + B;
	return FVector2D(x, y);
}

FVector2D GodfallUtil::Vector2D::EndPointInRect(float halfWidth, float halfHegith, const FVector2D& point)
{
	static FVector2D RightVector = FVector2D(1.0f, 0.0f);
	static float Dot45 = FMath::Cos(FMath::DegreesToRadians(45.0f));
	static float Dot135 = FMath::Cos(FMath::DegreesToRadians(135.0f));

	if (halfWidth <= 0.0f || halfHegith <= 0.0f) return FVector2D::ZeroVector;
	if (halfWidth * halfHegith == 0.0f) return FVector2D::ZeroVector;
	if (point.X == 0.0f && point.Y == 0.0f) return FVector2D::ZeroVector;

	float dot = FVector2D::DotProduct(FVector2D(1.0f, 0.0f), point.GetSafeNormal());
	FVector2D leftTop = FVector2D(-halfWidth - 1.0f, +halfHegith + 1.0f);
	FVector2D rightTop = FVector2D(+halfWidth + 1.0f, +halfHegith + 1.0f);
	FVector2D leftBottom = FVector2D(-halfWidth - 1.0f, -halfHegith - 1.0f);
	FVector2D rightBottom = FVector2D(+halfWidth + 1.0f, -halfHegith - 1.0f);
	FVector2D seg[2];
	
	if (dot > Dot45)
	{
		// Right segement
		seg[0] = rightTop;
		seg[1] = rightBottom;
	}
	else if (dot > Dot135)
	{
		if (point.Y > 0.0f)
		{
			// Top segement
			seg[0] = leftTop;
			seg[1] = rightTop;
		}
		else
		{
			// Bottom segement
			seg[0] = leftBottom;
			seg[1] = rightBottom;
		}
	}
	else
	{
		// Left segement
		seg[0] = leftTop;
		seg[1] = leftBottom;
	}

	float segLen = halfWidth * halfWidth + halfHegith * halfHegith;
	FVector2D normalizedPoint = point.GetSafeNormal() * (FMath::Max(halfWidth, halfHegith) + segLen);
	FVector seg3d[4] = { FVector(seg[0].X, seg[0].Y, 0.0f), FVector(seg[1].X, seg[1].Y, 0.0f), FVector::ZeroVector, FVector(normalizedPoint.X, normalizedPoint.Y, 0.0f) };
	FVector intersection;
	if (FMath::SegmentIntersection2D(seg3d[0], seg3d[1], seg3d[2], seg3d[3], intersection))
	{
		return FVector2D(intersection.X, intersection.Y);
	}
	else
	{
		return FVector2D::ZeroVector;
	}
}

bool GodfallUtil::String::CompareNames(const TArray<FName>& A, const TArray<FName>& B, FName* equalName)
{
	for (auto& a : A)
	{
		for (auto& b : B)
		{
			if (a == b)
			{
				if (equalName)
				{
					*equalName = a;
				}

				return true;
			}
		}
	}
	return false;
}

bool GodfallUtil::String::CompareStrings(const TArray<FString>& A, const TArray<FString>& B, FString* equalString)
{
	for (auto& a : A)
	{
		for (auto& b : B)
		{
			if (a == b)
			{
				if (equalString)
				{
					*equalString = a;
				}

				return true;
			}
		}
	}
	return false;
}
