#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct node_s Node;

struct node_s {
	int value;
	Node *left;
	Node *right;
};

Node* insert(Node *root, int v) {
	if (root == NULL) {
		root = malloc(sizeof (Node));
		root->value = v;
		root->left = NULL;
		root->right = NULL;
	} else if (v < root->value) {
		root->left = insert(root->left, v);
	} else {
		root->right = insert(root->right, v);
	}

	return root;
}

void traverse(Node *root) {
	if (root == NULL) {
		return;
	}

	traverse(root->left);
	traverse(root->right);
}

int main() {
	Node *root = NULL;
	const int max = 100000;
	int *a = malloc(sizeof (int)*max);
	clock_t start, end;

	printf("Generating random array with %d values...\n", max);

	start = clock();

	for (int i = 0; i < max; i++) {
		a[i] = rand() % 10000;
	}

	end = clock();

	printf("Done. Took %f seconds\n\n", (double) (end - start) / CLOCKS_PER_SEC);
	printf("Filling the tree with %d nodes...\n", max);

	start = clock();

	for (int i = 0; i < max; i++) {
		root = insert(root, a[i]);
	}

	end = clock();
	free(a);

	printf("Done. Took %f seconds\n\n", (double) (end - start) / CLOCKS_PER_SEC);
	printf("Traversing all %d nodes in tree...\n", max);

	start = clock();

	traverse(root);

	end = clock();
	free(root);

	printf("Done. Took %f seconds\n\n", (double) (end - start) / CLOCKS_PER_SEC);

	return (EXIT_SUCCESS);
}
