# Copyright Codeplay Software Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use these files except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.10.2)
include(SNNHelpers)

add_subdirectory(conv2d)
add_subdirectory(depthwise_conv2d)
add_subdirectory(matmul)
add_subdirectory(pointwise)
add_subdirectory(pooling)
add_subdirectory(binaryop)
add_subdirectory(transpose)
add_subdirectory(batchnorm)
add_subdirectory(roi_align)
add_subdirectory(reduce)
