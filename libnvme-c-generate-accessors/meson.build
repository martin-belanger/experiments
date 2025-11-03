project(
    'generate-accessors',
    ['c'],
    version: '0.1',
    default_options: [
        'buildtype=debug',
        #'buildtype=release',
        'c_std=c11',
    ],
    license: 'Apache-2.0',
)

add_global_arguments('-D_GNU_SOURCE', language : 'c')

cc = meson.get_compiler('c')
conf_data = configuration_data()
conf_data.set('HAVE_SENDFILE', cc.has_function('sendfile'))

config_h = configure_file(output: 'config.h', configuration: conf_data)

if meson.version().version_compare('>=1.4.0')
  config_h_path = config_h.full_path()
else
  config_h_path = meson.current_build_dir() / 'config.h'
endif

generate_accessors = executable(
    'generate-accessors',
    'generate-accessors.c',
    c_args: [
        '-include', config_h_path,
    ],
)

subdir('tests')

