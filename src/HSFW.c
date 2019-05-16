#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#include "HSFW.h"

#define report_clear_error 2
#define report_status 10
#define report_description 11
#define move_command 20
#define home_command 21


#define report_true 255
#define report_false 0

	int verify_wheel_handle(hsfw_wheel* wheel);

	hsfw_wheel_info HSFW_EXPORT * HSFW_CALL enumerate_wheels()
	{
		struct hid_device_info *devs, *cur_dev;
		devs = hid_enumerate(HSFW_VID, HSFW_PID);
		cur_dev = devs;
		hsfw_wheel_info *root = NULL;
		hsfw_wheel_info *cur = NULL;
		while (cur_dev)
		{
			hsfw_wheel_info *tmp = (hsfw_wheel_info*)calloc(1, sizeof(struct hsfw_wheel_info) + 1);

			tmp->product_id = cur_dev->product_id;
			tmp->vendor_id = cur_dev->vendor_id;

			tmp->serial_number = calloc(sizeof(cur_dev->serial_number) + 1, sizeof(cur_dev->serial_number[0]));
			tmp->serial_number = _wcsdup(cur_dev->serial_number);
			if (cur)
			{
				cur->next = tmp;
			}
			else
			{
				root = tmp;
			}
			cur = tmp;
			cur_dev = cur_dev->next;
		}
		hid_free_enumeration(devs);

		return root;
	}

	void HSFW_EXPORT HSFW_CALL wheels_free_enumeration(hsfw_wheel_info *wheels)
	{
		struct hsfw_wheel_info *w = wheels;
		while (w) {
			struct hsfw_wheel_info *next = w->next;
			free(w->serial_number);
			free(w);
			w = next;
		}
	}

	hsfw_wheel HSFW_EXPORT * HSFW_CALL open_hsfw(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number) {
		hid_device *handle;

		handle = hid_open(vendor_id, product_id, serial_number);
		if (!handle) {
			return 0;
		}

		hsfw_wheel* wheel = (hsfw_wheel*)calloc(1, sizeof(hsfw_wheel) + 1);

		wheel->handle = handle;
		wheel->vendor_id = vendor_id;
		wheel->product_id = product_id;

		wheel->serial_number = calloc(sizeof(serial_number) + 1, sizeof(serial_number[0]));
		wheel->serial_number = _wcsdup(serial_number);
		return wheel;
	}

	void HSFW_EXPORT HSFW_CALL close_hsfw(hsfw_wheel* wheel) {
		if (!wheel)
			return;
		hid_close(wheel->handle);
		free(wheel->serial_number);
		free(wheel);
	}

	void HSFW_EXPORT exit_hsfw() {
		hid_exit();
	}

	int verify_wheel_handle(hsfw_wheel* wheel) {
		if (!wheel)
			return -1;
		if (!wheel->handle)
			return -2;
		return 0;
	}

	int HSFW_EXPORT HSFW_CALL get_hsfw_status(hsfw_wheel* wheel, wheel_status* status) {
		if (verify_wheel_handle(wheel) != 0) {
			return -1;
		}

		unsigned char raw_status[8] = { 0 };
		raw_status[0] = report_status;

		int res = hid_get_input_report(wheel->handle, raw_status, sizeof(raw_status));

		if (!res)
			return -2;

		if (raw_status[0] != report_status)
			return -3;
		status->report_id = raw_status[0];

		if (raw_status[1] != report_true && raw_status[1] != report_false)
			return -3;
		status->is_homed = raw_status[1] == report_true;

		if (raw_status[2] != report_true && raw_status[2] != report_false)
			return -3;
		status->is_homing = raw_status[2] == report_true;

		if (raw_status[3] != report_true && raw_status[3] != report_false)
			return -3;
		status->is_moving = raw_status[3] == report_true;

		if (raw_status[4] > 10)
			return -3;
		status->position = raw_status[4];

		status->error_state = raw_status[5];

		return 0;
	}

	int HSFW_EXPORT HSFW_CALL get_hsfw_description(hsfw_wheel* wheel, wheel_description* description) {
		if (verify_wheel_handle(wheel) != 0) {
			return -1;
		}

		unsigned char raw_status[8] = { 0 };
		raw_status[0] = report_description;

		int res = hid_get_input_report(wheel->handle, raw_status, sizeof(raw_status));

		if (!res)
			return -2;

		if (raw_status[0] != report_description)
			return -3;
		description->report_id = raw_status[0];

		description->firmware_major = raw_status[1];
		description->firmware_minor = raw_status[2];
		description->firmware_revision = raw_status[3];

		if (raw_status[4] < 5 || raw_status[4] > 9)
			return -3;
		description->filter_count = raw_status[4];

		description->wheel_id = (char)raw_status[5];
		description->centering_offset = raw_status[6];

		return 0;
	}

	int HSFW_EXPORT HSFW_CALL home_hsfw(hsfw_wheel* wheel) {
		if (verify_wheel_handle(wheel) != 0) {
			return -1;
		}

		unsigned char homereport[14] = { 0 };
		homereport[0] = home_command;

		int res = hid_send_feature_report(wheel->handle, homereport, sizeof(homereport));
		if (!res)
			return -1;

		res = hid_get_feature_report(wheel->handle, homereport, sizeof(homereport));
		if (!res) 
			return -2;

		if (homereport[0] != home_command)
			return -3;

		unsigned char home_resp = homereport[1];

		res = hid_get_feature_report(wheel->handle, homereport, sizeof(homereport));
		if (!res)
			return -4;

		unsigned char error_resp = homereport[1];

		if (home_resp != report_true)
			return -5;

		if (error_resp != report_false)
			return error_resp;

		return 0;
	}

	int HSFW_EXPORT HSFW_CALL move_hsfw(hsfw_wheel* wheel, unsigned short position) {
		if (verify_wheel_handle(wheel) != 0) {
			return -1;
		}

		wheel_description description;

		int res = get_hsfw_description(wheel, &description);

		if (res)
			return -2;

		if (description.filter_count < position) {
			return -3;
		}

		unsigned char movereport[14] = { 0 };
		movereport[0] = move_command;

		movereport[1] = position;

		res = hid_send_feature_report(wheel->handle, movereport, sizeof(movereport));
		if (!res)
			return -4;

		res = hid_get_feature_report(wheel->handle, movereport, sizeof(movereport));
		if (!res)
			return -5;

		if (movereport[0] != move_command)
			return -6;

		unsigned char move_resp = movereport[1];

		res = hid_get_feature_report(wheel->handle, movereport, sizeof(movereport));
		if (!res)
			return -7;

		unsigned char error_resp = movereport[1];

		if (move_resp != report_true)
			return -8;

		if (error_resp != report_false)
			return error_resp;

		return 0;
	}

	int HSFW_EXPORT HSFW_CALL clear_error_hsfw(hsfw_wheel* wheel) {
		if (verify_wheel_handle(wheel) != 0) {
			return -1;
		}

		unsigned char clear_error[4] = { 0 };
		clear_error[0] = report_clear_error;

		int res = hid_send_output_report(wheel->handle, clear_error, sizeof(clear_error));

		return 0;
	}


#ifdef __cplusplus
} /* extern "C" */
#endif