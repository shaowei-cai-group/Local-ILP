/*=====================================================================================

    Filename:     LiftMove.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "LocalILP.h"

void LocalILP::LiftMove()
{
  auto &localObj = localConUtil.conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  vector<Integer> &lowerDelta = localVarUtil.lowerDeltaInLiftMove;
  vector<Integer> &upperDelta = localVarUtil.upperDeltaInLifiMove;
  if (!isKeepFeas)
  {
    for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
    {
      size_t varIdx = modelObj.varIdxs[termIdx];
      auto &localVar = localVarUtil.GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      lowerDelta[termIdx] = modelVar.lowerBound - localVar.nowValue;
      upperDelta[termIdx] = modelVar.upperBound - localVar.nowValue;
      for (size_t j = 0; j < modelVar.termNum; ++j)
      {
        size_t conIdx = modelVar.conIdxs[j];
        auto &localCon = localConUtil.conSet[conIdx];
        auto &modelCon = modelConUtil->conSet[conIdx];
        size_t posInCon = modelVar.posInCon[j];
        Integer coeff = modelCon.coeffSet[posInCon];
        if (conIdx == 0)
          continue;
        Integer delta;
        if (!TightDelta(localCon, modelCon, posInCon, delta))
          continue;
        else
        {
          if (coeff > 0)
          {
            if (delta < upperDelta[termIdx])
              upperDelta[termIdx] = delta;
          }
          else if (coeff < 0)
          {
            if (delta > lowerDelta[termIdx])
              lowerDelta[termIdx] = delta;
          }
        }
        if (lowerDelta[termIdx] >= upperDelta[termIdx])
          break;
      }
    }
  }
  Integer bestObjDelta = 0;
  size_t bestVarIdx = -1;
  Integer bestVarDelta = 0;
  Integer varDelta;
  Integer objDelta;
  Integer objDelta_l;
  Integer objDelta_u;
  for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
  {
    size_t varIdx = modelObj.varIdxs[termIdx];
    Integer coeff = modelObj.coeffSet[termIdx];
    Integer l_d = lowerDelta[termIdx];
    Integer u_d = upperDelta[termIdx];
    if (l_d == u_d)
      continue;
    auto &localVar = localVarUtil.GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    if (!modelVar.InBound(localVar.nowValue + l_d))
      lowerDelta[termIdx] = l_d = 0;
    if (!modelVar.InBound(localVar.nowValue + u_d))
      upperDelta[termIdx] = u_d = 0;
    // assert(modelVar.InBound(localVar.nowValue + l_d) &&
    //        modelVar.InBound(localVar.nowValue + u_d));
    objDelta_l = coeff * l_d;
    objDelta_u = coeff * u_d;
    if (objDelta_l < objDelta_u)
    {
      objDelta = objDelta_l;
      varDelta = l_d;
    }
    else
    {
      objDelta = objDelta_u;
      varDelta = u_d;
    }
    if (objDelta < bestObjDelta)
    {
      bestObjDelta = objDelta;
      bestVarIdx = varIdx;
      bestVarDelta = varDelta;
    }
  }

  if (bestVarIdx != -1 && bestVarDelta != 0)
  {
    ++liftStep;
    ApplyMove(bestVarIdx, bestVarDelta);
    isKeepFeas = true;
    unordered_set<size_t> &affectedVar = localVarUtil.affectedVar;
    affectedVar.clear();
    auto &bestLocalVar = localVarUtil.GetVar(bestVarIdx);
    auto &bestModelVar = modelVarUtil->GetVar(bestVarIdx);
    for (auto conIdx : bestModelVar.conIdxs)
    {
      if (conIdx == 0)
        continue;
      auto &localCon = localConUtil.GetCon(conIdx);
      auto &modelCon = modelConUtil->GetCon(conIdx);
      for (auto varIdx : modelCon.varIdxs)
        affectedVar.insert(varIdx);
    }
    for (auto varIdx : affectedVar)
    {
      size_t idxInObj = modelVarUtil->varIdx2ObjIdx[varIdx];
      if (idxInObj == -1)
        continue;
      auto &localVar = localVarUtil.GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      lowerDelta[idxInObj] = modelVar.lowerBound - localVar.nowValue;
      upperDelta[idxInObj] = modelVar.upperBound - localVar.nowValue;
      for (size_t termIdx = 0; termIdx < modelVar.termNum; ++termIdx)
      {
        size_t conIdx = modelVar.conIdxs[termIdx];
        auto &localCon = localConUtil.conSet[conIdx];
        auto &modelCon = modelConUtil->conSet[conIdx];
        size_t posInCon = modelVar.posInCon[termIdx];
        Integer coeff = modelCon.coeffSet[posInCon];
        if (conIdx == 0)
          continue;
        Integer delta;
        if (!TightDelta(localCon, modelCon, posInCon, delta))
          continue;
        else
        {
          if (coeff > 0)
          {
            if (delta < upperDelta[idxInObj])
              upperDelta[idxInObj] = delta;
          }
          else if (coeff < 0)
          {
            if (delta > lowerDelta[idxInObj])
              lowerDelta[idxInObj] = delta;
          }
        }
        if (lowerDelta[idxInObj] >= upperDelta[idxInObj])
          break;
      }
    }
  }
  else
  {
    isKeepFeas = false;
    for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
    {
      size_t randomIdx = mt() % (modelObj.termNum);
      size_t varIdx = modelObj.varIdxs[randomIdx];
      Integer coeff = modelObj.coeffSet[randomIdx];
      auto &localVar = localVarUtil.GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      Integer varDelta;
      if (coeff > 0)
        varDelta = -1;
      else
        varDelta = 1;
      if (!modelVar.InBound(varDelta + localVar.nowValue))
        continue;
      else
      {
        ++breakStep;
        ApplyMove(varIdx, varDelta);
        break;
      }
    }
  }
}