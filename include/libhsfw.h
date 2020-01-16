// This source is copyright Optec, Inc. and may not be redistributed.

#ifndef HSFW_H
#define HSFW_H

#include "hidapi.h"

#include <wchar.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
#define HSFW_EXPORT __declspec(dllexport)
#define HSFW_CALL
#else
#define HSFW_EXPORT /**< API export macro */
#define HSFW_CALL /**< API call macro */
#endif

#define HSFW_VID 0x10c4
#define HSFW_PID 0x82cd

    struct hsfw_wheel_info
    {
        /** Device Vendor ID */
		unsigned short vendor_id;
		/** Device Product ID */
		unsigned short product_id;
		/** Serial Number */
		wchar_t *serial_number;
        
		struct hsfw_wheel_info *next;
    };

	typedef struct {
		/** Device Vendor ID */
		unsigned short vendor_id;
		/** Device Product ID */
		unsigned short product_id;
		/** Serial Number */
		wchar_t *serial_number;
		hid_device *handle;
	}hsfw_wheel;

	typedef struct {
		char names[11][9];
	}hsfw_wheel_names;

	typedef struct {
		char names[8][9];
	}hsfw_wheel_filters;

	typedef struct {
		short report_id;
		bool is_homed;
		bool is_homing;
		bool is_moving;
		short position;
		short error_state;
	}wheel_status;

	typedef struct {
		short report_id;
		short firmware_major;
		short firmware_minor;
		short firmware_revision;
		short filter_count;
		char wheel_id;
		short centering_offset;
	}wheel_description;

	typedef struct hsfw_wheel_info hsfw_wheel_info;

	hsfw_wheel_info HSFW_EXPORT * HSFW_CALL enumerate_wheels();
	void  HSFW_EXPORT HSFW_CALL wheels_free_enumeration(hsfw_wheel_info *wheels);

	hsfw_wheel HSFW_EXPORT * HSFW_CALL open_hsfw(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number);
	void HSFW_EXPORT HSFW_CALL close_hsfw(hsfw_wheel* wheel);
	void HSFW_EXPORT exit_hsfw();
	int HSFW_EXPORT HSFW_CALL get_hsfw_status(hsfw_wheel* wheel, wheel_status* status);
	int HSFW_EXPORT HSFW_CALL get_hsfw_description(hsfw_wheel* wheel, wheel_description* description);

	int HSFW_EXPORT HSFW_CALL home_hsfw(hsfw_wheel* wheel);
	int HSFW_EXPORT HSFW_CALL move_hsfw(hsfw_wheel* wheel, unsigned short position);

	int HSFW_EXPORT HSFW_CALL read_wheel_names_hsfw(hsfw_wheel* wheel, hsfw_wheel_names* names);
	int HSFW_EXPORT HSFW_CALL read_filter_names_hsfw(hsfw_wheel* wheel, char wheel_id, hsfw_wheel_filters* filters);

	int HSFW_EXPORT HSFW_CALL clear_error_hsfw(hsfw_wheel* wheel);

#ifdef __cplusplus
}
#endif

#endif