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
 
//int32 ASpartaGameState::GetScore() const // int32는 32비트 정수타입, 그옆은 GameState안의 점수(Score)를 외부에서 가져올 함수
//{
//	/*return Score;*/ // 스코어값 반환
//	return 100; // 스코어값 반환
//}

void ASpartaGameState::AddScore(int32 Amount) // AddScore 점수를 더하는 함수 // Amount 더할 점수의 양 // int32 점수 단위를 정수로 받음
{
	if (UGameInstance* GameInstance = GetGameInstance()) // 현재 실행중인 게임의 인스턴스를 가져와서 GameInstance 포인터에 대입함.
	{
		// 기본 게임 인턴스를, 내가만든 버전으로 변환해서 거기있는 내 커스텀 함수를 쓰는 과정
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance); 
	
		// 내가 만든 SpartaGameInstance가 실제로 존재하고 작동중이라면 그 안에있는 점수 누적 함수가 실행된다.
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::StartWave() // GameState가 웨이브를 시작할때 실행할 함수 선언부
{
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!"), CurrentWave + 1); // 아웃로그에 워닝, 택스트 웨이브 시작 뜨게함

	if (GEngine) // 왼쪽 상단에 메세지 출력
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Wave %d Start!"), CurrentWave + 1));
	}

	// 현재 월드에서 첫번째 플레이어 컨트롤러를 찾아서 존재한다면 그걸 PlayerController 변수에 대입하고 아래 로직을 실행
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()) 
	{
		// 기본 컨트롤러를 내가만든 스파르타 전용 컨트롤러로 바꿔서 기능을 확장해 사용하는 코드
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD(); // SpartaPlayerController가 가지고 있는 ShowGameHUD 함수를 실행

			// GetHUDWidget이 반환한 HUD위젯의 주소를 가르키고 그게 존재한다면 HUDWidget 변수에 대입하여 사용
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// 현재 월드 내에 GetFirstPlayerController 존재하다면 그 객체의 주소를 APlayerController* 타입 변수인 PC 에담는다
				if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
				{
					// PC를 ASpartaPlayerController 타입으로 안전하게 Cast 하고 그 주소를 SpartaPC 포인터 변수에 저장
					if (ASpartaPlayerController* SpartaPC = Cast<ASpartaPlayerController>(PC))
					{
						if (UUserWidget* HUD = SpartaPC->GetHUDWidget()) // SpartaPC의 GetHUDWidget()이 반환한 위젯이 존재하다면 그주소를 HUD 에 담는다
						{
							// HUD 위젯에 PlayGameWave 라는 함수가 있다면 그것을 실행 시켜라
							HUD->CallFunctionByNameWithArguments(TEXT("PlayGameWave"), *GLog, nullptr, true);
							// CallFunctionByNameWithArguments 이름을 찾아 실행하는 함수
							// *GLog 언리얼의 전역 로그 객체 (로그를 출력할 수 있는 출력창) 함수를 실행할때 결과나 에러를 콘솔 로그에 기록
							// nullptr 이란 아무것도가르키지 않는다. 여기선 월드나 오브젝트 컨텍스트를 따로 전달하지 않겠다.
						}
					}
				}
			}
		}
	}

	//UGameInstance* GameInstance = GetGameInstance();
	//bool GameInstance = GetGameInstance();
	//int GameInstance = GetGameInstance();

	if (UGameInstance* GameInstance = GetGameInstance()) // 흐름은 알겠는데 왜 비슷한 이름들이 세번이 들어가는지 각자 다른역할인건 알고는 있음.
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Starting Wave %d"), CurrentWave + 1);

	SpawnedCoinCount = 0;   // 총 코인 개수
	CollectedCoinCount = 0; // 획득한 코인 개수

	TArray<AActor*> FoundVolumes; // FoundVolumes는 여러 개의 AActor를 담을 수 있는 상자(배열)
	// 현재 월드(GetWorld) 안에서 ASpawnVolume 타입의 액터들을 전부 찾아 그 주소들을 FoundVolumes 배열에 저장.
	// 진짜로 찾는건 GetAllActorsOfClass 하지만 무엇을 찾을지 기준을 잡아주는건 StaticClass
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40 + (CurrentWave * 10); // 웨이브가 증가함에 따라 아이템스폰 개수가 10개씩 늘어남

	for (int32 i = 0; i < ItemToSpawn; i++) // 스폰해야할 아이템 개수만큼 반복 생성
	{
		if (FoundVolumes.Num() > 0) // FoundVolumes 배열 안에 액터가 하나라도 있다면(true) 즉 찾은 스폰볼륨이 1개 이상이면 실행해라
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]); // 배열의 첫번째 액터를 ASpawnVolume타입으로 Cast해서 그주소를 스폰볼륨에 저장
			
			// 스폰볼륨이 유효하다면 랜덤 아이템을 스폰하고, 그게 코인이라면 코인 카운트를 하나 늘린다.
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

	// 월드의 타이머 매니저에 LevelTimerHandle 라는 타이머를 등록, LevelDuration초가 지나가면
	// ASpartaGameStatedml 의 OnWaveTimeUp() 함수를 한번 호출해라. false 는 한번만 실행
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle, this, &ASpartaGameState::OnWaveTimeUp, LevelDuration, false
	);
}

// 시간이 다 되면 EndWave를 실행시키기 위한 함수
void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

// && 는 논리 연산자 (AND) 즉 두조건이 모두 참일때만 전체가 참이된다.
// SpawnedCoinCount > 0 -> 코인이 실제로 스폰되어있다. // CollectedCoinCount >= SpawnedCoinCount 스폰된 코인을 전부 주웠다
// 즉 코인을 하나 주울때마다 CollectedCoinCount를 1늘리고 콘솔에 코인개수를 로그로 출력. 만약 스폰된 코인을 전부 주우면 웨이브 종료
void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount)

		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			EndWave();
	    }
}

void ASpartaGameState::EndWave() // 엔드 웨이브 함수 호출
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle); // 웨이브 제한시간 타이머를 종료
	/*GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);*/
	CurrentWave++; // 다음 웨이브로 넘어감

	if (CurrentWave >= MaxWave) // 현재 웨이브가 최대웨이브에 도달, 초과했다면 게임을 종료
	{
		OnGameOver();
	}

	else // 현재 프레임이 끝나고 바로 다음 프레임이 시작될때 StartWave() 를 실행
	{
		GetWorldTimerManager().SetTimerForNextTick([this]() // 다음틱 으로 미루는 이유는 현재 웨이브가 완전히 정리된뒤 안전하게 넘어가기 위함
			{
				StartWave();
			});
	}

}

void ASpartaGameState::OnGameOver() // 게임오버시 호출되는 함수 정의
{
	// 현재 월드에서 GetFirstPlayerController를 찾아서 그주소를 APlayerController* 타입변수 PlayerController에 저장
	// GetFirstPlayerController 란 ? 언리얼 엔진에 있는 여러 플레이어 컨트롤러중 첫번째 플레이어 컨트롤러 싱글모드 이기때문에 이렇게 써도가능
	// <ASpartaPlayerController>(PlayerController) 이부분은 부모와 자식으로 보는게맞음 부모의 기능을 내가만든 자식에 추가함
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true); // 일시정지 함수 true 넣었으니 멈춰라 라는 의미
			SpartaPlayerController->ShowMainMenu(true); // 메인 메뉴를 UI 화면에 띄우는 함수 true 이니 메뉴표시.
		}
	}
}

void ASpartaGameState::UpdateHUD() // ASpartaGameState클래스에 선언된 UpdateHUD() 함수를 정의
{
	// GetFirstPlayerController 를 찾아 PlayerController 에 저장
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		// PlayerController를 안전하게 ASpartaPlayerController에게 Cast 하고 SpartaPlayerController에 대입
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		{
			// SpartaPlayerController가 가지고 있는 HUD 위젯을 불러오고 그게 존재한다면 HUDWidget 에저장후 아래코드 실행
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// HUD 안에 이름이 Time 인 텍스트 블록을 찾아 존재하면 TimeText 변수에 저장후 텍스트 내용을 바꿀 준비
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					// LevelTimerHandle로 관리되는 타이머의 남은시간을 가져와서 RemainingTime 에 저장
					// LevelTimerHandle 레벨이 끝날때 까지 남은 시간을 추적하는 타이머
					// GetTimerRemaining 은 타이머에 남은 시간을 float 단위로 알려주는 함수
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					// FString::Printf 에서 문자열을 만들고 FText::FromString 로 그문자열을 UI에 쓸 수 있게 바꿈
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				// HUD위젯 안에서 Score인 텍스트블록을 찾고 그게 UTextBlock 이면 ScoreText변수에 저장후 아래코드 실행
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					// 월드에 연결된 게임인스턴스 객체가 존재하다면 그주소를 UGameInstance* 타입변수 GameInstance에 담고 아래코드 실행
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						// GameInstance를 내가만든 USpartaGameInstance 타입으로 형변환후 SpartaGameInstance 변수에 저장
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						// SpartaGameInstance 의 TotalScore 값을 문자열 형태로 만들어 HUD점수 영역에 표시
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}														
					}
				}
				// HUD 위젯 안에서 이름이 Wave인 텍스트 블록을 찾아 그게 실제로 UTextBlock 이라면 WaveText 변수에 저장
				// 그리고 현재 웨이브 번호 CurrentWave + 1 을 문자열로 변환해 HUD에 표시
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave + 1)));
				}
			}
		}
	}
}


