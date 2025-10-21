project(
    'generate-accessors',
    ['c'],
    version: '0.1',
    default_options: [
        #'buildtype=debug',
        'buildtype=release',
        'c_std=c11',
    ],
    license: 'Apache-2.0',

)

generate_accessors = executable('generate-accessors', 'generate-accessors.c')

subdir('tests')
