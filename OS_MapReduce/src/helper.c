#include "helper.h"

/* 
  Sets mapper_path to the corresponding mapper file path in the input_dir path.
*/
void getMapperPath(char *mapper_path, char *input_dir, int mapper)
{
  char filename[MAP_STR_LEN];
  filename[0] = '\0';
  mapper_path[0] = '\0';
  strcat(mapper_path, input_dir);
  strcat(mapper_path, "/");
  sprintf(filename, "Mapper_%d.txt", mapper);
  strcat(mapper_path, filename);
}

/*
  Restarts wait if interrupted by a signal.
*/
pid_t r_wait(int *stat_loc){
	int retval;
	while( ((retval = wait(stat_loc)) == -1) &&  (errno == EINTR) );
	return retval;
}
