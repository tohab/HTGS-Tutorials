
// NIST-developed software is provided by NIST as a public service. You may use, copy and distribute copies of the software in any medium, provided that you keep size_tact this entire notice. You may improve, modify and create derivative works of the software or any portion of the software, and you may copy and distribute such modifications or works. Modified works should carry a notice stating that you changed the software and should note the date and nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the source of the software.
// NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
// You are solely responsible for determining the appropriateness of using and distributing the software and you assume all risks associated with its use, including but not limited to the risks and costs of program errors, compliance with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or size_terruption of operation. This software is not size_tended to be used in any situation where a failure could cause risk of injury or damage to property. The software developed by NIST employees is not subject to copyright protection within the United States.

//
// Created by tjb3 on 1/15/16.
//

#ifndef HTGS_UTIL_MATRIX_H_H
#define HTGS_UTIL_MATRIX_H_H

#include <complex>
#include <iostream>
#include <fstream>
#include "enums/MatrixType.h"

#define IDX2C(r, c, ld) (((c)*(ld))+(r))

double *allocMatrix(long width, long height);
void initMatrix(double *matrix, long width, long height, bool columnStore);
void initMatrixDiagDom(double * matrix, long width, long height, bool columnStore);

std::string generateFilename(std::string directory, MatrixType type, size_t blockRow, size_t blockCol, std::string suffix);
std::string generateDirectoryName(std::string basePath, size_t totalWidth, size_t totalHeight, size_t blockSize);
std::string generateDirectoryName(std::string basePath, size_t totalWidth, size_t totalHeight);
int generateMatrixBlockFiles(std::string path,
                             MatrixType type,
                             size_t totalWidth,
                             size_t totalHeight,
                             size_t blockSize,
                             bool columnStore);
int generateFullMatrixFile(std::string path, MatrixType type, size_t totalWidth, size_t totalHeight);

double *readMatrix(std::string path,
                   MatrixType type,
                   size_t totalWidth,
                   size_t totalHeight,
                   size_t blockSize,
                   size_t blockRow,
                   size_t blockCol,
                   std::string suffix);

size_t readMatrix(std::string path,
                  MatrixType type,
                  size_t totalWidth,
                  size_t totalHeight,
                  size_t blockSize,
                  size_t blockRow,
                  size_t blockCol,
                  double *&matrix,
                  std::string suffix);

size_t writeMatrix(std::string file, size_t matrixWidth, size_t matrixHeight, double *&matrix, bool silent);
bool checkMatrixBlockFiles(std::string path, MatrixType type, size_t totalWidth, size_t totalHeight, size_t blockSize);
void checkAndValidateMatrixBlockFiles(std::string directory,
                                      size_t widthA,
                                      size_t heightA,
                                      size_t widthB,
                                      size_t heightB,
                                      size_t blockSize,
                                      bool columnStore);
void checkAndValidateMatrixFiles(std::string directory, size_t widthA, size_t heightA, size_t widthB, size_t heightB);

#endif //HTGS_UTIL_MATRIX_H_H
