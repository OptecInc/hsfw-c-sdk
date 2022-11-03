// This source is copyright Optec, Inc. and may not be redistributed.

// HSFW-Windows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "libhsfw.h"
#include <stdio.h> 
#include <stdlib.h> 

#ifdef WIN32
#include <windows.h>
#elif _WIN64
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32


void PrintStatus(wheel_status* status);
void PrintDescription(wheel_description* description);


int main()
{
	hsfw_wheel_info *devs, *cur_dev;

	devs = enumerate_wheels();
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found - type: %04hx %04hx serial_number: %ls\n",cur_dev->vendor_id, cur_dev->product_id, cur_dev->serial_number);

		cur_dev = cur_dev->next;
	}

	if (devs != NULL)
	{

		hsfw_wheel* wheel = open_hsfw(devs->vendor_id, devs->product_id, devs->serial_number);
		if (wheel > 0) {

			wheel_status status;
			int res = get_hsfw_status(wheel, &status);
			if (res != 0) {
				printf(get_error_text_hsfw(res));
				printf("ERROR reading status");
				return 0;
			}
			PrintStatus(&status);
			restore_default_names_hsfw(wheel);

			if (status.error_state != 0) {
				printf("Found error:\n");
				printf(get_error_text_hsfw(status.error_state));
				printf("\nClearing Error\n");
				clear_error_hsfw(wheel);
			}

			if (status.is_homed == 0) {
				res = home_hsfw(wheel);
				if (res != 0) {
					printf(get_error_text_hsfw(res));
					printf("ERROR homing wheel");
					return 0;
				}
			}

			wheel_description description;

			res = get_hsfw_description(wheel, &description);
			if (res != 0) {

				printf(get_error_text_hsfw(res));
				printf("\nERROR reading description");
				return 0;
			}
			PrintDescription(&description);

			res = home_hsfw(wheel);
			if (res != 0) {
				printf(get_error_text_hsfw(res));
				printf("ERROR homing wheel");
				return 0;
			}

			res = get_hsfw_status(wheel, &status);
			if (res != 0) {
				printf(get_error_text_hsfw(res));
				printf("ERROR reading status");
				return 0;
			}

			while (status.is_homing) {
				res = get_hsfw_status(wheel, &status);
				if (res != 0) {
					printf(get_error_text_hsfw(res));
					printf("ERROR reading status");
					return 0;
				}
				#ifdef WIN32
					Sleep(10);
				#else
					usleep(10000);
				#endif
			}

			res = get_hsfw_description(wheel, &description);
			if (res != 0) {

				printf(get_error_text_hsfw(res));
				printf("\nERROR reading description");
				return 0;
			}

			for (int i = description.filter_count; i > 0; i--) {
				res = move_hsfw(wheel, i);
				if (res != 0) {
					printf(get_error_text_hsfw(res));
					printf("ERROR moving");
					return 0;
				}

				res = get_hsfw_status(wheel, &status);
				if (res != 0) {
					printf(get_error_text_hsfw(res));
					printf("ERROR reading status");
					return 0;
				}

				while (status.is_moving) {
					res = get_hsfw_status(wheel, &status);
					if (res != 0) {
						printf(get_error_text_hsfw(res));
						printf("ERROR reading status");
						return 0;
					}
					#ifdef WIN32
					Sleep(10);
					#else
					usleep(10000);
					#endif
				}
			}

			hsfw_wheel_names names;
			hsfw_wheel_filters filters;

			//Read all names for the specified wheel
			res = read_wheel_names_hsfw(wheel, &names);
			if (res != 0) {
				printf(get_error_text_hsfw(res));
				printf("ERROR reading names");
				return 0;
			}

			//For each wheel ID, A-K. You could also load a specific Wheel_ID.
			//For example, you can read the current wheel_id from get_hsfw_description and then request the names only for that wheel
			for (int i = 0; i < 11; i++) {
				//Print the wheel name and ID.
				printf("Wheel: %c %s\n", 'A' + i, names.names[i]);

				//Read the filter names for the wheel id
				res = read_filter_names_hsfw(wheel, 'A' + i, &filters);

				//Print the filter names and number
				for (int j = 0; j < 8; j++) {
					printf("\t Filter %d: - %s\n", j + 1, filters.names[j]);
				}
			}

			close_hsfw(wheel);
			exit_hsfw();
		}
		else{
			printf("ERROR, could not open wheel");
		}

		wheels_free_enumeration(devs);
	}
	else 
	{
		printf("No HSFW Filter Wheels found");
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
	printf("Wheel ID: %c\n", description->wheel_id);
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
