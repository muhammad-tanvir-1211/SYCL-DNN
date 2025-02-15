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
#include <gtest/gtest.h>
#include <vector>

#include "test/transpose/transpose_fixture.h"
#include "test/types/kernel_data_types.h"
#include "test/types/to_gtest_types.h"

using DataTypeList = sycldnn::types::KernelDataTypes;
using GTestTypeList = sycldnn::types::ToGTestTypes<DataTypeList>::type;

template <typename DataType>
using TranposeOffsets = TransposeFixture<DataType>;
TYPED_TEST_SUITE(TranposeOffsets, GTestTypeList);

TYPED_TEST(TranposeOffsets, Offsets_2D) {
  using DataType = typename TestFixture::DataType;
  const std::vector<DataType> exp_out = {
      1.,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9.,  10., 11., 12., 13., 14.,
      15., 16., 17., 21., 25., 18., 22., 26., 19., 23., 27., 20., 24., 28.};
  const std::vector<int> sizes = {3, 4};
  const std::vector<int> perm = {1, 0};
  const DataType max_input_val = 2048.0;
  this->run(exp_out, sizes, perm, max_input_val, 16, 16);
}
TYPED_TEST(TranposeOffsets, DISABLED_Offsets_2D_NoOutputOffset_NoPerm) {
  // This test highlights a bug in ComputeCpp 1.0.3 when copying reinterpreted
  // buffers. The size of the original type is used in the copy rather than the
  // size of the reinterpreted type.
  // TODO: Re-enable this tests once underlying problem is fixed.
  using DataType = typename TestFixture::DataType;
  const std::vector<DataType> exp_out = {17., 18., 19., 20., 21., 22.,
                                         23., 24., 25., 26., 27., 28.};
  const std::vector<int> sizes = {3, 4};
  const std::vector<int> perm = {0, 1};
  const DataType max_input_val = 2048.0;
  this->run(exp_out, sizes, perm, max_input_val, 16, 0);
}
TYPED_TEST(TranposeOffsets, Offsets_2D_NoOutputOffset_WithPerm) {
  using DataType = typename TestFixture::DataType;
  const std::vector<DataType> exp_out = {17., 21., 25., 18., 22., 26.,
                                         19., 23., 27., 20., 24., 28.};
  const std::vector<int> sizes = {3, 4};
  const std::vector<int> perm = {1, 0};
  const DataType max_input_val = 2048.0;
  this->run(exp_out, sizes, perm, max_input_val, 16, 0);
}
