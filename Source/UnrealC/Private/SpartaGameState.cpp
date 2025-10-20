#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState() // ������, �߰��� �Ʒ��� �׳� �ʱⰪ ���� �ʱ�ȭ �Ѱ�.
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.0f;
	CurrentLevelIndex = 0;
	//MaxLevels = 3;  // ���̺�� ���� �Ͽ������� ������ ���ʿ��ؼ� �ּ�ó���Ͽ���.

	CurrentWave = 0;
	MaxWave = 3;
}

void ASpartaGameState::BeginPlay() // ��������
{
	Super::BeginPlay();            // �θ� Ŭ������ BeginPlay ���� ����

	StartWave();                   // ù��° ���̺� ����

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle, this, &ASpartaGameState::UpdateHUD, 0.1f, true
	);  // GetWorldTimerManager() Ÿ�̸� ���� ��ü  // HUDUpdateTimerHandle Ÿ�̸� �ĺ��� �ڵ�(�̸�ǥ)
}       // 0.1f �� ������ ���� // true �ݺ����� false �ѹ��� ���� 
		// this �� �Լ��� ������ ��ü�� ������? �� ASpartaGameState Ŭ���� �� ���̴ϱ� this = ���� �� GameState��ü �ڱ��ڽ�
 
//int32 ASpartaGameState::GetScore() const // int32�� 32��Ʈ ����Ÿ��, �׿��� GameState���� ����(Score)�� �ܺο��� ������ �Լ�
//{
//	/*return Score;*/ // ���ھ ��ȯ
//	return 100; // ���ھ ��ȯ
//}

void ASpartaGameState::AddScore(int32 Amount) // AddScore ������ ���ϴ� �Լ� // Amount ���� ������ �� // int32 ���� ������ ������ ����
{
	if (UGameInstance* GameInstance = GetGameInstance()) // ���� �������� ������ �ν��Ͻ��� �����ͼ� GameInstance �����Ϳ� ������.
	{
		// �⺻ ���� ���Ͻ���, �������� �������� ��ȯ�ؼ� �ű��ִ� �� Ŀ���� �Լ��� ���� ����
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance); 
	
		// ���� ���� SpartaGameInstance�� ������ �����ϰ� �۵����̶�� �� �ȿ��ִ� ���� ���� �Լ��� ����ȴ�.
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::StartWave() // GameState�� ���̺긦 �����Ҷ� ������ �Լ� �����
{
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!"), CurrentWave + 1); // �ƿ��α׿� ����, �ý�Ʈ ���̺� ���� �߰���

	if (GEngine) // ���� ��ܿ� �޼��� ���
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Wave %d Start!"), CurrentWave + 1));
	}

	// ���� ���忡�� ù��° �÷��̾� ��Ʈ�ѷ��� ã�Ƽ� �����Ѵٸ� �װ� PlayerController ������ �����ϰ� �Ʒ� ������ ����
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()) 
	{
		// �⺻ ��Ʈ�ѷ��� �������� ���ĸ�Ÿ ���� ��Ʈ�ѷ��� �ٲ㼭 ����� Ȯ���� ����ϴ� �ڵ�
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD(); // SpartaPlayerController�� ������ �ִ� ShowGameHUD �Լ��� ����

			// GetHUDWidget�� ��ȯ�� HUD������ �ּҸ� ����Ű�� �װ� �����Ѵٸ� HUDWidget ������ �����Ͽ� ���
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// ���� ���� ���� GetFirstPlayerController �����ϴٸ� �� ��ü�� �ּҸ� APlayerController* Ÿ�� ������ PC ����´�
				if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
				{
					// PC�� ASpartaPlayerController Ÿ������ �����ϰ� Cast �ϰ� �� �ּҸ� SpartaPC ������ ������ ����
					if (ASpartaPlayerController* SpartaPC = Cast<ASpartaPlayerController>(PC))
					{
						if (UUserWidget* HUD = SpartaPC->GetHUDWidget()) // SpartaPC�� GetHUDWidget()�� ��ȯ�� ������ �����ϴٸ� ���ּҸ� HUD �� ��´�
						{
							// HUD ������ PlayGameWave ��� �Լ��� �ִٸ� �װ��� ���� ���Ѷ�
							HUD->CallFunctionByNameWithArguments(TEXT("PlayGameWave"), *GLog, nullptr, true);
							// CallFunctionByNameWithArguments �̸��� ã�� �����ϴ� �Լ�
							// *GLog �𸮾��� ���� �α� ��ü (�α׸� ����� �� �ִ� ���â) �Լ��� �����Ҷ� ����� ������ �ܼ� �α׿� ���
							// nullptr �̶� �ƹ��͵�����Ű�� �ʴ´�. ���⼱ ���峪 ������Ʈ ���ؽ�Ʈ�� ���� �������� �ʰڴ�.
						}
					}
				}
			}
		}
	}

	//UGameInstance* GameInstance = GetGameInstance();
	//bool GameInstance = GetGameInstance();
	//int GameInstance = GetGameInstance();

	if (UGameInstance* GameInstance = GetGameInstance()) // �帧�� �˰ڴµ� �� ����� �̸����� ������ ������ ���� �ٸ������ΰ� �˰�� ����.
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Starting Wave %d"), CurrentWave + 1);

	SpawnedCoinCount = 0;   // �� ���� ����
	CollectedCoinCount = 0; // ȹ���� ���� ����

	TArray<AActor*> FoundVolumes; // FoundVolumes�� ���� ���� AActor�� ���� �� �ִ� ����(�迭)
	// ���� ����(GetWorld) �ȿ��� ASpawnVolume Ÿ���� ���͵��� ���� ã�� �� �ּҵ��� FoundVolumes �迭�� ����.
	// ��¥�� ã�°� GetAllActorsOfClass ������ ������ ã���� ������ ����ִ°� StaticClass
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40 + (CurrentWave * 10); // ���̺갡 �����Կ� ���� �����۽��� ������ 10���� �þ

	for (int32 i = 0; i < ItemToSpawn; i++) // �����ؾ��� ������ ������ŭ �ݺ� ����
	{
		if (FoundVolumes.Num() > 0) // FoundVolumes �迭 �ȿ� ���Ͱ� �ϳ��� �ִٸ�(true) �� ã�� ���������� 1�� �̻��̸� �����ض�
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]); // �迭�� ù��° ���͸� ASpawnVolumeŸ������ Cast�ؼ� ���ּҸ� ���������� ����
			
			// ���������� ��ȿ�ϴٸ� ���� �������� �����ϰ�, �װ� �����̶�� ���� ī��Ʈ�� �ϳ� �ø���.
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	// ������ Ÿ�̸� �Ŵ����� LevelTimerHandle ��� Ÿ�̸Ӹ� ���, LevelDuration�ʰ� ��������
	// ASpartaGameStatedml �� OnWaveTimeUp() �Լ��� �ѹ� ȣ���ض�. false �� �ѹ��� ����
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle, this, &ASpartaGameState::OnWaveTimeUp, LevelDuration, false
	);
}

// �ð��� �� �Ǹ� EndWave�� �����Ű�� ���� �Լ�
void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

// && �� �� ������ (AND) �� �������� ��� ���϶��� ��ü�� ���̵ȴ�.
// SpawnedCoinCount > 0 -> ������ ������ �����Ǿ��ִ�. // CollectedCoinCount >= SpawnedCoinCount ������ ������ ���� �ֿ���
// �� ������ �ϳ� �ֿﶧ���� CollectedCoinCount�� 1�ø��� �ֿܼ� ���ΰ����� �α׷� ���. ���� ������ ������ ���� �ֿ�� ���̺� ����
void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount)

		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			EndWave();
	    }
}

void ASpartaGameState::EndWave() // ���� ���̺� �Լ� ȣ��
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle); // ���̺� ���ѽð� Ÿ�̸Ӹ� ����
	/*GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);*/
	CurrentWave++; // ���� ���̺�� �Ѿ

	if (CurrentWave >= MaxWave) // ���� ���̺갡 �ִ���̺꿡 ����, �ʰ��ߴٸ� ������ ����
	{
		OnGameOver();
	}

	else // ���� �������� ������ �ٷ� ���� �������� ���۵ɶ� StartWave() �� ����
	{
		GetWorldTimerManager().SetTimerForNextTick([this]() // ����ƽ ���� �̷�� ������ ���� ���̺갡 ������ �����ȵ� �����ϰ� �Ѿ�� ����
			{
				StartWave();
			});
	}

}

void ASpartaGameState::OnGameOver() // ���ӿ����� ȣ��Ǵ� �Լ� ����
{
	// ���� ���忡�� GetFirstPlayerController�� ã�Ƽ� ���ּҸ� APlayerController* Ÿ�Ժ��� PlayerController�� ����
	// GetFirstPlayerController �� ? �𸮾� ������ �ִ� ���� �÷��̾� ��Ʈ�ѷ��� ù��° �÷��̾� ��Ʈ�ѷ� �̱۸�� �̱⶧���� �̷��� �ᵵ����
	// <ASpartaPlayerController>(PlayerController) �̺κ��� �θ�� �ڽ����� ���°Ը��� �θ��� ����� �������� �ڽĿ� �߰���
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true); // �Ͻ����� �Լ� true �־����� ����� ��� �ǹ�
			SpartaPlayerController->ShowMainMenu(true); // ���� �޴��� UI ȭ�鿡 ���� �Լ� true �̴� �޴�ǥ��.
		}
	}
}

void ASpartaGameState::UpdateHUD() // ASpartaGameStateŬ������ ����� UpdateHUD() �Լ��� ����
{
	// GetFirstPlayerController �� ã�� PlayerController �� ����
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		// PlayerController�� �����ϰ� ASpartaPlayerController���� Cast �ϰ� SpartaPlayerController�� ����
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		{
			// SpartaPlayerController�� ������ �ִ� HUD ������ �ҷ����� �װ� �����Ѵٸ� HUDWidget �������� �Ʒ��ڵ� ����
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// HUD �ȿ� �̸��� Time �� �ؽ�Ʈ ����� ã�� �����ϸ� TimeText ������ ������ �ؽ�Ʈ ������ �ٲ� �غ�
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					// LevelTimerHandle�� �����Ǵ� Ÿ�̸��� �����ð��� �����ͼ� RemainingTime �� ����
					// LevelTimerHandle ������ ������ ���� ���� �ð��� �����ϴ� Ÿ�̸�
					// GetTimerRemaining �� Ÿ�̸ӿ� ���� �ð��� float ������ �˷��ִ� �Լ�
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					// FString::Printf ���� ���ڿ��� ����� FText::FromString �� �׹��ڿ��� UI�� �� �� �ְ� �ٲ�
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				// HUD���� �ȿ��� Score�� �ؽ�Ʈ����� ã�� �װ� UTextBlock �̸� ScoreText������ ������ �Ʒ��ڵ� ����
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					// ���忡 ����� �����ν��Ͻ� ��ü�� �����ϴٸ� ���ּҸ� UGameInstance* Ÿ�Ժ��� GameInstance�� ��� �Ʒ��ڵ� ����
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						// GameInstance�� �������� USpartaGameInstance Ÿ������ ����ȯ�� SpartaGameInstance ������ ����
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						// SpartaGameInstance �� TotalScore ���� ���ڿ� ���·� ����� HUD���� ������ ǥ��
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}														
					}
				}
				// HUD ���� �ȿ��� �̸��� Wave�� �ؽ�Ʈ ����� ã�� �װ� ������ UTextBlock �̶�� WaveText ������ ����
				// �׸��� ���� ���̺� ��ȣ CurrentWave + 1 �� ���ڿ��� ��ȯ�� HUD�� ǥ��
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave + 1)));
				}
			}
		}
	}
}


