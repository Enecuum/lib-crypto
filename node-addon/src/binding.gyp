{
  "targets": [
    { 
      'cflags!': [ '-fno-exceptions' ],
        'cflags_cc!': [ '-fno-exceptions' ],
        'xcode_settings': {
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
          'CLANG_CXX_LIBRARY': 'libc++',
          'MACOSX_DEPLOYMENT_TARGET': '10.7',
        },
        'msvs_settings': {
          'VCCLCompilerTool': { 'ExceptionHandling': 1 },
        },
      "include_dirs" : [
        "<!@(node -p \"require('node-addon-api').include\")",
	"/tmp/givaro-build/include"
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      "target_name": "addon",
      "sources": [  "addon.cc", "crypto_addon.cc" ],
      "libraries": ['./node_modules/enq-bin/libenq.a',
        '/tmp/givaro-build/lib/libgivaro.a',
'-lgmpxx', '-lgmp' ],
    }
  ]
}
