// HSFW-Windows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <HSFW.h>
#include <stdlib.h> 

void PrintStatus(wheel_status* status);
void PrintDescription(wheel_description* description);

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32


void main()
{
	hsfw_wheel_info *devs, *cur_dev;

	devs = enumerate_wheels();
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found - type: %04hx %04hx serial_number: %ls\n",cur_dev->vendor_id, cur_dev->product_id, cur_dev->serial_number);

		cur_dev = cur_dev->next;
	}



	if (devs)
	{
		hsfw_wheel* wheel = open_hsfw(devs->vendor_id, devs->product_id, devs->serial_number);
		if (wheel > 0) {


			wheel_status status;
			if (get_hsfw_status(wheel, &status) < 0) {
				printf("ERROR");
				return;
			}
			PrintStatus(&status);

			wheel_description description;
			if (get_hsfw_description(wheel, &description) < 0) {
				printf("ERROR");
				return;
			}
			PrintDescription(&description);

			if(home_hsfw(wheel)) {
				printf("ERROR");
				return;
			}

			if (get_hsfw_status(wheel, &status) < 0) {
				printf("ERROR");
				return;
			}

			while (status.is_homing) {
				if (get_hsfw_status(wheel, &status) < 0) {
					printf("ERROR");
					return;
				}
				Sleep(10);
			}

			if (get_hsfw_description(wheel, &description) < 0) {
				printf("ERROR");
				return;
			}


			for (int i = description.filter_count; i > 0; i--) {
				if (move_hsfw(wheel, 1)) {
					printf("ERROR");
					return;
				}
				if (get_hsfw_status(wheel, &status) < 0) {
					printf("ERROR");
					return;
				}

				while (status.is_moving) {
					if (get_hsfw_status(wheel, &status) < 0) {
						printf("ERROR");
						return;
					}
					Sleep(10);
				}

			}


			PrintStatus(&status);

			close_hsfw(wheel);
			exit_hsfw();
		}

		wheels_free_enumeration(devs);
	}
}


void PrintStatus(wheel_status* status) {
	printf("Report ID: %d\n", status->report_id);
	printf("Position: %d\n", status->position);
	printf("Moving: %d\n", status->is_moving);
	printf("Homed: %d\n", status->is_homed);
	printf("Homing: %d\n", status->is_homing);
	printf("Error: %d\n", status->error_state);
}

void PrintDescription(wheel_description* description) {
	printf("Report ID: %d\n", description->report_id);
	printf("Firmware: V%d.%d.%d\n", description->firmware_major, description->firmware_minor, description->firmware_revision);
	printf("Filter Count: %d\n", description->filter_count);
	printf("Wheel ID: %d\n", description->wheel_id);
	printf("Centering Offset: %d\n", description->centering_offset);
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
