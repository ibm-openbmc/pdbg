#pragma once

#include <cstdio>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <span>
#include <type_traits>
#include <arpa/inet.h>
#include <targeting/target.H>
#include <transport/transport.H>
#include "sbe_cmd_response.H"
#include "sbe_cmd_protocol.H"

namespace sbei
{
using ConstTargetPtr_t = TARGETING::ConstTargetPtr;    

template <typename T>
inline std::span<const std::byte> as_byte_span(const std::vector<T>& v)
{
    static_assert(std::is_trivially_copyable_v<T>,
                  "as_byte_span requires trivially copyable element type");

    return {reinterpret_cast<const std::byte*>(v.data()), v.size() * sizeof(T)};
}

inline std::vector<uint8_t> to_u8(const std::vector<std::byte>& v) 
{
    const auto* p = reinterpret_cast<const uint8_t*>(v.data());
    return std::vector<uint8_t>(p, p + v.size());
}

inline std::vector<uint8_t> toByteVector(const std::vector<uint32_t>& words)
{
    std::vector<uint8_t> bytes(words.size() * sizeof(uint32_t));
    std::memcpy(bytes.data(), words.data(), bytes.size());
    return bytes;
}

inline ResponseInfo decodeResponse(const std::vector<uint8_t>& rawBytes)
{
    ResponseInfo out;
    const size_t wordSize = sizeof(uint32_t);
    const size_t wordCount = rawBytes.size() / wordSize;
    const uint32_t* words = reinterpret_cast<const uint32_t*>(rawBytes.data());

    if (wordCount < 3)
    {
        return out;
    }

    uint32_t trailerOffset = ntohl(words[wordCount - 1]);
    size_t headerPos = wordCount - trailerOffset;

    if (headerPos + 1 >= wordCount)
    {
        return out;
    }

    out.primaryStatus = ntohl(words[headerPos + 1]) >> 16;
    out.secondaryStatus = ntohl(words[headerPos + 1]) & 0xFFFF;

    size_t payloadSize = headerPos * wordSize;
    out.payload.assign(rawBytes.begin(), rawBytes.begin() + payloadSize);

    size_t ffdcStart = (headerPos + 2) * wordSize;
    if (ffdcStart + 8 <= rawBytes.size())
    {
        size_t offset = ffdcStart;
        while (offset + 8 <= rawBytes.size())
        {
            const uint8_t* p = &rawBytes[offset];
            uint16_t magic = ntohs(*(uint16_t*)p);

            if (magic != 0xFBAD && magic != 0xFFDC)
                break;

            ResponseInfo::FFDC entry;
            entry.slid = ntohs(*(uint16_t*)(p + 4));
            entry.severity = *(p + 6);
            entry.chipId = *(p + 7);

            uint16_t wordLen = ntohs(*(uint16_t*)(p + 2));
            size_t byteLen = wordLen * wordSize;

            if (offset + byteLen > rawBytes.size())
                break;

            entry.rawData.assign(p, p + byteLen);
            out.ffdcList.push_back(std::move(entry));
            offset += byteLen;
        }
    }

    return out;
}

int executeChipOp(ConstTargetPtr_t target,
                  const std::vector<uint32_t>& cmd_words,
                  std::vector<uint8_t>& data,
                  std::vector<ResponseInfo::FFDC>& ffdc)
{
    std::vector<uint8_t> request = toByteVector(cmd_words);

    uint32_t cmd = ntohl(cmd_words[1]);
    uint32_t timeoutMs = cmd::getRecommendedTimeoutMs(cmd);
    uint32_t out_len = cmd::getRecommendedSize(cmd);

    std::vector<std::byte> response;
    
    auto rc = transport::sendAndRecv(target, as_byte_span(request), timeoutMs, response);
    if (rc != 0) 
    {
        return rc;
    }
    
    auto decoded = decodeResponse(to_u8(response));
    data = std::move(decoded.payload);
    ffdc = std::move(decoded.ffdcList);
    fprintf(stderr, "SBE response status: primary=0x%08X secondary=0x%08X, payload=%zu bytes, FFDC=%zu\n",
            decoded.primaryStatus, decoded.secondaryStatus,
            decoded.payload.size(), decoded.ffdcList.size());

    return (decoded.primaryStatus != 0 || decoded.secondaryStatus != 0) ? -1 : 0;
}

int captureFfdc(ConstTargetPtr_t target,
                std::vector<uint8_t>& discard,
                std::vector<ResponseInfo::FFDC>& outFfdc)
{
    std::vector<std::byte> response;
    auto encoded = cmd::encodeGetFfdc();
    uint32_t cmd = ntohl(encoded[1]);
    uint32_t out_len = cmd::getRecommendedSize(cmd);
    uint32_t timeoutMs = cmd::getRecommendedTimeoutMs(cmd);
    
    auto rc = transport::sendAndRecv(target, as_byte_span(toByteVector(encoded)), timeoutMs, response);
    if (rc != 0)
    {
        return rc;
    }

    auto decoded = decodeResponse(to_u8(response));
    if (!decoded.ffdcList.empty()) {
        outFfdc.insert(outFfdc.end(),
                       std::make_move_iterator(decoded.ffdcList.begin()),
                       std::make_move_iterator(decoded.ffdcList.end()));
        return 0;
    }

    return -1;
}

void mergeExtraFfdc(ConstTargetPtr_t target,
                    std::vector<ResponseInfo::FFDC>& ffdc)
{
    std::vector<ResponseInfo::FFDC> extraFfdc;
    std::vector<uint8_t> discard;
    auto ffdcRc = captureFfdc(target, discard, extraFfdc);
    if (ffdcRc != 0 && !extraFfdc.empty()) 
    {
        ffdc.insert(ffdc.end(),
                    std::make_move_iterator(extraFfdc.begin()),
                    std::make_move_iterator(extraFfdc.end()));
    }
}

int getDump(ConstTargetPtr_t target,
            uint8_t type, uint8_t clock, uint8_t fa,
            std::vector<uint8_t>& data,
            std::vector<ResponseInfo::FFDC>& ffdc)
{
    auto cmd_words = cmd::encodeGetDump(type, clock, fa);
    auto rc = executeChipOp(target, cmd_words, data, ffdc);
    mergeExtraFfdc(target, ffdc);
    return rc;
}

int getScom(ConstTargetPtr_t target, uint64_t addr, uint64_t& val)
{
    std::vector<uint8_t> data;
    std::vector<ResponseInfo::FFDC> ffdc;

    auto cmd_words = cmd::encodeGetScom(addr);
    auto rc = executeChipOp(target, cmd_words, data, ffdc);
    
    if(rc == 0)
    {
        val = 0;
        for (int i = 0; i < 8; ++i)
        {
            val |= ((static_cast<uint64_t>(data[i])) << (8 * i));
        }
    }

    mergeExtraFfdc(target, ffdc);
    return rc;
}

int putScom(ConstTargetPtr_t target, uint64_t addr, uint64_t val)
{
    std::vector<uint8_t> data;
    std::vector<ResponseInfo::FFDC> ffdc;

    auto cmd_words = cmd::encodePutScom(addr, val);
    auto rc = executeChipOp(target, cmd_words, data, ffdc);
    mergeExtraFfdc(target, ffdc);
    return rc;
}
} //namespace sbei
