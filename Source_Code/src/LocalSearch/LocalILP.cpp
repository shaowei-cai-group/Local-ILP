/*=====================================================================================

    Filename:     LocalILP.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "LocalILP.h"

int LocalILP::LocalSearch(
    Integer optimalObj,
    chrono::_V2::system_clock::time_point clkStart)
{
  Allocate();
  InitSolution();
  InitState();
  auto &localObj = localConUtil.conSet[0];
  curStep = 0;
  while (true)
  {
    if (localConUtil.unsatConIdxs.empty())
    {
      if (!isFoundFeasible || localObj.gap <= 0)
      {
        UpdateBestSolution();
        LogObj(clkStart);
        isFoundFeasible = true;
      }
      LiftMove();
      if (GetObjValue() == optimalObj)
        return 1;
      ++curStep;
      if (Timeout(clkStart))
        break;
      continue;
    }
    if (Timeout(clkStart))
      break;
    if (curStep - lastImproveStep > restartStep)
    {
      Restart();
      continue;
    }

    if (!UnsatTightMove())
    {
      if (mt() % 10000 > smoothProbability)
        UpdateWeight();
      else
        SmoothWeight();
      RandomTightMove();
    }
    ++curStep;
  }
  return 0;
}

bool LocalILP::Timeout(
    chrono::_V2::system_clock::time_point &clkStart)
{
  auto clk_now = chrono::high_resolution_clock::now();
  auto solve_time =
      chrono::duration_cast<chrono::seconds>(clk_now - clkStart).count();
  if (solve_time >= OPT(cutoff))
    return true;
  return false;
}

void LocalILP::LogObj(
    chrono::_V2::system_clock::time_point &clkStart)
{
  auto clk = TimeNow();
  printf(
      "n %-20s %lf\n",
      itos(GetObjValue()).c_str(),
      ElapsedTime(clk, clkStart));
}

void LocalILP::InitSolution()
{
  for (size_t varIdx = 0; varIdx < modelVarUtil->varNum; varIdx++)
  {
    auto &localVar = localVarUtil.GetVar(varIdx);
    const auto &modelVar = modelVarUtil->GetVar(varIdx);
    if (modelVar.lowerBound > 0)
      localVar.nowValue = modelVar.lowerBound;
    else if (modelVar.upperBound < 0)
      localVar.nowValue = modelVar.upperBound;
    else
      localVar.nowValue = 0;
    if (!modelVar.InBound(localVar.nowValue))
      localVar.nowValue =
          (Integer)(modelVar.lowerBound / 2.0 + modelVar.upperBound / 2.0);
  }
}

void LocalILP::PrintResult()
{
  if (!isFoundFeasible)
    printf("o no feasible solution found.\n");
  else if (VerifySolution())
  {
    cout << "o Best objective: "
         << itos(GetObjValue()).c_str()
         << endl;
    cout << "B 1 "
         << itos(GetObjValue()).c_str()
         << endl;
    if (OPT(PrintSol))
      PrintSol();
  }
  else
    cout << "solution verify failed." << endl;
}

void LocalILP::InitState()
{
  for (size_t conIdx = 1; conIdx < modelConUtil->conNum; ++conIdx)
  {
    auto &localCon = localConUtil.conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    localCon.gap = 0;
    for (size_t termIdx = 0; termIdx < modelCon.termNum; ++termIdx)
      localCon.gap +=
          modelCon.coeffSet[termIdx] *
          localVarUtil.GetVar(modelCon.varIdxs[termIdx]).nowValue;
    localCon.gap -= localCon.rhs;
    if (localCon.gap > 0)
      localConUtil.insertUnsat(conIdx);
  }
  auto &localObj = localConUtil.conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  localObj.rhs = 1e38;
  localObj.gap = 0;
  for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
    localObj.gap +=
        modelObj.coeffSet[termIdx] *
        localVarUtil.GetVar(modelObj.varIdxs[termIdx]).nowValue;
  localObj.gap -= localObj.rhs;
}

void LocalILP::UpdateBestSolution()
{
  lastImproveStep = curStep;
  for (auto &localVar : localVarUtil.varSet)
    localVar.bestValue = localVar.nowValue;
  auto &localObj = localConUtil.conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  localObj.rhs = 0;
  for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
  {
    auto &localVar =
        localVarUtil.GetVar(modelObj.varIdxs[termIdx]);
    localObj.rhs +=
        localVar.nowValue * modelObj.coeffSet[termIdx];
  }

  localObj.rhs -= 1;
  localObj.gap = 1;
}

void LocalILP::ApplyMove(
    size_t varIdx,
    Integer delta)
{
  auto &localVar = localVarUtil.GetVar(varIdx);
  auto &modelVar = modelVarUtil->GetVar(varIdx);
  localVar.nowValue += delta;

  for (size_t termIdx = 0; termIdx < modelVar.termNum; ++termIdx)
  {
    size_t conIdx = modelVar.conIdxs[termIdx];
    size_t posInCon = modelVar.posInCon[termIdx];
    auto &localCon = localConUtil.conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    Integer newGap =
        localCon.gap + modelCon.coeffSet[posInCon] * delta;
    if (conIdx == 0)
      localCon.gap = newGap;
    else
    {
      bool isPreSat = localCon.gap <= 0;
      bool isNowSat = newGap <= 0;
      if (isPreSat && !isNowSat)
        localConUtil.insertUnsat(conIdx);
      else if (!isPreSat && isNowSat)
        localConUtil.RemoveUnsat(conIdx);
      localCon.gap = newGap;
    }
  }
  if (delta > 0)
  {
    localVar.lastIncStep = curStep;
    localVar.allowDecStep =
        curStep + tabuBase + mt() % tabuVariation;
  }
  else
  {
    localVar.lastDecStep = curStep;
    localVar.allowIncStep =
        curStep + tabuBase + mt() % tabuVariation;
  }
}

void LocalILP::Restart()
{
  lastImproveStep = curStep;
  ++restartTimes;
  for (auto unsatIdx : localConUtil.unsatConIdxs)
    localConUtil.RemoveUnsat(unsatIdx);
  for (size_t varIdx = 0; varIdx < modelVarUtil->varNum; varIdx++)
  {
    auto &localVar = localVarUtil.GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    if (modelVar.upperBound + 1 == modelVar.lowerBound)
      localVar.nowValue =
          (Integer)(modelVar.lowerBound / 2.0 + modelVar.upperBound / 2.0);
    else
      localVar.nowValue =
          modelVar.lowerBound + mt() % (modelVar.upperBound + 1 - modelVar.lowerBound);
    if (!modelVar.InBound(localVar.nowValue))
      localVar.nowValue =
          (Integer)(modelVar.lowerBound / 2.0 + modelVar.upperBound / 2.0);
    if (isFoundFeasible && mt() % 100 > 50)
      localVar.nowValue = localVar.bestValue;
    localVar.lastDecStep = curStep;
    localVar.allowIncStep = 0;
    localVar.lastIncStep = curStep;
    localVar.allowDecStep = 0;
  }
  for (size_t conIdx = 1; conIdx < modelConUtil->conNum; ++conIdx)
  {
    auto &localCon = localConUtil.conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    localCon.gap = 0;
    for (size_t termIdx = 0; termIdx < modelCon.termNum; ++termIdx)
      localCon.gap +=
          modelCon.coeffSet[termIdx] *
          localVarUtil.GetVar(modelCon.varIdxs[termIdx]).nowValue;

    localCon.gap -= localCon.rhs;
    if (localCon.gap > 0)
      localConUtil.insertUnsat(conIdx);
    localCon.weight = 1;
  }
  auto &localObj = localConUtil.conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  localObj.gap = 0;
  localObj.weight = 1;
  for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
    localObj.gap +=
        modelObj.coeffSet[termIdx] *
        localVarUtil.GetVar(modelObj.varIdxs[termIdx]).nowValue;
  localObj.gap -= localObj.rhs;
}

bool LocalILP::VerifySolution()
{
  for (size_t var_idx = 0; var_idx < modelVarUtil->varNum; var_idx++)
  {
    auto &var = localVarUtil.GetVar(var_idx);
    auto &modelVar = modelVarUtil->GetVar(var_idx);
    if (!modelVar.InBound(var.bestValue))
      return false;
  }

  for (size_t con_idx = 1; con_idx < modelConUtil->conNum; ++con_idx)
  {
    auto &con = localConUtil.conSet[con_idx];
    auto &modelCon = modelConUtil->conSet[con_idx];
    // calculate gap
    Integer con_gap = 0;
    for (size_t i = 0; i < modelCon.termNum; ++i)
      con_gap += modelCon.coeffSet[i] * localVarUtil.GetVar(modelCon.varIdxs[i]).bestValue;
    // setup unsat_con_idxs and total_weight
    if (con_gap > modelCon.rhs)
      cout << "con_gap:\t" << (double)con_gap << endl
           << "modelCon.ori_key:\t" << (double)modelCon.rhs << endl;
    if (con_gap > con.rhs)
      return false;
  }

  // Obj
  auto &localObj = localConUtil.conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  Integer objGap = 0;
  for (size_t i = 0; i < modelObj.termNum; ++i)
    objGap += modelObj.coeffSet[i] * localVarUtil.GetVar(modelObj.varIdxs[i]).bestValue;
  objGap -= localObj.rhs;
  return objGap == 1;
}

void LocalILP::PrintSol()
{
  for (int i = 0; i < modelVarUtil->varNum; i++)
  {
    const auto &var = localVarUtil.GetVar(i);
    const auto &modelVar = modelVarUtil->GetVar(i);
    if (var.bestValue)
      printf("%-30s        %lld\n", modelVar.name.c_str(), (long long)var.bestValue);
  }
}

void LocalILP::Allocate()
{
  localVarUtil.Allocate(
      modelVarUtil->varNum,
      modelConUtil->conSet[0].varIdxs.size());
  localConUtil.Allocate(modelConUtil->conNum);
  for (size_t conIdx = 1; conIdx < modelConUtil->conNum; conIdx++)
    localConUtil.conSet[conIdx].rhs = modelConUtil->conSet[conIdx].rhs;
}

Integer LocalILP::GetObjValue()
{
  return localConUtil.conSet[0].rhs + 1 + modelVarUtil->objBias;
}

LocalILP::LocalILP(
    const ModelConUtil *_modelConUtil,
    const ModelVarUtil *_modelVarUtil)
    : modelConUtil(_modelConUtil),
      modelVarUtil(_modelVarUtil)
{
  smoothProbability = 3;
  tabuBase = 3;
  tabuVariation = 10;
  isFoundFeasible = false;
  liftStep = 0;
  breakStep = 0;
  tightStepUnsat = 0;
  tightStepSat = 0;
  randomStep = 0;
  restartTimes = 0;
  weightUpperBound = 1000;
  objWeightUpperBound = 100;
  if (weightUpperBound < modelConUtil->conNum)
    weightUpperBound = modelConUtil->conNum;
  objWeightUpperBound = weightUpperBound / 10;
  lastImproveStep = 0;
  isBin = modelVarUtil->isBin;
  isKeepFeas = false;
  sampleUnsat = 3;
  bmsUnsat = 2000;
  sampleSat = 30;
  bmsSat = 350;
  bmsRandom = 150;
  restartStep = 1500000;
  rvd = 0.5;
  mt.seed(2832);
}

LocalILP::~LocalILP()
{
}