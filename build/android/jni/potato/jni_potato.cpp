#include <jni.h>
#include <android/log.h>

#include "me_alexchi_potato_Potato.h"
#include "me_alexchi_potato_Potato_Render.h"

#include "potato.h"

#define  LOG_TAG    "jni_potato"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

void GetConfig(std::string& rsPath, std::string& rsFile)
{
  rsPath = "";
  rsFile = "data/config.json";
}

JNIEXPORT void JNICALL Java_me_alexchi_potato_Potato_create
  (JNIEnv *, jobject)
{
  LOGI("create");
  std::string path = "";
  std::string file = "";
  ac::Potato::Instance(path, file);
  //
}

JNIEXPORT void JNICALL Java_me_alexchi_potato_Potato_destroy
  (JNIEnv *, jobject)
{
  //
}

JNIEXPORT void JNICALL Java_me_alexchi_potato_Potato_00024Render_start
 (JNIEnv *, jobject)
{
  //
}

JNIEXPORT void JNICALL Java_me_alexchi_potato_Potato_00024Render_resize
  (JNIEnv *, jobject, jint, jint)
{
  //
}

JNIEXPORT void JNICALL Java_me_alexchi_potato_Potato_00024Render_stop
  (JNIEnv *, jobject)
{
  //
}
