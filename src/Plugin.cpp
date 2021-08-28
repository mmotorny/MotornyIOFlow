// Copyright (c) 2021, Maksym Motorny. All rights reserved.
// Use of this source code is governed by a 3-clause BSD license that can be
// found in the LICENSE file.

#if defined(MOTORNY_DEVICE_DATAREFS_PLATFORM_WINDOWS)
#include <Windows.h>
#endif  // defined(MOTORNY_DEVICE_DATAREFS_PLATFORM_WINDOWS)

#include <cassert>
#include <cstring>
#include <format>
#include <regex>
#include <variant>

#include "XPLMDefs.h"
#include "XPLMUtilities.h"
#include "boost/locale/message.hpp"

namespace motorny {
namespace devicedatarefs {

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
  strcpy_s(name, kMaxOutStringLength,
           boost::locale::translate("Motorny Device Datarefs").str().c_str());
  strcpy_s(signature, kMaxOutStringLength, "motorny.devicedatarefs");
  strcpy_s(description, kMaxOutStringLength,
           boost::locale::translate(
               "Provides access to USB HID and COM devices through datarefs.")
               .str()
               .c_str());
}

Plugin::~Plugin() {}

namespace {

#if defined(MOTORNY_DEVICE_DATAREFS_PLATFORM_WINDOWS)
std::string ConvertUcs2ToUtf8(wchar_t *ucs2_string) {
  int utf8_string_size = WideCharToMultiByte(CP_UTF8, 0, ucs2_string, -1,
                                             nullptr, 0, nullptr, nullptr);
  std::vector<char> utf8_string(utf8_string_size, 0);
  WideCharToMultiByte(CP_UTF8, 0, ucs2_string, -1, utf8_string.data(),
                      utf8_string_size, nullptr, nullptr);
  return utf8_string.data();
}

std::string FormatSystemErrorMessage(DWORD error_code) {
  wchar_t *message;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, error_code, LANG_USER_DEFAULT,
                reinterpret_cast<wchar_t *>(&message), 0, nullptr);
  std::unique_ptr<void, decltype(LocalFree) *> message_deleter(message,
                                                               &LocalFree);
  return ConvertUcs2ToUtf8(message);
}
#endif  // defined(MOTORNY_DEVICE_DATAREFS_PLATFORM_WINDOWS)

using ComPortNamesOrErrorMessage =
    std::variant<std::vector<std::string>, std::string>;

ComPortNamesOrErrorMessage QueryComPortNames() {
#if defined(MOTORNY_DEVICE_DATAREFS_PLATFORM_WINDOWS)
  std::vector<wchar_t> response(1024, L'\0');
  while (QueryDosDevice(nullptr, response.data(),
                        static_cast<DWORD>(response.size())) == 0) {
    DWORD error_code = GetLastError();
    if (error_code != ERROR_INSUFFICIENT_BUFFER) {
      return FormatSystemErrorMessage(error_code);
    }

    response.resize(response.size() * 2, L'\0');
  }

  std::regex com_port_name_regex(R"(^COM\d)", std::regex::icase);

  std::vector<std::string> com_port_names;
  for (auto device_name_start = response.begin();;) {
    auto device_name_end = std::find(device_name_start, response.end(), L'\0');
    assert(device_name_end != response.end());
    if (device_name_start == device_name_end) break;

    std::string device_name = ConvertUcs2ToUtf8(&*device_name_start);
    if (std::regex_search(device_name, com_port_name_regex)) {
      com_port_names.push_back(device_name);
    }
    device_name_start = device_name_end + 1;
  }
  return com_port_names;
#else
#error Unsupported platform.
#endif  // defined(MOTORNY_DEVICE_DATAREFS_PLATFORM_WINDOWS)
}

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <typename... Args>
void LogToXPlane(std::string_view format, Args &&...args) {
  XPLMDebugString(std::vformat(format, std::make_format_args(args...)).c_str());
}

}  // namespace

void Plugin::Enable() {
  // TODO: Set plugin status dataref.
  std::visit(overloaded{[&](const std::vector<std::string> &com_port_names) {
                          for (auto com_port_name : com_port_names) {
                            LogToXPlane("{}: Found {}\n", __FUNCTION__,
                                        com_port_name);
                          }
                        },
                        [](const std::string &error_message) {
                          LogToXPlane("{}: {}\n", __FUNCTION__, error_message);
                        }},
             QueryComPortNames());
}

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

}  // namespace devicedatarefs
}  // namespace motorny
