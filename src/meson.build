statusengine_sources = ['Nebmodule.cpp', 'Statusengine.cpp', 'MessageHandler/MessageHandlerList.cpp']

statusengine_deps = [uchardet_dep, jsonc_dep, toml_dep]
statusengine_cpp_args = []

if get_option('nagios')
    statusengine_deps += [nagios_dep]
    statusengine_cpp_args += ['-DBUILD_NAGIOS']
else
    statusengine_deps += [naemon_dep, glib_dep]
endif

if get_option('gearman') and gearmand_dep.found()
    statusengine_deps += [gearmand_dep]
    statusengine_cpp_args += ['-DWITH_GEARMAN']
    statusengine_sources += ['MessageHandler/GearmanClient.cpp']
endif

if get_option('rabbitmq') and librabbitmq_dep.found()
    statusengine_deps += [librabbitmq_dep]
    statusengine_cpp_args += ['-DWITH_RABBITMQ']
    statusengine_sources += ['MessageHandler/RabbitmqClient.cpp']
    if librabbitmq_dep.version().version_compare('<0.6.0')
        statusengine_cpp_args += ['-DWITH_RABBITMQ_CX060']
    endif
    if librabbitmq_dep.version().version_compare('<0.8.0')
        statusengine_cpp_args += ['-DWITH_RABBITMQ_CX080']
    endif
endif

statusengine_lib = shared_module('statusengine',
    statusengine_sources,
    dependencies: statusengine_deps,
    cpp_args: statusengine_cpp_args,
    install: true)
