/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-08-31 00:36
   Filename      : main.cc
   Description   : 
*******************************************/
#include <iostream>
#include <string>

#include <fstream>

#include <brpc/channel.h>
#include "tensorflow_serving/apis/prediction_service.pb.h"
#include "tensorflow/core/framework/tensor.pb.h"


using tensorflow::serving::PredictRequest;
using tensorflow::serving::PredictResponse;
using tensorflow::serving::PredictionService;

typedef google::protobuf::Map<std::string, tensorflow::TensorProto> MapProto;


DEFINE_string(protocol, "h2:grpc", "Protocol type");
DEFINE_string(server, "10.144.97.105:11000", "IP Address of server");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");



int main(int argc, char* argv[]) {
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);
    brpc::Channel channel;

    brpc::ChannelOptions options;

    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(FLAGS_server.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    std::string model_name = "surface_detect";
    std::string model_signature_name = "serving_default";

    std::string file_path = "cat.jpg";

    tensorflow::serving::PredictionService_Stub stub(&channel);
    
    brpc::Controller cntl;
    PredictRequest request;
    PredictResponse response;

    request.mutable_model_spec() -> set_name(model_name);
    request.mutable_model_spec() -> set_signature_name(model_signature_name);

    std::ifstream img_file(file_path, std::ios::binary);
    std::filebuf* pbuf = img_file.rdbuf();
    auto file_size = pbuf -> pubseekoff(0, std::ios::end, std::ios::in);

    char* image = new char[file_size]();
    pbuf -> pubseekpos(0, std::ios::in);
    pbuf -> sgetn(image, file_size);

    img_file.close();

    tensorflow::TensorProto proto;
    proto.set_dtype(tensorflow::DataType::DT_STRING);
    proto.add_string_val(image, file_size);

    proto.mutable_tensor_shape() -> add_dim() -> set_size(1);

    MapProto& inputs = *request.mutable_inputs();
    inputs["input"] = proto;

    stub.Predict(&cntl, &request, &response, NULL);

    std::cout << "aaa" << std::endl;

    if(cntl.Failed()) {
        std::cout << "call error" << std::endl;
        LOG(WARNING) << cntl.ErrorText();
    }




    return 0;
}
