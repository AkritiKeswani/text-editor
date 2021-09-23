// #Akriti Keswani - ark365, CS283-001
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

typedef struct Node {
    char* ptr;
    struct Node* next;
    struct Node* prev;
} Node;

void populateLinkedList(Node*, char*, int*);
int findLengthUntilNextNewLine(int, char*);
void printLine(Node*);
void printFile(Node*);
void printFileWithLineNumbers(Node*);
void deleteLine(Node**, Node*);
Node* getPointerToLine(Node*, int);
void moveLine(Node**, Node*, int, int*);
char* readInput();
void writeToFile(Node*, char*);
Node* searchString(Node*, char*);
char* newpipe(char**, int, char*);
void runcommand(char*, int, int);
void moveLineToEnd(Node**, Node*, int*, int*);
void freeList(Node*);

int main(int argc, char *argv[]) {
    int fd;
    char choice[40];
    struct stat stats;
    char* mem;
    int sizeOfFile;
    int pos;
    Node* head = (Node*) malloc(sizeof(Node));
    Node* currentLine;
    int currentLineNumber;
    int numberOfLines = 0;

    // Takes in command line argumemnt naming a file
    if (argc == 1) {
        printf("File name required. Usage: ./texteditor FileName");
        return 0;
    }

    // if file doesnt exist it creates it for you - error handling is just if it cannot open the file thats given as input on command line
    else if (argc == 2) {
        fd = open(argv[1], O_CREAT | O_RDONLY);
        if (fd < 0) {
            printf("Error in opening the file.");
            return 0;
        }
    }

    flock(fd, LOCK_EX);

    // Store the file in a mem pointer
    fstat(fd, &stats);
    mem = (char *) malloc(stats.st_size);
    sizeOfFile = read(fd, mem, stats.st_size); 

    currentLineNumber = 0;

    populateLinkedList(head, mem, &numberOfLines);
    // free(mem);

    currentLine = head;
    
    printLine(currentLine);

    printf("Enter the command: ");

    while (fgets(choice, sizeof(choice), stdin) != NULL) {
        if (strcmp(choice, "q\n") == 0) {
            break;
        }
        else if (strcmp(choice, "p\n") == 0) {
            printFile(head);
        }
        else if (strcmp(choice, "n\n") == 0) {
            printf("%s\n", head->ptr);
            printFileWithLineNumbers(head);
        }
        else if (strcmp(choice, "d\n") == 0) {
            deleteLine(&head, currentLine);
        }
        else if (choice[0] == 'm') {
            int lineNumber;
            if (choice[1] == '$') {
                moveLineToEnd(&head, currentLine, &currentLineNumber, &numberOfLines);
                lineNumber = numberOfLines;
            }
            else {
                lineNumber = atoi(choice + 1);
                moveLine(&head, currentLine, lineNumber - 1, &currentLineNumber);
            }
        }
        else if (strcmp(choice, "a\n") == 0) {
            Node *nextLine = currentLine->next;
            Node *previousLine = currentLine;
            char *readLine;
            while (strcmp(readLine = readInput(), ".\n") != 0) {
                Node* newNode = (Node*) malloc(sizeof(Node));
                newNode->ptr = readLine;
                previousLine->next = newNode;
                newNode->prev = previousLine;
                previousLine = newNode;
            }
            previousLine->next = nextLine;
        }
        
        else if (strcmp(choice, "i\n") == 0) {
            Node *nextLine = currentLine;
            Node *previousLine = currentLine->prev;
            char *readLine;
            while (strcmp(readLine = readInput(), ".\n") != 0) {
                Node* newNode = (Node*) malloc(sizeof(Node));
                newNode->ptr = readLine;
                if (previousLine != NULL)
                    previousLine->next = newNode;
                else
                    head = newNode;
                newNode->prev = previousLine;
                previousLine = newNode;
            }
            previousLine->next = nextLine;
        }
        else if (strcmp(choice, "w\n") == 0) {
            writeToFile(head, argv[1]);
        }
        //added code for assignment 7 
        else if (choice[0] == 's') {
            char *stringToFind = choice + 2;
            Node *foundNode = searchString(currentLine, stringToFind);
            if (foundNode) {
                currentLine = foundNode;
            }     
        }

        else if (choice[0] == '|') {
            char *newString;
			for (Node* temp = head; temp != NULL; temp = temp->next)
			{
				newString = newpipe(&(temp->ptr), strlen(temp->ptr), (choice+1));
				free(temp->ptr);
				temp->ptr = newString;
			}
        }

        else {
            int line = atoi(choice);
            if (line == 0) {
                printf("Wrong number entered, please try again. \n");
            }
            else {
                currentLineNumber = line - 1;
                currentLine = getPointerToLine(head, currentLineNumber);
            }
            
        }
        printLine(currentLine);
        printf("\nEnter the command: ");
    }
    freeList(head);
    flock(fd, LOCK_UN);
    close(fd);

}

void populateLinkedList(Node* head, char* mem, int *numberOfLines) {
    int pos = 0; // current position into the file
    Node* currentNode;

    head->prev = NULL;
    if (mem[pos] != '\0')
        head->ptr = (char*) malloc(findLengthUntilNextNewLine(pos, mem));
    else
        head->ptr = NULL;

    currentNode = head;

    int cStringPos = 0;

    while (mem[pos] != '\0') {
        currentNode->ptr[cStringPos++] = mem[pos];
        if (mem[pos] == '\n') {
            (*numberOfLines)++;
            currentNode->ptr[cStringPos++] = '\0';
            cStringPos = 0;
            if (mem[pos + 1] != '\0') {
                Node* nextNode = (Node*) malloc(sizeof(Node));
                nextNode->ptr = (char*) malloc(findLengthUntilNextNewLine(pos + 1, mem));
                nextNode->prev = currentNode;
                currentNode->next = nextNode;
                currentNode = nextNode;
            }
        }
        pos++;
    }
}

int findLengthUntilNextNewLine(int pos, char* mem) {
    int length = 1;
    while (mem[pos + length] != '\n' && mem[pos + length] != '\0') {
        length++;
    }
    return length + 2; // extra two for the new line character, and the end of line char
}

void printLine(Node* node) {
    if (node->ptr != NULL)
        printf("Current Line: %s", node->ptr);
}

void printFile(Node* head) {
    printf("-------\n");
    Node* node = head;
    while (node != NULL) {
        if (node->ptr != NULL)
            printf("%s", node->ptr);
        node = node->next;
    }
    printf("-------\n");
}

void printFileWithLineNumbers(Node* head) {
    printf("-------\n");
    int lineNumber = 1;
    Node* node = head;
    while (node != NULL) {
        if (node->ptr != NULL) {
            printf("%d| %s", lineNumber, node->ptr);
        }
        else {
            lineNumber--;
        }
        node = node->next;
        lineNumber++;
    }
    printf("-------\n");
}

Node* getPointerToLine(Node* head, int lineNumber) {
    int currentLineNumber = 0;
    while (currentLineNumber != lineNumber) {
        currentLineNumber++;
        head = head->next;
    }
    return head;
}

void deleteLine(Node** head, Node* currentLine) {
    if (currentLine->prev != NULL)
        (currentLine->prev)->next = currentLine->next;
    else
        *head = currentLine->next;
    if (currentLine->next != NULL)
        (currentLine->next)->prev = currentLine->prev;
    free(currentLine);
}

void moveLine(Node** head, Node* currentLine, int lineNumber, int *currentLineNumber) {
    if (*currentLineNumber == lineNumber)
        return;
    *currentLineNumber = lineNumber;
    if (currentLine->prev != NULL)
        (currentLine->prev)->next = currentLine->next;
    else
        *head = currentLine->next;
    if (currentLine->next != NULL)
        (currentLine->next)->prev = currentLine->prev;
    Node* targetLine = *head;
    lineNumber--;
    while (lineNumber > 0) {
        targetLine = targetLine->next;
        lineNumber--;
    }
    currentLine->next = targetLine;
    if (targetLine->prev != NULL) {
        currentLine->prev = targetLine->prev;
        (targetLine->prev)->next = currentLine;
        targetLine->prev = currentLine;
    }
    else {
        *head = currentLine;
    }
}

void moveLineToEnd(Node** head, Node* currentLine, int *currentLineNumber, int *numberOfLines) {
    Node* newHead;
    if (*numberOfLines == 1) return;
    if (*currentLineNumber != 1) {
        newHead = (*head)->next;
    }
    else {
        newHead = (*head)->next->next;
    }
    
    Node* lastNode = *head;

    *currentLineNumber = *numberOfLines + 1;

    if (currentLine->prev != NULL)
        (currentLine->prev)->next = currentLine->next;
    if (currentLine->next != NULL)
        (currentLine->next)->prev = currentLine->prev;


    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    lastNode->next = currentLine;
    currentLine->prev = lastNode;
    currentLine->next = NULL;
    if (*head == currentLine) {
        *head = newHead;
    }
}

char* readInput() {
    char* line = malloc(100), *linePointer = line;
    int lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    while (1) {
        c = fgetc(stdin);

        if(--len == 0) {
            len = lenmax;

            char * lineNew = realloc(linePointer, lenmax *= 2);

            line = lineNew + (line - linePointer);
            linePointer = lineNew;
        }

        if((*line++ = c) == '\n')
            break;
    }

    *line = '\0';

    return linePointer;
}

void writeToFile(Node *head, char *fileName) {
    FILE *fp;
    fp = fopen("temp", "w+");
    while (head != NULL) {
        if (head->ptr != NULL)
            fputs(head->ptr, fp);
        head = head->next;
    }
    fclose(fp);   
    rename("temp", fileName);
}

Node* searchString(Node *currentLine, char *str) {
    str = strtok(str, "\n");
    Node* node = currentLine->next;
    char *rt; 
    while (node != NULL) {
        // if next line is present
    if (node->ptr != NULL) {
        rt = strstr(node->ptr, str);
        if (rt != NULL) {   // found something
            printf("String found. \n");
            return node;
        }
    }
    node = node->next;
    }
    printf("String not found. \n");
    return NULL;
}

char* newpipe(char** buf, int size, char* command) {
	int toChild[2];
	int fromChild[2];
	int pid1;
	pipe(toChild);
	pipe(fromChild);

	if ( (pid1=fork()) == 0) {
		close(toChild[1]);
		close(fromChild[0]);
		runcommand(command, fromChild[1], toChild[0]);
		close(toChild[0]);
		close(fromChild[1]);
		exit(1);
	}
	else if (pid1 > 0) {
		close(toChild[0]);
		close(fromChild[1]);
		write(toChild[1], *buf, strlen(*buf));
		close(toChild[1]);
		char *new;
		new = malloc(size*2);
		read(fromChild[0], new, size);	
		return new;
	}
    return NULL;
}

void runcommand(char* command, int fdw, int fdr) {
	int n = strlen(command);
	char *toks[32]; 
	if(command[n-1] == '\n')
		command[n-1] = '\0';

	toks[0] = strtok(command, " \t\n");
	for (int i = 1; i < 32; i++)
	{
		toks[i] = strtok(NULL, " \t\n");
		if (toks[i] == NULL)
			break;
	}
    if(fdw != -1) {
        close(1);
        dup(fdw);
    }
    if(fdr != -1) {
        close(0);
        dup(fdr);
    }	
	execvp(toks[0], toks);
	perror("execvp");
	exit(1);
}

void freeList(Node* head) {
   Node* tmp;
   while (head != NULL) {
       tmp = head;
       head = head->next;
       free(tmp);
    }
}
