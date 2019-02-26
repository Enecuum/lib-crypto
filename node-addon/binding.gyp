{
  "targets": [
    { 
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      'msvs_settings': {
    'VCCLCompilerTool': { 'ExceptionHandling': 1 },
  },
      "include_dirs" : [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      "target_name": "addon",
      "sources": [  "addon.cc", "node-bignumber.cc", "node-point.cc" , "node-curve.cc"  ],
      "libraries": [ "../ecc/x64/dll/crypto.lib" ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}