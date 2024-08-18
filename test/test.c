#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 32
#define NUM_VALUES  (1 << 16) // 65536 possible 16-bit values

typedef struct minheap_node *minheap_node_t;

// A Huffman tree node
struct minheap_node {
  int            data;
  unsigned       freq;
  minheap_node_t left;
  minheap_node_t right;
};

struct minheap {
  unsigned        size;     // Current size of min heap
  unsigned        capacity; // Capacity of min heap
  minheap_node_t *array;    // Array of minheap node pointers
};

// A utility function to allocate a new min heap node with given data and frequency of the data
minheap_node_t newNode(int data, unsigned freq) {
  minheap_node_t temp = malloc(sizeof(struct minheap_node));
  temp->left = temp->right = NULL;
  temp->data               = data;
  temp->freq               = freq;
  return temp;
}

// A utility function to create a min heap of given capacity
struct minheap *createMinHeap(unsigned capacity) {
  struct minheap *heap = malloc(sizeof(struct minheap));
  heap->size           = 0;
  heap->capacity       = capacity;
  heap->array          = malloc(heap->capacity * sizeof(minheap_node_t));
  return heap;
}

// A utility function to swap two min heap nodes
void swapMinHeapNode(minheap_node_t *a, minheap_node_t *b) {
  minheap_node_t t = *a;
  *a               = *b;
  *b               = t;
}

// The standard minHeapify function.
void minHeapify(struct minheap *minHeap, int idx) {
  int smallest = idx;
  int left     = 2 * idx + 1;
  int right    = 2 * idx + 2;

  if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
    smallest = left;

  if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
    smallest = right;

  if (smallest != idx) {
    swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
    minHeapify(minHeap, smallest);
  }
}

// A utility function to check if size of heap is 1 or not
int isSizeOne(struct minheap *minHeap) {
  return (minHeap->size == 1);
}

// A standard function to extract minimum value node from heap
minheap_node_t extractMin(struct minheap *minHeap) {
  minheap_node_t temp = minHeap->array[0];
  minHeap->array[0]   = minHeap->array[minHeap->size - 1];
  --minHeap->size;
  minHeapify(minHeap, 0);
  return temp;
}

// A utility function to insert a new node to Min Heap
void insertMinHeap(struct minheap *minHeap, minheap_node_t minHeapNode) {
  ++minHeap->size;
  int i = minHeap->size - 1;

  while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
    minHeap->array[i] = minHeap->array[(i - 1) / 2];
    i                 = (i - 1) / 2;
  }
  minHeap->array[i] = minHeapNode;
}

// A utility function to build min heap
void buildMinHeap(struct minheap *minHeap) {
  int n = minHeap->size - 1;
  int i;

  for (i = (n - 1) / 2; i >= 0; --i)
    minHeapify(minHeap, i);
}

// A utility function to print an array of size n
void printArr(int arr[], int n) {
  int i;
  for (i = 0; i < n; ++i)
    printf("%d", arr[i]);

  printf("\n");
}

// Utility function to check if this node is leaf
int isLeaf(minheap_node_t root) {
  return !(root->left) && !(root->right);
}

// Creates a min heap of capacity equal to size and inserts all character of data[] in min heap.
// Initially size of min heap is equal to capacity
struct minheap *createAndBuildMinHeap(int data[], int freq[], int size) {
  struct minheap *minHeap = createMinHeap(size);

  for (int i = 0; i < size; ++i)
    minHeap->array[i] = newNode(data[i], freq[i]);

  minHeap->size = size;
  buildMinHeap(minHeap);
  return minHeap;
}

// The main function that builds Huffman tree
minheap_node_t buildHuffmanTree(int data[], int freq[], int size) {
  minheap_node_t left, right, top;

  // Step 1: Create a min heap of capacity equal to size.  Initially, there are modes equal to size.
  struct minheap *minHeap = createAndBuildMinHeap(data, freq, size);

  // Iterate while size of heap doesn't become 1
  while (!isSizeOne(minHeap)) {
    // Step 2: Extract the two minimum freq items from min heap
    left  = extractMin(minHeap);
    right = extractMin(minHeap);

    // Step 3:  Create a new internal node with frequency equal to the sum of the two nodes frequencies.
    // Make the two extracted node as left and right children of this new node. Add this node to the min heap
    // '$' is a special value for internal nodes, not used
    top = newNode(-1, left->freq + right->freq);

    top->left  = left;
    top->right = right;

    insertMinHeap(minHeap, top);
  }

  // Step 4: The remaining node is the root node and the tree is complete.
  return extractMin(minHeap);
}

// Prints huffman codes from the root of Huffman Tree. It uses arr[] to store codes
void printCodes(minheap_node_t root, int arr[], int top, char *codeTable[NUM_VALUES]) {
  // Assign 0 to left edge and recur
  if (root->left) {
    arr[top] = 0;
    printCodes(root->left, arr, top + 1, codeTable);
  }

  // Assign 1 to right edge and recur
  if (root->right) {
    arr[top] = 1;
    printCodes(root->right, arr, top + 1, codeTable);
  }

  // If this is a leaf node, then it contains one of the input integers, print the integer and its code
  if (isLeaf(root)) {
    printf("Integer: %d, Code: ", root->data);
    printArr(arr, top);
    // Store the code in the code table
    codeTable[root->data] = (char *)malloc(top + 1);
    for (int i = 0; i < top; i++) {
      codeTable[root->data][i] = '0' + arr[i];
    }
    codeTable[root->data][top] = '\0';
  }
}

// The main function that builds a Huffman Tree and print codes by traversing the built Huffman Tree
void HuffmanCodes(int data[], int freq[], int size, char *codeTable[NUM_VALUES]) {
  // Construct Huffman Tree
  minheap_node_t root = buildHuffmanTree(data, freq, size);

  // Print Huffman codes using the Huffman tree built above
  int arr[MAX_TREE_HT], top = 0;

  printCodes(root, arr, top, codeTable);
}

// Function to encode the given data array using the Huffman codes
void encode(int data[], int length, char *codeTable[NUM_VALUES]) {
  printf("\nEncoded binary stream: \n");
  for (int i = 0; i < length; i++) {
    printf("%s", codeTable[data[i]]);
  }
  printf("\n");
}

int main() {
  int arr[] = {10, 20, 10, 30, 20, 30, 30, 40, 50, 10};
  int n     = sizeof(arr) / sizeof(arr[0]);

  int freq[NUM_VALUES] = {0};
  for (int i = 0; i < n; i++) {
    freq[arr[i]]++;
  }

  int uniqueData[NUM_VALUES];
  int uniqueFreq[NUM_VALUES];
  int size = 0;
  for (int i = 0; i < NUM_VALUES; i++) {
    if (freq[i] > 0) {
      uniqueData[size] = i;
      uniqueFreq[size] = freq[i];
      size++;
    }
  }

  char *codeTable[NUM_VALUES] = {0};

  HuffmanCodes(uniqueData, uniqueFreq, size, codeTable);

  encode(arr, n, codeTable);

  return 0;
}
