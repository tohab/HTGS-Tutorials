//
// Created by tjb3 on 2/23/16.
//
//#define DEBUG_FLAG
//#define DEBUG_LEVEL_VERBOSE
#include <htgs/api/TaskGraph.hpp>
#include <htgs/api/Runtime.hpp>
#include <string.h>
#include <sys/stat.h>

#include "data/MatrixRequestData.h"
#include "data/MatrixBlockData.h"
#include "tasks/ReadMatrixTask.h"
#include "memory/MatrixAllocator.h"
#include "rules/MatrixLoadRule.h"
#include "tasks/MatrixMulBlkTask.h"
#include "tasks/MatrixAccumTask.h"
#include "tasks/OutputTask.h"
#include "rules/MatrixAccumulateRule.h"
#include "rules/MatrixDistributeRule.h"
#include "rules/MatrixLoopRule.h"
#include "rules/MatrixOutputRule.h"
#include "../../tutorial-utils/SimpleClock.h"
#include "../../tutorial-utils/util-matrix.h"
#include "../../tutorial-utils/util-filesystem.h"

int validateResults(std::string baseDirectory, int matrixAHeight, int matrixBWidth, int blockSize)
{
  int blkHeightMatA = (int)ceil((double)matrixAHeight / (double)blockSize);
  int blkWidthMatB = (int)ceil((double)matrixBWidth / (double)blockSize);

  for (int row = 0; row < blkHeightMatA; row++)
  {
    for (int col = 0; col < blkWidthMatB; col++)
    {
      int matrixAHeight = (row == blkHeightMatA-1 && matrixAHeight % blockSize != 0) ? matrixAHeight % blockSize : blockSize;
      int matrixBWidth = (col == blkWidthMatB-1 && matrixBWidth % blockSize != 0) ? matrixBWidth % blockSize : blockSize;
      std::string fileName(baseDirectory + "/matrixC/" + std::to_string(row) + "_" + std::to_string(col));
      std::string fileNamePar(baseDirectory + "/matrixC_HTGS/" + std::to_string(row) + "_" + std::to_string(col));

      std::ifstream c(fileName, std::ios::binary);
      std::ifstream cPar(fileNamePar, std::ios::binary);

      double *cMem = new double[matrixAHeight*matrixBWidth];
      double *cMemPar = new double[matrixAHeight*matrixBWidth];

      c.read((char *)cMem, sizeof(double)*matrixAHeight*matrixBWidth);
      cPar.read((char *)cMemPar, sizeof(double)*matrixAHeight*matrixBWidth);

      int count = 0;
      for (int i = 0; i < matrixAHeight*matrixBWidth; i++)
      {
        if (cMem[i] != cMemPar[i]) {
          std::cout << i << ": cMem = " << cMem[i] << " cMemPar = " << cMemPar[i] << std::endl;
          if (count == 20) {
            return -1;
          }
           count++;
        }
      }


    }
  }

  return 0;
}

void computeSequentialMatMul(std::string directoryA, std::string directoryB, std::string outputDirectory, int fullMatrixAHeight, int fullMatrixAWidth, int fullMatrixBWidth, int blockSize)
{
  std::string matrixCDir(outputDirectory + "/matrixC");
  create_dir(matrixCDir);
  int blkHeightMatA = (int)ceil((double)fullMatrixAHeight / (double)blockSize);
  int blkWidthMatA = (int)ceil((double)fullMatrixAWidth / (double)blockSize);
  int blkHeightMatB = blkWidthMatA;
  int blkWidthMatB = (int)ceil((double)fullMatrixBWidth / (double)blockSize);

  double ***matALookup = new double**[blkHeightMatA];
  for (int i = 0; i < blkHeightMatA; i++)
  {
    matALookup[i] = new double*[blkWidthMatA];
  }

  double ***matBLookup = new double**[blkHeightMatB];
  for (int i = 0; i < blkHeightMatB; i++)
  {
    matBLookup[i] = new double*[blkWidthMatB];
  }

  for (int i = 0; i < blkHeightMatA; i++)
  {
    for (int j = 0; j < blkWidthMatA; j++)
    {
      matALookup[i][j] = nullptr;
    }
  }

  for (int i = 0; i < blkHeightMatB; i++)
  {
    for (int j = 0; j < blkWidthMatB; j++)
    {
      matBLookup[i][j] = nullptr;
    }
  }



  for (int blkRowA = 0; blkRowA < blkHeightMatA; blkRowA++)
  {
    for (int blkColB = 0; blkColB < blkWidthMatB; blkColB++)
    {
      int matrixAHeight = (blkRowA == blkHeightMatA-1 && fullMatrixAHeight % blockSize != 0) ? fullMatrixAHeight % blockSize : blockSize;
      int matrixBWidth = (blkColB == blkWidthMatB-1 && fullMatrixBWidth % blockSize != 0) ? fullMatrixBWidth % blockSize : blockSize;

      std::string matrixCFile(matrixCDir + "/" + std::to_string(blkRowA) + "_" + std::to_string(blkColB));

      double *finalResultC = new double[matrixAHeight*matrixBWidth];
      memset(finalResultC, 0, sizeof(double) *matrixAHeight * matrixBWidth);
      // matrix C . . .
      for (int blk = 0; blk < blkWidthMatA; blk++)
      {
        // Read A and B
        int matrixAWidth = (blk == blkWidthMatA-1 && fullMatrixAWidth % blockSize != 0) ? fullMatrixAWidth % blockSize : blockSize;

        double *matrixA;
        double *matrixB;
        if (matALookup[blkRowA][blk] == nullptr)
        {
          matrixA = new double[matrixAHeight*matrixAWidth];
          std::string matrixAFile(directoryA + "/MatrixA/" + std::to_string(blkRowA) + "_" + std::to_string(blk));
          std::ifstream fileA(matrixAFile, std::ios::binary);
          fileA.read((char *)matrixA, sizeof(double) * matrixAHeight * matrixAWidth);
          matALookup[blkRowA][blk] = matrixA;
        }
        else
        {
          matrixA = matALookup[blkRowA][blk];
        }


        if (matBLookup[blk][blkColB] == nullptr)
        {
          matrixB = new double[matrixBWidth*matrixAWidth];
          std::string matrixBFile(directoryB + "/MatrixB/" + std::to_string(blk) + "_" + std::to_string(blkColB));
          std::ifstream fileB(matrixBFile, std::ios::binary);
          fileB.read((char *)matrixB, sizeof(double) * matrixBWidth * matrixAWidth);
          matBLookup[blk][blkColB] = matrixB;
        }
        else
        {
          matrixB = matBLookup[blk][blkColB];
        }

        std::cout << "Seq Computing A(" << blkRowA << ", " << blk << ") x B(" << blk << ", " << blkColB << ") = C(" <<blkRowA << ", "<< blkColB << ")" <<std::endl;


        for (int i = 0; i < matrixAHeight; i++)
        {
          for (int j = 0; j < matrixBWidth; j++)
          {
            double sum = 0.0;
            for (int k = 0; k < matrixAWidth; k++)
            {
              sum += matrixA[i*matrixAWidth+k] * matrixB[k*matrixBWidth+j];

            }
            finalResultC[i * matrixBWidth + j] += sum;
          }

        }
      }

      std::ofstream fileC(matrixCFile, std::ios::binary);
      fileC.write((char *)finalResultC, sizeof(double) * matrixAHeight * matrixBWidth);

      delete [] finalResultC;
    }
  }

  for (int i = 0; i < blkHeightMatA; i++)
  {
    for (int j = 0; j < blkWidthMatA; j++)
    {
      delete [] matALookup[i][j];
    }
    delete [] matALookup[i];
  }

  delete []matALookup;

  for (int i = 0; i < blkHeightMatB; i++)
  {
    for (int j = 0; j < blkWidthMatB; j++)
    {
      delete [] matBLookup[i][j];
    }
    delete [] matBLookup[i];
  }

  delete []matBLookup;
}

int main(int argc, char *argv[])
{
  int matrixAHeight = 1024;
  int matrixBWidth = 1024;
  int sharedDim = 1024;

  int blockSize = 512;
  int numReadThreads = 1;
  int numProdThreads = 20;
  std::string directory("data");
  std::string outputDirectory(directory);
  bool runSequential = false;
  bool validate = false;

  for (int arg = 1; arg < argc; arg++)
  {
    std::string argvs(argv[arg]);

    if (argvs == "--width-b")
    {
      arg++;
      matrixBWidth= atoi(argv[arg]);
    }

    if (argvs == "--height-a")
    {
      arg++;
      matrixAHeight = atoi(argv[arg]);
    }

    if (argvs == "--shared-dim")
    {
      arg++;
      sharedDim = atoi(argv[arg]);
    }

    if (argvs == "--block-size")
    {
      arg++;
      blockSize = atoi(argv[arg]);
    }

    if (argvs == "--num-readers")
    {
      arg++;
      numReadThreads = atoi(argv[arg]);
    }

    if (argvs == "--num-workers")
    {
      arg++;
      numProdThreads = atoi(argv[arg]);
    }

    if (argvs == "--dir")
    {
      arg++;
      directory = argv[arg];
    }

    if (argvs == "--output-dir")
    {
      arg++;
      outputDirectory = argv[arg];
    }

    if (argvs == "--validate-results") {
      validate = true;
    }

    if (argvs == "--run-sequential") {
      runSequential = true;
    }

    if (argvs == "--help")
    {
      std::cout << argv[0] << " args: [--width-b <#>] [--height-a <#>] [--shared-dim <#>] [--block-size <#>] [--num-readers <#>] [--num-workers <#>] [--dir <dir>] [--output-dir <dir>] [--validate-results] [--run-sequential] [--help]" << std::endl;
      exit(0);
    }
  }

  create_dir(outputDirectory);

  checkAndValidateMatrixBlockFiles(directory, sharedDim, matrixAHeight, matrixBWidth, sharedDim, blockSize);

  std::string inputDirectoryA = generateDirectoryName(directory, sharedDim, matrixAHeight, blockSize);
  std::string inputDirectoryB = generateDirectoryName(directory, matrixBWidth, sharedDim, blockSize);


  SimpleClock clk;

  if (runSequential) {
    clk.start();
    computeSequentialMatMul(inputDirectoryA, inputDirectoryB, outputDirectory, matrixAHeight, sharedDim, matrixBWidth, blockSize);
  }
  else {

    ReadMatrixTask
        *readAMatTask = new ReadMatrixTask(numReadThreads, blockSize, sharedDim, matrixAHeight, inputDirectoryA, "A");
    ReadMatrixTask
        *readBMatTask = new ReadMatrixTask(numReadThreads, blockSize, matrixBWidth, sharedDim, inputDirectoryB, "B");
    MatrixMulBlkTask *mmulTask = new MatrixMulBlkTask(numProdThreads);
    MatrixAccumTask *accumTask = new MatrixAccumTask(numProdThreads / 2);

    OutputTask *outputTask = new OutputTask(outputDirectory);

    int blkHeightMatB = readBMatTask->getNumBlocksRows();
    int blkWidthMatB = readBMatTask->getNumBlocksCols();

    int blkHeightMatA = readAMatTask->getNumBlocksRows();
    int blkWidthMatA = readAMatTask->getNumBlocksCols();

    std::cout << "matA: " << blkHeightMatA << ", " << blkWidthMatA << " :: matB: " << blkHeightMatB << ", " << blkWidthMatB << std::endl;

    MatrixDistributeRule *distributeRuleMatA = new MatrixDistributeRule(MatrixType::MatrixA);
    MatrixDistributeRule *distributeRuleMatB = new MatrixDistributeRule(MatrixType::MatrixB);

    MatrixLoadRule *loadRule = new MatrixLoadRule(blkWidthMatA, blkHeightMatA, blkWidthMatB, blkHeightMatB);
    MatrixAccumulateRule *accumulateRule = new MatrixAccumulateRule(blkWidthMatB, blkHeightMatA, blkWidthMatA);

//  MatrixLoopRule *loopRuleMatA = new MatrixLoopRule(0);
//  MatrixLoopRule *loopRuleMatB = new MatrixLoopRule(0);

    MatrixOutputRule *outputRule = new MatrixOutputRule(blkWidthMatB, blkHeightMatA, blkWidthMatA);

    auto distributeBk = new htgs::Bookkeeper<MatrixRequestData>();
    auto matMulBk = new htgs::Bookkeeper<MatrixBlockData<MatrixMemoryData_t>>();
    auto matAccumBk = new htgs::Bookkeeper<MatrixBlockData<double *>>();


    auto taskGraph = new htgs::TaskGraph<MatrixRequestData, MatrixRequestData>();

    taskGraph->addGraphInputConsumer(distributeBk);
    taskGraph->addRule(distributeBk, readAMatTask, distributeRuleMatA);
    taskGraph->addRule(distributeBk, readBMatTask, distributeRuleMatB);

    taskGraph->addEdge(readAMatTask, matMulBk);
    taskGraph->addEdge(readBMatTask, matMulBk);

    taskGraph->addRule(matMulBk, mmulTask, loadRule);
//  taskGraph->addRule(matMulBk, readAMatTask, loopRuleMatA);
//  taskGraph->addRule(matMulBk, readBMatTask, loopRuleMatB);

    taskGraph->addEdge(mmulTask, matAccumBk);
    taskGraph->addRule(matAccumBk, accumTask, accumulateRule);
    taskGraph->addEdge(accumTask, matAccumBk);

    taskGraph->addRule(matAccumBk, outputTask, outputRule);
    taskGraph->addGraphOutputProducer(outputTask);

    taskGraph->addMemoryManagerEdge("MatrixA",
                                    readAMatTask,
                                    mmulTask,
                                    new MatrixAllocator(blockSize, blockSize),
                                    1000,
                                    htgs::MMType::Static);
    taskGraph->addMemoryManagerEdge("MatrixB",
                                    readBMatTask,
                                    mmulTask,
                                    new MatrixAllocator(blockSize, blockSize),
                                    1000,
                                    htgs::MMType::Static);

    taskGraph->writeDotToFile("matMul.dot");

    taskGraph->incrementGraphInputProducer();

    htgs::Runtime *runtime = new htgs::Runtime(taskGraph);

    clk.start();

    runtime->executeRuntime();

    for (int row = 0; row < blkHeightMatA; row++) {
      for (int col = 0; col < blkWidthMatA; col++) {

          MatrixRequestData *matrixA = new MatrixRequestData(row, col, MatrixType::MatrixA);
          taskGraph->produceData(matrixA);

      }
    }


    for (int row = 0; row <blkHeightMatB; row++) {
      for (int col = 0; col < blkWidthMatB; col++) {

        MatrixRequestData *matrixB = new MatrixRequestData(row, col, MatrixType::MatrixB);
        taskGraph->produceData(matrixB);

      }
    }

    taskGraph->finishedProducingData();

    while (!taskGraph->isOutputTerminated()) {
      auto data = taskGraph->consumeData();
      if (data != nullptr) {
//      std::cout << "Result received: " << data->getRow() << ", " << data->getCol() <<std::endl;
      }
    }

    runtime->waitForRuntime();

    delete runtime;
  }

  clk.stopAndIncrement();

  if (validate) {
    int res = validateResults(directory, matrixAHeight, matrixBWidth, blockSize);
    std::cout << "Finished (" << (res != 0 ? "FAILED - must run sequential" : "PASSED") << ") ";
  }

  std::cout << (runSequential ? "Sequential, " : "Parallel, ")
      << "width-b: " << matrixBWidth << ", height-a: " << matrixAHeight
      << ", shared-dim: " << sharedDim << ", blocksize: " << blockSize << ", time: " <<
      clk.getAverageTime(TimeVal::MILLI)
      << std::endl;




}