/*
 * Copyright Codeplay Software Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
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
#ifndef SYCLDNN_SRC_TRANSPOSE_QUEUE_KERNEL_H_
#define SYCLDNN_SRC_TRANSPOSE_QUEUE_KERNEL_H_

#include "sycldnn/mem_object.h"
#include "sycldnn/status.h"

namespace sycldnn {
namespace transpose {
namespace internal {

template <typename T, typename Index, int ND>
SNNStatus queue_kernel(BaseMemObject<T const>& input, BaseMemObject<T>& output,
                       std::vector<int> const& dimensions,
                       std::vector<int> const& permutation,
                       cl::sycl::queue& queue);

}  // namespace internal
}  // namespace transpose
}  // namespace sycldnn

#endif  // SYCLDNN_SRC_TRANSPOSE_QUEUE_KERNEL_H_
