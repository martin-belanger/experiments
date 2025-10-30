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

configure_file(output: 'config.h', configuration: conf_data)

generate_accessors = executable('generate-accessors', 'generate-accessors.c')

subdir('tests')

