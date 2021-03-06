def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')

def configure(conf):
  conf.env.CXXFLAGS += ['-O2', '-Wall', '-g', '-pipe']
  conf.load('compiler_cxx')
  conf.load('unittest_gtest')
  conf.check_cfg(package = 'pficommon', args = '--cflags --libs')

def build(bld):
  bld.recurse('src')

  bld.program(
    source = 'sample/sample.cpp',
    target = 'jsonconfig_sample',
    includes = 'src',
    use = 'jsonconfig'
    )
