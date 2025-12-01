//File: banker.c
//Programmer: Michael J. Overman
//Date: 11-30-2025

// Include needed libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Define maximums at the preprocessor level
#define MAX_PROCESSES 10
#define MAX_RESOURCES 10
#define MAX_LINE 256

// Global variables for the Banker's Algorithm
int num_processes; // total amount of processes
int num_resources; // total amount of resources
int total[MAX_RESOURCES];
int available[MAX_RESOURCES];
int allocation[MAX_PROCESSES][MAX_RESOURCES];
int max[MAX_PROCESSES][MAX_RESOURCES];
int need[MAX_PROCESSES][MAX_RESOURCES];

// Function prototypes
bool readInputFile(const char *filename); //called once to gather necessary 
                                          //information
void calculateNeed(); // determine resources needed by a process
void calculateAvailable(); // how many resources are currently available
void printSystemState(); // full system state displayed (for debug & verification)
bool isSafe(int safeSequence[]); //determines if the system is in a safe state

/**
 * Skips comment lines and empty lines
 * Returns true if line should be processed, false if it should be skipped
 */
bool isValidLine(char *line) {
    // Skip empty lines
    if (line[0] == '\n' || line[0] == '\0') {
        return false;
    }
    
    // Skip comment lines (starting with #)
    if (line[0] == '#') {
        return false;
    }
    
    return true;
}

/**
 * Reads and parses the input file
 * Returns true on success, false on failure
 */
bool readInputFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file '%s'\n", filename);
        return false;
    }
    
    char line[MAX_LINE];
    int section = 0; // Track which section we're reading
    int process_count = 0;
    
    printf("Reading input file: %s\n\n", filename);
    
    while (fgets(line, sizeof(line), file)) 
        //read a line of text from the file
    {
        // Skip comments and empty lines
        if (!isValidLine(line)) {
            continue;
        }
        
        // Section 0: Read number of processes and resources
        if (section == 0) {
            if (sscanf(line, "%d %d", &num_processes, &num_resources) == 2) {
                printf("Processes: %d, Resources: %d\n", 
                    num_processes, num_resources);
                section = 1;
            }
        }
        // Section 1: Read total available resources
        else if (section == 1) {
            char *token = strtok(line, " \t\n");
            int i = 0;
            while (token != NULL && i < num_resources) {
                total[i] = atoi(token); //convert from ascii to integer
                i++;
                token = strtok(NULL, " \t\n");
            }
            
            if (i == num_resources) {
                printf("Total Resources: ");
                for (int j = 0; j < num_resources; j++) {
                    printf("%d ", total[j]);
                }
                printf("\n");
                section = 2;
                process_count = 0;
            }
        }
        // Section 2: Read Allocation matrix
        else if (section == 2) {
            char process_name[10];
            int resources[MAX_RESOURCES];
            
            // Parse process name and resource values
            char *token = strtok(line, " \t\n");
            if (token != NULL) {
                strcpy(process_name, token);
                
                int i = 0;
                token = strtok(NULL, " \t\n");
                while (token != NULL && i < num_resources) {
                    resources[i] = atoi(token);
                    allocation[process_count][i] = resources[i];
                    i++;
                    token = strtok(NULL, " \t\n");
                }
                
                if (i == num_resources) {
                    process_count++;
                    
                    // Move to next section after reading all processes
                    if (process_count == num_processes) {
                        section = 3;
                        process_count = 0;
                    }
                }
            }
        }
        // Section 3: Read Maximum matrix
        else if (section == 3) {
            char process_name[10];
            int resources[MAX_RESOURCES];
            
            // Parse process name and resource values
            char *token = strtok(line, " \t\n");
            if (token != NULL) {
                strcpy(process_name, token);
                
                int i = 0;
                token = strtok(NULL, " \t\n");
                while (token != NULL && i < num_resources) {
                    resources[i] = atoi(token);
                    max[process_count][i] = resources[i];
                    i++;
                    token = strtok(NULL, " \t\n");
                }
                
                if (i == num_resources) {
                    process_count++;
                    
                    // Finished reading all data
                    if (process_count == num_processes) {
                        section = 4;
                    }
                }
            }
        }
    }
    
    fclose(file);
    
    // Verify we read all data
    if (section != 4) {
        printf("Error: Incomplete input file\n");
        return false;
    }
    
    printf("\nInput file parsed successfully!\n");
    return true;
}

/**
 * Calculate the Need matrix
 * Need[i][j] = Max[i][j] - Allocation[i][j]
 */
void calculateNeed() {
    for (int i = 0; i < num_processes; i++) {
        for (int j = 0; j < num_resources; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }
}

/**
 * Calculate the Available resources
 * Available[j] = Total[j] - Sum(Allocation[i][j]) for all i
 */
void calculateAvailable() {
    for (int j = 0; j < num_resources; j++) {
        available[j] = total[j];
        for (int i = 0; i < num_processes; i++) {
            available[j] -= allocation[i][j];
        }
    }
}

/**
 * Print the current system state (for debugging and verification)
 */
void printSystemState() {
    printf("\n========== SYSTEM STATE ==========\n\n");
    
    // Print Total Resources
    printf("Total Resources:\n");
    printf("   ");
    for (int j = 0; j < num_resources; j++) {
        printf("%4c", 'A' + j);
    }
    printf("\n   ");
    for (int j = 0; j < num_resources; j++) {
        printf("%4d", total[j]);
    }
    printf("\n\n");
    
    // Print Available Resources
    printf("Available Resources:\n");
    printf("   ");
    for (int j = 0; j < num_resources; j++) {
        printf("%4c", 'A' + j);
    }
    printf("\n   ");
    for (int j = 0; j < num_resources; j++) {
        printf("%4d", available[j]);
    }
    printf("\n\n");
    
    // Print Allocation Matrix
    printf("Allocation Matrix:\n");
    printf("     ");
    for (int j = 0; j < num_resources; j++) {
        printf("%4c", 'A' + j);
    }
    printf("\n");
    
    for (int i = 0; i < num_processes; i++) {
        printf("P%d:  ", i);
        for (int j = 0; j < num_resources; j++) {
            printf("%4d", allocation[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    // Print Maximum Matrix
    printf("Maximum Matrix:\n");
    printf("     ");
    for (int j = 0; j < num_resources; j++) {
        printf("%4c", 'A' + j);
    }
    printf("\n");
    
    for (int i = 0; i < num_processes; i++) {
        printf("P%d:  ", i);
        for (int j = 0; j < num_resources; j++) {
            printf("%4d", max[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    // Print Need Matrix
    printf("Need Matrix:\n");
    printf("     ");
    for (int j = 0; j < num_resources; j++) {
        printf("%4c", 'A' + j);
    }
    printf("\n");
    
    for (int i = 0; i < num_processes; i++) {
        printf("P%d:  ", i);
        for (int j = 0; j < num_resources; j++) {
            printf("%4d", need[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("==================================\n\n");
}

/**
 * Check if a process's need can be satisfied with available resources
 * Returns true if Need[process][j] <= work[j] for all j
 */
bool canAllocate(int process, int work[]) {
    for (int j = 0; j < num_resources; j++) {
        if (need[process][j] > work[j]) {
            return false;
        }
    }
    return true;
}

/**
 * Implements the Banker's Algorithm Safety Check
 * Returns true if system is in safe state, false otherwise
 * If safe, fills the safeSequence array with the safe execution order
 */
bool isSafe(int safeSequence[]) {
    // Work array represents available resources as we simulate execution
    int work[MAX_RESOURCES];
    for (int j = 0; j < num_resources; j++) {
        work[j] = available[j];
    }
    
    // Finish array tracks which processes have completed
    bool finish[MAX_PROCESSES];
    for (int i = 0; i < num_processes; i++) {
        finish[i] = false;
    }
    
    int count = 0; // Number of processes that have finished
    
    printf("=== Running Safety Algorithm ===\n\n");
    
    // Try to find a safe sequence
    while (count < num_processes) {
        bool found = false;
        
        // Look for a process that can be executed
        for (int i = 0; i < num_processes; i++) {
            // Skip if already finished
            if (finish[i]) {
                continue;
            }
            
            // Check if this process's needs can be satisfied
            if (canAllocate(i, work)) {
                // Process i can execute
                printf("Step %d: Process P%d can execute\n", count + 1, i);
                printf("  Need:      ");
                for (int j = 0; j < num_resources; j++) {
                    printf("%d ", need[i][j]);
                }
                printf("\n  Available: ");
                for (int j = 0; j < num_resources; j++) {
                    printf("%d ", work[j]);
                }
                printf("\n");
                
                // Simulate process execution and resource release
                printf("  -> P%d executes and releases resources\n", i);
                for (int j = 0; j < num_resources; j++) {
                    work[j] += allocation[i][j];
                }
                
                printf("  New Available: ");
                for (int j = 0; j < num_resources; j++) {
                    printf("%d ", work[j]);
                }
                printf("\n\n");
                
                // Mark process as finished and add to safe sequence
                finish[i] = true;
                safeSequence[count] = i;
                count++;
                found = true;
                break;
            }
        }
        
        // If no process could execute, system is not in safe state
        if (!found) {
            printf("No process can execute with current available resources.\n");
            printf("System is in UNSAFE state!\n\n");
            return false;
        }
    }
    
    // All processes finished successfully
    printf("All processes completed successfully.\n");
    printf("System is in SAFE state!\n\n");
    return true;
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    const char *filename = "input.txt";
    if (argc > 1) {
        filename = argv[1];
    }
    
    printf("=== Banker's Algorithm - Deadlock Avoidance ===\n\n");
    
    // Read and parse input file
    if (!readInputFile(filename)) {
        printf("Failed to read input file. Exiting.\n");
        return 1;
    }
    
    // Calculate Need matrix
    calculateNeed();
    
    // Calculate Available resources
    calculateAvailable();
    
    // Print the system state
    printSystemState();
    
    // Run the safety algorithm
    int safeSequence[MAX_PROCESSES];
    bool safe = isSafe(safeSequence); //store safety value
    
    // Print results
    printf("========== RESULTS ==========\n\n");
    if (safe) {
        printf("The system is in a SAFE state.\n\n");
        printf("Safe Sequence: < ");
        for (int i = 0; i < num_processes; i++) {
            printf("P%d", safeSequence[i]);
            if (i < num_processes - 1) {
                printf(", ");
            }
        }
        printf(" >\n\n");
    } else {
        printf("The system is in an UNSAFE state.\n");
        printf("No safe sequence exists.\n\n");
    }
    printf("=============================\n");
    
    return 0;
}