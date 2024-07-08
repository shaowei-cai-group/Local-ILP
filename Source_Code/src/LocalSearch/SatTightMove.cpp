/*=====================================================================================

    Filename:     SatTightMove.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "LocalILP.h"

bool LocalILP::SatTightMove(
    vector<bool> &score_table,
    vector<size_t> &score_idx)
{
  long bestScore = 0;
  size_t bestLastMoveStep = std::numeric_limits<size_t>::max();
  size_t bestVarIdx = -1;
  Integer bestDelta = 0;
  vector<size_t> &neighborVarIdxs = localVarUtil.tempVarIdxs;
  vector<Integer> &neighborDeltas = localVarUtil.tempDeltas;
  neighborVarIdxs.clear();
  neighborDeltas.clear();
  auto &neighborConIdxs = localConUtil.tempSatConIdxs;
  neighborConIdxs.clear();
  for (size_t conIdx = 1; conIdx < modelConUtil->conNum; ++conIdx)
  {
    if (localConUtil.conSet[conIdx].gap <= 0 && !modelConUtil->conSet[conIdx].inferSAT)
      neighborConIdxs.push_back(conIdx);
  }
  size_t neighborSize = neighborConIdxs.size();
  if (sampleSat < neighborSize)
  {
    neighborSize = sampleSat;
    for (size_t sampleIdx = 0; sampleIdx < sampleSat; ++sampleIdx)
    {
      size_t randomIdx = mt() % (neighborConIdxs.size() - sampleIdx);
      size_t temp = neighborConIdxs[sampleIdx];
      neighborConIdxs[sampleIdx] = neighborConIdxs[randomIdx + sampleIdx];
      neighborConIdxs[randomIdx + sampleIdx] = temp;
    }
  }
  for (size_t neighborIdx = 0; neighborIdx < neighborSize; ++neighborIdx)
  {
    auto &localCon = localConUtil.conSet[neighborConIdxs[neighborIdx]];
    auto &modelCon = modelConUtil->conSet[neighborConIdxs[neighborIdx]];
    for (size_t termIdx = 0; termIdx < modelCon.termNum; ++termIdx)
    {
      size_t varIdx = modelCon.varIdxs[termIdx];
      auto &localVar = localVarUtil.GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      Integer delta;
      if (!TightDelta(localCon, modelCon, termIdx, delta))
        if (modelCon.coeffSet[termIdx] > 0)
          delta = modelVar.upperBound - localVar.nowValue;
        else
          delta = modelVar.lowerBound - localVar.nowValue;
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
  if (neighborVarIdxs.size() > bmsSat)
  {
    scoreSize = bmsSat;
    for (size_t bmsIdx = 0; bmsIdx < bmsSat; ++bmsIdx)
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
      if (score_table[varIdx])
        continue;
      else
      {
        score_table[varIdx] = true;
        score_idx.push_back(varIdx);
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
  if (isBin)
    for (auto idx : score_idx)
      score_table[idx] = false;

  if (bestScore > 0)
  {
    ++tightStepSat;
    ApplyMove(bestVarIdx, bestDelta);
    return true;
  }
  return false;
}