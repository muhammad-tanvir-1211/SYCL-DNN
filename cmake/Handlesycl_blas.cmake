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
snn_include_guard(HANDLE_SYCLBLAS)

if(NOT SNN_DOWNLOAD_SYCLBLAS)
  find_package(sycl_blas)
  if(NOT sycl_blas_FOUND)
    find_package(sycl_blas NO_MODULE)
  endif()
endif()

if(NOT sycl_blas_FOUND AND (SNN_DOWNLOAD_SYCLBLAS OR SNN_DOWNLOAD_MISSING_DEPS))
  include(ExternalProject)
  set(sycl_blas_REPO "https://github.com/codeplaysoftware/sycl-blas.git" CACHE STRING
    "sycl_blas git repository to clone"
  )
  set(sycl_blas_GIT_TAG "1b002da" CACHE STRING
    "Git tag, branch or commit to use for the sycl_blas library"
  )
  set(sycl_blas_SOURCE_DIR ${sycldnn_BINARY_DIR}/sycl_blas-src)
  set(sycl_blas_BINARY_DIR ${sycldnn_BINARY_DIR}/sycl_blas-build)
  set(sycl_blas_LIBNAME ${CMAKE_SHARED_LIBRARY_PREFIX}sycl_blas${CMAKE_SHARED_LIBRARY_SUFFIX})
  set(sycl_blas_LIBRARY ${sycl_blas_BINARY_DIR}/${sycl_blas_LIBNAME})
  set(sycl_blas_BYPRODUCTS ${sycl_blas_LIBRARY})
  if(CMAKE_CROSSCOMPILING)
    set(cmake_toolchain "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
  endif()
  if(NOT TARGET sycl_blas_download)
    ExternalProject_Add(sycl_blas_download
      GIT_REPOSITORY    ${sycl_blas_REPO}
      GIT_TAG           ${sycl_blas_GIT_TAG}
      GIT_CONFIG        advice.detachedHead=false
      SOURCE_DIR        ${sycl_blas_SOURCE_DIR}
      BINARY_DIR        ${sycl_blas_BINARY_DIR}
      CMAKE_ARGS        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                        -DBUILD_SHARED_LIBS=ON
                        -DBLAS_ENABLE_TESTING=OFF
                        -DBLAS_ENABLE_BENCHMARK=OFF
                        -DBLAS_BUILD_SAMPLES=OFF
                        -DBLAS_ENABLE_CONST_INPUT=ON
                        -DComputeCpp_DIR=${ComputeCpp_DIR}
                        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
                        ${cmake_toolchain}
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
      BUILD_BYPRODUCTS ${sycl_blas_BYPRODUCTS}
    )
  endif()
  set(sycl_blas_INCLUDE_DIR
    ${sycl_blas_SOURCE_DIR}/include CACHE PATH
    "The sycl_blas include directory" FORCE
  )
  set(sycl_blas_VPTR_INCLUDE_DIR
    ${sycl_blas_SOURCE_DIR}/external/computecpp-sdk/include CACHE PATH
    "The sycl_blas virtual pointer include directory" FORCE
  )
  set(sycl_blas_INCLUDE_DIRS ${sycl_blas_INCLUDE_DIR} ${sycl_blas_VPTR_INCLUDE_DIR})
  # Have to explicitly make the include directories to add it to the library
  # target. This will be filled with the headers at build time when the
  # library is downloaded.
  file(MAKE_DIRECTORY ${sycl_blas_INCLUDE_DIR})
  file(MAKE_DIRECTORY ${sycl_blas_VPTR_INCLUDE_DIR})

  if(NOT TARGET SYCL_BLAS::sycl_blas)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(sycl_blas
      FOUND_VAR sycl_blas_FOUND
      REQUIRED_VARS sycl_blas_LIBRARY
                    sycl_blas_INCLUDE_DIR
                    sycl_blas_VPTR_INCLUDE_DIR
    )
  
    mark_as_advanced(sycl_blas_FOUND
                    sycl_blas_LIBRARY
                    sycl_blas_VPTR_INCLUDE_DIR
                    sycl_blas_INCLUDE_DIR
    )
    add_library(SYCL_BLAS::sycl_blas SHARED IMPORTED)
    set_target_properties(SYCL_BLAS::sycl_blas PROPERTIES
      IMPORTED_LOCATION "${sycl_blas_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${sycl_blas_INCLUDE_DIRS}"
    )
    add_dependencies(SYCL_BLAS::sycl_blas sycl_blas_download)
  endif()
  mark_as_advanced(sycl_blas_REPO sycl_blas_GIT_TAG)
endif()

if(NOT sycl_blas_FOUND)
  message(FATAL_ERROR
    "Could not find sycl_blas, consider setting SNN_DOWNLOAD_MISSING_DEPS")
endif()
