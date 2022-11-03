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
#define HSFW_CALL	/**< API call macro */
#endif

#define HSFW_MAJOR_VERSION 0
#define HSFW_MINOR_VERSION 0
#define HSFW_PATCH_VERSION 0

#define HSFW_VID 0x10c4
#define HSFW_PID 0x82cd

#define INVALID_WHEEL_HANDLE -1
#define INVALID_DEVICE_RESPONSE -2
#define INVALID_ARGUMENT -3

#define DEVICE_ERROR_NONE 0
#define DEVICE_ERROR_12V 1
#define DEVICE_ERROR_STALLED 2
#define DEVICE_ERROR_PARAMETER 3
#define DEVICE_ERROR_HOME_WHILE_MOVING 4
#define DEVICE_ERROR_MOVE_WHILE_MOVING 5
#define DEVICE_ERROR_NOT_HOMED 6
#define DEVICE_ERROR_NO_WHEEL 7
#define DEVICE_ERROR_NO_WHEELID 8

	struct hsfw_wheel_info
	{
		/** Device Vendor ID */
		unsigned short vendor_id;
		/** Device Product ID */
		unsigned short product_id;
		/** Serial Number */
		wchar_t* serial_number;
		/** Next available wheel */
		struct hsfw_wheel_info* next;
	};

	typedef struct
	{
		/** Device Vendor ID */
		unsigned short vendor_id;
		/** Device Product ID */
		unsigned short product_id;
		/** Serial Number */
		wchar_t* serial_number;
		/** Raw HID handle */
		hid_device* handle;
	} hsfw_wheel;

	typedef struct
	{
		/** A set of 0 terminated names for Wheels A-K indexed from A=0*/
		char names[11][9];
	} hsfw_wheel_names;

	typedef struct
	{
		/** A set of 0 terminated names for for filter slots 1-8*/
		char names[8][9];
	} hsfw_wheel_filters;

	typedef struct
	{
		short report_id;
		/** True if the Wheel is homed*/
		bool is_homed;
		/** True if the Wheel is homing*/
		bool is_homing;
		/** True if the Wheel is moving*/
		bool is_moving;
		/** The current position of the Wheel*/
		short position;
		/** The current error state of the Wheel*/
		short error_state;
	} wheel_status;

	typedef struct
	{
		short report_id;
		/** The wheel major firmware version*/
		short firmware_major;
		/** The wheel minor firmware version*/
		short firmware_minor;
		/** The wheel revision firmware version*/
		short firmware_revision;
		/** The number of filters in the current wheel*/
		short filter_count;
		/** The wheel id of the current wheel*/
		char wheel_id;
		/** the centering offset of the current wheel*/
		short centering_offset;
	} wheel_description;

	typedef struct hsfw_wheel_info hsfw_wheel_info;

	/*
	* Gets an hsfw_wheel_info containing information on attached wheels and a pointer to the next info
	*/
	hsfw_wheel_info HSFW_EXPORT* HSFW_CALL enumerate_wheels();

	/*
	* Frees the hsfw_wheel_info. It should not be used after freeing
	*/
	void HSFW_EXPORT HSFW_CALL wheels_free_enumeration(hsfw_wheel_info* wheels);

	/*
	* Opens the specified HSFW, returning a struct with a handle and wheel info
	*/
	hsfw_wheel HSFW_EXPORT* HSFW_CALL open_hsfw(unsigned short vendor_id, unsigned short product_id, const wchar_t* serial_number);
	/*
	* Closes the HSFW connection and frees the associated memory
	*/
	void HSFW_EXPORT HSFW_CALL close_hsfw(hsfw_wheel* wheel);
	/*
	* This cleans up any memory used by libhsfw or hidapi. The library may not be used after this is called.
	*/
	void HSFW_EXPORT exit_hsfw();
	/*
	* Gets the HSFW status struct of the HSFW, status values can change frequently. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL get_hsfw_status(hsfw_wheel* wheel, wheel_status* status);
	/*
	* Gets the description struct of the HSFW, description changes infrequently. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL get_hsfw_description(hsfw_wheel* wheel, wheel_description* description);

	/*
	* Homes the specified wheel. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL home_hsfw(hsfw_wheel* wheel);
	/*
	* Homes the specified wheel. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL move_hsfw(hsfw_wheel* wheel, unsigned short position);

	/*
	* Reads all wheel names, pass in an existing names struct. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL read_wheel_names_hsfw(hsfw_wheel* wheel, hsfw_wheel_names* names);
	/*
	* Reads the wheel name for the specified ID, pass in a char array of at least 9 characters. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL read_wheel_name_hsfw(hsfw_wheel* wheel, char wheel_id, char* name);

	/*
	* Writes a wheel name for the given ID, names are 8 characters long. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL write_wheel_name_hsfw(hsfw_wheel* wheel, char wheel_id, char* name);

	/*
	* Reads all filter names for a given wheel, pass in an existing names struct. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL read_filter_names_hsfw(hsfw_wheel* wheel, char wheel_id, hsfw_wheel_filters* filters);
	/*
	* Reads the filter name for the specified ID and position, pass in a char array of at least 9 characters. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL read_filter_name_hsfw(hsfw_wheel* wheel, char wheel_id, unsigned short position, char* name);

	/*
	* Writes a filter name for the given ID and position, names are 8 characters long. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL write_filter_name_hsfw(hsfw_wheel* wheel, char wheel_id, unsigned short position, char* name);

	/*
	* Restores all names to default. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL restore_default_names_hsfw(hsfw_wheel* wheel);

	/*
	* Sets a centering offset for a HSFW. Valid values are -127 to 127. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL write_centering_offset_hsfw(hsfw_wheel* wheel, short centering_offset);

	/*
	* Clear the currently set error. A return of 0 indicates success, any other value indicates an error.
	*/
	int HSFW_EXPORT HSFW_CALL clear_error_hsfw(hsfw_wheel* wheel);

	/*
	* Get the corresponding text for an HSFW error code. 
	*/
	const char HSFW_EXPORT HSFW_CALL* get_error_text_hsfw(int error_code);

#ifdef __cplusplus
}
#endif

#endif