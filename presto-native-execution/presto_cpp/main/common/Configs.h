/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <folly/SocketAddress.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "velox/core/Context.h"

namespace facebook::presto {

class ConfigBase {
 public:
  /// Reads configuration properties from the specified file. Must be called
  /// before calling any of the getters below.
  /// @param filePath Path to configuration file.
  void initialize(const std::string& filePath);

  /// Uses a config object already materialized.
  void initialize(std::unique_ptr<velox::Config>&& config) {
    config_ = std::move(config);
  }

  /// Adds or replaces value at the given key. Can be used by debugging or
  /// testing code.
  /// Returns previous value if there was any.
  folly::Optional<std::string> setValue(
      const std::string& propertyName,
      const std::string& value);

  template <typename T>
  T requiredProperty(const std::string& propertyName) const {
    auto propertyValue = config_->get<T>(propertyName);
    if (propertyValue.has_value()) {
      return propertyValue.value();
    } else {
      VELOX_USER_FAIL(
          "{} is required in the {} file.", propertyName, filePath_);
    }
  }

  std::string requiredProperty(const std::string& propertyName) const {
    auto propertyValue = config_->get(propertyName);
    if (propertyValue.has_value()) {
      return propertyValue.value();
    } else {
      VELOX_USER_FAIL(
          "{} is required in the {} file.", propertyName, filePath_);
    }
  }

  template <typename T>
  folly::Optional<T> optionalProperty(const std::string& propertyName) const {
    return config_->get<T>(propertyName);
  }

  folly::Optional<std::string> optionalProperty(
      const std::string& propertyName) const {
    return config_->get(propertyName);
  }

  /// Returns copy of the config values map.
  std::unordered_map<std::string, std::string> values() const {
    return config_->valuesCopy();
  }

 protected:
  ConfigBase();

  std::unique_ptr<velox::Config> config_;
  std::string filePath_;
};

/// Provides access to system properties defined in config.properties file.
class SystemConfig : public ConfigBase {
 public:
  static constexpr std::string_view kMutableConfig{"mutable-config"};
  static constexpr std::string_view kPrestoVersion{"presto.version"};
  static constexpr std::string_view kHttpServerHttpPort{
      "http-server.http.port"};

  /// This option allows a port closed in TIME_WAIT state to be reused
  /// immediately upon worker startup. This property is mainly used by batch
  /// processing. For interactive query, the worker uses a dynamic port upon
  /// startup.
  static constexpr std::string_view kHttpServerReusePort{
      "http-server.reuse-port"};
  static constexpr std::string_view kDiscoveryUri{"discovery.uri"};
  static constexpr std::string_view kMaxDriversPerTask{
      "task.max-drivers-per-task"};
  static constexpr std::string_view kConcurrentLifespansPerTask{
      "task.concurrent-lifespans-per-task"};
  static constexpr std::string_view kHttpExecThreads{"http_exec_threads"};
  static constexpr std::string_view kHttpServerHttpsPort{
      "http-server.https.port"};
  static constexpr std::string_view kHttpServerHttpsEnabled{
      "http-server.https.enabled"};
  static constexpr std::string_view kHttpsSupportedCiphers{
      "https-supported-ciphers"};
  static constexpr std::string_view kHttpsCertPath{"https-cert-path"};
  static constexpr std::string_view kHttpsKeyPath{"https-key-path"};
  static constexpr std::string_view kHttpsClientCertAndKeyPath{
      "https-client-cert-key-path"};

  /// Number of threads for async io. Disabled if zero.
  static constexpr std::string_view kNumIoThreads{"num-io-threads"};
  static constexpr std::string_view kNumConnectorIoThreads{
      "num-connector-io-threads"};
  static constexpr std::string_view kNumQueryThreads{"num-query-threads"};
  static constexpr std::string_view kNumSpillThreads{"num-spill-threads"};
  static constexpr std::string_view kSpillerSpillPath{
      "experimental.spiller-spill-path"};
  static constexpr std::string_view kShutdownOnsetSec{"shutdown-onset-sec"};
  static constexpr std::string_view kSystemMemoryGb{"system-memory-gb"};
  static constexpr std::string_view kAsyncCacheSsdGb{"async-cache-ssd-gb"};
  static constexpr std::string_view kAsyncCacheSsdCheckpointGb{
      "async-cache-ssd-checkpoint-gb"};
  static constexpr std::string_view kAsyncCacheSsdPath{"async-cache-ssd-path"};

  /// In file systems, such as btrfs, supporting cow (copy on write), the ssd
  /// cache can use all ssd space and stop working. To prevent that, use this
  /// option to disable cow for cache files.
  static constexpr std::string_view kAsyncCacheSsdDisableFileCow{
      "async-cache-ssd-disable-file-cow"};
  static constexpr std::string_view kEnableSerializedPageChecksum{
      "enable-serialized-page-checksum"};
  static constexpr std::string_view kUseMmapArena{"use-mmap-arena"};
  static constexpr std::string_view kMmapArenaCapacityRatio{
      "mmap-arena-capacity-ratio"};
  static constexpr std::string_view kUseMmapAllocator{"use-mmap-allocator"};
  static constexpr std::string_view kEnableVeloxTaskLogging{
      "enable_velox_task_logging"};
  static constexpr std::string_view kEnableVeloxExprSetLogging{
      "enable_velox_expression_logging"};
  static constexpr std::string_view kLocalShuffleMaxPartitionBytes{
      "shuffle.local.max-partition-bytes"};
  static constexpr std::string_view kShuffleName{"shuffle.name"};
  static constexpr std::string_view kHttpEnableAccessLog{
      "http-server.enable-access-log"};
  static constexpr std::string_view kHttpEnableStatsFilter{
      "http-server.enable-stats-filter"};
  static constexpr std::string_view kRegisterTestFunctions{
      "register-test-functions"};

  /// The options to configure the max quantized memory allocation size to store
  /// the received http response data.
  static constexpr std::string_view kHttpMaxAllocateBytes{
      "http-server.max-response-allocate-bytes"};
  static constexpr std::string_view kQueryMaxMemoryPerNode{
      "query.max-memory-per-node"};

  /// This system property is added for not crashing the cluster when memory
  /// leak is detected. The check should be disabled in production cluster.
  static constexpr std::string_view kEnableMemoryLeakCheck{
      "enable-memory-leak-check"};

  /// Port used by the remote function thrift server.
  static constexpr std::string_view kRemoteFunctionServerThriftPort{
      "remote-function-server.thrift.port"};

  /// Most server nodes today (May 2022) have at least 16 cores.
  /// Setting the default maximum drivers per task to this value will
  /// provide a better off-shelf experience.
  static constexpr int32_t kMaxDriversPerTaskDefault = 16;
  static constexpr bool kHttpServerReusePortDefault = false;
  static constexpr int32_t kConcurrentLifespansPerTaskDefault = 1;
  static constexpr int32_t kHttpExecThreadsDefault = 8;
  static constexpr bool kHttpServerHttpsEnabledDefault = false;
  static constexpr std::string_view kHttpsSupportedCiphersDefault{
      "ECDHE-ECDSA-AES256-GCM-SHA384,AES256-GCM-SHA384"};
  static constexpr int32_t kNumIoThreadsDefault = 30;
  static constexpr int32_t kNumConnectorIoThreadsDefault = 30;
  static constexpr int32_t kShutdownOnsetSecDefault = 10;
  static constexpr int32_t kSystemMemoryGbDefault = 40;
  static constexpr int32_t kMmapArenaCapacityRatioDefault = 10;
  static constexpr uint64_t kLocalShuffleMaxPartitionBytesDefault = 1 << 28;
  static constexpr uint64_t kAsyncCacheSsdGbDefault = 0;
  static constexpr uint64_t kAsyncCacheSsdCheckpointGbDefault = 0;
  static constexpr std::string_view kAsyncCacheSsdPathDefault{
      "/mnt/flash/async_cache."};
  static constexpr bool kAsyncCacheSsdDisableFileCowDefault{false};
  static constexpr std::string_view kShuffleNameDefault{""};
  static constexpr bool kEnableSerializedPageChecksumDefault = true;
  static constexpr bool kEnableVeloxTaskLoggingDefault = false;
  static constexpr bool kEnableVeloxExprSetLoggingDefault = false;
  static constexpr bool kUseMmapArenaDefault = false;
  static constexpr bool kUseMmapAllocatorDefault{true};
  static constexpr bool kHttpEnableAccessLogDefault = false;
  static constexpr bool kHttpEnableStatsFilterDefault = false;
  static constexpr bool kRegisterTestFunctionsDefault = false;
  static constexpr uint64_t kHttpMaxAllocateBytesDefault = 64 << 10;
  /// 1/10 of kSystemMemoryGbDefault.
  static constexpr uint64_t kQueryMaxMemoryPerNodeDefault = 4UL << 30;
  static constexpr bool kEnableMemoryLeakCheckDefault = true;

  static SystemConfig* instance();

  int httpServerHttpPort() const;

  bool httpServerReusePort() const;

  bool httpServerHttpsEnabled() const;

  int httpServerHttpsPort() const;

  // A list of ciphers (comma separated) that are supported by
  // server and client. Note Java and folly::SSLContext use different names to
  // refer to the same cipher. For e.g. TLS_RSA_WITH_AES_256_GCM_SHA384 in Java
  // and AES256-GCM-SHA384 in folly::SSLContext. More details can be found here:
  // https://www.openssl.org/docs/manmaster/man1/openssl-ciphers.html. The
  // ciphers enable worker to worker, worker to coordinator and
  // coordinator to worker communication. At least one cipher needs to be
  // shared for the above 3 communication to work.
  std::string httpsSupportedCiphers() const;

  // Note: Java packages cert and key in combined JKS file. But CPP requires
  // them separately. The HTTPS provides integrity and not
  // security(authentication/authorization). But the HTTPS will protect against
  // data corruption by bad router and man in middle attacks.
  std::optional<std::string> httpsCertPath() const;

  std::optional<std::string> httpsKeyPath() const;

  // Http client expects the cert and key file to be packed into a single file
  // (most commonly .pem format) The file should not be password protected. If
  // required, break this down to 3 configs one for cert,key and password later.
  std::optional<std::string> httpsClientCertAndKeyPath() const;

  std::string prestoVersion() const;

  std::optional<std::string> discoveryUri() const;

  std::optional<folly::SocketAddress> remoteFunctionServerLocation() const;

  int32_t maxDriversPerTask() const;

  int32_t concurrentLifespansPerTask() const;

  int32_t httpExecThreads() const;

  /// Size of global IO executor.
  int32_t numIoThreads() const;

  /// Size of IO executor for connectors to do preload/prefetch
  int32_t numConnectorIoThreads() const;

  int32_t numQueryThreads() const;

  int32_t numSpillThreads() const;

  std::string spillerSpillPath() const;

  int32_t shutdownOnsetSec() const;

  int32_t systemMemoryGb() const;

  uint64_t asyncCacheSsdGb() const;

  uint64_t asyncCacheSsdCheckpointGb() const;

  uint64_t localShuffleMaxPartitionBytes() const;

  std::string asyncCacheSsdPath() const;

  bool asyncCacheSsdDisableFileCow() const;

  std::string shuffleName() const;

  bool enableSerializedPageChecksum() const;

  bool enableVeloxTaskLogging() const;

  bool enableVeloxExprSetLogging() const;

  bool useMmapArena() const;

  int32_t mmapArenaCapacityRatio() const;

  bool useMmapAllocator() const;

  bool enableHttpAccessLog() const;

  bool enableHttpStatsFilter() const;

  bool registerTestFunctions() const;

  uint64_t httpMaxAllocateBytes() const;

  uint64_t queryMaxMemoryPerNode() const;

  bool enableMemoryLeakCheck() const;
};

/// Provides access to node properties defined in node.properties file.
class NodeConfig : public ConfigBase {
 public:
  static constexpr std::string_view kNodeEnvironment{"node.environment"};
  static constexpr std::string_view kNodeId{"node.id"};
  static constexpr std::string_view kNodeIp{"node.ip"};
  static constexpr std::string_view kNodeLocation{"node.location"};
  static constexpr std::string_view kNodeMemoryGb{"node.memory_gb"};

  static NodeConfig* instance();

  std::string nodeEnvironment() const;

  std::string nodeId() const;

  std::string nodeIp(
      const std::function<std::string()>& defaultIp = nullptr) const;

  std::string nodeLocation() const;

  uint64_t nodeMemoryGb(
      const std::function<uint64_t()>& defaultNodeMemoryGb = nullptr) const;
};

} // namespace facebook::presto
