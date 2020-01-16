// This source is copyright Optec, Inc. and may not be redistributed.

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#include "libhsfw.h"

#define report_clear_error 2
#define report_status 10
#define report_description 11
#define move_command 20
#define home_command 21
#define flashops_command 22

#define flash_set_default_names 1
#define flash_update_filter_name 2
#define flash_read_filter_name 3
#define flash_update_wheel_name 4
#define flash_read_wheel_name 5
#define flash_update_centering_offset 6


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
			#ifdef _WIN32
			tmp->serial_number = _wcsdup(cur_dev->serial_number);
			#elif _WIN64
			tmp->serial_number = _wcsdup(cur_dev->serial_number);
			#else
			tmp->serial_number = wcsdup(cur_dev->serial_number);
			#endif
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
		#ifdef WIN32
		wheel->serial_number = _wcsdup(serial_number);
		#elif _WIN64
		wheel->serial_number = _wcsdup(serial_number);
		#else
		wheel->serial_number = wcsdup(serial_number);
		#endif
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

		unsigned char clear_error[2] = { 0 };
		clear_error[0] = 2;

		int res = hid_send_output_report(wheel->handle, clear_error, sizeof(clear_error));

		return 0;
	}

	int HSFW_EXPORT HSFW_CALL read_wheel_names_hsfw(hsfw_wheel* wheel, hsfw_wheel_names* names) {

		memset(names->names, 0, sizeof(names->names));
		if (verify_wheel_handle(wheel) != 0) {
			return -1;
		}

		wheel_description description;

		int res = get_hsfw_description(wheel, &description);

		if (res)
			return -2;

		int fversion = description.firmware_major * 100 + description.firmware_minor * 10 + description.firmware_revision;

		int wheel_count = 8;

		if (fversion > 100) {
			wheel_count = 11;
		}

		for (int i = 0; i < wheel_count; i++)
		{
			unsigned char wheel_name_report[14] = { 0 };

			wheel_name_report[0] = flashops_command;
			wheel_name_report[1] = flash_read_wheel_name;
			wheel_name_report[2] = 'A' + i;

			// Send the initial report
			res = hid_send_feature_report(wheel->handle, wheel_name_report, sizeof(wheel_name_report));
			if (!res)
				return -3;

			res = hid_get_feature_report(wheel->handle, wheel_name_report, sizeof(wheel_name_report));
			if (!res)
				return -3;

			unsigned char name_code1 = wheel_name_report[1];
			unsigned char name_code2 = wheel_name_report[2];
			unsigned char name_code3 = wheel_name_report[3];
			unsigned char name_code4 = wheel_name_report[4];

			res = hid_get_feature_report(wheel->handle, wheel_name_report, sizeof(wheel_name_report));
			if (!res)
				return -3;

			if(!(name_code1 == wheel_name_report[1] && name_code1 == flash_read_wheel_name))
				return -3;

			if (!(name_code2 == wheel_name_report[2] && name_code2 == 0))
				return -3;

			if (!(name_code3 == wheel_name_report[3] && name_code3 == 'A' + i))
				return -3;

			if (!(name_code4 == wheel_name_report[4] && name_code4 == 0))
				return -3;

			memcpy(&(names->names[i][0]), &wheel_name_report[6], 8);
			names->names[i][8] = 0;
		}

		return 0;
	}

	int HSFW_EXPORT HSFW_CALL read_filter_names_hsfw(hsfw_wheel* wheel, char wheel_id, hsfw_wheel_filters* filters) {
		memset(filters->names, 0, sizeof(filters->names));
		if (verify_wheel_handle(wheel) != 0) {
			return -1;
	}

		wheel_description description;

		int res = get_hsfw_description(wheel, &description);

		if (res)
			return -2;

		int fversion = description.firmware_major * 100 + description.firmware_minor * 10 + description.firmware_revision;

		int filter_count = 8;

		if (wheel_id < 'A' || wheel_id > 'K')
			return -2;

		if (!(fversion > 100)) {
			if (wheel_id > 'H') {
				return -2;
			}
		}

		if (wheel_id <= 'E')
			filter_count = 5;

		if (wheel_id >= 'I')
			filter_count = 7;

		for (int i = 0; i < filter_count; i++)
		{
			unsigned char filter_name_report[14] = { 0 };

			filter_name_report[0] = flashops_command;
			filter_name_report[1] = flash_read_filter_name;
			filter_name_report[2] = wheel_id;
			filter_name_report[3] = i + 1;

			// Send the initial report
			res = hid_send_feature_report(wheel->handle, filter_name_report, sizeof(filter_name_report));
			if (!res)
				return -3;

			res = hid_get_feature_report(wheel->handle, filter_name_report, sizeof(filter_name_report));
			if (!res)
				return -3;

			unsigned char name_code1 = filter_name_report[1];
			unsigned char name_code2 = filter_name_report[2];
			unsigned char name_code3 = filter_name_report[3];
			unsigned char name_code4 = filter_name_report[4];

			res = hid_get_feature_report(wheel->handle, filter_name_report, sizeof(filter_name_report));
			if (!res)
				return -3;

			if (!(name_code1 == filter_name_report[1] && name_code1 == flash_read_filter_name))
				return -3;

			if (!(name_code2 == filter_name_report[2] && name_code2 == 0))
				return -3;

			if (!(name_code3 == filter_name_report[3] && name_code3 == wheel_id))
				return -3;

			if (!(name_code4 == filter_name_report[4] && name_code4 == i + 1))
				return -3;
			memcpy(&(filters->names[i][0]), &filter_name_report[6], 8);
			filters->names[i][8] = 0;
		}

		return 0;
	}


#ifdef __cplusplus
} /* extern "C" */
#endif
