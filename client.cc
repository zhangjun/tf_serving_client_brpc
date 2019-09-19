/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-09-03 18:28
   Filename      : client.cc
   Description   : 
*******************************************/

#include "client.h"

int ServingClient::Predict(const PredictRequest& request, PredictResponse& response) {
    brpc::Controller cntl;

    stub_ -> Predict(&cntl, &request, &response, NULL);
    if(cntl.Failed()) {
       LOG(INFO) << "Predict success, latecy: " << cntl.latency_us() << " us";
    } else {
        LOG(WARNING) << cntl.ErrorText();
        return -1;
    }

    return 0;
}


int ServingTask::Init() {
    client_ = new ServingClient(channel_);
    if(nullptr == client_) {
        LOG(INFO) << "failed to init ServingClient.";
        return -1;
    }
    LOG(INFO) << "Init ServingClient success.";
    return 0;
}

int ServingTask::ShutDown() {
    if(nullptr != client_) {
        delete client_;
        client_ = nullptr;
    }
    LOG(INFO) << "ServingClient shutdown.";
}

int ServingTask::Process(const std::string &model_name, const std::string &model_sig_name, const void* in_data, const size_t in_size, int &idx, float& prob) {
    if(nullptr == client_)
        return -1;

    PredictRequest request;
    PredictResponse response;

    request.mutable_model_spec() -> set_name(model_name);
    request.mutable_model_spec() -> set_signature_name(model_sig_name);

    tensorflow::TensorProto proto;
    proto.set_dtype(tensorflow::DataType::DT_STRING);
    proto.add_string_val(in_data, in_size);

    proto.mutable_tensor_shape() -> add_dim() -> set_size(1);

    MapProto& inputs = *request.mutable_inputs();
    inputs["input"] = proto;

    int ret = client_ -> Predict(request, response);

    LOG(INFO) << "outputs size is " << response.outputs_size();

    MapProto& map_outputs = *response.mutable_outputs();

    MapProto::iterator iter;
    int output_idx = 0;

    for(iter = map_outputs.begin(); iter != map_outputs.end(); ++iter) {
        tensorflow::TensorProto& result_tensor_proto = iter -> second;
        LOG(INFO) << "num of probs: " << result_tensor_proto.float_val_size();
        for(int i = 0; i < result_tensor_proto.float_val_size(); i ++) {
            LOG(INFO) << "idx: " << i << ", prob: " << result_tensor_proto.float_val(i);
        }
    }


    return ret;
}



int ServingTask::Process(const std::string &model_name, const std::string &model_sig_name, const void* in_data, const size_t in_size, std::vector<float> &bbox) {
    if(nullptr == client_)
        return -1;

    PredictRequest request;
    PredictResponse response;

    request.mutable_model_spec() -> set_name(model_name);
    request.mutable_model_spec() -> set_signature_name(model_sig_name);

    tensorflow::TensorProto proto;
    proto.set_dtype(tensorflow::DataType::DT_STRING);
    proto.add_string_val(in_data, in_size);

    proto.mutable_tensor_shape() -> add_dim() -> set_size(1);

    MapProto& inputs = *request.mutable_inputs();
    inputs["inputs"] = proto;

    int ret = client_ -> Predict(request, response);

    LOG(INFO) << "outputs size is " << response.outputs_size();

    MapProto& map_outputs = *response.mutable_outputs();

    MapProto::iterator iter;
    int output_idx = 0;

    auto it = map_outputs.find("detection_boxes");
    if(it == map_outputs.end()){
        return -2;
    }

    std::vector<float> data((it -> second).float_val().begin(), (it -> second).float_val().begin() + 4);
    LOG(INFO) << "data size: " << (it -> second).float_val_size(); 
    LOG(INFO) << "data : " << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3];
    //<< "," << (it -> second).float_val(1) << ", " << (it -> second).float_val(2) << ", " < (it -> second).float_val(3); 

    if((it -> second).has_tensor_shape()) {
        LOG(INFO) << "has tensor shape";

        //tensorflow::TensorShape& shape_data = *(iter -> second).mutable_tensor_shape();
        //LOG(INFO) << "tensorshape size is " << (iter -> second).tensor_shape_size();
    }

    //tensorflow::TensorProto& result_tensor_proto = it -> second;
    //LOG(INFO) << "num of probs: " << result_tensor_proto.float_val_size();
    //for(int i = 0; i < result_tensor_proto.float_val_size(); i ++) {
    //    LOG(INFO) << "idx: " << i << ", prob: " << result_tensor_proto.float_val(i);
    //}

    //for(iter = map_outputs.begin(); iter != map_outputs.end(); ++iter) {
    //    tensorflow::TensorProto& result_tensor_proto = iter -> second;
    //    LOG(INFO) << "num of probs: " << result_tensor_proto.float_val_size();
    //    for(int i = 0; i < result_tensor_proto.float_val_size(); i ++) {
    //        LOG(INFO) << "idx: " << i << ", prob: " << result_tensor_proto.float_val(i);
    //    }
    //}


    return ret;
}


int ServingTask::Process(const std::string &model_name, const std::string &model_sig_name, const void* in_data, const size_t in_size, const int height, const int width, std::vector<float> &detect_fea){

    if(nullptr == client_)
        return -1;

    PredictRequest request;
    PredictResponse response;

    request.mutable_model_spec() -> set_name(model_name);
    request.mutable_model_spec() -> set_signature_name(model_sig_name);

    tensorflow::TensorProto proto;
    proto.set_dtype(tensorflow::DataType::DT_STRING);
    proto.add_string_val(in_data, in_size);

    proto.mutable_tensor_shape() -> add_dim() -> set_size(1);

    MapProto& inputs = *request.mutable_inputs();
    inputs["input"] = proto;

    int ret = client_ -> Predict(request, response);

    LOG(INFO) << "outputs size is " << response.outputs_size();

    MapProto& map_outputs = *response.mutable_outputs();

    MapProto::iterator iter;
    int output_idx = 0;

    auto it = map_outputs.find("output");
    if(it == map_outputs.end()){
        return -2;
    }

    //std::vector<float> data((it -> second).float_val().begin(), (it -> second).float_val().begin() + 4);
    LOG(INFO) << "data size: " << (it -> second).float_val_size(); 
    //LOG(INFO) << "data : " << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3];
    //<< "," << (it -> second).float_val(1) << ", " << (it -> second).float_val(2) << ", " < (it -> second).float_val(3); 

    if((it -> second).has_tensor_shape()) {
        LOG(INFO) << "has tensor shape";
    }

    //tensorflow::TensorProto& result_tensor_proto = it -> second;
    //LOG(INFO) << "num of probs: " << result_tensor_proto.float_val_size();
    //for(int i = 0; i < result_tensor_proto.float_val_size(); i ++) {
    //    LOG(INFO) << "idx: " << i << ", prob: " << result_tensor_proto.float_val(i);
    //}

    //for(iter = map_outputs.begin(); iter != map_outputs.end(); ++iter) {
    //    tensorflow::TensorProto& result_tensor_proto = iter -> second;
    //    LOG(INFO) << "num of probs: " << result_tensor_proto.float_val_size();
    //    for(int i = 0; i < result_tensor_proto.float_val_size(); i ++) {
    //        LOG(INFO) << "idx: " << i << ", prob: " << result_tensor_proto.float_val(i);
    //    }
    //}


    return ret;
}

