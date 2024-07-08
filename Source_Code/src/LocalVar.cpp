/*=====================================================================================

    Filename:     LocalVar.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "LocalVar.h"

LocalVar::LocalVar()
    : allowIncStep(0),
      allowDecStep(0),
      lastIncStep(0),
      lastDecStep(0)
{
}

LocalVar::~LocalVar()
{
}

LocalVarUtil::LocalVarUtil()
{
}

void LocalVarUtil::Allocate(
    size_t varNum,
    size_t varNumInObj)
{
  tempDeltas.reserve(varNum);
  tempVarIdxs.reserve(varNum);
  tempTwoVarIdxs_1.reserve(varNum);
  tempTwoDeltas_1.reserve(varNum);
  tempTwoVarIdxs_2.reserve(varNum);
  tempTwoDeltas_2.reserve(varNum);
  affectedVar.reserve(varNum);
  varSet.resize(varNum);
  scoreTable.resize(varNum, false);
  lowerDeltaInLiftMove.resize(varNumInObj);
  upperDeltaInLifiMove.resize(varNumInObj);
}

LocalVarUtil::~LocalVarUtil()
{
  lowerDeltaInLiftMove.clear();
  upperDeltaInLifiMove.clear();
  scoreTable.clear();
  affectedVar.clear();
  tempTwoVarIdxs_1.clear();
  tempTwoDeltas_1.clear();
  tempTwoVarIdxs_2.clear();
  tempTwoDeltas_2.clear();
  varSet.clear();
  tempDeltas.clear();
  tempVarIdxs.clear();
}

LocalVar &LocalVarUtil::GetVar(
    size_t idx)
{
  return varSet[idx];
}