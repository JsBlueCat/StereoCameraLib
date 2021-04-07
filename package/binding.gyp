{
  "targets": [
    {
      "target_name": "api",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "hello.cc" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "./include",
        "D:/opencv-3.4.7/build/install/include"
      ],
      "libraries":[
        "-lD:/opencv-3.4.7/build/install/x64/vc16/lib/opencv_world347.lib",
        "-lD:/stereoRectify/StereoCameraLib/package/lib/MvCameraControl.lib",
        "-lD:/stereoRectify/StereoCameraLib/package/lib/Release/stereocamera.lib"
      ],
      "library_type":"static_library",
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      'configurations': {
            'Debug': {
                'msvs_settings': {
                            'VCCLCompilerTool': {
                                'RuntimeLibrary': '3' # /MDd
                    },
                },
            },
            'Release': {
                'msvs_settings': {
                            'VCCLCompilerTool': {
                                'RuntimeLibrary': '2' # /MD
                    },
                },
            },
        },

    }
  ]
}
