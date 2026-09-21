#pragma once
// Host fixture only: the Android boundary is tested against real desktop SDL.
#define ANDROID_LOG_ERROR 6
#define ANDROID_LOG_WARN 5
inline int __android_log_print(int, const char *, const char *, ...) { return 0; }
