# Local-ILP
A standalone local search solver for general integer linear programming

# How to Use
In order to run Local-ILP, you need to give the path to the input file (in MPS format), and the cutoff time (in seconds).
```
cd bin
./Local-ILP  --instance=<input_file_name.mps> --cutoff=<cutoff_time>
```

# Reference
If you use Local-ILP in an academic context, please acknowledge this and cite the following article.
Peng Lin, Shaowei Cai, Mengchuan Zou, and Jinkun Lin. New characterizations and efficient local search for general integer linear programming. arXiv preprint arXiv:2305.00188, 2023.
