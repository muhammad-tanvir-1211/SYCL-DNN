/*
 * Copyright Codeplay Software Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use these files except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYCLDNN_TEST_SOFTMAX_SOFTMAX_FIXTURE_H_
#define SYCLDNN_TEST_SOFTMAX_SOFTMAX_FIXTURE_H_

#include <gtest/gtest.h>

#include "sycldnn/helpers/scope_exit.h"

#include "sycldnn/softmax/direction.h"
#include "sycldnn/softmax/launch.h"
#include "sycldnn/softmax/params.h"

#include "test/backend/backend_test_fixture.h"
#include "test/gen/iota_initialised_data.h"
#include "test/helpers/float_comparison.h"

inline sycldnn::softmax::SoftmaxParams getSoftmaxParams(
    std::array<int, 4> in_shape, sycldnn::DataFormat data_format) {
  sycldnn::softmax::SoftmaxParams params;
  params.channels = in_shape[3];
  params.batch = in_shape[0];
  params.rows = in_shape[1];
  params.cols = in_shape[2];
  params.input_format = data_format;
  return params;
}

template <typename Pair, typename Direction>
struct SoftmaxFixture;

template <typename Pair>
struct SoftmaxFixture<Pair, sycldnn::softmax::Forward>
    : public BackendTestFixture<typename Pair::SecondType> {
  using DataType = typename Pair::FirstType;
  using Backend = typename Pair::SecondType;

  void test_softmax(std::vector<DataType> const& exp,
                    sycldnn::softmax::SoftmaxParams const& params,
                    DataType max_val = static_cast<DataType>(0)) {
    auto input_size =
        params.batch * params.rows * params.cols * params.channels;
    auto workspace_size = params.batch * params.rows * params.cols;
    ASSERT_EQ(input_size, exp.size());
    const auto size = exp.size();

    std::vector<DataType> input;
    if (max_val == DataType(0)) {
      input.reserve(size);
      std::generate_n(std::back_inserter(input), input_size,
                      [&max_val] { return max_val; });
    } else {
      input = iota_initialised_data<DataType>(input_size, max_val);
    }
    std::vector<DataType> output(size);
    std::vector<DataType> workspace(workspace_size);

    auto& provider = this->provider_;
    auto& backend = provider.get_backend();

    auto inp_gpu = provider.get_initialised_device_memory(input_size, input);
    auto workspace_gpu =
        provider.get_initialised_device_memory(workspace_size, workspace);
    auto out_gpu = provider.get_initialised_device_memory(size, output);
    SNN_ON_SCOPE_EXIT {
      provider.deallocate_ptr(inp_gpu);
      provider.deallocate_ptr(out_gpu);
      provider.deallocate_ptr(workspace_gpu);
    };

    auto status = sycldnn::softmax::launch<DataType, sycldnn::softmax::Forward>(
        inp_gpu, workspace_gpu, out_gpu, params, backend);

    ASSERT_EQ(sycldnn::StatusCode::OK, status.status);
    status.event.wait_and_throw();

    provider.copy_device_data_to_host(size, out_gpu, output);

    for (size_t i = 0; i < size; ++i) {
      SCOPED_TRACE("Element: " + std::to_string(i));
      SNN_ALMOST_EQUAL(exp[i], output[i], 10u);
    }
  }
};

template <typename Pair>
struct SoftmaxFixture<Pair, sycldnn::softmax::Gradient>
    : public BackendTestFixture<typename Pair::SecondType> {
  using DataType = typename Pair::FirstType;
  using Backend = typename Pair::SecondType;

  void test_softmax(std::vector<DataType> const& exp,
                    sycldnn::softmax::SoftmaxParams const& params,
                    DataType max_val = static_cast<DataType>(0)) {
    auto input_size =
        params.batch * params.rows * params.cols * params.channels;
    auto workspace_size = params.batch * params.rows * params.cols;
    ASSERT_EQ(input_size, exp.size());
    const auto size = exp.size();

    std::vector<DataType> input;
    if (max_val == DataType(0)) {
      input.reserve(size);
      std::generate_n(std::back_inserter(input), size,
                      [&max_val] { return max_val; });
    } else {
      input = iota_initialised_data<DataType>(size, max_val);
    }
    std::vector<DataType> output(size);
    std::vector<DataType> workspace_fwd(workspace_size);
    std::vector<DataType> workspace_grad(size);

    auto& provider = this->provider_;
    auto& backend = provider.get_backend();

    auto inp_gpu = provider.get_initialised_device_memory(size, input);
    auto workspace_fwd_gpu =
        provider.get_initialised_device_memory(workspace_size, workspace_fwd);
    auto workspace_grad_gpu =
        provider.get_initialised_device_memory(size, workspace_grad);
    auto out_fwd_gpu = provider.get_initialised_device_memory(size, output);
    auto out_grad_gpu = provider.get_initialised_device_memory(size, output);
    SNN_ON_SCOPE_EXIT {
      provider.deallocate_ptr(inp_gpu);
      provider.deallocate_ptr(out_fwd_gpu);
      provider.deallocate_ptr(out_grad_gpu);
      provider.deallocate_ptr(workspace_fwd_gpu);
      provider.deallocate_ptr(workspace_grad_gpu);
    };

    auto status = sycldnn::softmax::launch<DataType, sycldnn::softmax::Forward>(
        inp_gpu, workspace_fwd_gpu, out_fwd_gpu, params, backend);

    status.event.wait_and_throw();

    status = sycldnn::softmax::launch<DataType, sycldnn::softmax::Gradient>(
        out_fwd_gpu, inp_gpu, workspace_grad_gpu, out_grad_gpu, params,
        backend);

    ASSERT_EQ(sycldnn::StatusCode::OK, status.status);
    status.event.wait_and_throw();

    provider.copy_device_data_to_host(size, out_grad_gpu, output);

    for (size_t i = 0; i < size; ++i) {
      SCOPED_TRACE("Element: " + std::to_string(i));
      SNN_ALMOST_EQUAL(exp[i], output[i], 10u);
    }
  }
};

#endif  // SYCLDNN_TEST_SOFTMAX_SOFTMAX_FIXTURE_H_
