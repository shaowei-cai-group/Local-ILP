/*=====================================================================================

    Filename:     ModelVar.h

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

class ModelVar
{
public:
  string name;
  size_t idx;
  Integer upperBound;
  Integer lowerBound;
  vector<size_t> conIdxs;
  vector<size_t> posInCon;
  size_t termNum;
  VarType type;

  ModelVar(
      const string &_name,
      size_t _idx);
  ~ModelVar();
  bool InBound(
      Integer value) const;
  void SetType(VarType varType);
};

class ModelVarUtil
{
public:
  unordered_map<string, size_t> name2idx;
  vector<ModelVar> varSet;
  vector<size_t> varIdx2ObjIdx;
  bool isBin;
  size_t varNum;
  size_t integerNum;
  size_t binaryNum;
  size_t fixedNum;
  Integer objBias;

  ModelVarUtil();
  ~ModelVarUtil();
  size_t MakeVar(
      const string &name);
  const ModelVar &GetVar(
      size_t idx) const;
  ModelVar &GetVar(
      size_t idx);
  ModelVar &GetVar(
      const string &name);
};