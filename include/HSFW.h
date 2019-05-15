#ifndef HSFW_H
#define HSFW_H

#include "hidapi.h"

#include <wchar.h>

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
		short report_id;
		short is_homed;
		short is_homing;
		short is_moving;
		short position;
		short error_state;
	}wheel_status;

	typedef struct hsfw_wheel_info hsfw_wheel_info;

	hsfw_wheel_info HSFW_EXPORT * HSFW_CALL enumerate_wheels();
	void  HSFW_EXPORT HSFW_CALL wheels_free_enumeration(hsfw_wheel_info *wheels);

	hsfw_wheel HSFW_EXPORT  * HSFW_CALL open_hsfw(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number);

	void HSFW_EXPORT HSFW_CALL close_hsfw(hsfw_wheel* wheel);

	void HSFW_EXPORT exit_hsfw();

#ifdef __cplusplus
}
#endif

#endif