#include <stdio.h>

/* Interpolation search */

int main()
{
	int arr[10] = {4, 10, 15, 21, 29, 32, 38, 41, 42, 47};
	int low, mid, len, up;
	int x = 10;
	int index = -1;

	low = 0;
	len = sizeof(arr)/sizeof(arr[0]);
	up = len-1;

	while(low <= up) {
		mid = low + (((double)(up - low)/(arr[up]-arr[low])) * (x - arr[low]));

		if (arr[mid] == x) {
			index = mid;
			break;
		} else if(arr[mid] < x) {
			low = mid + 1;
		} else {
			up = mid -1;
		}
		
	}
	printf("Found index:%d\n", index);

}
