/*=====================================================================================

    Filename:     ModelCon.h

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

class ModelCon
{
public:
  string name;
  size_t idx;
  bool isEqual;
  bool isLess;
  bool isLarge;
  vector<Integer> coeffSet;
  vector<size_t> varIdxs;
  vector<size_t> posInVar;
  Integer rhs;
  bool inferSAT;
  size_t termNum;

  ModelCon(
      const string &_name,
      size_t _idx);
  ~ModelCon();
};

class ModelConUtil
{
public:
  unordered_map<string, size_t> name2idx;
  vector<ModelCon> conSet;
  string objName;
  size_t conNum;

  ModelConUtil();
  ~ModelConUtil();
  size_t MakeCon(
      const string &name);
  size_t GetConIdx(
      const string &name);
  const ModelCon &GetCon(
      size_t idx) const;
  ModelCon &GetCon(
      size_t idx);
  ModelCon &GetCon(
      const string &name);
};