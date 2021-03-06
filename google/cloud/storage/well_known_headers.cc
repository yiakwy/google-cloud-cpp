// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "google/cloud/storage/well_known_headers.h"
#include "google/cloud/storage/internal/openssl_util.h"
#include <openssl/sha.h>
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace google {
namespace cloud {
namespace storage {
inline namespace STORAGE_CLIENT_NS {
namespace {
std::string Sha256AsBase64(std::string const& key) {
  std::string hash(SHA256_DIGEST_LENGTH, ' ');

  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, key.c_str(), key.size());
  SHA256_Final(reinterpret_cast<unsigned char*>(&hash[0]), &sha256);

  return internal::OpenSslUtils::Base64Encode(hash);
}
}  // namespace

std::ostream& operator<<(std::ostream& os, CustomHeader const& rhs) {
  if (!rhs.has_value()) {
    return os;
  }
  return os << rhs.custom_header_name() << ": " << rhs.value();
}

EncryptionKeyData EncryptionDataFromBinaryKey(std::string const& key) {
  return EncryptionKeyData{"AES256", internal::OpenSslUtils::Base64Encode(key),
                           Sha256AsBase64(key)};
}

EncryptionKeyData EncryptionDataFromBase64Key(std::string const& key) {
  std::string binary_key = internal::OpenSslUtils::Base64Decode(key);
  return EncryptionKeyData{"AES256", key, Sha256AsBase64(binary_key)};
}

EncryptionKey EncryptionKey::FromBinaryKey(std::string const& key) {
  return EncryptionKey(EncryptionDataFromBinaryKey(key));
}

EncryptionKey EncryptionKey::FromBase64Key(std::string const& key) {
  return EncryptionKey(EncryptionDataFromBase64Key(key));
}

std::ostream& operator<<(std::ostream& os, EncryptionKey const& rhs) {
  char const* prefix = EncryptionKey::prefix();
  if (rhs.has_value()) {
    return os << prefix << "algorithm: " << rhs.value().algorithm << "\n"
              << prefix << "key: " << rhs.value().key << "\n"
              << prefix << "key-sha256: " << rhs.value().sha256;
  }
  return os << prefix << "*: <not set>";
}

SourceEncryptionKey SourceEncryptionKey::FromBinaryKey(std::string const& key) {
  return SourceEncryptionKey(EncryptionDataFromBinaryKey(key));
}

SourceEncryptionKey SourceEncryptionKey::FromBase64Key(std::string const& key) {
  return SourceEncryptionKey(EncryptionDataFromBase64Key(key));
}

std::ostream& operator<<(std::ostream& os, SourceEncryptionKey const& rhs) {
  char const* prefix = SourceEncryptionKey::prefix();
  if (rhs.has_value()) {
    return os << prefix << "algorithm: " << rhs.value().algorithm << "\n"
              << prefix << "key: " << rhs.value().key << "\n"
              << prefix << "key-sha256: " << rhs.value().sha256;
  }
  return os << prefix << "*: <not set>";
}

}  // namespace STORAGE_CLIENT_NS
}  // namespace storage
}  // namespace cloud
}  // namespace google
