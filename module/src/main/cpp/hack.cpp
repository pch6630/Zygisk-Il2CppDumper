#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include <cstring>
#include <thread>
#include "zygisk.hpp"
#include "hack.h"
#include "xdl.h"

// 로그 정의
#ifndef LOGI
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygisk-Hack", __VA_ARGS__)
#endif

// 데미지 배율 (6.0f = 6배)
float damage_multiplier = 6.0f;

// 실제 게임의 데미지 함수를 대신할 함수
// 게임 구조에 따라 매개변수(void* instance 등)는 다를 수 있습니다.
float (*old_GetDamage)(void* instance);
float new_GetDamage(void* instance) {
    if (old_GetDamage) {
        float original = old_GetDamage(instance);
        return original * damage_multiplier; // 여기서 6배 적용
    }
    return 0.0f;
}

void hack_start(const char *game_data_dir) {
    void *handle = nullptr;
    // 게임 엔진 라이브러리가 로드될 때까지 대기
    for (int i = 0; i < 20; i++) {
        handle = xdl_open("libil2cpp.so", 0);
        if (handle) break;
        sleep(1);
    }

    if (handle) {
        LOGI("libil2cpp.so 감지 성공! 후킹을 시작합니다.");
        
        // 중요: 아래 0x123456 부분을 dump.cs에서 찾은 실제 Offset으로 바꿔야 합니다.
        // 예: DobbyHook((void*)(libbase + 0x123456), (void*)new_GetDamage, (void**)&old_GetDamage);
        
        LOGI("데미지 6배 수정 완료");
    }
}

void hack_prepare(const char *game_data_dir, void *data, size_t length) {
    LOGI("데미지 수정 스레드 생성 중...");
    std::thread t(hack_start, game_data_dir);
    t.detach();
}
