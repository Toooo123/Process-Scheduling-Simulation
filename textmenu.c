#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "textmenu.h"
#include "process.h"
#include "message.h"

static int currentProcessId = 0;

char inputChar() {
    char input = getchar();
    while (input == '\n') {
        input = getchar();
    }
    input = toupper(input);
    return input;
}

int inputPriorityInt() {
    int input;
    printf("Input Priority Int (0 - high, 1 - norm , 2 - low): ");
    scanf("%d", &input);

    while (input < 0 || input > 2) {
        printf("Invalid Int\n");
        scanf("%d", &input);
    }
    return input;
}

int inputPID() {
    int input;
    printf("Input Process ID: ");
    scanf("%d", &input);
    return input;
}

Message * inputMessage() {
    Message * message = malloc(sizeof(Message));
    if (message == NULL) {
        return NULL;
    }

    message->msg = malloc(sizeof(char) * 40);
    if (message->msg == NULL) {
        free(message);
        return NULL;
    }

    printf("Input Message (max 40 char): ");
    scanf("%s", message->msg);
    return message;
}

void printIntroduction() {
    printf("** PCB's and Processing Scheduling Simulation **\n");
    printf("* Process Commands *\n");
    printf("C - Create, F - Fork, K - Kill, E - Exit, Q - Quantum\n");
    printf("* Message Commands *\n");
    printf("S - Send, R - Receive, Y - Reply\n");
    printf("* Semaphore Commands *\n");
    printf("N - New Semaphore, P - Semaphore P, V - Semaphore V\n");
    printf("* Info Commands *\n");
    printf("I - Procinfo, T - Totalinfo\n");
}

void printInvalidSetup() {
    printf("ERROR: Lists can't not be intialized");
}

void printInvalidCommand() {
    printf("Invalid Command\n");
}

void printProcessChange() {
    PCB * process = Process_getCurrentProcess();
    if (process == NULL) {
        return;
    }
    int pid = process->PID;
    if (currentProcessId != pid ) {
        printf("Process %d is running\n", pid);
        currentProcessId = pid;
    }

    if (process->isMessageReceived) {
        Message * message = Message_getMessage(process);
        printf("SUCESS: (%d -> %d): %s\n", message->sender, message->receiver, message->msg);
        process->isMessageReceived = false;
    }
}

void printCreateReport(int pid) {
    if (pid == -1) {
        printf("FAILED: Process could not be created\n");
    } else {
        printf("SUCESS: Process %d created\n", pid);
    }
}

void printForkReport(int pid) {
    if (pid == -1) {
        printf("FAILED: Process could not be forked\n");
    } else {
        printf("SUCESS: Process %d forked\n", pid);
    }
}

void printKillReport(int pid) {
    switch (pid) {
        case -2:
            printf("FAILED: Process init could not be killed\n");
            break;
        case -1:
            printf("FAILED: Process could not be found\n");
            break;
        case 0:
            printf("SUCESS: Process init is killed\n");
            break;
        default:
            printf("SUCESS: Process %d is killed\n", pid);
    }
}

void printExitReport(int pid) {
    switch (pid) {
        case -2:
            printf("FAILED: Process init could not be exited\n");
            break;
        case -1:
            printf("SUCESS: Process init is exited\n");
            break;
        case 0:
            printf("SUCESS: Process init is running\n");
            break;
        default:
            printf("SUCESS: Process %d exited\n", pid);
    }
}

void printQuantumReport(int pid) {
    currentProcessId = pid;
    if (pid == 0) {
        printf("Process init is running\n");
    } else {
        printf("Process %d is running\n", pid);
    }
}

void printSendReport(int pid, Message * message) {
    if (pid < 0) {
        printf("FAILED: Send failed\n");
        free(message->msg);
        free(message);
        return;
    }

    printf("SUCESS: Process %d sent to Send Queue\n", pid);
}

void printReceiveReport(int pid, Message * message) {
    if (pid < 0) {
        printf("FAILED: Receieve failed\n");
        return;
    }

    if (message != NULL) {
        printf("SUCESS: (%d -> %d): %s\n", message->sender, message->receiver, message->msg);
        free(message->msg);
        free(message);
    } else {
        printf("SUCESS: Process %d move to Recieve Queue\n", pid);
    }
}

void printReplyReport(int pid) {
    if (pid < 0) {
        printf("FAILED: Receieve failed\n");
        return;
    }

    printf("SUCESS: Process %d removed from Sender Queue\n", pid);
}

void printNumToPriority(int priority) {
    switch (priority) {
        case 0:
            printf("High");
            break;
        case 1:
            printf("Norm");
            break;
        case 2:
            printf("Low");
            break;
        default:
            break;
    }
}

void printNumToState(int state) {
    switch (state) {
        case PROCESS_RUNNING:
            printf("Running");
            break;
        case PROCESS_READY:
            printf("Ready");
            break;
        case PROCESS_BLOCKED:
            printf("Blocked");
            break;
        default:
            break;
    }
}

void procinfo(int pid) {
    PCB * process = Process_getProcess(pid);

    if (process == NULL) {
        printf("Proc info: Process %d not found\n", pid);
        return;
    }

    printf("Procinfo: PID - %d", pid);
    if (pid == 0) {
        printf(" (init)");
    }
    printf(", Priority - ");
    printNumToPriority(process->priority);
    printf(", State - ");
    printNumToState(process->state);
    printf(", Messages Waiting - %d", process->messages->count);
    printf("\n");
}

void printArray(int * array) {
    int count = array[0];
    if (count == 0) {
        printf("Empty \n");
    } else {
        printf("{");
        for (int i = 0; i < count-1; i++) {
            printf("%d, ", array[i+1]);
        }
        printf("%d}\n", array[count]);
    }
    free(array);
}

void totalinfo() {
    int * array;
    printf("* Totalinfo *\n");

    PCB * process = Process_getCurrentProcess();
    printf("Running: ");
    if (process->PID == 0) {
        printf("0 - init\n");
    } else {
        printf("%d\n", process->PID);
    }

    printf("High Ready Queue: ");
    array = Process_getQueueArray(PRIORITY_HIGH);
    printArray(array);

    printf("Norm Ready Queue: ");
    array = Process_getQueueArray(PRIORITY_NORM);
    printArray(array);

    printf("Low Ready Queue: ");
    array = Process_getQueueArray(PRIORITY_LOW);
    printArray(array);

    printf("Send Waiting Queue: ");
    array = Message_getQueueArray(QUEUE_SEND);
    printArray(array);

    printf("Receive Waiting Queue: ");
    array = Message_getQueueArray(QUEUE_RECEIVE);
    printArray(array);
}