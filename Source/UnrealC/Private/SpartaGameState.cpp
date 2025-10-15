#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState() // 생성자, 추가로 아래는 그냥 초기값 설정 초기화 한것.
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.0f;
	CurrentLevelIndex = 0;
	//MaxLevels = 3;  // 웨이브로 수정 하였음으로 레벨은 불필요해서 주석처리하였음.

	CurrentWave = 0;
	MaxWave = 3;
}

void ASpartaGameState::BeginPlay() // 시작지점
{
	Super::BeginPlay();            // 부모 클래스의 BeginPlay 먼저 실행

	StartWave();                   // 첫번째 웨이브 시작

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle, this, &ASpartaGameState::UpdateHUD, 0.1f, true
	);  // GetWorldTimerManager() 타이머 관리 객체  // HUDUpdateTimerHandle 타이머 식별용 핸들(이름표)
}       // 0.1f 초 단위로 실행 // true 반복실행 false 한번만 실행 
		// this 이 함수를 실행할 주체가 누구냐? 즉 ASpartaGameState 클래스 의 안이니까 this = 현재 이 GameState객체 자기자신
// 이부분 너무 헷갈림 설명 들을것.
 
int32 ASpartaGameState::GetScore() const // int32는 32비트 정수타입, 그옆은 GameState안의 점수(Score)를 외부에서 가져올 함수
{
	return Score; // 스코어값 반환
}

void ASpartaGameState::AddScore(int32 Amount) // AddScore 점수를 더하는 함수 // Amount 더할 점수의 양 // int32 점수 단위를 정수로 받음
{
	if (UGameInstance* GameInstance = GetGameInstance()) // 현재 실행중인 게임의 인스턴스를 가져와서 GameInstance 포인터에 대입함.
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
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!"), CurrentWave + 1); // 추가

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
				// 추가
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

	UE_LOG(LogTemp, Warning, TEXT("Starting Wave %d"), CurrentWave + 1); // 추가

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40 + (CurrentWave * 10); // 추가

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
		// 잠깐 대기후 웨이브 시작
		GetWorldTimerManager().SetTimerForNextTick([this]()
			{
				StartWave();
			});
	}

}

//void ASpartaGameState::EndLevel() // 엔드레벨 사용 X 
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