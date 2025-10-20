#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class UNREALC_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Components")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Components")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Components")
	float RotationSpeed;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Item|Actions")
	void ResetActorPosition();
	UFUNCTION(BlueprintPure, Category = "Item|Properties")
	float GetRotationSpeed() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Enent")
	void OnItemPickedUP();
};

// 생성자 - 메모리에 생김. 딱 한번 호출.
// PostInitializeComponents() - 컴포넌트가 완성된 직후 호출. 컴포넌트끼리 데이터 주고받기, 상호작용
// BeginPlay() - 배치 (Spawn) 직후
// Tick(float DeltaTime) - 매 프레임마다 호출됨.
// Destoyed() - 삭제 되기 직전에 호출된다.
// EndPlay() - 게임 종료, 파괴 (Destroyed()), 레벨 전환

