/*=====================================================================================

    Filename:     ReaderMPS.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "ReaderMPS.h"

ReaderMPS::ReaderMPS(
    ModelConUtil *_modelConUtil,
    ModelVarUtil *_modelVarUtil)
    : modelConUtil(_modelConUtil),
      modelVarUtil(_modelVarUtil)
{
}

ReaderMPS::~ReaderMPS()
{
}

void ReaderMPS::Read(char *filename)
{

  ifstream infile(filename);
  string modelName;
  string tempStr;
  char conType;
  string conName;
  string inverseConName;
  size_t inverseConIdx;
  size_t conIdx;
  string varName;
  Integer coefficient;
  double tempVal;
  Integer rhs;
  string varType;
  double inputBound;
  if (!infile)
  {
    printf("o The input filename %s is invalid.\n", filename);
    exit(-1);
  }
  while (getline(infile, readLine)) // NAME section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'R')
      break;
    IssSetup();
    if (!(iss >> tempStr >> modelName))
      continue;
    if (modelName.length() < 1)
      continue;
    printf("c Model name: %s\n", modelName.c_str());
  }
  modelConUtil->conSet.emplace_back("", 0); // obj
  while (getline(infile, readLine)) // ROWS section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'C')
      break;
    IssSetup();
    if (!(iss >> conType >> conName))
      continue;
    if (conType == 'L')
    {
      conIdx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[conIdx].isEqual = false;
      modelConUtil->conSet[conIdx].isLess = true;
    }
    else if (conType == 'E')
    {
      conIdx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[conIdx].isEqual = true;
      modelConUtil->conSet[conIdx].isLess = false;
      inverseConName = conName + "!";
      inverseConIdx = modelConUtil->MakeCon(inverseConName);
      modelConUtil->conSet[inverseConIdx].isEqual = true;
      modelConUtil->conSet[inverseConIdx].isLess = false;
    }
    else if (conType == 'G')
    {
      conIdx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[conIdx].isEqual = false;
      modelConUtil->conSet[conIdx].isLess = false;
    }
    else
    {
      assert(conType == 'N'); // type=='N',this con is obj
      modelConUtil->objName = conName;
      modelConUtil->conSet[0].isEqual = false;
      modelConUtil->conSet[0].isLess = false;
    }
  }
  while (getline(infile, readLine)) // COLUMNS section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'R')
      break;
    IssSetup();
    if (!(iss >> varName >> conName))
      continue;
    if (conName == "\'MARKER\'")
    {
      iss >> tempStr;
      if (tempStr == "\'INTORG\'" ||
          tempStr == "\'INTEND\'")
        continue;
      else
      {
        printf("c error %s\n", readLine.c_str());
        exit(-1);
      }
    }
    iss >> tempVal;
    coefficient = tempVal * ZoomTimes;
    conIdx = modelConUtil->GetConIdx(conName);
    PushCoeffVarIdx(conIdx, coefficient, varName);
    if (modelConUtil->conSet[conIdx].isEqual)
      PushCoeffVarIdx(conIdx + 1, -coefficient, varName);
    if (iss >> conName)
    {
      iss >> tempVal;
      coefficient = tempVal * ZoomTimes;
      conIdx = modelConUtil->GetConIdx(conName);
      PushCoeffVarIdx(conIdx, coefficient, varName);
      if (modelConUtil->conSet[conIdx].isEqual)
        PushCoeffVarIdx(conIdx + 1, -coefficient, varName);
    }
  }
  while (getline(infile, readLine)) // RHS  section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'B' ||
        readLine[0] == 'E')
      break;
    // assert(line[0] != 'R'); // do not handle RANGS and SOS
    IssSetup();
    if (!(iss >> tempStr >> conName >> tempVal))
      continue;
    if (conName.length() < 1)
      continue;
    rhs = tempVal * ZoomTimes;
    conIdx = modelConUtil->GetConIdx(conName);
    modelConUtil->conSet[conIdx].rhs = rhs;
    if (modelConUtil->conSet[conIdx].isEqual)
      modelConUtil->conSet[conIdx + 1].rhs = -rhs;

    if (iss >> conName)
    {
      iss >> tempVal;
      rhs = tempVal * ZoomTimes;
      conIdx = modelConUtil->GetConIdx(conName);
      modelConUtil->conSet[conIdx].rhs = rhs;
      if (modelConUtil->conSet[conIdx].isEqual)
        modelConUtil->conSet[conIdx + 1].rhs = -rhs;
    }
  }
  while (getline(infile, readLine)) // BOUNDS section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'E')
      break;
    assert(readLine[0] != 'I'); // do not handle INDICATORS
    IssSetup();
    if (!(iss >> varType >> tempStr >> varName))
      continue;
    iss >> inputBound;
    auto &var = modelVarUtil->GetVar(varName);
    if (var.type == VarType::Binary)
    {
      var.SetType(VarType::Integer);
      var.upperBound = InfiniteUpperBound;
    }
    if (varType == "UP")
      var.upperBound = floor(inputBound);
    else if (varType == "LO")
      var.lowerBound = ceil(inputBound);
    else if (varType == "BV")
    {
      var.upperBound = 1;
      var.lowerBound = 0;
    }
    else if (varType == "LI")
      var.lowerBound = ceil(inputBound);
    else if (varType == "UI")
      var.upperBound = floor(inputBound);
    else if (varType == "FX")
    {
      var.lowerBound = inputBound;
      var.upperBound = inputBound;
    }
    else if (varType == "FR")
    {
      var.upperBound = InfiniteUpperBound;
      var.lowerBound = InfiniteLowerBound;
    }
    else if (varType == "MI")
      var.lowerBound = InfiniteLowerBound;
    else if (varType == "PL")
      var.upperBound = InfiniteUpperBound;
  }
  infile.close();
  for (conIdx = 1; conIdx < modelConUtil->conSet.size(); ++conIdx) 
  {
    auto &con = modelConUtil->conSet[conIdx];
    if (con.isLess == false &&
        con.isEqual == false)
    {
      con.isLess = true;
      for (Integer &inverseCoefficient : con.coeffSet)
        inverseCoefficient = -inverseCoefficient;
      con.rhs = -con.rhs;
    }
  }
  modelConUtil->conNum = modelConUtil->conSet.size();
  modelVarUtil->varNum = modelVarUtil->varSet.size();
  TightenBound();
  if (!TightBoundGlobally())
  {
    printf("c model is infeasible.\n");
    exit(-1);
  }
  SetVarType();
  SetVarIdx2ObjIdx();
}

inline void ReaderMPS::IssSetup()
{
  iss.clear();
  iss.str(readLine);
  iss.seekg(0, ios::beg);
}

void ReaderMPS::PushCoeffVarIdx(
    const size_t _conIdx,
    const Integer _coeff,
    const string &_varName)
{
  auto &con = modelConUtil->conSet[_conIdx];
  size_t _varIdx = modelVarUtil->MakeVar(_varName);
  auto &var = modelVarUtil->GetVar(_varIdx);

  var.conIdxs.push_back(_conIdx);
  var.posInCon.push_back(con.varIdxs.size());

  con.coeffSet.push_back(_coeff);
  con.varIdxs.push_back(_varIdx);
  con.posInVar.push_back(var.conIdxs.size() - 1);
}

void ReaderMPS::TightenBound()
{
  for (size_t conIdx = 1; conIdx < modelConUtil->conNum; ++conIdx)
  {
    auto &modelCon = modelConUtil->conSet[conIdx];
    if (modelCon.varIdxs.size() == 1)
      TightenBoundVar(modelCon);
  }
}

void ReaderMPS::TightenBoundVar(ModelCon &modelCon)
{
  Integer coeff = modelCon.coeffSet[0];
  auto &modelvar = modelVarUtil->GetVar(modelCon.varIdxs[0]);
  double bound = (double)(modelCon.rhs / ZoomTimes) / (coeff / ZoomTimes);
  if (coeff > 0 && bound <= modelvar.upperBound) // x <= bound
    modelvar.upperBound = floor(bound);
  else if (coeff < 0 && modelvar.lowerBound <= bound) // x >= bound
    modelvar.lowerBound = ceil(bound);
}

bool ReaderMPS::TightBoundGlobally()
{
  for (auto &modelVar : modelVarUtil->varSet)
    if (modelVar.lowerBound == modelVar.upperBound)
    {
      modelVar.SetType(VarType::Fixed);
      fixedIdxs.push_back(modelVar.idx);
    }
  while (fixedIdxs.size() > 0)
  {
    size_t removeVarIdx = fixedIdxs.back();
    fixedIdxs.pop_back();
    deleteVarNum++;
    ModelVar &removeVar = modelVarUtil->GetVar(removeVarIdx);
    Integer removeVarValue = removeVar.lowerBound;
    for (int termIdx = 0; termIdx < removeVar.conIdxs.size(); termIdx++)
    {
      size_t conIdx = removeVar.conIdxs[termIdx];
      size_t posInCon = removeVar.posInCon[termIdx];
      ModelCon &modelCon = modelConUtil->GetCon(conIdx);
      Integer coeff = modelCon.coeffSet[posInCon];
      size_t movedVarIdx = modelCon.varIdxs.back();
      Integer movedCoeff = modelCon.coeffSet.back();
      size_t movedPosInVar = modelCon.posInVar.back();
      modelCon.varIdxs[posInCon] = movedVarIdx;
      modelCon.coeffSet[posInCon] = movedCoeff;
      modelCon.posInVar[posInCon] = movedPosInVar;
      ModelVar &movedVar = modelVarUtil->GetVar(movedVarIdx);
      assert(movedVar.conIdxs[movedPosInVar] == conIdx);
      movedVar.posInCon[movedPosInVar] = posInCon;
      modelCon.varIdxs.pop_back();
      modelCon.coeffSet.pop_back();
      modelCon.posInVar.pop_back();
      if (conIdx == 0)
        modelVarUtil->objBias += coeff * removeVarValue;
      else
      {
        modelCon.rhs -= coeff * removeVarValue;
        if (modelCon.varIdxs.size() == 1)
        {
          TightenBoundVar(modelCon);
          ModelVar &relatedVar = modelVarUtil->GetVar(modelCon.varIdxs[0]);
          if (relatedVar.type != VarType::Fixed &&
              relatedVar.lowerBound == relatedVar.upperBound)
          {
            relatedVar.SetType(VarType::Fixed);
            fixedIdxs.push_back(relatedVar.idx);
            inferVarNum++;
          }
        }
        else if (modelCon.varIdxs.size() == 0)
        {
          assert(modelCon.coeffSet.size() == 0 && modelCon.posInVar.size() == 0);
          if (modelCon.rhs >= 0)
          {
            modelCon.inferSAT = true;
            deleteConNum++;
          }
          else
          {
            printf("c con.rhs %lf\n", modelCon.rhs);
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool ReaderMPS::SetVarType()
{
  for (size_t varIdx = 0; varIdx < modelVarUtil->varNum; varIdx++)
  {
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    modelVar.termNum = modelVar.conIdxs.size();
    if (modelVar.lowerBound == modelVar.upperBound)
    {
      modelVarUtil->fixedNum++;
      modelVar.SetType(VarType::Fixed);
    }
    else if (
        modelVar.lowerBound == 0 &&
        modelVar.upperBound == 1)
    {
      modelVarUtil->binaryNum++;
      modelVar.SetType(VarType::Binary);
    }
    else
    {
      modelVar.SetType(VarType::Integer);
      modelVarUtil->integerNum++;
      modelVarUtil->isBin = false;
    }
  }
  for (size_t conIdx = 0; conIdx < modelConUtil->conNum; conIdx++)
  {
    auto &modelCon = modelConUtil->GetCon(conIdx);
    modelCon.termNum = modelCon.varIdxs.size();
    if (modelCon.inferSAT)
      assert(modelCon.termNum == 0);
  }
  return true;
}

void ReaderMPS::SetVarIdx2ObjIdx()
{
  modelVarUtil->varIdx2ObjIdx.resize(modelVarUtil->varNum, -1);
  const auto &modelObj = modelConUtil->conSet[0];
  for (size_t idx = 0; idx < modelObj.termNum; ++idx)
    modelVarUtil->varIdx2ObjIdx[modelObj.varIdxs[idx]] = idx;
}