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
        "C:/cygwin64/tmp/win-gmp/include",
        "C:/cygwin64/home/Anton/ecc/givaro-3.8.0/src/kernel/system",
        "C:/cygwin64/home/Anton/ecc/givaro-3.8.0/src/kernel/memory",
        "C:/cygwin64/home/Anton/ecc/givaro-3.8.0/src/kernel/zpz",
        "C:/cygwin64/home/Anton/ecc/givaro-3.8.0/src/kernel/integer",
        "C:/cygwin64/home/Anton/ecc/givaro-3.8.0/src/kernel/bstruct",
        "C:/cygwin64/home/Anton/ecc/givaro-3.8.0/src/kernel",
        "C:/cygwin64/home/Anton/ecc/givaro-3.8.0/src/library/poly1",
        "C:/cygwin64/home/Anton/ecc/givaro-3.8.0/src/library/tools",
        "D:/awork/lib-crypto/inc"
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      "target_name": "addon",
      "sources": [  "addon.cc", "node-bignumber.cc", "node-point.cc" , "node-curve.cc"  ],
      "libraries": [
        "C:/cygwin64/home/Anton/ecc/examples/crypto/x64/Debug/crypto.lib",
        "../libgivaro.lib",
        "C:/cygwin64/tmp/win-gmp/lib/libgmpxx.a",
        "C:/cygwin64/tmp/win-gmp/lib/libgmp.a"        
      ],
    }
  ]
}