#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include <thread>
#include <dlfcn.h>
#include <sys/stat.h>
#include "zygisk.hpp"
#include "hack.h"
#include "il2cpp_dump.h"
#include "xdl.h"

// 에러의 원인이었던 LOGI 정의를 확실히 추가합니다.
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygisk-Hack", __VA_ARGS__)

void hack_start() {
    void *handle = nullptr;
    LOGI("Zygisk-Hack: 엔진 추적을 고강도로 시작합니다.");

    // 가상 머신 환경에 맞춰 대기 횟수를 늘렸습니다.
    for (int i = 0; i < 200; i++) {
        // 방법 1: xdl 사용
        handle = xdl_open("libil2cpp.so", 0);
        
        // 방법 2: xdl 실패 시 기본 dlopen 시도
        if (!handle) {
            handle = dlopen("libil2cpp.so", RTLD_NOW);
        }

        if (handle) break;
        
        if (i % 10 == 0) {
            LOGI("Zygisk-Hack: 아직 엔진을 찾는 중... (%d초 경과)", i);
        }
        
        // sleep 함수 에러 방지를 위해 unistd.h가 상단에 포함되었습니다.
        sleep(1);
    }

    if (handle) {
        LOGI("Zygisk-Hack: ★ 성공! 엔진을 찾았습니다.");
        
        // 로그인 후 로비 진입까지 충분히 기다립니다.
        sleep(25); 

        // 경로 설정 및 폴더 생성
        const char* dump_path = "/data/data/com.yuanl3.zugpshu/files";
        mkdir(dump_path, 0777);

        LOGI("Zygisk-Hack: 덤프를 시작합니다. 경로: %s", dump_path);
        il2cpp_dump(dump_path); 
        
        LOGI("Zygisk-Hack: ★ 덤프 프로세스 완료!");
    } else {
        LOGI("Zygisk-Hack: 엔진 찾기 실패 (시간 초과)");
    }
}

void hack_prepare(const char *game_data_dir, void *data, size_t length) {
    LOGI("Zygisk-Hack: 게임 프로세스 진입 확인!");
    std::thread t(hack_start);
    t.detach();
}
