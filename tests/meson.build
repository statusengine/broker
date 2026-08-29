doctest_dep = dependency('doctest')

test_deps = [doctest_dep, uchardet_dep, toml_dep]
test_cpp_args = statusengine_cpp_args + [
    '-DSTATUSENGINE_EXAMPLE_CONFIG="' + meson.project_source_root() / 'statusengine.toml' + '"',
    '-DSTATUSENGINE_DEVENV_CONFIG="' + meson.project_source_root() / 'devtools/buildenv/statusengine.toml' + '"',
    '-DSTATUSENGINE_FIXTURE_DIR="' + meson.current_source_dir() / 'fixtures' + '"',
]

if get_option('nagios')
    test_deps += [nagios_dep]
else
    test_deps += [glib_dep]
    # The tests do not need an installed naemon, the checked in headers are enough:
    # nothing under test calls into libnaemon, and the few symbols that are referenced
    # are provided by naemon_stubs.cpp.
    test_deps += [declare_dependency(
        include_directories: include_directories('../devtools/ci')).as_system('system')]
endif

test_incdirs = include_directories('.', '../src')

statusengine_tests = executable('statusengine-tests',
    ['main.cpp', 'naemon_stubs.cpp', 'test_encoding.cpp', 'test_configuration.cpp', 'test_logstream.cpp', 'test_queue.cpp', 'test_messageformat.cpp', 'test_workerloop.cpp',
     '../src/Encoding.cpp', '../vendor/yyjson.c'],
    include_directories: test_incdirs,
    dependencies: test_deps,
    cpp_args: test_cpp_args,
    build_by_default: true,
    install: false)

test('unit', statusengine_tests, protocol: 'exitcode', timeout: 120)

statusengine_bench = executable('statusengine-bench',
    ['bench.cpp', 'naemon_stubs.cpp', '../src/Encoding.cpp', '../vendor/yyjson.c'],
    include_directories: test_incdirs,
    dependencies: test_deps,
    cpp_args: test_cpp_args,
    build_by_default: true,
    install: false)
