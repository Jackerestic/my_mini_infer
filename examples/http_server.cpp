#include "mini_infer/mlp.hpp"
#include "mini_infer/tensor.hpp"
#include "mini_infer/tensor_io.hpp"
#include "httplib.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include<sstream>
#include<vector>

using mini_infer::Mlp;
using mini_infer::Tensor;


int main()
{
    std::map<std::string, Tensor> inputs;
    if (!mini_infer::load_tensors("data/mlp.txt", inputs)) {
        std::cout << "failed to load data/mlp.txt\n";
        return 1;
    }

    Mlp model(inputs.at("W1"), inputs.at("b1"),
              inputs.at("W2"), inputs.at("b2"));

    const std::size_t input_dim=inputs.at("W1").cols();
    
    httplib::Server svr;
    svr.Get("/health",[](const httplib::Request&,httplib::Response& res){
        res.set_content("ok","text/plain");
    });

    svr.Post("/predict",
             [&model,input_dim](const httplib::Request& req,
                                httplib::Response& res){
                                    std::istringstream body(req.body);
                                    std::string line;
                                    std::vector<std::vector<float>> rows;

                                    while(std::getline(body,line)){
                                        if(line.empty())continue;

                                        std::istringstream row_stream(line);
                                        std::vector<float> row;
                                        float value;
                                        while(row_stream>>value){
                                            row.push_back(value);
                                        }

                                        if(row.empty()||row.size()!=input_dim){
                                            res.status=400;
                                            res.set_content("bad request","text/plain");
                                            return;
                                        }

                                        rows.push_back(row);
                                    }

                                    if(rows.empty()){
                                        res.status=400;
                                        res.set_content("empty request","text/plain");
                                        return;
                                    }

                                    Tensor x(rows.size(),input_dim);
                                    for(std::size_t r=0;r<rows.size();++r){
                                        for(std::size_t c=0;c<input_dim;++c){
                                            x.at(r,c)=rows[r][c];
                                        }
                                    }

                                    Tensor y=model.forward(x);

                                    std::ostringstream out;
                                    for(std::size_t r=0;r<y.rows();++r){
                                        for(std::size_t c=0;c<y.cols();++c){
                                            out<<y.at(r,c);
                                            if(c+1==y.cols()) out<<'\n';
                                            else out<<' ';
                                        }
                                    }

                                    res.set_content(out.str(),"text/plain");
                                });
    
    std::cout<<"server listening on 127.0.0.1:8080\n";
    svr.listen("127.0.0.1",8080);   
    
    return 0;
}