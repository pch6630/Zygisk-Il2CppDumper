#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include <thread>
#include <sys/stat.h> // 폴더 생성을 위한 헤더
#include "zygisk.hpp"
#include "hack.h"
#include "il2cpp_dump.h" 
#include "xdl.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygisk-Hack", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Zygisk-Hack", __VA_ARGS__)

void hack_start() {
    void *handle = nullptr;
    LOGI("추적 시작... 엔진 로드를 기다립니다.");

    for (int i = 0; i < 100; i++) {
        handle = xdl_open("libil2cpp.so", 0);
        if (handle) break;
        sleep(1);
    }

    if (handle) {
        // 튕김 방지를 위해 충분히 대기 (로그인 후 로비 진입까지)
        LOGI("엔진 발견! 30초 대기 후 덤프를 수행합니다. 게임을 진행하세요.");
        sleep(30); 

        // [중요] 권한 문제가 없는 앱 내부 저장소 경로
        // MT 관리자로 /data/data/com.yuanl3.zugpshu/files 경로를 확인하세요.
        const char* dump_path = "/data/data/com.yuanl3.zugpshu/files";
        
        // 폴더가 없으면 생성
        mkdir(dump_path, 0777);

        LOGI("덤프 시작 시도 중... 경로: %s", dump_path);
        
        // il2cpp_dump 실행
        il2cpp_dump(dump_path); 
        
        LOGI("★ 덤프 프로세스가 종료되었습니다! MT 관리자로 위 경로를 확인하세요.");
    } else {
        LOGE("엔진 로드 대기 시간 초과.");
    }
}

void hack_prepare(const char *game_data_dir, void *data, size_t length) {
    LOGI("Zygisk 주입 성공! 대상: %s", game_data_dir);
    std::thread t(hack_start);
    t.detach();
}
