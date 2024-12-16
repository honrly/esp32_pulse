// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from my_custom_message:msg/PulseData.idl
// generated code does not contain a copyright notice

#ifndef MY_CUSTOM_MESSAGE__MSG__DETAIL__PULSE_DATA__STRUCT_H_
#define MY_CUSTOM_MESSAGE__MSG__DETAIL__PULSE_DATA__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'timestamp'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/PulseData in the package my_custom_message.
typedef struct my_custom_message__msg__PulseData
{
  rosidl_runtime_c__String timestamp;
  int32_t bpm;
  int32_t ibi;
  float sdnn;
  float cvnn;
  float rmssd;
  float pnn10;
  float pnn20;
  float pnn30;
  float pnn40;
  float pnn50;
} my_custom_message__msg__PulseData;

// Struct for a sequence of my_custom_message__msg__PulseData.
typedef struct my_custom_message__msg__PulseData__Sequence
{
  my_custom_message__msg__PulseData * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} my_custom_message__msg__PulseData__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MY_CUSTOM_MESSAGE__MSG__DETAIL__PULSE_DATA__STRUCT_H_
