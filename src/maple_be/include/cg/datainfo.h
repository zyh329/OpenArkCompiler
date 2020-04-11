/*
 * Copyright (c) [2020] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * OpenArkCompiler is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *     http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */
#ifndef MAPLEBE_INCLUDE_CG_DATAINFO_H
#define MAPLEBE_INCLUDE_CG_DATAINFO_H
#include <vector>

namespace maplebe {
class DataInfo {
 public:
  explicit DataInfo(uint32 bitNum) : info(bitNum / kWordSize + 1, 0ULL) {}

  ~DataInfo() = default;

  void SetBit(uint32 bitNO) {
    ASSERT(bitNO < info.size() * kWordSize, "Out of Range");
    info[bitNO / kWordSize] |= (1ULL << (bitNO % kWordSize));
  }

  void ResetBit(uint32 bitNO) {
    info[bitNO / kWordSize] &= (~(1ULL << (bitNO % kWordSize)));
  }

  bool TestBit(uint32 bitNO) const {
    return (info[bitNO / kWordSize] & (1ULL << (bitNO % kWordSize))) != 0ULL;
  }

  const uint64 &GetElem(uint32 index) const {
    ASSERT(index < info.size(), "out of range");
    return info[index];
  }

  void SetElem(uint32 index, uint64 val) {
    ASSERT(index < info.size(), "out of range");
    info[index] = val;
  }

  bool NoneBit() const {
    for (auto &data : info) {
      if (data != 0ULL) {
        return false;
      }
    }
    return true;
  }

  size_t Size() const {
    return info.size() * kWordSize;
  }

  const std::vector<uint64> &GetInfo() const {
    return info;
  }

  bool IsEqual(const DataInfo &secondInfo) const {
    auto infoSize = static_cast<const int32>(info.size());
    ASSERT(infoSize == secondInfo.GetInfo().size(), "two dataInfo's size different");
    for (int32 i = 0; i != infoSize; i++) {
      if (info[i] != secondInfo.GetElem(i)) {
        return false;
      }
    }
    return true;
  }

  void AndBits(const DataInfo &secondInfo) {
    auto infoSize = static_cast<const int32>(info.size());
    ASSERT(infoSize == secondInfo.GetInfo().size(), "two dataInfo's size different");
    for (int32 i = 0; i != infoSize; i++) {
      info[i] &= secondInfo.GetElem(i);
    }
  }

  void OrBits(const DataInfo &secondInfo) {
    auto infoSize = static_cast<const int32>(info.size());
    ASSERT(infoSize == secondInfo.GetInfo().size(), "two dataInfo's size different");
    for (int32 i = 0; i != infoSize; i++) {
      info[i] |= secondInfo.GetElem(i);
    }
  }

  void OrDesignateBits(const DataInfo &secondInfo, uint32 infoIndex) {
    ASSERT(infoIndex < secondInfo.GetInfo().size(), "out of secondInfo's range");
    ASSERT(infoIndex < info.size(), "out of secondInfo's range");
    info[infoIndex] |= secondInfo.GetElem(infoIndex);
  }

  void EorBits(const DataInfo &secondInfo) {
    auto infoSize = static_cast<const int32>(info.size());
    ASSERT(infoSize == secondInfo.GetInfo().size(), "two dataInfo's size different");
    for (int32 i = 0; i != infoSize; i++) {
      info[i] ^= secondInfo.GetElem(i);
    }
  }

  /* if bit in secondElem is 1, bit in current DataInfo is set 0 */
  void Difference(const DataInfo &secondInfo) {
    auto infoSize = static_cast<const int32>(info.size());
    ASSERT(infoSize == secondInfo.GetInfo().size(), "two dataInfo's size different");
    for (int32 i = 0; i != infoSize; i++) {
      info[i] &= (~(secondInfo.GetElem(i)));
    }
  }

  void ResetAllBit() {
    for (auto &data : info) {
      data = 0ULL;
    }
  }

  void EnlargeCapacityToAdaptSize(uint32 bitNO) {
    /* add one more size for each enlarge action */
    auto sizeToEnlarge = static_cast<int32>((bitNO / kWordSize + 1) - info.size());
    for (int32 i = 0; i < sizeToEnlarge; i++) {
      info.push_back(0ULL);
    }
  }

  void GetNonZeroElemsIndex(std::set<uint32> &index) {
    auto infoSize = static_cast<const int32>(info.size());
    for (int32 i = 0; i < infoSize; i++) {
      if (info[i] != 0ULL) {
        index.insert(i);
      }
    }
  }

  int32 GetInfoIndex(const uint32 bitNO) const {
    return bitNO / kWordSize;
  }

  std::set<uint32> GetBitsOfInfo() const {
    std::set<uint32> wordRes;
    wordRes.clear();
    for (size_t i = 0; i != info.size(); ++i) {
      uint32 result = 0;
      uint64 word = info[i];
      uint32 offset = 0;
      uint32 baseWord = 0;
      bool firstTime = true;
      while (word) {
        int32 index = __builtin_ffsll(word);
        if (index == 0) {
          continue;
        }
        if (index == k64BitSize) {
          /* when the highest bit is 1, the shift operation will cause error, need special treatment. */
          result = i * kWordSize + (index - 1);
          wordRes.insert(result);
          break;
        }
        if (firstTime) {
          offset = index - 1;
          baseWord = i * kWordSize;
          firstTime = false;
        } else {
          offset = index;
          baseWord = 0;
        }
        result += baseWord + offset;
        wordRes.insert(result);
        word = word >> static_cast<uint64>(index);
      }
    }
    return wordRes;
  }

  void ClearDataInfo() {
    info.clear();
    info.shrink_to_fit();
  }

 private:
  /* long type has 8 bytes, 64 bits */
  static constexpr int32 kWordSize = 64;
  std::vector<uint64> info;
};
}  /* namespace maplebe */
#endif  /* MAPLEBE_INCLUDE_CG_INSN_H */