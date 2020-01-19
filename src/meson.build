statusengine_sources = ['Nebmodule.cpp', 'Statusengine.cpp', 'MessageHandler/MessageHandlerList.cpp', 'MessageHandler/GearmanClient.cpp', 'MessageHandler/RabbitmqClient.cpp']

statusengine_deps = [glib_dep, uchardet_dep, jsonc_dep, toml_dep]
statusengine_cpp_args = []

if get_option('nagios')
    statusengine_deps += [nagios_dep]
    statusengine_cpp_args += ['-DBUILD_NAGIOS']
else
    statusengine_deps += [naemon_dep]
endif

if gearmand_dep.found() and get_option('gearman')
    statusengine_deps += [gearmand_dep]
    statusengine_cpp_args += ['-DWITH_GEARMAN']
endif

if librabbitmq_dep.found() and get_option('rabbitmq')
    statusengine_deps += [librabbitmq_dep]
    statusengine_cpp_args += ['-DWITH_RABBITMQ']
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
