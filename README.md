Systems Programming Lab - Process Control and Exec Functions
Project Structure

src/: Contains all source files and challenge implementations
CMakeLists.txt OR Makefile: Builds all executables
lab_report.pdf: Contains screenshots proving build & run for the required demos and challenges
Source code and GitHub repo link: Provided for verification

Completed Challenges
✅ Challenge 3 - execl() to run ls -la

File: src/ch3_exec_ls.c
Description: Fork and child uses execl("ls","ls","-la",NULL) to list current directory; parent waits
Status: Complete - shows ls -la output from child process, parent prints final message

✅ Challenge 4 - Exec an internal helper program

Files: src/worker.c and src/ch4_exec_worker.c
Description: Two-program system where main program forks and executes worker binary with arguments and custom environment (MYVAR=hello)
Status: Complete - worker prints its args and MYVAR=hello environment variable

✅ Challenge 5 - Two child forks demonstration

File: src/ch5_exec_examples.c
Description: Demonstrates two child processes using different exec functions (execl vs execv) to produce same output
Status: Complete - both children print "one two" using different exec variants