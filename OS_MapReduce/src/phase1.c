#include "phase1.h"

/*
  Creates directory MapperInput and calls traverse to create the mapperInput
  textfiles. Returns count of files if successful, else it returns -1
*/
int makeMapperInput(char *path, char *input_dir, int m)
{
  if( mkdir("./MapperInput", 0700) )
  {
    printf("Failed to create MapperInput directory\n");
    return -1;
  }
  int file_count = 0;
  traverse(path, input_dir, m, 1, &file_count);
  return file_count;
}

/*
  Traverses through the given path, and it distributes the encountered files among the 
  m mapper files in a round robin fashion. The mapper files are created in the input_dir 
  path. If succesful it return the total number of encountered files, else it returns -1.
*/
int traverse(char *path, char *input_dir, int m, int mapper, int *total)
{

  DIR *dirp;
  struct dirent *direntp;
  char new_path[MAX_PATH];
  char mapper_path[MAX_PATH];
  int current_mapper; //keeps track of which mapper_i to put file into
  int count = 0;

  if ((dirp = opendir(path)) == NULL)
  {
    printf("Failed to open directory %s\n", path);
    return -1;
  }

  while ((direntp = readdir(dirp)) != NULL)
  {
    new_path[0] = '\0';
    strcat(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, direntp -> d_name);
    if (direntp -> d_type == DT_DIR)
    { //directory code
      if (strcmp(".", direntp -> d_name) && strcmp("..", direntp -> d_name))
      {
        mapper = traverse(new_path, input_dir, m, mapper, total);
      }
    }
    else
    {
      getMapperPath(mapper_path, input_dir, mapper);
      FILE *fp = fopen(mapper_path, "a");
      if(fp == NULL){
        printf("Failed to open file in traverse.\n");
        return -1;
      }
      fprintf(fp, "%s\n", new_path);
      fclose(fp);
      mapper = nextMapper(mapper, m);
      count++;
    }
  }
  *total += count;
  return mapper;
}

/*
  Returns the number that follows mapper in the range [1,m]. Helper funciton for
  traverse.
*/
int nextMapper(int mapper, int m)
{
  if (mapper == m)
    return 1;
  return mapper + 1;
}