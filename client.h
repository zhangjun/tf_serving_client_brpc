/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-09-02 10:36
   Filename      : client.h
   Description   : 
*******************************************/

#pragma once 

#include <string>

#include <brpc/channel.h>
#include "tensorflow_serving/apis/prediction_service.pb.h"
#include "tensorflow/core/framework/tensor.pb.h"

using tensorflow::serving::PredictRequest;
using tensorflow::serving::PredictResponse;
using tensorflow::serving::PredictionService;

typedef google::protobuf::Map<std::string, tensorflow::TensorProto> MapProto;


class ServingClient {
    public:
        ServingClient(::brpc::Channel* channel) : stub_(new tensorflow::serving::PredictionService_Stub(channel)) {}
        int Predict(const PredictRequest& request, PredictResponse& response);
        virtual ~ServingClient() {}

    private:
        std::unique_ptr<tensorflow::serving::PredictionService_Stub> stub_;

};


class ServingTask {
    public:
        ServingTask(::brpc::Channel* channel) : channel_(channel) {}
        virtual ~ServingTask() {};

        virtual int Init();
        virtual int ShutDown();

        // surface detect
        int Process(const std::string &model_name, const std::string &model_sig_name, const void* in_data, const size_t in_size, int &idx, float& prob);

        // bottle detect
        int Process(const std::string &model_name, const std::string &model_sig_name, const void* in_data, const size_t in_size, std::vector<float> &bbox);

        // edge detect
        int Process(const std::string &model_name, const std::string &model_sig_name, const void* in_data, const size_t in_size, const int height, const int width, std::vector<float> &detect_fea);

    private:
        ServingClient* client_ = nullptr;
        brpc::Channel* channel_;
        std::string addr_;
        //std::string model_name_;
        //std::string model_sig_name_;
};
