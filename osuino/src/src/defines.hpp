// MARK: - Environment
#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 480

// 랜더링시 노트를 매 프레임마다 그립니다. (아두이노에서는 절대 금지)
#define _RENDER_EVERY_FRAME_
// 노트가 미스된경우 콘솔에 출력
#undef _PRINT_NOTE_MISSED_
// 노트 인덱스를 노트위에 작성합니다. _RENDER_EVERY_FRAME_가 활성화 되어야 합니다.
#define _DRAW_NOTE_INDEX_
// FPS를 화면에 그립니다.
#undef _DRAW_FPS_
// info 수준의 로그를 출력합니다.
#define _PRINTF_INFO_
// 맵 로딩 로그를 출력합니다.
#define _PRINTF_MAP_LOAD_INFO_
// 노트 판정 시간을 출력합니다.
#undef _PRINT_EARLY_LATE_MS_
// 파일 시스템 로그를 출력합니다.
#undef _PRINTF_FS_INFO_
// 파싱 정보를 출력합니다.
#undef _PRINT_PARSE_INFO_
// 죽지 않습니다.
#undef _NO_DIE_
// 모든 동작 사이 애니메이션을 끕니다, 아두이노에서는 필수 입니다.
#define _DO_NOT_ANIMATE_
// API를 모킹합니다.
#define _API_MOCK_
// 퍼펙트 플레이를 달성했을떄의 점수
#define FULLSCORE 1000000.0

#ifdef _IS_ARDUINO_
#undef _RENDER_EVERY_FRAME_
#undef _DRAW_NOTE_INDEX_
#define _DO_NOT_ANIMATE_
#endif

// MARK: - Constants

// 맵 크기
#define __SELECT_MAP_HEIGHT__ 50
// 맵 선택창 간격
#define __SELECT_MAP_GAP__ 10

// 정확도 Y 좌표
#define _ACCURACY_BASE_Y_ CANVAS_HEIGHT / 4 * 3 - 10
// 콤보 Y 좌표
#define _COMBO_BASE_Y_ CANVAS_HEIGHT / 2 - 100
// 롱노트 콤보 스텍 주기
#define _LONG_NOTE_COMBO_STACK_MS_ 250

#define _REPEAT_POST_DELAY_ 175
#define _REPEAT_CYCLE_ 50

const int __SELECT_MAP_COUNT__ = (CANVAS_HEIGHT - __SELECT_MAP_GAP__) / (__SELECT_MAP_HEIGHT__ + __SELECT_MAP_GAP__);
const int __SELECT_PREV_MAPS__ = (__SELECT_MAP_COUNT__ - 1) / 2;
const int __SELECT_NEXT_MAPS__ = (__SELECT_MAP_COUNT__ - __SELECT_PREV_MAPS__ - 1);
const int __SELECT_MAP_START_Y__ = 10;

float _NOTE_SPEED_ = 600.0;

#define _JUDGE_MAX_100_ 41
#define _JUDGE_MAX_90_ 54
#define _JUDGE_MAX_80_ 75
#define _JUDGE_MAX_70_ 96
#define _JUDGE_MAX_60_ 120
#define _JUDGE_MAX_50_ 129
#define _JUDGE_MAX_40_ 138
#define _JUDGE_MAX_30_ 147
#define _JUDGE_MAX_20_ 159
#define _JUDGE_MAX_10_ 177

float _NOTE_START_Y_ = 0.0;
float _NOTE_END_Y_ = CANVAS_HEIGHT - 80.0;
float _NOTE_Y_RANGE_ = _NOTE_END_Y_ - _NOTE_START_Y_;
float _NOTE_PIXEL_PER_MS_ = _NOTE_Y_RANGE_ / _NOTE_SPEED_;

// single note 높이
float _SINGLE_NOTE_HEIGHT_ = 20;

// late 노트 인식 범위
float _NOTE_LATE_ALLOWANCE_ = _JUDGE_MAX_30_;
// 노트를 얼마나 내려오기 전까지 무시할 것인가
float _NOTE_IGNORE_DELTA_ = _NOTE_Y_RANGE_ / 2 * _NOTE_PIXEL_PER_MS_;

#define _JUDGE_MAX_0_ CANVAS_HEIGHT / 2 * _NOTE_PIXEL_PER_MS_

float _HEALTH_BY_SCORE_[] = {
    -7,
    0.5,  // 10,
    0.71, // 20
    0.8,  // 30
    0.88, // 40
    0.95, // 50
    1,    // 60
    1.05, // 70
    1.15, // 80
    1.3,  // 90
    1.5,  // 100
};