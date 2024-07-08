/*=====================================================================================

    Filename:     main.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "paras.h"
#include "../Solver.h"

int main(int argc, char **argv)
{

    INIT_ARGS

    Solver *solver = new Solver();
    solver->Run();

    return 0;
}