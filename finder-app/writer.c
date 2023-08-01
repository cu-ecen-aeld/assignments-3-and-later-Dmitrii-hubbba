#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

int main(int argc,  char *argv[]) {

openlog(NULL, LOG_CONS, LOG_USER);

if (argc != 3) {
  printf("Not enough arguments.\n");
  syslog(LOG_ERR, "%s","Too few arguments"); 
  return 1;
}

FILE *fd = fopen(argv[1], "w");

if (fd == NULL) {
  printf("File doesn't exist\n");
  return -1;
}

//char text[] = argv[3];

fprintf(fd, "%s\n", argv[2]);
syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);

fclose(fd);

return 0;

}
