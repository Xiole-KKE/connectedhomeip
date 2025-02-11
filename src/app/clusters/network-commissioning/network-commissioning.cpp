/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "network-commissioning.h"

#include <cstring>
#include <type_traits>

#include <lib/support/CodeUtils.h>
#include <lib/support/SafeInt.h>
#include <lib/support/Span.h>
#include <lib/support/ThreadOperationalDataset.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/ConnectivityManager.h>
#include <platform/DeviceControlServer.h>

#include <app-common/zap-generated/cluster-objects.h>

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <platform/ThreadStackManager.h>
#endif // CHIP_DEVICE_CONFIG_ENABLE_THREAD

// Include DeviceNetworkProvisioningDelegateImpl for WiFi provisioning.
// TODO: Enable wifi network should be done by ConnectivityManager. (Or other platform neutral interfaces)
#if defined(CHIP_DEVICE_LAYER_TARGET)
#define DEVICENETWORKPROVISIONING_HEADER <platform/CHIP_DEVICE_LAYER_TARGET/DeviceNetworkProvisioningDelegateImpl.h>
#include DEVICENETWORKPROVISIONING_HEADER
#endif

// TODO: Configuration should move to build-time configuration
#ifndef CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS
#define CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS 4
#endif // CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS

using namespace chip;
using namespace chip::app;

namespace chip {
namespace app {
namespace Clusters {
namespace NetworkCommissioning {

constexpr uint8_t kMaxNetworkIDLen       = 32;
constexpr uint8_t kMaxThreadDatasetLen   = 254; // As defined in Thread spec.
constexpr uint8_t kMaxWiFiSSIDLen        = 32;
constexpr uint8_t kMaxWiFiCredentialsLen = 64;
constexpr uint8_t kMaxNetworks           = CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS;

enum class NetworkType : uint8_t
{
    kUndefined = 0,
    kWiFi      = 1,
    kThread    = 2,
    kEthernet  = 3,
};

struct ThreadNetworkInfo
{
    uint8_t mDataset[kMaxThreadDatasetLen];
    uint8_t mDatasetLen;
};

struct WiFiNetworkInfo
{
    uint8_t mSSID[kMaxWiFiSSIDLen + 1];
    uint8_t mSSIDLen;
    uint8_t mCredentials[kMaxWiFiCredentialsLen];
    uint8_t mCredentialsLen;
};

struct NetworkInfo
{
    uint8_t mNetworkID[kMaxNetworkIDLen];
    uint8_t mNetworkIDLen;
    uint8_t mEnabled;
    NetworkType mNetworkType;
    union NetworkData
    {
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
        Thread::OperationalDataset mThread;
#endif
#if defined(CHIP_DEVICE_LAYER_TARGET)
        WiFiNetworkInfo mWiFi;
#endif
    } mData;
};

namespace {
// The internal network info containing credentials. Need to find some better place to save these info.
NetworkInfo sNetworks[kMaxNetworks];
} // namespace

void OnAddOrUpdateThreadNetworkCommandCallbackInternal(app::CommandHandler * apCommandHandler,
                                                       const app::ConcreteCommandPath & commandPath, ByteSpan operationalDataset,
                                                       uint64_t breadcrumb, uint32_t timeoutMs)
{
    Commands::NetworkConfigResponse::Type response;
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    NetworkCommissioningStatus err = NetworkCommissioningStatus::kBoundsExceeded;

    for (size_t i = 0; i < kMaxNetworks; i++)
    {
        if (sNetworks[i].mNetworkType == NetworkType::kUndefined)
        {
            Thread::OperationalDataset & dataset = sNetworks[i].mData.mThread;
            CHIP_ERROR error                     = dataset.Init(operationalDataset);

            if (error != CHIP_NO_ERROR)
            {
                ChipLogDetail(Zcl, "Failed to parse Thread operational dataset: %s", ErrorStr(error));
                err = NetworkCommissioningStatus::kUnknownError;
                break;
            }

            uint8_t extendedPanId[Thread::kSizeExtendedPanId];

            static_assert(sizeof(sNetworks[i].mNetworkID) >= sizeof(extendedPanId),
                          "Network ID must be larger than Thread extended PAN ID!");
            SuccessOrExit(dataset.GetExtendedPanId(extendedPanId));
            memcpy(sNetworks[i].mNetworkID, extendedPanId, sizeof(extendedPanId));
            sNetworks[i].mNetworkIDLen = sizeof(extendedPanId);

            sNetworks[i].mNetworkType = NetworkType::kThread;
            sNetworks[i].mEnabled     = false;

            err = NetworkCommissioningStatus::kSuccess;
            break;
        }
    }

exit:
    // TODO: We should encode response command here.

    ChipLogDetail(Zcl, "AddOrUpdateThreadNetwork: %" PRIu8, to_underlying(err));
    response.networkingStatus = err;
#else
    // The target does not supports ThreadNetwork. We should not add AddOrUpdateThreadNetwork command in that case then the upper
    // layer will return "Command not found" error.
    response.networkingStatus = NetworkCommissioningStatus::kUnknownError;
#endif
    apCommandHandler->AddResponseData(commandPath, response);
}

void OnAddOrUpdateWiFiNetworkCommandCallbackInternal(app::CommandHandler * apCommandHandler,
                                                     const app::ConcreteCommandPath & commandPath, ByteSpan ssid,
                                                     ByteSpan credentials, uint64_t breadcrumb, uint32_t timeoutMs)
{
    Commands::NetworkConfigResponse::Type response;
#if defined(CHIP_DEVICE_LAYER_TARGET)
    NetworkCommissioningStatus err = NetworkCommissioningStatus::kBoundsExceeded;

    for (size_t i = 0; i < kMaxNetworks; i++)
    {
        if (sNetworks[i].mNetworkType == NetworkType::kUndefined)
        {
            VerifyOrExit(ssid.size() <= sizeof(sNetworks[i].mData.mWiFi.mSSID), err = NetworkCommissioningStatus::kOutOfRange);
            memcpy(sNetworks[i].mData.mWiFi.mSSID, ssid.data(), ssid.size());

            using WiFiSSIDLenType = decltype(sNetworks[i].mData.mWiFi.mSSIDLen);
            VerifyOrExit(CanCastTo<WiFiSSIDLenType>(ssid.size()), err = NetworkCommissioningStatus::kOutOfRange);
            sNetworks[i].mData.mWiFi.mSSIDLen = static_cast<WiFiSSIDLenType>(ssid.size());

            VerifyOrExit(credentials.size() <= sizeof(sNetworks[i].mData.mWiFi.mCredentials),
                         err = NetworkCommissioningStatus::kOutOfRange);
            memcpy(sNetworks[i].mData.mWiFi.mCredentials, credentials.data(), credentials.size());

            using WiFiCredentialsLenType = decltype(sNetworks[i].mData.mWiFi.mCredentialsLen);
            VerifyOrExit(CanCastTo<WiFiCredentialsLenType>(ssid.size()), err = NetworkCommissioningStatus::kOutOfRange);
            sNetworks[i].mData.mWiFi.mCredentialsLen = static_cast<WiFiCredentialsLenType>(credentials.size());

            VerifyOrExit(ssid.size() <= sizeof(sNetworks[i].mNetworkID), err = NetworkCommissioningStatus::kOutOfRange);
            memcpy(sNetworks[i].mNetworkID, sNetworks[i].mData.mWiFi.mSSID, ssid.size());

            using NetworkIDLenType = decltype(sNetworks[i].mNetworkIDLen);
            VerifyOrExit(CanCastTo<NetworkIDLenType>(ssid.size()), err = NetworkCommissioningStatus::kOutOfRange);
            sNetworks[i].mNetworkIDLen = static_cast<NetworkIDLenType>(ssid.size());

            sNetworks[i].mNetworkType = NetworkType::kWiFi;
            sNetworks[i].mEnabled     = false;

            err = NetworkCommissioningStatus::kSuccess;
            break;
        }
    }

    VerifyOrExit(err == NetworkCommissioningStatus::kSuccess, );

    ChipLogDetail(Zcl, "WiFi provisioning data: SSID: %.*s", static_cast<int>(ssid.size()), ssid.data());
exit:
    // TODO: We should encode response command here.

    ChipLogDetail(Zcl, "AddOrUpdateWiFiNetwork: %" PRIu8, to_underlying(err));
    response.networkingStatus = err;
#else
    // The target does not supports WiFiNetwork.
    // return "Command not found" error.
    response.networkingStatus = NetworkCommissioningStatus::kUnknownError;
#endif
    apCommandHandler->AddResponseData(commandPath, response);
}

namespace {
CHIP_ERROR DoConnectNetwork(NetworkInfo * network)
{
    switch (network->mNetworkType)
    {
    case NetworkType::kThread:
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
// TODO: On linux, we are using Reset() instead of Detach() to disable thread network, which is not expected.
// Upstream issue: https://github.com/openthread/ot-br-posix/issues/755
#if !CHIP_DEVICE_LAYER_TARGET_LINUX
        ReturnErrorOnFailure(DeviceLayer::ThreadStackMgr().SetThreadEnabled(false));
#endif
        ReturnErrorOnFailure(DeviceLayer::ThreadStackMgr().SetThreadProvision(network->mData.mThread.AsByteSpan()));
        ReturnErrorOnFailure(DeviceLayer::ThreadStackMgr().SetThreadEnabled(true));
#else
        return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
#endif
        break;
    case NetworkType::kWiFi:
#if defined(CHIP_DEVICE_LAYER_TARGET)
    {
        // TODO: Currently, DeviceNetworkProvisioningDelegateImpl assumes that ssid and credentials are null terminated strings,
        // which is not correct, this should be changed once we have better method for commissioning wifi networks.
        DeviceLayer::DeviceNetworkProvisioningDelegateImpl deviceDelegate;
        ReturnErrorOnFailure(deviceDelegate.ProvisionWiFi(reinterpret_cast<const char *>(network->mData.mWiFi.mSSID),
                                                          reinterpret_cast<const char *>(network->mData.mWiFi.mCredentials)));
        break;
    }
#else
        return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
#endif
    break;
    case NetworkType::kEthernet:
    case NetworkType::kUndefined:
    default:
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }
    network->mEnabled = true;
    return CHIP_NO_ERROR;
}
} // namespace

void OnConnectNetworkCommandCallbackInternal(app::CommandHandler * apCommandHandler, const app::ConcreteCommandPath & commandPath,
                                             ByteSpan networkID, uint64_t breadcrumb, uint32_t timeoutMs)
{
    Commands::ConnectNetworkResponse::Type response;
    size_t networkSeq;
    NetworkCommissioningStatus err = NetworkCommissioningStatus::kNetworkIDNotFound;

    for (networkSeq = 0; networkSeq < kMaxNetworks; networkSeq++)
    {
        if (sNetworks[networkSeq].mNetworkIDLen == networkID.size() &&
            sNetworks[networkSeq].mNetworkType != NetworkType::kUndefined &&
            memcmp(sNetworks[networkSeq].mNetworkID, networkID.data(), networkID.size()) == 0)
        {
            // TODO: Currently, we cannot figure out the detailed error from network provisioning on DeviceLayer, we should
            // implement this in device layer.
            VerifyOrExit(DoConnectNetwork(&sNetworks[networkSeq]) == CHIP_NO_ERROR,
                         err = NetworkCommissioningStatus::kUnknownError);
            ExitNow(err = NetworkCommissioningStatus::kSuccess);
        }
    }
    // TODO: We should encode response command here.
exit:
    if (err == NetworkCommissioningStatus::kSuccess)
    {
        DeviceLayer::DeviceControlServer::DeviceControlSvr().ConnectNetworkForOperational(networkID);
    }
    response.networkingStatus = err;
    apCommandHandler->AddResponseData(commandPath, response);
}

} // namespace NetworkCommissioning
} // namespace Clusters
} // namespace app
} // namespace chip
