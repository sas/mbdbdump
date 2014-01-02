#include "mbdb_record.h"

#include "mbdb_read.h"

#include <arpa/inet.h>
#include <openssl/sha.h>

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
}

void mbdb_record::dump(std::ostream& out) const
{
  std::string   hash_input;
  unsigned char hash[SHA_DIGEST_LENGTH];
  std::string   hash_str;

  hash_input = this->domain + "-" + this->path;
  SHA1((unsigned char*) hash_input.c_str(), hash_input.size(), hash);
  hash2str(std::vector<uint8_t>(hash, hash + SHA_DIGEST_LENGTH), hash_str);

  out << hash_str << " " << this->path << std::endl;
}
