//#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>

#include <algorithm>
#include <cmath>
#include <cwchar>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

constexpr wchar_t kWindowClass[] = L"OnePointFourFourMBGameWindow";
constexpr wchar_t kWindowTitle[] = L"1.44MB Game";

constexpr int kDesignWidth = 800;
constexpr int kDesignHeight = 600;
constexpr int kSupersampleScale = 3;
constexpr int kPlayAreaX = 200;
constexpr int kPlayAreaY = 100;
constexpr int kPlayAreaSize = 400;
constexpr int kFloorColumns = 8;
constexpr int kFloorRows = 3;
constexpr double kFloorStartDepth = 0.07;
constexpr double kFloorWidthScale = 1.12;
constexpr double kFloorHeightScale = 0.30;
constexpr int kDefaultVanishingPointX = kPlayAreaSize / 2;
constexpr int kDefaultVanishingPointY = kPlayAreaSize / 2;

// 재료통들의 직사각형 배치와 여백을 조절하는 설정값이다.
constexpr int kMaterialBinTop = 294;
constexpr int kMaterialBinFirstRowBottom = 342;
constexpr int kMaterialBinRowGap = 9;
constexpr int kMaterialBinColumnGap = 9;
constexpr int kMaterialBinBorderWidth = 2;
constexpr int kPngSocketSize = 18;
constexpr double kPngSocketHoverScale = 1.05;
constexpr double kPngSocketEasingSpeed = 0.20;
constexpr int kMaterialBinVerticalOffset = 5;

// 테이블 상호작용 영역과 이징 애니메이션 설정값이다.
constexpr int kTableHoverTop = 275;
constexpr int kTableHoverBottom = 400;
constexpr double kTableLiftDistance = 10.0;
constexpr double kTableEasingSpeed = 0.18;
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
constexpr int kMaterialCloneSize = 24;
constexpr int kMaximumMaterialClones = 64;
constexpr int kResetButtonWidth = 50;
constexpr int kResetButtonHeight = 30;
constexpr int kResetButtonMargin = 10;
constexpr int kResetButtonRadius = 8;
constexpr double kResetButtonFadeSpeed = 0.08;
constexpr int kNarrationBoxX = 10;
constexpr int kNarrationBoxY = 10;
constexpr int kNarrationBoxWidth = kDesignWidth - 20;
constexpr int kNarrationBoxHeight = 160;
constexpr ULONGLONG kNarrationCharacterIntervalMilliseconds = 80;
constexpr UINT_PTR kAnimationTimerId = 1;
constexpr UINT kAnimationFrameMilliseconds = 16;

// 타이틀 화면의 배치와 화면 전환 애니메이션 설정값이다.
constexpr int kTitlePlaceholderWidth = 500;
constexpr int kTitlePlaceholderHeight = 300;
constexpr int kTitlePlaceholderY = 20;
constexpr BYTE kTitlePlaceholderOpacity = 26;
constexpr double kTitleBreathingScale = 1.025;
constexpr double kTitleBreathingSeconds = 3.2;
constexpr double kTitleAppearanceDelaySeconds = 0.5;
constexpr double kTitleFadeInSeconds = 0.8;
constexpr double kScreenFadeSeconds = 0.5;
constexpr int kTitleButtonFontHeight = 30;
constexpr int kTitleButtonPadding = 5;
constexpr double kTitleButtonHoverScale = 1.05;
constexpr double kTitleButtonEasingSpeed = 0.20;
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

constexpr COLORREF kLetterboxColor = RGB(0x00, 0x00, 0x00);
constexpr COLORREF kTileGreen = RGB(0xa8, 0xcb, 0xa9);
constexpr COLORREF kTileWhite = RGB(0xf2, 0xfb, 0xf1);
constexpr COLORREF kSkyColor = RGB(0x77, 0xcb, 0xe8);
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
constexpr COLORREF kClonePlaceholderColor = RGB(0xff, 0xea, 0x00);
constexpr COLORREF kTitlePlaceholderColor = RGB(0x77, 0xcb, 0xe8);
constexpr COLORREF kExitDialogColor = RGB(0x2a, 0x2a, 0x2a);
constexpr COLORREF kExitYesButtonColor = RGB(0x94, 0xc2, 0x93);
constexpr COLORREF kExitNoButtonColor = RGB(0xd9, 0x9a, 0x9a);

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

// 소실점 좌표는 플레이 영역의 왼쪽 위를 (0, 0)으로 삼는다.
POINT gVanishingPoint{kDefaultVanishingPointX, kDefaultVanishingPointY};
POINT gMouseDesignPosition{};
bool gHasMousePosition = false;
bool gIsTrackingMouse = false;

enum class ScreenState {
    Title,
    TitleFadingOut,
    GameFadingIn,
    Game
};
ScreenState gScreenState = ScreenState::Title;
ULONGLONG gTitleStartTime = 0;
ULONGLONG gScreenTransitionStartTime = 0;
int gHoveredTitleButton = -1;
double gTitleButtonScales[3]{1.0, 1.0, 1.0};
RECT gTitleButtonRects[3]{};
bool gIsExitDialogVisible = false;

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
};
MaterialClone gMaterialClones[kMaximumMaterialClones]{};
int gMaterialCloneCount = 0;
unsigned int gRandomState = 0x144u;
double gResetButtonOpacity = 0.0;
bool gIsNarrationActive = false;
bool gIsNarrationTyping = false;
ULONGLONG gNarrationStartTime = 0;
size_t gNarrationVisibleLength = 0;
struct DialogueTree {
    std::wstring id;
    std::vector<std::vector<std::wstring>> steps;
};
std::vector<std::wstring> gNarrationNames;
std::vector<DialogueTree> gDialogueTrees;
std::wstring gCurrentNarrationName;
std::wstring gCurrentNarrationText;
int gCurrentDialogueTree = -1;
size_t gCurrentDialogueStep = 0;
std::wstring gAssetsDirectory;
ULONG_PTR gGdiplusToken = 0;
Gdiplus::Image* gMaterialImages[kMaterialBinCount]{};

std::wstring GetExecutableDirectory() {
    wchar_t path[MAX_PATH]{};
    GetModuleFileName(nullptr, path, MAX_PATH);
    std::wstring directory(path);
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

    const size_t treesStart = FindJsonProperty(json, "dialogueTrees");
    const size_t treesEnd = FindMatchingJsonDelimiter(json, treesStart, '[', ']');
    if (treesStart == std::string::npos || treesEnd == std::string::npos) {
        return false;
    }

    size_t position = treesStart + 1;
    while (position < treesEnd) {
        position = json.find('{', position);
        if (position == std::string::npos || position >= treesEnd) {
            break;
        }
        const size_t objectEnd = FindMatchingJsonDelimiter(json, position, '{', '}');
        if (objectEnd == std::string::npos || objectEnd > treesEnd) {
            return false;
        }
        const std::string treeJson = json.substr(position, objectEnd - position + 1);

        size_t idPosition = FindJsonProperty(treeJson, "id");
        std::string id;
        if (!ParseJsonString(treeJson, idPosition, id)) {
            return false;
        }
        std::vector<std::string> sequence;
        if (!ParseJsonStringArray(
                treeJson, FindJsonProperty(treeJson, "sequence"), sequence)) {
            return false;
        }

        DialogueTree tree;
        tree.id = Utf8ToWide(id);
        for (const std::string& stage : sequence) {
            std::vector<std::string> lines;
            if (!ParseJsonStringArray(
                    treeJson, FindJsonProperty(treeJson, stage), lines)
                || lines.empty()) {
                return false;
            }
            std::vector<std::wstring> convertedLines;
            for (const std::string& line : lines) {
                convertedLines.push_back(Utf8ToWide(line));
            }
            tree.steps.push_back(convertedLines);
        }
        if (!tree.steps.empty()) {
            gDialogueTrees.push_back(tree);
        }
        position = objectEnd + 1;
    }
    return !gNarrationNames.empty() && !gDialogueTrees.empty();
}

void LoadMaterialImages() {
    for (int index = 0; index < kMaterialBinCount; ++index) {
        const std::wstring imagePath = gAssetsDirectory + kMaterialImagePaths[index];
        Gdiplus::Image* image = Gdiplus::Image::FromFile(imagePath.c_str(), FALSE);
        if (image != nullptr && image->GetLastStatus() == Gdiplus::Ok) {
            gMaterialImages[index] = image;
        } else {
            delete image;
        }
    }
}

void UnloadMaterialImages() {
    for (Gdiplus::Image*& image : gMaterialImages) {
        delete image;
        image = nullptr;
    }
}

struct Layout {
    double scale;
    int offsetX;
    int offsetY;
};

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
    HBITMAP layerBitmap = CreateCompatibleBitmap(dc, width, height);
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
    return minimum + static_cast<int>(
        NextRandom() % static_cast<unsigned int>(maximum - minimum + 1));
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
        graphics.DrawImage(
            gMaterialImages[materialIndex],
            Gdiplus::Rect(
                area.left,
                area.top,
                area.right - area.left,
                area.bottom - area.top));
    }
}

void DrawPngSocket(HDC dc, int binIndex, const RECT& socket) {
    // PNG8 이미지는 원근 변형 없이 정방형 소켓 전체에 그대로 그린다.
    FillSolid(dc, socket, kTileWhite);
    DrawMaterialImage(dc, binIndex, socket);
    const HBRUSH frameBrush = CreateSolidBrush(kShelfDarkColor);
    FrameRect(dc, &socket, frameBrush);
    DeleteObject(frameBrush);
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
            kMaterialCloneSize,
            kMaterialCloneSize);
        // PNG 복제 위치를 확인할 수 있도록 임시 노란색 박스를 먼저 그린다.
        FillSolid(dc, imageArea, kClonePlaceholderColor);
        DrawMaterialImage(dc, clone.materialIndex, imageArea);
    }
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
    clone.x = RandomRange(
        boardLeft + padding,
        boardLeft + kCuttingBoardWidth - padding - kMaterialCloneSize);
    clone.y = RandomRange(
        boardTop + padding,
        boardTop + kCuttingBoardHeight - padding - kMaterialCloneSize);
}

RECT ResetButtonRect(const Layout& layout) {
    return LogicalRect(
        layout,
        kPlayAreaX + kPlayAreaSize - kResetButtonMargin - kResetButtonWidth,
        kPlayAreaY + kPlayAreaSize - kResetButtonMargin - kResetButtonHeight,
        kResetButtonWidth,
        kResetButtonHeight);
}

void DrawResetButton(HDC dc, const Layout& layout) {
    const RECT button = ResetButtonRect(layout);
    const int radius = static_cast<int>(
        std::lround(kResetButtonRadius * layout.scale));
    const COLORREF fadedColor = BlendColor(
        kPlatformColor, kResetButtonColor, gResetButtonOpacity);
    FillRoundedRect(dc, button, radius, fadedColor);
}

void DrawInterior(HDC dc, const Layout& layout) {
    // 가장 아래 레이어인 하늘색을 플레이 영역 전체에 먼저 칠한다.
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY, kPlayAreaSize, kPlayAreaSize),
        kSkyColor);

    // 좌우 유리창은 중앙 창보다 한 단계 밝은 하늘색을 사용한다.
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY, 100, 105), kGlassColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX + 300, kPlayAreaY, 100, 105), kGlassColor);

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

void DrawMousePosition(HDC dc, const Layout& layout) {
    if (!gHasMousePosition) {
        return;
    }

    const int playX = gMouseDesignPosition.x - kPlayAreaX;
    const int playY = gMouseDesignPosition.y - kPlayAreaY;
    wchar_t text[128]{};
    swprintf_s(
        text,
        _countof(text),
        L"화면 X:%ld Y:%ld  |  플레이 X:%d Y:%d",
        gMouseDesignPosition.x,
        gMouseDesignPosition.y,
        playX,
        playY);

    const int fontHeight = (std::max)(
        1, static_cast<int>(std::lround(15.0 * layout.scale)));
    const HFONT font = CreateFont(
        -fontHeight,
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
        L"맑은 고딕");
    const HGDIOBJ oldFont = SelectObject(dc, font);
    const int oldBackgroundMode = SetBkMode(dc, TRANSPARENT);
    const COLORREF oldTextColor = SetTextColor(dc, RGB(0xff, 0xff, 0xff));
    const POINT textPosition = LogicalPoint(layout, 10, kDesignHeight - 24);
    TextOut(
        dc,
        textPosition.x,
        textPosition.y,
        text,
        static_cast<int>(wcslen(text)));
    SetTextColor(dc, oldTextColor);
    SetBkMode(dc, oldBackgroundMode);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

void StartCurrentNarrationLine() {
    if (gCurrentDialogueTree < 0
        || gCurrentDialogueTree >= static_cast<int>(gDialogueTrees.size())) {
        return;
    }
    const DialogueTree& tree = gDialogueTrees[gCurrentDialogueTree];
    if (gCurrentDialogueStep >= tree.steps.size()
        || tree.steps[gCurrentDialogueStep].empty()) {
        return;
    }
    const std::vector<std::wstring>& lines = tree.steps[gCurrentDialogueStep];
    gCurrentNarrationText = lines[NextRandom() % lines.size()];
    gIsNarrationActive = true;
    gIsNarrationTyping = true;
    gNarrationStartTime = GetTickCount64();
    gNarrationVisibleLength = gCurrentNarrationText.empty() ? 0 : 1;
}

void StartRandomDialogueTree() {
    if (gNarrationNames.empty() || gDialogueTrees.empty()) {
        return;
    }
    gCurrentNarrationName =
        gNarrationNames[NextRandom() % gNarrationNames.size()];
    gCurrentDialogueTree = static_cast<int>(
        NextRandom() % gDialogueTrees.size());
    gCurrentDialogueStep = 0;
    StartCurrentNarrationLine();
}

void AdvanceNarration() {
    if (!gIsNarrationActive) {
        StartRandomDialogueTree();
        return;
    }
    if (gIsNarrationTyping) {
        gNarrationVisibleLength = gCurrentNarrationText.size();
        gIsNarrationTyping = false;
        return;
    }

    const DialogueTree& tree = gDialogueTrees[gCurrentDialogueTree];
    ++gCurrentDialogueStep;
    if (gCurrentDialogueStep < tree.steps.size()) {
        StartCurrentNarrationLine();
    } else {
        StartRandomDialogueTree();
    }
}

RECT NarrationBoxRect(const Layout& layout) {
    return LogicalRect(
        layout,
        kNarrationBoxX,
        kNarrationBoxY,
        kNarrationBoxWidth,
        kNarrationBoxHeight);
}

void DrawNarration(HDC dc, const Layout& layout) {
    if (!gIsNarrationActive) {
        return;
    }

    const int nameFontHeight = (std::max)(
        1, static_cast<int>(std::lround(18.0 * layout.scale)));
    const HFONT nameFont = CreateFont(
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
        L"맑은 고딕");
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
        1, static_cast<int>(std::lround(24.0 * layout.scale)));
    const HFONT dialogueFont = CreateFont(
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
        L"맑은 고딕");
    SelectObject(dc, dialogueFont);
    const POINT textPosition = LogicalPoint(
        layout,
        kNarrationBoxX + 24,
        kNarrationBoxY + 62);
    TextOut(
        dc,
        textPosition.x,
        textPosition.y,
        gCurrentNarrationText.c_str(),
        static_cast<int>(gNarrationVisibleLength));
    SetTextColor(dc, oldTextColor);
    SetBkMode(dc, oldBackgroundMode);
    SelectObject(dc, oldFont);
    DeleteObject(dialogueFont);
    DeleteObject(nameFont);
}

double SmoothStep(double progress) {
    const double clamped = (std::max)(0.0, (std::min)(1.0, progress));
    return clamped * clamped * (3.0 - 2.0 * clamped);
}

HFONT CreateUiFont(
    const Layout& layout,
    double logicalHeight,
    int weight = FW_NORMAL) {
    const int fontHeight = (std::max)(
        1,
        static_cast<int>(std::lround(logicalHeight * layout.scale)));
    return CreateFont(
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
        L"맑은 고딕");
}

void DrawCenteredText(
    HDC dc,
    const Layout& layout,
    const RECT& logicalArea,
    const wchar_t* text,
    double fontHeight,
    COLORREF color,
    int weight = FW_NORMAL) {
    const HFONT font = CreateUiFont(layout, fontHeight, weight);
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
    const double titleScale = 1.0
        + (kTitleBreathingScale - 1.0) * breathingAmount;
    const int titleWidth = static_cast<int>(
        std::lround(kTitlePlaceholderWidth * titleScale));
    const int titleHeight = static_cast<int>(
        std::lround(kTitlePlaceholderHeight * titleScale));
    const int titleCenterX = kDesignWidth / 2;
    const int titleCenterY = kTitlePlaceholderY
        + kTitlePlaceholderHeight / 2;
    const RECT titleArea = LogicalRect(
        layout,
        titleCenterX - titleWidth / 2,
        titleCenterY - titleHeight / 2,
        titleWidth,
        titleHeight);

    // 실제 타이틀 PNG는 이 500x300 자리표시자 영역에 확대하여 출력한다.
    FillTranslucent(
        dc,
        titleArea,
        kTitlePlaceholderColor,
        kTitlePlaceholderOpacity);

    for (int index = 0; index < 3; ++index) {
        const double fontHeight = kTitleButtonFontHeight
            * gTitleButtonScales[index];
        const HFONT font = CreateUiFont(layout, fontHeight);
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
            RGB(0xff, 0xff, 0xff));
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

void StartGameScreenTransition() {
    if (!IsTitleInteractive() || gIsExitDialogVisible) {
        return;
    }
    gScreenState = ScreenState::TitleFadingOut;
    gScreenTransitionStartTime = GetTickCount64();
    gHoveredTitleButton = -1;
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
        && !gIsCookingTransitionRunning;
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

    // 최상단 암막은 플레이 영역과 레터박스 양쪽에 반투명하게 걸친다.
    FillTranslucent(dc, NarrationBoxRect(layout), RGB(0x2a, 0x2a, 0x2a), 222);
    DrawNarration(dc, layout);

    // 기능 배치를 위한 화면 좌표와 플레이 영역 좌표를 좌측 하단에 표시한다.
    DrawMousePosition(dc, layout);
}

void DrawApplication(HDC dc, const RECT& client) {
    if (gScreenState == ScreenState::Title
        || gScreenState == ScreenState::TitleFadingOut) {
        DrawTitleScreen(dc, client);
        return;
    }

    DrawGame(dc, client);
    if (gScreenState == ScreenState::GameFadingIn) {
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
        const int bufferWidth = width * kSupersampleScale;
        const int bufferHeight = height * kSupersampleScale;
        HDC bufferDc = CreateCompatibleDC(windowDc);
        HBITMAP bufferBitmap = CreateCompatibleBitmap(
            windowDc, bufferWidth, bufferHeight);
        HGDIOBJ oldBitmap = SelectObject(bufferDc, bufferBitmap);

        const RECT bufferClient{0, 0, bufferWidth, bufferHeight};
        DrawApplication(bufferDc, bufferClient);

        // 고품질 축소 필터로 체크무늬 바닥의 가장자리를 부드럽게 합성한다.
        SetStretchBltMode(windowDc, HALFTONE);
        SetBrushOrgEx(windowDc, 0, 0, nullptr);
        StretchBlt(
            windowDc,
            0, 0, width, height,
            bufferDc,
            0, 0, bufferWidth, bufferHeight,
            SRCCOPY);

        SelectObject(bufferDc, oldBitmap);
        DeleteObject(bufferBitmap);
        DeleteDC(bufferDc);
    }

    EndPaint(window, &paint);
}

LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
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
        if (gScreenState == ScreenState::Title) {
            gHoveredTitleButton = HitTestTitleButton(
                gMouseDesignPosition.x,
                gMouseDesignPosition.y);
            gIsTableHovered = false;
        } else if (gScreenState == ScreenState::Game) {
            const int playX = gMouseDesignPosition.x - kPlayAreaX;
            const int playY = gMouseDesignPosition.y - kPlayAreaY;
            const bool isInsidePlayArea = playX >= 0
                && playX <= kPlayAreaSize
                && playY >= 0
                && playY <= kPlayAreaSize;
            gIsTableHovered = !gIsCookingTransitionRunning
                && gCookingState == CookingState::NonCooking
                && isInsidePlayArea
                && playY >= kTableHoverTop
                && playY <= kTableHoverBottom;
            gHoveredTitleButton = -1;
        } else {
            gHoveredTitleButton = -1;
            gIsTableHovered = false;
        }

        if (!gIsTrackingMouse) {
            TRACKMOUSEEVENT tracking{};
            tracking.cbSize = sizeof(tracking);
            tracking.dwFlags = TME_LEAVE;
            tracking.hwndTrack = window;
            TrackMouseEvent(&tracking);
            gIsTrackingMouse = true;
        }
        InvalidateRect(window, nullptr, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN: {
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
                StartGameScreenTransition();
                InvalidateRect(window, nullptr, FALSE);
            } else if (clickedTitleButton == 1) {
                // 옵션 기능은 사운드 시스템을 추가할 때 이 위치에 연결한다.
            } else if (clickedTitleButton == 2) {
                gIsExitDialogVisible = true;
                gHoveredTitleButton = -1;
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
        if (PtInRect(&narrationBox, mousePoint)) {
            AdvanceNarration();
            InvalidateRect(window, nullptr, FALSE);
        } else if (IsCookingStateActive()
            && PtInRect(&resetButton, mousePoint)) {
            gMaterialCloneCount = 0;
            StartCookingTransition(CookingState::NonCooking);
            InvalidateRect(window, nullptr, FALSE);
        } else if (!gIsCookingTransitionRunning
            && gCookingState == CookingState::NonCooking
            && gIsTableHovered) {
            StartCookingTransition(CookingState::Cooking);
        } else if (!gIsCookingTransitionRunning
            && gCookingState == CookingState::Cooking
            && clickedSocket >= 0) {
            AddMaterialClone(clickedSocket);
            InvalidateRect(window, nullptr, FALSE);
        } else if (!gIsCookingTransitionRunning
            && gCookingState == CookingState::Cooking
            && isInsidePlayArea
            && playY <= kCookingTableY) {
            StartCookingTransition(CookingState::NonCooking);
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        gIsTrackingMouse = false;
        gIsTableHovered = false;
        gHoveredPngSocket = -1;
        gHoveredTitleButton = -1;
        InvalidateRect(window, nullptr, FALSE);
        return 0;
    case WM_TIMER:
        if (wParam == kAnimationTimerId) {
            bool visualChanged = false;
            const ULONGLONG now = GetTickCount64();

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
                    gScreenState = ScreenState::GameFadingIn;
                    gScreenTransitionStartTime = now;
                    gIsTableHovered = false;
                    gHoveredPngSocket = -1;
                }
            } else if (gScreenState == ScreenState::GameFadingIn) {
                const double elapsed = (
                    now - gScreenTransitionStartTime) / 1000.0;
                visualChanged = true;
                if (elapsed >= kScreenFadeSeconds) {
                    gScreenState = ScreenState::Game;
                }
            }

            if (gScreenState == ScreenState::Game) {
                if (gIsNarrationActive && gIsNarrationTyping) {
                    const ULONGLONG elapsed = now - gNarrationStartTime;
                    const size_t nextVisibleLength = (std::min)(
                        gCurrentNarrationText.size(),
                        static_cast<size_t>(
                            elapsed
                                / kNarrationCharacterIntervalMilliseconds
                                + 1));
                    if (nextVisibleLength != gNarrationVisibleLength) {
                        gNarrationVisibleLength = nextVisibleLength;
                        visualChanged = true;
                    }
                    if (gNarrationVisibleLength
                        >= gCurrentNarrationText.size()) {
                        gIsNarrationTyping = false;
                    }
                }

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

                gHoveredPngSocket = HitTestPngSocket();
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
        KillTimer(window, kAnimationTimerId);
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

    WNDCLASSEX windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    windowClass.lpszClassName = kWindowClass;

    if (!RegisterClassEx(&windowClass)) {
        UnloadMaterialImages();
        Gdiplus::GdiplusShutdown(gGdiplusToken);
        return 1;
    }

    constexpr DWORD windowStyle = WS_OVERLAPPEDWINDOW;
    RECT windowRect{0, 0, kDesignWidth, kDesignHeight};
    AdjustWindowRectEx(&windowRect, windowStyle, FALSE, 0);

    HWND window = CreateWindowEx(
        0, kWindowClass, kWindowTitle, windowStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, instance, nullptr);

    if (!window) {
        UnloadMaterialImages();
        Gdiplus::GdiplusShutdown(gGdiplusToken);
        return 1;
    }

    gTitleStartTime = GetTickCount64();
    ShowWindow(window, showCommand);
    UpdateWindow(window);
    SetTimer(
        window,
        kAnimationTimerId,
        kAnimationFrameMilliseconds,
        nullptr);

    MSG message{};
    while (GetMessage(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnloadMaterialImages();
    Gdiplus::GdiplusShutdown(gGdiplusToken);
    return static_cast<int>(message.wParam);
}
