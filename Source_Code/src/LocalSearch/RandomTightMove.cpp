/*=====================================================================================

    Filename:     RandomTightMove.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "LocalILP.h"

void LocalILP::RandomTightMove()
{
  long bestScore = -100000000000;
  size_t bestLastMoveStep = std::numeric_limits<size_t>::max();
  size_t bestVarIdx = -1;
  Integer bestDelta = 0;
  size_t conIdx = localConUtil.unsatConIdxs[mt() % localConUtil.unsatConIdxs.size()];
  auto &localCon = localConUtil.conSet[conIdx];
  auto &modelCon = modelConUtil->conSet[conIdx];
  vector<size_t> &neighborVarIdxs = localVarUtil.tempVarIdxs;
  vector<Integer> &neighborDeltas = localVarUtil.tempDeltas;
  neighborVarIdxs.clear();
  neighborDeltas.clear();
  for (size_t termIdx = 0; termIdx < modelCon.termNum; ++termIdx)
  {
    size_t varIdx = modelCon.varIdxs[termIdx];
    auto &localVar = localVarUtil.GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    Integer delta;
    if (!TightDelta(localCon, modelCon, termIdx, delta))
    {
      if (modelCon.coeffSet[termIdx] > 0)
        delta = modelVar.lowerBound - localVar.nowValue;
      else
        delta = modelVar.upperBound - localVar.nowValue;
    }
    if (
        (delta < 0 && curStep == localVar.lastIncStep + 1 ||
         delta > 0 && curStep == localVar.lastDecStep + 1))
      continue;
    if (delta == 0)
      continue;
    neighborVarIdxs.push_back(varIdx);
    neighborDeltas.push_back(delta);
  }
  size_t scoreSize = neighborVarIdxs.size();
  if (neighborVarIdxs.size() > bmsRandom)
  {
    scoreSize = bmsRandom;
    for (size_t bmsIdx = 0; bmsIdx < bmsRandom; ++bmsIdx)
    {
      size_t randomIdx = (mt() % (neighborVarIdxs.size() - bmsIdx)) + bmsIdx;
      size_t varIdx = neighborVarIdxs[randomIdx];
      Integer delta = neighborDeltas[randomIdx];
      neighborVarIdxs[randomIdx] = neighborVarIdxs[bmsIdx];
      neighborDeltas[randomIdx] = neighborDeltas[bmsIdx];
      neighborVarIdxs[bmsIdx] = varIdx;
      neighborDeltas[bmsIdx] = delta;
    }
  }
  for (size_t idx = 0; idx < scoreSize; ++idx)
  {
    size_t varIdx = neighborVarIdxs[idx];
    Integer delta = neighborDeltas[idx];
    auto &localVar = localVarUtil.GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    long score = TightScore(modelVar, delta);
    size_t lastMoveStep =
        delta < 0 ? localVar.lastDecStep : localVar.lastIncStep;
    if (bestScore < score ||
        bestScore == score && lastMoveStep < bestLastMoveStep)
    {
      bestScore = score;
      bestVarIdx = varIdx;
      bestDelta = delta;
      bestLastMoveStep = lastMoveStep;
    }
  }

  if (bestVarIdx != -1 && bestDelta != 0)
  {
    ++randomStep;
    ApplyMove(bestVarIdx, bestDelta);
    return;
  }
}