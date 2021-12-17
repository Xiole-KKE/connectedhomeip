/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
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

// THIS FILE IS GENERATED BY ZAP

#include <zap-generated/CHIPClientCallbacks.h>

#include <cinttypes>

#include <app-common/zap-generated/enums.h>
#include <app/util/CHIPDeviceCallbacksMgr.h>
#include <app/util/af-enums.h>
#include <app/util/af.h>
#include <app/util/basic-types.h>
#include <lib/core/CHIPEncoding.h>
#include <lib/support/SafeInt.h>
#include <lib/support/TypeTraits.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace ::chip;
using namespace ::chip::app::DataModel;

namespace {
[[maybe_unused]] constexpr uint16_t kByteSpanSizeLengthInBytes = 2;
} // namespace

#define CHECK_STATUS_WITH_RETVAL(error, retval)                                                                                    \
    if (CHIP_NO_ERROR != error)                                                                                                    \
    {                                                                                                                              \
        ChipLogError(Zcl, "CHECK_STATUS %s", ErrorStr(error));                                                                     \
        if (onFailureCallback != nullptr)                                                                                          \
        {                                                                                                                          \
            Callback::Callback<DefaultFailureCallback> * cb =                                                                      \
                Callback::Callback<DefaultFailureCallback>::FromCancelable(onFailureCallback);                                     \
            cb->mCall(cb->mContext, static_cast<uint8_t>(EMBER_ZCL_STATUS_INVALID_VALUE));                                         \
        }                                                                                                                          \
        return retval;                                                                                                             \
    }

#define CHECK_STATUS(error) CHECK_STATUS_WITH_RETVAL(error, true)
#define CHECK_STATUS_VOID(error) CHECK_STATUS_WITH_RETVAL(error, )

#define CHECK_MESSAGE_LENGTH_WITH_RETVAL(value, retval)                                                                            \
    if (!CanCastTo<uint16_t>(value))                                                                                               \
    {                                                                                                                              \
        ChipLogError(Zcl, "CHECK_MESSAGE_LENGTH expects a uint16_t value, got: %d", value);                                        \
        if (onFailureCallback != nullptr)                                                                                          \
        {                                                                                                                          \
            Callback::Callback<DefaultFailureCallback> * cb =                                                                      \
                Callback::Callback<DefaultFailureCallback>::FromCancelable(onFailureCallback);                                     \
            cb->mCall(cb->mContext, static_cast<uint8_t>(EMBER_ZCL_STATUS_INVALID_VALUE));                                         \
        }                                                                                                                          \
        return retval;                                                                                                             \
    }                                                                                                                              \
                                                                                                                                   \
    if (messageLen < value)                                                                                                        \
    {                                                                                                                              \
        ChipLogError(Zcl, "Unexpected response length: %d", messageLen);                                                           \
        if (onFailureCallback != nullptr)                                                                                          \
        {                                                                                                                          \
            Callback::Callback<DefaultFailureCallback> * cb =                                                                      \
                Callback::Callback<DefaultFailureCallback>::FromCancelable(onFailureCallback);                                     \
            cb->mCall(cb->mContext, static_cast<uint8_t>(EMBER_ZCL_STATUS_INVALID_VALUE));                                         \
        }                                                                                                                          \
        return retval;                                                                                                             \
    }                                                                                                                              \
                                                                                                                                   \
    messageLen = static_cast<uint16_t>(messageLen - static_cast<uint16_t>(value));

#define CHECK_MESSAGE_LENGTH(value) CHECK_MESSAGE_LENGTH_WITH_RETVAL(value, true)
#define CHECK_MESSAGE_LENGTH_VOID(value) CHECK_MESSAGE_LENGTH_WITH_RETVAL(value, )

#define GET_RESPONSE_CALLBACKS(name)                                                                                               \
    Callback::Cancelable * onSuccessCallback = nullptr;                                                                            \
    Callback::Cancelable * onFailureCallback = nullptr;                                                                            \
    NodeId sourceId                          = emberAfCurrentCommand()->SourceNodeId();                                            \
    uint8_t sequenceNumber                   = emberAfCurrentCommand()->seqNum;                                                    \
    CHIP_ERROR err = gCallbacks.GetResponseCallback(sourceId, sequenceNumber, &onSuccessCallback, &onFailureCallback);             \
                                                                                                                                   \
    if (CHIP_NO_ERROR != err)                                                                                                      \
    {                                                                                                                              \
        if (onSuccessCallback == nullptr)                                                                                          \
        {                                                                                                                          \
            ChipLogDetail(Zcl, "%s: Missing success callback", name);                                                              \
        }                                                                                                                          \
                                                                                                                                   \
        if (onFailureCallback == nullptr)                                                                                          \
        {                                                                                                                          \
            ChipLogDetail(Zcl, "%s: Missing failure callback", name);                                                              \
        }                                                                                                                          \
                                                                                                                                   \
        return true;                                                                                                               \
    }

#define GET_CLUSTER_RESPONSE_CALLBACKS(name)                                                                                       \
    Callback::Cancelable * onSuccessCallback = nullptr;                                                                            \
    Callback::Cancelable * onFailureCallback = nullptr;                                                                            \
    NodeId sourceIdentifier                  = reinterpret_cast<NodeId>(commandObj);                                               \
    /* #6559: Currently, we only have one commands for the IMInvokeCommands and to a device, so the seqNum is always set to 0. */  \
    CHIP_ERROR err = gCallbacks.GetResponseCallback(sourceIdentifier, 0, &onSuccessCallback, &onFailureCallback);                  \
                                                                                                                                   \
    if (CHIP_NO_ERROR != err)                                                                                                      \
    {                                                                                                                              \
        if (onSuccessCallback == nullptr)                                                                                          \
        {                                                                                                                          \
            ChipLogDetail(Zcl, "%s: Missing success callback", name);                                                              \
        }                                                                                                                          \
                                                                                                                                   \
        if (onFailureCallback == nullptr)                                                                                          \
        {                                                                                                                          \
            ChipLogDetail(Zcl, "%s: Missing failure callback", name);                                                              \
        }                                                                                                                          \
                                                                                                                                   \
        return true;                                                                                                               \
    }

// Singleton instance of the callbacks manager
app::CHIPDeviceCallbacksMgr & gCallbacks = app::CHIPDeviceCallbacksMgr::GetInstance();

void GeneralCommissioningClusterBasicCommissioningInfoListListAttributeFilter(TLV::TLVReader * tlvData,
                                                                              Callback::Cancelable * onSuccessCallback,
                                                                              Callback::Cancelable * onFailureCallback)
{
    chip::app::DataModel::DecodableList<
        chip::app::Clusters::GeneralCommissioning::Structs::BasicCommissioningInfoType::DecodableType>
        list;
    CHIP_ERROR err = Decode(*tlvData, list);
    if (err != CHIP_NO_ERROR)
    {
        if (onFailureCallback != nullptr)
        {
            Callback::Callback<DefaultFailureCallback> * cb =
                Callback::Callback<DefaultFailureCallback>::FromCancelable(onFailureCallback);
            cb->mCall(cb->mContext, EMBER_ZCL_STATUS_INVALID_VALUE);
        }
        return;
    }

    Callback::Callback<GeneralCommissioningBasicCommissioningInfoListListAttributeCallback> * cb =
        Callback::Callback<GeneralCommissioningBasicCommissioningInfoListListAttributeCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, list);
}

void NetworkCommissioningClusterNetworksListAttributeFilter(TLV::TLVReader * tlvData, Callback::Cancelable * onSuccessCallback,
                                                            Callback::Cancelable * onFailureCallback)
{
    chip::app::DataModel::DecodableList<chip::app::Clusters::NetworkCommissioning::Structs::NetworkInfo::DecodableType> list;
    CHIP_ERROR err = Decode(*tlvData, list);
    if (err != CHIP_NO_ERROR)
    {
        if (onFailureCallback != nullptr)
        {
            Callback::Callback<DefaultFailureCallback> * cb =
                Callback::Callback<DefaultFailureCallback>::FromCancelable(onFailureCallback);
            cb->mCall(cb->mContext, EMBER_ZCL_STATUS_INVALID_VALUE);
        }
        return;
    }

    Callback::Callback<NetworkCommissioningNetworksListAttributeCallback> * cb =
        Callback::Callback<NetworkCommissioningNetworksListAttributeCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, list);
}

void OperationalCredentialsClusterFabricsListListAttributeFilter(TLV::TLVReader * tlvData, Callback::Cancelable * onSuccessCallback,
                                                                 Callback::Cancelable * onFailureCallback)
{
    chip::app::DataModel::DecodableList<chip::app::Clusters::OperationalCredentials::Structs::FabricDescriptor::DecodableType> list;
    CHIP_ERROR err = Decode(*tlvData, list);
    if (err != CHIP_NO_ERROR)
    {
        if (onFailureCallback != nullptr)
        {
            Callback::Callback<DefaultFailureCallback> * cb =
                Callback::Callback<DefaultFailureCallback>::FromCancelable(onFailureCallback);
            cb->mCall(cb->mContext, EMBER_ZCL_STATUS_INVALID_VALUE);
        }
        return;
    }

    Callback::Callback<OperationalCredentialsFabricsListListAttributeCallback> * cb =
        Callback::Callback<OperationalCredentialsFabricsListListAttributeCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, list);
}

void OperationalCredentialsClusterTrustedRootCertificatesListAttributeFilter(TLV::TLVReader * tlvData,
                                                                             Callback::Cancelable * onSuccessCallback,
                                                                             Callback::Cancelable * onFailureCallback)
{
    chip::app::DataModel::DecodableList<chip::ByteSpan> list;
    CHIP_ERROR err = Decode(*tlvData, list);
    if (err != CHIP_NO_ERROR)
    {
        if (onFailureCallback != nullptr)
        {
            Callback::Callback<DefaultFailureCallback> * cb =
                Callback::Callback<DefaultFailureCallback>::FromCancelable(onFailureCallback);
            cb->mCall(cb->mContext, EMBER_ZCL_STATUS_INVALID_VALUE);
        }
        return;
    }

    Callback::Callback<OperationalCredentialsTrustedRootCertificatesListAttributeCallback> * cb =
        Callback::Callback<OperationalCredentialsTrustedRootCertificatesListAttributeCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, list);
}

bool emberAfGeneralCommissioningClusterArmFailSafeResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                                   uint8_t errorCode, chip::CharSpan debugText)
{
    ChipLogProgress(Zcl, "ArmFailSafeResponse:");
    ChipLogProgress(Zcl, "  errorCode: %" PRIu8 "", errorCode);
    ChipLogProgress(Zcl, "  debugText: %.*s", static_cast<int>(debugText.size()), debugText.data());

    GET_CLUSTER_RESPONSE_CALLBACKS("GeneralCommissioningClusterArmFailSafeResponseCallback");

    Callback::Callback<GeneralCommissioningClusterArmFailSafeResponseCallback> * cb =
        Callback::Callback<GeneralCommissioningClusterArmFailSafeResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, errorCode, debugText);
    return true;
}

bool emberAfGeneralCommissioningClusterCommissioningCompleteResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                                             uint8_t errorCode, chip::CharSpan debugText)
{
    ChipLogProgress(Zcl, "CommissioningCompleteResponse:");
    ChipLogProgress(Zcl, "  errorCode: %" PRIu8 "", errorCode);
    ChipLogProgress(Zcl, "  debugText: %.*s", static_cast<int>(debugText.size()), debugText.data());

    GET_CLUSTER_RESPONSE_CALLBACKS("GeneralCommissioningClusterCommissioningCompleteResponseCallback");

    Callback::Callback<GeneralCommissioningClusterCommissioningCompleteResponseCallback> * cb =
        Callback::Callback<GeneralCommissioningClusterCommissioningCompleteResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, errorCode, debugText);
    return true;
}

bool emberAfGeneralCommissioningClusterSetRegulatoryConfigResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                                           uint8_t errorCode, chip::CharSpan debugText)
{
    ChipLogProgress(Zcl, "SetRegulatoryConfigResponse:");
    ChipLogProgress(Zcl, "  errorCode: %" PRIu8 "", errorCode);
    ChipLogProgress(Zcl, "  debugText: %.*s", static_cast<int>(debugText.size()), debugText.data());

    GET_CLUSTER_RESPONSE_CALLBACKS("GeneralCommissioningClusterSetRegulatoryConfigResponseCallback");

    Callback::Callback<GeneralCommissioningClusterSetRegulatoryConfigResponseCallback> * cb =
        Callback::Callback<GeneralCommissioningClusterSetRegulatoryConfigResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, errorCode, debugText);
    return true;
}

bool emberAfNetworkCommissioningClusterConnectNetworkResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                                      uint8_t NetworkingStatus, chip::CharSpan DebugText,
                                                                      int32_t ErrorValue)
{
    ChipLogProgress(Zcl, "ConnectNetworkResponse:");
    ChipLogProgress(Zcl, "  NetworkingStatus: %" PRIu8 "", NetworkingStatus);
    ChipLogProgress(Zcl, "  DebugText: %.*s", static_cast<int>(DebugText.size()), DebugText.data());
    ChipLogProgress(Zcl, "  ErrorValue: %" PRId32 "", ErrorValue);

    GET_CLUSTER_RESPONSE_CALLBACKS("NetworkCommissioningClusterConnectNetworkResponseCallback");

    Callback::Callback<NetworkCommissioningClusterConnectNetworkResponseCallback> * cb =
        Callback::Callback<NetworkCommissioningClusterConnectNetworkResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, NetworkingStatus, DebugText, ErrorValue);
    return true;
}

bool emberAfNetworkCommissioningClusterNetworkConfigResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                                     uint8_t NetworkingStatus, chip::CharSpan DebugText)
{
    ChipLogProgress(Zcl, "NetworkConfigResponse:");
    ChipLogProgress(Zcl, "  NetworkingStatus: %" PRIu8 "", NetworkingStatus);
    ChipLogProgress(Zcl, "  DebugText: %.*s", static_cast<int>(DebugText.size()), DebugText.data());

    GET_CLUSTER_RESPONSE_CALLBACKS("NetworkCommissioningClusterNetworkConfigResponseCallback");

    Callback::Callback<NetworkCommissioningClusterNetworkConfigResponseCallback> * cb =
        Callback::Callback<NetworkCommissioningClusterNetworkConfigResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, NetworkingStatus, DebugText);
    return true;
}

bool emberAfNetworkCommissioningClusterScanNetworksResponseCallback(
    EndpointId endpoint, app::CommandSender * commandObj, uint8_t NetworkingStatus, chip::CharSpan DebugText,
    /* TYPE WARNING: array array defaults to */ uint8_t * WiFiScanResults,
    /* TYPE WARNING: array array defaults to */ uint8_t * ThreadScanResults)
{
    ChipLogProgress(Zcl, "ScanNetworksResponse:");
    ChipLogProgress(Zcl, "  NetworkingStatus: %" PRIu8 "", NetworkingStatus);
    ChipLogProgress(Zcl, "  DebugText: %.*s", static_cast<int>(DebugText.size()), DebugText.data());
    ChipLogProgress(Zcl, "  WiFiScanResults: %p", WiFiScanResults);
    ChipLogProgress(Zcl, "  ThreadScanResults: %p", ThreadScanResults);

    GET_CLUSTER_RESPONSE_CALLBACKS("NetworkCommissioningClusterScanNetworksResponseCallback");

    Callback::Callback<NetworkCommissioningClusterScanNetworksResponseCallback> * cb =
        Callback::Callback<NetworkCommissioningClusterScanNetworksResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, NetworkingStatus, DebugText, WiFiScanResults, ThreadScanResults);
    return true;
}

bool emberAfOperationalCredentialsClusterAttestationResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                                     chip::ByteSpan AttestationElements, chip::ByteSpan Signature)
{
    ChipLogProgress(Zcl, "AttestationResponse:");
    ChipLogProgress(Zcl, "  AttestationElements: %zu", AttestationElements.size());
    ChipLogProgress(Zcl, "  Signature: %zu", Signature.size());

    GET_CLUSTER_RESPONSE_CALLBACKS("OperationalCredentialsClusterAttestationResponseCallback");

    Callback::Callback<OperationalCredentialsClusterAttestationResponseCallback> * cb =
        Callback::Callback<OperationalCredentialsClusterAttestationResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, AttestationElements, Signature);
    return true;
}

bool emberAfOperationalCredentialsClusterCertificateChainResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                                          chip::ByteSpan Certificate)
{
    ChipLogProgress(Zcl, "CertificateChainResponse:");
    ChipLogProgress(Zcl, "  Certificate: %zu", Certificate.size());

    GET_CLUSTER_RESPONSE_CALLBACKS("OperationalCredentialsClusterCertificateChainResponseCallback");

    Callback::Callback<OperationalCredentialsClusterCertificateChainResponseCallback> * cb =
        Callback::Callback<OperationalCredentialsClusterCertificateChainResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, Certificate);
    return true;
}

bool emberAfOperationalCredentialsClusterNOCResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                             uint8_t StatusCode, uint8_t FabricIndex, chip::CharSpan DebugText)
{
    ChipLogProgress(Zcl, "NOCResponse:");
    ChipLogProgress(Zcl, "  StatusCode: %" PRIu8 "", StatusCode);
    ChipLogProgress(Zcl, "  FabricIndex: %" PRIu8 "", FabricIndex);
    ChipLogProgress(Zcl, "  DebugText: %.*s", static_cast<int>(DebugText.size()), DebugText.data());

    GET_CLUSTER_RESPONSE_CALLBACKS("OperationalCredentialsClusterNOCResponseCallback");

    Callback::Callback<OperationalCredentialsClusterNOCResponseCallback> * cb =
        Callback::Callback<OperationalCredentialsClusterNOCResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, StatusCode, FabricIndex, DebugText);
    return true;
}

bool emberAfOperationalCredentialsClusterOpCSRResponseCallback(EndpointId endpoint, app::CommandSender * commandObj,
                                                               chip::ByteSpan NOCSRElements, chip::ByteSpan AttestationSignature)
{
    ChipLogProgress(Zcl, "OpCSRResponse:");
    ChipLogProgress(Zcl, "  NOCSRElements: %zu", NOCSRElements.size());
    ChipLogProgress(Zcl, "  AttestationSignature: %zu", AttestationSignature.size());

    GET_CLUSTER_RESPONSE_CALLBACKS("OperationalCredentialsClusterOpCSRResponseCallback");

    Callback::Callback<OperationalCredentialsClusterOpCSRResponseCallback> * cb =
        Callback::Callback<OperationalCredentialsClusterOpCSRResponseCallback>::FromCancelable(onSuccessCallback);
    cb->mCall(cb->mContext, NOCSRElements, AttestationSignature);
    return true;
}
