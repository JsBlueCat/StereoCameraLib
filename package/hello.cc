#include <napi.h>
#include "hikvision_double_camera.h"


Napi::String Method(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  auto cam = StereoCamera::GetInstance();
  delete cam;
  return Napi::String::New(env, "world");
}

static Napi::String StartCamera(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try{
    StereoCamera *cam = StereoCamera::GetInstance();
    return Napi::String::New(env, "starting connecting camera\n");
  }catch(std::exception &ex){
    return Napi::String::New(env, "starting connecting camera\n");
  }
}


static Napi::Boolean QueryCamera(const Napi::CallbackInfo& info){
  Napi::Env env = info.Env();
  int id = info[0].As<Napi::Number>().Int32Value();
  StereoCamera *cam = StereoCamera::GetInstance();
  bool status = cam->QueryCamera(id);
  if(!status){
    delete StereoCamera::instance;
    StereoCamera::instance = nullptr;
  }
  return Napi::Boolean::New(env,status);
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "hello"),
              Napi::Function::New(env, Method));
              
  exports.Set(Napi::String::New(env, "connect_camera_cpp"),
              Napi::Function::New(env, StartCamera));

  exports.Set(Napi::String::New(env, "query_camera_cpp"),
              Napi::Function::New(env, QueryCamera));
  return exports;
}

NODE_API_MODULE(hello, Init)
