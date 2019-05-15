#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#include "HSFW.h"

#define report_status 10

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
			hsfw_wheel_info *tmp = (hsfw_wheel_info*) calloc(1, sizeof(struct hsfw_wheel_info));

            tmp->product_id = cur_dev->product_id;
            tmp->vendor_id = cur_dev->vendor_id;

			tmp->serial_number = calloc(sizeof(cur_dev->serial_number) + 1, sizeof(cur_dev->serial_number[0]));
			wcsncpy_s(tmp->serial_number, (sizeof(cur_dev->serial_number) + 1) * sizeof(cur_dev->serial_number[0]), cur_dev->serial_number, sizeof(cur_dev->serial_number) * sizeof(cur_dev->serial_number[0]));

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

		hsfw_wheel* wheel = (hsfw_wheel*)calloc(1, sizeof(hsfw_wheel));

		wheel->handle = handle;
		wheel->vendor_id = vendor_id;
		wheel->product_id = product_id;

		serial_number = calloc(sizeof(serial_number) + 1, sizeof(serial_number[0]));
		wcsncpy_s(serial_number, (sizeof(serial_number) + 1) * sizeof(serial_number[0]), serial_number, sizeof(serial_number)  * sizeof(serial_number[0]));

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

		memcpy(status, status, sizeof(wheel_status));

		return 0;
	}

#ifdef __cplusplus
} /* extern "C" */
#endif
