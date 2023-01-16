// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpArea.h"
#include "GodfallUtil.h"

AJumpArea::AJumpArea()
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(GodfallTags::JumpArea);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	float width = 64.0f;
	FVector extents = FVector(width, width, width) * 0.5f;
	
	Area0 = CreateDefaultSubobject<UBoxComponent>(TEXT("Area 0"));
	Area0->SetupAttachment(RootComponent);
	Area0->SetBoxExtent(extents);
	Area0->SetRelativeLocation(GetActorForwardVector() * width * 0.5f);
	Area0->SetCollisionProfileName(GodfallPresets::OverlapAll);
	Area0->SetCanEverAffectNavigation(false);
	
	Area1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Area 1"));
	Area1->SetupAttachment(RootComponent);
	Area1->SetBoxExtent(extents);
	Area1->SetRelativeLocation(GetActorForwardVector() * width * 0.5f * -1.0f);
	Area1->SetCollisionProfileName(GodfallPresets::OverlapAll);
	Area1->SetCanEverAffectNavigation(false);
}

void AJumpArea::BeginPlay()
{
	Super::BeginPlay();

	Area0->OnComponentBeginOverlap.AddDynamic(this, &AJumpArea::OnAreaBeginOverlap);
	Area0->OnComponentEndOverlap.AddDynamic(this, &AJumpArea::OnAreaEndOverlap);

	Area1->OnComponentBeginOverlap.AddDynamic(this, &AJumpArea::OnAreaBeginOverlap);
	Area1->OnComponentEndOverlap.AddDynamic(this, &AJumpArea::OnAreaEndOverlap);
}

void AJumpArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	#ifdef WITH_EDITOR
	if (GetWorld()->IsEditorWorld() && !GetWorld()->IsPlayInEditor())
	{
		EditorTick(DeltaTime);
	}
	else
	#endif
	{
		RuntimeTick(DeltaTime);
	}
}

void AJumpArea::EditorTick(float DeltaTime)
{
	#ifdef WITH_EDITOR

	FParabolaCoefficient3 coefficient;
	if (!GodfallUtil::Vector3D::CalcParabolaCoefficient3(
		Area0->GetComponentLocation(), 
		mCrossPointHeight, 
		Area1->GetComponentLocation(), 
		coefficient))
	{
		return;
	}

	const uint32 numSegements = 20;
	for (uint32 i = 0; i < numSegements; ++i)
	{
		double t0 = double(i) / double(numSegements);
		double t1 = double(i + 1) / double(numSegements);
		FVector p0 = GodfallUtil::Vector3D::Parabola(coefficient, t0);
		FVector p1 = GodfallUtil::Vector3D::Parabola(coefficient, t1);
		DrawDebugLine(GetWorld(), p0, p1, FColor::Green);
	}

	#endif
}

void AJumpArea::RuntimeTick(float DeltaTime)
{
}

void AJumpArea::OnAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	bool isPlayer = OtherActor->Tags.Contains(GodfallTags::Player);
	if (isPlayer)
	{
		if (OverlappedComponent == Area0)
		{
			mOverlappingArea0 = true;
		}
		else if (OverlappedComponent == Area1)
		{
			mOverlappingArea1 = true;
		}

		mPlayer = Cast<ACharacter>(OtherActor);
	}
}

void AJumpArea::OnAreaEndOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bool isPlayer = OtherActor->Tags.Contains(GodfallTags::Player);
	if (isPlayer)
	{
		if (OverlappedComponent == Area0)
		{
			mOverlappingArea0 = false;
		}
		else if (OverlappedComponent == Area1)
		{
			mOverlappingArea1 = false;
		}

		if (!mOverlappingArea0 && !mOverlappingArea1)
		{
			mPlayer = nullptr;
		}
	}
}

bool AJumpArea::GetParabolaCoefficient(FParabolaCoefficient3* out) const
{
	if (!out) return false;

	if (!PlayerIsOverlapping()) return false;

	FVector p0, p2;

	// Feet location
	p0 = mPlayer->GetActorLocation() + FVector::DownVector * mPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	if (mOverlappingArea0)
	{
		p2 = Area1->GetComponentLocation();
	}
	else if (mOverlappingArea1)
	{
		p2 = Area0->GetComponentLocation();
	}
	else
	{
		return false;
	}

	return GodfallUtil::Vector3D::CalcParabolaCoefficient3(p0, mCrossPointHeight, p2, *out);
}
