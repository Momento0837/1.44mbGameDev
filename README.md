# 1.44MB Game Dev

외부 이미지나 게임 엔진 없이 Windows GDI로 그리는 소형 C++ 게임 프로젝트입니다.

현재 화면 구성:

- 기본 클라이언트 크기: 800 x 600
- 중앙 플레이 영역: 400 x 400
- 플레이 영역 바깥: 검정 레터박스
- 장면: 하늘, 유리창, 벽, 제작대, 재료통, 체크 바닥, 보도 경계, 도로 순으로 구성
- 바닥: `#A8CBA9`, `#F2FBF1`을 사용한 깊이 3칸의 1점 투시 체크 격자
- 바닥 가로 폭: 시작선은 플레이 영역 전체를 채우고 하단에서 112%까지 완만하게 확장
- 바닥 세로 배율: `kFloorHeightScale` 값으로 시작 위치를 유지한 채 조절
- 바닥 안티앨리어싱: 3배 해상도 슈퍼샘플링 후 고품질 축소
- 기본 소실점: 플레이 영역 정중앙 `(200, 200)`
- 상단 암막: 장면과 레터박스 위에 놓이는 반투명 레이어
- 재료 이미지 소켓: 기존 회색 오브젝트 14개 위에 가로 7개, 세로 2줄로 배치
- 재료 이미지: `assets/materials/material_01.png`부터 `material_14.png`까지 서로 다른 PNG8 파일 사용
- 재료 이미지 원근: 회색 오브젝트의 사다리꼴 면에 맞춰 PNG를 줄 단위로 투영
- 창 크기 변경 시 800 x 600 기준 비율을 유지하며 전체 화면이 확대/축소됨

## 빌드

Visual Studio의 **C++를 사용한 데스크톱 개발** 구성과 CMake가 필요합니다.

```powershell
cmake -S . -B build
cmake --build build --config Release
```

실행 파일은 일반적으로 `build/Release/OnePointFourFourMBGame.exe`에 생성됩니다.
방향키로 소실점을 움직일 수 있고 `Esc` 키로 종료할 수 있습니다.

기본 소실점을 코드에서 바꾸려면 `src/main.cpp`의
`kDefaultVanishingPointX`, `kDefaultVanishingPointY` 값을 수정하면 됩니다.
