#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static void usage(const char *program_name)
{
  fprintf(stderr, "Usage: %s DIRECTORY\n", program_name);
}

static char *shift(int *argc, char ***argv)
{
  if(*argc == 0)
    return NULL;

  char *result = **argv;
  *argc -= 1;
  *argv += 1;
  return result;
}

struct directory
{
  char *path;
  DIR *dir;
};

static int directory_open(struct directory *directory, char *path)
{
  directory->path = path;
  if(!(directory->dir = opendir(directory->path)))
  {
    fprintf(stderr, "Error: Failed to open directory %s:%s\n", path, strerror(errno));
    return -1;
  }
  return 0;
}

struct paths
{
  char **items;
  size_t count;
  size_t capacity;
  size_t push_index;
  size_t pop_index;
};

static void paths_push(struct paths *paths, char *path)
{
  if(paths->count == paths->capacity)
  {
    assert(paths->push_index == paths->pop_index);

    paths->capacity = paths->capacity != 0 ? paths->capacity * 2 : 1;
    paths->items = realloc(paths->items, paths->capacity * sizeof *paths->items);

    memcpy(paths->items + paths->count, paths->items, paths->push_index * sizeof *paths->items);
    paths->push_index += paths->count;
  }

  paths->items[paths->push_index] = path;
  paths->push_index = (paths->push_index + 1) % paths->capacity;
  ++paths->count;
}

static char *paths_pop(struct paths *paths)
{
  if(paths->count == 0)
    return NULL;

  char *path = paths->items[paths->pop_index];
  paths->pop_index = (paths->pop_index + 1) % paths->capacity;
  --paths->count;
  return path;
}

int main(int argc, char *argv[])
{
  const char *program_name = shift(&argc, &argv);
  if(!program_name)
  {
    usage("bfind");
    return EXIT_FAILURE;
  }

  const char *directory = shift(&argc, &argv);
  if(!directory)
  {
    usage(program_name);
    return EXIT_FAILURE;
  }

  struct paths paths = {0};
  paths_push(&paths, strdup(directory));

  char *path;
  while((path = paths_pop(&paths)))
  {
    printf("%s\n", path);

    DIR *dirp = opendir(path);
    if(!dirp)
    {
      fprintf(stderr, "Warning: Failed to open directory %s:%s\n", path, strerror(errno));
      free(path);
      continue;
    }

    struct dirent *dirent;
    while((dirent = readdir(dirp)))
    {
      if(strcmp(dirent->d_name, ".") == 0) continue;
      if(strcmp(dirent->d_name, "..") == 0) continue;

      // NOTE: Option for find: -name ".*" -prune
      // FIXME: Maybe un-hardcode this.
      if(dirent->d_name[0] == '.') continue;

      // NOTE: Option for find: -type d
      // FIXME: Maybe un-hardcode this.
      if(dirent->d_type != DT_DIR) continue;

      char *child_path;
      if(asprintf(&child_path, "%s/%s", path, dirent->d_name) == -1)
        continue;

      paths_push(&paths, child_path);
    }

    closedir(dirp);
    free(path);
  }
}
