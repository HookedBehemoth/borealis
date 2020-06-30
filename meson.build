project('borealis_example', ['c', 'cpp'],
    version: '1.0.0',
    default_options: [ 'buildtype=release', 'strip=true', 'b_ndebug=if-release', 'cpp_std=c++1z' ],
)

dep_fmt   = dependency('fmt', version : '>=6.2.1')

subdir('library')

example_files = files(
    'example/main.cpp',
    'example/ui/recycler_view.cpp',
    'example/ui/sane_image.cpp',
    'example/ui/example_adapter.cpp',
    'example/sample_installer_page.cpp',
    'example/sample_loading_page.cpp'
)

borealis_example = executable(
    'borealis_example',
    [ example_files, borealis_files ],
    dependencies : [borealis_dependencies, dep_fmt],
    install: true,
    include_directories: [ borealis_include, include_directories('example')],
    cpp_args: [ '-g', '-O2', '-DBOREALIS_RESOURCES="./resources/"' ]
)
