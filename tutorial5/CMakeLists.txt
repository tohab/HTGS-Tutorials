
# NIST-developed software is provided by NIST as a public service. You may use, copy and distribute copies of the software in any medium, provided that you keep intact this entire notice. You may improve, modify and create derivative works of the software or any portion of the software, and you may copy and distribute such modifications or works. Modified works should carry a notice stating that you changed the software and should note the date and nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the source of the software.
# NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
# You are solely responsible for determining the appropriateness of using and distributing the software and you assume all risks associated with its use, including but not limited to the risks and costs of program errors, compliance with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or interruption of operation. This software is not intended to be used in any situation where a failure could cause risk of injury or damage to property. The software developed by NIST employees is not subject to copyright protection within the United States.

include_directories(${OpenBLAS_INCLUDE_DIR})
link_directories(${OpenBLAS_LIBRARIES})

set(TUTORIAL_UTILS_SRC
        ../../tutorial-utils/util-matrix.h
        ../../tutorial-utils/util-matrix.cpp
        ../../tutorial-utils/util-filesystem.cpp
        ../../tutorial-utils/util-filesystem.h
        ../../tutorial-utils/util-cuda.h
        ../../tutorial-utils/util-cuda.cpp
        ../../tutorial-utils/SimpleClock.h
        ../../tutorial-utils/enums/MatrixType.h
        ../../tutorial-utils/enums/MatrixType.cpp
        ../tutorial-utils/matrix-library/data/MatrixBlockData.h
        ../tutorial-utils/matrix-library/data/MatrixRequestData.h
        ../tutorial-utils/matrix-library/data/MatrixBlockMulData.h
        ../tutorial-utils/matrix-library/tasks/LoadMatrixTask.h
        ../tutorial-utils/matrix-library/operations/matmul.cpp
        ../tutorial-utils/matrix-library/operations/matmul.h
        ../tutorial-utils/matrix-library/args/MatMulArgs.h
        ../tutorial-utils/matrix-library/args/MatMulArgs.cpp
        ../tutorial-utils/matrix-library/tasks/CudaCopyInTask.h
        ../tutorial-utils/matrix-library/tasks/CudaCopyOutTask.h
        ../tutorial-utils/matrix-library/allocator/CudaAllocator.h
        )

set(CUDA_SRC
        rules/MatrixDecompositionRule.h
        )

cuda_add_executable(tutorial5-multigpu-matrixMul
        tutorial5-multigpu-cuda-matrix-mul.cpp
        #        tasks/ReadMatrixTask.h
        #        tasks/OutputTask.h
        #        tasks/MatrixAccumTask.h
        #        memory/MatrixMemoryRule.h
        #        memory/MatrixAllocator.h
        #        memory/CudaMatrixAllocator.h
        #        rules/MatrixLoadRule.h
        #        rules/MatrixDistributeRule.h
        #        rules/MatrixAccumulateRule.h
        #        rules/MatrixOutputRule.h
        #        tasks/MatrixMulBlkTask.h
        #        tasks/OutputTask.h
        #        tasks/MatrixCopyInTask.h
        #        tasks/MatrixCopyOutTask.h
        ${TUTORIAL_UTILS_SRC}
        ${CUDA_SRC}
        )

target_compile_definitions(tutorial5-multigpu-matrixMul PUBLIC -DUSE_CUDA)
cuda_add_cublas_to_target(tutorial5-multigpu-matrixMul)
target_link_libraries(tutorial5-multigpu-matrixMul ${CUDA_LIBRARIES})
target_link_libraries(tutorial5-multigpu-matrixMul cuda)
