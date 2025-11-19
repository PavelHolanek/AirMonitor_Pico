#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

// Logs a message to serial with a time prefix using printf().
// Format: [MM-DD HH:MM:SS] message\n
void LOG(const char* message);

#ifdef __cplusplus
}
#endif

#endif // LOG_H
