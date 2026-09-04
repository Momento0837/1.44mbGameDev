//#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment(lib, "winmm.lib")

#include <algorithm>
#include <cmath>
#include <cwchar>
#include <cwctype>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

namespace {

constexpr wchar_t kWindowClass[] = L"OnePointFourFourMBGameWindow";
constexpr wchar_t kWindowTitle[] = L"주문을 똑바로 말해!!!";
constexpr wchar_t kDefaultUiFontName[] = L"PFStardust 1.4 Medium";
constexpr wchar_t kTitleFontName[] = L"PFStardust 1.4 Medium";
constexpr wchar_t kNarrationFontName[] = L"DOSPilgi";

constexpr int kDesignWidth = 800;
constexpr int kDesignHeight = 600;
constexpr double kSupersampleScale = 1.5;
constexpr int kPlayAreaX = 200;
constexpr int kPlayAreaY = 100;
constexpr int kPlayAreaSize = 400;
constexpr int kFloorColumns = 8;
constexpr int kFloorRows = 3;
constexpr double kFloorStartDepth = 0.07;
constexpr double kFloorWidthScale = 1.12;
constexpr double kFloorHeightScale = 0.35;
constexpr int kDefaultVanishingPointX = kPlayAreaSize / 2;
constexpr int kDefaultVanishingPointY = kPlayAreaSize / 2;

// 재료통들의 직사각형 배치와 여백을 조절하는 설정값이다.
constexpr int kMaterialBinTop = 294;
constexpr int kMaterialBinFirstRowBottom = 342;
constexpr int kMaterialBinRowGap = 9;
constexpr int kMaterialBinColumnGap = 9;
constexpr int kMaterialBinBorderWidth = 2;
constexpr int kPngSocketSize = 64;
constexpr int kMaterialImageSourceSize = 16;
constexpr double kPngSocketHoverScale = 1.05;
constexpr double kPngSocketEasingSpeed = 0.20;
constexpr int kMaterialBinVerticalOffset = 5;

// 테이블 상호작용 영역과 이징 애니메이션 설정값이다.
constexpr int kTableHoverTop = 275;
constexpr int kTableHoverBottom = 400;
constexpr double kTableLiftDistance = 10.0;
constexpr double kTableEasingSpeed = 0.18;
constexpr int kCookingEntryIndicatorGradientHeight = 15;
constexpr double kCookingEntryIndicatorGradientMaxOpacity = 0.3;
constexpr int kCookingEntryIndicatorTriangleWidth = 10;
constexpr int kCookingEntryIndicatorTriangleHeight = 6;
constexpr double kCookingEntryIndicatorFadeSeconds = 0.5;
constexpr double kCookingEntryIndicatorBounceHalfSeconds = 0.5;
constexpr double kCookingEntryIndicatorBounceHeight = 3.0;
constexpr int kTableTop = 278;
constexpr int kCookingTableY = 150;
constexpr double kCookingTableLift = kTableTop - kCookingTableY;
constexpr double kCookingAnimationSeconds = 1.0;
constexpr int kTableHorizontalExtension = 15;
constexpr int kTableBottomExtension = 5;
constexpr int kTableBorderWidth = 4;
constexpr int kTableHiddenExtension =
    static_cast<int>(kCookingTableLift) + 15;
constexpr int kGlassDomeY = 120;
constexpr int kGlassDomeHeight = 160;
constexpr BYTE kGlassDomeOpacity = 42;
constexpr int kCuttingBoardX = 120;
constexpr int kCuttingBoardCookingY = 300;
constexpr int kCuttingBoardWidth = 160;
constexpr int kCuttingBoardHeight = 78;
constexpr int kCuttingBoardBorderWidth = 2;
constexpr int kMaximumMaterialClones = 64;
constexpr int kCompletedFoodSourceWidth = 32;
constexpr int kCompletedFoodSourceHeight = 16;
constexpr int kCompletedFoodDisplayWidth = 64;
constexpr int kCompletedFoodDisplayHeight = 32;
constexpr int kCompletedFoodShadowBlurRadius = 2;
constexpr double kCompletionPresentationSeconds = 1.2;
constexpr double kFogFallDistance = 50.0;
constexpr int kFogSourceSize = 16;
constexpr int kFogDisplaySize = 64;
constexpr int kMaximumStarParticles = 32;
constexpr double kStarParticleGravity = 260.0;
constexpr double kStarParticleSize = 7.0;
constexpr int kResetButtonWidth = 72;
constexpr int kResetButtonHeight = 30;
constexpr int kResetButtonMargin = 10;
constexpr int kResetButtonRadius = 8;
constexpr double kResetButtonFadeSpeed = 0.08;
constexpr int kNarrationBoxX = 10;
constexpr int kNarrationBoxY = 10;
constexpr int kNarrationBoxWidth = kDesignWidth - 20;
constexpr int kNarrationBoxHeight = 160;
constexpr ULONGLONG kNarrationCharacterIntervalMilliseconds = 80;
constexpr double kNarrationAutoAdvanceSeconds = 3.0;
constexpr int kMaximumNarrationParticles = 128;
constexpr double kNarrationParticleGravity = 120.0;
constexpr unsigned int kHoverNarrationChancePercent = 30;
constexpr unsigned int kExactNarrationChancePercent = 30;
constexpr UINT_PTR kAnimationTimerId = 1;
constexpr UINT kAnimationFrameMilliseconds = 16;

// MP3 효과음: 같은 용도의 동작은 하나의 파일을 공유한다.
constexpr wchar_t kPlayerClickSoundPath[] = L"sounds\\player_click.mp3";
constexpr wchar_t kInteractionClickSoundPath[] = L"sounds\\interaction_click.mp3";
constexpr wchar_t kButtonClickSoundPath[] = L"sounds\\button_click.mp3";
constexpr wchar_t kCookingSuccessSoundPath[] = L"sounds\\cooking_success.mp3";
constexpr wchar_t kCookingErrorOneSoundPath[] = L"sounds\\cooking_error_1.mp3";
constexpr wchar_t kCookingErrorTwoPlusSoundPath[] = L"sounds\\cooking_error_2_plus.mp3";
constexpr wchar_t kTrophyPurchaseSoundPath[] = L"sounds\\trophy_purchase.mp3";
constexpr wchar_t kReceiptSoundPath[] = L"sounds\\tearing_papers1.mp3";
constexpr wchar_t kNpcTalkingSoundPath[] = L"sounds\\NPC_talking.mp3";
constexpr wchar_t kBusinessMusicPath[] = L"sounds\\business_music.mp3";
constexpr double kMasterSoundOutputScale = 0.70;
constexpr double kBusinessMusicFadeOutSeconds = 0.5;
constexpr int kBusinessMusicFadeOutSteps = 10;
constexpr int kOptionsBackButtonX = 20;
constexpr int kOptionsBackButtonY = 20;
constexpr int kOptionsBackButtonSize = 44;
constexpr int kVolumeSliderLeft = 250;
constexpr int kVolumeSliderRight = 550;
constexpr int kVolumeSliderY = 190;
constexpr int kVolumeSliderTrackHeight = 6;
constexpr int kVolumeSliderKnobRadius = 10;
constexpr int kFontLicenseDropdownTop = 250;
constexpr int kFontLicenseDropdownHeight = 36;
constexpr int kFontLicenseDropdownWidth = 340;
constexpr int kFontLicenseDropdownGap = 20;
constexpr int kFontLicenseDropdownLeft =
    (kDesignWidth - kFontLicenseDropdownWidth * 2
        - kFontLicenseDropdownGap) / 2;
constexpr double kFontLicenseTextHeight = 15.0;
constexpr wchar_t kDosPhilgiLicenseText[] =
    L"The MIT License (MIT)\n\n"
    L"Copyright (c) 2016-2022 Damheo Lee (이담허) "
    L"(leedheo@gmail.com),\n\n"
    L"with Reserved Font Name DOSMyungjo, DOSGothic, DOSSaemmul, "
    L"Sam3KRFont, MiraeroNormal, and DOSIyagi,\n\n"
    L"its OTF and BDF suffix name. (e.g. DOSMyungjo OTF)\n\n"
    L"1. 상업적 이용 문의 (라이선스 관련) 현재 MIT 라이선스로 "
    L"배포하고 있습니다.\n\n"
    L"2. 영리목적 사용 (메신저 이모티콘 등) 가능합니다. 대신 부가 설명에 "
    L"‘글꼴(폰트)로 도스샘물체(leedheo 제작)를 사용하였습니다’ 형태로 "
    L"기재하시면 되겠습니다.\n\n"
    L"3. 폰트 출처 및 이용 명시 등에 관하여 폰트 원출처는 "
    L"https://github.com/hurss/fonts 입니다.";
constexpr wchar_t kPfStardustLicenseText[] =
    L"1. PF스타더스트 폰트의 저작권은 제작자인 피나타"
    L"(campanula913@naver.com)에게 있습니다.\n\n"
    L"2. PF스타더스트 폰트는 개인 및 기업사용자를 포함한 모두가 무료로 "
    L"상업적 사용이 가능합니다.\n\n"
    L"3. 제작자의 허락없이 수정 및 재배포, 유료로 판매하는 행위는 "
    L"금지합니다.";

// 타이틀 화면의 배치와 화면 전환 애니메이션 설정값이다.
constexpr int kTitlePlaceholderWidth = 500;
constexpr int kTitlePlaceholderHeight = 300;
constexpr int kTitlePlaceholderY = 20;
constexpr wchar_t kTitleImagePath[] = L"ui\\title.png";
constexpr int kTitleImageSourceWidth = 144;
constexpr int kTitleImageSourceHeight = 96;
constexpr int kTitleImageIntegerScale = 3;
constexpr double kTitleBreathingScale = 1.025;
constexpr double kTitleBreathingSeconds = 3.2;
constexpr double kTitleAppearanceDelaySeconds = 0.0;
constexpr double kTitleFadeInSeconds = 1.0;
constexpr double kScreenFadeSeconds = 0.5;
constexpr int kTitleButtonFontHeight = 30;
constexpr int kTitleButtonPadding = 5;
constexpr double kTitleButtonHoverScale = 1.05;
constexpr double kTitleButtonEasingSpeed = 0.20;
constexpr int kMoneyUiMargin = 12;
constexpr int kMoneyCoinSourceSize = 16;
constexpr int kMoneyCoinDisplaySize = 32;
constexpr int kMoneyTextGap = 8;
constexpr int kMoneyTextWidth = 120;
constexpr int kMoneyFontHeight = 24;
constexpr int kMoneyTooltipPadding = 5;
constexpr int kMoneyTooltipGap = 12;
constexpr long long kRevenueConversionNumerator = 16;
constexpr long long kRevenueConversionDenominator = 1000;
constexpr long long kInitialDailyRevenueGoal = 1000;
struct CustomerCategoryReward {
    const wchar_t* id;
    int baseReward;
};
constexpr CustomerCategoryReward kCustomerCategoryRewards[] = {
    {L"normal", 200},
    {L"sequence_obsessed", 300},
    {L"order_change", 400},
    {L"large_order", 500},
    {L"tmi", 600},
    {L"silent", 300}
};
constexpr int kStartBusinessButtonWidth = 112;
constexpr int kStartBusinessButtonHeight = kMoneyCoinDisplaySize;
constexpr double kStartBusinessButtonOpacity = 0.70;
constexpr double kCountdownStepSeconds = 1.0;
constexpr int kCountdownStepCount = 4;
constexpr double kCountdownPulseScale = 1.05;
constexpr double kNpcEntranceSeconds = 1.0;
constexpr double kNpcEntranceHorizontalFraction = 0.5;
constexpr int kNpcSourceSize = 32;
constexpr int kNpcDisplaySize = kNpcSourceSize * 5;
constexpr int kNpcEntranceRightStartX = 430;
constexpr int kNpcEntranceLeftStartX =
    -kNpcDisplaySize - (kNpcEntranceRightStartX - kPlayAreaSize);
constexpr int kNpcEntranceTargetX =
    (kPlayAreaSize - kNpcDisplaySize) / 2;
constexpr int kNpcEntranceStartBottomY = 200;
constexpr int kNpcEntranceTargetBottomY = 300;
constexpr double kNpcEntranceTargetScale = 1.10;
constexpr double kNpcDialogueBounceSeconds = 0.3;
constexpr double kNpcDialogueBounceHeight = 30.0;
constexpr ULONGLONG kNpcIdleStepMilliseconds = 500;
constexpr double kNpcIdleRise = 10.0;
constexpr double kNpcAfterCookingDepartureDelaySeconds = 1.0;
constexpr double kEmptySubmissionJumpSeconds = 0.3;
constexpr int kEmptySubmissionJumpCount = 2;
constexpr double kEmptySubmissionJumpHeight = 10.0;
constexpr double kEmptySubmissionExitSpeedMultiplier = 1.2;
constexpr double kDayDurationSeconds = 180.0;
constexpr double kSunsetStartSeconds = 90.0;
constexpr double kNightStartSeconds = 162.0;
constexpr double kBusinessClosingCharacterSeconds = 0.3;
constexpr double kBusinessClosingHoldSeconds = 1.0;
constexpr double kBusinessClosingFadeSeconds = 0.5;
constexpr wchar_t kBusinessClosingText[] = L"영업 종료...";
constexpr double kReceiptSlideSeconds = 0.5;
constexpr double kSettlementButtonDelaySeconds = 0.5;
constexpr double kSettlementButtonFadeSeconds = 0.5;
constexpr int kReceiptWidth = 360;
constexpr int kFinalReceiptWidth = 520;
constexpr int kReceiptHeight = 470;
constexpr int kReceiptTargetY = (kDesignHeight - kReceiptHeight) / 2;
constexpr int kNextDayButtonWidth = 110;
constexpr int kNextDayButtonHeight = 36;
constexpr int kNextDayButtonGap = 12;
constexpr double kNarrationAppearanceDelaySeconds = 0.5;
constexpr double kNarrationFadeInSeconds = 0.5;
constexpr wchar_t kOwnedMoneyCoinImagePath[] =
    L"ui\\owned_money_coin.png";
constexpr wchar_t kEarnedMoneyCoinImagePath[] =
    L"ui\\earned_money_coin.png";
constexpr wchar_t kObjectiveImagePath[] = L"ui\\objective.png";
constexpr wchar_t kTrophyImagePath[] = L"ui\\trophy.png";
constexpr int kObjectiveImageSourceSize = 16;
constexpr int kTrophyImageSourceSize = 32;
constexpr int kTrophyImageDisplaySize = kTrophyImageSourceSize * 2;
constexpr int kTrophyImageX = kPlayAreaX + 12;
constexpr int kTrophyImageY = kPlayAreaY + 72;
constexpr int kTrophyShadowBlurRadius = 2;
constexpr int kTrophyTooltipGap = 12;
constexpr int kTrophyTooltipPadding = 8;
constexpr double kTrophyTooltipFontHeight = 15.0;
constexpr long long kTrophyPrice = 1000;
constexpr int kTrophyPriceCoinSize = 16;
constexpr int kTrophyPriceRowHeight = 20;
constexpr int kTrophyPriceTextGap = 10;
constexpr ULONGLONG kTrophyPriceBlinkMilliseconds = 300;
constexpr ULONGLONG kTrophyPriceBlinkDurationMilliseconds =
    kTrophyPriceBlinkMilliseconds * 4;
constexpr int kTutorialContentWidth = 390;
constexpr int kTutorialContentLeft =
    (kDesignWidth - kTutorialContentWidth) / 2;
constexpr int kTutorialCheckboxLeft = 22;
constexpr int kTutorialCheckboxTop = 560;
constexpr int kTutorialCheckboxSize = 10;
constexpr double kPreGameMessageFadeSeconds = 1;
constexpr double kPreGameMessageHoldSeconds = 2.0;
constexpr double kPreGameMessageFontHeight = 20.0;
constexpr wchar_t kPreGameMessages[4][64] = {
    L"\"오늘의 목표는 매출이다. 부차적인 목표는 화내지 않는 것이다.\"",
    L"\"세상에는 두 종류의 손님이 있다. 설명하는 손님과 설명했다고 믿는 손님.\"",
    L"\"나는 요리를 한다. 왜 추리를 하고 있는지는 모르겠다.\"",
    L"\"심연을 들여다보면 심연도 나를 본다. 손님 주문서를 보면 더하다.\""
};
constexpr int kExitDialogWidth = 300;
constexpr int kExitDialogHeight = 200;
constexpr int kExitDialogButtonWidth = 80;
constexpr int kExitDialogButtonHeight = 36;
constexpr wchar_t kTitleButtonLabels[3][16] = {
    L"게임 시작",
    L"옵션",
    L"게임 종료"
};
constexpr int kTitleButtonCenterY[3] = {365, 425, 485};

constexpr COLORREF kLetterboxColor = RGB(0x0d, 0x0d, 0x0d);
constexpr COLORREF kTileGreen = RGB(0xa8, 0xcb, 0xa9);
constexpr COLORREF kTileWhite = RGB(0xf2, 0xfb, 0xf1);
constexpr COLORREF kSkyColor = RGB(0x77, 0xcb, 0xe8);
constexpr COLORREF kSunsetSkyColor = RGB(0xff, 0x8b, 0x6e);
constexpr COLORREF kNightSkyColor = RGB(0x2c, 0x2a, 0x6e);
constexpr COLORREF kReceiptColor = RGB(0xe2, 0xd8, 0xc5);
constexpr COLORREF kReceiptTextColor = RGB(0x5a, 0x43, 0x30);
constexpr COLORREF kGlassColor = RGB(0xac, 0xdf, 0xf1);
constexpr COLORREF kWallColor = RGB(0xe1, 0xf7, 0xe5);
constexpr COLORREF kWallTrimColor = RGB(0xd3, 0xf3, 0xd8);
constexpr COLORREF kShelfDarkColor = RGB(0x7c, 0x7c, 0x7c);
constexpr COLORREF kShelfColor = RGB(0x89, 0x89, 0x89);
constexpr COLORREF kShelfLightColor = RGB(0xa6, 0xa6, 0xa6);
constexpr COLORREF kMaterialColor = RGB(0xea, 0xe1, 0xd0);
constexpr COLORREF kMaterialBinColor = RGB(0x8d, 0x8d, 0x8d);
constexpr COLORREF kMaterialBinBorderColor = RGB(0xbf, 0xbf, 0xbf);
constexpr COLORREF kPlatformColor = RGB(0x5c, 0x3e, 0x1f);
constexpr COLORREF kTableBorderColor = RGB(0x42, 0x29, 0x10);
constexpr COLORREF kCuttingBoardColor = RGB(0xb6, 0xa1, 0x8c);
constexpr COLORREF kCuttingBoardBorderColor = RGB(0x7c, 0x65, 0x4e);
constexpr COLORREF kResetButtonColor = RGB(0x94, 0xc2, 0x93);
constexpr COLORREF kExitDialogColor = RGB(0x2a, 0x2a, 0x2a);
constexpr COLORREF kExitYesButtonColor = RGB(0x94, 0xc2, 0x93);
constexpr COLORREF kExitNoButtonColor = RGB(0xd9, 0x9a, 0x9a);
constexpr COLORREF kCoinPlaceholderColor = RGB(0x32, 0xcd, 0x32);
constexpr COLORREF kStartBusinessButtonColor = RGB(0x94, 0xc2, 0x93);

constexpr int kMaterialBinColumns = 7;
constexpr int kMaterialBinRows = 2;
constexpr int kMaterialBinCount = kMaterialBinColumns * kMaterialBinRows;
constexpr wchar_t kMaterialImagePaths[kMaterialBinCount][64] = {
    L"materials\\tortilla.png",
    L"materials\\lettuce.png",
    L"materials\\raw_bell_pepper.png",
    L"materials\\raw_carrot.png",
    L"materials\\egg_mayo.png",
    L"materials\\tomato.png",
    L"materials\\mint_chocolate.png", // 여기까지 제작
    L"materials\\seaweed_and_rice.png",
    L"materials\\burdock.png",
    L"materials\\pickled_radish.png",
    L"materials\\crab_stick.png",
    L"materials\\spinach.png",
    L"materials\\strawberry.png",
    L"materials\\chocolate.png"
};
constexpr wchar_t kMaterialIds[kMaterialBinCount][32] = {
    L"tortilla",
    L"lettuce",
    L"raw_bell_pepper",
    L"raw_carrot",
    L"egg_mayo",
    L"tomato",
    L"mint_chocolate",
    L"seaweed_and_rice",
    L"burdock",
    L"pickled_radish",
    L"crab_stick",
    L"spinach",
    L"strawberry",
    L"chocolate"
};

// 소실점 좌표는 플레이 영역의 왼쪽 위를 (0, 0)으로 삼는다.
POINT gVanishingPoint{kDefaultVanishingPointX, kDefaultVanishingPointY};
POINT gMouseDesignPosition{};
bool gHasMousePosition = false;
bool gIsTrackingMouse = false;
bool gIsTrophyHovered = false;
enum class MoneyTooltipKind {
    None,
    Owned,
    Earned,
    Objective
};
MoneyTooltipKind gHoveredMoneyTooltip = MoneyTooltipKind::None;
POINT gTrophyTooltipAnchorPosition{};
POINT gTrophyFeedbackTooltipAnchorPosition{};
ULONGLONG gLastAnimationTickTime = 0;

enum class ScreenState {
    Title,
    Options,
    TitleFadingOut,
    Tutorial,
    PreGameMessage,
    PreparationFadingIn,
    Preparation,
    Countdown,
    NpcEntering,
    NarrationStarting,
    Game,
    EmptySubmissionReacting,
    NpcExiting,
    BusinessClosing,
    Settlement
};
ScreenState gScreenState = ScreenState::Title;
ULONGLONG gTitleStartTime = 0;
ULONGLONG gScreenTransitionStartTime = 0;
int gHoveredTitleButton = -1;
double gTitleButtonScales[3]{1.0, 1.0, 1.0};
RECT gTitleButtonRects[3]{};
bool gIsExitDialogVisible = false;
bool gIsVolumeSliderDragging = false;
int gExpandedFontLicenseDropdown = -1;
ULONGLONG gPreparationSequenceStartTime = 0;
ULONGLONG gNarrationFadeStartTime = 0;
bool gIsNarrationBoxInteractive = false;
ULONGLONG gNpcIdleStartTime = 0;
int gNpcIdleStep = 0;
long long gOwnedMoney = 0;
bool gIsTrophyPurchased = false;
bool gSkipTutorial = false;
ULONGLONG gTrophyInsufficientFundsStartTime = 0;
ULONGLONG gPreGameMessageStartTime = 0;
int gPreGameMessageIndex = 0;
long long gEarnedMoney = 0;
long long gDayRevenue = 0;
long long gDayMenuRevenue[3]{};
int gCurrentDay = 1;
long long gDailyRevenueGoal = kInitialDailyRevenueGoal;
bool gCurrentDayGoalMet = false;
int gDailyGoalFailureCount = 0;
bool gIsFinalSettlement = false;
std::vector<long long> gDailyRevenueHistory;
ULONGLONG gDayStartTime = 0;
ULONGLONG gBusinessClosingStartTime = 0;
ULONGLONG gSettlementStartTime = 0;
int gCookingMistakeCount = 0;

bool gIsTableHovered = false;
double gTableLift = 0.0;
enum class CookingState {
    NonCooking,
    Cooking
};
CookingState gCookingState = CookingState::NonCooking;

// NPC의 최초 주문 진행 단계를 나타내며 조리상태와는 독립적으로 관리한다.
enum class NpcOrderState {
    BeforeOrder, // NPC 주문 전: NPC 입장 애니메이션이 진행되는 상태
    Ordering,    // NPC 주문 중: NPC가 최초 주문 대사를 말하는 상태
    AfterOrder   // NPC 주문 끝남: 최초 주문 대사가 끝난 이후의 모든 상태
};
NpcOrderState gNpcOrderState = NpcOrderState::BeforeOrder;
ULONGLONG gCookingEntryIndicatorStartTime = 0;

// 추후 AfterOrder 상태에서 재료 호버 조건에 따라 추가 나레이션을 연결한다.
CookingState gCookingTransitionTargetState = CookingState::NonCooking;
bool gIsCookingTransitionRunning = false;
double gCookingTransitionStartLift = 0.0;
double gCookingTransitionTargetLift = 0.0;
ULONGLONG gCookingTransitionStartTime = 0;
int gHoveredPngSocket = -1;
double gPngSocketScales[kMaterialBinCount]{};
struct MaterialClone {
    int materialIndex;
    int x;
    int y;
    int size;
    float angle;
};
struct FogParticle {
    double x;
    double y;
    double startAngle;
    double targetAngle;
};
MaterialClone gMaterialClones[kMaximumMaterialClones]{};
int gMaterialCloneCount = 0;
std::vector<FogParticle> gFogParticles;
bool gIsCompletionPresentationActive = false;
ULONGLONG gCompletionPresentationStartTime = 0;
int gCompletedFoodImageIndex = -1;
struct StarParticle {
    double x;
    double y;
    double velocityX;
    double velocityY;
    double angle;
    double angularVelocity;
};
std::vector<StarParticle> gStarParticles;
ULONGLONG gLastStarParticleUpdateTime = 0;
struct NarrationParticle {
    double x;
    double y;
    double velocityX;
    double velocityY;
    int size;
};
std::vector<NarrationParticle> gNarrationParticles;
ULONGLONG gLastNarrationParticleUpdateTime = 0;
size_t gNarrationParticleProcessedLength = 0;
bool gIsNarrationOverflowAnimating = false;
unsigned int gRandomState = 0x144u;
double gResetButtonOpacity = 0.0;
// 추후 옵션 화면에서 0.0(음소거)~1.0(최대) 범위로 연결할 전체 음량이다.
double gMasterSoundVolume = 1.0;
enum class SoundEffect {
    PlayerClick,
    InteractionClick,
    ButtonClick,
    CookingSuccess,
    CookingErrorOne,
    CookingErrorTwoPlus,
    TrophyPurchase,
    Receipt,
    NpcTalking,
    Count
};
struct SoundEffectPlayer {
    const wchar_t* relativePath;
    const wchar_t* alias;
    bool isOpen;
    double volumeScale;
};
SoundEffectPlayer gSoundEffects[] = {
    {kPlayerClickSoundPath, L"sfx_player_click", false, 0.30},
    {kInteractionClickSoundPath, L"sfx_interaction", false, 1.0},
    {kButtonClickSoundPath, L"sfx_button", false, 1.0},
    {kCookingSuccessSoundPath, L"sfx_cooking_success", false, 1.0},
    {kCookingErrorOneSoundPath, L"sfx_cooking_error_1", false, 1.0},
    {kCookingErrorTwoPlusSoundPath, L"sfx_cooking_error_2_plus", false, 1.0},
    {kTrophyPurchaseSoundPath, L"sfx_trophy_purchase", false, 1.0},
    {kReceiptSoundPath, L"sfx_receipt", false, 1.0},
    {kNpcTalkingSoundPath, L"sfx_npc_talking", false, 0.10}
};
SoundEffectPlayer gBusinessMusic{
    kBusinessMusicPath, L"bgm_business", false, 1.0};
HWND gSoundNotificationWindow = nullptr;
MCIDEVICEID gPlayerClickDeviceId = 0;
bool gIsPlayerClickPrepared = false;
bool gIsBusinessMusicPlaying = false;
bool gIsBusinessMusicFadingOut = false;
ULONGLONG gBusinessMusicFadeStartTime = 0;
double gBusinessMusicVolumeScale = 1.0;
int gBusinessMusicFadeStep = -1;
void PlaySoundEffect(SoundEffect effect);
void StartBusinessMusic();
void BeginBusinessMusicFadeOut(ULONGLONG now);
bool gIsNarrationActive = false;
bool gIsNarrationTyping = false;
ULONGLONG gNarrationStartTime = 0;
size_t gNarrationVisibleLength = 0;
ULONGLONG gNarrationCompletedTime = 0;
ULONGLONG gAfterCookingNarrationCompletedTime = 0;
struct DialogueTree {
    std::wstring menuId;
    std::wstring menuName;
    std::wstring id;
    std::vector<std::wstring> entryLines;
    std::vector<std::wstring> additionalLines;
    std::vector<std::wstring> afterCookingLines;
    std::vector<std::wstring> afterCooking70PercentLines;
    std::vector<std::wstring> afterCooking50PercentLines;
    std::vector<std::wstring> afterCooking0PercentLines;
    std::vector<std::wstring> emptySubmissionLines;
    struct RecipeIngredient {
        int materialIndex;
        int quantity;
    };
    struct HoverDialogue {
        int materialIndex;
        std::vector<std::wstring> nearbyLines;
        std::vector<std::wstring> exactLines;
        std::vector<bool> nearbyEnabled;
        std::vector<bool> exactEnabled;
    };
    struct NearbySlotDialogue {
        int socketIndex;
        std::wstring line;
    };
    std::vector<RecipeIngredient> recipe;
    std::vector<HoverDialogue> hoverDialogues;
    std::vector<NearbySlotDialogue> nearbySlotDialogues;
};
struct DialogueCategory {
    std::wstring id;
    std::vector<DialogueTree> menus;
};
enum class DialogueStage {
    Entry,
    Additional,
    AfterCooking
};
std::vector<std::wstring> gNarrationNames;
std::vector<DialogueCategory> gDialogueCategories;
std::wstring gCurrentNarrationName;
std::wstring gCurrentNarrationText;
std::wstring gPausedNarrationText;
size_t gPausedNarrationVisibleLength = 0;
bool gPausedNarrationWasActive = false;
bool gPausedNarrationWasTyping = false;
bool gIsHoverNarrationActive = false;
bool gWasNarrationRestoredFromHover = false;
int gCurrentDialogueCategory = -1;
int gCurrentDialogueTree = -1;
DialogueStage gCurrentDialogueStage = DialogueStage::Entry;
size_t gCurrentDialogueLineIndex = 0;
int gLastNarrationHoverSocket = -1;
bool gHasCookingResult = false;
bool gLastCookingSucceeded = false;
bool gWasEmptySubmission = false;
bool gShouldEndDayAfterOrder = false;
std::wstring gAssetsDirectory;
ULONG_PTR gGdiplusToken = 0;
Gdiplus::Image* gMaterialImages[kMaterialBinCount]{};
Gdiplus::Bitmap* gMaterialShadowImages[kMaterialBinCount]{};
Gdiplus::Image* gOwnedMoneyCoinImage = nullptr;
Gdiplus::Image* gEarnedMoneyCoinImage = nullptr;
Gdiplus::Image* gObjectiveImage = nullptr;
Gdiplus::Image* gTrophyImage = nullptr;
Gdiplus::Image* gTitleImage = nullptr;
Gdiplus::Bitmap* gTrophyShadowImage = nullptr;
constexpr int kCompletedFoodImageCount = 6;
Gdiplus::Image* gCompletedFoodImages[kCompletedFoodImageCount]{};
Gdiplus::Bitmap* gCompletedFoodShadowImages[kCompletedFoodImageCount]{};
Gdiplus::Image* gFogImage = nullptr;
HDC gBackBufferDc = nullptr;
HBITMAP gBackBufferBitmap = nullptr;
HGDIOBJ gBackBufferOldBitmap = nullptr;
int gBackBufferWidth = 0;
int gBackBufferHeight = 0;

std::wstring PlayerSavePath() {
    wchar_t executablePath[MAX_PATH]{};
    const DWORD length = GetModuleFileName(
        nullptr, executablePath, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return L"player_save.dat";
    }
    std::wstring path(executablePath, length);
    const size_t slash = path.find_last_of(L"\\/");
    path.resize(slash == std::wstring::npos ? 0 : slash + 1);
    return path + L"player_save.dat";
}

void LoadPlayerSave() {
    std::ifstream file(PlayerSavePath());
    long long savedOwnedMoney = 0;
    if (file >> savedOwnedMoney && savedOwnedMoney >= 0) {
        gOwnedMoney = savedOwnedMoney;
        int savedTrophyPurchased = 0;
        if (file >> savedTrophyPurchased) {
            gIsTrophyPurchased = savedTrophyPurchased != 0;
        }
        int savedSkipTutorial = 0;
        if (file >> savedSkipTutorial) {
            gSkipTutorial = savedSkipTutorial != 0;
        }
        double savedMasterSoundVolume = 1.0;
        if (file >> savedMasterSoundVolume) {
            gMasterSoundVolume = std::clamp(
                savedMasterSoundVolume, 0.0, 1.0);
        }
    }
}

void SavePlayerData() {
    std::ofstream file(PlayerSavePath(), std::ios::trunc);
    if (file) {
        file << gOwnedMoney << '\n'
             << (gIsTrophyPurchased ? 1 : 0) << '\n'
             << (gSkipTutorial ? 1 : 0) << '\n'
             << gMasterSoundVolume << '\n';
    }
}
enum class NpcPart {
    BackHair,
    Bottom,
    Top,
    Face,
    Eyes,
    FrontHair,
    Count
};
constexpr int kNpcPartCount = static_cast<int>(NpcPart::Count);
constexpr int kMaximumNpcPartVariants = 4;
constexpr int kNpcPartVariantCounts[kNpcPartCount] = {3, 3, 4, 1, 4, 3};
constexpr wchar_t kNpcPartImagePaths
    [kNpcPartCount][kMaximumNpcPartVariants][64] = {
    {
        L"npc\\back_hair\\back_hair_01.png",
        L"npc\\back_hair\\back_hair_02.png",
        L"npc\\back_hair\\back_hair_03.png"
    },
    {
        L"npc\\bottom\\bottom_01.png",
        L"npc\\bottom\\bottom_02.png",
        L"npc\\bottom\\bottom_03.png"
    },
    {
        L"npc\\top\\top_01.png",
        L"npc\\top\\top_02.png",
        L"npc\\top\\top_03.png",
        L"npc\\top\\top_04.png"
    },
    {L"npc\\face\\face_01.png"},
    {
        L"npc\\eyes\\eyes_01.png",
        L"npc\\eyes\\eyes_02.png",
        L"npc\\eyes\\eyes_03.png",
        L"npc\\eyes\\eyes_04.png"
    },
    {
        L"npc\\front_hair\\front_hair_01.png",
        L"npc\\front_hair\\front_hair_02.png",
        L"npc\\front_hair\\front_hair_03.png"
    }
};
Gdiplus::Image* gNpcPartImages
    [kNpcPartCount][kMaximumNpcPartVariants]{};
int gSelectedNpcPartVariants[kNpcPartCount]{};
bool gNpcEntersFromLeft = false;

std::wstring GetExecutableDirectory() {
    std::vector<wchar_t> path(512);
    DWORD length = 0;
    for (;;) {
        SetLastError(ERROR_SUCCESS);
        length = GetModuleFileName(
            nullptr, path.data(), static_cast<DWORD>(path.size()));
        if (length == 0) {
            return std::wstring();
        }
        if (length < path.size() - 1
            || (length < path.size() && GetLastError() != ERROR_INSUFFICIENT_BUFFER)) {
            break;
        }
        if (path.size() >= 32768) {
            return std::wstring();
        }
        path.resize((std::min)(path.size() * 2, static_cast<size_t>(32768)));
    }
    std::wstring directory(path.data(), length);
    const std::wstring::size_type slash = directory.find_last_of(L"\\/");
    return (slash == std::wstring::npos)
        ? std::wstring()
        : directory.substr(0, slash + 1);
}

bool FileExists(const std::wstring& path) {
    const DWORD attributes = GetFileAttributes(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES
        && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

std::wstring FindAssetsDirectory() {
    std::wstring directory = GetExecutableDirectory();
    for (int depth = 0; depth < 8 && !directory.empty(); ++depth) {
        const std::wstring candidate = directory + L"assets\\";
        if (FileExists(candidate + L"narration.json")) {
            return candidate;
        }

        while (!directory.empty()
            && (directory.back() == L'\\' || directory.back() == L'/')) {
            directory.pop_back();
        }
        const std::wstring::size_type slash = directory.find_last_of(L"\\/");
        if (slash == std::wstring::npos) {
            break;
        }
        directory.erase(slash + 1);
    }
    return std::wstring();
}

void SkipJsonWhitespace(const std::string& json, size_t& position) {
    while (position < json.size()
        && (json[position] == ' '
            || json[position] == '\t'
            || json[position] == '\r'
            || json[position] == '\n')) {
        ++position;
    }
}

bool ParseJsonString(
    const std::string& json, size_t& position, std::string& value) {
    SkipJsonWhitespace(json, position);
    if (position >= json.size() || json[position] != '"') {
        return false;
    }

    ++position;
    value.clear();
    while (position < json.size()) {
        const char character = json[position++];
        if (character == '"') {
            return true;
        }
        if (character == '\\' && position < json.size()) {
            const char escaped = json[position++];
            switch (escaped) {
            case '"': value.push_back('"'); break;
            case '\\': value.push_back('\\'); break;
            case '/': value.push_back('/'); break;
            case 'b': value.push_back('\b'); break;
            case 'f': value.push_back('\f'); break;
            case 'n': value.push_back('\n'); break;
            case 'r': value.push_back('\r'); break;
            case 't': value.push_back('\t'); break;
            default: return false;
            }
        } else {
            value.push_back(character);
        }
    }
    return false;
}

size_t FindJsonProperty(const std::string& json, const std::string& name) {
    const std::string propertyName = "\"" + name + "\"";
    size_t namePosition = 0;
    while ((namePosition = json.find(propertyName, namePosition))
        != std::string::npos) {
        size_t colonPosition = namePosition + propertyName.size();
        SkipJsonWhitespace(json, colonPosition);
        if (colonPosition < json.size() && json[colonPosition] == ':') {
            size_t valuePosition = colonPosition + 1;
            SkipJsonWhitespace(json, valuePosition);
            return valuePosition;
        }
        namePosition += propertyName.size();
    }
    return std::string::npos;
}

size_t FindMatchingJsonDelimiter(
    const std::string& json, size_t start, char opening, char closing) {
    int depth = 0;
    bool insideString = false;
    bool escaped = false;
    for (size_t position = start; position < json.size(); ++position) {
        const char character = json[position];
        if (insideString) {
            if (escaped) {
                escaped = false;
            } else if (character == '\\') {
                escaped = true;
            } else if (character == '"') {
                insideString = false;
            }
            continue;
        }
        if (character == '"') {
            insideString = true;
        } else if (character == opening) {
            ++depth;
        } else if (character == closing && --depth == 0) {
            return position;
        }
    }
    return std::string::npos;
}

bool ParseJsonStringArray(
    const std::string& json,
    size_t arrayPosition,
    std::vector<std::string>& values) {
    if (arrayPosition >= json.size() || json[arrayPosition] != '[') {
        return false;
    }
    ++arrayPosition;
    values.clear();
    while (arrayPosition < json.size()) {
        SkipJsonWhitespace(json, arrayPosition);
        if (arrayPosition >= json.size()) {
            return false;
        }
        if (json[arrayPosition] == ']') {
            return true;
        }
        std::string value;
        if (!ParseJsonString(json, arrayPosition, value)) {
            return false;
        }
        values.push_back(value);
        SkipJsonWhitespace(json, arrayPosition);
        if (arrayPosition >= json.size()) {
            return false;
        }
        if (json[arrayPosition] == ',') {
            ++arrayPosition;
        } else if (json[arrayPosition] != ']') {
            return false;
        }
    }
    return false;
}

bool ParseJsonInteger(
    const std::string& json, size_t position, int& value) {
    SkipJsonWhitespace(json, position);
    if (position >= json.size()) {
        return false;
    }
    bool negative = false;
    if (json[position] == '-') {
        negative = true;
        ++position;
    }
    if (position >= json.size()
        || json[position] < '0'
        || json[position] > '9') {
        return false;
    }
    unsigned int parsedValue = 0;
    const unsigned int maximumMagnitude = negative
        ? static_cast<unsigned int>((std::numeric_limits<int>::max)()) + 1u
        : static_cast<unsigned int>((std::numeric_limits<int>::max)());
    while (position < json.size()
        && json[position] >= '0'
        && json[position] <= '9') {
        const unsigned int digit = static_cast<unsigned int>(
            json[position] - '0');
        if (parsedValue > (maximumMagnitude - digit) / 10) {
            return false;
        }
        parsedValue = parsedValue * 10 + digit;
        ++position;
    }
    value = negative
        ? (parsedValue == maximumMagnitude
            ? (std::numeric_limits<int>::min)()
            : -static_cast<int>(parsedValue))
        : static_cast<int>(parsedValue);
    return true;
}

std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) {
        return std::wstring();
    }
    const int length = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
        static_cast<int>(text.size()), nullptr, 0);
    if (length <= 0) {
        return std::wstring();
    }
    std::wstring converted(static_cast<size_t>(length), L'\0');
    MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
        static_cast<int>(text.size()), converted.data(), length);
    return converted;
}

int FindMaterialIndex(const std::string& materialId) {
    const std::wstring wideId = Utf8ToWide(materialId);
    for (int index = 0; index < kMaterialBinCount; ++index) {
        if (wideId == kMaterialIds[index]) {
            return index;
        }
    }
    return -1;
}

bool LoadNarrationData() {
    const std::wstring path = gAssetsDirectory + L"narration.json";
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }
    const std::string json(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    std::vector<std::string> names;
    const size_t namesPosition = FindJsonProperty(json, "names");
    if (!ParseJsonStringArray(json, namesPosition, names)) {
        return false;
    }
    for (const std::string& name : names) {
        gNarrationNames.push_back(Utf8ToWide(name));
    }

    const size_t categoriesStart = FindJsonProperty(
        json, "customerCategories");
    const size_t categoriesEnd = FindMatchingJsonDelimiter(
        json, categoriesStart, '[', ']');
    if (categoriesStart == std::string::npos
        || categoriesEnd == std::string::npos) {
        return false;
    }

    size_t position = categoriesStart + 1;
    while (position < categoriesEnd) {
        position = json.find('{', position);
        if (position == std::string::npos || position >= categoriesEnd) {
            break;
        }
        const size_t objectEnd = FindMatchingJsonDelimiter(json, position, '{', '}');
        if (objectEnd == std::string::npos || objectEnd > categoriesEnd) {
            return false;
        }
        const std::string categoryJson = json.substr(
            position, objectEnd - position + 1);

        size_t categoryIdPosition = FindJsonProperty(categoryJson, "id");
        std::string categoryId;
        if (!ParseJsonString(
                categoryJson, categoryIdPosition, categoryId)) {
            return false;
        }

        const size_t menusStart = FindJsonProperty(categoryJson, "menus");
        const size_t menusEnd = FindMatchingJsonDelimiter(
            categoryJson, menusStart, '[', ']');
        if (menusStart == std::string::npos
            || menusEnd == std::string::npos) {
            return false;
        }

        DialogueCategory category;
        category.id = Utf8ToWide(categoryId);
        size_t menuPosition = menusStart + 1;
        while (menuPosition < menusEnd) {
            menuPosition = categoryJson.find('{', menuPosition);
            if (menuPosition == std::string::npos
                || menuPosition >= menusEnd) {
                break;
            }
            const size_t menuEnd = FindMatchingJsonDelimiter(
                categoryJson, menuPosition, '{', '}');
            if (menuEnd == std::string::npos || menuEnd > menusEnd) {
                return false;
            }
            const std::string menuJson = categoryJson.substr(
                menuPosition, menuEnd - menuPosition + 1);

            size_t menuIdPosition = FindJsonProperty(menuJson, "id");
            std::string menuId;
            if (!ParseJsonString(menuJson, menuIdPosition, menuId)) {
                return false;
            }
            size_t menuNamePosition = FindJsonProperty(menuJson, "name");
            std::string menuName;
            if (!ParseJsonString(menuJson, menuNamePosition, menuName)) {
                return false;
            }

            DialogueTree tree;
            tree.menuId = Utf8ToWide(menuId);
            tree.menuName = Utf8ToWide(menuName);

            const size_t ingredientsStart = FindJsonProperty(
                menuJson, "ingredients");
            const size_t ingredientsEnd = FindMatchingJsonDelimiter(
                menuJson, ingredientsStart, '[', ']');
            if (ingredientsStart == std::string::npos
                || ingredientsEnd == std::string::npos) {
                return false;
            }
            size_t ingredientPosition = ingredientsStart + 1;
            while (ingredientPosition < ingredientsEnd) {
                ingredientPosition = menuJson.find('{', ingredientPosition);
                if (ingredientPosition == std::string::npos
                    || ingredientPosition >= ingredientsEnd) {
                    break;
                }
                const size_t ingredientEnd = FindMatchingJsonDelimiter(
                    menuJson, ingredientPosition, '{', '}');
                if (ingredientEnd == std::string::npos
                    || ingredientEnd > ingredientsEnd) {
                    return false;
                }
                const std::string ingredientJson = menuJson.substr(
                    ingredientPosition,
                    ingredientEnd - ingredientPosition + 1);
                size_t materialPosition = FindJsonProperty(
                    ingredientJson, "materialId");
                std::string materialId;
                int quantity = 0;
                if (!ParseJsonString(
                        ingredientJson, materialPosition, materialId)
                    || !ParseJsonInteger(
                        ingredientJson,
                        FindJsonProperty(ingredientJson, "quantity"),
                        quantity)
                    || quantity <= 0) {
                    return false;
                }
                const int materialIndex = FindMaterialIndex(materialId);
                if (materialIndex < 0) {
                    return false;
                }
                tree.recipe.push_back({materialIndex, quantity});
                ingredientPosition = ingredientEnd + 1;
            }
            if (tree.recipe.empty()) {
                return false;
            }

            const size_t dialogueStart = FindJsonProperty(
                menuJson, "dialogueTree");
            const size_t dialogueEnd = FindMatchingJsonDelimiter(
                menuJson, dialogueStart, '{', '}');
            if (dialogueStart == std::string::npos
                || dialogueEnd == std::string::npos) {
                return false;
            }
            const std::string treeJson = menuJson.substr(
                dialogueStart, dialogueEnd - dialogueStart + 1);
            size_t treeIdPosition = FindJsonProperty(treeJson, "id");
            std::string treeId;
            if (!ParseJsonString(treeJson, treeIdPosition, treeId)) {
                return false;
            }
            tree.id = Utf8ToWide(treeId);

            auto parseRequiredLines = [&treeJson](
                const char* property,
                std::vector<std::wstring>& destination) {
                std::vector<std::string> lines;
                if (!ParseJsonStringArray(
                        treeJson,
                        FindJsonProperty(treeJson, property),
                        lines)
                    || lines.empty()) {
                    return false;
                }
                for (const std::string& line : lines) {
                    destination.push_back(Utf8ToWide(line));
                }
                return true;
            };
            if (!parseRequiredLines("I_entry", tree.entryLines)
                || !parseRequiredLines(
                    "II_additional", tree.additionalLines)
                || !parseRequiredLines(
                    "IV_afterCooking", tree.afterCookingLines)
                || !parseRequiredLines(
                    "IV_afterCooking70Percent",
                    tree.afterCooking70PercentLines)
                || !parseRequiredLines(
                    "IV_afterCooking50Percent",
                    tree.afterCooking50PercentLines)
                || !parseRequiredLines(
                    "IV_afterCooking0Percent",
                    tree.afterCooking0PercentLines)
                || !parseRequiredLines(
                    "V_emptySubmission", tree.emptySubmissionLines)) {
                return false;
            }

            const size_t hintsStart = FindJsonProperty(
                treeJson, "III_hoverHints");
            const size_t hintsEnd = FindMatchingJsonDelimiter(
                treeJson, hintsStart, '[', ']');
            if (hintsStart == std::string::npos
                || hintsEnd == std::string::npos) {
                return false;
            }
            size_t hintPosition = hintsStart + 1;
            while (hintPosition < hintsEnd) {
                hintPosition = treeJson.find('{', hintPosition);
                if (hintPosition == std::string::npos
                    || hintPosition >= hintsEnd) {
                    break;
                }
                const size_t hintEnd = FindMatchingJsonDelimiter(
                    treeJson, hintPosition, '{', '}');
                if (hintEnd == std::string::npos || hintEnd > hintsEnd) {
                    return false;
                }
                const std::string hintJson = treeJson.substr(
                    hintPosition, hintEnd - hintPosition + 1);
                size_t materialPosition = FindJsonProperty(
                    hintJson, "materialId");
                std::string materialId;
                if (!ParseJsonString(
                        hintJson, materialPosition, materialId)) {
                    return false;
                }
                DialogueTree::HoverDialogue hoverDialogue;
                hoverDialogue.materialIndex = FindMaterialIndex(materialId);
                std::vector<std::string> nearbyLines;
                std::vector<std::string> exactLines;
                if (hoverDialogue.materialIndex < 0
                    || !ParseJsonStringArray(
                        hintJson,
                        FindJsonProperty(hintJson, "nearby"),
                        nearbyLines)
                    || !ParseJsonStringArray(
                        hintJson,
                        FindJsonProperty(hintJson, "exact"),
                        exactLines)) {
                    return false;
                }
                for (const std::string& line : nearbyLines) {
                    hoverDialogue.nearbyLines.push_back(Utf8ToWide(line));
                }
                for (const std::string& line : exactLines) {
                    hoverDialogue.exactLines.push_back(Utf8ToWide(line));
                }
                tree.hoverDialogues.push_back(hoverDialogue);
                hintPosition = hintEnd + 1;
            }
            category.menus.push_back(tree);
            menuPosition = menuEnd + 1;
        }
        if (category.menus.size() != 3) {
            return false;
        }
        gDialogueCategories.push_back(category);
        position = objectEnd + 1;
    }
    return !gNarrationNames.empty() && gDialogueCategories.size() == 6;
}

void LoadMaterialImages() {
    for (int index = 0; index < kMaterialBinCount; ++index) {
        const std::wstring imagePath = gAssetsDirectory + kMaterialImagePaths[index];
        Gdiplus::Image* image = Gdiplus::Image::FromFile(imagePath.c_str(), FALSE);
        if (image != nullptr && image->GetLastStatus() == Gdiplus::Ok) {
            gMaterialImages[index] = image;
            Gdiplus::Bitmap* shadow = new Gdiplus::Bitmap(
                kMaterialImageSourceSize,
                kMaterialImageSourceSize,
                PixelFormat32bppARGB);
            if (shadow->GetLastStatus() == Gdiplus::Ok) {
                Gdiplus::Graphics graphics(shadow);
                graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
                Gdiplus::ColorMatrix matrix = {
                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.15f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f
                };
                Gdiplus::ImageAttributes attributes;
                attributes.SetColorMatrix(&matrix);
                graphics.DrawImage(
                    image,
                    Gdiplus::Rect(
                        0, 0,
                        kMaterialImageSourceSize,
                        kMaterialImageSourceSize),
                    0, 0,
                    kMaterialImageSourceSize,
                    kMaterialImageSourceSize,
                    Gdiplus::UnitPixel,
                    &attributes);
                gMaterialShadowImages[index] = shadow;
            } else {
                delete shadow;
            }
        } else {
            delete image;
        }
    }
}

Gdiplus::Image* LoadOptionalImage(const wchar_t* relativePath) {
    const std::wstring imagePath = gAssetsDirectory + relativePath;
    Gdiplus::Image* image = Gdiplus::Image::FromFile(imagePath.c_str(), FALSE);
    if (image != nullptr && image->GetLastStatus() == Gdiplus::Ok) {
        return image;
    }
    delete image;
    return nullptr;
}

void LoadMoneyImages() {
    gOwnedMoneyCoinImage = LoadOptionalImage(kOwnedMoneyCoinImagePath);
    gEarnedMoneyCoinImage = LoadOptionalImage(kEarnedMoneyCoinImagePath);
    gObjectiveImage = LoadOptionalImage(kObjectiveImagePath);
    gTrophyImage = LoadOptionalImage(kTrophyImagePath);
    gTitleImage = LoadOptionalImage(kTitleImagePath);
    constexpr wchar_t completedFoodPaths[kCompletedFoodImageCount][40] = {
        L"materials\\kimbop.png",
        L"materials\\mintkimbop.png",
        L"materials\\onlykimbop.png",
        L"materials\\saladwrap.png",
        L"materials\\salad.png",
        L"materials\\mint_chocolate_2.png"
    };
    for (int index = 0; index < kCompletedFoodImageCount; ++index) {
        gCompletedFoodImages[index] = LoadOptionalImage(
            completedFoodPaths[index]);
        if (gCompletedFoodImages[index] == nullptr) {
            continue;
        }
        const int shadowWidth = kCompletedFoodDisplayWidth
            + kCompletedFoodShadowBlurRadius * 2;
        const int shadowHeight = kCompletedFoodDisplayHeight
            + kCompletedFoodShadowBlurRadius * 2;
        Gdiplus::Bitmap* shadow = new Gdiplus::Bitmap(
            shadowWidth, shadowHeight, PixelFormat32bppARGB);
        if (shadow->GetLastStatus() != Gdiplus::Ok) {
            delete shadow;
            continue;
        }
        Gdiplus::Graphics graphics(shadow);
        graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        constexpr int blurWeights[] = {1, 4, 6, 4, 1};
        constexpr float totalBlurWeight = 256.0f;
        for (int y = -kCompletedFoodShadowBlurRadius;
             y <= kCompletedFoodShadowBlurRadius;
             ++y) {
            for (int x = -kCompletedFoodShadowBlurRadius;
                 x <= kCompletedFoodShadowBlurRadius;
                 ++x) {
                const float alpha = 0.15f
                    * blurWeights[x + kCompletedFoodShadowBlurRadius]
                    * blurWeights[y + kCompletedFoodShadowBlurRadius]
                    / totalBlurWeight;
                Gdiplus::ColorMatrix matrix = {
                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, alpha, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f
                };
                Gdiplus::ImageAttributes attributes;
                attributes.SetColorMatrix(&matrix);
                graphics.DrawImage(
                    gCompletedFoodImages[index],
                    Gdiplus::Rect(
                        kCompletedFoodShadowBlurRadius + x,
                        kCompletedFoodShadowBlurRadius + y,
                        kCompletedFoodDisplayWidth,
                        kCompletedFoodDisplayHeight),
                    0, 0,
                    kCompletedFoodSourceWidth,
                    kCompletedFoodSourceHeight,
                    Gdiplus::UnitPixel,
                    &attributes);
            }
        }
        gCompletedFoodShadowImages[index] = shadow;
    }
    gFogImage = LoadOptionalImage(L"materials\\fog.png");
    if (gTrophyImage != nullptr) {
        const int shadowSize = kTrophyImageDisplaySize
            + kTrophyShadowBlurRadius * 2;
        gTrophyShadowImage = new Gdiplus::Bitmap(
            shadowSize, shadowSize, PixelFormat32bppARGB);
        if (gTrophyShadowImage->GetLastStatus() == Gdiplus::Ok) {
            Gdiplus::Graphics graphics(gTrophyShadowImage);
            graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
            constexpr int blurWeights[] = {1, 4, 6, 4, 1};
            constexpr float totalBlurWeight = 256.0f;
            for (int y = -kTrophyShadowBlurRadius;
                 y <= kTrophyShadowBlurRadius;
                 ++y) {
                for (int x = -kTrophyShadowBlurRadius;
                     x <= kTrophyShadowBlurRadius;
                     ++x) {
                    const float alpha = 0.15f
                        * blurWeights[x + kTrophyShadowBlurRadius]
                        * blurWeights[y + kTrophyShadowBlurRadius]
                        / totalBlurWeight;
                    Gdiplus::ColorMatrix matrix = {
                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, alpha, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f
                    };
                    Gdiplus::ImageAttributes attributes;
                    attributes.SetColorMatrix(&matrix);
                    graphics.DrawImage(
                        gTrophyImage,
                        Gdiplus::Rect(
                            kTrophyShadowBlurRadius + x,
                            kTrophyShadowBlurRadius + y,
                            kTrophyImageDisplaySize,
                            kTrophyImageDisplaySize),
                        0, 0,
                        kTrophyImageSourceSize,
                        kTrophyImageSourceSize,
                        Gdiplus::UnitPixel,
                        &attributes);
                }
            }
        } else {
            delete gTrophyShadowImage;
            gTrophyShadowImage = nullptr;
        }
    }
}

void LoadNpcImages() {
    for (int part = 0; part < kNpcPartCount; ++part) {
        for (int variant = 0;
            variant < kNpcPartVariantCounts[part];
            ++variant) {
            gNpcPartImages[part][variant] = LoadOptionalImage(
                kNpcPartImagePaths[part][variant]);
        }
    }
}

void UnloadMaterialImages() {
    for (Gdiplus::Bitmap*& image : gMaterialShadowImages) {
        delete image;
        image = nullptr;
    }
    for (Gdiplus::Image*& image : gMaterialImages) {
        delete image;
        image = nullptr;
    }
}

void UnloadMoneyImages() {
    delete gTrophyShadowImage;
    gTrophyShadowImage = nullptr;
    for (Gdiplus::Bitmap*& image : gCompletedFoodShadowImages) {
        delete image;
        image = nullptr;
    }
    for (Gdiplus::Image*& image : gCompletedFoodImages) {
        delete image;
        image = nullptr;
    }
    delete gFogImage;
    gFogImage = nullptr;
    delete gOwnedMoneyCoinImage;
    gOwnedMoneyCoinImage = nullptr;
    delete gEarnedMoneyCoinImage;
    gEarnedMoneyCoinImage = nullptr;
    delete gObjectiveImage;
    gObjectiveImage = nullptr;
    delete gTrophyImage;
    gTrophyImage = nullptr;
    delete gTitleImage;
    gTitleImage = nullptr;
}

void UnloadNpcImages() {
    for (auto& partImages : gNpcPartImages) {
        for (Gdiplus::Image*& image : partImages) {
            delete image;
            image = nullptr;
        }
    }
}

struct Layout {
    double scale;
    int offsetX;
    int offsetY;
};

void DrawCenteredText(
    HDC dc,
    const Layout& layout,
    const RECT& logicalArea,
    const wchar_t* text,
    double fontHeight,
    COLORREF color,
    int weight = FW_NORMAL,
    const wchar_t* fontName = kDefaultUiFontName);

Layout GetLayout(const RECT& client) {
    const int width = client.right - client.left;
    const int height = client.bottom - client.top;
    const double scale = (std::min)(
        static_cast<double>(width) / kDesignWidth,
        static_cast<double>(height) / kDesignHeight);

    const int designWidth = static_cast<int>(std::lround(kDesignWidth * scale));
    const int designHeight = static_cast<int>(std::lround(kDesignHeight * scale));
    return {scale, (width - designWidth) / 2, (height - designHeight) / 2};
}

int ToScreen(double value, double scale, int offset) {
    return offset + static_cast<int>(std::lround(value * scale));
}

void FillSolid(HDC dc, const RECT& rect, COLORREF color) {
    const HBRUSH brush = CreateSolidBrush(color);
    if (brush == nullptr) {
        return;
    }
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

void FillTranslucent(HDC dc, const RECT& rect, COLORREF color, BYTE opacity) {
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    HDC layerDc = CreateCompatibleDC(dc);
    if (layerDc == nullptr) {
        return;
    }
    HBITMAP layerBitmap = CreateCompatibleBitmap(dc, width, height);
    if (layerBitmap == nullptr) {
        DeleteDC(layerDc);
        return;
    }
    HGDIOBJ oldBitmap = SelectObject(layerDc, layerBitmap);
    const RECT layerRect{0, 0, width, height};
    FillSolid(layerDc, layerRect, color);

    const BLENDFUNCTION blend{AC_SRC_OVER, 0, opacity, 0};
    AlphaBlend(
        dc, rect.left, rect.top, width, height,
        layerDc, 0, 0, width, height, blend);

    SelectObject(layerDc, oldBitmap);
    DeleteObject(layerBitmap);
    DeleteDC(layerDc);
}

RECT LogicalRect(const Layout& layout, int x, int y, int width, int height) {
    return {
        ToScreen(x, layout.scale, layout.offsetX),
        ToScreen(y, layout.scale, layout.offsetY),
        ToScreen(x + width, layout.scale, layout.offsetX),
        ToScreen(y + height, layout.scale, layout.offsetY)
    };
}

POINT LogicalPoint(const Layout& layout, double x, double y) {
    return {
        ToScreen(x, layout.scale, layout.offsetX),
        ToScreen(y, layout.scale, layout.offsetY)
    };
}

void FillPolygon(HDC dc, POINT* points, int pointCount, COLORREF color) {
    const HBRUSH brush = CreateSolidBrush(color);
    const HGDIOBJ oldBrush = SelectObject(dc, brush);
    const HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
    Polygon(dc, points, pointCount);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);
}

void FillRoundedRect(
    HDC dc, const RECT& rect, int radius, COLORREF color) {
    const HBRUSH brush = CreateSolidBrush(color);
    const HGDIOBJ oldBrush = SelectObject(dc, brush);
    const HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
    RoundRect(
        dc,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom,
        radius,
        radius);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);
}

COLORREF BlendColor(COLORREF background, COLORREF foreground, double opacity) {
    const double amount = (std::max)(0.0, (std::min)(1.0, opacity));
    const auto blendChannel = [amount](BYTE from, BYTE to) {
        return static_cast<BYTE>(std::lround(from + (to - from) * amount));
    };
    return RGB(
        blendChannel(GetRValue(background), GetRValue(foreground)),
        blendChannel(GetGValue(background), GetGValue(foreground)),
        blendChannel(GetBValue(background), GetBValue(foreground)));
}

unsigned int NextRandom() {
    gRandomState = gRandomState * 1664525u + 1013904223u;
    return gRandomState;
}

int RandomRange(int minimum, int maximum) {
    if (maximum <= minimum) {
        return minimum;
    }
    const unsigned int range = static_cast<unsigned int>(
        maximum - minimum + 1);
    // LCG의 하위 비트는 짧은 주기로 반복되므로 상위 비트를 사용해
    // 연속 추첨되는 NPC 파츠가 같은 번호로 묶여 보이는 현상을 줄인다.
    return minimum + static_cast<int>((NextRandom() >> 8) % range);
}

POINT ProjectFloorPoint(const Layout& layout, int column, int row) {
    const double rowRatio = static_cast<double>(row) / kFloorRows;
    const double startY = gVanishingPoint.y
        + (kPlayAreaSize - gVanishingPoint.y) * kFloorStartDepth;
    const double floorHeight = (kPlayAreaSize - startY) * kFloorHeightScale;
    const double columnRatio = static_cast<double>(column) / kFloorColumns - 0.5;
    const double rowWidthScale = 1.0
        + (kFloorWidthScale - 1.0) * rowRatio;

    // 바닥 시작선은 화면 폭을 가득 채우고 아래쪽으로만 완만하게 넓어진다.
    const double x = kPlayAreaX + gVanishingPoint.x
        + columnRatio * kPlayAreaSize * rowWidthScale;
    const double y = kPlayAreaY + startY + floorHeight * rowRatio;
    return LogicalPoint(layout, x, y);
}

void DrawPerspectiveFloor(HDC dc, const Layout& layout) {
    for (int row = 0; row < kFloorRows; ++row) {
        for (int column = 0; column < kFloorColumns; ++column) {
            POINT tile[] = {
                ProjectFloorPoint(layout, column, row),
                ProjectFloorPoint(layout, column + 1, row),
                ProjectFloorPoint(layout, column + 1, row + 1),
                ProjectFloorPoint(layout, column, row + 1)
            };
            const COLORREF color = ((row + column) % 2 == 0)
                ? kTileGreen
                : kTileWhite;
            FillPolygon(dc, tile, 4, color);
        }
    }
}

RECT MaterialBinRect(const Layout& layout, int row, int column) {
    const double totalGap = kMaterialBinColumnGap
        * (kMaterialBinColumns - 1);
    const double binWidth = (kPlayAreaSize - totalGap) / kMaterialBinColumns;
    const double left = kPlayAreaX
        + column * (binWidth + kMaterialBinColumnGap);
    const double right = left + binWidth;
    const int rowTop = ((row == 0)
        ? kMaterialBinTop
        : kMaterialBinFirstRowBottom + kMaterialBinRowGap)
        + kMaterialBinVerticalOffset;
    const int rowBottom = ((row == 0)
        ? kMaterialBinFirstRowBottom
        : kPlayAreaSize)
        + kMaterialBinVerticalOffset;
    const POINT topLeft = LogicalPoint(layout, left, kPlayAreaY + rowTop);
    const POINT bottomRight = LogicalPoint(layout, right, kPlayAreaY + rowBottom);
    return {topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
}

int HitTestPngSocket() {
    if (!gHasMousePosition
        || !gIsTrackingMouse
        || gIsCookingTransitionRunning
        || gCookingState != CookingState::Cooking) {
        return -1;
    }

    const double totalGap = kMaterialBinColumnGap
        * (kMaterialBinColumns - 1);
    const double binWidth = (kPlayAreaSize - totalGap) / kMaterialBinColumns;
    for (int row = 0; row < kMaterialBinRows; ++row) {
        const int rowTop = ((row == 0)
            ? kMaterialBinTop
            : kMaterialBinFirstRowBottom + kMaterialBinRowGap)
            + kMaterialBinVerticalOffset;
        const int rowBottom = ((row == 0)
            ? kMaterialBinFirstRowBottom
            : kPlayAreaSize)
            + kMaterialBinVerticalOffset;
        const double centerY = kPlayAreaY
            + (rowTop + rowBottom) * 0.5 - gTableLift;

        for (int column = 0; column < kMaterialBinColumns; ++column) {
            const double left = kPlayAreaX
                + column * (binWidth + kMaterialBinColumnGap);
            const double centerX = left + binWidth * 0.5;
            const double halfSize = kPngSocketSize * 0.5;
            if (gMouseDesignPosition.x >= centerX - halfSize
                && gMouseDesignPosition.x <= centerX + halfSize
                && gMouseDesignPosition.y >= centerY - halfSize
                && gMouseDesignPosition.y <= centerY + halfSize) {
                return row * kMaterialBinColumns + column;
            }
        }
    }
    return -1;
}

void DrawMaterialBins(HDC dc, const Layout& layout) {
    // 재료통들은 원근 변형 없이 같은 크기의 직사각형으로 배치한다.
    for (int row = 0; row < kMaterialBinRows; ++row) {
        for (int column = 0; column < kMaterialBinColumns; ++column) {
            const RECT bin = MaterialBinRect(layout, row, column);
            FillSolid(dc, bin, kMaterialBinBorderColor);

            // 재료통의 바깥 크기를 유지하면서 안쪽으로 2px 테두리를 만든다.
            const int borderWidth = static_cast<int>(
                std::lround(kMaterialBinBorderWidth * layout.scale));
            const RECT inner{
                bin.left + borderWidth,
                bin.top + borderWidth,
                bin.right - borderWidth,
                bin.bottom - borderWidth
            };
            FillSolid(dc, inner, kMaterialBinColor);
        }
    }
}

void DrawMaterialImage(HDC dc, int materialIndex, const RECT& area) {
    if (materialIndex >= 0
        && materialIndex < kMaterialBinCount
        && gMaterialImages[materialIndex] != nullptr) {
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
        graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
        graphics.DrawImage(
            gMaterialImages[materialIndex],
            Gdiplus::Rect(
                area.left,
                area.top,
                area.right - area.left,
                area.bottom - area.top),
            0,
            0,
            kMaterialImageSourceSize,
            kMaterialImageSourceSize,
            Gdiplus::UnitPixel);
    }
}

void RandomizeNpcAppearance() {
    for (int part = 0; part < kNpcPartCount; ++part) {
        const bool isHairPart = part == static_cast<int>(NpcPart::BackHair)
            || part == static_cast<int>(NpcPart::FrontHair);
        gSelectedNpcPartVariants[part] = RandomRange(
            isHairPart ? -1 : 0,
            kNpcPartVariantCounts[part] - 1);
    }
    gNpcEntersFromLeft = NextRandom() % 2 == 0;
}

void DrawMaterialShadow(HDC dc, int materialIndex, const RECT& area) {
    if (materialIndex < 0
        || materialIndex >= kMaterialBinCount
        || gMaterialShadowImages[materialIndex] == nullptr) {
        return;
    }

    Gdiplus::Graphics graphics(dc);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
    graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
    graphics.DrawImage(
        gMaterialShadowImages[materialIndex],
        Gdiplus::Rect(
            area.left,
            area.top,
            area.right - area.left,
            area.bottom - area.top),
        0,
        0,
        kMaterialImageSourceSize,
        kMaterialImageSourceSize,
        Gdiplus::UnitPixel);
}

void DrawRotatedMaterialImage(
    HDC dc, int materialIndex, const RECT& area, float angle) {
    if (materialIndex < 0
        || materialIndex >= kMaterialBinCount
        || gMaterialImages[materialIndex] == nullptr) {
        return;
    }

    Gdiplus::Graphics graphics(dc);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
    graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
    const float width = static_cast<float>(area.right - area.left);
    const float height = static_cast<float>(area.bottom - area.top);
    graphics.TranslateTransform(
        (area.left + area.right) * 0.5f,
        (area.top + area.bottom) * 0.5f);
    graphics.RotateTransform(angle);
    graphics.DrawImage(
        gMaterialImages[materialIndex],
        Gdiplus::RectF(-width * 0.5f, -height * 0.5f, width, height),
        0,
        0,
        kMaterialImageSourceSize,
        kMaterialImageSourceSize,
        Gdiplus::UnitPixel);
}

void DrawPngSocket(HDC dc, int binIndex, const RECT& socket) {
    // 투명 PNG만 재료통 중앙에 표시하며 별도 배경이나 테두리는 그리지 않는다.
    DrawMaterialShadow(dc, binIndex, socket);
    DrawMaterialImage(dc, binIndex, socket);
}

void DrawPngSockets(HDC dc, const Layout& layout) {
    // 각 PNG소켓은 재료통의 정중앙에 정방형으로 배치한다.
    for (int row = 0; row < kMaterialBinRows; ++row) {
        for (int column = 0; column < kMaterialBinColumns; ++column) {
            const int binIndex = row * kMaterialBinColumns + column;
            const RECT bin = MaterialBinRect(layout, row, column);
            const int socketSize = static_cast<int>(
                std::lround(
                    kPngSocketSize
                    * gPngSocketScales[binIndex]
                    * layout.scale));
            const int centerX = (bin.left + bin.right) / 2;
            const int centerY = (bin.top + bin.bottom) / 2;
            const RECT socket{
                centerX - socketSize / 2,
                centerY - socketSize / 2,
                centerX - socketSize / 2 + socketSize,
                centerY - socketSize / 2 + socketSize
            };
            DrawPngSocket(dc, binIndex, socket);
        }
    }
}

int CuttingBoardBaseY() {
    return kCuttingBoardCookingY + static_cast<int>(kCookingTableLift);
}

DialogueTree* CurrentDialogueTree();

bool RecipeContainsMaterial(const DialogueTree& tree, const wchar_t* id) {
    int materialIndex = -1;
    for (int index = 0; index < kMaterialBinCount; ++index) {
        if (std::wcscmp(kMaterialIds[index], id) == 0) {
            materialIndex = index;
            break;
        }
    }
    return std::any_of(
        tree.recipe.begin(), tree.recipe.end(),
        [materialIndex](const DialogueTree::RecipeIngredient& ingredient) {
            return ingredient.materialIndex == materialIndex;
        });
}

void StartCompletionPresentation() {
    DialogueTree* tree = CurrentDialogueTree();
    if (tree == nullptr || gMaterialCloneCount == 0) {
        return;
    }
    const bool hasSeaweed = RecipeContainsMaterial(*tree, L"seaweed_and_rice");
    const bool hasMint = RecipeContainsMaterial(*tree, L"mint_chocolate");
    const bool hasTortilla = RecipeContainsMaterial(*tree, L"tortilla");
    if (tree->menuName == L"kimbopx10"
        || tree->menuName == L"kimbop_x10") {
        gCompletedFoodImageIndex = 2;
    } else if (tree->menuName == L"mint_chocolate") {
        gCompletedFoodImageIndex = 5;
    } else if (hasSeaweed && hasMint) {
        gCompletedFoodImageIndex = 1;
    } else if (hasSeaweed) {
        gCompletedFoodImageIndex = 0;
    } else if (hasTortilla) {
        gCompletedFoodImageIndex = 3;
    } else {
        gCompletedFoodImageIndex = 4;
    }

    gFogParticles.clear();
    const int fogCount = RandomRange(2, 3);
    const int boardLeft = kPlayAreaX + kCuttingBoardX;
    const int boardTop = kPlayAreaY + CuttingBoardBaseY();
    for (int index = 0; index < fogCount; ++index) {
        gFogParticles.push_back({
            static_cast<double>(RandomRange(
                boardLeft - 10,
                boardLeft + kCuttingBoardWidth + 10 - kFogDisplaySize)),
            static_cast<double>(RandomRange(
                boardTop - 10,
                boardTop + kCuttingBoardHeight + 10 - kFogDisplaySize)),
            static_cast<double>(RandomRange(-3, 3)),
            NextRandom() % 2 == 0 ? -15.0 : 15.0
        });
    }
    gCompletionPresentationStartTime = GetTickCount64();
    gIsCompletionPresentationActive = true;
}

void DrawCompletionPresentation(HDC dc, const Layout& layout) {
    if (!gIsCompletionPresentationActive) {
        return;
    }
    const double progress = (std::min)(
        1.0,
        (GetTickCount64() - gCompletionPresentationStartTime)
            / 1000.0 / kCompletionPresentationSeconds);
    const int boardLeft = kPlayAreaX + kCuttingBoardX;
    const int boardTop = kPlayAreaY + CuttingBoardBaseY();
    if (gCompletedFoodImageIndex >= 0
        && gCompletedFoodImageIndex < kCompletedFoodImageCount
        && gCompletedFoodImages[gCompletedFoodImageIndex] != nullptr) {
        const RECT foodArea = LogicalRect(
            layout,
            boardLeft + (kCuttingBoardWidth - kCompletedFoodDisplayWidth) / 2,
            boardTop + (kCuttingBoardHeight - kCompletedFoodDisplayHeight) / 2,
            kCompletedFoodDisplayWidth,
            kCompletedFoodDisplayHeight);
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        if (gCompletedFoodShadowImages[gCompletedFoodImageIndex] != nullptr) {
            const RECT shadowArea = LogicalRect(
                layout,
                boardLeft
                    + (kCuttingBoardWidth - kCompletedFoodDisplayWidth) / 2
                    - kCompletedFoodShadowBlurRadius,
                boardTop
                    + (kCuttingBoardHeight - kCompletedFoodDisplayHeight) / 2
                    - kCompletedFoodShadowBlurRadius,
                kCompletedFoodDisplayWidth
                    + kCompletedFoodShadowBlurRadius * 2,
                kCompletedFoodDisplayHeight
                    + kCompletedFoodShadowBlurRadius * 2);
            graphics.DrawImage(
                gCompletedFoodShadowImages[gCompletedFoodImageIndex],
                Gdiplus::Rect(
                    shadowArea.left,
                    shadowArea.top,
                    shadowArea.right - shadowArea.left,
                    shadowArea.bottom - shadowArea.top));
        }
        graphics.DrawImage(
            gCompletedFoodImages[gCompletedFoodImageIndex],
            Gdiplus::Rect(foodArea.left, foodArea.top,
                foodArea.right - foodArea.left,
                foodArea.bottom - foodArea.top),
            0, 0, kCompletedFoodSourceWidth, kCompletedFoodSourceHeight,
            Gdiplus::UnitPixel);
    }
    if (gFogImage == nullptr) {
        return;
    }
    for (const FogParticle& fog : gFogParticles) {
        const POINT center = LogicalPoint(
            layout,
            fog.x + kFogDisplaySize * 0.5,
            fog.y + kFogDisplaySize * 0.5 + kFogFallDistance * progress);
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.TranslateTransform(
            static_cast<Gdiplus::REAL>(center.x),
            static_cast<Gdiplus::REAL>(center.y));
        graphics.RotateTransform(static_cast<Gdiplus::REAL>(
            fog.startAngle + (fog.targetAngle - fog.startAngle) * progress));
        Gdiplus::ColorMatrix opacityMatrix = {
            1, 0, 0, 0, 0,
            0, 1, 0, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 0, static_cast<Gdiplus::REAL>(1.0 - progress), 0,
            0, 0, 0, 0, 1
        };
        Gdiplus::ImageAttributes attributes;
        attributes.SetColorMatrix(&opacityMatrix);
        const int fogSize = static_cast<int>(std::lround(
            kFogDisplaySize * layout.scale));
        graphics.DrawImage(
            gFogImage,
            Gdiplus::Rect(-fogSize / 2, -fogSize / 2, fogSize, fogSize),
            0, 0, kFogSourceSize, kFogSourceSize,
            Gdiplus::UnitPixel, &attributes);
    }
}

void DrawCuttingBoardAndClones(HDC dc, const Layout& layout) {
    const RECT board = LogicalRect(
        layout,
        kPlayAreaX + kCuttingBoardX,
        kPlayAreaY + CuttingBoardBaseY(),
        kCuttingBoardWidth,
        kCuttingBoardHeight);
    FillSolid(dc, board, kCuttingBoardBorderColor);
    const int borderWidth = static_cast<int>(
        std::lround(kCuttingBoardBorderWidth * layout.scale));
    const RECT boardInner{
        board.left + borderWidth,
        board.top + borderWidth,
        board.right - borderWidth,
        board.bottom - borderWidth
    };
    FillSolid(dc, boardInner, kCuttingBoardColor);

    // 복제된 재료 PNG는 도마의 이동 좌표를 따라가며 도마 안에서만 표시된다.
    for (int index = 0; index < gMaterialCloneCount; ++index) {
        const MaterialClone& clone = gMaterialClones[index];
        const RECT imageArea = LogicalRect(
            layout,
            clone.x,
            clone.y,
            clone.size,
            clone.size);
        DrawRotatedMaterialImage(
            dc, clone.materialIndex, imageArea, clone.angle);
    }
    DrawCompletionPresentation(dc, layout);
}

void AddMaterialClone(int materialIndex) {
    if (materialIndex < 0
        || materialIndex >= kMaterialBinCount
        || gMaterialCloneCount >= kMaximumMaterialClones) {
        return;
    }

    constexpr int padding = 4;
    const int boardLeft = kPlayAreaX + kCuttingBoardX;
    const int boardTop = kPlayAreaY + CuttingBoardBaseY();
    MaterialClone& clone = gMaterialClones[gMaterialCloneCount++];
    clone.materialIndex = materialIndex;
    clone.size = static_cast<int>(std::lround(
        kPngSocketSize * RandomRange(95, 105) / 100.0));
    clone.angle = static_cast<float>(RandomRange(0, 359));
    clone.x = RandomRange(
        boardLeft + padding,
        boardLeft + kCuttingBoardWidth - padding - clone.size);
    clone.y = RandomRange(
        boardTop + padding,
        boardTop + kCuttingBoardHeight - padding - clone.size);
}

int HitTestMaterialClone(int designX, int designY) {
    if (gCookingState != CookingState::Cooking
        || gIsCookingTransitionRunning) {
        return -1;
    }
    constexpr double pi = 3.14159265358979323846;
    for (int index = gMaterialCloneCount - 1; index >= 0; --index) {
        const MaterialClone& clone = gMaterialClones[index];
        const double centerX = clone.x + clone.size * 0.5;
        const double centerY = clone.y - gTableLift + clone.size * 0.5;
        const double radians = -clone.angle * pi / 180.0;
        const double offsetX = designX - centerX;
        const double offsetY = designY - centerY;
        const double localX = offsetX * std::cos(radians)
            - offsetY * std::sin(radians);
        const double localY = offsetX * std::sin(radians)
            + offsetY * std::cos(radians);
        if (std::abs(localX) <= clone.size * 0.5
            && std::abs(localY) <= clone.size * 0.5) {
            return index;
        }
    }
    return -1;
}

void RemoveMaterialClone(int cloneIndex) {
    if (cloneIndex < 0 || cloneIndex >= gMaterialCloneCount) {
        return;
    }
    for (int index = cloneIndex; index + 1 < gMaterialCloneCount; ++index) {
        gMaterialClones[index] = gMaterialClones[index + 1];
    }
    --gMaterialCloneCount;
}

void AddClickStars(int designX, int designY) {
    if (gStarParticles.size() + 2 > kMaximumStarParticles) {
        gStarParticles.erase(
            gStarParticles.begin(),
            gStarParticles.begin() + 2);
    }
    for (int index = 0; index < 2; ++index) {
        const double direction = (index == 0) ? -1.0 : 1.0;
        gStarParticles.push_back({
            static_cast<double>(designX),
            static_cast<double>(designY),
            direction * RandomRange(45, 75),
            -static_cast<double>(RandomRange(90, 125)),
            static_cast<double>(RandomRange(0, 359)),
            direction * RandomRange(180, 300)
        });
    }
}

void DrawStarParticles(HDC dc, const Layout& layout) {
    Gdiplus::Graphics graphics(dc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
    constexpr double pi = 3.14159265358979323846;
    constexpr int pointCount = 10;
    for (const StarParticle& particle : gStarParticles) {
        const POINT center = LogicalPoint(layout, particle.x, particle.y);
        const double outerRadius = kStarParticleSize * layout.scale;
        const double innerRadius = outerRadius * 0.42;
        Gdiplus::PointF points[pointCount]{};
        for (int point = 0; point < pointCount; ++point) {
            const double radius = (point % 2 == 0)
                ? outerRadius
                : innerRadius;
            const double radians = (
                particle.angle - 90.0 + point * 36.0) * pi / 180.0;
            points[point] = Gdiplus::PointF(
                static_cast<float>(center.x + std::cos(radians) * radius),
                static_cast<float>(center.y + std::sin(radians) * radius));
        }
        graphics.FillPolygon(&brush, points, pointCount);
    }
}

void UpdateStarParticles(HWND window, ULONGLONG now) {
    if (gLastStarParticleUpdateTime == 0) {
        gLastStarParticleUpdateTime = now;
    }
    const double elapsed = (std::min)(
        0.05,
        (now - gLastStarParticleUpdateTime) / 1000.0);
    gLastStarParticleUpdateTime = now;

    RECT client{};
    GetClientRect(window, &client);
    const Layout layout = GetLayout(client);
    for (StarParticle& particle : gStarParticles) {
        particle.x += particle.velocityX * elapsed;
        particle.y += particle.velocityY * elapsed;
        particle.velocityY += kStarParticleGravity * elapsed;
        particle.angle += particle.angularVelocity * elapsed;
    }
    gStarParticles.erase(
        std::remove_if(
            gStarParticles.begin(),
            gStarParticles.end(),
            [&](const StarParticle& particle) {
                const POINT center = LogicalPoint(
                    layout, particle.x, particle.y);
                const int radius = static_cast<int>(std::ceil(
                    kStarParticleSize * layout.scale));
                return center.x + radius < client.left
                    || center.x - radius > client.right
                    || center.y + radius < client.top
                    || center.y - radius > client.bottom;
            }),
        gStarParticles.end());
}

RECT ResetButtonLogicalRect() {
    return {
        kPlayAreaX + kPlayAreaSize - kResetButtonMargin - kResetButtonWidth,
        kPlayAreaY + kPlayAreaSize - kResetButtonMargin - kResetButtonHeight,
        kPlayAreaX + kPlayAreaSize - kResetButtonMargin,
        kPlayAreaY + kPlayAreaSize - kResetButtonMargin
    };
}

RECT ResetButtonRect(const Layout& layout) {
    const RECT logicalButton = ResetButtonLogicalRect();
    return LogicalRect(
        layout,
        logicalButton.left,
        logicalButton.top,
        logicalButton.right - logicalButton.left,
        logicalButton.bottom - logicalButton.top);
}

void DrawResetButton(HDC dc, const Layout& layout) {
    const RECT logicalButton = ResetButtonLogicalRect();
    const RECT button = ResetButtonRect(layout);
    const int radius = static_cast<int>(
        std::lround(kResetButtonRadius * layout.scale));
    const COLORREF fadedColor = BlendColor(
        kPlatformColor, kResetButtonColor, gResetButtonOpacity);
    FillRoundedRect(dc, button, radius, fadedColor);
    const COLORREF textColor = BlendColor(
        kPlatformColor, RGB(0x18, 0x2a, 0x18), gResetButtonOpacity);
    DrawCenteredText(
        dc,
        layout,
        logicalButton,
        L"조리 완료",
        14.0,
        textColor,
        FW_BOLD);
}

void DrawGreenWalls(HDC dc, const Layout& layout) {
    // 창틀과 벽은 좌우 대칭으로 배치한다.
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 97, kPlayAreaY, 30, 110), kWallTrimColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 273, kPlayAreaY, 30, 110), kWallTrimColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY + 105, 127, 109), kWallColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 273, kPlayAreaY + 105, 127, 109), kWallColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY + 105, 127, 20), kWallTrimColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 273, kPlayAreaY + 105, 127, 20), kWallTrimColor);

    // 좌우 연두색 벽을 X=285, Y=98 지점과 같은 색상으로 통일한다.
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY + 123, 127, 91), kWallTrimColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 272, kPlayAreaY + 123, 128, 91),
        kWallTrimColor);
}

double CurrentDayElapsedSeconds() {
    if (gDayStartTime == 0) {
        return 0.0;
    }
    return (GetTickCount64() - gDayStartTime) / 1000.0;
}

COLORREF CurrentSkyColor() {
    const double elapsed = CurrentDayElapsedSeconds();
    if (elapsed < kSunsetStartSeconds) {
        return kSkyColor;
    }
    if (elapsed < kNightStartSeconds) {
        return BlendColor(
            kSkyColor,
            kSunsetSkyColor,
            (elapsed - kSunsetStartSeconds)
                / (kNightStartSeconds - kSunsetStartSeconds));
    }
    return BlendColor(
        kSunsetSkyColor,
        kNightSkyColor,
        (elapsed - kNightStartSeconds)
            / (kDayDurationSeconds - kNightStartSeconds));
}

void DrawInterior(HDC dc, const Layout& layout) {
    // 가장 아래 레이어인 하늘색을 플레이 영역 전체에 먼저 칠한다.
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY, kPlayAreaSize, kPlayAreaSize),
        CurrentSkyColor());

    // 좌우 유리창은 중앙 창보다 한 단계 밝은 하늘색을 사용한다.
    const COLORREF currentGlassColor = BlendColor(
        CurrentSkyColor(), RGB(0xff, 0xff, 0xff), 0.28);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY, 100, 105), currentGlassColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 300, kPlayAreaY, 100, 105), currentGlassColor);

    DrawGreenWalls(dc, layout);

    // 중앙 제작대와 재료 보관 영역은 원래의 수평 띠 형태를 유지한다.
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 127, kPlayAreaY + 107, 146, 18),
        kShelfDarkColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 127, kPlayAreaY + 130, 146, 23),
        kShelfColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 127, kPlayAreaY + 153, 146, 18),
        kShelfLightColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 127, kPlayAreaY + 171, 146, 43),
        kMaterialColor);
}

DialogueTree* CurrentDialogueTree() {
    if (gCurrentDialogueCategory < 0
        || gCurrentDialogueCategory
            >= static_cast<int>(gDialogueCategories.size())) {
        return nullptr;
    }
    DialogueCategory& category =
        gDialogueCategories[gCurrentDialogueCategory];
    if (gCurrentDialogueTree < 0
        || gCurrentDialogueTree >= static_cast<int>(category.menus.size())) {
        return nullptr;
    }
    return &category.menus[gCurrentDialogueTree];
}

void StartNarrationText(const std::wstring& text) {
    const ULONGLONG now = GetTickCount64();
    gCurrentNarrationText = text;
    gIsNarrationActive = true;
    gIsNarrationTyping = !text.empty();
    gNarrationStartTime = now;
    gNarrationVisibleLength = text.empty() ? 0 : 1;
    if (!text.empty() && !std::iswspace(text.front())) {
        PlaySoundEffect(SoundEffect::NpcTalking);
    }
    gNarrationCompletedTime = text.empty() ? now : 0;
    gWasNarrationRestoredFromHover = false;
    gNarrationParticles.clear();
    gNarrationParticleProcessedLength = 0;
    gLastNarrationParticleUpdateTime = now;
    gIsNarrationOverflowAnimating = false;
}

void StartRandomLine(const std::vector<std::wstring>& lines) {
    if (lines.empty()) {
        return;
    }
    StartNarrationText(lines[NextRandom() % lines.size()]);
}

bool IsSocketNearMaterial(int socketIndex, int materialIndex);
bool IsCookingStateActive();

void RollHoverDialogueAvailability(DialogueTree& tree) {
    tree.nearbySlotDialogues.clear();
    const bool exactOnly = tree.menuName == L"all_ingredient";
    for (DialogueTree::HoverDialogue& hover : tree.hoverDialogues) {
        hover.nearbyEnabled.clear();
        hover.exactEnabled.assign(hover.exactLines.size(), false);
        std::vector<size_t> exactCandidates;
        for (size_t index = 0; index < hover.exactLines.size(); ++index) {
            if (!hover.exactLines[index].empty()) {
                exactCandidates.push_back(index);
            }
        }
        if (!exactCandidates.empty()
            && NextRandom() % 100 < kExactNarrationChancePercent) {
            const size_t selected = exactCandidates[
                NextRandom() % exactCandidates.size()];
            hover.exactEnabled[selected] = true;
        }
    }

    if (exactOnly) {
        return;
    }

    const auto isExactSocket = [&tree](int socket) {
        return std::any_of(
            tree.hoverDialogues.begin(),
            tree.hoverDialogues.end(),
            [socket](const DialogueTree::HoverDialogue& hover) {
                return socket == hover.materialIndex;
            });
    };
    struct NearbySelection {
        size_t hoverIndex;
        int socketIndex;
    };
    std::vector<std::vector<int>> candidateSockets(tree.hoverDialogues.size());
    std::vector<NearbySelection> passed;
    for (size_t hoverIndex = 0;
         hoverIndex < tree.hoverDialogues.size();
         ++hoverIndex) {
        const DialogueTree::HoverDialogue& hover =
            tree.hoverDialogues[hoverIndex];
        const bool hasLine = std::any_of(
            hover.nearbyLines.begin(),
            hover.nearbyLines.end(),
            [](const std::wstring& line) { return !line.empty(); });
        if (!hasLine) {
            continue;
        }
        for (int socket = 0; socket < kMaterialBinCount; ++socket) {
            if (isExactSocket(socket)
                || !IsSocketNearMaterial(socket, hover.materialIndex)) {
                continue;
            }
            candidateSockets[hoverIndex].push_back(socket);
            if (NextRandom() % 100 < kHoverNarrationChancePercent) {
                passed.push_back({hoverIndex, socket});
            }
        }
    }

    if (passed.empty()) {
        std::vector<int> fallbackSockets;
        for (size_t hoverIndex = 0;
             hoverIndex < candidateSockets.size();
             ++hoverIndex) {
            for (const int socket : candidateSockets[hoverIndex]) {
                if (std::find(
                        fallbackSockets.begin(), fallbackSockets.end(), socket)
                    == fallbackSockets.end()) {
                    fallbackSockets.push_back(socket);
                }
            }
        }
        if (!fallbackSockets.empty()) {
            const int socket = fallbackSockets[
                NextRandom() % fallbackSockets.size()];
            std::vector<size_t> matchingHoverIndices;
            for (size_t hoverIndex = 0;
                 hoverIndex < candidateSockets.size();
                 ++hoverIndex) {
                if (std::find(
                        candidateSockets[hoverIndex].begin(),
                        candidateSockets[hoverIndex].end(),
                        socket) != candidateSockets[hoverIndex].end()) {
                    matchingHoverIndices.push_back(hoverIndex);
                }
            }
            passed.push_back({
                matchingHoverIndices[
                    NextRandom() % matchingHoverIndices.size()],
                socket
            });
        }
    }

    // If two materials only passed the same nearby cell, move the latter to a
    // free cell in its radius so both hints remain reachable.
    for (size_t index = 0; index < passed.size(); ++index) {
        for (size_t other = 0; other < index; ++other) {
            if (passed[index].socketIndex != passed[other].socketIndex
                || passed[index].hoverIndex == passed[other].hoverIndex) {
                continue;
            }
            const auto countForHover = [&passed](size_t hoverIndex) {
                return std::count_if(
                    passed.begin(), passed.end(),
                    [hoverIndex](const NearbySelection& selection) {
                        return selection.hoverIndex == hoverIndex;
                    });
            };
            if (countForHover(passed[index].hoverIndex) != 1
                || countForHover(passed[other].hoverIndex) != 1) {
                continue;
            }
            std::vector<int> freeSockets;
            for (const int candidate :
                 candidateSockets[passed[index].hoverIndex]) {
                const bool alreadyUsed = std::any_of(
                    passed.begin(), passed.end(),
                    [candidate](const NearbySelection& selection) {
                        return selection.socketIndex == candidate;
                    });
                if (!alreadyUsed) {
                    freeSockets.push_back(candidate);
                }
            }
            if (!freeSockets.empty()) {
                passed[index].socketIndex = freeSockets[
                    NextRandom() % freeSockets.size()];
            }
        }
    }

    std::vector<int> selectedSockets;
    while (!passed.empty() && selectedSockets.size() < 2) {
        const size_t pickedIndex = NextRandom() % passed.size();
        const int socket = passed[pickedIndex].socketIndex;
        std::vector<size_t> matchingHoverIndices;
        for (const NearbySelection& selection : passed) {
            if (selection.socketIndex == socket) {
                matchingHoverIndices.push_back(selection.hoverIndex);
            }
        }
        const size_t hoverIndex = matchingHoverIndices[
            NextRandom() % matchingHoverIndices.size()];
        const auto& lines = tree.hoverDialogues[hoverIndex].nearbyLines;
        std::vector<const std::wstring*> lineCandidates;
        for (const std::wstring& line : lines) {
            if (!line.empty()) {
                lineCandidates.push_back(&line);
            }
        }
        if (!lineCandidates.empty()) {
            tree.nearbySlotDialogues.push_back({
                socket,
                *lineCandidates[NextRandom() % lineCandidates.size()]
            });
            selectedSockets.push_back(socket);
        }
        passed.erase(
            std::remove_if(
                passed.begin(), passed.end(),
                [socket](const NearbySelection& selection) {
                    return selection.socketIndex == socket;
                }),
            passed.end());
    }
}

void StartDialogueLine(
    const std::vector<std::wstring>& lines,
    size_t lineIndex) {
    if (lineIndex >= lines.size()) {
        StartNarrationText(L"");
        return;
    }
    StartNarrationText(lines[lineIndex]);
}

void ClearHoverNarrationInterruption() {
    gPausedNarrationText.clear();
    gPausedNarrationVisibleLength = 0;
    gPausedNarrationWasActive = false;
    gPausedNarrationWasTyping = false;
    gIsHoverNarrationActive = false;
    gWasNarrationRestoredFromHover = false;
}

void StartHoverNarrationText(const std::wstring& text) {
    if (!gIsHoverNarrationActive) {
        gPausedNarrationText = gCurrentNarrationText;
        gPausedNarrationVisibleLength = gNarrationVisibleLength;
        gPausedNarrationWasActive = gIsNarrationActive;
        gPausedNarrationWasTyping = gIsNarrationTyping;
    }
    gIsHoverNarrationActive = true;
    StartNarrationText(text);
}

void ResumeInterruptedNarration() {
    if (!gIsHoverNarrationActive) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    gCurrentNarrationText = gPausedNarrationText;
    gIsNarrationActive = gPausedNarrationWasActive;
    gNarrationVisibleLength = (std::min)(
        gPausedNarrationVisibleLength,
        gCurrentNarrationText.size());
    gIsNarrationTyping = gPausedNarrationWasTyping
        && gNarrationVisibleLength < gCurrentNarrationText.size();
    if (gIsNarrationTyping) {
        const size_t completedCharacterIntervals =
            gNarrationVisibleLength > 0
                ? gNarrationVisibleLength - 1
                : 0;
        gNarrationStartTime = now
            - completedCharacterIntervals
                * kNarrationCharacterIntervalMilliseconds;
        gNarrationCompletedTime = 0;
    } else {
        gNarrationStartTime = now;
        gNarrationCompletedTime = gIsNarrationActive ? now : 0;
    }
    ClearHoverNarrationInterruption();
    gWasNarrationRestoredFromHover = true;
    gNarrationParticles.clear();
    gNarrationParticleProcessedLength = gNarrationVisibleLength;
    gLastNarrationParticleUpdateTime = now;
    gIsNarrationOverflowAnimating = false;
}

void PrepareHoverDialogues() {
    DialogueTree* tree = CurrentDialogueTree();
    if (tree == nullptr) {
        return;
    }
    gLastNarrationHoverSocket = -1;
    RollHoverDialogueAvailability(*tree);
}

int SelectRandomDialogueCategoryIndex() {
    int normalCategory = -1;
    std::vector<int> otherCategories;
    for (size_t index = 0; index < gDialogueCategories.size(); ++index) {
        if (gDialogueCategories[index].id == L"normal") {
            normalCategory = static_cast<int>(index);
        } else {
            otherCategories.push_back(static_cast<int>(index));
        }
    }

    const unsigned int normalChance = gCurrentDay <= 2
        ? 50u
        : (gCurrentDay <= 4 ? 30u : 0u);
    if (normalCategory >= 0 && NextRandom() % 100 < normalChance) {
        return normalCategory;
    }
    if (!otherCategories.empty()) {
        return otherCategories[NextRandom() % otherCategories.size()];
    }
    return normalCategory;
}

void StartRandomDialogueTree() {
    if (gNarrationNames.empty() || gDialogueCategories.empty()) {
        return;
    }
    gCurrentNarrationName =
        gNarrationNames[NextRandom() % gNarrationNames.size()];
    gCurrentDialogueCategory = SelectRandomDialogueCategoryIndex();
    if (gCurrentDialogueCategory < 0) {
        return;
    }
    const DialogueCategory& category =
        gDialogueCategories[gCurrentDialogueCategory];
    gCurrentDialogueTree = static_cast<int>(
        NextRandom() % category.menus.size());
    gCurrentDialogueStage = DialogueStage::Entry;
    gCurrentDialogueLineIndex = 0;
    gLastNarrationHoverSocket = -1;
    ClearHoverNarrationInterruption();
    gHasCookingResult = false;
    gWasEmptySubmission = false;
    gAfterCookingNarrationCompletedTime = 0;
    DialogueTree* tree = CurrentDialogueTree();
    if (tree != nullptr) {
        StartRandomLine(tree->entryLines);
    }
    if (!gIsNarrationTyping) {
        gNpcOrderState = NpcOrderState::AfterOrder;
        gCookingEntryIndicatorStartTime = GetTickCount64();
    }
}

void CompleteNarrationTyping(ULONGLONG now) {
    gNarrationVisibleLength = gCurrentNarrationText.size();
    gIsNarrationTyping = false;
    gNarrationCompletedTime = now;
    gNarrationParticles.clear();
    gNarrationParticleProcessedLength = gNarrationVisibleLength;
    gIsNarrationOverflowAnimating = false;
    if (gNpcOrderState == NpcOrderState::Ordering
        && gCurrentDialogueStage == DialogueStage::Entry) {
        gNpcOrderState = NpcOrderState::AfterOrder;
        gCookingEntryIndicatorStartTime = now;
    }
    if (gCurrentDialogueStage == DialogueStage::AfterCooking
        && gAfterCookingNarrationCompletedTime == 0) {
        gAfterCookingNarrationCompletedTime = now;
    }
}

void AdvanceNarration() {
    if (!gIsNarrationBoxInteractive) {
        return;
    }
    if (!gIsNarrationActive) {
        return;
    }
    if (gIsHoverNarrationActive) {
        ResumeInterruptedNarration();
        return;
    }
    const bool advanceRestoredLine = gWasNarrationRestoredFromHover;
    if (gIsNarrationTyping) {
        CompleteNarrationTyping(GetTickCount64());
        if (!advanceRestoredLine) {
            return;
        }
    }
    gWasNarrationRestoredFromHover = false;

    DialogueTree* tree = CurrentDialogueTree();
    if (tree == nullptr) {
        return;
    }
    gNarrationCompletedTime = 0;
    if (gCurrentDialogueStage == DialogueStage::Entry) {
        gCurrentDialogueStage = DialogueStage::Additional;
        gCurrentDialogueLineIndex = 0;
        StartDialogueLine(tree->additionalLines, gCurrentDialogueLineIndex);
    } else if (gCurrentDialogueStage == DialogueStage::Additional
        && gCurrentDialogueLineIndex + 1 < tree->additionalLines.size()) {
        ++gCurrentDialogueLineIndex;
        StartDialogueLine(tree->additionalLines, gCurrentDialogueLineIndex);
    }
}

bool IsSocketNearMaterial(int socketIndex, int materialIndex) {
    if (socketIndex < 0 || materialIndex < 0) {
        return false;
    }
    const int socketColumn = socketIndex % kMaterialBinColumns;
    const int socketRow = socketIndex / kMaterialBinColumns;
    const int materialColumn = materialIndex % kMaterialBinColumns;
    const int materialRow = materialIndex / kMaterialBinColumns;
    return std::abs(socketColumn - materialColumn) <= 1
        && std::abs(socketRow - materialRow) <= 1;
}

void TryStartHoverNarration(int hoveredSocket) {
    if (!IsCookingStateActive()
        || hoveredSocket < 0
        || hoveredSocket == gLastNarrationHoverSocket) {
        return;
    }
    gLastNarrationHoverSocket = hoveredSocket;
    DialogueTree* tree = CurrentDialogueTree();
    if (tree == nullptr) {
        return;
    }

    const bool isExactSocket = std::any_of(
        tree->hoverDialogues.begin(),
        tree->hoverDialogues.end(),
        [hoveredSocket](const DialogueTree::HoverDialogue& hover) {
            return hoveredSocket == hover.materialIndex;
        });
    if (!isExactSocket) {
        const auto nearby = std::find_if(
            tree->nearbySlotDialogues.begin(),
            tree->nearbySlotDialogues.end(),
            [hoveredSocket](const DialogueTree::NearbySlotDialogue& slot) {
                return slot.socketIndex == hoveredSocket;
            });
        if (nearby != tree->nearbySlotDialogues.end()) {
            StartHoverNarrationText(nearby->line);
        }
        return;
    }

    std::vector<const std::wstring*> exactCandidates;
    for (const DialogueTree::HoverDialogue& hover : tree->hoverDialogues) {
        const bool exact = hoveredSocket == hover.materialIndex;
        if (!exact) {
            continue;
        }
        const std::vector<std::wstring>& lines = hover.exactLines;
        const std::vector<bool>& enabled = hover.exactEnabled;
        for (size_t index = 0;
             index < lines.size() && index < enabled.size();
             ++index) {
            if (enabled[index] && !lines[index].empty()) {
                exactCandidates.push_back(&lines[index]);
            }
        }
    }
    if (!exactCandidates.empty()) {
        StartHoverNarrationText(
            *exactCandidates[NextRandom() % exactCandidates.size()]);
    }
}

struct CookingEvaluation {
    bool exactRecipe = false;
    bool sequenceCorrect = true;
    int materialCountDifference = 0;
    int materialTypeDifference = 0;
    int errorCount = 0;
    long long reward = 0;
};

constexpr long long CalculateCookingReward(
    int baseReward,
    int errorCount) {
    if (errorCount >= 3) {
        return 0;
    }
    const int multiplierTenths = errorCount == 0
        ? 10
        : (errorCount == 1 ? 7 : 5);
    return static_cast<long long>(baseReward) * multiplierTenths / 10;
}

static_assert(CalculateCookingReward(200, 0) == 200);
static_assert(CalculateCookingReward(200, 1) == 140);
static_assert(CalculateCookingReward(200, 2) == 100);
static_assert(CalculateCookingReward(200, 3) == 0);

constexpr long long CalculateSequenceErrorReward(int baseReward) {
    return static_cast<long long>(baseReward) / 10;
}

static_assert(CalculateSequenceErrorReward(200) == 20);

int RewardErrorCount(int actualErrorCount) {
    return (std::max)(
        0,
        actualErrorCount - (gIsTrophyPurchased ? 1 : 0));
}

constexpr long long CalculateNextDailyRevenueGoal(
    long long currentGoal,
    bool goalMet) {
    const int additionTenths = goalMet ? 8 : 9;
    const long long nextGoal = currentGoal
        + currentGoal * additionTenths / 10;
    return (std::max)(kInitialDailyRevenueGoal, nextGoal);
}

static_assert(CalculateNextDailyRevenueGoal(1000, true) == 1800);
static_assert(CalculateNextDailyRevenueGoal(1800, true) == 3240);
static_assert(CalculateNextDailyRevenueGoal(1000, false) == 1900);

int CurrentOrderBaseReward() {
    if (gCurrentDialogueCategory < 0
        || gCurrentDialogueCategory
            >= static_cast<int>(gDialogueCategories.size())) {
        return 0;
    }
    const std::wstring& categoryId =
        gDialogueCategories[gCurrentDialogueCategory].id;
    for (const CustomerCategoryReward& categoryReward
         : kCustomerCategoryRewards) {
        if (categoryId == categoryReward.id) {
            return categoryReward.baseReward;
        }
    }
    return 0;
}

CookingEvaluation EvaluateCurrentRecipe() {
    CookingEvaluation evaluation;
    DialogueTree* tree = CurrentDialogueTree();
    if (tree == nullptr || tree->recipe.empty()) {
        return evaluation;
    }
    if (gMaterialCloneCount == 0) {
        return evaluation;
    }

    const bool requiresExactSequence =
        gDialogueCategories[gCurrentDialogueCategory].id
            == L"sequence_obsessed";
    if (requiresExactSequence) {
        std::vector<int> expectedMaterialOrder;
        for (const DialogueTree::RecipeIngredient& ingredient : tree->recipe) {
            expectedMaterialOrder.push_back(ingredient.materialIndex);
        }

        std::vector<int> actualMaterialOrder;
        for (int index = 0; index < gMaterialCloneCount; ++index) {
            const int materialIndex = gMaterialClones[index].materialIndex;
            if (actualMaterialOrder.empty()
                || actualMaterialOrder.back() != materialIndex) {
                actualMaterialOrder.push_back(materialIndex);
            }
        }

        evaluation.sequenceCorrect =
            actualMaterialOrder == expectedMaterialOrder;
        if (!evaluation.sequenceCorrect) {
            evaluation.errorCount = 1;
            evaluation.reward = CalculateSequenceErrorReward(
                CurrentOrderBaseReward());
            return evaluation;
        }
    }

    int actualCounts[kMaterialBinCount]{};
    int expectedCounts[kMaterialBinCount]{};
    for (int index = 0; index < gMaterialCloneCount; ++index) {
        const int materialIndex = gMaterialClones[index].materialIndex;
        if (materialIndex >= 0 && materialIndex < kMaterialBinCount) {
            ++actualCounts[materialIndex];
        }
    }
    for (const DialogueTree::RecipeIngredient& ingredient : tree->recipe) {
        expectedCounts[ingredient.materialIndex] += ingredient.quantity;
    }
    evaluation.exactRecipe = true;
    for (int index = 0; index < kMaterialBinCount; ++index) {
        if (actualCounts[index] != expectedCounts[index]) {
            evaluation.exactRecipe = false;
        }
        if ((actualCounts[index] > 0) != (expectedCounts[index] > 0)) {
            ++evaluation.materialTypeDifference;
        }
        evaluation.materialCountDifference += std::abs(
            actualCounts[index] - expectedCounts[index]);
    }

    evaluation.errorCount = evaluation.materialCountDifference;
    evaluation.reward = CalculateCookingReward(
        CurrentOrderBaseReward(),
        RewardErrorCount(evaluation.errorCount));
    return evaluation;
}

void FinishCookingOrder() {
    DialogueTree* tree = CurrentDialogueTree();
    if (tree == nullptr || gHasCookingResult) {
        return;
    }
    const CookingEvaluation evaluation = EvaluateCurrentRecipe();
    gWasEmptySubmission = gMaterialCloneCount == 0;
    gShouldEndDayAfterOrder = CurrentDayElapsedSeconds()
        >= kDayDurationSeconds;
    gLastCookingSucceeded = evaluation.exactRecipe;
    gEarnedMoney += evaluation.reward;
    gDayRevenue += evaluation.reward;
    if (gCurrentDialogueTree >= 0 && gCurrentDialogueTree < 3) {
        gDayMenuRevenue[gCurrentDialogueTree] += evaluation.reward;
    }
    gCookingMistakeCount += evaluation.errorCount;
    gHasCookingResult = true;
    gCurrentDialogueStage = DialogueStage::AfterCooking;
    gNpcOrderState = NpcOrderState::AfterOrder;
    ClearHoverNarrationInterruption();
    const std::vector<std::wstring>* reactionLines = &tree->afterCookingLines;
    const int rewardErrorCount = RewardErrorCount(evaluation.errorCount);
    if (gWasEmptySubmission || !evaluation.sequenceCorrect) {
        PlaySoundEffect(SoundEffect::CookingErrorTwoPlus);
    } else if (rewardErrorCount == 0) {
        PlaySoundEffect(SoundEffect::CookingSuccess);
    } else if (rewardErrorCount == 1) {
        PlaySoundEffect(SoundEffect::CookingErrorOne);
    } else {
        PlaySoundEffect(SoundEffect::CookingErrorTwoPlus);
    }
    if (evaluation.sequenceCorrect && rewardErrorCount >= 3) {
        gOwnedMoney = (std::max)(0LL, gOwnedMoney - 50);
        SavePlayerData();
    }
    if (gWasEmptySubmission) {
        reactionLines = &tree->emptySubmissionLines;
    } else if (!evaluation.sequenceCorrect) {
        reactionLines = &tree->afterCooking0PercentLines;
    } else if (rewardErrorCount >= 3) {
        reactionLines = &tree->afterCooking0PercentLines;
    } else if (rewardErrorCount == 2) {
        reactionLines = &tree->afterCooking50PercentLines;
    } else if (rewardErrorCount == 1) {
        reactionLines = &tree->afterCooking70PercentLines;
    }
    StartRandomLine(*reactionLines);
    gAfterCookingNarrationCompletedTime = gIsNarrationTyping
        ? 0
        : GetTickCount64();
}

bool CanStartNpcExit(ULONGLONG now) {
    return gScreenState == ScreenState::Game
        && gHasCookingResult
        && !gWasEmptySubmission
        && gCurrentDialogueStage == DialogueStage::AfterCooking
        && !gIsNarrationTyping
        && gAfterCookingNarrationCompletedTime != 0
        && !gIsCompletionPresentationActive
        && !gIsCookingTransitionRunning
        && gCookingState == CookingState::NonCooking
        && (now - gAfterCookingNarrationCompletedTime) / 1000.0
            >= kNpcAfterCookingDepartureDelaySeconds;
}

bool CanStartEmptySubmissionReaction() {
    return gScreenState == ScreenState::Game
        && gHasCookingResult
        && gWasEmptySubmission
        && gCurrentDialogueStage == DialogueStage::AfterCooking
        && !gIsNarrationTyping
        && gAfterCookingNarrationCompletedTime != 0
        && !gIsCookingTransitionRunning
        && gCookingState == CookingState::NonCooking;
}

void StartEmptySubmissionReaction(ULONGLONG now) {
    gScreenState = ScreenState::EmptySubmissionReacting;
    gPreparationSequenceStartTime = now;
    gNpcIdleStartTime = 0;
    gIsNarrationBoxInteractive = false;
}

void StartNpcExit(ULONGLONG now) {
    gScreenState = ScreenState::NpcExiting;
    gPreparationSequenceStartTime = now;
    gNpcIdleStartTime = 0;
    gIsNarrationBoxInteractive = false;
    gCookingEntryIndicatorStartTime = 0;
    gIsTableHovered = false;
    gHoveredPngSocket = -1;
}

void StartNextNpcEntrance(ULONGLONG now) {
    gScreenState = ScreenState::NpcEntering;
    gPreparationSequenceStartTime = now;
    RandomizeNpcAppearance();

    gNpcOrderState = NpcOrderState::BeforeOrder;
    gNpcIdleStartTime = 0;
    gNpcIdleStep = 0;
    gCookingEntryIndicatorStartTime = 0;
    gCookingState = CookingState::NonCooking;
    gCookingTransitionTargetState = CookingState::NonCooking;
    gIsCookingTransitionRunning = false;
    gTableLift = 0.0;
    gResetButtonOpacity = 0.0;
    gIsTableHovered = false;
    gHoveredPngSocket = -1;
    for (double& scale : gPngSocketScales) {
        scale = 1.0;
    }
    gMaterialCloneCount = 0;
    gStarParticles.clear();
    gLastStarParticleUpdateTime = now;

    gCurrentNarrationName.clear();
    gCurrentNarrationText.clear();
    gCurrentDialogueCategory = -1;
    gCurrentDialogueTree = -1;
    gCurrentDialogueStage = DialogueStage::Entry;
    gCurrentDialogueLineIndex = 0;
    gLastNarrationHoverSocket = -1;
    gIsNarrationActive = false;
    gIsNarrationTyping = false;
    gNarrationStartTime = 0;
    gNarrationVisibleLength = 0;
    gNarrationCompletedTime = 0;
    ClearHoverNarrationInterruption();
    gNarrationFadeStartTime = 0;
    gIsNarrationBoxInteractive = false;
    gAfterCookingNarrationCompletedTime = 0;
    gHasCookingResult = false;
    gLastCookingSucceeded = false;
    gWasEmptySubmission = false;
    gShouldEndDayAfterOrder = false;
}

void StartBusinessClosing(ULONGLONG now) {
    BeginBusinessMusicFadeOut(now);
    gCurrentDayGoalMet = gDayRevenue >= gDailyRevenueGoal;
    gDailyRevenueHistory.push_back(gDayRevenue);
    if (!gCurrentDayGoalMet) {
        ++gDailyGoalFailureCount;
    }
    gIsFinalSettlement = gDailyGoalFailureCount >= 2;
    gScreenState = ScreenState::BusinessClosing;
    gBusinessClosingStartTime = now;
    gIsNarrationActive = false;
    gIsNarrationTyping = false;
    gNarrationCompletedTime = 0;
    ClearHoverNarrationInterruption();
    gIsNarrationBoxInteractive = false;
    gNarrationFadeStartTime = 0;
    gMaterialCloneCount = 0;
    gStarParticles.clear();
    gCookingState = CookingState::NonCooking;
    gIsCookingTransitionRunning = false;
    gTableLift = 0.0;
}

void StartSettlement(ULONGLONG now) {
    gOwnedMoney += gDayRevenue
        * kRevenueConversionNumerator
        / kRevenueConversionDenominator;
    SavePlayerData();
    gScreenState = ScreenState::Settlement;
    gSettlementStartTime = now;
    PlaySoundEffect(SoundEffect::Receipt);
}

void StartNextDay(ULONGLONG now) {
    gDailyRevenueGoal = CalculateNextDailyRevenueGoal(
        gDailyRevenueGoal,
        gCurrentDayGoalMet);
    ++gCurrentDay;
    gDayRevenue = 0;
    for (long long& revenue : gDayMenuRevenue) {
        revenue = 0;
    }
    gDayStartTime = 0;
    StartNextNpcEntrance(now);
    gScreenState = ScreenState::Countdown;
    gPreparationSequenceStartTime = now;
}

RECT NarrationBoxRect(const Layout& layout) {
    return LogicalRect(
        layout,
        kNarrationBoxX,
        kNarrationBoxY,
        kNarrationBoxWidth,
        kNarrationBoxHeight);
}

void AddNarrationParticle(double x, double y) {
    if (gNarrationParticles.size() >= kMaximumNarrationParticles) {
        gNarrationParticles.erase(gNarrationParticles.begin());
    }
    gNarrationParticles.push_back({
        x,
        y,
        static_cast<double>(RandomRange(-18, 18)),
        -static_cast<double>(RandomRange(25, 55)),
        RandomRange(1, 2)
    });
}

void UpdateNarrationParticles(ULONGLONG now) {
    if (gLastNarrationParticleUpdateTime == 0) {
        gLastNarrationParticleUpdateTime = now;
    }
    const double elapsed = (std::min)(
        0.05,
        (now - gLastNarrationParticleUpdateTime) / 1000.0);
    gLastNarrationParticleUpdateTime = now;
    for (NarrationParticle& particle : gNarrationParticles) {
        particle.x += particle.velocityX * elapsed;
        particle.y += particle.velocityY * elapsed;
        particle.velocityY += kNarrationParticleGravity * elapsed;
    }
    gNarrationParticles.erase(
        std::remove_if(
            gNarrationParticles.begin(),
            gNarrationParticles.end(),
            [](const NarrationParticle& particle) {
                return particle.y > kDesignHeight + particle.size;
            }),
        gNarrationParticles.end());
}

void DrawNarrationParticles(HDC dc, const Layout& layout) {
    RECT clip{};
    if (GetClipBox(dc, &clip) == ERROR) {
        return;
    }

    for (const NarrationParticle& particle : gNarrationParticles) {
        const POINT topLeft = LogicalPoint(layout, particle.x, particle.y);
        const int size = (std::max)(
            1,
            static_cast<int>(std::lround(particle.size * layout.scale)));
        const RECT square{
            topLeft.x,
            topLeft.y,
            topLeft.x + size,
            topLeft.y + size
        };
        if (square.right <= clip.left
            || square.left >= clip.right
            || square.bottom <= clip.top
            || square.top >= clip.bottom) {
            continue;
        }
        FillSolid(dc, square, RGB(0xff, 0xff, 0xff));
    }
}

void DrawNarration(HDC dc, const Layout& layout) {
    if (!gIsNarrationActive) {
        return;
    }

    const int nameFontHeight = (std::max)(
        1, static_cast<int>(std::lround(18.0 * layout.scale)));
    HFONT nameFont = CreateFont(
        -nameFontHeight,
        0,
        0,
        0,
        FW_BOLD,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        kNarrationFontName);
    const bool ownsNameFont = nameFont != nullptr;
    if (nameFont == nullptr) {
        nameFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }
    HGDIOBJ oldFont = SelectObject(dc, nameFont);
    const int oldBackgroundMode = SetBkMode(dc, TRANSPARENT);
    const COLORREF oldTextColor = SetTextColor(dc, RGB(0xff, 0xff, 0xff));
    const POINT namePosition = LogicalPoint(
        layout,
        kNarrationBoxX + 24,
        kNarrationBoxY + 20);
    TextOut(
        dc,
        namePosition.x,
        namePosition.y,
        gCurrentNarrationName.c_str(),
        static_cast<int>(gCurrentNarrationName.size()));

    const int dialogueFontHeight = (std::max)(
        1, static_cast<int>(std::lround(22.0 * layout.scale)));
    HFONT dialogueFont = CreateFont(
        -dialogueFontHeight,
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        kNarrationFontName);
    const bool ownsDialogueFont = dialogueFont != nullptr;
    if (dialogueFont == nullptr) {
        dialogueFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }
    SelectObject(dc, dialogueFont);
    const POINT textPosition = LogicalPoint(
        layout,
        kNarrationBoxX + 24,
        kNarrationBoxY + 62);
    SIZE fullTextSize{};
    GetTextExtentPoint32(
        dc,
        gCurrentNarrationText.c_str(),
        static_cast<int>(gCurrentNarrationText.size()),
        &fullTextSize);
    const RECT narrationBox = NarrationBoxRect(layout);
    const int textRightEdge = narrationBox.right
        - static_cast<int>(std::lround(24.0 * layout.scale));
    const bool shouldAnimateOverflow = gIsNarrationTyping
        && textPosition.x + fullTextSize.cx > textRightEdge;
    gIsNarrationOverflowAnimating = shouldAnimateOverflow;

    if (!shouldAnimateOverflow) {
        gNarrationParticles.clear();
        gNarrationParticleProcessedLength = gNarrationVisibleLength;
        TextOut(
            dc,
            textPosition.x,
            textPosition.y,
            gCurrentNarrationText.c_str(),
            static_cast<int>(gNarrationVisibleLength));
    } else {
        RECT clip{};
        GetClipBox(dc, &clip);
        const ULONGLONG now = GetTickCount64();
        int characterX = textPosition.x;
        const size_t visibleLength = (std::min)(
            gNarrationVisibleLength,
            gCurrentNarrationText.size());
        const size_t firstNewCharacter = (std::min)(
            gNarrationParticleProcessedLength,
            visibleLength);
        for (size_t index = 0; index < visibleLength; ++index) {
            if (characterX
                > clip.right + static_cast<int>(std::ceil(layout.scale))) {
                break;
            }
            const wchar_t character = gCurrentNarrationText[index];
            SIZE characterSize{};
            GetTextExtentPoint32(dc, &character, 1, &characterSize);

            if (index >= firstNewCharacter) {
                const double particleX = (
                    characterX + characterSize.cx - layout.offsetX)
                    / layout.scale;
                const double particleY = kNarrationBoxY + 62.0 + 18.0;
                AddNarrationParticle(particleX, particleY);
            }

            unsigned int jitterSeed = static_cast<unsigned int>(
                now / 40 + index * 0x9e3779b9u);
            jitterSeed ^= jitterSeed >> 16;
            const int jitterX = static_cast<int>(jitterSeed % 3) - 1;
            jitterSeed = jitterSeed * 1664525u + 1013904223u;
            const int jitterY = static_cast<int>(jitterSeed % 3) - 1;
            const int drawX = characterX + static_cast<int>(std::lround(
                jitterX * layout.scale));
            const int drawY = textPosition.y + static_cast<int>(std::lround(
                jitterY * layout.scale));
            if (drawX + characterSize.cx > clip.left
                && drawX < clip.right
                && drawY + dialogueFontHeight > clip.top
                && drawY < clip.bottom) {
                TextOut(dc, drawX, drawY, &character, 1);
            }
            characterX += characterSize.cx;
        }
        gNarrationParticleProcessedLength = visibleLength;
        DrawNarrationParticles(dc, layout);
    }
    SetTextColor(dc, oldTextColor);
    SetBkMode(dc, oldBackgroundMode);
    SelectObject(dc, oldFont);
    if (ownsDialogueFont) {
        DeleteObject(dialogueFont);
    }
    if (ownsNameFont) {
        DeleteObject(nameFont);
    }
}

double SmoothStep(double progress) {
    const double clamped = (std::max)(0.0, (std::min)(1.0, progress));
    return clamped * clamped * (3.0 - 2.0 * clamped);
}

HFONT CreateUiFont(
    const Layout& layout,
    double logicalHeight,
    int weight = FW_NORMAL,
    const wchar_t* fontName = kDefaultUiFontName) {
    const int fontHeight = (std::max)(
        1,
        static_cast<int>(std::lround(logicalHeight * layout.scale)));
    HFONT font = CreateFont(
        -fontHeight,
        0,
        0,
        0,
        weight,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        fontName);
    return font != nullptr
        ? font
        : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
}

void DrawCenteredText(
    HDC dc,
    const Layout& layout,
    const RECT& logicalArea,
    const wchar_t* text,
    double fontHeight,
    COLORREF color,
    int weight,
    const wchar_t* fontName) {
    const HFONT font = CreateUiFont(layout, fontHeight, weight, fontName);
    const HGDIOBJ oldFont = SelectObject(dc, font);
    const int oldBackgroundMode = SetBkMode(dc, TRANSPARENT);
    const COLORREF oldTextColor = SetTextColor(dc, color);
    RECT screenArea = LogicalRect(
        layout,
        logicalArea.left,
        logicalArea.top,
        logicalArea.right - logicalArea.left,
        logicalArea.bottom - logicalArea.top);
    DrawText(
        dc,
        text,
        -1,
        &screenArea,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SetTextColor(dc, oldTextColor);
    SetBkMode(dc, oldBackgroundMode);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

template<typename DrawFunction>
void DrawWithOpacity(
    HDC dc,
    const RECT& area,
    BYTE opacity,
    DrawFunction drawFunction) {
    if (opacity == 0 || area.right <= area.left || area.bottom <= area.top) {
        return;
    }
    if (opacity == 255) {
        drawFunction(dc);
        return;
    }

    const int width = area.right - area.left;
    const int height = area.bottom - area.top;
    HDC layerDc = CreateCompatibleDC(dc);
    if (layerDc == nullptr) {
        drawFunction(dc);
        return;
    }
    HBITMAP layerBitmap = CreateCompatibleBitmap(dc, width, height);
    if (layerBitmap == nullptr) {
        DeleteDC(layerDc);
        drawFunction(dc);
        return;
    }
    HGDIOBJ oldBitmap = SelectObject(layerDc, layerBitmap);
    BitBlt(layerDc, 0, 0, width, height, dc, area.left, area.top, SRCCOPY);
    SetViewportOrgEx(layerDc, -area.left, -area.top, nullptr);
    drawFunction(layerDc);
    SetViewportOrgEx(layerDc, 0, 0, nullptr);

    const BLENDFUNCTION blend{AC_SRC_OVER, 0, opacity, 0};
    AlphaBlend(
        dc,
        area.left,
        area.top,
        width,
        height,
        layerDc,
        0,
        0,
        width,
        height,
        blend);

    SelectObject(layerDc, oldBitmap);
    DeleteObject(layerBitmap);
    DeleteDC(layerDc);
}

double NarrationBoxOpacity() {
    if (gNarrationFadeStartTime == 0) {
        return 0.0;
    }
    const double elapsed = (
        GetTickCount64() - gNarrationFadeStartTime) / 1000.0;
    return SmoothStep(elapsed / kNarrationFadeInSeconds);
}

void DrawNarrationOverlay(
    HDC dc,
    const RECT& client,
    const Layout& layout) {
    const double opacity = NarrationBoxOpacity();
    if (opacity <= 0.0) {
        return;
    }

    const RECT narrationBox = NarrationBoxRect(layout);
    DrawWithOpacity(
        dc,
        client,
        static_cast<BYTE>(std::lround(opacity * 255.0)),
        [&](HDC targetDc) {
            FillTranslucent(
                targetDc,
                narrationBox,
                RGB(0x2a, 0x2a, 0x2a),
                222);
            DrawNarration(targetDc, layout);
        });
}

RECT MoneyUiRect() {
    const int left = kPlayAreaX + kMoneyUiMargin;
    const int top = kPlayAreaY + kMoneyUiMargin;
    return {
        left,
        top,
        left + kMoneyCoinDisplaySize + kMoneyTextGap + kMoneyTextWidth,
        top + kMoneyCoinDisplaySize
    };
}

RECT ObjectiveUiRect() {
    const RECT moneyArea = MoneyUiRect();
    const int right = kPlayAreaX + kPlayAreaSize - kMoneyUiMargin;
    const int width = moneyArea.right - moneyArea.left;
    return {
        right - width,
        moneyArea.top,
        right,
        moneyArea.bottom
    };
}

RECT StartBusinessButtonRect() {
    const int right = kPlayAreaX + kPlayAreaSize - kMoneyUiMargin;
    const int top = kPlayAreaY + kMoneyUiMargin;
    return {
        right - kStartBusinessButtonWidth,
        top,
        right,
        top + kStartBusinessButtonHeight
    };
}

void DrawMoneyInterface(
    HDC dc,
    const Layout& layout,
    bool showOwnedMoney,
    BYTE opacity) {
    const RECT logicalArea = MoneyUiRect();
    const RECT area = LogicalRect(
        layout,
        logicalArea.left,
        logicalArea.top,
        logicalArea.right - logicalArea.left,
        logicalArea.bottom - logicalArea.top);
    DrawWithOpacity(dc, area, opacity, [&](HDC targetDc) {
        const RECT coinArea = LogicalRect(
            layout,
            logicalArea.left,
            logicalArea.top,
            kMoneyCoinDisplaySize,
            kMoneyCoinDisplaySize);
        Gdiplus::Image* coinImage = showOwnedMoney
            ? gOwnedMoneyCoinImage
            : gEarnedMoneyCoinImage;
        if (coinImage != nullptr) {
            Gdiplus::Graphics graphics(targetDc);
            graphics.SetInterpolationMode(
                Gdiplus::InterpolationModeNearestNeighbor);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
            graphics.DrawImage(
                coinImage,
                Gdiplus::Rect(
                    coinArea.left,
                    coinArea.top,
                    coinArea.right - coinArea.left,
                    coinArea.bottom - coinArea.top),
                0,
                0,
                kMoneyCoinSourceSize,
                kMoneyCoinSourceSize,
                Gdiplus::UnitPixel);
        } else {
            FillSolid(targetDc, coinArea, kCoinPlaceholderColor);
        }

        RECT textArea = LogicalRect(
            layout,
            logicalArea.left + kMoneyCoinDisplaySize + kMoneyTextGap,
            logicalArea.top,
            kMoneyTextWidth,
            kMoneyCoinDisplaySize);
        const std::wstring amount = std::to_wstring(
            showOwnedMoney ? gOwnedMoney : gEarnedMoney);
        const HFONT font = CreateUiFont(layout, kMoneyFontHeight, FW_BOLD);
        const HGDIOBJ oldFont = SelectObject(targetDc, font);
        const int oldBackgroundMode = SetBkMode(targetDc, TRANSPARENT);
        const COLORREF oldTextColor = SetTextColor(
            targetDc, RGB(0x2a, 0x2a, 0x2a));
        DrawText(
            targetDc,
            amount.c_str(),
            -1,
            &textArea,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SetTextColor(targetDc, oldTextColor);
        SetBkMode(targetDc, oldBackgroundMode);
        SelectObject(targetDc, oldFont);
        DeleteObject(font);
    });
}

void DrawObjectiveInterface(HDC dc, const Layout& layout) {
    const RECT logicalArea = ObjectiveUiRect();
    const RECT iconArea = LogicalRect(
        layout,
        logicalArea.right - kMoneyCoinDisplaySize,
        logicalArea.top,
        kMoneyCoinDisplaySize,
        kMoneyCoinDisplaySize);
    if (gObjectiveImage != nullptr) {
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        graphics.DrawImage(
            gObjectiveImage,
            Gdiplus::Rect(
                iconArea.left,
                iconArea.top,
                iconArea.right - iconArea.left,
                iconArea.bottom - iconArea.top),
            0,
            0,
            kObjectiveImageSourceSize,
            kObjectiveImageSourceSize,
            Gdiplus::UnitPixel);
    } else {
        FillSolid(dc, iconArea, kCoinPlaceholderColor);
    }

    RECT textArea = LogicalRect(
        layout,
        logicalArea.left,
        logicalArea.top,
        kMoneyTextWidth,
        kMoneyCoinDisplaySize);
    const std::wstring amount = std::to_wstring(gDailyRevenueGoal);
    const HFONT font = CreateUiFont(layout, kMoneyFontHeight, FW_BOLD);
    const HGDIOBJ oldFont = SelectObject(dc, font);
    const int oldBackgroundMode = SetBkMode(dc, TRANSPARENT);
    const COLORREF oldTextColor = SetTextColor(dc, RGB(0x2a, 0x2a, 0x2a));
    DrawText(
        dc,
        amount.c_str(),
        -1,
        &textArea,
        DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SetTextColor(dc, oldTextColor);
    SetBkMode(dc, oldBackgroundMode);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

bool IsOwnedMoneyInterfaceVisible() {
    return gScreenState == ScreenState::PreparationFadingIn
        || gScreenState == ScreenState::Preparation;
}

bool IsEarnedMoneyInterfaceVisible() {
    return gScreenState == ScreenState::Countdown
        || gScreenState == ScreenState::NpcEntering
        || gScreenState == ScreenState::NarrationStarting
        || gScreenState == ScreenState::Game
        || gScreenState == ScreenState::EmptySubmissionReacting
        || gScreenState == ScreenState::NpcExiting;
}

bool IsObjectiveInterfaceVisible() {
    return IsEarnedMoneyInterfaceVisible();
}

void DrawMoneyTooltip(HDC dc, const Layout& layout) {
    if (gHoveredMoneyTooltip == MoneyTooltipKind::None) {
        return;
    }
    const wchar_t* title = L"";
    const wchar_t* description = L"";
    switch (gHoveredMoneyTooltip) {
    case MoneyTooltipKind::Owned:
        title = L"가진 돈";
        description = L"- 벌어들인돈의 일부가 전환된다.";
        break;
    case MoneyTooltipKind::Earned:
        title = L"벌어들인 돈";
        description = L"- 매 판마다 벌어들인돈이 표시된다.";
        break;
    case MoneyTooltipKind::Objective:
        title = L"목표 금액";
        description = L"- 매일 달성해야 할 금액.";
        break;
    default:
        return;
    }

    const auto measureText = [&](const wchar_t* text, double height) {
        const HFONT font = CreateUiFont(layout, height, FW_NORMAL);
        const HGDIOBJ oldFont = SelectObject(dc, font);
        SIZE size{};
        GetTextExtentPoint32W(
            dc, text, static_cast<int>(wcslen(text)), &size);
        SelectObject(dc, oldFont);
        DeleteObject(font);
        return SIZE{
            static_cast<LONG>(std::ceil(size.cx / layout.scale)),
            static_cast<LONG>(std::ceil(size.cy / layout.scale))};
    };
    const SIZE titleSize = measureText(title, 15.0);
    const SIZE descriptionSize = measureText(description, 12.0);
    constexpr int lineGap = 2;
    const int tooltipWidth = (std::max)(
        static_cast<int>(titleSize.cx),
        static_cast<int>(descriptionSize.cx))
        + kMoneyTooltipPadding * 2;
    const int tooltipHeight = titleSize.cy + descriptionSize.cy
        + lineGap + kMoneyTooltipPadding * 2;
    const RECT anchor = gHoveredMoneyTooltip == MoneyTooltipKind::Objective
        ? ObjectiveUiRect()
        : MoneyUiRect();
    int tooltipX = gHoveredMoneyTooltip == MoneyTooltipKind::Objective
        ? anchor.right - tooltipWidth
        : anchor.left;
    int tooltipY = anchor.bottom + kMoneyTooltipGap;
    tooltipX = std::clamp(tooltipX, 0, kDesignWidth - tooltipWidth);
    tooltipY = std::clamp(tooltipY, 0, kDesignHeight - tooltipHeight);
    const RECT tooltipArea{
        tooltipX,
        tooltipY,
        tooltipX + tooltipWidth,
        tooltipY + tooltipHeight};
    FillSolid(
        dc,
        LogicalRect(
            layout, tooltipX, tooltipY, tooltipWidth, tooltipHeight),
        RGB(0x2a, 0x2a, 0x2a));
    DrawCenteredText(
        dc,
        layout,
        {tooltipArea.left + kMoneyTooltipPadding,
         tooltipArea.top + kMoneyTooltipPadding,
         tooltipArea.right - kMoneyTooltipPadding,
         tooltipArea.top + kMoneyTooltipPadding + titleSize.cy},
        title,
        15.0,
        RGB(0xff, 0xff, 0xff));
    DrawCenteredText(
        dc,
        layout,
        {tooltipArea.left + kMoneyTooltipPadding,
         tooltipArea.top + kMoneyTooltipPadding + titleSize.cy + lineGap,
         tooltipArea.right - kMoneyTooltipPadding,
         tooltipArea.bottom - kMoneyTooltipPadding},
        description,
        12.0,
        RGB(0xff, 0xff, 0xff));
}

void DrawStartBusinessButton(HDC dc, const Layout& layout, BYTE opacity) {
    const RECT logicalButton = StartBusinessButtonRect();
    const RECT button = LogicalRect(
        layout,
        logicalButton.left,
        logicalButton.top,
        kStartBusinessButtonWidth,
        kStartBusinessButtonHeight);
    const BYTE effectiveOpacity = static_cast<BYTE>(std::lround(
        opacity * kStartBusinessButtonOpacity));
    DrawWithOpacity(dc, button, effectiveOpacity, [&](HDC targetDc) {
        FillSolid(targetDc, button, kStartBusinessButtonColor);
        DrawCenteredText(
            targetDc,
            layout,
            logicalButton,
            L"장사 시작",
            20.0,
            RGB(0x18, 0x2a, 0x18),
            FW_BOLD);
    });
}

void DrawCountdown(HDC dc, const Layout& layout) {
    const double elapsed = (
        GetTickCount64() - gPreparationSequenceStartTime) / 1000.0;
    const int step = (std::min)(
        kCountdownStepCount - 1,
        static_cast<int>(elapsed / kCountdownStepSeconds));
    constexpr wchar_t labels[kCountdownStepCount][8] = {
        L"3", L"2", L"1", L"시작!"
    };
    const double stepElapsed = std::fmod(elapsed, kCountdownStepSeconds);
    const double stepProgress = stepElapsed / kCountdownStepSeconds;
    constexpr double pi = 3.14159265358979323846;
    const double pulse = std::sin(stepProgress * pi);
    const double fontHeight = 64.0
        * (1.0 + (kCountdownPulseScale - 1.0) * pulse);
    const RECT countdownArea{
        kPlayAreaX,
        kPlayAreaY + kPlayAreaSize / 2 - 60,
        kPlayAreaX + kPlayAreaSize,
        kPlayAreaY + kPlayAreaSize / 2 + 60
    };
    DrawCenteredText(
        dc,
        layout,
        countdownArea,
        labels[step],
        fontHeight,
        kLetterboxColor,
        FW_BOLD);
}

void DrawPreparationSequenceUi(HDC dc, const Layout& layout) {
    if (gScreenState == ScreenState::PreparationFadingIn
        || gScreenState == ScreenState::Preparation) {
        DrawMoneyInterface(dc, layout, true, 255);
        DrawStartBusinessButton(dc, layout, 255);
    } else if (gScreenState == ScreenState::Countdown) {
        DrawMoneyInterface(dc, layout, false, 255);
        DrawObjectiveInterface(dc, layout);
        DrawCountdown(dc, layout);
        return;
    } else if (gScreenState == ScreenState::NpcEntering
        || gScreenState == ScreenState::NpcExiting) {
        DrawMoneyInterface(dc, layout, false, 255);
    } else if (gScreenState == ScreenState::NarrationStarting) {
        DrawMoneyInterface(dc, layout, false, 255);
    } else if (gScreenState == ScreenState::Game
        || gScreenState == ScreenState::EmptySubmissionReacting) {
        DrawMoneyInterface(dc, layout, false, 255);
    }

    if (gScreenState == ScreenState::Countdown
        || gScreenState == ScreenState::NpcEntering
        || gScreenState == ScreenState::NarrationStarting
        || gScreenState == ScreenState::Game
        || gScreenState == ScreenState::EmptySubmissionReacting
        || gScreenState == ScreenState::NpcExiting) {
        DrawObjectiveInterface(dc, layout);
    }
}

RECT ReceiptTargetRect() {
    const int width = gIsFinalSettlement
        ? kFinalReceiptWidth
        : kReceiptWidth;
    const int left = (kDesignWidth - width) / 2;
    return {
        left,
        kReceiptTargetY,
        left + width,
        kReceiptTargetY + kReceiptHeight
    };
}

RECT NextDayButtonRect() {
    const RECT receipt = ReceiptTargetRect();
    const int left = (kDesignWidth - kNextDayButtonWidth) / 2;
    const int top = receipt.bottom + kNextDayButtonGap;
    return {
        left,
        top,
        left + kNextDayButtonWidth,
        top + kNextDayButtonHeight
    };
}

std::wstring FormatMoney(long long value) {
    std::wstring digits = std::to_wstring(value);
    for (int position = static_cast<int>(digits.size()) - 3;
         position > 0;
         position -= 3) {
        digits.insert(static_cast<size_t>(position), 1, L',');
    }
    return digits;
}

void DrawBusinessClosing(HDC dc, const Layout& layout) {
    constexpr size_t textLength =
        (sizeof(kBusinessClosingText) / sizeof(kBusinessClosingText[0])) - 1;
    const double elapsed = (
        GetTickCount64() - gBusinessClosingStartTime) / 1000.0;
    const size_t visibleLength = (std::min)(
        textLength,
        static_cast<size_t>(elapsed / kBusinessClosingCharacterSeconds + 1));
    const double typingSeconds = textLength * kBusinessClosingCharacterSeconds;
    double opacity = 1.0;
    if (elapsed > typingSeconds + kBusinessClosingHoldSeconds) {
        opacity = 1.0 - SmoothStep(
            (elapsed - typingSeconds - kBusinessClosingHoldSeconds)
                / kBusinessClosingFadeSeconds);
    }
    const std::wstring visibleText(kBusinessClosingText, visibleLength);
    const RECT area{
        kPlayAreaX,
        kPlayAreaY + kPlayAreaSize / 2 - 60,
        kPlayAreaX + kPlayAreaSize,
        kPlayAreaY + kPlayAreaSize / 2 + 60
    };
    const RECT screenArea = LogicalRect(
        layout, area.left, area.top,
        area.right - area.left, area.bottom - area.top);
    DrawWithOpacity(
        dc,
        screenArea,
        static_cast<BYTE>(std::lround((std::max)(0.0, opacity) * 255.0)),
        [&](HDC targetDc) {
            DrawCenteredText(
                targetDc, layout, area, visibleText.c_str(), 54.0,
                kLetterboxColor, FW_BOLD);
        });
}

void DrawSettlement(HDC dc, const Layout& layout) {
    const double elapsed = (
        GetTickCount64() - gSettlementStartTime) / 1000.0;
    const double slideProgress = SmoothStep(elapsed / kReceiptSlideSeconds);
    const RECT targetReceipt = ReceiptTargetRect();
    const int receiptX = targetReceipt.left;
    const int receiptWidth = targetReceipt.right - targetReceipt.left;
    const int receiptY = static_cast<int>(std::lround(
        kDesignHeight
            + (targetReceipt.top - kDesignHeight) * slideProgress));
    const RECT receipt = LogicalRect(
        layout, receiptX, receiptY, receiptWidth, kReceiptHeight);
    FillSolid(dc, receipt, kReceiptColor);

    auto drawReceiptLine = [&](
        const std::wstring& value,
        int x,
        int y,
        int width,
        UINT alignment,
        int fontHeight = 24,
        int areaHeight = 32) {
        const HFONT font = CreateUiFont(layout, fontHeight, FW_BOLD);
        const HGDIOBJ oldFont = SelectObject(dc, font);
        const int oldMode = SetBkMode(dc, TRANSPARENT);
        const COLORREF oldColor = SetTextColor(dc, kReceiptTextColor);
        RECT lineArea = LogicalRect(layout, x, y, width, areaHeight);
        DrawText(
            dc, value.c_str(), -1, &lineArea,
            alignment | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SetTextColor(dc, oldColor);
        SetBkMode(dc, oldMode);
        SelectObject(dc, oldFont);
        DeleteObject(font);
    };

    if (gIsFinalSettlement) {
        drawReceiptLine(
            L"최종 영수증",
            receiptX + 24,
            receiptY + 35,
            receiptWidth - 48,
            DT_CENTER,
            30);

        const int dayCount = static_cast<int>(gDailyRevenueHistory.size());
        const int columnCount = dayCount > 10 ? 2 : 1;
        const int rowsPerColumn = (std::max)(
            1,
            (dayCount + columnCount - 1) / columnCount);
        const int availableHeight = kReceiptHeight - 180;
        const int lineHeight = (std::max)(
            14,
            (std::min)(28, availableHeight / rowsPerColumn));
        const int lineFontHeight = (std::max)(10, lineHeight - 5);
        const int contentWidth = receiptWidth - 48;
        const int columnWidth = contentWidth / columnCount;
        for (int index = 0; index < dayCount; ++index) {
            const int column = index / rowsPerColumn;
            const int row = index % rowsPerColumn;
            drawReceiptLine(
                L"Day" + std::to_wstring(index + 1)
                    + L"   " + FormatMoney(gDailyRevenueHistory[index]),
                receiptX + 24 + column * columnWidth,
                receiptY + 85 + row * lineHeight,
                columnWidth - 8,
                DT_LEFT,
                lineFontHeight,
                lineHeight);
        }

        FillSolid(
            dc,
            LogicalRect(
                layout,
                receiptX + 24,
                receiptY + kReceiptHeight - 80,
                receiptWidth - 48,
                2),
            kReceiptTextColor);
        drawReceiptLine(
            L"총 매출액",
            receiptX + 24,
            receiptY + kReceiptHeight - 58,
            180,
            DT_LEFT,
            26);
        drawReceiptLine(
            FormatMoney(gEarnedMoney),
            receiptX + 204,
            receiptY + kReceiptHeight - 58,
            receiptWidth - 228,
            DT_RIGHT,
            26);
    } else {
        drawReceiptLine(
            L"DAY " + std::to_wstring(gCurrentDay) + L" 매출",
            receiptX + 24,
            receiptY + 45,
            receiptWidth - 48,
            DT_LEFT,
            28);
        constexpr wchar_t menuLabels[3][16] = {
            L"기본랩", L"에그마요랩", L"김밥"
        };
        int lineY = receiptY + 105;
        for (int index = 0; index < 3; ++index) {
            if (gDayMenuRevenue[index] == 0) {
                continue;
            }
            drawReceiptLine(
                menuLabels[index], receiptX + 24, lineY, 190, DT_LEFT);
            drawReceiptLine(
                FormatMoney(gDayMenuRevenue[index]),
                receiptX + 210,
                lineY,
                receiptWidth - 234,
                DT_RIGHT);
            lineY += 42;
        }
        FillSolid(
            dc,
            LogicalRect(
                layout,
                receiptX + 24,
                receiptY + kReceiptHeight - 95,
                receiptWidth - 48,
                2),
            kReceiptTextColor);
        drawReceiptLine(
            L"총 매출액",
            receiptX + 24,
            receiptY + kReceiptHeight - 70,
            160,
            DT_LEFT,
            26);
        drawReceiptLine(
            FormatMoney(gDayRevenue),
            receiptX + 180,
            receiptY + kReceiptHeight - 70,
            receiptWidth - 204,
            DT_RIGHT,
            26);
    }

    const double buttonElapsed = elapsed
        - kReceiptSlideSeconds - kSettlementButtonDelaySeconds;
    if (buttonElapsed >= 0.0) {
        const double opacity = SmoothStep(
            buttonElapsed / kSettlementButtonFadeSeconds);
        const RECT logicalButton = NextDayButtonRect();
        const RECT button = LogicalRect(
            layout,
            logicalButton.left,
            logicalButton.top,
            logicalButton.right - logicalButton.left,
            logicalButton.bottom - logicalButton.top);
        DrawWithOpacity(
            dc,
            button,
            static_cast<BYTE>(std::lround(opacity * 255.0)),
            [&](HDC targetDc) {
                FillRoundedRect(targetDc, button, 8, kResetButtonColor);
                DrawCenteredText(
                    targetDc,
                    layout,
                    logicalButton,
                    gIsFinalSettlement ? L"메인화면" : L"다음날로",
                    20.0, RGB(0x18, 0x2a, 0x18), FW_BOLD);
            });
    }
}

bool IsTitleInteractive();
int HitTestTitleButton(int designX, int designY);
RECT ExitDialogButtonRect(bool yesButton);
RECT ResetButtonRect(const Layout& layout);
bool CanEnterCookingMode();
bool IsCookingStateActive();

bool IsClickableAt(HWND window, int mouseX, int mouseY) {
    RECT client{};
    GetClientRect(window, &client);
    const Layout layout = GetLayout(client);
    const int designX = static_cast<int>(std::lround(
        (mouseX - layout.offsetX) / layout.scale));
    const int designY = static_cast<int>(std::lround(
        (mouseY - layout.offsetY) / layout.scale));
    const POINT logicalPoint{designX, designY};
    const POINT screenPoint{mouseX, mouseY};

    if (gScreenState == ScreenState::Title && IsTitleInteractive()) {
        if (gIsExitDialogVisible) {
            const RECT yesButton = ExitDialogButtonRect(true);
            const RECT noButton = ExitDialogButtonRect(false);
            return PtInRect(&yesButton, logicalPoint)
                || PtInRect(&noButton, logicalPoint);
        }
        return HitTestTitleButton(designX, designY) >= 0;
    }
    if (gScreenState == ScreenState::Tutorial) {
        return true;
    }
    if (gScreenState == ScreenState::Preparation) {
        const RECT button = StartBusinessButtonRect();
        return PtInRect(&button, logicalPoint) != FALSE;
    }
    if (gScreenState == ScreenState::Settlement) {
        const double elapsed = (
            GetTickCount64() - gSettlementStartTime) / 1000.0;
        const RECT button = NextDayButtonRect();
        return elapsed >= kReceiptSlideSeconds + kSettlementButtonDelaySeconds
            && PtInRect(&button, logicalPoint);
    }
    if (gScreenState != ScreenState::Game) {
        return false;
    }
    const RECT narrationBox = NarrationBoxRect(layout);
    if (gIsNarrationBoxInteractive
        && PtInRect(&narrationBox, screenPoint)) {
        return true;
    }
    const RECT resetButton = ResetButtonRect(layout);
    if (IsCookingStateActive() && PtInRect(&resetButton, screenPoint)) {
        return true;
    }
    if (CanEnterCookingMode()) {
        const int playX = designX - kPlayAreaX;
        const int playY = designY - kPlayAreaY;
        return playX >= 0 && playX <= kPlayAreaSize
            && playY >= kTableHoverTop && playY <= kTableHoverBottom;
    }
    if (IsCookingStateActive()) {
        return HitTestMaterialClone(designX, designY) >= 0
            || HitTestPngSocket() >= 0;
    }
    return false;
}

void UpdateMouseCursor(HWND window, int mouseX, int mouseY) {
    SetCursor(LoadCursor(
        nullptr,
        IsClickableAt(window, mouseX, mouseY) ? IDC_HAND : IDC_ARROW));
}

RECT ExitDialogRect() {
    return {
        (kDesignWidth - kExitDialogWidth) / 2,
        (kDesignHeight - kExitDialogHeight) / 2,
        (kDesignWidth + kExitDialogWidth) / 2,
        (kDesignHeight + kExitDialogHeight) / 2
    };
}

RECT ExitDialogButtonRect(bool isYesButton) {
    constexpr int gap = 30;
    const int totalWidth = kExitDialogButtonWidth * 2 + gap;
    const int left = (kDesignWidth - totalWidth) / 2
        + (isYesButton ? 0 : kExitDialogButtonWidth + gap);
    const RECT dialog = ExitDialogRect();
    const int top = dialog.bottom - 62;
    return {
        left,
        top,
        left + kExitDialogButtonWidth,
        top + kExitDialogButtonHeight
    };
}

double TitleContentOpacity() {
    if (gScreenState == ScreenState::TitleFadingOut) {
        const double elapsed = (
            GetTickCount64() - gScreenTransitionStartTime) / 1000.0;
        return 1.0 - SmoothStep(elapsed / kScreenFadeSeconds);
    }

    const double elapsed = (GetTickCount64() - gTitleStartTime) / 1000.0;
    if (elapsed <= kTitleAppearanceDelaySeconds) {
        return 0.0;
    }
    return SmoothStep(
        (elapsed - kTitleAppearanceDelaySeconds) / kTitleFadeInSeconds);
}

bool IsTitleInteractive() {
    return gScreenState == ScreenState::Title
        && TitleContentOpacity() >= 0.999;
}

int HitTestTitleButton(int designX, int designY) {
    if (!IsTitleInteractive() || gIsExitDialogVisible) {
        return -1;
    }
    const POINT point{designX, designY};
    for (int index = 0; index < 3; ++index) {
        if (PtInRect(&gTitleButtonRects[index], point)) {
            return index;
        }
    }
    return -1;
}

void DrawExitDialog(HDC dc, const Layout& layout) {
    const RECT logicalDialog = ExitDialogRect();
    const RECT dialog = LogicalRect(
        layout,
        logicalDialog.left,
        logicalDialog.top,
        kExitDialogWidth,
        kExitDialogHeight);
    FillSolid(dc, dialog, kExitDialogColor);
    const HBRUSH borderBrush = CreateSolidBrush(RGB(0xb0, 0xb0, 0xb0));
    FrameRect(dc, &dialog, borderBrush);
    DeleteObject(borderBrush);

    const RECT questionArea{
        logicalDialog.left + 10,
        logicalDialog.top + 36,
        logicalDialog.right - 10,
        logicalDialog.top + 92
    };
    DrawCenteredText(
        dc,
        layout,
        questionArea,
        L"게임을 정말 종료하시겠습니까?",
        20.0,
        RGB(0xff, 0xff, 0xff));

    const RECT yesLogical = ExitDialogButtonRect(true);
    const RECT noLogical = ExitDialogButtonRect(false);
    FillRoundedRect(
        dc,
        LogicalRect(
            layout,
            yesLogical.left,
            yesLogical.top,
            kExitDialogButtonWidth,
            kExitDialogButtonHeight),
        static_cast<int>(std::lround(8.0 * layout.scale)),
        kExitYesButtonColor);
    FillRoundedRect(
        dc,
        LogicalRect(
            layout,
            noLogical.left,
            noLogical.top,
            kExitDialogButtonWidth,
            kExitDialogButtonHeight),
        static_cast<int>(std::lround(8.0 * layout.scale)),
        kExitNoButtonColor);
    DrawCenteredText(
        dc,
        layout,
        yesLogical,
        L"예",
        18.0,
        RGB(0x18, 0x2a, 0x18),
        FW_BOLD);
    DrawCenteredText(
        dc,
        layout,
        noLogical,
        L"아니요",
        18.0,
        RGB(0x36, 0x18, 0x18),
        FW_BOLD);
}

void DrawTitleScreen(HDC dc, const RECT& client) {
    FillSolid(dc, client, kLetterboxColor);
    if (client.right <= client.left || client.bottom <= client.top) {
        return;
    }

    const Layout layout = GetLayout(client);
    const double elapsed = (GetTickCount64() - gTitleStartTime) / 1000.0;
    const double breathingElapsed = (std::max)(
        0.0,
        elapsed - kTitleAppearanceDelaySeconds);
    constexpr double pi = 3.14159265358979323846;
    const double breathingAmount = 0.5 - 0.5 * std::cos(
        breathingElapsed * 2.0 * pi / kTitleBreathingSeconds);
    const double breathingScale = 1.0
        + (kTitleBreathingScale - 1.0) * breathingAmount;
    const int titleWidth = static_cast<int>(std::lround(
        kTitleImageSourceWidth * kTitleImageIntegerScale
        * breathingScale));
    const int titleHeight = static_cast<int>(std::lround(
        kTitleImageSourceHeight * kTitleImageIntegerScale
        * breathingScale));
    const int titleCenterX = kDesignWidth / 2;
    const int titleCenterY = kTitlePlaceholderY
        + kTitlePlaceholderHeight / 2;
    const RECT titleArea = LogicalRect(
        layout,
        titleCenterX - titleWidth / 2,
        titleCenterY - titleHeight / 2,
        titleWidth,
        titleHeight);

    if (gTitleImage != nullptr) {
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(
            Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        graphics.DrawImage(
            gTitleImage,
            Gdiplus::Rect(
                titleArea.left,
                titleArea.top,
                titleArea.right - titleArea.left,
                titleArea.bottom - titleArea.top),
            0,
            0,
            kTitleImageSourceWidth,
            kTitleImageSourceHeight,
            Gdiplus::UnitPixel);
    }

    for (int index = 0; index < 3; ++index) {
        const double fontHeight = kTitleButtonFontHeight
            * gTitleButtonScales[index];
        const HFONT font = CreateUiFont(
            layout,
            fontHeight,
            FW_NORMAL,
            kTitleFontName);
        const HGDIOBJ oldFont = SelectObject(dc, font);
        SIZE textSize{};
        GetTextExtentPoint32(
            dc,
            kTitleButtonLabels[index],
            static_cast<int>(wcslen(kTitleButtonLabels[index])),
            &textSize);
        SelectObject(dc, oldFont);
        DeleteObject(font);

        const int logicalTextWidth = static_cast<int>(
            std::ceil(textSize.cx / layout.scale));
        const int logicalTextHeight = static_cast<int>(
            std::ceil(textSize.cy / layout.scale));
        const int buttonWidth = logicalTextWidth + kTitleButtonPadding * 2;
        const int buttonHeight = logicalTextHeight + kTitleButtonPadding * 2;
        gTitleButtonRects[index] = {
            kDesignWidth / 2 - buttonWidth / 2,
            kTitleButtonCenterY[index] - buttonHeight / 2,
            kDesignWidth / 2 - buttonWidth / 2 + buttonWidth,
            kTitleButtonCenterY[index] - buttonHeight / 2 + buttonHeight
        };
        DrawCenteredText(
            dc,
            layout,
            gTitleButtonRects[index],
            kTitleButtonLabels[index],
            fontHeight,
            RGB(0xff, 0xff, 0xff),
            FW_NORMAL,
            kTitleFontName);
    }

    if (gIsExitDialogVisible) {
        DrawExitDialog(dc, layout);
    }

    const double opacity = TitleContentOpacity();
    if (opacity < 1.0) {
        FillTranslucent(
            dc,
            client,
            kLetterboxColor,
            static_cast<BYTE>(std::lround((1.0 - opacity) * 255.0)));
    }
}

void StartPreparationScreenTransition() {
    if (!IsTitleInteractive() || gIsExitDialogVisible) {
        return;
    }
    gEarnedMoney = 0;
    gDayRevenue = 0;
    for (long long& revenue : gDayMenuRevenue) {
        revenue = 0;
    }
    gCurrentDay = 1;
    gDailyRevenueGoal = kInitialDailyRevenueGoal;
    gCurrentDayGoalMet = false;
    gDailyGoalFailureCount = 0;
    gIsFinalSettlement = false;
    gDailyRevenueHistory.clear();
    gDayStartTime = 0;
    gCookingMistakeCount = 0;
    gScreenState = ScreenState::TitleFadingOut;
    gScreenTransitionStartTime = GetTickCount64();
    gHoveredTitleButton = -1;
}

void ReturnToMainScreen(ULONGLONG now) {
    gEarnedMoney = 0;
    gDayRevenue = 0;
    for (long long& revenue : gDayMenuRevenue) {
        revenue = 0;
    }
    gCurrentDay = 1;
    gDailyRevenueGoal = kInitialDailyRevenueGoal;
    gCurrentDayGoalMet = false;
    gDailyGoalFailureCount = 0;
    gDailyRevenueHistory.clear();
    gDayStartTime = 0;
    gCookingMistakeCount = 0;
    gScreenState = ScreenState::Preparation;
    gHoveredTitleButton = -1;
    gIsExitDialogVisible = false;
    gIsNarrationActive = false;
    gIsNarrationTyping = false;
    gIsNarrationBoxInteractive = false;
    gNarrationFadeStartTime = 0;
    gMaterialCloneCount = 0;
    gStarParticles.clear();
    gNarrationParticles.clear();
    gCookingState = CookingState::NonCooking;
    gIsCookingTransitionRunning = false;
    gTableLift = 0.0;
    gIsFinalSettlement = false;
}

void StartCookingTransition(CookingState targetState) {
    gCookingTransitionTargetState = targetState;
    gIsCookingTransitionRunning = true;
    gCookingTransitionStartLift = gTableLift;
    gCookingTransitionTargetLift = (targetState == CookingState::Cooking)
        ? kCookingTableLift
        : 0.0;
    gCookingTransitionStartTime = GetTickCount64();
    gIsTableHovered = false;
    gHoveredPngSocket = -1;
}

bool IsCookingStateActive() {
    return gCookingState == CookingState::Cooking
        && !gIsCookingTransitionRunning
        && !gIsCompletionPresentationActive;
}

RECT OptionsBackButtonRect() {
    return {
        kOptionsBackButtonX,
        kOptionsBackButtonY,
        kOptionsBackButtonX + kOptionsBackButtonSize,
        kOptionsBackButtonY + kOptionsBackButtonSize
    };
}

RECT VolumeSliderHitRect() {
    return {
        kVolumeSliderLeft - kVolumeSliderKnobRadius,
        kVolumeSliderY - kVolumeSliderKnobRadius * 2,
        kVolumeSliderRight + kVolumeSliderKnobRadius,
        kVolumeSliderY + kVolumeSliderKnobRadius * 2
    };
}

RECT FontLicenseDropdownRect(int index) {
    const int left = kFontLicenseDropdownLeft
        + index * (kFontLicenseDropdownWidth + kFontLicenseDropdownGap);
    return {
        left,
        kFontLicenseDropdownTop,
        left + kFontLicenseDropdownWidth,
        kFontLicenseDropdownTop + kFontLicenseDropdownHeight
    };
}

void DrawFontLicenseText(
    HDC dc,
    const Layout& layout,
    const RECT& logicalArea,
    const wchar_t* text,
    const wchar_t* fontName,
    UINT format) {
    const HFONT font = CreateUiFont(
        layout, kFontLicenseTextHeight, FW_NORMAL, fontName);
    const HGDIOBJ oldFont = SelectObject(dc, font);
    const int oldBackgroundMode = SetBkMode(dc, TRANSPARENT);
    const COLORREF oldTextColor = SetTextColor(dc, RGB(0xee, 0xee, 0xee));
    RECT screenArea = LogicalRect(
        layout,
        logicalArea.left,
        logicalArea.top,
        logicalArea.right - logicalArea.left,
        logicalArea.bottom - logicalArea.top);
    DrawText(dc, text, -1, &screenArea, format | DT_NOPREFIX);
    SetTextColor(dc, oldTextColor);
    SetBkMode(dc, oldBackgroundMode);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

void DrawOptionsScreen(HDC dc, const RECT& client) {
    FillSolid(dc, client, kLetterboxColor);
    if (client.right <= client.left || client.bottom <= client.top) {
        return;
    }
    const Layout layout = GetLayout(client);
    DrawCenteredText(
        dc, layout, OptionsBackButtonRect(), L"<", 32.0,
        RGB(0xff, 0xff, 0xff), FW_NORMAL, kTitleFontName);
    const RECT titleArea{250, 100, 550, 150};
    DrawCenteredText(
        dc, layout, titleArea, L"전체 음량", 30.0,
        RGB(0xff, 0xff, 0xff), FW_NORMAL, kTitleFontName);

    FillSolid(
        dc,
        LogicalRect(
            layout,
            kVolumeSliderLeft,
            kVolumeSliderY - kVolumeSliderTrackHeight / 2,
            kVolumeSliderRight - kVolumeSliderLeft,
            kVolumeSliderTrackHeight),
        RGB(0x70, 0x70, 0x70));
    const int knobX = kVolumeSliderLeft + static_cast<int>(std::lround(
        (kVolumeSliderRight - kVolumeSliderLeft)
        * std::clamp(gMasterSoundVolume, 0.0, 1.0)));
    const RECT knob = LogicalRect(
        layout,
        knobX - kVolumeSliderKnobRadius,
        kVolumeSliderY - kVolumeSliderKnobRadius,
        kVolumeSliderKnobRadius * 2,
        kVolumeSliderKnobRadius * 2);
    const HBRUSH knobBrush = CreateSolidBrush(RGB(0xff, 0xff, 0xff));
    const HGDIOBJ oldBrush = SelectObject(dc, knobBrush);
    Ellipse(dc, knob.left, knob.top, knob.right, knob.bottom);
    SelectObject(dc, oldBrush);
    DeleteObject(knobBrush);

    DrawCenteredText(
        dc, layout, {220, 210, 280, 240}, L"0", 18.0,
        RGB(0xff, 0xff, 0xff));
    DrawCenteredText(
        dc, layout, {520, 210, 580, 240}, L"100", 18.0,
        RGB(0xff, 0xff, 0xff));
    const int displayedVolume = static_cast<int>(std::floor(
        std::clamp(gMasterSoundVolume, 0.0, 1.0) * 100.0 + 0.000001));
    DrawCenteredText(
        dc,
        layout,
        {knobX - 30, kVolumeSliderY - 38,
         knobX + 30, kVolumeSliderY - 16},
        std::to_wstring(displayedVolume).c_str(),
        13.0,
        RGB(0xff, 0xff, 0xff),
        FW_NORMAL,
        kTitleFontName);

    constexpr wchar_t dropdownNames[2][16] = {
        L"DOS Philgi",
        L"PF Stardust"
    };
    constexpr const wchar_t* dropdownFonts[2] = {
        kNarrationFontName,
        kDefaultUiFontName
    };
    for (int index = 0; index < 2; ++index) {
        const RECT dropdown = FontLicenseDropdownRect(index);
        const RECT screenDropdown = LogicalRect(
            layout,
            dropdown.left,
            dropdown.top,
            dropdown.right - dropdown.left,
            dropdown.bottom - dropdown.top);
        FillSolid(dc, screenDropdown, RGB(0x20, 0x20, 0x20));
        const HBRUSH borderBrush = CreateSolidBrush(RGB(0x70, 0x70, 0x70));
        FrameRect(dc, &screenDropdown, borderBrush);
        DeleteObject(borderBrush);
        const std::wstring label =
            (gExpandedFontLicenseDropdown == index ? L"▼  " : L"▶  ")
            + std::wstring(dropdownNames[index]);
        DrawFontLicenseText(
            dc,
            layout,
            {dropdown.left + 10, dropdown.top,
             dropdown.right - 10, dropdown.bottom},
            label.c_str(),
            dropdownFonts[index],
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    if (gExpandedFontLicenseDropdown >= 0) {
        const RECT body{50, 294, 750, 580};
        const RECT screenBody = LogicalRect(
            layout,
            body.left,
            body.top,
            body.right - body.left,
            body.bottom - body.top);
        FillSolid(dc, screenBody, RGB(0x18, 0x18, 0x18));
        const HBRUSH borderBrush = CreateSolidBrush(RGB(0x70, 0x70, 0x70));
        FrameRect(dc, &screenBody, borderBrush);
        DeleteObject(borderBrush);
        const bool isDosLicense = gExpandedFontLicenseDropdown == 0;
        DrawFontLicenseText(
            dc,
            layout,
            {body.left + 12, body.top + 10,
             body.right - 12, body.bottom - 10},
            isDosLicense ? kDosPhilgiLicenseText : kPfStardustLicenseText,
            isDosLicense ? kNarrationFontName : kDefaultUiFontName,
            DT_LEFT | DT_TOP | DT_WORDBREAK);
    }
}

bool CanEnterCookingMode() {
    return gScreenState == ScreenState::Game
        && gNpcOrderState == NpcOrderState::AfterOrder
        && !gHasCookingResult
        && gCookingState == CookingState::NonCooking
        && !gIsCookingTransitionRunning;
}

bool IsMouseOverCookingTable() {
    if (!gHasMousePosition || !gIsTrackingMouse || !CanEnterCookingMode()) {
        return false;
    }
    const int playX = gMouseDesignPosition.x - kPlayAreaX;
    const int playY = gMouseDesignPosition.y - kPlayAreaY;
    return playX >= 0
        && playX <= kPlayAreaSize
        && playY >= kTableHoverTop
        && playY <= kTableHoverBottom;
}

void DrawCookingEntryIndicator(HDC dc, const Layout& layout) {
    if (!CanEnterCookingMode() || gCookingEntryIndicatorStartTime == 0) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const double elapsed = (
        now - gCookingEntryIndicatorStartTime) / 1000.0;
    const double opacity = SmoothStep(
        elapsed / kCookingEntryIndicatorFadeSeconds);
    for (int row = 0; row < kCookingEntryIndicatorGradientHeight; ++row) {
        const double rowOpacity = opacity
            * kCookingEntryIndicatorGradientMaxOpacity * (row + 1)
            / kCookingEntryIndicatorGradientHeight;
        FillTranslucent(
            dc,
            LogicalRect(
                layout,
                kPlayAreaX,
                kPlayAreaY + kPlayAreaSize
                    - kCookingEntryIndicatorGradientHeight + row,
                kPlayAreaSize,
                1),
            RGB(0x00, 0x00, 0x00),
            static_cast<BYTE>(std::lround(rowOpacity * 255.0)));
    }

    const double cycleSeconds = kCookingEntryIndicatorBounceHalfSeconds * 2.0;
    constexpr double pi = 3.14159265358979323846;
    const double bounce = kCookingEntryIndicatorBounceHeight * std::sin(
        std::fmod(elapsed, cycleSeconds) / cycleSeconds * pi);
    const double centerX = kPlayAreaX + kPlayAreaSize * 0.5;
    const double tipY = kPlayAreaY + kPlayAreaSize - 5.0 - bounce;
    const POINT left = LogicalPoint(
        layout,
        centerX - kCookingEntryIndicatorTriangleWidth * 0.5,
        tipY - kCookingEntryIndicatorTriangleHeight);
    const POINT right = LogicalPoint(
        layout,
        centerX + kCookingEntryIndicatorTriangleWidth * 0.5,
        tipY - kCookingEntryIndicatorTriangleHeight);
    const POINT tip = LogicalPoint(layout, centerX, tipY);
    Gdiplus::Point points[] = {
        Gdiplus::Point(left.x, left.y),
        Gdiplus::Point(right.x, right.y),
        Gdiplus::Point(tip.x, tip.y)
    };
    Gdiplus::Graphics graphics(dc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::SolidBrush brush(Gdiplus::Color(
        static_cast<BYTE>(std::lround(opacity * 255.0)), 0, 0, 0));
    graphics.FillPolygon(&brush, points, 3);
}

bool ShouldDrawNpc() {
    return gScreenState == ScreenState::NpcEntering
        || gScreenState == ScreenState::NarrationStarting
        || gScreenState == ScreenState::Game
        || gScreenState == ScreenState::EmptySubmissionReacting
        || gScreenState == ScreenState::NpcExiting;
}

double NpcVerticalMotionOffset() {
    if (gScreenState == ScreenState::NpcEntering
        || gScreenState == ScreenState::NpcExiting) {
        return 0.0;
    }
    if (gScreenState == ScreenState::EmptySubmissionReacting) {
        const double elapsed = (
            GetTickCount64() - gPreparationSequenceStartTime) / 1000.0;
        const double jumpProgress = std::fmod(
            elapsed, kEmptySubmissionJumpSeconds)
            / kEmptySubmissionJumpSeconds;
        constexpr double pi = 3.14159265358979323846;
        return -kEmptySubmissionJumpHeight * std::sin(jumpProgress * pi);
    }
    if (gIsNarrationActive && gIsNarrationTyping && !gWasEmptySubmission) {
        const double elapsed = (
            GetTickCount64() - gNarrationStartTime) / 1000.0;
        const double bounceProgress = std::fmod(
            elapsed, kNpcDialogueBounceSeconds) / kNpcDialogueBounceSeconds;
        constexpr double pi = 3.14159265358979323846;
        return -kNpcDialogueBounceHeight
            * std::sin(bounceProgress * pi);
    }
    return (gNpcIdleStep % 2 == 0) ? 0.0 : -kNpcIdleRise;
}

void DrawNpc(HDC dc, const Layout& layout) {
    if (!ShouldDrawNpc()) {
        return;
    }

    double entranceProgress = 1.0;
    if (gScreenState == ScreenState::NpcEntering
        || gScreenState == ScreenState::NpcExiting) {
        const double elapsed = (
            GetTickCount64() - gPreparationSequenceStartTime) / 1000.0;
        const double movementSeconds = gScreenState == ScreenState::NpcExiting
                && gWasEmptySubmission
            ? kNpcEntranceSeconds / kEmptySubmissionExitSpeedMultiplier
            : kNpcEntranceSeconds;
        const double movementProgress = (std::max)(
            0.0,
            (std::min)(1.0, elapsed / movementSeconds));
        entranceProgress = gScreenState == ScreenState::NpcExiting
            ? 1.0 - movementProgress
            : movementProgress;
    }

    double centerX = kNpcEntranceTargetX + kNpcDisplaySize / 2.0;
    double bottomY = kNpcEntranceStartBottomY;
    double spriteScale = 1.0;
    double horizontalRotationScale = gNpcEntersFromLeft ? -1.0 : 1.0;
    if (entranceProgress < kNpcEntranceHorizontalFraction) {
        const double progress = SmoothStep(
            entranceProgress / kNpcEntranceHorizontalFraction);
        const double startX = gNpcEntersFromLeft
            ? kNpcEntranceLeftStartX
            : kNpcEntranceRightStartX;
        const double leftX = startX
            + (kNpcEntranceTargetX - startX) * progress;
        centerX = leftX + kNpcDisplaySize / 2.0;
    } else {
        const double progress = SmoothStep(
            (entranceProgress - kNpcEntranceHorizontalFraction)
                / (1.0 - kNpcEntranceHorizontalFraction));
        bottomY = kNpcEntranceStartBottomY
            + (kNpcEntranceTargetBottomY - kNpcEntranceStartBottomY)
                * progress;
        spriteScale = 1.0
            + (kNpcEntranceTargetScale - 1.0) * progress;
        if (gNpcEntersFromLeft) {
            constexpr double pi = 3.14159265358979323846;
            horizontalRotationScale = -std::cos(pi * progress);
        }
    }
    bottomY += NpcVerticalMotionOffset();

    const POINT anchor = LogicalPoint(
        layout,
        kPlayAreaX + centerX,
        kPlayAreaY + bottomY);
    const double sourceToDisplay = static_cast<double>(kNpcDisplaySize)
        / kNpcSourceSize;
    const Gdiplus::REAL scaleX = static_cast<Gdiplus::REAL>(
        layout.scale * sourceToDisplay * spriteScale
            * horizontalRotationScale);
    const Gdiplus::REAL scaleY = static_cast<Gdiplus::REAL>(
        layout.scale * sourceToDisplay * spriteScale);

    Gdiplus::Graphics graphics(dc);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
    Gdiplus::Matrix transform(
        scaleX,
        0.0f,
        0.0f,
        scaleY,
        static_cast<Gdiplus::REAL>(anchor.x),
        static_cast<Gdiplus::REAL>(anchor.y));
    graphics.SetTransform(&transform);

    const int halfSourceSize = kNpcSourceSize / 2;
    // 배열 순서는 아래 레이어부터 위 레이어까지의 합성 순서다.
    for (int part = 0; part < kNpcPartCount; ++part) {
        const int variant = gSelectedNpcPartVariants[part];
        if (variant < 0) {
            continue;
        }
        Gdiplus::Image* image = gNpcPartImages[part][variant];
        if (image == nullptr) {
            continue;
        }
        graphics.DrawImage(
            image,
            Gdiplus::Rect(
                -halfSourceSize,
                -kNpcSourceSize,
                kNpcSourceSize,
                kNpcSourceSize),
            0,
            0,
            kNpcSourceSize,
            kNpcSourceSize,
            Gdiplus::UnitPixel);
    }
}

RECT TrophyLogicalRect() {
    return {
        kTrophyImageX,
        kTrophyImageY,
        kTrophyImageX + kTrophyImageDisplaySize,
        kTrophyImageY + kTrophyImageDisplaySize
    };
}

void DrawTrophy(HDC dc, const Layout& layout) {
    const RECT logicalArea = TrophyLogicalRect();
    const RECT area = LogicalRect(
        layout,
        logicalArea.left,
        logicalArea.top,
        logicalArea.right - logicalArea.left,
        logicalArea.bottom - logicalArea.top);
    if (gTrophyImage != nullptr) {
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        if (gTrophyShadowImage != nullptr) {
            const RECT shadowArea = LogicalRect(
                layout,
                logicalArea.left - kTrophyShadowBlurRadius,
                logicalArea.top - kTrophyShadowBlurRadius,
                kTrophyImageDisplaySize + kTrophyShadowBlurRadius * 2,
                kTrophyImageDisplaySize + kTrophyShadowBlurRadius * 2);
            graphics.DrawImage(
                gTrophyShadowImage,
                Gdiplus::Rect(
                    shadowArea.left,
                    shadowArea.top,
                    shadowArea.right - shadowArea.left,
                    shadowArea.bottom - shadowArea.top));
        }
        graphics.DrawImage(
            gTrophyImage,
            Gdiplus::Rect(
                area.left,
                area.top,
                area.right - area.left,
                area.bottom - area.top),
            0,
            0,
            kTrophyImageSourceSize,
            kTrophyImageSourceSize,
            Gdiplus::UnitPixel);
    }
}

void DrawTrophyTooltip(HDC dc, const Layout& layout) {
    if (!gIsTrophyHovered) {
        return;
    }

    constexpr wchar_t tooltipText[] =
        L"왠지 이걸 사면 하나 정도 실수해도 괜찮을 것 같다.";
    const HFONT font = CreateUiFont(
        layout,
        kTrophyTooltipFontHeight,
        FW_NORMAL,
        kDefaultUiFontName);
    const HGDIOBJ oldFont = SelectObject(dc, font);
    SIZE textSize{};
    GetTextExtentPoint32(
        dc,
        tooltipText,
        static_cast<int>(wcslen(tooltipText)),
        &textSize);
    SelectObject(dc, oldFont);
    DeleteObject(font);

    const int logicalTextWidth = static_cast<int>(
        std::ceil(textSize.cx / layout.scale));
    const int logicalTextHeight = static_cast<int>(
        std::ceil(textSize.cy / layout.scale));
    const int tooltipWidth = (std::max)(
        logicalTextWidth + kTrophyTooltipPadding * 2,
        120);
    const int tooltipHeight = logicalTextHeight
        + kTrophyPriceRowHeight
        + kTrophyTooltipPadding * 3;
    const POINT tooltipAnchor = gTrophyInsufficientFundsStartTime != 0
        ? gTrophyFeedbackTooltipAnchorPosition
        : gTrophyTooltipAnchorPosition;
    int tooltipX = tooltipAnchor.x + kTrophyTooltipGap;
    int tooltipY = tooltipAnchor.y + kTrophyTooltipGap;
    tooltipX = (std::min)(tooltipX, kDesignWidth - tooltipWidth);
    tooltipY = (std::min)(tooltipY, kDesignHeight - tooltipHeight);
    const RECT logicalTooltip{
        tooltipX,
        tooltipY,
        tooltipX + tooltipWidth,
        tooltipY + tooltipHeight
    };
    const RECT tooltip = LogicalRect(
        layout,
        logicalTooltip.left,
        logicalTooltip.top,
        tooltipWidth,
        tooltipHeight);
    FillSolid(dc, tooltip, RGB(0x2a, 0x2a, 0x2a));
    const RECT descriptionArea{
        logicalTooltip.left + kTrophyTooltipPadding,
        logicalTooltip.top + kTrophyTooltipPadding,
        logicalTooltip.right - kTrophyTooltipPadding,
        logicalTooltip.top + kTrophyTooltipPadding + logicalTextHeight
    };
    DrawCenteredText(
        dc,
        layout,
        descriptionArea,
        tooltipText,
        kTrophyTooltipFontHeight,
        RGB(0xff, 0xff, 0xff));

    const int priceTop = descriptionArea.bottom + kTrophyTooltipPadding;
    const RECT logicalCoin{
        logicalTooltip.left + kTrophyTooltipPadding,
        priceTop + (kTrophyPriceRowHeight - kTrophyPriceCoinSize) / 2,
        logicalTooltip.left + kTrophyTooltipPadding + kTrophyPriceCoinSize,
        priceTop + (kTrophyPriceRowHeight + kTrophyPriceCoinSize) / 2
    };
    const RECT coin = LogicalRect(
        layout,
        logicalCoin.left,
        logicalCoin.top,
        kTrophyPriceCoinSize,
        kTrophyPriceCoinSize);
    if (gOwnedMoneyCoinImage != nullptr) {
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        graphics.DrawImage(
            gOwnedMoneyCoinImage,
            Gdiplus::Rect(
                coin.left, coin.top,
                coin.right - coin.left, coin.bottom - coin.top),
            0, 0,
            kMoneyCoinSourceSize, kMoneyCoinSourceSize,
            Gdiplus::UnitPixel);
    }
    const RECT priceArea{
        logicalCoin.right + kTrophyPriceTextGap,
        priceTop,
        logicalTooltip.right - kTrophyTooltipPadding,
        priceTop + kTrophyPriceRowHeight
    };
    const ULONGLONG insufficientFundsElapsed =
        gTrophyInsufficientFundsStartTime == 0
            ? 0
            : GetTickCount64() - gTrophyInsufficientFundsStartTime;
    const bool insufficientFunds =
        gTrophyInsufficientFundsStartTime != 0
        && insufficientFundsElapsed
            < kTrophyPriceBlinkDurationMilliseconds;
    {
        const std::wstring priceText = gIsTrophyPurchased
            ? L"구매됨!"
            : FormatMoney(kTrophyPrice);
        const HFONT priceFont = CreateUiFont(
            layout, kTrophyTooltipFontHeight, FW_BOLD);
        const HGDIOBJ oldPriceFont = SelectObject(dc, priceFont);
        const int oldPriceBackgroundMode = SetBkMode(dc, TRANSPARENT);
        const COLORREF oldPriceColor = SetTextColor(
            dc,
            gIsTrophyPurchased
                ? RGB(0x66, 0xdd, 0x77)
                : (insufficientFunds
                    && (insufficientFundsElapsed
                            / kTrophyPriceBlinkMilliseconds) % 2 == 0
                    ? RGB(0xff, 0x55, 0x55)
                    : RGB(0xff, 0xff, 0xff)));
        RECT screenPriceArea = LogicalRect(
            layout,
            priceArea.left,
            priceArea.top,
            priceArea.right - priceArea.left,
            priceArea.bottom - priceArea.top);
        DrawText(
            dc,
            priceText.c_str(),
            -1,
            &screenPriceArea,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SetTextColor(dc, oldPriceColor);
        SetBkMode(dc, oldPriceBackgroundMode);
        SelectObject(dc, oldPriceFont);
        DeleteObject(priceFont);
    }
}

RECT TutorialCheckboxRect() {
    return {
        kTutorialCheckboxLeft,
        kTutorialCheckboxTop,
        kTutorialCheckboxLeft + kTutorialCheckboxSize,
        kTutorialCheckboxTop + kTutorialCheckboxSize
    };
}

void StartPreGameMessage(ULONGLONG now) {
    gPreGameMessageIndex = static_cast<int>(
        NextRandom() % std::size(kPreGameMessages));
    gPreGameMessageStartTime = now;
    gScreenState = ScreenState::PreGameMessage;
}

void DrawPreGameMessage(HDC dc, const RECT& client) {
    FillSolid(dc, client, kLetterboxColor);
    if (client.right <= client.left || client.bottom <= client.top) {
        return;
    }
    const double elapsed = (
        GetTickCount64() - gPreGameMessageStartTime) / 1000.0;
    const double fadeOutStart = kPreGameMessageFadeSeconds
        + kPreGameMessageHoldSeconds;
    double opacity = 1.0;
    if (elapsed < kPreGameMessageFadeSeconds) {
        opacity = SmoothStep(elapsed / kPreGameMessageFadeSeconds);
    } else if (elapsed > fadeOutStart) {
        opacity = 1.0 - SmoothStep(
            (elapsed - fadeOutStart) / kPreGameMessageFadeSeconds);
    }
    const Layout layout = GetLayout(client);
    const RECT messageArea{
        20,
        kDesignHeight / 2 - 30,
        kDesignWidth - 20,
        kDesignHeight / 2 + 30
    };
    const COLORREF textColor = BlendColor(
        kLetterboxColor,
        RGB(0xff, 0xff, 0xff),
        opacity);
    const HFONT font = CreateUiFont(
        layout, kPreGameMessageFontHeight, FW_NORMAL);
    const HGDIOBJ oldFont = SelectObject(dc, font);
    const int oldBackgroundMode = SetBkMode(dc, TRANSPARENT);
    const COLORREF oldTextColor = SetTextColor(dc, textColor);
    RECT screenArea = LogicalRect(
        layout,
        messageArea.left,
        messageArea.top,
        messageArea.right - messageArea.left,
        messageArea.bottom - messageArea.top);
    DrawTextW(
        dc,
        kPreGameMessages[gPreGameMessageIndex],
        -1,
        &screenArea,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SetTextColor(dc, oldTextColor);
    SetBkMode(dc, oldBackgroundMode);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

void DrawTutorialScreen(HDC dc, const RECT& client) {
    FillSolid(dc, client, kLetterboxColor);
    if (client.right <= client.left || client.bottom <= client.top) {
        return;
    }
    const Layout layout = GetLayout(client);
    const auto drawText = [&](int x, int y, int width, int height,
                              const wchar_t* text, double fontHeight,
                              int weight = FW_NORMAL) {
        RECT area = LogicalRect(layout, x, y, width, height);
        const HFONT font = CreateUiFont(layout, fontHeight, weight);
        const HGDIOBJ oldFont = SelectObject(dc, font);
        const int oldBackgroundMode = SetBkMode(dc, TRANSPARENT);
        const COLORREF oldTextColor = SetTextColor(dc, RGB(0xee, 0xee, 0xee));
        DrawText(dc, text, -1, &area,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SetTextColor(dc, oldTextColor);
        SetBkMode(dc, oldBackgroundMode);
        SelectObject(dc, oldFont);
        DeleteObject(font);
    };
    const auto drawImage = [&](Gdiplus::Image* image, int sourceSize,
                               int x, int y, int size) {
        if (image == nullptr) {
            return;
        }
        const RECT area = LogicalRect(layout, x, y, size, size);
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        graphics.DrawImage(image,
            Gdiplus::Rect(area.left, area.top,
                area.right - area.left, area.bottom - area.top),
            0, 0, sourceSize, sourceSize, Gdiplus::UnitPixel);
    };

    struct TutorialMoneyRow {
        Gdiplus::Image* image;
        const wchar_t* title;
        const wchar_t* description;
    };
    const TutorialMoneyRow rows[] = {
        {gOwnedMoneyCoinImage, L"가진 돈", L"- 벌어들인돈의 일부가 전환된다."},
        {gEarnedMoneyCoinImage, L"벌어들인 돈", L"- 매 판마다 벌어들인돈이 표시된다."},
        {gObjectiveImage, L"목표 금액", L"- 매일 달성해야 할 금액."}
    };
    for (int index = 0; index < 3; ++index) {
        const int y = 88 + index * 48;
        drawImage(rows[index].image, kMoneyCoinSourceSize,
            kTutorialContentLeft, y + 4, 22);
        drawText(kTutorialContentLeft + 38, y, 300, 22,
            rows[index].title, 15.0);
        drawText(kTutorialContentLeft + 38, y + 20, 330, 20,
            rows[index].description, 12.0);
    }
    drawImage(gTrophyImage, kTrophyImageSourceSize,
        kTutorialContentLeft - 6, 234, 40);
    drawText(kTutorialContentLeft + 38, 232, 300, 24,
        L"개발자의 트로피", 15.0);
    drawText(kTutorialContentLeft + 38, 254, 340, 20,
        L"- 가진 돈으로 살수있는 유일한 아이템.", 12.0);

    drawText(kTutorialContentLeft, 296, 300, 28, L"플레이 방법", 18.0);
    drawText(kTutorialContentLeft + 4, 330, 360, 22,
        L"1.  NPC의 주문을 듣는다.", 14.0, FW_BOLD);
    drawText(kTutorialContentLeft + 4, 358, 390, 22,
        L"2.  나레이션 박스를 클릭하여 추가 주문을 듣는다.", 14.0);
    drawText(kTutorialContentLeft + 4, 386, 390, 22,
        L"3.  주문에 따라 재료를 클릭하여 조리한다.", 14.0);

    constexpr wchar_t materialLabels[kMaterialBinCount][8] = {
        L"또띠아", L"양상추", L"파프리카", L"당근", L"에그마요",
        L"토마토", L"민트초코", L"김과 밥", L"우엉", L"단무지",
        L"게살", L"시금치", L"딸기", L"초콜릿"
    };
    for (int index = 0; index < kMaterialBinCount; ++index) {
        const int row = index / 7;
        const int column = index % 7;
        const int x = kTutorialContentLeft + column * 55;
        const int y = 420 + row * 48;
        const RECT imageArea = LogicalRect(layout, x + 9, y, 24, 24);
        DrawMaterialImage(dc, index, imageArea);
        RECT labelArea{x - 2, y + 25, x + 45, y + 43};
        DrawCenteredText(dc, layout, labelArea, materialLabels[index],
            10.0, RGB(0xdd, 0xdd, 0xdd));
    }

    const RECT logicalCheckbox = TutorialCheckboxRect();
    const RECT checkbox = LogicalRect(layout,
        logicalCheckbox.left, logicalCheckbox.top,
        kTutorialCheckboxSize, kTutorialCheckboxSize);
    const HPEN pen = CreatePen(PS_SOLID, 1, RGB(0xdd, 0xdd, 0xdd));
    const HGDIOBJ oldPen = SelectObject(dc, pen);
    const HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, checkbox.left, checkbox.top, checkbox.right, checkbox.bottom);
    if (gSkipTutorial) {
        MoveToEx(dc, checkbox.left + 2, checkbox.top + 5, nullptr);
        LineTo(dc, checkbox.left + 4, checkbox.bottom - 2);
        LineTo(dc, checkbox.right - 2, checkbox.top + 2);
    }
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
    drawText(kTutorialCheckboxLeft + kTutorialCheckboxSize + 6,
        kTutorialCheckboxTop - 5, 180, 20,
        L"다음부터는 표시하지 않기", 11.0);
}

void DrawGame(HDC dc, const RECT& client) {
    FillSolid(dc, client, kLetterboxColor);

    if (client.right <= client.left || client.bottom <= client.top) {
        return;
    }

    const Layout layout = GetLayout(client);
    const RECT playArea = LogicalRect(
        layout, kPlayAreaX, kPlayAreaY, kPlayAreaSize, kPlayAreaSize);
    DrawInterior(dc, layout);

    // 체크 바닥은 정적 배경에 두어 유리돔 이동 중에도 장면이 이어지게 한다.
    const int floorDc = SaveDC(dc);
    IntersectClipRect(
        dc, playArea.left, playArea.top, playArea.right, playArea.bottom);
    DrawPerspectiveFloor(dc, layout);
    RestoreDC(dc, floorDc);

    // NPC는 정적 배경보다 앞, 유리돔과 테이블 그룹보다 뒤에 배치한다.
    const int npcDc = SaveDC(dc);
    IntersectClipRect(
        dc, playArea.left, playArea.top, playArea.right, playArea.bottom);
    DrawNpc(dc, layout);
    RestoreDC(dc, npcDc);

    // 180도 전환이 끝날 때까지 NPC 위에 벽을 다시 그려 뒤쪽에 숨긴다.
    if (gScreenState == ScreenState::NpcEntering
        || gScreenState == ScreenState::NpcExiting) {
        DrawGreenWalls(dc, layout);
    }

    // 테이블과 재료통들을 하나의 그룹으로 묶어 위아래로 이동시킨다.
    const int tableOffset = -static_cast<int>(
        std::lround(gTableLift * layout.scale));
    const int tableDc = SaveDC(dc);
    IntersectClipRect(
        dc, playArea.left, playArea.top, playArea.right, playArea.bottom);
    SetViewportOrgEx(dc, 0, tableOffset, nullptr);

    // 유리돔은 배경과 분리된 반투명 흰색 박스로 테이블과 함께 움직인다.
    const RECT domeArea = LogicalRect(
        layout,
        kPlayAreaX,
        kPlayAreaY + kGlassDomeY,
        kPlayAreaSize,
        kGlassDomeHeight);
    FillTranslucent(
        dc,
        domeArea,
        RGB(0xff, 0xff, 0xff),
        kGlassDomeOpacity);

    // 확장된 테이블 외곽을 먼저 칠하고 내부를 덮어 위쪽 테두리만 노출한다.
    const int tableOuterX = kPlayAreaX - kTableHorizontalExtension;
    const int tableOuterWidth = kPlayAreaSize + kTableHorizontalExtension * 2;
    const int tableOuterHeight = kPlayAreaSize - kTableTop
        + kTableHiddenExtension + kTableBottomExtension;
    FillSolid(dc, LogicalRect(
        layout,
        tableOuterX,
        kPlayAreaY + kTableTop,
        tableOuterWidth,
        tableOuterHeight),
        kTableBorderColor);
    FillSolid(dc, LogicalRect(
        layout,
        tableOuterX + kTableBorderWidth,
        kPlayAreaY + kTableTop + kTableBorderWidth,
        tableOuterWidth - kTableBorderWidth * 2,
        tableOuterHeight - kTableBorderWidth * 2),
        kPlatformColor);

    DrawMaterialBins(dc, layout);
    DrawPngSockets(dc, layout);
    DrawCuttingBoardAndClones(dc, layout);
    RestoreDC(dc, tableDc);

    if (IsCookingStateActive() && gResetButtonOpacity > 0.001) {
        DrawResetButton(dc, layout);
    }

    DrawCookingEntryIndicator(dc, layout);
    DrawTrophy(dc, layout);

}

void DrawApplication(HDC dc, const RECT& client) {
    if (gScreenState == ScreenState::Title
        || gScreenState == ScreenState::TitleFadingOut) {
        DrawTitleScreen(dc, client);
        return;
    }
    if (gScreenState == ScreenState::Options) {
        DrawOptionsScreen(dc, client);
        return;
    }
    if (gScreenState == ScreenState::Tutorial) {
        DrawTutorialScreen(dc, client);
        return;
    }
    if (gScreenState == ScreenState::PreGameMessage) {
        DrawPreGameMessage(dc, client);
        return;
    }

    DrawGame(dc, client);
    const Layout layout = GetLayout(client);
    DrawPreparationSequenceUi(dc, layout);
    // 코인 UI를 먼저 그리고 나레이션 박스가 그 위를 덮도록 한다.
    DrawNarrationOverlay(dc, client, layout);
    if (gScreenState == ScreenState::BusinessClosing) {
        DrawBusinessClosing(dc, layout);
    } else if (gScreenState == ScreenState::Settlement) {
        DrawSettlement(dc, layout);
    }
    if (gScreenState == ScreenState::PreparationFadingIn) {
        const double elapsed = (
            GetTickCount64() - gScreenTransitionStartTime) / 1000.0;
        const double opacity = SmoothStep(elapsed / kScreenFadeSeconds);
        if (opacity < 1.0) {
            FillTranslucent(
                dc,
                client,
                kLetterboxColor,
                static_cast<BYTE>(std::lround((1.0 - opacity) * 255.0)));
        }
    }

    // 별 파티클은 플레이 영역의 클리핑이 끝난 뒤 그려 검은 여백에서도 보이게 한다.
    DrawStarParticles(dc, layout);
    DrawMoneyTooltip(dc, layout);
    DrawTrophyTooltip(dc, layout);
}

void ReleaseBackBuffer() {
    if (gBackBufferDc != nullptr && gBackBufferOldBitmap != nullptr) {
        SelectObject(gBackBufferDc, gBackBufferOldBitmap);
    }
    if (gBackBufferBitmap != nullptr) {
        DeleteObject(gBackBufferBitmap);
    }
    if (gBackBufferDc != nullptr) {
        DeleteDC(gBackBufferDc);
    }
    gBackBufferDc = nullptr;
    gBackBufferBitmap = nullptr;
    gBackBufferOldBitmap = nullptr;
    gBackBufferWidth = 0;
    gBackBufferHeight = 0;
}

bool EnsureBackBuffer(HDC referenceDc, int width, int height) {
    if (gBackBufferDc != nullptr
        && gBackBufferBitmap != nullptr
        && gBackBufferWidth == width
        && gBackBufferHeight == height) {
        return true;
    }
    ReleaseBackBuffer();
    gBackBufferDc = CreateCompatibleDC(referenceDc);
    if (gBackBufferDc == nullptr) {
        return false;
    }
    gBackBufferBitmap = CreateCompatibleBitmap(referenceDc, width, height);
    if (gBackBufferBitmap == nullptr) {
        ReleaseBackBuffer();
        return false;
    }
    gBackBufferOldBitmap = SelectObject(gBackBufferDc, gBackBufferBitmap);
    gBackBufferWidth = width;
    gBackBufferHeight = height;
    return true;
}

void PaintWindow(HWND window) {
    PAINTSTRUCT paint{};
    HDC windowDc = BeginPaint(window, &paint);

    RECT client{};
    GetClientRect(window, &client);
    const int width = client.right - client.left;
    const int height = client.bottom - client.top;

    if (width > 0 && height > 0) {
        // 설정 배율만큼 큰 후면 버퍼에 그린 뒤 축소해 사선 경계의 계단 현상을 줄인다.
        const int bufferWidth = static_cast<int>(std::lround(
            width * kSupersampleScale));
        const int bufferHeight = static_cast<int>(std::lround(
            height * kSupersampleScale));
        if (!EnsureBackBuffer(windowDc, bufferWidth, bufferHeight)) {
            DrawApplication(windowDc, client);
            EndPaint(window, &paint);
            return;
        }

        const RECT bufferClient{0, 0, bufferWidth, bufferHeight};
        DrawApplication(gBackBufferDc, bufferClient);

        if (kSupersampleScale <= 1.0) {
            BitBlt(
                windowDc,
                0, 0, width, height,
                gBackBufferDc,
                0, 0,
                SRCCOPY);
        } else {
            SetStretchBltMode(windowDc, HALFTONE);
            SetBrushOrgEx(windowDc, 0, 0, nullptr);
            StretchBlt(
                windowDc,
                0, 0, width, height,
                gBackBufferDc,
                0, 0, bufferWidth, bufferHeight,
                SRCCOPY);
        }
    }

    EndPaint(window, &paint);
}

void SetSoundPlayerVolume(
    const SoundEffectPlayer& sound,
    double volumeScale) {
    if (!sound.isOpen) {
        return;
    }
    const int volume = static_cast<int>(std::lround(
        std::clamp(
            gMasterSoundVolume * kMasterSoundOutputScale
                * sound.volumeScale * volumeScale,
            0.0,
            1.0)
        * 1000.0));
    const std::wstring command = L"setaudio "
        + std::wstring(sound.alias) + L" volume to "
        + std::to_wstring(volume);
    mciSendStringW(command.c_str(), nullptr, 0, nullptr);
}

void ApplyMasterSoundVolume() {
    for (const SoundEffectPlayer& sound : gSoundEffects) {
        SetSoundPlayerVolume(sound, 1.0);
    }
    SetSoundPlayerVolume(gBusinessMusic, gBusinessMusicVolumeScale);
}

void OpenSoundPlayer(SoundEffectPlayer& sound) {
    const std::wstring absolutePath = gAssetsDirectory
        + sound.relativePath;
    if (GetFileAttributesW(absolutePath.c_str())
        == INVALID_FILE_ATTRIBUTES) {
        return;
    }
    const std::wstring command = L"open \"" + absolutePath
        + L"\" type mpegvideo alias " + sound.alias;
    sound.isOpen = mciSendStringW(
        command.c_str(), nullptr, 0, nullptr) == 0;
}

void PreparePlayerClickSound() {
    SoundEffectPlayer& sound = gSoundEffects[
        static_cast<size_t>(SoundEffect::PlayerClick)];
    if (!sound.isOpen) {
        return;
    }
    const std::wstring seekCommand = L"seek "
        + std::wstring(sound.alias) + L" to start";
    mciSendStringW(seekCommand.c_str(), nullptr, 0, nullptr);
    const std::wstring cueCommand = L"cue "
        + std::wstring(sound.alias) + L" output";
    gIsPlayerClickPrepared = mciSendStringW(
        cueCommand.c_str(), nullptr, 0, nullptr) == 0;
}

void InitializeSoundSystem(HWND notificationWindow) {
    gSoundNotificationWindow = notificationWindow;
    for (SoundEffectPlayer& sound : gSoundEffects) {
        OpenSoundPlayer(sound);
    }
    OpenSoundPlayer(gBusinessMusic);
    ApplyMasterSoundVolume();
    SoundEffectPlayer& clickSound = gSoundEffects[
        static_cast<size_t>(SoundEffect::PlayerClick)];
    if (clickSound.isOpen) {
        gPlayerClickDeviceId = mciGetDeviceIDW(clickSound.alias);
        PreparePlayerClickSound();
    }
}

void ShutdownSoundSystem() {
    for (SoundEffectPlayer& sound : gSoundEffects) {
        if (sound.isOpen) {
            const std::wstring command = L"close "
                + std::wstring(sound.alias);
            mciSendStringW(command.c_str(), nullptr, 0, nullptr);
            sound.isOpen = false;
        }
    }
    if (gBusinessMusic.isOpen) {
        const std::wstring command = L"close "
            + std::wstring(gBusinessMusic.alias);
        mciSendStringW(command.c_str(), nullptr, 0, nullptr);
        gBusinessMusic.isOpen = false;
    }
    gSoundNotificationWindow = nullptr;
    gPlayerClickDeviceId = 0;
    gIsPlayerClickPrepared = false;
}

void PlaySoundEffect(SoundEffect effect) {
    const size_t index = static_cast<size_t>(effect);
    if (gMasterSoundVolume <= 0.0
        || index >= std::size(gSoundEffects)
        || !gSoundEffects[index].isOpen) {
        return;
    }
    if (effect == SoundEffect::PlayerClick && gIsPlayerClickPrepared) {
        const std::wstring command = L"play "
            + std::wstring(gSoundEffects[index].alias) + L" notify";
        gIsPlayerClickPrepared = false;
        mciSendStringW(
            command.c_str(), nullptr, 0, gSoundNotificationWindow);
    } else {
        const std::wstring command = L"play "
            + std::wstring(gSoundEffects[index].alias) + L" from 0"
            + (effect == SoundEffect::PlayerClick ? L" notify" : L"");
        mciSendStringW(
            command.c_str(),
            nullptr,
            0,
            effect == SoundEffect::PlayerClick
                ? gSoundNotificationWindow
                : nullptr);
    }
}

void StartBusinessMusic() {
    if (gIsBusinessMusicPlaying
        || !gBusinessMusic.isOpen
        || gMasterSoundVolume <= 0.0) {
        return;
    }
    gBusinessMusicVolumeScale = 1.0;
    gIsBusinessMusicFadingOut = false;
    SetSoundPlayerVolume(gBusinessMusic, 1.0);
    const std::wstring command = L"play "
        + std::wstring(gBusinessMusic.alias) + L" from 0 repeat";
    mciSendStringW(command.c_str(), nullptr, 0, nullptr);
    gIsBusinessMusicPlaying = true;
}

void BeginBusinessMusicFadeOut(ULONGLONG now) {
    if (!gIsBusinessMusicPlaying) {
        return;
    }
    gIsBusinessMusicFadingOut = true;
    gBusinessMusicFadeStartTime = now;
    gBusinessMusicFadeStep = -1;
}

void UpdateBusinessMusicFadeOut(ULONGLONG now) {
    if (!gIsBusinessMusicFadingOut) {
        return;
    }
    const double progress = std::clamp(
        (now - gBusinessMusicFadeStartTime)
            / (kBusinessMusicFadeOutSeconds * 1000.0),
        0.0,
        1.0);
    const int fadeStep = static_cast<int>(std::floor(
        progress * kBusinessMusicFadeOutSteps));
    if (fadeStep != gBusinessMusicFadeStep) {
        gBusinessMusicFadeStep = fadeStep;
        gBusinessMusicVolumeScale = 1.0
            - static_cast<double>(fadeStep)
                / kBusinessMusicFadeOutSteps;
        SetSoundPlayerVolume(
            gBusinessMusic, gBusinessMusicVolumeScale);
    }
    if (progress >= 1.0) {
        const std::wstring command = L"stop "
            + std::wstring(gBusinessMusic.alias);
        mciSendStringW(command.c_str(), nullptr, 0, nullptr);
        gIsBusinessMusicPlaying = false;
        gIsBusinessMusicFadingOut = false;
    }
}

void UpdateMasterVolumeFromSlider(int designX) {
    const double position = std::clamp(
        static_cast<double>(designX - kVolumeSliderLeft)
            / (kVolumeSliderRight - kVolumeSliderLeft),
        0.0,
        1.0);
    const double newVolume = std::lround(position * 100.0) / 100.0;
    if (newVolume != gMasterSoundVolume) {
        gMasterSoundVolume = newVolume;
        ApplyMasterSoundVolume();
    }
}

LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case MM_MCINOTIFY:
        if (wParam == MCI_NOTIFY_SUCCESSFUL
            && static_cast<MCIDEVICEID>(lParam)
                == gPlayerClickDeviceId) {
            PreparePlayerClickSound();
        }
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT) {
            POINT cursor{};
            GetCursorPos(&cursor);
            ScreenToClient(window, &cursor);
            UpdateMouseCursor(window, cursor.x, cursor.y);
            return TRUE;
        }
        return DefWindowProc(window, message, wParam, lParam);
    case WM_ERASEBKGND:
        return 1;
    case WM_SIZE:
        InvalidateRect(window, nullptr, FALSE);
        return 0;
    case WM_PAINT:
        PaintWindow(window);
        return 0;
    case WM_MOUSEMOVE: {
        RECT client{};
        GetClientRect(window, &client);
        const Layout layout = GetLayout(client);
        const int mouseX = GET_X_LPARAM(lParam);
        const int mouseY = GET_Y_LPARAM(lParam);
        gMouseDesignPosition.x = static_cast<LONG>(std::lround(
            (mouseX - layout.offsetX) / layout.scale));
        gMouseDesignPosition.y = static_cast<LONG>(std::lround(
            (mouseY - layout.offsetY) / layout.scale));
        gHasMousePosition = true;
        const RECT trophy = TrophyLogicalRect();
        const bool wasTrophyHovered = gIsTrophyHovered;
        gIsTrophyHovered = PtInRect(
            &trophy, gMouseDesignPosition) != FALSE;
        if (!wasTrophyHovered && gIsTrophyHovered) {
            gTrophyTooltipAnchorPosition = gMouseDesignPosition;
        }
        if (wasTrophyHovered && !gIsTrophyHovered) {
            gTrophyInsufficientFundsStartTime = 0;
        }
        const MoneyTooltipKind previousMoneyTooltip =
            gHoveredMoneyTooltip;
        gHoveredMoneyTooltip = MoneyTooltipKind::None;
        const RECT moneyUi = MoneyUiRect();
        const RECT objectiveUi = ObjectiveUiRect();
        if (IsOwnedMoneyInterfaceVisible()
            && PtInRect(&moneyUi, gMouseDesignPosition)) {
            gHoveredMoneyTooltip = MoneyTooltipKind::Owned;
        } else if (IsEarnedMoneyInterfaceVisible()
            && PtInRect(&moneyUi, gMouseDesignPosition)) {
            gHoveredMoneyTooltip = MoneyTooltipKind::Earned;
        } else if (IsObjectiveInterfaceVisible()
            && PtInRect(&objectiveUi, gMouseDesignPosition)) {
            gHoveredMoneyTooltip = MoneyTooltipKind::Objective;
        }
        if (gScreenState == ScreenState::Title) {
            gHoveredTitleButton = HitTestTitleButton(
                gMouseDesignPosition.x,
                gMouseDesignPosition.y);
            gIsTableHovered = false;
        } else if (gScreenState == ScreenState::Game) {
            gIsTableHovered = IsMouseOverCookingTable();
            gHoveredTitleButton = -1;
        } else {
            gHoveredTitleButton = -1;
            gIsTableHovered = false;
        }

        if (gScreenState == ScreenState::Options
            && gIsVolumeSliderDragging) {
            UpdateMasterVolumeFromSlider(gMouseDesignPosition.x);
            InvalidateRect(window, nullptr, FALSE);
        }

        if (!gIsTrackingMouse) {
            TRACKMOUSEEVENT tracking{};
            tracking.cbSize = sizeof(tracking);
            tracking.dwFlags = TME_LEAVE;
            tracking.hwndTrack = window;
            TrackMouseEvent(&tracking);
            gIsTrackingMouse = true;
        }
        UpdateMouseCursor(window, mouseX, mouseY);
        if (wasTrophyHovered != gIsTrophyHovered
            || previousMoneyTooltip != gHoveredMoneyTooltip) {
            InvalidateRect(window, nullptr, FALSE);
        }
        const ULONGLONG now = GetTickCount64();
        if (gLastAnimationTickTime == 0
            || now - gLastAnimationTickTime
                >= kAnimationFrameMilliseconds) {
            // WM_TIMER and WM_PAINT are low-priority messages. Continuous mouse
            // input can otherwise starve every time-based animation in the game.
            SendMessage(window, WM_TIMER, kAnimationTimerId, 0);
            UpdateWindow(window);
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        if (gScreenState == ScreenState::TitleFadingOut) {
            return 0;
        }
        RECT client{};
        GetClientRect(window, &client);
        const Layout layout = GetLayout(client);
        const int mouseX = GET_X_LPARAM(lParam);
        const int mouseY = GET_Y_LPARAM(lParam);
        const int designX = static_cast<int>(std::lround(
            (mouseX - layout.offsetX) / layout.scale));
        const int designY = static_cast<int>(std::lround(
            (mouseY - layout.offsetY) / layout.scale));
        const int playX = designX - kPlayAreaX;
        const int playY = designY - kPlayAreaY;
        const bool isInsidePlayArea = playX >= 0
            && playX <= kPlayAreaSize
            && playY >= 0
            && playY <= kPlayAreaSize;

        const POINT logicalMouse{designX, designY};
        SoundEffect clickSound = SoundEffect::PlayerClick;
        bool deferClickSoundToAction = false;
        if (gScreenState == ScreenState::Options) {
            const RECT backButton = OptionsBackButtonRect();
            const RECT slider = VolumeSliderHitRect();
            const RECT dosLicense = FontLicenseDropdownRect(0);
            const RECT pfLicense = FontLicenseDropdownRect(1);
            if (PtInRect(&backButton, logicalMouse)
                || PtInRect(&slider, logicalMouse)
                || PtInRect(&dosLicense, logicalMouse)
                || PtInRect(&pfLicense, logicalMouse)) {
                clickSound = SoundEffect::ButtonClick;
            }
        } else {
            const RECT trophyForSound = TrophyLogicalRect();
            if (gScreenState != ScreenState::Title
                && gScreenState != ScreenState::TitleFadingOut
                && PtInRect(&trophyForSound, logicalMouse)
                && !gIsTrophyPurchased
                && gOwnedMoney >= kTrophyPrice) {
                deferClickSoundToAction = true;
            } else if (gScreenState == ScreenState::Title
                && IsTitleInteractive()) {
                bool clickedButton = HitTestTitleButton(
                    designX, designY) >= 0;
                if (gIsExitDialogVisible) {
                    const RECT yesButton = ExitDialogButtonRect(true);
                    const RECT noButton = ExitDialogButtonRect(false);
                    clickedButton = PtInRect(&yesButton, logicalMouse)
                        || PtInRect(&noButton, logicalMouse);
                }
                if (clickedButton) {
                    clickSound = SoundEffect::ButtonClick;
                }
            } else if (gScreenState == ScreenState::Preparation) {
                const RECT startButton = StartBusinessButtonRect();
                if (PtInRect(&startButton, logicalMouse)) {
                    clickSound = SoundEffect::ButtonClick;
                }
            } else if (gScreenState == ScreenState::Settlement) {
                const double elapsed = (
                    GetTickCount64() - gSettlementStartTime) / 1000.0;
                const RECT nextDayButton = NextDayButtonRect();
                if (elapsed >= kReceiptSlideSeconds
                        + kSettlementButtonDelaySeconds
                    && PtInRect(&nextDayButton, logicalMouse)) {
                    clickSound = SoundEffect::ButtonClick;
                }
            } else if (gScreenState == ScreenState::Game) {
                const RECT resetButtonForSound = ResetButtonRect(layout);
                if (IsCookingStateActive()
                    && PtInRect(&resetButtonForSound, {mouseX, mouseY})) {
                    deferClickSoundToAction = true;
                } else {
                    const int clickedCloneForSound =
                        HitTestMaterialClone(designX, designY);
                    const int clickedSocketForSound = HitTestPngSocket();
                    if (clickedCloneForSound >= 0
                        || (!gIsCookingTransitionRunning
                            && gCookingState == CookingState::Cooking
                            && clickedSocketForSound >= 0)) {
                        clickSound = SoundEffect::InteractionClick;
                    }
                }
            }
        }
        if (!deferClickSoundToAction) {
            PlaySoundEffect(clickSound);
        }
        if (gScreenState == ScreenState::Options) {
            const RECT backButton = OptionsBackButtonRect();
            const RECT slider = VolumeSliderHitRect();
            const RECT dosLicense = FontLicenseDropdownRect(0);
            const RECT pfLicense = FontLicenseDropdownRect(1);
            if (PtInRect(&backButton, logicalMouse)) {
                gScreenState = ScreenState::Title;
                gTitleStartTime = GetTickCount64();
                SavePlayerData();
            } else if (PtInRect(&slider, logicalMouse)) {
                gIsVolumeSliderDragging = true;
                SetCapture(window);
                UpdateMasterVolumeFromSlider(designX);
            } else if (PtInRect(&dosLicense, logicalMouse)) {
                gExpandedFontLicenseDropdown =
                    gExpandedFontLicenseDropdown == 0 ? -1 : 0;
            } else if (PtInRect(&pfLicense, logicalMouse)) {
                gExpandedFontLicenseDropdown =
                    gExpandedFontLicenseDropdown == 1 ? -1 : 1;
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }
        if (gScreenState == ScreenState::Tutorial) {
            const RECT checkbox = TutorialCheckboxRect();
            const RECT checkboxHitArea{
                checkbox.left - 4,
                checkbox.top - 4,
                checkbox.right + 190,
                checkbox.bottom + 4
            };
            if (PtInRect(&checkboxHitArea, logicalMouse)) {
                gSkipTutorial = !gSkipTutorial;
                SavePlayerData();
            } else {
                StartPreGameMessage(GetTickCount64());
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }
        if (gScreenState == ScreenState::PreGameMessage) {
            return 0;
        }
        const RECT trophy = TrophyLogicalRect();
        if (gScreenState != ScreenState::Title
            && gScreenState != ScreenState::TitleFadingOut
            && PtInRect(&trophy, logicalMouse)) {
            if (!gIsTrophyPurchased) {
                if (gOwnedMoney >= kTrophyPrice) {
                    gOwnedMoney -= kTrophyPrice;
                    gIsTrophyPurchased = true;
                    gTrophyInsufficientFundsStartTime = 0;
                    SavePlayerData();
                    PlaySoundEffect(SoundEffect::TrophyPurchase);
                } else {
                    gTrophyFeedbackTooltipAnchorPosition =
                        gTrophyTooltipAnchorPosition;
                    gTrophyInsufficientFundsStartTime = GetTickCount64();
                }
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }

        if (gScreenState == ScreenState::Title) {
            if (!IsTitleInteractive()) {
                return 0;
            }

            const POINT logicalMouse{designX, designY};
            if (gIsExitDialogVisible) {
                const RECT yesButton = ExitDialogButtonRect(true);
                const RECT noButton = ExitDialogButtonRect(false);
                if (PtInRect(&yesButton, logicalMouse)) {
                    DestroyWindow(window);
                } else if (PtInRect(&noButton, logicalMouse)) {
                    gIsExitDialogVisible = false;
                    InvalidateRect(window, nullptr, FALSE);
                }
                return 0;
            }

            const int clickedTitleButton = HitTestTitleButton(
                designX,
                designY);
            if (clickedTitleButton == 0) {
                StartBusinessMusic();
                StartPreparationScreenTransition();
                InvalidateRect(window, nullptr, FALSE);
            } else if (clickedTitleButton == 1) {
                gScreenState = ScreenState::Options;
                gHoveredTitleButton = -1;
                InvalidateRect(window, nullptr, FALSE);
            } else if (clickedTitleButton == 2) {
                gIsExitDialogVisible = true;
                gHoveredTitleButton = -1;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        if (gScreenState == ScreenState::Preparation) {
            const POINT logicalMouse{designX, designY};
            const RECT startButton = StartBusinessButtonRect();
            if (PtInRect(&startButton, logicalMouse)) {
                gScreenState = ScreenState::Countdown;
                gPreparationSequenceStartTime = GetTickCount64();
                gIsTableHovered = false;
                gHoveredPngSocket = -1;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        if (gScreenState == ScreenState::Settlement) {
            const double elapsed = (
                GetTickCount64() - gSettlementStartTime) / 1000.0;
            const POINT logicalMouse{designX, designY};
            const RECT nextDayButton = NextDayButtonRect();
            if (elapsed >= kReceiptSlideSeconds
                    + kSettlementButtonDelaySeconds
                && PtInRect(&nextDayButton, logicalMouse)) {
                const ULONGLONG now = GetTickCount64();
                if (gIsFinalSettlement) {
                    ReturnToMainScreen(now);
                } else {
                    StartNextDay(now);
                }
                UpdateMouseCursor(window, mouseX, mouseY);
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        if (gScreenState != ScreenState::Game) {
            return 0;
        }

        const POINT mousePoint{mouseX, mouseY};
        const RECT narrationBox = NarrationBoxRect(layout);
        const RECT resetButton = ResetButtonRect(layout);
        const int clickedSocket = HitTestPngSocket();
        const int clickedClone = HitTestMaterialClone(designX, designY);
        if (gIsNarrationBoxInteractive
            && PtInRect(&narrationBox, mousePoint)) {
            AdvanceNarration();
            InvalidateRect(window, nullptr, FALSE);
        } else if (IsCookingStateActive()
            && PtInRect(&resetButton, mousePoint)) {
            StartCompletionPresentation();
            FinishCookingOrder();
            gMaterialCloneCount = 0;
            if (!gIsCompletionPresentationActive) {
                StartCookingTransition(CookingState::NonCooking);
            }
            InvalidateRect(window, nullptr, FALSE);
        } else if (CanEnterCookingMode()
            && gIsTableHovered) {
            PrepareHoverDialogues();
            StartCookingTransition(CookingState::Cooking);
        } else if (clickedClone >= 0) {
            RemoveMaterialClone(clickedClone);
            InvalidateRect(window, nullptr, FALSE);
        } else if (!gIsCookingTransitionRunning
            && gCookingState == CookingState::Cooking
            && clickedSocket >= 0) {
            AddMaterialClone(clickedSocket);
            AddClickStars(designX, designY);
            InvalidateRect(window, nullptr, FALSE);
        } else if (!gIsCookingTransitionRunning
            && gCookingState == CookingState::Cooking
            && isInsidePlayArea
            && playY <= kCookingTableY) {
            StartCookingTransition(CookingState::NonCooking);
        }
        return 0;
    }
    case WM_LBUTTONUP:
        if (gIsVolumeSliderDragging) {
            gIsVolumeSliderDragging = false;
            ReleaseCapture();
            SavePlayerData();
        }
        return 0;
    case WM_MOUSELEAVE:
        gIsTrackingMouse = false;
        gIsTrophyHovered = false;
        gHoveredMoneyTooltip = MoneyTooltipKind::None;
        gTrophyInsufficientFundsStartTime = 0;
        gIsTableHovered = false;
        gHoveredPngSocket = -1;
        gHoveredTitleButton = -1;
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        InvalidateRect(window, nullptr, FALSE);
        return 0;
    case WM_TIMER:
        if (wParam == kAnimationTimerId) {
            bool visualChanged = false;
            const ULONGLONG now = GetTickCount64();
            gLastAnimationTickTime = now;
            UpdateBusinessMusicFadeOut(now);

            if (gIsTrophyHovered
                && gTrophyInsufficientFundsStartTime != 0) {
                if (now - gTrophyInsufficientFundsStartTime
                    >= kTrophyPriceBlinkDurationMilliseconds) {
                    gTrophyInsufficientFundsStartTime = 0;
                }
                visualChanged = true;
            }

            if (!gStarParticles.empty()) {
                UpdateStarParticles(window, now);
                visualChanged = true;
            } else {
                gLastStarParticleUpdateTime = now;
            }

            if (gIsCompletionPresentationActive) {
                visualChanged = true;
                if ((now - gCompletionPresentationStartTime) / 1000.0
                    >= kCompletionPresentationSeconds) {
                    gIsCompletionPresentationActive = false;
                    gFogParticles.clear();
                    gCompletedFoodImageIndex = -1;
                    StartCookingTransition(CookingState::NonCooking);
                }
            }

            if (gIsNarrationTyping && gIsNarrationOverflowAnimating) {
                UpdateNarrationParticles(now);
                visualChanged = true;
            } else {
                gLastNarrationParticleUpdateTime = now;
            }

            if (gScreenState == ScreenState::Title
                || gScreenState == ScreenState::TitleFadingOut) {
                if (gScreenState == ScreenState::Title
                    && gHasMousePosition
                    && gIsTrackingMouse) {
                    gHoveredTitleButton = HitTestTitleButton(
                        gMouseDesignPosition.x,
                        gMouseDesignPosition.y);
                } else {
                    gHoveredTitleButton = -1;
                }
                for (int index = 0; index < 3; ++index) {
                    const double targetScale = (
                        !gIsExitDialogVisible
                        && index == gHoveredTitleButton)
                        ? kTitleButtonHoverScale
                        : 1.0;
                    gTitleButtonScales[index] += (
                        targetScale - gTitleButtonScales[index])
                        * kTitleButtonEasingSpeed;
                    if (std::abs(
                            targetScale - gTitleButtonScales[index]) < 0.0001) {
                        gTitleButtonScales[index] = targetScale;
                    }
                }
                visualChanged = true;
            }

            if (gScreenState == ScreenState::TitleFadingOut) {
                const double elapsed = (
                    now - gScreenTransitionStartTime) / 1000.0;
                if (elapsed >= kScreenFadeSeconds) {
                    if (gSkipTutorial) {
                        StartPreGameMessage(now);
                    } else {
                        gScreenState = ScreenState::Tutorial;
                    }
                    gScreenTransitionStartTime = now;
                    gIsTableHovered = false;
                    gHoveredPngSocket = -1;
                }
            } else if (gScreenState == ScreenState::PreGameMessage) {
                const double elapsed = (
                    now - gPreGameMessageStartTime) / 1000.0;
                const double totalSeconds =
                    kPreGameMessageFadeSeconds * 2.0
                    + kPreGameMessageHoldSeconds;
                visualChanged = true;
                if (elapsed >= totalSeconds) {
                    gScreenState = ScreenState::PreparationFadingIn;
                    gScreenTransitionStartTime = now;
                }
            } else if (gScreenState == ScreenState::PreparationFadingIn) {
                const double elapsed = (
                    now - gScreenTransitionStartTime) / 1000.0;
                visualChanged = true;
                if (elapsed >= kScreenFadeSeconds) {
                    gScreenState = ScreenState::Preparation;
                }
            } else if (gScreenState == ScreenState::Countdown) {
                const double elapsed = (
                    now - gPreparationSequenceStartTime) / 1000.0;
                visualChanged = true;
                if (elapsed >= kCountdownStepSeconds * kCountdownStepCount) {
                    if (gDayStartTime == 0) {
                        gDayStartTime = now;
                        StartBusinessMusic();
                    }
                    gScreenState = ScreenState::NpcEntering;
                    gPreparationSequenceStartTime = now;
                    RandomizeNpcAppearance();
                }
            } else if (gScreenState == ScreenState::NpcEntering) {
                const double elapsed = (
                    now - gPreparationSequenceStartTime) / 1000.0;
                visualChanged = true;
                if (elapsed >= kNpcEntranceSeconds) {
                    gScreenState = ScreenState::NarrationStarting;
                    gPreparationSequenceStartTime = now;
                    gNpcIdleStartTime = now;
                    gNpcIdleStep = 0;
                }
            } else if (gScreenState == ScreenState::NarrationStarting) {
                const double elapsed = (
                    now - gPreparationSequenceStartTime) / 1000.0;
                visualChanged = true;
                if (elapsed >= kNarrationAppearanceDelaySeconds
                    && gNarrationFadeStartTime == 0) {
                    gNarrationFadeStartTime = now;
                    gNpcOrderState = NpcOrderState::Ordering;
                    StartRandomDialogueTree();
                }
                if (elapsed >= kNarrationAppearanceDelaySeconds
                        + kNarrationFadeInSeconds) {
                    gScreenState = ScreenState::Game;
                    if (gNpcOrderState == NpcOrderState::AfterOrder) {
                        gCookingEntryIndicatorStartTime = now;
                    }
                }
            } else if (gScreenState == ScreenState::EmptySubmissionReacting) {
                const double elapsed = (
                    now - gPreparationSequenceStartTime) / 1000.0;
                visualChanged = true;
                if (elapsed >= kEmptySubmissionJumpSeconds
                        * kEmptySubmissionJumpCount) {
                    StartNpcExit(now);
                }
            } else if (gScreenState == ScreenState::NpcExiting) {
                const double elapsed = (
                    now - gPreparationSequenceStartTime) / 1000.0;
                visualChanged = true;
                const double exitSeconds = gWasEmptySubmission
                    ? kNpcEntranceSeconds
                        / kEmptySubmissionExitSpeedMultiplier
                    : kNpcEntranceSeconds;
                if (elapsed >= exitSeconds) {
                    if (gShouldEndDayAfterOrder) {
                        StartBusinessClosing(now);
                    } else {
                        StartNextNpcEntrance(now);
                    }
                }
            } else if (gScreenState == ScreenState::BusinessClosing) {
                constexpr size_t closingTextLength =
                    (sizeof(kBusinessClosingText)
                        / sizeof(kBusinessClosingText[0])) - 1;
                const double elapsed = (
                    now - gBusinessClosingStartTime) / 1000.0;
                const double totalSeconds = closingTextLength
                        * kBusinessClosingCharacterSeconds
                    + kBusinessClosingHoldSeconds
                    + kBusinessClosingFadeSeconds;
                visualChanged = true;
                if (elapsed >= totalSeconds) {
                    StartSettlement(now);
                }
            } else if (gScreenState == ScreenState::Settlement) {
                const double elapsed = (
                    now - gSettlementStartTime) / 1000.0;
                const double animationEnd = kReceiptSlideSeconds
                    + kSettlementButtonDelaySeconds
                    + kSettlementButtonFadeSeconds;
                if (elapsed < animationEnd) {
                    visualChanged = true;
                }
            }

            if (ShouldDrawNpc()
                && gScreenState != ScreenState::NpcEntering
                && gScreenState != ScreenState::EmptySubmissionReacting
                && gScreenState != ScreenState::NpcExiting) {
                if (gIsNarrationActive && gIsNarrationTyping) {
                    visualChanged = true;
                } else if (gNpcIdleStartTime != 0) {
                    const int idleStep = static_cast<int>(
                        (now - gNpcIdleStartTime)
                            / kNpcIdleStepMilliseconds);
                    if (idleStep != gNpcIdleStep) {
                        gNpcIdleStep = idleStep;
                        visualChanged = true;
                    }
                }
            }

            if (gNarrationFadeStartTime != 0) {
                const double narrationFadeElapsed = (
                    now - gNarrationFadeStartTime) / 1000.0;
                if (narrationFadeElapsed < kNarrationFadeInSeconds) {
                    visualChanged = true;
                } else if (!gIsNarrationBoxInteractive) {
                    gIsNarrationBoxInteractive = true;
                    visualChanged = true;
                }

                if (gIsNarrationActive && gIsNarrationTyping) {
                    const ULONGLONG elapsed = now - gNarrationStartTime;
                    const size_t nextVisibleLength = (std::min)(
                        gCurrentNarrationText.size(),
                        static_cast<size_t>(
                            elapsed
                                / kNarrationCharacterIntervalMilliseconds
                                + 1));
                    if (nextVisibleLength != gNarrationVisibleLength) {
                        bool revealedNonWhitespace = false;
                        for (size_t index = gNarrationVisibleLength;
                             index < nextVisibleLength;
                             ++index) {
                            if (!std::iswspace(
                                    gCurrentNarrationText[index])) {
                                revealedNonWhitespace = true;
                                break;
                            }
                        }
                        gNarrationVisibleLength = nextVisibleLength;
                        if (revealedNonWhitespace) {
                            PlaySoundEffect(SoundEffect::NpcTalking);
                        }
                        visualChanged = true;
                    }
                    if (gNarrationVisibleLength
                        >= gCurrentNarrationText.size()) {
                        CompleteNarrationTyping(now);
                    }
                }
            }

            if (gScreenState == ScreenState::Game
                && gIsNarrationBoxInteractive
                && gIsNarrationActive
                && !gIsNarrationTyping
                && gNarrationCompletedTime != 0
                && (now - gNarrationCompletedTime) / 1000.0
                    >= kNarrationAutoAdvanceSeconds) {
                AdvanceNarration();
                visualChanged = true;
            }

            if (gScreenState == ScreenState::Game) {
                if (CanEnterCookingMode()) {
                    visualChanged = true;
                }
                gIsTableHovered = IsMouseOverCookingTable();
                const double previousLift = gTableLift;
                if (gIsCookingTransitionRunning) {
                    const double elapsedSeconds = (
                        now - gCookingTransitionStartTime) / 1000.0;
                    const double progress = elapsedSeconds
                        / kCookingAnimationSeconds;
                    const double easedProgress = SmoothStep(progress);
                    gTableLift = gCookingTransitionStartLift
                        + (gCookingTransitionTargetLift
                            - gCookingTransitionStartLift)
                        * easedProgress;
                    if (progress >= 1.0) {
                        gTableLift = gCookingTransitionTargetLift;
                        gCookingState = gCookingTransitionTargetState;
                        gIsCookingTransitionRunning = false;
                    }
                } else if (gCookingState == CookingState::NonCooking) {
                    const double target = gIsTableHovered
                        ? kTableLiftDistance
                        : 0.0;
                    gTableLift += (target - gTableLift) * kTableEasingSpeed;
                    if (std::abs(target - gTableLift) < 0.01) {
                        gTableLift = target;
                    }
                }
                if (gTableLift != previousLift) {
                    visualChanged = true;
                }

                const int previousHoveredSocket = gHoveredPngSocket;
                gHoveredPngSocket = HitTestPngSocket();
                if (gHoveredPngSocket != previousHoveredSocket) {
                    gLastNarrationHoverSocket = -1;
                    if (gHoveredPngSocket >= 0
                        && !gIsHoverNarrationActive) {
                        TryStartHoverNarration(gHoveredPngSocket);
                    }
                }
                for (int index = 0; index < kMaterialBinCount; ++index) {
                    const double targetScale = (index == gHoveredPngSocket)
                        ? kPngSocketHoverScale
                        : 1.0;
                    const double previousScale = gPngSocketScales[index];
                    gPngSocketScales[index] += (
                        targetScale - gPngSocketScales[index])
                        * kPngSocketEasingSpeed;
                    if (std::abs(
                            targetScale - gPngSocketScales[index]) < 0.0001) {
                        gPngSocketScales[index] = targetScale;
                    }
                    if (gPngSocketScales[index] != previousScale) {
                        visualChanged = true;
                    }
                }

                const double targetButtonOpacity = IsCookingStateActive()
                    ? 1.0
                    : 0.0;
                const double previousButtonOpacity = gResetButtonOpacity;
                gResetButtonOpacity += (
                    targetButtonOpacity - gResetButtonOpacity)
                    * kResetButtonFadeSpeed;
                if (std::abs(
                        targetButtonOpacity - gResetButtonOpacity) < 0.001) {
                    gResetButtonOpacity = targetButtonOpacity;
                }
                if (gResetButtonOpacity != previousButtonOpacity) {
                    visualChanged = true;
                }

                if (CanStartEmptySubmissionReaction()) {
                    StartEmptySubmissionReaction(now);
                    visualChanged = true;
                } else if (CanStartNpcExit(now)) {
                    StartNpcExit(now);
                    visualChanged = true;
                }
            }

            const double dayElapsed = CurrentDayElapsedSeconds();
            if (gDayStartTime != 0
                && gScreenState != ScreenState::Settlement
                && dayElapsed >= kSunsetStartSeconds
                && dayElapsed < kDayDurationSeconds) {
                visualChanged = true;
            }

            if (visualChanged) {
                InvalidateRect(window, nullptr, FALSE);
            }
        }
        return 0;
    case WM_KEYDOWN:
        // ESC는 추후 일시정지 기능에 사용할 예정이므로 현재는 무시한다.
        if (wParam == VK_ESCAPE) {
            return 0;
        }
        return DefWindowProc(window, message, wParam, lParam);
    case WM_DESTROY:
        SavePlayerData();
        KillTimer(window, kAnimationTimerId);
        ReleaseBackBuffer();
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(window, message, wParam, lParam);
    }
}

} // 이름 없는 네임스페이스

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand) {
    for (double& scale : gPngSocketScales) {
        scale = 1.0;
    }
    gRandomState ^= static_cast<unsigned int>(GetTickCount());
    gAssetsDirectory = FindAssetsDirectory();
    LoadPlayerSave();
    if (gAssetsDirectory.empty() || !LoadNarrationData()) {
        MessageBox(
            nullptr,
            L"실행 파일 또는 상위 폴더에서 assets\\narration.json 파일을 "
            L"찾지 못했습니다.",
            L"나레이션 데이터 오류",
            MB_OK | MB_ICONERROR);
        return 1;
    }

    Gdiplus::GdiplusStartupInput gdiplusInput;
    if (Gdiplus::GdiplusStartup(
            &gGdiplusToken, &gdiplusInput, nullptr) != Gdiplus::Ok) {
        return 1;
    }
    LoadMaterialImages();
    LoadMoneyImages();
    LoadNpcImages();

    WNDCLASSEX windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = instance;
    windowClass.hIcon = static_cast<HICON>(LoadImage(
        instance,
        MAKEINTRESOURCE(101),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXICON),
        GetSystemMetrics(SM_CYICON),
        LR_DEFAULTCOLOR));
    windowClass.hIconSm = static_cast<HICON>(LoadImage(
        instance,
        MAKEINTRESOURCE(101),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        LR_DEFAULTCOLOR));
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    windowClass.lpszClassName = kWindowClass;

    if (!RegisterClassEx(&windowClass)) {
        UnloadNpcImages();
        UnloadMoneyImages();
        UnloadMaterialImages();
        Gdiplus::GdiplusShutdown(gGdiplusToken);
        return 1;
    }

    constexpr DWORD windowStyle = WS_OVERLAPPEDWINDOW;
    RECT windowRect{0, 0, kDesignWidth, kDesignHeight};
    AdjustWindowRectEx(&windowRect, windowStyle, FALSE, 0);
    const int windowWidth = windowRect.right - windowRect.left;
    const int windowHeight = windowRect.bottom - windowRect.top;
    const int windowX = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
    const int windowY = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;

    HWND window = CreateWindowEx(
        0, kWindowClass, kWindowTitle, windowStyle,
        windowX, windowY,
        windowWidth,
        windowHeight,
        nullptr, nullptr, instance, nullptr);

    if (!window) {
        UnloadNpcImages();
        UnloadMoneyImages();
        UnloadMaterialImages();
        Gdiplus::GdiplusShutdown(gGdiplusToken);
        return 1;
    }

    if (SetTimer(
            window,
            kAnimationTimerId,
            kAnimationFrameMilliseconds,
            nullptr) == 0) {
        MessageBox(
            window,
            L"애니메이션 타이머를 시작하지 못했습니다.",
            L"초기화 오류",
            MB_OK | MB_ICONERROR);
        DestroyWindow(window);
        UnloadNpcImages();
        UnloadMoneyImages();
        UnloadMaterialImages();
        Gdiplus::GdiplusShutdown(gGdiplusToken);
        return 1;
    }
    InitializeSoundSystem(window);
    // 이미지와 MP3 준비 시간이 페이드 시간에 포함되지 않도록 실제 창을
    // 표시하기 직전에 타이틀 애니메이션 기준 시각을 기록한다.
    gTitleStartTime = GetTickCount64();
    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message{};
    while (GetMessage(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    ShutdownSoundSystem();
    UnloadNpcImages();
    UnloadMoneyImages();
    UnloadMaterialImages();
    Gdiplus::GdiplusShutdown(gGdiplusToken);
    return static_cast<int>(message.wParam);
}
