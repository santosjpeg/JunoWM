#include <stdio.h>
#include <time.h>

void debug_message(const char *msg) {
  // Get current time
  time_t rawtime;
  struct tm *timeinfo;
  char time_str[20]; // Buffer to hold the formatted time string

  time(&rawtime);                 // Get the current time
  timeinfo = localtime(&rawtime); // Convert to local time

  // Format time as "YYYY-MM-DD HH:MM:SS"
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

  // Print the timestamp with the debug message
  printf("[%s] DEBUG: %s\n", time_str, msg);
}
