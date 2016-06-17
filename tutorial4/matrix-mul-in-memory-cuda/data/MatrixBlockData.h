//
// Created by tjb3 on 2/23/16.
//

#ifndef HTGS_MATRIXBLOCKDATA_H
#define HTGS_MATRIXBLOCKDATA_H

#include <htgs/api/MemoryData.hpp>

typedef std::shared_ptr<htgs::MemoryData<double *>> MatrixMemoryData_t;

template <class T>
class MatrixBlockData : public htgs::IData
{
 public:


  MatrixBlockData(std::shared_ptr<MatrixRequestData> request,
                  T matrixData,
                  long matrixWidth,
                  long matrixHeight) :
  request(request), matrixData(matrixData), matrixWidth(matrixWidth), matrixHeight(matrixHeight) { }

  std::shared_ptr<MatrixRequestData> getRequest() const {
    return request;
  }
  T getMatrixData() const {
    return matrixData;
  }
  long getMatrixWidth() const {
    return matrixWidth;
  }
  long getMatrixHeight() const {
    return matrixHeight;
  }

 private:
  std::shared_ptr<MatrixRequestData> request;
  T matrixData;
  long matrixWidth;
  long matrixHeight;
};
#endif //HTGS_MATRIXBLOCKDATA_H