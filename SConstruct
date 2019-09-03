#!/search/odin/oyc/anaconda3/bin/python

import os

env = Environment()

root_path = os.getcwd()

env.Append(CPPPATH = [root_path,
        './include',
        '/search/odin/zhangjun/tools/protobuf-3.7.1/include',
        '/search/odin/zhangjun/tools/brpc/include',
    ])
env.Append(LIBPATH = [
	    './libs',
        '.',
        '/search/odin/zhangjun/tools/protobuf-3.7.1/lib',
        '/search/odin/zhangjun/tools/brpc/lib',
	])

env.Append(LIBS = ['protobuf'])
env.Append(LIBS = ['brpc'])
env.Append(LIBS = ['libserving_proto'])

env.Append(RPATH = [
	'./libs',
        '.',
    '/search/odin/zhangjun/tools/protobuf-3.7.1/lib',
    '/search/odin/zhangjun/tools/brpc/lib',
	])

#env.Append(CCFLAGS = ['-Wall', '-O2', '-std=c++11', '-g'])
env.Append(CCFLAGS = ['-O2', '-std=c++11', '-g', '-DUSE_RTTI=1', '-Wl,-Bstatic','-DGFLAGS_NS=gflags'])
env.Append(LINKLAGS = ['-static'])


# https://github.com/SCons/scons/wiki/ProtocBuilder
#protoc = Builder(action="protoc --cpp_out=. $SOURCE")
#env2 = Environment(
#        ENV = {'PATH': os.environ['PATH']},
#        BUILDERS={'Protoc':protoc})
#env2.Protoc([
#        "tensorflow_serving/apis/model.pb.h",
#        "tensorflow_serving/apis/model.pb.cc",
#    ], "tensorflow_serving/apis/model.proto")
##env2.Protoc("surface_srv.pb.cc surface_srv.pb.h".split(), "surface_srv.proto")
##env2.Protoc("surface_srv.pb.cc surface_srv.pb.h".split(), "surface_srv.proto")


#PROTOC = Builder(action="protoc -I . --cpp_out=. $SOURCES",src_surffix=".proto")
PROTOC = Builder(action="protoc -I protos --cpp_out=. $SOURCES",src_surffix=".proto")
env_pb = Environment(
        ENV = {'PATH': os.environ['PATH']},
        BUILDERS = {'Protoc':PROTOC}
        )

PROTOFILE = Glob('protos/service/*.proto')
CCFILE = Glob('*.cc')
#print(PROTOFILE[0])

#env_pb.Protoc(PROTOFILE)

def gen_proto(source, proto_path='', out_path = ''):
#def gen_proto(source = [], proto_path, out_path):
    for item in source:
        proto_file = str(item)
        env_pb.Protoc(
                os.path.splitext(proto_file)[0] + '.pb.cc',
                proto_file)

gen_proto(
        source = Glob('protos/tensorflow/core/framework/*.proto') +
        Glob("protos/tensorflow/core/protobuf/*.proto") + 
        Glob("protos/tensorflow_serving/apis/*.proto"),
        )

env.Library(
    target = "serving_proto",
    source = Glob('tensorflow/core/framework/*.pb.cc') +
    Glob("tensorflow/core/protobuf/*.pb.cc") + 
    Glob("tensorflow_serving/apis/*.pb.cc"),
)

env.Program(
    target = "main",
    source = [
        "main.cc",
#        "service.cc",
#        "base_task.cc",
#        "surface_detect.cc",
#        "bottle_detect.cc",
#        "edge_detect.cc",
#        "reply.cc",
#        "surface_srv.pb.cc",
    ],
)
