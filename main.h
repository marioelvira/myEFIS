#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MAIN_H_
#define _MAIN_H_

///////////
// Debug //
///////////
#define _DEBUG_           0

#if (_DEBUG_ == 1)
#define _PINOUT_DEBUG_    1
#define _SERIAL_DEBUG_    1
#else
#define _PINOUT_DEBUG_    0
#define _SERIAL_DEBUG_    1
#endif

#endif // _MAIN_H_

#ifdef __cplusplus
} // extern "C"
#endif
