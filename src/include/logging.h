#ifndef LOGGING_H
#define LOGGING_H

void set_logging_enabled(bool b);
void alert(const char *msg, ...);
void error(const char *msg, ...);
void die(const char *msg, ...);

#endif
