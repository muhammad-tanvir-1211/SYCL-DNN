#!python
#
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

from generate_conv2d_tests import generate_conv2d_tests
from generate_depthwise_conv2d_tests import generate_depthwise_conv2d_tests
from generate_matmul_tests import generate_matmul_tests
from generate_pooling_tests import generate_pooling_tests, generate_fastdiv_tests
from generate_pointwise_tests import generate_pointwise_tests
from generate_bias_tests import generate_bias_tests
from generate_batchnorm_tests import generate_batchnorm_tests
from generate_softmax_tests import generate_softmax_tests
from generate_transpose_tests import generate_transpose_tests
from generate_reduce_tests import generate_reduce_tests


def generate_all():
    generate_conv2d_tests()
    generate_depthwise_conv2d_tests()
    generate_matmul_tests()
    generate_pooling_tests()
    generate_fastdiv_tests()
    generate_pointwise_tests()
    generate_softmax_tests()
    generate_transpose_tests()
    generate_bias_tests()
    generate_batchnorm_tests()
    generate_reduce_tests()


if __name__ == "__main__":
    generate_all()
