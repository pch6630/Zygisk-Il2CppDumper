#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include <thread>
#include "zygisk.hpp"
#include "hack.h"
#include "il2cpp_dump.h" // [중요] 덤프 헤더 포함
#include "xdl.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygisk-Hack", __VA_ARGS__)

void hack_start() {
    void *handle = nullptr;
    LOGI("Zygisk-Hack: 추적 시작...");

    // 엔진이 로드될 때까지 최대 100초 대기
    for (int i = 0; i < 100; i++) {
        handle = xdl_open("libil2cpp.so", 0);
        if (handle) break;
        sleep(1);
    }

    if (handle) {
        LOGI("Zygisk-Hack: 엔진 발견! 5초 후 덤프를 실행합니다.");
        sleep(5); // 엔진이 완전히 안정화될 때까지 대기

        // [핵심] 가상 머신에서 가장 쓰기 쉬운 경로인 /sdcard/Download 사용
        // 만약 이 경로에 파일이 안 생기면 /data/data/com.yuanl3.zugpshu/cache 로 바꿔보세요.
        const char* dump_path = "/sdcard/Download";
        il2cpp_dump(dump_path, handle); 
        
        LOGI("Zygisk-Hack: 덤프 프로세스 종료. 경로 확인: %s", dump_path);
    } else {
        LOGI("Zygisk-Hack: 엔진 발견 실패 (시간 초과)");
    }
}

void hack_prepare(const char *game_data_dir, void *data, size_t length) {
    // 패키지명 일치 시 실행
    LOGI("Zygisk-Hack: 게임 진입 확인!");
    std::thread t(hack_start);
    t.detach();
}
