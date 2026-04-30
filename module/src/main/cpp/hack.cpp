#include <jni.h>
#include <unistd.h>
#include <sys/types.h>
#include <android/log.h>
#include <cstring>
#include "zygisk.hpp"
#include "hack.h" // hack_prepare 함수 호출을 위해 필요

using namespace zygisk;

class MyModule : public ModuleBase {
public:
    void onLoad(Api* api, JNIEnv* env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs* args) override {
        // 1. 현재 실행 중인 앱의 패키지명을 가져옴
        const char* process = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (process != nullptr) {
            // 2. 패키지명이 com.yuanl3.zugpshu 인지 확인[cite: 2]
            if (strcmp(process, "com.yuanl3.zugpshu") == 0) {
                LOGI("대상 게임 감지됨: %s", process);

                // 3. 데이터 저장 경로 설정 (앱 전용 내부 경로)
                const char* game_data_dir = "/data/user/0/com.yuanl3.zugpshu/files";

                // 4. hack.cpp의 hack_prepare 함수 호출
                // 여기서 별도 스레드가 생성되어 libil2cpp.so를 찾기 시작합니다.
                hack_prepare(game_data_dir, nullptr, 0);
            }
            
            // 메모리 해제
            env->ReleaseStringUTFChars(args->nice_name, process);
        }
    }

private:
    Api* api;
    JNIEnv* env;
};

// Zygisk 모듈 등록
REGISTER_ZYGISK_MODULE(MyModule)
