#include <dlfcn.h> // xdl 대신 기본 dlopen 사용 시도

void hack_start() {
    void *handle = nullptr;
    LOGI("Zygisk-Hack: 엔진 추적을 고강도로 시작합니다.");

    // 가상 머신에서는 로딩이 늦을 수 있으므로 200번(약 200초) 시도합니다.
    for (int i = 0; i < 200; i++) {
        // 방법 1: xdl 사용
        handle = xdl_open("libil2cpp.so", 0);
        
        // 방법 2: 만약 xdl이 실패하면 기본 dlopen으로 재시도
        if (!handle) {
            handle = dlopen("libil2cpp.so", RTLD_NOW);
        }

        if (handle) break;
        
        // 1초마다 로그를 찍어 살아있는지 확인
        if (i % 10 == 0) LOGI("Zygisk-Hack: 아직 엔진을 찾는 중... (%d초 경과)", i);
        
        sleep(1);
    }

    if (handle) {
        LOGI("Zygisk-Hack: ★ 성공! 엔진을 드디어 찾았습니다.");
        // 이후 덤프 로직 실행...
        sleep(20); 
        il2cpp_dump("/data/data/com.yuanl3.zugpshu/files");
        LOGI("Zygisk-Hack: ★ 덤프 완료!");
    } else {
        LOGI("Zygisk-Hack: 엔진 찾기 포기 (시간 초과)");
    }
}
