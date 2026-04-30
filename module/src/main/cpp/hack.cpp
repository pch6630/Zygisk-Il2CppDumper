#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include <string>
#include <thread>
#include "zygisk.hpp"
#include "hack.h"
#include "xdl.h"

// 로그 태그 설정
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygisk-Hack", __VA_ARGS__)

// 덤프 함수 선언 (외부 라이브러리 사용 시)
extern void il2cpp_dump(const char* out_dir);

// 데미지 배율 (6배)
float damage_multiplier = 6.0f;

// 실제 게임의 데미지 함수를 대신할 함수
float (*old_GetDamage)(void* instance);
float new_GetDamage(void* instance) {
    if (old_GetDamage) {
        float original = old_GetDamage(instance);
        return original * damage_multiplier; // 6배 적용
    }
    return 0.0f;
}

void hack_start(const char* game_data_dir) {
    void *handle = nullptr;
    LOGI("게임 엔진 감지 대기 중...");

    // libil2cpp.so가 로드될 때까지 최대 20초간 대기
    for (int i = 0; i < 20; i++) {
        handle = xdl_open("libil2cpp.so", 0);
        if (handle) break;
        sleep(1);
    }

    if (handle) {
        LOGI("libil2cpp.so 감지 성공! 분석 및 수정을 시작합니다.");

        // [중요] 자동 덤프 실행: 분석 파일을 생성합니다.
        il2cpp_dump(game_data_dir);
        LOGI("덤프 완료! 경로: %s/il2cpp", game_data_dir);

        // [주의] 아래 0x123456 부분을 나중에 dump.cs에서 찾은 주소로 바꿔야 합니다.
        // DobbyHook((void*)(xdl_addr(handle, "libil2cpp.so") + 0x123456), (void*)new_GetDamage, (void**)&old_GetDamage);
        
        LOGI("현재는 덤프 파일 생성 모드입니다. 주소를 찾으면 코드를 재수정하세요.");
    }
}

void hack_prepare(const char *game_data_dir, void *data, size_t length) {
    LOGI("데미지 수정 스레드 생성 중...");
    std::thread t(hack_start, game_data_dir);
    t.detach();
}
