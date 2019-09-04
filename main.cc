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

#include <opencv2/opencv.hpp>


#include "client.h"

DEFINE_string(protocol, "h2:grpc", "Protocol type");
//DEFINE_string(server, "10.144.97.105:11000", "IP Address of server");
DEFINE_string(server, "10.144.120.23:11000", "IP Address of server");
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

    //std::string model_name = "surface_detect";
    //std::string model_signature_name = "serving_default";

    //std::string model_name = "bottle_detect";
    //std::string model_signature_name = "detection_signature";

    std::string model_name = "edge_detect";
    std::string model_signature_name = "serving_default";

    std::string file_path = "cat.jpg";
    //std::string file_path = "bottle.jpg";


    ServingTask* task = new ServingTask(&channel);

    task -> Init();

    std::ifstream img_file(file_path, std::ios::binary);
    std::filebuf* pbuf = img_file.rdbuf();
    auto file_size = pbuf -> pubseekoff(0, std::ios::end, std::ios::in);

    char* image = new char[file_size]();
    pbuf -> pubseekpos(0, std::ios::in);
    pbuf -> sgetn(image, file_size);

    img_file.close();


    // cv mat
    //cv::Mat img = cv::imread(file_path.c_str());
    //std::vector<uchar> data_encode;
    //imencode(".jpg", img, data_encode);
    //unsigned char* image = &data_encode.at(0);
    //auto file_size = data_encode.size();


    //int idx = 0;
    //float prob = 0.0;
    //task -> Process(model_name, model_signature_name, image, file_size, idx, prob); 

    // bottle detect
    //std::vector<float> bbox;
    //task -> Process(model_name, model_signature_name, image, file_size, bbox); 

    // edge detect
    std::vector<float> detect_fea;
    task -> Process(model_name, model_signature_name, image, file_size, 10, 10, detect_fea); 


    task -> ShutDown();


    return 0;
}
