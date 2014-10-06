{
  "targets": [
    {
      "target_name": "NativeTypes",
      "sources": ["src/common.cc",
                  "src/NativeTypes.cc",
                  "src/Map.cc",
                  "src/Set.cc",
                  "src/Vector.cc"],
      "conditions": [
        ['OS=="linux"', {
          'cflags_cc!': ['-fno-exceptions']
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-fexceptions'
            ]
          }
        }],
        ['OS=="win"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalOptions': [ '/EHsc' ]
            }
          }
        }]
      ]
    }
  ]
}
