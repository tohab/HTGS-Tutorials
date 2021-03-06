
// NIST-developed software is provided by NIST as a public service. You may use, copy and distribute copies of the software in any medium, provided that you keep intact this entire notice. You may improve, modify and create derivative works of the software or any portion of the software, and you may copy and distribute such modifications or works. Modified works should carry a notice stating that you changed the software and should note the date and nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the source of the software.
// NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
// You are solely responsible for determining the appropriateness of using and distributing the software and you assume all risks associated with its use, including but not limited to the risks and costs of program errors, compliance with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or interruption of operation. This software is not intended to be used in any situation where a failure could cause risk of injury or damage to property. The software developed by NIST employees is not subject to copyright protection within the United States.

//
// Created by tjb3 on 6/15/16.
//

#ifndef HTGS_TUTORIALS_MATRIXCOPYINFACTORTASK_H
#define HTGS_TUTORIALS_MATRIXCOPYINFACTORTASK_H

#include <htgs/api/ICudaTask.hpp>
#include "../data/MatrixFactorCudaData.h"
#include "../memory/MatrixMemoryRule.h"
#include <cuda.h>
#include <cublas_v2.h>

class MatrixCopyInFactorTask : public htgs::ICudaTask<MatrixBlockData<double *>, MatrixBlockData<MatrixMemoryData_t>> {
 public:
  MatrixCopyInFactorTask(int blockSize, CUcontext *contexts, int *cudaIds, int numGpus, long leadingDimensionFullMatrix, int numBlocksWidth, std::string memoryEdge) :
      ICudaTask(contexts, cudaIds, numGpus),
      blockSize(blockSize),
      leadingDimensionFullMatrix(leadingDimensionFullMatrix),
      numBlocksWidth(numBlocksWidth),
      memoryEdge(memoryEdge){}

  virtual void
  initializeCudaGPU(CUcontext context, CUstream stream, int cudaId, int numGPUs, int pipelineId, int numPipelines) {
  }

  virtual void executeGPUTask(std::shared_ptr<MatrixBlockData<double *>> data, CUstream stream) {

    // CPU Memory
    double *memoryIn = data->getMatrixData();

    // Cuda Memory
    int releaseCount = numBlocksWidth - (data->getRequest()->getCol() +1);
    auto memoryOut = this->memGet<double *>(memoryEdge, new MatrixMemoryRule(releaseCount));

    cublasSetMatrixAsync((int) data->getMatrixHeight(), (int) data->getMatrixWidth(), sizeof(double),
                         memoryIn, (int) leadingDimensionFullMatrix,
                         memoryOut->get(), (int) data->getMatrixHeight(), stream);

    auto retData = new MatrixBlockData<MatrixMemoryData_t>(data->getRequest(), memoryOut, data->getMatrixWidth(), data->getMatrixHeight());
    this->syncStream();

    this->addResult(retData);
  }

  virtual void shutdownCuda() {
  }

  virtual std::string getName() {
    return "CudaCopyInFactorTask";
  }

  virtual MatrixCopyInFactorTask *copy() {
    return new MatrixCopyInFactorTask(
                                this->blockSize,
                                this->getContexts(),
                                this->getCudaIds(),
                                this->getNumGPUs(),
                                this->leadingDimensionFullMatrix,
                                this->numBlocksWidth,
                                this->memoryEdge);
  }

 private:
  int blockSize;
  long leadingDimensionFullMatrix;
  int numBlocksWidth;
  std::string memoryEdge;
};

#endif //HTGS_TUTORIALS_MATRIXCOPYINFACTORTASK_H
