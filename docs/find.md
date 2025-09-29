FogOS - Find Implementation

Build
- Build the OS as usual (make qemu)

Run
- In the command line type: find <path> <expression>
- If "." is given as path, then it will look through current directory
- If 0 args are given it will return an error statement
- If expression/file does not exist in path, then it will return an error statement
- The output will be printed in the following format if successful
	- Path: /tests/itests test1.txt

Test
- Created local testing directory (tests) containing two text files and another directory (itests) with a text file

Test Input 1: find . test1.txt
Test Output 1: Path: /tests/itest/test1.txt test1.txt				/tests/test1.txt test1.txt

Test Input 2: find /tests test2.txt
Test Output 2: Path: /tests/test2.txt test2.txt


