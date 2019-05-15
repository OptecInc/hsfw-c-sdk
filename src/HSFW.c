#ifdef __cplusplus
extern "C"
{
#endif

#include "HSFW.h"


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
			tmp->serial_number = malloc(sizeof(cur_dev->serial_number));
			memcpy(tmp->serial_number, cur_dev->serial_number, sizeof(cur_dev->serial_number));

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
			return -1;
		}

		hsfw_wheel* wheel = (hsfw_wheel*)calloc(1, sizeof(hsfw_wheel));

		wheel->handle = handle;
		wheel->vendor_id = vendor_id;
		wheel->product_id = product_id;
		wheel->serial_number = malloc(sizeof(serial_number));
		memcpy(wheel->serial_number, serial_number, sizeof(serial_number));

		return wheel;
	}

	void HSFW_EXPORT HSFW_CALL close_hsfw(hsfw_wheel* wheel) {
		if (!wheel)
			return;
		hid_close(wheel->handle);
		free(wheel->handle);
		free(wheel->serial_number);
		free(wheel);
	}

	void HSFW_EXPORT exit_hsfw() {
		hid_exit();
	}

#ifdef __cplusplus
} /* extern "C" */
#endif
