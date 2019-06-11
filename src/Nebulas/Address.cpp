// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"
#include "../Hash.h"
#include "../Base58.h"
#include "../HexCoding.h"

using namespace TW::Nebulas;

bool Address::isValid(const std::string& string) {
    auto data = Base58::bitcoin.decode(string);
    if (data.size() != Address::size) {
        return false;
    }

    if(data[0] != Address::AddressPrefix) {
        return false;
    }
    if (data[1] != Address::NormalType && data[1] != Address::ContractType) {
        return false;
    }

    Data content(data.begin(), data.begin()+22);
    Data checksum(data.begin()+22, data.end());
    auto dataSha3 = Hash::sha3_256(content);
    return ::memcmp(dataSha3.data(),checksum.data(),4)==0;
}

Address::Address(const std::string& string) {
    if (!isValid(string)) {
        throw std::invalid_argument("Invalid address data");
    }
    
    auto data = Base58::bitcoin.decode(string);
    std::copy(data.begin(), data.end(), bytes.begin());
}

Address::Address(const Data& data) {
    if (!isValid(data)) {
        throw std::invalid_argument("Invalid address data");
    }
    std::copy(data.begin(), data.end(), bytes.begin());
}

Address::Address(const PublicKey& publicKey) {
    if (publicKey.type != TWPublicKeyTypeSECP256k1Extended) {
        throw std::invalid_argument("Nebulas::Address needs an extended SECP256k1 public key.");
    }
    const auto data = publicKey.hash({Address::AddressPrefix,Address::NormalType},
        static_cast<Data(*)(const byte*, const byte*)>(Hash::sha3_256ripemd), false);
    if (data.size() != 22){
        throw std::invalid_argument("Nebulas::Address the size of data should be 22.");
    }
    std::copy(data.begin(), data.end(), bytes.begin());
    auto checksum = Hash::sha3_256(data);
    std::copy(checksum.begin(),checksum.begin()+4,bytes.begin()+22);
}

std::string Address::string() const {
    return Base58::bitcoin.encode(bytes);
}