#pragma once

#ifndef MCGA_DISALLOW_COPY_AND_MOVE
#define MCGA_DISALLOW_COPY_AND_MOVE(ClassName)                                 \
  ClassName(const ClassName&) = delete;                                        \
  ClassName(ClassName&&) = delete;                                             \
  ClassName& operator=(const ClassName&) = delete;                             \
  ClassName& operator=(ClassName&&) = delete
#endif
