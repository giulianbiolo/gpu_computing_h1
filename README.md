# GPU Computing - Deliverable 1

## File Structure:
```
|-transpose_all.cpp    # Used for creating the data of time against block size and optimization level
|-transpose_single.cpp # Used for creating the data of cache misses rates using valgrind's cachegrind tool
|-valgrind_report.py   # Used for generating the various graphs in cache rates
|-graphgen.py          # Used for generating the various graphs of time against block size and optimization level
```
## Usage:

### Time Estimation Graphs:

First run:
```
mkdir images                        # This will later be used to store the generated graphs
make transpose_all OPTLEVEL=-O<N>   # This will build the executable for the time against block size data for the given optimization level
make run_all N=<N> B=<B>            # This will run the executable and generate the data, that will be printed to the terminal, given your maximum Matrix and Block sizes
```
At this point we can copy the generated data, and paste it into the graphgen.py file in the appropriate array at the beginning of the file.
Repeat this process for every optimization level you want to test against.

Then consider the constants at the beginning of the file graphgen.py:
- **GRAPH_OPT**: Defines the optimization level data used to plot the time against block size graph for various matrix sizes.
- **GRAPH_EFF_BANDWIDTH_WITH_OPT_N**: Sets the Matrix Size to be used for the graph picturing time against block size, for the various optimization levels.

Finally run:
```
python graphgen.py
```
This will generate the various graphs from the data you have provided.

### Cache Miss Rate Graphs:

First run:
```
make transpose_single OPTLEVEL=-O<N>   # This will build the executable for the cache miss rate data for the given optimization level
python valgrind_report.py              # This will run the transpose_single executable with valgrind's cachegrind tool and generate the data with the graphs
```
This will generate a valgrind_report.txt file containing all of the logs of valgrind generated in the process,
along with all of the graphs generated from the data which will also be saved in the images folder.
