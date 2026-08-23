//#define NOMINMAX
#include <windows.h>
#include <gdiplus.h>

#include <algorithm>
#include <cmath>
#include <string>

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

// 도로 보호블록의 여백과 원근 간격을 조절하는 설정값이다.
constexpr int kRoadHorizontalGapY = 342;
constexpr int kRoadHorizontalGapHeight = 9;
constexpr double kRoadJointTopSpacing = 54.0;
constexpr double kRoadJointBottomSpacing = 58.0;
constexpr double kRoadJointTopHalfWidth = 4.0;
constexpr double kRoadJointBottomHalfWidth = 7.0;

// PNG 소켓이 회색 블록 안쪽에서 차지하는 여백과 높이이다.
constexpr double kMaterialSocketInset = 0.28;
constexpr int kMaterialSocketTopMargin = 6;
constexpr int kMaterialSocketHeight = 18;

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
constexpr COLORREF kRoadColor = RGB(0x8d, 0x8d, 0x8d);
constexpr COLORREF kRoadLineColor = RGB(0x50, 0x3d, 0x2a);
constexpr COLORREF kCurbColor = RGB(0x88, 0x7b, 0x6e);
constexpr COLORREF kPlatformColor = RGB(0x5c, 0x3e, 0x1f);

constexpr int kMaterialBinColumns = 7;
constexpr int kMaterialBinRows = 2;
constexpr int kMaterialBinCount = kMaterialBinColumns * kMaterialBinRows;
constexpr wchar_t kMaterialImagePaths[kMaterialBinCount][64] = {
    L"assets\\materials\\material_01.png",
    L"assets\\materials\\material_02.png",
    L"assets\\materials\\material_03.png",
    L"assets\\materials\\material_04.png",
    L"assets\\materials\\material_05.png",
    L"assets\\materials\\material_06.png",
    L"assets\\materials\\material_07.png",
    L"assets\\materials\\material_08.png",
    L"assets\\materials\\material_09.png",
    L"assets\\materials\\material_10.png",
    L"assets\\materials\\material_11.png",
    L"assets\\materials\\material_12.png",
    L"assets\\materials\\material_13.png",
    L"assets\\materials\\material_14.png"
};

// 소실점 좌표는 플레이 영역의 왼쪽 위를 (0, 0)으로 삼는다.
POINT gVanishingPoint{kDefaultVanishingPointX, kDefaultVanishingPointY};
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

void LoadMaterialImages() {
    const std::wstring executableDirectory = GetExecutableDirectory();
    for (int index = 0; index < kMaterialBinCount; ++index) {
        const std::wstring imagePath = executableDirectory + kMaterialImagePaths[index];
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

void DrawRoad(HDC dc, const Layout& layout) {
    constexpr int roadTop = 294;
    const RECT road = LogicalRect(
        layout, kPlayAreaX, kPlayAreaY + roadTop,
        kPlayAreaSize, kPlayAreaSize - roadTop);
    FillSolid(dc, road, kRoadColor);

    // 도로의 가로 이음매는 화면 아래로 갈수록 간격이 넓어진다.
    FillSolid(dc, LogicalRect(
        layout,
        kPlayAreaX,
        kPlayAreaY + kRoadHorizontalGapY,
        kPlayAreaSize,
        kRoadHorizontalGapHeight),
        kRoadLineColor);

    // 세로 이음매는 중앙을 비운 반 칸 오프셋으로 여섯 개를 배치한다.
    for (int line = -3; line < 3; ++line) {
        const double lineOffset = line + 0.5;
        const double bottomX = kPlayAreaX + kPlayAreaSize * 0.5
            + lineOffset * kRoadJointBottomSpacing;
        const double topX = kPlayAreaX + kPlayAreaSize * 0.5
            + lineOffset * kRoadJointTopSpacing;
        POINT joint[] = {
            LogicalPoint(layout, topX - kRoadJointTopHalfWidth, kPlayAreaY + roadTop),
            LogicalPoint(layout, topX + kRoadJointTopHalfWidth, kPlayAreaY + roadTop),
            LogicalPoint(layout, bottomX + kRoadJointBottomHalfWidth, kPlayAreaY + kPlayAreaSize),
            LogicalPoint(layout, bottomX - kRoadJointBottomHalfWidth, kPlayAreaY + kPlayAreaSize)
        };
        FillPolygon(dc, joint, 4, kRoadLineColor);
    }
}

double RoadBoundaryX(int boundary, double y) {
    constexpr int roadTop = 294;
    if (boundary == 0) {
        return kPlayAreaX;
    }
    if (boundary == kMaterialBinColumns) {
        return kPlayAreaX + kPlayAreaSize;
    }

    const double depth = (y - roadTop) / (kPlayAreaSize - roadTop);
    const double lineOffset = boundary - 3.5;
    const double topX = kPlayAreaX + kPlayAreaSize * 0.5
        + lineOffset * kRoadJointTopSpacing;
    const double bottomX = kPlayAreaX + kPlayAreaSize * 0.5
        + lineOffset * kRoadJointBottomSpacing;
    return topX + (bottomX - topX) * depth;
}

void DrawMaterialImageSlot(HDC dc, int binIndex, POINT* socket) {
    // 재료 이미지 삽입 위치: PNG8 이미지를 회색 블록과 같은 사다리꼴로 투영한다.
    FillPolygon(dc, socket, 4, kTileWhite);
    if (binIndex >= 0
        && binIndex < kMaterialBinCount
        && gMaterialImages[binIndex] != nullptr) {
        Gdiplus::Graphics graphics(dc);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        const UINT sourceWidth = gMaterialImages[binIndex]->GetWidth();
        const UINT sourceHeight = gMaterialImages[binIndex]->GetHeight();

        // 한 줄씩 폭을 달리해 사각 PNG를 원근 사다리꼴로 변형한다.
        for (UINT sourceY = 0; sourceY < sourceHeight; ++sourceY) {
            const double ratio = (sourceY + 0.5) / sourceHeight;
            const double leftX = socket[0].x
                + (socket[3].x - socket[0].x) * ratio;
            const double rightX = socket[1].x
                + (socket[2].x - socket[1].x) * ratio;
            const double topY = socket[0].y;
            const double bottomY = socket[3].y;
            const double rowTop = topY
                + (bottomY - topY) * sourceY / sourceHeight;
            const double rowBottom = topY
                + (bottomY - topY) * (sourceY + 1) / sourceHeight;
            graphics.DrawImage(
                gMaterialImages[binIndex],
                Gdiplus::RectF(
                    static_cast<Gdiplus::REAL>(leftX),
                    static_cast<Gdiplus::REAL>(rowTop),
                    static_cast<Gdiplus::REAL>(rightX - leftX),
                    static_cast<Gdiplus::REAL>(rowBottom - rowTop + 0.5)),
                0,
                static_cast<Gdiplus::REAL>(sourceY),
                static_cast<Gdiplus::REAL>(sourceWidth),
                1.0f,
                Gdiplus::UnitPixel);
        }
    }

    POINT outline[] = {socket[0], socket[1], socket[2], socket[3], socket[0]};
    const HPEN framePen = CreatePen(PS_SOLID, 1, kShelfDarkColor);
    const HGDIOBJ oldPen = SelectObject(dc, framePen);
    Polyline(dc, outline, 5);
    SelectObject(dc, oldPen);
    DeleteObject(framePen);
}

void DrawRoadMaterialSockets(HDC dc, const Layout& layout) {
    constexpr int rowTops[kMaterialBinRows] = {294, 351};
    constexpr int rowBottoms[kMaterialBinRows] = {342, kPlayAreaSize};

    // 기존 회색 오브젝트 14개의 윗면 중앙에 이미지 소켓만 겹쳐 그린다.
    for (int row = 0; row < kMaterialBinRows; ++row) {
        const double socketTop = rowTops[row] + kMaterialSocketTopMargin;
        const double socketBottom = (std::min)(
            static_cast<double>(rowBottoms[row]),
            socketTop + kMaterialSocketHeight);
        for (int column = 0; column < kMaterialBinColumns; ++column) {
            const int binIndex = row * kMaterialBinColumns + column;
            const double topLeft = RoadBoundaryX(column, socketTop);
            const double topRight = RoadBoundaryX(column + 1, socketTop);
            const double bottomLeft = RoadBoundaryX(column, socketBottom);
            const double bottomRight = RoadBoundaryX(column + 1, socketBottom);
            double socketTopLeft = topLeft
                + (topRight - topLeft) * kMaterialSocketInset;
            double socketTopRight = topRight
                - (topRight - topLeft) * kMaterialSocketInset;
            double socketBottomLeft = bottomLeft
                + (bottomRight - bottomLeft) * kMaterialSocketInset;
            double socketBottomRight = bottomRight
                - (bottomRight - bottomLeft) * kMaterialSocketInset;

            POINT socket[] = {
                LogicalPoint(
                    layout,
                    socketTopLeft,
                    kPlayAreaY + socketTop),
                LogicalPoint(
                    layout,
                    socketTopRight,
                    kPlayAreaY + socketTop),
                LogicalPoint(
                    layout,
                    socketBottomRight,
                    kPlayAreaY + socketBottom),
                LogicalPoint(
                    layout,
                    socketBottomLeft,
                    kPlayAreaY + socketBottom)
            };
            DrawMaterialImageSlot(dc, binIndex, socket);
        }
    }
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

void DrawGame(HDC dc, const RECT& client) {
    FillSolid(dc, client, kLetterboxColor);

    if (client.right <= client.left || client.bottom <= client.top) {
        return;
    }

    const Layout layout = GetLayout(client);
    const RECT playArea = LogicalRect(
        layout, kPlayAreaX, kPlayAreaY, kPlayAreaSize, kPlayAreaSize);
    DrawInterior(dc, layout);

    // 넓어진 바닥이 플레이 영역 바깥의 레터박스를 침범하지 않도록 자른다.
    const int savedDc = SaveDC(dc);
    IntersectClipRect(
        dc, playArea.left, playArea.top, playArea.right, playArea.bottom);
    DrawPerspectiveFloor(dc, layout);
    RestoreDC(dc, savedDc);

    // 체크 바닥 아래에 보도 경계와 도로를 레이어 순서대로 겹친다.
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY + 268, kPlayAreaSize, 10),
        kCurbColor);
    FillSolid(dc, LogicalRect(
        layout, kPlayAreaX, kPlayAreaY + 278, kPlayAreaSize, 16),
        kPlatformColor);
    DrawRoad(dc, layout);
    DrawRoadMaterialSockets(dc, layout);

    // 최상단 암막은 플레이 영역과 레터박스 양쪽에 반투명하게 걸친다.
    FillTranslucent(
        dc,
        LogicalRect(layout, 10, 10, kDesignWidth - 20, 160),
        RGB(0x2a, 0x2a, 0x2a),
        222);
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
        DrawGame(bufferDc, bufferClient);

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
    case WM_KEYDOWN:
        // 방향키로 소실점을 옮기면 체크 바닥이 즉시 다시 계산된다.
        if (wParam == VK_LEFT) {
            gVanishingPoint.x = (std::max)(20L, gVanishingPoint.x - 4);
            InvalidateRect(window, nullptr, FALSE);
        } else if (wParam == VK_RIGHT) {
            gVanishingPoint.x = (std::min)(
                static_cast<LONG>(kPlayAreaSize - 20), gVanishingPoint.x + 4);
            InvalidateRect(window, nullptr, FALSE);
        } else if (wParam == VK_UP) {
            gVanishingPoint.y = (std::max)(20L, gVanishingPoint.y - 4);
            InvalidateRect(window, nullptr, FALSE);
        } else if (wParam == VK_DOWN) {
            gVanishingPoint.y = (std::min)(
                static_cast<LONG>(kPlayAreaSize - 80), gVanishingPoint.y + 4);
            InvalidateRect(window, nullptr, FALSE);
        }
        if (wParam == VK_ESCAPE) {
            DestroyWindow(window);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(window, message, wParam, lParam);
    }
}

} // 이름 없는 네임스페이스

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand) {
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

    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message{};
    while (GetMessage(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnloadMaterialImages();
    Gdiplus::GdiplusShutdown(gGdiplusToken);
    return static_cast<int>(message.wParam);
}
