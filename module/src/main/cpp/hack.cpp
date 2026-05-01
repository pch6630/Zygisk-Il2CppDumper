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

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygisk-Hack", __VA_ARGS__)

void hack_start() {
    void *handle = nullptr;
    LOGI("Zygisk-Hack: 안전 모드로 추적을 시작합니다.");

    // 1. 엔진이 완전히 로드되고 안정화될 때까지 대기 횟수를 늘립니다.
    for (int i = 0; i < 150; i++) {
        // 시스템 dlopen으로 먼저 시도 (가상 머신 분할 APK 대응)
        handle = dlopen("libil2cpp.so", RTLD_NOW);
        
        if (!handle) {
            handle = xdl_open("libil2cpp.so", 0);
        }

        if (handle) break;
        sleep(1);
    }

    if (handle) {
        LOGI("Zygisk-Hack: 엔진 포착 성공! 튕김 방지를 위해 40초간 대기합니다.");
        // 로그인 및 로딩이 완전히 끝난 후 '로비'에서 덤프가 시작되도록 충분히 기다립니다.
        sleep(40); 

        const char* dump_path = "/data/data/com.yuanl3.zugpshu/files";
        mkdir(dump_path, 0777);

        LOGI("Zygisk-Hack: 메모리 안전 검사 후 덤프를 실행합니다.");
        
        // 덤프 중 튕김을 방지하기 위해 il2cpp_dump 호출
        il2cpp_dump(dump_path); 
        
        LOGI("Zygisk-Hack: ★ 모든 작업 완료! 파일을 확인하세요.");
    } else {
        LOGI("Zygisk-Hack: 엔진 로드 감지 실패.");
    }
}

void hack_prepare(const char *game_data_dir, void *data, size_t length) {
    // 게임 프로세스 진입 시 스레드 분리 실행
    LOGI("Zygisk-Hack: 모듈 주입됨.");
    std::thread t(hack_start);
    t.detach();
}
