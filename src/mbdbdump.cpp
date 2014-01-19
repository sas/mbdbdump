#include "mbdb_record.h"

#include "syscall_err.h"

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <list>
#include <iostream>

#define MBDB_SIG        "mbdb\5\0"
#define MBDB_SIG_LEN    ((sizeof MBDB_SIG) - 1)

static void map(const char* path, const char** begin_addr, const char** end_addr)
{
  int     fd;
  struct  stat buf;
  void*   addr;

  fd = SYSCALL_ERR(open, path, O_RDONLY);
  SYSCALL_ERR(stat, path, &buf);

  if ((addr = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    err(EXIT_FAILURE, "mmap(%s)", path);

  close(fd);

  *begin_addr = (const char*) addr;
  *end_addr = (const char*) addr + buf.st_size;
}

static void unmap(const char* begin_addr, const char* end_addr)
{
  munmap((void*) begin_addr, end_addr - begin_addr);
}

void usage_die(char* argv0)
{
  const char* base = basename(argv0);

  std::cerr
    << "usage: " << base << " MBDB_FILE [ list | extract | cat PATH ]" << std::endl
    << "       list:    list contents of the backup" << std::endl
    << "       extract: extract contents of the backup to the current directory" << std::endl
    << "       cat:     dump contents of a file to standard output" << std::endl;

  exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
  const char*             begin_addr;
  const char*             end_addr;
  const char*             addr;
  std::list<mbdb_record>  contents;

  if (argc < 2)
    usage_die(argv[0]);

  map(argv[1], &begin_addr, &end_addr);

  addr = begin_addr;

  if (memcmp(addr, MBDB_SIG, MBDB_SIG_LEN) != 0)
    errx(EXIT_FAILURE, "%s: is not a valid MBDB file", argv[1]);

  addr += MBDB_SIG_LEN;

  while (addr < end_addr)
    contents.push_back(mbdb_record(addr));

  if (argc == 2 || strcmp(argv[2], "list") == 0) {
    for (const auto& e : contents)
      std::cout << e.get_path() << std::endl;
  } else if (strcmp(argv[2], "extract") == 0) {
    const char* dir = dirname(argv[1]);

    for (const auto& e : contents)
      e.extract(dir);
  } else if (strcmp(argv[2], "cat") == 0) {
    const char* dir = dirname(argv[1]);
    bool        did_it = false;

    if (argc != 4)
      usage_die(argv[0]);

    for (const auto& e : contents) {
      if (e.get_path() == argv[3]) {
        e.cat(dir, "/dev/stdout");
        did_it = true;
        break;
      }
    }

    if (!did_it) {
      errno = ENOENT;
      warn("%s", argv[3]);
    }
  } else {
    usage_die(argv[0]);
  }

  unmap(begin_addr, end_addr);

  return EXIT_SUCCESS;
}
