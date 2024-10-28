project('network_visualization', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

# Add include directory
inc = include_directories('include')

pistache_dep = dependency('libpistache')
json_dep = dependency('nlohmann_json')

src_files = [
    'src/main.cpp',
    'src/Router.cpp',
    'src/Switch.cpp',
    'src/Device.cpp'
]

executable('network_server',
           src_files,
           include_directories : inc,
           dependencies : [pistache_dep, json_dep],
           install : true)
