// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from my_custom_message:msg/BrainData.idl
// generated code does not contain a copyright notice

#ifndef MY_CUSTOM_MESSAGE__MSG__DETAIL__BRAIN_DATA__STRUCT_H_
#define MY_CUSTOM_MESSAGE__MSG__DETAIL__BRAIN_DATA__STRUCT_H_

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

/// Struct defined in msg/BrainData in the package my_custom_message.
typedef struct my_custom_message__msg__BrainData
{
  rosidl_runtime_c__String timestamp;
  int32_t poorsignal;
  int32_t delta;
  int32_t theta;
  int32_t alpha_l;
  int32_t alpha_h;
  int32_t beta_l;
  int32_t beta_h;
  int32_t gamma_l;
  int32_t gamma_m;
} my_custom_message__msg__BrainData;

// Struct for a sequence of my_custom_message__msg__BrainData.
typedef struct my_custom_message__msg__BrainData__Sequence
{
  my_custom_message__msg__BrainData * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} my_custom_message__msg__BrainData__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MY_CUSTOM_MESSAGE__MSG__DETAIL__BRAIN_DATA__STRUCT_H_
