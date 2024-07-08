/*=====================================================================================

    Filename:     Solver.h

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#pragma once
#include "utils/paras.h"
#include "ReaderMPS.h"
#include "ModelCon.h"
#include "ModelVar.h"
#include "LocalSearch/LocalILP.h"

class Solver
{
private:
  char *fileName;
  Integer optimalObj;
  void ParseObj();
  bool SkipInstance();

public:
  ReaderMPS *readerMPS;
  ModelConUtil *modelConUtil;
  ModelVarUtil *modelVarUtil;
  LocalILP *localILP;
  chrono::_V2::system_clock::time_point clkStart =
      chrono::high_resolution_clock::now();
  Solver();
  ~Solver();
  void Run();
};
