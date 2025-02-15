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
#include "sycldnn/internal/reduce/launch.h"
#include "sycldnn/reduce/operators.h"

#include "sycldnn/mem_object.h"

#include "src/reduce/queue_reduction.h"

namespace sycldnn {
namespace reduce {
namespace internal {

// Launch the reduce kernel for the passed parameters.
template <typename T, typename Op>
SNNStatus launch(BaseMemObject<T const>& input, BaseMemObject<T>& output,
                 int batches, int outer, int inner, cl::sycl::queue& queue) {
  return queue_kernel<T, int, Op>(input, output, batches, outer, inner, queue);
}

#define INSTANTIATE_LAUNCHER(DTYPE, OP)                                  \
  template SNN_EXPORT SNNStatus launch<DTYPE, OP>(                       \
      BaseMemObject<DTYPE const> & input, BaseMemObject<DTYPE> & output, \
      int batches, int outer, int inner, cl::sycl::queue& queue);

#define INSTANTIATE_FOR_TYPE(DTYPE) \
  INSTANTIATE_LAUNCHER(DTYPE, Add)  \
  INSTANTIATE_LAUNCHER(DTYPE, Mean)

INSTANTIATE_FOR_TYPE(float);

#ifdef SNN_USE_DOUBLE
INSTANTIATE_FOR_TYPE(double);
#endif  // SNN_USE_DOUBLE

#ifdef SNN_USE_HALF
INSTANTIATE_FOR_TYPE(cl::sycl::half);
#endif  // SNN_USE_HALF

#undef INSTANTIATE_FOR_TYPE
#undef INSTANTIATE_LAUNCHER

}  // namespace internal
}  // namespace reduce
}  // namespace sycldnn
