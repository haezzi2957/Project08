#pragma once

#include "CoreMinimal.h"                        // 언리얼의 핵심 타입과 매크로들이 정의돼 있는 최소한의 헤더 ( 문자열, 백터, 로깅, 스마트 포인터 같은 기본자료형과 기능제공 )
#include "GameFramework/Character.h"            // 언리얼의 ACharacter 클래스 정의가 들어있는 헤더 ( 이동구현 같은거 ASpartaCharacter가 ACharacter를 상속해서 필요 )
#include "SpartaCharacter.generated.h"          // 언리어 헤더 툴의 자동생성 헤더 UCLASS/UFUNCTION/UPROPERTY 등 리플렉션 매크로 처리 결과를 담고 있음(항상 마지막에 작성 해야함 매우중요 )

class USpringArmComponent; // 카메라 연결선 전방선언
class UCameraComponent;    // 카메라 존재 전방선언
class UWidgetComponent;
struct FInputActionValue;  // 입력값을 담아 전달하는 구조체 타입
// U가 앞에 붙은 친구는 class 기반. // F가 앞에 붙은 친구는 구조체타입이라서 struct 

UCLASS() // 언리얼 리플렉션 시스템에 클래스를 등록하는 매크로 !
class UNREALC_API ASpartaCharacter : public ACharacter // class UNREALC_API 에서 ASpartaCharacter 를 public ACharacter 로 상속해서 캐릭터 기능을 이어받음
{
	GENERATED_BODY() // 컴파일 과정에서 언리얼 헤더툴에 의해 클래스나 구조체에 필요한 자동 생성코드를 삽입하는 역할. !

public:
	ASpartaCharacter(); // 생성자 선언 ( 클래스 이름과 동일하고 () 가 붙어있으므로 생성자임을 알아봤음 )

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") //  BlueprintReadOnly 알아올것 // UPROPERTY() 매크로와 함께 사용되는 지정자
	// UPROPERTY 에디터 디테일창에 보이게 해주는 역할
	USpringArmComponent* SpringArmComp;
	/*앞에 있는게 타입, 뒤에 있는게 변수*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UFUNCTION(Blueprintpure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float Health;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// protected로 자기 자신이나 파생 클래스에서만 접근할 수 있도록 하고, virtual 가상 함수로 선언해 void 반환 없이 SetupPlayerInputComponent에서
	// 입력 바인딩 캐릭터 이동을 처리하며, 매개변수로 UInputComponent* PlayerInputComponent를 받아 입력값을 연결하고, override로 부모 클래스 함수를 재정의한다.
	virtual float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EnentInstigator, AActor* DamageCauser) override;

	UFUNCTION() // 언리얼 엔진의 함수 리플렉션 매크로 !
	void Move(const FInputActionValue& value);          // 이동구현 앞뒤좌우
	UFUNCTION()
	void StartJump(const FInputActionValue& value);     // 점프 시작
	UFUNCTION()
	void StopJump(const FInputActionValue& value);      // 점프 종료
	UFUNCTION()
	void Look(const FInputActionValue& value);          // 마우스 시야회전
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);   // 달리기 시작
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);    // 달리기 끝

	void OnDeath();
	void UpdateOverheadHP();

private:
	float NormalSpeed;                                  // 기본 이동속도
	float SprintSpeedMultiplier;                        // 달리기 속도(곱해줄 값)
	float SprintSpeed;                                  // 실제 달리기 속도 ( 기본 속도 * 계산된 값 )
};

