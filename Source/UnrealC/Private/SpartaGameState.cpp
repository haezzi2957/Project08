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
// �̺κ� �ʹ� �򰥸� ���� ������.
 
int32 ASpartaGameState::GetScore() const // int32�� 32��Ʈ ����Ÿ��, �׿��� GameState���� ����(Score)�� �ܺο��� ������ �Լ�
{
	return Score; // ���ھ ��ȯ
}

void ASpartaGameState::AddScore(int32 Amount) // AddScore ������ ���ϴ� �Լ� // Amount ���� ������ �� // int32 ���� ������ ������ ����
{
	if (UGameInstance* GameInstance = GetGameInstance()) // ���� �������� ������ �ν��Ͻ��� �����ͼ� GameInstance �����Ϳ� ������.
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!"), CurrentWave + 1); // �߰�

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Wave %d Start!"), CurrentWave + 1));
	}

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();

			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// �߰�
				if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
				{
					if (ASpartaPlayerController* SpartaPC = Cast<ASpartaPlayerController>(PC))
					{
						if (UUserWidget* HUD = SpartaPC->GetHUDWidget())
						{
							HUD->CallFunctionByNameWithArguments(TEXT("PlayGameWave"), *GLog, nullptr, true);
						}
					}
				}
			}
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Starting Wave %d"), CurrentWave + 1); // �߰�

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40 + (CurrentWave * 10); // �߰�

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
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

	//GetWorldTimerManager().SetTimer(
	//	LevelTimerHandle, this, &ASpartaGameState::OnLevelTimeUp, LevelDuration, false
	//);

	GetWorldTimerManager().SetTimer(
		LevelTimerHandle, this, &ASpartaGameState::OnWaveTimeUp, LevelDuration, false
	);
}

void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

//void ASpartaGameState::OnLevelTimeUp()
//{
//	EndLevel();
//}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount)

		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			/*EndLevel();*/
			EndWave();
	    }
}

void ASpartaGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	/*GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);*/
	CurrentWave++;

	if (CurrentWave >= MaxWave)
	{
		OnGameOver();
	}

	else
	{
		// ��� ����� ���̺� ����
		GetWorldTimerManager().SetTimerForNextTick([this]()
			{
				StartWave();
			});
	}

}

//void ASpartaGameState::EndLevel() // ���巹�� ��� X 
//{
//	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
//	//CurrentLevelIndex++;
//
//	if (UGameInstance* GameInstance = GetGameInstance())
//	{
//		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
//		if (SpartaGameInstance)
//		{
//			AddScore(Score);
//			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
//		}
//	}
//
//	if (CurrentLevelIndex >= MaxLevels)
//	{
//		OnGameOver();
//		return;
//	}
//
//	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
//	{
//		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
//	}
//	else
//	{
//		OnGameOver();
//	}
//}



void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}														
					}
				}

				//if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				//{					
				//	LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				//}

				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave + 1)));
				}
			}
		}
	}
}