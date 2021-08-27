// Copyright (c) 2021, Maksym Motorny. All rights reserved.
// Use of this source code is governed by a 3-clause BSD license that can be
// found in the LICENSE file.

#include <cstring>

#include "XPLMDefs.h"

namespace motorny {
namespace helloxplane {

enum PluginStart {
  kPlugingStartFailure = 0,
  kPlugingStartSuccess = 1,
};

enum PluginEnable {
  kPlugingEnableFailure = 0,
  kPlugingEnableSuccess = 1,
};

class Plugin {
 public:
  Plugin(char *name, char *signature, char *description);
  ~Plugin();

  void Enable();
  void Disable();
  void ReceiveMessage(XPLMPluginID plugin_id, int message, void *message_data);
};

namespace {
Plugin *g_plugin = nullptr;
}  // namespace

Plugin::Plugin(char *name, char *signature, char *description) {
  constexpr auto kMaxOutStringLength = 256;
  strcpy_s(name, kMaxOutStringLength, "Motorny Device Datarefs");
  strcpy_s(signature, kMaxOutStringLength, "motorny.devicedatarefs");
  strcpy_s(description, kMaxOutStringLength, "Provides access to USB HID and COM devices through datarefs.");
}

Plugin::~Plugin() {}

void Plugin::Enable() {}

void Plugin::Disable() {}

void Plugin::ReceiveMessage(XPLMPluginID /*plugin_id*/, int /*message*/,
                            void * /*message_data*/) {}

PLUGIN_API int XPluginStart(char *name, char *signature, char *description) {
  g_plugin = new Plugin(name, signature, description);
  return kPlugingStartSuccess;
}

PLUGIN_API void XPluginStop() {
  delete g_plugin;
  g_plugin = nullptr;
}

PLUGIN_API int XPluginEnable() {
  g_plugin->Enable();
  return kPlugingEnableSuccess;
}

PLUGIN_API void XPluginDisable() { g_plugin->Disable(); }

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID plugin_id, int message,
                                      void *message_data) {
  g_plugin->ReceiveMessage(plugin_id, message, message_data);
}

}  // namespace helloxplane
}  // namespace motorny
