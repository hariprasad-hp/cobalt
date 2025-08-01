// Copyright 2024 The Cobalt Authors. All Rights Reserved.
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

#ifndef COBALT_BROWSER_COBALT_CONTENT_BROWSER_CLIENT_H_
#define COBALT_BROWSER_COBALT_CONTENT_BROWSER_CLIENT_H_

#include "base/threading/thread_checker.h"
#include "cobalt/browser/client_hint_headers/cobalt_trusted_url_loader_header_client.h"
#include "cobalt/browser/cobalt_web_contents_delegate.h"
#include "cobalt/shell/browser/shell_content_browser_client.h"
#include "content/public/browser/generated_code_cache_settings.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"

class PrefService;

namespace content {
class BrowserMainParts;
class RenderFrameHost;
class RenderProcessHost;
class WebContents;
}  // namespace content

namespace metrics_services_manager {
class MetricsServicesManager;
}  // namespace metrics_services_manager

namespace mojo {
template <typename>
class BinderMapWithContext;
}  // namespace mojo

namespace cobalt {

namespace media {
class VideoGeometrySetterService;
}  // namespace media

class CobaltMetricsServicesManagerClient;
class CobaltWebContentsObserver;

// This class allows Cobalt to inject specific logic in the business of the
// browser (i.e. of Content), for example for startup or to override the UA.
// TODO(b/390021478): In time CobaltContentBrowserClient should derive and
// implement ContentBrowserClient, since ShellContentBrowserClient is more like
// a demo around Content.
class CobaltContentBrowserClient : public content::ShellContentBrowserClient {
 public:
  CobaltContentBrowserClient();

  CobaltContentBrowserClient(const CobaltContentBrowserClient&) = delete;
  CobaltContentBrowserClient& operator=(const CobaltContentBrowserClient&) =
      delete;

  ~CobaltContentBrowserClient() override;

  // ShellContentBrowserClient overrides.
  std::unique_ptr<content::BrowserMainParts> CreateBrowserMainParts(
      bool is_integration_test) override;
  std::vector<std::unique_ptr<content::NavigationThrottle>>
  CreateThrottlesForNavigation(content::NavigationHandle* handle) override;
  content::GeneratedCodeCacheSettings GetGeneratedCodeCacheSettings(
      content::BrowserContext* context) override;
  std::string GetApplicationLocale() override;
  std::string GetUserAgent() override;
  std::string GetFullUserAgent() override;
  std::string GetReducedUserAgent() override;
  blink::UserAgentMetadata GetUserAgentMetadata() override;
  content::StoragePartitionConfig GetStoragePartitionConfigForSite(
      content::BrowserContext* browser_context,
      const GURL& site) override;
  void ConfigureNetworkContextParams(
      content::BrowserContext* context,
      bool in_memory,
      const base::FilePath& relative_partition_path,
      network::mojom::NetworkContextParams* network_context_params,
      cert_verifier::mojom::CertVerifierCreationParams*
          cert_verifier_creation_params) override;
  void OverrideWebkitPrefs(content::WebContents* web_contents,
                           blink::web_pref::WebPreferences* prefs) override;
  void OnWebContentsCreated(content::WebContents* web_contents) override;
  void RegisterBrowserInterfaceBindersForFrame(
      content::RenderFrameHost* render_frame_host,
      mojo::BinderMapWithContext<content::RenderFrameHost*>* binder_map)
      override;
  void ExposeInterfacesToRenderer(
      service_manager::BinderRegistry* registry,
      blink::AssociatedInterfaceRegistry* associated_registry,
      content::RenderProcessHost* render_process_host) override;
  void BindGpuHostReceiver(mojo::GenericPendingReceiver receiver) override;

  // Initializes all necessary parameters to create the feature list and calls
  // base::FeatureList::SetInstance() to set the global instance.
  void CreateFeatureListAndFieldTrials();

  // Read from the experiment config, override features, and associate feature
  // params for Cobalt experiments.
  void SetUpCobaltFeaturesAndParams(base::FeatureList* feature_list);

  bool WillCreateURLLoaderFactory(
      content::BrowserContext* browser_context,
      content::RenderFrameHost* frame,
      int render_process_id,
      URLLoaderFactoryType type,
      const url::Origin& request_initiator,
      absl::optional<int64_t> navigation_id,
      ukm::SourceIdObj ukm_source_id,
      mojo::PendingReceiver<network::mojom::URLLoaderFactory>* factory_receiver,
      mojo::PendingRemote<network::mojom::TrustedURLLoaderHeaderClient>*
          header_client,
      bool* bypass_redirect_checks,
      bool* disable_secure_dns,
      network::mojom::URLLoaderFactoryOverridePtr* factory_override) override;

 private:
  void CreateVideoGeometrySetterService();

  std::unique_ptr<CobaltWebContentsObserver> web_contents_observer_;
  std::unique_ptr<CobaltWebContentsDelegate> web_contents_delegate_;
  std::unique_ptr<media::VideoGeometrySetterService, base::OnTaskRunnerDeleter>
      video_geometry_setter_service_;
  std::vector<std::unique_ptr<browser::CobaltTrustedURLLoaderHeaderClient>>
      cobalt_header_clients_;

  THREAD_CHECKER(thread_checker_);
};

}  // namespace cobalt

#endif  // COBALT_BROWSER_COBALT_CONTENT_BROWSER_CLIENT_H_
