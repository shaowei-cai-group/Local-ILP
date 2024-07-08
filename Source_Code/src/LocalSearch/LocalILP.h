/*=====================================================================================

    Filename:     LocalILP.h

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
#include "ModelCon.h"
#include "ModelVar.h"
#include "LocalCon.h"
#include "LocalVar.h"

class LocalILP
{
private:
  const ModelConUtil *modelConUtil;
  const ModelVarUtil *modelVarUtil;
  LocalVarUtil localVarUtil;
  LocalConUtil localConUtil;
  long curStep;
  std::mt19937 mt;
  int smoothProbability;
  int tabuBase;
  int tabuVariation;
  bool isFoundFeasible;
  size_t liftStep;
  size_t breakStep;
  size_t tightStepUnsat;
  size_t tightStepSat;
  size_t randomStep;
  size_t weightUpperBound;
  size_t objWeightUpperBound;
  size_t lastImproveStep;
  size_t restartTimes;
  bool isBin;
  bool isKeepFeas;
  size_t sampleUnsat;
  size_t bmsUnsat;
  size_t sampleSat;
  size_t bmsSat;
  size_t bmsRandom;
  size_t restartStep;
  double rvd;

  bool VerifySolution();
  void InitState();
  void UpdateBestSolution();
  void Restart();
  bool UnsatTightMove();
  void RandomTightMove();
  void LiftMove();
  bool SatTightMove(
      vector<bool> &score_table,
      vector<size_t> &score_idx);
  void UpdateWeight();
  void SmoothWeight();
  void ApplyMove(
      size_t var_idx,
      Integer delta);
  long TightScore(
      const ModelVar &var,
      Integer delta);
  bool TightDelta(
      LocalCon &con,
      const ModelCon &modelCon,
      size_t i,
      Integer &res);
  void InitSolution();
  bool Timeout(
      chrono::_V2::system_clock::time_point &clkStart);
  void LogObj(
      chrono::_V2::system_clock::time_point &clkStart);

public:
  LocalILP(
      const ModelConUtil *modelConUtil,
      const ModelVarUtil *modelVarUtil);
  ~LocalILP();
  int LocalSearch(
      Integer optimalObj,
      chrono::_V2::system_clock::time_point clkStart);
  void PrintResult();
  void PrintSol();
  void Allocate();
  Integer GetObjValue();
};