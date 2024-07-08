/*=====================================================================================

    Filename:     UnsatTightMove.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "LocalILP.h"

bool LocalILP::UnsatTightMove()
{
  vector<bool> &scoreTable = localVarUtil.scoreTable;
  vector<size_t> scoreIdxs;
  long bestScore = 0;
  size_t bestLastMoveStep = std::numeric_limits<size_t>::max();
  size_t bestVarIdx = -1;
  Integer bestDelta = 0;
  vector<size_t> &neighborVarIdxs = localVarUtil.tempVarIdxs;
  vector<Integer> &neighborDeltas = localVarUtil.tempDeltas;
  neighborVarIdxs.clear();
  neighborDeltas.clear();
  size_t neighborSize = localConUtil.unsatConIdxs.size();
  vector<size_t> *neighborConIdxs = &localConUtil.unsatConIdxs;
  if (sampleUnsat < neighborSize)
  {
    neighborSize = sampleUnsat;
    neighborConIdxs = &localConUtil.tempUnsatConIdxs;
    neighborConIdxs->clear();
    neighborConIdxs->assign(localConUtil.unsatConIdxs.begin(), localConUtil.unsatConIdxs.end());
    for (size_t sampleIdx = 0; sampleIdx < sampleUnsat; ++sampleIdx)
    {
      size_t randomIdx = mt() % (neighborConIdxs->size() - sampleIdx);
      size_t temp = neighborConIdxs->at(sampleIdx);
      neighborConIdxs->at(sampleIdx) = neighborConIdxs->at(randomIdx + sampleIdx);
      neighborConIdxs->at(randomIdx + sampleIdx) = temp;
    }
  }
  for (size_t neighborIdx = 0; neighborIdx < neighborSize; ++neighborIdx)
  {
    auto &localCon = localConUtil.conSet[neighborConIdxs->at(neighborIdx)];
    auto &modelCon = modelConUtil->conSet[neighborConIdxs->at(neighborIdx)];
    for (size_t termIdx = 0; termIdx < modelCon.termNum; ++termIdx)
    {
      size_t varIdx = modelCon.varIdxs[termIdx];
      auto &localVar = localVarUtil.GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      Integer delta;
      if (!TightDelta(localCon, modelCon, termIdx, delta))
        if (modelCon.coeffSet[termIdx] > 0)
          delta = modelVar.lowerBound - localVar.nowValue;
        else
          delta = modelVar.upperBound - localVar.nowValue;
      if (delta < 0 && curStep < localVar.allowDecStep ||
          delta > 0 && curStep < localVar.allowIncStep)
        continue;
      if (delta == 0)
        continue;
      neighborVarIdxs.push_back(varIdx);
      neighborDeltas.push_back(delta);
    }
  }
  size_t scoreSize = neighborVarIdxs.size();
  if (!isFoundFeasible && scoreSize > bmsUnsat)
  {
    scoreSize = bmsUnsat;
    for (size_t bmsIdx = 0; bmsIdx < bmsUnsat; ++bmsIdx)
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
    if (isBin)
    {
      if (scoreTable[varIdx])
        continue;
      else
      {
        scoreTable[varIdx] = true;
        scoreIdxs.push_back(varIdx);
      }
    }
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
  if (bestScore > 0)
  {
    ++tightStepUnsat;
    ApplyMove(bestVarIdx, bestDelta);
    if (isBin)
      for (auto idx : scoreIdxs)
        scoreTable[idx] = false;
    return true;
  }
  else
  {
    if (isFoundFeasible)
      return SatTightMove(scoreTable, scoreIdxs);
  }
  if (isBin)
    for (auto idx : scoreIdxs)
      scoreTable[idx] = false;
  return false;
}