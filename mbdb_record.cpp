#include "mbdb_record.h"

#include "mbdb_read.h"

#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>

template<typename T>
static void hash2str(const T hash, std::string& output)
{
  static const char* nibble2str = "0123456789abcdef";

  for (typename T::value_type tmp : hash) {
    output.push_back(nibble2str[tmp >> 4]);
    output.push_back(nibble2str[tmp & 0xf]);
  }
}

static std::string generate_storage_hash(const std::string& domain, const std::string& path)
{
  std::string   hash_input;
  unsigned char hash[SHA_DIGEST_LENGTH];
  std::string   hash_str;

  hash_input = domain + "-" + path;
  SHA1((unsigned char*) hash_input.c_str(), hash_input.size(), hash);
  hash2str(std::vector<uint8_t>(hash, hash + SHA_DIGEST_LENGTH), hash_str);

  return hash_str;
}

mbdb_record::mbdb_record(const char*& addr)
{
  std::vector<uint8_t>  tmp_hash;
  uint8_t               prop_count;
  std::string           unused_str;
  uint32_t              unused_u32;

  read<std::string>(addr, this->domain);
  read<std::string>(addr, this->path);
  read<std::string>(addr, this->link_target);

  read<std::vector<uint8_t>>(addr, tmp_hash);
  hash2str(tmp_hash, this->data_hash);

  read<std::string>(addr, unused_str);

  read<uint16_t>(addr, this->mode);
  read<uint32_t>(addr, unused_u32);
  read<uint32_t>(addr, unused_u32);
  read<uint32_t>(addr, this->uid);
  read<uint32_t>(addr, this->gid);
  read<uint32_t>(addr, this->mtime);
  read<uint32_t>(addr, this->atime);
  read<uint32_t>(addr, this->ctime);
  read<uint64_t>(addr, this->size);
  read<uint8_t>(addr, this->flag);

  read<uint8_t>(addr, prop_count);

  for (int i = 0; i < prop_count; ++i) {
    std::string name;
    std::string value;

    read<std::string>(addr, name);
    read<std::string>(addr, value);

    this->properties[name] = value;
  }

  this->storage_hash = generate_storage_hash(this->domain, this->path);
}

void mbdb_record::dump(std::ostream& out) const
{
  out << this->storage_hash << " " << this->path << " (" << this->domain << ")" << std::endl;
}

void mbdb_record::extract(const char* mbdb_dir) const
{
  if (this->mode & S_IFREG) {
    std::string out_path;
    std::string in_path;
    int         in_fd;
    int         out_fd;
    ssize_t     size;
    char        buf[4096];

    out_path.reserve(this->domain.size() + sizeof '/' + this->path.size());
    out_path.append(this->domain);
    out_path.append("/");
    out_path.append(this->path);

    if ((out_fd = open(out_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
      warn("%s", out_path.c_str());
      goto cleanup_out;
    }

    if (this->size == 0)
      goto cleanup_out;

    in_path.reserve(strlen(mbdb_dir) + sizeof '/' + 2 * SHA_DIGEST_LENGTH);
    in_path.append(mbdb_dir);
    in_path.append("/");
    in_path.append(this->storage_hash);

    if ((in_fd = open(in_path.c_str(), O_RDONLY)) == -1) {
      warn("trying to extract %s: %s", this->path.c_str(), in_path.c_str());
      goto cleanup_in;
    }

    while ((size = read(in_fd, buf, sizeof buf)) > 0)
      write(out_fd, buf, size);

cleanup_in:
    close(in_fd);
cleanup_out:
    close(out_fd);
  } else if (this->mode & S_IFDIR) {
    if (this->path.size() == 0) {
      /*
       * If the path is empty, this entry was just a place holder for the
       * initial domain dir.
       */
      if (mkdir(this->domain.c_str(), 0777) == -1 && errno != EEXIST)
        err(1, "%s", this->domain.c_str());
    } else {
      std::string full_path;


      full_path.reserve(this->domain.size() + sizeof '/' + this->path.size());
      full_path.append(this->domain);
      full_path.append("/");
      full_path.append(this->path);

      if (mkdir(full_path.c_str(), 0777) == -1 && errno != EEXIST)
        err(1, "%s", full_path.c_str());
    }
  } else {
    warnx("%s: unknown file type, skipping", this->path.c_str());
    return;
  }
}
