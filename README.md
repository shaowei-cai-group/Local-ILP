# Local-ILP
A standalone local search solver for general integer linear programming

The 3 folder:
- `bin`: precompiled solver
- `Source_Code`: source code of the solver
- `Statistics_and_Results`: detail benchmark statistics and experiment result in the paper.

## Run the Precompiled Solver
We provide the precompiled solver, you could easily run it as follow.
In order to run Local-ILP, you need to give the path to the input file (in MPS format), and the cutoff time (in seconds).
```
cd bin
chmod a+x Local-ILP
./Local-ILP  --instance=<input_file_name.mps> --cutoff=<cutoff_time>
```

## Compile and Run
We provide the source code, so you can also compile and run it as follow.

Example: To run the instance `2club200v15p5scn.mps` in the `10` second time limit.

```
cd Source_Code
mkdir build
cd src
chmod a+x run
./run --instance=2club200v15p5scn.mps --cutoff=10
```

## Reference
If you use Local-ILP in an academic context, please acknowledge this and cite the following article.

Peng Lin, Shaowei Cai, Mengchuan Zou, and Jinkun Lin. New characterizations and efficient local search for general integer linear programming. arXiv preprint arXiv:2305.00188, 2023.

## New Records for Open Instances
1. https://miplib.zib.de/instance_details_sorrell7.html
2. https://miplib.zib.de/instance_details_supportcase22.html
3. https://miplib.zib.de/instance_details_cdc7-4-3-2.html
4. https://miplib.zib.de/instance_details_ns1828997.html
5. https://miplib.zib.de/instance_details_scpm1.html
6. https://miplib.zib.de/instance_details_scpn2.html