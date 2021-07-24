#pragma once
#include<string>
#include<vector>
#include <iostream>
#include "debug.h"

typedef enum {
  Success = 0,
  CamNotFound,
  CamNotConnected,
  CamNotReady,
  ImageNotPaired,
  NotSufficientImage,
  ConfigNotFound,
  Cp3PointNotEqual,
  Cp3PointFoundNotPair,
  Cp3NotFound,
  Unknown,
} ErrorInfo;

#define Assert_Error(err) if(err != ErrorInfo::Success) { return err;}

static std::vector<std::string> error_string{
  "success",//0
  "camera is not founded , please check camera",//1
  "camera is not connected , please try again",//2
  "camera is not Ready , please wait amoment",//3
  "the points is not in pair, please check cp3",//4
  "not sufficient image , please grab 18-24 images",//5
  "not found config files, please check premission",//6
  "found unequal cp3, check position",//7
  "not found pair cp3, please connect adminer",//8
  "not found cp3 ",//9
  "unkown problem",//10
};

#ifndef NODE_API_SWALLOW_UNTHROWABLE_EXCEPTIONS
#include<exception>
#define Check_Api(err) do { \
  if(err != ErrorInfo::Success) throw error_string[err];\
} while(0);
#else

#define Check_Api(err) do {                     \
  if(err!= ErrorInfo::Success) std::cout << error_string[err]  << std::endl; \
} while(0);
#endif