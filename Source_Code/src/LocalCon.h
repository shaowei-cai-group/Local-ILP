/*=====================================================================================

    Filename:     LocalCon.h

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

class LocalCon
{
public:
  size_t weight;
  Integer gap;
  size_t posInUnsatConIdxs;
  Integer rhs;

  LocalCon();
  ~LocalCon();
};

class LocalConUtil
{
public:
  vector<LocalCon> conSet;
  vector<size_t> unsatConIdxs;
  vector<size_t> tempUnsatConIdxs;
  vector<size_t> tempSatConIdxs;
  vector<Integer> newConGap;
  vector<bool> isNewConGap;

  LocalConUtil();
  ~LocalConUtil();
  void Allocate(
      size_t conNum);
  LocalCon &GetCon(
      size_t idx);
  void insertUnsat(
      size_t con_idx);
  void RemoveUnsat(
      size_t con_idx);
};
