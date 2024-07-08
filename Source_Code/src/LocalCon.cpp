/*=====================================================================================

    Filename:     LocalCon.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "LocalCon.h"

LocalCon::LocalCon()
    : weight(1),
      rhs(0)
{

}
LocalCon::~LocalCon()
{
}

LocalConUtil::LocalConUtil()
{
}

void LocalConUtil::Allocate(
    size_t conNum)
{
  unsatConIdxs.reserve(conNum);
  tempSatConIdxs.reserve(conNum);
  tempUnsatConIdxs.reserve(conNum);
  newConGap.resize(conNum);
  isNewConGap.resize(conNum, false);
  conSet.resize(conNum);
}

LocalConUtil::~LocalConUtil()
{
  tempSatConIdxs.clear();
  tempUnsatConIdxs.clear();
  newConGap.clear();
  isNewConGap.clear();
  conSet.clear();
  unsatConIdxs.clear();
}

LocalCon &LocalConUtil::GetCon(
    size_t idx)
{
  return conSet[idx];
}

void LocalConUtil::insertUnsat(
    size_t conIdx)
{
  conSet[conIdx].posInUnsatConIdxs = unsatConIdxs.size();
  unsatConIdxs.push_back(conIdx);
}

void LocalConUtil::RemoveUnsat(
    size_t conIdx)
{
  assert(unsatConIdxs.size() > 0);
  if (unsatConIdxs.size() == 1)
  {
    unsatConIdxs.pop_back();
    return;
  }
  size_t pos = conSet[conIdx].posInUnsatConIdxs;
  unsatConIdxs[pos] = *unsatConIdxs.rbegin();
  unsatConIdxs.pop_back();
  conSet[unsatConIdxs[pos]].posInUnsatConIdxs = pos;
}