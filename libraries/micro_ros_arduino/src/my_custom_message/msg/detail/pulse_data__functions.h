// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from my_custom_message:msg/PulseData.idl
// generated code does not contain a copyright notice

#ifndef MY_CUSTOM_MESSAGE__MSG__DETAIL__PULSE_DATA__FUNCTIONS_H_
#define MY_CUSTOM_MESSAGE__MSG__DETAIL__PULSE_DATA__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "my_custom_message/msg/rosidl_generator_c__visibility_control.h"

#include "my_custom_message/msg/detail/pulse_data__struct.h"

/// Initialize msg/PulseData message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * my_custom_message__msg__PulseData
 * )) before or use
 * my_custom_message__msg__PulseData__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
bool
my_custom_message__msg__PulseData__init(my_custom_message__msg__PulseData * msg);

/// Finalize msg/PulseData message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
void
my_custom_message__msg__PulseData__fini(my_custom_message__msg__PulseData * msg);

/// Create msg/PulseData message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * my_custom_message__msg__PulseData__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
my_custom_message__msg__PulseData *
my_custom_message__msg__PulseData__create();

/// Destroy msg/PulseData message.
/**
 * It calls
 * my_custom_message__msg__PulseData__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
void
my_custom_message__msg__PulseData__destroy(my_custom_message__msg__PulseData * msg);

/// Check for msg/PulseData message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
bool
my_custom_message__msg__PulseData__are_equal(const my_custom_message__msg__PulseData * lhs, const my_custom_message__msg__PulseData * rhs);

/// Copy a msg/PulseData message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
bool
my_custom_message__msg__PulseData__copy(
  const my_custom_message__msg__PulseData * input,
  my_custom_message__msg__PulseData * output);

/// Initialize array of msg/PulseData messages.
/**
 * It allocates the memory for the number of elements and calls
 * my_custom_message__msg__PulseData__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
bool
my_custom_message__msg__PulseData__Sequence__init(my_custom_message__msg__PulseData__Sequence * array, size_t size);

/// Finalize array of msg/PulseData messages.
/**
 * It calls
 * my_custom_message__msg__PulseData__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
void
my_custom_message__msg__PulseData__Sequence__fini(my_custom_message__msg__PulseData__Sequence * array);

/// Create array of msg/PulseData messages.
/**
 * It allocates the memory for the array and calls
 * my_custom_message__msg__PulseData__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
my_custom_message__msg__PulseData__Sequence *
my_custom_message__msg__PulseData__Sequence__create(size_t size);

/// Destroy array of msg/PulseData messages.
/**
 * It calls
 * my_custom_message__msg__PulseData__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
void
my_custom_message__msg__PulseData__Sequence__destroy(my_custom_message__msg__PulseData__Sequence * array);

/// Check for msg/PulseData message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
bool
my_custom_message__msg__PulseData__Sequence__are_equal(const my_custom_message__msg__PulseData__Sequence * lhs, const my_custom_message__msg__PulseData__Sequence * rhs);

/// Copy an array of msg/PulseData messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_my_custom_message
bool
my_custom_message__msg__PulseData__Sequence__copy(
  const my_custom_message__msg__PulseData__Sequence * input,
  my_custom_message__msg__PulseData__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // MY_CUSTOM_MESSAGE__MSG__DETAIL__PULSE_DATA__FUNCTIONS_H_
