#pragma once

#include "CoreMinimal.h"                        // �𸮾��� �ٽ� Ÿ�԰� ��ũ�ε��� ���ǵ� �ִ� �ּ����� ��� ( ���ڿ�, ����, �α�, ����Ʈ ������ ���� �⺻�ڷ����� ������� )
#include "GameFramework/Character.h"            // �𸮾��� ACharacter Ŭ���� ���ǰ� ����ִ� ��� ( �̵����� ������ ASpartaCharacter�� ACharacter�� ����ؼ� �ʿ� )
#include "SpartaCharacter.generated.h"          // �𸮾� ��� ���� �ڵ����� ��� UCLASS/UFUNCTION/UPROPERTY �� ���÷��� ��ũ�� ó�� ����� ��� ����(�׻� �������� �ۼ� �ؾ��� �ſ��߿� )

class USpringArmComponent; // ī�޶� ���ἱ ���漱��
class UCameraComponent;    // ī�޶� ���� ���漱��
class UWidgetComponent;
struct FInputActionValue;  // �Է°��� ��� �����ϴ� ����ü Ÿ��
// U�� �տ� ���� ģ���� class ���. // F�� �տ� ���� ģ���� ����üŸ���̶� struct 

UCLASS() // �𸮾� ���÷��� �ý��ۿ� Ŭ������ ����ϴ� ��ũ�� !
class UNREALC_API ASpartaCharacter : public ACharacter // class UNREALC_API ���� ASpartaCharacter �� public ACharacter �� ����ؼ� ĳ���� ����� �̾����
{
	GENERATED_BODY() // ������ �������� �𸮾� ������� ���� Ŭ������ ����ü�� �ʿ��� �ڵ� �����ڵ带 �����ϴ� ����. !

public:
	ASpartaCharacter(); // ������ ���� ( Ŭ���� �̸��� �����ϰ� () �� �پ������Ƿ� ���������� �˾ƺ��� )

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") //  BlueprintReadOnly �˾ƿð� // UPROPERTY() ��ũ�ο� �Բ� ���Ǵ� ������
	// UPROPERTY ������ ������â�� ���̰� ���ִ� ����
	USpringArmComponent* SpringArmComp;
	/*�տ� �ִ°� Ÿ��, �ڿ� �ִ°� ����*/
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
	// protected�� �ڱ� �ڽ��̳� �Ļ� Ŭ���������� ������ �� �ֵ��� �ϰ�, virtual ���� �Լ��� ������ void ��ȯ ���� SetupPlayerInputComponent����
	// �Է� ���ε� ĳ���� �̵��� ó���ϸ�, �Ű������� UInputComponent* PlayerInputComponent�� �޾� �Է°��� �����ϰ�, override�� �θ� Ŭ���� �Լ��� �������Ѵ�.
	virtual float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EnentInstigator, AActor* DamageCauser) override;

	UFUNCTION() // �𸮾� ������ �Լ� ���÷��� ��ũ�� !
	void Move(const FInputActionValue& value);          // �̵����� �յ��¿�
	UFUNCTION()
	void StartJump(const FInputActionValue& value);     // ���� ����
	UFUNCTION()
	void StopJump(const FInputActionValue& value);      // ���� ����
	UFUNCTION()
	void Look(const FInputActionValue& value);          // ���콺 �þ�ȸ��
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);   // �޸��� ����
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);    // �޸��� ��

	void OnDeath();
	void UpdateOverheadHP();

private:
	float NormalSpeed;                                  // �⺻ �̵��ӵ�
	float SprintSpeedMultiplier;                        // �޸��� �ӵ�(������ ��)
	float SprintSpeed;                                  // ���� �޸��� �ӵ� ( �⺻ �ӵ� * ���� �� )
};

