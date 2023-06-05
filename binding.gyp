{
  'targets': [
    {
      'target_name': 'wmi_native_module',
      'sources': [ 'src/main.cpp'],
      'include_dirs': ["<!(node -p \"require('node-addon-api').include_dir\")"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags': [ '-fno-exceptions' ],
      'cflags_cc': [ '-fno-exceptions' ],
      'conditions': [
        ["OS=='linux'", {"sources": [ 'src/unsupported_wmi_wrapper.cpp' ], "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ]}],
        ["OS=='win'", {'sources': [ 'src/wmi_wrapper.cpp' ],  "defines": [ "_HAS_EXCEPTIONS=1" ],
          "msvs_settings": { "VCCLCompilerTool": { "ExceptionHandling": 1 , "AdditionalOptions": ['/sdl'] } },
        }],
      ],
    },
    {
      "target_name": "copy_js",
      "type": "none",
      "dependencies": [ "wmi_native_module" ],
      "copies": [
        {
               'destination': '<(module_root_dir)/build/<(CONFIGURATION_NAME)/',
               'files': ['<(module_root_dir)/wmi_native_module.d.ts', '<(module_root_dir)/package.json']
        }
      ]
    }
  ],
}