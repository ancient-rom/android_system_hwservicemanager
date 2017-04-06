#define LOG_TAG "hwservicemanager"

#include "Vintf.h"

#include <android-base/logging.h>
#include <hidl-util/FQName.h>
#include <vintf/parse_string.h>
#include <vintf/VintfObject.h>

namespace android {
namespace hardware {

vintf::Transport getTransportFromManifest(
        const FQName &fqName, const std::string &instanceName,
        const vintf::HalManifest *vm) {
    if (vm == nullptr) {
        return vintf::Transport::EMPTY;
    }
    return vm->getTransport(fqName.package(),
            vintf::Version{fqName.getPackageMajorVersion(), fqName.getPackageMinorVersion()},
            fqName.name(), instanceName);
}

vintf::Transport getTransport(const std::string &interfaceName, const std::string &instanceName) {
    FQName fqName(interfaceName);
    if (!fqName.isValid()) {
        LOG(ERROR) << "getTransport: " << interfaceName << " is not a valid fully-qualified name.";
        return vintf::Transport::EMPTY;
    }
    if (!fqName.hasVersion()) {
        LOG(ERROR) << "getTransport: " << fqName.string()
                   << " does not specify a version. Using default transport.";
        return vintf::Transport::EMPTY;
    }
    if (fqName.name().empty()) {
        LOG(ERROR) << "getTransport: " << fqName.string()
                   << " does not specify an interface name. Using default transport.";
        return vintf::Transport::EMPTY;
    }

    vintf::Transport tr = getTransportFromManifest(fqName, instanceName,
            vintf::VintfObject::GetFrameworkHalManifest());
    if (tr != vintf::Transport::EMPTY) {
        return tr;
    }
    tr = getTransportFromManifest(fqName, instanceName,
            vintf::VintfObject::GetDeviceHalManifest());
    if (tr != vintf::Transport::EMPTY) {
        return tr;
    }

    LOG(WARNING) << "getTransportFromManifest: Cannot find entry "
                 << fqName.string()
                 << " in either framework or device manifest, using default transport.";
    return vintf::Transport::EMPTY;
}

}  // hardware
}  // android