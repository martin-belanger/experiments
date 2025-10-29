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

generate_accessors = executable('generate-accessors', 'generate-accessors.c')

subdir('tests')
