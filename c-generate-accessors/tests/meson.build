test_env = environment({'MALLOC_PERTURB_': '0'})

test('Test with exclusion list', generate_accessors, args: ['-c', '-o', 'output', '-e', files('excl.list'), files('structs.h')], env: test_env)
