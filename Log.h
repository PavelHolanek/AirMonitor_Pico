#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#define LOGS_NUMBER

char* logsToSave[LOGS_NUMBER];
void LOG(const char* message);

#ifdef __cplusplus
}
#endif

#endif // LOG_H
