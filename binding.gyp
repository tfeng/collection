{
  "targets": [
    {
      "target_name": "NativeTypes",
      "sources": ["src/common.cc",
                  "src/NativeTypes.cc",
                  "src/Set.cc",
                  "src/Vector.cc"],
      "conditions": [
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
