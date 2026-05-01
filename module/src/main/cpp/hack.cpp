#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include <thread>
#include "zygisk.hpp"
#include "hack.h"
#include "il2cpp_dump.h" // 덤프 기능을 위한 헤더 포함
#include "xdl.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygisk-Hack", __VA_ARGS__)

void hack_start() {
    void *handle = nullptr;
    LOGI("Zygisk-Hack: [시작] libil2cpp.so 추적 중...");

    // 엔진이 로드될 때까지 최대 100초간 대기합니다.
    for (int i = 0; i < 100; i++) {
        handle = xdl_open("libil2cpp.so", 0);
        if (handle) break;
        sleep(1);
    }

    if (handle) {
        LOGI("Zygisk-Hack: [성공] 엔진 발견! 5초 후 덤프를 실행합니다.");
        sleep(5); // 엔진 안정화를 위한 대기

        // 빌드 에러를 방지하기 위해 인자를 1개(경로)만 전달합니다.
        // 가상 머신에서 접근하기 가장 쉬운 경로입니다.
        const char* dump_path = "/sdcard/Download";
        il2cpp_dump(dump_path); 
        
        LOGI("Zygisk-Hack: [완료] 덤프 프로세스 종료. 경로: %s", dump_path);
    } else {
        LOGI("Zygisk-Hack: [실패] 엔진 로드 대기 시간 초과.");
    }
}

void hack_prepare(const char *game_data_dir, void *data, size_t length) {
    // 패키지명이 com.yuanl3.zugpshu와 일치할 때 실행됩니다.
    LOGI("Zygisk-Hack: 게임 프로세스 진입 확인!");
    std::thread t(hack_start);
    t.detach();
}
