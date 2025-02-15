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

#include "sycldnn/mem_object.h"

#include "sycldnn/pooling/operators.h"
#include "sycldnn/pooling/params.h"
#include "sycldnn/pooling/sizes.h"

#include "sycldnn/internal/pooling/launch_internal.h"

#include "src/pooling/can_fastdiv.h"
#include "src/pooling/can_vectorize.h"
#include "src/pooling/kernels.h"
#include "src/pooling/queue_pooling_kernel.h"

#include <CL/sycl.hpp>

#include "sycldnn/export.h"

namespace sycldnn {
namespace pooling {
namespace internal {

template <typename T, typename Index, template <typename> class PoolType,
          typename Direction, int VectorWidth>
SNNStatus launch_with_vector_size(BaseMemObject<T const>& input,
                                  BaseMemObject<T>& output,
                                  const PoolingParams& pp, size_t threads,
                                  cl::sycl::queue& queue) {
  threads /= VectorWidth;
  if (can_use_fastdiv<Direction>(pp, VectorWidth)) {
    return queue_pooling<T, Index, PoolType, Direction, VectorWidth, true>(
        input, output, pp, threads, queue);
  } else {
    return queue_pooling<T, Index, PoolType, Direction, VectorWidth, false>(
        input, output, pp, threads, queue);
  }
}

template <typename T, typename Index, template <typename> class PoolType,
          typename Direction>
SNNStatus launch_with_index(BaseMemObject<T const>& input,
                            BaseMemObject<T>& output, const PoolingParams& pp,
                            size_t threads, cl::sycl::queue& queue) {
  if (can_vectorize<Direction, PoolType>(pp, 4)) {
    return launch_with_vector_size<T, Index, PoolType, Direction, 4>(
        input, output, pp, threads, queue);
  } else if (can_vectorize<Direction, PoolType>(pp, 2)) {
    return launch_with_vector_size<T, Index, PoolType, Direction, 2>(
        input, output, pp, threads, queue);
  } else {
    return launch_with_vector_size<T, Index, PoolType, Direction, 1>(
        input, output, pp, threads, queue);
  }
}

template <typename T, template <typename> class PoolType, typename Direction,
          DisableIfMaxGradient<T, PoolType, Direction>>
SNNStatus launch_pooling(BaseMemObject<T const>& input,
                         BaseMemObject<T>& output, const PoolingParams& pp,
                         cl::sycl::queue& queue) {
  auto sizes = get_sizes<Direction>(pp);
  size_t threads = sizes.output_size;
  if (threads > std::numeric_limits<int32_t>::max()) {
#ifdef SNN_USE_INT64
    return launch_with_index<T, int64_t, PoolType, Direction>(input, output, pp,
                                                              threads, queue);
#else
    return StatusCode::IndexExceeded;
#endif  // SNN_USE_INT64
  } else {
    return launch_with_index<T, int32_t, PoolType, Direction>(input, output, pp,
                                                              threads, queue);
  }
}

#define INSTANTIATE_LAUNCH(DTYPE, OP, DIRECTION)                      \
  template SNN_EXPORT SNNStatus launch_pooling<DTYPE, OP, DIRECTION>( \
      BaseMemObject<DTYPE const> & inp_access,                        \
      BaseMemObject<DTYPE> & outp_access, const PoolingParams& pp,    \
      cl::sycl::queue& queue)

#define INSTANTIATE_FOR_TYPE(DTYPE)               \
  INSTANTIATE_LAUNCH(DTYPE, Max, Forward);        \
  INSTANTIATE_LAUNCH(DTYPE, MaxWithNan, Forward); \
  INSTANTIATE_LAUNCH(DTYPE, Average, Forward);    \
  INSTANTIATE_LAUNCH(DTYPE, Average, Backpropagate)

INSTANTIATE_FOR_TYPE(float);

#ifdef SNN_USE_HALF
INSTANTIATE_FOR_TYPE(cl::sycl::half);
#endif  // SNN_USE_HALF

#ifdef SNN_USE_DOUBLE
INSTANTIATE_FOR_TYPE(double);
#endif  // SNN_USE_DOUBLE

#undef INSTANTIATE_FOR_TYPE
#undef INSTANTIATE_LAUNCH

}  // namespace internal
}  // namespace pooling
}  // namespace sycldnn
