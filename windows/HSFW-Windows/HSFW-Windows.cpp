// HSFW-Windows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <HSFW.h>

int main()
{
	hsfw_wheel_info *devs, *cur_dev;

	devs = enumerate_wheels();
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n serial_number: %ls\n", cur_dev->vendor_id, cur_dev->product_id, cur_dev->serial_number);

		cur_dev = cur_dev->next;
	}

	if (devs)
	{
		hsfw_wheel* wheel = open_hsfw(devs->vendor_id, devs->product_id, devs->serial_number);
	}
	else
	{

	}

    std::cout << "Hello World!\n"; 
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
