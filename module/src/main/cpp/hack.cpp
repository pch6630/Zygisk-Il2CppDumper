#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include <string>
#include <thread>
#include "zygisk.hpp"
#include "hack.h"
#include "xdl.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygisk-Hack", __VA_ARGS__)

// 데미지 배율 설정
float damage_multiplier = 6.0f;

// 1. 함수 포인터 정의 (게임마다 파라미터 구성은 다를 수 있음)
float (*old_GetDamage)(void* instance);

// 2. 우리가 만든 변조 함수
float new_GetDamage(void* instance) {
    if (old_GetDamage) {
        float original = old_GetDamage(instance);
        return original * damage_multiplier; 
    }
    return 0.0f;
}

void hack_start(const char* game_data_dir) {
    void *handle = nullptr;
    LOGI("엔진 로드 대기 중...");

    // 엔진이 로드될 때까지 충분히 대기 (튕김 방지)
    for (int i = 0; i < 30; i++) {
        handle = xdl_open("libil2cpp.so", 0);
        if (handle) break;
        sleep(1);
    }

    if (handle) {
        LOGI("libil2cpp.so 감지 성공!");

        // --- [중요] 튕김 방지 구간 ---
        // 현재 dump.cs가 없어서 튕기는 것이라면, 아래 DobbyHook을 주석처리하고
        // 먼저 il2cpp 폴더가 생기는지 확인해야 합니다.
        
        /* 
        // 주소를 알아낸 후 아래 주석을 해제하세요.
        unsigned long long target_offset = 0x123456; // 여기에 dump.cs에서 찾은 RVA 주소를 넣으세요.
        void* target_addr = (void*)((unsigned long long)xdl_addr(handle, "libil2cpp.so") + target_offset);
        
        // Dobby를 이용한 후킹 (Dobby 라이브러리가 포함되어 있어야 함)
        // DobbyHook(target_addr, (void*)new_GetDamage, (void**)&old_GetDamage);
        */

        LOGI("분석을 위해 il2cpp 폴더를 확인하세요.");
    }
}

void hack_prepare(const char *game_data_dir, void *data, size_t length) {
    // 스레드를 분리하여 게임 메인 로직 방해 금지
    std::thread t(hack_start, game_data_dir);
    t.detach();
}
