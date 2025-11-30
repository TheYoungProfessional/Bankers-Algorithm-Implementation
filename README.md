# Bankers-Algorithm-Implementation
INCLUDED FILES

input.txt: A text file containing data concerning the system's resources and current state.

banker.c: Source code for the Banker's Algorithm, determines if the system is in a safe state, 
and if it is, finds a safe sequence for the active processes to complete in.

README.md: This file serves to describe each of the files in the repo and provide detailed instructions
for installing and using this project.

LICENSE: The GNU Public License, allows anyone who downloads this repo to modify and distribute the repo's
contents as they please.

Documentation.txt: Explains what the program does and what execution results to expect. Explains the project's
usage of safety checking and resource allocation logic.

INSTALLATION INSTRUCTIONS FOR LINUX/UNIX USERS
1. Download "input.txt" and "banker.c" to an empty directory on your machine
2. Open your terminal and navigate to the directory you saved the files to
3. To compile the project, input the following command:

      gcc banker.c -o banker

4. To run the project input this command:

      ./banker input.txt

The program should now execute and behave as expected.
