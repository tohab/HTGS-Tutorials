//
// Created by tjb3 on 2/23/16.
//

#ifndef HTGS_MATRIXBLOCKMULDATA_H
#define HTGS_MATRIXBLOCKMULDATA_H

#include <htgs/api/IData.hpp>
#include "MatrixBlockData.h"

template<class T>
class MatrixBlockMulData : public htgs::IData {
 public:

  MatrixBlockMulData(std::shared_ptr<MatrixBlockData<T>> matrixA,
                     std::shared_ptr<MatrixBlockData<T>> matrixB) :
      matrixA(matrixA), matrixB(matrixB) {}

  std::shared_ptr<MatrixBlockData<T>> getMatrixA() const {
    return matrixA;
  }

  std::shared_ptr<MatrixBlockData<T>> getMatrixB() const {
    return matrixB;
  }

 private:
  std::shared_ptr<MatrixBlockData<T>> matrixA;
  std::shared_ptr<MatrixBlockData<T>> matrixB;
};

#endif //HTGS_MATRIXBLOCKMULDATA_H