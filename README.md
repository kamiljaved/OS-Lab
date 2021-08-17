<h1 align="center">
  <br>
  OS Lab - Programming Assignments
  <br>
</h1>

<h4 align="center">Processes, Threads, CPU Scheduling, Synchronization, Virtual Memory, Parallelization</h4>

<hr>

## Labs

(1) This program solves a partial differential equation on (N+2) × (N+2) grid (2D), in a parallel fashion; that is, it performs Gauss-Seidel sweeps over the grid until convergence.

Two solutions are explored in this program, namely:
* Red-Black Cells Approach
* Anti-Diagonals Approach

In each case, the following parallelization techniques were tried out and the results were compared:
* Sequential Implementation
* Parallel Implementation with <b>OpenMP</b>
  - using <i>#pragma omp critical</i>
  - using per-thread <i>diff</i> variable (no padding)
  - using per-thread <i>diff</i> variable (with padding)
* Parallel Implementation with <b>Pthreads (POSIX Threads)</b>
  - using mutex lock
  - using per-thread <i>diff</i> variable (no padding)
  - using per-thread <i>diff</i> variable (with padding)

<h2>Read the <a href="https://docs.google.com/viewer?url=https://github.com/kamiljaved98/OS-Lab_CEP/raw/master/report.pdf">report.pdf</a> document for further details.</h2>


---

> [kamiljaved.pythonanywhere.com](https://kamiljaved.pythonanywhere.com/) &nbsp;&middot;&nbsp;
> GitHub [@kamiljaved98](https://github.com/kamiljaved98)
