env = Environment()
env.Append(CCFLAGS=['/EHsc'], CPPPATH=['inc'])
env.StaticLibrary(target='react-xml', source=Glob('src/*.cpp'))
env.Program(target='sample', source=['sample.cpp'], LIBS=['react-xml'])
