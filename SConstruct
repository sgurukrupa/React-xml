env = Environment()
env.Append(CCFLAGS=['/EHsc'], CPPPATH=['inc'])
env.StaticLibrary(target='cpputils', source=Glob('src/*.cpp'))
env.Program(target='sample', source=['sample.cpp'], LIBS=['cpputils'])
