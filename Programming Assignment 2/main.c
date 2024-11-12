/* COP 3502C Assignment 2
This program is written by: Kevin Rodriguez */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define EMPTY -1
#define MAX_GARAGES 10
#define MAX_STUDENTS 10000

typedef struct Student {
  int sequenceNumber;
  struct Student* next;
} Student;
//Student struct that acts as a node holding the sequence number of the student and the next student in the queue.
typedef struct queue {
  Student* front;
  Student* rear;
  int nodeCount;
  int k;
  int th;
  int numStudents;
  int garageNum;
} queue;
//Queue struct that holds the front student pointer and rear student pointer, the number of student nodes, the k value, the threshold value, the number of students, and the garage number.
Student* createStudent(int sequence) {
  Student* newStudent = (Student*) malloc(sizeof(Student));
  if (newStudent != NULL) {
    newStudent->sequenceNumber = sequence;
    newStudent->next = NULL;
  }
  return newStudent;
}//It takes an integer, dynamically allocate a Student structure and returns a Student node.

int isEmpty(queue* qPtr) {
  return (qPtr->nodeCount == EMPTY); //checks if the queue is empty by checking if the nodeCount is equal to EMPTY.
}

void init(queue* qPtr) {
  qPtr->front = NULL;
  qPtr->rear = NULL;
  qPtr->nodeCount = EMPTY;
  qPtr->k = 0;
  qPtr->th = 0;
  qPtr->numStudents = 0;
  qPtr->garageNum = 0;
}
//initializes the queue by setting front & rear to NULL, nodeCount to empty(-1) k to 0, th to 0, numStudents to 0, and garageNum to 0.
int enqueue(queue* q, int sequenceNum) {
  Student* newStudent = createStudent(sequenceNum); //allocates memory for the student being added.
  if (newStudent == NULL) //checks if student memory was allocated properly
    return 0;
  if (isEmpty(q)) { //checks if the queue is empty, if it is then the front and rear are set to the new student.
    q->front = newStudent;
    q->rear = newStudent;
    q->nodeCount = 0;
  } else { //if the queue is not empty, the rear's next is set to the new student and the rear becomes to the new student.
    q->rear->next = newStudent;
    q->rear = newStudent;
  }

  newStudent->next = q->front; //ensures that the newStudent is pointing to the front of the queue.
  q->nodeCount++; //increases the count of nodes in the queue.
  return 1; //if successful
}

int dequeue(queue* qPtr) {
  if (isEmpty(qPtr)) {
    return EMPTY;  // Queue is empty, nothing to dequeue
  }

  Student* tempFront = qPtr->front; //preserves the queue's current front student to another pointer to be freed later.

  if (qPtr->front == qPtr->rear) {
    // Only one element in the queue
    qPtr->front = NULL;
    qPtr->rear = NULL;
  } else {
    // More than one element, update the front pointer
    qPtr->front = qPtr->front->next;
    qPtr->rear->next = qPtr->front;  // Maintain the circular linked list.
  }

  qPtr->nodeCount--;  // Decrease the node count
  free(tempFront);     // Free the removed element which was the original front.
  if (qPtr->nodeCount == 0) {
    qPtr->nodeCount = EMPTY;  //if the nodecount hits 0, we set it to empty which is the defined value we use in other functions.
  }
  return 1;  // Successful dequeue
}

int peek(queue* qPtr) {
  if (isEmpty(qPtr)) { //if the queue is empty returns -1
    return EMPTY;
  }
  else {
    return qPtr->front->sequenceNumber; //returns the sequence number of the front student without removing it.
  }
}

void createReverseCircle(queue *q) {
  for (int i = q->numStudents; i > 0; i--) { //starts from the number of students in the queue and decrements until it reaches 0.
    enqueue(q, i);
  }
} /* It takes the reference of a queue, and creates a circular singly linked list for that queue where the nodes contain sequence numbers in reverse order . For example, if n=5 it should produce a circular singly linked list starting from 5 and ending at 1 as sequence number. During this process, use the createStudent function as well as enqueu() function to add the Student to the queue.*/
void rearrangeCircle(queue* q) {
  if (isEmpty(q)) {
    return;
  } else {
    Student* current = q->front; //stores the current student in the queue.
    Student* prev = q->rear;  // The previous node starts as the rear in a circular list
    Student* next = NULL;

    // Traverse the circular linked list and reverse the next pointers
    do {
      next = current->next;  // Save the next node
      current->next = prev;  // Reverse the next pointer
      prev = current;        // Move prev to current
      current = next;        // Move to the next node
    } while (current != q->front);  // Stop when we've looped back to the front

    // After the loop, the new front will be the previous rear
    q->rear = q->front;  // The original front becomes the new rear
    q->front = prev;     // The previous node (which was the rear) becomes the new front
  }
} /*This function takes the reference of a queue and reverse the given circular singly linked list where the first node of the linked list is pointed by the front of the queue*/

void display(queue *q) {
  if (isEmpty(q)) { //returns -1
    return;
  } else {
    Student* traverse = q->front;   //has a pointer that starts at the front and traverses the queue printing the sequence number of each student.
    for (int i = 0; i < q->nodeCount; i++) {
      printf(" %d", traverse->sequenceNumber);
      traverse = traverse->next;
    }
    printf("\n");
  }
}

void phase1Elimination(queue *qPtr) {
    Student* current = qPtr->front;
    Student* prev = NULL;
    int remaining = 0;

    // Count the number of students initially
    Student* temp = qPtr->front;
    printf("\nGroup for Garage# %d\n", qPtr->garageNum);
    do {
        remaining++;
        temp = temp->next;
    } while (temp != qPtr->front);

    if (remaining <= qPtr->th) {
        return;
    }

    // Keep eliminating until the number of students left is equal to th
    while (remaining > qPtr->th) {
        // Skip k-1 students
        for (int i = 0; i < qPtr->k - 1; i++) {
            prev = current;
            current = current->next;
        }

        // Remove the k-th student
        printf("Student# %d eliminated\n", current->sequenceNumber);
        prev->next = current->next; // Bypass the current student

        if (current == qPtr->front) {
            qPtr->front = current->next;
        }
        free(current); // Free the removed student's memory
        current = prev->next; // Move to the next student
        remaining--;
        qPtr->nodeCount--;
    }
}

void phase2Elimination(queue garages[MAX_GARAGES], int numGarages) {
  int remainingGroups = numGarages;
  while (remainingGroups > 1) {
    int highestSeq = -1;   // To track the highest sequence number
    int garageToEliminate = -1;  // To track the garage with the student to be eliminated

    // Iterate through all garages to find the student with the highest sequence number at the front
    for (int i = 0; i < MAX_GARAGES; i++) {
      if (isEmpty(&garages[i])) {
        continue;
      }
      int currentSeq = peek(&garages[i]);      //sets the current sequence number to the front of the queue.

      // Find the highest sequence number
      if (currentSeq > highestSeq) {
        highestSeq = currentSeq;
        garageToEliminate = i;
      } else if (currentSeq == highestSeq) {
        // Tie-breaking: select the garage with the smaller number
        if (garageToEliminate == -1 || garages[i].garageNum < garages[garageToEliminate].garageNum) {
          garageToEliminate = i;
        }
      }
    }
    if (garageToEliminate != -1) {
      // Eliminate the student from the selected garage
      printf("Eliminated student %d from group for garage %d\n", peek(&garages[garageToEliminate]), garages[garageToEliminate].garageNum);
      dequeue(&garages[garageToEliminate]);  // Eliminate the student
      // If the garage becomes empty, reduce the count of remaining groups
      if (isEmpty(&garages[garageToEliminate])) {
        remainingGroups--;
      }
    } else {
      break;
    }
  }
  // Now, only one garage is remaining with students.
  // Find that garage and continue dequeueing until only one student remains.
  for (int i = 0; i < MAX_GARAGES; i++) {
    if (!isEmpty(&garages[i])) {
      while (garages[i].nodeCount > 1) {
        printf("Eliminated student %d from group for garage %d\n", peek(&garages[i]), garages[i].garageNum);
        dequeue(&garages[i]);  // Eliminate the student
      }
      // The last remaining student is the winner
      printf("\nStudent %d from the group for garage %d is the winner!\n", peek(&garages[i]), garages[i].garageNum);
      break;
    }
  }
}

int main(void) {
  int numGarages, garageNumber;  

  scanf("%d", &numGarages);
  if (numGarages > MAX_GARAGES) {
    printf("Too many garages");  //makes sure we don't go over 10 garages(the max).
    return 0;
  }
  queue garages[MAX_GARAGES];  //statically allocates an array of queues.

  for (int i = 0; i < MAX_GARAGES; i++) { //initializes all the garages (even the empty ones).
    init(&garages[i]);
  }

  for (int i = 0; i < numGarages; i++) {
    scanf("%d", &garageNumber);
    if (garageNumber < 1) {
      printf("Invalid garage number\n");    //makes sure the garage number entered is valid.
      return 0;
    } else if (garageNumber > MAX_GARAGES) {
      printf("Invalid garage number\n");
      return 0;
    }
    scanf("%d%d%d", &garages[garageNumber - 1].numStudents, &garages[garageNumber - 1].k, &garages[garageNumber - 1].th);  //makes sure we scan the garage number minus one to be the position of that garage in the array.
    if (garages[garageNumber - 1].numStudents > MAX_STUDENTS) {   
      printf("Too many students\n"); //makes sure we don't go over 10000 students
      return 0;
    } else if (garages[garageNumber - 1].numStudents < 2) {
      printf("Too few students\n");    //makes sure we have at least 2 students in said garage.
      return 0;
    }
    if (garages[garageNumber - 1].k < 1) {
      printf("Invalid k value\n"); //makes sure k is at least 1.
      return 0;
    }
    garages[garageNumber - 1].garageNum = garageNumber;
    createReverseCircle(&garages[garageNumber - 1]);      //creates the reversed link list of the group in said garage.

  }
  printf("Initial status of nonempty queues\n");
  for (int i = 0; i < MAX_GARAGES; i++) {
    if (!isEmpty(&garages[i])) {
      printf("%d", garages[i].garageNum);  //printing the garage number and their reversed linked list.
    }
    display(&garages[i]);
  }

  printf("\nAfter ordering status of nonempty queues\n");
  for (int i = 0; i < MAX_GARAGES; i++) {
    rearrangeCircle(&garages[i]);
    if (!isEmpty(&garages[i])) {
      printf("%d", garages[i].garageNum);     //printing the garage number with their correctly ordered linked list.
    }
    display(&garages[i]);
  }

  printf("\nPhase1 elimination\n");

  for (int i = 0; i < MAX_GARAGES; i++) {
    if (!isEmpty(&garages[i])) {
      phase1Elimination(&garages[i]);      //goes through the phase1 process calling the function.
    }
  }

  printf("\nPhase2 elimination\n");

  phase2Elimination(garages, numGarages);  //goes through phase 2 process.

  for (int i = 0; i < MAX_GARAGES; i++) {
    if (!isEmpty(&garages[i])) {
      for (int j = 0; j < garages[i].nodeCount; j++) {     //makes sure we dequeue the last remaining student(which was the winner) or really any students(not that there should be more than one).
        dequeue(&garages[i]);
      }
    }
  }

  return 0;
}